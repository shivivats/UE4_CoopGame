// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

USTRUCT()
struct FInteractionData
{
	// GENERATED_BODY is needed to be added to any struct
	GENERATED_BODY()

	// Make a constructor
	FInteractionData()
	{
		ViewedInteractionComponent = nullptr;
		LastInteractionCheckTime = 0.f;
		bInteractHeld = false;
	}

	// The current interactable component we're viewing, if there is one
	UPROPERTY()
	class USInteractionComponent* ViewedInteractionComponent;

	// The time when we last checked for an interactable
	UPROPERTY()
	float LastInteractionCheckTime;

	// Whether the player is holding the interact key
	UPROPERTY()
	bool bInteractHeld;
};

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Movement System */
	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();

	/* Weapon System */
	void BeginZoom();
	void EndZoom();

	void PrepareThrow();
	void Throw();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPrepareThrow();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerThrow();

	/* Interaction System */
	void PerformInteractionCheck();

	void CouldntFindInteractable();
	void FoundNewInteractable(USInteractionComponent* Interactable);

	void BeginInteract();
	void EndInteract();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBeginInteract();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndInteract();

	void Interact();

	/* Health System */
	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category="Spectator")
	void ChangePlayerToSpectator();
	
	/* Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComp;


	/* Weapon System */
	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	/* Default FOV set during begin play */
	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100.0))
	float ZoomInterpSpeed;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	class ASWeapon* CurrentWeapon;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	TSubclassOf<class AGrenade> StarterGrenadeClass;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	class AGrenade* CurrentGrenade;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName GrenadeAttachSocketName;

	/* Health System */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USHealthComponent* HealthComp;

	/* Pawn died previously */
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Player")
	bool bDied;

	/* Interaction System */
	// How often in seconds to check for an interactable object. Set this to zero if you want to check every tick
	// This is for optimisation
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckFrequency;

	// How far we'll trace when we check if the player is looking at an interactable object
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckDistance;

	// Information about the current state of the players interaction
	UPROPERTY()
	FInteractionData InteractionData;

	// Helper function to make grabbing interatable faster
	FORCEINLINE class USInteractionComponent* GetInteractable() const { return InteractionData.ViewedInteractionComponent; }

	FTimerHandle TimerHandle_Interact;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	/* Weapon System */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();

	/* Interaction System */
	// True if we're interacting with an item that has an interaction time (for eg a lamp taht takes 2 seconds to turn on)
	bool IsInteracting() const;

	// Get the time till we interact with the current interactable
	float GetRemainingInteractTime() const;
};
