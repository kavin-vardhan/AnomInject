#pragma once

namespace AnomalyTargetPolicy
{
	enum class ENaniteDecision : unsigned char
	{
		Admit = 0,
		Refuse = 1,
		RefuseProbeMissing = 2
	};

	inline const char* DescribeNaniteReason()
	{
		return "nanite_unmaskable";
	}

	inline const char* DescribeNaniteProbeMissingReason()
	{
		return "nanite_probe_missing";
	}

	inline ENaniteDecision DecideNanite(bool bAllowNanite, int NumRenderable, int NumNanite)
	{
		return (!bAllowNanite && NumRenderable > 0 && NumNanite > 0) ? ENaniteDecision::Refuse : ENaniteDecision::Admit;
	}

	inline ENaniteDecision DecideNaniteTarget(bool bAllowNanite, bool bProbeAvailable, int NumDrawn, int NumNanite)
	{
		if (bAllowNanite)
		{
			return ENaniteDecision::Admit;
		}
		if (!bProbeAvailable)
		{
			return ENaniteDecision::RefuseProbeMissing;
		}
		return DecideNanite(false, NumDrawn, NumNanite);
	}

	inline bool NaniteBlocksLabel(bool bAllowNanite, bool bProbeAvailable, bool bHasTargetActor, int NumDrawn, int NumNanite)
	{
		return bHasTargetActor && DecideNaniteTarget(bAllowNanite, bProbeAvailable, NumDrawn, NumNanite) != ENaniteDecision::Admit;
	}

	struct FPrimitiveFacts
	{
		bool bVisible = false;
		bool bGeometryClass = false;
		bool bZeroInstances = false;
		bool bExcludedByTargetPattern = false;
		bool bFoliageOwner = false;
		bool bNanite = false;
	};

	inline bool CountsAsDrawnPrimitive(const FPrimitiveFacts& P)
	{
		return P.bVisible && P.bGeometryClass && !P.bZeroInstances;
	}

	inline int CountDrawnNanite(const FPrimitiveFacts* Prims, int Num, int& OutDrawn)
	{
		OutDrawn = 0;
		int Nanite = 0;
		for (int i = 0; Prims && i < Num; ++i)
		{
			if (!CountsAsDrawnPrimitive(Prims[i]))
			{
				continue;
			}
			++OutDrawn;
			if (Prims[i].bNanite)
			{
				++Nanite;
			}
		}
		return Nanite;
	}

	inline bool RouteIsNanite(bool bStaticMeshClass, bool bSplineMesh, bool bDisallowNanite, bool bFallbackDisplayed,
		bool bHasNaniteData, bool bPlatformNanite)
	{
		return bStaticMeshClass && !bSplineMesh && !bDisallowNanite && !bFallbackDisplayed && bHasNaniteData && bPlatformNanite;
	}

	template <typename TCandidate, typename TIsNanite>
	int FilterNaniteCandidates(TCandidate* Candidates, int Num, bool bAllowNanite, TIsNanite&& IsNanite)
	{
		if (bAllowNanite || !Candidates || Num <= 0)
		{
			return Num < 0 ? 0 : Num;
		}
		int Kept = 0;
		for (int i = 0; i < Num; ++i)
		{
			if (IsNanite(Candidates[i]))
			{
				continue;
			}
			if (Kept != i)
			{
				Candidates[Kept] = Candidates[i];
			}
			++Kept;
		}
		return Kept;
	}
}
