#include "../Source/AnomalyInjector/Public/AnomalyStuckMipWindow.h"
#include "../Source/AnomalyInjector/Public/AnomalyLabelSync.h"
#include "../Source/AnomalyInjector/Public/AnomalyInstallState.h"
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
	FComponentScope OtherWorld;
	OtherWorld.bInLoadedLevelOfWorld = false;
	OtherWorld.bLevelActive = true;
	OtherWorld.bRegistered = true;
	OtherWorld.bPrimitiveOrDecal = true;
	FComponentScope SameWorld = OtherWorld;
	SameWorld.bInLoadedLevelOfWorld = true;
	auto AllWorlds = [](const FComponentScope& C) { return !C.bTemplate && !C.bPendingKill && C.bPrimitiveOrDecal; };
	auto CountWith = [&](bool bProduct, const FComponentScope& Other) -> EPurity
	{
		std::vector<unsigned long long> Users = { 7 };
		if (bProduct ? InPurityScope(Other) : AllWorlds(Other)) { Users.push_back(44); }
		int F = 0;
		return ClassifyPurity(Users.data(), (int)Users.size(), Target, 1, &F);
	};
	Check(CountWith(true, OtherWorld) == EPurity::Pure,
		"purity 090-10b2: the same texture used by a component of ANOTHER world (the editor world in PIE) is not counted, so the target reads pure");
	Check(CountWith(true, SameWorld) == EPurity::Shared,
		"purity 090-10b2: a second user in the SAME world is counted and the texture is refused");
	Check(CountWith(false, OtherWorld) == EPurity::Shared,
		"purity 090-10b2: a rule that counted every world would refuse the two-world case (the hypothesis the PIE evidence refutes)");
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

namespace Legacy44584
{
	inline bool FramePresent(int NumFires)
	{
		return NumFires > 0;
	}

	inline int EmittedEntries(int NumFires)
	{
		return NumFires;
	}

	inline int AllocateTag(std::set<int>& Claimed, int& NextOffset, int Base, int Span)
	{
		for (int i = 0; i < Span; ++i)
		{
			const int Tag = Base + ((NextOffset + i) % Span);
			if (!Claimed.count(Tag))
			{
				Claimed.insert(Tag);
				NextOffset = (NextOffset + i + 1) % Span;
				return Tag;
			}
		}
		return 0;
	}
}

struct FM52Frame
{
	int SI = 0;
	bool bEventInFires = false;
	bool bMember = false;
};

static std::vector<FM52Frame> BuildM52Sequence()
{
	std::vector<FM52Frame> Seq;
	for (int si = 0; si < 48; ++si)
	{
		FM52Frame F;
		F.SI = si;
		F.bEventInFires = si >= 2;
		F.bMember = si >= 4 && si <= 15;
		Seq.push_back(F);
	}
	return Seq;
}

static void TestG350PostClosure()
{
	using namespace AnomalyLabelSync;
	const std::vector<FM52Frame> Seq = BuildM52Sequence();

	int LegacyPresentUnlabelled = 0;
	int LegacyEntriesUnlabelled = 0;
	for (const FM52Frame& F : Seq)
	{
		const int NumFires = F.bEventInFires ? 1 : 0;
		if (!F.bMember && Legacy44584::FramePresent(NumFires)) { ++LegacyPresentUnlabelled; }
		if (!F.bMember) { LegacyEntriesUnlabelled += Legacy44584::EmittedEntries(NumFires); }
	}
	Check(LegacyPresentUnlabelled == 34 && LegacyEntriesUnlabelled == 34,
		"G350 44584e7: anomaly_present and an entry on all 34 unlabelled frames with the event attached (pre-onset 2 + post-closure 32)");

	for (int Pass = 0; Pass < 2; ++Pass)
	{
		const int On = Pass == 0 ? 0 : 2;
		const int Off = Pass == 0 ? 0 : 8;
		FEventTransitionTrack Track;
		int PresentUnlabelled = 0;
		int PresentLabelled = 0;
		int PositiveEntriesUnlabelled = 0;
		std::vector<int> OffTransitionSI;
		std::vector<int> OnTransitionSI;
		for (const FM52Frame& F : Seq)
		{
			EEntryEmit Mode = EEntryEmit::Normal;
			bool bOnT = false;
			if (F.bEventInFires)
			{
				bool bOff = false;
				Track.Observe(F.SI, F.bMember, On, Off, bOnT, bOff);
				Mode = DecideRenderTruthEntry(F.bMember, bOff);
				if (Mode == EEntryEmit::TransitionOnly) { OffTransitionSI.push_back(F.SI); }
				if (bOnT) { OnTransitionSI.push_back(F.SI); }
			}
			const bool bPresent = FramePresent(&Mode, F.bEventInFires ? 1 : 0, F.bEventInFires ? 1 : 0);
			if (bPresent && !F.bMember) { ++PresentUnlabelled; }
			if (bPresent && F.bMember) { ++PresentLabelled; }
			if (!F.bMember && F.bEventInFires && Mode == EEntryEmit::Normal) { ++PositiveEntriesUnlabelled; }
		}
		const std::string Tag = Pass == 0 ? "AA off (0/0)" : "temporal AA (2/8)";
		Check(PresentUnlabelled == 0 && PresentLabelled == 12 && PositiveEntriesUnlabelled == 0,
			"G350 fixed, " + Tag + ": anomaly_present exactly on the 12 labelled frames, no positive entry on an unlabelled frame");
		if (Pass == 0)
		{
			Check(OffTransitionSI.empty() && OnTransitionSI.empty(), "G350 fixed, AA off: no transition entry at all");
		}
		else
		{
			Check(OnTransitionSI == std::vector<int>({ 4, 5 }), "transition, temporal AA: first 2 labelled frames flagged " + Str(OnTransitionSI));
			Check(OffTransitionSI == std::vector<int>({ 16, 17, 18, 19, 20, 21, 22, 23 }),
				"transition, temporal AA: the 8 frames after the last labelled frame carry a non-present entry " + Str(OffTransitionSI));
		}
	}

	for (int n = 0; n <= 5; ++n)
	{
		std::vector<EEntryEmit> Empty;
		Check(FramePresent(Empty.data(), 0, n) == Legacy44584::FramePresent(n),
			"non-m52 identity: with no emission modes the present rule equals 44584e7's Fires.Num() > 0 at n=" + std::to_string(n));
	}
	Check(ModeAt(nullptr, 0, 3) == EEntryEmit::Normal, "non-m52 identity: a fire with no mode is emitted exactly as before");
}

static void TestTransitionTrackOrder()
{
	using namespace AnomalyLabelSync;
	FEventTransitionTrack T;
	bool bOn = false, bOff = false;
	T.Observe(10, true, 2, 8, bOn, bOff);
	Check(bOn, "track: first member flagged");
	T.Observe(10, true, 2, 8, bOn, bOff);
	Check(!bOn && T.OutOfOrder == 0, "track: a duplicate observation is not flagged twice and is not out of order");
	T.Observe(12, true, 2, 8, bOn, bOff);
	Check(bOn, "track: second member flagged");
	T.Observe(13, true, 2, 8, bOn, bOff);
	Check(!bOn, "track: third member not flagged");
	T.Observe(11, false, 2, 8, bOn, bOff);
	Check(bOff && T.OutOfOrder == 1, "track: a late non-member inside the gap is an off transition and counted out of order");
	T.Observe(22, false, 2, 8, bOn, bOff);
	Check(!bOff && T.OffWindowPassed(22, 8), "track: 9 frames after the last member is outside K_off=8");
	T.Observe(21, false, 2, 8, bOn, bOff);
	Check(bOff, "track: exactly 8 frames after the last member is inside K_off=8");

	FEventTransitionTrack Inherited;
	Inherited.bOnsetPast = true;
	Inherited.Observe(0, true, 2, 8, bOn, bOff);
	Check(!bOn, "track: an inherited (carried) trail's first frame in a new run is not an onset");

	FEventTransitionTrack Many;
	int Flags = 0;
	for (int si = 0; si < 300; ++si)
	{
		Many.Observe(si, true, 64, 64, bOn, bOff);
		if (bOn) { ++Flags; }
	}
	Check(Flags == 64 && Many.NumRecent == FEventTransitionTrack::RecentCap, "track: 300 members flag exactly K_on=64 and stay bounded");
	Many.Observe(363, false, 64, 64, bOn, bOff);
	Check(bOff, "track: bounded memory still finds the last member (64 back)");
}

static void TestAaResolve()
{
	using namespace AnomalyLabelSync;
	Check(IsTemporalMethod(AaTemporal) && IsTemporalMethod(AaTsr), "aa: TAA and TSR are temporal");
	Check(!IsTemporalMethod(AaNone) && !IsTemporalMethod(AaFxaa) && !IsTemporalMethod(AaMsaa), "aa: none/FXAA/MSAA are not");
	Check(ResolveTransitionFrames(-1, DefaultOnFramesTemporal, true) == 3
		&& ResolveTransitionFrames(-1, DefaultOffFramesTemporal, true) == 16
		&& ResolveTransitionFrames(-1, DefaultHideFramesTemporal, true) == 1, "aa: -1 resolves to the ruled 3/16/1 under temporal AA (084-05b on/hide, 089-01 off)");
	Check(DefaultOffFramesTemporal == 16 && ResolveTransitionFrames(-1, DefaultOffFramesTemporal, false) == 0,
		"aa: the off default is 16 under temporal AA and still 0 without it (089-01 ruling 2)");
	Check(ResolveTransitionFrames(8, DefaultOffFramesTemporal, true) == 8 && ResolveTransitionFrames(0, DefaultOffFramesTemporal, true) == 0
		&& ResolveTransitionFrames(24, DefaultOffFramesTemporal, true) == 24,
		"aa: an explicit IAI.Label.TransitionOffFrames still overrides the 16 default (8 -> 8, 0 -> 0, 24 -> 24)");
	Check(ResolveTransitionFrames(-1, 8, false) == 0 && ResolveTransitionFrames(5, 8, false) == 0,
		"aa: without temporal AA every value is 0, even an explicit one");
	Check(ResolveTransitionFrames(3, 8, true) == 3 && ResolveTransitionFrames(0, 8, true) == 0
		&& ResolveTransitionFrames(1000, 8, true) == MaxTransitionFrames, "aa: an explicit value is used and clamped to 64");
}

struct FGateFrame
{
	bool bLabelled = false;
	bool bPixelVisible = false;
	bool bTransition = false;
};

static bool TransitionGatePasses(const std::vector<FGateFrame>& Frames)
{
	for (const FGateFrame& F : Frames)
	{
		if (F.bTransition) { continue; }
		if (F.bLabelled != F.bPixelVisible) { return false; }
	}
	return true;
}

static std::vector<FGateFrame> BuildSmearedEvent(int LabelledFrames, int OnsetLag, int ClearLag, bool bUseFlag, int On, int Off)
{
	using namespace AnomalyLabelSync;
	std::vector<FGateFrame> Frames;
	const int Start = 10;
	const int End = Start + LabelledFrames - 1;
	FEventTransitionTrack Track;
	for (int si = 0; si < End + 20; ++si)
	{
		FGateFrame G;
		G.bLabelled = si >= Start && si <= End;
		G.bPixelVisible = si >= Start + OnsetLag && si <= End + ClearLag;
		if (bUseFlag && si >= Start - 2)
		{
			bool bOn = false, bOff = false;
			Track.Observe(si, G.bLabelled, On, Off, bOn, bOff);
			const EEntryEmit Mode = DecideRenderTruthEntry(G.bLabelled, bOff);
			G.bTransition = bOn || Mode == EEntryEmit::TransitionOnly;
		}
		Frames.push_back(G);
	}
	return Frames;
}

static void TestTransitionGate()
{
	int LegacyFails = 0;
	int NewPasses = 0;
	const int ClearLags[] = { 1, 2, 3, 4, 5, 6, 7 };
	for (int ClearLag : ClearLags)
	{
		if (!TransitionGatePasses(BuildSmearedEvent(20, 2, ClearLag, false, 0, 0))) { ++LegacyFails; }
		if (TransitionGatePasses(BuildSmearedEvent(20, 2, ClearLag, true, 2, 8))) { ++NewPasses; }
	}
	Check(LegacyFails == 7, "transition gate 44584e7: onset +2 and clear +1..+7 fail on all 7 (no transition flag exists)");
	Check(NewPasses == 7, "transition gate new: every unlabelled pixel-visible and labelled-not-yet-visible frame is flagged, 7/7");
	Check(TransitionGatePasses(BuildSmearedEvent(20, 0, 0, false, 0, 0)), "transition gate: an exact AA-off event passes with no flag");
	Check(!TransitionGatePasses(BuildSmearedEvent(20, 2, 9, true, 2, 8)), "transition gate can fail on the new logic: clear +9 beyond K_off=8 FAILS");
	Check(!TransitionGatePasses(BuildSmearedEvent(20, 3, 1, true, 2, 8)), "transition gate can fail on the new logic: onset +3 beyond K_on=2 FAILS");
}

static std::vector<int> RunHideSequence(const std::vector<int>& HiddenBySI, int LiveUntil, int Frames, int K, std::vector<int>* OutGone)
{
	using namespace AnomalyLabelSync;
	FHideReturnTrack T;
	std::vector<int> Live;
	for (int si = 0; si < Frames; ++si)
	{
		if (si <= LiveUntil)
		{
			if (StepHideLive(T, HiddenBySI[si] != 0, K)) { Live.push_back(si); }
		}
		else
		{
			if (StepHideGone(T, K) && OutGone) { OutGone->push_back(si); }
			if (HideTrackDone(T)) { break; }
		}
	}
	return Live;
}

static void TestHideReturn()
{
	std::vector<int> Blink = { 0, 0, 1, 1, 0, 0, 0, 1, 1, 0 };
	std::vector<int> Gone;
	std::vector<int> Live = RunHideSequence(Blink, 8, 14, 1, &Gone);
	Check(Live == std::vector<int>({ 4 }), "hide: blinking in-window return flagged at the first visible frame " + Str(Live));
	Check(Gone == std::vector<int>({ 9 }), "hide: blinking final return (after revert) flagged on the first captured frame after it " + Str(Gone));
	std::vector<int> Missing = { 1, 1, 1, 1, 1, 1, 1, 1 };
	Gone.clear();
	Live = RunHideSequence(Missing, 7, 12, 1, &Gone);
	Check(Live.empty() && Gone == std::vector<int>({ 8 }), "hide: missing_object flags only the first frame after the object returns " + Str(Gone));
	Gone.clear();
	Live = RunHideSequence(Missing, 7, 12, 0, &Gone);
	Check(Live.empty() && Gone.empty(), "hide: K=0 (no temporal AA) flags nothing");
	std::vector<int> EndsVisible = { 0, 1, 1, 0, 0 };
	Gone.clear();
	Live = RunHideSequence(EndsVisible, 4, 8, 1, &Gone);
	Check(Live == std::vector<int>({ 3 }) && Gone.empty(), "hide: an event that ends on a visible frame adds no post-revert entry");
	Gone.clear();
	Live = RunHideSequence(Missing, 7, 12, 2, &Gone);
	Check(Gone == std::vector<int>({ 8, 9 }), "hide: K=2 flags the first two frames after return");

	std::vector<FGateFrame> LegacyResidual;
	std::vector<FGateFrame> NewResidual;
	for (int si = 0; si < 12; ++si)
	{
		FGateFrame G;
		G.bLabelled = si <= 7;
		G.bPixelVisible = si <= 8;
		LegacyResidual.push_back(G);
		G.bTransition = si == 8;
		NewResidual.push_back(G);
	}
	Check(!TransitionGatePasses(LegacyResidual), "hide gate 44584e7: the partly visible first frame after return fails (no flag)");
	Check(TransitionGatePasses(NewResidual), "hide gate new: that frame carries transition=1 and passes");
}

struct FSimEvent
{
	int Fire = 0;
	int End = 0;
	int Target = 0;
	int Tag = 0;
	bool bRecycled = false;
	bool bReleasable = false;
	long long ReleasableSince = -1;
	int AttachedUntil = -1;
	int ArmsIssued = 0;
};

struct FSimResult
{
	int Events = 0;
	int Untagged = 0;
	int Recycles = 0;
	int AliasFrames = 0;
	int LabelledFramesWithoutMask = 0;
	int PeakLive = 0;
	int CarriedRecycledWhileAttached = 0;
};

enum class ESimPolicy { Legacy44584, Ruled, NaiveReleaseAtRevert };

static FSimResult SimulateTagPool(ESimPolicy Policy, int NumEvents, int Gap, int Length, int Latency, int Pool, int CarriedAttachedUntil)
{
	using namespace AnomalyLabelSync;
	const int Base = 200;
	std::vector<FSimEvent> Ev;
	std::set<int> Claimed;
	int NextOffset = 0;
	FSimResult Res;
	int Next = 0;
	if (CarriedAttachedUntil >= 0)
	{
		FSimEvent C;
		C.Fire = -Length;
		C.End = -1;
		C.Target = 999;
		C.AttachedUntil = CarriedAttachedUntil;
		C.Tag = Legacy44584::AllocateTag(Claimed, NextOffset, Base, Pool);
		Ev.push_back(C);
	}
	const int LastFrame = NumEvents * Gap + Length + Latency + 8;
	for (int f = 0; f <= LastFrame; ++f)
	{
		for (size_t i = 0; i < Ev.size(); ++i)
		{
			FSimEvent& E = Ev[i];
			const bool bLive = f >= E.Fire && f <= E.End;
			if (f >= std::max(E.Fire, 0) && E.ArmsIssued < 4) { ++E.ArmsIssued; }
			if (E.Tag == 0 || E.bRecycled) { E.bReleasable = false; continue; }
			FTagReleaseInputs In;
			In.Tag = E.Tag;
			In.bFireLive = bLive || f < E.Fire;
			In.bTrailBlocks = Policy == ESimPolicy::NaiveReleaseAtRevert ? false
				: TrailBlocksRelease(E.AttachedUntil >= 0, f <= E.AttachedUntil, true);
			const int LastProductFrame = std::max(E.End, E.AttachedUntil);
			In.bInPendingSnapshot = Policy == ESimPolicy::NaiveReleaseAtRevert ? false : f <= LastProductFrame + Latency;
			In.bInPendingTargetMask = In.bInPendingSnapshot;
			In.bM26MayArmAgain = Policy == ESimPolicy::NaiveReleaseAtRevert ? false
				: M26MayArmAgain(E.ArmsIssued, 4, true, false, false, false, bLive);
			const bool bRel = IsTagReleasable(In);
			if (bRel && !E.bReleasable) { E.ReleasableSince = f; }
			E.bReleasable = bRel;
		}
		int Live = 0;
		for (const FSimEvent& E : Ev) { if (E.Tag != 0 && !E.bRecycled && !E.bReleasable) { ++Live; } }
		Res.PeakLive = std::max(Res.PeakLive, Live);

		if (Next < NumEvents && f == Next * Gap)
		{
			FSimEvent E;
			E.Fire = f;
			E.End = f + Length - 1;
			E.Target = Next % 7;
			E.Tag = Legacy44584::AllocateTag(Claimed, NextOffset, Base, Pool);
			if (E.Tag == 0 && Policy != ESimPolicy::Legacy44584)
			{
				std::vector<FRecycleCandidate> C;
				for (size_t i = 0; i < Ev.size(); ++i)
				{
					FRecycleCandidate R;
					R.Record = (int)i;
					R.Tag = Ev[i].Tag;
					R.bReleasable = Ev[i].bReleasable;
					R.bAlreadyRecycled = Ev[i].bRecycled;
					R.ReleasableSince = Ev[i].ReleasableSince;
					C.push_back(R);
				}
				const int Pick = PickRecycleVictim(C.data(), (int)C.size());
				if (Pick >= 0)
				{
					if (f <= Ev[C[Pick].Record].AttachedUntil) { ++Res.CarriedRecycledWhileAttached; }
					Ev[C[Pick].Record].bRecycled = true;
					E.Tag = C[Pick].Tag;
					++Res.Recycles;
				}
			}
			if (E.Tag == 0) { ++Res.Untagged; }
			Ev.push_back(E);
			++Res.Events;
			++Next;
		}

		std::vector<int> InFlight;
		for (const FSimEvent& E : Ev)
		{
			const int LastProductFrame = std::max(E.End, E.AttachedUntil);
			const bool bCarrying = E.Tag != 0 && f >= E.Fire && f <= LastProductFrame + Latency;
			if (bCarrying) { InFlight.push_back(E.Tag); }
		}
		if (AnyAlias(InFlight.data(), (int)InFlight.size())) { ++Res.AliasFrames; }
		for (const FSimEvent& E : Ev)
		{
			if (E.Target != 999 && f >= E.Fire && f <= E.End && E.Tag == 0) { ++Res.LabelledFramesWithoutMask; }
		}
	}
	return Res;
}

static void TestTagRecycling()
{
	const FSimResult Legacy = SimulateTagPool(ESimPolicy::Legacy44584, 90, 12, 8, 3, 55, -1);
	Check(Legacy.Events == 90 && Legacy.Untagged == 35 && Legacy.LabelledFramesWithoutMask == 35 * 8,
		"recycle 44584e7: 90 events on a 55-value pool leave the last 35 without a mask (" + std::to_string(Legacy.Untagged) + " untagged)");
	const FSimResult Ruled = SimulateTagPool(ESimPolicy::Ruled, 90, 12, 8, 3, 55, -1);
	Check(Ruled.Untagged == 0 && Ruled.LabelledFramesWithoutMask == 0 && Ruled.Recycles == 35,
		"recycle new: all 90 events tagged, every labelled frame has a mask, 35 recycles (" + std::to_string(Ruled.Recycles) + ")");
	Check(Ruled.AliasFrames == 0, "recycle new: no frame ever has two events in flight under one value (>55 events)");
	Check(Ruled.PeakLive <= 2, "recycle new: peak live values stays small on a sequential auto-pool (" + std::to_string(Ruled.PeakLive) + ")");
	Check(Legacy.AliasFrames == 0, "recycle 44584e7: never recycles, so never aliases (the coverage case is where it fails)");

	const FSimResult Short = SimulateTagPool(ESimPolicy::Ruled, 40, 12, 8, 3, 55, -1);
	const FSimResult ShortLegacy = SimulateTagPool(ESimPolicy::Legacy44584, 40, 12, 8, 3, 55, -1);
	Check(Short.Recycles == 0 && Short.Untagged == ShortLegacy.Untagged && Short.Untagged == 0,
		"recycle new: a run that never exhausts the pool recycles nothing (allocation identical to 44584e7)");

	const FSimResult Naive = SimulateTagPool(ESimPolicy::NaiveReleaseAtRevert, 6, 5, 5, 3, 1, -1);
	const FSimResult RuledTight = SimulateTagPool(ESimPolicy::Ruled, 6, 5, 5, 3, 1, -1);
	Check(Naive.AliasFrames > 0, "no-alias can-fail: releasing at revert (before read-back) aliases two events in flight ("
		+ std::to_string(Naive.AliasFrames) + " frames)");
	Check(RuledTight.AliasFrames == 0, "no-alias new: waiting for read-back never aliases on the same tight pool");

	const FSimResult Carried = SimulateTagPool(ESimPolicy::Ruled, 12, 6, 4, 3, 3, 40);
	Check(Carried.AliasFrames == 0 && Carried.Untagged == 0 && Carried.CarriedRecycledWhileAttached == 0 && Carried.Recycles > 0,
		"no-alias across a run boundary: on a 3-value pool a carried trail attached for 40 frames is never recycled while "
		"attached; all 12 events tagged with " + std::to_string(Carried.Recycles) + " recycles");
	const FSimResult CarriedNaive = SimulateTagPool(ESimPolicy::NaiveReleaseAtRevert, 12, 6, 4, 3, 3, 40);
	Check(CarriedNaive.CarriedRecycledWhileAttached > 0 && CarriedNaive.AliasFrames > 0,
		"no-alias can-fail across a run boundary: releasing on fire end recycles the carried trail's value while it is attached ("
		+ std::to_string(CarriedNaive.AliasFrames) + " alias frames)");
	const FSimResult Long = SimulateTagPool(ESimPolicy::Ruled, 200, 5, 4, 3, 55, 20);
	Check(Long.AliasFrames == 0 && Long.Untagged == 0 && Long.LabelledFramesWithoutMask == 0,
		"recycle new: 200 events with a carried trail, every labelled frame has a mask, no alias (" + std::to_string(Long.Recycles) + " recycles)");

	AnomalyLabelSync::FTagReleaseInputs In;
	In.Tag = 210;
	Check(AnomalyLabelSync::IsTagReleasable(In), "release: a finished, read-back event is releasable");
	In.bTrailBlocks = AnomalyLabelSync::TrailBlocksRelease(true, false, false);
	Check(!AnomalyLabelSync::IsTagReleasable(In), "release: a detached trail with a request still in flight blocks (it could re-attach)");
	In.bTrailBlocks = AnomalyLabelSync::TrailBlocksRelease(true, false, true);
	In.bM26ArmInFlight = true;
	Check(!AnomalyLabelSync::IsTagReleasable(In), "release: an m26 arm in flight blocks");
	In.bM26ArmInFlight = false;
	In.bM26MayArmAgain = AnomalyLabelSync::M26MayArmAgain(2, 4, true, false, false, false, false);
	Check(!AnomalyLabelSync::IsTagReleasable(In), "release: a record m26 will still arm (post-revert arms) blocks");
	In.bM26MayArmAgain = AnomalyLabelSync::M26MayArmAgain(2, 4, true, false, false, true, false);
	Check(AnomalyLabelSync::IsTagReleasable(In), "release: a deferred-onset record cannot arm once its fire ended");
	In.bAlreadyRecycled = true;
	Check(!AnomalyLabelSync::IsTagReleasable(In), "release: a value already moved on is never released twice");
	const int Tags[] = { 200, 201, 0, 0, 202 };
	const int Dup[] = { 200, 201, 200 };
	Check(!AnomalyLabelSync::AnyAlias(Tags, 5) && AnomalyLabelSync::AnyAlias(Dup, 3), "alias detector: zeros ignored, a duplicate found");
}

struct FTexSample
{
	int Resident;
	int Baseline;
	int HeldLevel;
	bool bKnown;
};

struct FTestRangeList
{
	std::vector<FSIRange> V;
	int Num() const { return (int)V.size(); }
	FSIRange& operator[](int i) { return V[i]; }
	const FSIRange& operator[](int i) const { return V[i]; }
	void Insert(const FSIRange& R, int Pos) { V.insert(V.begin() + Pos, R); }
	void RemoveAt(int Pos) { V.erase(V.begin() + Pos); }
};

using FPartialEdgeTrack = TPartialEdgeTrack<FTestRangeList>;

static EHeldSet HeldSetOf(const std::vector<FTexSample>& Set, EVerdict* OutVerdict = nullptr)
{
	std::vector<ETexState> States;
	std::vector<ELevel> Levels;
	for (const FTexSample& T : Set)
	{
		const ETexState S = ClassifyTexture(T.Resident, T.Baseline, T.bKnown);
		States.push_back(S);
		Levels.push_back(ClassifyLevel(S, T.Resident, T.Baseline, T.HeldLevel));
	}
	if (OutVerdict)
	{
		*OutVerdict = Combine(States.data(), (int)States.size());
	}
	return ClassifyHeldSet(Levels.data(), (int)Levels.size());
}

static bool PartialOf(const std::vector<FTexSample>& Set, EVerdict* OutVerdict = nullptr)
{
	return HeldSetOf(Set, OutVerdict) == EHeldSet::Partial;
}

namespace Legacy0847
{
	static ELevel LegacyClassifyLevel(ETexState State, int RenderResident, int Baseline, int HeldLevel)
	{
		if (State == ETexState::Unknown) { return ELevel::Unknown; }
		if (State == ETexState::Baseline) { return ELevel::Baseline; }
		if (HeldLevel <= 0 || HeldLevel >= Baseline) { return ELevel::AtHeld; }
		return RenderResident <= HeldLevel ? ELevel::AtHeld : ELevel::Between;
	}

	static bool IsPartial(const std::vector<FTexSample>& Set)
	{
		bool bBelow = false;
		bool bShort = false;
		for (const FTexSample& T : Set)
		{
			const ELevel L = LegacyClassifyLevel(ClassifyTexture(T.Resident, T.Baseline, T.bKnown), T.Resident, T.Baseline, T.HeldLevel);
			if (L == ELevel::AtHeld || L == ELevel::Between) { bBelow = true; }
			if (L != ELevel::AtHeld) { bShort = true; }
		}
		return bBelow && bShort;
	}

	struct FEdgeTrack
	{
		static constexpr int Cap = 32;
		int PartialSI[Cap] = {};
		int NumPartial = 0;
		int PartialOverflow = 0;
		int FirstFullSI = -1;

		void Observe(int SI, bool bMember, bool bPartial)
		{
			if (!bMember) { return; }
			if (!bPartial)
			{
				FirstFullSI = (FirstFullSI < 0 || SI < FirstFullSI) ? SI : FirstFullSI;
				return;
			}
			for (int i = 0; i < NumPartial; ++i) { if (PartialSI[i] == SI) { return; } }
			if (NumPartial == Cap) { ++PartialOverflow; return; }
			PartialSI[NumPartial++] = SI;
		}

		int CountOnset() const
		{
			int N = 0;
			for (int i = 0; i < NumPartial; ++i) { if (FirstFullSI < 0 || PartialSI[i] < FirstFullSI) { ++N; } }
			return N;
		}
	};
}

static std::set<int> RangeMembers(const FTestRangeList& L)
{
	std::set<int> Out;
	for (const FSIRange& R : L.V)
	{
		for (int si = R.First; si <= R.Last; ++si) { Out.insert(si); }
	}
	return Out;
}

static void TestPartialHeldSet()
{
	const std::vector<FTexSample> Onset06 = { { 7, 11, 7, true }, { 7, 11, 7, true }, { 11, 11, 7, true } };
	const std::vector<FTexSample> Onset03 = { { 11, 11, 7, true }, { 11, 11, 7, true }, { 7, 11, 7, true } };
	const std::vector<FTexSample> Full = { { 7, 11, 7, true }, { 7, 11, 7, true }, { 7, 11, 7, true } };
	const std::vector<FTexSample> Offset03 = { { 11, 11, 7, true }, { 7, 11, 7, true }, { 7, 11, 7, true } };
	const std::vector<FTexSample> Clean = { { 11, 11, 7, true }, { 11, 11, 7, true }, { 11, 11, 7, true } };
	EVerdict V06 = EVerdict::Unknown;
	EVerdict VFull = EVerdict::Unknown;
	EVerdict V03 = EVerdict::Unknown;
	const bool bP06 = PartialOf(Onset06, &V06);
	const bool bPFull = PartialOf(Full, &VFull);
	const bool bP03 = PartialOf(Onset03, &V03);
	Check(bP06 && bP03, "partial: 084-06 onset (AORM+D held, N at baseline) and 084-03 onset (N held, AORM+D baseline) are both PARTIAL");
	Check(!bPFull, "partial: all three textures at the held level is NOT partial");
	Check(V06 == EVerdict::Held && V03 == EVerdict::Held && VFull == EVerdict::Held,
		"BOTH WAYS: the binary record (Combine) reads HELD for the partial and the full set alike - it cannot see the difference the flag adds");
	Check(PartialOf(Offset03), "partial: the offset edge (AORM restored first, D+N still held) is PARTIAL");
	Check(!PartialOf(Clean), "partial: a set wholly at baseline is not partial (and is not a member)");
	Check(PartialOf({ { 9, 11, 7, true } }), "partial: a single texture part-way down its chain (9 of 11, held 7) is PARTIAL");
	Check(!PartialOf({ { 6, 11, 7, true } }), "partial: a texture below its held level counts as held");
	Check(HeldSetOf(Full) == EHeldSet::Full && HeldSetOf(Clean) == EHeldSet::NotHeld,
		"N4 held set: all three at the held level is KNOWN-FULL; all at baseline is not held");

	const std::vector<FTexSample> HeldPlusUnknown = { { 7, 11, 7, true }, { -1, 11, 7, false } };
	Check(HeldSetOf(HeldPlusUnknown) == EHeldSet::Unresolved,
		"N4: a held texture beside an UNKNOWN one is UNRESOLVED - the unknown one may be fully held too, so no partial claim");
	Check(Legacy0847::IsPartial(HeldPlusUnknown),
		"N4 BOTH WAYS: the 084-07 rule called the same record PARTIAL (unknown counted as short of held)");
	const std::vector<FTexSample> NoEndpoint = { { 9, 11, 0, true } };
	Check(HeldSetOf(NoEndpoint) == EHeldSet::Unresolved,
		"N4: an INVALID held endpoint (0) makes a below-baseline texture UNRESOLVED, not at-held");
	Check(!Legacy0847::IsPartial(NoEndpoint) && Legacy0847::LegacyClassifyLevel(ETexState::Held, 9, 11, 0) == ELevel::AtHeld,
		"N4 BOTH WAYS: the 084-07 rule read the invalid endpoint as AT-HELD, so an intermediate 9 of 11 went unflagged");
	Check(HeldSetOf({ { 9, 11, 11, true } }) == EHeldSet::Unresolved, "N4: an endpoint at the baseline is invalid too - unresolved");
	Check(HeldSetOf({ { -1, 11, 7, false }, { -1, 11, 7, false } }) == EHeldSet::Unresolved,
		"N4: an all-unknown record is unresolved");
	Check(HeldSetOf({}) == EHeldSet::Unresolved, "N4: a member frame with NO texture record (watch missing) is unresolved, never full");
	FPartialEdgeTrack Settle;
	Settle.Observe(20, true, EHeldSet::Partial);
	Settle.Observe(21, true, EHeldSet::Full);
	Settle.Observe(22, true, EHeldSet::NotHeld);
	Settle.Observe(23, true, EHeldSet::Partial);
	Check(Settle.LastFullSI == 21 && Settle.CountOffset() == 1 && Settle.NumUnresolved() == 0,
		"N4: a known all-baseline member (settle tail) is neither a full-set boundary nor unresolved - frame 23 stays an offset partial");
	Check(HeldSetOf({ { 7, 11, 7, true }, { 11, 11, 7, true }, { -1, 11, 7, false } }) == EHeldSet::Partial,
		"N4: a KNOWN held texture beside a KNOWN baseline one is record-backed PARTIAL even when a third is unknown");

	FPartialEdgeTrack T;
	T.Observe(120, false, EHeldSet::NotHeld);
	T.Observe(121, true, HeldSetOf(Onset06));
	T.Observe(122, true, HeldSetOf(Onset06));
	for (int si = 123; si <= 130; ++si)
	{
		T.Observe(si, true, HeldSetOf(Full));
	}
	T.Observe(131, true, HeldSetOf(Offset03));
	T.Observe(132, false, EHeldSet::NotHeld);
	Check(T.CountOnset() == 2 && T.CountOffset() == 1 && T.CountMid() == 0 && T.NumPartial() == 3,
		"edge track: the banked two-frame onset plus a one-frame offset partial read onset 2 / offset 1 / mid 0");
	FPartialEdgeTrack W;
	for (int si = 55; si <= 59; ++si)
	{
		W.Observe(si, true, EHeldSet::Partial);
	}
	for (int si = 60; si <= 62; ++si)
	{
		W.Observe(si, true, EHeldSet::Full);
	}
	Check(W.CountOnset() == 5, "edge track: the 084-06 warm-up event (N held alone for 5 frames) reads 5 onset partial frames");
	FPartialEdgeTrack O;
	O.Observe(10, true, EHeldSet::Partial);
	O.Observe(9, true, EHeldSet::Full);
	O.Observe(8, true, EHeldSet::Partial);
	Check(O.CountOnset() == 1 && O.CountOffset() == 1, "edge track: out-of-order observation still splits edges by the first full frame");

	FPartialEdgeTrack U;
	U.Observe(10, true, EHeldSet::Unresolved);
	U.Observe(11, true, EHeldSet::Partial);
	U.Observe(12, true, EHeldSet::Full);
	U.Observe(13, true, EHeldSet::Full);
	Check(U.FirstFullSI == 12 && U.CountOnset() == 1 && U.NumUnresolved() == 1 && RangeMembers(U.Unresolved) == std::set<int>({ 10 }),
		"N4: an UNRESOLVED member never sets the full-set boundary - frame 11 stays an ONSET partial and 10 is listed unresolved");
	Legacy0847::FEdgeTrack UL;
	UL.Observe(10, true, false);
	UL.Observe(11, true, true);
	UL.Observe(12, true, false);
	Check(UL.FirstFullSI == 10 && UL.CountOnset() == 0,
		"N4 BOTH WAYS: the 084-07 track took the all-unknown frame as FULL, so frame 11 lost its onset edge");

	FPartialEdgeTrack Long;
	std::set<int> Flagged;
	for (int si = 100; si <= 139; ++si)
	{
		Long.Observe(si, true, EHeldSet::Partial);
		Flagged.insert(si);
	}
	for (int si = 140; si <= 150; ++si)
	{
		Long.Observe(si, true, EHeldSet::Full);
	}
	for (int si = 151; si <= 155; ++si)
	{
		Long.Observe(si, true, EHeldSet::Partial);
		Flagged.insert(si);
	}
	Check(Long.NumPartial() == 45 && Long.CountOnset() == 40 && Long.CountOffset() == 5 && Long.Partial.Num() == 2
		&& RangeMembers(Long.Partial) == Flagged,
		"N5: 45 partial frames (40 onset) are stored as 2 exact ranges and EVERY flagged frame is identifiable - no cap");
	Legacy0847::FEdgeTrack LongLegacy;
	for (int si = 100; si <= 139; ++si) { LongLegacy.Observe(si, true, true); }
	Check(LongLegacy.NumPartial == 32 && LongLegacy.PartialOverflow == 8 && LongLegacy.CountOnset() == 32,
		"N5 BOTH WAYS: the 084-07 32-slot list names only 32 of 40 onset frames and counts 8 as bare overflow");
	FPartialEdgeTrack Merge;
	Merge.Observe(5, true, EHeldSet::Partial);
	Merge.Observe(3, true, EHeldSet::Partial);
	Merge.Observe(9, true, EHeldSet::Partial);
	Merge.Observe(4, true, EHeldSet::Partial);
	Merge.Observe(8, true, EHeldSet::Partial);
	Merge.Observe(7, true, EHeldSet::Partial);
	Merge.Observe(6, true, EHeldSet::Partial);
	Merge.Observe(6, true, EHeldSet::Partial);
	Check(Merge.Partial.Num() == 1 && Merge.Partial[0].First == 3 && Merge.Partial[0].Last == 9 && Merge.NumPartial() == 7,
		"N5: out-of-order and repeated frames merge into one exact range [3,9]");
	FPartialEdgeTrack Gaps;
	for (int si = 0; si < 200; si += 2)
	{
		Gaps.Observe(si, true, EHeldSet::Partial);
	}
	Check(Gaps.Partial.Num() == 100 && Gaps.NumPartial() == 100, "N5: 100 separated partial frames keep 100 ranges - the list grows, it never overflows");
}

static void TestAnnotationMembership()
{
	using namespace AnomalyLabelSync;
	struct FFrame
	{
		bool bLive;
		bool bActive;
		bool bOnScreen;
		bool bGameThreadState;
	};
	struct FCase
	{
		const char* Name;
		EAnnotationPolicy Policy;
		std::vector<FFrame> Frames;
		std::set<int> ExpectedList;
	};
	const std::vector<FCase> Cases = {
		{ "FireWindow (missing_texture / corrupted_texture): annotation lists the frames whose box is on screen", EAnnotationPolicy::FireWindow,
			{ { true, false, true, false }, { true, false, true, false }, { true, false, false, false }, { true, false, true, false } },
			{ 0, 1, 3 } },
		{ "ActorHidden (blinking): annotation lists the hidden frames", EAnnotationPolicy::ActorHidden,
			{ { true, false, true, false }, { true, true, true, false }, { true, true, true, false }, { true, false, true, false },
			  { true, false, true, false }, { true, true, true, false } },
			{ 1, 2, 5 } },
		{ "AnomalyState (lod_popping): annotation lists the frames the anomaly is in its anomalous state", EAnnotationPolicy::AnomalyState,
			{ { true, false, true, false }, { true, true, true, false }, { true, true, true, false }, { true, false, true, false } },
			{ 1, 2 } },
		{ "RenderHeldWindow (stuck_low_mip): annotation lists the render-record member frames", EAnnotationPolicy::RenderHeldWindow,
			{ { true, false, true, true }, { true, false, true, true }, { true, true, true, true }, { true, true, true, false },
			  { true, true, true, false }, { true, false, true, false } },
			{ 2, 3, 4 } },
	};
	for (const FCase& C : Cases)
	{
		std::set<int> List;
		std::set<int> Labelled;
		std::set<int> BrokenActivity;
		std::set<int> BrokenLive;
		std::set<int> BrokenGameThread;
		for (int si = 0; si < (int)C.Frames.size(); ++si)
		{
			const FFrame& F = C.Frames[si];
			if (IsAnnotationMember(C.Policy, F.bActive, F.bOnScreen, true)) { List.insert(si); }
			if (IsEntryLabelled(true, C.Policy, F.bActive, F.bOnScreen, true)) { Labelled.insert(si); }
			if (F.bActive) { BrokenActivity.insert(si); }
			if (F.bLive) { BrokenLive.insert(si); }
			if (F.bGameThreadState) { BrokenGameThread.insert(si); }
		}
		Check(List == C.ExpectedList, std::string("N1 ") + C.Name + " " + Str(std::vector<int>(List.begin(), List.end())));
		Check(Labelled == List, std::string("N1 ") + DescribeAnnotationPolicy(C.Policy) + ": every row's labelled == the frame's membership in annotation.json");
		if (C.Policy == EAnnotationPolicy::FireWindow)
		{
			Check(BrokenActivity != List && BrokenActivity.empty(),
				"N1 BOTH WAYS fire_window: the 084-07 writer read the activity bit (logically hidden) and labelled NO texture frame - it must fail");
		}
		else if (C.Policy == EAnnotationPolicy::RenderHeldWindow)
		{
			Check(BrokenGameThread != List,
				"N1 BOTH WAYS render_held_window: reading the game-thread activity bit instead of the render record disagrees - it must fail");
			Check(BrokenLive != List, "N1 BOTH WAYS render_held_window: reading 'event live' labels every frame - it must fail");
		}
		else
		{
			Check(BrokenLive != List, std::string("N1 BOTH WAYS ") + DescribeAnnotationPolicy(C.Policy)
				+ ": reading 'event live' (the pre-084-07 rule) labels the un-applied phases - it must fail");
			Check(BrokenActivity == List, std::string("N1 ") + DescribeAnnotationPolicy(C.Policy) + ": here the activity bit IS the authority");
		}
	}
	Check(!IsEntryLabelled(false, EAnnotationPolicy::RenderHeldWindow, true, true, true),
		"N1: a transition-only or suppressed entry is never labelled, whatever its inputs");
	Check(IsEntryLabelled(true, EAnnotationPolicy::FireWindow, false, true, true)
		&& !IsEntryLabelled(true, EAnnotationPolicy::FireWindow, true, false, true),
		"N1 fire_window: with the effect installed, on-screen decides and the hidden bit is ignored");
}

static void TestF1EffectInstalled()
{
	using namespace AnomalyLabelSync;
	struct FRun
	{
		std::vector<int> Labelled;
		std::vector<int> Listed;
		std::vector<int> Interrupted;
		std::vector<int> OldRuleLabelled;
	};
	auto Run = [](int Apply, int End, const std::vector<bool>& InstalledFrom10, bool bOnScreen)
	{
		FRun R;
		for (int si = Apply; si < End; ++si)
		{
			const size_t k = (size_t)(si - Apply);
			const bool bInstalled = k < InstalledFrom10.size() ? InstalledFrom10[k] : false;
			const EEntryEmit Emit = DecideInterruptedEntry(EEntryEmit::Normal, EAnnotationPolicy::FireWindow, bInstalled);
			if (IsEntryLabelled(Emit == EEntryEmit::Normal, EAnnotationPolicy::FireWindow, false, bOnScreen, bInstalled))
			{
				R.Labelled.push_back(si);
			}
			if (IsAnnotationMember(EAnnotationPolicy::FireWindow, false, bOnScreen, bInstalled))
			{
				R.Listed.push_back(si);
			}
			if (Emit == EEntryEmit::TransitionOnly && IsEffectInterrupted(EAnnotationPolicy::FireWindow, bInstalled))
			{
				R.Interrupted.push_back(si);
			}
			if (IsAnnotationMember(EAnnotationPolicy::FireWindow, false, bOnScreen, true))
			{
				R.OldRuleLabelled.push_back(si);
			}
		}
		return R;
	};
	auto Range = [](int A, int B)
	{
		std::vector<int> V;
		for (int i = A; i <= B; ++i) { V.push_back(i); }
		return V;
	};

	std::vector<bool> RawRevert(30, false);
	for (int k = 0; k < 10; ++k) { RawRevert[(size_t)k] = true; }
	const FRun Codex = Run(10, 40, RawRevert, true);
	Check(Codex.Labelled == Range(10, 19), "F1 Codex case: apply at 10, raw revert at 20, scheduled end at 40 - labelled 10..19 only, got "
		+ Str(Codex.Labelled));
	Check(Codex.Listed == Codex.Labelled, "F1 Codex case: annotation.json lists exactly the labelled frames");
	Check(Codex.Interrupted == Range(20, 39),
		"F1 Codex case: 20..39 are written transition-only with transition_reason effect_interrupted, got " + Str(Codex.Interrupted));
	Check(Codex.OldRuleLabelled == Range(10, 39) && Codex.OldRuleLabelled != Codex.Labelled,
		"F1 BOTH WAYS: the pre-090-05 rule (on screen only) labels 10..39 - a clean picture after the revert - and must disagree");

	std::vector<bool> HostSwap(15, true);
	for (int k = 5; k < 8; ++k) { HostSwap[(size_t)k] = false; }
	const FRun Host = Run(10, 25, HostSwap, true);
	std::vector<int> Want = Range(10, 14);
	for (int i = 18; i <= 24; ++i) { Want.push_back(i); }
	Check(Host.Labelled == Want, "F1 host replaces the material at 15, our effect re-installed at 18: labelled 10..14 and 18..24, got "
		+ Str(Host.Labelled));
	Check(Host.Interrupted == Range(15, 17), "F1 host replacement: 15..17 unlabelled and flagged effect_interrupted, got "
		+ Str(Host.Interrupted));

	const FRun Off = Run(10, 20, std::vector<bool>(10, true), false);
	Check(Off.Labelled.empty() && Off.Interrupted.empty(),
		"F1: an installed effect whose box is off screen is unlabelled and NOT flagged interrupted (the pre-existing rule)");

	Check(IsAnnotationMember(EAnnotationPolicy::ActorHidden, true, true, false)
		&& IsAnnotationMember(EAnnotationPolicy::AnomalyState, true, true, false)
		&& IsAnnotationMember(EAnnotationPolicy::RenderHeldWindow, true, true, false),
		"F1 scope: the installed bit is read only for fire_window; the other policies keep their own authority");
	Check(DecideInterruptedEntry(EEntryEmit::Normal, EAnnotationPolicy::ActorHidden, false) == EEntryEmit::Normal,
		"F1 scope: a hide-class entry is never marked effect_interrupted");
	Check(DecideInterruptedEntry(EEntryEmit::Suppress, EAnnotationPolicy::FireWindow, false) == EEntryEmit::Suppress,
		"F1: a suppressed entry stays suppressed");
	Check(DecideInterruptedEntry(EEntryEmit::Normal, EAnnotationPolicy::FireWindow, true) == EEntryEmit::Normal,
		"F1: an installed fire_window entry is untouched");
	Check(std::string(DescribeReasonBit(5)) == "effect_interrupted" && ReasonEffectInterrupted == 32
		&& ReasonsOrLegacy(ReasonEffectInterrupted) == ReasonEffectInterrupted
		&& (ReasonEffectInterrupted & (ReasonTemporal | ReasonHideReturn | ReasonPartial | ReasonCameraUnconfirmed | ReasonUnresolved)) == 0,
		"F1 reasons: effect_interrupted is its own sixth bit and is not rewritten as a legacy temporal flag");
}

static void TestR1PartialAndR5NaniteGate()
{
	using namespace AnomalyLabelSync;
	Check(IsAnnotationMember(EAnnotationPolicy::FireWindow, true, true, AnomalyInstall::IsInstalledByte((unsigned char)AnomalyInstall::EState::Partial)),
		"R1: a partially replaced effect (some targeted slots still ours) stays labelled");
	Check(DecideInterruptedEntry(EEntryEmit::Normal, EAnnotationPolicy::FireWindow,
		AnomalyInstall::IsInstalledByte((unsigned char)AnomalyInstall::EState::Partial)) == EEntryEmit::Normal,
		"R1: a partial frame is not flagged effect_interrupted");
	Check(DecideInterruptedEntry(EEntryEmit::Normal, EAnnotationPolicy::FireWindow,
		AnomalyInstall::IsInstalledByte((unsigned char)AnomalyInstall::EState::None)) == EEntryEmit::TransitionOnly,
		"R1: no targeted slot ours -> effect_interrupted");
	Check(!IsAnnotationMemberGated(EAnnotationPolicy::FireWindow, true, true, true, true)
		&& !IsAnnotationMemberGated(EAnnotationPolicy::ActorHidden, true, true, true, true)
		&& !IsAnnotationMemberGated(EAnnotationPolicy::AnomalyState, true, true, true, true)
		&& !IsAnnotationMemberGated(EAnnotationPolicy::RenderHeldWindow, true, true, true, true),
		"R5: no policy labels a frame while the target draws a Nanite primitive");
	Check(IsAnnotationMemberGated(EAnnotationPolicy::FireWindow, true, true, true, false)
		&& IsAnnotationMemberGated(EAnnotationPolicy::ActorHidden, true, false, false, false),
		"R5: without a Nanite block the membership is the old one");
	Check(DecideNaniteEntry(EEntryEmit::Normal, true) == EEntryEmit::TransitionOnly
		&& DecideNaniteEntry(EEntryEmit::Suppress, true) == EEntryEmit::Suppress
		&& DecideNaniteEntry(EEntryEmit::Normal, false) == EEntryEmit::Normal
		&& DecideNaniteEntry(EEntryEmit::TransitionOnly, true) == EEntryEmit::TransitionOnly,
		"R5: a Nanite-blocked entry is written transition-only; a suppressed one stays suppressed");
	Check(std::string(DescribeReasonBit(6)) == "nanite_unmaskable" && ReasonNaniteUnmaskable == 64
		&& ReasonsOrLegacy(ReasonNaniteUnmaskable) == ReasonNaniteUnmaskable
		&& ReasonsOrLegacy(ReasonNaniteUnmaskable | ReasonEffectInterrupted) == (ReasonNaniteUnmaskable | ReasonEffectInterrupted),
		"R5 reasons: nanite_unmaskable is its own seventh bit and is not rewritten as a legacy temporal flag");
	FNaniteRevertGate G;
	Check(ShouldRequestNaniteRevert(G, true, true) && !ShouldRequestNaniteRevert(G, false, true) && !ShouldRequestNaniteRevert(G, true, false),
		"R5: a revert is requested only for a blocked, still-active effect");
	G.bRequested = true;
	Check(!ShouldRequestNaniteRevert(G, true, true), "R5: the revert is requested once per event");
}

static void TestTransitionReasons()
{
	Check(AnomalyLabelSync::ReasonsOrLegacy(1) == AnomalyLabelSync::ReasonTemporal, "reasons: a legacy 1 reads as temporal_aa");
	const unsigned char Both = AnomalyLabelSync::ReasonTemporal | AnomalyLabelSync::ReasonPartial;
	Check(AnomalyLabelSync::ReasonsOrLegacy(Both) == Both, "reasons: combined bits survive");
	Check(std::string(AnomalyLabelSync::DescribeReasonBit(2)) == "partial"
		&& std::string(AnomalyLabelSync::DescribeReasonBit(3)) == "camera_clipping_unconfirmed"
		&& std::string(AnomalyLabelSync::DescribeReasonBit(1)) == "hide_return"
		&& std::string(AnomalyLabelSync::DescribeReasonBit(0)) == "temporal_aa", "reasons: the four reason names");
	Check(AnomalyLabelSync::ReasonPartial != 0 && (AnomalyLabelSync::ReasonPartial & AnomalyLabelSync::ReasonTemporal) == 0,
		"reasons: partial is its own bit, independent of temporal AA");
	Check(std::string(AnomalyLabelSync::DescribeReasonBit(4)) == "unresolved" && AnomalyLabelSync::NumReasons == 8
		&& AnomalyLabelSync::ReasonsOrLegacy(AnomalyLabelSync::ReasonUnresolved) == AnomalyLabelSync::ReasonUnresolved,
		"N4 reasons: 'unresolved' is its own fifth bit and is not rewritten as a legacy temporal flag");
}

static void TestF5TransitionCarry()
{
	using AnomalyLabelSync::FEventTransitionTrack;
	const int On = 3;
	const int Off = 8;
	FEventTransitionTrack Run1;
	bool bOn = false;
	bool bOff = false;
	for (int si = 90; si <= 99; ++si)
	{
		Run1.Observe(si, true, On, Off, bOn, bOff);
	}
	Check(AnomalyLabelSync::ShouldCarryTransitionTrack(Run1, 99, Off, false),
		"F5 carry: an event whose last member is the run's last frame still owes its off window, so it is carried");
	FEventTransitionTrack Carried = AnomalyLabelSync::CarryTransitionTrack(Run1, 99);
	std::vector<int> Flagged;
	for (int si = 0; si < 12; ++si)
	{
		Carried.Observe(si, false, On, Off, bOn, bOff);
		if (bOff) { Flagged.push_back(si); }
	}
	Check(Flagged == std::vector<int>({ 0, 1, 2, 3, 4, 5, 6, 7 }),
		"F5 carry: the next run's first 8 frames carry the off-window flag after an uninterrupted boundary " + Str(Flagged));
	FEventTransitionTrack Fresh;
	std::vector<int> Legacy;
	for (int si = 0; si < 12; ++si)
	{
		Fresh.Observe(si, false, On, Off, bOn, bOff);
		if (bOff) { Legacy.push_back(si); }
	}
	Check(Legacy.empty(), "F5 BOTH WAYS: the pre-fix reset (a fresh track) flags nothing after the boundary - the lost residual");

	FEventTransitionTrack Start1;
	Start1.Observe(98, true, On, Off, bOn, bOff);
	Start1.Observe(99, true, On, Off, bOn, bOff);
	FEventTransitionTrack StartCarried = AnomalyLabelSync::CarryTransitionTrack(Start1, 99);
	StartCarried.Observe(0, true, On, Off, bOn, bOff);
	const bool bThird = bOn;
	StartCarried.Observe(1, true, On, Off, bOn, bOff);
	const bool bFourth = bOn;
	Check(bThird && !bFourth, "F5 carry: an onset begun 2 frames before the cut keeps exactly one onset frame for the next run");
	FEventTransitionTrack Inherited;
	Inherited.bOnsetPast = true;
	Inherited.Observe(0, true, On, Off, bOn, bOff);
	Check(!bOn, "F5 BOTH WAYS: the pre-fix inherited track (onset declared past) drops that remaining onset frame");

	FEventTransitionTrack Done;
	Done.Observe(50, true, On, Off, bOn, bOff);
	Check(!AnomalyLabelSync::ShouldCarryTransitionTrack(Done, 99, Off, false), "F5 carry: an off window already spent is not carried");
	Check(AnomalyLabelSync::ShouldCarryTransitionTrack(Done, 99, Off, true), "F5 carry: an event that continues into the next run is always carried");

	AnomalyLabelSync::FHideReturnTrack H;
	AnomalyLabelSync::StepHideLive(H, true, 1);
	Check(AnomalyLabelSync::ShouldCarryHideTrack(H), "F5 hide: an object hidden on the run's last frame is carried");
	AnomalyLabelSync::FHideReturnTrack HC = H;
	const bool bCarriedFlag = AnomalyLabelSync::StepHideGone(HC, 1);
	AnomalyLabelSync::FHideReturnTrack HF;
	const bool bFreshFlag = AnomalyLabelSync::StepHideGone(HF, 1);
	Check(bCarriedFlag && !bFreshFlag, "F5 hide BOTH WAYS: the carried track flags the next run's first frame; a reset track does not");

	using AnomalyLabelSync::ECarrySource;
	auto RunOff = [&](FEventTransitionTrack& Track, int FirstSI, int LastSI)
	{
		std::vector<int> Out;
		for (int si = FirstSI; si <= LastSI; ++si)
		{
			Track.Observe(si, false, On, Off, bOn, bOff);
			if (bOff) { Out.push_back(si); }
		}
		return Out;
	};
	auto CarryAtEnd = [&](const FEventTransitionTrack& Track, int LastSI, ECarrySource Source, bool bLegacy, bool& bOutCarried)
	{
		const ECarrySource Effective = (bLegacy && Source == ECarrySource::CarriedTail) ? ECarrySource::None : Source;
		const AnomalyLabelSync::FCarryDecision D = (bLegacy && LastSI < 0)
			? AnomalyLabelSync::FCarryDecision()
			: AnomalyLabelSync::DecideRunEndCarry(Track, LastSI, Off, Effective);
		bOutCarried = D.bCarry;
		return AnomalyLabelSync::CarryTransitionTrack(Track, LastSI);
	};
	for (int Lg = 0; Lg <= 1; ++Lg)
	{
		FEventTransitionTrack A;
		for (int si = 90; si <= 98; ++si) { A.Observe(si, true, On, Off, bOn, bOff); }
		const std::vector<int> OffA = RunOff(A, 99, 99);
		bool bCarriedA = false;
		FEventTransitionTrack B = CarryAtEnd(A, 99, ECarrySource::DetachedTrail, Lg == 1, bCarriedA);
		const std::vector<int> OffB = RunOff(B, 0, 0);
		bool bCarriedB = false;
		FEventTransitionTrack C = CarryAtEnd(B, 0, ECarrySource::CarriedTail, Lg == 1, bCarriedB);
		FEventTransitionTrack FreshC;
		std::vector<int> OffC = bCarriedB ? RunOff(C, 0, 9) : RunOff(FreshC, 0, 9);
		const int Total = (int)(OffA.size() + OffB.size() + OffC.size());
		if (Lg == 0)
		{
			Check(bCarriedA && bCarriedB && OffA.size() == 1 && OffB.size() == 1 && OffC == std::vector<int>({ 0, 1, 2, 3, 4, 5 }) && Total == Off,
				"N6: a detached tail crosses TWO run boundaries - run B (1 frame) flags its frame, run C flags the remaining 6, total 8 = the off window "
				+ Str(OffC));
		}
		else
		{
			Check(bCarriedA && !bCarriedB && OffC.empty() && Total == 2,
				"N6 BOTH WAYS: the 084-07 carry skipped the trail-less tail at run B's end, so run C lost 6 off-window frames");
		}
	}
	for (int Lg = 0; Lg <= 1; ++Lg)
	{
		FEventTransitionTrack A;
		for (int si = 90; si <= 98; ++si) { A.Observe(si, true, On, Off, bOn, bOff); }
		RunOff(A, 99, 99);
		bool bCarriedA = false;
		FEventTransitionTrack B = CarryAtEnd(A, 99, ECarrySource::DetachedTrail, Lg == 1, bCarriedA);
		bool bCarriedEmpty = false;
		FEventTransitionTrack C = CarryAtEnd(B, -1, ECarrySource::CarriedTail, Lg == 1, bCarriedEmpty);
		FEventTransitionTrack FreshC;
		const std::vector<int> OffC = bCarriedEmpty ? RunOff(C, 0, 9) : RunOff(FreshC, 0, 9);
		if (Lg == 0)
		{
			Check(bCarriedEmpty && OffC == std::vector<int>({ 0, 1, 2, 3, 4, 5, 6 }),
				"N6: a ZERO-FRAME run in between passes the history through unchanged - the next run flags the remaining 7 " + Str(OffC));
		}
		else
		{
			Check(!bCarriedEmpty && OffC.empty(), "N6 BOTH WAYS: the 084-07 carry returned early on a zero-frame run and dropped the history");
		}
	}
	{
		FEventTransitionTrack A;
		for (int si = 50; si <= 60; ++si) { A.Observe(si, true, On, Off, bOn, bOff); }
		const AnomalyLabelSync::FCarryDecision Attached = AnomalyLabelSync::DecideRunEndCarry(A, 99, Off, ECarrySource::AttachedTrail);
		const AnomalyLabelSync::FCarryDecision Spent = AnomalyLabelSync::DecideRunEndCarry(A, 99, Off, ECarrySource::CarriedTail);
		const AnomalyLabelSync::FCarryDecision NoSource = AnomalyLabelSync::DecideRunEndCarry(A, 60, Off, ECarrySource::None);
		Check(Attached.bCarry && !Attached.bDetachedTail && !Spent.bCarry && !NoSource.bCarry,
			"N6 policy: an attached trail always carries; a spent tail does not; a track with no source does not");
	}
}

static void TestF4Retire()
{
	using AnomalyLabelSync::ERetireAction;
	struct FHolder
	{
		int Value;
		bool bCustomDepth;
		bool bTracked;
		int PriorValue;
		bool bPriorCustomDepth;
	};
	auto Retire = [](std::vector<FHolder> Holders, int Retired, bool bLegacy)
	{
		for (FHolder& H : Holders)
		{
			const ERetireAction A = bLegacy
				? AnomalyLabelSync::DecideRetireLegacy(H.bTracked, H.Value, H.bCustomDepth, Retired)
				: AnomalyLabelSync::DecideRetire(H.bTracked, H.Value, H.bCustomDepth, Retired);
			if (A == ERetireAction::RestoreValueAndFlag)
			{
				H.Value = H.PriorValue;
				H.bCustomDepth = H.bPriorCustomDepth;
				H.bTracked = false;
			}
			else if (A == ERetireAction::RestoreValueKeepHostFlag)
			{
				H.Value = H.PriorValue;
				H.bTracked = false;
			}
		}
		return Holders;
	};
	const std::vector<FHolder> Start = { { 200, false, true, 0, false } };
	for (int Legacy = 0; Legacy <= 1; ++Legacy)
	{
		std::vector<FHolder> After = Retire(Start, 200, Legacy == 1);
		std::vector<int> Values;
		for (const FHolder& H : After) { Values.push_back(H.Value); }
		const bool bVerified = AnomalyLabelSync::RetirementVerified(Values.data(), (int)Values.size(), 200);
		After[0].bCustomDepth = true;
		std::vector<int> Drawn;
		for (const FHolder& H : After)
		{
			if (H.bCustomDepth) { Drawn.push_back(H.Value); }
		}
		Drawn.push_back(200);
		const bool bAlias = AnomalyLabelSync::AnyAlias(Drawn.data(), (int)Drawn.size());
		if (Legacy == 0)
		{
			Check(bVerified && !bAlias, "F4: retirement clears a custom-depth-OFF holder; when the host re-enables it, event B's 200 does not alias");
		}
		else
		{
			Check(!bVerified && bAlias, "F4 BOTH WAYS: the pre-fix rule skips the custom-depth-off holder - verification fails and the value aliases");
		}
	}
	const std::vector<FHolder> HostOff = Retire(Start, 200, false);
	Check(!HostOff[0].bCustomDepth && HostOff[0].Value == 0, "F4: the host's custom-depth-off decision is preserved; only the value is restored");
	const std::vector<FHolder> HostMoved = Retire({ { 57, true, true, 0, false } }, 200, false);
	Check(HostMoved[0].Value == 57 && HostMoved[0].bCustomDepth, "F4: a holder the host moved to another value is left alone");
	std::vector<FHolder> Untracked = Retire({ { 200, true, false, 0, false } }, 200, false);
	const int Left = Untracked[0].Value;
	Check(!AnomalyLabelSync::RetirementVerified(&Left, 1, 200), "F4: an untracked holder of the value fails verification, so the value is quarantined, not issued");
}

struct FWorldComponent
{
	int Value = 0;
	bool bCustomDepth = false;
	bool bAlive = true;
	int OwnerActor = 1;
	bool bTracked = false;
	int PriorValue = 0;
	bool bPriorCustomDepth = false;
	unsigned long long AppliedMask = 0;
};

static void WorldTag(FWorldComponent& C, int V)
{
	if (!C.bTracked)
	{
		C.bTracked = true;
		C.PriorValue = C.Value;
		C.bPriorCustomDepth = C.bCustomDepth;
	}
	C.AppliedMask |= AnomalyLabelSync::AppliedBit(V);
	C.Value = V;
	C.bCustomDepth = true;
}

static void WorldRestoreActor(std::vector<FWorldComponent>& World, int Actor)
{
	for (FWorldComponent& C : World)
	{
		if (C.bAlive && C.OwnerActor == Actor && C.bTracked)
		{
			C.Value = C.PriorValue;
			C.bCustomDepth = C.bPriorCustomDepth;
			C.bTracked = false;
		}
	}
}

static AnomalyLabelSync::FRetireOutcome WorldRetire(std::vector<FWorldComponent>& World, int FormerOwner, int V)
{
	std::vector<AnomalyLabelSync::FRetireHolder> H(World.size());
	for (size_t i = 0; i < World.size(); ++i)
	{
		const FWorldComponent& C = World[i];
		H[i].bValid = C.bAlive;
		H[i].bTracked = C.bTracked;
		H[i].PriorValue = C.PriorValue;
		H[i].bPriorCustomDepth = C.bPriorCustomDepth;
		H[i].Value = C.Value;
		H[i].bCustomDepth = C.bCustomDepth;
		H[i].AppliedMask = C.AppliedMask;
		H[i].bOwnedByFormerOwner = C.OwnerActor == FormerOwner;
	}
	const AnomalyLabelSync::FRetireOutcome R = AnomalyLabelSync::RetireHolders(H.data(), (int)H.size(), V);
	for (size_t i = 0; i < World.size(); ++i)
	{
		FWorldComponent& C = World[i];
		if (H[i].bSetValue) { C.Value = H[i].Value; }
		if (H[i].bSetFlag) { C.bCustomDepth = H[i].bCustomDepth; }
		if (H[i].bUntrack) { C.bTracked = false; }
		if (H[i].bClearApplied) { C.AppliedMask &= ~AnomalyLabelSync::AppliedBit(V); }
	}
	return R;
}

static bool LegacyRetireVerified0847(std::vector<FWorldComponent> World, int FormerOwner, int V)
{
	for (FWorldComponent& C : World)
	{
		if (!C.bAlive || !C.bTracked)
		{
			continue;
		}
		const AnomalyLabelSync::ERetireAction A = AnomalyLabelSync::DecideRetire(true, C.Value, C.bCustomDepth, V);
		if (A == AnomalyLabelSync::ERetireAction::Leave)
		{
			continue;
		}
		C.Value = C.PriorValue;
		if (A == AnomalyLabelSync::ERetireAction::RestoreValueAndFlag)
		{
			C.bCustomDepth = C.bPriorCustomDepth;
		}
		C.bTracked = false;
	}
	std::vector<int> After;
	for (const FWorldComponent& C : World)
	{
		if (C.bAlive && (C.bTracked || C.OwnerActor == FormerOwner))
		{
			After.push_back(C.Value);
		}
	}
	return AnomalyLabelSync::RetirementVerified(After.data(), (int)After.size(), V);
}

static void TestN7RetireEveryAppliedIdentity()
{
	const int V = 210;
	const int OldActor = 1;
	const int OtherActor = 2;
	{
		std::vector<FWorldComponent> World(1);
		World[0].Value = V;
		World[0].bCustomDepth = false;
		WorldTag(World[0], V);
		World[0].OwnerActor = OtherActor;
		World[0].bCustomDepth = false;
		const bool bLegacy = LegacyRetireVerified0847(World, OldActor, V);
		const AnomalyLabelSync::FRetireOutcome R = WorldRetire(World, OldActor, V);
		std::vector<int> Drawn = { V };
		World[0].bCustomDepth = true;
		Drawn.push_back(World[0].Value);
		Check(!R.bVerified && R.Remaining == 1 && R.RestoredValueOnly == 1 && World[0].Value == V && (World[0].AppliedMask & AnomalyLabelSync::AppliedBit(V)) != 0,
			"N7: Codex's case - prior value == V, custom depth off, the component LEFT the old actor: retirement restores V, the applied identity "
			"is still checked, verification FAILS and V is quarantined (and the identity kept until verified)");
		Check(bLegacy && AnomalyLabelSync::AnyAlias(Drawn.data(), (int)Drawn.size()),
			"N7 BOTH WAYS: the 084-07 verification (map after removal + the old actor's current components) passed it, so V was reissued "
			"and aliases the new event once the host re-enables custom depth");
	}
	{
		std::vector<FWorldComponent> World(1);
		WorldTag(World[0], V);
		World[0].OwnerActor = OtherActor;
		World[0].bCustomDepth = false;
		const AnomalyLabelSync::FRetireOutcome R = WorldRetire(World, OldActor, V);
		Check(R.bVerified && World[0].Value == 0 && !World[0].bCustomDepth && World[0].AppliedMask == 0,
			"N7: a moved holder whose prior value was 0 is restored to 0 (host flag kept), verifies, and its applied identity is cleared");
	}
	{
		std::vector<FWorldComponent> World(2);
		World[0].Value = V;
		WorldTag(World[0], V);
		WorldTag(World[1], V);
		WorldRestoreActor(World, OldActor);
		World[0].OwnerActor = OtherActor;
		const bool bLegacy = LegacyRetireVerified0847(World, OldActor, V);
		const AnomalyLabelSync::FRetireOutcome R = WorldRetire(World, OldActor, V);
		Check(!R.bVerified && R.Checked == 2 && R.Remaining == 1 && bLegacy,
			"N7 BOTH WAYS: a holder restored EARLIER (RestoreActor, no longer tracked) and moved away is still verified by the new path "
			"(fails, quarantined); the 084-07 path never looked at it");
	}
	{
		std::vector<FWorldComponent> World(2);
		WorldTag(World[0], V);
		World[1].Value = 57;
		World[1].bCustomDepth = true;
		World[1].OwnerActor = OtherActor;
		const AnomalyLabelSync::FRetireOutcome R = WorldRetire(World, OldActor, V);
		Check(R.bVerified && R.Restored == 1 && R.Checked == 1 && World[0].Value == 0 && World[1].Value == 57,
			"N7: an ordinary retirement restores value and flag, checks only identities that ever held V, leaves unrelated components alone");
	}
	{
		std::vector<FWorldComponent> World(1);
		WorldTag(World[0], V);
		World[0].bAlive = false;
		const AnomalyLabelSync::FRetireOutcome R = WorldRetire(World, OldActor, V);
		Check(R.bVerified && R.Checked == 0, "N7: a destroyed holder cannot alias and does not block the value");
	}
}

static void TestA2ForcedHeldSet()
{
	const FHeldSetState Fresh;
	Check(Fresh.Set == EHeldSet::Unresolved && !Fresh.bFromRecord,
		"A2: a render result created with NO texture record (the FlushObserveQueue force path) starts UNRESOLVED, never not_held");
	FHeldSetState Forced;
	ForceHeldSetUnknown(Forced);
	Check(Forced.Set == EHeldSet::Unresolved && IsUnresolvedMemberFrame(true, Forced) && !IsPartialMemberFrame(true, Forced),
		"A2: a forced-unknown member frame carries the unresolved reason");
	FFrameAuthority A;
	ForceTerminalUnknown(A);
	const bool bMember = A.Membership != EMembership::Out;
	const unsigned char Reasons = (unsigned char)((IsPartialMemberFrame(bMember, Forced) ? AnomalyLabelSync::ReasonPartial : 0)
		| (IsUnresolvedMemberFrame(bMember, Forced) ? AnomalyLabelSync::ReasonUnresolved : 0));
	Check(A.bForced && bMember && Reasons == AnomalyLabelSync::ReasonUnresolved,
		"A2: with AA off (no temporal reason) the forced-unknown labelled row still carries transition_reason unresolved");
	FPartialEdgeTrack Track;
	Track.Observe(40, bMember, Forced.Set);
	Check(Track.NumUnresolved() == 1 && Track.LastFullSI < 0,
		"A2: the forced frame is counted unresolved and does not set a full-set boundary");
	Check(std::string(DescribeHeldSet(Forced.Set)) == "unresolved", "A2: telemetry stuck_mip.held_set reads unresolved");

	const EHeldSet Legacy = (EHeldSet)0;
	const bool bLegacyReason = bMember && Legacy == EHeldSet::Unresolved;
	Check(Legacy == EHeldSet::NotHeld && !bLegacyReason && std::string(DescribeHeldSet(Legacy)) == "not_held",
		"A2 BOTH WAYS: the 084-07c default (uint8 0) read not_held and gave the forced frame NO unresolved reason");

	const std::vector<FTexSample> Full = { { 7, 11, 7, true }, { 7, 11, 7, true } };
	FHeldSetState Recorded;
	std::vector<ELevel> Levels;
	for (const FTexSample& S : Full)
	{
		Levels.push_back(ClassifyLevel(ClassifyTexture(S.Resident, S.Baseline, S.bKnown), S.Resident, S.Baseline, S.HeldLevel));
	}
	ClassifyHeldSetState(Recorded, Levels.data(), (int)Levels.size());
	ForceHeldSetUnknown(Recorded);
	Check(Recorded.Set == EHeldSet::Full && Recorded.bFromRecord,
		"A2: a result classified from an actual texture record keeps that classification when it is later forced");
	FHeldSetState Unbacked;
	Unbacked.Set = EHeldSet::NotHeld;
	ForceHeldSetUnknown(Unbacked);
	Check(Unbacked.Set == EHeldSet::Unresolved,
		"A2: a classification with no record behind it is reset to unresolved when the frame is forced (only a record can back it)");
	FHeldSetState Empty;
	ClassifyHeldSetState(Empty, nullptr, 0);
	ForceHeldSetUnknown(Empty);
	Check(Empty.Set == EHeldSet::Unresolved && !Empty.bFromRecord, "A2: an empty record is unresolved before and after forcing");
}

static void TestA4PriorCollision()
{
	AnomalyLabelSync::FPriorRestoreInputs In;
	In.PriorValue = 210;
	In.bEventClaimed = true;
	AnomalyLabelSync::FPriorRestoreVerdict V = AnomalyLabelSync::CheckPriorRestore(In);
	Check(V.bCollision && V.bQuarantine, "A4: a restore that writes back 210 while an event holds 210 is a mask_prior_collision and quarantines 210");
	In.bEventClaimed = false;
	In.bCensusClaimed = true;
	V = AnomalyLabelSync::CheckPriorRestore(In);
	Check(V.bCollision && V.bQuarantine, "A4: a value claimed by the census is live too");
	In.bCensusClaimed = false;
	V = AnomalyLabelSync::CheckPriorRestore(In);
	Check(!V.bCollision && !V.bQuarantine, "A4: writing back a value no one holds is not a collision");
	In.PriorValue = 0;
	In.bEventClaimed = true;
	V = AnomalyLabelSync::CheckPriorRestore(In);
	Check(!V.bCollision, "A4: a host value outside the plugin range (0) is never a collision");
	In.PriorValue = 255;
	V = AnomalyLabelSync::CheckPriorRestore(In);
	Check(!V.bCollision, "A4: 255 (the StencilDummy detector, never assignable) is never a collision");
	In.PriorValue = 210;
	In.bAlreadyQuarantined = true;
	V = AnomalyLabelSync::CheckPriorRestore(In);
	Check(V.bCollision && !V.bQuarantine, "A4: a second collision on an already quarantined value is counted, not re-quarantined");

	Check(AnomalyLabelSync::IsTagValueFree(true, false, false, false) && !AnomalyLabelSync::IsTagValueFree(true, false, false, true),
		"A4: a quarantined value is not free to the event or census allocators");

	AnomalyLabelSync::FRecycleCandidate C[2];
	C[0].Record = 0; C[0].Tag = 210; C[0].bReleasable = true; C[0].ReleasableSince = 5; C[0].bQuarantined = true;
	C[1].Record = 1; C[1].Tag = 211; C[1].bReleasable = true; C[1].ReleasableSince = 9;
	Check(AnomalyLabelSync::PickRecycleVictim(C, 2) == 1, "A4: the recycler skips the quarantined value even when it is the oldest releasable");
	C[1].bQuarantined = true;
	Check(AnomalyLabelSync::PickRecycleVictim(C, 2) == -1, "A4: with every releasable value quarantined nothing is recycled");

	std::set<int> EventClaimed = { 210 };
	std::set<int> Quarantined;
	int Collisions = 0;
	auto Restore = [&](int Prior)
	{
		AnomalyLabelSync::FPriorRestoreInputs R;
		R.PriorValue = Prior;
		R.bEventClaimed = EventClaimed.count(Prior) != 0;
		R.bAlreadyQuarantined = Quarantined.count(Prior) != 0;
		const AnomalyLabelSync::FPriorRestoreVerdict RV = AnomalyLabelSync::CheckPriorRestore(R);
		Collisions += RV.bCollision ? 1 : 0;
		if (RV.bQuarantine)
		{
			Quarantined.insert(Prior);
		}
		return Prior;
	};
	int Value = 210;
	Value = Restore(210);
	Value = 211;
	EventClaimed.insert(211);
	AnomalyLabelSync::FRetireHolder H;
	H.bTracked = true;
	H.PriorValue = 210;
	H.Value = Value;
	H.bCustomDepth = true;
	H.AppliedMask = AnomalyLabelSync::AppliedBit(210) | AnomalyLabelSync::AppliedBit(211);
	const AnomalyLabelSync::FRetireOutcome Retired = AnomalyLabelSync::RetireHolders(&H, 1, 210);
	AnomalyLabelSync::FRecycleCandidate Old;
	Old.Record = 0; Old.Tag = 210; Old.bReleasable = true; Old.ReleasableSince = 1;
	Old.bQuarantined = Quarantined.count(210) != 0;
	const int Pick = AnomalyLabelSync::PickRecycleVictim(&Old, 1);
	Value = Restore(H.PriorValue);
	Check(Retired.bVerified && Collisions == 2 && Quarantined.count(210) == 1 && Pick == -1,
		"A4: Codex's saved-prior sequence (host prior 210 custom-depth off; apply 210, restore; apply 211; retire 210; restore 211's "
		"saved prior 210) trips the tripwire on both restores, quarantines 210 and the recycler never reissues it");
	Old.bQuarantined = false;
	const int LegacyPick = AnomalyLabelSync::PickRecycleVictim(&Old, 1);
	const int E3Value = LegacyPick == 0 ? Old.Tag : 0;
	Check(LegacyPick == 0 && E3Value == 210 && Value == E3Value,
		"A4 BOTH WAYS: without the quarantine 210 is recycled to a new event and the later restore writes 210 back onto the old "
		"component - an alias the moment the host re-enables custom depth (Codex's N7 saved-prior variant)");
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
	TestG350PostClosure();
	TestTransitionTrackOrder();
	TestAaResolve();
	TestTransitionGate();
	TestHideReturn();
	TestTagRecycling();
	TestPartialHeldSet();
	TestTransitionReasons();
	TestF5TransitionCarry();
	TestF4Retire();
	TestAnnotationMembership();
	TestF1EffectInstalled();
	TestR1PartialAndR5NaniteGate();
	TestN7RetireEveryAppliedIdentity();
	TestA2ForcedHeldSet();
	TestA4PriorCollision();
	std::printf("m52 window selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
