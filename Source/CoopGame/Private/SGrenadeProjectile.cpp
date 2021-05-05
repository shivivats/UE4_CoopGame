// Fill out your copyright notice in the Description page of Project Settings.


#include "SGrenadeProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASGrenadeProjectile::ASGrenadeProjectile()
{
	// Visible mesh component
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));

	// Set as root component
	RootComponent = MeshComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = MeshComp;
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 1000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Explode after 3 seconds by default
	ExplosionDelay = 3.f;

	ExplosionDamage = 20.f;

	ExplosionRadius = 100.f;

	bExploded = false;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void ASGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_ExplosionDelay, this, &ASGrenadeProjectile::Explode, ExplosionDelay, false);

}

void ASGrenadeProjectile::OnRep_Exploded()
{
	PlayExplosionEffects();
}

void ASGrenadeProjectile::Explode()
{
	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		bExploded = true;

		PlayExplosionEffects();

		if (HasAuthority())
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Black, false, 1.f, 2.f);

			// cause some radial damage
			TArray<AActor*> IgnoredActors = {MyOwner};
			UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRadius, DamageType, IgnoredActors, this, MyOwner->GetInstigatorController(), true);

			// destroy this actor
			// Dont immediately destroy it
			//Destroy();

			// Set lifetime to a small value (2 secs here) instead of destroying immediately bc we want to let the explosion and other effects to play on the client before this gets destroyed
			SetLifeSpan(2.0f);
		}
	}

}

void ASGrenadeProjectile::PlayExplosionEffects()
{
	// spawn explosion effect
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	MeshComp->SetVisibility(false, true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASGrenadeProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASGrenadeProjectile, bExploded, COND_SkipOwner);
}