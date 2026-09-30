#pragma once

namespace AnomalyTargetPolicy
{
	enum class ENaniteDecision : unsigned char
	{
		Admit = 0,
		Refuse = 1
	};

	inline const char* DescribeNaniteReason()
	{
		return "nanite_unmaskable";
	}

	inline ENaniteDecision DecideNanite(bool bAllowNanite, int NumRenderable, int NumNanite)
	{
		return (!bAllowNanite && NumRenderable > 0 && NumNanite > 0) ? ENaniteDecision::Refuse : ENaniteDecision::Admit;
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
