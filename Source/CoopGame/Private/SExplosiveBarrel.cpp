// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"

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
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultMaterial = MeshComp->GetMaterial(0);

	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);
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

	// play particle effect
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	if (ExplodedMaterial)
	{
		// change the material on the mesh
		MeshComp->SetMaterial(0, ExplodedMaterial);
	}

	// launch the barrel upwards
	FVector ExplosionImpulse = GetActorUpVector()* ExplosionImpulseIntensity;
	MeshComp->AddImpulse(ExplosionImpulse, NAME_None, true);

	// push away nearby physics actors (hint: radialforcecomponent)
	RadialForceComp->FireImpulse();
}