// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SInteractionComponent.h"
#include "SCharacter.h"
#include "Widgets/SInteractionWidget.h"


USInteractionComponent::USInteractionComponent()
{
	SetComponentTickEnabled(false);

	InteractionTime = 0.f;
	InteractionDistance = 200.f; // Distance is in cms
	InteractableNameText = FText::FromString("Interactable Object");
	InteractableActionText = FText::FromString("Interact");
	bAllowMultipleInteractors = true;

	// Draw widget in worldspace or screen space? We draw in screen space
	Space = EWidgetSpace::Screen;
	// How big the UI is going to be on the screen
	DrawSize = FIntPoint(600, 100);
	bDrawAtDesiredSize = true;

	SetActive(true);
	// The reason we're hiding it in game is bc we dont want it to show up by default
	// It only shows up when player gets closer to it
	SetHiddenInGame(true);
}

void USInteractionComponent::SetInteractableNameText(const FText& NewNameText)
{
	InteractableNameText = NewNameText;
	RefreshWidget();
}

void USInteractionComponent::SetInteractableActionText(const FText& NewActionText)
{
	InteractableActionText = NewActionText;
	RefreshWidget();
}

void USInteractionComponent::Deactivate()
{
	Super::Deactivate();

	// loop over all the interactors for this object
	for (int32 i = Interactors.Num() - 1; i >= 0; --i)
	{
		if (ASCharacter* Interactor = Interactors[i])
		{
			EndFocus(Interactor);
			EndInteract(Interactor);
		}
	}

	// clear all the interactors out
	Interactors.Empty();
}

bool USInteractionComponent::CanInteract(class ASCharacter* Character) const
{
	// here if we allow multiple interactors, first bool is true
	// if we have more than one interactor, that will be true, so overall true
	// and that should fail
	
	// if we dont allow multiple interactors, first one is false
	// if we already have one itneracting, interactors num will be >= 1, hence true
	// hence overall check will fail

	const bool bPlayerAlreadyInteracting = !bAllowMultipleInteractors && Interactors.Num() >= 1;

	// check some conditions for if we can interact
	return !bPlayerAlreadyInteracting && IsActive() && GetOwner() != nullptr && Character != nullptr;
}

void USInteractionComponent::RefreshWidget()
{
	if (!bHiddenInGame)
	{
		// Make sure the widget is initialised and that we are displaying the right values (these may have changed)
		if (USInteractionWidget* InteractionWidget = Cast<USInteractionWidget>(GetUserWidgetObject()))
		{
			InteractionWidget->UpdateInteractionWidget(this);
		}
	}
}

void USInteractionComponent::BeginFocus(class ASCharacter* Character)
{
	if (!IsActive() || !GetOwner() || !Character)
	{
		return;
	}

	// call the delegate
	OnBeginFocus.Broadcast(Character);

	// show this interactable in game
	SetHiddenInGame(false);

	/*
	// if not the server
	if (!GetOwner()->HasAuthority())
	{
		// grab any visual components (they are called primitive components in ue4)
		for (auto& VisualComp : GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass()))
		{
			// this enables the outline around the visual object
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
			{
				Prim->SetRenderCustomDepth(true);
			}
		}
	}
	*/

	RefreshWidget();
}

void USInteractionComponent::EndFocus(class ASCharacter* Character)
{
	// call the delegate
	OnEndFocus.Broadcast(Character);

	// hide this interactable in game
	SetHiddenInGame(true);

	/*
	// if not the server
	if (!GetOwner()->HasAuthority())
	{
		// grab any visual components (they are called primitive components in ue4)
		for (auto& VisualComp : GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass()))
		{
			// this disables the outline around the visual object
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
			{
				Prim->SetRenderCustomDepth(false);
			}
		}
	}
	*/
}

void USInteractionComponent::BeginInteract(class ASCharacter* Character)
{
	if (CanInteract(Character))
	{
		Interactors.AddUnique(Character);
		OnBeginInteract.Broadcast(Character);
	}
}

void USInteractionComponent::EndInteract(class ASCharacter* Character)
{
	Interactors.RemoveSingle(Character);
	OnEndInteract.Broadcast(Character);
}

void USInteractionComponent::Interact(class ASCharacter* Character)
{
	//if (CanInteract(Character))
	//{
	UE_LOG(LogTemp, Warning, TEXT("calling interact delegate"));

	// call the delegate
	OnInteract.Broadcast(Character);
	//}

}

float USInteractionComponent::GetInteractPercentage()
{
	// the first interactor is us always on our local machine
	if (Interactors.IsValidIndex(0))
	{
		if (ASCharacter* Interactor = Interactors[0])
		{
			if (Interactor && Interactor->IsInteracting())
			{
				return 1.f - FMath::Abs(Interactor->GetRemainingInteractTime() / InteractionTime);
			}
		}
	}

	return 0.f;
}
