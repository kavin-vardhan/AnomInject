#include "Anomalies/Anomaly_LodPopping.h"

#include "AnomalyLod.h"
#include "AnomalyArgs.h"
#include "AnomalyViewport.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "Components/MeshComponent.h"

bool FAnomaly_LodPopping::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("lod_popping: usage <substring> [hz]"));
		return false;
	}

	if (bActive)
	{
		Revert();
	}

	const FString& Substring = Args[0];

	const float Hz = AnomalyArgs::GetFloat(Args, 1, DefaultHz, MinHz, MaxHz);
	HalfPeriodSeconds = 0.5f / Hz;

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
		UE_LOG(LogAnomaly, Log, TEXT("lod_popping: matched 0 mesh component(s) for '%s'."), *Substring);
		return false;
	}

	Targets.Reset();
	for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
	{
		UMeshComponent* Mesh = Weak.Get();
		if (!Mesh)
		{
			continue;
		}

		FPoppingTarget Target;
		Target.Mesh = Mesh;
		Target.BaselineLod = AnomalyLod::GetForcedLod(Mesh);
		Target.PoppedLod   = AnomalyLod::GetWorstLod(Mesh);
		Targets.Add(Target);
	}

	Accumulator = 0.0f;
	bPoppedPhase = false;
	bActive = Targets.Num() > 0;

	UE_LOG(LogAnomaly, Log, TEXT("lod_popping: matched %d component(s) for '%s' at %.2f Hz (half-period %.3fs)."),
		Targets.Num(), *Substring, Hz, HalfPeriodSeconds);
	return bActive;
}

void FAnomaly_LodPopping::Tick(float DeltaSeconds)
{
	if (!bActive)
	{
		return;
	}

	Accumulator += DeltaSeconds;
	while (Accumulator >= HalfPeriodSeconds)
	{
		Accumulator -= HalfPeriodSeconds;
		bPoppedPhase = !bPoppedPhase;

		int32 Affected = 0;
		for (const FPoppingTarget& Target : Targets)
		{
			if (UMeshComponent* Mesh = Target.Mesh.Get())
			{
				AnomalyLod::SetForcedLod(Mesh, bPoppedPhase ? Target.PoppedLod : Target.BaselineLod);
				++Affected;
			}
		}
		UE_LOG(LogAnomaly, Verbose, TEXT("lod_popping snap -> %s (%d components)."),
			bPoppedPhase ? TEXT("POPPED") : TEXT("BASELINE"), Affected);
	}
}

void FAnomaly_LodPopping::Revert()
{
	for (const FPoppingTarget& Target : Targets)
	{
		if (UMeshComponent* Mesh = Target.Mesh.Get())
		{
			AnomalyLod::SetForcedLod(Mesh, Target.BaselineLod);
		}
	}

	Targets.Reset();
	Accumulator = 0.0f;
	bPoppedPhase = false;
	bActive = false;
}
