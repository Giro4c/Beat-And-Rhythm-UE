// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSystems/Interval.h"
#include "BaseSystems/BeatSubsystem.h"

bool FIntervalData::operator==(const FIntervalData& other) const
{
	return Steps == other.Steps
		&& bUseOffset == other.bUseOffset
		&& OffsetSteps == other.OffsetSteps
		&& bFixedOffsetBpm == other.bFixedOffsetBpm
		&& OffsetBpm == other.OffsetBpm;
}

bool FIntervalData::Equals(const FIntervalData& other, const EIntervalComparisonFilters filter) const
{
	switch (filter)
	{
		case DEFAULT:
			return *this == other;
		case NO_OFFSET:
			return this->Steps == other.Steps;
		case NO_FIXED_BPM:
			return this->Steps == other.Steps
				&& this->bUseOffset == other.bUseOffset
				&& this->OffsetSteps == other.OffsetSteps;
		case ADAPTABLE:
			if (this->Steps != other.Steps) return false;
			if (this->bUseOffset != other.bUseOffset) return false;
			if (this->bUseOffset)
			{
				if (this->OffsetSteps != other.OffsetSteps) return false;
				if (this->bFixedOffsetBpm != other.bFixedOffsetBpm) return false;
				if (this->bFixedOffsetBpm)
				{
					if (this->OffsetBpm != other.OffsetBpm) return false;
				}
			}
			return true;
		case VALUES_ONLY:
			return this->Steps == other.Steps
				&& this->OffsetSteps == other.OffsetSteps
				&& this->OffsetBpm == other.OffsetBpm;
		case BOOLEANS_ONLY:
			return this->bUseOffset == other.bUseOffset
				&& this->bFixedOffsetBpm == other.bFixedOffsetBpm;
		
		default:
			return false;
	}
}

void UInterval::CheckForNewBeat(double intervalTime, float bpm)
{
	if (data.bUseOffset)
	{
		// Which bpm to use for the offset length calculation?
		const float offsetLength = data.bFixedOffsetBpm ? 60.f / (data.OffsetBpm * data.OffsetSteps) : (60.f / (bpm * data.OffsetSteps));
		intervalTime += offsetLength;
	}

	UE_LOG(LogBeat, Log, TEXT("UIntervals::CheckForNewBeat - Current Interval: %f"), intervalTime);
            
	if (static_cast<int>(intervalTime) != lastInterval)
	{
		lastInterval = static_cast<int>(intervalTime);
		UE_LOG(LogBeat, Log, TEXT("UIntervals::CheckForNewBeat - Interval Triggered: Steps: %f, OffsetSteps: %f, FixedOffsetBpm: %s, OffsetBpm: %d"), data.Steps, data.OffsetSteps, data.bFixedOffsetBpm ? TEXT("True") : TEXT("False"), data.OffsetBpm);
		onTrigger.Broadcast();
	}
}

void UInterval::CheckForNewBeat_Playback(double playbackTime, double trackLength, float bpm)
{
	// const double intervalLength = trackLength / (60.0 / (bpm * data.Steps));
	const double intervalLength = (60.0 / (bpm * data.Steps));
	UE_LOG(LogBeat, Log, TEXT("UIntervals::CheckForNewBeat_Playback - Interval Length: %f"), intervalLength);
	
	double currentInterval = playbackTime / intervalLength;
	if (data.bUseOffset)
	{
		// Which bpm to use for the offset length calculation?
		const double offsetLength = data.bFixedOffsetBpm ? 60.0 / (data.OffsetBpm * data.OffsetSteps) : (60.0 / (bpm * data.OffsetSteps));
		currentInterval += offsetLength;
	}

	UE_LOG(LogBeat, Log, TEXT("UIntervals::CheckForNewBeat_Playback - Current Interval: %f"), currentInterval);
            
	if (static_cast<int>(currentInterval) != lastInterval)
	{
		lastInterval = static_cast<int>(currentInterval);
		UE_LOG(LogBeat, Log, TEXT("UIntervals::CheckForNewBeat_Playback - Interval Triggered: Steps: %f, OffsetSteps: %f, FixedOffsetBpm: %s, OffsetBpm: %d"), data.Steps, data.OffsetSteps, data.bFixedOffsetBpm ? TEXT("True") : TEXT("False"), data.OffsetBpm);
		onTrigger.Broadcast();
	}
}

bool UInterval::MatchData(const FIntervalData& other, const EIntervalComparisonFilters filter) const
{
	return data.Equals(other, filter);
}

TArray<UInterval*> UInterval::CreateIntervalsFromDataList(const TArray<FIntervalData>& dataList)
{
	TSet<FIntervalData> uniqueDataSet; // To store unique data
	TArray<UInterval*> intervals;    // To store created objects

	for (int i = 0; i < dataList.Num(); i++)
	// for (const FIntervalData& data : dataList)
	{
		const FIntervalData& data = dataList[i];
		if (!uniqueDataSet.Contains(data))
		{
			uniqueDataSet.Add(data);

			UInterval* newInterval = NewObject<UInterval>();
			newInterval->data = data;
			intervals.Add(newInterval);
		}
	}

	return intervals;
}
