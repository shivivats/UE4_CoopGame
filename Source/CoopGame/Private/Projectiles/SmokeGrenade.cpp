// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/SmokeGrenade.h"
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
#include "Components/BoxComponent.h"

// Sets default values
ASmokeGrenade::ASmokeGrenade()
{
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetBoxExtent(FVector(32.f, 150.f, 180.f));
	BoxCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollider->SetRelativeLocation(FVector(0.f, 0.f, 160.f));
	BoxCollider->SetupAttachment(RootComponent);
	
	SmokeDuration = 10.f;

	SetReplicates(true);
	SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void ASmokeGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASmokeGrenade::Explode()
{
	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		bExploded = true;

		PlayExplosionEffects();

		if (HasAuthority())
		{
			//DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Black, false, 1.f, 2.f);
			// have a collider with the grenade that we enable

			// Set lifetime to a small value (2 secs here) instead of destroying immediately bc we want to let the explosion and other effects to play on the client before this gets destroyed
			SetLifeSpan(SmokeDuration);
		}
	}

}

void ASmokeGrenade::PlayExplosionEffects()
{
	// spawn explosion effect
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Play explosion effects called on server"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Play explosion effects called on client"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Play explosion effects called in general"));

	UGameplayStatics::SpawnEmitterAttached(ExplosionEffect, MeshComp, NAME_None, GetActorLocation(), FRotator::ZeroRotator, ExplosionScale, EAttachLocation::KeepWorldPosition);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());

	MeshComp->SetVisibility(false, false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetSimulatePhysics(false);

	//BoxCollider->SetCollisionResponseToAllChannels(ECR_Block);
	BoxCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	BoxCollider->SetGenerateOverlapEvents(true);
}