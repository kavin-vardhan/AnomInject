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

	enum class ETargetToken : unsigned char { Substring = 0, ExactName = 1, Path = 2 };

	static constexpr int AmbiguousPathsLogged = 3;

	inline ETargetToken ClassifyTargetToken(int FirstChar)
	{
		return FirstChar == '@' ? ETargetToken::Path : FirstChar == '=' ? ETargetToken::ExactName : ETargetToken::Substring;
	}

	inline const char* DescribeTargetToken(ETargetToken Kind)
	{
		return Kind == ETargetToken::Path ? "path" : Kind == ETargetToken::ExactName ? "exact_name" : "substring";
	}

	inline const char* DescribeAmbiguousReason()
	{
		return "target_ambiguous";
	}

	inline bool IsApplyTokenAmbiguous(ETargetToken Kind, int NumActorsNamed)
	{
		return Kind != ETargetToken::Path && NumActorsNamed > 1;
	}

	inline bool IsTargetedNameAmbiguous(ETargetToken Kind, int NumMatches)
	{
		return Kind != ETargetToken::Path && NumMatches > 1;
	}

	inline const char* DescribeTargetHiddenReason()
	{
		return "target_hidden";
	}

	inline bool IsHostHidden(bool bActorHidden, bool bHiddenByPlugin, int NumGeometry, int NumVisibleGeometry)
	{
		return (bActorHidden && !bHiddenByPlugin) || (NumGeometry > 0 && NumVisibleGeometry == 0);
	}

	inline const char* DescribeHlodProxyReason()
	{
		return "hlod_proxy";
	}

	inline const char* LegacyHlodActorClassPath()
	{
		return "/Script/Engine.LODActor";
	}

	inline const char* PartitionHlodActorClassPath()
	{
		return "/Script/Engine.WorldPartitionHLOD";
	}

	enum class ETargetEligibility : unsigned char { Eligible = 0, Hidden = 1, HlodProxy = 2 };

	inline ETargetEligibility DecideTargetEligibility(bool bHostHidden, bool bHlodProxy)
	{
		return bHostHidden ? ETargetEligibility::Hidden : bHlodProxy ? ETargetEligibility::HlodProxy : ETargetEligibility::Eligible;
	}

	inline const char* DescribeEligibility(ETargetEligibility Eligibility)
	{
		return Eligibility == ETargetEligibility::Hidden ? DescribeTargetHiddenReason()
			: Eligibility == ETargetEligibility::HlodProxy ? DescribeHlodProxyReason() : "";
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
