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
		Missing = 3
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

	enum class EObserve : unsigned char
	{
		Wait = 0,
		Labelled = 1,
		NotLabelled = 2
	};

	enum class EHoldUserAction : unsigned char
	{
		None = 0,
		RevertNow = 1
	};

	static constexpr int ConfirmFrames = 2;
	static constexpr int CursorCapacity = 256;
	static constexpr int GapAgeTicks = 16;
	static constexpr int DeferredMaskMaxAgeTicks = 16;
	static constexpr int ObserveHardBoundTicks = 64;

	inline ETexState ClassifyTexture(int RenderResident, int Baseline, bool bKnown)
	{
		if (!bKnown || RenderResident < 0 || Baseline <= 0)
		{
			return ETexState::Unknown;
		}
		return RenderResident < Baseline ? ETexState::Held : ETexState::Baseline;
	}

	inline ETexState ClassifyTextureSample(int RenderResident, int Baseline, bool bKnown,
		unsigned long long BoundResourceId, unsigned long long SampleResourceId, bool* bOutReplaced)
	{
		if (bOutReplaced)
		{
			*bOutReplaced = false;
		}
		if (bKnown && BoundResourceId != 0 && SampleResourceId != BoundResourceId)
		{
			if (bOutReplaced)
			{
				*bOutReplaced = true;
			}
			return ETexState::Unknown;
		}
		return ClassifyTexture(RenderResident, Baseline, bKnown);
	}

	inline EVerdict Combine(const ETexState* States, int Num)
	{
		if (Num <= 0)
		{
			return EVerdict::Unknown;
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
		case EVerdict::NotHeld: return EMembership::Out;
		default:                return EMembership::Unknown;
		}
	}

	struct FTrailSlot
	{
		int SessionIndex = -1;
		EVerdict Verdict = EVerdict::Unknown;
		EMembership Membership = EMembership::Out;
		bool bSettled = false;
		bool bReceived = false;
		bool bProcessed = false;
	};

	struct FTrail
	{
		int RevertSessionIndex = -1;
		int SettleTailFrames = 0;
		int TimeoutFrames = 120;
		int NextSI = -1;
		int LastArmedSI = -1;
		int FirstBaselineSI = -1;
		int CleanRun = 0;
		int ClosingAtSI = -1;
		int FenceSI = -1;
		int ClosedAtSI = -1;
		int LastReopenSI = -1;
		int HeadWaitTicks = 0;
		int HeldFrames = 0;
		int UnknownFrames = 0;
		int MissingFrames = 0;
		int TailFrames = 0;
		int UnsettledBaselineFrames = 0;
		int Reopens = 0;
		int ReopensAfterDetach = 0;
		int LateReceipts = 0;
		int Overflows = 0;
		int DetachedAtSI = -1;
		bool bOpen = false;
		bool bClosing = false;
		bool bClosed = false;
		bool bDetached = false;
		bool bUnresolved = false;
		bool bInherited = false;
		FTrailSlot Slots[CursorCapacity];

		void Open(int InRevertSessionIndex, int InSettleTailFrames, int InTimeoutFrames)
		{
			*this = FTrail();
			RevertSessionIndex = InRevertSessionIndex;
			SettleTailFrames = InSettleTailFrames < 0 ? 0 : InSettleTailFrames;
			TimeoutFrames = InTimeoutFrames < 1 ? 1 : InTimeoutFrames;
			bOpen = true;
		}

		void RebaseForNewRun(int NewRevertSessionIndex)
		{
			const int Tail = SettleTailFrames;
			const int Timeout = TimeoutFrames;
			const bool bWasUnresolved = bUnresolved;
			*this = FTrail();
			RevertSessionIndex = NewRevertSessionIndex;
			SettleTailFrames = Tail;
			TimeoutFrames = Timeout;
			bUnresolved = bWasUnresolved;
			bInherited = true;
			bOpen = true;
		}

		static int SlotIndex(int SessionIndex)
		{
			const int M = SessionIndex % CursorCapacity;
			return M < 0 ? M + CursorCapacity : M;
		}

		FTrailSlot& Slot(int SessionIndex)
		{
			return Slots[SlotIndex(SessionIndex)];
		}

		const FTrailSlot& Slot(int SessionIndex) const
		{
			return Slots[SlotIndex(SessionIndex)];
		}

		void NoteArmed(int SessionIndex)
		{
			if (!bOpen || bDetached || SessionIndex <= LastArmedSI)
			{
				return;
			}
			if (NextSI < 0)
			{
				NextSI = SessionIndex;
			}
			const int From = LastArmedSI < 0 ? SessionIndex : LastArmedSI + 1;
			LastArmedSI = SessionIndex;
			for (int s = From; s < SessionIndex; ++s)
			{
				Store(s, EVerdict::Missing, false);
			}
		}

		bool Receive(int SessionIndex, EVerdict V, bool bSettled)
		{
			return Store(SessionIndex, V, bSettled);
		}

		bool MarkMissing(int SessionIndex)
		{
			return Store(SessionIndex, EVerdict::Missing, false);
		}

		bool Store(int SessionIndex, EVerdict V, bool bSettled)
		{
			if (!bOpen || NextSI < 0 || SessionIndex < NextSI)
			{
				++LateReceipts;
				return false;
			}
			if (SessionIndex - NextSI >= CursorCapacity)
			{
				++Overflows;
				return false;
			}
			FTrailSlot& S = Slot(SessionIndex);
			if (S.bReceived && S.SessionIndex == SessionIndex)
			{
				return false;
			}
			S = FTrailSlot();
			S.SessionIndex = SessionIndex;
			S.Verdict = V;
			S.bSettled = bSettled;
			S.bReceived = true;
			Advance();
			return true;
		}

		void Advance()
		{
			while (NextSI >= 0)
			{
				FTrailSlot& S = Slot(NextSI);
				if (!S.bReceived || S.SessionIndex != NextSI || S.bProcessed)
				{
					break;
				}
				S.Membership = Process(NextSI, S.Verdict, S.bSettled);
				S.bProcessed = true;
				++NextSI;
				HeadWaitTicks = 0;
			}
		}

		bool TryGetMembership(int SessionIndex, EMembership& Out) const
		{
			if (NextSI < 0 || SessionIndex >= NextSI)
			{
				return false;
			}
			const FTrailSlot& S = Slot(SessionIndex);
			if (!S.bProcessed || S.SessionIndex != SessionIndex)
			{
				return false;
			}
			Out = S.Membership;
			return true;
		}

		bool IsOrderPending(int SessionIndex) const
		{
			return bOpen && NextSI >= 0 && SessionIndex >= NextSI;
		}

		bool HasPendingHead() const
		{
			return bOpen && NextSI >= 0 && NextSI <= LastArmedSI;
		}

		bool TickHeadWait()
		{
			if (!HasPendingHead())
			{
				HeadWaitTicks = 0;
				return false;
			}
			++HeadWaitTicks;
			return HeadWaitTicks > GapAgeTicks;
		}

		void BreakRun(int SessionIndex, bool bRegression)
		{
			CleanRun = 0;
			if (bClosing)
			{
				bClosing = false;
				ClosingAtSI = -1;
				FenceSI = -1;
			}
			if (bRegression && bClosed)
			{
				bClosed = false;
				++Reopens;
				LastReopenSI = SessionIndex;
				if (bDetached)
				{
					bDetached = false;
					++ReopensAfterDetach;
				}
			}
		}

		bool AllNotedProcessed() const
		{
			return NextSI < 0 || NextSI > LastArmedSI;
		}

		bool TryDetach(int NextArmSessionIndex, bool bNextFireBegins)
		{
			if (!bOpen || !bClosed || bDetached)
			{
				return false;
			}
			if (!AllNotedProcessed() && !bNextFireBegins)
			{
				return false;
			}
			bDetached = true;
			DetachedAtSI = NextArmSessionIndex;
			return true;
		}

		EMembership Process(int SessionIndex, EVerdict V, bool bSettled)
		{
			if (V != EVerdict::NotHeld)
			{
				FirstBaselineSI = -1;
				BreakRun(SessionIndex, true);
				if (V == EVerdict::Held)
				{
					++HeldFrames;
					return EMembership::Held;
				}
				if (V == EVerdict::Missing)
				{
					++MissingFrames;
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
			if (!bSettled)
			{
				++UnsettledBaselineFrames;
				BreakRun(SessionIndex, false);
				return EMembership::Out;
			}
			++CleanRun;
			if (!bClosed && !bClosing && CleanRun >= ConfirmFrames)
			{
				bClosing = true;
				ClosingAtSI = SessionIndex;
				FenceSI = LastArmedSI > SessionIndex ? LastArmedSI : SessionIndex;
			}
			if (bClosing && SessionIndex >= FenceSI)
			{
				bClosing = false;
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

		bool AttachedForProducts() const
		{
			return bOpen && !bDetached;
		}
	};

	struct FTrailOwnership
	{
		bool bReserved = false;
		bool bOwnsRefusal = false;
		bool bServiced = false;
	};

	struct FOwnershipRelease
	{
		bool bUnreserve = false;
		bool bClearRefusal = false;
	};

	inline void OwnOnRefusal(FTrailOwnership& O)
	{
		O.bOwnsRefusal = true;
	}

	inline void OwnOnReserve(FTrailOwnership& O)
	{
		O.bReserved = true;
	}

	inline void OwnOnReopen(FTrailOwnership& O)
	{
		O.bReserved = true;
		O.bOwnsRefusal = true;
		O.bServiced = false;
	}

	inline FOwnershipRelease OwnOnDetach(FTrailOwnership& O, bool bOtherReserverOfTarget, bool bOtherRefusalOwnerOfId)
	{
		FOwnershipRelease R;
		if (O.bServiced)
		{
			return R;
		}
		O.bServiced = true;
		R.bUnreserve = O.bReserved && !bOtherReserverOfTarget;
		R.bClearRefusal = O.bOwnsRefusal && !bOtherRefusalOwnerOfId;
		O.bReserved = false;
		O.bOwnsRefusal = false;
		return R;
	}

	inline bool NeedsMaskRecordAtArm(bool bLiveFire, bool bTrailAttachedForProducts, bool bRenderTruthRun)
	{
		return bLiveFire || (bRenderTruthRun && bTrailAttachedForProducts);
	}

	inline bool ShouldReapplyCarriedOwnership(int NumCarried, bool bDeinitializing)
	{
		return NumCarried > 0 && !bDeinitializing;
	}

	struct FStreamerFence
	{
		bool bFlushed = false;
		bool bAmortizedCopies = false;
		bool bStreamingEnabled = true;
		int FramesSinceRevert = 0;
		int MinFramesIfUnfenced = 0;
	};

	inline bool StreamerPlansRetired(const FStreamerFence& F)
	{
		if (!F.bStreamingEnabled)
		{
			return true;
		}
		if (F.bFlushed && !F.bAmortizedCopies)
		{
			return true;
		}
		return F.FramesSinceRevert >= F.MinFramesIfUnfenced;
	}

	inline int UnfencedMinFrames(int FramesForFullUpdate)
	{
		const int Stages = FramesForFullUpdate < 1 ? 1 : FramesForFullUpdate;
		return 2 * (Stages + 2);
	}

	inline bool IsArmSettled(int NumTextures, bool bAnyRestoring, bool bAnyPendingOperation, bool bPlansRetired)
	{
		return NumTextures > 0 && !bAnyRestoring && !bAnyPendingOperation && bPlansRetired;
	}

	struct FFrameAuthority
	{
		bool bHasResult = false;
		bool bOrderPending = false;
		bool bForced = false;
		EMembership Membership = EMembership::Unknown;
	};

	inline void ForceTerminalUnknown(FFrameAuthority& A)
	{
		if (A.bHasResult && !A.bOrderPending)
		{
			return;
		}
		A.bHasResult = true;
		A.bOrderPending = false;
		A.bForced = true;
		A.Membership = EMembership::Unknown;
	}

	inline bool AcceptsLateReceipt(const FFrameAuthority& A)
	{
		return !A.bHasResult;
	}

	inline bool RejudgeOnRegistration(bool bRegisteredWhenJudged, bool bRegisteredNow)
	{
		return !bRegisteredWhenJudged && bRegisteredNow;
	}

	struct FCostSummary
	{
		int Samples = 0;
		double MeanMs = 0.0;
		double P95Ms = 0.0;
		double MaxMs = 0.0;
	};

	inline FCostSummary SummarizeCost(const float* SortedMs, int Num, double SumMs)
	{
		FCostSummary S;
		S.Samples = Num;
		if (Num <= 0)
		{
			return S;
		}
		S.MeanMs = SumMs / (double)Num;
		int Idx = (int)((95 * (long long)Num + 99) / 100) - 1;
		Idx = Idx < 0 ? 0 : (Idx >= Num ? Num - 1 : Idx);
		S.P95Ms = SortedMs[Idx];
		S.MaxMs = SortedMs[Num - 1];
		return S;
	}

	inline bool ShouldCarryAcrossRun(const FTrail& T)
	{
		return T.StillAttached();
	}

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

	struct FComponentScope
	{
		bool bTemplate = false;
		bool bPendingKill = false;
		bool bInLoadedLevelOfWorld = false;
		bool bLevelActive = false;
		bool bRegistered = false;
		bool bPrimitiveOrDecal = false;
	};

	inline bool InPurityScope(const FComponentScope& C)
	{
		return !C.bTemplate && !C.bPendingKill && C.bInLoadedLevelOfWorld && C.bPrimitiveOrDecal;
	}

	inline EHoldUserAction EvaluateNewHoldUser(const FComponentScope& C, bool bUsesHeldTexture, bool bOwnedByTarget,
		bool bHoldActive)
	{
		return (bHoldActive && InPurityScope(C) && bUsesHeldTexture && !bOwnedByTarget)
			? EHoldUserAction::RevertNow : EHoldUserAction::None;
	}

	inline bool IsContaminatedFrame(int ContaminatedFromSI, int SessionIndex, EMembership M)
	{
		return ContaminatedFromSI >= 0 && SessionIndex >= ContaminatedFromSI && IsMember(M);
	}

	inline EObserve ResolveObserve(bool bHasFinalResult, bool bResultMember, bool bSnapshotTerminal, int AgeTicks,
		bool bForce)
	{
		if (bHasFinalResult)
		{
			return bResultMember ? EObserve::Labelled : EObserve::NotLabelled;
		}
		if (bForce || bSnapshotTerminal || AgeTicks > ObserveHardBoundTicks)
		{
			return EObserve::Labelled;
		}
		return EObserve::Wait;
	}

	inline bool DeferredMaskExpired(int AgeTicks)
	{
		return AgeTicks > DeferredMaskMaxAgeTicks;
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

	enum class ELevel : unsigned char
	{
		Unknown = 0,
		Baseline = 1,
		Between = 2,
		AtHeld = 3
	};

	inline ELevel ClassifyLevel(ETexState State, int RenderResident, int Baseline, int HeldLevel)
	{
		if (State == ETexState::Unknown)
		{
			return ELevel::Unknown;
		}
		if (State == ETexState::Baseline)
		{
			return ELevel::Baseline;
		}
		if (HeldLevel <= 0 || HeldLevel >= Baseline)
		{
			return ELevel::AtHeld;
		}
		return RenderResident <= HeldLevel ? ELevel::AtHeld : ELevel::Between;
	}

	inline bool IsPartialHeldSet(const ELevel* Levels, int Num)
	{
		bool bAnyBelowBaseline = false;
		bool bAnyShortOfHeld = false;
		for (int i = 0; i < Num; ++i)
		{
			if (Levels[i] == ELevel::AtHeld || Levels[i] == ELevel::Between)
			{
				bAnyBelowBaseline = true;
			}
			if (Levels[i] != ELevel::AtHeld)
			{
				bAnyShortOfHeld = true;
			}
		}
		return bAnyBelowBaseline && bAnyShortOfHeld;
	}

	inline const char* DescribeLevel(ELevel L)
	{
		switch (L)
		{
		case ELevel::Baseline: return "baseline";
		case ELevel::Between:  return "between";
		case ELevel::AtHeld:   return "held";
		default:               return "unknown";
		}
	}

	struct FPartialEdgeTrack
	{
		static constexpr int Cap = 32;
		int PartialSI[Cap] = {};
		int NumPartial = 0;
		int PartialOverflow = 0;
		int FirstFullSI = -1;
		int LastFullSI = -1;

		void Observe(int SI, bool bMember, bool bPartial)
		{
			if (!bMember)
			{
				return;
			}
			if (!bPartial)
			{
				FirstFullSI = (FirstFullSI < 0 || SI < FirstFullSI) ? SI : FirstFullSI;
				LastFullSI = SI > LastFullSI ? SI : LastFullSI;
				return;
			}
			for (int i = 0; i < NumPartial; ++i)
			{
				if (PartialSI[i] == SI)
				{
					return;
				}
			}
			if (NumPartial == Cap)
			{
				++PartialOverflow;
				return;
			}
			int Pos = NumPartial;
			while (Pos > 0 && PartialSI[Pos - 1] > SI)
			{
				PartialSI[Pos] = PartialSI[Pos - 1];
				--Pos;
			}
			PartialSI[Pos] = SI;
			++NumPartial;
		}

		bool IsOnsetEdge(int SI) const
		{
			return FirstFullSI < 0 || SI < FirstFullSI;
		}

		int CountOnset() const
		{
			int N = 0;
			for (int i = 0; i < NumPartial; ++i)
			{
				if (IsOnsetEdge(PartialSI[i])) { ++N; }
			}
			return N;
		}

		int CountOffset() const
		{
			return NumPartial - CountOnset() - CountMid();
		}

		int CountMid() const
		{
			int N = 0;
			for (int i = 0; i < NumPartial; ++i)
			{
				if (!IsOnsetEdge(PartialSI[i]) && PartialSI[i] < LastFullSI) { ++N; }
			}
			return N;
		}
	};
}
