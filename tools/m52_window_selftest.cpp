#include "../Source/AnomalyInjector/Public/AnomalyStuckMipWindow.h"

#include <cstdio>
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

struct FSynthFrame
{
	int SessionIndex;
	std::vector<int> RenderResident;
	std::vector<int> GtResident;
	bool bTrailing;
};

static EVerdict VerdictOf(const std::vector<int>& Resident, const std::vector<int>& Baseline)
{
	std::vector<ETexState> States;
	for (size_t i = 0; i < Resident.size(); ++i)
	{
		States.push_back(ClassifyTexture(Resident[i], Baseline[i], Resident[i] >= 0));
	}
	return Combine(States.data(), (int)States.size());
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
	Check(Combine(nullptr, 0) == EVerdict::BeforeApply, "no watched texture = armed before apply");
}

static void TestOnsetAgainstLaggingMirror()
{
	const std::vector<int> Baseline = { 11, 11, 11 };
	std::vector<FSynthFrame> Frames;
	for (int si = 0; si < 12; ++si)
	{
		FSynthFrame F;
		F.SessionIndex = si;
		F.bTrailing = false;
		const bool bRtHeld = si >= 5;
		const bool bGtHeld = si >= 6;
		const bool bSecondLate = si >= 7;
		F.RenderResident = { bRtHeld ? 7 : 11, bRtHeld ? 7 : 11, bSecondLate ? 7 : 11 };
		F.GtResident = { bGtHeld ? 7 : 11, bGtHeld ? 7 : 11, (si >= 8) ? 7 : 11 };
		Frames.push_back(F);
	}
	std::vector<int> NewLabelled, OldLabelled, RenderHeld;
	for (const FSynthFrame& F : Frames)
	{
		const EVerdict Rv = VerdictOf(F.RenderResident, Baseline);
		const EVerdict Gv = VerdictOf(F.GtResident, Baseline);
		if (IsMember(LiveMembership(Rv))) { NewLabelled.push_back(F.SessionIndex); }
		if (Gv == EVerdict::Held) { OldLabelled.push_back(F.SessionIndex); }
		if (Rv == EVerdict::Held) { RenderHeld.push_back(F.SessionIndex); }
	}
	Check(NewLabelled == RenderHeld, "onset: render-record label window == render-held frames " + Str(NewLabelled));
	Check(!NewLabelled.empty() && NewLabelled.front() == 5, "onset: first labelled frame is the first render-held frame (5)");
	Check(!OldLabelled.empty() && OldLabelled.front() == 6, "onset: the game-thread mirror opens one frame late (6) - the defect reproduced");
	Check(OldLabelled != NewLabelled, "onset: the legacy mirror window differs from the render window (can-fail)");
}

static std::vector<EMembership> RunTrail(FTrail& T, int FirstSI, const std::vector<EVerdict>& V)
{
	std::vector<EMembership> Out;
	for (size_t i = 0; i < V.size(); ++i)
	{
		Out.push_back(T.Step(FirstSI + (int)i, V[i]));
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
	Check(T.bClosed && T.ClosedAtSI == 105, "offset: closes after ConfirmFrames=2 consecutive baseline frames (at 105)");
	Check(!T.GatesNextBurst(), "offset: a closed trail releases the next burst");
	int LegacyUnlabelledHeld = 0;
	for (size_t i = 0; i < V.size(); ++i)
	{
		if (V[i] == EVerdict::Held) { ++LegacyUnlabelledHeld; }
	}
	Check(LegacyUnlabelledHeld == 5,
		"offset: the legacy window, closed at revert, leaves 5 render-held frames unlabelled - the defect reproduced");
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
		"flap: a held frame after a baseline frame is labelled again (per-frame membership, holes stay holes)");
	Check(M[3] == EMembership::Unknown, "unknown trailing frame is labelled, never silently clean");
	Check(T.bClosed && T.ClosedAtSI == 5, "flap: one baseline frame does not close; two consecutive after the last held/unknown do (5)");
}

static void TestTimeoutAndGating()
{
	FTrail T;
	T.Open(50, 0, 30);
	Check(T.GatesNextBurst(), "gate: an open trail holds the next burst");
	Check(!T.ShouldTimeout(79), "timeout: 29 captured frames after revert is not yet a timeout");
	Check(T.ShouldTimeout(80), "timeout: 30 captured frames after revert is a timeout");
	T.bUnresolved = true;
	Check(!T.GatesNextBurst(), "timeout: an unresolved trail stops gating (capture continues, fires resume except m52)");
	Check(T.StillAttached(), "timeout: an unresolved trail stays attached, so held frames are still labelled");
	const std::vector<EVerdict> V(40, EVerdict::Held);
	const std::vector<EMembership> M = RunTrail(T, 50, V);
	Check((int)Members(M, 50).size() == 40, "timeout: every render-held frame after the timeout is still labelled");
	const std::vector<EVerdict> W = { EVerdict::NotHeld, EVerdict::NotHeld };
	RunTrail(T, 90, W);
	Check(T.bClosed, "timeout: a late restore still closes the trail");
}

static void TestPurity()
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
		"purity: an OFF-SCREEN user (the glove/wall case) is refused - the old visible-only rule did not see it");
	Check(ClassifyPurity(SelfTwo, 2, TargetTwo, 2, &Foreign) == EPurity::Shared && Foreign == 0,
		"purity: two target components sharing a texture is refused (the rule is exactly one user component)");
	Check(ClassifyPurity(nullptr, 0, Target, 1, &Foreign) == EPurity::NoUsers, "purity: no users is not pure");
	const int VisibleOthers = 0;
	const bool bLegacyWouldFire = VisibleOthers <= 0;
	Check(bLegacyWouldFire && ClassifyPurity(Offscreen, 2, Target, 1, &Foreign) != EPurity::Pure,
		"purity can-fail: the legacy visible-only rule fires the off-screen-shared texture, the new rule refuses it");
}

int main()
{
	TestTextureAndCombine();
	TestOnsetAgainstLaggingMirror();
	TestTrailingNoTail();
	TestTrailingWithTail();
	TestTrailingFlapAndUnknown();
	TestTimeoutAndGating();
	TestPurity();
	std::printf("m52 window selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
