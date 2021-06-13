// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NetworkReplayStreaming.h"
#include "SReplayGameInstance.generated.h"

/*
USTRUCT(BlueprintType)
struct FS_ReplayInfo {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString ReplayName;

	UPROPERTY(BlueprintReadOnly)
	FString FriendlyName;

	UPROPERTY(BlueprintReadOnly)
	FDateTime Timestamp;

	UPROPERTY(BlueprintReadOnly)
	int32 LengthInMS;

	UPROPERTY(BlueprintReadOnly)
	bool bIsValid;

	FS_ReplayInfo(FString NewName, FString NewFriendlyName, FDateTime NewTimestamp, int32 NewLengthInMS)
	{
		ReplayName = NewName;
		FriendlyName = NewFriendlyName;
		Timestamp = NewTimestamp;
		LengthInMS = NewLengthInMS;
		bIsValid = true;
	}

	FS_ReplayInfo()
	{
		ReplayName = "Replay";
		FriendlyName = "Replay";
		Timestamp = FDateTime::MinValue();
		LengthInMS = 0;
		bIsValid = false;
	}

}; */

/**
 *
 */
UCLASS()
class COOPGAME_API USReplayGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USReplayGameInstance();

	/* Name of replay file on disk. */
	UPROPERTY(EditDefaultsOnly, Category = "Replays")
		FString RecordingName;

	/* Name of replay in UI. */
	UPROPERTY(EditDefaultsOnly, Category = "Replays")
		FString FriendlyRecordingName;

	UFUNCTION(BlueprintCallable, Category = "Replays")
		void StartRecording(FString ReplayName, FString FriendlyName);

	UFUNCTION(BlueprintCallable, Category = "Replays")
		void StopRecording();

	UFUNCTION(BlueprintCallable, Category = "Replays")
		void PlayRecordedReplay(FString ReplayName);
/*
	UFUNCTION(BlueprintCallable, Category = "Replays")
		void FindReplays();

	UFUNCTION(BlueprintCallable, Category = "Replays")
		void RenameReplay(const FString& ReplayName, const FString& NewFriendlyReplayName);

	UFUNCTION(BlueprintCallable, Category = "Replays")
		void DeleteReplay(const FString& ReplayName);

	virtual void Init() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Replays")
	void BP_OnFindReplaysComplete(const TArray& AllReplays);

private:

	// for FindReplays()
	TSharedPtr EnumerateStreamsPtr;

	FOnEnumerateStreamsComplete OnEnumerateStreamsCompleteDelegate;

	void OnEnumerateStreamsComplete(const TArray& StreamInfos);

	// for DeleteReplays(..)
	FOnDeleteFinishedStreamComplete OnDeleteFinishedStreamCompleteDelegate;

	void OnDeleteFinishedStreamComplete(const bool bDeleteSucceeded);
*/
};
