// Fill out your copyright notice in the Description page of Project Settings.


#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Components/SHealthComponent.h"
#include "Components/SphereComponent.h"
#include "SCharacter.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"

static int32 DebugTrackerBotDrawing = 0;
FAutoConsoleVariableRef CVARDebugTrackerBotDrawing(
	TEXT("COOP.DebugTrackerBot"),
	DebugTrackerBotDrawing,
	TEXT("Draw Debug Lines for Tracker Bot"),
	ECVF_Cheat);

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200.f);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(RootComponent);

	bActive = true;

	bUseVelocityChange = false;
	MovementForce = 500.f;

	RequiredDistanceToTarget = 100.f;

	SelfDamageInterval = 0.25f;

	ExplosionDamage = 60.f;
	ExplosionRadius = 350.f;

	bStartedSelfDestruction = false;

	SetReplicates(true);
	SetReplicateMovement(true);

	NearbyBotsCheckRadius = 600.f;
	MaxPowerLevel = 4;
	CurrentPowerLevel = 0;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	// navigation stuff only done on the server
	if (HasAuthority() && bActive)
	{
		NextPathPoint = GetNextPathPoint();	

		FTimerHandle TimerHandle_CheckNearbyBots;
		GetWorldTimerManager().SetTimer(TimerHandle_CheckNearbyBots, this, &ASTrackerBot::OnCheckNearbyBots, 1.f, true);
	}
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// Explode on hp 0

	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));

		// we need to create a dynamic instance bc we want to change the material for this particular pawn
		// otherwise it would change the material for all pawns in the level
		// when we wanna change something at runtime like this, we always wanna create a dynamic instance
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	if (Health <= 0.f)
	{
		SelfDestruct();
	}
}

FVector ASTrackerBot::GetNextPathPoint()
{
	AActor* BestTarget = nullptr;
	float NearestTargetDistance = FLT_MAX;

	// iterate over all pawns in the world
	for (TActorIterator<APawn> Itr(GetWorld()); Itr; ++Itr)
	{
		APawn* TestPawn = *Itr;
		if (TestPawn == nullptr || USHealthComponent::IsFriendly(TestPawn, this))
		{
			continue;
		}

		USHealthComponent* TestPawnHealthComp = Cast<USHealthComponent>(TestPawn->FindComponentByClass(USHealthComponent::StaticClass()));
		if (TestPawnHealthComp && TestPawnHealthComp->GetHealth() > 0.f)
		{
			float Distance = (TestPawn->GetActorLocation() - this->GetActorLocation()).Size();

			if(NearestTargetDistance > Distance)
			{
				BestTarget = TestPawn;
				NearestTargetDistance = Distance;
			}
		}
	}

	// Make sure we have a best target, for eg, like in the case when all players are dead, we will not have one
	if(BestTarget)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), BestTarget);

		// this makes sure that we can unstuck ourselves if its been 5 seconds and we havent reached the next path point
		GetWorldTimerManager().ClearTimer(TimerHandle_RefreshPath);
		GetWorldTimerManager().SetTimer(TimerHandle_RefreshPath, this, &ASTrackerBot::RefreshPath, 5.f, false);

		if (NavPath && NavPath->PathPoints.Num() > 1)
		{
			return NavPath->PathPoints[1];
		}
	}
	
	// Failed to find path
	return GetActorLocation();
}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;

	// we wouldnt wanna run this if we had a "dedicated" server, but since server is also a client in a way here, this isnt an issue
	PlayExplosionEffects();

	// disable the mesh so that we dont see or feel anything while waiting for the explosion and for the object to get destroyed

	if (HasAuthority())
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		float ActualDamage = ExplosionDamage + (ExplosionDamage * CurrentPowerLevel);

		// Apply damage on the server
		UGameplayStatics::ApplyRadialDamage(this, ActualDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		if (DebugTrackerBotDrawing)
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.f, 0, 1.f);
		}

		// Destroy on the server
		// Dont immediately destroy it
		//Destroy();

		// Set lifetime to a small value (2 secs here) instead of destroying immediately bc we want to let the explosion and other effects to play on the client before this gets destroyed
		SetLifeSpan(2.0f);
	}
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20.f, GetInstigatorController(), this, nullptr);
}

void ASTrackerBot::OnRep_Exploded()
{
	PlayExplosionEffects();
}

void ASTrackerBot::PlayExplosionEffects()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());

	MeshComp->SetVisibility(false, true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetSimulatePhysics(false);
}

void ASTrackerBot::OnCheckNearbyBots()
{
	// Create a temporary collision shape for overlaps
	FCollisionShape CollShape;
	CollShape.SetSphere(NearbyBotsCheckRadius);

	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);

	//DrawDebugSphere(GetWorld(), GetActorLocation(), NearbyBotsCheckRadius, 12, FColor::White, false, 1.f);

	int32 NumBots = 0;

	for (FOverlapResult Result : Overlaps)
	{
		ASTrackerBot* OtherBot = Cast<ASTrackerBot>(Result.GetActor());

		if (OtherBot && OtherBot != this)
		{
			NumBots++;
		}
	}

	CurrentPowerLevel = FMath::Clamp(NumBots, 0, MaxPowerLevel);

	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst)
	{
		float Alpha = CurrentPowerLevel / (float)MaxPowerLevel;
		MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	}

	if(DebugTrackerBotDrawing)
	{
		// draw on the bot location
		DrawDebugString(GetWorld(), GetActorLocation(), FString::FromInt(CurrentPowerLevel), this, FColor::White, 1.f, true);
	}
}

void ASTrackerBot::RefreshPath()
{
	NextPathPoint = GetNextPathPoint();
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// all the movement will only be done on the server, and only check for it if we're not exploded
	if (HasAuthority() && !bExploded && bActive)
	{
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			// if close enough, get next path point
			NextPathPoint = GetNextPathPoint();

			//DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!");
		}
		else
		{
			// keep moving towards next target
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();

			MeshComp->AddForce(ForceDirection * MovementForce, NAME_None, bUseVelocityChange);
			if (DebugTrackerBotDrawing)
			{
				DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + (ForceDirection * MovementForce), 32, FColor::Yellow, false, 0.f, 0, 1.f);
			}
		}
		if (DebugTrackerBotDrawing)
		{
			DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 4.f, 1.f);
		}
	}

	// Change sound volume based on velocity
	AudioComp->SetVolumeMultiplier(FMath::GetMappedRangeValueClamped(FVector2D(10, MovementForce), FVector2D(0.1, 2), GetVelocity().Size()));
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!bStartedSelfDestruction && !bExploded && bActive)
	{
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);

		if (PlayerPawn && !USHealthComponent::IsFriendly(OtherActor, this))
		{
			// overlapped with player

			if (HasAuthority())
			{
				//start self destruction sequence
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.f);
			}

			bStartedSelfDestruction = true;

			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
		}
	}
}



void ASTrackerBot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASTrackerBot, bExploded, COND_SkipOwner);
}