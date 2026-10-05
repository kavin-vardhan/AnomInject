#include "Anomalies/AnomalyPrivateVT.h"
#include "AnomalyBenchGate.h"
#include "AnomalyInjectorLog.h"
#include "Components/StaticMeshComponent.h"
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
		TStrongObjectPtr<UMaterialInstanceDynamic> Mid;
		TStrongObjectPtr<UAnomalyPrivateVT> Texture;
		TStrongObjectPtr<UTexture2D> Source;
		FName Parameter;
	};

	TUniquePtr<FProbe> Probe;

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

	FAutoConsoleCommandWithWorldAndArgs Command(
		TEXT("IAI.Bench.ProxyVT"),
		TEXT("BENCH <actor-name> <texture-parameter> <drop>, or restore. Private VT identity/constructor proof."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			if (!AnomalyBenchGate::IsEnabled()) { return; }
			if (Args.Num() == 1 && Args[0] == TEXT("restore")) { Restore(); return; }
			if (!World || Args.Num() != 3) { return; }
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
			NewProbe->Source.Reset(Source);
			NewProbe->Parameter = Parameter;
			NewProbe->Texture.Reset(NewObject<UAnomalyPrivateVT>(GetTransientPackage()));
			FString Reason;
			if (!NewProbe->Texture->Initialize(Source, FCString::Atoi(*Args[2]), Reason))
			{
				UE_LOG(LogAnomaly, Error, TEXT("PROXYVT-PROBE refused %s"), *Reason);
				return;
			}
			NewProbe->Mid.Reset(UMaterialInstanceDynamic::Create(Material, GetTransientPackage()));
			NewProbe->Mid->SetTextureParameterValue(Parameter, NewProbe->Texture.Get());
			Component->SetMaterial(0, NewProbe->Mid.Get());
			UE_LOG(LogAnomaly, Display,
				TEXT("PROXYVT-PROBE installed actor=%s parameter=%s source=%s drop=%d first_mip=%d size=%dx%d tile=%d virtual=%d resources=%d producers=%d"),
				*Args[0], *Args[1], *Source->GetPathName(), NewProbe->Texture->GetDrop(), NewProbe->Texture->GetFirstMip(),
				static_cast<int32>(NewProbe->Texture->GetSurfaceWidth()), static_cast<int32>(NewProbe->Texture->GetSurfaceHeight()),
				NewProbe->Texture->GetTileSize(), NewProbe->Texture->IsCurrentlyVirtualTextured() ? 1 : 0,
				UAnomalyPrivateVT::LiveResources(), UAnomalyPrivateVT::LiveProducers());
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
#endif
