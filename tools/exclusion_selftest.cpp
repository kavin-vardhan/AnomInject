#include "../Source/AnomalyInjector/Public/AnomalyExclusion.h"
#include "../Source/AnomalyInjector/Private/Anomalies/TexCorruptPure.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#ifndef EXCL_MUTANT
#define EXCL_MUTANT 0
#endif

using namespace AnomalyExclusion;
using AnomalyLabelSync::FEventTransitionTrack;
using AnomalyStuckMipWindow::EVerdict;
using AnomalyStuckMipWindow::FTrail;

static int GChecks = 0;
static int GFailures = 0;

static void Check(bool bOk, const std::string& What)
{
	++GChecks;
	if (!bOk)
	{
		++GFailures;
		std::printf("FAIL %s\n", What.c_str());
	}
}

static ETrail Classify(const FTrailFacts& F)
{
	if constexpr (EXCL_MUTANT == 3)
	{
		FTrailFacts G = F;
		G.bLabelTail = false;
		return ClassifyTrail(G);
	}
	else if constexpr (EXCL_MUTANT == 4)
	{
		if (!F.bOpen)
		{
			return F.bLabelTail ? ETrail::LabelTail : ETrail::None;
		}
		if (F.bClosed)
		{
			return ETrail::Closed;
		}
		return F.bUnresolved ? ETrail::Unresolved : ETrail::Gating;
	}
	else
	{
		return ClassifyTrail(F);
	}
}

static ETrail ClassifyT(const FTrail& T, bool bLabelTail)
{
	return Classify(FactsOf(T, bLabelTail));
}

static EState DeriveM52(const FM52Signals& S)
{
	if constexpr (EXCL_MUTANT == 1)
	{
		return S.bFireLive ? EState::FireLive : EState::Idle;
	}
	else if constexpr (EXCL_MUTANT == 5)
	{
		FM52Signals G = S;
		G.bRestoringPastTimeout = false;
		return M52State(G);
	}
	else
	{
		return M52State(S);
	}
}

static EState DeriveM53(const FM53Signals& S)
{
	if constexpr (EXCL_MUTANT == 2)
	{
		return S.bFireLive ? EState::FireLive : EState::Idle;
	}
	else if constexpr (EXCL_MUTANT == 9)
	{
		FM53Signals G = S;
		G.Trail = ETrail::None;
		return M53State(G);
	}
	else
	{
		return M53State(S);
	}
}

static ERecordAction RecordAction(bool bApplied, bool bActiveAfter)
{
	if constexpr (EXCL_MUTANT == 6)
	{
		return bApplied ? ERecordAction::Replace : ERecordAction::Drop;
	}
	else
	{
		return RecordAfterApply(bApplied, bActiveAfter);
	}
}

static bool FireLive(bool bHasRecord, bool bActive)
{
	if constexpr (EXCL_MUTANT == 7)
	{
		return bHasRecord && bActive;
	}
	else
	{
		(void)bHasRecord;
		return IsFireLiveForExclusion(bActive);
	}
}

static bool RetainedOwes(EFamily Partner)
{
	if constexpr (EXCL_MUTANT == 10)
	{
		(void)Partner;
		return false;
	}
	else
	{
		return RetainedLiveFireOwesFrames(Partner);
	}
}

static bool PendingOwes(EFamily Partner, bool bRenderTruthFire)
{
	if constexpr (EXCL_MUTANT == 11)
	{
		(void)Partner;
		return bRenderTruthFire;
	}
	else
	{
		return PendingFireOwesFrames(Partner, bRenderTruthFire);
	}
}

static ETrail M53ProviderTrail(bool bRetainedFire, bool bPendingSnapshot)
{
	ETrail Best = ETrail::None;
	if (bRetainedFire && RetainedOwes(EFamily::M53))
	{
		Best = CombineTrail(Best, EmitterTrail(true));
	}
	if (bPendingSnapshot && PendingOwes(EFamily::M53, false))
	{
		Best = CombineTrail(Best, EmitterTrail(true));
	}
	return Best;
}

static bool CoEntry(const std::vector<FFrameEntry>& Entries)
{
	if constexpr (EXCL_MUTANT == 8)
	{
		bool bM52 = false;
		bool bM53 = false;
		for (const FFrameEntry& X : Entries)
		{
			bM52 = bM52 || IsM52CoEntry(X);
			bM53 = bM53 || (X.Family == EFamily::M53 && X.bLabelled);
		}
		return bM52 && bM53;
	}
	else
	{
		return IsCoEntryFrame(Entries.data(), (int)Entries.size());
	}
}

struct FCountingStream
{
	unsigned Seed = 4242u;
	unsigned Steps = 0;

	float GetFraction()
	{
		Seed = Seed * 196314165u + 907633515u;
		++Steps;
		const unsigned Bits = 0x3F800000u | (Seed >> 9);
		float F = 0.0f;
		std::memcpy(&F, &Bits, sizeof(F));
		return F - 1.0f;
	}

	int RandHelper(int A)
	{
		return A > 0 ? (int)(GetFraction() * (float)A) : 0;
	}

	double FRandRange(double InMin, double InMax)
	{
		return InMin + (InMax - InMin) * (double)GetFraction();
	}
};

static FTrail GatingTrail()
{
	FTrail T;
	T.Open(100, 0, 120);
	return T;
}

static FTrail ClosedDetachedTrail()
{
	FTrail T;
	T.Open(100, 0, 120);
	for (int si = 100; si <= 103; ++si)
	{
		T.NoteArmed(si);
		T.Receive(si, EVerdict::NotHeld, true);
	}
	T.TryDetach(104, false);
	return T;
}

static FTrail ReopenedAfterDetachTrail()
{
	FTrail T = ClosedDetachedTrail();
	T.Receive(104, EVerdict::Held, true);
	return T;
}

static FTrail UnresolvedTrail()
{
	FTrail T;
	T.Open(100, 0, 5);
	if (T.ShouldTimeout(105))
	{
		T.bUnresolved = true;
	}
	return T;
}

static FTrail AdoptedTrail(const FTrail& Carried)
{
	FTrail T = Carried;
	T.RebaseForNewRun(0);
	return T;
}

static FTrail ClosedWithReceiptOutstanding()
{
	FTrail P;
	P.Open(500, 0, 120);
	for (int si = 500; si <= 503; ++si)
	{
		P.NoteArmed(si);
	}
	P.Receive(500, EVerdict::NotHeld, true);
	P.Receive(501, EVerdict::NotHeld, true);
	P.NoteArmed(504);
	P.Receive(502, EVerdict::NotHeld, true);
	P.Receive(503, EVerdict::NotHeld, true);
	return P;
}

static FM52Signals M52Provider(ETrail Trail, bool bRestoringSet)
{
	FM52Signals S;
	S.bProvider = true;
	S.Trail = Trail;
	S.bRestoringSet = bRestoringSet;
	return S;
}

static FVerdict Row(const char* Name, EFamily Candidate, const FM52Signals& M52, const FM53Signals& M53,
	EState ExpectPartnerState, bool bExpectExcluded, bool bExpectAdmittedAfterUnresolved)
{
	const EState S52 = DeriveM52(M52);
	const EState S53 = DeriveM53(M53);
	const FVerdict V = Evaluate(Candidate, S52, S53);
	const std::string Base = std::string(Name) + " ";
	Check(V.PartnerState == ExpectPartnerState, Base + "state expected=" + DescribeState(ExpectPartnerState) + " got="
		+ DescribeState(V.PartnerState));
	Check(V.bExcluded == bExpectExcluded, Base + (bExpectExcluded ? "must be EXCLUDED" : "must be ADMITTED"));
	Check(V.bAdmittedAfterUnresolved == bExpectAdmittedAfterUnresolved,
		Base + (bExpectAdmittedAfterUnresolved ? "must count admitted_after_unresolved" : "must not count admitted_after_unresolved"));
	return V;
}

static FM53Signals M53Revert(long long RevertFrame, long long Now)
{
	FM53Signals S;
	S.bRevertSettling = IsRevertSettlingAt(RevertFrame, Now);
	return S;
}

static unsigned DrawsForPool(const EFamily* Pool, int Num, EState S52, EState S53, int& OutAdmitted)
{
	bool Admitted[8] = {};
	OutAdmitted = FilterAdmitted(Pool, Num, S52, S53, Admitted);
	FCountingStream Stream;
	const unsigned SeedBefore = Stream.Seed;
	auto CandidateCount = [](int) { return 5; };
	auto ModeCount = [](int) { return 2; };
	TexCorruptPure::DrawAttempt(Stream, OutAdmitted, CandidateCount, ModeCount, 3.0f, 6.0f);
	if (OutAdmitted == 0)
	{
		Check(Stream.Seed == SeedBefore, "all_excluded seed_after == seed_before");
	}
	return Stream.Steps;
}

static bool ResolveDetachedFrame(FEventTransitionTrack& Track, int SI, int OnFrames, int OffFrames)
{
	bool bOn = false;
	bool bOff = false;
	Track.Observe(SI, false, OnFrames, OffFrames, bOn, bOff);
	if (bOff)
	{
		return true;
	}
	if (Track.OffWindowPassed(SI, OffFrames))
	{
		Track.bOffDone = true;
	}
	return false;
}

static bool M53AdmittedBehind(const FTrail& Trail, bool bTrailExists, const FEventTransitionTrack& Track, bool bRenderTruth,
	int OffFrames, bool bPendingSnapshot)
{
	const bool bTail = bPendingSnapshot || TransitionTailOwed(RunEmitsTransitionTail(bRenderTruth, OffFrames), Track);
	FTrailFacts F = bTrailExists ? FactsOf(Trail, bTail) : FTrailFacts();
	F.bLabelTail = bTail;
	const FM52Signals S = M52Provider(Classify(F), false);
	return !Evaluate(EFamily::M53, DeriveM52(S), EState::Idle).bExcluded;
}

static void TestF1TemporalOffTail()
{
	const int On = AnomalyLabelSync::DefaultOnFramesTemporal;
	const int Off = AnomalyLabelSync::DefaultOffFramesTemporal;
	FTrail T;
	T.Open(100, 0, 120);
	FEventTransitionTrack Track;
	std::vector<int> TailFrames;
	for (int si = 100; si <= 105; ++si)
	{
		T.NoteArmed(si);
	}
	const EVerdict Verdicts[] = { EVerdict::Held, EVerdict::Held, EVerdict::NotHeld, EVerdict::NotHeld, EVerdict::NotHeld,
		EVerdict::NotHeld };
	for (int si = 100; si <= 105; ++si)
	{
		T.Receive(si, Verdicts[si - 100], true);
		bool bOn = false;
		bool bOff = false;
		Track.Observe(si, Verdicts[si - 100] == EVerdict::Held, On, Off, bOn, bOff);
		if (bOff)
		{
			TailFrames.push_back(si);
		}
	}
	Check(T.bClosed && !T.bDetached, "F1 setup: the trail closes at its fence 105 and is still attached");
	Check(T.TryDetach(106, false) && T.AllNotedProcessed(), "F1 setup: quiet detach with every noted request processed");
	Check(Track.LastMember() == 101 && !Track.bOffDone, "F1 setup: last labelled frame 101, off tail still owed");

	const FVerdict V = Row("F1 m53 admitted inside m52's TAA off-frames (closed, detached, processed, tail owed)", EFamily::M53,
		M52Provider(ClassifyT(T, TransitionTailOwed(RunEmitsTransitionTail(true, Off), Track)), false), FM53Signals(),
		EState::LabelTail, true, false);
	(void)V;

	int AdmitSI = -1;
	for (int si = 106; si <= 107 + Off; ++si)
	{
		if (AdmitSI < 0 && M53AdmittedBehind(T, true, Track, true, Off, false))
		{
			AdmitSI = si;
		}
		if (!Track.bOffDone && ResolveDetachedFrame(Track, si, On, Off))
		{
			TailFrames.push_back(si);
		}
	}
	bool bCoEntry = false;
	for (int f : TailFrames)
	{
		bCoEntry = bCoEntry || (AdmitSI >= 0 && f >= AdmitSI);
	}
	Check((int)TailFrames.size() == Off && TailFrames.front() == 102 && TailFrames.back() == 101 + Off,
		"F1 sequence: m52 transition entries on 102.." + std::to_string(101 + Off) + " (the " + std::to_string(Off)
		+ " temporal off-frames after 101)");
	Check(AdmitSI == 103 + Off, "F1 sequence: m53 first admitted at si " + std::to_string(103 + Off) + ", after frame "
		+ std::to_string(102 + Off) + " closed the off window (got " + std::to_string(AdmitSI) + ")");
	Check(!bCoEntry, "F1 sequence: no frame carries both an m52 transition entry and an m53 entry");
	Row("F1 tail done releases", EFamily::M53, M52Provider(ClassifyT(T, TransitionTailOwed(true, Track)), false), FM53Signals(),
		EState::Closed, false, false);

	FEventTransitionTrack AaOff;
	for (int si = 100; si <= 101; ++si)
	{
		bool bOn = false;
		bool bOff = false;
		AaOff.Observe(si, true, 0, 0, bOn, bOff);
	}
	Row("F1 AA off: no off tail is owed", EFamily::M53,
		M52Provider(ClassifyT(T, TransitionTailOwed(RunEmitsTransitionTail(true, 0), AaOff)), false), FM53Signals(),
		EState::Closed, false, false);
	Row("F1 non-render-truth run: the detached tail is never emitted, so it is not owed", EFamily::M53,
		M52Provider(ClassifyT(T, TransitionTailOwed(RunEmitsTransitionTail(false, Off), AaOff)), false), FM53Signals(),
		EState::Closed, false, false);
}

static void TestF1CarriedTailOnly()
{
	const int On = AnomalyLabelSync::DefaultOnFramesTemporal;
	const int Off = AnomalyLabelSync::DefaultOffFramesTemporal;
	FEventTransitionTrack Track;
	bool bOn = false;
	bool bOff = false;
	for (int si = 90; si <= 101; ++si)
	{
		Track.Observe(si, si <= 101 && si >= 98, On, Off, bOn, bOff);
	}
	Check(AnomalyLabelSync::ShouldCarryTransitionTrack(Track, 105, Off, false), "F1 carry: run ends at 105 inside the off window");
	FEventTransitionTrack Carried = AnomalyLabelSync::CarryTransitionTrack(Track, 105);
	FTrailFacts NoTrail;
	NoTrail.bLabelTail = TransitionTailOwed(RunEmitsTransitionTail(true, Off), Carried);
	Row("F1 carried tail-only next run (no trail, off tail owed)", EFamily::M53, M52Provider(Classify(NoTrail), false),
		FM53Signals(), EState::LabelTail, true, false);
	std::vector<int> Emitted;
	int AdmitSI = -1;
	const FTrail Unused = FTrail();
	for (int si = 0; si <= Off; ++si)
	{
		if (AdmitSI < 0 && M53AdmittedBehind(Unused, false, Carried, true, Off, false))
		{
			AdmitSI = si;
		}
		if (!Carried.bOffDone && ResolveDetachedFrame(Carried, si, On, Off))
		{
			Emitted.push_back(si);
		}
	}
	bool bCoEntry = false;
	for (int f : Emitted)
	{
		bCoEntry = bCoEntry || (AdmitSI >= 0 && f >= AdmitSI);
	}
	Check((int)Emitted.size() == Off - 4 && Emitted.front() == 0 && Emitted.back() == Off - 5,
		"F1 carry: the next run's frames 0.." + std::to_string(Off - 5) + " carry the carried m52 transition entries (old frames 106.."
		+ std::to_string(101 + Off) + ")");
	Check(AdmitSI == Off - 3, "F1 carry: m53 first admitted at next-run si " + std::to_string(Off - 3) + " (got "
		+ std::to_string(AdmitSI) + ")");
	Check(!bCoEntry, "F1 carry: no next-run frame carries both the carried m52 tail and an m53 entry");
}

static void TestF1PendingSnapshot()
{
	FTrailFacts Pending;
	Pending.bLabelTail = true;
	Row("F1 an unfinalized snapshot still carries the m52 event (no trail)", EFamily::M53, M52Provider(Classify(Pending), false),
		FM53Signals(), EState::LabelTail, true, false);
	FTrail T = ClosedDetachedTrail();
	Row("F1 closed+detached trail but an unfinalized snapshot carries it", EFamily::M53, M52Provider(ClassifyT(T, true), false),
		FM53Signals(), EState::LabelTail, true, false);

	const int On = AnomalyLabelSync::DefaultOnFramesTemporal;
	const int Off = AnomalyLabelSync::DefaultOffFramesTemporal;
	FEventTransitionTrack Track;
	bool bOn = false;
	bool bOff = false;
	Track.Observe(100, true, On, Off, bOn, bOff);
	for (int si = 101; si <= 101 + Off; ++si)
	{
		ResolveDetachedFrame(Track, si, On, Off);
	}
	Check(Track.bOffDone, "F1 out-of-order: the window after 100 is done before member frame 104 is finalized");
	Row("F1 out-of-order: member frame 104 still pending in a snapshot", EFamily::M53,
		M52Provider(ClassifyT(T, true), false), FM53Signals(), EState::LabelTail, true, false);
	Track.Observe(104, true, On, Off, bOn, bOff);
	Check(!Track.bOffDone && TransitionTailOwed(true, Track), "F1 out-of-order: finalizing 104 re-opens the owed tail");
	Row("F1 out-of-order: after 104 lands the tail is owed again", EFamily::M53,
		M52Provider(ClassifyT(T, TransitionTailOwed(true, Track)), false), FM53Signals(), EState::LabelTail, true, false);
}

static void TestF2LateReceipt()
{
	FTrail P = ClosedWithReceiptOutstanding();
	Check(P.bClosed && !P.bDetached && !P.AllNotedProcessed(), "F2 setup: closed at 503 with 504 in flight (m52 window case)");
	Row("F2 closed, attached, receipt 504 outstanding", EFamily::M53, M52Provider(ClassifyT(P, false), false), FM53Signals(),
		EState::TrailReopenable, true, false);
	Check(P.TryDetach(505, true) && P.bDetached, "F2 setup: the next fire force-detaches with 504 still outstanding");
	Row("F2 force-detached with a receipt outstanding (a detached trail that can reopen)", EFamily::M53,
		M52Provider(ClassifyT(P, false), false), FM53Signals(), EState::TrailReopenable, true, false);

	FTrail Held = P;
	Held.Receive(504, EVerdict::Held, true);
	Check(!Held.bClosed && Held.ReopensAfterDetach == 1, "F2 the late HELD receipt reopens the detached trail");
	Row("F2 after the late held receipt the trail gates again", EFamily::M53, M52Provider(ClassifyT(Held, false), false),
		FM53Signals(), EState::TrailOpen, true, false);

	FTrail Clean = P;
	Clean.Receive(504, EVerdict::NotHeld, true);
	Check(Clean.bClosed && Clean.bDetached && Clean.AllNotedProcessed(), "F2 a NOT-held 504 leaves it closed, detached, processed");
	Row("F2 every receipt processed and detached: released", EFamily::M53, M52Provider(ClassifyT(Clean, false), false),
		FM53Signals(), EState::Closed, false, false);

	FTrail Attached;
	Attached.Open(700, 0, 120);
	for (int si = 700; si <= 702; ++si)
	{
		Attached.NoteArmed(si);
		Attached.Receive(si, EVerdict::NotHeld, true);
	}
	Check(Attached.bClosed && !Attached.bDetached && Attached.AllNotedProcessed(), "F2 setup: closed, processed, not yet detached");
	Row("F2 closed but still attached for products (the next armed frame can reopen it)", EFamily::M53,
		M52Provider(ClassifyT(Attached, false), false), FM53Signals(), EState::TrailReopenable, true, false);

	bool bAdmittedBeforeReceipt = !Evaluate(EFamily::M53, DeriveM52(M52Provider(ClassifyT(P, false), false)), EState::Idle).bExcluded;
	Check(!bAdmittedBeforeReceipt, "F2 sequence: no m53 admission between the closure and the late receipt");
}

static void TestFreeRunningBound()
{
	const int Timeout = 120;
	const int Two[] = { 130, 10 };
	const int TwoPast[] = { 130, 121 };
	const int One[] = { 120 };
	Check(!AllRestoringPastTimeout(Two, 2, Timeout), "bound: one texture still inside its timeout keeps the set restoring");
	Check(AllRestoringPastTimeout(TwoPast, 2, Timeout), "bound: every texture past the timeout");
	Check(AllRestoringPastTimeout(One, 1, Timeout), "bound: exactly at the timeout counts as past");
	Check(!AllRestoringPastTimeout(nullptr, 0, Timeout), "bound: an empty set is not past a timeout");
	Check(IsRestoreEntryPastTimeout(1, 0), "bound: a non-positive timeout reads as one frame");

	FM52Signals Inside;
	Inside.bRestoringSet = true;
	Row("bound: free-running, restoring inside the timeout", EFamily::M53, Inside, FM53Signals(), EState::Restoring, true, false);
	FM52Signals Past = Inside;
	Past.bRestoringPastTimeout = true;
	Row("bound: free-running, restoring past restore_unresolved releases (counted)", EFamily::M53, Past, FM53Signals(),
		EState::Unresolved, false, true);
	FM52Signals ProviderPast = M52Provider(ETrail::None, true);
	ProviderPast.bRestoringPastTimeout = true;
	Row("bound: provider without a trail, restoring past the timeout", EFamily::M53, ProviderPast, FM53Signals(),
		EState::Unresolved, false, true);

	int FirstAdmitted = -1;
	for (int Waited = 0; Waited <= 200; ++Waited)
	{
		FM52Signals S;
		S.bRestoringSet = true;
		S.bRestoringPastTimeout = AllRestoringPastTimeout(&Waited, 1, Timeout);
		if (!Evaluate(EFamily::M53, DeriveM52(S), EState::Idle).bExcluded)
		{
			FirstAdmitted = Waited;
			break;
		}
	}
	Check(FirstAdmitted == Timeout, "bound: the exclusion releases exactly at the timeout, never lasts forever (got "
		+ std::to_string(FirstAdmitted) + ")");
}

struct FModelInstance
{
	bool bActive = false;
	bool bRecord = false;
};

static bool ModelApply(FModelInstance& I, bool bValidArgs)
{
	bool bApplied = false;
	if (bValidArgs)
	{
		I.bActive = true;
		bApplied = true;
	}
	switch (RecordAction(bApplied, I.bActive))
	{
	case ERecordAction::Replace: I.bRecord = true; break;
	case ERecordAction::Keep:    break;
	case ERecordAction::Drop:    I.bRecord = false; break;
	}
	return bApplied;
}

static void TestF3RecordLifecycle()
{
	Check(RecordAfterApply(true, true) == ERecordAction::Replace, "F3 applied replaces the record");
	Check(RecordAfterApply(false, true) == ERecordAction::Keep, "F3 refused while installed keeps the record");
	Check(RecordAfterApply(false, false) == ERecordAction::Drop, "F3 refused and not installed drops the record");

	FModelInstance Uv;
	Check(ModelApply(Uv, true), "F3 m53: valid Apply");
	Check(!ModelApply(Uv, false), "F3 m53: re-Apply with a missing target is refused before any revert");
	Check(Uv.bActive, "F3 m53: the effect stays installed after the refused re-Apply");
	Check(!Uv.bActive || Uv.bRecord, "F3 m53: no installed effect without a record");
	FM53Signals S53;
	S53.bFireLive = FireLive(Uv.bRecord, Uv.bActive);
	Row("F3 m52 after a refused m53 re-Apply", EFamily::M52, FM52Signals(), S53, EState::FireLive, true, false);

	FModelInstance Mip;
	Check(ModelApply(Mip, true), "F3 m52: valid Apply");
	Check(!ModelApply(Mip, false), "F3 m52: re-Apply with a missing target is refused");
	Check(!Mip.bActive || Mip.bRecord, "F3 m52: no installed effect without a record");
	FM52Signals S52;
	S52.bFireLive = FireLive(Mip.bRecord, Mip.bActive);
	Row("F3 m53 after a refused m52 re-Apply", EFamily::M53, S52, FM53Signals(), EState::FireLive, true, false);

	FM53Signals Lost;
	Lost.bFireLive = FireLive(false, true);
	Row("F3 record lost, effect installed: still live", EFamily::M52, FM52Signals(), Lost, EState::FireLive, true, false);
	FM53Signals Stale;
	Stale.bFireLive = FireLive(true, false);
	Row("F3 stale record, effect gone: not live (cannot block forever)", EFamily::M52, FM52Signals(), Stale, EState::Idle, false,
		false);
}

static FFrameEntry E(EFamily F, bool bLabelled, bool bTransition)
{
	FFrameEntry X;
	X.Family = F;
	X.bLabelled = bLabelled;
	X.bTransition = bTransition;
	return X;
}

static void TestCoEntryPredicate()
{
	Check(CoEntry({ E(EFamily::M52, true, false), E(EFamily::M53, true, false) }), "co-entry: m52 labelled + m53 labelled");
	Check(CoEntry({ E(EFamily::M52, false, true), E(EFamily::M53, true, false) }), "co-entry: m52 transition + m53 labelled");
	Check(CoEntry({ E(EFamily::M52, false, true), E(EFamily::M53, false, false) }),
		"co-entry: m52 transition + an UNLABELLED m53 entry (off screen) is still a co-entry");
	Check(CoEntry({ E(EFamily::M52, true, false), E(EFamily::Other, true, false), E(EFamily::M53, false, false) }),
		"co-entry: other entries do not hide it");
	Check(!CoEntry({ E(EFamily::M52, false, false), E(EFamily::M53, true, false) }),
		"co-entry: an m52 entry neither labelled nor transition is not one");
	Check(!CoEntry({ E(EFamily::M52, true, false), E(EFamily::Other, true, true) }), "co-entry: no m53 entry");
	Check(!CoEntry({ E(EFamily::M53, true, false), E(EFamily::M53, false, false) }), "co-entry: m53 only");
	Check(!CoEntry({}), "co-entry: an empty frame");
}

struct FReverseRun
{
	int AdmitSI = -1;
	bool bCoEntry = false;
	int M53First = -1;
	int M53Last = -1;
};

static FReverseRun RunF1Reverse(int ApplySI, int RawRevertSI, int BeginRevertSI, int Latency, int LastSI)
{
	FReverseRun Out;
	std::vector<int> M53Armed;
	const int InstanceEnd = RawRevertSI >= 0 ? RawRevertSI : BeginRevertSI;
	for (int si = ApplySI; si <= LastSI; ++si)
	{
		const bool bActive = si < InstanceEnd;
		const bool bRetained = si < BeginRevertSI;
		bool bPending = false;
		for (int a : M53Armed)
		{
			bPending = bPending || (a + Latency > si);
		}
		if (Out.AdmitSI < 0 && si > ApplySI)
		{
			FM53Signals S;
			S.bFireLive = bActive;
			S.bRevertSettling = !bActive && IsRevertSettlingAt(InstanceEnd, si);
			S.Trail = M53ProviderTrail(bRetained, bPending);
			if (!Evaluate(EFamily::M52, EState::Idle, DeriveM53(S)).bExcluded)
			{
				Out.AdmitSI = si;
			}
		}
		std::vector<FFrameEntry> Entries;
		if (bRetained)
		{
			Entries.push_back(E(EFamily::M53, true, false));
			M53Armed.push_back(si);
			Out.M53First = Out.M53First < 0 ? si : Out.M53First;
			Out.M53Last = si;
		}
		if (Out.AdmitSI >= 0 && si >= Out.AdmitSI)
		{
			Entries.push_back(E(EFamily::M52, true, false));
		}
		Out.bCoEntry = Out.bCoEntry || CoEntry(Entries);
	}
	return Out;
}

static void TestF1ReverseRetained()
{
	Check(RetainedLiveFireOwesFrames(EFamily::M53) && !RetainedLiveFireOwesFrames(EFamily::M52),
		"F1 reverse rule: a retained auto live fire owes frames for m53 only (m52 keeps its render-truth rule)");
	Check(PendingFireOwesFrames(EFamily::M53, false) && PendingFireOwesFrames(EFamily::M52, true)
		&& !PendingFireOwesFrames(EFamily::M52, false),
		"F1 reverse rule: a pending FireWindow snapshot owes m53 frames; m52 needs a render-truth fire, as before");
	Check(EmitterTrail(true) == ETrail::LabelTail && EmitterTrail(false) == ETrail::None,
		"F1 reverse rule: an emitter that still owes frames reads label_tail");

	FM53Signals Retained;
	Retained.Trail = M53ProviderTrail(true, false);
	Row("m52<-m53 raw early IAI.Revert (or target loss) past R+2, auto fire entry still retained", EFamily::M52, FM52Signals(),
		Retained, EState::LabelTail, true, false);
	FM53Signals Pending;
	Pending.Trail = M53ProviderTrail(false, true);
	Row("m52<-m53 retained entry cleared, a FireWindow snapshot still pending", EFamily::M52, FM52Signals(), Pending,
		EState::LabelTail, true, false);
	FM53Signals Cleared;
	Cleared.Trail = M53ProviderTrail(false, false);
	Row("m52<-m53 retained entry cleared and every snapshot finalized", EFamily::M52, FM52Signals(), Cleared, EState::Idle,
		false, false);

	const FReverseRun Raw1 = RunF1Reverse(10, 20, 40, 1, 60);
	Check(Raw1.M53First == 10 && Raw1.M53Last == 39, "F1 reverse setup: the retained m53 entry is on frames 10..39 after a raw "
		"revert at 20 (it clears only at the scheduled BeginRevert at 40)");
	Check(Raw1.AdmitSI == 40, "F1 reverse sequence (readback latency 1): a forced m52 fire is first admitted at si 40, when the "
		"retained entry clears, not at R+2 = 22 (got " + std::to_string(Raw1.AdmitSI) + ")");
	Check(!Raw1.bCoEntry, "F1 reverse sequence (latency 1): no frame carries both the retained m53 entry and an m52 entry");

	const FReverseRun Raw3 = RunF1Reverse(10, 20, 40, 3, 60);
	Check(Raw3.AdmitSI == 42, "F1 reverse sequence (readback latency 3): admitted at si 42, after the last m53-carrying "
		"FireWindow snapshot (armed 39) is finalized (got " + std::to_string(Raw3.AdmitSI) + ")");
	Check(!Raw3.bCoEntry, "F1 reverse sequence (latency 3): no co-entry frame");

	const FReverseRun Ordinary1 = RunF1Reverse(10, -1, 40, 1, 60);
	Check(Ordinary1.AdmitSI == 42 && !Ordinary1.bCoEntry,
		"F1 reverse control: an ordinary scheduled revert at 40 still releases at R+2 = 42 at latency 1 (got "
		+ std::to_string(Ordinary1.AdmitSI) + ")");
	const FReverseRun Ordinary4 = RunF1Reverse(10, -1, 40, 4, 60);
	Check(Ordinary4.AdmitSI == 43 && !Ordinary4.bCoEntry,
		"F1 reverse control: at latency 4 the release is max(R+2, last m53 snapshot finalized) = 43 (got "
		+ std::to_string(Ordinary4.AdmitSI) + ")");
}

int main()
{
	const FM52Signals NoM52;
	const FM53Signals NoM53;

	Check(FamilyOf("stuck_low_mip") == EFamily::M52, "family stuck_low_mip");
	Check(FamilyOf("uv_corruption") == EFamily::M53, "family uv_corruption");
	Check(FamilyOf("normal_corruption") == EFamily::M53, "family normal_corruption");
	Check(FamilyOf("missing_texture") == EFamily::Other, "family missing_texture");
	Check(FamilyOf("uv_corruption_x") == EFamily::Other, "family prefix is not a match");
	Check(std::strcmp(DescribeState(EState::TrailOpen), "trail_open") == 0, "describe trail_open");
	Check(std::strcmp(DescribeState(EState::RevertSettling), "revert_settling") == 0, "describe revert_settling");
	Check(std::strcmp(DescribeState(EState::Unresolved), "restore_unresolved") == 0, "describe restore_unresolved");
	Check(std::strcmp(DescribeState(EState::TrailReopenable), "trail_reopenable") == 0, "describe trail_reopenable");
	Check(std::strcmp(DescribeState(EState::LabelTail), "label_tail") == 0, "describe label_tail");

	const FTrail Gating = GatingTrail();
	const FTrail ClosedDetached = ClosedDetachedTrail();
	const FTrail Reopened = ReopenedAfterDetachTrail();
	const FTrail Unresolved = UnresolvedTrail();
	const FTrail Adopted = AdoptedTrail(Gating);
	const FTrail AdoptedUnresolved = AdoptedTrail(Unresolved);

	Check(Gating.GatesNextBurst() && ClassifyT(Gating, false) == ETrail::Gating, "trail gating classifies gating");
	Check(ClosedDetached.bClosed && ClosedDetached.bDetached && !ClosedDetached.GatesNextBurst()
		&& ClassifyT(ClosedDetached, false) == ETrail::Closed, "trail closed, detached, processed, no tail classifies closed");
	Check(Reopened.ReopensAfterDetach == 1 && !Reopened.bDetached && Reopened.GatesNextBurst()
		&& ClassifyT(Reopened, false) == ETrail::Gating, "trail reopened after detach classifies gating");
	Check(Unresolved.bUnresolved && !Unresolved.GatesNextBurst() && ClassifyT(Unresolved, false) == ETrail::Unresolved,
		"trail unresolved classifies unresolved");
	Check(Adopted.bInherited && ClassifyT(Adopted, false) == ETrail::Gating, "adopted trail classifies gating");
	Check(AdoptedUnresolved.bInherited && ClassifyT(AdoptedUnresolved, false) == ETrail::Unresolved,
		"adopted unresolved trail keeps unresolved");
	Check(Classify(FTrailFacts()) == ETrail::None, "no trail and no tail classifies none");
	Check(CombineTrail(ETrail::Closed, ETrail::Gating) == ETrail::Gating
		&& CombineTrail(ETrail::Gating, ETrail::Reopenable) == ETrail::Gating
		&& CombineTrail(ETrail::Reopenable, ETrail::LabelTail) == ETrail::Reopenable
		&& CombineTrail(ETrail::LabelTail, ETrail::Unresolved) == ETrail::LabelTail
		&& CombineTrail(ETrail::Unresolved, ETrail::Closed) == ETrail::Unresolved
		&& CombineTrail(ETrail::None, ETrail::Closed) == ETrail::Closed,
		"combine: gating > reopenable > label_tail > unresolved > closed > none");

	FM52Signals FireLiveSig;
	FireLiveSig.bFireLive = true;
	FM52Signals FireLiveAndTrail = M52Provider(ETrail::Gating, true);
	FireLiveAndTrail.bFireLive = true;
	FM52Signals NoProviderRestoring;
	NoProviderRestoring.bRestoringSet = true;
	FM52Signals NoProviderTrailIgnored;
	NoProviderTrailIgnored.Trail = ETrail::Gating;

	Row("m53<-m52 idle", EFamily::M53, NoM52, NoM53, EState::Idle, false, false);
	Row("m53<-m52 fire live", EFamily::M53, FireLiveSig, NoM53, EState::FireLive, true, false);
	Row("m53<-m52 fire live with trail", EFamily::M53, FireLiveAndTrail, NoM53, EState::FireLive, true, false);
	Row("m53<-m52 trail gating after BeginRevert", EFamily::M53, M52Provider(ClassifyT(Gating, false), true), NoM53,
		EState::TrailOpen, true, false);
	Row("m53<-m52 trail gating, game-thread mirror already closed", EFamily::M53, M52Provider(ClassifyT(Gating, false), false),
		NoM53, EState::TrailOpen, true, false);
	Row("m53<-m52 trail reopened after detach", EFamily::M53, M52Provider(ClassifyT(Reopened, false), false), NoM53,
		EState::TrailOpen, true, false);
	Row("m53<-m52 adopted trail gating", EFamily::M53, M52Provider(ClassifyT(Adopted, false), false), NoM53,
		EState::TrailOpen, true, false);
	Row("m53<-m52 carried trail gating (not yet adopted)", EFamily::M53, M52Provider(ClassifyT(Gating, false), false), NoM53,
		EState::TrailOpen, true, false);
	Row("m53<-m52 closed + gating trails combine", EFamily::M53,
		M52Provider(CombineTrail(ClassifyT(ClosedDetached, false), ClassifyT(Gating, false)), false), NoM53, EState::TrailOpen,
		true, false);
	Row("m53<-m52 closed + label-tail trails combine", EFamily::M53,
		M52Provider(CombineTrail(ClassifyT(ClosedDetached, false), ClassifyT(ClosedDetached, true)), false), NoM53,
		EState::LabelTail, true, false);
	Row("m53<-m52 unresolved + label-tail trails combine (the tail still excludes)", EFamily::M53,
		M52Provider(CombineTrail(ClassifyT(Unresolved, false), ClassifyT(ClosedDetached, true)), false), NoM53,
		EState::LabelTail, true, false);
	Row("m53<-m52 restore_unresolved (mirror still restoring)", EFamily::M53, M52Provider(ClassifyT(Unresolved, false), true),
		NoM53, EState::Unresolved, false, true);
	Row("m53<-m52 adopted unresolved", EFamily::M53, M52Provider(ClassifyT(AdoptedUnresolved, false), false), NoM53,
		EState::Unresolved, false, true);
	Row("m53<-m52 closed and detached", EFamily::M53, M52Provider(ClassifyT(ClosedDetached, false), false), NoM53,
		EState::Closed, false, false);
	Row("m53<-m52 closed trail outranks the mirror", EFamily::M53, M52Provider(ClassifyT(ClosedDetached, false), true), NoM53,
		EState::Closed, false, false);
	Row("m53<-m52 provider without a trail falls back to restoring", EFamily::M53, M52Provider(ETrail::None, true), NoM53,
		EState::Restoring, true, false);
	Row("m53<-m52 no provider, restoring set non-empty", EFamily::M53, NoProviderRestoring, NoM53, EState::Restoring, true,
		false);
	Row("m53<-m52 no provider, trail value ignored", EFamily::M53, NoProviderTrailIgnored, NoM53, EState::Idle, false, false);

	FM53Signals M53Live;
	M53Live.bFireLive = true;
	const long long R = 1000;
	Row("m52<-m53 idle", EFamily::M52, NoM52, NoM53, EState::Idle, false, false);
	Row("m52<-m53 fire live", EFamily::M52, NoM52, M53Live, EState::FireLive, true, false);
	Row("m52<-m53 revert R+0", EFamily::M52, NoM52, M53Revert(R, R), EState::RevertSettling, true, false);
	Row("m52<-m53 revert R+1", EFamily::M52, NoM52, M53Revert(R, R + 1), EState::RevertSettling, true, false);
	Row("m52<-m53 revert R+2 due", EFamily::M52, NoM52, M53Revert(R, R + 2), EState::Idle, false, false);
	Row("m52<-m53 no revert yet", EFamily::M52, NoM52, M53Revert(-1, R), EState::Idle, false, false);
	Row("m52<-m53 clock before revert", EFamily::M52, NoM52, M53Revert(R, R - 1), EState::Idle, false, false);
	FM53Signals M53Tail;
	M53Tail.Trail = ETrail::LabelTail;
	Row("m52<-m53 label tail owed (both directions, if m53 ever gets flags)", EFamily::M52, NoM52, M53Tail, EState::LabelTail, true,
		false);
	FM53Signals M53Reopenable;
	M53Reopenable.Trail = ETrail::Reopenable;
	Row("m52<-m53 reopenable trail (both directions)", EFamily::M52, NoM52, M53Reopenable, EState::TrailReopenable, true, false);
	FM53Signals M53ClosedTrail;
	M53ClosedTrail.Trail = ETrail::Closed;
	Row("m52<-m53 closed trail releases", EFamily::M52, NoM52, M53ClosedTrail, EState::Idle, false, false);

	Row("other<-both live", EFamily::Other, FireLiveSig, M53Live, EState::Idle, false, false);
	Row("m53<-m53 live only (cross-family rule)", EFamily::M53, NoM52, M53Live, EState::Idle, false, false);
	Row("m52<-m52 trail only (cross-family rule)", EFamily::M52, M52Provider(ETrail::Gating, true), NoM53, EState::Idle, false,
		false);

	{
		const EFamily Pool[] = { EFamily::M53, EFamily::M53 };
		int Admitted = -1;
		const unsigned Draws = DrawsForPool(Pool, 2, DeriveM52(M52Provider(ClassifyT(Gating, false), false)), DeriveM53(NoM53),
			Admitted);
		Check(Admitted == 0, "all_excluded m53 pool behind a gating m52 trail admits none");
		Check(Draws == 0, "all_excluded m53 pool consumes zero draws");
	}
	{
		const EFamily Pool[] = { EFamily::M52 };
		int Admitted = -1;
		const unsigned Draws = DrawsForPool(Pool, 1, DeriveM52(NoM52), DeriveM53(M53Revert(R, R + 1)), Admitted);
		Check(Admitted == 0, "all_excluded m52 pool at revert R+1 admits none");
		Check(Draws == 0, "all_excluded m52 pool consumes zero draws");
	}
	{
		const EFamily Pool[] = { EFamily::M53, EFamily::Other };
		int Admitted = -1;
		const unsigned Draws = DrawsForPool(Pool, 2, DeriveM52(M52Provider(ClassifyT(Gating, false), false)), DeriveM53(NoM53),
			Admitted);
		Check(Admitted == 1, "control: a non-m53 pool member stays eligible behind a gating trail");
		Check(Draws == 4, "control: one admitted id draws id, target, hold and (callable says m53) mode");
	}

	TestF1TemporalOffTail();
	TestF1CarriedTailOnly();
	TestF1PendingSnapshot();
	TestF2LateReceipt();
	TestFreeRunningBound();
	TestF3RecordLifecycle();
	TestCoEntryPredicate();
	TestF1ReverseRetained();

	std::printf("exclusion selftest (mutant %d): %d checks, %d failures\n", EXCL_MUTANT, GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
