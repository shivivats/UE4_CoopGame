// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SRifle.generated.h"

// Contains information of a single hitscan weapon linetrace
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

/**
 * Hitscan weapon derived off the base weapon class
 */
UCLASS()
class COOPGAME_API ASRifle : public ASWeapon
{
	GENERATED_BODY()

	ASRifle();

protected:
	virtual void BeginPlay() override;

	virtual void PlayFireEffects(FVector TracerEnd);

	virtual void PlayImpactEffects(EPhysicalSurface HitSurfaceType, FVector ImpactPoint);

	virtual void Fire() override;

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFiredTime;

	/* RPM - Bullets per minute fired by the rifle */
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	float RateOfFire;

	float TimeBetweenShots;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();

public:

	virtual void StartFire();

	virtual void StopFire();
};
