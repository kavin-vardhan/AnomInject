#include "../Source/AnomalyInjector/Private/Anomalies/TexCorruptPure.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#ifndef DRAW_MUTANT
#define DRAW_MUTANT 0
#endif

struct FKatRow
{
	const char* Case;
	int Seed;
	int Attempt;
	int NumEligible;
	int NumCandidates;
	int Modes[4];
	int NumModeEntries;
	float HoldMin;
	float HoldMax;
	int Outcome;
	int Id;
	int Target;
	unsigned HoldBits;
	int ModeDrawn;
	int Mode;
	unsigned SeedAfter;
	unsigned Draws;
};

static const FKatRow GKat[] =
{
	{ "m53_zero_modes", 4242, 0, 1, 5, { 0, -2, -2, -2 }, 1, 3.0f, 6.0f, 2, 0, 3, 0x40B07B8Eu, 1, -1, 0xA7BF3286u, 4u },
	{ "m53_one_mode", 4242, 0, 1, 5, { 1, -2, -2, -2 }, 1, 3.0f, 6.0f, 2, 0, 3, 0x40B07B8Eu, 1, 0, 0xA7BF3286u, 4u },
	{ "m53_one_mode", 4242, 1, 1, 5, { 1, -2, -2, -2 }, 1, 3.0f, 6.0f, 2, 0, 3, 0x406E8CE8u, 1, 0, 0xA6AA2A3Au, 4u },
	{ "m53_several_modes", 4242, 0, 1, 6, { 2, -2, -2, -2 }, 1, 3.0f, 6.0f, 2, 0, 4, 0x40B07B8Eu, 1, 1, 0xA7BF3286u, 4u },
	{ "m53_several_modes", 4242, 1, 1, 6, { 2, -2, -2, -2 }, 1, 3.0f, 6.0f, 2, 0, 4, 0x406E8CE8u, 1, 1, 0xA6AA2A3Au, 4u },
	{ "m53_several_modes", 4242, 2, 1, 6, { 2, -2, -2, -2 }, 1, 3.0f, 6.0f, 2, 0, 1, 0x40544192u, 1, 0, 0x444F8BAEu, 4u },
	{ "m53_several_modes", 4242, 3, 1, 6, { 4, -2, -2, -2 }, 1, 3.0f, 6.0f, 2, 0, 1, 0x408C5CDEu, 1, 3, 0xD231AAE2u, 4u },
	{ "m53_several_modes", 4242, 4, 1, 6, { 4, -2, -2, -2 }, 1, 3.0f, 6.0f, 2, 0, 1, 0x40886D4Bu, 1, 0, 0x33FA9BD6u, 4u },
	{ "m53_several_modes", 4242, 5, 1, 6, { 4, -2, -2, -2 }, 1, 3.0f, 6.0f, 2, 0, 4, 0x405A44E6u, 1, 3, 0xE970328Au, 4u },
	{ "non_m53_no_mode_draw", 4242, 0, 2, 7, { -1, -1, -2, -2 }, 2, 3.0f, 6.0f, 2, 0, 5, 0x40B07B8Eu, 0, -1, 0xD69ED00Fu, 3u },
	{ "non_m53_no_mode_draw", 4242, 1, 2, 7, { -1, -1, -2, -2 }, 2, 3.0f, 6.0f, 2, 1, 4, 0x40A0742Cu, 0, -1, 0xABE074E8u, 3u },
	{ "non_m53_no_mode_draw", 4242, 2, 2, 7, { -1, -1, -2, -2 }, 2, 3.0f, 6.0f, 2, 0, 4, 0x408D9F03u, 0, -1, 0x79A8096Du, 3u },
	{ "no_candidates_one_draw", 4242, 0, 3, 0, { -1, 2, 2, -2 }, 3, 3.0f, 6.0f, 1, 0, -1, 0x00000000u, 0, -1, 0x1ABE19A5u, 1u },
	{ "no_candidates_one_draw", 4242, 1, 3, 4, { -1, 2, 2, -2 }, 3, 3.0f, 6.0f, 2, 2, 3, 0x409EE7B3u, 1, 1, 0x9C6DF129u, 4u },
	{ "interleaved", 4242, 0, 4, 9, { -1, 2, -1, 2 }, 4, 3.0f, 6.0f, 2, 0, 7, 0x40B07B8Eu, 0, -1, 0xD69ED00Fu, 3u },
	{ "interleaved", 4242, 1, 4, 9, { -1, 2, -1, 2 }, 4, 3.0f, 6.0f, 2, 2, 5, 0x40A0742Cu, 0, -1, 0xABE074E8u, 3u },
	{ "interleaved", 4242, 2, 4, 9, { -1, 2, -1, 2 }, 4, 3.0f, 6.0f, 2, 0, 5, 0x408D9F03u, 0, -1, 0x79A8096Du, 3u },
	{ "interleaved", 4242, 3, 4, 9, { -1, 2, -1, 2 }, 4, 3.0f, 6.0f, 2, 1, 0, 0x40733BA8u, 1, 1, 0x83410671u, 4u },
	{ "interleaved", 4242, 4, 4, 9, { -1, 2, -1, 2 }, 4, 3.0f, 6.0f, 2, 1, 4, 0x40AED2A0u, 1, 1, 0x88844C35u, 4u },
	{ "interleaved", 4242, 5, 4, 9, { -1, 2, -1, 2 }, 4, 3.0f, 6.0f, 2, 1, 3, 0x4066FBF4u, 1, 0, 0x59F7FEB9u, 4u },
	{ "interleaved", 4242, 6, 4, 9, { -1, 2, -1, 2 }, 4, 3.0f, 6.0f, 2, 3, 1, 0x40B78A13u, 1, 1, 0xA8E101FDu, 4u },
	{ "interleaved", 4242, 7, 4, 9, { -1, 2, -1, 2 }, 4, 3.0f, 6.0f, 2, 1, 8, 0x40894F01u, 1, 1, 0xA786FA01u, 4u },
	{ "interleaved", 4242, 8, 4, 9, { -1, 2, -1, 2 }, 4, 3.0f, 6.0f, 2, 1, 6, 0x40911E44u, 1, 0, 0x0E384AC5u, 4u },
	{ "interleaved", 4242, 9, 4, 9, { -1, 2, -1, 2 }, 4, 3.0f, 6.0f, 2, 3, 2, 0x40A3F644u, 1, 1, 0xE90E1849u, 4u },
	{ "empty_set_one_draw_then_none", 99, 0, 1, 3, { 0, -2, -2, -2 }, 1, 3.0f, 6.0f, 2, 0, 1, 0x40B428EEu, 1, -1, 0x3CA92F87u, 4u },
	{ "empty_set_one_draw_then_none", 99, 1, 2, 3, { -1, 0, -2, -2 }, 2, 3.0f, 6.0f, 2, 0, 2, 0x40BB7221u, 0, -1, 0xF3DB0200u, 3u },
	{ "empty_set_one_draw_then_none", 99, 2, 2, 3, { -1, 0, -2, -2 }, 2, 3.0f, 6.0f, 2, 0, 2, 0x408735C0u, 0, -1, 0x688F57A5u, 3u },
	{ "all_excluded_zero_draws", 4242, 0, 0, 5, { -2, -2, -2, -2 }, 0, 3.0f, 6.0f, 0, -1, -1, 0x00000000u, 0, -1, 0x00001092u, 0u },
	{ "all_excluded_zero_draws", 4242, 1, 0, 5, { -2, -2, -2, -2 }, 0, 3.0f, 6.0f, 0, -1, -1, 0x00000000u, 0, -1, 0x00001092u, 0u },
	{ "all_excluded_zero_draws", 4242, 2, 1, 5, { -1, -2, -2, -2 }, 1, 3.0f, 6.0f, 2, 0, 3, 0x40B07B8Eu, 0, -1, 0xD69ED00Fu, 3u },
	{ "negative_seed_odd_hold", -123456, 0, 3, 11, { 2, -1, 3, -2 }, 3, 0.349999994f, 7.9000001f, 2, 0, 10, 0x3F87F37Eu, 1, 0, 0x39296C54u, 4u },
	{ "negative_seed_odd_hold", -123456, 1, 3, 11, { 2, -1, 3, -2 }, 3, 0.349999994f, 7.9000001f, 2, 2, 4, 0x40287012u, 1, 1, 0x5B92AEA8u, 4u },
	{ "negative_seed_odd_hold", -123456, 2, 3, 11, { 2, -1, 3, -2 }, 3, 0.349999994f, 7.9000001f, 2, 0, 0, 0x40700CA2u, 1, 0, 0x768218BCu, 4u },
	{ "negative_seed_odd_hold", -123456, 3, 3, 11, { 2, -1, 3, -2 }, 3, 0.349999994f, 7.9000001f, 2, 0, 2, 0x3EEADF3Du, 1, 1, 0x9DC19E90u, 4u },
	{ "negative_seed_odd_hold", -123456, 4, 3, 11, { 2, -1, 3, -2 }, 3, 0.349999994f, 7.9000001f, 2, 2, 9, 0x4002CE90u, 1, 1, 0x6070F424u, 4u },
};

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

	int GetCurrentSeed() const
	{
		return (int)Seed;
	}
};

template <typename TStream, typename TCandidateCount, typename TModeCount>
TexCorruptPure::FDrawAttemptResult MutantDrawAttempt(TStream& Stream, int NumEligible, TCandidateCount&& CandidateCount,
	TModeCount&& ModeCount, float HoldMin, float HoldMax)
{
	TexCorruptPure::FDrawAttemptResult R;
	if (NumEligible <= 0)
	{
		return R;
	}
	R.IdIndex = Stream.RandHelper(NumEligible);
	R.NumCandidates = CandidateCount(R.IdIndex);
	if (R.NumCandidates <= 0)
	{
		R.Outcome = TexCorruptPure::EDrawOutcome::NoCandidates;
		return R;
	}
	R.TargetIndex = Stream.RandHelper(R.NumCandidates);
	R.Hold = (float)Stream.FRandRange(HoldMin, HoldMax);
	R.NumModes = ModeCount(R.IdIndex);
	if constexpr (DRAW_MUTANT == 3)
	{
		Stream.GetFraction();
	}
	if (R.NumModes >= 0)
	{
		R.bModeDrawn = true;
		if constexpr (DRAW_MUTANT == 1)
		{
			R.ModeIndex = R.NumModes > 0 ? 0 : -1;
		}
		else
		{
			R.ModeFraction = Stream.GetFraction();
			R.ModeIndex = DRAW_MUTANT == 2 ? (R.NumModes > 0 ? 0 : -1)
				: TexCorruptPure::ModeIndexFromFraction(R.ModeFraction, R.NumModes);
		}
	}
	R.Outcome = TexCorruptPure::EDrawOutcome::Drawn;
	return R;
}

static TexCorruptPure::FDrawAttemptResult RunDraw(FKatStream& Stream, const FKatRow& Row)
{
	auto CandidateCount = [&Row](int) { return Row.NumCandidates; };
	auto ModeCount = [&Row](int IdIndex) { return Row.Modes[IdIndex]; };
	if constexpr (DRAW_MUTANT != 0)
	{
		return MutantDrawAttempt(Stream, Row.NumEligible, CandidateCount, ModeCount, Row.HoldMin, Row.HoldMax);
	}
	else
	{
		return TexCorruptPure::DrawAttempt(Stream, Row.NumEligible, CandidateCount, ModeCount, Row.HoldMin, Row.HoldMax);
	}
}

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

static unsigned FloatBits(float F)
{
	unsigned Bits = 0;
	std::memcpy(&Bits, &F, sizeof(Bits));
	return Bits;
}

static std::string RowName(const FKatRow& Row, const char* Field)
{
	char Buf[160];
	std::snprintf(Buf, sizeof(Buf), "%s[%d].%s", Row.Case, Row.Attempt, Field);
	return Buf;
}

struct FObserved
{
	int Outcome;
	int Id;
	int Target;
	unsigned HoldBits;
	int ModeDrawn;
	int Mode;
	unsigned SeedAfter;
	unsigned Draws;
};

static std::vector<FObserved> RunCase(const char* Case, const std::vector<bool>& Applied)
{
	std::vector<FObserved> Out;
	FKatStream Stream;
	bool bStarted = false;
	size_t k = 0;
	for (const FKatRow& Row : GKat)
	{
		if (std::strcmp(Row.Case, Case) != 0)
		{
			continue;
		}
		if (!bStarted)
		{
			Stream.Initialize(Row.Seed);
			bStarted = true;
		}
		const unsigned Before = Stream.Steps;
		const TexCorruptPure::FDrawAttemptResult R = RunDraw(Stream, Row);
		const bool bApplied = k < Applied.size() ? Applied[k] : true;
		++k;
		FObserved O;
		O.Outcome = (int)R.Outcome;
		O.Id = R.IdIndex;
		O.Target = R.TargetIndex;
		O.HoldBits = R.Outcome == TexCorruptPure::EDrawOutcome::Drawn ? FloatBits(R.Hold) : 0u;
		O.ModeDrawn = R.bModeDrawn ? 1 : 0;
		O.Mode = R.ModeIndex;
		O.SeedAfter = (unsigned)Stream.GetCurrentSeed();
		O.Draws = Stream.Steps - Before;
		(void)bApplied;
		Out.push_back(O);
	}
	return Out;
}

int main()
{
	std::vector<std::string> Cases;
	for (const FKatRow& Row : GKat)
	{
		bool bSeen = false;
		for (const std::string& C : Cases)
		{
			bSeen = bSeen || C == Row.Case;
		}
		if (!bSeen)
		{
			Cases.push_back(Row.Case);
		}
	}

	for (const std::string& Case : Cases)
	{
		const std::vector<FObserved> Obs = RunCase(Case.c_str(), std::vector<bool>());
		size_t k = 0;
		for (const FKatRow& Row : GKat)
		{
			if (Case != Row.Case)
			{
				continue;
			}
			const FObserved& O = Obs[k++];
			Check(O.Outcome == Row.Outcome, RowName(Row, "outcome"));
			Check(O.Id == Row.Id, RowName(Row, "id_index"));
			Check(O.Target == Row.Target, RowName(Row, "target_index"));
			Check(O.HoldBits == Row.HoldBits, RowName(Row, "hold_bits"));
			Check(O.ModeDrawn == Row.ModeDrawn, RowName(Row, "mode_drawn"));
			Check(O.Mode == Row.Mode, RowName(Row, "mode_index"));
			Check(O.SeedAfter == Row.SeedAfter, RowName(Row, "seed_after"));
			Check(O.Draws == Row.Draws, RowName(Row, "draws"));
		}
	}

	{
		std::vector<bool> AllApplied(10, true);
		std::vector<bool> Alternating;
		for (int i = 0; i < 10; ++i)
		{
			Alternating.push_back((i % 2) == 0);
		}
		const std::vector<FObserved> A = RunCase("interleaved", AllApplied);
		const std::vector<FObserved> B = RunCase("interleaved", Alternating);
		bool bSame = A.size() == B.size();
		for (size_t i = 0; bSame && i < A.size(); ++i)
		{
			bSame = A[i].SeedAfter == B[i].SeedAfter && A[i].Id == B[i].Id && A[i].Target == B[i].Target
				&& A[i].HoldBits == B[i].HoldBits && A[i].Mode == B[i].Mode;
		}
		Check(bSame, "applied_and_refused_sequences_identical");
	}

	Check(TexCorruptPure::ModeIndexFromFraction(0.0f, 3) == 0, "mode_fraction_zero");
	Check(TexCorruptPure::ModeIndexFromFraction(0.99999994f, 3) == 2, "mode_fraction_top");
	Check(TexCorruptPure::ModeIndexFromFraction(0.34f, 3) == 1, "mode_fraction_mid");
	Check(TexCorruptPure::ModeIndexFromFraction(0.5f, 0) == -1, "mode_fraction_empty_none");
	Check(TexCorruptPure::ModeIndexFromFraction(0.5f, 1) == 0, "mode_fraction_single");

	std::printf("texcorrupt draw test (mutant %d): %d checks, %d failures\n", DRAW_MUTANT, GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
