// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interval.h"
#include "GameFramework/WorldSettings.h"
#include "BeatWorldSettings.generated.h"

UENUM(BlueprintType)
enum class EPlayMode : uint8
{
	Looping,
	Sequential,
	Random
};

class UTrackData;
/**
 * 
 */
UCLASS()
class BEATANDRHYTHM_API ABeatWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Beat & Rhythm|Tracks")
	TArray<UTrackData*> worldTracks;

	/**
	 * A list that will define all regular rhythms that emulate beats.
	 * Default Interval (step 1 offset 0) will always exist and duplicate intervals cannot exist.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Beat & Rhythm|Rhythm")
	TArray<FIntervalData> worldBeatInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Beat & Rhythm|Tracks")
	EPlayMode tracksPlayMode = EPlayMode::Looping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Beat & Rhythm|Rhythm")
	FIntervalData rhythmInterval;
};
