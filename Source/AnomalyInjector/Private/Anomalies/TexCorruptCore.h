#pragma once

#include "CoreMinimal.h"
#include "HAL/ThreadSafeCounter.h"
#include "MaterialShared.h"
#include "Materials/MaterialInterface.h"
#include "AnomalyTexCorrupt.h"
#include "Anomalies/TexCorruptPure.h"

class AActor;
class UMeshComponent;
class UPrimitiveComponent;
class UTexture;
class UTexture2D;
class UTextureRenderTarget2D;
class UMaterialInstanceDynamic;
class UAnomalyInjectorSubsystem;
class UWorld;

namespace AnomalyTexCorrupt
{
	enum class EFamily : uint8
	{
		UV,
		Normal
	};

	enum class EClass : uint8
	{
		None,
		Colour,
		Data,
		Normal
	};

	enum class EMode : uint8
	{
		None,
		Identity,
		IdentityRedraw,
		TileProbe,
		Tile,
		Scramble,
		Invert,
		GreenFlip
	};

	enum class EWrongCopy : uint8
	{
		None,
		ChanSwap,
		SrgbTwice,
		MipShift,
		MipGen,
		TexelShift,
		Normal,
		Alpha,
		NoClear
	};

	namespace Why
	{
		extern const TCHAR* AssetsUnavailable;
		extern const TCHAR* CorruptorNotReady;
		extern const TCHAR* Dxt5NormalHost;
		extern const TCHAR* RuntimeLodBias;
		extern const TCHAR* ModeInvalid;
		extern const TCHAR* NoMesh;
		extern const TCHAR* NaniteUnmaskable;
		extern const TCHAR* SlotEmpty;
		extern const TCHAR* SlotTranslucent;
		extern const TCHAR* HostMid;
		extern const TCHAR* NaniteOverride;
		extern const TCHAR* ShaderMapUnavailable;
		extern const TCHAR* ShaderMapIncomplete;
		extern const TCHAR* DrawShadersMissing;
		extern const TCHAR* DefaultMaterialPath;
		extern const TCHAR* NoTextures;
		extern const TCHAR* VirtualTexture;
		extern const TCHAR* UnsupportedType;
		extern const TCHAR* ExcludedGroup;
		extern const TCHAR* TextureNotParameter;
		extern const TCHAR* UnsupportedEncoding;
		extern const TCHAR* MipChainShape;
		extern const TCHAR* HeldByStuckLowMip;
		extern const TCHAR* ResourceNotReady;
		extern const TCHAR* StreamingPending;
		extern const TCHAR* NotFullyResident;
		extern const TCHAR* Transformable;
		extern const TCHAR* NonSpatialExempt;
		extern const TCHAR* NoNormalMap;
		extern const TCHAR* NormalUnconnected;
		extern const TCHAR* BelowSizePolicy;
		extern const TCHAR* MapSetOverCap;
		extern const TCHAR* TextureUniform;
		extern const TCHAR* NoEligibleSlot;
		extern const TCHAR* PartialFootprint;
		extern const TCHAR* OverBudget;
		extern const TCHAR* RtAllocFailed;
		extern const TCHAR* DrawPreconditionFailed;
		extern const TCHAR* ParamReadbackMismatch;
		extern const TCHAR* Qualified;
		extern const TCHAR* NotRequired;
	}

	namespace Param
	{
		extern const FName SrcColor;
		extern const FName SrcData;
		extern const FName SrcNormal;
		extern const FName NoiseNormal;
		extern const FName SrcKind;
		extern const FName SrcMip;
		extern const FName NoiseMip;
		extern const FName UvScale;
		extern const FName UvOffsetU;
		extern const FName UvOffsetV;
		extern const FName UvSwap;
		extern const FName ScrambleOn;
		extern const FName ScrambleK;
		extern const FName ScrambleAInv;
		extern const FName ScrambleB;
		extern const FName NormalSignX;
		extern const FName NormalSignY;
		extern const FName FlatMix;
		extern const FName NoiseAmp;
		extern const FName DbgChanSwap;
		extern const FName DbgSrgbTwice;
		extern const FName DbgTexelShift;
		extern const FName TexelSizeU;
		extern const FName TexelSizeV;
		extern const FName DbgSkipNormalEncode;
		extern const FName DbgOpaque;
	}

	const TCHAR* LexFamily(EFamily Family);
	const TCHAR* LexClass(EClass Class);
	const TCHAR* LexMode(EMode Mode);
	const TCHAR* LexWrongCopy(EWrongCopy Fault);

	bool ModeFamily(EMode Mode, EFamily& OutFamily);
	TexCorruptPure::EModeFamilyP ToPureFamily(EFamily Family);
	EMode FromPureMode(TexCorruptPure::EModeP Mode);

	const TArray<FName>& UvCorruptorScalars();
	const TArray<FName>& UvCorruptorTextures();
	const TArray<FName>& NormalCorruptorScalars();
	const TArray<FName>& NormalCorruptorTextures();

	struct FKnobs
	{
		static constexpr int32 MaxRtBytesCompiled = 134217728;
		static constexpr int32 MaxRtBytesMin = 1048576;
		static constexpr int32 MaxRtBytesMax = 1073741824;
		static constexpr int32 MinTexturePxCompiled = 64;
		static constexpr int32 MinTexturePxMin = 1;
		static constexpr int32 MinTexturePxMax = 16384;
		static constexpr int32 MaxTexturesCompiled = 8;
		static constexpr int32 MaxTexturesMin = 1;
		static constexpr int32 MaxTexturesMax = 64;
		static constexpr int32 TileNCompiled = 8;
		static constexpr int32 TileNMin = 2;
		static constexpr int32 TileNMax = 16;
		static constexpr int32 ScrambleKCompiled = 8;
		static constexpr int32 ScrambleKMin = 2;
		static constexpr int32 ScrambleKMax = 64;
	};

	int32 GetMaxRtBytes();
	FString DescribeMaxRtBytes();
	int32 GetMinTexturePx();
	FString DescribeMinTexturePx();
	int32 GetMaxTextures();
	FString DescribeMaxTextures();
	int32 GetTileN();
	FString DescribeTileN();
	int32 GetScrambleK();
	FString DescribeScrambleK();
	uint32 GetEnabledModeMask(EFamily Family);
	FString DescribeModeSet(EFamily Family, uint32 Mask);
	FString DescribeEnabledModes(EFamily Family);
	void EchoRunStartKnobs();

	int32 TakeAttemptOrdinal(EFamily Family);
	TexCorruptPure::FScramble DeriveScramble(uint32 Seed, int32 Ordinal, int32 K);

	struct FLevers
	{
		int32 NoApply = 0;
		EWrongCopy WrongCopy = EWrongCopy::None;
		bool bIdentity = false;
		bool bIdentityRedraw = false;
		int32 TileProbe = 0;
		bool bForceMissingAsset = false;
		int32 FailStep = 0;
		int32 FailAllocOrdinal = -1;
		bool bForeignReplace = false;
		bool bCollateralDetail = false;
		int32 CommitDelay = 0;
		int32 RestoreDelay = 0;
		bool bAllReasonsFirstOnly = false;
		int32 RtSamplerBias = -1;
		int32 SrcMipCompensation = -1;
		bool bNoPartScope = false;
	};

	FLevers& Levers();
	FString DescribeLevers();

	using FLedger = TexCorruptPure::FLedgerCore;

	FLedger& Ledger();

	struct FStatsAccess
	{
		static FRunStats& Mutable();
	};

	uint32 RunStatsDigest();

	void CountReason(TMap<FString, int32>& Map, const FString& Key);

	FThreadSafeCounter& TripwireCounter();

	int64 ChainBytes(int32 W, int32 H, int32 M);

	int64 ScratchBytes(int32 W, int32 H, int32 M);

	struct FBinding
	{
		EMaterialTextureParameterType Type = EMaterialTextureParameterType::Standard2D;
		int32 Index = 0;
		FMaterialParameterInfo Info;
		FName ParamName = NAME_None;
		UTexture* Texture = nullptr;
		UTexture2D* Tex2D = nullptr;
		FString TextureName;
		EClass Class = EClass::None;
		EPixelFormat Format = PF_Unknown;
		bool bSRGB = false;
		bool bNormalMap = false;
		int32 LODGroup = -1;
		int32 M = 0;
		int32 W = 0;
		int32 H = 0;
		TArray<FIntPoint> LevelSizes;
		int32 ResidentLODs = -1;
		int32 MaxLODs = -1;
		int32 NonOptionalLODs = -1;
		int32 AssetLODBias = -1;
		int32 CinematicMips = 0;
		int32 CachedLODBias = 0;
		int32 CookedM = 0;
		int32 CookedW = 0;
		int32 CookedH = 0;
		int32 FirstMip = 0;
		int32 CopyDrop = 0;
		bool bResidentMappable = true;
		bool bStreams = false;
		bool bDefaultTexture = false;
		int32 Step = 0;
		FString Reason;
		FString Sub;
		bool bRequired = false;
		bool bNonSpatialExempt = false;
		TArray<FString> AllKeys;
		TArray<FString> Notes;
		bool bUnassessed = false;

		bool IsTransformable() const { return Step == 0; }
		FString DispositionKey() const;
	};

	struct FSlot
	{
		UMeshComponent* Comp = nullptr;
		AActor* Owner = nullptr;
		FName CompName = NAME_None;
		int32 SlotIndex = 0;
		UMaterialInterface* Raw = nullptr;
		int32 RawArrayLen = 0;
		UMaterialInterface* Asset = nullptr;
		UMaterialInterface* Resolved = nullptr;
		UMaterialInterface* Effective = nullptr;
		bool bSkinned = false;
		bool bStatic = false;
		int32 Step = 0;
		bool bUntouched = false;
		FString Reason;
		FString Sub;
		FString Where;
		FString Kind;
		bool bPartial = false;
		TArray<FBinding> Bindings;
		TArray<FString> AllKeys;
		TArray<FString> Notes;
		bool bUnassessed = false;

		bool bComponentAdmitted = true;

		bool IsQualified() const { return Step == 0 && !bUntouched; }
		bool IsSelected() const { return IsQualified() && bComponentAdmitted; }
		FString DispositionKey() const;
	};

	struct FAttemptInfo
	{
		int32 Ordinal = -1;
		bool bScramble = false;
		TexCorruptPure::FScramble Scramble;

		FString Describe() const;
	};

	struct FTreeResult
	{
		EFamily Family = EFamily::UV;
		EMode Mode = EMode::None;
		int32 TileN = 1;
		FAttemptInfo Attempt;
		FString TargetQuery;
		bool bApply = false;
		FString Reason;
		FString Sub;
		FString EventStep;
		TArray<FSlot> Slots;
		int32 SlotsQualified = 0;
		int64 RequiredBytes = 0;
		int32 DistinctTextures = 0;
		int32 ScratchClasses = 0;
		TArray<FString> EventKeys;
		TArray<FString> EventNotes;
		int32 ComponentsAdmittable = 0;
		int32 ComponentsTouchable = 0;
		int32 ComponentsSkipped = 0;
		int32 SlotsSelected = 0;

		FString FinalKey() const;
		FString CensusKey() const;
	};

	struct FTreeInputs
	{
		EFamily Family = EFamily::UV;
		EMode Mode = EMode::None;
		int32 TileN = 1;
		bool bCensus = false;
		bool bAllReasons = false;
		bool bModeArgGiven = false;
		FString ModeArg;
		FString ModeRefusalSub;
		FAttemptInfo Attempt;
		FString TargetQuery;
		TWeakObjectPtr<AActor> TargetActor;
	};

	UAnomalyInjectorSubsystem* ResolveInjector(UWorld* World);

	struct FHostDrawReadiness
	{
		TexCorruptPure::EDrawReadiness Verdict = TexCorruptPure::EDrawReadiness::NoVertexFactoryShaders;
		bool bWholeMapComplete = false;
		bool bCompileFinished = false;
		FString VertexFactory = TEXT("none");
		int32 VertexFactoryShaders = 0;
		int32 BasePassVertexShaders = 0;
		int32 BasePassPixelShaders = 0;

		bool IsReady() const { return Verdict == TexCorruptPure::EDrawReadiness::Ready; }
	};

	FHostDrawReadiness ReadHostDrawReadiness(UMaterialInterface* Material, const UPrimitiveComponent* Comp, UWorld* World);

	bool CorruptorShadersReady(UMaterialInterface* Corruptor, UWorld* World);

	bool CheckCorruptorContract(UMaterialInterface* Corruptor, const TArray<FName>& Scalars, const TArray<FName>& Textures,
		FString& OutMissing);

	void ReadActiveBindings(UWorld* World, UMaterialInterface* Resolved, const UPrimitiveComponent* Comp, TArray<FBinding>& OutBindings,
		bool& bOutResourceOk, bool& bOutShaderMapOk, bool& bOutComplete, FHostDrawReadiness* OutReadiness = nullptr);

	namespace Uniform
	{
		TexCorruptPure::Uniform::EVerdict Query(UTexture2D* Tex, int32 FirstMip, int32 W, int32 H, TexCorruptPure::Uniform::ERule Rule,
			FString& OutDetail);
		bool Kick(UTexture2D* Tex);
		int32 KickForActor(AActor* Actor);
		void Pump();
		int32 NumPending();
		int32 NumMeasured();
		int32 NumUniformNoSpatial();
		int32 NumFailed();
		void ResetCache();
		FString DescribeCounters();
	}

	void EvaluateTree(UWorld* World, const FTreeInputs& In, FTreeResult& Out);

	void CountTreeDispositions(const FTreeResult& Result);

	void LogTree(const FTreeResult& Result, const TCHAR* Tag, bool bVerboseBindings);

	struct FScratchKey
	{
		int32 W = 0;
		int32 H = 0;
		bool bSRGB = false;

		bool operator==(const FScratchKey& O) const { return W == O.W && H == O.H && bSRGB == O.bSRGB; }
	};

	UTextureRenderTarget2D* AllocateTarget(int32 W, int32 H, bool bSRGB, int32 ExpectedMips, UTexture2D* SamplerSource,
		FString& OutFailure, bool& bOutResourceCreated);

	bool IsTargetDrawable(UTextureRenderTarget2D* Target);

	bool DrawLevel(UWorld* World, UTextureRenderTarget2D* Target, UMaterialInterface* Material, int32 W, int32 H, bool bClear);

	void EnqueueMipCopy(UTextureRenderTarget2D* Scratch, UTextureRenderTarget2D* Out, int32 Mip, int32 W, int32 H);

	struct FTripwireHold;

	TSharedRef<FTripwireHold, ESPMode::ThreadSafe> MakeTripwireHold();

	void EnqueueTripwire(UTexture2D* Source, int32 M, int32 W, int32 H, int32 FirstMip, const FString& Name,
		const TSharedRef<FTripwireHold, ESPMode::ThreadSafe>& Hold, bool bPost);

	FThreadSafeCounter& SourceChangedCounter();

	float EffectiveRtSamplerBias();

	float EffectiveSrcMipCompensation();

	void ReleaseTarget(UTextureRenderTarget2D* Target);

	void RunCensus(UWorld* World, const FString& Query);

	FString CensusKey(const FString& Reason, const FString& Sub);
}
