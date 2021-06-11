// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SPlayerController.h"
#include "SGameMode.h"

ASPlayerController::ASPlayerController()
{

}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("Turn", this, &ASPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &ASPlayerController::LookUp);
}

void ASPlayerController::Turn(float Rate)
{
	AddYawInput(Rate);
}

void ASPlayerController::LookUp(float Rate)
{
	AddPitchInput(Rate);
}

void ASPlayerController::PlayerDied()
{
	// here we make the player watch another player

	// 1. Spawn a camera actor thingy

	// 2. Attach that to a spectating player

	// 3. go there with setviewtargetwithblend

	// 4. set self to spectating??

	ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
	GM->OnPlayerDied.Broadcast(this);
}

void ASPlayerController::PlayerRespawn()
{
	// here we make the controller go back to the player pawn

	// unpossess the spectating pawn thingy
	//UnPossess();
	GetPawn()->SetLifeSpan(2.f);

	PawnPendingDestroy(GetPawn());

	//ChangeState(NAME_Inactive);

	if (!HasAuthority())
	{
		ServerPlayerRespawn();
	}
	else
	{
		ServerRestartPlayer();
	}
}

void ASPlayerController::ServerPlayerRespawn_Implementation()
{
	PlayerRespawn();
}

bool ASPlayerController::ServerPlayerRespawn_Validate()
{
	return true;
}
