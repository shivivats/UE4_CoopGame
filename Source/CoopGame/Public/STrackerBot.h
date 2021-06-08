// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components")
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class USHealthComponent* HealthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class USphereComponent* SphereComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class UAudioComponent* AudioComp;

	UPROPERTY(EditAnywhere, Category = "TrackerBot|Testing")
	bool bActive;

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	FVector GetNextPathPoint();

	// Next point in navigation
	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot|Tracking")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot|Tracking")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot|Tracking")
	float RequiredDistanceToTarget;

	// Dynamic material to pulse on damage
	UMaterialInstanceDynamic* MatInst;

	void SelfDestruct();
	
	UPROPERTY(EditDefaultsOnly, Category="TrackerBot|Explosion")
	class UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot|Explosion")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot|Explosion")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot|Explosion")
	float SelfDamageInterval;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot|Explosion")
	class USoundCue* SelfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot|Explosion")
	class USoundCue* ExplosionSound;

	UPROPERTY(ReplicatedUsing=OnRep_Exploded, EditDefaultsOnly, BlueprintReadOnly, Category = "TrackerBot|Explosion")
	bool bExploded;

	bool bStartedSelfDestruction;

	FTimerHandle TimerHandle_SelfDamage;

	UFUNCTION()
	void DamageSelf();

	UFUNCTION()
	void OnRep_Exploded();
	
	void PlayExplosionEffects();

	void OnCheckNearbyBots();
	
	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot|Hording")
	float NearbyBotsCheckRadius;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot|Hording")
	int32 MaxPowerLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TrackerBot|Hording")
	int32 CurrentPowerLevel;
	
	FTimerHandle TimerHandle_RefreshPath;

	void RefreshPath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
