// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGrenadeProjectile.generated.h"

UCLASS()
class COOPGAME_API ASGrenadeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASGrenadeProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Sphere collision component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	class UStaticMeshComponent* MeshComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
		float ExplosionDelay;

	FTimerHandle TimerHandle_ExplosionDelay;

	UPROPERTY(ReplicatedUsing=OnRep_Exploded, BlueprintReadOnly, Category="Damage")
	bool bExploded;

	UFUNCTION()
	void OnRep_Exploded();

	void Explode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	class UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
		float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
		float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
		TSubclassOf<class UDamageType> DamageType;

	void PlayExplosionEffects();
};
