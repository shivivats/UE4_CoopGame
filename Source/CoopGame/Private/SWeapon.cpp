// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShake.h"
#include "Net/UnrealNetwork.h"




// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	SetReplicates(true);
}

void ASWeapon::PlayFireEffects(FVector TracerEnd)
{

}

void ASWeapon::Fire()
{

}

void ASWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ASWeapon::ServerFire_Validate()
{
	return true;
}

void ASWeapon::StartFire()
{

}

void ASWeapon::StopFire()
{

}

void ASWeapon::ReduceAmmo()
{
	CurrentAmmo -= 1;

	if (CurrentAmmo == 0 && MagazineCount >= 0)
	{
		Reload();
	}
}

void ASWeapon::Reload()
{
	MagazineCount -= 1;

	if (MagazineCount < 0)
	{
		MagazineCount = 0;
		bCanShoot = false;
	}
	else
	{
		CurrentAmmo = MaximumAmmo;
	}
}


void ASWeapon::AddRecoilEffects()
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		MyOwner->AddControllerPitchInput(-1.f * FMath::RandRange(PitchRecoilRangeMin, PitchRecoilRangeMax));
		MyOwner->AddControllerYawInput(FMath::RandRange(YawRecoilRangeMin, YawRecoilRangeMax));
	}
}