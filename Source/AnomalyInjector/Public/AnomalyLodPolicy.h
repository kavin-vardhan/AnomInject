#pragma once

namespace AnomalyLodPolicy
{
	enum class EForcedLodRefusal : unsigned char { None = 0, HierarchicalInstanced = 1, SingleLod = 2, SkeletalUnmeasured = 3 };

	inline const char* DescribeRefusal(EForcedLodRefusal Refusal)
	{
		return Refusal == EForcedLodRefusal::HierarchicalInstanced ? "hism_ignores_forced_lod"
			: Refusal == EForcedLodRefusal::SingleLod ? "single_lod"
			: Refusal == EForcedLodRefusal::SkeletalUnmeasured ? "skeletal_unmeasured" : "";
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

	inline const char* DominantRefusal(int NumHierarchicalInstanced, int NumSingleLod, int NumSkeletal, int NumOther)
	{
		const int Counts[3] = { NumHierarchicalInstanced, NumSingleLod, NumSkeletal };
		const EForcedLodRefusal Kinds[3] = { EForcedLodRefusal::HierarchicalInstanced, EForcedLodRefusal::SingleLod, EForcedLodRefusal::SkeletalUnmeasured };
		int Best = -1;
		for (int i = 0; i < 3; ++i)
		{
			if (Counts[i] > 0 && Counts[i] > NumOther && (Best < 0 || Counts[i] > Counts[Best]))
			{
				Best = i;
			}
		}
		return Best < 0 ? "" : DescribeRefusal(Kinds[Best]);
	}
}
