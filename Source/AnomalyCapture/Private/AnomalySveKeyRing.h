#pragma once

#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

namespace AnomalySveKeyRing
{
	struct FKeyEntry
	{
		uint32 FamilyFrameNumber = 0;
		uint64 GameFrameCounter = 0;
		uint64 Serial = 0;
		bool bWanted = false;
		bool bValid = false;
	};

	struct FCounters
	{
		int32 Published = 0;
		int32 Consumed = 0;
		int32 Missed = 0;
		int32 Wrapped = 0;
		int32 Corrupted = 0;
	};

	void PublishKey(uint32 FamilyFrameNumber, uint64 GameFrameCounter, bool bWanted);
	bool LookupKey(uint32 FamilyFrameNumber, FKeyEntry& Out);

	void Reset();
	FCounters GetCounters();

	int32 GetCapacity();
	int32 GetForceMissMode();
	int32 GetForceMissPhase();
	bool IsForceMiss();
}

#endif
