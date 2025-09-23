// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrackData.generated.h"

UCLASS(Blueprintable)
class BEATSIMPLE_API UTrackData : public UDataAsset
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	// class USoundBase* AudioFile;
	TObjectPtr<USoundBase> AudioFile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rhythm)
	int Bpm = 60;

protected:



public:

	UFUNCTION(BlueprintCallable, Category=Track)
	USoundBase* GetAudioFile() const {return AudioFile.Get();}

	UFUNCTION(BlueprintCallable, Category=Rhythm)
	int GetBpm() const {return Bpm;}
	
};

