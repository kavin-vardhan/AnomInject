#include "Anomalies/Anomaly_StuckLowMip.h"

#include "AnomalyDefaults.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyLod.h"
#include "AnomalyStuckMipStats.h"
#include "AnomalyViewport.h"
#include "Components/MeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/StreamableRenderAsset.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "SceneTypes.h"
#include "UnrealClient.h"

namespace
{
	bool GStuckMipNoHold = false;
	bool GStuckMipUnlinkLock = false;
	AnomalyStuckMip::FRunStats GStuckMipStats;

	int32 ReadIntCVar(const TCHAR* Name, int32 Fallback)
	{
		if (IConsoleVariable* Var = IConsoleManager::Get().FindConsoleVariable(Name))
		{
			return Var->GetInt();
		}
		return Fallback;
	}

	int32 PredictMaxAllowedMips(const UTexture2D* Tex)
	{
		const FStreamableRenderResourceState& S = Tex->GetStreamableResourceState();
		const int32 Bias = FMath::Max(0, Tex->GetCachedLODBias() - (int32)S.AssetLODBias);
		return FMath::Clamp((int32)S.MaxNumLODs - Bias, (int32)S.NumNonStreamingLODs, (int32)S.MaxNumLODs);
	}

	int32 TopMipWidthAt(const UTexture2D* Tex, int32 ResidentMipCount)
	{
		const int32 Full = Tex->GetNumMips();
		const int32 Shift = FMath::Clamp(Full - ResidentMipCount, 0, 31);
		return FMath::Max(1, Tex->GetSizeX() >> Shift);
	}

	bool IsExcludedLodGroup(TextureGroup Group)
	{
		switch (Group)
		{
		case TEXTUREGROUP_UI:
		case TEXTUREGROUP_Lightmap:
		case TEXTUREGROUP_Shadowmap:
		case TEXTUREGROUP_Terrain_Heightmap:
		case TEXTUREGROUP_Terrain_Weightmap:
		case TEXTUREGROUP_Bokeh:
			return true;
		default:
			return false;
		}
	}

	enum class EEligibility : uint8
	{
		Eligible,
		Virtual,
		NotStreamable,
		ExcludedGroup
	};

	EEligibility ClassifyTexture(UTexture2D* Tex)
	{
		if (!Tex)
		{
			return EEligibility::NotStreamable;
		}
		if (Tex->IsCurrentlyVirtualTextured())
		{
			return EEligibility::Virtual;
		}
		if (IsExcludedLodGroup(Tex->LODGroup))
		{
			return EEligibility::ExcludedGroup;
		}
		if (Tex->NeverStream || !Tex->IsStreamable() || !Tex->RenderResourceSupportsStreaming())
		{
			return EEligibility::NotStreamable;
		}
		const FStreamableRenderResourceState& S = Tex->GetStreamableResourceState();
		if (!S.IsValid() || (int32)S.MaxNumLODs <= (int32)S.NumNonStreamingLODs)
		{
			return EEligibility::NotStreamable;
		}
		return EEligibility::Eligible;
	}

	void CollectComponentTextures(UPrimitiveComponent* Component, TArray<UTexture2D*>& Out)
	{
		if (!Component)
		{
			return;
		}
		TArray<UTexture*> Used;
		Component->GetUsedTextures(Used, EMaterialQualityLevel::Num);
		for (UTexture* T : Used)
		{
			if (UTexture2D* T2 = Cast<UTexture2D>(T))
			{
				Out.AddUnique(T2);
			}
		}
	}

	void BuildVisibleTextureUserCounts(UWorld* World, const TSet<const AActor*>& IgnoreActors,
		TMap<UTexture2D*, int32>& Out)
	{
		for (const TWeakObjectPtr<AActor>& Weak : AnomalyViewport::GetVisibleRenderableActors(World))
		{
			AActor* Other = Weak.Get();
			if (!Other || IgnoreActors.Contains(Other))
			{
				continue;
			}
			TArray<UPrimitiveComponent*> Comps;
			Other->GetComponents<UPrimitiveComponent>(Comps);
			for (UPrimitiveComponent* Comp : Comps)
			{
				if (!Comp || !AnomalyViewport::IsRenderableComponent(Comp))
				{
					continue;
				}
				TArray<UTexture2D*> Textures;
				CollectComponentTextures(Comp, Textures);
				for (UTexture2D* T : Textures)
				{
					Out.FindOrAdd(T)++;
				}
			}
		}
	}

	FIntPoint ResolveViewportSize()
	{
		if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
		{
			const FIntPoint S = GEngine->GameViewport->Viewport->GetSizeXY();
			if (S.X > 0 && S.Y > 0)
			{
				return S;
			}
		}
		return FIntPoint(1920, 1080);
	}

	float LongestBboxSidePx(UWorld* World, const AActor* Actor)
	{
		FAnomalyViewInfo View;
		if (!AnomalyViewport::GetActiveViewInfo(World, View))
		{
			return -1.0f;
		}
		FVector2D Min(FVector2D::ZeroVector);
		FVector2D Max(FVector2D::ZeroVector);
		if (!AnomalyViewport::ProjectActorBoundsToScreenRect(View, Actor, Min, Max))
		{
			return -1.0f;
		}
		const FIntPoint Size = ResolveViewportSize();
		const float WidthPx = (float)((Max.X - Min.X) * (double)Size.X);
		const float HeightPx = (float)((Max.Y - Min.Y) * (double)Size.Y);
		return FMath::Max(WidthPx, HeightPx);
	}
}

namespace AnomalyStuckMip
{
	void ResetRunStats()
	{
		GStuckMipStats = FRunStats();
	}

	FRunStats GetRunStats()
	{
		return GStuckMipStats;
	}

	bool IsNoHoldLeverOn()
	{
		return GStuckMipNoHold;
	}

	bool IsUnlinkLockOn()
	{
		return GStuckMipUnlinkLock;
	}
}

bool FAnomaly_StuckLowMip::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("stuck_low_mip: usage <substring> [mip_levels]"));
		return false;
	}

	const int32 EffectiveLevels = AnomalyDefaults::GetStuckMipLevels();
	int32 Levels = EffectiveLevels;
	if (Args.Num() >= 2)
	{
		if (Args[1].IsNumeric())
		{
			Levels = FCString::Atoi(*Args[1]);
		}
		else
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: '%s' is not a whole number of mip levels; using %d."), *Args[1], EffectiveLevels);
		}
	}
	if (Levels < AnomalyDefaults::StuckMipLevelsMin || Levels > AnomalyDefaults::StuckMipLevelsMax)
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("stuck_low_mip: mip_levels %d out of range [%d..%d]; using %d."),
			Levels, AnomalyDefaults::StuckMipLevelsMin, AnomalyDefaults::StuckMipLevelsMax, EffectiveLevels);
		Levels = EffectiveLevels;
	}

	const FString& Substring = Args[0];

	TArray<TWeakObjectPtr<UMeshComponent>> Meshes = AnomalyLod::ResolveLodComponents(World, Substring);
	if (UAnomalyInjectorSubsystem::IsViewportScopingEnabled(World))
	{
		FAnomalyViewInfo View;
		if (AnomalyViewport::GetActiveViewInfo(World, View))
		{
			Meshes = AnomalyViewport::FilterVisibleComponents(View, World, Meshes);
		}
	}
	if (Meshes.Num() == 0)
	{
		UE_LOG(LogAnomaly, Log, TEXT("stuck_low_mip: matched 0 mesh component(s) for '%s'."), *Substring);
		return false;
	}

	const AActor* RequestedActor = nullptr;
	for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
	{
		if (UMeshComponent* Mesh = Weak.Get())
		{
			RequestedActor = Mesh->GetOwner();
			break;
		}
	}

	if (bActive)
	{
		if (RequestedActor && RequestedActor == PrimaryOwner.Get())
		{
			++GStuckMipStats.RefusedAlreadyHeld;
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: REFUSED already_held - '%s' resolves to '%s', which is the target this anomaly is ")
				TEXT("HOLDING RIGHT NOW (%d texture(s)). Reverting and re-applying in one call would restart the hold ")
				TEXT("with a baseline read from the ALREADY-HELD state, so the second event would record a baseline ")
				TEXT("that is itself the anomaly. No fire is recorded and nothing is changed."),
				*Substring, *PrimaryOwnerName, Held.Num());
			return false;
		}
		Revert();
	}

	bNoHoldLever = GStuckMipNoHold;
	bUseAllMips = ReadIntCVar(TEXT("r.Streaming.UseAllMips"), 0) != 0;
	bStreamingDisabled = ReadIntCVar(TEXT("r.TextureStreaming"), 1) == 0;

	UE_LOG(LogAnomaly, Log,
		TEXT("stuck_low_mip: PRECONDITION READ-BACK r.TextureStreaming=%d r.Streaming.UseAllMips=%d. These are READ ")
		TEXT("from the live console, never assumed: with streaming off nothing is streamable and the anomaly has no ")
		TEXT("lever, and with UseAllMips non-zero the streamer skips the per-texture LOD bias entirely ")
		TEXT("(StreamingTexture.cpp:189) so the hold cannot engage. Either one is reported per frame rather than ")
		TEXT("producing a clean null."),
		ReadIntCVar(TEXT("r.TextureStreaming"), 1), ReadIntCVar(TEXT("r.Streaming.UseAllMips"), 0));

	const bool bAutoPool = UAnomalyInjectorSubsystem::IsAutoPoolSelection(World);
	const int32 MaxCoAffected = bAutoPool ? AnomalyDefaults::GetStuckMipMaxCoAffected() : TNumericLimits<int32>::Max();
	const float MinTexelRatio = bAutoPool ? AnomalyDefaults::GetStuckMipMinTexelRatio() : 0.0f;

	if (!bAutoPool)
	{
		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: TARGETED FIRE on '%s' - the co-affected gate (max %d other VISIBLE components per ")
			TEXT("texture) and the %.2fx perceptibility ratio are BYPASSED because they govern AUTO-POOL SELECTION ")
			TEXT("only. An explicitly named object is the operator's decision. LABELLING IS UNCHANGED and still ")
			TEXT("discriminates per frame: the label is driven by the MEASURED resident mip count, so a fire that ")
			TEXT("never drops a mip carries zero positive frames."),
			*Substring, AnomalyDefaults::GetStuckMipMaxCoAffected(), AnomalyDefaults::GetStuckMipMinTexelRatio());
	}

	TSet<const AActor*> IgnoreActors;
	for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
	{
		if (UMeshComponent* Mesh = Weak.Get())
		{
			IgnoreActors.Add(Mesh->GetOwner());
		}
	}

	TMap<UTexture2D*, int32> VisibleUsers;
	if (bAutoPool)
	{
		BuildVisibleTextureUserCounts(World, IgnoreActors, VisibleUsers);
	}

	const AActor* PrimaryActor = RequestedActor;
	float PrimaryBboxPx = -1.0f;
	if (PrimaryActor)
	{
		PrimaryBboxPx = LongestBboxSidePx(World, PrimaryActor);
	}

	TArray<UTexture2D*> Candidates;
	for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
	{
		CollectComponentTextures(Weak.Get(), Candidates);
	}

	for (UTexture2D* Tex : Candidates)
	{
		if (!IsAwaitingRestore(Tex))
		{
			continue;
		}
		++GStuckMipStats.RefusedNotRestored;
		const int32 Resident = Tex ? Tex->GetNumResidentMips() : -1;
		int32 Baseline = -1;
		int32 Waited = -1;
		for (const FRestoringTexture& R : Restoring)
		{
			if (R.Texture.Get() == Tex)
			{
				Baseline = R.BaselineResidentMips;
				Waited = R.FramesWaited;
				break;
			}
		}
		UE_LOG(LogAnomaly, Warning,
			TEXT("stuck_low_mip: REFUSED not_restored - '%s' uses texture '%s', which a PREVIOUS hold has not given ")
			TEXT("back yet (resident %d, baseline %d, %d frame(s) of re-asserted stream-in so far). Firing now would ")
			TEXT("record the still-depressed count as this event's baseline, so the event could never read held:true ")
			TEXT("and would produce no labelled frame at all. REFUSING is what makes that visible instead of silent."),
			*Substring, *GetNameSafe(Tex), Resident, Baseline, Waited);
		return false;
	}

	Held.Reset();
	CapturedFramesSeen = 0;
	OnsetLatencyFrames = -1;
	int32 RefusedVirtual = 0;
	int32 RefusedNotStreamable = 0;
	int32 RefusedGroup = 0;
	int32 RefusedShared = 0;
	int32 RefusedImperceptible = 0;

	for (UTexture2D* Tex : Candidates)
	{
		const EEligibility Class = ClassifyTexture(Tex);
		if (Class == EEligibility::Virtual)
		{
			++RefusedVirtual;
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: NOT_APPLICABLE '%s' - it is CURRENTLY VIRTUAL TEXTURED, so it is not in the ")
				TEXT("render-asset streamer at all and GetNumResidentMips() returns a virtual-resource constant, not ")
				TEXT("a streaming state. Read per texture, never inferred from the project's r.VirtualTextures flag."),
				*GetNameSafe(Tex));
			continue;
		}
		if (Class == EEligibility::ExcludedGroup)
		{
			++RefusedGroup;
			UE_LOG(LogAnomaly, Log,
				TEXT("stuck_low_mip: SKIPPED '%s' - LOD group %d is UI/lightmap/shadowmap/terrain/bokeh. Those are ")
				TEXT("not scene-surface textures of this object; holding one would blur something the label does not ")
				TEXT("name."),
				*GetNameSafe(Tex), (int32)Tex->LODGroup);
			continue;
		}
		if (Class == EEligibility::NotStreamable)
		{
			++RefusedNotStreamable;
			UE_LOG(LogAnomaly, Log,
				TEXT("stuck_low_mip: SKIPPED '%s' - it has no streamable mips (never-stream, unlinked, or every mip ")
				TEXT("is non-streaming). There is nothing to hold."),
				*GetNameSafe(Tex));
			continue;
		}

		const int32 CoAffected = VisibleUsers.FindRef(Tex);
		if (CoAffected > MaxCoAffected)
		{
			++RefusedShared;
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: REFUSED TEXTURE '%s' - %d other VISIBLE component(s) sample it and the maximum ")
				TEXT("is %d. Holding it would blur objects the label does not name, and an unlabelled blurry object ")
				TEXT("in a labelled frame teaches the model that blurry is normal - worse than a missing label. The ")
				TEXT("gate is PER TEXTURE, so the target's other textures are still eligible."),
				*GetNameSafe(Tex), CoAffected, MaxCoAffected);
			continue;
		}

		const FStreamableRenderResourceState& S = Tex->GetStreamableResourceState();
		const int32 FloorMips = (int32)S.NumNonStreamingLODs;
		const int32 FullMips = Tex->GetNumMips();
		const int32 BaselineResident = Tex->GetNumResidentMips();
		const int32 TargetMips = (Levels < 0)
			? FloorMips
			: FMath::Clamp(BaselineResident - Levels, FloorMips, (int32)S.MaxNumLODs);
		if (TargetMips >= BaselineResident)
		{
			++RefusedNotStreamable;
			UE_LOG(LogAnomaly, Log,
				TEXT("stuck_low_mip: SKIPPED '%s' - it is already at or below the requested resident mip count ")
				TEXT("(resident %d, target %d, floor %d). Holding it would change nothing."),
				*GetNameSafe(Tex), BaselineResident, TargetMips, FloorMips);
			continue;
		}

		const int32 TopResidentPx = TopMipWidthAt(Tex, TargetMips);
		if (MinTexelRatio > 0.0f && PrimaryBboxPx > 0.0f && PrimaryBboxPx < MinTexelRatio * (float)TopResidentPx)
		{
			++RefusedImperceptible;
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: REFUSED TEXTURE '%s' - at the held mip its top resident level is %d px wide ")
				TEXT("while the target's longest on-screen side is %.1f px, i.e. below the %.2fx ratio the gate ")
				TEXT("requires. The object would not LOOK blurry, and a positive label with no visible change is the ")
				TEXT("dataset-poisoning direction. THIS IS A PICK-TIME FILTER ONLY and never decides observable: it ")
				TEXT("assumes the texture maps roughly once across the object, which a tiling texture does not."),
				*GetNameSafe(Tex), TopResidentPx, PrimaryBboxPx, MinTexelRatio);
			continue;
		}

		FHeldTexture H;
		H.Texture = Tex;
		H.TextureName = Tex->GetName();
		H.SavedCinematicMips = Tex->NumCinematicMipLevels;
		H.BaselineResidentMips = BaselineResident;
		H.FullMips = FullMips;
		H.FloorMips = FloorMips;
		H.TargetMips = TargetMips;
		H.TopResidentPxAtTarget = TopResidentPx;
		H.CoAffectedVisible = CoAffected;

		if (!bNoHoldLever)
		{
			int32 Candidate = H.SavedCinematicMips + ((int32)S.MaxNumLODs - TargetMips);
			int32 Predicted = 0;
			for (int32 Attempt = 0; Attempt < 8; ++Attempt)
			{
				Tex->NumCinematicMipLevels = Candidate;
				Tex->UpdateCachedLODBias();
				Predicted = PredictMaxAllowedMips(Tex);
				if (Predicted == TargetMips)
				{
					break;
				}
				const int32 Next = (Predicted > TargetMips) ? Candidate + 1 : Candidate - 1;
				if (Next <= H.SavedCinematicMips)
				{
					break;
				}
				Candidate = Next;
			}
			H.AppliedCinematicMips = Tex->NumCinematicMipLevels;
			H.PredictedMaxAllowedMips = Predicted;
		}
		else
		{
			H.AppliedCinematicMips = H.SavedCinematicMips;
			H.PredictedMaxAllowedMips = PredictMaxAllowedMips(Tex);
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: IAI.Bench.StuckMipNoHold IS ON - '%s' is recorded as a held texture and the ")
				TEXT("streaming bias is DELIBERATELY NOT WRITTEN. The mip will not drop, so held reads false on every ")
				TEXT("frame, observable reads false, and the event carries no labelled frame. That is the can-fail ")
				TEXT("proof (G96), not a defect."),
				*GetNameSafe(Tex));
		}

		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: HOLD '%s' full_mips=%d floor_mips=%d baseline_resident=%d target_resident=%d ")
			TEXT("top_resident_px=%d cinematic_mips %d->%d predicted_max_allowed=%d co_affected_visible=%d [%s]."),
			*H.TextureName, H.FullMips, H.FloorMips, H.BaselineResidentMips, H.TargetMips, H.TopResidentPxAtTarget,
			H.SavedCinematicMips, H.AppliedCinematicMips, H.PredictedMaxAllowedMips, H.CoAffectedVisible,
			bAutoPool ? TEXT("auto-pool, gates ENFORCED") : TEXT("targeted, selection gates BYPASSED"));

		Held.Add(H);
	}

	GStuckMipStats.CandidateTexturesSeen += Candidates.Num();
	GStuckMipStats.RefusedVirtual += RefusedVirtual;
	GStuckMipStats.RefusedNotStreamable += RefusedNotStreamable + RefusedGroup;
	GStuckMipStats.RefusedShared += RefusedShared;
	GStuckMipStats.RefusedImperceptible += RefusedImperceptible;

	PrimaryOwner = const_cast<AActor*>(PrimaryActor);
	PrimaryOwnerName = PrimaryActor ? PrimaryActor->GetName() : FString();
	PrimaryIndex = 0;
	int32 WidestPx = -1;
	for (int32 i = 0; i < Held.Num(); ++i)
	{
		if (Held[i].FullMips > WidestPx)
		{
			WidestPx = Held[i].FullMips;
			PrimaryIndex = i;
		}
	}

	bActive = Held.Num() > 0;
	if (!bActive)
	{
		++GStuckMipStats.RefusedNoEligibleTextures;
		UE_LOG(LogAnomaly, Warning,
			TEXT("stuck_low_mip: matched %d component(s) for '%s' with %d candidate texture(s) but HELD NONE [%s] - ")
			TEXT("%d virtual, %d not streamable or already at the floor, %d excluded LOD group, %d shared with a ")
			TEXT("visible component, %d imperceptible at this on-screen size. Applying nothing, so no fire is ")
			TEXT("recorded and no label is written."),
			Meshes.Num(), *Substring, Candidates.Num(),
			bAutoPool ? TEXT("auto-pool, gates ENFORCED") : TEXT("targeted, selection gates BYPASSED"),
			RefusedVirtual, RefusedNotStreamable, RefusedGroup, RefusedShared, RefusedImperceptible);
		return false;
	}

	++GStuckMipStats.FiresApplied;
	GStuckMipStats.TexturesHeld += Held.Num();

	UE_LOG(LogAnomaly, Log,
		TEXT("stuck_low_mip: matched %d component(s) for '%s' - HOLDING %d of %d candidate texture(s) [%s]; ")
		TEXT("%d virtual, %d not streamable, %d excluded group, %d shared, %d imperceptible. The label is driven by ")
		TEXT("the MEASURED resident mip count, so frames before the streamer completes the stream-out are NOT ")
		TEXT("labelled."),
		Meshes.Num(), *Substring, Held.Num(), Candidates.Num(),
		bAutoPool ? TEXT("auto-pool, gates ENFORCED") : TEXT("targeted, selection gates BYPASSED"),
		RefusedVirtual, RefusedNotStreamable, RefusedGroup, RefusedShared, RefusedImperceptible);
	return bActive;
}

void FAnomaly_StuckLowMip::Revert()
{
	int32 Restored = 0;
	int32 LeftToGame = 0;
	int32 Unresolved = 0;
	int32 Relinked = 0;
	int32 AlreadyBack = 0;
	int32 Tracked = 0;

	for (const FHeldTexture& H : Held)
	{
		UTexture2D* Tex = H.Texture.Get();
		if (!Tex)
		{
			++Unresolved;
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: revert could not resolve texture '%s' (destroyed or garbage collected) - the ")
				TEXT("asset no longer exists, so nothing is left held."),
				*H.TextureName);
			continue;
		}

		if (H.bUnlinked)
		{
			Tex->LinkStreaming();
			++Relinked;
		}

		if (Tex->NumCinematicMipLevels != H.AppliedCinematicMips)
		{
			++LeftToGame;
			UE_LOG(LogAnomaly, Log,
				TEXT("stuck_low_mip: revert left '%s' untouched - NumCinematicMipLevels is %d and we wrote %d, so ")
				TEXT("the host changed it after apply. Restoring our saved value would stomp the game's own setting."),
				*H.TextureName, Tex->NumCinematicMipLevels, H.AppliedCinematicMips);
			continue;
		}

		Tex->NumCinematicMipLevels = H.SavedCinematicMips;
		Tex->UpdateCachedLODBias();

		UStreamableRenderAsset* Asset = Tex;
		const bool bBusy = Asset->HasPendingInitOrStreaming();
		if (!bBusy)
		{
			Asset->StreamIn(H.BaselineResidentMips, true);
		}
		++Restored;

		if (Tex->GetNumResidentMips() >= H.BaselineResidentMips)
		{
			++AlreadyBack;
			continue;
		}

		FRestoringTexture R;
		R.Texture = Tex;
		R.TextureName = H.TextureName;
		R.BaselineResidentMips = H.BaselineResidentMips;
		R.StreamInRequests = bBusy ? 0 : 1;
		R.SkippedPending = bBusy ? 1 : 0;
		Restoring.Add(R);
		++Tracked;
		if (bBusy)
		{
			++GStuckMipStats.RestoreSkippedPending;
			UE_LOG(LogAnomaly, Log,
				TEXT("stuck_low_mip: revert of '%s' found a stream operation ALREADY IN FLIGHT, so StreamIn(%d) was ")
				TEXT("NOT issued this frame - HasPendingInitOrStreaming() guards it. That is the silent skip: a single ")
				TEXT("request at revert time is the likeliest one to be dropped, because a bias change is exactly what ")
				TEXT("puts the asset in flight. It is now TRACKED and re-asserted every frame until the resident count ")
				TEXT("reaches the baseline."),
				*H.TextureName, H.BaselineResidentMips);
		}
		else
		{
			++GStuckMipStats.RestoreStreamInReissues;
		}
	}

	UE_LOG(LogAnomaly, Log,
		TEXT("stuck_low_mip: revert of %d held texture(s) - restored=%d left-to-game=%d unresolved=%d relinked=%d ")
		TEXT("already-back=%d awaiting-restore=%d (total tracked %d). The bias is cleared on every restored texture; ")
		TEXT("a texture whose resident count is not yet back at its baseline is POLLED every frame until it is, and ")
		TEXT("while it is pending, any fire on a target that uses it is refused as not_restored."),
		Held.Num(), Restored, LeftToGame, Unresolved, Relinked, AlreadyBack, Tracked, Restoring.Num());

	Held.Reset();
	PrimaryOwner.Reset();
	PrimaryOwnerName.Reset();
	PrimaryIndex = 0;
	CapturedFramesSeen = 0;
	OnsetLatencyFrames = -1;
	bNoHoldLever = false;
	bActive = false;
	GStuckMipStats.TexturesAwaitingRestore = Restoring.Num();
}

bool FAnomaly_StuckLowMip::IsAwaitingRestore(const UTexture2D* Tex) const
{
	if (!Tex)
	{
		return false;
	}
	for (const FRestoringTexture& R : Restoring)
	{
		if (R.Texture.Get() == Tex)
		{
			return true;
		}
	}
	return false;
}

void FAnomaly_StuckLowMip::TickAlways(float DeltaSeconds)
{
	if (Restoring.Num() == 0)
	{
		return;
	}

	const int32 Timeout = AnomalyDefaults::GetStuckMipRestoreTimeout();

	for (int32 i = Restoring.Num() - 1; i >= 0; --i)
	{
		FRestoringTexture& R = Restoring[i];
		UTexture2D* Tex = R.Texture.Get();
		if (!Tex)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: restore watch dropped '%s' - the texture was destroyed or garbage collected after ")
				TEXT("%d frame(s) of waiting. The asset no longer exists, so nothing is left held and nothing further ")
				TEXT("can be asserted about it."),
				*R.TextureName, R.FramesWaited);
			Restoring.RemoveAt(i);
			continue;
		}

		const int32 Resident = Tex->GetNumResidentMips();
		if (Resident >= R.BaselineResidentMips)
		{
			GStuckMipStats.RestoreFramesMax = FMath::Max(GStuckMipStats.RestoreFramesMax, R.FramesWaited);
			UE_LOG(LogAnomaly, Log,
				TEXT("stuck_low_mip: RESTORE VERIFIED '%s' resident %d >= baseline %d after %d frame(s), %d ")
				TEXT("re-asserted stream-in request(s) and %d frame(s) where the engine was already busy. This is a ")
				TEXT("READ-BACK of the engine's own resident count, not an assumption that the revert took."),
				*R.TextureName, Resident, R.BaselineResidentMips, R.FramesWaited, R.StreamInRequests, R.SkippedPending);
			Restoring.RemoveAt(i);
			continue;
		}

		++R.FramesWaited;

		UStreamableRenderAsset* Asset = Tex;
		if (Asset->HasPendingInitOrStreaming())
		{
			++R.SkippedPending;
			++GStuckMipStats.RestoreSkippedPending;
		}
		else
		{
			Asset->StreamIn(R.BaselineResidentMips, true);
			++R.StreamInRequests;
			++GStuckMipStats.RestoreStreamInReissues;
		}

		if (R.FramesWaited >= Timeout && !R.bTimeoutReported)
		{
			R.bTimeoutReported = true;
			++GStuckMipStats.RestoreTimeouts;
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: RESTORE TIMEOUT '%s' - resident %d is still below baseline %d after %d frame(s), ")
				TEXT("%d re-asserted stream-in request(s) and %d frame(s) skipped because the engine was already busy. ")
				TEXT("POLLING CONTINUES: the timeout is a REPORT, not a give-up, and the texture stays tracked so any ")
				TEXT("fire on a target that uses it is refused as not_restored. NO CAUSE IS CLAIMED for the shortfall."),
				*R.TextureName, Resident, R.BaselineResidentMips, R.FramesWaited, R.StreamInRequests, R.SkippedPending);
		}
	}

	for (const FRestoringTexture& R : Restoring)
	{
		GStuckMipStats.RestoreFramesMax = FMath::Max(GStuckMipStats.RestoreFramesMax, R.FramesWaited);
	}
	GStuckMipStats.TexturesAwaitingRestore = Restoring.Num();
}

void FAnomaly_StuckLowMip::NoteCapturedFrame(bool bAnomalousThisFrame)
{
	if (!bActive)
	{
		return;
	}
	if (OnsetLatencyFrames >= 0)
	{
		return;
	}
	if (bAnomalousThisFrame)
	{
		OnsetLatencyFrames = CapturedFramesSeen;
		for (FHeldTexture& H : Held)
		{
			const UTexture2D* Tex = H.Texture.Get();
			H.ResidentAtOnset = Tex ? Tex->GetNumResidentMips() : -1;
		}
		return;
	}
	++CapturedFramesSeen;
}

bool FAnomaly_StuckLowMip::IsCurrentlyAnomalous() const
{
	if (!bActive)
	{
		return false;
	}
	for (const FHeldTexture& H : Held)
	{
		const UTexture2D* Tex = H.Texture.Get();
		if (Tex && Tex->GetNumResidentMips() < H.BaselineResidentMips)
		{
			return true;
		}
	}
	return false;
}

bool FAnomaly_StuckLowMip::GetTelemetry(FAnomalyTelemetry& Out) const
{
	if (!bActive || Held.Num() == 0)
	{
		return false;
	}

	int32 HeldNow = 0;
	int32 Resolved = 0;
	bool bForceResident = false;
	bool bRelinked = false;
	for (const FHeldTexture& H : Held)
	{
		const UTexture2D* Tex = H.Texture.Get();
		if (!Tex)
		{
			continue;
		}
		++Resolved;
		if (Tex->GetNumResidentMips() < H.BaselineResidentMips)
		{
			++HeldNow;
		}
		if (Tex->ShouldMipLevelsBeForcedResident())
		{
			bForceResident = true;
		}
		if (Tex->NumCinematicMipLevels != H.AppliedCinematicMips)
		{
			bRelinked = true;
		}
	}

	const FHeldTexture& P = Held[FMath::Clamp(PrimaryIndex, 0, Held.Num() - 1)];
	const UTexture2D* PrimaryTex = P.Texture.Get();
	const int32 Resident = PrimaryTex ? PrimaryTex->GetNumResidentMips() : -1;

	Out.AddInt(TEXT("stuck_mip.primary_resident_mips"), Resident);
	Out.AddInt(TEXT("stuck_mip.primary_baseline_mips"), P.BaselineResidentMips);
	Out.AddInt(TEXT("stuck_mip.full_mips"), P.FullMips);
	Out.AddInt(TEXT("stuck_mip.floor_mips"), P.FloorMips);
	Out.AddInt(TEXT("stuck_mip.forced_mips"), P.TargetMips);
	Out.AddInt(TEXT("stuck_mip.top_resident_px"), PrimaryTex ? TopMipWidthAt(PrimaryTex, FMath::Max(Resident, 1)) : -1);
	Out.AddInt(TEXT("stuck_mip.textures_held"), HeldNow);
	Out.AddInt(TEXT("stuck_mip.textures_armed"), Held.Num());
	Out.AddInt(TEXT("stuck_mip.co_affected_visible"), P.CoAffectedVisible);
	Out.AddInt(TEXT("stuck_mip.onset_latency_frames"), OnsetLatencyFrames);
	Out.AddBool(TEXT("stuck_mip.held"), HeldNow > 0);
	Out.AddBool(TEXT("stuck_mip.held_all"), Resolved > 0 && HeldNow == Resolved);
	Out.AddString(TEXT("stuck_mip.texture"), P.TextureName);

	for (const FHeldTexture& H : Held)
	{
		const UTexture2D* Tex = H.Texture.Get();
		const int32 Now = Tex ? Tex->GetNumResidentMips() : -1;
		FAnomalyTelemetryFields& Rec = Out.AddArrayEntry(TEXT("stuck_mip.textures"));
		Rec.AddString(TEXT("name"), H.TextureName);
		Rec.AddInt(TEXT("baseline_mips"), H.BaselineResidentMips);
		Rec.AddInt(TEXT("forced_mips"), H.TargetMips);
		Rec.AddInt(TEXT("resident_mips"), Now);
		Rec.AddInt(TEXT("resident_mips_at_onset"), H.ResidentAtOnset);
		Rec.AddInt(TEXT("co_affected_visible"), H.CoAffectedVisible);
		Rec.AddBool(TEXT("held"), Tex != nullptr && Now < H.BaselineResidentMips);
	}

	if (bNoHoldLever)
	{
		Out.AddBool(TEXT("stuck_mip.bench_no_hold"), true);
	}
	if (bUseAllMips)
	{
		Out.AddBool(TEXT("stuck_mip.fail_use_all_mips"), true);
	}
	if (bStreamingDisabled)
	{
		Out.AddBool(TEXT("stuck_mip.fail_streaming_off"), true);
	}
	if (bForceResident)
	{
		Out.AddBool(TEXT("stuck_mip.fail_force_resident"), true);
	}
	if (bRelinked)
	{
		Out.AddBool(TEXT("stuck_mip.fail_host_changed_bias"), true);
	}
	return true;
}

static FAutoConsoleCommandWithWorldAndArgs GBenchStuckMipNoHoldCmd(
	TEXT("IAI.Bench.StuckMipNoHold"),
	TEXT("BENCH DEVICE, console only, default OFF - never in a client payload. ON makes stuck_low_mip do all of its ")
	TEXT("bookkeeping and DELIBERATELY NOT write the streaming bias, so the mip never drops. Pre-declared reading: ")
	TEXT("held reads false on every frame, observable reads false on every frame, injected_frames is EMPTY, and the ")
	TEXT("event is still present with observability_measured false. It exists ONLY to prove the instrument can read ")
	TEXT("FALSE (G96): a held:true that has never been contrasted with a held:false is not a result. ")
	TEXT("Usage: IAI.Bench.StuckMipNoHold <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.StuckMipNoHold <0|1>  (current: %s)"),
					GStuckMipNoHold ? TEXT("ON") : TEXT("off"));
				return;
			}
			GStuckMipNoHold = FCString::Atoi(*Args[0]) != 0;
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Bench.StuckMipNoHold -> %s. BENCH DEVICE. This is the deliberate non-application the ")
				TEXT("can-fail gate must CATCH."),
				GStuckMipNoHold ? TEXT("ON") : TEXT("off"));
		}));

static FAutoConsoleCommandWithWorldAndArgs GBenchStuckMipUnlinkLockCmd(
	TEXT("IAI.Bench.StuckMipUnlinkLock"),
	TEXT("BENCH DEVICE, console only, default OFF - never in a client payload. Reserved fallback: if a fixture ever ")
	TEXT("measures the streamer winning against the per-texture bias, this is where UnlinkStreaming() would be ")
	TEXT("applied as a post-hold lock. It is DELIBERATELY INERT until such a measurement exists, because unlinking ")
	TEXT("before the stream-out completes stalls it (nothing ticks an unlinked asset's pending update) and ")
	TEXT("unlinking after is the cancellation race. Usage: IAI.Bench.StuckMipUnlinkLock <0|1>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.StuckMipUnlinkLock <0|1>  (current: %s)"),
					GStuckMipUnlinkLock ? TEXT("ON") : TEXT("off"));
				return;
			}
			GStuckMipUnlinkLock = FCString::Atoi(*Args[0]) != 0;
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Bench.StuckMipUnlinkLock -> %s. BENCH DEVICE, and INERT by design until a fixture measures ")
				TEXT("the streamer defeating the bias."),
				GStuckMipUnlinkLock ? TEXT("ON") : TEXT("off"));
		}));
