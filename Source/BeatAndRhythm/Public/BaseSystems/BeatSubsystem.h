// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interval.h"
#include "TrackData.h"
#include "Subsystems/WorldSubsystem.h"
#include "BeatSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBeat, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeginPlayDelegate);

class ABeatWorldSettings;
class UTrackData;
class UInterval;
/**
 * 
 */
UCLASS()
class BEATANDRHYTHM_API UBeatSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

// -------------------------------------------- *
// -------------- TICKING LOGIC --------------- *
// -------------------------------------------- *

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Initialization", meta = (AllowPrivateAccess = "true"))
	bool bInitialized = false;
	
public: // Tick related overrides
	
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsAllowedToTick() const override;
	virtual bool IsTickable() const override
	{
		if (!IsValid(this))
		{
			return false;
		}
		
		return !IsTemplate();
	}
	/*virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UBeatSubsystem, STATGROUP_Tickables);
	}*/

	virtual TStatId GetStatId() const override
	{
		return UObject::GetStatID();
	}

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

// -------------------------------------------- *
// --------------- INITIALIZATION ------------- *
// -------------------------------------------- *

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Initialization", meta = (AllowPrivateAccess = "true"))
	bool bHasBegunPlay = false;
	
public:
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UFUNCTION(BlueprintCallable, Category="Initialization")
	bool HasBegunPlay() const { return bHasBegunPlay; }

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category="Initialization|Events")
	FBeginPlayDelegate OnBeginPlay;
	
protected:

	UFUNCTION(BlueprintCallable, Category = "Initialization")
	void InitializeAudioComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Initialization")
	void InitializeTrack();
	
	UFUNCTION(BlueprintCallable, Category = "Initialization")
	void InitializeIntervals();

	UFUNCTION()
	void OnAudioTrackFinished();

	UFUNCTION(BlueprintCallable, Category = "Helpers")
	AActor* CreateHelperActor(const FString& ActorName);

// -------------------------------------------- *
// ---------------- BEAT LOGIC ---------------- *
// -------------------------------------------- *

/* ---------------- PROPERTIES ---------------- */
	
private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UTrackData* currentTrack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beats", meta = (AllowPrivateAccess = "true"))
	TArray<UInterval*> beatIntervals;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	double PlayStartTime = 0;

	UPROPERTY()
	UWorld* CachedWorld;

	UPROPERTY()
	ABeatWorldSettings* CachedSettings;

/* ----------------- METHODS ------------------ */

protected:

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PreloadAudio(USoundWave* SoundWave);
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayBackgroundMusic(USoundBase* Music);

	UFUNCTION(BlueprintCallable, Category = "Beats|Getters")
	int GetBPM() const { return currentTrack->GetBpm(); }

	
/* ------------------------------------------- *
 * ---------------- INTERVALS ---------------- *
 * ------------------------------------------- */
	
	UFUNCTION(BlueprintCallable, Category = "Beats|Getters")
	TArray<UInterval*> GetIntervals() const { return beatIntervals; }

	UFUNCTION(BlueprintCallable, Category = "Beats|Getters")
	UInterval* GetIntervalByIndex(int index) const 
	{
		if (index >= 0 && index < beatIntervals.Num())
		{
			return beatIntervals[index];
		}
		UE_LOG(LogTemp, Warning, TEXT("UBeatSubsystem::GetIntervalByIndex - Index out of bounds!"));
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Beats|Getters")
	UInterval* GetInterval(const FIntervalData& intervalData) const 
	{
		for (UInterval* interval : beatIntervals)
		{
			if (interval->MatchData(intervalData)) return interval;
		}
		UE_LOG(LogTemp, Warning, TEXT("UBeatSubsystem::GetIntervalBySteps - Interval not found!"));
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Beats|Getters")
	TArray<UInterval*> GetIntervalsByData(TArray<FIntervalData> dataList) const;
	
};
