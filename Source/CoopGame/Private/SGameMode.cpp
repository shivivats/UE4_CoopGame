// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "TimerManager.h"
#include "Components/SHealthComponent.h"
#include "EngineUtils.h"


ASGameMode::ASGameMode()
{
	BotSpawnRate = 1.f;
	InitialBotSpawnDelay = 0.f;

	TimeBetweenWaves = 2.f;

	PrimaryActorTick.bCanEverTick = true;

	// make it tick every one second
	PrimaryActorTick.TickInterval = 1.f;
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// check the wave state every second
	CheckWaveState();

	// check every second if any players are alive
	CheckAnyPlayerAlive();
}

void ASGameMode::StartWave()
{
	WaveCount++;

	NrOfBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnBotTimerElapsed, BotSpawnRate, true, InitialBotSpawnDelay);
}

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
}

void ASGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBetweenWaves, false);
}

void ASGameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	// dont run this logic if we're still in the spawning bots phase or in the preparing for wave phase
	if (NrOfBotsToSpawn > 0 || bIsPreparingForWave)
	{
		return;
	}

	bool bIsAnyBotAlive = false;

	// iterate over all pawns in the world
	for (TActorIterator<APawn> Itr(GetWorld()); Itr; ++Itr)
	{
		APawn* TestPawn = *Itr;
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->FindComponentByClass(USHealthComponent::StaticClass()));
		if(HealthComp && HealthComp->GetHealth() > 0.f)
		{
			// we found an alive bot
			bIsAnyBotAlive = true;

			// now we can stop searching bc we found a bot
			break;
		}
	}

	if(!bIsAnyBotAlive)
	{
		PrepareForNextWave();
	}
}

void ASGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			USHealthComponent* HealthComp = Cast<USHealthComponent>(MyPawn->FindComponentByClass(USHealthComponent::StaticClass()));
			// ensure throws a breakpoint when we cant find the healthcomponent on the player
			// throwing a breakpoint makes sure that we realise that there's something wrong with our code
			// since all players should have a health component attached
			if(ensure(HealthComp) && HealthComp->GetHealth() > 0.f)
			{
				// a player is still alive

				//FString Name = MyPawn->GetName();
				//UE_LOG(LogTemp, Log, TEXT("found a player alive with name %s"), *Name);
				
				return;
			}
		}
	}

	// No Player alive
	GameOver();
}

void ASGameMode::GameOver()
{
	EndWave();

	// @TODO: Finish up the match, present 'game over' to the players
	UE_LOG(LogTemp, Log, TEXT("GAME OVER! Players Died"));
}

void ASGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();
	
	NrOfBotsToSpawn--;

	if(NrOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}