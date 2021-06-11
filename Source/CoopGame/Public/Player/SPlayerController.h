// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASPlayerController();

protected:

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	void Turn(float Rate);

	void LookUp(float Rate);

public:

	UFUNCTION(BlueprintCallable)
	void PlayerDied();

	UFUNCTION(BlueprintCallable)
	void PlayerRespawn();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPlayerRespawn();
	
};
