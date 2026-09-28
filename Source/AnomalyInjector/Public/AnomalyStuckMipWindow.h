#pragma once

namespace AnomalyStuckMipWindow
{
	enum class ETexState : unsigned char
	{
		Unknown = 0,
		Held = 1,
		Baseline = 2
	};

	enum class EVerdict : unsigned char
	{
		NotHeld = 0,
		Held = 1,
		Unknown = 2,
		BeforeApply = 3
	};

	enum class EMembership : unsigned char
	{
		Out = 0,
		Held = 1,
		Unknown = 2,
		SettleTail = 3
	};

	enum class EPurity : unsigned char
	{
		Pure = 0,
		Shared = 1,
		NoUsers = 2
	};

	static constexpr int ConfirmFrames = 2;

	inline ETexState ClassifyTexture(int RenderResident, int Baseline, bool bKnown)
	{
		if (!bKnown || RenderResident < 0 || Baseline <= 0)
		{
			return ETexState::Unknown;
		}
		return RenderResident < Baseline ? ETexState::Held : ETexState::Baseline;
	}

	inline EVerdict Combine(const ETexState* States, int Num)
	{
		if (Num <= 0)
		{
			return EVerdict::BeforeApply;
		}
		bool bUnknown = false;
		for (int i = 0; i < Num; ++i)
		{
			if (States[i] == ETexState::Held)
			{
				return EVerdict::Held;
			}
			if (States[i] == ETexState::Unknown)
			{
				bUnknown = true;
			}
		}
		return bUnknown ? EVerdict::Unknown : EVerdict::NotHeld;
	}

	inline bool IsMember(EMembership M)
	{
		return M != EMembership::Out;
	}

	inline EMembership LiveMembership(EVerdict V)
	{
		switch (V)
		{
		case EVerdict::Held:    return EMembership::Held;
		case EVerdict::Unknown: return EMembership::Unknown;
		default:                return EMembership::Out;
		}
	}

	struct FTrail
	{
		int RevertSessionIndex = -1;
		int SettleTailFrames = 0;
		int TimeoutFrames = 120;
		int FirstBaselineSI = -1;
		int CleanRun = 0;
		int LastProcessedSI = -1;
		int ClosedAtSI = -1;
		int HeldFrames = 0;
		int UnknownFrames = 0;
		int TailFrames = 0;
		bool bOpen = false;
		bool bClosed = false;
		bool bUnresolved = false;

		void Open(int InRevertSessionIndex, int InSettleTailFrames, int InTimeoutFrames)
		{
			*this = FTrail();
			RevertSessionIndex = InRevertSessionIndex;
			SettleTailFrames = InSettleTailFrames < 0 ? 0 : InSettleTailFrames;
			TimeoutFrames = InTimeoutFrames < 1 ? 1 : InTimeoutFrames;
			bOpen = true;
		}

		EMembership Step(int SessionIndex, EVerdict V)
		{
			if (SessionIndex > LastProcessedSI)
			{
				LastProcessedSI = SessionIndex;
			}
			if (V == EVerdict::Held || V == EVerdict::Unknown)
			{
				FirstBaselineSI = -1;
				CleanRun = 0;
				if (V == EVerdict::Held)
				{
					++HeldFrames;
					return EMembership::Held;
				}
				++UnknownFrames;
				return EMembership::Unknown;
			}
			if (FirstBaselineSI < 0)
			{
				FirstBaselineSI = SessionIndex;
			}
			if (SessionIndex < FirstBaselineSI + SettleTailFrames)
			{
				++TailFrames;
				return EMembership::SettleTail;
			}
			++CleanRun;
			if (!bClosed && CleanRun >= ConfirmFrames)
			{
				bClosed = true;
				ClosedAtSI = SessionIndex;
			}
			return EMembership::Out;
		}

		bool ShouldTimeout(int NextArmSessionIndex) const
		{
			return bOpen && !bClosed && !bUnresolved
				&& (NextArmSessionIndex - RevertSessionIndex) >= TimeoutFrames;
		}

		bool GatesNextBurst() const
		{
			return bOpen && !bClosed && !bUnresolved;
		}

		bool StillAttached() const
		{
			return bOpen && !bClosed;
		}
	};

	inline EPurity ClassifyPurity(const unsigned long long* UserComponents, int NumUsers,
		const unsigned long long* TargetComponents, int NumTargets, int* OutForeign)
	{
		int Foreign = 0;
		for (int u = 0; u < NumUsers; ++u)
		{
			bool bTarget = false;
			for (int t = 0; t < NumTargets; ++t)
			{
				if (UserComponents[u] == TargetComponents[t])
				{
					bTarget = true;
					break;
				}
			}
			if (!bTarget)
			{
				++Foreign;
			}
		}
		if (OutForeign)
		{
			*OutForeign = Foreign;
		}
		if (NumUsers <= 0)
		{
			return EPurity::NoUsers;
		}
		return (NumUsers == 1 && Foreign == 0) ? EPurity::Pure : EPurity::Shared;
	}

	inline const char* DescribeMembership(EMembership M)
	{
		switch (M)
		{
		case EMembership::Held:       return "held";
		case EMembership::Unknown:    return "unknown";
		case EMembership::SettleTail: return "settle_tail";
		default:                      return "baseline";
		}
	}
}
