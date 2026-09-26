#pragma once

namespace TexCorruptPure
{
	namespace Rank
	{
		constexpr int S1 = 1;
		constexpr int S2 = 2;
		constexpr int S3 = 3;
		constexpr int S4 = 4;
		constexpr int S5 = 5;
		constexpr int S6 = 6;
		constexpr int S7 = 7;
		constexpr int S8 = 8;
		constexpr int TBase = 10;
		constexpr int A2 = 31;
		constexpr int A3 = 32;
		constexpr int A5 = 35;
		constexpr int A6 = 36;

		inline int ForBindingStep(int TStep)
		{
			return TBase + TStep;
		}
	}

	enum class EFmt : int
	{
		DXT1,
		DXT5,
		BC7,
		BGRA8,
		BC4,
		G8,
		BC5,
		Other
	};

	enum class EClassP : int
	{
		None,
		Colour,
		Data,
		Normal
	};

	enum class EClassWhy : int
	{
		Ok,
		NormalMapNotBc5,
		Bc5NotNormalMap,
		SrgbSingleChannel,
		FormatNotAdmitted
	};

	inline int MaxInt(int A, int B)
	{
		return A > B ? A : B;
	}

	inline int FullChainLength(int W, int H)
	{
		int Longest = MaxInt(W, H);
		int Levels = 1;
		while (Longest > 1)
		{
			Longest >>= 1;
			++Levels;
		}
		return Levels;
	}

	inline long long ChainBytes(int W, int H, int M)
	{
		long long Sum = 0;
		const int Levels = M < 1 ? 1 : M;
		for (int m = 0; m < Levels; ++m)
		{
			Sum += 4LL * (long long)MaxInt(1, W >> m) * (long long)MaxInt(1, H >> m);
		}
		return Sum;
	}

	inline long long ScratchBytes(int W, int H, int M)
	{
		if (M <= 1)
		{
			return 0;
		}
		return ChainBytes(W, H, M) - 4LL * (long long)W * (long long)H;
	}

	inline bool IsAdmittedChainShape(int M, int W, int H, const int* LevelW, const int* LevelH)
	{
		if (M < 1 || W <= 0 || H <= 0)
		{
			return false;
		}
		if (M == 1)
		{
			return true;
		}
		if (M != FullChainLength(W, H))
		{
			return false;
		}
		for (int m = 0; m < M; ++m)
		{
			if (LevelW[m] != MaxInt(1, W >> m) || LevelH[m] != MaxInt(1, H >> m))
			{
				return false;
			}
		}
		return true;
	}

	inline EClassP ClassifyFormat(EFmt Fmt, bool bSRGB, bool bNormalMap, EClassWhy& OutWhy)
	{
		OutWhy = EClassWhy::Ok;
		if (bNormalMap)
		{
			if (Fmt == EFmt::BC5)
			{
				return EClassP::Normal;
			}
			OutWhy = EClassWhy::NormalMapNotBc5;
			return EClassP::None;
		}
		switch (Fmt)
		{
		case EFmt::BC5:
			OutWhy = EClassWhy::Bc5NotNormalMap;
			return EClassP::None;
		case EFmt::DXT1:
		case EFmt::DXT5:
		case EFmt::BC7:
		case EFmt::BGRA8:
			return bSRGB ? EClassP::Colour : EClassP::Data;
		case EFmt::BC4:
		case EFmt::G8:
			if (bSRGB)
			{
				OutWhy = EClassWhy::SrgbSingleChannel;
				return EClassP::None;
			}
			return EClassP::Data;
		default:
			OutWhy = EClassWhy::FormatNotAdmitted;
			return EClassP::None;
		}
	}

	struct FReqTex
	{
		long long Id = 0;
		int W = 0;
		int H = 0;
		int M = 0;
		bool bSRGB = false;
	};

	struct FRequirement
	{
		long long Chains = 0;
		long long Scratch = 0;
		int DistinctTextures = 0;
		int ScratchClasses = 0;

		long long Total() const { return Chains + Scratch; }
	};

	inline bool IsFirstOccurrence(const FReqTex* Tex, int i)
	{
		for (int j = 0; j < i; ++j)
		{
			if (Tex[j].Id == Tex[i].Id)
			{
				return false;
			}
		}
		return true;
	}

	inline bool OpensScratchClass(const FReqTex* Tex, int i)
	{
		if (Tex[i].M <= 1)
		{
			return false;
		}
		bool bClassSeen = false;
		for (int j = 0; j < i && !bClassSeen; ++j)
		{
			bClassSeen = Tex[j].M > 1 && Tex[j].W == Tex[i].W && Tex[j].H == Tex[i].H && Tex[j].bSRGB == Tex[i].bSRGB;
		}
		return !bClassSeen;
	}

	inline FRequirement EventRequirement(const FReqTex* Tex, int N)
	{
		FRequirement R;
		for (int i = 0; i < N; ++i)
		{
			if (!IsFirstOccurrence(Tex, i))
			{
				continue;
			}
			++R.DistinctTextures;
			R.Chains += ChainBytes(Tex[i].W, Tex[i].H, Tex[i].M);
			if (OpensScratchClass(Tex, i))
			{
				++R.ScratchClasses;
				R.Scratch += ScratchBytes(Tex[i].W, Tex[i].H, Tex[i].M);
			}
		}
		return R;
	}

	struct FAllocStep
	{
		int Kind = 0;
		int Tex = -1;
		int Level = 0;
		int W = 0;
		int H = 0;
		long long Bytes = 0;
	};

	namespace AllocKind
	{
		constexpr int Output = 0;
		constexpr int Scratch = 1;
	}

	inline long long LevelBytes(int W, int H, int Level)
	{
		return 4LL * (long long)MaxInt(1, W >> Level) * (long long)MaxInt(1, H >> Level);
	}

	inline int PlanAllocations(const FReqTex* Tex, int N, FAllocStep* Out, int MaxOut)
	{
		int Count = 0;
		for (int i = 0; i < N; ++i)
		{
			if (!IsFirstOccurrence(Tex, i))
			{
				continue;
			}
			if (Count >= MaxOut)
			{
				return -1;
			}
			FAllocStep& O = Out[Count++];
			O.Kind = AllocKind::Output;
			O.Tex = i;
			O.Level = 0;
			O.W = Tex[i].W;
			O.H = Tex[i].H;
			O.Bytes = ChainBytes(Tex[i].W, Tex[i].H, Tex[i].M);
			if (!OpensScratchClass(Tex, i))
			{
				continue;
			}
			for (int m = 1; m < Tex[i].M; ++m)
			{
				if (Count >= MaxOut)
				{
					return -1;
				}
				FAllocStep& S = Out[Count++];
				S.Kind = AllocKind::Scratch;
				S.Tex = i;
				S.Level = m;
				S.W = MaxInt(1, Tex[i].W >> m);
				S.H = MaxInt(1, Tex[i].H >> m);
				S.Bytes = LevelBytes(Tex[i].W, Tex[i].H, m);
			}
		}
		return Count;
	}

	inline int MaxAllocSteps(int N)
	{
		return N * 16;
	}

	struct FLedgerCore
	{
		static constexpr int MaxBuckets = 16;

		long long Live = 0;
		long long Peak = 0;
		unsigned long long Due[MaxBuckets] = {};
		long long Bytes[MaxBuckets] = {};
		int Buckets = 0;

		long long PendingSum() const
		{
			long long Sum = 0;
			for (int i = 0; i < Buckets; ++i)
			{
				Sum += Bytes[i];
			}
			return Sum;
		}

		long long Available(long long Cap) const
		{
			return Cap - Live - PendingSum();
		}

		void NotePeak()
		{
			const long long Now = Live + PendingSum();
			if (Now > Peak)
			{
				Peak = Now;
			}
		}

		bool Reserve(long long B, long long Cap)
		{
			if (B < 0 || B > Available(Cap))
			{
				return false;
			}
			Live += B;
			NotePeak();
			return true;
		}

		void ForceReserve(long long B)
		{
			if (B <= 0)
			{
				return;
			}
			Live += B;
			NotePeak();
		}

		void Unreserve(long long B)
		{
			if (B <= 0)
			{
				return;
			}
			Live = Live > B ? Live - B : 0;
		}

		void ReleaseToPending(long long B, unsigned long long Frame)
		{
			if (B <= 0)
			{
				return;
			}
			Live = Live > B ? Live - B : 0;
			const unsigned long long D = Frame + 2;
			for (int i = 0; i < Buckets; ++i)
			{
				if (Due[i] == D)
				{
					Bytes[i] += B;
					NotePeak();
					return;
				}
			}
			if (Buckets < MaxBuckets)
			{
				Due[Buckets] = D;
				Bytes[Buckets] = B;
				++Buckets;
			}
			else
			{
				int Latest = 0;
				for (int i = 1; i < Buckets; ++i)
				{
					if (Due[i] > Due[Latest])
					{
						Latest = i;
					}
				}
				Bytes[Latest] += B;
				if (Due[Latest] < D)
				{
					Due[Latest] = D;
				}
			}
			NotePeak();
		}

		void Tick(unsigned long long Frame)
		{
			for (int i = Buckets - 1; i >= 0; --i)
			{
				if (Due[i] <= Frame)
				{
					Due[i] = Due[Buckets - 1];
					Bytes[i] = Bytes[Buckets - 1];
					--Buckets;
				}
			}
		}
	};

	struct FEventAccount
	{
		long long Reserved = 0;
		long long Created = 0;

		bool Reserve(FLedgerCore& L, long long B, long long Cap)
		{
			if (!L.Reserve(B, Cap))
			{
				return false;
			}
			Reserved += B;
			return true;
		}

		bool NoteCreated(FLedgerCore& L, long long B)
		{
			if (B <= 0)
			{
				return B == 0;
			}
			Created += B;
			if (Created <= Reserved)
			{
				return true;
			}
			const long long Excess = Created - Reserved;
			L.ForceReserve(Excess);
			Reserved = Created;
			return false;
		}

		bool ReleaseCreated(FLedgerCore& L, long long B, unsigned long long Frame)
		{
			if (B <= 0)
			{
				return B == 0;
			}
			const bool bOk = B <= Created;
			const long long Moved = bOk ? B : Created;
			L.ReleaseToPending(Moved, Frame);
			Created -= Moved;
			Reserved -= Moved;
			return bOk;
		}

		void Close(FLedgerCore& L, unsigned long long Frame)
		{
			L.ReleaseToPending(Created, Frame);
			L.Unreserve(Reserved - Created);
			Reserved = 0;
			Created = 0;
		}
	};

	enum class EHeld : int
	{
		Held,
		NoEvent,
		NoExpectedSet,
		SlotNotInstalled,
		BindingReadback
	};

	inline const char* LexHeld(EHeld H)
	{
		switch (H)
		{
		case EHeld::Held:             return "installed";
		case EHeld::NoEvent:          return "no_event";
		case EHeld::NoExpectedSet:    return "no_expected_set";
		case EHeld::SlotNotInstalled: return "slot_not_installed";
		default:                      return "binding_readback";
		}
	}

	inline EHeld ConditionHeld(bool bActive, const bool* SlotHoldsMid, int NumSlots, const bool* BindingReadsBack, int NumBindings,
		int& OutFirstBad)
	{
		OutFirstBad = -1;
		if (!bActive)
		{
			return EHeld::NoEvent;
		}
		if (NumSlots <= 0 || NumBindings <= 0)
		{
			return EHeld::NoExpectedSet;
		}
		for (int i = 0; i < NumSlots; ++i)
		{
			if (!SlotHoldsMid[i])
			{
				OutFirstBad = i;
				return EHeld::SlotNotInstalled;
			}
		}
		for (int i = 0; i < NumBindings; ++i)
		{
			if (!BindingReadsBack[i])
			{
				OutFirstBad = i;
				return EHeld::BindingReadback;
			}
		}
		return EHeld::Held;
	}

	namespace Usage
	{
		constexpr unsigned SkeletalMesh = 1u << 0;
		constexpr unsigned Clothing = 1u << 1;
		constexpr unsigned MorphTargets = 1u << 2;
		constexpr unsigned InstancedStaticMeshes = 1u << 3;
		constexpr unsigned Nanite = 1u << 4;
		constexpr unsigned SplineMesh = 1u << 5;
		constexpr unsigned StaticLighting = 1u << 6;
		constexpr int Count = 7;
	}

	enum class EMeshKind : int
	{
		Unknown,
		Static,
		Skinned
	};

	enum class EUsageGap : int
	{
		None,
		UnknownComponent,
		NoRenderData,
		NoLods
	};

	inline const char* LexUsageGap(EUsageGap G)
	{
		switch (G)
		{
		case EUsageGap::None:             return "none";
		case EUsageGap::UnknownComponent: return "unknown_component";
		case EUsageGap::NoRenderData:     return "no_render_data";
		default:                          return "no_lods";
		}
	}

	struct FUsageFacts
	{
		EMeshKind Kind = EMeshKind::Unknown;
		bool bRenderData = false;
		bool bInstanced = false;
		bool bSpline = false;
		bool bNanite = false;
		bool bForceVolumetric = false;
		bool bLodsShareLighting = false;
		int NumLods = 0;
		const bool* LodDataLit = nullptr;
		const bool* LodDataHasBuildData = nullptr;
		const bool* LodUsesSlot = nullptr;
		bool bSlotHasClothSection = false;
		bool bHasMorphTargets = false;
	};

	inline bool LodHasSurfaceLighting(const FUsageFacts& F, int L)
	{
		if (F.bForceVolumetric || L < 0 || L >= F.NumLods)
		{
			return false;
		}
		const bool bShare = F.bLodsShareLighting || F.bInstanced;
		if (L > 0 && bShare && F.LodDataHasBuildData[0])
		{
			return F.LodDataLit[0];
		}
		return F.LodDataLit[L];
	}

	inline unsigned RequiredUsages(const FUsageFacts& F, EUsageGap& OutGap)
	{
		OutGap = EUsageGap::None;
		if (F.Kind == EMeshKind::Unknown)
		{
			OutGap = EUsageGap::UnknownComponent;
			return 0;
		}
		if (!F.bRenderData)
		{
			OutGap = EUsageGap::NoRenderData;
			return 0;
		}
		if (F.Kind == EMeshKind::Skinned)
		{
			unsigned R = Usage::SkeletalMesh;
			if (F.bSlotHasClothSection)
			{
				R |= Usage::Clothing;
			}
			if (F.bHasMorphTargets)
			{
				R |= Usage::MorphTargets;
			}
			return R;
		}
		if (F.NumLods <= 0 || !F.LodDataLit || !F.LodDataHasBuildData || !F.LodUsesSlot)
		{
			OutGap = EUsageGap::NoLods;
			return 0;
		}
		unsigned R = 0;
		if (F.bInstanced)
		{
			R |= Usage::InstancedStaticMeshes;
		}
		if (F.bNanite && !F.bSpline)
		{
			R |= Usage::Nanite;
			if (!F.bForceVolumetric && F.LodDataLit[0])
			{
				R |= Usage::StaticLighting;
			}
			return R;
		}
		if (F.bSpline)
		{
			R |= Usage::SplineMesh;
		}
		for (int L = 0; L < F.NumLods; ++L)
		{
			if (F.LodUsesSlot[L] && LodHasSurfaceLighting(F, L))
			{
				R |= Usage::StaticLighting;
				break;
			}
		}
		return R;
	}

	enum class EChainWalk : int
	{
		Clean,
		RuntimeLink,
		LimitReached
	};

	template <typename FNode, typename FIsRuntime, typename FParentOf>
	inline EChainWalk WalkChain(FNode* Start, int MaxLinks, int& OutDepth, FIsRuntime IsRuntime, FParentOf ParentOf)
	{
		FNode* Link = Start;
		OutDepth = 0;
		for (int Depth = 0; Link; ++Depth)
		{
			OutDepth = Depth;
			if (Depth >= MaxLinks)
			{
				return EChainWalk::LimitReached;
			}
			if (IsRuntime(Link))
			{
				return EChainWalk::RuntimeLink;
			}
			Link = ParentOf(Link);
		}
		return EChainWalk::Clean;
	}

	inline double CollateralWindowSeconds(double DeltaSeconds)
	{
		const double Floor = 0.2;
		const double FromDelta = DeltaSeconds + 0.0001;
		return FromDelta > Floor ? FromDelta : Floor;
	}

	inline bool IsCollateralPrimitive(bool bRegistered, bool bIsTarget, double SecondsSinceOnScreen, double DeltaSeconds)
	{
		return bRegistered && !bIsTarget && SecondsSinceOnScreen <= CollateralWindowSeconds(DeltaSeconds);
	}

	inline unsigned long long PostRevertSampleFrame(unsigned long long RevertFrame)
	{
		return RevertFrame + 2;
	}

	template <typename FMat>
	inline FMat* MeasuredSlotMaterial(FMat* Assigned, FMat* EngineDefault, int& InOutNullSlots, int& InOutUnresolved)
	{
		if (Assigned)
		{
			return Assigned;
		}
		++InOutNullSlots;
		if (!EngineDefault)
		{
			++InOutUnresolved;
		}
		return EngineDefault;
	}

	enum class ECollEntry : int
	{
		Measured,
		NotTexture2D,
		Unresolved
	};

	inline ECollEntry ClassifyCollateralEntry(bool bResolved, bool bTexture2D, int& InOutUnresolved)
	{
		if (!bResolved)
		{
			++InOutUnresolved;
			return ECollEntry::Unresolved;
		}
		return bTexture2D ? ECollEntry::Measured : ECollEntry::NotTexture2D;
	}

	inline int CollateralIncompleteCount(int DroppedByCap, int UnmeasuredMaterials, int Unresolved, int UnknownResidency, int RenderedPrimitives)
	{
		return DroppedByCap + UnmeasuredMaterials + Unresolved + UnknownResidency + (RenderedPrimitives == 0 ? 1 : 0);
	}

	inline bool CollateralComplete(bool bTaken, int Incomplete)
	{
		return bTaken && Incomplete == 0;
	}

	enum class ELedgerBalance : int
	{
		NotYetDue,
		Balanced,
		Unbalanced
	};

	inline const char* LexLedgerBalance(ELedgerBalance B)
	{
		switch (B)
		{
		case ELedgerBalance::NotYetDue: return "not_yet_due";
		case ELedgerBalance::Balanced:  return "balanced";
		default:                        return "unbalanced";
		}
	}

	inline ELedgerBalance JudgeLedgerReading(unsigned long long TerminalFrame, unsigned long long ReadingFrame, long long Live, long long Pending)
	{
		if (ReadingFrame < PostRevertSampleFrame(TerminalFrame))
		{
			return ELedgerBalance::NotYetDue;
		}
		return Live == 0 && Pending == 0 ? ELedgerBalance::Balanced : ELedgerBalance::Unbalanced;
	}

	inline bool Fits(long long Required, long long Cap, long long Live, long long Pending)
	{
		return Required <= Cap - Live - Pending;
	}

	inline int PickFirstFailing(const int* Steps, const bool* Required, int N)
	{
		int Best = -1;
		for (int i = 0; i < N; ++i)
		{
			if (!Required[i] || Steps[i] == 0)
			{
				continue;
			}
			if (Best < 0 || Steps[i] < Steps[Best])
			{
				Best = i;
			}
		}
		return Best;
	}

	inline int PickEarliestSlot(const int* Ranks, const bool* Untouched, const bool* Qualified, int N)
	{
		int Best = -1;
		for (int i = 0; i < N; ++i)
		{
			if (Qualified[i] || Untouched[i])
			{
				continue;
			}
			if (Best < 0 || Ranks[i] < Ranks[Best])
			{
				Best = i;
			}
		}
		return Best;
	}

	inline bool IsNonSpatial(int M, int W, int H)
	{
		return M == 1 && W == 1 && H == 1;
	}

	inline int SourceMipFor(int Level, int M, int TileN, bool bMipShift)
	{
		int Mip = Level;
		if (TileN > 1)
		{
			int Shift = 0;
			int N = TileN;
			while (N > 1)
			{
				N >>= 1;
				++Shift;
			}
			Mip += Shift;
		}
		if (bMipShift)
		{
			Mip += 1;
		}
		const int Last = M > 1 ? M - 1 : 0;
		return Mip < 0 ? 0 : (Mip > Last ? Last : Mip);
	}

	inline bool StreamingBudgetPossible(bool bSupportsStreaming, int UsePerTextureBias, float MipBias)
	{
		return bSupportsStreaming && UsePerTextureBias != 0 && MipBias > 0.0f;
	}

	inline bool GlobalStreamingBias(int UsePerTextureBias, float MipBias)
	{
		return MipBias > 0.0f && UsePerTextureBias == 0;
	}
}
