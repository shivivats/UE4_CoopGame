// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SGrenadeLauncher.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGrenadeLauncher : public ASWeapon
{
	GENERATED_BODY()

	ASGrenadeLauncher();

protected:
	virtual void BeginPlay() override;

	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class ASGrenadeProjectile> GrenadeProjectile;

	/* RPM - Bullets per minute fired by the rifle */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	/* Allowed time bc this is semi automatic so we gotta wait atleast these many seconds */
	float AllowedTimeBetweenShots;

	float LastFiredTime;
	
public:

	virtual void StartFire() override;

	virtual void StopFire() override;
};
