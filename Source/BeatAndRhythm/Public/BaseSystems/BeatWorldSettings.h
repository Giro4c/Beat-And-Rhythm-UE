// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interval.h"
#include "GameFramework/WorldSettings.h"
#include "BeatWorldSettings.generated.h"

class UTrackData;
/**
 * 
 */
UCLASS()
class BEATANDRHYTHM_API ABeatWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Tracks)
	TArray<UTrackData*> worldTracks;

	/**
	 * A list that will define all regular rhythms that emulate beats.
	 * Default Interval (step 1 offset 0) will always exist and duplicate intervals cannot exist.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Rhythm)
	TArray<FIntervalData> worldBeatInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Tracks)
	bool bLoopModeEnabled = true;
	
};
