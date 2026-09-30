#include "AnomalyDefaults.h"

#include "AnomalyInjectorLog.h"
#include "AnomalyViewport.h"
#include "HAL/IConsoleManager.h"
#include "Misc/ConfigCacheIni.h"

namespace
{
	TArray<FString> ReadExcludedTargetPatternsIni()
	{
		TArray<FString> Raw;
		TArray<FString> Patterns;
		if (GConfig)
		{
			GConfig->GetArray(AnomalyDefaults::SectionName(), AnomalyDefaults::ExcludedTargetPatternsKey(), Raw, GGameIni);
		}
		for (const FString& Entry : Raw)
		{
			const FString Trimmed = Entry.TrimStartAndEnd();
			if (!Trimmed.IsEmpty())
			{
				Patterns.Add(Trimmed);
			}
		}
		return Patterns;
	}

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

	int32& StuckMipLevelsOverride()
	{
		static int32 Value = 0;
		return Value;
	}

	bool& StuckMipLevelsOverrideSet()
	{
		static bool bSet = false;
		return bSet;
	}

	int32& StuckMipRestoreTimeoutOverride()
	{
		static int32 Value = AnomalyDefaults::StuckMipRestoreTimeoutCompiled;
		return Value;
	}

	bool& StuckMipRestoreTimeoutOverrideSet()
	{
		static bool bSet = false;
		return bSet;
	}

	int32& StuckMipMaxCoAffectedOverride()
	{
		static int32 Value = 0;
		return Value;
	}

	bool& StuckMipMaxCoAffectedOverrideSet()
	{
		static bool bSet = false;
		return bSet;
	}

	float& StuckMipMinTexelRatioOverride()
	{
		static float Value = 0.0f;
		return Value;
	}

	bool& StuckMipMinTexelRatioOverrideSet()
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

	bool& RequireHighestLodOverride()
	{
		static bool bValue = true;
		return bValue;
	}

	bool& RequireHighestLodOverrideSet()
	{
		static bool bSet = false;
		return bSet;
	}

	bool& AllowTranslucentOnlyOverride()
	{
		static bool bValue = false;
		return bValue;
	}

	bool& AllowTranslucentOnlyOverrideSet()
	{
		static bool bSet = false;
		return bSet;
	}

	bool& AllowNaniteOverride()
	{
		static bool bValue = false;
		return bValue;
	}

	bool& AllowNaniteOverrideSet()
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
		if (AnomalyViewport::IsReadOnlyEnumeration())
		{
			static TArray<FString> QuietPatterns;
			QuietPatterns = ReadExcludedTargetPatternsIni();
			return QuietPatterns;
		}
		bResolved = true;
		Patterns = ReadExcludedTargetPatternsIni();

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

	const TCHAR* StuckMipLevelsKey()
	{
		return TEXT("StuckMipLevels");
	}

	int32 GetStuckMipLevels()
	{
		if (StuckMipLevelsOverrideSet())
		{
			return StuckMipLevelsOverride();
		}
		static bool bResolved = false;
		static int32 Value = StuckMipLevelsCompiled;
		static const TCHAR* Source = TEXT("compiled");
		if (bResolved)
		{
			return Value;
		}
		bResolved = true;

		int32 FromIni = 0;
		if (GConfig && GConfig->GetInt(SectionName(), StuckMipLevelsKey(), FromIni, GGameIni))
		{
			if (FromIni < StuckMipLevelsMin || FromIni > StuckMipLevelsMax)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("stuck_low_mip: DefaultGame.ini [%s] %s = %d is out of range [%d..%d]; using the COMPILED ")
					TEXT("default %d. The key is REFUSED rather than clamped, so a typo cannot quietly become a ")
					TEXT("different blur depth."),
					SectionName(), StuckMipLevelsKey(), FromIni, StuckMipLevelsMin, StuckMipLevelsMax,
					StuckMipLevelsCompiled);
			}
			else
			{
				Value = FromIni;
				Source = TEXT("ini");
			}
		}

		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: mip levels to drop = %d (%s). -1 means HOLD AT THE FLOOR, i.e. the resource's own ")
			TEXT("NumNonStreamingLODs, which is the blurriest state the streamer can reach and is READ from the ")
			TEXT("texture rather than assumed."),
			Value, Source);
		return Value;
	}

	FString DescribeStuckMipLevels()
	{
		const int32 V = GetStuckMipLevels();
		const TCHAR* Source = StuckMipLevelsOverrideSet() ? TEXT("console") : TEXT("ini-or-compiled");
		return FString::Printf(TEXT("%d(%s)"), V, Source);
	}

	bool SetStuckMipLevelsOverride(int32 Levels)
	{
		if (Levels < StuckMipLevelsMin || Levels > StuckMipLevelsMax)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: mip levels %d is out of range [%d..%d]; the console override is REFUSED and the ")
				TEXT("previous value still stands."),
				Levels, StuckMipLevelsMin, StuckMipLevelsMax);
			return false;
		}
		StuckMipLevelsOverride() = Levels;
		StuckMipLevelsOverrideSet() = true;
		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: mip levels to drop set to %d by console override. This BEATS DefaultGame.ini [%s] %s ")
			TEXT("(G88: a loose ini beside a package is a no-op)."),
			Levels, SectionName(), StuckMipLevelsKey());
		return true;
	}

	void ClearStuckMipLevelsOverride()
	{
		StuckMipLevelsOverrideSet() = false;
		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: console mip-levels override cleared; the ini value or the compiled default takes over."));
	}

	const TCHAR* StuckMipMaxCoAffectedKey()
	{
		return TEXT("StuckMipMaxCoAffected");
	}

	int32 GetStuckMipMaxCoAffected()
	{
		if (StuckMipMaxCoAffectedOverrideSet())
		{
			return StuckMipMaxCoAffectedOverride();
		}
		static bool bResolved = false;
		static int32 Value = StuckMipMaxCoAffectedCompiled;
		static const TCHAR* Source = TEXT("compiled");
		if (bResolved)
		{
			return Value;
		}
		bResolved = true;

		int32 FromIni = 0;
		if (GConfig && GConfig->GetInt(SectionName(), StuckMipMaxCoAffectedKey(), FromIni, GGameIni))
		{
			if (FromIni < StuckMipMaxCoAffectedMin || FromIni > StuckMipMaxCoAffectedMax)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("stuck_low_mip: DefaultGame.ini [%s] %s = %d is out of range [%d..%d]; using the COMPILED ")
					TEXT("default %d. REFUSED rather than clamped."),
					SectionName(), StuckMipMaxCoAffectedKey(), FromIni, StuckMipMaxCoAffectedMin,
					StuckMipMaxCoAffectedMax, StuckMipMaxCoAffectedCompiled);
			}
			else
			{
				Value = FromIni;
				Source = TEXT("ini");
			}
		}

		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: co-affected maximum = %d other VISIBLE component(s) per texture (%s). A texture ")
			TEXT("sampled by more than this is REFUSED, PER TEXTURE, so the target's other textures stay eligible. 0 ")
			TEXT("is the only value that guarantees no unlabelled blurry object in a labelled frame."),
			Value, Source);
		return Value;
	}

	FString DescribeStuckMipMaxCoAffected()
	{
		const int32 V = GetStuckMipMaxCoAffected();
		const TCHAR* Source = StuckMipMaxCoAffectedOverrideSet() ? TEXT("console") : TEXT("ini-or-compiled");
		return FString::Printf(TEXT("%d(%s)"), V, Source);
	}

	bool SetStuckMipMaxCoAffectedOverride(int32 Max)
	{
		if (Max < StuckMipMaxCoAffectedMin || Max > StuckMipMaxCoAffectedMax)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: co-affected maximum %d is out of range [%d..%d]; the console override is REFUSED ")
				TEXT("and the previous value still stands."),
				Max, StuckMipMaxCoAffectedMin, StuckMipMaxCoAffectedMax);
			return false;
		}
		StuckMipMaxCoAffectedOverride() = Max;
		StuckMipMaxCoAffectedOverrideSet() = true;
		UE_LOG(LogAnomaly, Warning,
			TEXT("stuck_low_mip: co-affected maximum set to %d by console override. RAISING THIS ABOVE 0 ADMITS ")
			TEXT("EVENTS WHOSE FRAMES CONTAIN BLURRY OBJECTS THE LABEL DOES NOT NAME. That is a dataset decision, ")
			TEXT("not a tuning knob, and it BEATS DefaultGame.ini [%s] %s."),
			Max, SectionName(), StuckMipMaxCoAffectedKey());
		return true;
	}

	void ClearStuckMipMaxCoAffectedOverride()
	{
		StuckMipMaxCoAffectedOverrideSet() = false;
		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: console co-affected override cleared; the ini value or the compiled default 0 takes over."));
	}

	const TCHAR* StuckMipRestoreTimeoutKey()
	{
		return TEXT("StuckMipRestoreTimeoutFrames");
	}

	int32 GetStuckMipRestoreTimeout()
	{
		if (StuckMipRestoreTimeoutOverrideSet())
		{
			return StuckMipRestoreTimeoutOverride();
		}
		static bool bResolved = false;
		static int32 Value = StuckMipRestoreTimeoutCompiled;
		static const TCHAR* Source = TEXT("compiled");
		if (bResolved)
		{
			return Value;
		}
		bResolved = true;

		int32 FromIni = 0;
		if (GConfig && GConfig->GetInt(SectionName(), StuckMipRestoreTimeoutKey(), FromIni, GGameIni))
		{
			if (FromIni < StuckMipRestoreTimeoutMin || FromIni > StuckMipRestoreTimeoutMax)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("stuck_low_mip: DefaultGame.ini [%s] %s = %d is out of range [%d..%d]; using the COMPILED ")
					TEXT("default %d. REFUSED rather than clamped."),
					SectionName(), StuckMipRestoreTimeoutKey(), FromIni, StuckMipRestoreTimeoutMin,
					StuckMipRestoreTimeoutMax, StuckMipRestoreTimeoutCompiled);
			}
			else
			{
				Value = FromIni;
				Source = TEXT("ini");
			}
		}

		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: restore timeout = %d frame(s) (%s). The revert re-asserts the stream-in every frame ")
			TEXT("until the resident mip count reaches the recorded baseline; past this many frames the texture is ")
			TEXT("COUNTED as a restore timeout and named in the log, and it STAYS TRACKED so a later fire on it is ")
			TEXT("refused as not_restored rather than silently producing nothing."),
			Value, Source);
		return Value;
	}

	FString DescribeStuckMipRestoreTimeout()
	{
		const int32 V = GetStuckMipRestoreTimeout();
		const TCHAR* Source = StuckMipRestoreTimeoutOverrideSet() ? TEXT("console") : TEXT("ini-or-compiled");
		return FString::Printf(TEXT("%d(%s)"), V, Source);
	}

	bool SetStuckMipRestoreTimeoutOverride(int32 Frames)
	{
		if (Frames < StuckMipRestoreTimeoutMin || Frames > StuckMipRestoreTimeoutMax)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: restore timeout %d is out of range [%d..%d]; the console override is REFUSED and ")
				TEXT("the previous value still stands."),
				Frames, StuckMipRestoreTimeoutMin, StuckMipRestoreTimeoutMax);
			return false;
		}
		StuckMipRestoreTimeoutOverride() = Frames;
		StuckMipRestoreTimeoutOverrideSet() = true;
		UE_LOG(LogAnomaly, Warning,
			TEXT("stuck_low_mip: restore timeout set to %d frame(s) by console override. This changes ONLY when the ")
			TEXT("timeout is COUNTED AND LOGGED - polling continues either way until the baseline is reached - and it ")
			TEXT("BEATS DefaultGame.ini [%s] %s."),
			Frames, SectionName(), StuckMipRestoreTimeoutKey());
		return true;
	}

	void ClearStuckMipRestoreTimeoutOverride()
	{
		StuckMipRestoreTimeoutOverrideSet() = false;
		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: console restore-timeout override cleared; the ini value or the compiled default %d ")
			TEXT("takes over."),
			StuckMipRestoreTimeoutCompiled);
	}

	const TCHAR* StuckMipMinTexelRatioKey()
	{
		return TEXT("StuckMipMinTexelRatio");
	}

	float GetStuckMipMinTexelRatio()
	{
		if (StuckMipMinTexelRatioOverrideSet())
		{
			return StuckMipMinTexelRatioOverride();
		}
		static bool bResolved = false;
		static float Value = StuckMipMinTexelRatioCompiled;
		static const TCHAR* Source = TEXT("compiled");
		if (bResolved)
		{
			return Value;
		}
		bResolved = true;

		float FromIni = 0.0f;
		if (GConfig && GConfig->GetFloat(SectionName(), StuckMipMinTexelRatioKey(), FromIni, GGameIni))
		{
			if (FromIni < StuckMipMinTexelRatioMin || FromIni > StuckMipMinTexelRatioMax)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("stuck_low_mip: DefaultGame.ini [%s] %s = %.2f is out of range [%.0f..%.0f]; using the ")
					TEXT("COMPILED default %.2f. REFUSED rather than clamped."),
					SectionName(), StuckMipMinTexelRatioKey(), FromIni, StuckMipMinTexelRatioMin,
					StuckMipMinTexelRatioMax, StuckMipMinTexelRatioCompiled);
			}
			else
			{
				Value = FromIni;
				Source = TEXT("ini");
			}
		}

		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: perceptibility ratio = %.2f (%s). The target's longest on-screen side must be at ")
			TEXT("least this many times the held mip's width in pixels, or the object would not LOOK blurry. The ")
			TEXT("compiled default was raised 4.00 -> 8.00 for m52 because two auto-pool events at ratio 6.27 were ")
			TEXT("read by the label-vs-pixel verifier as NO-TRACE: labelled, held, observable, and carrying no change ")
			TEXT("above the noise floor. IT REMAINS A PICK-TIME FILTER ONLY and never decides observable: it assumes ")
			TEXT("the texture maps roughly once across the object, which a tiling texture does not. 0 disables it."),
			Value, Source);
		return Value;
	}

	FString DescribeStuckMipMinTexelRatio()
	{
		const float V = GetStuckMipMinTexelRatio();
		const TCHAR* Source = StuckMipMinTexelRatioOverrideSet() ? TEXT("console") : TEXT("ini-or-compiled");
		return FString::Printf(TEXT("%.2f(%s)"), V, Source);
	}

	bool SetStuckMipMinTexelRatioOverride(float Ratio)
	{
		if (Ratio < StuckMipMinTexelRatioMin || Ratio > StuckMipMinTexelRatioMax)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("stuck_low_mip: perceptibility ratio %.2f is out of range [%.0f..%.0f]; the console override is ")
				TEXT("REFUSED and the previous value still stands."),
				Ratio, StuckMipMinTexelRatioMin, StuckMipMinTexelRatioMax);
			return false;
		}
		StuckMipMinTexelRatioOverride() = Ratio;
		StuckMipMinTexelRatioOverrideSet() = true;
		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: perceptibility ratio set to %.2f by console override. This BEATS DefaultGame.ini ")
			TEXT("[%s] %s. 0 disables the perceptibility gate only; the co-affected gate is untouched."),
			Ratio, SectionName(), StuckMipMinTexelRatioKey());
		return true;
	}

	void ClearStuckMipMinTexelRatioOverride()
	{
		StuckMipMinTexelRatioOverrideSet() = false;
		UE_LOG(LogAnomaly, Log,
			TEXT("stuck_low_mip: console perceptibility override cleared; the ini value or the compiled default takes over."));
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

	const TCHAR* LodPoppingRequireHighestLodKey()
	{
		return TEXT("LodPoppingRequireHighestLod");
	}

	bool GetLodPoppingRequireHighestLod()
	{
		if (RequireHighestLodOverrideSet())
		{
			return RequireHighestLodOverride();
		}
		static bool bResolved = false;
		static bool bValue = LodPoppingRequireHighestLodCompiled;
		static const TCHAR* Source = TEXT("compiled");
		if (bResolved)
		{
			return bValue;
		}
		bResolved = true;

		bool bFromIni = false;
		if (GConfig && GConfig->GetBool(SectionName(), LodPoppingRequireHighestLodKey(), bFromIni, GGameIni))
		{
			bValue = bFromIni;
			Source = TEXT("ini");
		}

		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: highest-LOD requirement = %s (%s). The anomaly's visible magnitude is the CONTRAST ")
			TEXT("between the LOD an object is CURRENTLY rendering and the one forced onto it, so an object already ")
			TEXT("at a reduced LOD pops to something close to itself. Distance and screen coverage are proxies for ")
			TEXT("that contrast; this is the contrast itself. AUTO-POOL selection only - a targeted fire warns and ")
			TEXT("fires anyway."),
			bValue ? TEXT("ON") : TEXT("off"), Source);
		return bValue;
	}

	void SetLodPoppingRequireHighestLodOverride(bool bRequire)
	{
		RequireHighestLodOverride() = bRequire;
		RequireHighestLodOverrideSet() = true;
		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: highest-LOD requirement set to %s by console override. This BEATS DefaultGame.ini [%s] ")
			TEXT("%s (G88). Turning it OFF restores the pre-change auto-pool candidate set exactly."),
			bRequire ? TEXT("ON") : TEXT("off"), SectionName(), LodPoppingRequireHighestLodKey());
	}

	void ClearLodPoppingRequireHighestLodOverride()
	{
		RequireHighestLodOverrideSet() = false;
		UE_LOG(LogAnomaly, Log,
			TEXT("lod_popping: console highest-LOD override cleared; the ini value or the compiled default takes over."));
	}

	FString DescribeLodPoppingRequireHighestLod()
	{
		const bool bV = GetLodPoppingRequireHighestLod();
		const TCHAR* Src = TEXT("compiled");
		if (RequireHighestLodOverrideSet())
		{
			Src = TEXT("console");
		}
		else
		{
			bool bFromIni = false;
			if (GConfig && GConfig->GetBool(SectionName(), LodPoppingRequireHighestLodKey(), bFromIni, GGameIni))
			{
				Src = TEXT("ini");
			}
		}
		return FString::Printf(TEXT("%s(%s)"), bV ? TEXT("on") : TEXT("off"), Src);
	}

	const TCHAR* AllowTranslucentOnlyTargetsKey()
	{
		return TEXT("AllowTranslucentOnlyTargets");
	}

	bool GetAllowTranslucentOnlyTargets()
	{
		if (AllowTranslucentOnlyOverrideSet())
		{
			return AllowTranslucentOnlyOverride();
		}
		static bool bResolved = false;
		static bool bValue = AllowTranslucentOnlyTargetsCompiled;
		static const TCHAR* Source = TEXT("compiled");
		if (bResolved)
		{
			return bValue;
		}
		bool bFromIni = false;
		const bool bHasIni = GConfig && GConfig->GetBool(SectionName(), AllowTranslucentOnlyTargetsKey(), bFromIni, GGameIni);
		if (AnomalyViewport::IsReadOnlyEnumeration())
		{
			return bHasIni ? bFromIni : AllowTranslucentOnlyTargetsCompiled;
		}
		bResolved = true;

		if (bHasIni)
		{
			bValue = bFromIni;
			Source = TEXT("ini");
		}

		UE_LOG(LogAnomaly, Log,
			TEXT("selection: translucent-only targets are %s (%s). An actor whose every renderable-visible component ")
			TEXT("draws ONLY through translucent material slots writes no depth and no custom depth, so the target ")
			TEXT("mask cannot measure it: target_pixels reads -1 and observable reads null on every frame of its ")
			TEXT("event, which m49 then reports as observability_measured=false. Excluding it at the PICKER is the ")
			TEXT("only place the dataset never gains the event at all. This mirrors the census's own translucent ")
			TEXT("rule and shares its predicate. ALLOWING them restores the pre-m49-A2 candidate set exactly - a ")
			TEXT("G140 boundary: the same seed picks different targets across this change."),
			bValue ? TEXT("ALLOWED") : TEXT("EXCLUDED"), Source);
		return bValue;
	}

	void SetAllowTranslucentOnlyTargetsOverride(bool bAllow)
	{
		AllowTranslucentOnlyOverride() = bAllow;
		AllowTranslucentOnlyOverrideSet() = true;
		UE_LOG(LogAnomaly, Log,
			TEXT("selection: translucent-only targets set to %s by console override. This BEATS DefaultGame.ini [%s] ")
			TEXT("%s (G88). Setting it ALLOWED restores the pre-m49-A2 candidate set exactly."),
			bAllow ? TEXT("ALLOWED") : TEXT("EXCLUDED"), SectionName(), AllowTranslucentOnlyTargetsKey());
	}

	void ClearAllowTranslucentOnlyTargetsOverride()
	{
		AllowTranslucentOnlyOverrideSet() = false;
		UE_LOG(LogAnomaly, Log,
			TEXT("selection: console translucent-only override cleared; the ini value or the compiled default takes over."));
	}

	FString DescribeAllowTranslucentOnlyTargets()
	{
		const bool bV = GetAllowTranslucentOnlyTargets();
		const TCHAR* Src = TEXT("compiled");
		if (AllowTranslucentOnlyOverrideSet())
		{
			Src = TEXT("console");
		}
		else
		{
			bool bFromIni = false;
			if (GConfig && GConfig->GetBool(SectionName(), AllowTranslucentOnlyTargetsKey(), bFromIni, GGameIni))
			{
				Src = TEXT("ini");
			}
		}
		return FString::Printf(TEXT("%s(%s)"), bV ? TEXT("allowed") : TEXT("excluded"), Src);
	}

	const TCHAR* AllowNaniteTargetsKey()
	{
		return TEXT("AllowNaniteTargets");
	}

	bool GetAllowNaniteTargets()
	{
		if (AllowNaniteOverrideSet())
		{
			return AllowNaniteOverride();
		}
		static bool bResolved = false;
		static bool bValue = AllowNaniteTargetsCompiled;
		static const TCHAR* Source = TEXT("compiled");
		if (bResolved)
		{
			return bValue;
		}
		bool bFromIni = false;
		const bool bHasIni = GConfig && GConfig->GetBool(SectionName(), AllowNaniteTargetsKey(), bFromIni, GGameIni);
		if (AnomalyViewport::IsReadOnlyEnumeration())
		{
			return bHasIni ? bFromIni : AllowNaniteTargetsCompiled;
		}
		bResolved = true;

		if (bHasIni)
		{
			bValue = bFromIni;
			Source = TEXT("ini");
		}

		UE_LOG(LogAnomaly, Log,
			TEXT("selection: Nanite targets are %s (%s). On UE 5.1 a Nanite primitive never writes custom depth, so an ")
			TEXT("event on a target that draws any Nanite primitive cannot carry a pixel mask for that part. REFUSED ")
			TEXT("(the default) skips such a target before apply - in the auto-pool, a targeted fire and ApplyAnomaly - as ")
			TEXT("nanite_unmaskable, so every labelled event has a real mask; the cost is fewer eligible objects on ")
			TEXT("Nanite-heavy content. ALLOWED restores the previous contract exactly (label and box, no mask, ")
			TEXT("observability_measured false). A G140 boundary: the same seed picks different targets across a change ")
			TEXT("of this setting when a Nanite target is on screen."),
			bValue ? TEXT("ALLOWED") : TEXT("REFUSED"), Source);
		return bValue;
	}

	void SetAllowNaniteTargetsOverride(bool bAllow)
	{
		AllowNaniteOverride() = bAllow;
		AllowNaniteOverrideSet() = true;
		UE_LOG(LogAnomaly, Log,
			TEXT("selection: Nanite targets set to %s by console override. This BEATS DefaultGame.ini [%s] %s (G88)."),
			bAllow ? TEXT("ALLOWED") : TEXT("REFUSED"), SectionName(), AllowNaniteTargetsKey());
	}

	void ClearAllowNaniteTargetsOverride()
	{
		AllowNaniteOverrideSet() = false;
		UE_LOG(LogAnomaly, Log,
			TEXT("selection: console Nanite-target override cleared; the ini value or the compiled default takes over."));
	}

	FString DescribeAllowNaniteTargets()
	{
		const bool bV = GetAllowNaniteTargets();
		const TCHAR* Src = TEXT("compiled");
		if (AllowNaniteOverrideSet())
		{
			Src = TEXT("console");
		}
		else
		{
			bool bFromIni = false;
			if (GConfig && GConfig->GetBool(SectionName(), AllowNaniteTargetsKey(), bFromIni, GGameIni))
			{
				Src = TEXT("ini");
			}
		}
		return FString::Printf(TEXT("%s(%s)"), bV ? TEXT("allowed") : TEXT("refused"), Src);
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

	void HandleLodRequireHighestLod(const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Anomaly.LodRequireHighestLod <0|1|default>  (current: %s)"),
				*AnomalyDefaults::DescribeLodPoppingRequireHighestLod());
			return;
		}
		if (Args[0].Equals(TEXT("default"), ESearchCase::IgnoreCase))
		{
			AnomalyDefaults::ClearLodPoppingRequireHighestLodOverride();
		}
		else if (Args[0].IsNumeric())
		{
			AnomalyDefaults::SetLodPoppingRequireHighestLodOverride(FCString::Atoi(*Args[0]) != 0);
		}
		else
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Anomaly.LodRequireHighestLod: '%s' is not 0, 1 or 'default'."), *Args[0]);
			return;
		}
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Anomaly.LodRequireHighestLod: EFFECTIVE READ-BACK = %s."),
			*AnomalyDefaults::DescribeLodPoppingRequireHighestLod());
	}

	void HandleAllowTranslucentOnlyTargets(const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("Usage: IAI.Select.AllowTranslucentOnlyTargets <0|1|default>  (current: %s)"),
				*AnomalyDefaults::DescribeAllowTranslucentOnlyTargets());
			return;
		}
		if (Args[0].Equals(TEXT("default"), ESearchCase::IgnoreCase))
		{
			AnomalyDefaults::ClearAllowTranslucentOnlyTargetsOverride();
		}
		else if (Args[0].IsNumeric())
		{
			AnomalyDefaults::SetAllowTranslucentOnlyTargetsOverride(FCString::Atoi(*Args[0]) != 0);
		}
		else
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Select.AllowTranslucentOnlyTargets: '%s' is not 0, 1 or 'default'."), *Args[0]);
			return;
		}
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Select.AllowTranslucentOnlyTargets: EFFECTIVE READ-BACK = %s."),
			*AnomalyDefaults::DescribeAllowTranslucentOnlyTargets());
	}

	void HandleAllowNaniteTargets(const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("Usage: IAI.Targets.AllowNanite <0|1|default>  (current: %s)"),
				*AnomalyDefaults::DescribeAllowNaniteTargets());
			return;
		}
		if (Args[0].Equals(TEXT("default"), ESearchCase::IgnoreCase))
		{
			AnomalyDefaults::ClearAllowNaniteTargetsOverride();
		}
		else if (Args[0] == TEXT("0") || Args[0] == TEXT("1"))
		{
			AnomalyDefaults::SetAllowNaniteTargetsOverride(Args[0] == TEXT("1"));
		}
		else
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Targets.AllowNanite: '%s' is not 0, 1 or 'default'; nothing changed."), *Args[0]);
			return;
		}
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Targets.AllowNanite: EFFECTIVE READ-BACK = %s."),
			*AnomalyDefaults::DescribeAllowNaniteTargets());
	}

	void HandleStuckMipLevels(const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Anomaly.StuckMipLevels <levels|-1|default>  (current: %s)"),
				*AnomalyDefaults::DescribeStuckMipLevels());
			return;
		}
		if (Args[0].Equals(TEXT("default"), ESearchCase::IgnoreCase))
		{
			AnomalyDefaults::ClearStuckMipLevelsOverride();
		}
		else if (Args[0].IsNumeric())
		{
			AnomalyDefaults::SetStuckMipLevelsOverride(FCString::Atoi(*Args[0]));
		}
		else
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Anomaly.StuckMipLevels: '%s' is not a whole number of mip levels (or 'default')."), *Args[0]);
			return;
		}
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Anomaly.StuckMipLevels: EFFECTIVE READ-BACK = %s."),
			*AnomalyDefaults::DescribeStuckMipLevels());
	}

	void HandleStuckMipMaxCoAffected(const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Anomaly.StuckMipMaxCoAffected <n|default>  (current: %s)"),
				*AnomalyDefaults::DescribeStuckMipMaxCoAffected());
			return;
		}
		if (Args[0].Equals(TEXT("default"), ESearchCase::IgnoreCase))
		{
			AnomalyDefaults::ClearStuckMipMaxCoAffectedOverride();
		}
		else if (Args[0].IsNumeric())
		{
			AnomalyDefaults::SetStuckMipMaxCoAffectedOverride(FCString::Atoi(*Args[0]));
		}
		else
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Anomaly.StuckMipMaxCoAffected: '%s' is not a whole number (or 'default')."), *Args[0]);
			return;
		}
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Anomaly.StuckMipMaxCoAffected: EFFECTIVE READ-BACK = %s."),
			*AnomalyDefaults::DescribeStuckMipMaxCoAffected());
	}

	void HandleStuckMipRestoreTimeout(const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Anomaly.StuckMipRestoreTimeout <frames|default>  (current: %s)"),
				*AnomalyDefaults::DescribeStuckMipRestoreTimeout());
			return;
		}
		if (Args[0].Equals(TEXT("default"), ESearchCase::IgnoreCase))
		{
			AnomalyDefaults::ClearStuckMipRestoreTimeoutOverride();
		}
		else if (Args[0].IsNumeric())
		{
			AnomalyDefaults::SetStuckMipRestoreTimeoutOverride(FCString::Atoi(*Args[0]));
		}
		else
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Anomaly.StuckMipRestoreTimeout: '%s' is not a whole number (or 'default')."), *Args[0]);
			return;
		}
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Anomaly.StuckMipRestoreTimeout: EFFECTIVE READ-BACK = %s."),
			*AnomalyDefaults::DescribeStuckMipRestoreTimeout());
	}

	void HandleStuckMipMinTexelRatio(const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Anomaly.StuckMipMinTexelRatio <ratio|default>  (current: %s)"),
				*AnomalyDefaults::DescribeStuckMipMinTexelRatio());
			return;
		}
		if (Args[0].Equals(TEXT("default"), ESearchCase::IgnoreCase))
		{
			AnomalyDefaults::ClearStuckMipMinTexelRatioOverride();
		}
		else if (Args[0].IsNumeric())
		{
			AnomalyDefaults::SetStuckMipMinTexelRatioOverride(FCString::Atof(*Args[0]));
		}
		else
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Anomaly.StuckMipMinTexelRatio: '%s' is not a ratio (or 'default')."), *Args[0]);
			return;
		}
		UE_LOG(LogAnomaly, Log, TEXT("IAI.Anomaly.StuckMipMinTexelRatio: EFFECTIVE READ-BACK = %s."),
			*AnomalyDefaults::DescribeStuckMipMinTexelRatio());
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

static FAutoConsoleCommand GStuckMipLevelsCmd(
	TEXT("IAI.Anomaly.StuckMipLevels"),
	TEXT("Set how many mip levels stuck_low_mip drops below the target texture's CURRENT resident count. -1 (the "
	     "compiled default) means HOLD AT THE FLOOR - the resource's own NumNonStreamingLODs, read from the texture "
	     "and never assumed - which is the blurriest state the streamer can reach; on a cooked streaming texture that "
	     "is typically 7 resident mips, i.e. a 64 px top level. The hold is applied by raising the texture's own "
	     "NumCinematicMipLevels and calling UpdateCachedLODBias(), which lowers MaxAllowedMips so THE STREAMER "
	     "PERFORMS THE STREAM-OUT ITSELF and then holds it there; nothing races us, because RequestedMips equals "
	     "WantedMips. PRECEDENCE: console beats DefaultGame.ini [AnomalyInjector] StuckMipLevels, which beats the "
	     "compiled default. Range [-1..15]; out of range is REFUSED, never clamped. A targeted fire's own second "
	     "argument still beats this. Pass 'default' to clear the override. "
	     "Usage: IAI.Anomaly.StuckMipLevels <levels|-1|default>"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleStuckMipLevels));

static FAutoConsoleCommand GStuckMipMaxCoAffectedCmd(
	TEXT("IAI.Anomaly.StuckMipMaxCoAffected"),
	TEXT("Set how many OTHER currently-VISIBLE primitive components may sample a texture before stuck_low_mip "
	     "refuses to hold it. COMPILED DEFAULT 0, and 0 is the only value that guarantees no unlabelled blurry object "
	     "in a labelled frame: a texture is an ASSET, so holding it blurs every component that samples it while the "
	     "label names one target, and an unlabelled blurry object teaches a model that blurry is normal - worse than "
	     "a missing label. THE GATE IS PER TEXTURE, NOT PER TARGET: a target whose distinctive textures are exclusive "
	     "still fires with its shared utility textures left alone, which is why this is not simply an eligibility "
	     "filter. MEASURED on the bench fixtures at plan time: level-wide, 0 of 18 StackOBot targets have every "
	     "texture exclusive while 7 have a least-shared texture of at most 1, and 4 of 5 Lyra targets are at most 1. "
	     "AUTO-POOL SELECTION ONLY - a targeted fire on a named object is never blocked. PRECEDENCE: console beats "
	     "DefaultGame.ini [AnomalyInjector] StuckMipMaxCoAffected, which beats the compiled default 0. Range "
	     "[0..4096]; out of range is REFUSED. Usage: IAI.Anomaly.StuckMipMaxCoAffected <n|default>"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleStuckMipMaxCoAffected));

static FAutoConsoleCommand GStuckMipRestoreTimeoutCmd(
	TEXT("IAI.Anomaly.StuckMipRestoreTimeout"),
	TEXT("Set how many frames stuck_low_mip keeps re-asserting a reverted texture's stream-in before it COUNTS AND "
	     "NAMES a restore timeout. COMPILED DEFAULT 120. The revert clears the streaming bias and then polls every "
	     "frame until the resident mip count reaches the baseline recorded at Apply, re-issuing StreamIn on each "
	     "poll the engine is not already busy with - because a single request at revert time is SILENTLY DROPPED "
	     "when a stream operation is already in flight, which is the likely state right after a bias change and is "
	     "the measured cause of a later fire on the same target finding nothing left to hold. THE TIMEOUT DOES NOT "
	     "STOP THE POLLING: the texture stays tracked until it actually reaches its baseline, and while it is "
	     "tracked any fire on a target using it is refused as not_restored rather than producing an event with no "
	     "frames. PRECEDENCE: console beats DefaultGame.ini [AnomalyInjector] StuckMipRestoreTimeoutFrames, which "
	     "beats the compiled default. Range [1..100000]; out of range is REFUSED. "
	     "Usage: IAI.Anomaly.StuckMipRestoreTimeout <frames|default>"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleStuckMipRestoreTimeout));

static FAutoConsoleCommand GStuckMipMinTexelRatioCmd(
	TEXT("IAI.Anomaly.StuckMipMinTexelRatio"),
	TEXT("Set the stuck_low_mip PERCEPTIBILITY ratio for AUTO-POOL selection: the target's longest on-screen side "
	     "must be at least this many times the held mip's width in pixels, or the texture is refused. A low mip only "
	     "LOOKS blurry if the object out-resolves it; without this gate the anomaly ships 'injected but invisible' "
	     "labels, which is the dataset-poisoning direction. COMPILED DEFAULT 4.0, i.e. one held texel must cover at "
	     "least four screen pixels. IT IS A PICK-TIME FILTER ONLY AND NEVER DECIDES observable - observable stays the "
	     "m49 measurement (labelled AND condition held AND target_pixels >= the minimum). Its stated weakness is "
	     "real: it assumes the texture maps roughly once across the object, so a TILING texture repeats N times and "
	     "the rule over-admits; UV density is not available at pick time. Evaluated on projected bounds only, no "
	     "pixel read. 0 disables it. PRECEDENCE: console beats DefaultGame.ini [AnomalyInjector] "
	     "StuckMipMinTexelRatio, which beats the compiled default. Range [0..4096]; out of range is REFUSED. "
	     "Usage: IAI.Anomaly.StuckMipMinTexelRatio <ratio|default>"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleStuckMipMinTexelRatio));

static FAutoConsoleCommand GLodMinCoverageCmd(
	TEXT("IAI.Anomaly.LodMinCoverage"),
	TEXT("Set the lod_popping SCREEN-COVERAGE minimum, in PERCENT of frame, for AUTO-POOL selection. A candidate is "
	     "refused if its bounds-projected screen coverage at pick time is below this. PRECEDENCE: console beats "
	     "DefaultGame.ini [AnomalyInjector] LodPoppingMinCoveragePct, which beats the compiled default 7.0. Range "
	     "[0..100]; out of range is REFUSED, never clamped. 0 disables the COVERAGE gate only - the distance gate and "
	     "the highest-LOD requirement are untouched. A targeted fire on a named object already bypasses this gate "
	     "entirely. THE COMPILED DEFAULT 7.0 IS A MEASURED NUMBER, NOT A PREFERENCE: m30 calibrated it against a "
	     "last-visible anchor of 9.3453% and a first-invisible anchor of 3.9045%, with margins 1.34x below the visible "
	     "anchor and 1.79x above the invisible one, biased toward REFUSING because a positive label with no visible "
	     "change is the dataset-poisoning direction. Changing it at runtime is an operator decision and is NOT a "
	     "re-calibration; the default is deliberately not moved. Pass 'default' to clear the override. "
	     "Usage: IAI.Anomaly.LodMinCoverage <pct|default>"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleLodMinCoverage));

static FAutoConsoleCommand GLodRequireHighestLodCmd(
	TEXT("IAI.Anomaly.LodRequireHighestLod"),
	TEXT("Require an AUTO-POOL lod_popping candidate to be rendering at its highest-detail LOD (level 0). The "
	     "anomaly's visible magnitude is the CONTRAST between the LOD the object is currently at and the one forced "
	     "onto it, so forcing a low LOD onto something already at a low LOD changes little or nothing - which is why "
	     "the same anomaly reads strong on some objects and invisible on others. Distance and screen coverage are "
	     "proxies for that contrast; this is the contrast itself. It is the GRADED form of the existing single-LOD "
	     "guard, which refuses a mesh that cannot pop at all. AUTO-POOL SELECTION ONLY: a targeted fire on a named "
	     "object is never blocked, but it WARNS naming the current level so a weak-looking targeted fire explains "
	     "itself. The current level is read on the GAME THREAD - for a skinned mesh from "
	     "USkinnedMeshComponent::GetPredictedLODLevel(), for a static mesh by the engine's own screen-size "
	     "computation against the asset's authored per-LOD thresholds. It is NOT a 'was it rendered' read. "
	     "PRECEDENCE: console beats DefaultGame.ini [AnomalyInjector] LodPoppingRequireHighestLod, which beats the "
	     "compiled default ON. Setting it OFF restores the previous auto-pool candidate set exactly. "
	     "Usage: IAI.Anomaly.LodRequireHighestLod <0|1|default>"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleLodRequireHighestLod));

static FAutoConsoleCommand GAllowTranslucentOnlyTargetsCmd(
	TEXT("IAI.Select.AllowTranslucentOnlyTargets"),
	TEXT("Allow the picker to select an actor whose every renderable-visible component draws ONLY through translucent "
	     "material slots. DEFAULT: EXCLUDED. Such a target writes no depth and no custom depth, so the m43 target mask "
	     "measures nothing for it: every frame of its event reads target_pixels -1 and observable null, and m49 then "
	     "writes observability_measured=false and falls affected_frames back to the injected subset. That is an event "
	     "the dataset cannot verify, so it is refused at the PICKER - the only place it never enters the dataset at "
	     "all. The predicate is the SAME shared test the census uses for its EXCLUDED(translucent) verdict "
	     "(AnomalyViewport::IsTranslucentOnlyComponent), so selection and the census cannot drift apart. A component "
	     "whose material opts in via IsTranslucencyWritingCustomDepth() is NOT translucent-only for this purpose - it "
	     "is measurable. ACTOR-LEVEL: an actor with any opaque renderable-visible component is admitted unchanged. "
	     "TARGETED FIRE IS NEVER BLOCKED - this gates AUTO-POOL selection only. Excluded actors are logged once per "
	     "run as EXCLUDED-TRANSLUCENT and counted in run_summary.translucent_only_excluded_targets. PRECEDENCE: "
	     "console beats DefaultGame.ini [AnomalyInjector] AllowTranslucentOnlyTargets, which beats the compiled "
	     "default (excluded). Setting it to 1 restores the pre-m49-A2 candidate set exactly. G140 BOUNDARY: the same "
	     "seed picks different targets across this change, so banked auto-pool runs are non-comparable to a run with "
	     "a different setting. Pass 'default' to clear the override. "
	     "Usage: IAI.Select.AllowTranslucentOnlyTargets <0|1|default>"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleAllowTranslucentOnlyTargets));

static FAutoConsoleCommand GAllowNaniteTargetsCmd(
	TEXT("IAI.Targets.AllowNanite"),
	TEXT("Allow a target whose drawn primitives include a Nanite component. DEFAULT: 0 (REFUSED). On UE 5.1 a Nanite "
	     "primitive never writes custom depth, so the target mask cannot carry it. With 0 such a target is refused "
	     "BEFORE apply as nanite_unmaskable in all three places a target is chosen or applied: the auto-pool (the actor "
	     "is dropped from the candidate list before the target is drawn, so the random stream is untouched when no "
	     "Nanite actor is on screen), a targeted fire, and the ApplyAnomaly backstop. Each refused actor is logged as "
	     "REFUSED-NANITE and counted once per run in run_summary.refused_nanite. The cost is fewer eligible objects on "
	     "Nanite-heavy content. With 1 the previous contract returns unchanged: label and box, no mask, "
	     "observability_measured false. camera_clipping has no target and is unaffected. PRECEDENCE: console beats "
	     "DefaultGame.ini [AnomalyInjector] AllowNaniteTargets, which beats the compiled default (0). Pass 'default' to "
	     "clear the override. Usage: IAI.Targets.AllowNanite <0|1|default>"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&HandleAllowNaniteTargets));

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
