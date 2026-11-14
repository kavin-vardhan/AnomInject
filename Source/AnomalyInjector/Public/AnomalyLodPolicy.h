#pragma once

namespace AnomalyLodPolicy
{
	enum class EForcedLodRefusal : unsigned char { None = 0, HierarchicalInstanced = 1, SingleLod = 2 };

	inline const char* DescribeRefusal(EForcedLodRefusal Refusal)
	{
		return Refusal == EForcedLodRefusal::HierarchicalInstanced ? "hism_ignores_forced_lod"
			: Refusal == EForcedLodRefusal::SingleLod ? "single_lod" : "";
	}

	inline EForcedLodRefusal DecideForcedLod(bool bHierarchicalInstanced, int NumLods)
	{
		return bHierarchicalInstanced ? EForcedLodRefusal::HierarchicalInstanced
			: NumLods < 2 ? EForcedLodRefusal::SingleLod : EForcedLodRefusal::None;
	}

	inline const char* DominantRefusal(int NumHierarchicalInstanced, int NumSingleLod, int NumOther)
	{
		if (NumHierarchicalInstanced > 0 && NumHierarchicalInstanced >= NumSingleLod && NumHierarchicalInstanced > NumOther)
		{
			return DescribeRefusal(EForcedLodRefusal::HierarchicalInstanced);
		}
		return (NumSingleLod > 0 && NumSingleLod > NumOther) ? DescribeRefusal(EForcedLodRefusal::SingleLod) : "";
	}
}
