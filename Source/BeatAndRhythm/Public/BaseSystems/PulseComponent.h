// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PulseComponent.generated.h"


UCLASS(ClassGroup=(Beat), meta=(BlueprintSpawnableComponent))
class BEATANDRHYTHM_API UPulseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPulseComponent();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulse", meta = (AllowPrivateAccess = "true"))
	float pulseScale = 1.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulse", meta = (AllowPrivateAccess = "true"))
	float returnSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	bool useTestBeat = false;

	UPROPERTY()
	FTimerHandle pulseTestTimer;

	UPROPERTY()
	FVector originalScale;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Pulse")
	void Pulse();

};
