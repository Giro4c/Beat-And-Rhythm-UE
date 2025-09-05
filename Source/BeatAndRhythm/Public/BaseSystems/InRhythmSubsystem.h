// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "InRhythmSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRhythm, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStreakUpdateDelegate, int, NewStreak);

/**
 * 
 */
UCLASS()
class BEATANDRHYTHM_API UInRhythmSubsystem : public UWorldSubsystem/*, public FTickableGameObject*/
{
	GENERATED_BODY()

// -------------------------------------------- *
// -------------- TICKING LOGIC --------------- *
// -------------------------------------------- *
	
private:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Initialization", meta=(AllowPrivateAccess="true"))
    bool bInitialized = false;
	
public: // Tick related overrides
	
	/*virtual void Tick(float DeltaTime) override;
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

	virtual TStatId GetStatId() const override
	{
		return UObject::GetStatID();
	}*/

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

// -------------------------------------------- *
// --------------- INITIALIZATION ------------- *
// -------------------------------------------- *

private:

	UFUNCTION()
	void SyncToBeatSubsystem();
	
public:
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

// -------------------------------------------- *
// --------------- RHYTHM LOGIC --------------- *
// -------------------------------------------- *

/* ---------------- PROPERTIES ---------------- */
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rhythm", meta=(AllowPrivateAccess="true"))
	int Streak = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rhythm", meta=(AllowPrivateAccess="true"))
	double TimingMargin = 0.15; // seconds

	/**
	 * Whether an action was detected in a case where the streak does not immediately breaks.
	 * (E.g. when the action is before the next beat and still within the timing margin)
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rhythm|Timers", meta=(AllowPrivateAccess="true"))
	bool bActionInValidation = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rhythm|Timers", meta=(AllowPrivateAccess="true"))
	FTimerHandle ActionTimeoutHandle;


	/**
	 * Whether an beat was detected in a case where the streak does not immediately increase.
	 * (E.g. when the beat is detected but there can still be an action within the timing margin)
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rhythm|Timers", meta=(AllowPrivateAccess="true"))
	bool bBeatInValidation = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rhythm|Timers", meta=(AllowPrivateAccess="true"))
	FTimerHandle BeatTimeoutHandle;


public:

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category="Rhythm|Events")
	FOnStreakUpdateDelegate OnStreakUpdate;


/* ----------------- METHODS ------------------ */

public:
	
	UFUNCTION(BlueprintCallable, Category="Rhythm|Streak")
	int GetStreak() const { return Streak; }

	UFUNCTION(BlueprintCallable, Category="Rhythm|Streak")
	void SetStreak(int NewStreak);
	
	UFUNCTION(BlueprintCallable, Category="Rhythm|Streak")
	void ResetStreak();

	UFUNCTION(BlueprintCallable, Category="Rhythm|Streak")
	void IncrementStreak();

	UFUNCTION(BlueprintCallable, Category="Rhythm|Input")
	void ActionDetected();

	UFUNCTION(BlueprintCallable, Category="Rhythm|Input")
	void BeatDetected();

private:
	
	UFUNCTION(Category="Rhythm|Input")
	void ActionTimeout();

	UFUNCTION(Category="Rhythm|Input")
	void BeatTimeout();
	
};
