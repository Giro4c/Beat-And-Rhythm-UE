// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSystems/BeatPulseActor.h"

#include "BaseSystems/BeatSubsystem.h"
#include "BaseSystems/PulseComponent.h"


// Sets default values
ABeatPulseActor::ABeatPulseActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	PulseComponent = CreateDefaultSubobject<UPulseComponent>(TEXT("PulseComponent"));
	MeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABeatPulseActor::BeginPlay()
{
	Super::BeginPlay();

	if (const UBeatSubsystem* beatSubsystem = GetWorld()->GetSubsystem<UBeatSubsystem>())
	{
		TArray<UInterval*> intervals = beatSubsystem->GetIntervalsByData(linkedIntervals);
		for (UInterval* interval : intervals)
		{
			interval->onTrigger.AddDynamic(this, &ABeatPulseActor::OnBeat);
		}
	}
	
	
}

void ABeatPulseActor::OnBeat()
{
	if (PulseComponent)
	{
		PulseComponent->Pulse();
	}
}



