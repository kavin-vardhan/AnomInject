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
		int LateReceipts = 0;
		int Overflows = 0;
		bool bOpen = false;
		bool bClosing = false;
		bool bClosed = false;
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
			if (!bOpen || SessionIndex <= LastArmedSI)
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
			}
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
	};

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
}
