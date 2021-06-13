// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/SReplayGameInstance.h"

USReplayGameInstance::USReplayGameInstance()
{
	RecordingName = "MyReplay";
	FriendlyRecordingName = "My Replay";
}

void USReplayGameInstance::StartRecording(FString ReplayName, FString FriendlyName)
{
	StartRecordingReplay(ReplayName, FriendlyName);
}

void USReplayGameInstance::StopRecording()
{
	StopRecordingReplay();
}

void USReplayGameInstance::PlayRecordedReplay(FString ReplayName)
{
	PlayReplay(ReplayName, nullptr);
}
