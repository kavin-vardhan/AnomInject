#include <cmath>
#include <cstdio>
#include <string>

#include "TexCorruptPure.h"

using namespace TexCorruptPure;

static int GFailures = 0;
static int GChecks = 0;

static void Check(const char* Name, const std::string& Expected, const std::string& Actual)
{
	++GChecks;
	const bool bPass = Expected == Actual;
	if (!bPass)
	{
		++GFailures;
	}
	std::printf("%-4s %-58s expected=%-14s actual=%s\n", bPass ? "PASS" : "FAIL", Name, Expected.c_str(), Actual.c_str());
}

static std::string MiB(long long Bytes)
{
	char Buf[64];
	std::snprintf(Buf, sizeof(Buf), "%.2f", (double)Bytes / (1024.0 * 1024.0));
	return Buf;
}

static std::string Int(long long V)
{
	return std::to_string(V);
}

static std::string Bool(bool V)
{
	return V ? "true" : "false";
}

static FReqTex Tex(long long Id, int W, int H, bool bSRGB)
{
	FReqTex T;
	T.Id = Id;
	T.W = W;
	T.H = H;
	T.M = FullChainLength(W, H);
	T.bSRGB = bSRGB;
	return T;
}

static std::string Requirement(const FReqTex* T, int N)
{
	return MiB(EventRequirement(T, N).Total());
}

static const char* ClassName(EClassP C)
{
	switch (C)
	{
	case EClassP::Colour: return "colour";
	case EClassP::Data:   return "data";
	case EClassP::Normal: return "normal";
	default:              return "refused";
	}
}

int main()
{
	std::printf("m53 TexCorruptPure offline checks (the header the plugin compiles)\n\n[1] R3.2 budget table, full chains\n");
	const int Sizes[] = { 256, 1024, 2048, 4096 };
	const char* ExpectB[] = { "0.33", "5.33", "21.33", "85.33" };
	const char* ExpectT[] = { "0.08", "1.33", "5.33", "21.33" };
	const char* ExpectBT[] = { "0.42", "6.67", "26.67", "106.67" };
	for (int i = 0; i < 4; ++i)
	{
		const int S = Sizes[i];
		const int M = FullChainLength(S, S);
		const std::string Tag = std::to_string(S) + "^2";
		Check((Tag + " chain B (MiB)").c_str(), ExpectB[i], MiB(ChainBytes(S, S, M)));
		Check((Tag + " scratch T (MiB)").c_str(), ExpectT[i], MiB(ScratchBytes(S, S, M)));
		Check((Tag + " B+T (MiB)").c_str(), ExpectBT[i], MiB(ChainBytes(S, S, M) + ScratchBytes(S, S, M)));
	}
	Check("4096^2 full chain length", "13", Int(FullChainLength(4096, 4096)));
	Check("single-mip 4096^2 chain has no scratch", "0.00", MiB(ScratchBytes(4096, 4096, 1)));

	std::printf("\n[2] P3 and R14 event requirements (scratch shared per (W,H,sRGB) class)\n");
	{
		const FReqTex Same[] = { Tex(1, 4096, 4096, true), Tex(2, 4096, 4096, true) };
		Check("P3 two same-class 4096^2 maps", "192.00", Requirement(Same, 2));
		const FReqTex Diff[] = { Tex(1, 4096, 4096, true), Tex(2, 4096, 4096, false) };
		Check("P3 two different-class 4096^2 maps", "213.33", Requirement(Diff, 2));
		const FReqTex One[] = { Tex(1, 4096, 4096, true) };
		Check("one 4096^2 map", "106.67", Requirement(One, 1));
		const FReqTex Dup[] = { Tex(7, 4096, 4096, true), Tex(7, 4096, 4096, true) };
		Check("the same texture under two bindings counts once", "106.67", Requirement(Dup, 2));
		const FReqTex Floor[] = { Tex(1, 1024, 1024, false) };
		Check("MainWorld SM_FloorBase T_Grid_A", "6.67", Requirement(Floor, 1));
		const FReqTex Cube[] = { Tex(1, 2048, 2048, true), Tex(2, 2048, 2048, false), Tex(3, 1024, 1024, false) };
		Check("Lyra cube, three maps", "60.00", Requirement(Cube, 3));
		const FReqTex CubeO[] = { Tex(1, 2048, 2048, true), Tex(2, 2048, 2048, false), Tex(3, 1024, 1024, false), Tex(4, 2048, 2048, false) };
		Check("Lyra cube with the opacity map", "81.33", Requirement(CubeO, 4));
		const FReqTex Weapon[] = { Tex(1, 2048, 2048, true), Tex(2, 2048, 2048, false), Tex(3, 2048, 2048, false), Tex(4, 2048, 2048, false) };
		Check("Lyra weapon UV set (LODBias 1 -> 2048^2)", "96.00", Requirement(Weapon, 4));
		const FReqTex Rock[] = { Tex(1, 4096, 4096, true), Tex(2, 4096, 4096, false), Tex(3, 4096, 4096, false), Tex(4, 2048, 2048, false) };
		Check("MainWorld rock UV set", "325.33", Requirement(Rock, 4));
		const FReqTex Flats[] = { Tex(1, 2048, 2048, true), Tex(2, 4096, 4096, true) };
		Check("SM_RockFlats with the sand map", "133.33", Requirement(Flats, 2));
		const FReqTex Char[] = { Tex(1, 4096, 4096, true), Tex(2, 4096, 4096, false), Tex(3, 4096, 4096, false) };
		Check("Lyra character set (D, N, MSK)", "298.67", Requirement(Char, 3));
	}

	std::printf("\n[3] V2 fit against the run-wide cap (never downsampled)\n");
	{
		const long long MiBy = 1024LL * 1024LL;
		const FReqTex One[] = { Tex(1, 4096, 4096, true) };
		const FReqTex Two[] = { Tex(1, 4096, 4096, true), Tex(2, 4096, 4096, true) };
		const long long R1 = EventRequirement(One, 1).Total();
		const long long R2 = EventRequirement(Two, 2).Total();
		Check("106.67 MiB at cap 64", "false", Bool(Fits(R1, 64 * MiBy, 0, 0)));
		Check("106.67 MiB at cap 128", "true", Bool(Fits(R1, 128 * MiBy, 0, 0)));
		Check("192 MiB at cap 128 (over_budget)", "false", Bool(Fits(R2, 128 * MiBy, 0, 0)));
		Check("192 MiB at cap 256", "true", Bool(Fits(R2, 256 * MiBy, 0, 0)));
		Check("106.67 MiB at cap 128 with 30 MiB pending_release", "false", Bool(Fits(R1, 128 * MiBy, 0, 30 * MiBy)));
		Check("106.67 MiB at cap 128 with 20 MiB live + 1 MiB pending", "true", Bool(Fits(R1, 128 * MiBy, 20 * MiBy, 1 * MiBy)));
		Check("exact fit is admitted", "true", Bool(Fits(128 * MiBy, 128 * MiBy, 0, 0)));
		Check("one byte over is refused", "false", Bool(Fits(128 * MiBy + 1, 128 * MiBy, 0, 0)));
	}

	std::printf("\n[4] T6 chain shape\n");
	{
		int W[16];
		int H[16];
		for (int m = 0; m < 13; ++m) { W[m] = 4096 >> m; H[m] = 4096 >> m; }
		Check("4096^2 full chain of 13", "true", Bool(IsAdmittedChainShape(13, 4096, 4096, W, H)));
		Check("4096^2 chain of 12 (neither 1 nor full)", "false", Bool(IsAdmittedChainShape(12, 4096, 4096, W, H)));
		Check("single mip", "true", Bool(IsAdmittedChainShape(1, 4096, 4096, W, H)));
		for (int m = 0; m < 9; ++m) { W[m] = (256 >> m) > 0 ? (256 >> m) : 1; H[m] = (64 >> m) > 0 ? (64 >> m) : 1; }
		Check("256x64 full chain of 9", "true", Bool(IsAdmittedChainShape(9, 256, 64, W, H)));
		H[3] = 4;
		Check("256x64 with a wrong level size", "false", Bool(IsAdmittedChainShape(9, 256, 64, W, H)));
		const int NW[] = { 300, 150, 75, 37, 18, 9, 4, 2, 1 };
		const int NH[] = { 200, 100, 50, 25, 12, 6, 3, 1, 1 };
		Check("300x200 non-power-of-two full chain of 9", "true", Bool(IsAdmittedChainShape(9, 300, 200, NW, NH)));
		Check("zero width", "false", Bool(IsAdmittedChainShape(1, 0, 4, NW, NH)));
		Check("non-spatial 1x1 single mip", "true", Bool(IsNonSpatial(1, 1, 1)));
		Check("1x256 is spatial", "false", Bool(IsNonSpatial(1, 1, 256)));
	}

	std::printf("\n[5] T5 format classes (R3.1 allowlist)\n");
	{
		struct FRow { EFmt Fmt; bool bSRGB; bool bNormal; const char* Expect; const char* Name; };
		const FRow Rows[] = {
			{ EFmt::DXT1, true, false, "colour", "DXT1 sRGB" },
			{ EFmt::DXT5, true, false, "colour", "DXT5 sRGB" },
			{ EFmt::BC7, true, false, "colour", "BC7 sRGB" },
			{ EFmt::BGRA8, true, false, "colour", "BGRA8 sRGB" },
			{ EFmt::DXT1, false, false, "data", "DXT1 linear" },
			{ EFmt::DXT5, false, false, "data", "DXT5 linear" },
			{ EFmt::BC7, false, false, "data", "BC7 linear" },
			{ EFmt::BGRA8, false, false, "data", "BGRA8 linear" },
			{ EFmt::BC4, false, false, "data", "BC4 linear" },
			{ EFmt::G8, false, false, "data", "G8 linear" },
			{ EFmt::BC4, true, false, "refused", "BC4 sRGB" },
			{ EFmt::G8, true, false, "refused", "G8 sRGB" },
			{ EFmt::BC5, false, true, "normal", "BC5 normal map" },
			{ EFmt::BC5, false, false, "refused", "BC5 not a normal map" },
			{ EFmt::DXT5, false, true, "refused", "normal map in DXT5" },
			{ EFmt::BGRA8, false, true, "refused", "normal map uncompressed" },
			{ EFmt::Other, false, false, "refused", "float/16-bit/BC6H/LQ" },
		};
		for (const FRow& R : Rows)
		{
			EClassWhy Why = EClassWhy::Ok;
			Check(R.Name, R.Expect, ClassName(ClassifyFormat(R.Fmt, R.bSRGB, R.bNormal, Why)));
		}
	}

	std::printf("\n[6] Decision-tree precedence (A4 first failing binding, V1 earliest slot)\n");
	{
		Check("step order S8 < T1 < T10 < A2 < A3 < A5 < A6", "true",
			Bool(Rank::S8 < Rank::ForBindingStep(1) && Rank::ForBindingStep(10) < Rank::A2 && Rank::A2 < Rank::A3
				&& Rank::A3 < Rank::A5 && Rank::A5 < Rank::A6));
		Check("S3 host_mid precedes S4..S8", "true", Bool(Rank::S3 < Rank::S4 && Rank::S4 < Rank::S5 && Rank::S7 < Rank::S8));
		const int Steps[] = { 0, 10, 3, 3, 5 };
		const bool Req[] = { true, true, true, true, true };
		Check("A4 picks the lowest failing T step (T3 over T10, T5)", "2", Int(PickFirstFailing(Steps, Req, 5)));
		const bool ReqNot[] = { true, true, false, true, true };
		Check("A4 ignores a non-required failing binding", "3", Int(PickFirstFailing(Steps, ReqNot, 5)));
		const int AllOk[] = { 0, 0, 0 };
		const bool Req3[] = { true, true, true };
		Check("A4 all transformable -> no failing binding", "-1", Int(PickFirstFailing(AllOk, Req3, 3)));
		const int Ranks[] = { 1, 13, 3, 20, 3 };
		const bool Untouched[] = { true, false, false, false, false };
		const bool Qualified[] = { false, false, false, false, false };
		Check("V1 earliest refused slot, first on a tie (S3 at index 2)", "2", Int(PickEarliestSlot(Ranks, Untouched, Qualified, 5)));
		const bool AllUntouched[] = { true, true };
		const int TwoRanks[] = { 1, 2 };
		const bool TwoQual[] = { false, false };
		Check("V1 all untouched -> no_eligible_slot", "-1", Int(PickEarliestSlot(TwoRanks, AllUntouched, TwoQual, 2)));
		const bool OneQual[] = { false, false, true, false, false };
		Check("V1 skips a qualified slot", "4", Int(PickEarliestSlot(Ranks, Untouched, OneQual, 5)));
		Check("A2 no_normal_map ranks after every T step", "true", Bool(Rank::A2 > Rank::ForBindingStep(10)));
	}

	std::printf("\n[7] Source mip per level (R3.4.6) and the mipshift lever\n");
	{
		Check("identity level 5 of 13", "5", Int(SourceMipFor(5, 13, 1, false)));
		Check("tile x2 level 0 of 11", "1", Int(SourceMipFor(0, 11, 2, false)));
		Check("tile x4 level 3 of 11", "5", Int(SourceMipFor(3, 11, 4, false)));
		Check("tile x4 last level clamps", "10", Int(SourceMipFor(10, 11, 4, false)));
		Check("mipshift level 2 of 9", "3", Int(SourceMipFor(2, 9, 1, true)));
		Check("mipshift last level clamps", "8", Int(SourceMipFor(8, 9, 1, true)));
		Check("single-mip source", "0", Int(SourceMipFor(0, 1, 4, true)));
	}

	std::printf("\n[8] Runtime LOD bias predicates (E5, T9 streaming_budget, fix C)\n");
	{
		Check("E5: MipBias 1, UsePerTextureBias 0", "true", Bool(GlobalStreamingBias(0, 1.0f)));
		Check("E5: MipBias 1, UsePerTextureBias 1", "false", Bool(GlobalStreamingBias(1, 1.0f)));
		Check("E5: MipBias 0", "false", Bool(GlobalStreamingBias(0, 0.0f)));
		Check("T9 streaming_budget: streamed, per-texture on, MipBias 1", "true", Bool(StreamingBudgetPossible(true, 1, 1.0f)));
		Check("T9 streaming_budget: streamed, MipBias 0.5 (strict, > 0)", "true", Bool(StreamingBudgetPossible(true, 1, 0.5f)));
		Check("T9 streaming_budget: MipBias 0", "false", Bool(StreamingBudgetPossible(true, 1, 0.0f)));
		Check("T9 streaming_budget: per-texture off (E5 decides)", "false", Bool(StreamingBudgetPossible(true, 0, 1.0f)));
		Check("T9 streaming_budget: texture does not stream", "false", Bool(StreamingBudgetPossible(false, 1, 1.0f)));
	}

	std::printf("\n%d check(s), %d failure(s)\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
