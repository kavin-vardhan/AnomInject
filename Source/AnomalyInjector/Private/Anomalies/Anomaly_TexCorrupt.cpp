#include "Anomalies/Anomaly_TexCorrupt.h"
#include "Engine/Texture.h"
#include "MaterialTypes.h"
#include "Materials/MaterialInterface.h"
#if __has_include("Materials/MaterialParameters.h")
#include "Materials/MaterialParameters.h"
#endif
#if __has_include("Templates/SharedPointerFwd.h")
#include "Templates/SharedPointerFwd.h"
#endif
#include "Templates/SharedPointerInternals.h"
#include "Anomalies/TexCorruptPure.h"
#include "AnomalyInstallState.h"

#include "AnomalyAutoInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyViewport.h"
#include "Components/MeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "EngineUtils.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MaterialShared.h"
#if __has_include("MaterialDomain.h")
#include "MaterialDomain.h"
#endif
#include "HAL/PlatformTime.h"
#include "Misc/App.h"
#include "UObject/Package.h"

namespace AnomalyTexCorrupt
{
namespace
{
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
		switch (Mode)
		{
		case EMode::TileProbe:
		case EMode::Tile:
		case EMode::Scramble:
		case EMode::Invert:
			return TEXT("strong");
		case EMode::GreenFlip:
			return TEXT("medium");
		default:
			return TEXT("none");
		}
	}

	TArray<FAnomaly_TexCorrupt*>& LiveInstances()
	{
		static TArray<FAnomaly_TexCorrupt*> Instances;
		return Instances;
	}

	uint32 ConfiguredSeed(UWorld* World)
	{
		const UAnomalyAutoInjectorSubsystem* Auto = World ? World->GetSubsystem<UAnomalyAutoInjectorSubsystem>() : nullptr;
		return Auto ? (uint32)Auto->GetSeed() : 0u;
	}

	double ElapsedMs(double StartSeconds)
	{
		return (FPlatformTime::Seconds() - StartSeconds) * 1000.0;
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

FAnomaly_TexCorrupt::FAnomaly_TexCorrupt(FName InId, EFamily InFamily)
	: Id(InId)
	, Family(InFamily)
{
	LiveInstances().Add(this);
}

FAnomaly_TexCorrupt::~FAnomaly_TexCorrupt()
{
	LiveInstances().RemoveSingleSwap(this);
}

FString FAnomaly_TexCorrupt::GetDescription() const
{
	return Family == EFamily::UV
		? TEXT("UV corruption: the host's own active texture parameters, redrawn per mip into render targets through a UV transform and bound on a MID of its own material. Modes: tile (N x N repeat) and scramble (a K x K cell permutation).")
		: TEXT("Normal-map corruption: the host's own normal-map parameters, redrawn per mip into render targets with a sign change and bound on a MID of its own material. Modes: invert (x and y negated) and green_flip (y negated).");
}

bool FAnomaly_TexCorrupt::IsRevertSettlingIn(const UWorld* World) const
{
	if (!World || EventWorld.Get() != World)
	{
		return false;
	}
	return TexCorruptPure::IsRevertSettlingAt(PostRevertFrame, bTerminalRollback, bRestorePending, GFrameCounter);
}

bool IsRevertSettling(UWorld* World)
{
	for (const FAnomaly_TexCorrupt* Instance : LiveInstances())
	{
		if (Instance && Instance->IsRevertSettlingIn(World))
		{
			return true;
		}
	}
	return false;
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
	ClearPartScope();
	PartsCorrupted.Reset();
	ComponentsCorrupted = 0;
	ComponentsSkipped = 0;
	Owners.Reset();
	PrimaryOwner.Reset();
	PrimaryName.Reset();
	Mode = EMode::None;
	TileN = 1;
	Attempt = FAttemptInfo();
	bCommitPending = false;
	CommitDueFrame = 0;
	bRestorePending = false;
	RestoreDueFrame = 0;
	RestoreDelayedBy = 0;
	Fault = EWrongCopy::None;
	NoApply = 0;
	RequiredBytes = 0;
	Account = TexCorruptPure::FEventAccount();
	SlotsCorrupted = 0;
	SlotsTotal = 0;
	TicksSinceApply = 0;
	bForeignReplacePending = false;
}

void FAnomaly_TexCorrupt::ClearPartScope()
{
	for (const TWeakObjectPtr<AActor>& W : ScopedOwners)
	{
		if (const AActor* A = W.Get())
		{
			AnomalyViewport::ClearEventComponentScope(A);
		}
	}
	if (ScopedOwners.Num() > 0)
	{
		AnomalyViewport::PruneEventComponentScopes();
	}
	ScopedOwners.Reset();
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
	return TexCorruptPure::SourceMipFor(Level, O.M, (Mode == EMode::TileProbe || Mode == EMode::Tile) ? TileN : 1,
		Fault == EWrongCopy::MipShift);
}

void FAnomaly_TexCorrupt::ReleaseScratchSet(FScratchSet& S)
{
	if (!S.bLive)
	{
		return;
	}
	for (int32 m = 0; m < S.Levels.Num(); ++m)
	{
		if (S.Levels[m])
		{
			ReleaseTarget(S.Levels[m]);
			Account.ReleaseCreated(Ledger(), TexCorruptPure::LevelBytes(S.Key.W, S.Key.H, m), GFrameCounter);
			S.Levels[m] = nullptr;
		}
	}
	S.bLive = false;
}

void FAnomaly_TexCorrupt::ReleaseAllTargets()
{
	for (FOutput& O : Outputs)
	{
		if (O.Target)
		{
			ReleaseTarget(O.Target);
			Account.ReleaseCreated(Ledger(), O.Bytes, GFrameCounter);
			O.Target = nullptr;
		}
	}
	for (FScratchSet& S : Scratch)
	{
		ReleaseScratchSet(S);
	}
	Account.Close(Ledger(), GFrameCounter);
}

bool FAnomaly_TexCorrupt::FailAt(int32 Step, const TCHAR* Reason, const FString& Detail)
{
	const int64 CreatedAtFail = Account.Created;
	const int64 NeverCreated = Account.Reserved - Account.Created;
	ReleaseAllTargets();
	LetGoAll();
	FRunStats& Stats = FStatsAccess::Mutable();
	CountReason(Stats.RollbackByStep, FString::FromInt(Step));
	CountReason(Stats.RefusedByReason, Reason);
	UE_LOG(LogAnomaly, Warning,
		TEXT("%s: REFUSED %s at transaction step %d (%s) - ROLLED BACK: every render target, corruptor MID and host MID this ")
		TEXT("event created is released, NO SLOT WAS TOUCHED (the commit is step 7 alone). Ledger: %lld created byte(s) moved ")
		TEXT("to the two-frame pending ledger, %lld never-created byte(s) un-reserved; live=%lld pending=%lld frame=%llu. ")
		TEXT("pending > 0 here is expected (created bytes wait two rendered frames) and this line is not a balance verdict; ")
		TEXT("the balance reading is TEXCORRUPT-LEDGER kind=rollback at frame %llu, which must read live=0 pending=0.%s"),
		*Id.ToString(), Reason, Step, *Detail, CreatedAtFail, NeverCreated, Ledger().Live, Ledger().PendingSum(), GFrameCounter,
		TexCorruptPure::PostRevertSampleFrame(GFrameCounter), *Attempt.Describe());
	ResetEventState();
	RevertFrame = GFrameCounter;
	PostRevertFrame = TexCorruptPure::PostRevertSampleFrame(RevertFrame);
	bTerminalRollback = true;
	bActive = false;
	return false;
}

bool FAnomaly_TexCorrupt::AllocateAll(FString& OutDetail)
{
	TArray<TexCorruptPure::FReqTex> Req;
	for (int32 i = 0; i < Outputs.Num(); ++i)
	{
		TexCorruptPure::FReqTex& R = Req.AddDefaulted_GetRef();
		R.Id = i;
		R.W = Outputs[i].W;
		R.H = Outputs[i].H;
		R.M = Outputs[i].M;
		R.bSRGB = Outputs[i].Key.bSRGB;
	}
	TArray<TexCorruptPure::FAllocStep> Steps;
	Steps.SetNum(TexCorruptPure::MaxAllocSteps(Req.Num()));
	const int32 NumSteps = TexCorruptPure::PlanAllocations(Req.GetData(), Req.Num(), Steps.GetData(), Steps.Num());
	if (NumSteps < 0)
	{
		OutDetail = TEXT("allocation_plan_overflow");
		return false;
	}
#if !UE_BUILD_SHIPPING
	const int32 FailOrdinal = Levers().FailStep == 2 ? Levers().FailAllocOrdinal : -1;
	if (FailOrdinal >= 0)
	{
		Levers().FailStep = 0;
		Levers().FailAllocOrdinal = -1;
	}
#endif
	for (int32 k = 0; k < NumSteps; ++k)
	{
		const TexCorruptPure::FAllocStep& St = Steps[k];
		FOutput& O = Outputs[St.Tex];
		const bool bOutput = St.Kind == TexCorruptPure::AllocKind::Output;
		FScratchSet* S = nullptr;
		if (!bOutput)
		{
			S = FindScratch(O.Key);
			if (!S)
			{
				S = &Scratch.AddDefaulted_GetRef();
				S->Key = O.Key;
				S->M = O.M;
				S->Bytes = ScratchBytes(O.W, O.H, O.M);
				S->Levels.SetNumZeroed(O.M);
				S->bLive = true;
			}
		}
		FString Failure;
		bool bCreated = false;
		UTextureRenderTarget2D* T = bOutput
			? AllocateTarget(O.W, O.H, O.Class == EClass::Colour, O.M, O.Source, Failure, bCreated)
			: AllocateTarget(St.W, St.H, O.Key.bSRGB, 1, nullptr, Failure, bCreated);
#if !UE_BUILD_SHIPPING
		if (T && k == FailOrdinal)
		{
			ReleaseTarget(T);
			T = nullptr;
			Failure = FString::Printf(TEXT("IAI.Bench.TexCorruptFailStep 2 %d (created then rejected)"), k);
		}
#endif
		if (!T)
		{
			if (bCreated)
			{
				Account.NoteCreated(Ledger(), St.Bytes);
				Account.ReleaseCreated(Ledger(), St.Bytes, GFrameCounter);
			}
			OutDetail = bOutput
				? FString::Printf(TEXT("output %s (plan step %d of %d, resource_created=%d): %s"), *O.SourceName, k, NumSteps,
					bCreated ? 1 : 0, *Failure)
				: FString::Printf(TEXT("scratch %dx%d mip %d (plan step %d of %d, resource_created=%d): %s"), O.W, O.H, St.Level, k,
					NumSteps, bCreated ? 1 : 0, *Failure);
			return false;
		}
		Hold(T);
		if (!Account.NoteCreated(Ledger(), St.Bytes))
		{
			UE_LOG(LogAnomaly, Error, TEXT("%s: LEDGER - plan step %d created %lld byte(s) beyond the reservation; the excess was ")
				TEXT("force-reserved so live stays true."), *Id.ToString(), k, St.Bytes);
		}
		if (bOutput)
		{
			O.Target = T;
		}
		else
		{
			S->Levels[St.Level] = T;
		}
	}
	return true;
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
	Scalars.Emplace(Param::SrcMip, (float)(SourceMipFor(O, Level) + O.Drop) - EffectiveSrcMipCompensation());
	Scalars.Emplace(Param::DbgChanSwap, Fault == EWrongCopy::ChanSwap ? 1.0f : 0.0f);
	Scalars.Emplace(Param::DbgTexelShift, Fault == EWrongCopy::TexelShift ? 1.0f : 0.0f);
	Scalars.Emplace(Param::TexelSizeU, 1.0f / (float)Wm);
	Scalars.Emplace(Param::TexelSizeV, 1.0f / (float)Hm);
	Scalars.Emplace(Param::DbgSkipNormalEncode, Fault == EWrongCopy::Normal ? 1.0f : 0.0f);
	if (Family == EFamily::UV)
	{
		const int32 Kind = SrcKindFor(O.Class);
		Scalars.Emplace(Param::SrcKind, (float)Kind);
		Scalars.Emplace(Param::UvScale, (Mode == EMode::TileProbe || Mode == EMode::Tile) ? (float)TileN : 1.0f);
		Scalars.Emplace(Param::UvOffsetU, 0.0f);
		Scalars.Emplace(Param::UvOffsetV, 0.0f);
		Scalars.Emplace(Param::UvSwap, 0.0f);
		if (Mode == EMode::Scramble)
		{
			Scalars.Emplace(Param::ScrambleOn, 1.0f);
			Scalars.Emplace(Param::ScrambleK, (float)Attempt.Scramble.K);
			Scalars.Emplace(Param::ScrambleAInv, (float)Attempt.Scramble.AInv);
			Scalars.Emplace(Param::ScrambleB, (float)Attempt.Scramble.B);
		}
		else
		{
			Scalars.Emplace(Param::ScrambleOn, 0.0f);
		}
		Scalars.Emplace(Param::DbgSrgbTwice, Fault == EWrongCopy::SrgbTwice ? 1.0f : 0.0f);
		Scalars.Emplace(Param::DbgOpaque, Fault == EWrongCopy::Alpha ? 1.0f : 0.0f);
		Textures.Emplace(Kind == 0 ? Param::SrcColor : (Kind == 1 ? Param::SrcData : Param::SrcNormal), O.Source);
	}
	else
	{
		const int32 NoiseMips = Noise ? Noise->GetNumMips() : 1;
		Scalars.Emplace(Param::NoiseMip, (float)FMath::Clamp(Level, 0, FMath::Max(0, NoiseMips - 1)));
		Scalars.Emplace(Param::NormalSignX, Mode == EMode::Invert ? -1.0f : 1.0f);
		Scalars.Emplace(Param::NormalSignY, (Mode == EMode::Invert || Mode == EMode::GreenFlip) ? -1.0f : 1.0f);
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
	const TSharedRef<FTripwireHold, ESPMode::ThreadSafe> TripHold = MakeTripwireHold();
	EnqueueTripwire(O.Source, O.RM, O.RW, O.RH, O.FirstMip, O.SourceName, TripHold, false);
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
	EnqueueTripwire(O.Source, O.RM, O.RW, O.RH, O.FirstMip, O.SourceName, TripHold, true);
	return bOk;
}

void FAnomaly_TexCorrupt::GatherCollateral(UWorld* World)
{
	if (PostRevertFrame != 0)
	{
		TakePostRevertSample();
	}
	Collateral.Reset();
	bCollateralTruncated = false;
	CollateralDroppedByCap = 0;
	CollateralUnmeasuredMaterials = 0;
	CollateralUnresolved = 0;
	CollateralNullSlots = 0;
	CollateralUnknownAtApply = 0;
	CollateralRenderedPrimitives = 0;
	bCollateralTaken = World != nullptr;
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
	const double Now = World->GetTimeSeconds();
	const double Delta = World->GetDeltaSeconds();
	const double Window = TexCorruptPure::CollateralWindowSeconds(Delta);
	UMaterialInterface* EngineDefault = UMaterial::GetDefaultMaterial(MD_Surface);
	TSet<UMaterialInterface*> SeenMaterials;
	TMap<FString, UTexture2D*> ByPath;
	int32 CollateralPrimitives = 0;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor)
		{
			continue;
		}
		const bool bTarget = Ignore.Contains(Actor);
		TInlineComponentArray<UPrimitiveComponent*> Prims(Actor);
		for (UPrimitiveComponent* Prim : Prims)
		{
			if (!Prim)
			{
				continue;
			}
			const double Since = Now - (double)Prim->GetLastRenderTimeOnScreen();
			if (Prim->IsRegistered() && Since <= Window)
			{
				++CollateralRenderedPrimitives;
			}
			if (!TexCorruptPure::IsCollateralPrimitive(Prim->IsRegistered(), bTarget, Since, Delta))
			{
				continue;
			}
			++CollateralPrimitives;
			TArray<UMaterialInterface*> Mats;
			Prim->GetUsedMaterials(Mats, false);
			for (UMaterialInterface* Assigned : Mats)
			{
				UMaterialInterface* Mat = TexCorruptPure::MeasuredSlotMaterial<UMaterialInterface>(Assigned, EngineDefault,
					CollateralNullSlots, CollateralUnresolved);
				if (!Mat || SeenMaterials.Contains(Mat))
				{
					continue;
				}
				SeenMaterials.Add(Mat);
				TArray<FBinding> Bindings;
				bool bRes = false;
				bool bSm = false;
				bool bComplete = false;
				ReadActiveBindings(World, Mat, Prim, Bindings, bRes, bSm, bComplete);
				if (!bRes || !bSm || !bComplete)
				{
					++CollateralUnmeasuredMaterials;
				}
				for (const FBinding& B : Bindings)
				{
					if (TexCorruptPure::ClassifyCollateralEntry(B.Texture != nullptr, B.Tex2D != nullptr, CollateralUnresolved)
						== TexCorruptPure::ECollEntry::Measured)
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
	CollateralDroppedByCap = FMath::Max(0, Paths.Num() - Cap);
	bCollateralTruncated = CollateralDroppedByCap > 0;
	for (int32 i = 0; i < Paths.Num() && i < Cap; ++i)
	{
		UTexture2D* Tex = ByPath[Paths[i]];
		FCollateral& C = Collateral.AddDefaulted_GetRef();
		C.Texture = Tex;
		C.Path = Paths[i];
		C.ApplyLevel = Tex->GetStreamableResourceState().IsValid() ? (int32)Tex->GetStreamableResourceState().NumResidentLODs : -1;
		if (C.ApplyLevel < 0)
		{
			++CollateralUnknownAtApply;
		}
	}
	const int32 Incomplete = CollateralIncomplete();
	UE_LOG(LogAnomaly, Log,
		TEXT("%s: G-COLL set taken at Apply - %d collateral texture(s) of %d, from %d primitive(s) other than the target's drawn ")
		TEXT("on screen within %.3f s (%d rendered on screen in total, target included). No injection-selection filter is ")
		TEXT("applied. %d null material slot(s) measured through the engine default material '%s', which the mesh proxy draws ")
		TEXT("there. incomplete=%d (dropped_by_cap=%d unmeasured_materials=%d unresolved=%d unknown_residency=%d ")
		TEXT("no_render_evidence=%d) - %s. ")
		TEXT("Paired per texture across legs by (path, frame offset) from the TEXCORRUPT-COLL lines; never compared as counts."),
		*Id.ToString(), Collateral.Num(), Paths.Num(), CollateralPrimitives, Window, CollateralRenderedPrimitives, CollateralNullSlots,
		*GetPathNameSafe(EngineDefault), Incomplete, CollateralDroppedByCap, CollateralUnmeasuredMaterials, CollateralUnresolved,
		CollateralUnknownAtApply, CollateralRenderedPrimitives == 0 ? 1 : 0,
		Incomplete > 0 ? TEXT("INCOMPLETE, NOT A CLEAN READING") : TEXT("complete"));
	LogCollateral(TEXT("apply"));
}

int32 FAnomaly_TexCorrupt::CountCollateralDrops(int32* OutUnknownNow) const
{
	int32 Drops = 0;
	int32 Unknown = 0;
	for (const FCollateral& C : Collateral)
	{
		const UTexture2D* Tex = C.Texture.Get();
		if (!Tex || C.ApplyLevel < 0 || !Tex->GetStreamableResourceState().IsValid())
		{
			++Unknown;
			continue;
		}
		if ((int32)Tex->GetStreamableResourceState().NumResidentLODs < C.ApplyLevel)
		{
			++Drops;
		}
	}
	if (OutUnknownNow)
	{
		*OutUnknownNow = Unknown;
	}
	return Drops;
}

int32 FAnomaly_TexCorrupt::CollateralIncomplete() const
{
	if (!bCollateralTaken)
	{
		return 0;
	}
	int32 UnknownNow = 0;
	CountCollateralDrops(&UnknownNow);
	return TexCorruptPure::CollateralIncompleteCount(CollateralDroppedByCap, CollateralUnmeasuredMaterials, CollateralUnresolved,
		UnknownNow, CollateralRenderedPrimitives);
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
	int32 UnknownNow = 0;
	const int32 Drops = CountCollateralDrops(&UnknownNow);
	UE_LOG(LogAnomaly, Log,
		TEXT("TEXCORRUPT-COLL id=%s kind=%s apply_frame=%llu offset=%llu noapply=%d count=%d truncated=%d dropped_by_cap=%d ")
		TEXT("unmeasured_materials=%d unresolved=%d null_slots=%d unknown_now=%d incomplete=%d drops=%d levels=%s"),
		*Id.ToString(), Kind, CollateralApplyFrame, GFrameCounter - CollateralApplyFrame, CollateralNoApply, Collateral.Num(),
		bCollateralTruncated ? 1 : 0, CollateralDroppedByCap, CollateralUnmeasuredMaterials, CollateralUnresolved, CollateralNullSlots,
		UnknownNow, CollateralIncomplete(), Drops, *Levels);
}

void FAnomaly_TexCorrupt::TakePostRevertSample()
{
	const uint64 Offset = GFrameCounter - RevertFrame;
	const TCHAR* Endpoint = GFrameCounter == PostRevertFrame ? TEXT("on_frame") : (GFrameCounter < PostRevertFrame ? TEXT("early") : TEXT("late"));
	const TCHAR* Terminal = bTerminalRollback ? TEXT("rollback") : TEXT("revert");
	if (bCollateralTaken)
	{
		int32 UnknownNow = 0;
		const int32 Drops = CountCollateralDrops(&UnknownNow);
		UE_LOG(LogAnomaly, Log,
			TEXT("%s: G-COLL post-%s sample at frame %llu, %llu rendered frame(s) after the %s at frame %llu (endpoint %s): ")
			TEXT("%d collateral texture(s) below their Apply-time level; incomplete=%d."),
			*Id.ToString(), Terminal, GFrameCounter, Offset, Terminal, RevertFrame, Endpoint, Drops, CollateralIncomplete());
		LogCollateral(bTerminalRollback ? TEXT("post_rollback") : TEXT("post_revert"));
	}
	const TexCorruptPure::ELedgerBalance Balance = TexCorruptPure::JudgeLedgerReading(RevertFrame, GFrameCounter, Ledger().Live,
		Ledger().PendingSum());
	UE_LOG(LogAnomaly, Log,
		TEXT("TEXCORRUPT-LEDGER id=%s kind=%s %s_frame=%llu frame=%llu offset=%llu endpoint=%s live=%lld pending=%lld ")
		TEXT("peak=%lld balance=%s"),
		*Id.ToString(), bTerminalRollback ? TEXT("rollback") : TEXT("post_revert"), Terminal, RevertFrame, GFrameCounter, Offset,
		Endpoint, Ledger().Live, Ledger().PendingSum(), Ledger().Peak, ANSI_TO_TCHAR(TexCorruptPure::LexLedgerBalance(Balance)));
	Collateral.Reset();
	bCollateralTaken = false;
	PostRevertFrame = 0;
	bTerminalRollback = false;
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
	const int32 Ordinal = TakeAttemptOrdinal(Family);
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("%s: usage <substring> <mode> (attempt ordinal %d used up)"), *Id.ToString(), Ordinal);
		return false;
	}
	if (bActive)
	{
		Revert();
	}
	if (bRestorePending)
	{
		FinishPendingRestore(TEXT("re-apply"));
	}
	ResetEventState();

	const FLevers L = Levers();
	const bool bAutoPool = UAnomalyInjectorSubsystem::IsAutoPoolSelection(World);
	const bool bModeArg = Args.Num() >= 2 && !Args[1].IsEmpty();
	EMode RequestedMode = EMode::None;
	int32 RequestedTile = 1;
	FString ModeRefusalSub;
	if (bModeArg)
	{
		TexCorruptPure::EModeP Pure = TexCorruptPure::EModeP::None;
		const TexCorruptPure::FModeName* Name = nullptr;
		const TexCorruptPure::EModeArg Parsed = TexCorruptPure::ClassifyModeArg(*Args[1], ToPureFamily(Family), bAutoPool, Pure, Name);
		switch (Parsed)
		{
		case TexCorruptPure::EModeArg::Ok:
			RequestedMode = FromPureMode(Pure);
			if (RequestedMode == EMode::Tile)
			{
				RequestedTile = GetTileN();
			}
			break;
		case TexCorruptPure::EModeArg::NoModeEnabled:
			ModeRefusalSub = TEXT("no_mode_enabled");
			break;
		case TexCorruptPure::EModeArg::WrongFamily:
			ModeRefusalSub = FString::Printf(TEXT("family:%s"), *Args[1]);
			break;
		case TexCorruptPure::EModeArg::NotInDelivery:
			ModeRefusalSub = FString::Printf(TEXT("not_in_delivery:%s"), Name ? ANSI_TO_TCHAR(Name->Name) : *Args[1]);
			break;
		default:
			ModeRefusalSub = FString::Printf(TEXT("unknown:%s"), *Args[1]);
			break;
		}
	}
	else
	{
		switch (TexCorruptPure::NoModeLever(L.TileProbe, L.bIdentityRedraw, L.bIdentity, bAutoPool, Family == EFamily::UV))
		{
		case TexCorruptPure::ENoModeLever::TileProbe:
			RequestedMode = EMode::TileProbe;
			RequestedTile = L.TileProbe;
			break;
		case TexCorruptPure::ENoModeLever::IdentityRedraw:
			RequestedMode = EMode::IdentityRedraw;
			break;
		case TexCorruptPure::ENoModeLever::Identity:
			RequestedMode = EMode::Identity;
			break;
		default:
			break;
		}
		if (RequestedMode == EMode::None)
		{
			ModeRefusalSub = TEXT("no_mode");
		}
	}

	FAttemptInfo AttemptInfo;
	AttemptInfo.Ordinal = Ordinal;
	if (RequestedMode == EMode::Scramble)
	{
		AttemptInfo.bScramble = true;
		AttemptInfo.Scramble = DeriveScramble(ConfiguredSeed(World), Ordinal, GetScrambleK());
	}

	FTreeInputs In;
	In.Family = Family;
	In.Mode = RequestedMode;
	In.TileN = RequestedTile;
	In.TargetQuery = Args[0];
	In.bModeArgGiven = bModeArg;
	In.ModeArg = bModeArg ? Args[1] : FString();
	In.ModeRefusalSub = ModeRefusalSub;
	In.Attempt = AttemptInfo;

	FTreeResult Tree;
	EvaluateTree(World, In, Tree);
	CountTreeDispositions(Tree);
	LogTree(Tree, Tree.bApply ? TEXT("DECIDE") : TEXT("REFUSED"), true);
	if (!Tree.bApply)
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("%s: REFUSED %s (step %s) for '%s'%s. The decision tree has no side effect: nothing was reserved, allocated or ")
			TEXT("touched, and the event records no fire."),
			*Id.ToString(), *Tree.FinalKey(), *Tree.EventStep, *In.TargetQuery, *AttemptInfo.Describe());
		return false;
	}
	const double ApplyStartSeconds = FPlatformTime::Seconds();

	UAnomalyInjectorSubsystem* Injector = ResolveInjector(World);
	Holder = Injector;
	EventWorld = World;
	Mode = RequestedMode;
	TileN = RequestedTile;
	Attempt = AttemptInfo;
	Fault = L.WrongCopy;
	NoApply = FMath::Clamp(L.NoApply, 0, 2);
	RequiredBytes = Tree.RequiredBytes;
	ApplyFrame = GFrameCounter;

	for (const FSlot& S : Tree.Slots)
	{
		if (S.Owner && S.IsSelected())
		{
			Owners.AddUnique(S.Owner);
		}
		if (!S.IsSelected())
		{
			FUntouched& U = Untouched.AddDefaulted_GetRef();
			U.Slot = FString::Printf(TEXT("%s[%d]"), *S.CompName.ToString(), S.SlotIndex);
			U.Reason = S.IsQualified() ? FString(TEXT("component_skipped")) : S.DispositionKey();
		}
	}
	ComponentsSkipped = Tree.ComponentsSkipped;
	SlotsTotal = Tree.Slots.Num();
	if (Owners.Num() > 0)
	{
		PrimaryOwner = Owners[0];
		PrimaryName = GetNameSafe(Owners[0].Get());
	}

	TMap<UTexture2D*, int32> OutputIndex;
	for (const FSlot& S : Tree.Slots)
	{
		if (!S.IsSelected())
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
				O.RW = B.W;
				O.RH = B.H;
				O.RM = B.M;
				O.Drop = FMath::Clamp(B.CopyDrop, 0, FMath::Max(0, B.M - 1));
				O.M = B.M - O.Drop;
				O.W = FMath::Max(1, B.W >> O.Drop);
				O.H = FMath::Max(1, B.H >> O.Drop);
				O.FirstMip = B.FirstMip;
				O.CookedM = B.CookedM;
				O.Bytes = ChainBytes(O.W, O.H, O.M);
				O.Key.W = O.W;
				O.Key.H = O.H;
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
			const int32 Drop = FMath::Clamp(B.CopyDrop, 0, FMath::Max(0, B.M - 1));
			R.W = FMath::Max(1, B.W >> Drop);
			R.H = FMath::Max(1, B.H >> Drop);
			R.M = B.M - Drop;
			R.FirstMip = B.FirstMip + Drop;
			R.CookedM = B.CookedM;
			R.RtBytes = ChainBytes(R.W, R.H, R.M);
		}
	}

	GatherCollateral(World);

#if !UE_BUILD_SHIPPING
	if (NoApply == 2)
	{
		RegisterTargetWatch(Injector);
		bActive = true;
		++FStatsAccess::Mutable().FiresApplied;
		int32 FirstBad = -1;
		const TexCorruptPure::EHeld Reading = EvaluateCondition(FirstBad);
		UE_LOG(LogAnomaly, Warning,
			TEXT("%s: IAI.Bench.TexCorruptNoApply 2 - the decision tree said APPLY and the reservation arithmetic reads %lld byte(s), ")
			TEXT("but NOTHING IS RESERVED, ALLOCATED, DRAWN OR COMMITTED. This is G-COLL's no-allocation null (plan I1). The target ")
			TEXT("watches (EndPlay, destroy, world end) ARE registered, as for an applied event. condition read through the live ")
			TEXT("predicate at apply: %s.%s apply_ms=%.3f"),
			*Id.ToString(), RequiredBytes, ANSI_TO_TCHAR(TexCorruptPure::LexHeld(Reading)), *Attempt.Describe(),
			ElapsedMs(ApplyStartSeconds));
		return true;
	}
#endif

	for (const FSlot& S : Tree.Slots)
	{
		if (!S.IsSelected())
		{
			continue;
		}
		PartsCorrupted.Add(FString::Printf(TEXT("%s[%d]"), *S.CompName.ToString(), S.SlotIndex));
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
	if (!Account.Reserve(Ledger(), RequiredBytes, Cap))
	{
		return FailAt(1, Why::OverBudget, FString::Printf(TEXT("need %lld available %lld"), RequiredBytes, Ledger().Available(Cap)));
	}

#if !UE_BUILD_SHIPPING
	if (Levers().FailStep == 2 && Levers().FailAllocOrdinal < 0)
	{
		Levers().FailStep = 0;
		return FailAt(2, Why::RtAllocFailed, TEXT("IAI.Bench.TexCorruptFailStep 2 (before any allocation)"));
	}
#endif
	{
		FString AllocDetail;
		if (!AllocateAll(AllocDetail))
		{
			return FailAt(2, Why::RtAllocFailed, AllocDetail);
		}
	}

	UMaterialInterface* Corruptor = Injector ? (Family == EFamily::UV ? Injector->GetTexCorruptUvMaterial() : Injector->GetTexCorruptNormalMaterial()) : nullptr;
	UTexture2D* Noise = Injector ? Injector->GetTexCorruptNoiseTexture() : nullptr;
#if !UE_BUILD_SHIPPING
	if (Levers().FailStep == 3)
	{
		Levers().FailStep = 0;
		return FailAt(3, Why::ParamReadbackMismatch, TEXT("IAI.Bench.TexCorruptFailStep 3"));
	}
#endif
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

#if !UE_BUILD_SHIPPING
	if (Levers().FailStep == 4)
	{
		Levers().FailStep = 0;
		return FailAt(4, Why::DrawPreconditionFailed, TEXT("IAI.Bench.TexCorruptFailStep 4"));
	}
#endif
	{
		FString Why4;
		if (!FApp::CanEverRender()) { Why4 = TEXT("cannot_ever_render"); }
		else if (!IsValid(World)) { Why4 = TEXT("world_invalid"); }
		else if (!Corruptor) { Why4 = TEXT("corruptor_null"); }
		else
		{
			FMaterialResource* Res = Corruptor->GetMaterialResource(World->FeatureLevel);
			if (!Res || !CorruptorShadersReady(Corruptor, World))
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
#if !UE_BUILD_SHIPPING
	if (Levers().FailStep == 5)
	{
		Levers().FailStep = 0;
		return FailAt(5, Why::DrawPreconditionFailed, TEXT("IAI.Bench.TexCorruptFailStep 5 (after enqueue)"));
	}
#endif
	if (Mode != EMode::IdentityRedraw)
	{
		for (FScratchSet& S : Scratch)
		{
			ReleaseScratchSet(S);
		}
	}

#if !UE_BUILD_SHIPPING
	if (Levers().FailStep == 6)
	{
		Levers().FailStep = 0;
		return FailAt(6, Why::ParamReadbackMismatch, TEXT("IAI.Bench.TexCorruptFailStep 6"));
	}
#endif
	for (const FSlot& S : Tree.Slots)
	{
		if (!S.IsSelected())
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

	bool bCommitNow = true;
#if !UE_BUILD_SHIPPING
	if (NoApply == 1)
	{
		bCommitNow = false;
		UE_LOG(LogAnomaly, Warning,
			TEXT("%s: IAI.Bench.TexCorruptNoApply 1 - the whole transaction ran (reserve, allocate, draw, host MIDs) and ONLY ")
			TEXT("THE SLOT COMMIT (step 7) IS SKIPPED. condition_held is read by the same live predicate as an applied event and ")
			TEXT("reads slot_not_installed because each expected slot still holds its original."),
			*Id.ToString());
	}
	else if (L.CommitDelay > 0)
	{
		bCommitNow = false;
		bCommitPending = true;
		CommitDueFrame = GFrameCounter + (uint64)L.CommitDelay;
		UE_LOG(LogAnomaly, Warning,
			TEXT("%s: IAI.Bench.TexCorruptCommitDelay %d - the event is APPLIED and labelled from frame %llu, but the slot commit ")
			TEXT("(step 7) is held until frame %llu, so the picture changes %d frame(s) after the label. BENCH DEVICE (B-M53 can-fail)."),
			*Id.ToString(), L.CommitDelay, GFrameCounter, CommitDueFrame, L.CommitDelay);
	}
#endif
	if (bCommitNow)
	{
		CommitSlots();
	}

	{
		TMap<AActor*, TArray<const UPrimitiveComponent*>> ByOwner;
		for (const FOwnedSlot& OS : Slots)
		{
			if (UMeshComponent* C = OS.Comp.Get())
			{
				ByOwner.FindOrAdd(C->GetOwner()).AddUnique(C);
			}
		}
		for (const TPair<AActor*, TArray<const UPrimitiveComponent*>>& P : ByOwner)
		{
			ComponentsCorrupted += P.Value.Num();
		}
		if (ComponentsSkipped > 0 && !Levers().bNoPartScope)
		{
			for (const TPair<AActor*, TArray<const UPrimitiveComponent*>>& P : ByOwner)
			{
				if (P.Key)
				{
					AnomalyViewport::SetEventComponentScope(P.Key, P.Value);
					ScopedOwners.Add(P.Key);
				}
			}
		}
		FRunStats& Stats = FStatsAccess::Mutable();
		for (const FOutput& O : Outputs)
		{
			Stats.ResidentChainOutputs += O.FirstMip > 0 ? 1 : 0;
		}
		if (ComponentsSkipped > 0)
		{
			++Stats.FiresWithSkippedParts;
			Stats.ComponentsSkipped += ComponentsSkipped;
			UE_LOG(LogAnomaly, Log,
				TEXT("%s: PARTS - %d component(s) corrupted, %d skipped because not every part of them qualified; the mask and the ")
				TEXT("projected box cover the corrupted component(s) only (scope %s)."),
				*Id.ToString(), ComponentsCorrupted, ComponentsSkipped, Levers().bNoPartScope ? TEXT("OFF - bench lever") : TEXT("set"));
		}
	}

	RegisterTargetWatch(Injector);

	bForeignReplacePending = L.bForeignReplace && NoApply == 0;
	bActive = true;
	++FStatsAccess::Mutable().FiresApplied;
	int32 FirstBad = -1;
	const TexCorruptPure::EHeld Reading = EvaluateCondition(FirstBad);
	const double ApplyMs = ElapsedMs(ApplyStartSeconds);
	UE_LOG(LogAnomaly, Log,
		TEXT("%s: APPLIED mode=%s tile=%d fault=%s noapply=%d on '%s' - %d output chain(s), %d scratch class(es), %d host MID(s), ")
		TEXT("slots %d/%d committed, required=%lld reserved=%lld live=%lld pending=%lld peak=%lld cap=%s, condition read through ")
		TEXT("the live predicate: %s (first failing index %d). Every level of every output was drawn from the source's own mip ")
		TEXT("and copied, enqueued before any scene draw of this frame (plan R7.4).%s apply_ms=%.3f"),
		*Id.ToString(), LexMode(Mode), TileN, LexWrongCopy(Fault), NoApply, *In.TargetQuery, Outputs.Num(), Scratch.Num(),
		HostMids.Num(), SlotsCorrupted, SlotsTotal, RequiredBytes, Account.Reserved, Ledger().Live, Ledger().PendingSum(),
		Ledger().Peak, *DescribeMaxRtBytes(), ANSI_TO_TCHAR(TexCorruptPure::LexHeld(Reading)), FirstBad, *Attempt.Describe(), ApplyMs);
	return true;
}

int32 FAnomaly_TexCorrupt::CommitSlots()
{
	int32 Committed = 0;
	for (FOwnedSlot& OS : Slots)
	{
		UMeshComponent* Comp = OS.Comp.Get();
		if (Comp && OS.HostMid && !OS.bCommitted)
		{
			Comp->SetMaterial(OS.SlotIndex, OS.HostMid);
			OS.bCommitted = true;
			OS.OverrideLenAfter = Comp->OverrideMaterials.Num();
			++SlotsCorrupted;
			++Committed;
		}
	}
	return Committed;
}

void FAnomaly_TexCorrupt::RegisterTargetWatch(UAnomalyInjectorSubsystem* Injector)
{
	int32 Watched = 0;
	for (const TWeakObjectPtr<AActor>& W : Owners)
	{
		if (Injector && W.Get())
		{
			Injector->WatchTargetForAnomaly(W.Get(), Id);
			++Watched;
		}
	}
	UE_LOG(LogAnomaly, Log, TEXT("%s: target watch registered on %d owner(s) (noapply=%d)."), *Id.ToString(), Watched, NoApply);
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
	Ledger().Tick(GFrameCounter);

#if !UE_BUILD_SHIPPING
	if (bRestorePending && GFrameCounter >= RestoreDueFrame)
	{
		FinishPendingRestore(TEXT("due"));
	}
#endif

	if (PostRevertFrame != 0 && GFrameCounter >= PostRevertFrame)
	{
		TakePostRevertSample();
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

#if !UE_BUILD_SHIPPING
	if (bCommitPending && GFrameCounter >= CommitDueFrame)
	{
		bCommitPending = false;
		const int32 Committed = CommitSlots();
		UE_LOG(LogAnomaly, Warning,
			TEXT("%s: IAI.Bench.TexCorruptCommitDelay - committed %d slot(s) at frame %llu, %llu frame(s) after APPLIED at frame %llu."),
			*Id.ToString(), Committed, GFrameCounter, GFrameCounter - ApplyFrame, ApplyFrame);
	}
#endif

	++TicksSinceApply;
#if !UE_BUILD_SHIPPING
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
#endif

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
	const double StartSeconds = FPlatformTime::Seconds();
	ReleaseTargetWatch();
	bCommitPending = false;
#if !UE_BUILD_SHIPPING
	const int32 RestoreDelay = Levers().RestoreDelay;
	if (RestoreDelay > 0)
	{
		bActive = false;
		bRestorePending = true;
		RestoreDelayedBy = RestoreDelay;
		RestoreDueFrame = GFrameCounter + (uint64)RestoreDelay;
		UE_LOG(LogAnomaly, Warning,
			TEXT("%s: IAI.Bench.TexCorruptRestoreDelay %d - REVERT called at frame %llu and the event ends here, but every slot keeps ")
			TEXT("the corrupted MID (and every render target stays live) until frame %llu, when the REVERT line below runs the ")
			TEXT("restore. BENCH DEVICE (B-M53 can-fail)."),
			*Id.ToString(), RestoreDelay, GFrameCounter, RestoreDueFrame);
		return;
	}
#endif
	RestoreAndRelease(StartSeconds, 0);
}

void FAnomaly_TexCorrupt::FinishPendingRestore(const TCHAR* Context)
{
	if (!bRestorePending)
	{
		return;
	}
	const double StartSeconds = FPlatformTime::Seconds();
	const int32 DelayedBy = RestoreDelayedBy;
	UE_LOG(LogAnomaly, Log, TEXT("%s: delayed restore runs now (%s) at frame %llu, due %llu."), *Id.ToString(), Context, GFrameCounter,
		RestoreDueFrame);
	bRestorePending = false;
	RestoreAndRelease(StartSeconds, DelayedBy);
}

void FAnomaly_TexCorrupt::RestoreAndRelease(double StartSeconds, int32 DelayedBy)
{
	ClearPartScope();
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
		if (!Comp)
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
		if (OS.SlotIndex >= Comp->GetNumMaterials())
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("%s: '%s' now has %d slot(s) but our MID is still stored at index %d - it is restored there too, so it ")
				TEXT("cannot come back on a later mesh change."),
				*Id.ToString(), *Comp->GetName(), Comp->GetNumMaterials(), OS.SlotIndex);
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

	int32 Residual = 0;
	{
		TArray<UMeshComponent*> Check;
		for (const FOwnedSlot& OS : Slots)
		{
			if (UMeshComponent* C = OS.Comp.Get())
			{
				Check.AddUnique(C);
			}
		}
		for (const TWeakObjectPtr<AActor>& W : Owners)
		{
			if (AActor* Owner = W.Get())
			{
				TArray<UMeshComponent*> Comps;
				Owner->GetComponents<UMeshComponent>(Comps);
				for (UMeshComponent* C : Comps)
				{
					if (C)
					{
						Check.AddUnique(C);
					}
				}
			}
		}
		for (UMeshComponent* C : Check)
		{
			TArray<bool> Ours;
			for (int32 i = 0; i < C->OverrideMaterials.Num(); ++i)
			{
				UMaterialInstanceDynamic* AsMid = Cast<UMaterialInstanceDynamic>(C->OverrideMaterials[i].Get());
				Ours.Add(AsMid != nullptr && OurMids.Contains(AsMid));
			}
			Residual += AnomalyInstall::CountOwnedOverrides(Ours.GetData(), Ours.Num());
		}
	}
	if (Residual > 0)
	{
		UE_LOG(LogAnomaly, Error,
			TEXT("%s: REVERT-RESIDUAL %d override(s) of ours still stored after revert - the corruption can reappear with no ")
			TEXT("event or label. This is a defect; report it."),
			*Id.ToString(), Residual);
	}

	FRunStats& Stats = FStatsAccess::Mutable();
	Stats.RestoredExact += Exact;
	Stats.RestoredDefault += Default;
	Stats.LeftToGame += LeftToGame;
	Stats.Swept += Swept;

	ReleaseAllTargets();
	LetGoAll();

	UE_LOG(LogAnomaly, Log,
		TEXT("%s: REVERT restored-exact=%d restored-default=%d left-to-game=%d unresolved=%d swept=%d residual=%d; every render target ")
		TEXT("(scratch included) released, originals let go after their slots were restored; live=%lld pending=%lld.%s ")
		TEXT("restore_delay=%d revert_ms=%.3f"),
		*Id.ToString(), Exact, Default, LeftToGame, Unresolved, Swept, Residual, Ledger().Live, Ledger().PendingSum(), *Attempt.Describe(),
		DelayedBy, ElapsedMs(StartSeconds));

	RevertFrame = GFrameCounter;
	PostRevertFrame = TexCorruptPure::PostRevertSampleFrame(RevertFrame);
	bTerminalRollback = false;
	ResetEventState();
	bActive = false;
}

TexCorruptPure::EHeld FAnomaly_TexCorrupt::EvaluateCondition(int32& OutFirstBad) const
{
	TArray<bool> SlotHolds;
	for (const FOwnedSlot& OS : Slots)
	{
		if (!OS.HostMid)
		{
			continue;
		}
		const UMeshComponent* Comp = OS.Comp.Get();
		SlotHolds.Add(Comp && Comp->OverrideMaterials.IsValidIndex(OS.SlotIndex) && Comp->OverrideMaterials[OS.SlotIndex].Get() == OS.HostMid);
	}
	TArray<bool> BindingReads;
	for (const FHostMid& HM : HostMids)
	{
		for (const TPair<FMaterialParameterInfo, UTextureRenderTarget2D*>& Pair : HM.Bound)
		{
			UTexture* Read = nullptr;
			BindingReads.Add(HM.Mid && Pair.Value && HM.Mid->GetTextureParameterValue(FHashedMaterialParameterInfo(Pair.Key), Read, true)
				&& Read == Pair.Value);
		}
	}
	int FirstBad = -1;
	const TexCorruptPure::EHeld Result = TexCorruptPure::ConditionHeld(bActive, SlotHolds.GetData(), SlotHolds.Num(),
		BindingReads.GetData(), BindingReads.Num(), FirstBad);
	OutFirstBad = FirstBad;
	return Result;
}

bool FAnomaly_TexCorrupt::IsVisualConditionHeld() const
{
	return AnomalyInstall::IsInstalledByte(GetVisualConditionState());
}

unsigned char FAnomaly_TexCorrupt::GetVisualConditionState() const
{
	if (!bActive)
	{
		return (unsigned char)AnomalyInstall::EState::None;
	}
	int32 Targeted = 0;
	int32 Ours = 0;
	for (const FOwnedSlot& OS : Slots)
	{
		if (!OS.HostMid)
		{
			continue;
		}
		++Targeted;
		bool bBindingsHold = false;
		for (const FHostMid& HM : HostMids)
		{
			if (HM.Mid != OS.HostMid)
			{
				continue;
			}
			bBindingsHold = true;
			for (const TPair<FMaterialParameterInfo, UTextureRenderTarget2D*>& Pair : HM.Bound)
			{
				UTexture* Read = nullptr;
				if (!Pair.Value || !HM.Mid->GetTextureParameterValue(FHashedMaterialParameterInfo(Pair.Key), Read, true) || Read != Pair.Value)
				{
					bBindingsHold = false;
					break;
				}
			}
			break;
		}
		const UMeshComponent* Comp = OS.Comp.Get();
		if (!Comp)
		{
			Comp = FindComponentByName(OS.Owner.Get(), OS.CompName);
		}
		AnomalyInstall::FSlotView View;
		View.bComponentValid = Comp != nullptr;
		if (Comp)
		{
			View.bRegistered = Comp->IsRegistered();
			View.bRenders = Comp->ShouldRender();
			View.SlotIndex = OS.SlotIndex;
			View.NumMaterials = Comp->GetNumMaterials();
			View.bResolvedIsOurs = bBindingsHold && OS.SlotIndex >= 0 && OS.SlotIndex < View.NumMaterials
				&& Comp->GetMaterial(OS.SlotIndex) == OS.HostMid;
		}
		if (AnomalyInstall::SlotRendersOurs(View))
		{
			++Ours;
		}
	}
	return (unsigned char)AnomalyInstall::Classify(Targeted, Ours);
}

void FAnomaly_TexCorrupt::NoteCapturedFrame(bool bAnomalousThisFrame)
{
	if (!bActive || !bAnomalousThisFrame)
	{
		return;
	}
	FRunStats& Stats = FStatsAccess::Mutable();
	Stats.CollateralDrops += CountCollateralDrops();
	if (CollateralIncomplete() > 0)
	{
		++Stats.CollateralIncompleteFrames;
	}
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
	Out.AddInt(TEXT("texcorrupt.components_corrupted"), ComponentsCorrupted);
	Out.AddInt(TEXT("texcorrupt.components_skipped"), ComponentsSkipped);
	int32 FirstBad = -1;
	const TexCorruptPure::EHeld Reading = EvaluateCondition(FirstBad);
	Out.AddBool(TEXT("texcorrupt.condition_held"), Reading == TexCorruptPure::EHeld::Held);
	Out.AddString(TEXT("texcorrupt.condition_detail"), ANSI_TO_TCHAR(TexCorruptPure::LexHeld(Reading)));
	Out.AddInt(TEXT("texcorrupt.required_bytes"), (int32)FMath::Min<int64>(RequiredBytes, MAX_int32));
	const int32 CollIncomplete = CollateralIncomplete();
	Out.AddInt(TEXT("texcorrupt.collateral_drops"), CountCollateralDrops());
	Out.AddInt(TEXT("texcorrupt.collateral_count"), Collateral.Num());
	Out.AddBool(TEXT("texcorrupt.collateral_truncated"), bCollateralTruncated);
	Out.AddInt(TEXT("texcorrupt.collateral_incomplete"), CollIncomplete);
	Out.AddInt(TEXT("texcorrupt.collateral_unresolved"), CollateralUnresolved);
	Out.AddBool(TEXT("texcorrupt.collateral_complete"), TexCorruptPure::CollateralComplete(bCollateralTaken, CollIncomplete));
	if (Mode == EMode::TileProbe || Mode == EMode::Tile)
	{
		Out.AddInt(TEXT("texcorrupt.tile"), TileN);
		Out.AddInt(TEXT("texcorrupt.tile_detail_mips"), FMath::FloorLog2(FMath::Max(1, TileN)));
	}
	if (Attempt.Ordinal >= 0)
	{
		Out.AddInt(TEXT("texcorrupt.ordinal"), Attempt.Ordinal);
	}
	if (Attempt.bScramble)
	{
		Out.AddInt(TEXT("texcorrupt.scramble_cells"), Attempt.Scramble.K);
		Out.AddInt(TEXT("texcorrupt.scramble_a"), Attempt.Scramble.A);
		Out.AddInt(TEXT("texcorrupt.scramble_b"), Attempt.Scramble.B);
		Out.AddInt(TEXT("texcorrupt.scramble_a_inv"), Attempt.Scramble.AInv);
		Out.AddString(TEXT("texcorrupt.scramble_h"), FString::Printf(TEXT("%u"), Attempt.Scramble.H));
	}
	if (NoApply != 0)
	{
		Out.AddInt(TEXT("texcorrupt.bench_noapply"), NoApply);
	}
	if (Fault != EWrongCopy::None)
	{
		Out.AddString(TEXT("texcorrupt.bench_wrong_copy"), LexWrongCopy(Fault));
	}
	for (const FString& Part : PartsCorrupted)
	{
		FAnomalyTelemetryFields& Rec = Out.AddArrayEntry(TEXT("texcorrupt.slots_corrupted_list"));
		Rec.AddString(TEXT("slot"), Part);
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
		Rec.AddInt(TEXT("snapshot_mip"), R.FirstMip);
		Rec.AddInt(TEXT("cooked_mip_count"), R.CookedM);
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
	ClearPartScope();
	if (bRestorePending)
	{
		FinishPendingRestore(TEXT("world teardown"));
	}
	if (PostRevertFrame != 0)
	{
		const TexCorruptPure::ELedgerBalance Balance = TexCorruptPure::JudgeLedgerReading(RevertFrame, GFrameCounter, Ledger().Live,
			Ledger().PendingSum());
		UE_LOG(LogAnomaly, Log,
			TEXT("TEXCORRUPT-LEDGER id=%s kind=world_teardown %s_frame=%llu frame=%llu offset=%llu endpoint=cancelled live=%lld ")
			TEXT("pending=%lld peak=%lld balance=%s"),
			*Id.ToString(), bTerminalRollback ? TEXT("rollback") : TEXT("revert"), RevertFrame, GFrameCounter, GFrameCounter - RevertFrame,
			Ledger().Live, Ledger().PendingSum(), Ledger().Peak, ANSI_TO_TCHAR(TexCorruptPure::LexLedgerBalance(Balance)));
	}
	Collateral.Reset();
	bCollateralTaken = false;
	PostRevertFrame = 0;
	bTerminalRollback = false;
}
}
