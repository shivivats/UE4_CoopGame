// Fill out your copyright notice in the Description page of Project Settings.


#include "SGrenadeLauncher.h"
#include "Kismet/GameplayStatics.h"
#include "SGrenadeProjectile.h"

ASGrenadeLauncher::ASGrenadeLauncher()
{
	MaximumAmmo = 5;
	CurrentAmmo = MaximumAmmo;

	MagazineCapacity = 3;
	MagazineCount = MagazineCapacity;

	bCanShoot = true;

	PitchRecoilRangeMax = 0.3f;
	PitchRecoilRangeMin = 0.0f;

	YawRecoilRangeMax = 0.1f;
	YawRecoilRangeMin = -0.1f;

	RateOfFire = 10.f;
}

void ASGrenadeLauncher::BeginPlay()
{
	Super::BeginPlay();

	// Derive from rate of fire

	AllowedTimeBetweenShots = 60.f / RateOfFire;
	LastFiredTime = -1.f * AllowedTimeBetweenShots;
}

void ASGrenadeLauncher::Fire()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner && bCanShoot && GrenadeProjectile)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		//FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.Owner = MyOwner;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* CurrentProjectile = GetWorld()->SpawnActor<AActor>(GrenadeProjectile, MuzzleLocation, EyeRotation, ActorSpawnParams);
		CurrentProjectile->SetOwner(MyOwner);		

		LastFiredTime = GetWorld()->TimeSeconds;

		ReduceAmmo();

		AddRecoilEffects();
	}
}

void ASGrenadeLauncher::StartFire()
{
	if (bCanShoot && (GetWorld()->TimeSeconds > (LastFiredTime + AllowedTimeBetweenShots)))
	{
		Fire();
	}
}

void ASGrenadeLauncher::StopFire()
{
	// dont really do anything here for the grenade launcher
}