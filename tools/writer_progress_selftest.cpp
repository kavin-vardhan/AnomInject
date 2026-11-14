#include "../Source/AnomalyCapture/Private/AnomalyWriterProgress.h"

#include <cstdio>
#include <set>
#include <string>
#include <vector>

using namespace AnomalyWriterProgress;

static int GFailures = 0;
static int GChecks = 0;

static void Check(bool bOk, const std::string& What)
{
	++GChecks;
	if (!bOk)
	{
		++GFailures;
		std::printf("FAIL %s\n", What.c_str());
	}
}

struct FModelWriter
{
	double LastCompletion = 0.0;
	bool bStalled = false;
	unsigned int Generation = 1;
	std::set<int> InFlight;
	std::vector<int> Failed;
	int Written = 0;
	int Late = 0;
	bool bDead = false;
	double CompletionPeriod = 1.0;

	double WaitForCapacity(double Start, double PerJobNeeded)
	{
		if (bStalled)
		{
			return 0.0;
		}
		double Now = Start;
		double Waited = 0.0;
		while (Waited < PerJobNeeded)
		{
			Now += 1.0;
			Waited += 1.0;
			if (!bDead && Now - LastCompletion >= CompletionPeriod)
			{
				LastCompletion = Now;
			}
			if (IsStalled(Now, Start, LastCompletion, false, StallWindowSeconds))
			{
				bStalled = true;
				break;
			}
		}
		return Now - Start;
	}

	void Complete(unsigned int JobGen, int Session, bool bOk)
	{
		if (!CountsForRun(JobGen, Generation))
		{
			++Late;
			return;
		}
		InFlight.erase(Session);
		if (bOk) { ++Written; } else { Failed.push_back(Session); }
	}

	void Reset()
	{
		++Generation;
		InFlight.clear();
		Failed.clear();
		Written = 0;
		Late = 0;
		bStalled = false;
	}
};

static void TestReference()
{
	Check(StallReference(10.0, 4.0, false) == 10.0, "the reference is the wait start when the last completion is older");
	Check(StallReference(10.0, 14.0, false) == 14.0, "the reference is the last completion when it is newer");
	Check(StallReference(10.0, 4.0, true) == 4.0, "once stalled the reference stays the last completion");
}

static void TestStoppedWriter()
{
	Check(!IsStalled(119.0, 0.0, -500.0, false, StallWindowSeconds), "a stopped writer is not stalled at 119 s");
	Check(!IsStalled(120.0, 0.0, -500.0, false, StallWindowSeconds), "a stopped writer is not stalled at exactly 120 s");
	Check(IsStalled(121.0, 0.0, -500.0, false, StallWindowSeconds), "a stopped writer is stalled after 120 s without a completion");
	Check(!IsStalled(200.0, 0.0, 100.0, false, StallWindowSeconds), "a completion at 100 s resets the window");
	Check(IsStalled(221.0, 0.0, 100.0, false, StallWindowSeconds), "120 s after the last completion the writer is stalled");
}

static void TestSlowButWorkingDisk()
{
	bool bEver = false;
	double Last = 0.0;
	for (double Now = 1.0; Now <= 3600.0; Now += 1.0)
	{
		if (static_cast<int>(Now) % 100 == 0) { Last = Now; }
		bEver = bEver || IsStalled(Now, 0.0, Last, false, StallWindowSeconds);
	}
	Check(!bEver, "a disk that completes a job every 100 s waits an hour and is never declared stalled");
}

static void TestFlushAfterStall()
{
	Check(IsStalled(1000.0, 1000.0, 0.0, true, StallWindowSeconds), "a flush after a declared stall gives up at once when nothing completed since");
	Check(!IsStalled(1000.0, 1000.0, 990.0, true, StallWindowSeconds), "a completion after the stall lets the flush wait again");
	Check(IsStalled(1111.0, 1000.0, 990.0, true, StallWindowSeconds), "and it gives up 120 s after that completion");
	Check(!IsStalled(1000.0, 1000.0, 0.0, false, StallWindowSeconds), "a flush that starts without a stall gets its own 120 s");
}

static void TestTotalFreezeIsBounded()
{
	FModelWriter W;
	W.bDead = true;
	double Total = 0.0;
	for (int Frame = 0; Frame < 80; ++Frame)
	{
		Total += W.WaitForCapacity(Total, 1.0e9);
	}
	Check(W.bStalled, "a dead writer is declared stalled");
	Check(Total > 120.0 && Total < 122.0, "a dead writer costs about 120 s of game-thread wait for a whole run, not 120 s per frame");
	const double OldRuleTotal = 80.0 * 120.0;
	Check(OldRuleTotal > 75.0 * Total, "the old per-frame cap would have frozen the run 9,600 s, about 80 times longer");

	FModelWriter Live;
	Live.CompletionPeriod = 30.0;
	double LiveTotal = 0.0;
	for (int Frame = 0; Frame < 4; ++Frame)
	{
		LiveTotal += Live.WaitForCapacity(LiveTotal, 400.0);
	}
	Check(!Live.bStalled, "a writer completing every 30 s is never stalled even when each wait lasts 400 s");
	Check(LiveTotal >= 1600.0, "and the capture waits for it");
}

static void TestGenerations()
{
	Check(CountsForRun(3, 3), "a job of the current run counts");
	Check(!CountsForRun(2, 3), "a job of an earlier run does not count");

	FModelWriter W;
	const unsigned int OldGen = W.Generation;
	W.InFlight.insert(5);
	W.Reset();
	const unsigned int NewGen = W.Generation;
	W.InFlight.insert(5);
	W.Complete(OldGen, 5, false);
	Check(W.InFlight.count(5) == 1, "a late job of the earlier run does not remove the new run's session 5 from in flight");
	Check(W.Failed.empty(), "a late failed job of the earlier run does not mark the new run's session 5 write_failed");
	Check(W.Written == 0, "a late job of the earlier run does not count as written");
	Check(W.Late == 1, "the late completion is counted as late");
	W.Complete(NewGen, 5, true);
	Check(W.InFlight.empty() && W.Written == 1, "the new run's own job completes normally");

	FModelWriter Mutant;
	Mutant.InFlight.insert(5);
	Mutant.Reset();
	Mutant.InFlight.insert(5);
	Mutant.InFlight.erase(5);
	Mutant.Failed.push_back(5);
	Check(!(Mutant.InFlight.count(5) == 1 && Mutant.Failed.empty()), "without the generation check the new run loses session 5 (the defect)");
}

int main()
{
	TestReference();
	TestStoppedWriter();
	TestSlowButWorkingDisk();
	TestFlushAfterStall();
	TestTotalFreezeIsBounded();
	TestGenerations();
	std::printf("writer_progress_selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
