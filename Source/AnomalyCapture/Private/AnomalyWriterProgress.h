#pragma once

namespace AnomalyWriterProgress
{
	static constexpr double StallWindowSeconds = 120.0;

	inline double StallReference(double WaitStart, double LastCompletion, bool bAlreadyStalled)
	{
		if (bAlreadyStalled)
		{
			return LastCompletion;
		}
		return WaitStart > LastCompletion ? WaitStart : LastCompletion;
	}

	inline bool IsStalled(double Now, double WaitStart, double LastCompletion, bool bAlreadyStalled, double Window)
	{
		return Now - StallReference(WaitStart, LastCompletion, bAlreadyStalled) > Window;
	}

	inline bool CountsForRun(unsigned int JobGeneration, unsigned int CurrentGeneration)
	{
		return JobGeneration == CurrentGeneration;
	}
}
