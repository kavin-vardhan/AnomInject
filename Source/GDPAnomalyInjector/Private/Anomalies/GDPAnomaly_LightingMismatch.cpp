// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "Anomalies/GDPAnomaly_LightingMismatch.h"

#include "GDPTargeting.h"
#include "GDPArgs.h"
#include "GDPAnomalyInjectorLog.h"
#include "Components/LightComponent.h"

bool FGDPAnomaly_LightingMismatch::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogGDPAnomaly, Warning, TEXT("lighting_mismatch: usage <substring> [off | dim <f> | recolor <r g b> | noshadow]"));
		return false;
	}

	// Re-entrancy: revert-then-reapply so re-firing never leaks (single capture set; prior lights restored).
	if (bActive)
	{
		Revert();
	}

	const FString& Substring = Args[0];

	// Resolve the mode (default dim); unknown -> warn + fall back to dim.
	FString Mode = GDPArgs::GetString(Args, 1, TEXT("dim")).ToLower();
	if (Mode != TEXT("off") && Mode != TEXT("dim") && Mode != TEXT("recolor") && Mode != TEXT("noshadow"))
	{
		UE_LOG(LogGDPAnomaly, Warning, TEXT("lighting_mismatch: unknown mode '%s'; using 'dim'."), *Mode);
		Mode = TEXT("dim");
	}

	// Parse mode-specific args up front (clamped via A3).
	float DimFactor = 0.1f;
	FLinearColor RecolorColor(1.0f, 0.0f, 1.0f);   // magenta default
	if (Mode == TEXT("dim"))
	{
		DimFactor = GDPArgs::GetFloat(Args, 2, 0.1f, 0.0f, 1.0f);
	}
	else if (Mode == TEXT("recolor"))
	{
		const float R = GDPArgs::GetFloat(Args, 2, 1.0f, 0.0f, 1.0f);
		const float G = GDPArgs::GetFloat(Args, 3, 0.0f, 0.0f, 1.0f);
		const float B = GDPArgs::GetFloat(Args, 4, 1.0f, 0.0f, 1.0f);
		RecolorColor = FLinearColor(R, G, B);
	}

	const TArray<TWeakObjectPtr<ULightComponent>> Lights = GDPTargeting::FindComponentsMatching<ULightComponent>(World, Substring);
	if (Lights.Num() == 0)
	{
		UE_LOG(LogGDPAnomaly, Log, TEXT("lighting_mismatch: matched 0 light component(s) for '%s'."), *Substring);
		return false;   // AMB-2: zero match -> not applied / inactive
	}

	for (const TWeakObjectPtr<ULightComponent>& Weak : Lights)
	{
		ULightComponent* Light = Weak.Get();
		if (!Light)
		{
			continue;
		}

		// Capture exactly what we can mutate, BEFORE mutating.
		FCapturedLight Record;
		Record.Light = Light;
		Record.Intensity = Light->Intensity;
		Record.Color = Light->GetLightColor();
		Record.bVisible = Light->GetVisibleFlag();
		Record.bCastShadows = (Light->CastShadows != 0);
		Captured.Add(Record);

		if (Mode == TEXT("off"))
		{
			Light->SetVisibility(false);
		}
		else if (Mode == TEXT("recolor"))
		{
			Light->SetLightColor(RecolorColor);
		}
		else if (Mode == TEXT("noshadow"))
		{
			Light->SetCastShadows(false);
		}
		else   // dim
		{
			Light->SetIntensity(Record.Intensity * DimFactor);
		}
	}

	bActive = Captured.Num() > 0;
	UE_LOG(LogGDPAnomaly, Log, TEXT("lighting_mismatch: mode '%s' on %d light component(s) for '%s'."),
		*Mode, Captured.Num(), *Substring);
	return bActive;
}

void FGDPAnomaly_LightingMismatch::Revert()
{
	// Restore the full captured state per still-live component; skip stale ptrs (GC-safe).
	for (const FCapturedLight& Record : Captured)
	{
		ULightComponent* Light = Record.Light.Get();
		if (!Light)
		{
			continue;
		}
		Light->SetIntensity(Record.Intensity);
		Light->SetLightColor(Record.Color);
		Light->SetVisibility(Record.bVisible);
		Light->SetCastShadows(Record.bCastShadows);
	}

	Captured.Reset();
	bActive = false;
}
