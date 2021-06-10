// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Barricade.generated.h"

UCLASS()
class COOPGAME_API ABarricade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABarricade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Barricade")
	class UStaticMeshComponent* FrameMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Barricade")
	class UStaticMeshComponent* BarricadeMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Barricade")
	class UBoxComponent* InteractingBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Barricade")
	class USInteractionComponent* InteractionComp;

};
