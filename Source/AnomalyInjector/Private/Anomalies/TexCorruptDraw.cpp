#include "Anomalies/TexCorruptCore.h"

#include "AnomalyInjectorLog.h"
#include "AnomalyInjectorSubsystem.h"
#include "DeviceProfiles/DeviceProfile.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include "Engine/Canvas.h"
#include "Engine/TextureLODSettings.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/App.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RendererInterface.h"
#include "RenderingThread.h"
#include "RHI.h"
#include "RHIResources.h"
#include "TextureResource.h"
#include "UObject/Package.h"

namespace AnomalyTexCorrupt
{
	UTextureRenderTarget2D* AllocateTarget(int32 W, int32 H, bool bSRGB, int32 ExpectedMips, UTexture2D* SamplerSource,
		FString& OutFailure, bool& bOutResourceCreated)
	{
		OutFailure.Reset();
		bOutResourceCreated = false;
		if (W <= 0 || H <= 0 || W > 16384 || H > 16384)
		{
			OutFailure = FString::Printf(TEXT("bad_size_%dx%d"), W, H);
			return nullptr;
		}
		UTextureRenderTarget2D* Target = NewObject<UTextureRenderTarget2D>(GetTransientPackage(), NAME_None, RF_Transient);
		if (!Target)
		{
			OutFailure = TEXT("newobject_failed");
			return nullptr;
		}
		Target->ClearColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
		if (SamplerSource)
		{
			Target->LODGroup = SamplerSource->LODGroup;
			Target->Filter = SamplerSource->Filter;
			Target->AddressX = SamplerSource->AddressX;
			Target->AddressY = SamplerSource->AddressY;
			Target->MipsAddressU = SamplerSource->AddressX;
			Target->MipsAddressV = SamplerSource->AddressY;
		}
		Target->bAutoGenerateMips = ExpectedMips > 1;
		Target->InitCustomFormat(W, H, PF_B8G8R8A8, !bSRGB);
		Target->UpdateResourceImmediate(true);
		bOutResourceCreated = true;

		if (!Target->GameThread_GetRenderTargetResource())
		{
			OutFailure = TEXT("no_resource");
		}
		else if ((int32)Target->SizeX != W || (int32)Target->SizeY != H)
		{
			OutFailure = FString::Printf(TEXT("size_%dx%d_expected_%dx%d"), (int32)Target->SizeX, (int32)Target->SizeY, W, H);
		}
		else if (Target->GetNumMips() != ExpectedMips)
		{
			OutFailure = FString::Printf(TEXT("mips_%d_expected_%d"), Target->GetNumMips(), ExpectedMips);
		}
		if (!OutFailure.IsEmpty())
		{
			Target->ReleaseResource();
			return nullptr;
		}
		const float Bias = SamplerSource ? EffectiveRtSamplerBias() : 0.0f;
		if (Bias != 0.0f)
		{
			FTextureResource* Res = Target->GetResource();
			const ESamplerFilter Filter = (ESamplerFilter)UDeviceProfileManager::Get().GetActiveProfile()->GetTextureLODSettings()->GetSamplerFilter(Target);
			const ESamplerAddressMode AddrU = Target->AddressX == TA_Wrap ? AM_Wrap : (Target->AddressX == TA_Clamp ? AM_Clamp : AM_Mirror);
			const ESamplerAddressMode AddrV = Target->AddressY == TA_Wrap ? AM_Wrap : (Target->AddressY == TA_Clamp ? AM_Clamp : AM_Mirror);
			ENQUEUE_RENDER_COMMAND(AnomalyTexCorruptRtSamplerBias)(
				[Res, Filter, AddrU, AddrV, Bias](FRHICommandListImmediate&)
				{
					if (Res)
					{
						Res->SamplerStateRHI = RHICreateSamplerState(FSamplerStateInitializerRHI(Filter, AddrU, AddrV, AM_Wrap, Bias));
					}
				});
		}
		return Target;
	}

	float EffectiveRtSamplerBias()
	{
		const int32 L = Levers().RtSamplerBias;
		if (L == 0)
		{
			return 0.0f;
		}
		return UTexture2D::GetGlobalMipMapLODBias();
	}

	float EffectiveSrcMipCompensation()
	{
		if (Levers().SrcMipCompensation == 0)
		{
			return 0.0f;
		}
		return UTexture2D::GetGlobalMipMapLODBias();
	}

	struct FTripwireHold
	{
		FTextureRHIRef Ref;
		bool bTaken = false;
	};

	TSharedRef<FTripwireHold, ESPMode::ThreadSafe> MakeTripwireHold()
	{
		return MakeShared<FTripwireHold, ESPMode::ThreadSafe>();
	}

	FThreadSafeCounter& SourceChangedCounter()
	{
		static FThreadSafeCounter Counter;
		return Counter;
	}

	bool IsTargetDrawable(UTextureRenderTarget2D* Target)
	{
		return Target && Target->GetResource() && Target->GameThread_GetRenderTargetResource();
	}

	bool DrawLevel(UWorld* World, UTextureRenderTarget2D* Target, UMaterialInterface* Material, int32 W, int32 H, bool bClear)
	{
		if (!World || !Target || !Material)
		{
			return false;
		}
		if (bClear)
		{
			UKismetRenderingLibrary::ClearRenderTarget2D(World, Target, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
		}
		UCanvas* Canvas = nullptr;
		FVector2D Size(0.0, 0.0);
		FDrawToRenderTargetContext Context;
		UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(World, Target, Canvas, Size, Context);
		if (!Canvas)
		{
			return false;
		}
		Canvas->K2_DrawMaterial(Material, FVector2D(0.0, 0.0), FVector2D((double)W, (double)H), FVector2D(0.0, 0.0),
			FVector2D(1.0, 1.0));
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(World, Context);
		return true;
	}

	void EnqueueMipCopy(UTextureRenderTarget2D* Scratch, UTextureRenderTarget2D* Out, int32 Mip, int32 W, int32 H)
	{
		FTextureRenderTargetResource* SrcRes = Scratch ? Scratch->GameThread_GetRenderTargetResource() : nullptr;
		FTextureRenderTargetResource* DstRes = Out ? Out->GameThread_GetRenderTargetResource() : nullptr;
		if (!SrcRes || !DstRes)
		{
			TripwireCounter().Increment();
			UE_LOG(LogAnomaly, Error,
				TEXT("texcorrupt: MIP COPY NOT ENQUEUED for mip %d (%dx%d) - a render-target resource was null after the ")
				TEXT("draw preconditions passed. Counted in texcorrupt_rt_mip_mismatch so it is loud, never silent."),
				Mip, W, H);
			return;
		}
		ENQUEUE_RENDER_COMMAND(AnomalyTexCorruptMipCopy)(
			[SrcRes, DstRes, Mip, W, H](FRHICommandListImmediate& RHICmdList)
			{
				FRHITexture* Src = SrcRes->GetRenderTargetTexture().GetReference();
				FRHITexture* Dst = DstRes->GetRenderTargetTexture().GetReference();
				if (!Src || !Dst || (int32)Dst->GetNumMips() <= Mip)
				{
					TripwireCounter().Increment();
					return;
				}
				FRDGBuilder GraphBuilder(RHICmdList);
				FRDGTextureRef SrcRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(Src, TEXT("AnomalyTexCorruptScratch")));
				FRDGTextureRef DstRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(Dst, TEXT("AnomalyTexCorruptOutput")));
				FRHICopyTextureInfo Info;
				Info.Size = FIntVector(W, H, 1);
				Info.DestMipIndex = (uint32)Mip;
				AddCopyTexturePass(GraphBuilder, SrcRDG, DstRDG, Info);
				GraphBuilder.Execute();
			});
	}

	void EnqueueTripwire(UTexture2D* Source, int32 M, int32 W, int32 H, int32 FirstMip, const FString& Name,
		const TSharedRef<FTripwireHold, ESPMode::ThreadSafe>& Hold, bool bPost)
	{
		FTextureResource* Res = Source ? Source->GetResource() : nullptr;
		if (!Res)
		{
			TripwireCounter().Increment();
			UE_LOG(LogAnomaly, Error, TEXT("texcorrupt: TRIPWIRE '%s' has no texture resource on the game thread."), *Name);
			return;
		}
		ENQUEUE_RENDER_COMMAND(AnomalyTexCorruptTripwire)(
			[Res, M, W, H, FirstMip, Name, Hold, bPost](FRHICommandListImmediate& RHICmdList)
			{
				FRHITexture* Tex = Res->TextureRHI.GetReference();
				const int32 Mips = Tex ? (int32)Tex->GetNumMips() : -1;
				const FIntPoint Extent = Tex ? Tex->GetSizeXY() : FIntPoint(-1, -1);
				if (!bPost)
				{
					Hold->Ref = Res->TextureRHI;
					Hold->bTaken = true;
				}
				else
				{
					if (Hold->bTaken && Hold->Ref.GetReference() != Tex)
					{
						SourceChangedCounter().Increment();
						UE_LOG(LogAnomaly, Warning,
							TEXT("texcorrupt: COPY SOURCE CHANGED '%s' - the source RHI texture was replaced (streaming) between the ")
							TEXT("first and the last level draw of its copy. The copy keeps the levels it drew and stays visibly ")
							TEXT("corrupted; identity against the new resource is not claimed. Counted in texcorrupt_copy_source_changed ")
							TEXT("(a note, not a refusal)."),
							*Name);
					}
					Hold->Ref.SafeRelease();
				}
				if (Mips != M || Extent != FIntPoint(W, H))
				{
					TripwireCounter().Increment();
					UE_LOG(LogAnomaly, Error,
						TEXT("texcorrupt: MIP TRIPWIRE '%s' (%s) - on the render thread the source RHI texture has %d mip(s) at %dx%d, ")
						TEXT("while admission derived %d resident mip(s) at %dx%d (cooked mip %d first) from game-thread state. The ")
						TEXT("premise that RHI mip i IS cooked mip %d+i does not hold for this texture now; counted in ")
						TEXT("texcorrupt_rt_mip_mismatch, which stops S1."),
						*Name, bPost ? TEXT("after the copy") : TEXT("before the copy"), Mips, Extent.X, Extent.Y, M, W, H, FirstMip, FirstMip);
				}
			});
	}

	void ReleaseTarget(UTextureRenderTarget2D* Target)
	{
		if (Target)
		{
			Target->ReleaseResource();
		}
	}

	namespace
	{
		struct FWarmSet
		{
			TWeakObjectPtr<UAnomalyInjectorSubsystem> Holder;
			TArray<TWeakObjectPtr<UObject>> Objects;
			TArray<TWeakObjectPtr<UTextureRenderTarget2D>> Targets;
		};

		FWarmSet GWarm;

		void Hold(UAnomalyInjectorSubsystem* Injector, UObject* Obj)
		{
			if (Injector && Obj)
			{
				Injector->TexCorruptHold(Obj);
				GWarm.Objects.Add(Obj);
			}
		}
	}

	void GatherCorruptorMaterials(UWorld* World, TArray<UMaterialInterface*>& Out)
	{
		if (UAnomalyInjectorSubsystem* Injector = ResolveInjector(World))
		{
			if (UMaterialInterface* Uv = Injector->GetTexCorruptUvMaterial()) { Out.Add(Uv); }
			if (UMaterialInterface* Normal = Injector->GetTexCorruptNormalMaterial()) { Out.Add(Normal); }
		}
	}

	int32 BeginWarmDraw(UWorld* World)
	{
		EndWarmDraw();
		UAnomalyInjectorSubsystem* Injector = ResolveInjector(World);
		UMaterialInterface* Uv = Injector ? Injector->GetTexCorruptUvMaterial() : nullptr;
		UMaterialInterface* Normal = Injector ? Injector->GetTexCorruptNormalMaterial() : nullptr;
		UTexture2D* Noise = Injector ? Injector->GetTexCorruptNoiseTexture() : nullptr;
		if (!World || !Uv || !Normal || !Noise || !FApp::CanEverRender())
		{
			UE_LOG(LogAnomaly, Warning,
				TEXT("texcorrupt: WARM DRAW SKIPPED - uv=%d normal=%d noise=%d can_render=%d. Nothing was drawn, so the first ")
				TEXT("labelled m53 Apply of this run pays every first-use PSO itself. With an asset missing, every m53 fire is ")
				TEXT("refused assets_unavailable anyway."),
				Uv ? 1 : 0, Normal ? 1 : 0, Noise ? 1 : 0, FApp::CanEverRender() ? 1 : 0);
			return 0;
		}
		GWarm.Holder = Injector;

		struct FPass
		{
			UMaterialInterface* Corruptor;
			bool bSRGB;
			int32 Kind;
			const TCHAR* Name;
		};
		const FPass Passes[] = {
			{ Uv, true, 0, TEXT("uv_rgba8_srgb") },
			{ Uv, false, 1, TEXT("uv_rgba8") },
			{ Normal, false, 2, TEXT("normal_rgba8") },
		};

		int32 Draws = 0;
		for (const FPass& Pass : Passes)
		{
			FString Failure;
			bool bCreated = false;
			UTextureRenderTarget2D* Out = AllocateTarget(8, 8, Pass.bSRGB, 4, nullptr, Failure, bCreated);
			UTextureRenderTarget2D* Scratch = Out ? AllocateTarget(4, 4, Pass.bSRGB, 1, nullptr, Failure, bCreated) : nullptr;
			UMaterialInstanceDynamic* Mid = UMaterialInstanceDynamic::Create(Pass.Corruptor, GetTransientPackage());
			if (!Out || !Scratch || !Mid)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("texcorrupt: WARM DRAW pass %s could not allocate (%s)."), Pass.Name, *Failure);
				ReleaseTarget(Out);
				ReleaseTarget(Scratch);
				continue;
			}
			Hold(Injector, Out);
			Hold(Injector, Scratch);
			Hold(Injector, Mid);
			GWarm.Targets.Add(Out);
			GWarm.Targets.Add(Scratch);
			if (Pass.Corruptor == Uv)
			{
				Mid->SetScalarParameterValue(Param::SrcKind, (float)Pass.Kind);
				Mid->SetTextureParameterValue(Pass.Kind == 0 ? Param::SrcColor : Param::SrcData, Noise);
				Mid->SetScalarParameterValue(Param::UvScale, 1.0f);
			}
			else
			{
				Mid->SetTextureParameterValue(Param::SrcNormal, Noise);
				Mid->SetTextureParameterValue(Param::NoiseNormal, Noise);
				Mid->SetScalarParameterValue(Param::NormalSignX, 1.0f);
				Mid->SetScalarParameterValue(Param::NormalSignY, 1.0f);
			}
			Mid->SetScalarParameterValue(Param::SrcMip, 0.0f);
			if (DrawLevel(World, Out, Mid, 8, 8, true))
			{
				++Draws;
			}
			if (DrawLevel(World, Scratch, Mid, 4, 4, true))
			{
				++Draws;
				EnqueueMipCopy(Scratch, Out, 1, 4, 4);
			}
		}
		UE_LOG(LogAnomaly, Log,
			TEXT("texcorrupt: WARM DRAW issued %d draw(s) across %d target format pass(es) (allocation update, clear, draw, and ")
			TEXT("one per-mip copy each), outside any captured frame. Whether these hit the same PSOs as a real Apply is NOT ")
			TEXT("PROVED - the engine offers no PSO introspection - so G-COST measures the first labelled Apply with this on and ")
			TEXT("off (plan R9.3)."),
			Draws, (int32)UE_ARRAY_COUNT(Passes));
		return Draws;
	}

	void EndWarmDraw()
	{
		for (const TWeakObjectPtr<UTextureRenderTarget2D>& Weak : GWarm.Targets)
		{
			ReleaseTarget(Weak.Get());
		}
		if (UAnomalyInjectorSubsystem* Holder = GWarm.Holder.Get())
		{
			for (const TWeakObjectPtr<UObject>& Weak : GWarm.Objects)
			{
				Holder->TexCorruptLetGo(Weak.Get());
			}
		}
		const bool bHad = GWarm.Targets.Num() > 0;
		GWarm = FWarmSet();
		if (bHad)
		{
			UE_LOG(LogAnomaly, Log, TEXT("texcorrupt: WARM DRAW released its scratch targets."));
		}
	}
}
