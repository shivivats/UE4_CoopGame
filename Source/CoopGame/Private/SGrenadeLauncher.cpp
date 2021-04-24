// Fill out your copyright notice in the Description page of Project Settings.


#include "SGrenadeLauncher.h"
#include "Kismet/GameplayStatics.h"


ASGrenadeLauncher::ASGrenadeLauncher()
{

}

void ASGrenadeLauncher::BeginPlay()
{
	Super::BeginPlay();
}

void ASGrenadeLauncher::Fire()
{
	// Trace the world, from the pawn eyes to crosshair location (i.e. center of the screen)

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		if (MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		}

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.Owner = MyOwner;
		//ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		if (GrenadeProjectile)
		{
			GetWorld()->SpawnActor<AActor>(GrenadeProjectile, MuzzleLocation, EyeRotation, ActorSpawnParams);
		}
	}
}
