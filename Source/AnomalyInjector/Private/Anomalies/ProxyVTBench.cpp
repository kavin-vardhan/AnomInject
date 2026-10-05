#include "Anomalies/AnomalyPrivateVT.h"
#include "AnomalyBenchGate.h"
#include "AnomalyInjectorLog.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/Ticker.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"

#if !UE_BUILD_SHIPPING
namespace AnomalyProxyVTBenchDetail
{
	struct FProbe
	{
		TWeakObjectPtr<UWorld> World;
		TWeakObjectPtr<UStaticMeshComponent> Component;
		TStrongObjectPtr<UMaterialInterface> Original;
		TStrongObjectPtr<UMaterialInterface> Resolved;
		TStrongObjectPtr<UMaterialInstanceDynamic> Mid;
		TStrongObjectPtr<UAnomalyPrivateVT> Texture;
		TStrongObjectPtr<UTexture2D> Source;
		FName Parameter;
		bool bInstalled = false;
	};

	TUniquePtr<FProbe> Probe;
	bool bNoBind = false;
	FAutoConsoleCommand NoBindCommand(TEXT("IAI.Bench.ProxyVTNoBind"),
		TEXT("Bench-only mutant: install the host MID with its original VT source. Pixel-change proof must fail."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			if (!AnomalyBenchGate::IsEnabled()) { return; }
			bNoBind = Args.Num() == 1 && Args[0] == TEXT("1");
			UE_LOG(LogAnomaly, Display, TEXT("IAI.Bench.ProxyVTNoBind=%d"), bNoBind ? 1 : 0);
		}));

	void Restore()
	{
		if (Probe)
		{
			UStaticMeshComponent* Component = Probe->Component.Get();
			if (Component && Component->GetMaterial(0) == Probe->Mid.Get())
			{
				Component->SetMaterial(0, Probe->Original.Get());
			}
			if (Probe->Mid.IsValid())
			{
				Probe->Mid->SetTextureParameterValue(Probe->Parameter, Probe->Source.Get());
			}
			if (Probe->Texture.IsValid()) { Probe->Texture->ReleasePrivate(); }
			Probe.Reset();
		}
		UE_LOG(LogAnomaly, Display, TEXT("PROXYVT-PROBE restored resources=%d producers=%d"),
			UAnomalyPrivateVT::LiveResources(), UAnomalyPrivateVT::LiveProducers());
	}

	bool TickPrefetch(float DeltaTime)
	{
		if (!Probe || Probe->bInstalled) { return true; }
		UStaticMeshComponent* Component = Probe->Component.Get();
		if (!Component || Component->GetMaterial(0) != Probe->Resolved.Get())
		{
			UE_LOG(LogAnomaly, Display, TEXT("PROXYVT-PROBE refused prefetch_target_changed"));
			Restore();
			return true;
		}
		FString Reason;
		if (!Probe->Texture->PollPrefetch(Reason))
		{
			if (!Reason.IsEmpty())
			{
				UE_LOG(LogAnomaly, Error, TEXT("PROXYVT-PROBE refused %s attempts=%d pages=%d"), *Reason, Probe->Texture->PrefetchAttempts(), Probe->Texture->PrefetchPages());
				Restore();
			}
			return true;
		}
		UE_LOG(LogAnomaly, Display, TEXT("PROXYVT-PROBE prefetch_ready attempts=%d pages=%d signal=gpu_page_table_exact_levels original_still_bound=1"), Probe->Texture->PrefetchAttempts(), Probe->Texture->PrefetchPages());
		Probe->Mid.Reset(UMaterialInstanceDynamic::Create(Probe->Resolved.Get(), GetTransientPackage()));
		Probe->Mid->SetTextureParameterValue(Probe->Parameter, Probe->Texture.Get());
		Component->SetMaterial(0, Probe->Mid.Get());
		Probe->bInstalled = true;
		UE_LOG(LogAnomaly, Display,
			TEXT("PROXYVT-PROBE installed actor=%s parameter=%s source=%s drop=%d first_mip=%d size=%dx%d tile=%d virtual=%d resources=%d producers=%d"),
			*Component->GetOwner()->GetName(), *Probe->Parameter.ToString(), *Probe->Source->GetPathName(), Probe->Texture->GetDrop(), Probe->Texture->GetFirstMip(),
			static_cast<int32>(Probe->Texture->GetSurfaceWidth()), static_cast<int32>(Probe->Texture->GetSurfaceHeight()),
			Probe->Texture->GetTileSize(), Probe->Texture->IsCurrentlyVirtualTextured() ? 1 : 0,
			UAnomalyPrivateVT::LiveResources(), UAnomalyPrivateVT::LiveProducers());
		return true;
	}

	FTSTicker::FDelegateHandle PrefetchTicker = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateStatic(&TickPrefetch));

	FAutoConsoleCommandWithWorldAndArgs Command(
		TEXT("IAI.Bench.ProxyVT"),
		TEXT("BENCH <actor-name> <texture-parameter> <drop> [no_request], or restore. Private VT identity/constructor proof."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			if (!AnomalyBenchGate::IsEnabled()) { return; }
			if (Args.Num() == 1 && Args[0] == TEXT("restore")) { Restore(); return; }
			if (!World || (Args.Num() != 3 && Args.Num() != 4) || (Args.Num() == 4 && Args[3] != TEXT("no_request"))) { return; }
			Restore();
			UStaticMeshComponent* Component = nullptr;
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				if (It->GetName() == Args[0])
				{
					Component = It->FindComponentByClass<UStaticMeshComponent>();
					break;
				}
			}
			UMaterialInterface* Material = Component ? Component->GetMaterial(0) : nullptr;
			UTexture* Bound = nullptr;
			const FName Parameter(*Args[1]);
			if (!Material || !Material->GetTextureParameterValue(FHashedMaterialParameterInfo(Parameter), Bound))
			{
				UE_LOG(LogAnomaly, Error, TEXT("PROXYVT-PROBE refused missing_component_or_parameter"));
				return;
			}
			UTexture2D* Source = Cast<UTexture2D>(Bound);
			TUniquePtr<FProbe> NewProbe = MakeUnique<FProbe>();
			NewProbe->World = World;
			NewProbe->Component = Component;
			NewProbe->Original.Reset(Component->OverrideMaterials.IsValidIndex(0) ? Component->OverrideMaterials[0].Get() : nullptr);
			NewProbe->Resolved.Reset(Material);
			NewProbe->Source.Reset(Source);
			NewProbe->Parameter = Parameter;
			NewProbe->Texture.Reset(NewObject<UAnomalyPrivateVT>(GetTransientPackage()));
			if (Args.Num() == 4) { NewProbe->Texture->BenchSkipRequests(); }
			FString Reason;
			if (!NewProbe->Texture->Initialize(Source, FCString::Atoi(*Args[2]), Reason))
			{
				UE_LOG(LogAnomaly, Error, TEXT("PROXYVT-PROBE refused %s"), *Reason);
				return;
			}
			UE_LOG(LogAnomaly, Display, TEXT("PROXYVT-PROBE preparing actor=%s original_still_bound=1"), *Args[0]);
			Probe = MoveTemp(NewProbe);
		}));

	struct FWorldCleanup
	{
		FWorldCleanup()
		{
			FWorldDelegates::OnWorldCleanup.AddLambda([](UWorld* World, bool bSessionEnded, bool bCleanupResources)
			{
				if (Probe && Probe->World.Get() == World) { Restore(); }
			});
		}
	};
	FWorldCleanup Cleanup;
}

bool AnomalyProxyVTBenchNoBind()
{
	return AnomalyBenchGate::IsEnabled() && AnomalyProxyVTBenchDetail::bNoBind;
}
#endif
