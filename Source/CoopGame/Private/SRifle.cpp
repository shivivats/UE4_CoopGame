// Fill out your copyright notice in the Description page of Project Settings.


#include "SRifle.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame/CoopGame.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Engine/StaticMeshActor.h"
#include "Components/SPenetrationComponent.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

ASRifle::ASRifle()
{
	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage = 20.f;

	BulletSpread = 2.f;

	// this is the amount of bullets per minute
	RateOfFire = 600.f;

	MaximumAmmo = 30;
	CurrentAmmo = MaximumAmmo;

	MagazineCapacity = 5;
	MagazineCount = MagazineCapacity;

	bCanShoot = true;

	PitchRecoilRangeMax = 0.2f;
	PitchRecoilRangeMin = 0.0f;

	YawRecoilRangeMax = 0.05f;
	YawRecoilRangeMin = -0.05f;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

}

void ASRifle::BeginPlay()
{
	Super::BeginPlay();

	// Derive from rate of fire

	TimeBetweenShots = 60.f / RateOfFire;
}

void ASRifle::Fire()
{
	if (!HasAuthority())
	{
		ServerFire();
	}

	AActor* MyOwner = GetOwner();

	if (MyOwner && bCanShoot)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// Bullet Spread
		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		// this will make the trace trace through every single triangle of the mesh that we hit
		// more expensive but will give us the exact location for the purpose of spawning an effect and being more precise, etc
		QueryParams.bTraceComplex = true;
		// we wanna return a physical material bc we use it for surfaces effects later on
		QueryParams.bReturnPhysicalMaterial = true;

		// smoke particle "target" parameter
		FVector TracerEndPoint = TraceEnd;

		EPhysicalSurface HitSurfaceType = SurfaceType_Default;

		FHitResult Hit;
		// this returns true only if we had a blocking it, false otherwise
		// Trace the world, from the pawn eyes to crosshair location (i.e. center of the screen)
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			// Blocking hit! Process damage

			AActor* HitActor = Hit.GetActor();

			HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (HitSurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.f;
			}

			// we can apply point damage here since we have the information needed for it
			// it can allow us to do more complex things later on
			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

			PlayImpactEffects(HitSurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;


		}
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.f, 0, 1.f);
		}

		PlayFireEffects(TracerEndPoint);

		if (HasAuthority())
		{
			HitScanTrace.TraceTo = TracerEndPoint;
			// this makes sure that even if we dont have a blocking hit, we still set the surface type to something
			HitScanTrace.SurfaceType = HitSurfaceType;
		}

		LastFiredTime = GetWorld()->TimeSeconds;

		ReduceAmmo();

		AddRecoilEffects();
	}
}

void ASRifle::PenetratingFire()
{
	if (!HasAuthority())
	{
		ServerPenetratingFire();
	}

	AActor* MyOwner = GetOwner();

	if (MyOwner && bCanShoot)
	{
		// Initialise hitresults array
		TArray<FHitResult> HitResults;
		HitResults.Init(FHitResult(ForceInit), 10);

		TArray<FHitScanTrace> HitScanTraces;

		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// Bullet Spread
		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		// this will make the trace trace through every single triangle of the mesh that we hit
		// more expensive but will give us the exact location for the purpose of spawning an effect and being more precise, etc
		QueryParams.bTraceComplex = true;
		// we wanna return a physical material bc we use it for surfaces effects later on
		QueryParams.bReturnPhysicalMaterial = true;

		// this will allow us to perform bullet penetration and get multiple actors as return
		FCollisionResponseParams ResponseParams(ECollisionResponse::ECR_Overlap);

		// smoke particle "target" parameter
		FVector TracerEndPoint = TraceEnd;

		// this makes sure that even if we dont have a blocking hit, we still set the surface type to something
		EPhysicalSurface HitSurfaceType = SurfaceType_Default;

		float ActualDamage = BaseDamage;

		UE_LOG(LogTemp, Warning, TEXT("trying to start trace"));

		// this returns true only if we had a blocking it, false otherwise
		// Trace the world, from the pawn eyes to crosshair location (i.e. center of the screen)
		// here we do multi by channel trace bc we want to get multiple results
		GetWorld()->LineTraceMultiByChannel(HitResults, EyeLocation, TraceEnd, COLLISION_WEAPON_PENETRABLE, QueryParams, ResponseParams);

		if (HitResults.Num() > 0)
		{
			// Blocking hit! Process damage
			UE_LOG(LogTemp, Warning, TEXT("hit atleast one thing in trace"));
			if (HitResults.Num() > 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("try make bullet hole"));
				CreateBulletHole(&HitResults[0]);
			}

			for (FHitResult Hit : HitResults)
			{
				AStaticMeshActor* PotentialSolidMaterial = Cast<AStaticMeshActor>(Hit.GetActor());

				// if we hit a solid wall or cube, and its not pending kill then stop processing hit results
				if (PotentialSolidMaterial != nullptr && !PotentialSolidMaterial->IsPendingKill())
				{
					// secondly, check if we have hit a penetrable actor
					USPenetrationComponent* PenetrationComponent = Cast<USPenetrationComponent>(PotentialSolidMaterial->GetComponentByClass(USPenetrationComponent::StaticClass()));

					if (PenetrationComponent)
					{
						// if penetrable actor, then reduce damage and continue

						UE_LOG(LogTemp, Warning, TEXT("hit a solid actor which has penetration, reducing damage and continuing"));
						ActualDamage /= PenetrationComponent->MaterialResistance;
						continue;
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("hit a solid actor which doesnt have penetration, breaking"));
						break;
					}
				}

				UE_LOG(LogTemp, Warning, TEXT("hit an actual target. processing hit"));
				AActor* HitActor = Hit.GetActor();

				HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
				
				if (HitSurfaceType == SURFACE_FLESHVULNERABLE)
				{
					ActualDamage *= 4.f;
				}

				// we can apply point damage here since we have the information needed for it
				// it can allow us to do more complex things later on
				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

				PlayImpactEffects(HitSurfaceType, Hit.ImpactPoint);

				TracerEndPoint = Hit.ImpactPoint;
			}
		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.f, 0, 1.f);
		}

		PlayFireEffects(TracerEndPoint);

		if (HasAuthority())
		{
			HitScanTrace.TraceTo = TracerEndPoint;

			HitScanTrace.SurfaceType = HitSurfaceType;
		}

		LastFiredTime = GetWorld()->TimeSeconds;

		ReduceAmmo();

		AddRecoilEffects();
	}
}

void ASRifle::OnRep_HitScanTrace()
{
	// Play cosmetix FX
	PlayFireEffects(HitScanTrace.TraceTo);

	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);

}

void ASRifle::ServerPenetratingFire_Implementation()
{
	PenetratingFire();
}

bool ASRifle::ServerPenetratingFire_Validate()
{
	return true;
}

void ASRifle::CreateBulletHole(FHitResult* Object)
{
	if (BulletHoleDecal)
	{
		FVector BulletHoleSize = FVector(3.5f, 7.f, 7.f);
		UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletHoleDecal, BulletHoleSize, Object->ImpactPoint, Object->ImpactNormal.Rotation(), 10.f);
		UE_LOG(LogTemp, Warning, TEXT("Made bullet hole"));
	}
}

void ASRifle::StartFire()
{
	if (bCanShoot)
	{
		// can be minimum 1, cant let this be negative
		float FirstDelay = FMath::Max(LastFiredTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.f);
		if(bShootPenetrating)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASRifle::PenetratingFire, TimeBetweenShots, true, FirstDelay);
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASRifle::Fire, TimeBetweenShots, true, FirstDelay);
		}
		
	}
}

void ASRifle::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASRifle::PlayFireEffects(FVector TracerEnd)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

	UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
	if (TracerComp)
	{
		TracerComp->SetVectorParameter(TracerTargetName, TracerEnd);
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientStartCameraShake(FireCamShake);
		}
	}
}

void ASRifle::PlayImpactEffects(EPhysicalSurface HitSurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;

	switch (HitSurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
		SelectedEffect = FleshImpactEffect;
		break;
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		// play the impact effect here
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void ASRifle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASRifle, HitScanTrace, COND_SkipOwner);
}