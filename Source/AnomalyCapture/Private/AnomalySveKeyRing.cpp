#include "AnomalySveKeyRing.h"

#if ANOMALY_CAPTURE

#include "AnomalyCaptureLog.h"

#include "HAL/IConsoleManager.h"
#include "HAL/ThreadSafeCounter.h"
#include "Misc/ScopeLock.h"

namespace AnomalySveKeyRing
{
	static constexpr int32 GRingCapacity = 64;
	static constexpr uint32 GForceMissMask = 0x5A5A5A5Au;

	static TAutoConsoleVariable<int32> CVarForceMiss(
		TEXT("IAI.Capture.SVE.ForceMiss"), 0,
		TEXT("Corrupt published keys so render-thread lookups miss, proving the loud-miss guard fires on the production path. ")
		TEXT("0 = off. 1 = corrupt EVERY key (total failure). N>1 = corrupt every Nth key (INTERMITTENT failure — the shape a ")
		TEXT("real starvation event produces, and the one that tests whether the guard DISCRIMINATES rather than merely notices)."),
		ECVF_Default);

	static FCriticalSection RingCS;
	static TArray<FKeyEntry> Ring;
	static uint64 PublishSerial = 0;

	static FThreadSafeCounter Published;
	static FThreadSafeCounter Consumed;
	static FThreadSafeCounter Missed;
	static FThreadSafeCounter Wrapped;
	static FThreadSafeCounter Corrupted;

	int32 GetCapacity()
	{
		return GRingCapacity;
	}

	int32 GetForceMissMode()
	{
		return FMath::Max(0, CVarForceMiss.GetValueOnAnyThread());
	}

	bool IsForceMiss()
	{
		return GetForceMissMode() != 0;
	}

	void Reset()
	{
		FScopeLock Lock(&RingCS);
		Ring.Reset();
		PublishSerial = 0;
		Published.Reset();
		Consumed.Reset();
		Missed.Reset();
		Wrapped.Reset();
		Corrupted.Reset();
	}

	FCounters GetCounters()
	{
		FCounters Out;
		Out.Published = Published.GetValue();
		Out.Consumed = Consumed.GetValue();
		Out.Missed = Missed.GetValue();
		Out.Wrapped = Wrapped.GetValue();
		Out.Corrupted = Corrupted.GetValue();
		return Out;
	}

	void PublishKey(uint32 FamilyFrameNumber, uint64 GameFrameCounter, bool bWanted)
	{
		const int32 Mode = GetForceMissMode();

		FKeyEntry Entry;
		Entry.GameFrameCounter = GameFrameCounter;
		Entry.bWanted = bWanted;
		Entry.bValid = true;

		FScopeLock Lock(&RingCS);
		Entry.Serial = ++PublishSerial;

		const bool bCorrupt = (Mode == 1) || (Mode > 1 && (Entry.Serial % (uint64)Mode) == 0);
		Entry.FamilyFrameNumber = bCorrupt ? (FamilyFrameNumber ^ GForceMissMask) : FamilyFrameNumber;
		if (bCorrupt)
		{
			Corrupted.Increment();
		}

		Ring.Add(Entry);
		while (Ring.Num() > GRingCapacity)
		{
			Ring.RemoveAt(0);
			Wrapped.Increment();
		}
		Published.Increment();
	}

	bool LookupKey(uint32 FamilyFrameNumber, FKeyEntry& Out)
	{
		{
			FScopeLock Lock(&RingCS);
			for (int32 i = Ring.Num() - 1; i >= 0; --i)
			{
				if (Ring[i].bValid && Ring[i].FamilyFrameNumber == FamilyFrameNumber)
				{
					Out = Ring[i];
					Consumed.Increment();
					return true;
				}
			}
		}
		Missed.Increment();
		return false;
	}

	static void RingSelfTest(const TArray<FString>& Args)
	{
		const int32 Count = (Args.Num() > 0 && Args[0].IsNumeric())
			? FMath::Clamp(FCString::Atoi(*Args[0]), 1, 4096)
			: 8;

		Reset();

		const uint32 BaseFamily = 100000u;
		const uint64 BaseGfc = 900000ull;

		for (int32 i = 0; i < Count; ++i)
		{
			PublishKey(BaseFamily + (uint32)i, BaseGfc + (uint64)i, (i % 2) == 0);
		}

		int32 Hits = 0;
		int32 WantedHits = 0;
		int32 KeyMismatches = 0;
		for (int32 i = 0; i < Count; ++i)
		{
			FKeyEntry Entry;
			if (LookupKey(BaseFamily + (uint32)i, Entry))
			{
				++Hits;
				if (Entry.bWanted)
				{
					++WantedHits;
				}
				if (Entry.GameFrameCounter != BaseGfc + (uint64)i)
				{
					++KeyMismatches;
				}
			}
		}

		const FCounters C = GetCounters();
		const int32 Mode = GetForceMissMode();
		const int32 FirstSurviving = FMath::Max(0, Count - GRingCapacity);

		int32 ExpectedHits = 0;
		for (int32 i = FirstSurviving; i < Count; ++i)
		{
			const uint64 Serial = (uint64)i + 1;
			const bool bCorrupt = (Mode == 1) || (Mode > 1 && (Serial % (uint64)Mode) == 0);
			if (!bCorrupt)
			{
				++ExpectedHits;
			}
		}

		UE_LOG(LogAnomalyCapture, Display,
			TEXT("SVE ring self-test: n=%d capacity=%d forceMiss=%d | published=%d corrupted=%d hits=%d expectedHits=%d misses=%d wrapped=%d keyMismatches=%d wantedHits=%d | %s"),
			Count, GRingCapacity, Mode,
			C.Published, C.Corrupted, Hits, ExpectedHits, C.Missed, C.Wrapped, KeyMismatches, WantedHits,
			(KeyMismatches == 0 && Hits == ExpectedHits) ? TEXT("PASS") : TEXT("FAIL"));

		Reset();
	}

	static FAutoConsoleCommand GRingSelfTestCmd(
		TEXT("IAI.Capture.SVE.RingTest"),
		TEXT("Exercise the B' key ring headlessly: publish N keys, look them all up, report hits/misses/wraps. Optional arg N (default 8)."),
		FConsoleCommandWithArgsDelegate::CreateStatic(&RingSelfTest));
}

#endif
