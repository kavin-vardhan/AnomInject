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

	enum class EDrawReadiness : int
	{
		Ready,
		CompilePending,
		NoVertexFactoryShaders,
		NoBasePassVertexShader,
		NoBasePassPixelShader
	};

	inline const char* LexDrawReadiness(EDrawReadiness R)
	{
		switch (R)
		{
		case EDrawReadiness::Ready:                  return "ready";
		case EDrawReadiness::CompilePending:         return "compile_pending";
		case EDrawReadiness::NoVertexFactoryShaders: return "no_vertex_factory_shaders";
		case EDrawReadiness::NoBasePassVertexShader: return "no_base_pass_vs";
		case EDrawReadiness::NoBasePassPixelShader:  return "no_base_pass_ps";
		}
		return "unknown";
	}

	inline bool StartsWithAscii(const char* S, const char* Prefix)
	{
		if (!S || !Prefix)
		{
			return false;
		}
		while (*Prefix)
		{
			if (*S != *Prefix)
			{
				return false;
			}
			++S;
			++Prefix;
		}
		return true;
	}

	inline int ClassifyBasePassShaderTypeName(const char* Name)
	{
		if (StartsWithAscii(Name, "TBasePassVS") || StartsWithAscii(Name, "TMobileBasePassVS"))
		{
			return 1;
		}
		if (StartsWithAscii(Name, "TBasePassPS") || StartsWithAscii(Name, "TMobileBasePassPS"))
		{
			return 2;
		}
		return 0;
	}

	inline EDrawReadiness JudgeDrawReadiness(bool bCompileFinished, bool bHasVertexFactoryShaders, int BasePassVertexShaders,
		int BasePassPixelShaders)
	{
		if (!bCompileFinished)
		{
			return EDrawReadiness::CompilePending;
		}
		if (!bHasVertexFactoryShaders)
		{
			return EDrawReadiness::NoVertexFactoryShaders;
		}
		if (BasePassVertexShaders <= 0)
		{
			return EDrawReadiness::NoBasePassVertexShader;
		}
		if (BasePassPixelShaders <= 0)
		{
			return EDrawReadiness::NoBasePassPixelShader;
		}
		return EDrawReadiness::Ready;
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

	inline int PickEarliestNonQualified(const int* Ranks, const bool* Qualified, int N)
	{
		int Best = -1;
		for (int i = 0; i < N; ++i)
		{
			if (Qualified[i])
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

	enum class EFootprint : int
	{
		NoneQualified,
		Partial,
		Full
	};

	inline EFootprint JudgeFootprint(int Qualified, int Slots)
	{
		if (Qualified <= 0 || Slots <= 0)
		{
			return EFootprint::NoneQualified;
		}
		return Qualified < Slots ? EFootprint::Partial : EFootprint::Full;
	}

	enum class EFootprintStep : int
	{
		V1,
		V1P,
		V2,
		Apply
	};

	inline const char* LexFootprintStep(EFootprintStep S)
	{
		switch (S)
		{
		case EFootprintStep::V1:  return "V1";
		case EFootprintStep::V1P: return "V1P";
		case EFootprintStep::V2:  return "V2";
		default:                  return "apply";
		}
	}

	inline EFootprintStep DecideFootprintStep(int Qualified, int Slots, bool bFits)
	{
		const EFootprint F = JudgeFootprint(Qualified, Slots);
		if (F == EFootprint::NoneQualified)
		{
			return EFootprintStep::V1;
		}
		if (F == EFootprint::Partial)
		{
			return EFootprintStep::V1P;
		}
		return bFits ? EFootprintStep::Apply : EFootprintStep::V2;
	}

	enum class EModeFamilyP : int
	{
		UV,
		Normal
	};

	enum class EModeP : int
	{
		None,
		Tile,
		Scramble,
		Invert,
		GreenFlip
	};

	struct FModeName
	{
		const char* Name;
		EModeFamilyP Family;
		EModeP Mode;
		bool bDelivered;
	};

	constexpr int NumModeNames = 8;

	inline const FModeName& ModeNameAt(int i)
	{
		static const FModeName Table[NumModeNames] = {
			{ "tile", EModeFamilyP::UV, EModeP::Tile, true },
			{ "scramble", EModeFamilyP::UV, EModeP::Scramble, true },
			{ "drift", EModeFamilyP::UV, EModeP::None, false },
			{ "swap", EModeFamilyP::UV, EModeP::None, false },
			{ "invert", EModeFamilyP::Normal, EModeP::Invert, true },
			{ "green_flip", EModeFamilyP::Normal, EModeP::GreenFlip, true },
			{ "flat", EModeFamilyP::Normal, EModeP::None, false },
			{ "noise", EModeFamilyP::Normal, EModeP::None, false } };
		return Table[i];
	}

	inline const char* LexModeP(EModeP Mode)
	{
		switch (Mode)
		{
		case EModeP::Tile:      return "tile";
		case EModeP::Scramble:  return "scramble";
		case EModeP::Invert:    return "invert";
		case EModeP::GreenFlip: return "green_flip";
		default:                return "none";
		}
	}

	inline bool ModeFamilyOf(EModeP Mode, EModeFamilyP& OutFamily)
	{
		switch (Mode)
		{
		case EModeP::Tile:
		case EModeP::Scramble:
			OutFamily = EModeFamilyP::UV;
			return true;
		case EModeP::Invert:
		case EModeP::GreenFlip:
			OutFamily = EModeFamilyP::Normal;
			return true;
		default:
			return false;
		}
	}

	inline unsigned ModeBit(EModeP Mode)
	{
		return Mode == EModeP::None ? 0u : (1u << ((int)Mode - 1));
	}

	inline int DeliveredModesInOrder(EModeFamilyP Family, unsigned Mask, EModeP* Out, int MaxOut)
	{
		int Count = 0;
		for (int i = 0; i < NumModeNames; ++i)
		{
			const FModeName& M = ModeNameAt(i);
			if (M.Family != Family || !M.bDelivered || (Mask & ModeBit(M.Mode)) == 0)
			{
				continue;
			}
			if (Count < MaxOut)
			{
				Out[Count] = M.Mode;
			}
			++Count;
		}
		return Count;
	}

	inline unsigned DeliveredMask(EModeFamilyP Family)
	{
		unsigned Mask = 0;
		for (int i = 0; i < NumModeNames; ++i)
		{
			const FModeName& M = ModeNameAt(i);
			if (M.Family == Family && M.bDelivered)
			{
				Mask |= ModeBit(M.Mode);
			}
		}
		return Mask;
	}

	inline int LowerAscii(int C)
	{
		return (C >= 'A' && C <= 'Z') ? C - 'A' + 'a' : C;
	}

	template <typename TChar>
	inline int AsciiLength(const TChar* A)
	{
		int N = 0;
		while (A && A[N] != 0)
		{
			++N;
		}
		return N;
	}

	template <typename TChar>
	inline bool EqualsAsciiNoCaseN(const TChar* A, int Len, const char* B)
	{
		if (!A || !B)
		{
			return false;
		}
		int i = 0;
		for (; i < Len; ++i)
		{
			if (B[i] == 0 || LowerAscii((int)A[i]) != LowerAscii((int)(unsigned char)B[i]))
			{
				return false;
			}
		}
		return B[i] == 0;
	}

	template <typename TChar>
	inline const FModeName* FindModeNameN(const TChar* A, int Len)
	{
		for (int i = 0; i < NumModeNames; ++i)
		{
			if (EqualsAsciiNoCaseN(A, Len, ModeNameAt(i).Name))
			{
				return &ModeNameAt(i);
			}
		}
		return nullptr;
	}

	enum class EModeArg : int
	{
		Ok,
		NoMode,
		NoModeEnabled,
		Unknown,
		WrongFamily,
		NotInDelivery
	};

	inline const char* LexModeArg(EModeArg R)
	{
		switch (R)
		{
		case EModeArg::Ok:            return "ok";
		case EModeArg::NoMode:        return "no_mode";
		case EModeArg::NoModeEnabled: return "no_mode_enabled";
		case EModeArg::Unknown:       return "unknown";
		case EModeArg::WrongFamily:   return "family";
		default:                      return "not_in_delivery";
		}
	}

	constexpr const char* NoModeEnabledToken = "!none";

	template <typename TChar>
	inline EModeArg ClassifyModeArg(const TChar* Arg, EModeFamilyP Family, bool bAutoPool, EModeP& OutMode, const FModeName*& OutName)
	{
		OutMode = EModeP::None;
		OutName = nullptr;
		const int Len = AsciiLength(Arg);
		if (Len == 0)
		{
			return EModeArg::NoMode;
		}
		if (EqualsAsciiNoCaseN(Arg, Len, NoModeEnabledToken))
		{
			return bAutoPool ? EModeArg::NoModeEnabled : EModeArg::Unknown;
		}
		const FModeName* Name = FindModeNameN(Arg, Len);
		if (!Name)
		{
			return EModeArg::Unknown;
		}
		OutName = Name;
		if (Name->Family != Family)
		{
			return EModeArg::WrongFamily;
		}
		if (!Name->bDelivered)
		{
			return EModeArg::NotInDelivery;
		}
		OutMode = Name->Mode;
		return EModeArg::Ok;
	}

	struct FModeSetParse
	{
		bool bOk = false;
		unsigned Mask = 0;
		EModeArg Why = EModeArg::Ok;
		int BadStart = 0;
		int BadLen = 0;
		const FModeName* BadName = nullptr;
	};

	inline bool IsAsciiSpace(int C)
	{
		return C == ' ' || C == '\t';
	}

	template <typename TChar>
	inline FModeSetParse ParseModeSet(const TChar* Text, EModeFamilyP Family)
	{
		FModeSetParse R;
		const int Len = AsciiLength(Text);
		int Start = 0;
		int End = Len;
		while (Start < End && IsAsciiSpace((int)Text[Start]))
		{
			++Start;
		}
		while (End > Start && IsAsciiSpace((int)Text[End - 1]))
		{
			--End;
		}
		if (End == Start)
		{
			R.Why = EModeArg::NoMode;
			return R;
		}
		if (EqualsAsciiNoCaseN(Text + Start, End - Start, "none"))
		{
			R.bOk = true;
			return R;
		}
		int TokStart = Start;
		for (int i = Start; i <= End; ++i)
		{
			if (i < End && Text[i] != '+')
			{
				continue;
			}
			int A = TokStart;
			int B = i;
			while (A < B && IsAsciiSpace((int)Text[A]))
			{
				++A;
			}
			while (B > A && IsAsciiSpace((int)Text[B - 1]))
			{
				--B;
			}
			const FModeName* Name = FindModeNameN(Text + A, B - A);
			EModeArg Why = EModeArg::Ok;
			if (!Name)
			{
				Why = EModeArg::Unknown;
			}
			else if (Name->Family != Family)
			{
				Why = EModeArg::WrongFamily;
			}
			else if (!Name->bDelivered)
			{
				Why = EModeArg::NotInDelivery;
			}
			if (Why != EModeArg::Ok)
			{
				R.Why = Why;
				R.BadStart = A;
				R.BadLen = B - A;
				R.BadName = Name;
				R.Mask = 0;
				return R;
			}
			R.Mask |= ModeBit(Name->Mode);
			TokStart = i + 1;
		}
		R.bOk = true;
		return R;
	}

	constexpr unsigned ScrambleTag = 0x53434D52u;

	inline void ScrambleBytes(unsigned Seed, unsigned Ordinal, unsigned char Out[12])
	{
		const unsigned Words[3] = { Seed, Ordinal, ScrambleTag };
		for (int w = 0; w < 3; ++w)
		{
			for (int k = 0; k < 4; ++k)
			{
				Out[w * 4 + k] = (unsigned char)((Words[w] >> (8 * k)) & 0xFFu);
			}
		}
	}

	inline int GcdInt(int A, int B)
	{
		A = A < 0 ? -A : A;
		B = B < 0 ? -B : B;
		while (B != 0)
		{
			const int T = A % B;
			A = B;
			B = T;
		}
		return A;
	}

	inline int InverseModN(int A, int N)
	{
		if (N <= 1)
		{
			return -1;
		}
		long long T = 0;
		long long NewT = 1;
		long long R = N;
		long long NewR = ((A % N) + N) % N;
		while (NewR != 0)
		{
			const long long Q = R / NewR;
			const long long TT = T - Q * NewT;
			T = NewT;
			NewT = TT;
			const long long RR = R - Q * NewR;
			R = NewR;
			NewR = RR;
		}
		if (R != 1)
		{
			return -1;
		}
		if (T < 0)
		{
			T += N;
		}
		return (int)T;
	}

	struct FScramble
	{
		bool bValid = false;
		unsigned H = 0;
		int K = 0;
		int N = 0;
		int A = 1;
		int B = 0;
		int AInv = 1;
	};

	inline FScramble ScrambleFromHash(unsigned H, int K)
	{
		FScramble S;
		S.H = H;
		S.K = K;
		if (K < 2)
		{
			return S;
		}
		const int N = K * K;
		S.N = N;
		int Count = 0;
		for (int a = 1; a < N; ++a)
		{
			if (GcdInt(a, N) == 1)
			{
				++Count;
			}
		}
		if (Count == 0)
		{
			return S;
		}
		const int Pick = (int)((H >> 16) % (unsigned)Count);
		int Seen = 0;
		int A = 1;
		for (int a = 1; a < N; ++a)
		{
			if (GcdInt(a, N) != 1)
			{
				continue;
			}
			if (Seen == Pick)
			{
				A = a;
				break;
			}
			++Seen;
		}
		int B = (int)(H % (unsigned)N);
		if (A == 1 && B == 0)
		{
			B = 1;
		}
		S.A = A;
		S.B = B;
		S.AInv = InverseModN(A, N);
		S.bValid = S.AInv > 0;
		return S;
	}

	template <typename FCrc32>
	inline FScramble ScramblePair(unsigned Seed, unsigned Ordinal, int K, FCrc32 Crc)
	{
		unsigned char Bytes[12];
		ScrambleBytes(Seed, Ordinal, Bytes);
		return ScrambleFromHash((unsigned)Crc(Bytes, 12), K);
	}

	inline int ScrambleForward(int A, int B, int N, int I)
	{
		return (int)((((long long)A * I + B) % N + N) % N);
	}

	inline int ScrambleInverse(int AInv, int B, int N, int J)
	{
		const long long D = (((long long)J - B) % N + N) % N;
		return (int)(((long long)AInv * D) % N);
	}

	struct FAttemptOrdinals
	{
		int Next[2] = { 0, 0 };

		int Take(EModeFamilyP Family)
		{
			return Next[(int)Family]++;
		}

		void Reset()
		{
			Next[0] = 0;
			Next[1] = 0;
		}
	};

	inline bool IsRevertSettlingAt(unsigned long long PostRevertFrame, bool bRollback, bool bRestorePending, unsigned long long Now)
	{
		if (bRestorePending)
		{
			return true;
		}
		if (PostRevertFrame == 0 || bRollback)
		{
			return false;
		}
		return Now < PostRevertFrame;
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

	enum class EDrawOutcome : int
	{
		NoEligible,
		NoCandidates,
		Drawn
	};

	struct FDrawAttemptResult
	{
		EDrawOutcome Outcome = EDrawOutcome::NoEligible;
		int IdIndex = -1;
		int NumCandidates = 0;
		int TargetIndex = -1;
		float Hold = 0.0f;
		int NumModes = -1;
		bool bModeDrawn = false;
		float ModeFraction = 0.0f;
		int ModeIndex = -1;
	};

	inline int ModeIndexFromFraction(float U, int N)
	{
		return N > 0 ? (int)(U * (float)N) : -1;
	}

	template <typename TStream, typename TCandidateCount, typename TModeCount>
	FDrawAttemptResult DrawAttempt(TStream& Stream, int NumEligible, TCandidateCount&& CandidateCount,
		TModeCount&& ModeCount, float HoldMin, float HoldMax)
	{
		FDrawAttemptResult R;
		if (NumEligible <= 0)
		{
			R.Outcome = EDrawOutcome::NoEligible;
			return R;
		}
		R.IdIndex = Stream.RandHelper(NumEligible);
		R.NumCandidates = CandidateCount(R.IdIndex);
		if (R.NumCandidates <= 0)
		{
			R.Outcome = EDrawOutcome::NoCandidates;
			return R;
		}
		R.TargetIndex = Stream.RandHelper(R.NumCandidates);
		R.Hold = (float)Stream.FRandRange(HoldMin, HoldMax);
		R.NumModes = ModeCount(R.IdIndex);
		if (R.NumModes >= 0)
		{
			R.bModeDrawn = true;
			R.ModeFraction = Stream.GetFraction();
			R.ModeIndex = ModeIndexFromFraction(R.ModeFraction, R.NumModes);
		}
		R.Outcome = EDrawOutcome::Drawn;
		return R;
	}

	inline int RoundRobinNext(unsigned& Counter, int NumModes)
	{
		if (NumModes <= 0)
		{
			return -1;
		}
		const int Index = (int)(Counter % (unsigned)NumModes);
		++Counter;
		return Index;
	}

	enum class ENoModeLever : unsigned char
	{
		None = 0,
		TileProbe = 1,
		IdentityRedraw = 2,
		Identity = 3
	};

	inline ENoModeLever NoModeLever(int TileProbe, bool bIdentityRedraw, bool bIdentity, bool bAutoPool, bool bUvFamily)
	{
		if ((TileProbe == 2 || TileProbe == 4) && !bAutoPool && bUvFamily)
		{
			return ENoModeLever::TileProbe;
		}
		if (bIdentityRedraw)
		{
			return ENoModeLever::IdentityRedraw;
		}
		if (bIdentity)
		{
			return ENoModeLever::Identity;
		}
		return ENoModeLever::None;
	}

	inline const char* LexNoModeLever(ENoModeLever L)
	{
		switch (L)
		{
		case ENoModeLever::TileProbe:      return "tile_probe";
		case ENoModeLever::IdentityRedraw: return "identity_redraw";
		case ENoModeLever::Identity:       return "identity";
		default:                           return "none";
		}
	}

	struct FTargetedAttemptResult
	{
		float Hold = 0.0f;
		bool bRoundRobin = false;
		bool bBenchLever = false;
		int ModeIndex = -1;
		int NumModes = -1;
	};

	template <typename TStream>
	FTargetedAttemptResult TargetedAttempt(TStream& Stream, float HoldMin, float HoldMax, bool bModeGiven, int NumModes,
		unsigned& RoundRobinCounter, bool bBenchLeverMode = false)
	{
		FTargetedAttemptResult R;
		R.Hold = (float)Stream.FRandRange(HoldMin, HoldMax);
		R.NumModes = NumModes;
		if (!bModeGiven && NumModes >= 0)
		{
			if (bBenchLeverMode)
			{
				R.bBenchLever = true;
			}
			else
			{
				R.bRoundRobin = true;
				R.ModeIndex = RoundRobinNext(RoundRobinCounter, NumModes);
			}
		}
		return R;
	}
}
