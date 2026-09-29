#pragma once

#include "AnomalyStuckMipWindow.h"
#include "AnomalyLabelSync.h"

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
		Closed = 6,
		TrailReopenable = 7,
		LabelTail = 8
	};

	enum class ETrail : unsigned char
	{
		None = 0,
		Closed = 1,
		Unresolved = 2,
		LabelTail = 3,
		Reopenable = 4,
		Gating = 5
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
		case EState::FireLive:        return "fire_live";
		case EState::TrailOpen:       return "trail_open";
		case EState::Restoring:       return "restoring";
		case EState::RevertSettling:  return "revert_settling";
		case EState::Unresolved:      return "restore_unresolved";
		case EState::Closed:          return "closed";
		case EState::TrailReopenable: return "trail_reopenable";
		case EState::LabelTail:       return "label_tail";
		default:                      return "idle";
		}
	}

	inline bool IsExcludingState(EState S)
	{
		return S == EState::FireLive || S == EState::TrailOpen || S == EState::Restoring || S == EState::RevertSettling
			|| S == EState::TrailReopenable || S == EState::LabelTail;
	}

	struct FTrailFacts
	{
		bool bOpen = false;
		bool bClosed = false;
		bool bDetached = false;
		bool bUnresolved = false;
		bool bAllNotedProcessed = true;
		bool bLabelTail = false;
	};

	inline ETrail ClassifyTrail(const FTrailFacts& F)
	{
		if (!F.bOpen)
		{
			return F.bLabelTail ? ETrail::LabelTail : ETrail::None;
		}
		if (!F.bClosed)
		{
			return F.bUnresolved ? ETrail::Unresolved : ETrail::Gating;
		}
		if (!F.bDetached || !F.bAllNotedProcessed)
		{
			return ETrail::Reopenable;
		}
		return F.bLabelTail ? ETrail::LabelTail : ETrail::Closed;
	}

	inline FTrailFacts FactsOf(const AnomalyStuckMipWindow::FTrail& T, bool bLabelTail)
	{
		FTrailFacts F;
		F.bOpen = T.bOpen;
		F.bClosed = T.bClosed;
		F.bDetached = T.bDetached;
		F.bUnresolved = T.bUnresolved;
		F.bAllNotedProcessed = T.AllNotedProcessed();
		F.bLabelTail = bLabelTail;
		return F;
	}

	inline ETrail ClassifyTrail(const AnomalyStuckMipWindow::FTrail& T, bool bLabelTail)
	{
		return ClassifyTrail(FactsOf(T, bLabelTail));
	}

	inline ETrail CombineTrail(ETrail A, ETrail B)
	{
		return (unsigned char)A >= (unsigned char)B ? A : B;
	}

	inline bool RunEmitsTransitionTail(bool bRenderTruthRun, int OffFrames)
	{
		return bRenderTruthRun && OffFrames > 0;
	}

	inline bool TransitionTailOwed(bool bRunEmitsTail, bool bHasMember, bool bOffDone)
	{
		return bRunEmitsTail && bHasMember && !bOffDone;
	}

	inline bool TransitionTailOwed(bool bRunEmitsTail, const AnomalyLabelSync::FEventTransitionTrack& T)
	{
		return TransitionTailOwed(bRunEmitsTail, T.LastMember() != AnomalyLabelSync::FEventTransitionTrack::NoMember, T.bOffDone);
	}

	inline bool PendingFireOwesFrames(EFamily Partner, bool bRenderTruthFire)
	{
		return bRenderTruthFire || Partner == EFamily::M53;
	}

	inline bool RetainedLiveFireOwesFrames(EFamily Partner)
	{
		return Partner == EFamily::M53;
	}

	inline ETrail EmitterTrail(bool bOwesFrames)
	{
		FTrailFacts F;
		F.bLabelTail = bOwesFrames;
		return ClassifyTrail(F);
	}

	inline EState StateOfTrail(ETrail T)
	{
		switch (T)
		{
		case ETrail::Gating:     return EState::TrailOpen;
		case ETrail::Reopenable: return EState::TrailReopenable;
		case ETrail::LabelTail:  return EState::LabelTail;
		case ETrail::Unresolved: return EState::Unresolved;
		case ETrail::Closed:     return EState::Closed;
		default:                 return EState::Idle;
		}
	}

	inline bool IsRestoreEntryPastTimeout(int FramesWaited, int TimeoutFrames)
	{
		return FramesWaited >= (TimeoutFrames < 1 ? 1 : TimeoutFrames);
	}

	inline bool AllRestoringPastTimeout(const int* FramesWaited, int Num, int TimeoutFrames)
	{
		if (!FramesWaited || Num <= 0)
		{
			return false;
		}
		for (int i = 0; i < Num; ++i)
		{
			if (!IsRestoreEntryPastTimeout(FramesWaited[i], TimeoutFrames))
			{
				return false;
			}
		}
		return true;
	}

	inline EState RestoringState(bool bRestoringSet, bool bPastTimeout)
	{
		if (!bRestoringSet)
		{
			return EState::Idle;
		}
		return bPastTimeout ? EState::Unresolved : EState::Restoring;
	}

	struct FM52Signals
	{
		bool bFireLive = false;
		bool bProvider = false;
		ETrail Trail = ETrail::None;
		bool bRestoringSet = false;
		bool bRestoringPastTimeout = false;
	};

	inline EState M52State(const FM52Signals& S)
	{
		if (S.bFireLive)
		{
			return EState::FireLive;
		}
		if (S.bProvider && S.Trail != ETrail::None)
		{
			return StateOfTrail(S.Trail);
		}
		return RestoringState(S.bRestoringSet, S.bRestoringPastTimeout);
	}

	struct FM53Signals
	{
		bool bFireLive = false;
		bool bRevertSettling = false;
		ETrail Trail = ETrail::None;
	};

	inline EState M53State(const FM53Signals& S)
	{
		if (S.bFireLive)
		{
			return EState::FireLive;
		}
		if (S.bRevertSettling)
		{
			return EState::RevertSettling;
		}
		const EState FromTrail = StateOfTrail(S.Trail);
		return IsExcludingState(FromTrail) ? FromTrail : EState::Idle;
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

	enum class ERecordAction : unsigned char
	{
		Replace = 0,
		Keep = 1,
		Drop = 2
	};

	inline ERecordAction RecordAfterApply(bool bApplied, bool bInstanceActiveAfter)
	{
		if (bApplied)
		{
			return ERecordAction::Replace;
		}
		return bInstanceActiveAfter ? ERecordAction::Keep : ERecordAction::Drop;
	}

	inline bool IsFireLiveForExclusion(bool bInstanceActive)
	{
		return bInstanceActive;
	}

	struct FFrameEntry
	{
		EFamily Family = EFamily::Other;
		bool bLabelled = false;
		bool bTransition = false;
	};

	inline bool IsM52CoEntry(const FFrameEntry& E)
	{
		return E.Family == EFamily::M52 && (E.bLabelled || E.bTransition);
	}

	inline bool IsM53CoEntry(const FFrameEntry& E)
	{
		return E.Family == EFamily::M53;
	}

	inline bool IsCoEntryFrame(const FFrameEntry* Entries, int Num)
	{
		bool bM52 = false;
		bool bM53 = false;
		for (int i = 0; Entries && i < Num; ++i)
		{
			bM52 = bM52 || IsM52CoEntry(Entries[i]);
			bM53 = bM53 || IsM53CoEntry(Entries[i]);
		}
		return bM52 && bM53;
	}
}
