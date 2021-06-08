// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SInteractionWidget.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void UpdateInteractionWidget(class USInteractionComponent* InteractionComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateInteractionWidget();

	UPROPERTY(BlueprintReadOnly, Category = "Interaction", meta = (ExposeOnSpawn))
	class USInteractionComponent* OwningInteractionComponent;
};
