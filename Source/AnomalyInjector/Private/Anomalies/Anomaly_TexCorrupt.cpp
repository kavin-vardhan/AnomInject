#include "Anomalies/Anomaly_TexCorrupt.h"
#include "Anomalies/TexCorruptPure.h"

#include "AnomalyInjectorLog.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyViewport.h"
#include "Components/MeshComponent.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/App.h"
#include "UObject/Package.h"

namespace AnomalyTexCorrupt
{
namespace
{
	TArray<const FAnomaly_TexCorrupt*>& LiveTexCorruptInstances()
	{
		static TArray<const FAnomaly_TexCorrupt*> Instances;
		return Instances;
	}

	UMeshComponent* FindComponentByName(AActor* Owner, const FName& Name)
	{
		if (!Owner || Name.IsNone())
		{
			return nullptr;
		}
		TArray<UMeshComponent*> Components;
		Owner->GetComponents<UMeshComponent>(Components);
		for (UMeshComponent* C : Components)
		{
			if (C && C->GetFName() == Name)
			{
				return C;
			}
		}
		return nullptr;
	}

	const TCHAR* ExpectedStrengthClass(EMode Mode)
	{
		return Mode == EMode::TileProbe ? TEXT("strong") : TEXT("none");
	}

	int32 SrcKindFor(EClass Class)
	{
		switch (Class)
		{
		case EClass::Colour: return 0;
		case EClass::Data:   return 1;
		default:             return 2;
		}
	}
}

	FString GetLiveModeName(UWorld* World, FName Id)
	{
		for (const FAnomaly_TexCorrupt* Instance : LiveTexCorruptInstances())
		{
			if (Instance && Instance->GetId() == Id && Instance->IsActive())
			{
				const FString Name = Instance->GetLiveModeName(World);
				if (!Name.IsEmpty())
				{
					return Name;
				}
			}
		}
		return FString();
	}

FAnomaly_TexCorrupt::FAnomaly_TexCorrupt(FName InId, EFamily InFamily)
	: Id(InId)
	, Family(InFamily)
{
	LiveTexCorruptInstances().Add(this);
}

FAnomaly_TexCorrupt::~FAnomaly_TexCorrupt()
{
	LiveTexCorruptInstances().RemoveSingleSwap(this);
}

FString FAnomaly_TexCorrupt::GetDescription() const
{
	return Family == EFamily::UV
		? TEXT("UV corruption: the host's own active texture parameters, redrawn per mip into render targets and bound on a MID of its own material (m53 S1: identity and a bench tile probe only).")
		: TEXT("Normal-map corruption: the host's own normal-map parameters, redrawn per mip into render targets and bound on a MID of its own material (m53 S1: identity only).");
}

FString FAnomaly_TexCorrupt::GetLiveModeName(const UWorld* World) const
{
	if (!bActive || (World && EventWorld.Get() != World))
	{
		return FString();
	}
	return LexMode(Mode);
}

void FAnomaly_TexCorrupt::Hold(UObject* Obj)
{
	if (!Obj)
	{
		return;
	}
	if (UAnomalyInjectorSubsystem* Injector = Holder.Get())
	{
		Injector->TexCorruptHold(Obj);
		Held.Add(Obj);
	}
}

void FAnomaly_TexCorrupt::LetGoAll()
{
	if (UAnomalyInjectorSubsystem* Injector = Holder.Get())
	{
		for (UObject* Obj : Held)
		{
			Injector->TexCorruptLetGo(Obj);
		}
	}
	Held.Reset();
}

void FAnomaly_TexCorrupt::ResetEventState()
{
	Outputs.Reset();
	Scratch.Reset();
	HostMids.Reset();
	Slots.Reset();
	TexRecords.Reset();
	Untouched.Reset();
	Owners.Reset();
	PrimaryOwner.Reset();
	PrimaryName.Reset();
	Mode = EMode::None;
	TileN = 1;
	Fault = EWrongCopy::None;
	NoApply = 0;
	RequiredBytes = 0;
	ReservedBytes = 0;
	AllocatedBytes = 0;
	SlotsCorrupted = 0;
	SlotsTotal = 0;
	TicksSinceApply = 0;
	bForeignReplacePending = false;
}

FAnomaly_TexCorrupt::FScratchSet* FAnomaly_TexCorrupt::FindScratch(const FScratchKey& Key)
{
	for (FScratchSet& S : Scratch)
	{
		if (S.Key == Key)
		{
			return &S;
		}
	}
	return nullptr;
}

int32 FAnomaly_TexCorrupt::SourceMipFor(const FOutput& O, int32 Level) const
{
	return TexCorruptPure::SourceMipFor(Level, O.M, Mode == EMode::TileProbe ? TileN : 1, Fault == EWrongCopy::MipShift);
}

void FAnomaly_TexCorrupt::ReleaseAllTargets(bool bRollback)
{
	for (FOutput& O : Outputs)
	{
		ReleaseTarget(O.Target);
	}
	for (FScratchSet& S : Scratch)
	{
		if (S.bLive)
		{
			for (UTextureRenderTarget2D* T : S.Levels)
			{
				ReleaseTarget(T);
			}
			S.bLive = false;
		}
	}
	FLedger& L = Ledger();
	L.ReleaseToPending(AllocatedBytes);
	if (bRollback)
	{
		L.Unreserve(FMath::Max<int64>(0, ReservedBytes - AllocatedBytes));
	}
	ReservedBytes = 0;
	AllocatedBytes = 0;
}

bool FAnomaly_TexCorrupt::FailAt(int32 Step, const TCHAR* Reason, const FString& Detail)
{
	ReleaseAllTargets(true);
	LetGoAll();
	FRunStats& Stats = FStatsAccess::Mutable();
	CountReason(Stats.RollbackByStep, FString::FromInt(Step));
	CountReason(Stats.RefusedByReason, Reason);
	UE_LOG(LogAnomaly, Warning,
		TEXT("%s: REFUSED %s at transaction step %d (%s) - ROLLED BACK: every render target, corruptor MID and host MID this ")
		TEXT("event created is released, its bytes are un-reserved, and NO SLOT WAS TOUCHED (the commit is step 7 alone). ")
		TEXT("live=%lld pending=%lld."),
		*Id.ToString(), Reason, Step, *Detail, Ledger().Live, Ledger().PendingSum());
	ResetEventState();
	bActive = false;
	return false;
}

bool FAnomaly_TexCorrupt::SetupLevelMid(UWorld* World, FOutput& O, int32 Level, UMaterialInterface* Corruptor,
	UTexture2D* Noise, FString& OutFail)
{
	UMaterialInstanceDynamic* Mid = UMaterialInstanceDynamic::Create(Corruptor, GetTransientPackage());
	if (!Mid)
	{
		OutFail = FString::Printf(TEXT("mid_create_failed_%s_L%d"), *O.SourceName, Level);
		return false;
	}
	Hold(Mid);
	O.LevelMids.Add(Mid);

	const int32 Wm = FMath::Max(1, O.W >> Level);
	const int32 Hm = FMath::Max(1, O.H >> Level);
	TArray<TPair<FName, float>> Scalars;
	TArray<TPair<FName, UTexture*>> Textures;
	Scalars.Emplace(Param::SrcMip, (float)SourceMipFor(O, Level));
	Scalars.Emplace(Param::DbgChanSwap, Fault == EWrongCopy::ChanSwap ? 1.0f : 0.0f);
	Scalars.Emplace(Param::DbgTexelShift, Fault == EWrongCopy::TexelShift ? 1.0f : 0.0f);
	Scalars.Emplace(Param::TexelSizeU, 1.0f / (float)Wm);
	Scalars.Emplace(Param::TexelSizeV, 1.0f / (float)Hm);
	Scalars.Emplace(Param::DbgSkipNormalEncode, Fault == EWrongCopy::Normal ? 1.0f : 0.0f);
	if (Family == EFamily::UV)
	{
		const int32 Kind = SrcKindFor(O.Class);
		Scalars.Emplace(Param::SrcKind, (float)Kind);
		Scalars.Emplace(Param::UvScale, Mode == EMode::TileProbe ? (float)TileN : 1.0f);
		Scalars.Emplace(Param::UvOffsetU, 0.0f);
		Scalars.Emplace(Param::UvOffsetV, 0.0f);
		Scalars.Emplace(Param::UvSwap, 0.0f);
		Scalars.Emplace(Param::ScrambleOn, 0.0f);
		Scalars.Emplace(Param::DbgSrgbTwice, Fault == EWrongCopy::SrgbTwice ? 1.0f : 0.0f);
		Scalars.Emplace(Param::DbgOpaque, Fault == EWrongCopy::Alpha ? 1.0f : 0.0f);
		Textures.Emplace(Kind == 0 ? Param::SrcColor : (Kind == 1 ? Param::SrcData : Param::SrcNormal), O.Source);
	}
	else
	{
		const int32 NoiseMips = Noise ? Noise->GetNumMips() : 1;
		Scalars.Emplace(Param::NoiseMip, (float)FMath::Clamp(Level, 0, FMath::Max(0, NoiseMips - 1)));
		Scalars.Emplace(Param::NormalSignX, 1.0f);
		Scalars.Emplace(Param::NormalSignY, 1.0f);
		Scalars.Emplace(Param::FlatMix, 0.0f);
		Scalars.Emplace(Param::NoiseAmp, 0.0f);
		Textures.Emplace(Param::SrcNormal, O.Source);
		Textures.Emplace(Param::NoiseNormal, Noise);
	}

	for (const TPair<FName, float>& S : Scalars)
	{
		Mid->SetScalarParameterValue(S.Key, S.Value);
	}
	for (const TPair<FName, UTexture*>& T : Textures)
	{
		Mid->SetTextureParameterValue(T.Key, T.Value);
	}
	for (const TPair<FName, float>& S : Scalars)
	{
		float Read = -12345.0f;
		if (!Mid->GetScalarParameterValue(FHashedMaterialParameterInfo(S.Key), Read, true) || Read != S.Value)
		{
			OutFail = FString::Printf(TEXT("scalar_%s_L%d_set_%g_read_%g"), *S.Key.ToString(), Level, S.Value, Read);
			return false;
		}
	}
	for (const TPair<FName, UTexture*>& T : Textures)
	{
		UTexture* Read = nullptr;
		if (!Mid->GetTextureParameterValue(FHashedMaterialParameterInfo(T.Key), Read, true) || Read != T.Value)
		{
			OutFail = FString::Printf(TEXT("texture_%s_L%d"), *T.Key.ToString(), Level);
			return false;
		}
	}
	return true;
}

bool FAnomaly_TexCorrupt::EnqueueOutput(UWorld* World, FOutput& O, bool bClear)
{
	EnqueueTripwire(O.Source, O.M, O.W, O.H, O.SourceName);
	bool bOk = true;
	FScratchSet* S = O.M > 1 ? FindScratch(O.Key) : nullptr;
	for (int32 m = 0; m < O.M; ++m)
	{
		const int32 Wm = FMath::Max(1, O.W >> m);
		const int32 Hm = FMath::Max(1, O.H >> m);
		if (m == 0)
		{
			bOk &= DrawLevel(World, O.Target, O.LevelMids[0], Wm, Hm, bClear);
			continue;
		}
		if (Fault == EWrongCopy::MipGen)
		{
			continue;
		}
		if (!S || !S->bLive || !S->Levels.IsValidIndex(m) || !S->Levels[m])
		{
			bOk = false;
			continue;
		}
		bOk &= DrawLevel(World, S->Levels[m], O.LevelMids[m], Wm, Hm, bClear);
		EnqueueMipCopy(S->Levels[m], O.Target, m, Wm, Hm);
	}
	if (Fault == EWrongCopy::MipGen && O.M > 1)
	{
		O.Target->UpdateResourceImmediate(false);
	}
	EnqueueTripwire(O.Source, O.M, O.W, O.H, O.SourceName);
	return bOk;
}

void FAnomaly_TexCorrupt::GatherCollateral(UWorld* World)
{
	Collateral.Reset();
	bCollateralTruncated = false;
	PostRevertCountdown = -1;
	CollateralApplyFrame = GFrameCounter;
	CollateralNoApply = NoApply;
	if (!World)
	{
		return;
	}
	TSet<const AActor*> Ignore;
	for (const TWeakObjectPtr<AActor>& W : Owners)
	{
		Ignore.Add(W.Get());
	}
	TMap<FString, UTexture2D*> ByPath;
	for (const TWeakObjectPtr<AActor>& Weak : AnomalyViewport::GetVisibleRenderableActors(World))
	{
		AActor* Actor = Weak.Get();
		if (!Actor || Ignore.Contains(Actor))
		{
			continue;
		}
		TArray<UMeshComponent*> Comps;
		Actor->GetComponents<UMeshComponent>(Comps);
		for (UMeshComponent* Comp : Comps)
		{
			if (!Comp || !AnomalyViewport::IsRenderableComponent(Comp))
			{
				continue;
			}
			for (int32 i = 0; i < Comp->GetNumMaterials(); ++i)
			{
				TArray<FBinding> Bindings;
				bool bRes = false;
				bool bSm = false;
				bool bComplete = false;
				ReadActiveBindings(World, Comp->GetMaterial(i), Bindings, bRes, bSm, bComplete);
				for (const FBinding& B : Bindings)
				{
					if (B.Tex2D)
					{
						ByPath.Add(B.Tex2D->GetPathName(), B.Tex2D);
					}
				}
			}
		}
	}
	TArray<FString> Paths;
	ByPath.GetKeys(Paths);
	Paths.Sort();
	constexpr int32 Cap = 256;
	bCollateralTruncated = Paths.Num() > Cap;
	for (int32 i = 0; i < Paths.Num() && i < Cap; ++i)
	{
		UTexture2D* Tex = ByPath[Paths[i]];
		FCollateral& C = Collateral.AddDefaulted_GetRef();
		C.Texture = Tex;
		C.Path = Paths[i];
		C.ApplyLevel = Tex->GetStreamableResourceState().IsValid() ? (int32)Tex->GetStreamableResourceState().NumResidentLODs : -1;
	}
	UE_LOG(LogAnomaly, Log,
		TEXT("%s: G-COLL set taken at Apply - %d collateral texture(s) of %d bound to visible actors other than the target%s. ")
		TEXT("Paired per texture across legs by (path, frame offset) from the TEXCORRUPT-COLL lines; never compared as counts."),
		*Id.ToString(), Collateral.Num(), Paths.Num(), bCollateralTruncated ? TEXT(", TRUNCATED at 256 (incomplete)") : TEXT(""));
	LogCollateral(TEXT("apply"));
}

int32 FAnomaly_TexCorrupt::CountCollateralDrops() const
{
	int32 Drops = 0;
	for (const FCollateral& C : Collateral)
	{
		const UTexture2D* Tex = C.Texture.Get();
		if (!Tex || C.ApplyLevel < 0)
		{
			continue;
		}
		const FStreamableRenderResourceState& St = Tex->GetStreamableResourceState();
		if (St.IsValid() && (int32)St.NumResidentLODs < C.ApplyLevel)
		{
			++Drops;
		}
	}
	return Drops;
}

void FAnomaly_TexCorrupt::LogCollateral(const TCHAR* Kind) const
{
	if (!Levers().bCollateralDetail)
	{
		return;
	}
	FString Levels;
	Levels.Reserve(Collateral.Num() * 48);
	for (const FCollateral& C : Collateral)
	{
		const UTexture2D* Tex = C.Texture.Get();
		const int32 Now = (Tex && Tex->GetStreamableResourceState().IsValid()) ? (int32)Tex->GetStreamableResourceState().NumResidentLODs : -1;
		Levels += FString::Printf(TEXT("%s:%d:%d;"), *C.Path, C.ApplyLevel, Now);
	}
	UE_LOG(LogAnomaly, Log,
		TEXT("TEXCORRUPT-COLL id=%s kind=%s apply_frame=%llu offset=%llu noapply=%d count=%d truncated=%d drops=%d levels=%s"),
		*Id.ToString(), Kind, CollateralApplyFrame, GFrameCounter - CollateralApplyFrame, CollateralNoApply, Collateral.Num(),
		bCollateralTruncated ? 1 : 0, CountCollateralDrops(), *Levels);
}

void FAnomaly_TexCorrupt::ReleaseTargetWatch()
{
	if (UWorld* World = EventWorld.Get())
	{
		if (UAnomalyInjectorSubsystem* Injector = World->GetSubsystem<UAnomalyInjectorSubsystem>())
		{
			Injector->ClearTargetWatchForAnomaly(Id);
		}
	}
}

bool FAnomaly_TexCorrupt::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("%s: usage <substring> [mode]"), *Id.ToString());
		return false;
	}
	if (bActive)
	{
		Revert();
	}
	ResetEventState();

	const FLevers L = Levers();
	const bool bAutoPool = UAnomalyInjectorSubsystem::IsAutoPoolSelection(World);
	const bool bModeArg = Args.Num() >= 2 && !Args[1].IsEmpty();
	EMode RequestedMode = EMode::None;
	int32 RequestedTile = 1;
	if (!bModeArg)
	{
		if ((L.TileProbe == 2 || L.TileProbe == 4) && !bAutoPool && Family == EFamily::UV)
		{
			RequestedMode = EMode::TileProbe;
			RequestedTile = L.TileProbe;
		}
		else if (L.bIdentityRedraw)
		{
			RequestedMode = EMode::IdentityRedraw;
		}
		else if (L.bIdentity)
		{
			RequestedMode = EMode::Identity;
		}
	}

	FTreeInputs In;
	In.Family = Family;
	In.Mode = RequestedMode;
	In.TileN = RequestedTile;
	In.TargetQuery = Args[0];
	In.bModeArgGiven = bModeArg;
	In.ModeArg = bModeArg ? Args[1] : FString();

	FTreeResult Tree;
	EvaluateTree(World, In, Tree);
	CountTreeDispositions(Tree);
	LogTree(Tree, Tree.bApply ? TEXT("DECIDE") : TEXT("REFUSED"), true);
	if (!Tree.bApply)
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("%s: REFUSED %s (step %s) for '%s'. The decision tree has no side effect: nothing was reserved, allocated or ")
			TEXT("touched, and the event records no fire."),
			*Id.ToString(), *Tree.FinalKey(), *Tree.EventStep, *In.TargetQuery);
		return false;
	}

	UAnomalyInjectorSubsystem* Injector = ResolveInjector(World);
	Holder = Injector;
	EventWorld = World;
	Mode = RequestedMode;
	TileN = RequestedTile;
	Fault = L.WrongCopy;
	NoApply = FMath::Clamp(L.NoApply, 0, 2);
	RequiredBytes = Tree.RequiredBytes;
	ApplyFrame = GFrameCounter;

	for (const FSlot& S : Tree.Slots)
	{
		if (S.Owner)
		{
			Owners.AddUnique(S.Owner);
		}
		if (!S.IsQualified())
		{
			FUntouched& U = Untouched.AddDefaulted_GetRef();
			U.Slot = FString::Printf(TEXT("%s[%d]"), *S.CompName.ToString(), S.SlotIndex);
			U.Reason = S.DispositionKey();
		}
	}
	SlotsTotal = Tree.Slots.Num();
	if (Owners.Num() > 0)
	{
		PrimaryOwner = Owners[0];
		PrimaryName = GetNameSafe(Owners[0].Get());
	}

	TMap<UTexture2D*, int32> OutputIndex;
	for (const FSlot& S : Tree.Slots)
	{
		if (!S.IsQualified())
		{
			continue;
		}
		for (const FBinding& B : S.Bindings)
		{
			if (B.bNonSpatialExempt)
			{
				FTexRecord& R = TexRecords.AddDefaulted_GetRef();
				R.Name = B.TextureName;
				R.Param = B.ParamName.ToString();
				R.Association = B.Info.Association == EMaterialParameterAssociation::LayerParameter ? TEXT("layer")
					: (B.Info.Association == EMaterialParameterAssociation::BlendParameter ? TEXT("blend") : TEXT("global"));
				R.LayerIndex = B.Info.Index;
				R.PixelFormat = GetPixelFormatString(B.Format);
				R.Class = LexClass(B.Class);
				R.W = B.W;
				R.H = B.H;
				R.M = B.M;
				R.bNonSpatialExempt = true;
				continue;
			}
			if (!B.bRequired || !B.IsTransformable() || !B.Tex2D)
			{
				continue;
			}
			if (!OutputIndex.Contains(B.Tex2D))
			{
				FOutput& O = Outputs.AddDefaulted_GetRef();
				O.Source = B.Tex2D;
				O.SourceName = B.Tex2D->GetPathName();
				O.Class = B.Class;
				O.M = B.M;
				O.W = B.W;
				O.H = B.H;
				O.Bytes = ChainBytes(B.W, B.H, B.M);
				O.Key.W = B.W;
				O.Key.H = B.H;
				O.Key.bSRGB = (B.Class == EClass::Colour);
				OutputIndex.Add(B.Tex2D, Outputs.Num() - 1);
			}
			FTexRecord& R = TexRecords.AddDefaulted_GetRef();
			R.Name = B.TextureName;
			R.Param = B.ParamName.ToString();
			R.Association = B.Info.Association == EMaterialParameterAssociation::LayerParameter ? TEXT("layer")
				: (B.Info.Association == EMaterialParameterAssociation::BlendParameter ? TEXT("blend") : TEXT("global"));
			R.LayerIndex = B.Info.Index;
			R.Class = LexClass(B.Class);
			R.PixelFormat = GetPixelFormatString(B.Format);
			R.W = B.W;
			R.H = B.H;
			R.M = B.M;
			R.RtBytes = ChainBytes(B.W, B.H, B.M);
		}
	}

	GatherCollateral(World);

	if (NoApply == 2)
	{
		bActive = true;
		++FStatsAccess::Mutable().FiresApplied;
		UE_LOG(LogAnomaly, Warning,
			TEXT("%s: IAI.Bench.TexCorruptNoApply 2 - the decision tree said APPLY and the reservation arithmetic reads %lld byte(s), ")
			TEXT("but NOTHING IS RESERVED, ALLOCATED, DRAWN OR COMMITTED. This is G-COLL's no-allocation null (plan I1); the event ")
			TEXT("is labelled with condition_held false."),
			*Id.ToString(), RequiredBytes);
		return true;
	}

	for (const FSlot& S : Tree.Slots)
	{
		if (!S.IsQualified())
		{
			continue;
		}
		FOwnedSlot& OS = Slots.AddDefaulted_GetRef();
		OS.Comp = S.Comp;
		OS.CompName = S.CompName;
		OS.Owner = S.Owner;
		OS.SlotIndex = S.SlotIndex;
		OS.Raw = S.Raw;
		OS.RawArrayLen = S.RawArrayLen;
		OS.Asset = S.Asset;
		OS.Resolved = S.Resolved;
		Hold(S.Raw);
		Hold(S.Asset);
		Hold(S.Resolved);
	}

	const int64 Cap = GetMaxRtBytes();
	if (!Ledger().Reserve(RequiredBytes, Cap))
	{
		return FailAt(1, Why::OverBudget, FString::Printf(TEXT("need %lld available %lld"), RequiredBytes, Ledger().Available(Cap)));
	}
	ReservedBytes = RequiredBytes;

	if (Levers().FailStep == 2)
	{
		Levers().FailStep = 0;
		return FailAt(2, Why::RtAllocFailed, TEXT("IAI.Bench.TexCorruptFailStep 2"));
	}
	for (FOutput& O : Outputs)
	{
		FString Failure;
		O.Target = AllocateTarget(O.W, O.H, O.Class == EClass::Colour, O.M, O.Source, Failure);
		if (!O.Target)
		{
			return FailAt(2, Why::RtAllocFailed, FString::Printf(TEXT("output %s: %s"), *O.SourceName, *Failure));
		}
		Hold(O.Target);
		AllocatedBytes += O.Bytes;
		if (O.M > 1 && !FindScratch(O.Key))
		{
			FScratchSet& S = Scratch.AddDefaulted_GetRef();
			S.Key = O.Key;
			S.M = O.M;
			S.Bytes = ScratchBytes(O.W, O.H, O.M);
			S.Levels.SetNumZeroed(O.M);
			S.bLive = true;
			for (int32 m = 1; m < O.M; ++m)
			{
				S.Levels[m] = AllocateTarget(FMath::Max(1, O.W >> m), FMath::Max(1, O.H >> m), O.Key.bSRGB, 1, nullptr, Failure);
				if (!S.Levels[m])
				{
					return FailAt(2, Why::RtAllocFailed, FString::Printf(TEXT("scratch %dx%d mip %d: %s"), O.W, O.H, m, *Failure));
				}
				Hold(S.Levels[m]);
			}
			AllocatedBytes += S.Bytes;
		}
	}

	UMaterialInterface* Corruptor = Injector ? (Family == EFamily::UV ? Injector->GetTexCorruptUvMaterial() : Injector->GetTexCorruptNormalMaterial()) : nullptr;
	UTexture2D* Noise = Injector ? Injector->GetTexCorruptNoiseTexture() : nullptr;
	if (Levers().FailStep == 3)
	{
		Levers().FailStep = 0;
		return FailAt(3, Why::ParamReadbackMismatch, TEXT("IAI.Bench.TexCorruptFailStep 3"));
	}
	for (FOutput& O : Outputs)
	{
		for (int32 m = 0; m < O.M; ++m)
		{
			FString Failure;
			if (!Corruptor || !SetupLevelMid(World, O, m, Corruptor, Noise, Failure))
			{
				return FailAt(3, Why::ParamReadbackMismatch, Corruptor ? Failure : FString(TEXT("corruptor_null")));
			}
		}
	}

	if (Levers().FailStep == 4)
	{
		Levers().FailStep = 0;
		return FailAt(4, Why::DrawPreconditionFailed, TEXT("IAI.Bench.TexCorruptFailStep 4"));
	}
	{
		FString Why4;
		if (!FApp::CanEverRender()) { Why4 = TEXT("cannot_ever_render"); }
		else if (!IsValid(World)) { Why4 = TEXT("world_invalid"); }
		else if (!Corruptor) { Why4 = TEXT("corruptor_null"); }
		else
		{
			FMaterialResource* Res = Corruptor->GetMaterialResource(World->FeatureLevel);
			if (!Res || !Res->GetGameThreadShaderMap() || !Res->IsGameThreadShaderMapComplete())
			{
				Why4 = TEXT("corruptor_shader_map_incomplete");
			}
		}
		for (const FOutput& O : Outputs)
		{
			if (Why4.IsEmpty() && !IsTargetDrawable(O.Target))
			{
				Why4 = FString::Printf(TEXT("output_resource_%s"), *O.SourceName);
			}
			if (Why4.IsEmpty() && !O.Source->GetResource())
			{
				Why4 = FString::Printf(TEXT("source_resource_%s"), *O.SourceName);
			}
		}
		for (const FScratchSet& S : Scratch)
		{
			for (int32 m = 1; Why4.IsEmpty() && m < S.Levels.Num(); ++m)
			{
				if (!IsTargetDrawable(S.Levels[m]))
				{
					Why4 = FString::Printf(TEXT("scratch_resource_%dx%d_mip_%d"), S.Key.W, S.Key.H, m);
				}
			}
		}
		if (!Why4.IsEmpty())
		{
			return FailAt(4, Why::DrawPreconditionFailed, Why4);
		}
	}

	bool bDrawn = true;
	for (FOutput& O : Outputs)
	{
		bDrawn &= EnqueueOutput(World, O, true);
	}
	if (!bDrawn)
	{
		return FailAt(5, Why::DrawPreconditionFailed, TEXT("a draw exited after the step-4 precondition check"));
	}
	if (Levers().FailStep == 5)
	{
		Levers().FailStep = 0;
		return FailAt(5, Why::DrawPreconditionFailed, TEXT("IAI.Bench.TexCorruptFailStep 5 (after enqueue)"));
	}
	if (Mode != EMode::IdentityRedraw)
	{
		for (FScratchSet& S : Scratch)
		{
			if (!S.bLive)
			{
				continue;
			}
			for (UTextureRenderTarget2D* T : S.Levels)
			{
				ReleaseTarget(T);
			}
			S.bLive = false;
			Ledger().ReleaseToPending(S.Bytes);
			ReservedBytes -= S.Bytes;
			AllocatedBytes -= S.Bytes;
		}
	}

	if (Levers().FailStep == 6)
	{
		Levers().FailStep = 0;
		return FailAt(6, Why::ParamReadbackMismatch, TEXT("IAI.Bench.TexCorruptFailStep 6"));
	}
	for (const FSlot& S : Tree.Slots)
	{
		if (!S.IsQualified())
		{
			continue;
		}
		FHostMid* HM = HostMids.FindByPredicate([&S](const FHostMid& H) { return H.Resolved == S.Resolved; });
		if (!HM)
		{
			HM = &HostMids.AddDefaulted_GetRef();
			HM->Resolved = S.Resolved;
			HM->Mid = UMaterialInstanceDynamic::Create(S.Resolved, GetTransientPackage());
			if (!HM->Mid)
			{
				return FailAt(6, Why::ParamReadbackMismatch, FString::Printf(TEXT("host_mid_create_%s"), *GetNameSafe(S.Resolved)));
			}
			Hold(HM->Mid);
			for (const FBinding& B : S.Bindings)
			{
				if (!B.bRequired || !B.IsTransformable() || !B.Tex2D)
				{
					continue;
				}
				const int32* Idx = OutputIndex.Find(B.Tex2D);
				UTextureRenderTarget2D* Target = Idx ? Outputs[*Idx].Target : nullptr;
				HM->Mid->SetTextureParameterValueByInfo(B.Info, Target);
				UTexture* Read = nullptr;
				if (!Target || !HM->Mid->GetTextureParameterValue(FHashedMaterialParameterInfo(B.Info), Read, true) || Read != Target)
				{
					return FailAt(6, Why::ParamReadbackMismatch,
						FString::Printf(TEXT("host_param_%s_on_%s"), *B.ParamName.ToString(), *GetNameSafe(S.Resolved)));
				}
				HM->Bound.Emplace(B.Info, Target);
			}
		}
		for (FOwnedSlot& OS : Slots)
		{
			if (OS.Comp.Get() == S.Comp && OS.SlotIndex == S.SlotIndex)
			{
				OS.HostMid = HM->Mid;
			}
		}
	}

	if (NoApply == 1)
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("%s: IAI.Bench.TexCorruptNoApply 1 - the whole transaction ran (reserve, allocate, draw, host MIDs) and ONLY ")
			TEXT("THE SLOT COMMIT (step 7) IS SKIPPED. condition_held reads false because nothing was installed."),
			*Id.ToString());
	}
	else
	{
		for (FOwnedSlot& OS : Slots)
		{
			UMeshComponent* Comp = OS.Comp.Get();
			if (Comp && OS.HostMid)
			{
				Comp->SetMaterial(OS.SlotIndex, OS.HostMid);
				OS.bCommitted = true;
				OS.OverrideLenAfter = Comp->OverrideMaterials.Num();
				++SlotsCorrupted;
			}
		}
	}

	for (const TWeakObjectPtr<AActor>& W : Owners)
	{
		if (Injector && W.Get())
		{
			Injector->WatchTargetForAnomaly(W.Get(), Id);
		}
	}

	bForeignReplacePending = L.bForeignReplace && NoApply == 0;
	bActive = true;
	++FStatsAccess::Mutable().FiresApplied;
	UE_LOG(LogAnomaly, Log,
		TEXT("%s: APPLIED mode=%s tile=%d fault=%s noapply=%d on '%s' - %d output chain(s), %d scratch class(es), %d host MID(s), ")
		TEXT("slots %d/%d committed, required=%lld reserved=%lld live=%lld pending=%lld peak=%lld cap=%s. Every level of every ")
		TEXT("output was drawn from the source's own mip and copied, enqueued before any scene draw of this frame (plan R7.4)."),
		*Id.ToString(), LexMode(Mode), TileN, LexWrongCopy(Fault), NoApply, *In.TargetQuery, Outputs.Num(), Scratch.Num(),
		HostMids.Num(), SlotsCorrupted, SlotsTotal, RequiredBytes, ReservedBytes, Ledger().Live, Ledger().PendingSum(),
		Ledger().Peak, *DescribeMaxRtBytes());
	return true;
}

void FAnomaly_TexCorrupt::Redraw()
{
	UWorld* World = EventWorld.Get();
	if (!World)
	{
		return;
	}
	const bool bClear = Fault != EWrongCopy::NoClear;
	for (FOutput& O : Outputs)
	{
		if (O.Target && O.Source && IsTargetDrawable(O.Target))
		{
			EnqueueOutput(World, O, bClear);
		}
	}
}

void FAnomaly_TexCorrupt::TickAlways(float DeltaSeconds)
{
	Ledger().Tick();

	if (PostRevertCountdown > 0)
	{
		--PostRevertCountdown;
		if (PostRevertCountdown == 0)
		{
			const int32 Drops = CountCollateralDrops();
			UE_LOG(LogAnomaly, Log, TEXT("%s: G-COLL post-revert sample (2 frames after revert): %d collateral texture(s) below their Apply-time level."),
				*Id.ToString(), Drops);
			LogCollateral(TEXT("post_revert"));
			Collateral.Reset();
			PostRevertCountdown = -1;
		}
	}

	if (!bActive)
	{
		return;
	}

	if (Owners.Num() > 0 && !PrimaryOwner.IsValid())
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("%s: TARGET LOST (weak-pointer backstop) - '%s' is no longer valid and no OnEndPlay reached us. Reverting now."),
			*Id.ToString(), *PrimaryName);
		Revert();
		return;
	}

	++TicksSinceApply;
	if (bForeignReplacePending && TicksSinceApply >= 2)
	{
		bForeignReplacePending = false;
		Levers().bForeignReplace = false;
		for (FOwnedSlot& OS : Slots)
		{
			UMeshComponent* Comp = OS.Comp.Get();
			if (Comp && OS.bCommitted)
			{
				UMaterialInterface* Foreign = UMaterial::GetDefaultMaterial(MD_Surface);
				Comp->SetMaterial(OS.SlotIndex, Foreign);
				UE_LOG(LogAnomaly, Warning,
					TEXT("%s: IAI.Bench.TexCorruptForeignReplace - set a FOREIGN material '%s' on '%s' slot %d mid-event. The revert ")
					TEXT("must leave it alone and log left-to-game (G4)."),
					*Id.ToString(), *GetNameSafe(Foreign), *Comp->GetName(), OS.SlotIndex);
				break;
			}
		}
	}

	if (Mode == EMode::IdentityRedraw)
	{
		Redraw();
	}
}

void FAnomaly_TexCorrupt::Revert()
{
	if (!bActive)
	{
		return;
	}
	ReleaseTargetWatch();

	int32 Exact = 0;
	int32 Default = 0;
	int32 LeftToGame = 0;
	int32 Unresolved = 0;
	int32 Swept = 0;
	TArray<UMaterialInstanceDynamic*> OurMids;
	for (const FHostMid& HM : HostMids)
	{
		OurMids.Add(HM.Mid);
	}

	for (FOwnedSlot& OS : Slots)
	{
		if (!OS.bCommitted)
		{
			continue;
		}
		UMeshComponent* Comp = OS.Comp.Get();
		if (!Comp)
		{
			Comp = FindComponentByName(OS.Owner.Get(), OS.CompName);
		}
		if (!Comp || OS.SlotIndex >= Comp->GetNumMaterials())
		{
			++Unresolved;
			UE_LOG(LogAnomaly, Warning, TEXT("%s: revert could not resolve '%s' slot %d (unresolved); the owner sweep follows."),
				*Id.ToString(), *OS.CompName.ToString(), OS.SlotIndex);
			continue;
		}
		const bool bOurs = Comp->OverrideMaterials.IsValidIndex(OS.SlotIndex) && Comp->OverrideMaterials[OS.SlotIndex].Get() == OS.HostMid;
		if (!bOurs)
		{
			++LeftToGame;
			UE_LOG(LogAnomaly, Log, TEXT("%s: revert left '%s' slot %d to the game (left-to-game) - it now holds '%s', not our MID."),
				*Id.ToString(), *Comp->GetName(), OS.SlotIndex,
				*GetNameSafe(Comp->OverrideMaterials.IsValidIndex(OS.SlotIndex) ? Comp->OverrideMaterials[OS.SlotIndex].Get() : nullptr));
			continue;
		}
		const int32 LenBefore = Comp->OverrideMaterials.Num();
		if (OS.Raw)
		{
			Comp->SetMaterial(OS.SlotIndex, OS.Raw);
			++Exact;
		}
		else
		{
			Comp->SetMaterial(OS.SlotIndex, nullptr);
			++Default;
		}
		UE_LOG(LogAnomaly, Log,
			TEXT("%s: revert '%s' slot %d -> %s ('%s'); override_len_before_apply=%d before_revert=%d after=%d."),
			*Id.ToString(), *Comp->GetName(), OS.SlotIndex, OS.Raw ? TEXT("restored-exact") : TEXT("restored-default"),
			*GetNameSafe(OS.Raw ? OS.Raw : OS.Asset), OS.RawArrayLen, LenBefore, Comp->OverrideMaterials.Num());
	}

	for (const TWeakObjectPtr<AActor>& W : Owners)
	{
		AActor* Owner = W.Get();
		if (!Owner)
		{
			continue;
		}
		TArray<UMeshComponent*> Comps;
		Owner->GetComponents<UMeshComponent>(Comps);
		for (UMeshComponent* Comp : Comps)
		{
			if (!Comp)
			{
				continue;
			}
			for (int32 i = 0; i < Comp->OverrideMaterials.Num(); ++i)
			{
				UMaterialInstanceDynamic* AsMid = Cast<UMaterialInstanceDynamic>(Comp->OverrideMaterials[i].Get());
				if (AsMid && OurMids.Contains(AsMid))
				{
					Comp->SetMaterial(i, nullptr);
					++Swept;
					UE_LOG(LogAnomaly, Warning, TEXT("%s: swept our MID off '%s' slot %d (cleanup, NOT evidence of an exact restore)."),
						*Id.ToString(), *Comp->GetName(), i);
				}
			}
		}
	}

	FRunStats& Stats = FStatsAccess::Mutable();
	Stats.RestoredExact += Exact;
	Stats.RestoredDefault += Default;
	Stats.LeftToGame += LeftToGame;
	Stats.Swept += Swept;

	ReleaseAllTargets(false);
	LetGoAll();

	UE_LOG(LogAnomaly, Log,
		TEXT("%s: REVERT restored-exact=%d restored-default=%d left-to-game=%d unresolved=%d swept=%d; every render target ")
		TEXT("(scratch included) released, originals let go after their slots were restored; live=%lld pending=%lld."),
		*Id.ToString(), Exact, Default, LeftToGame, Unresolved, Swept, Ledger().Live, Ledger().PendingSum());

	if (Collateral.Num() > 0)
	{
		PostRevertCountdown = 2;
	}
	ResetEventState();
	bActive = false;
}

bool FAnomaly_TexCorrupt::IsVisualConditionHeld() const
{
	if (!bActive || NoApply != 0 || SlotsCorrupted == 0)
	{
		return false;
	}
	for (const FOwnedSlot& OS : Slots)
	{
		if (!OS.bCommitted)
		{
			continue;
		}
		const UMeshComponent* Comp = OS.Comp.Get();
		if (!Comp || !Comp->OverrideMaterials.IsValidIndex(OS.SlotIndex) || Comp->OverrideMaterials[OS.SlotIndex].Get() != OS.HostMid)
		{
			return false;
		}
	}
	for (const FHostMid& HM : HostMids)
	{
		for (const TPair<FMaterialParameterInfo, UTextureRenderTarget2D*>& Pair : HM.Bound)
		{
			UTexture* Read = nullptr;
			if (!HM.Mid || !HM.Mid->GetTextureParameterValue(FHashedMaterialParameterInfo(Pair.Key), Read, true) || Read != Pair.Value)
			{
				return false;
			}
		}
	}
	return true;
}

void FAnomaly_TexCorrupt::NoteCapturedFrame(bool bAnomalousThisFrame)
{
	if (!bActive || !bAnomalousThisFrame)
	{
		return;
	}
	FStatsAccess::Mutable().CollateralDrops += CountCollateralDrops();
	LogCollateral(TEXT("labelled"));
}

bool FAnomaly_TexCorrupt::GetTelemetry(FAnomalyTelemetry& Out) const
{
	if (!bActive)
	{
		return false;
	}
	Out.AddString(TEXT("texcorrupt.mode"), LexMode(Mode));
	Out.AddString(TEXT("texcorrupt.expected_strength_class"), ExpectedStrengthClass(Mode));
	Out.AddInt(TEXT("texcorrupt.slots_corrupted"), SlotsCorrupted);
	Out.AddInt(TEXT("texcorrupt.slots_total"), SlotsTotal);
	Out.AddBool(TEXT("texcorrupt.condition_held"), IsVisualConditionHeld());
	Out.AddInt(TEXT("texcorrupt.required_bytes"), (int32)FMath::Min<int64>(RequiredBytes, MAX_int32));
	Out.AddInt(TEXT("texcorrupt.collateral_drops"), CountCollateralDrops());
	Out.AddInt(TEXT("texcorrupt.collateral_count"), Collateral.Num());
	Out.AddBool(TEXT("texcorrupt.collateral_truncated"), bCollateralTruncated);
	if (Mode == EMode::TileProbe)
	{
		Out.AddInt(TEXT("texcorrupt.tile"), TileN);
		Out.AddInt(TEXT("texcorrupt.tile_detail_mips"), FMath::FloorLog2(FMath::Max(1, TileN)));
	}
	if (NoApply != 0)
	{
		Out.AddInt(TEXT("texcorrupt.bench_noapply"), NoApply);
	}
	if (Fault != EWrongCopy::None)
	{
		Out.AddString(TEXT("texcorrupt.bench_wrong_copy"), LexWrongCopy(Fault));
	}
	for (const FUntouched& U : Untouched)
	{
		FAnomalyTelemetryFields& Rec = Out.AddArrayEntry(TEXT("texcorrupt.slots_untouched"));
		Rec.AddString(TEXT("slot"), U.Slot);
		Rec.AddString(TEXT("reason"), U.Reason);
	}
	for (const FTexRecord& R : TexRecords)
	{
		FAnomalyTelemetryFields& Rec = Out.AddArrayEntry(TEXT("texcorrupt.textures"));
		Rec.AddString(TEXT("name"), R.Name);
		Rec.AddString(TEXT("param"), R.Param);
		Rec.AddString(TEXT("association"), R.Association);
		Rec.AddInt(TEXT("layer_index"), R.LayerIndex);
		Rec.AddString(TEXT("class"), R.Class);
		Rec.AddString(TEXT("pixel_format"), R.PixelFormat);
		Rec.AddInt(TEXT("snapshot_mip"), 0);
		Rec.AddInt(TEXT("snapshot_px_w"), R.W);
		Rec.AddInt(TEXT("snapshot_px_h"), R.H);
		Rec.AddInt(TEXT("rt_bytes"), (int32)FMath::Min<int64>(R.RtBytes, MAX_int32));
		Rec.AddInt(TEXT("mip_count"), R.M);
		Rec.AddBool(TEXT("non_spatial_exempt"), R.bNonSpatialExempt);
	}
	return true;
}

void FAnomaly_TexCorrupt::OnTargetLost(AActor* Actor, bool bWorldEnding)
{
	if (!bActive)
	{
		return;
	}
	UE_LOG(LogAnomaly, Warning, TEXT("%s: TARGET LOST - '%s' ended play (%s). Reverting now."),
		*Id.ToString(), *GetNameSafe(Actor), bWorldEnding ? TEXT("world ending") : TEXT("destroyed or removed"));
	Revert();
}

void FAnomaly_TexCorrupt::OnWorldTeardown()
{
	if (bActive)
	{
		Revert();
	}
	Collateral.Reset();
	PostRevertCountdown = -1;
}
}
