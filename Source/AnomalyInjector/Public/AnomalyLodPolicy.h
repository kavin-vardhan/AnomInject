#pragma once

namespace AnomalyLodPolicy
{
	enum class EForcedLodRefusal : unsigned char { None = 0, HierarchicalInstanced = 1, SingleLod = 2, SkeletalUnmeasured = 3, NotCoarser = 4, CurrentLodUndetermined = 5 };

	static constexpr int NumRefusalKinds = 6;

	inline const char* DescribeRefusal(EForcedLodRefusal Refusal)
	{
		return Refusal == EForcedLodRefusal::HierarchicalInstanced ? "hism_ignores_forced_lod"
			: Refusal == EForcedLodRefusal::SingleLod ? "single_lod"
			: Refusal == EForcedLodRefusal::SkeletalUnmeasured ? "skeletal_unmeasured"
			: Refusal == EForcedLodRefusal::NotCoarser ? "not_coarser_than_current"
			: Refusal == EForcedLodRefusal::CurrentLodUndetermined ? "current_lod_undetermined" : "";
	}

	inline int FirstUsableLod(int NumLods, int EffectiveMinLod, unsigned int LodsWithVertices)
	{
		if (NumLods <= 0)
		{
			return 0;
		}
		int Lod = EffectiveMinLod < 0 ? 0 : (EffectiveMinLod > NumLods - 1 ? NumLods - 1 : EffectiveMinLod);
		while (Lod < NumLods && Lod < 32 && (LodsWithVertices & (1u << Lod)) == 0u)
		{
			++Lod;
		}
		return Lod;
	}

	inline int UsableLodCount(int NumLods, int EffectiveMinLod, unsigned int LodsWithVertices)
	{
		return NumLods <= 0 ? 0 : NumLods - FirstUsableLod(NumLods, EffectiveMinLod, LodsWithVertices);
	}

	inline EForcedLodRefusal DecideForcedLod(bool bHierarchicalInstanced, bool bSkeletal, bool bAllowSkeletal, int NumLods)
	{
		return bHierarchicalInstanced ? EForcedLodRefusal::HierarchicalInstanced
			: (bSkeletal && !bAllowSkeletal) ? EForcedLodRefusal::SkeletalUnmeasured
			: NumLods < 2 ? EForcedLodRefusal::SingleLod : EForcedLodRefusal::None;
	}

	inline int EffectiveForcedLod(int ForcedLodModel, int FirstUsable, int NumLods, int FirstResident)
	{
		const int Lod = ForcedLodModel < FirstUsable + 1 ? FirstUsable : (ForcedLodModel > NumLods ? NumLods - 1 : ForcedLodModel - 1);
		return Lod < FirstResident ? FirstResident : Lod;
	}

	inline int EffectiveShownLod(int PredictedLod, int FirstUsable, int NumLods, int FirstResident)
	{
		const int Lod = PredictedLod < FirstUsable ? FirstUsable : (PredictedLod > NumLods - 1 ? NumLods - 1 : PredictedLod);
		return FirstResident > Lod ? FirstResident : Lod;
	}

	inline EForcedLodRefusal DecideContrast(bool bCurrentKnown, int ShownLod, int ForcedLod)
	{
		return !bCurrentKnown ? EForcedLodRefusal::CurrentLodUndetermined
			: ForcedLod <= ShownLod ? EForcedLodRefusal::NotCoarser : EForcedLodRefusal::None;
	}

	inline bool ShouldScopeToForced(int NumForced, int NumRenderableNotForced)
	{
		return NumForced > 0 && NumRenderableNotForced > 0;
	}

	inline const char* DominantRefusalOf(const int* Counts, int NumKinds, int NumOther)
	{
		int Best = -1;
		for (int i = 1; i < NumKinds && i < NumRefusalKinds; ++i)
		{
			if (Counts[i] > 0 && Counts[i] > NumOther && (Best < 0 || Counts[i] > Counts[Best]))
			{
				Best = i;
			}
		}
		return Best < 0 ? "" : DescribeRefusal(static_cast<EForcedLodRefusal>(Best));
	}

	inline const char* DominantRefusal(int NumHierarchicalInstanced, int NumSingleLod, int NumSkeletal, int NumOther)
	{
		const int ByKind[4] = { 0, NumHierarchicalInstanced, NumSingleLod, NumSkeletal };
		return DominantRefusalOf(ByKind, 4, NumOther);
	}
}
