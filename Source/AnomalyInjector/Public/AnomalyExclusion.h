#pragma once

#include "AnomalyStuckMipWindow.h"

namespace AnomalyExclusion
{
	enum class EFamily : unsigned char
	{
		Other = 0,
		M52 = 1,
		M53 = 2
	};

	enum class EState : unsigned char
	{
		Idle = 0,
		FireLive = 1,
		TrailOpen = 2,
		Restoring = 3,
		RevertSettling = 4,
		Unresolved = 5,
		Closed = 6
	};

	enum class ETrail : unsigned char
	{
		None = 0,
		Closed = 1,
		Unresolved = 2,
		Gating = 3
	};

	static constexpr int RevertSettleFrames = 2;
	static constexpr const char* M52Id = "stuck_low_mip";
	static constexpr const char* M53UvId = "uv_corruption";
	static constexpr const char* M53NormalId = "normal_corruption";

	inline bool SameId(const char* A, const char* B)
	{
		if (!A || !B)
		{
			return false;
		}
		while (*A && *A == *B)
		{
			++A;
			++B;
		}
		return *A == *B;
	}

	inline EFamily FamilyOf(const char* Id)
	{
		if (SameId(Id, M52Id))
		{
			return EFamily::M52;
		}
		if (SameId(Id, M53UvId) || SameId(Id, M53NormalId))
		{
			return EFamily::M53;
		}
		return EFamily::Other;
	}

	inline const char* DescribeState(EState S)
	{
		switch (S)
		{
		case EState::FireLive:       return "fire_live";
		case EState::TrailOpen:      return "trail_open";
		case EState::Restoring:      return "restoring";
		case EState::RevertSettling: return "revert_settling";
		case EState::Unresolved:     return "restore_unresolved";
		case EState::Closed:         return "closed";
		default:                     return "idle";
		}
	}

	inline bool IsExcludingState(EState S)
	{
		return S == EState::FireLive || S == EState::TrailOpen || S == EState::Restoring || S == EState::RevertSettling;
	}

	inline ETrail ClassifyTrail(bool bOpen, bool bClosed, bool bUnresolved)
	{
		if (!bOpen)
		{
			return ETrail::None;
		}
		if (bClosed)
		{
			return ETrail::Closed;
		}
		return bUnresolved ? ETrail::Unresolved : ETrail::Gating;
	}

	inline ETrail ClassifyTrail(const AnomalyStuckMipWindow::FTrail& T)
	{
		return ClassifyTrail(T.bOpen, T.bClosed, T.bUnresolved);
	}

	inline ETrail CombineTrail(ETrail A, ETrail B)
	{
		return (unsigned char)A >= (unsigned char)B ? A : B;
	}

	struct FM52Signals
	{
		bool bFireLive = false;
		bool bProvider = false;
		ETrail Trail = ETrail::None;
		bool bRestoringSet = false;
	};

	inline EState M52State(const FM52Signals& S)
	{
		if (S.bFireLive)
		{
			return EState::FireLive;
		}
		if (S.bProvider && S.Trail != ETrail::None)
		{
			switch (S.Trail)
			{
			case ETrail::Gating:     return EState::TrailOpen;
			case ETrail::Unresolved: return EState::Unresolved;
			default:                 return EState::Closed;
			}
		}
		return S.bRestoringSet ? EState::Restoring : EState::Idle;
	}

	struct FM53Signals
	{
		bool bFireLive = false;
		bool bRevertSettling = false;
	};

	inline EState M53State(const FM53Signals& S)
	{
		if (S.bFireLive)
		{
			return EState::FireLive;
		}
		return S.bRevertSettling ? EState::RevertSettling : EState::Idle;
	}

	inline bool IsRevertSettlingAt(long long RevertFrame, long long NowFrame)
	{
		return RevertFrame >= 0 && NowFrame >= RevertFrame && (NowFrame - RevertFrame) < RevertSettleFrames;
	}

	struct FVerdict
	{
		bool bExcluded = false;
		bool bAdmittedAfterUnresolved = false;
		EFamily Partner = EFamily::Other;
		EState PartnerState = EState::Idle;
	};

	inline FVerdict Evaluate(EFamily Candidate, EState M52, EState M53)
	{
		FVerdict V;
		if (Candidate == EFamily::M53)
		{
			V.Partner = EFamily::M52;
			V.PartnerState = M52;
			V.bExcluded = IsExcludingState(M52);
			V.bAdmittedAfterUnresolved = !V.bExcluded && M52 == EState::Unresolved;
		}
		else if (Candidate == EFamily::M52)
		{
			V.Partner = EFamily::M53;
			V.PartnerState = M53;
			V.bExcluded = IsExcludingState(M53);
		}
		return V;
	}

	inline int FilterAdmitted(const EFamily* Candidates, int Num, EState M52, EState M53, bool* OutAdmitted)
	{
		int Admitted = 0;
		for (int i = 0; i < Num; ++i)
		{
			const bool bAdmit = !Evaluate(Candidates[i], M52, M53).bExcluded;
			if (OutAdmitted)
			{
				OutAdmitted[i] = bAdmit;
			}
			Admitted += bAdmit ? 1 : 0;
		}
		return Admitted;
	}
}
