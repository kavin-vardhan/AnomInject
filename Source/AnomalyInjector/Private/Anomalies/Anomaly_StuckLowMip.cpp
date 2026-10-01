#include "Anomalies/Anomaly_StuckLowMip.h"

#include "AnomalyDefaults.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyLod.h"
#include "AnomalyStuckMipStats.h"
#include "AnomalyStuckMipWindow.h"
#include "AnomalyViewport.h"
#include "Components/DecalComponent.h"
#include "Components/MeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "ContentStreaming.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "HAL/PlatformTime.h"
#include "Materials/MaterialInterface.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Level.h"
#include "Engine/LevelStreaming.h"
#include "Engine/StreamableRenderAsset.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "Misc/ScopeLock.h"
#include "SceneTypes.h"
#include "TextureResource.h"
#include "UObject/UObjectIterator.h"
#include "UnrealClient.h"

namespace
{
	bool GStuckMipNoHold = false;
	bool GStuckMipUnlinkLock = false;
#if !UE_BUILD_SHIPPING
	bool GStuckMipLegacyPurity = false;
#endif
	AnomalyStuckMip::FRunStats GStuckMipStats;

	struct FWorldTextureUsers
	{
		TArray<const UActorComponent*> Components;
		TArray<FString> Names;
		int32 InactiveLevelUsers = 0;
		int32 UnregisteredUsers = 0;
	};

	void GatherLoadedLevels(UWorld* World, TSet<const ULevel*>& Out)
	{
		Out.Reset();
		if (!World)
		{
			return;
		}
		for (ULevel* Level : World->GetLevels())
		{
			if (Level)
			{
				Out.Add(Level);
			}
		}
		for (ULevelStreaming* Streaming : World->GetStreamingLevels())
		{
			if (Streaming)
			{
				if (ULevel* Loaded = Streaming->GetLoadedLevel())
				{
					Out.Add(Loaded);
				}
			}
		}
	}

	bool IsComponentStillLoading(const UObject* Object)
	{
		return Object->HasAnyFlags(RF_NeedLoad | RF_NeedPostLoad)
			|| Object->HasAnyInternalFlags(EInternalObjectFlags::Async | EInternalObjectFlags::AsyncLoading);
	}

	AnomalyStuckMipWindow::FComponentScope DescribeComponentScope(const UActorComponent* AC, const TSet<const ULevel*>& Loaded)
	{
		AnomalyStuckMipWindow::FComponentScope S;
		S.bTemplate = AC->IsTemplate();
		S.bPendingKill = !IsValid(AC);
		const ULevel* Level = AC->GetComponentLevel();
		S.bInLoadedLevelOfWorld = Level && Loaded.Contains(Level);
		S.bLevelActive = Level && Level->bIsVisible;
		S.bRegistered = AC->IsRegistered();
		S.bPrimitiveOrDecal = AC->IsA<UPrimitiveComponent>() || AC->IsA<UDecalComponent>();
		return S;
	}

	bool SettleHoldComponent(const UActorComponent* AC, const TSet<const ULevel*>& Loaded)
	{
		const AnomalyStuckMipWindow::FComponentScope S = DescribeComponentScope(AC, Loaded);
		return AnomalyStuckMipWindow::InPurityScope(S) || S.bTemplate || S.bPendingKill || !AC->GetComponentLevel();
	}

	void CollectAnyComponentTextures(UActorComponent* AC, TArray<UTexture*>& Used)
	{
		Used.Reset();
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(AC))
		{
			Prim->GetUsedTextures(Used, EMaterialQualityLevel::Num);
		}
		else if (UDecalComponent* Decal = Cast<UDecalComponent>(AC))
		{
			if (UMaterialInterface* M = Decal->GetDecalMaterial())
			{
				M->GetUsedTextures(Used, EMaterialQualityLevel::Num, true, ERHIFeatureLevel::Num, true);
			}
		}
	}

	void BuildWorldTextureUsers(UWorld* World, const TSet<UTexture2D*>& Wanted,
		TMap<UTexture2D*, FWorldTextureUsers>& Out, int32& OutComponentsScanned, int32& OutLevelsScanned)
	{
		OutComponentsScanned = 0;
		OutLevelsScanned = 0;
		if (!World || Wanted.Num() == 0)
		{
			return;
		}
		TSet<const ULevel*> Loaded;
		GatherLoadedLevels(World, Loaded);
		OutLevelsScanned = Loaded.Num();
		TArray<UTexture*> Used;
		for (const ULevel* Level : Loaded)
		{
			for (AActor* Actor : Level->Actors)
			{
				if (!Actor)
				{
					continue;
				}
				TInlineComponentArray<UActorComponent*> Comps;
				Actor->GetComponents(Comps);
				for (UActorComponent* AC : Comps)
				{
					if (!AC)
					{
						continue;
					}
					const AnomalyStuckMipWindow::FComponentScope Scope = DescribeComponentScope(AC, Loaded);
					if (!AnomalyStuckMipWindow::InPurityScope(Scope))
					{
						continue;
					}
					CollectAnyComponentTextures(AC, Used);
					++OutComponentsScanned;
					for (UTexture* T : Used)
					{
						UTexture2D* T2 = Cast<UTexture2D>(T);
						if (!T2 || !Wanted.Contains(T2))
						{
							continue;
						}
						FWorldTextureUsers& Users = Out.FindOrAdd(T2);
						if (!Users.Components.Contains(AC))
						{
							Users.Components.Add(AC);
							if (!Scope.bLevelActive)
							{
								++Users.InactiveLevelUsers;
							}
							if (!Scope.bRegistered)
							{
								++Users.UnregisteredUsers;
							}
							Users.Names.Add(FString::Printf(TEXT("%s.%s(%s%s%s)"),
								*GetNameSafe(Actor), *AC->GetName(), *AC->GetClass()->GetName(),
								Scope.bLevelActive ? TEXT("") : TEXT(",inactive_level"),
								Scope.bRegistered ? TEXT("") : TEXT(",unregistered")));
						}
					}
				}
			}
		}
	}

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

#if !UE_BUILD_SHIPPING
	void SetLegacyPurityLever(bool bOn)
	{
		GStuckMipLegacyPurity = bOn;
	}

	bool IsLegacyPurityLeverOn()
	{
		return GStuckMipLegacyPurity;
	}
#endif
}

bool FAnomaly_StuckLowMip::Apply(UWorld* World, const TArray<FString>& Args)
{
	LastRefusal = TEXT("refused");
	if (!World)
	{
		LastRefusal = TEXT("no_world");
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("stuck_low_mip: usage <substring> [mip_levels]"));
		LastRefusal = TEXT("usage");
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
		LastRefusal = TEXT("no_mesh");
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
			LastRefusal = TEXT("already_held");
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

#if UE_BUILD_SHIPPING
	const bool bLegacyPurity = false;
#else
	const bool bLegacyPurity = GStuckMipLegacyPurity;
#endif
	if (!bAutoPool)
	{
		const TCHAR* PurityNote =
			TEXT("The PURITY RULE STILL APPLIES: a texture held here must have exactly ONE user component in the ")
			TEXT("whole loaded world, because a blurred object the label does not name is a wrong label whoever ")
			TEXT("chose the target.");
#if !UE_BUILD_SHIPPING
		if (bLegacyPurity)
		{
			PurityNote =
				TEXT("IAI.Bench.StuckMipLegacyPurity IS ON: the legacy visible-only co-affected gate is used and a ")
				TEXT("targeted fire BYPASSES it, exactly as before the 084-02 fix. BENCH DEVICE, can-fail only.");
		}
#endif
		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: TARGETED FIRE on '%s' - the %.2fx perceptibility ratio is BYPASSED because it governs ")
			TEXT("AUTO-POOL SELECTION only. %s"),
			*Substring, AnomalyDefaults::GetStuckMipMinTexelRatio(), PurityNote);
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

	TArray<uint64> TargetComponentIds;
	for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
	{
		if (UMeshComponent* Mesh = Weak.Get())
		{
			TargetComponentIds.Add((uint64)(UPTRINT)static_cast<const UActorComponent*>(Mesh));
		}
	}
	TMap<UTexture2D*, FWorldTextureUsers> WorldUsers;
	int32 ComponentsScanned = 0;
	int32 LevelsScanned = 0;
	double PurityMs = 0.0;
	if (!bLegacyPurity)
	{
		TSet<UTexture2D*> Wanted;
		for (UTexture2D* Tex : Candidates)
		{
			if (Tex)
			{
				Wanted.Add(Tex);
			}
		}
		const double T0 = FPlatformTime::Seconds();
		BuildWorldTextureUsers(World, Wanted, WorldUsers, ComponentsScanned, LevelsScanned);
		PurityMs = (FPlatformTime::Seconds() - T0) * 1000.0;
		++GStuckMipStats.PurityEnumerations;
		GStuckMipStats.PurityEnumerationMsMax = FMath::Max(GStuckMipStats.PurityEnumerationMsMax, PurityMs);
		GStuckMipStats.PurityLevelsScannedMax = FMath::Max(GStuckMipStats.PurityLevelsScannedMax, LevelsScanned);
		int32 InactiveUsers = 0;
		int32 UnregisteredUsers = 0;
		for (const TPair<UTexture2D*, FWorldTextureUsers>& KV : WorldUsers)
		{
			InactiveUsers += KV.Value.InactiveLevelUsers;
			UnregisteredUsers += KV.Value.UnregisteredUsers;
		}
		GStuckMipStats.PurityInactiveLevelUsers += InactiveUsers;
		GStuckMipStats.PurityUnregisteredUsers += UnregisteredUsers;
		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: PURITY ENUMERATION for '%s' - scope ALL LOADED LEVELS (%d, active or not): %d component(s) ")
			TEXT("of every actor in them scanned, REGISTERED OR NOT (primitives of every type, plus decals), for %d ")
			TEXT("candidate texture(s) in %.2f ms; %d user(s) sit in an inactive loaded level and %d are unregistered. A ")
			TEXT("texture is held only if it has exactly ONE user component in that scope and that component belongs to ")
			TEXT("the target. Visibility is NOT consulted: an off-screen user still shows the blur the moment the camera ")
			TEXT("turns to it."),
			*Substring, LevelsScanned, ComponentsScanned, Wanted.Num(), PurityMs, InactiveUsers, UnregisteredUsers);
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
		LastRefusal = TEXT("not_restored");
		return false;
	}

	Held.Reset();
	HeldOwners.Reset();
	CapturedFramesSeen = 0;
	OnsetLatencyFrames = -1;
	int32 RefusedVirtual = 0;
	int32 RefusedNotStreamable = 0;
	int32 RefusedGroup = 0;
	int32 RefusedShared = 0;
	int32 RefusedImperceptible = 0;
	int32 RefusedTooSmallForRatio = 0;

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
		int32 WorldUserCount = -1;
#if !UE_BUILD_SHIPPING
		if (bLegacyPurity)
		{
			if (CoAffected > MaxCoAffected)
			{
				++RefusedShared;
				UE_LOG(LogAnomaly, Warning,
					TEXT("stuck_low_mip: REFUSED TEXTURE '%s' - %d other VISIBLE component(s) sample it and the maximum ")
					TEXT("is %d (LEGACY visible-only rule, IAI.Bench.StuckMipLegacyPurity)."),
					*GetNameSafe(Tex), CoAffected, MaxCoAffected);
				continue;
			}
		}
		else
#endif
		{
			const FWorldTextureUsers* Users = WorldUsers.Find(Tex);
			TArray<uint64> UserIds;
			if (Users)
			{
				for (const UActorComponent* C : Users->Components)
				{
					UserIds.Add((uint64)(UPTRINT)C);
				}
			}
			WorldUserCount = UserIds.Num();
			int32 Foreign = 0;
			const AnomalyStuckMipWindow::EPurity Purity = AnomalyStuckMipWindow::ClassifyPurity(
				UserIds.GetData(), UserIds.Num(), TargetComponentIds.GetData(), TargetComponentIds.Num(), &Foreign);
			if (Purity != AnomalyStuckMipWindow::EPurity::Pure)
			{
				++GStuckMipStats.RefusedSharedWorld;
				++RefusedShared;
				FString Others;
				if (Users)
				{
					for (int32 u = 0; u < Users->Names.Num() && u < 6; ++u)
					{
						Others += Users->Names[u] + TEXT(" ");
					}
				}
				UE_LOG(LogAnomaly, Warning,
					TEXT("stuck_low_mip: REFUSED TEXTURE '%s' shared_world - %d user component(s) in the whole loaded ")
					TEXT("world (%d not a target component), and the rule is exactly ONE. Holding it would blur every ")
					TEXT("other user, on screen or not, while the label and mask name only the target. Users: [ %s]. ")
					TEXT("The gate is PER TEXTURE, so the target's other textures are still eligible."),
					*GetNameSafe(Tex), UserIds.Num(), Foreign, *Others);
				continue;
			}
		}

		if (Tex->HasPendingInitOrStreaming())
		{
			++GStuckMipStats.RefusedBaselinePending;
			++RefusedNotStreamable;
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: REFUSED TEXTURE '%s' baseline_pending - a stream operation is IN FLIGHT, so the ")
				TEXT("game-thread resident count (%d) is not yet the count the renderer draws with. A baseline read ")
				TEXT("now could be the pre- or post-transition value, and every later frame's held/restored verdict ")
				TEXT("is measured against it. Refusing costs one fire; a wrong baseline costs a wrong label."),
				*GetNameSafe(Tex), Tex->GetNumResidentMips());
			continue;
		}

		const FStreamableRenderResourceState& S = Tex->GetStreamableResourceState();
		const int32 FloorMips = (int32)S.NumNonStreamingLODs;
		const int32 FullMips = Tex->GetNumMips();
		const int32 BaselineResident = Tex->GetNumResidentMips();

		const int32 DeepestMips = FMath::Max(FloorMips, AnomalyDefaults::StuckMipMinResidentMips);
		const bool bDepthFromRatioRule = (Levels < 0);
		const int32 TargetMips = bDepthFromRatioRule
			? DeepestMips
			: FMath::Clamp(BaselineResident - Levels, FloorMips, (int32)S.MaxNumLODs);
		if (TargetMips >= BaselineResident)
		{
			++RefusedNotStreamable;
			UE_LOG(LogAnomaly, Log,
				TEXT("stuck_low_mip: SKIPPED '%s' - it is already at or below the requested resident mip count ")
				TEXT("(resident %d, target %d, engine floor %d, deepest this lever can reach %d). Holding it would ")
				TEXT("change nothing."),
				*GetNameSafe(Tex), BaselineResident, TargetMips, FloorMips, DeepestMips);
			continue;
		}

		const int32 TopResidentPx = TopMipWidthAt(Tex, TargetMips);
		const float RatioAtPick = (PrimaryBboxPx > 0.0f && TopResidentPx > 0)
			? PrimaryBboxPx / (float)TopResidentPx
			: -1.0f;

		if (MinTexelRatio > 0.0f && RatioAtPick >= 0.0f && RatioAtPick < MinTexelRatio)
		{
			if (bDepthFromRatioRule)
			{
				++RefusedTooSmallForRatio;
				UE_LOG(LogAnomaly, Warning,
					TEXT("stuck_low_mip: REFUSED TEXTURE '%s' too_small_for_ratio - the DEEPEST hold this lever can ")
					TEXT("ever reach on it leaves a %d px top resident mip, while the target's longest on-screen side ")
					TEXT("is only %.1f px: ratio %.2f against the %.2fx the gate requires. THE DEPTH IS NOT THE ")
					TEXT("BINDING CONSTRAINT AND CANNOT BE MADE ONE - the streamer clamps MaxAllowedMips to ")
					TEXT("NumNonStreamingLODs (%d here) and asserts it (StreamingTexture.cpp:229/233, check at :236), ")
					TEXT("so no bias can drive this texture below %d resident mips. The object is simply too small on ")
					TEXT("screen for any achievable blur, and a positive label with no visible change is the ")
					TEXT("dataset-poisoning direction. NO CAUSE IS CLAIMED beyond the arithmetic."),
					*GetNameSafe(Tex), TopResidentPx, PrimaryBboxPx, RatioAtPick, MinTexelRatio, FloorMips, DeepestMips);
			}
			else
			{
				++RefusedImperceptible;
				UE_LOG(LogAnomaly, Warning,
					TEXT("stuck_low_mip: REFUSED TEXTURE '%s' imperceptible - the EXPLICITLY REQUESTED depth leaves a ")
					TEXT("%d px top resident mip while the target's longest on-screen side is %.1f px: ratio %.2f ")
					TEXT("against the %.2fx the gate requires. A DEEPER hold would satisfy it (the engine floor is %d ")
					TEXT("resident mips), so this is the requested depth being too shallow, not the object being too ")
					TEXT("small. THIS IS A PICK-TIME FILTER ONLY and never decides observable: it assumes the texture ")
					TEXT("maps roughly once across the object, which a tiling texture does not."),
					*GetNameSafe(Tex), TopResidentPx, PrimaryBboxPx, RatioAtPick, MinTexelRatio, DeepestMips);
			}
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
		H.WorldUsers = WorldUserCount;
		H.RatioAtPick = RatioAtPick;
		H.BaselineResourceId = (uint64)(UPTRINT)Tex->GetResource();

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
#if !UE_BUILD_SHIPPING
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: IAI.Bench.StuckMipNoHold IS ON - '%s' is recorded as a held texture and the ")
				TEXT("streaming bias is DELIBERATELY NOT WRITTEN. The mip will not drop, so held reads false on every ")
				TEXT("frame, observable reads false, and the event carries no labelled frame. That is the can-fail ")
				TEXT("proof (G96), not a defect."),
				*GetNameSafe(Tex));
#endif
		}

		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: HOLD '%s' full_mips=%d floor_mips=%d baseline_resident=%d target_resident=%d ")
			TEXT("top_resident_px=%d ratio_at_pick=%.2f cinematic_mips %d->%d predicted_max_allowed=%d ")
			TEXT("co_affected_visible=%d world_users=%d [%s]."),
			*H.TextureName, H.FullMips, H.FloorMips, H.BaselineResidentMips, H.TargetMips, H.TopResidentPxAtTarget,
			H.RatioAtPick, H.SavedCinematicMips, H.AppliedCinematicMips, H.PredictedMaxAllowedMips, H.CoAffectedVisible,
			H.WorldUsers,
			bAutoPool ? TEXT("auto-pool, gates ENFORCED") : TEXT("targeted, selection gates BYPASSED"));

		Held.Add(H);
	}

	GStuckMipStats.CandidateTexturesSeen += Candidates.Num();
	GStuckMipStats.RefusedVirtual += RefusedVirtual;
	GStuckMipStats.RefusedNotStreamable += RefusedNotStreamable + RefusedGroup;
	GStuckMipStats.RefusedShared += RefusedShared;
	GStuckMipStats.RefusedImperceptible += RefusedImperceptible;
	GStuckMipStats.RefusedTooSmallForRatio += RefusedTooSmallForRatio;

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
			TEXT("visible component, %d too small for the ratio at the deepest achievable hold, %d imperceptible at ")
			TEXT("the explicitly requested depth. Applying nothing, so no fire is recorded and no label is written."),
			Meshes.Num(), *Substring, Candidates.Num(),
			bAutoPool ? TEXT("auto-pool, gates ENFORCED") : TEXT("targeted, selection gates BYPASSED"),
			RefusedVirtual, RefusedNotStreamable, RefusedGroup, RefusedShared, RefusedTooSmallForRatio,
			RefusedImperceptible);
		{
			const TPair<const TCHAR*, int32> Buckets[] = { { TEXT("virtual"), RefusedVirtual }, { TEXT("not_streamable"), RefusedNotStreamable }, { TEXT("excluded_group"), RefusedGroup }, { TEXT("shared"), RefusedShared }, { TEXT("too_small"), RefusedTooSmallForRatio }, { TEXT("imperceptible"), RefusedImperceptible } };
			int32 Best = 0;
			LastRefusal = TEXT("no_textures");
			for (const TPair<const TCHAR*, int32>& B : Buckets)
			{
				if (B.Value > Best)
				{
					Best = B.Value;
					LastRefusal = B.Key;
				}
			}
		}
		return false;
	}

	++GStuckMipStats.FiresApplied;
	GStuckMipStats.TexturesHeld += Held.Num();

	HeldWorld = World;
	for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
	{
		if (UMeshComponent* Mesh = Weak.Get())
		{
			if (AActor* Owner = Mesh->GetOwner())
			{
				HeldOwners.AddUnique(Owner);
			}
		}
	}
	if (UAnomalyInjectorSubsystem* Injector = World->GetSubsystem<UAnomalyInjectorSubsystem>())
	{
		for (const TWeakObjectPtr<AActor>& Weak : HeldOwners)
		{
			Injector->WatchTargetForAnomaly(Weak.Get(), GetId());
		}
	}
	StartHoldMonitor(World);

	UE_LOG(LogAnomaly, Log,
		TEXT("stuck_low_mip: matched %d component(s) for '%s' - HOLDING %d of %d candidate texture(s) [%s]; ")
		TEXT("%d virtual, %d not streamable, %d excluded group, %d shared, %d too small for the ratio, %d ")
		TEXT("imperceptible at an explicit depth. %d owning actor(s) are WATCHED for destruction via ")
		TEXT("AActor::OnEndPlay. The label is driven by the MEASURED resident mip count, so frames before the ")
		TEXT("streamer completes the stream-out are NOT labelled."),
		Meshes.Num(), *Substring, Held.Num(), Candidates.Num(),
		bAutoPool ? TEXT("auto-pool, gates ENFORCED") : TEXT("targeted, selection gates BYPASSED"),
		RefusedVirtual, RefusedNotStreamable, RefusedGroup, RefusedShared, RefusedTooSmallForRatio,
		RefusedImperceptible, HeldOwners.Num());
	return bActive;
}

void FAnomaly_StuckLowMip::ReleaseTargetWatch()
{
	if (UWorld* World = HeldWorld.Get())
	{
		if (UAnomalyInjectorSubsystem* Injector = World->GetSubsystem<UAnomalyInjectorSubsystem>())
		{
			Injector->ClearTargetWatchForAnomaly(GetId());
		}
	}
	HeldOwners.Reset();
	HeldWorld.Reset();
}

void FAnomaly_StuckLowMip::OnTargetLost(AActor* Actor, bool bWorldEnding)
{
	if (!bActive)
	{
		return;
	}

	if (!bWorldEnding)
	{
		++GStuckMipStats.RevertOnDestroy;
	}

	UE_LOG(LogAnomaly, Warning,
		TEXT("stuck_low_mip: TARGET LOST - '%s' ended play (%s) while %d texture(s) were held for it. Reverting NOW ")
		TEXT("rather than at the scheduled end of the window. This matters because the anomaly holds a TEXTURE ASSET, ")
		TEXT("not the actor: the mip stays down after the actor is gone, so IsCurrentlyAnomalous() would keep ")
		TEXT("returning true and the capture would keep labelling frames positive for an object that is no longer in ")
		TEXT("the scene. After this revert the anomaly is inactive, so NO FRAME AFTER THIS TICK IS LABELLED for it."),
		*GetNameSafe(Actor), bWorldEnding ? TEXT("world ending") : TEXT("destroyed or removed from the level"),
		Held.Num());

	Revert();
}

void FAnomaly_StuckLowMip::OnWorldTeardown()
{
	StopHoldMonitor();
	const int32 Unverified = Restoring.Num();
	GStuckMipStats.UnverifiedAtTeardown = Unverified;
	if (Unverified <= 0)
	{
		return;
	}

	FString Names;
	for (const FRestoringTexture& R : Restoring)
	{
		Names += FString::Printf(TEXT("'%s'(waited %d) "), *R.TextureName, R.FramesWaited);
	}

	UE_LOG(LogAnomaly, Warning,
		TEXT("stuck_low_mip: UNVERIFIED AT TEARDOWN - %d texture(s) still awaiting a read-back confirmation when the ")
		TEXT("world ended: %s. THE BIAS IS CLEARED ON EVERY ONE OF THEM - nothing is holding the mip down - but ")
		TEXT("nothing ticks after the world is gone, so the restore is NOT VERIFIED the way every other exit path ")
		TEXT("verifies it. That is a weaker piece of evidence than 'RESTORE VERIFIED', and it is counted in ")
		TEXT("run_summary.stuck_mip_unverified_at_teardown rather than folded into the restored count."),
		Unverified, *Names);

	Restoring.Reset();
	bStreamerFencePending = false;
}

void FAnomaly_StuckLowMip::Revert()
{
	StopHoldMonitor();
	ReleaseTargetWatch();

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
			if (!bBusy)
			{
				++AlreadyBack;
				FRestoringTexture Back;
				Back.Texture = Tex;
				Back.TextureName = H.TextureName;
				Back.BaselineResidentMips = H.BaselineResidentMips;
				Back.HeldResidentMips = HeldLevelOf(H);
				Back.BaselineResourceId = H.BaselineResourceId;
				Back.Owner = PrimaryOwner;
				Back.OwnerName = PrimaryOwnerName;
				Restoring.Add(Back);
				++GStuckMipStats.RestoreHeldForStreamerPlans;
				continue;
			}
			++GStuckMipStats.RestoreTrackedWhileBusy;
			UE_LOG(LogAnomaly, Log,
				TEXT("stuck_low_mip: revert of '%s' reads resident %d >= baseline %d BUT a stream operation is still in ")
				TEXT("flight, so it is TRACKED rather than counted already-back: an in-flight stream-out can still land ")
				TEXT("after this revert and leave the picture low with nobody re-asserting the stream-in."),
				*H.TextureName, Tex->GetNumResidentMips(), H.BaselineResidentMips);
		}

		FRestoringTexture R;
		R.Texture = Tex;
		R.TextureName = H.TextureName;
		R.BaselineResidentMips = H.BaselineResidentMips;
		R.HeldResidentMips = HeldLevelOf(H);
		R.BaselineResourceId = H.BaselineResourceId;
		R.Owner = PrimaryOwner;
		R.OwnerName = PrimaryOwnerName;
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

	if (Restoring.Num() > 0)
	{
		bStreamerFencePending = true;
	}

	UE_LOG(LogAnomaly, Log,
		TEXT("stuck_low_mip: revert of %d held texture(s) - restored=%d left-to-game=%d unresolved=%d relinked=%d ")
		TEXT("already-back=%d awaiting-restore=%d (total tracked %d). The bias is cleared on every restored texture; ")
		TEXT("a texture whose resident count is not yet back at its baseline is POLLED every frame until it is, and ")
		TEXT("while it is pending, any fire on a target that uses it is refused as not_restored. Every tracked texture, ")
		TEXT("already-back ones included, is released only after the STREAMER FENCE has retired any mip plan the ")
		TEXT("streamer computed under the hold (queued but not yet issued), which runs on the next tick."),
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

bool FAnomaly_StuckLowMip::RunStreamerFence(int32& OutRetireFrames)
{
	OutRetireFrames = 0;
	AnomalyStuckMipWindow::FStreamerFence Fence;
	Fence.bStreamingEnabled = !IStreamingManager::HasShutdown() && IStreamingManager::Get().IsTextureStreamingEnabled();
	Fence.bAmortizedCopies = ReadIntCVar(TEXT("r.Streaming.AmortizeCPUToGPUCopy"), 0) != 0
		&& ReadIntCVar(TEXT("r.Streaming.MaxNumTexturesToStreamPerFrame"), 0) > 0;
	Fence.MinFramesIfUnfenced = AnomalyStuckMipWindow::UnfencedMinFrames(ReadIntCVar(TEXT("r.Streaming.FramesForFullUpdate"), 5));
	double Ms = 0.0;
	if (Fence.bStreamingEnabled)
	{
		const double Start = FPlatformTime::Seconds();
		IStreamingManager::Get().GetRenderAssetStreamingManager().UpdateResourceStreaming(0.0f, true);
		Ms = (FPlatformTime::Seconds() - Start) * 1000.0;
		Fence.bFlushed = true;
		++GStuckMipStats.StreamerFences;
		GStuckMipStats.StreamerFenceMsMax = FMath::Max(GStuckMipStats.StreamerFenceMsMax, Ms);
	}
	const bool bRetired = AnomalyStuckMipWindow::StreamerPlansRetired(Fence);
	if (!bRetired)
	{
		OutRetireFrames = Fence.MinFramesIfUnfenced;
		++GStuckMipStats.StreamerFenceIncomplete;
		UE_LOG(LogAnomaly, Warning,
			TEXT("stuck_low_mip: STREAMER FENCE INCOMPLETE - r.Streaming.AmortizeCPUToGPUCopy is on with ")
			TEXT("r.Streaming.MaxNumTexturesToStreamPerFrame > 0, so mip copies the streamer queued under the hold can still ")
			TEXT("be issued from its private copy queue after the synchronous update (UE 5.1 exposes no public generation for ")
			TEXT("it). The restoring textures are held for %d further frame(s), two full streaming cycles, before a baseline ")
			TEXT("may confirm; this bound is a heuristic, not a proof. Counted in run_summary.stuck_mip_streamer_fence_incomplete."),
			OutRetireFrames);
	}
	UE_LOG(LogAnomaly, Log,
		TEXT("stuck_low_mip: STREAMER FENCE flushed=%d amortizedCopies=%d streamingEnabled=%d ms=%.3f retired=%d - a ")
		TEXT("synchronous full streaming update (UpdateResourceStreaming(0, true), the engine's own StreamAllResources ")
		TEXT("step) recomputes every wanted mip with the reverted bias and REPLACES the in-flight mip plan, so a stream-out ")
		TEXT("computed under the hold but not yet issued can no longer be issued."),
		Fence.bFlushed ? 1 : 0, Fence.bAmortizedCopies ? 1 : 0, Fence.bStreamingEnabled ? 1 : 0, Ms, bRetired ? 1 : 0);
	return bRetired;
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
	if (bActive && HeldOwners.Num() > 0 && !PrimaryOwner.IsValid())
	{
		++GStuckMipStats.RevertOnDestroy;
		UE_LOG(LogAnomaly, Warning,
			TEXT("stuck_low_mip: TARGET LOST (weak-pointer backstop) - the labelled target '%s' is no longer a valid ")
			TEXT("object and no OnEndPlay reached us, which is the garbage-collected / streamed-out route rather than ")
			TEXT("the Destroy() one. Reverting NOW. The backstop exists because AActor::OnEndPlay covers explicit ")
			TEXT("destruction and level removal but is not a guarantee against every disappearance; a poll of the weak ")
			TEXT("pointer is."),
			*PrimaryOwnerName);
		Revert();
	}

	if (bActive && bHoldMonitorOn)
	{
		const double ScanStart = FPlatformTime::Seconds();
		ScanHoldForNewUsers(TEXT("per-tick new-component diff"));
		const double ScanMs = (FPlatformTime::Seconds() - ScanStart) * 1000.0;
		GStuckMipStats.HoldMonitorScanMsSum += ScanMs;
		if (GStuckMipStats.HoldMonitorScanMs.Num() < 262144)
		{
			GStuckMipStats.HoldMonitorScanMs.Add((float)ScanMs);
		}
	}

	if (bStreamerFencePending)
	{
		bStreamerFencePending = false;
		int32 RetireFrames = 0;
		const bool bRetired = RunStreamerFence(RetireFrames);
		for (FRestoringTexture& R : Restoring)
		{
			if (!R.bFenceRan)
			{
				R.bFenceRan = true;
				R.PlansRetireFrames = bRetired ? 0 : R.FramesWaited + RetireFrames;
			}
		}
	}

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
		const bool bPending = static_cast<UStreamableRenderAsset*>(Tex)->HasPendingInitOrStreaming();
		const bool bPlansRetired = R.bFenceRan && R.FramesWaited >= R.PlansRetireFrames;
		if (Resident >= R.BaselineResidentMips && !bPending && bPlansRetired)
		{
			GStuckMipStats.RestoreFramesMax = FMath::Max(GStuckMipStats.RestoreFramesMax, R.FramesWaited);
			UE_LOG(LogAnomaly, Log,
				TEXT("stuck_low_mip: RESTORE VERIFIED '%s' resident %d >= baseline %d after %d frame(s), %d ")
				TEXT("re-asserted stream-in request(s) and %d frame(s) where the engine was already busy, with the ")
				TEXT("streamer fence past (plans retire frame %d). This is a READ-BACK of the engine's own resident count, ")
				TEXT("not an assumption that the revert took."),
				*R.TextureName, Resident, R.BaselineResidentMips, R.FramesWaited, R.StreamInRequests, R.SkippedPending,
				R.PlansRetireFrames);
			Restoring.RemoveAt(i);
			continue;
		}

		++R.FramesWaited;

		if (Resident >= R.BaselineResidentMips && !bPending)
		{
			continue;
		}

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

int32 FAnomaly_StuckLowMip::HeldLevelOf(const FHeldTexture& H)
{
	const int32 Aim = H.PredictedMaxAllowedMips > 0 ? H.PredictedMaxAllowedMips : H.TargetMips;
	const int32 Floor = FMath::Max(1, H.FloorMips);
	return FMath::Clamp(Aim, Floor, FMath::Max(Floor, H.BaselineResidentMips));
}

bool FAnomaly_StuckLowMip::GetRenderTruthTextures(TArray<FAnomalyRenderTruthTexture>& Out) const
{
	if (!bActive)
	{
		return false;
	}
	for (const FHeldTexture& H : Held)
	{
		FAnomalyRenderTruthTexture R;
		R.Texture = H.Texture;
		R.Name = H.TextureName;
		R.BaselineResidentMips = H.BaselineResidentMips;
		R.HeldResidentMips = HeldLevelOf(H);
		R.BaselineResourceId = H.BaselineResourceId;
		R.Owner = PrimaryOwner;
		R.OwnerName = PrimaryOwnerName;
		Out.Add(R);
	}
	return Out.Num() > 0;
}

bool FAnomaly_StuckLowMip::GetRestoringRenderTruthTextures(TArray<FAnomalyRenderTruthTexture>& Out) const
{
	for (const FRestoringTexture& Rs : Restoring)
	{
		FAnomalyRenderTruthTexture R;
		R.Texture = Rs.Texture;
		R.Name = Rs.TextureName;
		R.BaselineResidentMips = Rs.BaselineResidentMips;
		R.HeldResidentMips = Rs.HeldResidentMips;
		R.BaselineResourceId = Rs.BaselineResourceId;
		R.Owner = Rs.Owner;
		R.OwnerName = Rs.OwnerName;
		Out.Add(R);
	}
	return Out.Num() > 0;
}

bool FAnomaly_StuckLowMip::ConsumeHoldContamination(FString& OutReason)
{
	if (!bContaminationPending)
	{
		return false;
	}
	bContaminationPending = false;
	OutReason = ContaminationReason;
	ContaminationReason.Reset();
	return true;
}

FAnomaly_StuckLowMip::~FAnomaly_StuckLowMip()
{
	StopHoldMonitor();
}

void FAnomaly_StuckLowMip::StartHoldMonitor(UWorld* World)
{
	StopHoldMonitor();
	if (!World)
	{
		return;
	}
	HoldKnownComponents.Reset();
	HoldUnregisteredWatch.Reset();
	TSet<const ULevel*> Loaded;
	GatherLoadedLevels(World, Loaded);
	int32 Deferred = 0;
	auto Seed = [this, &Loaded, &Deferred](UActorComponent* AC)
	{
		if (IsComponentStillLoading(AC))
		{
			++Deferred;
			return;
		}
		HoldKnownComponents.Add(FObjectKey(AC));
		if (!AC->IsRegistered() && AnomalyStuckMipWindow::InPurityScope(DescribeComponentScope(AC, Loaded)))
		{
			HoldUnregisteredWatch.Add(AC);
		}
	};
	for (TObjectIterator<UPrimitiveComponent> It; It; ++It)
	{
		Seed(*It);
	}
	for (TObjectIterator<UDecalComponent> It; It; ++It)
	{
		Seed(*It);
	}
	GStuckMipStats.HoldMonitorUnregisteredWatchedMax =
		FMath::Max(GStuckMipStats.HoldMonitorUnregisteredWatchedMax, HoldUnregisteredWatch.Num());
	{
		FScopeLock Lock(&HoldDirtyCS);
		HoldDirtyRecheck.Reset();
	}
	HoldLevelAddedHandle = FWorldDelegates::LevelAddedToWorld.AddRaw(this, &FAnomaly_StuckLowMip::OnHoldLevelAdded);
	HoldRenderDirtyHandle = UActorComponent::MarkRenderStateDirtyEvent.AddRaw(this, &FAnomaly_StuckLowMip::OnHoldRenderStateDirty);
	bHoldMonitorOn = true;
	UE_LOG(LogAnomaly, Log,
		TEXT("stuck_low_mip: HOLD MONITOR ON - %d existing component(s) recorded as already judged by the purity census ")
		TEXT("(%d still loading, judged when they finish). For the whole hold, a component that appears (spawned, ")
		TEXT("streamed in, newly created or registered: a per-tick diff against that set, because UE 5.1 has no global ")
		TEXT("component-registered delegate), a level ADDED to the world (FWorldDelegates::LevelAddedToWorld), or a ")
		TEXT("component whose render state is marked dirty (a material change) is checked; a new user of a held texture ")
		TEXT("that is not the target REVERTS THE HOLD IMMEDIATELY and flags the event contaminated. %d in-scope component(s) ")
		TEXT("were UNREGISTERED when judged; each is judged again the tick it registers, because registration creates its ")
		TEXT("render state without a dirty broadcast and a material set while it was unregistered is otherwise never seen."),
		HoldKnownComponents.Num(), Deferred, HoldUnregisteredWatch.Num());
}

void FAnomaly_StuckLowMip::StopHoldMonitor()
{
	if (HoldLevelAddedHandle.IsValid())
	{
		FWorldDelegates::LevelAddedToWorld.Remove(HoldLevelAddedHandle);
		HoldLevelAddedHandle.Reset();
	}
	if (HoldRenderDirtyHandle.IsValid())
	{
		UActorComponent::MarkRenderStateDirtyEvent.Remove(HoldRenderDirtyHandle);
		HoldRenderDirtyHandle.Reset();
	}
	bHoldMonitorOn = false;
	HoldKnownComponents.Reset();
	HoldUnregisteredWatch.Reset();
	FScopeLock Lock(&HoldDirtyCS);
	HoldDirtyRecheck.Reset();
}

void FAnomaly_StuckLowMip::OnHoldLevelAdded(ULevel* Level, UWorld* World)
{
	if (!bActive || !bHoldMonitorOn || !Level || World != HeldWorld.Get())
	{
		return;
	}
	TSet<const ULevel*> Loaded;
	GatherLoadedLevels(World, Loaded);
	for (AActor* Actor : Level->Actors)
	{
		if (!Actor)
		{
			continue;
		}
		TInlineComponentArray<UActorComponent*> Comps;
		Actor->GetComponents(Comps);
		for (UActorComponent* AC : Comps)
		{
			if (AC && ConsiderHoldUser(AC, Loaded, TEXT("level added to world")))
			{
				return;
			}
		}
	}
}

void FAnomaly_StuckLowMip::OnHoldRenderStateDirty(UActorComponent& Component)
{
	if (!bHoldMonitorOn)
	{
		return;
	}
	FScopeLock Lock(&HoldDirtyCS);
	HoldDirtyRecheck.Add(&Component);
}

bool FAnomaly_StuckLowMip::ConsiderHoldUser(UActorComponent* Component, const TSet<const ULevel*>& Loaded, const TCHAR* Trigger)
{
	if (!bActive || !Component)
	{
		return false;
	}
	const AnomalyStuckMipWindow::FComponentScope Scope = DescribeComponentScope(Component, Loaded);
	if (!AnomalyStuckMipWindow::InPurityScope(Scope))
	{
		return false;
	}
	TArray<UTexture*> Used;
	CollectAnyComponentTextures(Component, Used);
	const UTexture2D* HitTex = nullptr;
	for (UTexture* T : Used)
	{
		const UTexture2D* T2 = Cast<UTexture2D>(T);
		if (!T2)
		{
			continue;
		}
		for (const FHeldTexture& H : Held)
		{
			if (H.Texture.Get() == T2)
			{
				HitTex = T2;
				break;
			}
		}
		if (HitTex)
		{
			break;
		}
	}
	const AActor* Owner = Component->GetOwner();
	bool bOwnedByTarget = false;
	for (const TWeakObjectPtr<AActor>& Weak : HeldOwners)
	{
		if (Weak.Get() == Owner)
		{
			bOwnedByTarget = true;
			break;
		}
	}
	if (AnomalyStuckMipWindow::EvaluateNewHoldUser(Scope, HitTex != nullptr, bOwnedByTarget, bActive)
		!= AnomalyStuckMipWindow::EHoldUserAction::RevertNow)
	{
		return false;
	}
	++GStuckMipStats.HoldContaminations;
	bContaminationPending = true;
	ContaminationReason = FString::Printf(TEXT("new user %s.%s(%s%s%s) of held texture '%s' appeared mid-hold (%s)"),
		*GetNameSafe(Owner), *Component->GetName(), *Component->GetClass()->GetName(),
		Scope.bLevelActive ? TEXT("") : TEXT(",inactive_level"), Scope.bRegistered ? TEXT("") : TEXT(",unregistered"),
		*GetNameSafe(HitTex), Trigger);
	UE_LOG(LogAnomaly, Warning,
		TEXT("stuck_low_mip: HOLD CONTAMINATED - %s while '%s' holds it. The purity rule (exactly one user component) ")
		TEXT("no longer holds, so the blur now reaches an object the label and mask do not name. REVERTING THE HOLD NOW; ")
		TEXT("every labelled frame from here until the render record shows the texture back at baseline is flagged ")
		TEXT("stuck_mip.contaminated = 1."),
		*ContaminationReason, *PrimaryOwnerName);
	Revert();
	return true;
}

void FAnomaly_StuckLowMip::ScanHoldForNewUsers(const TCHAR* Trigger)
{
	UWorld* World = HeldWorld.Get();
	if (!bActive || !bHoldMonitorOn || !World)
	{
		return;
	}
	++GStuckMipStats.HoldMonitorScans;
	TSet<const ULevel*> Loaded;
	GatherLoadedLevels(World, Loaded);
	TArray<UActorComponent*> Fresh;
	int32 Walked = 0;
	auto Diff = [this, &Loaded, &Fresh, &Walked](UActorComponent* AC)
	{
		++Walked;
		if (!AC || IsComponentStillLoading(AC))
		{
			return;
		}
		const FObjectKey Key(AC);
		if (!HoldKnownComponents.Contains(Key) && SettleHoldComponent(AC, Loaded))
		{
			HoldKnownComponents.Add(Key);
			Fresh.Add(AC);
			if (!AC->IsRegistered() && AnomalyStuckMipWindow::InPurityScope(DescribeComponentScope(AC, Loaded)))
			{
				HoldUnregisteredWatch.Add(AC);
			}
		}
	};
	for (TObjectIterator<UPrimitiveComponent> It; It; ++It)
	{
		Diff(*It);
	}
	for (TObjectIterator<UDecalComponent> It; It; ++It)
	{
		Diff(*It);
	}
	GStuckMipStats.HoldMonitorComponentsWalkedMax = FMath::Max(GStuckMipStats.HoldMonitorComponentsWalkedMax, Walked);
	GStuckMipStats.HoldMonitorUnregisteredWatchedMax =
		FMath::Max(GStuckMipStats.HoldMonitorUnregisteredWatchedMax, HoldUnregisteredWatch.Num());
	for (UActorComponent* AC : Fresh)
	{
		if (ConsiderHoldUser(AC, Loaded, Trigger))
		{
			return;
		}
	}
	for (int32 i = HoldUnregisteredWatch.Num() - 1; i >= 0; --i)
	{
		UActorComponent* AC = HoldUnregisteredWatch[i].Get();
		if (!AC)
		{
			HoldUnregisteredWatch.RemoveAtSwap(i);
			continue;
		}
		if (!AnomalyStuckMipWindow::RejudgeOnRegistration(false, AC->IsRegistered()))
		{
			continue;
		}
		HoldUnregisteredWatch.RemoveAtSwap(i);
		++GStuckMipStats.HoldMonitorRegistrationRejudges;
		if (ConsiderHoldUser(AC, Loaded, TEXT("registered mid-hold (a material set while it was unregistered)")))
		{
			return;
		}
	}
	TArray<TWeakObjectPtr<UActorComponent>> Dirty;
	{
		FScopeLock Lock(&HoldDirtyCS);
		Dirty = MoveTemp(HoldDirtyRecheck);
		HoldDirtyRecheck.Reset();
	}
	TArray<TWeakObjectPtr<UActorComponent>> StillLoading;
	for (const TWeakObjectPtr<UActorComponent>& Weak : Dirty)
	{
		UActorComponent* AC = Weak.Get();
		if (!AC)
		{
			continue;
		}
		if (IsComponentStillLoading(AC))
		{
			StillLoading.Add(Weak);
			continue;
		}
		if (ConsiderHoldUser(AC, Loaded, TEXT("render state marked dirty")))
		{
			return;
		}
	}
	if (StillLoading.Num() > 0)
	{
		GStuckMipStats.HoldMonitorDirtyRequeued += StillLoading.Num();
		FScopeLock Lock(&HoldDirtyCS);
		HoldDirtyRecheck.Append(StillLoading);
	}
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
	Out.AddInt(TEXT("stuck_mip.forced_top_px"), P.TopResidentPxAtTarget);
	Out.AddFloat(TEXT("stuck_mip.ratio_at_pick"), (double)P.RatioAtPick);
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
		Rec.AddInt(TEXT("forced_top_px"), H.TopResidentPxAtTarget);
		Rec.AddFloat(TEXT("ratio_at_pick"), (double)H.RatioAtPick);
		Rec.AddInt(TEXT("resident_mips"), Now);
		Rec.AddInt(TEXT("resident_mips_at_onset"), H.ResidentAtOnset);
		Rec.AddInt(TEXT("co_affected_visible"), H.CoAffectedVisible);
		Rec.AddInt(TEXT("world_users"), H.WorldUsers);
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

#if !UE_BUILD_SHIPPING
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

static FAutoConsoleCommandWithWorldAndArgs GBenchStuckMipVirtualProbeCmd(
	TEXT("IAI.Bench.StuckMipVirtualProbe"),
	TEXT("BENCH DEVICE, console only, read-only - never in a client payload. Exercises the F-e / NOT_APPLICABLE ")
	TEXT("branch of stuck_low_mip's eligibility test, which no gate leg on either fixture has ever reached: ")
	TEXT("stuck_mip_refused_virtual has read 0 everywhere, and a 0 from a branch that was never reachable is ")
	TEXT("BLINDNESS, not a reading. It does three things and reports all three. (1) A CENSUS of every loaded ")
	TEXT("UTexture2D: how many serialise VirtualTextureStreaming=true, and how many answer TRUE to the runtime ")
	TEXT("predicate IsCurrentlyVirtualTextured(), which additionally requires cooked VT page data ")
	TEXT("(Texture2D.cpp:1219). (2) For every texture that IS currently virtual it runs the SHIPPED eligibility ")
	TEXT("classifier and asserts the verdict is NOT_APPLICABLE(virtual), incrementing the same counter a fire ")
	TEXT("would. (3) A SYNTHETIC CONTROL: a transient UTexture2D with VirtualTextureStreaming forced true, to show ")
	TEXT("whether the branch can be reached without cooked content at all. If no currently-virtual texture exists ")
	TEXT("the probe says the path is UNEXERCISED IN THIS FIXTURE and names why, rather than reporting a clean zero. ")
	TEXT("Usage: IAI.Bench.StuckMipVirtualProbe"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			int32 Total = 0;
			int32 FlagSet = 0;
			int32 RuntimeVirtual = 0;
			int32 ClassifiedVirtual = 0;
			int32 ClassifiedOther = 0;
			FString Names;

			for (TObjectIterator<UTexture2D> It; It; ++It)
			{
				UTexture2D* Tex = *It;
				if (!Tex || Tex->HasAnyFlags(RF_ClassDefaultObject))
				{
					continue;
				}
				++Total;
				if (Tex->VirtualTextureStreaming)
				{
					++FlagSet;
				}
				if (!Tex->IsCurrentlyVirtualTextured())
				{
					continue;
				}
				++RuntimeVirtual;

				const EEligibility Verdict = ClassifyTexture(Tex);
				if (Verdict == EEligibility::Virtual)
				{
					++ClassifiedVirtual;
					++GStuckMipStats.RefusedVirtual;
					if (Names.Len() < 400)
					{
						Names += FString::Printf(TEXT("'%s' "), *Tex->GetName());
					}
					UE_LOG(LogAnomaly, Warning,
						TEXT("stuck_low_mip: NOT_APPLICABLE '%s' - IsCurrentlyVirtualTextured() is TRUE, so it is not ")
						TEXT("in the render-asset streamer at all and GetNumResidentMips() returns a virtual-resource ")
						TEXT("constant rather than a streaming state. Counted in stuck_mip_refused_virtual by the ")
						TEXT("probe exactly as a fire would count it."),
						*Tex->GetName());
				}
				else
				{
					++ClassifiedOther;
					UE_LOG(LogAnomaly, Error,
						TEXT("stuck_low_mip: PROBE DISAGREEMENT '%s' - IsCurrentlyVirtualTextured() is TRUE but the ")
						TEXT("shipped classifier returned %d instead of Virtual. That is a defect in the classifier, ")
						TEXT("not in the probe, and it is reported rather than counted."),
						*Tex->GetName(), (int32)Verdict);
				}
			}

			UTexture2D* Synthetic = UTexture2D::CreateTransient(4, 4, PF_B8G8R8A8);
			bool bSyntheticVirtual = false;
			if (Synthetic)
			{
				Synthetic->VirtualTextureStreaming = true;
				bSyntheticVirtual = Synthetic->IsCurrentlyVirtualTextured();
			}

			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: VIRTUAL-TEXTURE PROBE CENSUS - %d loaded UTexture2D, %d serialise ")
				TEXT("VirtualTextureStreaming=true, %d answer TRUE to the RUNTIME predicate ")
				TEXT("IsCurrentlyVirtualTextured(). The two counts differ because the runtime predicate ALSO requires ")
				TEXT("cooked VT page data (Texture2D.cpp:1219: VirtualTextureStreaming && GetPlatformData() && ")
				TEXT("GetPlatformData()->VTData), and the per-texture runtime answer is what the anomaly reads - ")
				TEXT("never the project's r.VirtualTextures feature flag."),
				Total, FlagSet, RuntimeVirtual);

			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: VIRTUAL-TEXTURE PROBE SYNTHETIC CONTROL - a transient UTexture2D with ")
				TEXT("VirtualTextureStreaming forced true reads IsCurrentlyVirtualTextured() = %s. %s"),
				bSyntheticVirtual ? TEXT("TRUE") : TEXT("FALSE"),
				bSyntheticVirtual
					? TEXT("The branch is therefore reachable synthetically.")
					: TEXT("The branch is therefore NOT reachable synthetically, and the reason is structural rather ")
					  TEXT("than incidental: CreateTransient builds platform data with no VTData, which the runtime ")
					  TEXT("predicate requires. Only genuinely cooked virtual-textured content can reach it."));

			if (ClassifiedVirtual > 0)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("stuck_low_mip: VIRTUAL-TEXTURE PROBE VERDICT = EXERCISED. %d texture(s) reached the ")
					TEXT("NOT_APPLICABLE(virtual) branch and were counted: %s. stuck_mip_refused_virtual is now a ")
					TEXT("PROVEN counter on this fixture, so a zero from it on a capture leg means absence rather ")
					TEXT("than blindness."),
					ClassifiedVirtual, *Names);
			}
			else
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("stuck_low_mip: VIRTUAL-TEXTURE PROBE VERDICT = UNEXERCISED IN THIS FIXTURE. Of %d loaded ")
					TEXT("textures, %d carry the serialised flag and NONE answers TRUE at runtime (%d classifier ")
					TEXT("disagreements), and the synthetic control cannot reach the branch either. ")
					TEXT("stuck_mip_refused_virtual's zero on this host is therefore NOT a proven counter - it is a ")
					TEXT("branch that has never been reached, and it must be recorded as UNEXERCISED rather than as a ")
					TEXT("clean read. Reaching it needs a host whose cooked content contains a virtual-textured ")
					TEXT("material actually applied to a candidate target."),
					Total, FlagSet, ClassifiedOther);
			}
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
#endif
