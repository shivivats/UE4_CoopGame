// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SRifle.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASRifle : public ASWeapon
{
	GENERATED_BODY()

		ASRifle();

protected:
	virtual void BeginPlay() override;

	virtual void PlayFireEffects(FVector TracerEnd);

	virtual void Fire() override;

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFiredTime;

	/* RPM - Bullets per minute fired by the rifle */
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	float RateOfFire;

	float TimeBetweenShots;

public:

	virtual void StartFire();

	virtual void StopFire();

	virtual void ReduceAmmo();

	virtual void Reload();
};
