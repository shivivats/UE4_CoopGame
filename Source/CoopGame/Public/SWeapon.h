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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Weapon|Components")
	class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Shooting")
	TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Shooting")
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Weapon|Shooting")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Shooting")
	class UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Shooting")
	class UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Shooting")
	class UParticleSystem* TracerEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Shooting")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Shooting")
	TSubclassOf<class UMatineeCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Shooting")
	float BaseDamage;

	virtual void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	/* Ammo stuffs */
	UPROPERTY(Replicated, EditDefaultsOnly,  BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MaximumAmmo;

	UPROPERTY(Replicated, VisibleDefaultsOnly,  BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 CurrentAmmo;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly,  Category = "Weapon|Ammo")
	int32 MagazineCapacity;

	UPROPERTY(Replicated, VisibleDefaultsOnly,  BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MagazineCount;

	UPROPERTY(Replicated, VisibleDefaultsOnly,  BlueprintReadOnly, Category = "Weapon|Ammo")
	bool bCanShoot;

	/* Vertical Recoil. This will be reversed since positive pitch value actually goes down. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Recoil")
	float PitchRecoilRangeMax;

	/* Vertical Recoil. This will be reversed since positive pitch value actually goes down. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Recoil")
	float PitchRecoilRangeMin;

	/* Horizontal Recoil */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Recoil")
	float YawRecoilRangeMax;

	/* Horizontal Recoil */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Recoil")
	float YawRecoilRangeMin;

public:	

	virtual void StartFire();

	virtual void StopFire();

	virtual void ReduceAmmo();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerReduceAmmo();

	UFUNCTION(BlueprintCallable, Category="Weapon|Ammo")
	virtual void AddAmmo();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerAddAmmo();

	virtual void Reload();

	virtual void AddRecoilEffects();
};
