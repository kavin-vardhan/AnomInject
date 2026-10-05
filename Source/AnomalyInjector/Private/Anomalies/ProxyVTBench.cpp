#include "Anomalies/AnomalyPrivateVT.h"
#include "AnomalyBenchGate.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyViewport.h"
#include "Anomalies/TexCorruptCore.h"
#include "Components/StaticMeshComponent.h"
#include "Components/MeshComponent.h"
#include "Containers/Ticker.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Misc/Crc.h"
#include "TextureResource.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectIterator.h"

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
	FAutoConsoleCommandWithWorldAndArgs CensusStateCommand(TEXT("IAI.Bench.VTCensusState"),
		TEXT("Bench-only: hash material bindings and texture resources/residency without changing them."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>&, UWorld* World)
		{
			if (!AnomalyBenchGate::IsEnabled() || !World) { return; }
			AnomalyViewport::FReadOnlyEnumerationScope ReadOnly;
			uint32 Materials = 0, Textures = 0;
			int32 Slots = 0, Bindings = 0;
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				TInlineComponentArray<UMeshComponent*> Components(*It);
				for (UMeshComponent* Component : Components)
				{
					for (int32 Slot = 0; Slot < Component->GetNumMaterials(); ++Slot)
					{
						UMaterialInterface* Material = Component->GetMaterial(Slot);
						Materials = FCrc::MemCrc32(&Material, sizeof(Material), Materials);
						++Slots;
						TArray<AnomalyTexCorrupt::FBinding> Active;
						bool Resource = false, Shader = false, Complete = false;
						AnomalyTexCorrupt::ReadActiveBindings(World, Material, Component, Active, Resource, Shader, Complete);
						for (const AnomalyTexCorrupt::FBinding& Binding : Active)
						{
							++Bindings;
							const UTexture2D* Texture = Binding.Tex2D;
							const FTextureResource* R = Texture ? Texture->GetResource() : nullptr;
							const FVirtualTexture2DResource* VT = R ? R->GetVirtualTexture2DResource() : nullptr;
							const uint32 Producer = VT ? VT->GetProducerHandle().PackedValue : 0;
							const int32 Resident = Texture && !VT ? Texture->GetNumResidentMips() : -1;
							const uint32 Flags = Texture ? (Texture->bForceMiplevelsToBeResident ? 1u : 0u)
								| (Texture->bGlobalForceMipLevelsToBeResident ? 2u : 0u) | (Texture->NeverStream ? 4u : 0u) : 0u;
							const double ForcedUntil = Texture ? Texture->ForceMipLevelsToBeResidentTimestamp : 0.0;
							Textures = FCrc::MemCrc32(&Texture, sizeof(Texture), Textures);
							Textures = FCrc::MemCrc32(&R, sizeof(R), Textures);
							Textures = FCrc::MemCrc32(&Producer, sizeof(Producer), Textures);
							Textures = FCrc::MemCrc32(&Resident, sizeof(Resident), Textures);
							Textures = FCrc::MemCrc32(&Flags, sizeof(Flags), Textures);
							Textures = FCrc::MemCrc32(&ForcedUntil, sizeof(ForcedUntil), Textures);
						}
					}
				}
			}
			UE_LOG(LogAnomaly, Display, TEXT("VT-CENSUS-STATE slots=%d bindings=%d materials=%u textures=%u resources=%d producers=%d"),
				Slots, Bindings, Materials, Textures, UAnomalyPrivateVT::LiveResources(), UAnomalyPrivateVT::LiveProducers());
		}));
	FAutoConsoleCommandWithWorldAndArgs InventoryCommand(TEXT("IAI.Bench.ProxyVTInventory"),
		TEXT("Bench-only: enumerate genuine VT textures, mesh users, compiled named bindings and blurry tree eligibility."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>&, UWorld* World)
		{
			if (!AnomalyBenchGate::IsEnabled() || !World) { return; }
			TMap<UTexture2D*, TSet<FString>> Users;
			TMap<UTexture2D*, TSet<FString>> Named;
			TMap<UTexture2D*, TSet<FString>> Eligible;
			int32 Meshes = 0;
			int32 Unread = 0;
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				AActor* Actor = *It;
				TSet<UTexture2D*> ActorVTs;
				TInlineComponentArray<UMeshComponent*> Components(Actor);
				for (UMeshComponent* Component : Components)
				{
					++Meshes;
					for (int32 Slot = 0; Slot < Component->GetNumMaterials(); ++Slot)
					{
						UMaterialInterface* Material = Component->GetMaterial(Slot);
						if (!Material) { continue; }
						TArray<UTexture*> Used;
						Material->GetUsedTextures(Used, EMaterialQualityLevel::Num, true, World->FeatureLevel, true);
						bool bUsesVT = false;
						for (UTexture* Texture : Used)
						{
							UTexture2D* VT = Cast<UTexture2D>(Texture);
							if (VT && VT->IsCurrentlyVirtualTextured())
							{
								Users.FindOrAdd(VT).Add(Actor->GetName());
								ActorVTs.Add(VT);
								bUsesVT = true;
							}
						}
						if (!bUsesVT) { continue; }
						TArray<AnomalyTexCorrupt::FBinding> Bindings;
						bool bResource = false, bShader = false, bComplete = false;
						AnomalyTexCorrupt::ReadActiveBindings(World, Material, Component, Bindings, bResource, bShader, bComplete);
						Unread += bResource && bShader && bComplete ? 0 : 1;
						for (const AnomalyTexCorrupt::FBinding& Binding : Bindings)
						{
							if (Binding.Tex2D && Binding.Type == EMaterialTextureParameterType::Virtual && !Binding.ParamName.IsNone())
							{
								Named.FindOrAdd(Binding.Tex2D).Add(Actor->GetName());
							}
						}
					}
				}
				if (ActorVTs.Num() > 0)
				{
					AnomalyTexCorrupt::Uniform::KickForActor(Actor);
					AnomalyTexCorrupt::FTreeInputs Inputs;
					Inputs.bProxyBlur = true;
					Inputs.Mode = AnomalyTexCorrupt::EMode::ProxyBlur;
					Inputs.bCensus = true;
					Inputs.TargetActor = Actor;
					AnomalyTexCorrupt::FTreeResult Result;
					AnomalyTexCorrupt::EvaluateTree(World, Inputs, Result);
					for (UTexture2D* Texture : ActorVTs)
					{
						if (Result.bApply) { Eligible.FindOrAdd(Texture).Add(Actor->GetName()); }
						UE_LOG(LogAnomaly, Display, TEXT("PROXYVT-INVENTORY user actor='%s' texture='%s' eligible=%d reason='%s'"),
							*Actor->GetName(), *Texture->GetPathName(), Result.bApply ? 1 : 0, *Result.FinalKey());
					}
				}
			}
			int32 Count = 0;
			for (TObjectIterator<UTexture2D> It; It; ++It)
			{
				if (!It->IsCurrentlyVirtualTextured()) { continue; }
				++Count;
				UE_LOG(LogAnomaly, Display, TEXT("PROXYVT-INVENTORY texture='%s' users=%d named=%d eligible=%d"),
					*It->GetPathName(), Users.FindRef(*It).Num(), Named.FindRef(*It).Num(), Eligible.FindRef(*It).Num());
			}
			UE_LOG(LogAnomaly, Display, TEXT("PROXYVT-INVENTORY end world='%s' genuine_vts=%d meshes=%d unread_vt_slots=%d resources=%d producers=%d"),
				*World->GetName(), Count, Meshes, Unread, UAnomalyPrivateVT::LiveResources(), UAnomalyPrivateVT::LiveProducers());
		}));
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
