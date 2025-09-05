// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSystems/BeatSubsystem.h"

#include "Components/AudioComponent.h"
#include "Sound/AmbientSound.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "BaseSystems/BeatWorldSettings.h"

DEFINE_LOG_CATEGORY(LogBeat);

// -------------------------------------------- *
// -------------- TICKING LOGIC --------------- *
// -------------------------------------------- *

void UBeatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	check(!bInitialized);

	// Log the world type
	if (UWorld* World = GetWorld())
	{
		UE_LOG(LogBeat, Warning, TEXT("Initializing BeatSubsystem for World: %s (Type: %d)"), *World->GetName(), static_cast<int32>(World->WorldType));
	}
	
	bInitialized = true;
	// UE_LOG(LogBeat, Warning, TEXT("BeatSubsystem Initialized"));
}

void UBeatSubsystem::Deinitialize()
{
	check(bInitialized);
	bInitialized = false;
}

ETickableTickType UBeatSubsystem::GetTickableTickType() const 
{ 
	// By default (if the child class doesn't override GetTickableTickType), don't let CDOs ever tick: 
	return IsTemplate() ? ETickableTickType::Never : FTickableGameObject::GetTickableTickType(); 
}

bool UBeatSubsystem::IsAllowedToTick() const
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
}

// -------------------------------------------- *
// --------------- INITIALIZATION ------------- *
// -------------------------------------------- *

AActor* UBeatSubsystem::CreateHelperActor(const FString& ActorName)
{
	AActor* HelperActor = CachedWorld->SpawnActor<AActor>();
	HelperActor->SetActorLabel(ActorName);
	if (HelperActor)
	{
		// Add a default root component to the actor
		USceneComponent* RootComponent = NewObject<USceneComponent>(HelperActor);
		HelperActor->SetRootComponent(RootComponent);
		RootComponent->RegisterComponent();
	}
	return HelperActor;
}

void UBeatSubsystem::InitializeAudioComponent()
{
	// Create a helper actor to manage the audio component
	AActor* AudioManagerActor = CachedWorld->SpawnActor<AAmbientSound>();
	AudioManagerActor->SetActorLabel(TEXT("AudioManagerActor"));
	if (AudioManagerActor)
	{
		AudioComponent = AudioManagerActor->GetComponentByClass<UAudioComponent>();
		// Create and attach the audio component to the helper actor
		if (AudioComponent)
		{
			AudioComponent->bAutoActivate = false; // Prevent auto-play
			AudioComponent->bIsUISound = false;    // Set as non-UI sound
			AudioComponent->bAllowSpatialization = false; // Disable spatialization for background music
			AudioComponent->OnAudioFinished.AddDynamic(this, &UBeatSubsystem::OnAudioTrackFinished);
		}
		UE_LOG(LogBeat, Log, TEXT("AudioComponent initialized successfully."));
	}
}

void UBeatSubsystem::InitializeTrack()
{
	// The current track is by default the first track in the list
	currentTrack = CachedSettings->worldTracks.Num() > 0 ? CachedSettings->worldTracks[0] : nullptr;

	// Play the track
	if (currentTrack)
	{
		PlayBackgroundMusic(currentTrack->GetAudioFile());
	}
	else
	{
		UE_LOG(LogBeat, Warning, TEXT("UBeatSubsystem::InitializeTrack - CurrentTrack or its AudioFile is null!"));
	}
}

void UBeatSubsystem::InitializeIntervals()
{
	// Clear any existing intervals (in case of re-initialization)
	beatIntervals.Empty();
	// Ensure that there is at least the default interval in the world settings
	CachedSettings->worldBeatInterval.AddUnique(FIntervalData::GetDefault());
	// Create intervals based on the world settings (All duplicates will be ignored)
	beatIntervals = UInterval::CreateIntervalsFromDataList(CachedSettings->worldBeatInterval);
}

void UBeatSubsystem::OnAudioTrackFinished()
{
	if (CachedSettings)
	{
		if (CachedSettings->bLoopModeEnabled)
		{
			// Restart the current track
			PlayBackgroundMusic(currentTrack->GetAudioFile());
		}
		else
		{
			int currentIndex = CachedSettings->worldTracks.IndexOfByKey(currentTrack);
			if (currentIndex != INDEX_NONE && currentIndex < CachedSettings->worldTracks.Num() - 1)
			{
				// Move to the next track in the list
				currentTrack = CachedSettings->worldTracks[currentIndex + 1];
				PlayBackgroundMusic(currentTrack->GetAudioFile());
			}
			else
			{
				// No more tracks to play, return to the first track
				currentTrack = CachedSettings->worldTracks.Num() > 0 ? CachedSettings->worldTracks[0] : nullptr;
				if (currentTrack)
				{
					PlayBackgroundMusic(currentTrack->GetAudioFile());
				}
			}
		}
	}
}

void UBeatSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	UE_LOG(LogBeat, Warning, TEXT("World has begun play in BeatSubsystem"));
	CachedWorld = InWorld.GetWorld();
	checkf(CachedWorld != nullptr, TEXT("UBeatSubsystem::OnWorldBeginPlay - CachedWorld is null!"));
	
	InitializeAudioComponent();
	
	CachedSettings = Cast<ABeatWorldSettings>(InWorld.GetWorldSettings());
	if (CachedSettings != nullptr)
	{
		// Initialize the current track
		InitializeTrack();
		// Initialize the intervals
		InitializeIntervals();
		UE_LOG(LogBeat, Log, TEXT("UBeatSubsystem::OnWorldBeginPlay - BeatSubsystem initialized successfully."));
	}
	else
	{
		UE_LOG(LogBeat, Warning, TEXT("UBeatSubsystem::OnWorldBeginPlay - BeatWorldSettings is null!"));
	}

	bHasBegunPlay = true;
	// Broadcast the begin play event
	OnBeginPlay.Broadcast();
}

// -------------------------------------------- *
// ---------------- BEAT LOGIC ---------------- *
// -------------------------------------------- *

void UBeatSubsystem::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Ticking BeatSubsystem"));
	
	if (CachedWorld == nullptr) return;
	
	// Get the current playback time using audio time for better accuracy.
	const double playbackTime = CachedWorld->GetAudioTimeSeconds() - PlayStartTime; 
	for (UInterval* interval : beatIntervals)
	{
		const double trackLength = currentTrack->GetAudioFile()->GetDuration();
		interval->CheckForNewBeat_Playback(playbackTime, trackLength, currentTrack->GetBpm());
		// const double intervalLength = (60.0 / (currentTrack->GetBpm() * interval->GetSteps()));
		// double intervalTime = playbackTime / intervalLength;
		// interval->CheckForNewBeat(intervalTime, currentTrack->GetBpm());
	}
}

void UBeatSubsystem::PreloadAudio(USoundWave* SoundWave)
{
	if (SoundWave)
	{
		// Ensure the audio is fully loaded into memory
		SoundWave->bStreaming = false; // Disable streaming for preloading
		
		// Use the StreamableManager to load the asset
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestSyncLoad(SoundWave->GetPathName());
		
		UE_LOG(LogBeat, Log, TEXT("Audio preloaded: %s"), *SoundWave->GetName());
	}
	else
	{
		UE_LOG(LogBeat, Warning, TEXT("PreloadAudio - SoundWave is null!"));
	}
}

void UBeatSubsystem::PlayBackgroundMusic(USoundBase* Music)
{
	if (AudioComponent && Music)
	{
		if (USoundWave* SoundWave = Cast<USoundWave>(Music))
		{
			PreloadAudio(SoundWave); // Preload the audio
		}

		// Set and play the music
		AudioComponent->SetSound(Music);
		AudioComponent->Play();

		// Reset the play start time
		UWorld* World = GetWorld();
		checkf(World != nullptr, TEXT("UBeatSubsystem::PlayBackgroundMusic - World is null!"));
		PlayStartTime = World->GetAudioTimeSeconds(); // Use Audio Time Seconds for more accurate timing
	}
}

TArray<UInterval*> UBeatSubsystem::GetIntervalsByData(TArray<FIntervalData> dataList) const
{
	TArray<UInterval*> foundIntervals = {};
	for (UInterval* interval : beatIntervals)
	{
		if (dataList.Contains(interval->GetData()))
		{
			foundIntervals.Add(interval);
		}
	}
	return foundIntervals;
}
