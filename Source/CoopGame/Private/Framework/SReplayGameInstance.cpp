// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/SReplayGameInstance.h"

USReplayGameInstance::USReplayGameInstance()
{
	RecordingName = "MyReplay";
	FriendlyRecordingName = "My Replay";
}

void USReplayGameInstance::StartRecording()
{
	StartRecordingReplay(RecordingName, FriendlyRecordingName);
}

void USReplayGameInstance::StopRecording()
{
	StopRecordingReplay();
}

void USReplayGameInstance::ReplayRecording()
{
	PlayReplay(RecordingName, nullptr);
}
