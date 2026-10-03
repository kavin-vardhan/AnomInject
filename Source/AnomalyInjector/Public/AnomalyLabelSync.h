#pragma once

namespace AnomalyLabelSync
{
	enum class EEntryEmit : unsigned char
	{
		Normal = 0,
		Suppress = 1,
		TransitionOnly = 2
	};

	static constexpr int MaxTransitionFrames = 256;
	static constexpr int DefaultOnFramesTemporal = 3;
	static constexpr int DefaultOffFramesTemporal = 256;
	static constexpr int DefaultHideFramesTemporal = 1;

	static constexpr unsigned short ReasonTemporal = 1;
	static constexpr unsigned short ReasonHideReturn = 2;
	static constexpr unsigned short ReasonPartial = 4;
	static constexpr unsigned short ReasonCameraUnconfirmed = 8;
	static constexpr unsigned short ReasonUnresolved = 16;
	static constexpr unsigned short ReasonEffectInterrupted = 32;
	static constexpr unsigned short ReasonNaniteUnmaskable = 64;
	static constexpr unsigned short ReasonCaptureUnpaired = 128;
	static constexpr unsigned short ReasonPieEndSettle = 256;
	static constexpr int NumReasons = 9;

	inline const char* DescribeReasonBit(int Bit)
	{
		switch (Bit)
		{
		case 0: return "temporal_aa";
		case 1: return "hide_return";
		case 2: return "partial";
		case 3: return "camera_clipping_unconfirmed";
		case 4: return "unresolved";
		case 5: return "effect_interrupted";
		case 6: return "nanite_unmaskable";
		case 7: return "capture_unpaired";
		case 8: return "pie_end_settle";
		default: return "unknown";
		}
	}

	inline unsigned short ReasonsOrLegacy(unsigned short Value)
	{
		return (Value != 0 && (Value & (ReasonTemporal | ReasonHideReturn | ReasonPartial | ReasonCameraUnconfirmed
			| ReasonUnresolved | ReasonEffectInterrupted | ReasonNaniteUnmaskable | ReasonCaptureUnpaired
			| ReasonPieEndSettle)) == 0)
			? ReasonTemporal : Value;
	}

	enum class EAnnotationPolicy : unsigned char
	{
		FireWindow = 0,
		ActorHidden = 1,
		AnomalyState = 2,
		RenderHeldWindow = 3
	};

	inline const char* DescribeAnnotationPolicy(EAnnotationPolicy Policy)
	{
		switch (Policy)
		{
		case EAnnotationPolicy::ActorHidden:      return "actor_hidden";
		case EAnnotationPolicy::AnomalyState:     return "anomaly_state";
		case EAnnotationPolicy::RenderHeldWindow: return "render_held_window";
		default:                                  return "fire_window";
		}
	}

	inline bool IsAnnotationMember(EAnnotationPolicy Policy, bool bActive, bool bOnScreen, bool bInstalled)
	{
		return Policy == EAnnotationPolicy::FireWindow ? (bOnScreen && bInstalled) : bActive;
	}

	inline bool IsEntryLabelled(bool bNormalEmit, EAnnotationPolicy Policy, bool bActive, bool bOnScreen, bool bInstalled)
	{
		return bNormalEmit && IsAnnotationMember(Policy, bActive, bOnScreen, bInstalled);
	}

	inline bool IsEffectInterrupted(EAnnotationPolicy Policy, bool bInstalled)
	{
		return Policy == EAnnotationPolicy::FireWindow && !bInstalled;
	}

	inline EEntryEmit DecideInterruptedEntry(EEntryEmit Current, EAnnotationPolicy Policy, bool bInstalled)
	{
		if (!IsEffectInterrupted(Policy, bInstalled) || Current == EEntryEmit::Suppress)
		{
			return Current;
		}
		return EEntryEmit::TransitionOnly;
	}

	inline bool IsAnnotationMemberGated(EAnnotationPolicy Policy, bool bActive, bool bOnScreen, bool bInstalled, bool bNaniteBlocked)
	{
		return !bNaniteBlocked && IsAnnotationMember(Policy, bActive, bOnScreen, bInstalled);
	}

	inline EEntryEmit DecideNaniteEntry(EEntryEmit Current, bool bNaniteBlocked)
	{
		if (!bNaniteBlocked || Current == EEntryEmit::Suppress)
		{
			return Current;
		}
		return EEntryEmit::TransitionOnly;
	}

	inline bool IsAnnotationMemberPaired(EAnnotationPolicy Policy, bool bActive, bool bOnScreen, bool bInstalled,
		bool bNaniteBlocked, bool bCaptureUnpaired)
	{
		return !bCaptureUnpaired && IsAnnotationMemberGated(Policy, bActive, bOnScreen, bInstalled, bNaniteBlocked);
	}

	inline EEntryEmit DecideUnpairedEntry(EEntryEmit Current, bool bCaptureUnpaired)
	{
		if (!bCaptureUnpaired || Current == EEntryEmit::Suppress)
		{
			return Current;
		}
		return EEntryEmit::TransitionOnly;
	}

	enum class EPieSettleAction : unsigned char
	{
		None = 0,
		FlagPresent = 1,
		FlagGone = 2
	};

	inline bool TrackPieEndSettle(bool bPieWorld, EAnnotationPolicy Policy, bool bLabelledNow)
	{
		return bPieWorld && Policy == EAnnotationPolicy::FireWindow && bLabelledNow;
	}

	inline EPieSettleAction DecidePieEndSettle(bool bWasTrackedLabelled, bool bPresentNow, bool bLabelledNow)
	{
		if (!bWasTrackedLabelled)
		{
			return EPieSettleAction::None;
		}
		if (!bPresentNow)
		{
			return EPieSettleAction::FlagGone;
		}
		return bLabelledNow ? EPieSettleAction::None : EPieSettleAction::FlagPresent;
	}

	inline EEntryEmit DecidePieSettleEntry(EEntryEmit Current)
	{
		return Current == EEntryEmit::Suppress ? Current : EEntryEmit::TransitionOnly;
	}

	struct FNaniteRevertGate
	{
		bool bRequested = false;
		bool bDone = false;
	};

	inline bool ShouldRequestNaniteRevert(const FNaniteRevertGate& G, bool bNaniteBlocked, bool bEffectActive)
	{
		return bNaniteBlocked && bEffectActive && !G.bRequested && !G.bDone;
	}

	static constexpr int AaNone = 0;
	static constexpr int AaFxaa = 1;
	static constexpr int AaTemporal = 2;
	static constexpr int AaMsaa = 3;
	static constexpr int AaTsr = 4;

	inline bool IsTemporalMethod(int Method)
	{
		return Method != AaNone && Method != AaFxaa && Method != AaMsaa;
	}

	inline bool HasTemporalHistory(int Method, bool bViewKnown, bool bUpscaler)
	{
		return !bViewKnown || bUpscaler || IsTemporalMethod(Method);
	}

	inline const char* DescribeAaMethod(int Method)
	{
		switch (Method)
		{
		case AaNone: return "none";
		case AaFxaa: return "fxaa";
		case AaTemporal: return "taa";
		case AaMsaa: return "msaa";
		case AaTsr: return "tsr";
		default: return "unknown";
		}
	}

	inline int ResolveTransitionFrames(int Configured, int TemporalDefault, bool bTemporal)
	{
		if (!bTemporal)
		{
			return 0;
		}
		if (Configured < 0)
		{
			return TemporalDefault;
		}
		return Configured > MaxTransitionFrames ? MaxTransitionFrames : Configured;
	}

	inline EEntryEmit ModeAt(const EEntryEmit* Modes, int NumModes, int Index)
	{
		return (Modes && Index >= 0 && Index < NumModes) ? Modes[Index] : EEntryEmit::Normal;
	}

	inline bool FramePresent(const EEntryEmit* Modes, int NumModes, int NumFires)
	{
		for (int i = 0; i < NumFires; ++i)
		{
			if (ModeAt(Modes, NumModes, i) == EEntryEmit::Normal)
			{
				return true;
			}
		}
		return false;
	}

	inline EEntryEmit DecideRenderTruthEntry(bool bMember, bool bOffTransition)
	{
		if (bMember)
		{
			return EEntryEmit::Normal;
		}
		return bOffTransition ? EEntryEmit::TransitionOnly : EEntryEmit::Suppress;
	}

	struct FEventTransitionTrack
	{
		static constexpr int FirstCap = MaxTransitionFrames;
		static constexpr int RecentCap = 2 * MaxTransitionFrames;

		int First[FirstCap] = {};
		int NumFirst = 0;
		int Recent[RecentCap] = {};
		int NumRecent = 0;
		static constexpr int NoMember = -2147483647 - 1;

		int MaxSISeen = -1;
		int OutOfOrder = 0;
		bool bOnsetPast = false;
		bool bOffDone = false;

		bool HasMember(int SI) const
		{
			for (int i = 0; i < NumRecent; ++i)
			{
				if (Recent[i] == SI) { return true; }
			}
			for (int i = 0; i < NumFirst; ++i)
			{
				if (First[i] == SI) { return true; }
			}
			return false;
		}

		int CountFirstBelow(int SI) const
		{
			int N = 0;
			for (int i = 0; i < NumFirst; ++i)
			{
				if (First[i] < SI) { ++N; }
			}
			return N;
		}

		int PrevMember(int SI) const
		{
			int Best = NoMember;
			for (int i = 0; i < NumRecent; ++i)
			{
				if (Recent[i] < SI && Recent[i] > Best) { Best = Recent[i]; }
			}
			for (int i = 0; i < NumFirst; ++i)
			{
				if (First[i] < SI && First[i] > Best) { Best = First[i]; }
			}
			return Best;
		}

		int LastMember() const
		{
			int Best = NoMember;
			for (int i = 0; i < NumRecent; ++i)
			{
				if (Recent[i] > Best) { Best = Recent[i]; }
			}
			return Best;
		}

		static void InsertSorted(int* Arr, int& Num, int Cap, int SI, bool bDropLargest)
		{
			if (Num == Cap)
			{
				if (bDropLargest)
				{
					if (SI >= Arr[Num - 1]) { return; }
					--Num;
				}
				else
				{
					if (SI <= Arr[0]) { return; }
					for (int i = 1; i < Num; ++i) { Arr[i - 1] = Arr[i]; }
					--Num;
				}
			}
			int Pos = Num;
			while (Pos > 0 && Arr[Pos - 1] > SI)
			{
				Arr[Pos] = Arr[Pos - 1];
				--Pos;
			}
			Arr[Pos] = SI;
			++Num;
		}

		void AddMember(int SI)
		{
			if (HasMember(SI))
			{
				return;
			}
			InsertSorted(First, NumFirst, FirstCap, SI, true);
			InsertSorted(Recent, NumRecent, RecentCap, SI, false);
		}

		void Observe(int SI, bool bMember, int OnFrames, int OffFrames, bool& bOutOn, bool& bOutOff)
		{
			bOutOn = false;
			bOutOff = false;
			if (SI < MaxSISeen)
			{
				++OutOfOrder;
			}
			else
			{
				MaxSISeen = SI;
			}
			if (bMember)
			{
				bOffDone = false;
				if (!bOnsetPast && CountFirstBelow(SI) < OnFrames && !HasMember(SI))
				{
					bOutOn = true;
				}
				AddMember(SI);
				return;
			}
			const int Prev = PrevMember(SI);
			if (Prev != NoMember && SI - Prev <= OffFrames)
			{
				bOutOff = true;
			}
		}

		bool OffWindowPassed(int SI, int OffFrames) const
		{
			const int Last = LastMember();
			return Last == NoMember || SI - Last > OffFrames;
		}

		void Rebase(int Offset)
		{
			for (int i = 0; i < NumFirst; ++i) { First[i] -= Offset; }
			for (int i = 0; i < NumRecent; ++i) { Recent[i] -= Offset; }
			MaxSISeen -= Offset;
		}
	};

	inline bool ShouldCarryTransitionTrack(const FEventTransitionTrack& T, int LastSIOfRun, int OffFrames,
		bool bEventContinues)
	{
		if (T.LastMember() == FEventTransitionTrack::NoMember)
		{
			return false;
		}
		if (bEventContinues)
		{
			return true;
		}
		return !T.bOffDone && !T.OffWindowPassed(LastSIOfRun, OffFrames);
	}

	inline FEventTransitionTrack CarryTransitionTrack(const FEventTransitionTrack& T, int LastSIOfRun)
	{
		FEventTransitionTrack C = T;
		C.Rebase(LastSIOfRun + 1);
		C.OutOfOrder = 0;
		return C;
	}

	enum class ECarrySource : unsigned char
	{
		None = 0,
		AttachedTrail = 1,
		DetachedTrail = 2,
		CarriedTail = 3
	};

	struct FCarryDecision
	{
		bool bCarry = false;
		bool bDetachedTail = false;
	};

	inline FCarryDecision DecideRunEndCarry(const FEventTransitionTrack& T, int LastSIOfRun, int OffFrames, ECarrySource Source)
	{
		FCarryDecision D;
		if (Source == ECarrySource::None)
		{
			return D;
		}
		const bool bContinues = Source == ECarrySource::AttachedTrail;
		D.bCarry = ShouldCarryTransitionTrack(T, LastSIOfRun, OffFrames, bContinues);
		D.bDetachedTail = D.bCarry && !bContinues;
		return D;
	}

	enum class ERetireAction : unsigned char
	{
		Leave = 0,
		RestoreValueAndFlag = 1,
		RestoreValueKeepHostFlag = 2
	};

	inline ERetireAction DecideRetire(bool bTracked, int CurrentValue, bool bRenderCustomDepth, int RetiredValue)
	{
		if (!bTracked || CurrentValue != RetiredValue)
		{
			return ERetireAction::Leave;
		}
		return bRenderCustomDepth ? ERetireAction::RestoreValueAndFlag : ERetireAction::RestoreValueKeepHostFlag;
	}

	inline ERetireAction DecideRetireLegacy(bool bTracked, int CurrentValue, bool bRenderCustomDepth, int RetiredValue)
	{
		if (!bTracked || !bRenderCustomDepth || CurrentValue != RetiredValue)
		{
			return ERetireAction::Leave;
		}
		return ERetireAction::RestoreValueAndFlag;
	}

	inline bool RetirementVerified(const int* ValuesAfter, int Num, int RetiredValue)
	{
		for (int i = 0; i < Num; ++i)
		{
			if (ValuesAfter[i] == RetiredValue)
			{
				return false;
			}
		}
		return true;
	}

	static constexpr int AppliedMaskBase = 192;

	inline unsigned long long AppliedBit(int Value)
	{
		return (Value >= AppliedMaskBase && Value < AppliedMaskBase + 64) ? (1ull << (Value - AppliedMaskBase)) : 0ull;
	}

	struct FRetireHolder
	{
		bool bValid = true;
		bool bTracked = false;
		int PriorValue = 0;
		bool bPriorCustomDepth = false;
		int Value = 0;
		bool bCustomDepth = false;
		unsigned long long AppliedMask = 0;
		bool bOwnedByFormerOwner = false;
		bool bChecked = false;
		bool bSetValue = false;
		bool bSetFlag = false;
		bool bUntrack = false;
		bool bClearApplied = false;
	};

	struct FRetireOutcome
	{
		int Restored = 0;
		int RestoredValueOnly = 0;
		int Checked = 0;
		int Remaining = 0;
		bool bVerified = true;
	};

	inline bool IsRetireIdentity(const FRetireHolder& H, int RetiredValue)
	{
		return H.bTracked || (H.AppliedMask & AppliedBit(RetiredValue)) != 0 || H.bOwnedByFormerOwner;
	}

	inline FRetireOutcome RetireHolders(FRetireHolder* Holders, int Num, int RetiredValue)
	{
		FRetireOutcome R;
		for (int i = 0; i < Num; ++i)
		{
			Holders[i].bChecked = Holders[i].bValid && IsRetireIdentity(Holders[i], RetiredValue);
		}
		for (int i = 0; i < Num; ++i)
		{
			FRetireHolder& H = Holders[i];
			if (!H.bValid || !H.bTracked)
			{
				continue;
			}
			const ERetireAction Action = DecideRetire(true, H.Value, H.bCustomDepth, RetiredValue);
			if (Action == ERetireAction::Leave)
			{
				continue;
			}
			H.Value = H.PriorValue;
			H.bSetValue = true;
			if (Action == ERetireAction::RestoreValueAndFlag)
			{
				H.bCustomDepth = H.bPriorCustomDepth;
				H.bSetFlag = true;
				++R.Restored;
			}
			else
			{
				++R.RestoredValueOnly;
			}
			H.bTracked = false;
			H.bUntrack = true;
		}
		for (int i = 0; i < Num; ++i)
		{
			if (!Holders[i].bChecked)
			{
				continue;
			}
			++R.Checked;
			if (Holders[i].Value == RetiredValue)
			{
				++R.Remaining;
			}
		}
		R.bVerified = R.Remaining == 0;
		if (R.bVerified)
		{
			for (int i = 0; i < Num; ++i)
			{
				Holders[i].bClearApplied = (Holders[i].AppliedMask & AppliedBit(RetiredValue)) != 0;
			}
		}
		return R;
	}

	struct FHideReturnTrack
	{
		bool bPrevHidden = false;
		int Remaining = 0;
		bool bGone = false;
	};

	inline bool StepHideLive(FHideReturnTrack& T, bool bHiddenNow, int Frames)
	{
		if (bHiddenNow)
		{
			T.bPrevHidden = true;
			T.Remaining = 0;
			return false;
		}
		if (T.bPrevHidden)
		{
			T.Remaining = Frames;
			T.bPrevHidden = false;
		}
		if (T.Remaining > 0)
		{
			--T.Remaining;
			return true;
		}
		return false;
	}

	inline bool StepHideGone(FHideReturnTrack& T, int Frames)
	{
		if (!T.bGone)
		{
			T.bGone = true;
			if (T.bPrevHidden)
			{
				T.Remaining = Frames;
				T.bPrevHidden = false;
			}
		}
		if (T.Remaining > 0)
		{
			--T.Remaining;
			return true;
		}
		return false;
	}

	inline bool HideTrackDone(const FHideReturnTrack& T)
	{
		return T.bGone && T.Remaining <= 0;
	}

	inline bool ShouldCarryHideTrack(const FHideReturnTrack& T)
	{
		return T.bPrevHidden || T.Remaining > 0;
	}

	struct FTagReleaseInputs
	{
		int Tag = 0;
		bool bAlreadyRecycled = false;
		bool bFireLive = false;
		bool bTrailBlocks = false;
		bool bInPendingSnapshot = false;
		bool bInPendingTargetMask = false;
		bool bM26ArmInFlight = false;
		bool bM26MayArmAgain = false;
	};

	inline bool M26MayArmAgain(int ArmsIssued, int MaxArms, bool bActorValid, bool bKnownUnmeasurable, bool bTagFailed,
		bool bAwaitLabelledGated, bool bFireLive)
	{
		if (ArmsIssued >= MaxArms || !bActorValid || bKnownUnmeasurable || bTagFailed)
		{
			return false;
		}
		if (bAwaitLabelledGated && !bFireLive)
		{
			return false;
		}
		return true;
	}

	inline bool TrailBlocksRelease(bool bTrailExists, bool bAttachedForProducts, bool bAllNotedProcessed)
	{
		return bTrailExists && (bAttachedForProducts || !bAllNotedProcessed);
	}

	inline bool IsTagReleasable(const FTagReleaseInputs& In)
	{
		return In.Tag != 0
			&& !In.bAlreadyRecycled
			&& !In.bFireLive
			&& !In.bTrailBlocks
			&& !In.bInPendingSnapshot
			&& !In.bInPendingTargetMask
			&& !In.bM26ArmInFlight
			&& !In.bM26MayArmAgain;
	}

	struct FRecycleCandidate
	{
		int Record = -1;
		int Tag = 0;
		bool bReleasable = false;
		bool bAlreadyRecycled = false;
		bool bQuarantined = false;
		long long ReleasableSince = -1;
	};

	inline bool IsTagValueFree(bool bAssignable, bool bEventClaimed, bool bCensusClaimed, bool bQuarantined)
	{
		return bAssignable && !bEventClaimed && !bCensusClaimed && !bQuarantined;
	}

	struct FPriorRestoreInputs
	{
		int PriorValue = 0;
		int AssignableMin = 200;
		int AssignableMax = 254;
		bool bEventClaimed = false;
		bool bCensusClaimed = false;
		bool bAlreadyQuarantined = false;
	};

	struct FPriorRestoreVerdict
	{
		bool bCollision = false;
		bool bQuarantine = false;
	};

	inline FPriorRestoreVerdict CheckPriorRestore(const FPriorRestoreInputs& In)
	{
		FPriorRestoreVerdict V;
		const bool bPluginValue = In.PriorValue >= In.AssignableMin && In.PriorValue <= In.AssignableMax;
		V.bCollision = bPluginValue && (In.bEventClaimed || In.bCensusClaimed);
		V.bQuarantine = V.bCollision && !In.bAlreadyQuarantined;
		return V;
	}

	inline int PickRecycleVictim(const FRecycleCandidate* C, int N)
	{
		int Best = -1;
		for (int i = 0; i < N; ++i)
		{
			if (!C[i].bReleasable || C[i].bAlreadyRecycled || C[i].bQuarantined || C[i].Tag == 0)
			{
				continue;
			}
			if (Best < 0
				|| C[i].ReleasableSince < C[Best].ReleasableSince
				|| (C[i].ReleasableSince == C[Best].ReleasableSince && C[i].Record < C[Best].Record))
			{
				Best = i;
			}
		}
		return Best;
	}

	inline bool AnyAlias(const int* Tags, int N)
	{
		for (int i = 0; i < N; ++i)
		{
			if (Tags[i] == 0) { continue; }
			for (int j = i + 1; j < N; ++j)
			{
				if (Tags[i] == Tags[j]) { return true; }
			}
		}
		return false;
	}
}
