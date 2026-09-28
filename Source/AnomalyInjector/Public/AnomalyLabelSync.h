#pragma once

namespace AnomalyLabelSync
{
	enum class EEntryEmit : unsigned char
	{
		Normal = 0,
		Suppress = 1,
		TransitionOnly = 2
	};

	static constexpr int MaxTransitionFrames = 64;
	static constexpr int DefaultOnFramesTemporal = 2;
	static constexpr int DefaultOffFramesTemporal = 8;
	static constexpr int DefaultHideFramesTemporal = 1;

	static constexpr int AaNone = 0;
	static constexpr int AaFxaa = 1;
	static constexpr int AaTemporal = 2;
	static constexpr int AaMsaa = 3;
	static constexpr int AaTsr = 4;

	inline bool IsTemporalMethod(int Method)
	{
		return Method == AaTemporal || Method == AaTsr;
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
			int Best = -1;
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
			int Best = -1;
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
			if (Prev >= 0 && SI - Prev <= OffFrames)
			{
				bOutOff = true;
			}
		}

		bool OffWindowPassed(int SI, int OffFrames) const
		{
			const int Last = LastMember();
			return Last < 0 || SI - Last > OffFrames;
		}
	};

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
		long long ReleasableSince = -1;
	};

	inline int PickRecycleVictim(const FRecycleCandidate* C, int N)
	{
		int Best = -1;
		for (int i = 0; i < N; ++i)
		{
			if (!C[i].bReleasable || C[i].bAlreadyRecycled || C[i].Tag == 0)
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
