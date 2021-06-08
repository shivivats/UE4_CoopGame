// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SInteractionWidget.h"
#include "Components/SInteractionComponent.h"

void USInteractionWidget::UpdateInteractionWidget(class USInteractionComponent* InteractionComponent)
{
	OwningInteractionComponent = InteractionComponent;

	OnUpdateInteractionWidget();
}
