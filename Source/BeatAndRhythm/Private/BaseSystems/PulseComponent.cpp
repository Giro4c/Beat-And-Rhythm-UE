// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSystems/PulseComponent.h"


// Sets default values for this component's properties
UPulseComponent::UPulseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPulseComponent::BeginPlay()
{
	Super::BeginPlay();
	
	originalScale = GetOwner()->GetActorRelativeScale3D();
	if (useTestBeat)
	{
		UWorld* world = GetWorld();
		checkf(world != nullptr, TEXT("UPulseComponent::BeginPlay: World is null!"));
		world->GetTimerManager().SetTimer(pulseTestTimer, this, &UPulseComponent::Pulse, 1.0f, true);
	}
}


// Called every frame
void UPulseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	const FVector newScale = FMath::Lerp(GetOwner()->GetActorRelativeScale3D(), originalScale, DeltaTime * returnSpeed);
	GetOwner()->SetActorRelativeScale3D(newScale);
}

void UPulseComponent::Pulse()
{
	GetOwner()->SetActorRelativeScale3D(originalScale * pulseScale);
}

