// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interval.h"
#include "GameFramework/Actor.h"
#include "BeatPulseActor.generated.h"

UCLASS()
class BEATANDRHYTHM_API ABeatPulseActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABeatPulseActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pulse")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pulse")
	class UPulseComponent* PulseComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulse")
	TArray<FIntervalData> linkedIntervals;

public:

	UFUNCTION()
	void OnBeat();
	
};
