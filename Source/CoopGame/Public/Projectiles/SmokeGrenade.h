// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/Grenade.h"
#include "SmokeGrenade.generated.h"

UCLASS()
class COOPGAME_API ASmokeGrenade : public AGrenade
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASmokeGrenade();

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SmokeGrenade")
	class UBoxComponent* BoxCollider;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SmokeGrenade")
		float SmokeDuration;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Explode() override;

	virtual void PlayExplosionEffects() override;

};
