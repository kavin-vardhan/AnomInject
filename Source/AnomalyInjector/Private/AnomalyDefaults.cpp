#include "AnomalyDefaults.h"

#include "AnomalyInjectorLog.h"
#include "HAL/IConsoleManager.h"
#include "Misc/ConfigCacheIni.h"

namespace
{
	struct FResolvedDefault
	{
		int32 Value = 0;
		const TCHAR* Source = TEXT("compiled");
	};

	TMap<FString, FResolvedDefault>& ResolvedCache()
	{
		static TMap<FString, FResolvedDefault> Cache;
		return Cache;
	}

	TMap<FString, int32>& OverrideMap()
	{
		static TMap<FString, int32> Overrides;
		return Overrides;
	}

	TArray<FString>& ExcludedOverride()
	{
		static TArray<FString> Patterns;
		return Patterns;
	}

	bool& ExcludedOverrideSet()
	{
		static bool bSet = false;
		return bSet;
	}

	float& MaxDistanceOverride()
	{
		static float Value = 0.0f;
		return Value;
	}

	bool& MaxDistanceOverrideSet()
	{
		static bool bSet = false;
		return bSet;
	}

	float& MinCoverageOverride()
	{
		static float Value = 0.0f;
		return Value;
	}

	bool& MinCoverageOverrideSet()
	{
		static bool bSet = false;
		return bSet;
	}

	float& TriggerRadiusOverride()
	{
		static float Value = 0.0f;
		return Value;
	}

	bool& TriggerRadiusOverrideSet()
	{
		static bool bSet = false;
		return bSet;
	}

	FResolvedDefault Resolve(const TCHAR* IniKey, int32 CompiledDefault, int32 MinFrames, int32 MaxFrames,
		const TCHAR* AnomalyName)
	{
		const FString CacheKey(IniKey);
		if (const int32* Override = OverrideMap().Find(CacheKey))
		{
			FResolvedDefault FromConsole;
			FromConsole.Value = *Override;
			FromConsole.Source = TEXT("console");
			return FromConsole;
		}
		if (const FResolvedDefault* Hit = ResolvedCache().Find(CacheKey))
		{
			return *Hit;
		}

		FResolvedDefault Out;
		Out.Value = CompiledDefault;
		Out.Source = TEXT("compiled");

		int32 FromIni = 0;
		if (GConfig && GConfig->GetInt(AnomalyDefaults::SectionName(), IniKey, FromIni, GGameIni))
		{
			if (FromIni < MinFrames || FromIni > MaxFrames)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("%s: DefaultGame.ini [%s] %s = %d is out of range [%d..%d]; using the COMPILED default %d. ")
					TEXT("The key is REFUSED rather than clamped, so a typo cannot quietly become a different cadence."),
					AnomalyName, AnomalyDefaults::SectionName(), IniKey, FromIni, MinFrames, MaxFrames, CompiledDefault);
			}
			else
			{
				Out.Value = FromIni;
				Out.Source = TEXT("ini");
				UE_LOG(LogAnomaly, Log,
					TEXT("%s: auto-pool half-period default = %d frames, from DefaultGame.ini [%s] %s. This is the ")
					TEXT("value an AUTO-POOL fire uses; a TARGETED fire's own argument still beats it."),
					AnomalyName, Out.Value, AnomalyDefaults::SectionName(), IniKey);
			}
		}
		else
		{
			UE_LOG(LogAnomaly, Log,
				TEXT("%s: auto-pool half-period default = %d frames, from the COMPILED DEFAULT; no [%s] %s key is ")
				TEXT("present, so behaviour is byte-identical to a build without this key."),
				AnomalyName, Out.Value, AnomalyDefaults::SectionName(), IniKey);
		}

		ResolvedCache().Add(CacheKey, Out);
		return Out;
	}
}

namespace AnomalyDefaults
{
	const TCHAR* SectionName()
	{
		return TEXT("AnomalyInjector");
	}

	const TCHAR* BlinkingHalfPeriodKey()
	{
		return TEXT("BlinkingHalfPeriodFramesDefault");
	}

	const TCHAR* LodPoppingHalfPeriodKey()
	{
		return TEXT("LodPoppingHalfPeriodFramesDefault");
	}

	FString DescribeBlinkingHalfPeriod()
	{
		return Describe(BlinkingHalfPeriodKey(), BlinkingHalfPeriodCompiled,
			HalfPeriodMin, HalfPeriodMax, TEXT("blinking"));
	}

	FString DescribeLodPoppingHalfPeriod()
	{
		return Describe(LodPoppingHalfPeriodKey(), LodPoppingHalfPeriodCompiled,
			HalfPeriodMin, HalfPeriodMax, TEXT("lod_popping"));
	}

	int32 GetHalfPeriodFrames(const TCHAR* IniKey, int32 CompiledDefault, int32 MinFrames, int32 MaxFrames,
		const TCHAR* AnomalyName)
	{
		return Resolve(IniKey, CompiledDefault, MinFrames, MaxFrames, AnomalyName).Value;
	}

	FString Describe(const TCHAR* IniKey, int32 CompiledDefault, int32 MinFrames, int32 MaxFrames,
		const TCHAR* AnomalyName)
	{
		const FResolvedDefault R = Resolve(IniKey, CompiledDefault, MinFrames, MaxFrames, AnomalyName);
		return FString::Printf(TEXT("%d(%s)"), R.Value, R.Source);
	}

	bool SetConsoleOverride(const TCHAR* IniKey, int32 Frames, int32 MinFrames, int32 MaxFrames,
		const TCHAR* AnomalyName)
	{
		if (Frames < MinFrames || Frames > MaxFrames)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("%s: half-period %d is out of range [%d..%d]; the console override is REFUSED and the ")
				TEXT("previous default still stands."),
				AnomalyName, Frames, MinFrames, MaxFrames);
			return false;
		}
		OverrideMap().Add(FString(IniKey), Frames);
		UE_LOG(LogAnomaly, Log,
			TEXT("%s: AUTO-POOL half-period default set to %d frames by console override. This BEATS ")
			TEXT("DefaultGame.ini [%s] %s, which matters because a loose ini beside a package is a no-op (G88) - ")
			TEXT("the cooked config wins, so on a packaged build this command is the ONLY way to change the ")
			TEXT("auto-pool cadence without a re-cook. A TARGETED fire's own argument still beats this."),
			AnomalyName, Frames, SectionName(), IniKey);
		return true;
	}

	void ClearConsoleOverride(const TCHAR* IniKey)
	{
		OverrideMap().Remove(FString(IniKey));
		UE_LOG(LogAnomaly, Log,
			TEXT("AnomalyDefaults: console override for %s cleared; the ini value or the compiled default ")
			TEXT("takes over again."), IniKey);
	}

	const TCHAR* ExcludedTargetPatternsKey()
	{
		return TEXT("ExcludedTargetNamePatterns");
	}

	const TArray<FString>& GetExcludedTargetPatterns()
	{
		if (ExcludedOverrideSet())
		{
			return ExcludedOverride();
		}

		static bool bResolved = false;
		static TArray<FString> Patterns;
		if (bResolved)
		{
			return Patterns;
		}
		bResolved = true;

		TArray<FString> Raw;
		if (GConfig)
		{
			GConfig->GetArray(SectionName(), ExcludedTargetPatternsKey(), Raw, GGameIni);
		}
		for (const FString& Entry : Raw)
		{
			const FString Trimmed = Entry.TrimStartAndEnd();
			if (!Trimmed.IsEmpty())
			{
				Patterns.Add(Trimmed);
			}
		}

		if (Patterns.Num() > 0)
		{
			UE_LOG(LogAnomaly, Log,
				TEXT("AnomalyInjector: target-exclusion patterns = %d, from DefaultGame.ini [%s] %s. A candidate is ")
				TEXT("REFUSED if any pattern is a case-insensitive substring of its ACTOR name, its COMPONENT name or ")
				TEXT("its MESH ASSET name. This is a LABEL-QUALITY exclusion at the same chokepoint as the foliage ")
				TEXT("exclusion, so it reaches the selector, the auto-injector and capture alike."),
				Patterns.Num(), SectionName(), ExcludedTargetPatternsKey());
		}
		else
		{
			UE_LOG(LogAnomaly, Log,
				TEXT("AnomalyInjector: target-exclusion patterns = NONE, from the COMPILED DEFAULT; no [%s] %s key is ")
				TEXT("present, so selection is byte-identical to a build without this feature."),
				SectionName(), ExcludedTargetPatternsKey());
		}
		return Patterns;
	}

	void SetExcludedTargetPatternsOverride(const TArray<FString>& Patterns)
	{
		ExcludedOverride().Reset();
		for (const FString& Entry : Patterns)
		{
			const FString Trimmed = Entry.TrimStartAndEnd();
			if (!Trimmed.IsEmpty())
			{
				ExcludedOverride().Add(Trimmed);
			}
		}
		ExcludedOverrideSet() = true;
		UE_LOG(LogAnomaly, Log,
			TEXT("AnomalyInjector: target-exclusion patterns SET BY CONSOLE to %d [%s]. This BEATS DefaultGame.ini ")
			TEXT("[%s] %s, which matters because a loose ini beside a package is a no-op (G88) - the cooked config ")
			TEXT("wins, so on a packaged build this command is the ONLY way to change the exclusion list without a ")
			TEXT("re-cook. It takes effect on the NEXT selection poll."),
			ExcludedOverride().Num(), *FString::Join(ExcludedOverride(), TEXT("|")),
			SectionName(), ExcludedTargetPatternsKey());
	}

	void ClearExcludedTargetPatternsOverride()
	{
		ExcludedOverride().Reset();
		ExcludedOverrideSet() = false;
		UE_LOG(LogAnomaly, Log,
			TEXT("AnomalyInjector: console target-exclusion override cleared; the ini list or the empty compiled ")
			TEXT("default takes over again."));
	}

	FString ExcludedTargetPatternsSource()
	{
		if (ExcludedOverrideSet())
		{
			return FString(TEXT("IAI.SetExcludedTargets (console override, beats the ini)"));
		}
		return FString::Printf(TEXT("DefaultGame.ini [%s] %s"), SectionName(), ExcludedTargetPatternsKey());
	}

	FString DescribeExcludedTargetPatterns()
	{
		const TArray<FString>& Patterns = GetExcludedTargetPatterns();
		const TCHAR* Source = ExcludedOverrideSet() ? TEXT("console") : TEXT("ini");
		if (Patterns.Num() == 0)
		{
			return ExcludedOverrideSet()
				? FString(TEXT("none(console)"))
				: FString(TEXT("none(COMPILED DEFAULT; no ini key)"));
		}
		return FString::Printf(TEXT("%d(%s)[%s]"), Patterns.Num(), Source, *FString::Join(Patterns, TEXT("|")));
	}

	const TCHAR* LodPoppingMaxDistanceKey()
	{
		return TEXT("LodPoppingMaxDistanceCm");
	}

	float GetLodPoppingMaxDistanceCm()
	{
		if (MaxDistanceOverrideSet())
		{
			return MaxDistanceOverride();
		}
		static bool bResolved = false;
		static float Value = LodPoppingMaxDistanceCompiled;
		static const TCHAR* Source = TEXT("compiled");
		if (bResolved)
		{
			return Value;
		}
		bResolved = true;

		float FromIni = 0.0f;
		if (GConfig && GConfig->GetFloat(SectionName(), LodPoppingMaxDistanceKey(), FromIni, GGameIni))
		{
			if (FromIni < MaxDistanceMin || FromIni > MaxDistanceMax)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("lod_popping: DefaultGame.ini [%s] %s = %.2f is out of range [%.0f..%.0f]; using the ")
					TEXT("COMPILED default %.2f cm. The key is REFUSED rather than clamped, so a typo cannot quietly ")
					TEXT("become a different reach."),
					SectionName(), LodPoppingMaxDistanceKey(), FromIni, MaxDistanceMin, MaxDistanceMax,
					LodPoppingMaxDistanceCompiled);
			}
			else
			{
				Value = FromIni;
				Source = TEXT("ini");
			}
		}

		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: proximity gate = %.2f cm (%s). Measured with the SAME metric as the poll radius ")
			TEXT("(sphere-approx bounds distance from ResolvePollOrigin). It ANDs with the %.4f%% screen-coverage ")
			TEXT("gate; it does not replace it. 0 disables the distance gate and leaves coverage alone."),
			Value, Source, 7.0f);
		return Value;
	}

	bool SetLodPoppingMaxDistanceOverride(float Cm)
	{
		if (Cm < MaxDistanceMin || Cm > MaxDistanceMax)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: proximity maximum %.2f is out of range [%.0f..%.0f]; the console override is ")
				TEXT("REFUSED and the previous value still stands."), Cm, MaxDistanceMin, MaxDistanceMax);
			return false;
		}
		MaxDistanceOverride() = Cm;
		MaxDistanceOverrideSet() = true;
		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: proximity maximum set to %.2f cm by console override. This BEATS DefaultGame.ini ")
			TEXT("[%s] %s (G88: a loose ini beside a package is a no-op). 0 disables the DISTANCE gate only - the ")
			TEXT("calibrated screen-coverage gate is untouched and still applies."),
			Cm, SectionName(), LodPoppingMaxDistanceKey());
		return true;
	}

	void ClearLodPoppingMaxDistanceOverride()
	{
		MaxDistanceOverrideSet() = false;
		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: console proximity override cleared; the ini value or the compiled default takes over."));
	}

	const TCHAR* LodPoppingMinCoverageKey()
	{
		return TEXT("LodPoppingMinCoveragePct");
	}

	float GetLodPoppingMinCoveragePct()
	{
		if (MinCoverageOverrideSet())
		{
			return MinCoverageOverride();
		}
		static bool bResolved = false;
		static float Value = LodPoppingMinCoverageCompiled;
		static const TCHAR* Source = TEXT("compiled");
		if (bResolved)
		{
			return Value;
		}
		bResolved = true;

		float FromIni = 0.0f;
		if (GConfig && GConfig->GetFloat(SectionName(), LodPoppingMinCoverageKey(), FromIni, GGameIni))
		{
			if (FromIni < MinCoverageMin || FromIni > MinCoverageMax)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("lod_popping: DefaultGame.ini [%s] %s = %.4f is out of range [%.0f..%.0f]; using the ")
					TEXT("COMPILED default %.4f. The key is REFUSED rather than clamped, so a typo cannot quietly ")
					TEXT("discard a calibrated threshold."),
					SectionName(), LodPoppingMinCoverageKey(), FromIni, MinCoverageMin, MinCoverageMax,
					LodPoppingMinCoverageCompiled);
			}
			else
			{
				Value = FromIni;
				Source = TEXT("ini");
			}
		}

		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: screen-coverage gate = %.4f%% (%s). THE COMPILED DEFAULT %.4f IS A MEASURED NUMBER, ")
			TEXT("not a preference: m30 calibrated it against last-visible %.4f%% and first-invisible %.4f%%, biased ")
			TEXT("toward REFUSING because a positive label with no visible change is the dataset-poisoning direction. ")
			TEXT("Tuning it at runtime is an operator decision; a different value is NOT a re-calibration. It gates ")
			TEXT("AUTO-POOL selection only, and 0 disables the coverage gate alone."),
			Value, Source, LodPoppingMinCoverageCompiled, 9.3453f, 3.9045f);
		return Value;
	}

	bool SetLodPoppingMinCoverageOverride(float Pct)
	{
		if (Pct < MinCoverageMin || Pct > MinCoverageMax)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("lod_popping: screen-coverage minimum %.4f is out of range [%.0f..%.0f]; the console override is ")
				TEXT("REFUSED and the previous value still stands."), Pct, MinCoverageMin, MinCoverageMax);
			return false;
		}
		MinCoverageOverride() = Pct;
		MinCoverageOverrideSet() = true;
		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: screen-coverage minimum set to %.4f%% by console override (compiled default %.4f is the ")
			TEXT("m30-CALIBRATED value and is unchanged). This BEATS DefaultGame.ini [%s] %s (G88: a loose ini beside ")
			TEXT("a package is a no-op). It gates AUTO-POOL selection only; a targeted fire already bypasses it."),
			Pct, LodPoppingMinCoverageCompiled, SectionName(), LodPoppingMinCoverageKey());
		return true;
	}

	void ClearLodPoppingMinCoverageOverride()
	{
		MinCoverageOverrideSet() = false;
		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: console screen-coverage override cleared; the ini value or the m30-calibrated compiled ")
			TEXT("default takes over again."));
	}

	FString DescribeLodPoppingMinCoverage()
	{
		const float V = GetLodPoppingMinCoveragePct();
		const TCHAR* Src = TEXT("compiled");
		if (MinCoverageOverrideSet())
		{
			Src = TEXT("console");
		}
		else
		{
			float FromIni = 0.0f;
			if (GConfig && GConfig->GetFloat(SectionName(), LodPoppingMinCoverageKey(), FromIni, GGameIni)
				&& FromIni >= MinCoverageMin && FromIni <= MinCoverageMax)
			{
				Src = TEXT("ini");
			}
		}
		return FString::Printf(TEXT("%.4f%%(%s)"), V, Src);
	}

	const TCHAR* CameraClippingTriggerRadiusKey()
	{
		return TEXT("CameraClippingTriggerRadiusCm");
	}

	float GetCameraClippingTriggerRadiusCm()
	{
		if (TriggerRadiusOverrideSet())
		{
			return TriggerRadiusOverride();
		}
		static bool bResolved = false;
		static float Value = CameraClippingTriggerRadiusCompiled;
		static const TCHAR* Source = TEXT("compiled");
		if (bResolved)
		{
			return Value;
		}
		bResolved = true;

		float FromIni = 0.0f;
		if (GConfig && GConfig->GetFloat(SectionName(), CameraClippingTriggerRadiusKey(), FromIni, GGameIni))
		{
			if (FromIni < TriggerRadiusMin || FromIni > TriggerRadiusMax)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("camera_clipping: DefaultGame.ini [%s] %s = %.2f is out of range [%.0f..%.0f]; using the ")
					TEXT("COMPILED default %.2f cm. The key is REFUSED rather than clamped, so a typo cannot quietly ")
					TEXT("become a different trigger reach. Note 0 is REFUSED on purpose - a zero radius would never ")
					TEXT("fire and would look like a working configuration."),
					SectionName(), CameraClippingTriggerRadiusKey(), FromIni, TriggerRadiusMin, TriggerRadiusMax,
					CameraClippingTriggerRadiusCompiled);
			}
			else
			{
				Value = FromIni;
				Source = TEXT("ini");
			}
		}

		UE_LOG(LogAnomaly, Log,
			TEXT("camera_clipping: TARGETED trigger radius = %.2f cm (%s). Measured with the SAME metric as the poll ")
			TEXT("radius and the lod_popping proximity gate (sphere-approx bounds distance from ResolvePollOrigin), so ")
			TEXT("all three numbers are directly comparable. It applies ONLY to a targeted camera_clipping fire; the ")
			TEXT("session-global path is untouched by it. The compiled default deliberately matches the lod_popping ")
			TEXT("proximity default so the product carries ONE 'right next to the player' distance, not two."),
			Value, Source);
		return Value;
	}

	bool SetCameraClippingTriggerRadiusOverride(float Cm)
	{
		if (Cm < TriggerRadiusMin || Cm > TriggerRadiusMax)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("camera_clipping: trigger radius %.2f is out of range [%.0f..%.0f]; the console override is ")
				TEXT("REFUSED and the previous value still stands."), Cm, TriggerRadiusMin, TriggerRadiusMax);
			return false;
		}
		TriggerRadiusOverride() = Cm;
		TriggerRadiusOverrideSet() = true;
		UE_LOG(LogAnomaly, Log,
			TEXT("camera_clipping: TARGETED trigger radius set to %.2f cm by console override. This BEATS ")
			TEXT("DefaultGame.ini [%s] %s (G88: a loose ini beside a package is a no-op). It takes effect on the NEXT ")
			TEXT("targeted fire; a fire already live keeps the radius it was applied with."),
			Cm, SectionName(), CameraClippingTriggerRadiusKey());
		return true;
	}

	void ClearCameraClippingTriggerRadiusOverride()
	{
		TriggerRadiusOverrideSet() = false;
		UE_LOG(LogAnomaly, Log,
			TEXT("camera_clipping: console trigger-radius override cleared; the ini value or the compiled default takes over."));
	}

	FString DescribeCameraClippingTriggerRadius()
	{
		const float V = GetCameraClippingTriggerRadiusCm();
		const TCHAR* Src = TEXT("compiled");
		if (TriggerRadiusOverrideSet())
		{
			Src = TEXT("console");
		}
		else
		{
			float FromIni = 0.0f;
			if (GConfig && GConfig->GetFloat(SectionName(), CameraClippingTriggerRadiusKey(), FromIni, GGameIni)
				&& FromIni >= TriggerRadiusMin && FromIni <= TriggerRadiusMax)
			{
				Src = TEXT("ini");
			}
		}
		return FString::Printf(TEXT("%.0fcm(%s)"), V, Src);
	}

	FString DescribeLodPoppingMaxDistance()
	{
		const float V = GetLodPoppingMaxDistanceCm();
		const TCHAR* Src = TEXT("compiled");
		if (MaxDistanceOverrideSet())
		{
			Src = TEXT("console");
		}
		else
		{
			float FromIni = 0.0f;
			if (GConfig && GConfig->GetFloat(SectionName(), LodPoppingMaxDistanceKey(), FromIni, GGameIni)
				&& FromIni >= MaxDistanceMin && FromIni <= MaxDistanceMax)
			{
				Src = TEXT("ini");
			}
		}
		return FString::Printf(TEXT("%.0fcm(%s)"), V, Src);
	}
}

namespace
{
	void HandleHalfPeriodCommand(const TArray<FString>& Args, const TCHAR* CommandName, const TCHAR* IniKey,
		int32 CompiledDefault, const TCHAR* AnomalyName)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: %s <frames|default>  (current: %s)"),
				CommandName,
				*AnomalyDefaults::Describe(IniKey, CompiledDefault,
					AnomalyDefaults::HalfPeriodMin, AnomalyDefaults::HalfPeriodMax, AnomalyName));
			return;
		}
		if (Args[0].Equals(TEXT("default"), ESearchCase::IgnoreCase))
		{
			AnomalyDefaults::ClearConsoleOverride(IniKey);
		}
		else if (Args[0].IsNumeric())
		{
			AnomalyDefaults::SetConsoleOverride(IniKey, FCString::Atoi(*Args[0]),
				AnomalyDefaults::HalfPeriodMin, AnomalyDefaults::HalfPeriodMax, AnomalyName);
		}
		else
		{
			UE_LOG(LogAnomaly, Warning, TEXT("%s: '%s' is not a whole number of frames (or 'default')."),
				CommandName, *Args[0]);
			return;
		}
		UE_LOG(LogAnomaly, Log, TEXT("%s: EFFECTIVE READ-BACK = %s."), CommandName,
			*AnomalyDefaults::Describe(IniKey, CompiledDefault,
				AnomalyDefaults::HalfPeriodMin, AnomalyDefaults::HalfPeriodMax, AnomalyName));
	}

	void HandleBlinkHalfPeriod(const TArray<FString>& Args)
	{
		HandleHalfPeriodCommand(Args, TEXT("IAI.Anomaly.BlinkHalfPeriod"),
			AnomalyDefaults::BlinkingHalfPeriodKey(), AnomalyDefaults::BlinkingHalfPeriodCompiled,
			TEXT("blinking"));
	}

	void HandleLodHalfPeriod(const TArray<FString>& Args)
	{
		HandleHalfPeriodCommand(Args, TEXT("IAI.Anomaly.LodHalfPeriod"),
			AnomalyDefaults::LodPoppingHalfPeriodKey(), AnomalyDefaults::LodPoppingHalfPeriodCompiled,
			TEXT("lod_popping"));
	}
}

namespace
{
	void HandleExcludedTargets(const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.SetExcludedTargets <pattern> [pattern...] | clear   (current: %s)"),
				*AnomalyDefaults::DescribeExcludedTargetPatterns());
			return;
		}
		if (Args.Num() == 1 && Args[0].Equals(TEXT("clear"), ESearchCase::IgnoreCase))
		{
			AnomalyDefaults::ClearExcludedTargetPatternsOverride();
		}
		else
		{
			AnomalyDefaults::SetExcludedTargetPatternsOverride(Args);
		}
		UE_LOG(LogAnomaly, Log, TEXT("IAI.SetExcludedTargets: EFFECTIVE READ-BACK = %s."),
			*AnomalyDefaults::DescribeExcludedTargetPatterns());
	}
}

namespace
{
	void HandleLodMaxDistance(const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Anomaly.LodMaxDistance <cm|default>  (current: %s)"),
				*AnomalyDefaults::DescribeLodPoppingMaxDistance());
			return;
		}
		if (Args[0].Equals(TEXT("default"), ESearchCase::IgnoreCase))
		{
			AnomalyDefaults::ClearLodPoppingMaxDistanceOverride();
		}
		else if (Args[0].IsNumeric())
		{
			AnomalyDefaults::SetLodPoppingMaxDistanceOverride(FCString::Atof(*Args[0]));
		}
		else
		{
			UE_LOG(LogAnomaly, Warning, TEXT("IAI.Anomaly.LodMaxDistance: '%s' is not a number of cm (or 'default')."),
				*Args[0]);
			return;
		}
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Anomaly.LodMaxDistance: EFFECTIVE READ-BACK = %s."),
			*AnomalyDefaults::DescribeLodPoppingMaxDistance());
	}

	void HandleLodMinCoverage(const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Anomaly.LodMinCoverage <pct|default>  (current: %s)"),
				*AnomalyDefaults::DescribeLodPoppingMinCoverage());
			return;
		}
		if (Args[0].Equals(TEXT("default"), ESearchCase::IgnoreCase))
		{
			AnomalyDefaults::ClearLodPoppingMinCoverageOverride();
		}
		else if (Args[0].IsNumeric())
		{
			AnomalyDefaults::SetLodPoppingMinCoverageOverride(FCString::Atof(*Args[0]));
		}
		else
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Anomaly.LodMinCoverage: '%s' is not a percentage (or 'default')."), *Args[0]);
			return;
		}
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Anomaly.LodMinCoverage: EFFECTIVE READ-BACK = %s."),
			*AnomalyDefaults::DescribeLodPoppingMinCoverage());
	}

	void HandleCameraClipTriggerRadius(const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Anomaly.CameraClipTriggerRadius <cm|default>  (current: %s)"),
				*AnomalyDefaults::DescribeCameraClippingTriggerRadius());
			return;
		}
		if (Args[0].Equals(TEXT("default"), ESearchCase::IgnoreCase))
		{
			AnomalyDefaults::ClearCameraClippingTriggerRadiusOverride();
		}
		else if (Args[0].IsNumeric())
		{
			AnomalyDefaults::SetCameraClippingTriggerRadiusOverride(FCString::Atof(*Args[0]));
		}
		else
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Anomaly.CameraClipTriggerRadius: '%s' is not a number of cm (or 'default')."), *Args[0]);
			return;
		}
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Anomaly.CameraClipTriggerRadius: EFFECTIVE READ-BACK = %s."),
			*AnomalyDefaults::DescribeCameraClippingTriggerRadius());
	}
}

static FAutoConsoleCommand GLodMinCoverageCmd(
	TEXT("IAI.Anomaly.LodMinCoverage"),
	TEXT("Set the lod_popping SCREEN-COVERAGE minimum, in PERCENT of frame, for AUTO-POOL selection. A candidate is "
	     "refused if its bounds-projected screen coverage at pick time is below this. PRECEDENCE: console beats "
	     "DefaultGame.ini [AnomalyInjector] LodPoppingMinCoveragePct, which beats the compiled default 7.0. Range "
	     "[0..100]; out of range is REFUSED, never clamped. 0 disables the COVERAGE gate only - the distance gate and "
	     "Usage: IAI.Anomaly.LodMinCoverage <pct|default>"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleLodMinCoverage));

static FAutoConsoleCommand GCameraClipTriggerRadiusCmd(
	TEXT("IAI.Anomaly.CameraClipTriggerRadius"),
	TEXT("Set the TARGETED camera_clipping trigger radius, in CM. It applies ONLY when camera_clipping is fired on a "
	     "named object (IAI.Capture.Start ... camera_clipping =<ActorName>, or IAI.Apply camera_clipping =<ActorName>); "
	     "the session-global whole-run behaviour is untouched and byte-identical when the targeted mode is unused. Per "
	     "tick the near plane goes anomalous while the player is within this distance of the target and is restored "
	     "when the player leaves, so the rest of the scene is not spuriously clipped. The metric is the SAME one the "
	     "poll radius and the lod_popping proximity gate use (sphere-approx bounds distance from the poll origin), so "
	     "the three numbers are directly comparable. PRECEDENCE: console beats DefaultGame.ini [AnomalyInjector] "
	     "CameraClippingTriggerRadiusCm, which beats the compiled default 200 - deliberately the same number as the "
	     "lod_popping proximity default, because the product should carry ONE 'right next to the player' distance. "
	     "Range [1..1000000]; out of range is REFUSED, never clamped, and 0 is out of range on purpose because a zero "
	     "radius would never fire while looking like a working configuration. LABELLING IS UNCHANGED: a frame counts "
	     "positive only when the near plane is anomalous AND geometry is actually within the near-clip radius, so if "
	     "the player never approaches, the event carries zero positives and the m23 F-LABEL guard reports it. Pass "
	     "'default' to clear the override. Usage: IAI.Anomaly.CameraClipTriggerRadius <cm|default>"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleCameraClipTriggerRadius));

static FAutoConsoleCommand GLodMaxDistanceCmd(
	TEXT("IAI.Anomaly.LodMaxDistance"),
	TEXT("Set the lod_popping PROXIMITY maximum, in CM. A candidate is refused if its distance from the poll origin "
	     "exceeds this. The metric is the SAME one the poll radius uses (sphere-approx bounds distance), so the two "
	     "are directly comparable. It ANDs with the calibrated 7.0% screen-coverage gate and does NOT replace it: the "
	     "coverage gate was calibrated against measured visibility (last visible 9.3453%, first invisible 3.9045%), "
	     "whereas this distance is an owner PRODUCT PREFERENCE - removing a calibrated gate to install an "
	     "uncalibrated one would be backwards. PRECEDENCE: console beats DefaultGame.ini [AnomalyInjector] "
	     "LodPoppingMaxDistanceCm, which beats the compiled default 200. 0 disables the DISTANCE gate only. Range "
	     "[0..1000000]; out of range is REFUSED, never clamped. Pass 'default' to clear the override. "
	     "Usage: IAI.Anomaly.LodMaxDistance <cm|default>"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleLodMaxDistance));

static FAutoConsoleCommand GExcludedTargetsCmd(
	TEXT("IAI.SetExcludedTargets"),
	TEXT("Set the target-exclusion patterns for this session. A candidate is REFUSED at the shared renderable "
	     "chokepoint if any pattern is a case-insensitive SUBSTRING of its ACTOR name, its COMPONENT name or its "
	     "MESH ASSET name, so the exclusion reaches the selector, the auto-injector and capture alike. PRECEDENCE: "
	     "this console list beats DefaultGame.ini [AnomalyInjector] ExcludedTargetNamePatterns, which beats the "
	     "COMPILED DEFAULT of an EMPTY list. The console form exists for the same reason as IAI.Anomaly.BlinkHalfPeriod: "
	     "a loose ini beside a package is a NO-OP (G88), so on a packaged client build this is the only way to change "
	     "the list without a re-cook. This is a LABEL-QUALITY exclusion, not a claim the anomaly would not occur - it "
	     "removes objects whose anomaly a viewer cannot see, so their label would point at nothing. Every excluded "
	     "actor is logged ONCE per run as EXCLUDED-TARGET naming the pattern and the field that matched, and the "
	     "per-run count reaches run_summary.json as pattern_excluded_targets. Pass 'clear' to drop the override. "
	     "Usage: IAI.SetExcludedTargets <pattern> [pattern...] | clear"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleExcludedTargets));

static FAutoConsoleCommand GBlinkHalfPeriodCmd(
	TEXT("IAI.Anomaly.BlinkHalfPeriod"),
	TEXT("Set the AUTO-POOL default half-period for blinking, in FRAMES. PRECEDENCE: a TARGETED fire's own "
	     "argument beats this; this beats DefaultGame.ini [AnomalyInjector] BlinkingHalfPeriodFramesDefault; "
	     "that beats the compiled default 3. The console form exists because a loose ini beside a package is a "
	     "NO-OP (G88) - the cooked config wins - so on a packaged client build this is the ONLY way to change the "
	     "auto-pool cadence without a re-cook, and the client ships an AUTO-POOL config. Range [1..600]; an "
	     "out-of-range value is REFUSED, never clamped, so a typo cannot quietly become a different cadence. Pass "
	     "'default' to clear the override. The effective value prints on the run-config line at "
	     "IAI.Capture.Start. NO VALUE IS CHANGED BY THIS COMMAND EXISTING - absent override and absent ini key "
	     "mean the compiled default, byte-identical to a build without it. "
	     "Usage: IAI.Anomaly.BlinkHalfPeriod <frames|default>"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleBlinkHalfPeriod));

static FAutoConsoleCommand GLodHalfPeriodCmd(
	TEXT("IAI.Anomaly.LodHalfPeriod"),
	TEXT("Set the AUTO-POOL default half-period for lod_popping, in FRAMES. Same precedence, same range [1..600], "
	     "same refuse-never-clamp rule and the same G88 reasoning as IAI.Anomaly.BlinkHalfPeriod; the compiled "
	     "default is 8. Pass 'default' to clear the override. "
	     "Usage: IAI.Anomaly.LodHalfPeriod <frames|default>"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleLodHalfPeriod));
