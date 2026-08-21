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
