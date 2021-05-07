// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Sound/SoundCue.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->Radius = 250.f;
	RadialForceComp->ImpulseStrength = 500.f;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bIgnoreOwningActor = true;
	RadialForceComp->bAutoActivate = false;
	
	bHasExploded = false;

	ExplosionImpulseIntensity = 500.f;

	SetReplicates(true);
	SetReplicateMovement(true);

	ExplosionDamage = 40.f;
	ExplosionRadius = 100.f;
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultMaterial = MeshComp->GetMaterial(0);

	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);
}

void ASExplosiveBarrel::OnRep_HasExploded()
{
	PlayExplosionEffects();
}

void ASExplosiveBarrel::PlayExplosionEffects()
{
	// play particle effect
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	if (ExplodedMaterial)
	{
		// change the material on the mesh
		MeshComp->SetMaterial(0, ExplodedMaterial);
	}

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());

	// launch the barrel upwards
	FVector ExplosionImpulse = GetActorUpVector() * ExplosionImpulseIntensity;
	MeshComp->AddImpulse(ExplosionImpulse, NAME_None, true);

	// push away nearby physics actors (hint: radialforcecomponent)
	RadialForceComp->FireImpulse();

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.f, 0, 1.f);
}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Barrel health is %f"), Health)

	if (Health <= 0.f && !bHasExploded)
	{
		Explode();
	}
}

void ASExplosiveBarrel::Explode()
{
	// set exploded state to true
	bHasExploded = true;

	PlayExplosionEffects();

}

void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASExplosiveBarrel, bHasExploded, COND_SkipOwner);
}