// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShake.h"




// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
}

void ASWeapon::PlayFireEffects(FVector TracerEnd)
{

}

void ASWeapon::Fire()
{

}

void ASWeapon::StartFire()
{

}

void ASWeapon::StopFire()
{

}

void ASWeapon::ReduceAmmo()
{

}

void ASWeapon::Reload()
{

}
