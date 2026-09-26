#include <cmath>
#include <cstdio>
#include <set>
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

static std::string StepName(const FAllocStep& S)
{
	return (S.Kind == AllocKind::Output ? std::string("O") : std::string("S")) + std::to_string(S.Tex)
		+ (S.Kind == AllocKind::Output ? std::string() : "." + std::to_string(S.Level));
}

static std::string UsageString(unsigned Bits)
{
	const char* Names[Usage::Count] = { "skeletal_mesh", "clothing", "morph_targets", "ism", "nanite", "spline_mesh", "static_lighting" };
	std::string Out;
	for (int i = 0; i < Usage::Count; ++i)
	{
		if (Bits & (1u << i))
		{
			Out += (Out.empty() ? "" : "+");
			Out += Names[i];
		}
	}
	return Out.empty() ? "none" : Out;
}

struct FSeq
{
	int Sequences = 0;
	int Violations = 0;
	int LinePendingPositive = 0;
	std::string FirstViolation;
};

static void Violate(FSeq& Q, const std::string& What)
{
	if (Q.Violations++ == 0)
	{
		Q.FirstViolation = What;
	}
}

static void RunRollback(FSeq& Q, const FReqTex* T, int N, int FailK, bool bCreatedThenRejected, const char* Set)
{
	++Q.Sequences;
	FLedgerCore L;
	FEventAccount A;
	const long long Cap = 1LL << 40;
	const long long Req = EventRequirement(T, N).Total();
	const unsigned long long F = 1000;
	const std::string Tag = std::string(Set) + " fail@" + std::to_string(FailK) + (bCreatedThenRejected ? " created" : " none");
	if (!A.Reserve(L, Req, Cap))
	{
		Violate(Q, Tag + " reserve");
		return;
	}
	FAllocStep Steps[1024];
	const int NS = PlanAllocations(T, N, Steps, 1024);
	long long Created = 0;
	int Made = 0;
	for (int k = 0; k < NS; ++k)
	{
		if (k == FailK)
		{
			if (bCreatedThenRejected)
			{
				A.NoteCreated(L, Steps[k].Bytes);
				A.ReleaseCreated(L, Steps[k].Bytes, F);
				Created += Steps[k].Bytes;
			}
			break;
		}
		A.NoteCreated(L, Steps[k].Bytes);
		Created += Steps[k].Bytes;
		++Made;
	}
	for (int k = 0; k < Made; ++k)
	{
		A.ReleaseCreated(L, Steps[k].Bytes, F);
	}
	A.Close(L, F);
	if (L.Live != 0)
	{
		Violate(Q, Tag + " live after rollback " + std::to_string(L.Live));
	}
	if (L.PendingSum() != Created)
	{
		Violate(Q, Tag + " pending " + std::to_string(L.PendingSum()) + " != created " + std::to_string(Created));
	}
	if (L.PendingSum() > 0)
	{
		++Q.LinePendingPositive;
	}
	if (JudgeLedgerReading(F, F, L.Live, L.PendingSum()) != ELedgerBalance::NotYetDue)
	{
		Violate(Q, Tag + " the rollback line was judged as a balance verdict");
	}
	L.Tick(F + 1);
	if (L.PendingSum() != Created)
	{
		Violate(Q, Tag + " pending dropped before frame+2");
	}
	L.Tick(F + 2);
	if (L.Live != 0 || L.PendingSum() != 0)
	{
		Violate(Q, Tag + " not balanced at frame+2");
	}
	if (JudgeLedgerReading(F, F + 2, L.Live, L.PendingSum()) != ELedgerBalance::Balanced)
	{
		Violate(Q, Tag + " the frame+2 reading is not judged balanced");
	}
	if (L.Peak != Req)
	{
		Violate(Q, Tag + " peak " + std::to_string(L.Peak) + " != reserved " + std::to_string(Req));
	}
}

static std::string RunSuccess(const FReqTex* T, int N, bool bRedraw)
{
	FLedgerCore L;
	FEventAccount A;
	const long long Req = EventRequirement(T, N).Total();
	const long long Chains = EventRequirement(T, N).Chains;
	if (!A.Reserve(L, Req, 1LL << 40))
	{
		return "reserve";
	}
	FAllocStep Steps[1024];
	const int NS = PlanAllocations(T, N, Steps, 1024);
	for (int k = 0; k < NS; ++k)
	{
		A.NoteCreated(L, Steps[k].Bytes);
	}
	const unsigned long long Apply = 500;
	long long ScratchReleased = 0;
	if (!bRedraw)
	{
		for (int k = 0; k < NS; ++k)
		{
			if (Steps[k].Kind == AllocKind::Scratch)
			{
				A.ReleaseCreated(L, Steps[k].Bytes, Apply);
				ScratchReleased += Steps[k].Bytes;
			}
		}
	}
	if (L.Live != Req - ScratchReleased || L.PendingSum() != ScratchReleased)
	{
		return "after static scratch release";
	}
	L.Tick(Apply + 2);
	if (L.PendingSum() != 0 || L.Live != (bRedraw ? Req : Chains))
	{
		return "held set at apply+2";
	}
	const unsigned long long Revert = 900;
	for (int k = 0; k < NS; ++k)
	{
		if (bRedraw || Steps[k].Kind == AllocKind::Output)
		{
			A.ReleaseCreated(L, Steps[k].Bytes, Revert);
		}
	}
	A.Close(L, Revert);
	if (L.Live != 0 || L.PendingSum() != (bRedraw ? Req : Chains))
	{
		return "at revert";
	}
	if (JudgeLedgerReading(Revert, Revert, L.Live, L.PendingSum()) != ELedgerBalance::NotYetDue)
	{
		return "the revert line was judged as a balance verdict";
	}
	L.Tick(Revert + 1);
	if (L.PendingSum() == 0)
	{
		return "pending dropped at revert+1";
	}
	L.Tick(PostRevertSampleFrame(Revert));
	if (L.Live != 0 || L.PendingSum() != 0)
	{
		return "not balanced at the post-revert frame";
	}
	if (JudgeLedgerReading(Revert, PostRevertSampleFrame(Revert), L.Live, L.PendingSum()) != ELedgerBalance::Balanced)
	{
		return "the post-revert reading is not judged balanced";
	}
	return L.Peak == Req ? "balanced" : "peak " + std::to_string(L.Peak);
}

struct FNode
{
	FNode* Parent = nullptr;
	bool bRuntime = false;
};

static std::string Walk(FNode* Start, int& Depth)
{
	const EChainWalk W = WalkChain(Start, 16, Depth, [](FNode* N) { return N->bRuntime; }, [](FNode* N) { return N->Parent; });
	return W == EChainWalk::Clean ? "clean" : (W == EChainWalk::RuntimeLink ? "runtime_link" : "limit_reached");
}

struct FEntry
{
	const char* Tex;
	bool b2D;
};

struct FMat
{
	const char* Name;
	bool bReadable;
	int NumEntries;
	const FEntry* Entries;
};

struct FCollOut
{
	std::string Set;
	int NullSlots = 0;
	int Unresolved = 0;
	int Unmeasured = 0;
	int Incomplete = 0;
	bool bComplete = false;
};

static FCollOut CollectSlots(FMat* const* Slots, int N, FMat* EngineDefault)
{
	FCollOut O;
	std::set<const FMat*> Seen;
	std::set<std::string> Paths;
	for (int s = 0; s < N; ++s)
	{
		FMat* M = MeasuredSlotMaterial(Slots[s], EngineDefault, O.NullSlots, O.Unresolved);
		if (!M || !Seen.insert(M).second)
		{
			continue;
		}
		if (!M->bReadable)
		{
			++O.Unmeasured;
			continue;
		}
		for (int e = 0; e < M->NumEntries; ++e)
		{
			const FEntry& E = M->Entries[e];
			if (ClassifyCollateralEntry(E.Tex != nullptr, E.b2D, O.Unresolved) == ECollEntry::Measured)
			{
				Paths.insert(E.Tex);
			}
		}
	}
	for (const std::string& P : Paths)
	{
		O.Set += (O.Set.empty() ? std::string() : std::string(",")) + P;
	}
	O.Incomplete = CollateralIncompleteCount(0, O.Unmeasured, O.Unresolved, 0, 1);
	O.bComplete = CollateralComplete(true, O.Incomplete);
	return O;
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

	std::printf("\n[9] P2-5 allocation plan: the order Apply executes, and its byte total\n");
	{
		const FReqTex Set[] = { Tex(1, 4096, 4096, true), Tex(2, 4096, 4096, true), Tex(3, 2048, 2048, false), Tex(1, 4096, 4096, true) };
		FAllocStep Steps[256];
		const int NS = PlanAllocations(Set, 4, Steps, 256);
		Check("steps: A out + 12 scratch, B out, C out + 11 scratch", "26", Int(NS));
		std::string Head;
		for (int k = 0; k < 3; ++k) { Head += StepName(Steps[k]) + " "; }
		Check("A's output precedes its scratch levels", "O0 S0.1 S0.2 ", Head);
		Check("B shares A's class: output only", "O1", StepName(Steps[13]));
		Check("C opens its own class after its output", "O2 S2.1", StepName(Steps[14]) + " " + StepName(Steps[15]));
		Check("last step is C's 1x1 scratch level", "S2.11", StepName(Steps[25]));
		Check("scratch level 1 of 4096^2 (bytes)", Int(4LL * 2048 * 2048), Int(Steps[1].Bytes));
		long long Sum = 0;
		for (int k = 0; k < NS; ++k) { Sum += Steps[k].Bytes; }
		Check("plan bytes == EventRequirement (duplicate counted once)", Int(EventRequirement(Set, 4).Total()), Int(Sum));
		const FReqTex Rock[] = { Tex(1, 4096, 4096, true), Tex(2, 4096, 4096, false), Tex(3, 4096, 4096, false), Tex(4, 2048, 2048, false) };
		const int NR = PlanAllocations(Rock, 4, Steps, 256);
		long long RockSum = 0;
		for (int k = 0; k < NR; ++k) { RockSum += Steps[k].Bytes; }
		Check("MainWorld rock set: plan bytes == requirement", MiB(EventRequirement(Rock, 4).Total()), MiB(RockSum));
		FReqTex Single = Tex(9, 512, 512, false);
		Single.M = 1;
		Check("a single-mip source plans one output and no scratch", "1", Int(PlanAllocations(&Single, 1, Steps, 256)));
		Check("a plan larger than the buffer is refused", "-1", Int(PlanAllocations(Set, 4, Steps, 5)));
	}

	std::printf("\n[10] P2-5 / P3-3 ledger core: two-frame pending, frame passed in, monotone peak\n");
	{
		FLedgerCore L;
		Check("reserve 100 of cap 150", "true", Bool(L.Reserve(100, 150)));
		Check("reserve 60 more is refused", "false", Bool(L.Reserve(60, 150)));
		L.ReleaseToPending(40, 10);
		Check("40 released at frame 10: live", "60", Int(L.Live));
		Check("40 released at frame 10: pending", "40", Int(L.PendingSum()));
		Check("pending blocks the cap: available", "50", Int(L.Available(150)));
		L.Tick(11);
		Check("still pending at frame 11", "40", Int(L.PendingSum()));
		L.ReleaseToPending(10, 11);
		L.Tick(12);
		Check("frame 12 retires frame 10's release only", "10", Int(L.PendingSum()));
		L.Tick(13);
		Check("frame 13 retires frame 11's release", "0", Int(L.PendingSum()));
		L.Unreserve(50);
		Check("unreserve returns live to 0", "0", Int(L.Live));
		Check("peak stays the run peak, never reset", "100", Int(L.Peak));
		FLedgerCore M;
		M.Reserve(2000, 1LL << 40);
		for (int f = 0; f < 20; ++f) { M.ReleaseToPending(100, 50 + f); }
		Check("20 releases on 20 frames (bucket overflow merges later)", "2000", Int(M.PendingSum() + M.Live));
		M.Tick(50 + 19 + 2);
		Check("all retired by the last due frame", "0", Int(M.PendingSum()));
	}

	std::printf("\n[11] P2-5 transaction sequences: tree order, a failure at every step, balance back to zero\n");
	{
		const FReqTex SetA[] = { Tex(1, 4096, 4096, true), Tex(2, 4096, 4096, true), Tex(3, 2048, 2048, false) };
		const FReqTex SetB[] = { Tex(1, 2048, 2048, true), Tex(2, 2048, 2048, false), Tex(3, 1024, 1024, false), Tex(4, 2048, 2048, false) };
		FReqTex SetC[] = { Tex(1, 256, 256, false) };
		SetC[0].M = 1;
		struct FSet { const FReqTex* T; int N; const char* Name; };
		const FSet Sets[] = { { SetA, 3, "two same-class 4096 + 2048" }, { SetB, 4, "Lyra cube with opacity" }, { SetC, 1, "single-mip" } };
		FSeq Q;
		for (const FSet& S : Sets)
		{
			FAllocStep Steps[1024];
			const int NS = PlanAllocations(S.T, S.N, Steps, 1024);
			for (int k = 0; k <= NS; ++k)
			{
				RunRollback(Q, S.T, S.N, k, false, S.Name);
				if (k < NS)
				{
					RunRollback(Q, S.T, S.N, k, true, S.Name);
				}
			}
		}
		std::printf("     %d rollback sequences (every plan step, fail-before-create and created-then-rejected, plus fail after all)\n", Q.Sequences);
		Check("sequences run (2N+1 per set; N = 26, 36, 1 plan steps)", "129", Int(Q.Sequences));
		Check("violations (live 0, pending == created, zero at frame+2)", "0", Int(Q.Violations));
		Check("P3-3 rollback lines with pending > 0 (allowed, judged not_yet_due)", "126", Int(Q.LinePendingPositive));
		if (Q.Violations > 0)
		{
			std::printf("     first violation: %s\n", Q.FirstViolation.c_str());
		}
		Check("success, static scratch released after the draws", "balanced", RunSuccess(SetA, 3, false));
		Check("success, identity_redraw holds scratch until revert", "balanced", RunSuccess(SetA, 3, true));
		Check("success, Lyra cube set", "balanced", RunSuccess(SetB, 4, false));
		FLedgerCore L;
		FEventAccount A;
		L.Reserve(100, 150);
		Check("over-budget reserve refused", "false", Bool(A.Reserve(L, 60, 150)));
		Check("a refused reserve leaves the account empty", "0", Int(A.Reserved + A.Created));
		FEventAccount B;
		B.Reserve(L, 40, 150);
		Check("creating past the reservation is flagged", "false", Bool(B.NoteCreated(L, 50)));
		Check("and force-reserved so live stays true", "150", Int(L.Live));
	}

	std::printf("\n[12] P2-1 condition decision: one predicate, no NoApply input\n");
	{
		int Bad = -1;
		const bool Two[] = { true, true };
		const bool Three[] = { true, true, true };
		Check("applied: slots hold the MID, bindings read back", "installed", LexHeld(ConditionHeld(true, Two, 2, Three, 3, Bad)));
		const bool Orig[] = { false, false };
		Check("NoApply 1: slots still hold originals", "slot_not_installed", LexHeld(ConditionHeld(true, Orig, 2, Three, 3, Bad)));
		Check("NoApply 2: no binding set was created", "no_expected_set", LexHeld(ConditionHeld(true, nullptr, 0, nullptr, 0, Bad)));
		Check("no event", "no_event", LexHeld(ConditionHeld(false, Two, 2, Three, 3, Bad)));
		const bool OneBad[] = { true, false, true };
		Check("a binding that no longer reads back", "binding_readback", LexHeld(ConditionHeld(true, Two, 2, OneBad, 3, Bad)));
		Check("its index is reported", "1", Int(Bad));
		const bool Partly[] = { true, false };
		Check("one expected slot replaced by the game", "slot_not_installed", LexHeld(ConditionHeld(true, Partly, 2, Three, 3, Bad)));
	}

	std::printf("\n[13] P2-4 required usage flags (engine proxy rules, offline rows)\n");
	{
		struct FRow
		{
			const char* Name;
			EMeshKind Kind;
			bool bInstanced, bSpline, bNanite, bForceVol, bShare;
			int NumLods;
			bool Lit[3];
			bool Build[3];
			bool Uses[3];
			bool bCloth, bMorph;
			const char* Expect;
		};
		const FRow Rows[] = {
			{ "static unlit", EMeshKind::Static, false, false, false, false, false, 1, { false }, { false }, { true }, false, false, "none" },
			{ "static, LOD0 lightmapped", EMeshKind::Static, false, false, false, false, false, 1, { true }, { true }, { true }, false, false, "static_lighting" },
			{ "instanced + lightmapped (Codex P2-4 case)", EMeshKind::Static, true, false, false, false, false, 1, { true }, { true }, { true }, false, false, "ism+static_lighting" },
			{ "instanced, unlit", EMeshKind::Static, true, false, false, false, false, 1, { false }, { false }, { true }, false, false, "ism" },
			{ "Nanite + LOD0 lightmapped", EMeshKind::Static, false, false, true, false, false, 1, { true }, { true }, { true }, false, false, "nanite+static_lighting" },
			{ "Nanite instanced", EMeshKind::Static, true, false, true, false, false, 1, { false }, { false }, { true }, false, false, "ism+nanite" },
			{ "Nanite, ForceVolumetric", EMeshKind::Static, false, false, true, true, false, 1, { true }, { true }, { true }, false, false, "nanite" },
			{ "spline, lightmapped", EMeshKind::Static, false, true, false, false, false, 1, { true }, { true }, { true }, false, false, "spline_mesh+static_lighting" },
			{ "spline never takes the Nanite proxy", EMeshKind::Static, false, true, true, false, false, 1, { false }, { false }, { true }, false, false, "spline_mesh" },
			{ "LOD1-only lighting, slot only in LOD1", EMeshKind::Static, false, false, false, false, false, 2, { false, true }, { false, true }, { false, true }, false, false, "static_lighting" },
			{ "LOD1-only lighting, slot only in LOD0", EMeshKind::Static, false, false, false, false, false, 2, { false, true }, { false, true }, { true, false }, false, false, "none" },
			{ "shared lighting: LOD0 lit, slot only in LOD1", EMeshKind::Static, false, false, false, false, true, 2, { true, false }, { true, false }, { false, true }, false, false, "static_lighting" },
			{ "instanced forces sharing: LOD0 data unlit wins", EMeshKind::Static, true, false, false, false, false, 2, { false, true }, { true, true }, { false, true }, false, false, "ism" },
			{ "shared, LOD0 has no build data: LOD1 own data", EMeshKind::Static, false, false, false, false, true, 2, { false, true }, { false, true }, { false, true }, false, false, "static_lighting" },
			{ "static ForceVolumetric", EMeshKind::Static, false, false, false, true, false, 1, { true }, { true }, { true }, false, false, "none" },
			{ "skeletal", EMeshKind::Skinned, false, false, false, false, false, 0, { false }, { false }, { false }, false, false, "skeletal_mesh" },
			{ "skeletal, cloth section on this slot", EMeshKind::Skinned, false, false, false, false, false, 0, { false }, { false }, { false }, true, false, "skeletal_mesh+clothing" },
			{ "skeletal with morph targets", EMeshKind::Skinned, false, false, false, false, false, 0, { false }, { false }, { false }, false, true, "skeletal_mesh+morph_targets" },
		};
		for (const FRow& R : Rows)
		{
			FUsageFacts F;
			F.Kind = R.Kind;
			F.bRenderData = true;
			F.bInstanced = R.bInstanced;
			F.bSpline = R.bSpline;
			F.bNanite = R.bNanite;
			F.bForceVolumetric = R.bForceVol;
			F.bLodsShareLighting = R.bShare;
			F.NumLods = R.NumLods;
			F.LodDataLit = R.Lit;
			F.LodDataHasBuildData = R.Build;
			F.LodUsesSlot = R.Uses;
			F.bSlotHasClothSection = R.bCloth;
			F.bHasMorphTargets = R.bMorph;
			EUsageGap Gap = EUsageGap::None;
			const unsigned Bits = RequiredUsages(F, Gap);
			Check(R.Name, R.Expect, Gap == EUsageGap::None ? UsageString(Bits) : std::string("gap:") + LexUsageGap(Gap));
		}
		FUsageFacts U;
		EUsageGap Gap = EUsageGap::None;
		RequiredUsages(U, Gap);
		Check("unknown component kind is undetermined -> refuse", "unknown_component", LexUsageGap(Gap));
		U.Kind = EMeshKind::Static;
		RequiredUsages(U, Gap);
		Check("static without render data is undetermined -> refuse", "no_render_data", LexUsageGap(Gap));
	}

	std::printf("\n[14] P3-2 chain walk; P2-2 collateral selection; P3-1 post-revert frame\n");
	{
		FNode Chain[20];
		for (int i = 0; i < 19; ++i) { Chain[i].Parent = &Chain[i + 1]; }
		int Depth = 0;
		Chain[3].Parent = nullptr;
		Check("4-link chain, nothing runtime", "clean", Walk(&Chain[0], Depth));
		Chain[2].bRuntime = true;
		Check("runtime link at depth 2", "runtime_link", Walk(&Chain[0], Depth));
		Check("its depth", "2", Int(Depth));
		Chain[2].bRuntime = false;
		Chain[3].Parent = &Chain[4];
		Chain[15].Parent = nullptr;
		Check("exactly 16 links then terminal", "clean", Walk(&Chain[0], Depth));
		Chain[15].Parent = &Chain[16];
		Chain[16].Parent = nullptr;
		Check("17 links: the tail past the limit is unverified", "limit_reached", Walk(&Chain[0], Depth));
		FNode A, B;
		A.Parent = &B;
		B.Parent = &A;
		Check("a cycle ends at the limit, never clean", "limit_reached", Walk(&A, Depth));

		struct FPrim { const char* Name; bool bRegistered; bool bTarget; double Since; bool bFoliage; bool bTranslucentOnly; double CoveragePct; double DistanceCm; };
		const FPrim Scene[] = {
			{ "target", true, true, 0.0, false, false, 20.0, 400.0 },
			{ "prop_2pct", true, false, 0.0, false, false, 2.0, 600.0 },
			{ "backdrop_far", true, false, 0.03, false, false, 40.0, 50000.0 },
			{ "foliage", true, false, 0.0, true, false, 8.0, 900.0 },
			{ "glass", true, false, 0.0, false, true, 5.0, 700.0 },
			{ "shadow_only", true, false, 12.0, false, false, 0.0, 800.0 },
			{ "unregistered", false, false, 0.0, false, false, 10.0, 500.0 },
		};
		std::string In;
		for (const FPrim& P : Scene)
		{
			if (IsCollateralPrimitive(P.bRegistered, P.bTarget, P.Since, 1.0 / 30.0))
			{
				In += std::string(In.empty() ? "" : ",") + P.Name;
			}
		}
		Check("synthetic scene: every drawn non-target primitive, whatever the injection filters say", "prop_2pct,backdrop_far,foliage,glass", In);
		Check("window at 30 fps is the engine's 0.2 s floor", "0.2000", [] { char B[16]; std::snprintf(B, 16, "%.4f", CollateralWindowSeconds(1.0 / 30.0)); return std::string(B); }());
		Check("window follows a long frame", "0.5001", [] { char B[16]; std::snprintf(B, 16, "%.4f", CollateralWindowSeconds(0.5)); return std::string(B); }());
		Check("post-revert sample frame = revert frame + 2", "102", Int((long long)PostRevertSampleFrame(100)));
	}

	std::printf("\n[15] 082-06c: P2-2 null slot and unresolved entries; P3-3 rollback balance read at F+2\n");
	{
		const FEntry GridEntries[] = { { "T_Default_Grid_D", true }, { "T_Default_Grid_N", true } };
		const FEntry RockEntries[] = { { "T_Rock_D", true }, { "T_Rock_N", true } };
		const FEntry WallEntries[] = { { "T_Wall_D", true }, { nullptr, true } };
		const FEntry SkyEntries[] = { { "T_Sky_Cube", false } };
		FMat Grid = { "WorldGridMaterial", true, 2, GridEntries };
		FMat Rock = { "M_Rock", true, 2, RockEntries };
		FMat Wall = { "M_Wall", true, 2, WallEntries };
		FMat Sky = { "M_Sky", true, 1, SkyEntries };

		FMat* RockOnly[] = { &Rock };
		const FCollOut Control = CollectSlots(RockOnly, 1, &Grid);
		Check("control: one material, every entry resolved", "T_Rock_D,T_Rock_N", Control.Set);
		Check("control: complete", "true", Bool(Control.bComplete));

		FMat* WithNull[] = { &Rock, nullptr };
		const FCollOut Null = CollectSlots(WithNull, 2, &Grid);
		Check("P2-2 null slot measured through the default material", "T_Default_Grid_D,T_Default_Grid_N,T_Rock_D,T_Rock_N", Null.Set);
		Check("P2-2 null slots counted", "1", Int(Null.NullSlots));
		Check("P2-2 a measured null slot leaves the set complete", "true", Bool(Null.bComplete));

		FMat* NullOnly[] = { nullptr };
		const FCollOut NoDefault = CollectSlots(NullOnly, 1, nullptr);
		Check("P2-2 null slot, default unresolvable: unresolved", "1", Int(NoDefault.Unresolved));
		Check("P2-2 null slot, default unresolvable: never clean", "false", Bool(NoDefault.bComplete));

		FMat* WithWall[] = { &Wall };
		const FCollOut Unres = CollectSlots(WithWall, 1, &Grid);
		Check("P2-2 unresolvable entry: resolved entries still measured", "T_Wall_D", Unres.Set);
		Check("P2-2 unresolvable entry: collateral_unresolved", "1", Int(Unres.Unresolved));
		Check("P2-2 unresolvable entry: incomplete", "1", Int(Unres.Incomplete));
		Check("P2-2 unresolvable entry: never reported clean", "false", Bool(Unres.bComplete));

		FMat* WithSky[] = { &Sky };
		const FCollOut Cube = CollectSlots(WithSky, 1, &Grid);
		Check("resolved non-2D entry: not collected, not unresolved", "none/0/true",
			(Cube.Set.empty() ? std::string("none") : Cube.Set) + "/" + Int(Cube.Unresolved) + "/" + Bool(Cube.bComplete));

		FLedgerCore L;
		FEventAccount A;
		const FReqTex One[] = { Tex(1, 1024, 1024, false) };
		A.Reserve(L, EventRequirement(One, 1).Total(), 1LL << 40);
		FAllocStep Steps[64];
		const int NS = PlanAllocations(One, 1, Steps, 64);
		const int Made = NS < 3 ? NS : 3;
		for (int k = 0; k < Made; ++k) { A.NoteCreated(L, Steps[k].Bytes); }
		const unsigned long long F = 700;
		for (int k = 0; k < Made; ++k) { A.ReleaseCreated(L, Steps[k].Bytes, F); }
		A.Close(L, F);
		Check("P3-3 rollback line at F: live 0, pending > 0 is allowed", "0/true", Int(L.Live) + "/" + Bool(L.PendingSum() > 0));
		Check("P3-3 rollback line: not a balance verdict", "not_yet_due", LexLedgerBalance(JudgeLedgerReading(F, F, L.Live, L.PendingSum())));
		L.Tick(F + 1);
		Check("P3-3 reading at F+1: not yet due", "not_yet_due", LexLedgerBalance(JudgeLedgerReading(F, F + 1, L.Live, L.PendingSum())));
		L.Tick(F + 2);
		Check("P3-3 reading at F+2: live/pending", "0/0", Int(L.Live) + "/" + Int(L.PendingSum()));
		Check("P3-3 reading at F+2: judged", "balanced", LexLedgerBalance(JudgeLedgerReading(F, F + 2, L.Live, L.PendingSum())));
		FLedgerCore Late;
		Late.Reserve(100, 1LL << 40);
		Late.ReleaseToPending(100, F + 1);
		Late.Tick(F + 2);
		Check("P3-3 bytes still pending at F+2", "unbalanced", LexLedgerBalance(JudgeLedgerReading(F, F + 2, Late.Live, Late.PendingSum())));
		FLedgerCore Leak;
		Leak.Reserve(64, 1LL << 40);
		Check("P3-3 bytes still live at F+2", "unbalanced", LexLedgerBalance(JudgeLedgerReading(F, F + 2, Leak.Live, Leak.PendingSum())));
		Check("P3-3 a late reading is still judged", "balanced", LexLedgerBalance(JudgeLedgerReading(F, F + 5, 0, 0)));
	}

	std::printf("\n%d check(s), %d failure(s)\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
