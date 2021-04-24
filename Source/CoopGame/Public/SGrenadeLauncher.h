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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> GrenadeProjectile;


};
