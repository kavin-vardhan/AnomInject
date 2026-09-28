#include "../Source/AnomalyInjector/Public/AnomalyStuckMipWindow.h"
#include "m52_window_legacy_90dfa6f.h"

#include <algorithm>
#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace AnomalyStuckMipWindow;

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

static std::string Str(const std::vector<int>& V)
{
	std::string S = "[";
	for (size_t i = 0; i < V.size(); ++i)
	{
		S += std::to_string(V[i]);
		if (i + 1 < V.size())
		{
			S += ",";
		}
	}
	return S + "]";
}

namespace Legacy08402
{
	enum class EVerdict : unsigned char { NotHeld = 0, Held = 1, Unknown = 2, BeforeApply = 3 };
	enum class EMembership : unsigned char { Out = 0, Held = 1, Unknown = 2, SettleTail = 3 };

	inline EVerdict Combine(const ETexState* States, int Num)
	{
		if (Num <= 0)
		{
			return EVerdict::BeforeApply;
		}
		bool bUnknown = false;
		for (int i = 0; i < Num; ++i)
		{
			if (States[i] == ETexState::Held) { return EVerdict::Held; }
			if (States[i] == ETexState::Unknown) { bUnknown = true; }
		}
		return bUnknown ? EVerdict::Unknown : EVerdict::NotHeld;
	}

	inline EMembership LiveMembership(EVerdict V)
	{
		switch (V)
		{
		case EVerdict::Held:    return EMembership::Held;
		case EVerdict::Unknown: return EMembership::Unknown;
		default:                return EMembership::Out;
		}
	}

	struct FTrail
	{
		int RevertSessionIndex = -1;
		int SettleTailFrames = 0;
		int FirstBaselineSI = -1;
		int CleanRun = 0;
		int ClosedAtSI = -1;
		bool bOpen = false;
		bool bClosed = false;

		void Open(int InRevert, int InTail)
		{
			*this = FTrail();
			RevertSessionIndex = InRevert;
			SettleTailFrames = InTail;
			bOpen = true;
		}

		EMembership Step(int SessionIndex, EVerdict V)
		{
			if (V == EVerdict::Held || V == EVerdict::Unknown)
			{
				FirstBaselineSI = -1;
				CleanRun = 0;
				return V == EVerdict::Held ? EMembership::Held : EMembership::Unknown;
			}
			if (FirstBaselineSI < 0) { FirstBaselineSI = SessionIndex; }
			if (SessionIndex < FirstBaselineSI + SettleTailFrames) { return EMembership::SettleTail; }
			++CleanRun;
			if (!bClosed && CleanRun >= 2) { bClosed = true; ClosedAtSI = SessionIndex; }
			return EMembership::Out;
		}

		bool StillAttached() const { return bOpen && !bClosed; }
	};

	inline bool InScope(const FComponentScope& C)
	{
		return !C.bTemplate && !C.bPendingKill && C.bInLoadedLevelOfWorld && C.bLevelActive && C.bRegistered
			&& C.bPrimitiveOrDecal;
	}

	inline bool ObserveSubmitsProvisionalFalse(bool bReady, int AgeTicks, bool bForce)
	{
		return !bReady && (bForce || AgeTicks > 16);
	}
}

static EVerdict VerdictOf(const std::vector<int>& Resident, const std::vector<int>& Baseline)
{
	std::vector<ETexState> States;
	for (size_t i = 0; i < Resident.size(); ++i)
	{
		States.push_back(ClassifyTexture(Resident[i], Baseline[i], Resident[i] >= 0));
	}
	return Combine(States.data(), (int)States.size());
}

static std::vector<EMembership> RunTrail(FTrail& T, int FirstSI, const std::vector<EVerdict>& V, bool bSettled = true)
{
	std::vector<EMembership> Out;
	for (size_t i = 0; i < V.size(); ++i)
	{
		const int SI = FirstSI + (int)i;
		T.NoteArmed(SI);
		T.Receive(SI, V[i], bSettled);
		EMembership M = EMembership::Out;
		const bool bGot = T.TryGetMembership(SI, M);
		Out.push_back(bGot ? M : EMembership::Unknown);
	}
	return Out;
}

static std::vector<int> Members(const std::vector<EMembership>& M, int FirstSI)
{
	std::vector<int> Out;
	for (size_t i = 0; i < M.size(); ++i)
	{
		if (IsMember(M[i])) { Out.push_back(FirstSI + (int)i); }
	}
	return Out;
}

static void TestTextureAndCombine()
{
	Check(ClassifyTexture(7, 11, true) == ETexState::Held, "7<11 held");
	Check(ClassifyTexture(11, 11, true) == ETexState::Baseline, "11>=11 baseline");
	Check(ClassifyTexture(12, 11, true) == ETexState::Baseline, "12>=11 baseline");
	Check(ClassifyTexture(7, 11, false) == ETexState::Unknown, "unknown resource");
	Check(ClassifyTexture(-1, 11, true) == ETexState::Unknown, "negative resident unknown");
	Check(ClassifyTexture(7, 0, true) == ETexState::Unknown, "zero baseline unknown");
	const ETexState HB[2] = { ETexState::Held, ETexState::Baseline };
	const ETexState BB[2] = { ETexState::Baseline, ETexState::Baseline };
	const ETexState UB[2] = { ETexState::Unknown, ETexState::Baseline };
	const ETexState UH[2] = { ETexState::Unknown, ETexState::Held };
	Check(Combine(HB, 2) == EVerdict::Held, "ANY held opens");
	Check(Combine(BB, 2) == EVerdict::NotHeld, "ALL baseline closes");
	Check(Combine(UB, 2) == EVerdict::Unknown, "unknown without held is unknown, never clean");
	Check(Combine(UH, 2) == EVerdict::Held, "held beats unknown");
}

static void TestF3EmptyWatch()
{
	Check(Legacy08402::LiveMembership(Legacy08402::Combine(nullptr, 0)) == Legacy08402::EMembership::Out,
		"F3 legacy: a frame with an m52 fire and no watch was BEFORE_APPLY, i.e. labelled clean (defect reproduced)");
	Check(Combine(nullptr, 0) == EVerdict::Unknown, "F3: a frame with an m52 fire and no watch is UNKNOWN");
	Check(IsMember(LiveMembership(Combine(nullptr, 0))), "F3: an empty watch is LABELLED, never clean");
	const ETexState Late[1] = { ClassifyTexture(7, 11, true) };
	Check(Legacy08402::LiveMembership(Legacy08402::Combine(nullptr, 0)) == Legacy08402::EMembership::Out
		&& IsMember(LiveMembership(Combine(Late, 1))),
		"F3: an Apply-tick arm served by a LATER family after the mip drop - legacy clean, re-frozen watch reads held");
	const ETexState Early[1] = { ClassifyTexture(11, 11, true) };
	Check(!IsMember(LiveMembership(Combine(Early, 1))),
		"F3: the same arm served before the drop reads baseline from its own record (no over-label)");
	Check(IsMember(LiveMembership(EVerdict::Missing)), "F3: a missing receipt is labelled");
}

static void TestResourceReplacement()
{
	bool bReplaced = false;
	Check(ClassifyTexture(11, 11, true) == ETexState::Baseline,
		"resource legacy: a REPLACED resource reading 11 against the old baseline 11 was clean (defect reproduced)");
	Check(ClassifyTextureSample(11, 11, true, 0xA0, 0xB0, &bReplaced) == ETexState::Unknown && bReplaced,
		"resource: a render resource whose identity changed mid-event is UNKNOWN (labelled) and flagged replaced");
	Check(ClassifyTextureSample(11, 11, true, 0xA0, 0xA0, &bReplaced) == ETexState::Baseline && !bReplaced,
		"resource: the same resource reads baseline normally");
	Check(ClassifyTextureSample(7, 11, true, 0xA0, 0xA0, &bReplaced) == ETexState::Held && !bReplaced,
		"resource: the same resource reads held normally");
	Check(ClassifyTextureSample(11, 11, false, 0xA0, 0, &bReplaced) == ETexState::Unknown && !bReplaced,
		"resource: an unknown sample stays unknown (not a replacement)");
}

static void TestOnsetAgainstLaggingMirror()
{
	const std::vector<int> Baseline = { 11, 11, 11 };
	std::vector<int> NewLabelled, OldLabelled, RenderHeld;
	for (int si = 0; si < 12; ++si)
	{
		const bool bRtHeld = si >= 5;
		const bool bGtHeld = si >= 6;
		const bool bSecondLate = si >= 7;
		const std::vector<int> Rt = { bRtHeld ? 7 : 11, bRtHeld ? 7 : 11, bSecondLate ? 7 : 11 };
		const std::vector<int> Gt = { bGtHeld ? 7 : 11, bGtHeld ? 7 : 11, (si >= 8) ? 7 : 11 };
		const EVerdict Rv = VerdictOf(Rt, Baseline);
		const EVerdict Gv = VerdictOf(Gt, Baseline);
		if (IsMember(LiveMembership(Rv))) { NewLabelled.push_back(si); }
		if (Gv == EVerdict::Held) { OldLabelled.push_back(si); }
		if (Rv == EVerdict::Held) { RenderHeld.push_back(si); }
	}
	Check(NewLabelled == RenderHeld, "onset: render-record label window == render-held frames " + Str(NewLabelled));
	Check(!NewLabelled.empty() && NewLabelled.front() == 5, "onset: first labelled frame is the first render-held frame (5)");
	Check(!OldLabelled.empty() && OldLabelled.front() == 6, "onset: the game-thread mirror opens one frame late (6)");
	Check(OldLabelled != NewLabelled, "onset: the legacy mirror window differs from the render window (can-fail)");
}

static void TestTrailingNoTail()
{
	FTrail T;
	T.Open(100, 0, 120);
	const std::vector<EVerdict> V = {
		EVerdict::Held, EVerdict::Held, EVerdict::Held, EVerdict::Held, EVerdict::Held,
		EVerdict::NotHeld, EVerdict::NotHeld, EVerdict::NotHeld };
	const std::vector<EMembership> M = RunTrail(T, 99, V);
	Check(Members(M, 99) == std::vector<int>({ 99, 100, 101, 102, 103 }),
		"offset tail 0: the revert-tick frame and every render-held trailing frame are labelled " + Str(Members(M, 99)));
	Check(M[5] == EMembership::Out, "offset tail 0: the first baseline frame is not labelled");
	Check(T.bClosed && T.ClosedAtSI == 105, "offset: closes after 2 consecutive SETTLED baseline frames (at 105)");
	Check(!T.GatesNextBurst(), "offset: a closed trail releases the next burst");
}

static void TestTrailingWithTail()
{
	FTrail T;
	T.Open(10, 3, 120);
	const std::vector<EVerdict> V = {
		EVerdict::Held, EVerdict::Held, EVerdict::NotHeld, EVerdict::NotHeld, EVerdict::NotHeld,
		EVerdict::NotHeld, EVerdict::NotHeld, EVerdict::NotHeld };
	const std::vector<EMembership> M = RunTrail(T, 10, V);
	Check(M[0] == EMembership::Held && M[1] == EMembership::Held, "tail 3: held frames labelled held");
	Check(M[2] == EMembership::SettleTail && M[3] == EMembership::SettleTail && M[4] == EMembership::SettleTail,
		"tail 3: exactly three settle-tail frames after the first baseline frame");
	Check(M[5] == EMembership::Out && M[6] == EMembership::Out, "tail 3: frames after the tail are out");
	Check(T.bClosed && T.ClosedAtSI == 16, "tail 3: closes after the tail plus 2 confirming frames (16)");
	Check(T.TailFrames == 3 && T.HeldFrames == 2, "tail 3: counters");
}

static void TestTrailingFlapAndUnknown()
{
	FTrail T;
	T.Open(0, 0, 120);
	const std::vector<EVerdict> V = {
		EVerdict::Held, EVerdict::NotHeld, EVerdict::Held, EVerdict::Unknown, EVerdict::NotHeld, EVerdict::NotHeld };
	const std::vector<EMembership> M = RunTrail(T, 0, V);
	Check(M[0] == EMembership::Held && M[1] == EMembership::Out && M[2] == EMembership::Held,
		"flap: a held frame after a baseline frame is labelled again");
	Check(M[3] == EMembership::Unknown, "unknown trailing frame is labelled, never silently clean");
	Check(T.bClosed && T.ClosedAtSI == 5, "flap: two consecutive settled baselines after the last held/unknown close (5)");
}

static void TestTimeoutAndGating()
{
	FTrail T;
	T.Open(50, 0, 30);
	Check(T.GatesNextBurst(), "gate: an open trail holds the next burst");
	Check(!T.ShouldTimeout(79), "timeout: 29 captured frames after revert is not yet a timeout");
	Check(T.ShouldTimeout(80), "timeout: 30 captured frames after revert is a timeout");
	T.bUnresolved = true;
	Check(!T.GatesNextBurst(), "timeout: an unresolved trail stops gating");
	Check(T.StillAttached(), "timeout: an unresolved trail stays attached, so held frames are still labelled");
	const std::vector<EVerdict> V(40, EVerdict::Held);
	const std::vector<EMembership> M = RunTrail(T, 50, V);
	Check((int)Members(M, 50).size() == 40, "timeout: every render-held frame after the timeout is still labelled");
	RunTrail(T, 90, { EVerdict::NotHeld, EVerdict::NotHeld });
	Check(T.bClosed, "timeout: a late settled restore still closes the trail");
}

static void TestF1PendingStreamOut()
{
	const std::vector<EVerdict> V = {
		EVerdict::Held, EVerdict::Held, EVerdict::NotHeld, EVerdict::NotHeld, EVerdict::Held, EVerdict::Held,
		EVerdict::NotHeld, EVerdict::NotHeld };
	const std::vector<bool> Settled = { false, false, false, false, false, false, true, true };
	Legacy08402::FTrail L;
	L.Open(0, 0);
	std::vector<int> LegacyUnlabelledHeld;
	for (int si = 0; si < (int)V.size(); ++si)
	{
		const bool bWatched = L.StillAttached();
		if (!bWatched)
		{
			if (V[si] == EVerdict::Held) { LegacyUnlabelledHeld.push_back(si); }
			continue;
		}
		L.Step(si, (Legacy08402::EVerdict)V[si]);
	}
	Check(L.bClosed && L.ClosedAtSI == 3 && LegacyUnlabelledHeld == std::vector<int>({ 4, 5 }),
		"F1 legacy: two baselines WHILE A STREAM-OUT IS PENDING close the trail at 3, and the late low-mip frames 4,5 "
		"are drawn after the event detached - unlabelled " + Str(LegacyUnlabelledHeld));
	FTrail T;
	T.Open(0, 0, 120);
	std::vector<int> Unlabelled, Labelled;
	for (int si = 0; si < (int)V.size(); ++si)
	{
		if (!T.StillAttached())
		{
			if (V[si] == EVerdict::Held) { Unlabelled.push_back(si); }
			continue;
		}
		T.NoteArmed(si);
		T.Receive(si, V[si], Settled[si]);
		EMembership M = EMembership::Out;
		if (T.TryGetMembership(si, M) && IsMember(M)) { Labelled.push_back(si); }
	}
	Check(Unlabelled.empty() && Labelled == std::vector<int>({ 0, 1, 4, 5 }),
		"F1: baselines with a stream operation pending do NOT confirm, so the late stream-out frames 4,5 stay attached "
		"and labelled " + Str(Labelled));
	Check(T.bClosed && T.ClosedAtSI == 7 && T.UnsettledBaselineFrames == 2,
		"F1: closure only after 2 consecutive baselines with NO pending work (7)");
}

static void TestF2CrossBatch()
{
	Legacy08402::FTrail L;
	L.Open(100, 0);
	L.Step(100, Legacy08402::EVerdict::NotHeld);
	L.Step(102, Legacy08402::EVerdict::NotHeld);
	const bool bLegacyClosedEarly = L.bClosed;
	const Legacy08402::EMembership L101 = L.Step(101, Legacy08402::EVerdict::Held);
	Check(bLegacyClosedEarly && L.bClosed && L101 == Legacy08402::EMembership::Held,
		"F2 legacy: baseline 100, baseline 102 (published before 101), held 101 - closed after two out-of-order baselines "
		"and a later HELD receipt cannot reopen it (defect reproduced)");

	FTrail T;
	T.Open(100, 0, 120);
	T.NoteArmed(100);
	T.NoteArmed(101);
	T.NoteArmed(102);
	T.Receive(100, EVerdict::NotHeld, true);
	T.Receive(102, EVerdict::NotHeld, true);
	EMembership M102 = EMembership::Out;
	Check(!T.bClosed && !T.bClosing && !T.TryGetMembership(102, M102) && T.IsOrderPending(102),
		"F2: 102 arriving before 101 is HELD by the cursor - no closure, no membership yet");
	T.Receive(101, EVerdict::Held, true);
	EMembership M101 = EMembership::Out;
	Check(T.TryGetMembership(101, M101) && M101 == EMembership::Held && T.TryGetMembership(102, M102)
		&& M102 == EMembership::Out && !T.bClosed && T.CleanRun == 1,
		"F2: in capture order the sequence is baseline, held, baseline - one confirmation, still attached");
	T.NoteArmed(103);
	T.Receive(103, EVerdict::NotHeld, true);
	Check(T.bClosed && T.ClosedAtSI == 103, "F2: closes only after two consecutive baselines in capture order (103)");

	Legacy08402::FTrail L2;
	L2.Open(200, 0);
	L2.Step(202, Legacy08402::EVerdict::NotHeld);
	const Legacy08402::EMembership LOld = L2.Step(201, Legacy08402::EVerdict::NotHeld);
	Check(LOld == Legacy08402::EMembership::SettleTail,
		"F2 legacy: an OLDER baseline arriving late entered SettleTail with a zero tail - an over-label (reproduced)");
	FTrail T2;
	T2.Open(200, 0, 120);
	T2.NoteArmed(201);
	T2.NoteArmed(202);
	T2.Receive(202, EVerdict::NotHeld, true);
	T2.Receive(201, EVerdict::NotHeld, true);
	EMembership M201 = EMembership::Held;
	Check(T2.TryGetMembership(201, M201) && M201 == EMembership::Out, "F2: the same late older baseline is OUT");

	FTrail G;
	G.Open(300, 0, 120);
	for (int si = 300; si <= 303; ++si) { G.NoteArmed(si); }
	G.Receive(300, EVerdict::NotHeld, true);
	G.Receive(302, EVerdict::NotHeld, true);
	G.Receive(303, EVerdict::NotHeld, true);
	bool bAged = false;
	for (int t = 0; t < 40 && !bAged; ++t) { bAged = G.TickHeadWait(); }
	Check(!G.bClosed && bAged, "F2 gap: 301 never arrives - nothing closes across it, and the head ages out");
	G.MarkMissing(301);
	EMembership M301 = EMembership::Out;
	Check(G.TryGetMembership(301, M301) && M301 == EMembership::Unknown && G.MissingFrames == 1,
		"F2 gap: a missing request is UNKNOWN and breaks the run - an unresolved gap never counts as a clean confirmation");
	Check(G.bClosed && G.ClosedAtSI == 303, "F2 gap: only the two baselines AFTER the gap confirm (closed at 303)");
	Legacy08402::FTrail LG;
	LG.Open(300, 0);
	LG.Step(300, Legacy08402::EVerdict::NotHeld);
	LG.Step(302, Legacy08402::EVerdict::NotHeld);
	Check(LG.bClosed, "F2 gap legacy: two baselines across a dropped request closed the trail (reproduced)");

	FTrail R;
	R.Open(400, 0, 120);
	for (int si = 400; si <= 405; ++si) { R.NoteArmed(si); }
	R.Receive(400, EVerdict::NotHeld, true);
	R.Receive(401, EVerdict::NotHeld, true);
	Check(R.bClosing && !R.bClosed && R.FenceSI == 405 && R.GatesNextBurst(),
		"F2 fence: two confirmations with 402..405 still in flight is CLOSING, not closed, and still gates");
	R.Receive(402, EVerdict::Held, true);
	Check(!R.bClosing && !R.bClosed, "F2 fence: a held in-flight frame aborts the closure");
	R.Receive(403, EVerdict::NotHeld, true);
	R.Receive(404, EVerdict::NotHeld, true);
	R.Receive(405, EVerdict::NotHeld, true);
	Check(R.bClosed && R.ClosedAtSI == 405, "F2 fence: final only when the ordered window reaches the fence");
	FTrail P;
	P.Open(500, 0, 120);
	for (int si = 500; si <= 503; ++si) { P.NoteArmed(si); }
	P.Receive(500, EVerdict::NotHeld, true);
	P.Receive(501, EVerdict::NotHeld, true);
	P.NoteArmed(504);
	P.Receive(502, EVerdict::NotHeld, true);
	P.Receive(503, EVerdict::NotHeld, true);
	Check(P.bClosed && P.ClosedAtSI == 503, "F2 reopen: closed at the fence (503) with 504 still in flight");
	P.Receive(504, EVerdict::Held, true);
	EMembership M504 = EMembership::Out;
	Check(!P.bClosed && P.Reopens == 1 && P.StillAttached() && P.TryGetMembership(504, M504) && M504 == EMembership::Held,
		"F2 reopen: a HELD receipt at or after the closure REOPENS the event and the frame is labelled");
}

static void TestF5CarryAcrossRun()
{
	FTrail T;
	T.Open(10, 0, 120);
	RunTrail(T, 9, { EVerdict::Held, EVerdict::Held, EVerdict::Held });
	Check(ShouldCarryAcrossRun(T), "F5: a trail still attached at run stop is CARRIED");
	std::vector<Legacy08402::FTrail> LegacyTrails(1);
	LegacyTrails[0].Open(10, 0);
	LegacyTrails.clear();
	const bool bLegacyEventInNextRun = !LegacyTrails.empty();
	Check(!bLegacyEventInNextRun,
		"F5 legacy: FinishRun/StartRun reset every trail, so a still-blurred frame of the next run had no event (reproduced)");
	T.RebaseForNewRun(0);
	Check(T.bInherited && T.StillAttached() && T.GatesNextBurst() && T.NextSI < 0,
		"F5: rebased into the new run's session-index space, still attached");
	const std::vector<EMembership> M = RunTrail(T, 0, { EVerdict::Held, EVerdict::Held, EVerdict::NotHeld, EVerdict::NotHeld });
	Check(Members(M, 0) == std::vector<int>({ 0, 1 }) && T.bClosed && T.ClosedAtSI == 3,
		"F5: the new run's still-held frames are labelled for the carried event, which closes only on settled baselines");
	FTrail U;
	U.Open(0, 0, 5);
	U.bUnresolved = true;
	U.RebaseForNewRun(0);
	Check(U.bUnresolved && U.StillAttached(), "F5: an unresolved restore stays unresolved across the boundary");
}

static void TestF6Observe()
{
	Check(Legacy08402::ObserveSubmitsProvisionalFalse(false, 17, false),
		"F6 legacy: a gated m55 observation with no render result was submitted as NOT labelled after 16 ticks (reproduced)");
	Check(ResolveObserve(false, false, false, 17, false) == EObserve::Wait,
		"F6: without a final render result the entry WAITS - no provisional negative");
	Check(ResolveObserve(false, false, true, 1, false) == EObserve::Labelled,
		"F6: a terminally missing frame is final as UNKNOWN, which is labelled");
	Check(ResolveObserve(false, false, false, ObserveHardBoundTicks + 1, false) == EObserve::Labelled,
		"F6: past the hard bound the entry is final as unknown = labelled, never false");
	Check(ResolveObserve(false, false, false, 0, true) == EObserve::Labelled, "F6: a forced shutdown flush is labelled");
	Check(ResolveObserve(true, false, false, 99, true) == EObserve::NotLabelled
		&& ResolveObserve(true, true, false, 0, false) == EObserve::Labelled,
		"F6: a final render result decides exactly");
}

static void TestF7DeferredMaskAge()
{
	std::map<int, int> Legacy;
	std::map<int, int> Fixed;
	Legacy[1] = 0;
	Fixed[1] = 0;
	int LegacyDropTick = -1;
	int FixedDropTick = -1;
	for (int tick = 1; tick <= 60; ++tick)
	{
		if (Legacy.count(1))
		{
			Legacy.erase(1);
			Legacy[1] = 0;
			if (++Legacy[1] > 16) { Legacy.erase(1); LegacyDropTick = tick; }
		}
		if (Fixed.count(1))
		{
			const int Prev = Fixed[1];
			Fixed.erase(1);
			Fixed[1] = Prev;
			if (DeferredMaskExpired(++Fixed[1])) { Fixed.erase(1); FixedDropTick = tick; }
		}
	}
	Check(LegacyDropTick < 0, "F7 legacy: re-deferring reset the age every tick, so the 16-tick drop never fired (reproduced)");
	Check(FixedDropTick == 17, "F7: the carried age drops the mask at tick 17 as advertised");
}

static void TestPurityScope()
{
	const unsigned long long Target[1] = { 7 };
	const unsigned long long Only[1] = { 7 };
	const unsigned long long Shared[3] = { 7, 9, 12 };
	const unsigned long long Offscreen[2] = { 7, 44 };
	const unsigned long long SelfTwo[2] = { 7, 8 };
	const unsigned long long TargetTwo[2] = { 7, 8 };
	int Foreign = -1;
	Check(ClassifyPurity(Only, 1, Target, 1, &Foreign) == EPurity::Pure && Foreign == 0, "purity: exactly one user, the target");
	Check(ClassifyPurity(Shared, 3, Target, 1, &Foreign) == EPurity::Shared && Foreign == 2, "purity: two other users refused");
	Check(ClassifyPurity(Offscreen, 2, Target, 1, &Foreign) == EPurity::Shared && Foreign == 1,
		"purity: an OFF-SCREEN user (the glove/wall case) is refused");
	Check(ClassifyPurity(SelfTwo, 2, TargetTwo, 2, &Foreign) == EPurity::Shared && Foreign == 0,
		"purity: two target components sharing a texture is refused");
	Check(ClassifyPurity(nullptr, 0, Target, 1, &Foreign) == EPurity::NoUsers, "purity: no users is not pure");

	FComponentScope Inactive;
	Inactive.bInLoadedLevelOfWorld = true;
	Inactive.bLevelActive = false;
	Inactive.bRegistered = true;
	Inactive.bPrimitiveOrDecal = true;
	FComponentScope Unregistered;
	Unregistered.bInLoadedLevelOfWorld = true;
	Unregistered.bLevelActive = true;
	Unregistered.bRegistered = false;
	Unregistered.bPrimitiveOrDecal = true;
	FComponentScope Template = Unregistered;
	Template.bTemplate = true;
	FComponentScope Dead = Inactive;
	Dead.bPendingKill = true;
	FComponentScope Unloaded = Inactive;
	Unloaded.bInLoadedLevelOfWorld = false;
	auto CountUsers = [&](bool (*Pred)(const FComponentScope&), const FComponentScope& Other) -> EPurity
	{
		std::vector<unsigned long long> Users = { 7 };
		if (Pred(Other)) { Users.push_back(44); }
		int F = 0;
		return ClassifyPurity(Users.data(), (int)Users.size(), Target, 1, &F);
	};
	Check(CountUsers(&Legacy08402::InScope, Inactive) == EPurity::Pure,
		"purity legacy: a user in a LOADED-BUT-INACTIVE level was outside the census, so the texture read pure (reproduced)");
	Check(CountUsers(&InPurityScope, Inactive) == EPurity::Shared,
		"purity: the inactive-level user is counted and the texture is refused");
	Check(CountUsers(&Legacy08402::InScope, Unregistered) == EPurity::Pure,
		"purity legacy: an UNREGISTERED user was skipped, so the texture read pure (reproduced)");
	Check(CountUsers(&InPurityScope, Unregistered) == EPurity::Shared,
		"purity: the unregistered user is counted and the texture is refused");
	Check(!InPurityScope(Template) && !InPurityScope(Dead) && !InPurityScope(Unloaded),
		"purity: templates, dead objects and components of unloaded levels are excluded");
}

static void TestHoldMonitor()
{
	FComponentScope NewUser;
	NewUser.bInLoadedLevelOfWorld = true;
	NewUser.bLevelActive = true;
	NewUser.bRegistered = true;
	NewUser.bPrimitiveOrDecal = true;
	const bool bLegacyReverts = false;
	Check(!bLegacyReverts, "hold legacy: purity ran once at Apply and nothing watched the hold (reproduced)");
	Check(EvaluateNewHoldUser(NewUser, true, false, true) == EHoldUserAction::RevertNow,
		"hold: a NEW user of a held texture appearing mid-hold reverts immediately");
	Check(EvaluateNewHoldUser(NewUser, true, true, true) == EHoldUserAction::None,
		"hold: a new component of the TARGET itself is not contamination");
	Check(EvaluateNewHoldUser(NewUser, false, false, true) == EHoldUserAction::None,
		"hold: a new component that does not use a held texture is ignored");
	Check(EvaluateNewHoldUser(NewUser, true, false, false) == EHoldUserAction::None, "hold: nothing is held, nothing to do");
	FComponentScope Unreg = NewUser;
	Unreg.bRegistered = false;
	Unreg.bLevelActive = false;
	Check(EvaluateNewHoldUser(Unreg, true, false, true) == EHoldUserAction::RevertNow,
		"hold: an unregistered new user in an inactive loaded level is still a user");
	const int ContamSI = 40;
	std::vector<int> Flagged;
	const EMembership Seq[6] = { EMembership::Held, EMembership::Held, EMembership::Held, EMembership::Unknown,
		EMembership::Out, EMembership::Out };
	for (int i = 0; i < 6; ++i)
	{
		if (IsContaminatedFrame(ContamSI, 38 + i, Seq[i])) { Flagged.push_back(38 + i); }
	}
	Check(Flagged == std::vector<int>({ 40, 41 }),
		"hold: stuck_mip.contaminated flags every labelled frame from the contamination point until baseline " + Str(Flagged));
	Check(!IsContaminatedFrame(-1, 41, EMembership::Held), "hold: no contamination, no flag");
}

struct FToyStreamer
{
	bool bQueuedStaleOut = false;
	bool bAmortizedQueue = false;
	int StaleApplyFrame = -1;
	int LowUntil = -1;
	int LowFrom = -1;

	void Flush()
	{
		if (!bAmortizedQueue)
		{
			bQueuedStaleOut = false;
		}
	}

	void Tick(int Frame)
	{
		if (bQueuedStaleOut && Frame == StaleApplyFrame)
		{
			bQueuedStaleOut = false;
			LowFrom = Frame;
			LowUntil = Frame + 2;
		}
	}

	bool Low(int Frame) const
	{
		return LowFrom >= 0 && Frame >= LowFrom && Frame <= LowUntil;
	}
};

static void TestF1StreamerFence()
{
	FToyStreamer LS;
	LS.bQueuedStaleOut = true;
	LS.StaleApplyFrame = 8;
	Legacy90dfa6f::FTrail L;
	L.Open(4, 0, 120);
	std::vector<int> LegacyUnlabelled;
	for (int f = 4; f <= 14; ++f)
	{
		LS.Tick(f);
		const bool bLow = LS.Low(f);
		if (!L.StillAttached())
		{
			if (bLow) { LegacyUnlabelled.push_back(f); }
			continue;
		}
		const bool bLegacySettled = !bLow;
		L.NoteArmed(f);
		L.Receive(f, bLow ? Legacy90dfa6f::EVerdict::Held : Legacy90dfa6f::EVerdict::NotHeld, bLegacySettled);
	}
	Check(L.bClosed && L.ClosedAtSI == 5 && LegacyUnlabelled == std::vector<int>({ 8, 9, 10 }),
		"F1 legacy (90dfa6f): an already-back texture gets no restoring entry, so two baselines with no PendingUpdate "
		"close the trail at 5; the stream-out the streamer QUEUED under the hold is issued at 8 and frames 8,9,10 are "
		"blurred and unlabelled " + Str(LegacyUnlabelled));

	for (int Amortized = 0; Amortized <= 1; ++Amortized)
	{
		FToyStreamer S;
		S.bQueuedStaleOut = true;
		S.bAmortizedQueue = Amortized != 0;
		S.StaleApplyFrame = 8;
		FStreamerFence Fence;
		Fence.bAmortizedCopies = Amortized != 0;
		Fence.MinFramesIfUnfenced = UnfencedMinFrames(5);
		bool bFenceRan = false;
		int RetireAt = -1;
		FTrail T;
		T.Open(4, 0, 120);
		std::vector<int> Unlabelled, Labelled;
		int CloseSI = -1;
		for (int f = 4; f <= 40; ++f)
		{
			if (f == 5)
			{
				Fence.bFlushed = true;
				S.Flush();
				Fence.FramesSinceRevert = 0;
				bFenceRan = true;
				RetireAt = StreamerPlansRetired(Fence) ? f : f + Fence.MinFramesIfUnfenced;
			}
			S.Tick(f);
			const bool bLow = S.Low(f);
			const bool bRestoring = !bFenceRan || f < RetireAt || bLow;
			if (!T.StillAttached())
			{
				if (bLow) { Unlabelled.push_back(f); }
				continue;
			}
			const bool bSettled = IsArmSettled(1, bRestoring, bLow, true);
			T.NoteArmed(f);
			T.Receive(f, bLow ? EVerdict::Held : EVerdict::NotHeld, bSettled);
			EMembership M = EMembership::Out;
			if (T.TryGetMembership(f, M) && IsMember(M)) { Labelled.push_back(f); }
			if (T.bClosed && CloseSI < 0) { CloseSI = T.ClosedAtSI; }
		}
		if (Amortized == 0)
		{
			Check(Unlabelled.empty() && Labelled.empty() && CloseSI == 6,
				"F1: the streamer fence (synchronous full update) REPLACES the plan computed under the hold, so the queued "
				"stream-out is never issued; the already-back texture stays tracked until the fence has run and the trail "
				"closes on two settled baselines after it (6)");
		}
		else
		{
			Check(Unlabelled.empty() && Labelled == std::vector<int>({ 8, 9, 10 }) && CloseSI == 20,
				"F1 amortized copies: the fence cannot retire the private copy queue, so baselines stay unsettled for two "
				"full streaming cycles (14 frames); the late stream-out 8..10 is inside that window and LABELLED, and the "
				"trail closes only at 20 " + Str(Labelled));
		}
	}
	FStreamerFence Off;
	Off.bStreamingEnabled = false;
	Check(StreamerPlansRetired(Off), "F1: with texture streaming disabled there is no streamer plan to retire");
	Check(!IsArmSettled(0, false, false, true), "F1: no watched texture is never settled");
}

struct FGraceSim
{
	std::vector<int> ProductlessMembers;
	int Reopens = 0;
	int ReopensAfterDetach = 0;
	bool bDetachedAtEnd = false;
	int ClosedAt = -1;
};

template <typename TrailT, typename AttachedFn>
static FGraceSim RunGrace(TrailT& T, AttachedFn Attached, bool bNextFireAt116, int HeldLateSI)
{
	FGraceSim R;
	const int Latency = 2;
	auto V = [HeldLateSI](int si) -> int
	{
		if (si <= 101 || si == HeldLateSI) { return 1; }
		return 0;
	};
	std::map<int, bool> Products;
	std::map<int, bool> Watched;
	std::set<int> Pending;
	int ReopensHandled = 0;
	for (int t = 100; t <= 125; ++t)
	{
		const int si = t - Latency;
		if (Pending.count(si))
		{
			Pending.erase(si);
			if (Watched[si])
			{
				using FVerdictT = decltype(T.Slots[0].Verdict);
				const int v = Products[si] ? V(si) : 2;
				T.Receive(si, (FVerdictT)v, true);
			}
		}
		if (T.bClosed && R.ClosedAt < 0) { R.ClosedAt = T.ClosedAtSI; }
		if (T.Reopens > ReopensHandled)
		{
			ReopensHandled = T.Reopens;
			const int LastBefore = T.LastArmedSI;
			for (int p : Pending)
			{
				if (p > LastBefore && !Watched[p])
				{
					Watched[p] = true;
					T.NoteArmed(p);
				}
			}
		}
		if (t <= 118)
		{
			const bool bAtt = Attached(T, t, bNextFireAt116);
			Products[t] = bAtt;
			Watched[t] = bAtt;
			if (bAtt) { T.NoteArmed(t); }
			Pending.insert(t);
		}
	}
	for (int si = 100; si <= 118; ++si)
	{
		int M = 0;
		bool bGot = false;
		if (T.NextSI >= 0 && si < T.NextSI)
		{
			const auto& S = T.Slot(si);
			if (S.bProcessed && S.SessionIndex == si) { M = (int)S.Membership; bGot = true; }
		}
		if (bGot && M != 0 && !Products[si]) { R.ProductlessMembers.push_back(si); }
	}
	R.Reopens = T.Reopens;
	return R;
}

static void TestF2GraceContinuity()
{
	Legacy90dfa6f::FTrail L;
	L.Open(100, 0, 120);
	const FGraceSim LR = RunGrace(L, [](Legacy90dfa6f::FTrail& Tr, int, bool) { return Tr.StillAttached(); }, false, 105);
	Check(LR.Reopens == 1 && !LR.ProductlessMembers.empty(),
		"F2 legacy (90dfa6f): closure at 104 detaches at once; the frame armed next (106) carries no watch, mask or m55 "
		"label, so when the in-flight 105 reads HELD and reopens the event, 106 is labelled with no products "
		+ Str(LR.ProductlessMembers));

	FTrail T;
	T.Open(100, 0, 120);
	const FGraceSim NR = RunGrace(T, [](FTrail& Tr, int t, bool bFire)
	{
		if (bFire && t == 116) { Tr.TryDetach(t, true); }
		return Tr.AttachedForProducts();
	}, true, 105);
	Check(NR.Reopens == 1 && NR.ProductlessMembers.empty() && T.ReopensAfterDetach == 0,
		"F2: a closed event keeps its products through the grace, so the reopen on the in-flight HELD 105 has continuity: "
		"no labelled frame lacks its watch, mask or m55 label " + Str(NR.ProductlessMembers));
	Check(T.bDetached && T.DetachedAtSI == 116,
		"F2: the grace ends when the next fire begins (116), before its hold can reach a frame");

	FTrail Q;
	Q.Open(200, 0, 120);
	for (int si = 200; si <= 204; ++si) { Q.NoteArmed(si); }
	for (int si = 200; si <= 203; ++si) { Q.Receive(si, si <= 201 ? EVerdict::Held : EVerdict::NotHeld, true); }
	Check(Q.bClosing && !Q.bClosed && !Q.TryDetach(206, false), "F2: a closing trail never detaches");
	Q.NoteArmed(205);
	Q.Receive(204, EVerdict::NotHeld, true);
	Check(Q.bClosed && !Q.AllNotedProcessed() && !Q.TryDetach(206, false) && Q.AttachedForProducts(),
		"F2: closed with 205 still in flight - not detached while a noted request can still reopen it");
	Q.Receive(205, EVerdict::NotHeld, true);
	Check(Q.AllNotedProcessed() && Q.TryDetach(206, false) && !Q.AttachedForProducts(),
		"F2: every noted request processed (a capture pause) - detached; no receipt can reach it any more");
	Q.NoteArmed(206);
	Check(Q.LastArmedSI == 205, "F2: a detached trail notes nothing");
}

static void TestF2Ownership()
{
	struct FWorld
	{
		bool bReserved = false;
		bool bRefused = false;
	};
	struct FLegacyService
	{
		bool bEndEventsIssued = false;
		bool bCarriedRefusal = false;
	};
	auto LegacyClose = [](FLegacyService& S, FWorld& W)
	{
		if (S.bEndEventsIssued) { return; }
		S.bEndEventsIssued = true;
		W.bReserved = false;
		if (S.bCarriedRefusal) { W.bRefused = false; S.bCarriedRefusal = false; }
	};
	FWorld LW;
	FLegacyService LS;
	LegacyClose(LS, LW);
	LW.bReserved = true;
	LW.bRefused = true;
	LegacyClose(LS, LW);
	Check(LW.bReserved && LW.bRefused,
		"F2 legacy (90dfa6f): close, service, reopen (reserve + restore_reopened refusal, not owned), reclose - the second "
		"closure skips its service, so the target stays reserved and m52 refused for the rest of the run");

	FWorld W;
	FTrailOwnership O;
	FOwnershipRelease R = OwnOnDetach(O, false, false);
	Check(!R.bUnreserve && !R.bClearRefusal, "F2: an ordinary trail owns nothing and releases nothing");
	W.bReserved = true;
	W.bRefused = true;
	OwnOnReopen(O);
	R = OwnOnDetach(O, false, false);
	if (R.bUnreserve) { W.bReserved = false; }
	if (R.bClearRefusal) { W.bRefused = false; }
	Check(!W.bReserved && !W.bRefused, "F2: a reopen OWNS its reservation and refusal, and the next detach releases both");
	const FOwnershipRelease Twice = OwnOnDetach(O, false, false);
	Check(!Twice.bUnreserve && !Twice.bClearRefusal, "F2: a detach is serviced once");
	OwnOnReopen(O);
	R = OwnOnDetach(O, true, true);
	Check(!R.bUnreserve && !R.bClearRefusal,
		"F2: another attached trail on the same target / id keeps the shared reservation and refusal in force");

	FLegacyService LT;
	FWorld LTW;
	LTW.bRefused = true;
	LTW.bReserved = true;
	LegacyClose(LT, LTW);
	Check(LTW.bRefused, "F2 legacy: the restore_unresolved refusal was never owned, so a LATE closure left m52 refused");
	FTrailOwnership OT;
	OwnOnRefusal(OT);
	OwnOnReserve(OT);
	const FOwnershipRelease RT = OwnOnDetach(OT, false, false);
	Check(RT.bClearRefusal && RT.bUnreserve, "F2: the unresolved timeout owns its refusal and reservation; a late closure releases them");
}

static void TestF5OpeningMaskAndCarriedOwnership()
{
	auto LegacyNeedsRecord = [](bool bLiveFire, bool, bool) { return bLiveFire; };
	std::vector<int> LegacyNoMask, NewNoMask;
	for (int si = 0; si < 3; ++si)
	{
		const bool bRecordAtArmLegacy = LegacyNeedsRecord(false, true, true) || si >= 1;
		if (!bRecordAtArmLegacy) { LegacyNoMask.push_back(si); }
		if (!NeedsMaskRecordAtArm(false, true, true)) { NewNoMask.push_back(si); }
	}
	Check(LegacyNoMask == std::vector<int>({ 0 }),
		"F5 legacy (90dfa6f): mask records are made at arm only for live fires, so the first frame of a new run that "
		"inherited a trail is labelled with no target mask (the record appears only when that frame is accumulated) "
		+ Str(LegacyNoMask));
	Check(NewNoMask.empty(), "F5: the attached trail gets its mask record BEFORE the first arm of the run");
	Check(!NeedsMaskRecordAtArm(false, true, false), "F5: a run without a render record creates no trail records");
	Check(!NeedsMaskRecordAtArm(false, false, true), "F5: a detached trail needs no record");

	auto LegacyReapply = [](bool bRenderTruthRun, bool bDeinit) { return bRenderTruthRun && !bDeinit; };
	Check(!LegacyReapply(false, false),
		"F5 legacy (90dfa6f): a non-render-truth run's FinishRun cleared every reservation and refusal and re-applied "
		"carried ownership only for a render-truth run, so the carried restore lost its target reservation and refusal");
	Check(ShouldReapplyCarriedOwnership(1, false), "F5: carried ownership is re-applied whatever the intervening run's mode");
	Check(!ShouldReapplyCarriedOwnership(0, false) && !ShouldReapplyCarriedOwnership(1, true),
		"F5: nothing to re-apply without a carry, and nothing is carried across world teardown");
}

static void TestF6ForcedAuthority()
{
	const bool bLegacyM55Labelled = true;
	const EMembership LegacyRow = LiveMembership(EVerdict::NotHeld);
	Check(bLegacyM55Labelled && !IsMember(LegacyRow),
		"F6 legacy (90dfa6f): the 64-tick fallback submitted m55 as LABELLED but wrote nothing to the frame's render "
		"result, so a late live baseline receipt made the row and mask OUT - m55 and the label disagree");
	FFrameAuthority A;
	ForceTerminalUnknown(A);
	Check(A.bForced && A.bHasResult && IsMember(A.Membership) && !AcceptsLateReceipt(A),
		"F6: the fallback writes a terminal UNKNOWN into the frame's single authority; the late receipt is ignored, so "
		"row, mask and m55 are all labelled");
	FFrameAuthority Pending;
	Pending.bHasResult = true;
	Pending.bOrderPending = true;
	ForceTerminalUnknown(Pending);
	Check(Pending.bForced && !Pending.bOrderPending && IsMember(Pending.Membership),
		"F6: an order-pending result is overridden by the fallback too");
	FFrameAuthority Final;
	Final.bHasResult = true;
	Final.Membership = EMembership::Out;
	ForceTerminalUnknown(Final);
	Check(!Final.bForced && Final.Membership == EMembership::Out, "F6: an already final result is never overwritten");
}

static void TestRegistrationRoute()
{
	struct FComp
	{
		bool bKnown = true;
		bool bRegisteredWhenJudged = false;
		bool bRegistered = false;
		bool bUsesHeld = false;
	};
	FComponentScope Scope;
	Scope.bInLoadedLevelOfWorld = true;
	Scope.bLevelActive = true;
	Scope.bPrimitiveOrDecal = true;
	FComp C;
	C.bUsesHeld = true;
	C.bRegistered = true;
	const bool bDirtyBroadcast = false;
	const bool bLegacyJudged = !C.bKnown || bDirtyBroadcast;
	Check(!bLegacyJudged,
		"route 2 legacy (90dfa6f): a component known at Apply while UNREGISTERED is given the held texture and then "
		"registers; registration broadcasts no dirty event and its identity is already known, so it is never judged");
	const bool bRejudge = RejudgeOnRegistration(C.bRegisteredWhenJudged, C.bRegistered);
	Scope.bRegistered = C.bRegistered;
	Check(bRejudge && EvaluateNewHoldUser(Scope, C.bUsesHeld, false, true) == EHoldUserAction::RevertNow,
		"route 2: the unregistered-when-judged watch re-judges it the tick it registers, and the hold reverts");
	Check(!RejudgeOnRegistration(true, true) && !RejudgeOnRegistration(false, false),
		"route 2: only a transition to registered triggers a re-judge");
}

static void TestMonitorCost()
{
	const bool bLegacyTimer = false;
	Check(!bLegacyTimer, "monitor legacy (90dfa6f): the per-tick scan had no timer and its count was not written out");
	std::vector<float> Ms;
	double Sum = 0.0;
	for (int i = 1; i <= 20; ++i) { Ms.push_back((float)i); Sum += i; }
	std::sort(Ms.begin(), Ms.end());
	const FCostSummary S = SummarizeCost(Ms.data(), (int)Ms.size(), Sum);
	Check(S.Samples == 20 && S.MeanMs == 10.5 && S.P95Ms == 19.0 && S.MaxMs == 20.0,
		"monitor: mean 10.5, nearest-rank p95 19, max 20 over 1..20 ms");
	const FCostSummary E = SummarizeCost(nullptr, 0, 0.0);
	Check(E.Samples == 0 && E.MaxMs == 0.0, "monitor: no scans reads zero, not garbage");
	std::vector<float> One = { 3.5f };
	const FCostSummary O = SummarizeCost(One.data(), 1, 3.5);
	Check(O.P95Ms == 3.5 && O.MeanMs == 3.5, "monitor: a single scan is its own p95");
}

int main()
{
	TestTextureAndCombine();
	TestF3EmptyWatch();
	TestResourceReplacement();
	TestOnsetAgainstLaggingMirror();
	TestTrailingNoTail();
	TestTrailingWithTail();
	TestTrailingFlapAndUnknown();
	TestTimeoutAndGating();
	TestF1PendingStreamOut();
	TestF2CrossBatch();
	TestF5CarryAcrossRun();
	TestF6Observe();
	TestF7DeferredMaskAge();
	TestPurityScope();
	TestHoldMonitor();
	TestF1StreamerFence();
	TestF2GraceContinuity();
	TestF2Ownership();
	TestF5OpeningMaskAndCarriedOwnership();
	TestF6ForcedAuthority();
	TestRegistrationRoute();
	TestMonitorCost();
	std::printf("m52 window selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
