// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/Grenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AGrenade::AGrenade()
{
	// Visible mesh component
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
	MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// Set as root component
	RootComponent = MeshComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	/*
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = MeshComp;
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 1000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	*/

	// Explode after 3 seconds by default
	ExplosionDelay = 3.f;

	/*
	ExplosionDamage = 20.f;
	ExplosionRadius = 100.f;
	*/
	bExploded = false;

	SetReplicates(true);
	SetReplicateMovement(true);

	GrenadeLifetimeAfterExplosion = 2.f;
}

// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Added owner to ignored actors"))
		MeshComp->IgnoreActorWhenMoving(GetOwner(), true);
	}	
}


void AGrenade::OnRep_Exploded()
{
	UE_LOG(LogTemp, Warning, TEXT("onrep exploded called"));
	PlayExplosionEffects();
}


void AGrenade::Explode()
{
	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		bExploded = true;

		PlayExplosionEffects();

		if (HasAuthority())
		{
			//DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Black, false, 1.f, 2.f);

			// cause some radial damage
			//TArray<AActor*> IgnoredActors = { MyOwner };
			//UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRadius, DamageType, IgnoredActors, this, MyOwner->GetInstigatorController(), true);

			// destroy this actor
			// Dont immediately destroy it
			// Set lifetime to a small value instead of destroying immediately bc we want to let the explosion and other effects to play on the client before this gets destroyed
			SetLifeSpan(GrenadeLifetimeAfterExplosion);
		}
	}

}


void AGrenade::PlayExplosionEffects()
{
	// spawn explosion effect
	UParticleSystemComponent* SpawnedEffect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	//UGameplayStatics::SpawnEmitterAttached(ExplosionEffect, MeshComp, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, FVector(ExplosionScale));

	// Can also save that particle variable and set its relative scale 3d if we need to
	SpawnedEffect->SetRelativeScale3D(ExplosionScale);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());

	MeshComp->SetVisibility(false, false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetSimulatePhysics(false);
}

void AGrenade::ThrowGrenade(FVector EyeLocation, FVector HandLocation, FVector ThrowDirection)
{
	if (HasAuthority())
	{
		FVector ImpulseDirection = (ThrowDirection * 10000) - EyeLocation;

		ImpulseDirection.Normalize();

		FVector Impulse = ImpulseDirection * ImpulseIntensity;

		MeshComp->SetSimulatePhysics(true);

		MeshComp->AddImpulse(Impulse, NAME_None, true);
	}
		
	GetWorldTimerManager().SetTimer(TimerHandle_ExplosionDelay, this, &AGrenade::Explode, ExplosionDelay, false);
}

void AGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGrenade, bExploded, COND_SkipOwner);
}