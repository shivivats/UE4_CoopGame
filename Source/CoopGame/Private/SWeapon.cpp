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
	if(!HasAuthority())
	{
		ServerReduceAmmo();
	}

	CurrentAmmo -= 1;

	if (CurrentAmmo == 0 && MagazineCount >= 0)
	{
		Reload();
	}
}

void ASWeapon::ServerReduceAmmo_Implementation()
{
	ReduceAmmo();
}

bool ASWeapon::ServerReduceAmmo_Validate()
{
	return true;
}

void ASWeapon::AddAmmo()
{
	if (!HasAuthority())
	{
		ServerAddAmmo();
	}

	int32 OldMagCount = MagazineCount;

	MagazineCount += 1;

	// if we werent able to shoot before reload, and it was because of magazine count 0
	// then we also enable shooting
	if (bCanShoot == false && OldMagCount == 0)
	{
		bCanShoot = true;
		Reload();
	}	
}

void ASWeapon::ServerAddAmmo_Implementation()
{
	AddAmmo();
}

bool ASWeapon::ServerAddAmmo_Validate()
{
	return true;
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

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASWeapon, MaximumAmmo);
	DOREPLIFETIME(ASWeapon, CurrentAmmo);
	DOREPLIFETIME(ASWeapon, MagazineCapacity);
	DOREPLIFETIME(ASWeapon, MagazineCount);
	DOREPLIFETIME(ASWeapon, bCanShoot);
}