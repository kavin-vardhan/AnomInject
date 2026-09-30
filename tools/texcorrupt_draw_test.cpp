#include "../Source/AnomalyInjector/Private/Anomalies/TexCorruptPure.h"
#include "../Source/AnomalyInjector/Public/AnomalyTargetPolicy.h"

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

struct FKatTargetedRow
{
	const char* Case;
	int Seed;
	int Fire;
	int IdSlot;
	int ModeGiven;
	int BenchLever;
	int NumModes;
	float HoldMin;
	float HoldMax;
	unsigned HoldBits;
	int RoundRobin;
	int BenchLeverOut;
	int Mode;
	unsigned CounterAfter;
	unsigned SeedAfter;
	unsigned Draws;
};

static const FKatTargetedRow GKatTargeted[] =
{
	{ "rr_two_modes", 4242, 0, 0, 0, 0, 2, 3.0f, 6.0f, 0x40540E92u, 1, 0, 0, 1u, 0x1ABE19A5u, 1u },
	{ "rr_two_modes", 4242, 1, 0, 0, 0, 2, 3.0f, 6.0f, 0x40ABC4CAu, 1, 0, 1, 2u, 0xCA0CC694u, 1u },
	{ "rr_two_modes", 4242, 2, 0, 0, 0, 2, 3.0f, 6.0f, 0x40B07B8Eu, 1, 0, 0, 3u, 0xD69ED00Fu, 1u },
	{ "rr_two_modes", 4242, 3, 0, 0, 0, 2, 3.0f, 6.0f, 0x409EE7B3u, 1, 0, 1, 4u, 0xA7BF3286u, 1u },
	{ "rr_two_modes", 4242, 4, 0, 0, 0, 2, 3.0f, 6.0f, 0x409AA93Au, 1, 0, 0, 5u, 0x9C6DF129u, 1u },
	{ "rr_four_modes", 4242, 0, 0, 0, 0, 4, 3.0f, 6.0f, 0x40540E92u, 1, 0, 0, 1u, 0x1ABE19A5u, 1u },
	{ "rr_four_modes", 4242, 1, 0, 0, 0, 4, 3.0f, 6.0f, 0x40ABC4CAu, 1, 0, 1, 2u, 0xCA0CC694u, 1u },
	{ "rr_four_modes", 4242, 2, 0, 0, 0, 4, 3.0f, 6.0f, 0x40B07B8Eu, 1, 0, 2, 3u, 0xD69ED00Fu, 1u },
	{ "rr_four_modes", 4242, 3, 0, 0, 0, 4, 3.0f, 6.0f, 0x409EE7B3u, 1, 0, 3, 4u, 0xA7BF3286u, 1u },
	{ "rr_four_modes", 4242, 4, 0, 0, 0, 4, 3.0f, 6.0f, 0x409AA93Au, 1, 0, 0, 5u, 0x9C6DF129u, 1u },
	{ "rr_four_modes", 4242, 5, 0, 0, 0, 4, 3.0f, 6.0f, 0x40A0742Cu, 1, 0, 1, 6u, 0xABE074E8u, 1u },
	{ "rr_empty_set", 99, 0, 0, 0, 0, 0, 3.0f, 6.0f, 0x40A6B211u, 1, 0, -1, 0u, 0xBC8583EAu, 1u },
	{ "rr_empty_set", 99, 1, 0, 0, 0, 0, 3.0f, 6.0f, 0x40842422u, 1, 0, -1, 0u, 0x60605ADDu, 1u },
	{ "rr_empty_set", 99, 2, 0, 0, 0, 0, 3.0f, 6.0f, 0x40B428EEu, 1, 0, -1, 0u, 0xE06D272Cu, 1u },
	{ "non_m53_no_rr", 4242, 0, 0, 0, 0, -1, 3.0f, 6.0f, 0x40540E92u, 0, 0, -1, 0u, 0x1ABE19A5u, 1u },
	{ "non_m53_no_rr", 4242, 1, 0, 1, 0, -1, 3.0f, 6.0f, 0x40ABC4CAu, 0, 0, -1, 0u, 0xCA0CC694u, 1u },
	{ "non_m53_no_rr", 4242, 2, 0, 0, 0, -1, 3.0f, 6.0f, 0x40B07B8Eu, 0, 0, -1, 0u, 0xD69ED00Fu, 1u },
	{ "mixed_two_modes", 4242, 0, 0, 0, 0, 2, 3.0f, 6.0f, 0x40540E92u, 1, 0, 0, 1u, 0x1ABE19A5u, 1u },
	{ "mixed_two_modes", 4242, 1, 0, 1, 0, 2, 3.0f, 6.0f, 0x40ABC4CAu, 0, 0, -1, 1u, 0xCA0CC694u, 1u },
	{ "mixed_two_modes", 4242, 2, 0, 0, 0, 2, 3.0f, 6.0f, 0x40B07B8Eu, 1, 0, 1, 2u, 0xD69ED00Fu, 1u },
	{ "mixed_two_modes", 4242, 3, 0, 1, 0, 2, 3.0f, 6.0f, 0x409EE7B3u, 0, 0, -1, 2u, 0xA7BF3286u, 1u },
	{ "mixed_two_modes", 4242, 4, 0, 1, 0, 2, 3.0f, 6.0f, 0x409AA93Au, 0, 0, -1, 2u, 0x9C6DF129u, 1u },
	{ "mixed_two_modes", 4242, 5, 0, 0, 0, 2, 3.0f, 6.0f, 0x40A0742Cu, 1, 0, 0, 3u, 0xABE074E8u, 1u },
	{ "mixed_two_modes", 4242, 6, 0, 0, 0, 2, 3.0f, 6.0f, 0x406E8CE8u, 1, 0, 1, 4u, 0x3E113773u, 1u },
	{ "mixed_four_modes_odd_hold", -123456, 0, 0, 1, 0, 4, 0.349999994f, 7.9000001f, 0x4022A3AFu, 0, 0, -1, 0u, 0x4A4C8C2Bu, 1u },
	{ "mixed_four_modes_odd_hold", -123456, 1, 0, 0, 0, 4, 0.349999994f, 7.9000001f, 0x40E940A4u, 1, 0, 0, 1u, 0xEB499452u, 1u },
	{ "mixed_four_modes_odd_hold", -123456, 2, 0, 0, 0, 4, 0.349999994f, 7.9000001f, 0x3F87F37Eu, 1, 0, 1, 2u, 0x18256065u, 1u },
	{ "mixed_four_modes_odd_hold", -123456, 3, 0, 1, 0, 4, 0.349999994f, 7.9000001f, 0x40024AFCu, 0, 0, -1, 2u, 0x39296C54u, 1u },
	{ "mixed_four_modes_odd_hold", -123456, 4, 0, 0, 0, 4, 0.349999994f, 7.9000001f, 0x40C289E5u, 1, 0, 2, 3u, 0xC24420CFu, 1u },
	{ "mixed_four_modes_odd_hold", -123456, 5, 0, 1, 0, 4, 0.349999994f, 7.9000001f, 0x4054C57Du, 0, 0, -1, 3u, 0x64DBEA46u, 1u },
	{ "mixed_four_modes_odd_hold", -123456, 6, 0, 0, 0, 4, 0.349999994f, 7.9000001f, 0x40287012u, 1, 0, 3, 4u, 0x4D5EFBE9u, 1u },
	{ "mixed_four_modes_odd_hold", -123456, 7, 0, 0, 0, 4, 0.349999994f, 7.9000001f, 0x40433E75u, 1, 0, 0, 5u, 0x5B92AEA8u, 1u },
	{ "two_ids_separate_counters", 777, 0, 0, 0, 0, 2, 3.0f, 6.0f, 0x40A5BC1Fu, 1, 0, 0, 1u, 0xB9F5A848u, 1u },
	{ "two_ids_separate_counters", 777, 1, 1, 0, 0, 4, 3.0f, 6.0f, 0x405A4DC4u, 1, 0, 0, 1u, 0x23125A53u, 1u },
	{ "two_ids_separate_counters", 777, 2, 0, 0, 0, 2, 3.0f, 6.0f, 0x409F30B5u, 1, 0, 1, 2u, 0xA881E29Au, 1u },
	{ "two_ids_separate_counters", 777, 3, 1, 1, 0, 4, 3.0f, 6.0f, 0x40592207u, 0, 0, -1, 1u, 0x2182B54Du, 1u },
	{ "two_ids_separate_counters", 777, 4, 1, 0, 0, 4, 3.0f, 6.0f, 0x409BCE5Cu, 1, 0, 1, 2u, 0x9F7BA05Cu, 1u },
	{ "two_ids_separate_counters", 777, 5, 0, 0, 0, 2, 3.0f, 6.0f, 0x404A47C4u, 1, 0, 0, 3u, 0x0DB50677u, 1u },
	{ "two_ids_separate_counters", 777, 6, 1, 0, 0, 4, 3.0f, 6.0f, 0x40449690u, 1, 0, 2, 3u, 0x061E160Eu, 1u },
	{ "bench_lever_mixed", 4242, 0, 0, 0, 0, 2, 3.0f, 6.0f, 0x40540E92u, 1, 0, 0, 1u, 0x1ABE19A5u, 1u },
	{ "bench_lever_mixed", 4242, 1, 0, 0, 1, 2, 3.0f, 6.0f, 0x40ABC4CAu, 0, 1, -1, 1u, 0xCA0CC694u, 1u },
	{ "bench_lever_mixed", 4242, 2, 0, 0, 0, 2, 3.0f, 6.0f, 0x40B07B8Eu, 1, 0, 1, 2u, 0xD69ED00Fu, 1u },
	{ "bench_lever_mixed", 4242, 3, 0, 0, 1, 2, 3.0f, 6.0f, 0x409EE7B3u, 0, 1, -1, 2u, 0xA7BF3286u, 1u },
	{ "bench_lever_mixed", 4242, 4, 0, 1, 0, 2, 3.0f, 6.0f, 0x409AA93Au, 0, 0, -1, 2u, 0x9C6DF129u, 1u },
	{ "bench_lever_mixed", 4242, 5, 0, 0, 0, 2, 3.0f, 6.0f, 0x40A0742Cu, 1, 0, 0, 3u, 0xABE074E8u, 1u },
	{ "bench_lever_mixed", 4242, 6, 0, 0, 1, 2, 3.0f, 6.0f, 0x406E8CE8u, 0, 1, -1, 3u, 0x3E113773u, 1u },
	{ "bench_lever_mixed", 4242, 7, 0, 0, 0, 2, 3.0f, 6.0f, 0x409E7FD0u, 1, 0, 1, 4u, 0xA6AA2A3Au, 1u },
	{ "bench_lever_empty_set", 99, 0, 0, 0, 1, 0, 3.0f, 6.0f, 0x40A6B211u, 0, 1, -1, 0u, 0xBC8583EAu, 1u },
	{ "bench_lever_empty_set", 99, 1, 0, 0, 0, 0, 3.0f, 6.0f, 0x40842422u, 1, 0, -1, 0u, 0x60605ADDu, 1u },
	{ "bench_lever_mode_given_wins", 4242, 0, 0, 1, 1, 2, 3.0f, 6.0f, 0x40540E92u, 0, 0, -1, 0u, 0x1ABE19A5u, 1u },
	{ "bench_lever_mode_given_wins", 4242, 1, 0, 0, 0, 2, 3.0f, 6.0f, 0x40ABC4CAu, 1, 0, 0, 1u, 0xCA0CC694u, 1u },
	{ "bench_lever_non_m53", 4242, 0, 0, 0, 1, -1, 3.0f, 6.0f, 0x40540E92u, 0, 0, -1, 0u, 0x1ABE19A5u, 1u },
	{ "bench_lever_non_m53", 4242, 1, 0, 0, 0, -1, 3.0f, 6.0f, 0x40ABC4CAu, 0, 0, -1, 0u, 0xCA0CC694u, 1u },
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

static void Check(bool bOk, const std::string& What);

struct FNaniteKatObserved
{
	int Outcome = -1;
	std::string Target;
	int TargetIndex = -1;
	int NumCandidates = 0;
	unsigned SeedAfter = 0;
	unsigned Draws = 0;
};

static std::vector<FNaniteKatObserved> RunNaniteKat(bool bAllowNanite, bool bAllNanite)
{
	static const char* Pool[] = { "Rock0", "Wall1", "Rock2", "Wall3", "Rock4", "Rock5", "Rock6" };
	FKatStream Stream;
	Stream.Initialize(4242);
	std::vector<FNaniteKatObserved> Out;
	for (int k = 0; k < 3; ++k)
	{
		std::vector<std::string> Cands;
		for (const char* P : Pool)
		{
			Cands.push_back(bAllNanite ? std::string("Wall") + (P + 4) : std::string(P));
		}
		auto CandidateCount = [&Cands, bAllowNanite](int) -> int
		{
			const int Kept = AnomalyTargetPolicy::FilterNaniteCandidates(Cands.data(), (int)Cands.size(), bAllowNanite,
				[](const std::string& C) { return C.rfind("Wall", 0) == 0; });
			Cands.resize((size_t)Kept);
			return Kept;
		};
		auto ModeCount = [](int) { return -1; };
		const unsigned Before = Stream.Steps;
		TexCorruptPure::FDrawAttemptResult R;
		if constexpr (DRAW_MUTANT != 0)
		{
			R = MutantDrawAttempt(Stream, 2, CandidateCount, ModeCount, 3.0f, 6.0f);
		}
		else
		{
			R = TexCorruptPure::DrawAttempt(Stream, 2, CandidateCount, ModeCount, 3.0f, 6.0f);
		}
		FNaniteKatObserved O;
		O.Outcome = (int)R.Outcome;
		O.NumCandidates = R.NumCandidates;
		O.TargetIndex = R.TargetIndex;
		O.Target = (R.TargetIndex >= 0 && R.TargetIndex < (int)Cands.size()) ? Cands[(size_t)R.TargetIndex] : std::string();
		O.SeedAfter = (unsigned)Stream.GetCurrentSeed();
		O.Draws = Stream.Steps - Before;
		Out.push_back(O);
	}
	return Out;
}

static void TestNaniteKat()
{
	static const unsigned Seeds[3] = { 0xD69ED00Fu, 0xABE074E8u, 0x79A8096Du };
	const std::vector<FNaniteKatObserved> Allow = RunNaniteKat(true, false);
	const std::vector<FNaniteKatObserved> Skip = RunNaniteKat(false, false);
	const std::vector<FNaniteKatObserved> All = RunNaniteKat(false, true);
	static const char* AllowTargets[3] = { "Rock5", "Rock4", "Rock4" };
	for (int k = 0; k < 3; ++k)
	{
		const std::string N = "nanite_kat[" + std::to_string(k) + "]";
		Check(Allow[k].Outcome == (int)TexCorruptPure::EDrawOutcome::Drawn && Allow[k].NumCandidates == 7
			&& Allow[k].Target == AllowTargets[k] && Allow[k].SeedAfter == Seeds[k] && Allow[k].Draws == 3u,
			N + " setting 1: the production DrawAttempt reproduces the non_m53_no_mode_draw known answers on the unfiltered pool");
		Check(Skip[k].Outcome == (int)TexCorruptPure::EDrawOutcome::Drawn && Skip[k].NumCandidates == 5
			&& Skip[k].Target == "Rock5" && Skip[k].TargetIndex == 3 && Skip[k].SeedAfter == Seeds[k] && Skip[k].Draws == 3u,
			N + " setting 0: the two Nanite candidates are dropped before the target draw; still 3 draws and the same seed "
			"after, target index 3 of 5 = Rock5 (the independent Python replica's answer)");
		Check(All[k].Outcome == (int)TexCorruptPure::EDrawOutcome::NoCandidates && All[k].Draws == 1u,
			N + " every candidate Nanite: the id draw then the no-candidate path (1 draw)");
	}
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

template <typename TStream>
TexCorruptPure::FTargetedAttemptResult MutantTargetedAttempt(TStream& Stream, float HoldMin, float HoldMax, bool bModeGiven,
	int NumModes, unsigned& RoundRobinCounter, bool bBenchLeverMode)
{
	TexCorruptPure::FTargetedAttemptResult R;
	R.Hold = (float)Stream.FRandRange(HoldMin, HoldMax);
	R.NumModes = NumModes;
	if constexpr (DRAW_MUTANT == 6)
	{
		if (bModeGiven && NumModes >= 0)
		{
			++RoundRobinCounter;
		}
	}
	if (!bModeGiven && NumModes >= 0 && bBenchLeverMode && DRAW_MUTANT != 7)
	{
		R.bBenchLever = true;
		if constexpr (DRAW_MUTANT == 8)
		{
			++RoundRobinCounter;
		}
		return R;
	}
	if (!bModeGiven && NumModes >= 0)
	{
		R.bRoundRobin = true;
		if constexpr (DRAW_MUTANT == 4)
		{
			R.ModeIndex = NumModes > 0 ? Stream.RandHelper(NumModes) : -1;
		}
		else if constexpr (DRAW_MUTANT == 5)
		{
			R.ModeIndex = NumModes > 0 ? 0 : -1;
		}
		else
		{
			R.ModeIndex = TexCorruptPure::RoundRobinNext(RoundRobinCounter, NumModes);
		}
	}
	return R;
}

static TexCorruptPure::FTargetedAttemptResult TargetedCall(FKatStream& Stream, float HoldMin, float HoldMax, bool bModeGiven,
	int NumModes, unsigned& Counter, bool bBenchLever)
{
	if constexpr (DRAW_MUTANT >= 4)
	{
		return MutantTargetedAttempt(Stream, HoldMin, HoldMax, bModeGiven, NumModes, Counter, bBenchLever);
	}
	else
	{
		return TexCorruptPure::TargetedAttempt(Stream, HoldMin, HoldMax, bModeGiven, NumModes, Counter, bBenchLever);
	}
}

static TexCorruptPure::FTargetedAttemptResult RunTargeted(FKatStream& Stream, const FKatTargetedRow& Row, unsigned& Counter)
{
	return TargetedCall(Stream, Row.HoldMin, Row.HoldMax, Row.ModeGiven != 0, Row.NumModes, Counter, Row.BenchLever != 0);
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

static std::string TargetedRowName(const FKatTargetedRow& Row, const char* Field)
{
	char Buf[160];
	std::snprintf(Buf, sizeof(Buf), "targeted:%s[%d].%s", Row.Case, Row.Fire, Field);
	return Buf;
}

struct FTargetedObserved
{
	unsigned HoldBits;
	int RoundRobin;
	int BenchLever;
	int Mode;
	unsigned CounterAfter;
	unsigned SeedAfter;
	unsigned Draws;
	unsigned BareSeedAfter;
	unsigned BareSteps;
	unsigned BareHoldBits;
	unsigned Steps;
};

static std::vector<FTargetedObserved> RunTargetedCase(const char* Case)
{
	std::vector<FTargetedObserved> Out;
	FKatStream Stream;
	FKatStream Bare;
	unsigned Counters[2] = { 0u, 0u };
	bool bStarted = false;
	for (const FKatTargetedRow& Row : GKatTargeted)
	{
		if (std::strcmp(Row.Case, Case) != 0)
		{
			continue;
		}
		if (!bStarted)
		{
			Stream.Initialize(Row.Seed);
			Bare.Initialize(Row.Seed);
			bStarted = true;
		}
		const unsigned Before = Stream.Steps;
		const TexCorruptPure::FTargetedAttemptResult R = RunTargeted(Stream, Row, Counters[Row.IdSlot]);
		const float BareHold = (float)Bare.FRandRange(Row.HoldMin, Row.HoldMax);
		FTargetedObserved O;
		O.HoldBits = FloatBits(R.Hold);
		O.RoundRobin = R.bRoundRobin ? 1 : 0;
		O.BenchLever = R.bBenchLever ? 1 : 0;
		O.Mode = R.ModeIndex;
		O.CounterAfter = Counters[Row.IdSlot];
		O.SeedAfter = (unsigned)Stream.GetCurrentSeed();
		O.Draws = Stream.Steps - Before;
		O.Steps = Stream.Steps;
		O.BareSeedAfter = (unsigned)Bare.GetCurrentSeed();
		O.BareSteps = Bare.Steps;
		O.BareHoldBits = FloatBits(BareHold);
		Out.push_back(O);
	}
	return Out;
}

static const char* ApplyNoModeSelection(int TileProbe, bool bIdentityRedraw, bool bIdentity, bool bUv)
{
	switch (TexCorruptPure::NoModeLever(TileProbe, bIdentityRedraw, bIdentity, false, bUv))
	{
	case TexCorruptPure::ENoModeLever::TileProbe:      return "tile_probe";
	case TexCorruptPure::ENoModeLever::IdentityRedraw: return "identity_redraw";
	case TexCorruptPure::ENoModeLever::Identity:       return "identity";
	default:                                           return "no_mode";
	}
}

static void TestTargetedToApplySelection()
{
	using TexCorruptPure::ENoModeLever;
	struct FLeverRow
	{
		int TileProbe;
		int IdentityRedraw;
		int Identity;
		int AutoPool;
		int Uv;
		ENoModeLever Expect;
	};
	static const FLeverRow Rows[] =
	{
		{ 0, 0, 0, 0, 1, ENoModeLever::None },
		{ 0, 0, 1, 0, 1, ENoModeLever::Identity },
		{ 0, 0, 1, 0, 0, ENoModeLever::Identity },
		{ 0, 1, 0, 0, 0, ENoModeLever::IdentityRedraw },
		{ 0, 1, 1, 0, 1, ENoModeLever::IdentityRedraw },
		{ 2, 0, 0, 0, 1, ENoModeLever::TileProbe },
		{ 4, 1, 1, 0, 1, ENoModeLever::TileProbe },
		{ 2, 0, 0, 0, 0, ENoModeLever::None },
		{ 4, 0, 1, 0, 0, ENoModeLever::Identity },
		{ 2, 0, 0, 1, 1, ENoModeLever::None },
		{ 3, 0, 0, 0, 1, ENoModeLever::None },
		{ 2, 0, 1, 1, 1, ENoModeLever::Identity },
	};
	for (const FLeverRow& L : Rows)
	{
		char Buf[96];
		std::snprintf(Buf, sizeof(Buf), "no_mode_lever[tp=%d ir=%d id=%d auto=%d uv=%d]", L.TileProbe, L.IdentityRedraw, L.Identity,
			L.AutoPool, L.Uv);
		Check(TexCorruptPure::NoModeLever(L.TileProbe, L.IdentityRedraw != 0, L.Identity != 0, L.AutoPool != 0, L.Uv != 0) == L.Expect,
			Buf);
	}

	const int TileProbes[] = { 0, 2, 4 };
	for (int Tp : TileProbes)
	{
		for (int Ir = 0; Ir <= 1; ++Ir)
		{
			for (int Id = 0; Id <= 1; ++Id)
			{
				for (int Uv = 0; Uv <= 1; ++Uv)
				{
					for (int Given = 0; Given <= 1; ++Given)
					{
						const bool bUv = Uv != 0;
						const char* Expected = Given ? "argument"
							: ((Tp == 2 || Tp == 4) && bUv) ? "tile_probe"
							: Ir ? "identity_redraw"
							: Id ? "identity"
							: "round_robin";
						const ENoModeLever Lever = Given ? ENoModeLever::None
							: TexCorruptPure::NoModeLever(Tp, Ir != 0, Id != 0, false, bUv);
						FKatStream Stream;
						Stream.Initialize(4242);
						unsigned Counter = 5u;
						const TexCorruptPure::FTargetedAttemptResult R = TargetedCall(Stream, 3.0f, 6.0f, Given != 0, 2, Counter,
							Lever != ENoModeLever::None);
						const bool bInserted = R.bRoundRobin && R.ModeIndex >= 0;
						const char* Selected = Given ? "argument"
							: bInserted ? "round_robin"
							: ApplyNoModeSelection(Tp, Ir != 0, Id != 0, bUv);
						char Buf[128];
						std::snprintf(Buf, sizeof(Buf), "targeted_to_apply[tp=%d ir=%d id=%d uv=%d given=%d]", Tp, Ir, Id, Uv, Given);
						const std::string Name(Buf);
						Check(std::strcmp(Selected, Expected) == 0, Name + ".apply_selects_" + Expected + "_got_" + Selected);
						Check(Stream.Steps == 1u, Name + ".exactly_one_draw");
						const bool bRr = std::strcmp(Expected, "round_robin") == 0;
						Check(Counter == (bRr ? 6u : 5u), Name + ".counter_advances_only_on_round_robin");
						Check(!bRr || R.ModeIndex == 1, Name + ".round_robin_index");
						Check(R.bBenchLever == (Lever != ENoModeLever::None && !Given), Name + ".mode_source_bench_lever");
					}
				}
			}
		}
	}
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

	{
		std::vector<std::string> TargetedCases;
		for (const FKatTargetedRow& Row : GKatTargeted)
		{
			bool bSeen = false;
			for (const std::string& C : TargetedCases)
			{
				bSeen = bSeen || C == Row.Case;
			}
			if (!bSeen)
			{
				TargetedCases.push_back(Row.Case);
			}
		}

		int RoundRobinWithMode = 0;
		int GivenOnM53 = 0;
		int EmptySetRoundRobin = 0;
		int NonM53 = 0;
		int LeverNoMode = 0;
		for (const std::string& Case : TargetedCases)
		{
			const std::vector<FTargetedObserved> Obs = RunTargetedCase(Case.c_str());
			size_t k = 0;
			bool bEquivalent = true;
			for (const FKatTargetedRow& Row : GKatTargeted)
			{
				if (Case != Row.Case)
				{
					continue;
				}
				const FTargetedObserved& O = Obs[k++];
				Check(O.HoldBits == Row.HoldBits, TargetedRowName(Row, "hold_bits"));
				Check(O.RoundRobin == Row.RoundRobin, TargetedRowName(Row, "round_robin"));
				Check(O.BenchLever == Row.BenchLeverOut, TargetedRowName(Row, "bench_lever"));
				Check(O.Mode == Row.Mode, TargetedRowName(Row, "mode_index"));
				Check(O.CounterAfter == Row.CounterAfter, TargetedRowName(Row, "counter_after"));
				Check(O.SeedAfter == Row.SeedAfter, TargetedRowName(Row, "seed_after"));
				Check(O.Draws == Row.Draws, TargetedRowName(Row, "draws"));
				Check(O.Draws == 1u, TargetedRowName(Row, "exactly_one_draw"));
				bEquivalent = bEquivalent && O.SeedAfter == O.BareSeedAfter && O.Steps == O.BareSteps
					&& O.HoldBits == O.BareHoldBits;
				if (Row.NumModes > 0 && Row.ModeGiven == 0)
				{
					++RoundRobinWithMode;
				}
				if (Row.NumModes >= 0 && Row.ModeGiven != 0)
				{
					++GivenOnM53;
				}
				if (Row.NumModes == 0 && Row.ModeGiven == 0)
				{
					++EmptySetRoundRobin;
				}
				if (Row.NumModes < 0)
				{
					++NonM53;
				}
				if (Row.BenchLever != 0 && Row.ModeGiven == 0 && Row.NumModes >= 0)
				{
					++LeverNoMode;
				}
			}
			Check(bEquivalent, "targeted:" + Case + ".same_stream_as_bare_frandrange");
		}
		Check(RoundRobinWithMode > 0 && GivenOnM53 > 0 && EmptySetRoundRobin > 0 && NonM53 > 0 && LeverNoMode > 0,
			"targeted_table_covers_every_shape");
	}

	TestTargetedToApplySelection();

	{
		unsigned C = 7u;
		Check(TexCorruptPure::RoundRobinNext(C, 0) == -1 && C == 7u, "round_robin_empty_counter_unchanged");
		Check(TexCorruptPure::RoundRobinNext(C, -1) == -1 && C == 7u, "round_robin_negative_counter_unchanged");
		unsigned D = 0u;
		const int A0 = TexCorruptPure::RoundRobinNext(D, 3);
		const int A1 = TexCorruptPure::RoundRobinNext(D, 3);
		const int A2 = TexCorruptPure::RoundRobinNext(D, 3);
		const int A3 = TexCorruptPure::RoundRobinNext(D, 3);
		Check(A0 == 0 && A1 == 1 && A2 == 2 && A3 == 0 && D == 4u, "round_robin_cycles_in_order");
		unsigned E = 0u;
		Check(TexCorruptPure::RoundRobinNext(E, 1) == 0 && TexCorruptPure::RoundRobinNext(E, 1) == 0 && E == 2u, "round_robin_single_mode");
	}

	Check(TexCorruptPure::ModeIndexFromFraction(0.0f, 3) == 0, "mode_fraction_zero");
	Check(TexCorruptPure::ModeIndexFromFraction(0.99999994f, 3) == 2, "mode_fraction_top");
	Check(TexCorruptPure::ModeIndexFromFraction(0.34f, 3) == 1, "mode_fraction_mid");
	Check(TexCorruptPure::ModeIndexFromFraction(0.5f, 0) == -1, "mode_fraction_empty_none");
	Check(TexCorruptPure::ModeIndexFromFraction(0.5f, 1) == 0, "mode_fraction_single");

	TestNaniteKat();

	std::printf("texcorrupt draw test (mutant %d): %d checks, %d failures\n", DRAW_MUTANT, GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
