// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "RythmEvents.h"
#include "UObject/Object.h"
#include "Interval.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeatDelegate);

UENUM(BlueprintType)
enum EIntervalComparisonFilters
{
	/**
	 * All values must match exactly.
	 */
	DEFAULT,

	/**
	 * Only the steps must match. Offset values not included.
	 */
	NO_OFFSET,

	/**
	 * Offset values related to bpm are not included in the comparison.
	 */
	NO_FIXED_BPM,

	/**
	 * Which filter is applied depend on boolean values (if they match).
	 * For example, if both bUseOffset are true but bFixedBpm are false
	 * then only the steps offset will be used for the comparison (beside the default steps attribute).
	 */
	ADAPTABLE,

	/**
	 * Only the values are compared, no boolean values are taken into account.
	 */
	VALUES_ONLY,

	/**
	 * Only the boolean values are compared, no actual values are taken into account.
	 */
	BOOLEANS_ONLY,

	
	
};

USTRUCT(BlueprintType)
struct FIntervalData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	float Steps = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offsets")
	bool bUseOffset = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offset")
	float OffsetSteps = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offset")
	bool bFixedOffsetBpm = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offset")
	int OffsetBpm = 0;

	bool operator==(const FIntervalData& other) const;

	bool Equals(const FIntervalData& other, const EIntervalComparisonFilters filter = EIntervalComparisonFilters::DEFAULT) const;
	
	/**
	 * Creates a default interval data with 1 step and no offset.
	 * @return The default interval data: 1 step per beat, no offset.
	 */
	static FIntervalData GetDefault()
	{
		return FIntervalData{1.f, false, 0.f, false, 0};
	}
};

// Hash function for FIntervalData
FORCEINLINE uint32 GetTypeHash(const FIntervalData& Data)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Data.Steps));
	Hash = HashCombine(Hash, GetTypeHash(Data.bUseOffset));
	Hash = HashCombine(Hash, GetTypeHash(Data.OffsetSteps));
	Hash = HashCombine(Hash, GetTypeHash(Data.bFixedOffsetBpm));
	Hash = HashCombine(Hash, GetTypeHash(Data.OffsetBpm));
	return Hash;
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntervalTriggeredDelegate, FIntervalData, IntervalData);

/**
 * 
 */
UCLASS(Blueprintable)
class BEATSIMPLE_API UInterval : public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = "true"))
	FIntervalData data;

	UPROPERTY()
	int lastInterval;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events")
	FOnBeatDelegate onTrigger;

	UFUNCTION(BlueprintCallable, Category = "Intervals")
	void CheckForNewBeat(double intervalTime, float bpm);

	UFUNCTION(BlueprintCallable, Category = "Intervals")
	void CheckForNewBeat_Playback(double playbackTime, float bpm);

	UFUNCTION(BlueprintCallable, Category = "Getters")
	float GetSteps() const { return data.Steps; }
	
	UFUNCTION(BlueprintCallable, Category = "Getters")
	bool IsUsingOffset() const { return data.bUseOffset; }
	
	UFUNCTION(BlueprintCallable, Category = "Getters")
	float GetOffsetSteps() const { return data.OffsetSteps; }

	UFUNCTION(BlueprintCallable, Category = "Getters")
	bool IsUsingFixedOffsetBpm() const { return data.bFixedOffsetBpm; }
	
	UFUNCTION(BlueprintCallable, Category = "Getters")
	float GetOffsetBpm() const { return data.OffsetBpm; }

	UFUNCTION(BlueprintCallable, Category=Comparison)
	bool MatchData(const FIntervalData& other, const EIntervalComparisonFilters filter = EIntervalComparisonFilters::DEFAULT) const;

	UFUNCTION(BlueprintCallable, Category = "Getters")
	FIntervalData GetData() const { return data; }

	UFUNCTION(BlueprintCallable, Category=Initialization)
	static TArray<UInterval*> CreateIntervalsFromDataList(const TArray<FIntervalData>& dataList);
	
};
