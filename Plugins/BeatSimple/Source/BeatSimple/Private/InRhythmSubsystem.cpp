// Fill out your copyright notice in the Description page of Project Settings.


#include "InRhythmSubsystem.h"

#include "BeatGlobalSettings.h"
#include "BeatSubsystem.h"
#include "BeatWorldSettings.h"

DEFINE_LOG_CATEGORY(LogRhythm);

// -------------------------------------------- *
// -------------- TICKING LOGIC --------------- *
// -------------------------------------------- *

/*ETickableTickType UInRhythmSubsystem::GetTickableTickType() const 
{ 
	// By default (if the child class doesn't override GetTickableTickType), don't let CDOs ever tick: 
	return IsTemplate() ? ETickableTickType::Never : FTickableGameObject::GetTickableTickType(); 
}

bool UInRhythmSubsystem::IsAllowedToTick() const
{
	if (!bInitialized || IsTemplate())
	{
		return false;
	}

	if (UWorld* World = GetWorld())
	{
		// Disable ticking for editor worlds
		return !(World->WorldType == EWorldType::Editor || World->WorldType == EWorldType::EditorPreview);
	}
	return false;
}*/

void UInRhythmSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	check(!bInitialized);

	// Log the world type
	if (UWorld* World = GetWorld())
	{
		UE_LOG(LogRhythm, Warning, TEXT("Initializing BeatSubsystem for World: %s (Type: %d)"), *World->GetName(), static_cast<int32>(World->WorldType));
	}
	
	bInitialized = true;
	// UE_LOG(LogRhythm, Warning, TEXT("BeatSubsystem Initialized"));
}

void UInRhythmSubsystem::Deinitialize()
{
	check(bInitialized);
	bInitialized = false;
}

// -------------------------------------------- *
// --------------- INITIALIZATION ------------- *
// -------------------------------------------- *

void UInRhythmSubsystem::SyncToBeatSubsystem()
{
	UWorld& World = GetWorldRef();
	// Synchronize beats with BeatSubsystem
	if (UBeatSubsystem* BeatSubsystem = World.GetSubsystem<UBeatSubsystem>())
	{
		UInterval* FollowedInterval;
		
		ABeatWorldSettings* WorldSettings = Cast<ABeatWorldSettings>(World.GetWorldSettings());
		if (WorldSettings)
		{
			// Get the interval to follow from the world settings
			FollowedInterval = BeatSubsystem->GetInterval(WorldSettings->rhythmInterval);
			if (FollowedInterval == nullptr)
			{
				UE_LOG(LogRhythm, Error, TEXT("Interval from WorldSettings not found in BeatSubsystem!"));
				FollowedInterval = BeatSubsystem->GetInterval(FIntervalData::GetDefault());
			}
		}
		else
		{
			// Get the default interval if the world settings is not of the correct type
			FollowedInterval = BeatSubsystem->GetInterval(FIntervalData::GetDefault());
			UE_LOG(LogRhythm, Error, TEXT("WorldSettings is not of type ABeatRhythmWorldSettings! Cannot chose an interval to follow. Reverting to default one."));
		}
		
		// Check that we have a valid interval to follow. Gets the default one if beat subsystem didn't have it.
		if (FollowedInterval != nullptr)
		{
			// Get the beat event from the default interval
			FollowedInterval->onTrigger.AddDynamic(this, &UInRhythmSubsystem::BeatDetected);
			UE_LOG(LogRhythm, Warning, TEXT("Synchronized with BeatSubsystem"));
		}
		else
		{
			UE_LOG(LogRhythm, Error, TEXT("Rhythm Interval not found in BeatSubsystem! Cannot synchronize with beats."));
		}
		
		// Unbind from the OnBeginPlay event to avoid multiple bindings
		BeatSubsystem->OnBeginPlay.RemoveDynamic(this, &UInRhythmSubsystem::SyncToBeatSubsystem);
	}
	else
	{
		UE_LOG(LogRhythm, Error, TEXT("BeatSubsystem not found! Cannot synchronize with beats."));
	}
}

void UInRhythmSubsystem::ApplySettings()
{
	if (const UBeatAndRhythmSettings* Settings = GetDefault<UBeatAndRhythmSettings>())
	{
		TimingMargin = Settings->DefaultTimingMargin;
		UE_LOG(LogRhythm, Warning, TEXT("Applied Settings: TimingMargin = %f"), TimingMargin);
	}
	else
	{
		UE_LOG(LogRhythm, Error, TEXT("Failed to get BeatAndRhythmSettings! Using default values."));
	}
}

void UInRhythmSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	UE_LOG(LogRhythm, Warning, TEXT("InRhythmSubsystem OnWorldBeginPlay"));

	// Synchronize beats with BeatSubsystem
	if (UBeatSubsystem* BeatSubsystem = InWorld.GetSubsystem<UBeatSubsystem>())
	{
		if (BeatSubsystem->HasBegunPlay())
		{
			SyncToBeatSubsystem();
		}
		else
		{
			// If BeatSubsystem is not initialized yet, bind to its OnBeginPlay event
			BeatSubsystem->OnBeginPlay.AddDynamic(this, &UInRhythmSubsystem::SyncToBeatSubsystem);
			UE_LOG(LogRhythm, Warning, TEXT("Waiting for BeatSubsystem to initialize..."));
		}
	}
	else
	{
		UE_LOG(LogRhythm, Error, TEXT("BeatSubsystem not found! Cannot synchronize with beats."));
	}
}

// -------------------------------------------- *
// --------------- RHYTHM LOGIC --------------- *
// -------------------------------------------- *


void UInRhythmSubsystem::SetStreak(int NewStreak)
{
	Streak = NewStreak;
	OnStreakUpdate.Broadcast(Streak);
}

void UInRhythmSubsystem::ResetStreak()
{
	SetStreak(0);
}

void UInRhythmSubsystem::IncrementStreak()
{
	++Streak;
	OnStreakUpdate.Broadcast(Streak);
}

void UInRhythmSubsystem::ActionDetected()
{
	UE_LOG(LogRhythm, Warning, TEXT("Action Detected"));

	// Action in waiting ?
	if (bActionInValidation)
	{
		UE_LOG(LogRhythm, Log, TEXT("Action Timeout - Streak Broken (Case: Another Action before Beat)"));
		// Clear the previous timeout (to prevent early timeout for the new action)
		GetWorldRef().GetTimerManager().ClearTimer(ActionTimeoutHandle);
		// Trigger the timeout now (for the previous action)
		ActionTimeout();
		// Reset the timer for the new action
		bActionInValidation = true;
		GetWorldRef().GetTimerManager().SetTimer(ActionTimeoutHandle, this, &UInRhythmSubsystem::ActionTimeout, TimingMargin, false);
		return;
	}

	// No action in waiting
	// Beat in waiting ?
	if (bBeatInValidation)
	{
		// Action was performed within the timing margin after a beat
		UE_LOG(LogRhythm, Log, TEXT("Action Validated - Streak Increased (Case: Action after Beat)"));
		// Increment streak
		IncrementStreak();
		// Stop beat timeout
		GetWorldRef().GetTimerManager().ClearTimer(BeatTimeoutHandle);
		// No longer in validation state (waiting for an action)
		bBeatInValidation = false;
	}
	else
	{
		UE_LOG(LogRhythm, Log, TEXT("Action Waiting for Beat (Case: Action before Beat)"));
		// No beat in waiting
		// Start action timeout (to wait for a beat within the timing margin)
		bActionInValidation = true;
		GetWorldRef().GetTimerManager().SetTimer(ActionTimeoutHandle, this, &UInRhythmSubsystem::ActionTimeout, TimingMargin, false);
	}

	
}

void UInRhythmSubsystem::ActionTimeout()
{
	// The action has timed out
	// Timeout is triggered after the timing margin has passed without a beat
	// A beat or another action (before a beat) will stop the timer supposed to execute this function
	UE_LOG(LogRhythm, Log, TEXT("Action Timeout - Streak Broken"));
	
	// Reset the streak
	ResetStreak();
	// No longer in validation state (waiting for a beat)
	bActionInValidation = false;
}


void UInRhythmSubsystem::BeatDetected()
{
	// Action in waiting ?
	if (bActionInValidation)
	{
		UE_LOG(LogRhythm, Log, TEXT("Action Validated - Streak Increased (Case: Beat after Action)"));
		// Increment streak
		IncrementStreak();
		// Stop action timeout
		GetWorldRef().GetTimerManager().ClearTimer(ActionTimeoutHandle);
		// No longer in validation state (waiting for a beat)
		bActionInValidation = false;
		return;
	}

	// No action in waiting
	if (bBeatInValidation)
	{
		// Stop beat timeout
		GetWorldRef().GetTimerManager().ClearTimer(BeatTimeoutHandle);
		// Call the timeout now (for the previous beat)
		BeatTimeout(); // Changes nothing but here in for future rhythm modes (e.g. missing a beat breaks the streak)
	}

	// Start beat timeout (to wait for an action within the timing margin)
	bBeatInValidation = true;
	GetWorldRef().GetTimerManager().SetTimer(BeatTimeoutHandle, this, &UInRhythmSubsystem::BeatTimeout, TimingMargin, false);
}

void UInRhythmSubsystem::BeatTimeout()
{
	// The beat has timed out
	// Missing a beat does not break the streak
	UE_LOG(LogRhythm, Log, TEXT("Beat Timeout - Streak Unchanged"));
	
	// No longer in validation state (waiting for an action)
	bBeatInValidation = false;
}
