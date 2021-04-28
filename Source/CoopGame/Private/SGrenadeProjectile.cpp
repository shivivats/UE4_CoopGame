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
}

void ASGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_ExplosionDelay, this, &ASGrenadeProjectile::Explode, ExplosionDelay, false);

}

void ASGrenadeProjectile::Explode()
{

	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Black, false, 1.f, 2.f);

		// cause some radial damage
		TArray<AActor*> IgnoredActors = {MyOwner};
		UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRadius, DamageType, IgnoredActors, this, MyOwner->GetInstigatorController(), true);

		// spawn explosion effect
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

		// destroy this actor
		Destroy();
	}

}