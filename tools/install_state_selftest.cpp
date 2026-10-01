#include "../Source/AnomalyInjector/Public/AnomalyInstallState.h"
#include "../Source/AnomalyInjector/Public/AnomalyLabelSync.h"

#include <cstdio>
#include <string>
#include <vector>

using namespace AnomalyInstall;

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

enum EMat : int
{
	MatNone = 0,
	MatOurs = 1,
	MatAssetA = 2,
	MatAssetB = 3,
	MatHost = 4
};

struct FComp
{
	bool bValid = true;
	bool bRegistered = true;
	bool bRenders = true;
	std::vector<int> AssetSlots;
	std::vector<int> Overrides;

	int NumMaterials() const { return (int)AssetSlots.size(); }

	int Resolved(int i) const
	{
		if (i < 0 || i >= NumMaterials())
		{
			return MatNone;
		}
		if (i < (int)Overrides.size() && Overrides[(size_t)i] != MatNone)
		{
			return Overrides[(size_t)i];
		}
		return AssetSlots[(size_t)i];
	}

	int SlotMaterial(int i) const
	{
		if (i < NumMaterials())
		{
			return Resolved(i);
		}
		return i < (int)Overrides.size() ? Overrides[(size_t)i] : MatNone;
	}

	void SetMaterial(int i, int M)
	{
		if ((int)Overrides.size() <= i)
		{
			Overrides.resize((size_t)i + 1, MatNone);
		}
		Overrides[(size_t)i] = M;
	}
};

struct FSlot
{
	int Comp = 0;
	int Index = 0;
	bool bWasExplicit = false;
	int Original = MatNone;
};

static void Apply(std::vector<FComp>& Comps, std::vector<FSlot>& Captured, int Comp)
{
	FComp& C = Comps[(size_t)Comp];
	for (int i = 0; i < C.NumMaterials(); ++i)
	{
		FSlot S;
		S.Comp = Comp;
		S.Index = i;
		S.Original = C.Resolved(i);
		S.bWasExplicit = i < (int)C.Overrides.size() && C.Overrides[(size_t)i] != MatNone;
		Captured.push_back(S);
		C.SetMaterial(i, MatOurs);
	}
}

static EState StateOf(const std::vector<FComp>& Comps, const std::vector<FSlot>& Captured)
{
	std::vector<FSlotView> Views;
	for (const FSlot& S : Captured)
	{
		const FComp& C = Comps[(size_t)S.Comp];
		FSlotView V;
		V.bComponentValid = C.bValid;
		V.bRegistered = C.bRegistered;
		V.bRenders = C.bRenders;
		V.SlotIndex = S.Index;
		V.NumMaterials = C.NumMaterials();
		V.bResolvedIsOurs = S.Index < V.NumMaterials && C.Resolved(S.Index) == MatOurs;
		Views.push_back(V);
	}
	return ClassifySlots(Views.data(), (int)Views.size());
}

static EState StateOfOld(const std::vector<FComp>& Comps, const std::vector<FSlot>& Captured)
{
	int Live = 0;
	for (const FSlot& S : Captured)
	{
		const FComp& C = Comps[(size_t)S.Comp];
		if (!C.bValid)
		{
			continue;
		}
		++Live;
		if (C.Resolved(S.Index) != MatOurs)
		{
			return EState::None;
		}
	}
	return Live > 0 ? EState::Full : EState::None;
}

struct FRevertResult
{
	int Restored = 0;
	int Cleared = 0;
	int Left = 0;
	int Swept = 0;
	int Residual = 0;
};

static FRevertResult Revert(std::vector<FComp>& Comps, const std::vector<FSlot>& Captured)
{
	FRevertResult R;
	for (const FSlot& S : Captured)
	{
		FComp& C = Comps[(size_t)S.Comp];
		if (!C.bValid)
		{
			continue;
		}
		const ERevertSlot A = DecideRevertSlot(C.SlotMaterial(S.Index) == MatOurs, S.bWasExplicit, S.Original != MatNone);
		if (A == ERevertSlot::Leave)
		{
			++R.Left;
			continue;
		}
		if (A == ERevertSlot::Restore)
		{
			C.SetMaterial(S.Index, S.Original);
			++R.Restored;
		}
		else
		{
			C.SetMaterial(S.Index, MatNone);
			++R.Cleared;
		}
	}
	for (FComp& C : Comps)
	{
		const int Extent = SweepExtent(C.NumMaterials(), (int)C.Overrides.size());
		for (int i = 0; i < Extent; ++i)
		{
			if (C.SlotMaterial(i) == MatOurs)
			{
				C.SetMaterial(i, MatNone);
				++R.Swept;
			}
		}
	}
	for (const FComp& C : Comps)
	{
		bool Arr[64] = {};
		const int N = (int)C.Overrides.size() < 64 ? (int)C.Overrides.size() : 64;
		for (int i = 0; i < N; ++i)
		{
			Arr[i] = C.Overrides[(size_t)i] == MatOurs;
		}
		R.Residual += CountOwnedOverrides(Arr, N);
	}
	return R;
}

static FRevertResult RevertOld(std::vector<FComp>& Comps, const std::vector<FSlot>& Captured)
{
	FRevertResult R;
	for (const FSlot& S : Captured)
	{
		FComp& C = Comps[(size_t)S.Comp];
		if (!C.bValid || S.Index >= C.NumMaterials())
		{
			continue;
		}
		if (C.Resolved(S.Index) != MatOurs)
		{
			++R.Left;
			continue;
		}
		if (S.bWasExplicit && S.Original != MatNone)
		{
			C.SetMaterial(S.Index, S.Original);
			++R.Restored;
		}
		else
		{
			C.SetMaterial(S.Index, MatNone);
			++R.Cleared;
		}
	}
	for (FComp& C : Comps)
	{
		for (int i = 0; i < C.NumMaterials(); ++i)
		{
			if (C.Resolved(i) == MatOurs)
			{
				C.SetMaterial(i, MatNone);
				++R.Swept;
			}
		}
	}
	for (const FComp& C : Comps)
	{
		for (int M : C.Overrides)
		{
			R.Residual += M == MatOurs ? 1 : 0;
		}
	}
	return R;
}

static FComp TwoSlot()
{
	FComp C;
	C.AssetSlots = { MatAssetA, MatAssetB };
	return C;
}

static void TestSlotPredicate()
{
	FSlotView V;
	V.bComponentValid = true;
	V.bRegistered = true;
	V.bRenders = true;
	V.SlotIndex = 1;
	V.NumMaterials = 2;
	V.bResolvedIsOurs = true;
	Check(SlotRendersOurs(V), "R3: a valid, registered, rendering component whose slot exists and resolves to ours renders ours");
	FSlotView W = V;
	W.bComponentValid = false;
	Check(!SlotRendersOurs(W), "R3: an invalid component renders nothing of ours");
	W = V;
	W.bRegistered = false;
	Check(!SlotRendersOurs(W), "R3: an unregistered component renders nothing of ours");
	W = V;
	W.bRenders = false;
	Check(!SlotRendersOurs(W), "R3: a hidden component renders nothing of ours");
	W = V;
	W.NumMaterials = 1;
	Check(!SlotRendersOurs(W), "R3: a slot index outside the current mesh does not render ours (mesh changed)");
	W = V;
	W.bResolvedIsOurs = false;
	Check(!SlotRendersOurs(W), "R3: a slot that now resolves to a foreign material does not render ours");
	Check(Classify(2, 2) == EState::Full && Classify(2, 1) == EState::Partial && Classify(2, 0) == EState::None
		&& Classify(0, 0) == EState::None, "R1: full / partial / none by how many targeted slots render ours");
	Check(IsInstalled(EState::Partial) && IsInstalled(EState::Full) && !IsInstalled(EState::None)
		&& IsPartialByte((unsigned char)EState::Partial) && !IsPartialByte((unsigned char)EState::Full),
		"R1: partial and full are installed; only partial is counted as partial");
	Check(std::string(DescribeState(EState::Partial)) == "partial", "R1: state names");
}

static void TestR1PartialReplace()
{
	std::vector<FComp> Comps = { TwoSlot() };
	std::vector<FSlot> Cap;
	Apply(Comps, Cap, 0);
	Check(StateOf(Comps, Cap) == EState::Full, "R1: right after apply both slots render ours");
	Comps[0].SetMaterial(0, MatHost);
	Check(StateOf(Comps, Cap) == EState::Partial, "R1: the host replaces one of two slots -> partial (still labelled)");
	Check(StateOfOld(Comps, Cap) == EState::None, "R1 old rule disagrees: one foreign slot made the whole event a negative");
	Check(AnomalyLabelSync::IsAnnotationMember(AnomalyLabelSync::EAnnotationPolicy::FireWindow, true, true,
		IsInstalled(StateOf(Comps, Cap))), "R1: the partial frame is an annotation member");
	Comps[0].SetMaterial(1, MatHost);
	Check(StateOf(Comps, Cap) == EState::None, "R1: both slots replaced -> none -> effect_interrupted");
	Check(AnomalyLabelSync::DecideInterruptedEntry(AnomalyLabelSync::EEntryEmit::Normal, AnomalyLabelSync::EAnnotationPolicy::FireWindow,
		IsInstalled(StateOf(Comps, Cap))) == AnomalyLabelSync::EEntryEmit::TransitionOnly,
		"R1: with no targeted slot ours the entry is written transition-only (effect_interrupted)");
}

static void TestR3RenderState()
{
	std::vector<FComp> Comps = { TwoSlot(), TwoSlot() };
	std::vector<FSlot> Cap;
	Apply(Comps, Cap, 0);
	Comps[0].bRenders = false;
	Check(StateOf(Comps, Cap) == EState::None,
		"R3: the host hides the original component (and draws a clean replacement) -> none, never labelled");
	Check(StateOfOld(Comps, Cap) == EState::Full, "R3 old rule disagrees: the hidden component still read as installed");
	Comps[0].bRenders = true;
	Comps[0].bRegistered = false;
	Check(StateOf(Comps, Cap) == EState::None, "R3: an unregistered original component -> none");
	Comps[0].bRegistered = true;
	Comps[0].AssetSlots = { MatAssetA };
	Check(StateOf(Comps, Cap) == EState::Partial,
		"R3: the host swaps to a one-slot mesh -> slot 1 no longer exists, slot 0 still ours -> partial");
	Check(StateOfOld(Comps, Cap) == EState::None, "R3 old rule: the dormant index read as a non-ours slot and dropped the event");
}

static void TestR2DormantOverride()
{
	std::vector<FComp> Comps = { TwoSlot() };
	std::vector<FSlot> Cap;
	Apply(Comps, Cap, 0);
	Comps[0].AssetSlots = { MatAssetA };
	const FRevertResult R = Revert(Comps, Cap);
	Check(R.Residual == 0, "R2: after revert no override of ours remains at any index (assertion reads 0)");
	Comps[0].AssetSlots = { MatAssetA, MatAssetB };
	Check(Comps[0].Resolved(0) == MatAssetA && Comps[0].Resolved(1) == MatAssetB,
		"R2: switching back to the two-slot mesh shows the asset materials, not ours");

	std::vector<FComp> Old = { TwoSlot() };
	std::vector<FSlot> CapOld;
	Apply(Old, CapOld, 0);
	Old[0].AssetSlots = { MatAssetA };
	const FRevertResult RO = RevertOld(Old, CapOld);
	Old[0].AssetSlots = { MatAssetA, MatAssetB };
	Check(RO.Residual == 1 && Old[0].Resolved(1) == MatOurs,
		"R2 old revert disagrees: the dormant index-1 override survives and our material comes back with no event");

	std::vector<FComp> Ex = { TwoSlot() };
	Ex[0].SetMaterial(1, MatHost);
	std::vector<FSlot> CapEx;
	Apply(Ex, CapEx, 0);
	Ex[0].AssetSlots = { MatAssetA };
	Revert(Ex, CapEx);
	Check(Ex[0].SlotMaterial(1) == MatHost && Ex[0].Overrides[1] == MatHost,
		"R2: an explicit pre-apply override at the now out-of-range index is restored, not cleared");

	std::vector<FComp> Fo = { TwoSlot() };
	std::vector<FSlot> CapFo;
	Apply(Fo, CapFo, 0);
	Fo[0].SetMaterial(0, MatHost);
	const FRevertResult RF = Revert(Fo, CapFo);
	Check(Fo[0].Resolved(0) == MatHost && RF.Left == 1 && RF.Residual == 0,
		"R2: a slot the host re-took after apply is left alone (foreign overrides are preserved)");
	Check(SweepExtent(1, 2) == 2 && SweepExtent(3, 1) == 3 && SweepExtent(-1, 0) == 0,
		"R2: the sweep covers every stored override index as well as every current slot");
	Check(DecideRevertSlot(true, true, true) == ERevertSlot::Restore && DecideRevertSlot(true, true, false) == ERevertSlot::Clear
		&& DecideRevertSlot(true, false, true) == ERevertSlot::Clear && DecideRevertSlot(false, true, true) == ERevertSlot::Leave,
		"R2: restore an explicit original that still exists, clear otherwise, leave a slot that is not ours");
}

int main()
{
	TestSlotPredicate();
	TestR1PartialReplace();
	TestR3RenderState();
	TestR2DormantOverride();
	std::printf("install state selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
