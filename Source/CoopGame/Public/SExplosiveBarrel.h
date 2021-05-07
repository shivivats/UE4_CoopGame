// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

UCLASS()
class COOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USHealthComponent* HealthComp;

	void Explode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Explosion")
	class UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	class USoundCue* ExplosionSound;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	class UMaterialInterface* DefaultMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	class UMaterialInterface* ExplodedMaterial;

	UPROPERTY(ReplicatedUsing=OnRep_HasExploded, EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	bool bHasExploded;

	UFUNCTION()
	void OnRep_HasExploded();

	void PlayExplosionEffects();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	class URadialForceComponent* RadialForceComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	float ExplosionImpulseIntensity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	float ExplosionRadius;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
