// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmmoCrate.generated.h"

UCLASS()
class COOPGAME_API AAmmoCrate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoCrate();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo Crate|Components")
	class UStaticMeshComponent* CrateMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo Crate|Components")
	class USInteractionComponent* InteractionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ammo Crate|Ammo")
	int32 NumResupplies;

};
