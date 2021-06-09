// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grenade.generated.h"

UCLASS()
class COOPGAME_API AGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Sphere collision component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	class UStaticMeshComponent* MeshComp;

	/** Projectile movement component */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	//class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float ImpulseIntensity;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ExplosionDelay;

	FTimerHandle TimerHandle_ExplosionDelay;

	UPROPERTY(ReplicatedUsing = OnRep_Exploded, BlueprintReadOnly, Category = "Explosion")
	bool bExploded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	float GrenadeLifetimeAfterExplosion;

	UFUNCTION()
	virtual void OnRep_Exploded();

	virtual void Explode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	class UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	class USoundCue* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	FVector ExplosionScale;

	/*
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UDamageType> DamageType;
	*/

	virtual void PlayExplosionEffects();

public:

	void ThrowGrenade(FVector EyeLocation, FVector HandLocation, FVector ThrowDirection);

};
