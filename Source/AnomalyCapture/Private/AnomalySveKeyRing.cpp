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
		TEXT("1 = corrupt every published key so every render-thread lookup misses. Proves the loud-miss guard fires on the production path."),
		ECVF_Default);

	static FCriticalSection RingCS;
	static TArray<FKeyEntry> Ring;
	static uint64 PublishSerial = 0;

	static FThreadSafeCounter Published;
	static FThreadSafeCounter Consumed;
	static FThreadSafeCounter Missed;
	static FThreadSafeCounter Wrapped;

	int32 GetCapacity()
	{
		return GRingCapacity;
	}

	bool IsForceMiss()
	{
		return CVarForceMiss.GetValueOnAnyThread() != 0;
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
	}

	FCounters GetCounters()
	{
		FCounters Out;
		Out.Published = Published.GetValue();
		Out.Consumed = Consumed.GetValue();
		Out.Missed = Missed.GetValue();
		Out.Wrapped = Wrapped.GetValue();
		return Out;
	}

	void PublishKey(uint32 FamilyFrameNumber, uint64 GameFrameCounter, bool bWanted)
	{
		FKeyEntry Entry;
		Entry.FamilyFrameNumber = IsForceMiss() ? (FamilyFrameNumber ^ GForceMissMask) : FamilyFrameNumber;
		Entry.GameFrameCounter = GameFrameCounter;
		Entry.bWanted = bWanted;
		Entry.bValid = true;

		FScopeLock Lock(&RingCS);
		Entry.Serial = ++PublishSerial;
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
		const int32 ExpectedHits = FMath::Min(Count, GRingCapacity);

		UE_LOG(LogAnomalyCapture, Display,
			TEXT("SVE ring self-test: n=%d capacity=%d forceMiss=%d | published=%d hits=%d expectedHits=%d misses=%d wrapped=%d keyMismatches=%d wantedHits=%d | %s"),
			Count, GRingCapacity, IsForceMiss() ? 1 : 0,
			C.Published, Hits, IsForceMiss() ? 0 : ExpectedHits, C.Missed, C.Wrapped, KeyMismatches, WantedHits,
			(KeyMismatches == 0 && Hits == (IsForceMiss() ? 0 : ExpectedHits)) ? TEXT("PASS") : TEXT("FAIL"));

		Reset();
	}

	static FAutoConsoleCommand GRingSelfTestCmd(
		TEXT("IAI.Capture.SVE.RingTest"),
		TEXT("Exercise the B' key ring headlessly: publish N keys, look them all up, report hits/misses/wraps. Optional arg N (default 8)."),
		FConsoleCommandWithArgsDelegate::CreateStatic(&RingSelfTest));
}

#endif
