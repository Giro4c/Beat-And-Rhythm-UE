// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BeatGlobalSettings.generated.h"

/**
 * 
 */
UCLASS(Config=BeatAndRhythm, defaultconfig, meta = (DisplayName="Beat & Rhythm Settings"))
class BEATANDRHYTHM_API UBeatAndRhythmSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category="Timing",
		meta=(ClampMin="0.0", UIMin="0.0", Units="s"))
	float DefaultTimingMargin = 0.15f; // seconds
	
};
