// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/AmmoCrate.h"
#include "Components/SInteractionComponent.h"

// Sets default values
AAmmoCrate::AAmmoCrate()
{
	CrateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrateMesh"));
	RootComponent = CrateMesh;

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>(TEXT("InteractionComp"));
	InteractionComp->InteractionTime = 5.f;
	InteractionComp->InteractionDistance = 300.f;
	InteractionComp->InteractableNameText = FText::FromString("Ammo Crate");
	InteractionComp->InteractableActionText = FText::FromString("resupply ammo");
	InteractionComp->bAllowMultipleInteractors = true;
	InteractionComp->SetupAttachment(RootComponent);

	NumResupplies = 5;

	SetReplicates(true);
	SetReplicateMovement(true);

}
