#include "../Source/AnomalyInjector/Public/AnomalyExclusion.h"
#include "../Source/AnomalyInjector/Private/Anomalies/TexCorruptPure.h"

#include <cstdio>
#include <cstring>
#include <string>

#ifndef EXCL_MUTANT
#define EXCL_MUTANT 0
#endif

using namespace AnomalyExclusion;
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

static EState DeriveM52(const FM52Signals& S)
{
	if constexpr (EXCL_MUTANT == 1)
	{
		return S.bFireLive ? EState::FireLive : EState::Idle;
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
	else
	{
		return M53State(S);
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

static FM52Signals M52Provider(ETrail Trail, bool bRestoringSet)
{
	FM52Signals S;
	S.bProvider = true;
	S.Trail = Trail;
	S.bRestoringSet = bRestoringSet;
	return S;
}

static void Row(const char* Name, EFamily Candidate, const FM52Signals& M52, const FM53Signals& M53,
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

	const FTrail Gating = GatingTrail();
	const FTrail ClosedDetached = ClosedDetachedTrail();
	const FTrail Reopened = ReopenedAfterDetachTrail();
	const FTrail Unresolved = UnresolvedTrail();
	const FTrail Adopted = AdoptedTrail(Gating);
	const FTrail AdoptedUnresolved = AdoptedTrail(Unresolved);

	Check(Gating.GatesNextBurst() && ClassifyTrail(Gating) == ETrail::Gating, "trail gating classifies gating");
	Check(ClosedDetached.bClosed && ClosedDetached.bDetached && !ClosedDetached.GatesNextBurst()
		&& ClassifyTrail(ClosedDetached) == ETrail::Closed, "trail closed and detached classifies closed");
	Check(Reopened.ReopensAfterDetach == 1 && !Reopened.bDetached && Reopened.GatesNextBurst()
		&& ClassifyTrail(Reopened) == ETrail::Gating, "trail reopened after detach classifies gating");
	Check(Unresolved.bUnresolved && !Unresolved.GatesNextBurst() && ClassifyTrail(Unresolved) == ETrail::Unresolved,
		"trail unresolved classifies unresolved");
	Check(Adopted.bInherited && ClassifyTrail(Adopted) == ETrail::Gating, "adopted trail classifies gating");
	Check(AdoptedUnresolved.bInherited && ClassifyTrail(AdoptedUnresolved) == ETrail::Unresolved,
		"adopted unresolved trail keeps unresolved");
	Check(ClassifyTrail(false, false, false) == ETrail::None, "trail not open classifies none");
	Check(CombineTrail(ETrail::Closed, ETrail::Gating) == ETrail::Gating
		&& CombineTrail(ETrail::Gating, ETrail::Unresolved) == ETrail::Gating
		&& CombineTrail(ETrail::Unresolved, ETrail::Closed) == ETrail::Unresolved
		&& CombineTrail(ETrail::None, ETrail::Closed) == ETrail::Closed, "combine: gating > unresolved > closed > none");

	FM52Signals FireLive;
	FireLive.bFireLive = true;
	FM52Signals FireLiveAndTrail = M52Provider(ETrail::Gating, true);
	FireLiveAndTrail.bFireLive = true;
	FM52Signals NoProviderRestoring;
	NoProviderRestoring.bRestoringSet = true;
	FM52Signals NoProviderTrailIgnored;
	NoProviderTrailIgnored.Trail = ETrail::Gating;

	Row("m53<-m52 idle", EFamily::M53, NoM52, NoM53, EState::Idle, false, false);
	Row("m53<-m52 fire live", EFamily::M53, FireLive, NoM53, EState::FireLive, true, false);
	Row("m53<-m52 fire live with trail", EFamily::M53, FireLiveAndTrail, NoM53, EState::FireLive, true, false);
	Row("m53<-m52 trail gating after BeginRevert", EFamily::M53, M52Provider(ClassifyTrail(Gating), true), NoM53,
		EState::TrailOpen, true, false);
	Row("m53<-m52 trail gating, game-thread mirror already closed", EFamily::M53, M52Provider(ClassifyTrail(Gating), false),
		NoM53, EState::TrailOpen, true, false);
	Row("m53<-m52 trail reopened after detach", EFamily::M53, M52Provider(ClassifyTrail(Reopened), false), NoM53,
		EState::TrailOpen, true, false);
	Row("m53<-m52 adopted trail gating", EFamily::M53, M52Provider(ClassifyTrail(Adopted), false), NoM53,
		EState::TrailOpen, true, false);
	Row("m53<-m52 carried trail gating (not yet adopted)", EFamily::M53, M52Provider(ClassifyTrail(Gating), false), NoM53,
		EState::TrailOpen, true, false);
	Row("m53<-m52 closed + gating trails combine", EFamily::M53,
		M52Provider(CombineTrail(ClassifyTrail(ClosedDetached), ClassifyTrail(Gating)), false), NoM53, EState::TrailOpen, true,
		false);
	Row("m53<-m52 restore_unresolved (mirror still restoring)", EFamily::M53, M52Provider(ClassifyTrail(Unresolved), true),
		NoM53, EState::Unresolved, false, true);
	Row("m53<-m52 adopted unresolved", EFamily::M53, M52Provider(ClassifyTrail(AdoptedUnresolved), false), NoM53,
		EState::Unresolved, false, true);
	Row("m53<-m52 closed and detached", EFamily::M53, M52Provider(ClassifyTrail(ClosedDetached), false), NoM53,
		EState::Closed, false, false);
	Row("m53<-m52 closed trail outranks the mirror", EFamily::M53, M52Provider(ClassifyTrail(ClosedDetached), true), NoM53,
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

	Row("other<-both live", EFamily::Other, FireLive, M53Live, EState::Idle, false, false);
	Row("m53<-m53 live only (cross-family rule)", EFamily::M53, NoM52, M53Live, EState::Idle, false, false);
	Row("m52<-m52 trail only (cross-family rule)", EFamily::M52, M52Provider(ETrail::Gating, true), NoM53, EState::Idle, false,
		false);

	{
		const EFamily Pool[] = { EFamily::M53, EFamily::M53 };
		int Admitted = -1;
		const unsigned Draws = DrawsForPool(Pool, 2, DeriveM52(M52Provider(ClassifyTrail(Gating), false)), DeriveM53(NoM53),
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
		const unsigned Draws = DrawsForPool(Pool, 2, DeriveM52(M52Provider(ClassifyTrail(Gating), false)), DeriveM53(NoM53),
			Admitted);
		Check(Admitted == 1, "control: a non-m53 pool member stays eligible behind a gating trail");
		Check(Draws == 4, "control: one admitted id draws id, target, hold and (callable says m53) mode");
	}

	std::printf("exclusion selftest (mutant %d): %d checks, %d failures\n", EXCL_MUTANT, GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
