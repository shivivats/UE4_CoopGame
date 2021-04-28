// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:

	virtual void PlayFireEffects(FVector TracerEnd);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* TracerEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	virtual void Fire();

	/* Ammo stuffs */
	UPROPERTY(EditDefaultsOnly,  BlueprintReadOnly, Category = "Weapon")
	int32 MaximumAmmo;

	UPROPERTY(VisibleDefaultsOnly,  BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,  Category = "Weapon")
	int32 MagazineCapacity;

	UPROPERTY(VisibleDefaultsOnly,  BlueprintReadOnly, Category = "Weapon")
	int32 MagazineCount;

	UPROPERTY(VisibleDefaultsOnly,  BlueprintReadOnly, Category = "Weapon")
	bool bCanShoot;

	/* Vertical Recoil. This will be reversed since positive pitch value actually goes down. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float PitchRecoilRangeMax;

	/* Vertical Recoil. This will be reversed since positive pitch value actually goes down. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float PitchRecoilRangeMin;

	/* Horizontal Recoil */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float YawRecoilRangeMax;

	/* Horizontal Recoil */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float YawRecoilRangeMin;

public:	

	virtual void StartFire();

	virtual void StopFire();

	virtual void ReduceAmmo();

	virtual void Reload();

	virtual void AddRecoilEffects();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class UUserWidget> AmmoHUD;
};
