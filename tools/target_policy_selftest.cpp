#include "../Source/AnomalyInjector/Public/AnomalyTargetPolicy.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace AnomalyTargetPolicy;

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

struct FKatStream
{
	unsigned Seed = 0;
	unsigned Steps = 0;

	void Initialize(int InSeed)
	{
		Seed = (unsigned)InSeed;
		Steps = 0;
	}

	void Mutate()
	{
		Seed = Seed * 196314165u + 907633515u;
		++Steps;
	}

	float GetFraction()
	{
		Mutate();
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

static unsigned FloatBits(float F)
{
	unsigned U = 0;
	std::memcpy(&U, &F, sizeof(U));
	return U;
}

struct FCand
{
	const char* Name;
	bool bNanite;
};

struct FAttempt
{
	int Outcome = 0;
	int Id = -1;
	int NumCandidates = 0;
	const char* Target = nullptr;
	int TargetIndex = -1;
	unsigned HoldBits = 0;
	unsigned SeedAfter = 0;
	unsigned Draws = 0;
};

static FAttempt AutoPoolAttempt(FKatStream& Stream, int NumEligible, std::vector<FCand> Candidates, bool bAllowNanite,
	float HoldMin, float HoldMax)
{
	FAttempt A;
	const unsigned Before = Stream.Steps;
	if (NumEligible <= 0)
	{
		A.Outcome = 0;
		A.SeedAfter = Stream.Seed;
		return A;
	}
	A.Id = Stream.RandHelper(NumEligible);
	const int Kept = FilterNaniteCandidates(Candidates.data(), (int)Candidates.size(), bAllowNanite,
		[](const FCand& C) { return C.bNanite; });
	Candidates.resize((size_t)Kept);
	A.NumCandidates = Kept;
	if (Kept <= 0)
	{
		A.Outcome = 1;
	}
	else
	{
		A.TargetIndex = Stream.RandHelper(Kept);
		A.Target = Candidates[(size_t)A.TargetIndex].Name;
		A.HoldBits = FloatBits((float)Stream.FRandRange(HoldMin, HoldMax));
		A.Outcome = 2;
	}
	A.SeedAfter = Stream.Seed;
	A.Draws = Stream.Steps - Before;
	return A;
}

static void TestDecision()
{
	Check(DecideNanite(false, 3, 1) == ENaniteDecision::Refuse, "setting 0: a target with one Nanite primitive of three is refused");
	Check(DecideNanite(false, 3, 3) == ENaniteDecision::Refuse, "setting 0: an all-Nanite target is refused");
	Check(DecideNanite(false, 3, 0) == ENaniteDecision::Admit, "setting 0: a target with no Nanite primitive is admitted");
	Check(DecideNanite(false, 0, 0) == ENaniteDecision::Admit, "setting 0: a target with nothing drawn is not a Nanite refusal");
	Check(DecideNanite(true, 3, 1) == ENaniteDecision::Admit && DecideNanite(true, 3, 3) == ENaniteDecision::Admit,
		"setting 1: every Nanite target is admitted (the previous unmeasured contract)");
	Check(std::string(DescribeNaniteReason()) == "nanite_unmaskable", "the refusal reason is nanite_unmaskable");
}

static void TestFilter()
{
	std::vector<FCand> C = { { "A", false }, { "B", true }, { "C", false }, { "D", true }, { "E", false } };
	std::vector<FCand> Copy = C;
	const int Kept = FilterNaniteCandidates(Copy.data(), (int)Copy.size(), false, [](const FCand& X) { return X.bNanite; });
	Check(Kept == 3 && std::string(Copy[0].Name) == "A" && std::string(Copy[1].Name) == "C" && std::string(Copy[2].Name) == "E",
		"the filter drops only the Nanite candidates and keeps the others in their sorted order");
	std::vector<FCand> Copy1 = C;
	const int Kept1 = FilterNaniteCandidates(Copy1.data(), (int)Copy1.size(), true, [](const FCand& X) { return X.bNanite; });
	Check(Kept1 == 5 && std::string(Copy1[1].Name) == "B" && std::string(Copy1[3].Name) == "D",
		"setting 1: the filter keeps every candidate, unchanged");
	std::vector<FCand> None = { { "A", false }, { "B", false } };
	Check(FilterNaniteCandidates(None.data(), 2, false, [](const FCand& X) { return X.bNanite; }) == 2,
		"no Nanite candidate: the list is unchanged");
	Check(FilterNaniteCandidates<FCand>(nullptr, 0, false, [](const FCand& X) { return X.bNanite; }) == 0,
		"an empty list stays empty");
}

static const std::vector<FCand> GPoolNoNanite = {
	{ "Rock0", false }, { "Rock1", false }, { "Rock2", false }, { "Rock3", false }, { "Rock4", false }, { "Rock5", false }, { "Rock6", false } };
static const std::vector<FCand> GPoolTwoNanite = {
	{ "Rock0", false }, { "Wall1", true }, { "Rock2", false }, { "Wall3", true }, { "Rock4", false }, { "Rock5", false }, { "Rock6", false } };
static const std::vector<FCand> GPoolAllNanite = {
	{ "Wall0", true }, { "Wall1", true }, { "Wall2", true }, { "Wall3", true }, { "Wall4", true }, { "Wall5", true }, { "Wall6", true } };

struct FKnown
{
	int Id;
	int Target;
	unsigned HoldBits;
	unsigned SeedAfter;
	unsigned Draws;
};

static const FKnown GKnownNoNanite[] = {
	{ 0, 5, 0x40B07B8Eu, 0xD69ED00Fu, 3u },
	{ 1, 4, 0x40A0742Cu, 0xABE074E8u, 3u },
	{ 0, 4, 0x408D9F03u, 0x79A8096Du, 3u },
};

static std::vector<FAttempt> RunThree(const std::vector<FCand>& Pool, bool bAllow)
{
	FKatStream S;
	S.Initialize(4242);
	std::vector<FAttempt> Out;
	for (int k = 0; k < 3; ++k)
	{
		Out.push_back(AutoPoolAttempt(S, 2, Pool, bAllow, 3.0f, 6.0f));
	}
	return Out;
}

static void TestKat()
{
	const std::vector<FAttempt> A = RunThree(GPoolNoNanite, false);
	for (int k = 0; k < 3; ++k)
	{
		const FKnown& K = GKnownNoNanite[k];
		Check(A[k].Outcome == 2 && A[k].Id == K.Id && A[k].TargetIndex == K.Target && A[k].HoldBits == K.HoldBits
			&& A[k].SeedAfter == K.SeedAfter && A[k].Draws == K.Draws,
			"KAT no Nanite in the pool, setting 0, attempt " + std::to_string(k)
			+ ": id/target/hold/seed/draws equal the m53 draw-KAT known answers (non_m53_no_mode_draw), so the filter "
			"leaves the random stream untouched");
	}

	const std::vector<FAttempt> B = RunThree(GPoolTwoNanite, false);
	for (int k = 0; k < 3; ++k)
	{
		const FKnown& K = GKnownNoNanite[k];
		Check(B[k].Outcome == 2 && B[k].NumCandidates == 5 && B[k].Id == K.Id && B[k].Draws == 3u && B[k].SeedAfter == K.SeedAfter,
			"KAT two of seven candidates Nanite, attempt " + std::to_string(k)
			+ ": still three draws and the same stream state after, so later attempts are unaffected");
		Check(B[k].Target != nullptr && std::strncmp(B[k].Target, "Wall", 4) != 0,
			"KAT two Nanite, attempt " + std::to_string(k) + ": the drawn target is never a Nanite actor");
	}
	Check(B[0].TargetIndex == 3 && std::string(B[0].Target) == "Rock5" && B[1].TargetIndex == 3 && std::string(B[1].Target) == "Rock5"
		&& B[2].TargetIndex == 3 && std::string(B[2].Target) == "Rock5",
		"KAT two Nanite: the target index is drawn over the five survivors - known answers Rock5 / Rock5 / Rock5 "
		"(index 3 of 5, from the independent Python stream replica tools/texcorrupt_draw_kat.py); without the filter "
		"the same fractions pick index 5 / 4 / 4 of seven: Rock5 / Rock4 / Rock4");

	const std::vector<FAttempt> C = RunThree(GPoolAllNanite, false);
	Check(C[0].Outcome == 1 && C[0].Draws == 1u && C[1].Outcome == 1 && C[1].Draws == 1u && C[2].Outcome == 1 && C[2].Draws == 1u,
		"KAT every candidate Nanite: each attempt takes the no-candidate path after the id draw (1 draw, nothing fired)");

	const std::vector<FAttempt> D = RunThree(GPoolTwoNanite, true);
	for (int k = 0; k < 3; ++k)
	{
		const FKnown& K = GKnownNoNanite[k];
		Check(D[k].TargetIndex == K.Target && D[k].SeedAfter == K.SeedAfter && D[k].HoldBits == K.HoldBits,
			"KAT setting 1 with Nanite in the pool, attempt " + std::to_string(k)
			+ ": identical to the unfiltered protocol (the previous behaviour, byte for byte)");
	}
	Check(std::string(D[2].Target) == "Rock4" && std::string(D[1].Target) == "Rock4",
		"KAT setting 1: Nanite actors stay drawable (attempt 1 index 4 = Rock4)");
}

static void TestR6FailClosed()
{
	Check(DecideNaniteTarget(false, false, 3, 0) == ENaniteDecision::RefuseProbeMissing,
		"R6: setting 0 with no classifier registered refuses even a target that reads zero Nanite parts (fail closed)");
	Check(DecideNaniteTarget(false, false, 0, 0) == ENaniteDecision::RefuseProbeMissing,
		"R6: setting 0 with no classifier refuses whatever the enumeration says");
	Check(DecideNaniteTarget(true, false, 3, 1) == ENaniteDecision::Admit,
		"R6: setting 1 is the deliberate bypass and admits with or without a classifier");
	Check(DecideNaniteTarget(false, true, 3, 1) == ENaniteDecision::Refuse && DecideNaniteTarget(false, true, 3, 0) == ENaniteDecision::Admit,
		"R6: with a classifier registered, setting 0 behaves as before");
	Check(std::string(DescribeNaniteProbeMissingReason()) == "nanite_probe_missing", "R6: the refusal reason is nanite_probe_missing");
	Check(NaniteBlocksLabel(false, false, true, 2, 0), "R6: a frame cannot be labelled while the classifier is missing (setting 0)");
}

static void TestR5Enumeration()
{
	FPrimitiveFacts Body;
	Body.bVisible = true;
	Body.bGeometryClass = true;
	FPrimitiveFacts ExcludedNanite = Body;
	ExcludedNanite.bExcludedByTargetPattern = true;
	ExcludedNanite.bNanite = true;
	FPrimitiveFacts FoliageNanite = Body;
	FoliageNanite.bFoliageOwner = true;
	FoliageNanite.bNanite = true;
	FPrimitiveFacts HiddenNanite = Body;
	HiddenNanite.bVisible = false;
	HiddenNanite.bNanite = true;
	FPrimitiveFacts EmptyIsm = Body;
	EmptyIsm.bZeroInstances = true;
	EmptyIsm.bNanite = true;
	FPrimitiveFacts NotGeometry = Body;
	NotGeometry.bGeometryClass = false;

	const FPrimitiveFacts A[] = { Body, ExcludedNanite };
	int Drawn = 0;
	const int Nanite = CountDrawnNanite(A, 2, Drawn);
	Check(Drawn == 2 && Nanite == 1 && DecideNanite(false, Drawn, Nanite) == ENaniteDecision::Refuse,
		"R5: a visible Nanite part excluded by a target-name pattern still counts, so the actor is refused");
	const FPrimitiveFacts B[] = { Body, FoliageNanite };
	CountDrawnNanite(B, 2, Drawn);
	Check(CountDrawnNanite(B, 2, Drawn) == 1, "R5: the foliage-owner selection exclusion does not hide a Nanite part either");
	const FPrimitiveFacts C[] = { Body, HiddenNanite, EmptyIsm, NotGeometry };
	const int NC = CountDrawnNanite(C, 4, Drawn);
	Check(NC == 0 && Drawn == 1, "R5: a hidden component, an empty instanced mesh and a non-geometry primitive draw nothing");
	Check(NaniteBlocksLabel(false, true, true, 2, 1) && !NaniteBlocksLabel(true, true, true, 2, 1)
		&& !NaniteBlocksLabel(false, true, false, 2, 1) && !NaniteBlocksLabel(false, true, true, 2, 0),
		"R5: mid-event, a frame is blocked only at setting 0, with a target actor, and a drawn Nanite part");
	HiddenNanite.bVisible = true;
	const FPrimitiveFacts D[] = { Body, HiddenNanite };
	Check(CountDrawnNanite(D, 2, Drawn) == 1 && NaniteBlocksLabel(false, true, true, Drawn, 1),
		"R5: a hidden Nanite part made visible mid-event blocks the label from that frame");
}

static void TestR11SplineRoute()
{
	Check(!RouteIsNanite(true, true, false, false, true, true),
		"R11: a spline mesh on a Nanite-enabled asset renders through FSplineMeshSceneProxy, not Nanite");
	Check(RouteIsNanite(true, false, false, false, true, true), "R11: a plain static mesh with Nanite data on a Nanite platform is Nanite");
	Check(!RouteIsNanite(true, false, true, false, true, true) && !RouteIsNanite(true, false, false, true, true, true)
		&& !RouteIsNanite(true, false, false, false, false, true) && !RouteIsNanite(true, false, false, false, true, false)
		&& !RouteIsNanite(false, false, false, false, true, true),
		"R11: bDisallowNanite, the editor fallback, no Nanite data, a non-Nanite platform or a non-static mesh are not Nanite");
	std::vector<FCand> Pool = { { "Spline0", RouteIsNanite(true, true, false, false, true, true) } };
	FKatStream S;
	S.Initialize(4242);
	const FAttempt A = AutoPoolAttempt(S, 2, Pool, false, 3.0f, 6.0f);
	Check(A.Outcome == 2 && A.Target != nullptr && std::string(A.Target) == "Spline0" && A.Draws == 3u,
		"R11 KAT: a pool whose only candidate is a spline on a Nanite asset still fires on it at setting 0 (3 draws)");
	std::vector<FCand> PoolOld = { { "Spline0", true } };
	FKatStream S2;
	S2.Initialize(4242);
	const FAttempt B = AutoPoolAttempt(S2, 2, PoolOld, false, 3.0f, 6.0f);
	Check(B.Outcome == 1 && B.Draws == 1u,
		"R11 KAT control: under the old asset-only classification that pool took the no-candidate path (the defect)");
}

int main()
{
	TestDecision();
	TestFilter();
	TestKat();
	TestR6FailClosed();
	TestR5Enumeration();
	TestR11SplineRoute();
	std::printf("target policy selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
