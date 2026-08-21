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

	static TAutoConsoleVariable<int32> CVarForceMissPhase(
		TEXT("IAI.Capture.SVE.ForceMissPhase"), 0,
		TEXT("Phase offset for the every-Nth-key corruption, so it can be shifted relative to the capture cadence. ")
		TEXT("Both are periodic and phase-lock at offset 0: an unshifted sweep can drop only non-positive frames and ")
		TEXT("never exercise a dropped POSITIVE frame. Corruption fires when ((serial + phase) mod N) == 0."),
		ECVF_Default);

	static FCriticalSection RingCS;
	static TArray<FKeyEntry> Ring;
	static uint64 PublishSerial = 0;

	static FThreadSafeCounter Published;
	static FThreadSafeCounter Consumed;
	static FThreadSafeCounter Missed;
	static FThreadSafeCounter Wrapped;
	static FThreadSafeCounter Corrupted;
	static FThreadSafeCounter WantedMatches;

	int32 GetCapacity()
	{
		return GRingCapacity;
	}

	int32 GetForceMissMode()
	{
		return FMath::Max(0, CVarForceMiss.GetValueOnAnyThread());
	}

	int32 GetForceMissPhase()
	{
		return FMath::Max(0, CVarForceMissPhase.GetValueOnAnyThread());
	}

	bool IsForceMiss()
	{
		return GetForceMissMode() != 0;
	}

	static bool ShouldCorrupt(uint64 Serial, int32 Mode, int32 Phase)
	{
		if (Mode == 1)
		{
			return true;
		}
		if (Mode > 1)
		{
			return ((Serial + (uint64)Phase) % (uint64)Mode) == 0;
		}
		return false;
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
		WantedMatches.Reset();
	}

	FCounters GetCounters()
	{
		FCounters Out;
		Out.Published = Published.GetValue();
		Out.Consumed = Consumed.GetValue();
		Out.Missed = Missed.GetValue();
		Out.Wrapped = Wrapped.GetValue();
		Out.Corrupted = Corrupted.GetValue();
		Out.WantedMatches = WantedMatches.GetValue();
		return Out;
	}

	void PublishKey(uint32 FamilyFrameNumber, uint64 RequestId, bool bWanted)
	{
		const int32 Mode = GetForceMissMode();
		const int32 Phase = GetForceMissPhase();

		FKeyEntry Entry;
		Entry.RequestId = RequestId;
		Entry.bWanted = bWanted;
		Entry.bValid = true;

		FScopeLock Lock(&RingCS);
		Entry.Serial = ++PublishSerial;

		const bool bCorrupt = ShouldCorrupt(Entry.Serial, Mode, Phase);
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
		if (bWanted)
		{
			WantedMatches.Increment();
		}
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
		const uint64 BaseRequest = 900000ull;

		for (int32 i = 0; i < Count; ++i)
		{
			PublishKey(BaseFamily + (uint32)i, BaseRequest + (uint64)i, (i % 2) == 0);
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
				if (Entry.RequestId != BaseRequest + (uint64)i)
				{
					++KeyMismatches;
				}
			}
		}

		const FCounters C = GetCounters();
		const int32 Mode = GetForceMissMode();
		const int32 Phase = GetForceMissPhase();
		const int32 FirstSurviving = FMath::Max(0, Count - GRingCapacity);

		int32 ExpectedHits = 0;
		for (int32 i = FirstSurviving; i < Count; ++i)
		{
			if (!ShouldCorrupt((uint64)i + 1, Mode, Phase))
			{
				++ExpectedHits;
			}
		}

		UE_LOG(LogAnomalyCapture, Display,
			TEXT("SVE ring self-test: n=%d capacity=%d forceMiss=%d phase=%d | published=%d corrupted=%d hits=%d expectedHits=%d misses=%d wrapped=%d keyMismatches=%d wantedHits=%d | %s"),
			Count, GRingCapacity, Mode, Phase,
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
