// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Barricade.h"
#include "Components/BoxComponent.h"
#include "Components/SInteractionComponent.h"
#include "CoopGame/CoopGame.h"

// Sets default values
ABarricade::ABarricade()
{
	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrameMesh"));
	RootComponent = FrameMesh;

	BarricadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarricadeMesh"));
	BarricadeMesh->SetVisibility(false);
	BarricadeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Scale used for default cube mesh is 0.125, 0.875, 1.375
	BarricadeMesh->SetupAttachment(RootComponent);

	InteractingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractingBox"));
	InteractingBox->SetBoxExtent(FVector(11.f, 40.f, 62.f));
	InteractingBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractingBox->SetCollisionResponseToChannel(COLLISION_INTERACTACTION, ECR_Block);
	InteractingBox->SetupAttachment(RootComponent);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>(TEXT("InteractionComp"));
	InteractionComp->InteractionTime = 3.f;
	InteractionComp->InteractionDistance = 300.f;
	InteractionComp->InteractableNameText = FText::FromString("");
	InteractionComp->InteractableActionText = FText::FromString("Barricade");
	InteractionComp->bAllowMultipleInteractors = false;

	InteractionComp->SetupAttachment(RootComponent);

	/** @TODO: Setup default values here.*/

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ABarricade::BeginPlay()
{
	Super::BeginPlay();
	
}
