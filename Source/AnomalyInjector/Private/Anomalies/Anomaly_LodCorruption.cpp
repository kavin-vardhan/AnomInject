#include "Anomalies/Anomaly_LodCorruption.h"

#include "AnomalyLod.h"
#include "AnomalyArgs.h"
#include "AnomalyViewport.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "Components/MeshComponent.h"
#include "Components/SkinnedMeshComponent.h"

bool FAnomaly_LodCorruption::Apply(UWorld* World, const TArray<FString>& Args)
{
	LastRefusal.Reset();
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("lod_corruption: usage <substring> [lod-index]"));
		return false;
	}

	if (bActive)
	{
		Revert();
	}

	const FString& Substring = Args[0];

	const bool bHasExplicitIndex = Args.IsValidIndex(1);
	const int32 RequestedIndex = bHasExplicitIndex
		? AnomalyArgs::GetInt(Args, 1, 1, 1, 64)
		: AnomalyLod::WorstLodSentinel;

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
		UE_LOG(LogAnomaly, Log, TEXT("lod_corruption: matched 0 mesh component(s) for '%s'."), *Substring);
		return false;
	}

	int32 RefusedHism = 0;
	int32 RefusedSingleLod = 0;
	int32 RefusedSkeletal = 0;
	int32 RefusedNotCoarser = 0;
	int32 RefusedUndetermined = 0;
	for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
	{
		UMeshComponent* Mesh = Weak.Get();
		if (!Mesh)
		{
			continue;
		}

		const AnomalyLodPolicy::EForcedLodRefusal Refusal = AnomalyLod::GetForcedLodRefusal(Mesh);
		if (Refusal != AnomalyLodPolicy::EForcedLodRefusal::None)
		{
			const bool bHism = Refusal == AnomalyLodPolicy::EForcedLodRefusal::HierarchicalInstanced;
			const bool bSkeletal = Refusal == AnomalyLodPolicy::EForcedLodRefusal::SkeletalUnmeasured;
			++(bHism ? RefusedHism : bSkeletal ? RefusedSkeletal : RefusedSingleLod);
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_corruption: REFUSED '%s' (%s) — %s [%s]. This applies to every fire, auto-pool or targeted, with or without a lod-index."),
				*Mesh->GetName(), UTF8_TO_TCHAR(AnomalyLodPolicy::DescribeRefusal(Refusal)),
				bHism ? TEXT("it is a hierarchical instanced static mesh, which picks its own LODs and ignores a forced LOD, so forcing one changes nothing on screen: a positive label with no visible change, which the mask veto cannot catch (it still draws pixels)")
				: bSkeletal ? TEXT("it is a skeletal mesh, which applies a forced LOD one frame late, so the labels at both edges of the event may be one frame off the picture; that is unmeasured, so it is refused unless IAI.Anomaly.LodAllowSkeletal is 1")
				: TEXT("it has at most one usable LOD (its own LOD count, less any LODs below its MinLOD or stripped of vertices; the engine clamps a forced LOD to the usable range), so the forced LOD is the one it already shows: a positive label with no visible change, which the mask veto cannot catch (it still draws pixels)"),
				*AnomalyLod::DescribeLodFacts(AnomalyLod::GetLodFacts(Mesh)));
			continue;
		}

		const int32 NumLODs = AnomalyLod::GetWorstLod(Mesh);
		const int32 Target = AnomalyLod::ResolveTargetLod(Mesh, RequestedIndex);

		const AnomalyLod::FForcedLodContrast Contrast = AnomalyLod::GetForcedLodContrast(World, Mesh, Target);
		if (Contrast.Refusal != AnomalyLodPolicy::EForcedLodRefusal::None)
		{
			const bool bUndetermined = Contrast.Refusal == AnomalyLodPolicy::EForcedLodRefusal::CurrentLodUndetermined;
			++(bUndetermined ? RefusedUndetermined : RefusedNotCoarser);
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_corruption: REFUSED '%s' (%s) — forced_lod_model=%d resolves to LOD %d; the LOD it shows now is %s (source %s, screen_size %.6f) [%s]. %s This applies to every fire, auto-pool or targeted, with or without a lod-index."),
				*Mesh->GetName(), UTF8_TO_TCHAR(AnomalyLodPolicy::DescribeRefusal(Contrast.Refusal)), Target, Contrast.ForcedLod,
				Contrast.bCurrentKnown ? *FString::FromInt(Contrast.ShownLod) : TEXT("UNDETERMINED"), Contrast.Source, Contrast.ScreenSize,
				*AnomalyLod::DescribeLodFacts(AnomalyLod::GetLodFacts(Mesh)),
				bUndetermined
					? TEXT("Without the current LOD there is no way to know that forcing this one changes the picture, and a positive label with no visible change is the defect this guard exists to prevent, so it is refused rather than guessed.")
					: TEXT("The forced LOD is not coarser than the one already on screen, so forcing it changes nothing: a positive label with no visible change, which the mask veto cannot catch (it still draws pixels)."));
			continue;
		}

		if (Mesh->IsA<USkinnedMeshComponent>())
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_corruption: '%s' is a skeletal mesh, allowed by IAI.Anomaly.LodAllowSkeletal 1. Its forced LOD reaches the picture one frame late, so the labels at both edges of this event may be one frame off. This is UNMEASURED."),
				*Mesh->GetName());
		}
		UE_LOG(LogAnomaly, Log,
			TEXT("lod_corruption: CONTRAST '%s' shows LOD %d now (source %s, screen_size %.6f); forced_lod_model=%d resolves to LOD %d, which is coarser [%s]."),
			*Mesh->GetName(), Contrast.ShownLod, Contrast.Source, Contrast.ScreenSize, Target, Contrast.ForcedLod,
			*AnomalyLod::DescribeLodFacts(AnomalyLod::GetLodFacts(Mesh)));

		FCapturedLod Record;
		Record.Mesh = Mesh;
		Record.PrevForcedLodModel = AnomalyLod::GetForcedLod(Mesh);
		Captured.Add(Record);

		AnomalyLod::SetForcedLod(Mesh, Target);
		UE_LOG(LogAnomaly, Log, TEXT("lod_corruption: '%s' forced LOD %d of %d [was %d]."),
			*Mesh->GetName(), Target, NumLODs, Record.PrevForcedLodModel);
	}

	bActive = Captured.Num() > 0;
	if (!bActive)
	{
		int32 ByKind[AnomalyLodPolicy::NumRefusalKinds] = {};
		ByKind[(int32)AnomalyLodPolicy::EForcedLodRefusal::HierarchicalInstanced] = RefusedHism;
		ByKind[(int32)AnomalyLodPolicy::EForcedLodRefusal::SingleLod] = RefusedSingleLod;
		ByKind[(int32)AnomalyLodPolicy::EForcedLodRefusal::SkeletalUnmeasured] = RefusedSkeletal;
		ByKind[(int32)AnomalyLodPolicy::EForcedLodRefusal::NotCoarser] = RefusedNotCoarser;
		ByKind[(int32)AnomalyLodPolicy::EForcedLodRefusal::CurrentLodUndetermined] = RefusedUndetermined;
		LastRefusal = UTF8_TO_TCHAR(AnomalyLodPolicy::DominantRefusalOf(ByKind, AnomalyLodPolicy::NumRefusalKinds, 0));
		UE_LOG(LogAnomaly, Warning,
			TEXT("lod_corruption: matched %d component(s) for '%s' but REFUSED ALL — %d hierarchical-instanced, %d single-LOD, %d skeletal, %d not coarser than the current LOD, %d current LOD undetermined. Applying nothing, so no fire is recorded and no label is written."),
			Meshes.Num(), *Substring, RefusedHism, RefusedSingleLod, RefusedSkeletal, RefusedNotCoarser, RefusedUndetermined);
		return false;
	}
	TArray<UMeshComponent*> Forced;
	for (const FCapturedLod& Record : Captured)
	{
		if (UMeshComponent* Mesh = Record.Mesh.Get())
		{
			Forced.Add(Mesh);
		}
	}
	AnomalyLod::ScopeEventToForcedComponents(Forced, ScopedOwners, TEXT("lod_corruption"));
	UE_LOG(LogAnomaly, Log, TEXT("lod_corruption: forced LOD on %d component(s) for '%s' (%d refused hierarchical instanced, %d refused single LOD, %d refused skeletal, %d refused not coarser than current, %d refused current LOD undetermined)."),
		Captured.Num(), *Substring, RefusedHism, RefusedSingleLod, RefusedSkeletal, RefusedNotCoarser, RefusedUndetermined);
	return bActive;
}

void FAnomaly_LodCorruption::Revert()
{
	for (const FCapturedLod& Record : Captured)
	{
		UMeshComponent* Mesh = Record.Mesh.Get();
		if (!Mesh)
		{
			continue;
		}
		AnomalyLod::SetForcedLod(Mesh, Record.PrevForcedLodModel);
	}

	AnomalyLod::ClearForcedComponentScope(ScopedOwners);
	Captured.Reset();
	bActive = false;
}
