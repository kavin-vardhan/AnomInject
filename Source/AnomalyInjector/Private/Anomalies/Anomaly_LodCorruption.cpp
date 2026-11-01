#include "Anomalies/Anomaly_LodCorruption.h"

#include "AnomalyLod.h"
#include "AnomalyArgs.h"
#include "AnomalyViewport.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "Components/MeshComponent.h"
#if ANOMALY_ENGINE_425
#include "Compat/AnomalyAutoLod425.h"
#endif

bool FAnomaly_LodCorruption::Apply(UWorld* World, const TArray<FString>& Args)
{
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

	for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
	{
		UMeshComponent* Mesh = Weak.Get();
		if (!Mesh)
		{
			continue;
		}

        if (UAnomalyInjectorSubsystem::IsAutoPoolSelection(World) && !AnomalyLod::HasMultipleLods(Mesh)) { continue; }
		const int32 NumLODs = AnomalyLod::GetWorstLod(Mesh);
		int32 Target = AnomalyLod::ResolveTargetLod(Mesh, RequestedIndex);
#if ANOMALY_ENGINE_425
        if (UAnomalyInjectorSubsystem::IsAutoPoolSelection(World) && !bHasExplicitIndex)
        {
            const auto Drawn = AnomalyLod::PredictDrawnLod425(World, Mesh);
            Target = Drawn.bKnown ? AnomalyAutoLod425::ForcedModel(NumLODs, Drawn.Level) : 0;
            UE_LOG(LogAnomaly, Log, TEXT("Auto.LodChoice component=%s drawn=%d count=%d forced=%d screen=%.9f source=%s"), *Mesh->GetName(), Drawn.Level, NumLODs, Target, Drawn.ScreenSize, Drawn.Source);
            if (Target == 0) { continue; }
        }
#endif

		FCapturedLod Record;
		Record.Mesh = Mesh;
		Record.PrevForcedLodModel = AnomalyLod::GetForcedLod(Mesh);
		Captured.Add(Record);

		AnomalyLod::SetForcedLod(Mesh, Target);
		UE_LOG(LogAnomaly, Log, TEXT("lod_corruption: '%s' forced LOD %d of %d [was %d]."),
			*Mesh->GetName(), Target, NumLODs, Record.PrevForcedLodModel);
	}

	bActive = Captured.Num() > 0;
	UE_LOG(LogAnomaly, Log, TEXT("lod_corruption: forced LOD on %d component(s) for '%s'."), Captured.Num(), *Substring);
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

	Captured.Reset();
	bActive = false;
}
