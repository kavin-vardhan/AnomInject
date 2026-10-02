#include "AnomalyStuckMipStats.h"
#include "AnomalyInjectorLog.h"
#include "HAL/IConsoleManager.h"
#include "Misc/ConfigCacheIni.h"

namespace
{
	FString GRouteOverride;

	bool ParseRoute(const FString& Text, AnomalyProxyBlur::ERoute& Out)
	{
		if (Text == TEXT("auto")) { Out = AnomalyProxyBlur::ERoute::Auto; return true; }
		if (Text == TEXT("hold")) { Out = AnomalyProxyBlur::ERoute::Hold; return true; }
		if (Text == TEXT("proxy")) { Out = AnomalyProxyBlur::ERoute::Proxy; return true; }
		return false;
	}
}

namespace AnomalyStuckMip
{
	AnomalyProxyBlur::ERoute GetRoute()
	{
		FString Value = GRouteOverride;
		if (Value.IsEmpty() && GConfig)
		{
			GConfig->GetString(TEXT("AnomalyInjector"), TEXT("StuckMipRoute"), Value, GGameIni);
		}
		AnomalyProxyBlur::ERoute Route = AnomalyProxyBlur::ERoute::Auto;
		ParseRoute(Value.ToLower(), Route);
		return Route;
	}

	const TCHAR* LexRoute(AnomalyProxyBlur::ERoute Route)
	{
		return Route == AnomalyProxyBlur::ERoute::Proxy ? TEXT("proxy")
			: Route == AnomalyProxyBlur::ERoute::Hold ? TEXT("hold") : TEXT("auto");
	}
}

static FAutoConsoleCommand GStuckMipRouteCmd(
	TEXT("IAI.Anomaly.StuckMipRoute"),
	TEXT("stuck_low_mip route: auto selects a private target copy for shared textures, hold retains the pure-texture streaming route, proxy always uses private copies. DefaultGame.ini [AnomalyInjector] StuckMipRoute=auto. Usage: IAI.Anomaly.StuckMipRoute [auto|hold|proxy|default]"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() > 0)
		{
			AnomalyProxyBlur::ERoute Parsed;
			const FString Value = Args[0].ToLower();
			if (Value == TEXT("default")) { GRouteOverride.Reset(); }
			else if (ParseRoute(Value, Parsed)) { GRouteOverride = Value; }
			else { UE_LOG(LogAnomaly, Warning, TEXT("stuck_low_mip: invalid route '%s'; unchanged."), *Args[0]); return; }
		}
		UE_LOG(LogAnomaly, Log, TEXT("stuck_low_mip: route=%s"), AnomalyStuckMip::LexRoute(AnomalyStuckMip::GetRoute()));
	}));
