#include "Anomalies/TexCorruptCore.h"

#include "AnomalyInjectorLog.h"
#include "AnomalyTargeting.h"
#include "AnomalyViewport.h"
#include "Components/MeshComponent.h"
#include "Containers/Ticker.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "RenderingThread.h"
#include "RHI.h"
#include "TextureResource.h"

#if ANOMALY_SHADERS
#include "AnomalyTexStatsShader.h"
#include "GlobalShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RendererInterface.h"
#include "RHIGPUReadback.h"
#endif

namespace AnomalyTexCorrupt
{
	namespace Uniform
	{
		namespace
		{
			using TexCorruptPure::Uniform::EVerdict;
			using TexCorruptPure::Uniform::ERule;

			struct FKey
			{
				const UTexture2D* Tex = nullptr;
				int32 FirstMip = 0;

				bool operator==(const FKey& O) const { return Tex == O.Tex && FirstMip == O.FirstMip; }
				friend uint32 GetTypeHash(const FKey& K) { return HashCombine(::GetTypeHash(K.Tex), ::GetTypeHash(K.FirstMip)); }
			};

			enum class EState : uint8
			{
				Queued,
				InFlight,
				Done,
				Failed
			};

			struct FEntry
			{
				TWeakObjectPtr<UTexture2D> Weak;
				FString Name;
				int32 W = 0;
				int32 H = 0;
				EState State = EState::Queued;
				TexCorruptPure::Uniform::FStats Stats;
				uint64 Id = 0;
				int32 Retries = 0;
				FString Fail;
			};

			struct FPublished
			{
				uint64 Id = 0;
				bool bOk = false;
				uint32 Raw[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
				FString Fail;
			};

			constexpr int32 MaxKicksPerPump = 4;
			constexpr int32 MaxInFlight = 16;
			constexpr int32 MaxPolls = 300;
			constexpr int32 MaxRetries = 1;

			TMap<FKey, FEntry>& Cache()
			{
				static TMap<FKey, FEntry> M;
				return M;
			}

			TArray<FKey>& Queue()
			{
				static TArray<FKey> Q;
				return Q;
			}

			TMap<uint64, FKey>& IdToKey()
			{
				static TMap<uint64, FKey> M;
				return M;
			}

			FCriticalSection& PublishedCS()
			{
				static FCriticalSection CS;
				return CS;
			}

			TArray<FPublished>& Published()
			{
				static TArray<FPublished> A;
				return A;
			}

			uint64 GNextId = 1;
			int32 GInFlight = 0;
			bool GTickerAdded = false;
			int32 GMeasured = 0;
			int32 GUniformUv = 0;
			int32 GFailed = 0;

			void Publish(FPublished&& P)
			{
				FScopeLock Lock(&PublishedCS());
				Published().Add(MoveTemp(P));
			}

#if ANOMALY_SHADERS
			struct FInflightRT
			{
				uint64 Id = 0;
				TUniquePtr<FRHIGPUBufferReadback> Readback;
				int32 Polls = 0;
			};

			TArray<FInflightRT>& InflightRT()
			{
				static TArray<FInflightRT> A;
				return A;
			}

			void PollRT()
			{
				TArray<FInflightRT>& A = InflightRT();
				for (int32 I = A.Num() - 1; I >= 0; --I)
				{
					FInflightRT& F = A[I];
					if (F.Readback.IsValid() && F.Readback->IsReady())
					{
						FPublished P;
						P.Id = F.Id;
						const uint32* Data = static_cast<const uint32*>(F.Readback->Lock(9 * sizeof(uint32)));
						if (Data)
						{
							FMemory::Memcpy(P.Raw, Data, sizeof(P.Raw));
							P.bOk = true;
						}
						else
						{
							P.Fail = TEXT("lock_failed");
						}
						F.Readback->Unlock();
						Publish(MoveTemp(P));
						A.RemoveAtSwap(I);
					}
					else if (++F.Polls > MaxPolls)
					{
						FPublished P;
						P.Id = F.Id;
						P.Fail = TEXT("timeout");
						Publish(MoveTemp(P));
						A.RemoveAtSwap(I);
					}
				}
			}

			void KickRT(FRHICommandListImmediate& RHICmdList, FTextureResource* Res, uint64 Id, int32 W, int32 H)
			{
				FRHITexture* T = Res ? Res->TextureRHI.GetReference() : nullptr;
				if (!T)
				{
					FPublished P;
					P.Id = Id;
					P.Fail = TEXT("no_rhi_texture");
					Publish(MoveTemp(P));
					return;
				}
				const FIntPoint Extent = T->GetSizeXY();
				if (Extent != FIntPoint(W, H))
				{
					FPublished P;
					P.Id = Id;
					P.Fail = FString::Printf(TEXT("rhi_extent_%dx%d"), Extent.X, Extent.Y);
					Publish(MoveTemp(P));
					return;
				}
				FRDGBuilder GraphBuilder(RHICmdList);
				FRDGTextureRef Src = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(T, TEXT("AnomalyTexStatsSource")));
				FRDGTextureSRVDesc SrvDesc = FRDGTextureSRVDesc::CreateForMipLevel(Src, 0);
				SrvDesc.SRGBOverride = SRGBO_ForceDisable;
				FRDGBufferRef Buffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateBufferDesc(sizeof(uint32), 9), TEXT("AnomalyTexStats"));
				FRDGBufferUAVRef Uav = GraphBuilder.CreateUAV(FRDGBufferUAVDesc(Buffer, PF_R32_UINT));
				AddClearUAVPass(GraphBuilder, Uav, 0u);
				FAnomalyTexStatsCS::FParameters* Params = GraphBuilder.AllocParameters<FAnomalyTexStatsCS::FParameters>();
				Params->SourceTexture = GraphBuilder.CreateSRV(SrvDesc);
				Params->OutStats = Uav;
				Params->SourceSize = FIntPoint(W, H);
				TShaderMapRef<FAnomalyTexStatsCS> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
				FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("AnomalyTexStats"), Shader, Params,
					FComputeShaderUtils::GetGroupCount(FIntPoint(W, H), 8));
				FInflightRT Item;
				Item.Id = Id;
				Item.Readback = MakeUnique<FRHIGPUBufferReadback>(TEXT("AnomalyTexStatsReadback"));
				AddEnqueueCopyPass(GraphBuilder, Item.Readback.Get(), Buffer, 9 * sizeof(uint32));
				GraphBuilder.Execute();
				InflightRT().Add(MoveTemp(Item));
			}
#endif

#if ANOMALY_SHADERS
			bool ResidentTop(UTexture2D* Tex, int32& OutFirst, int32& OutW, int32& OutH)
			{
				const FTexturePlatformData* PD = Tex ? Tex->GetPlatformData() : nullptr;
				if (!PD)
				{
					return false;
				}
				const FStreamableRenderResourceState& St = Tex->GetStreamableResourceState();
				if (!St.IsValid())
				{
					return false;
				}
				const int32 M = PD->Mips.Num();
				const int32 First = (int32)St.AssetLODBias + (int32)St.MaxNumLODs - (int32)St.NumResidentLODs;
				if ((int32)St.AssetLODBias + (int32)St.MaxNumLODs != M || St.NumResidentLODs < 1 || First < 0 || First >= M)
				{
					return false;
				}
				OutFirst = First;
				OutW = PD->Mips[First].SizeX;
				OutH = PD->Mips[First].SizeY;
				return OutW > 0 && OutH > 0;
			}

			void EnsureTicker()
			{
				if (GTickerAdded)
				{
					return;
				}
				GTickerAdded = true;
				FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([](float) -> bool
				{
					Pump();
					return true;
				}));
			}

			bool Enqueue(UTexture2D* Tex, int32 FirstMip, int32 W, int32 H)
			{
				const FKey K{ Tex, FirstMip };
				FEntry* E = Cache().Find(K);
				if (E && E->Weak.Get() == Tex)
				{
					return false;
				}
				FEntry& N = Cache().Add(K);
				N = FEntry();
				N.Weak = Tex;
				N.Name = Tex->GetName();
				N.W = W;
				N.H = H;
				N.State = EState::Queued;
				Queue().Add(K);
				EnsureTicker();
				return true;
			}
#endif

			void LogMeasured(const FEntry& E, int32 FirstMip)
			{
				const TexCorruptPure::Uniform::FStats& S = E.Stats;
				const float Tol = TexCorruptPure::Uniform::TolRaw;
				UE_LOG(LogAnomaly, Log,
					TEXT("texcorrupt: UNIFORM-STATS '%s' resident top mip %d (%dx%d, %u texels) raw min (%.4f %.4f %.4f %.4f) max (%.4f %.4f %.4f %.4f) ")
					TEXT("-> no_spatial_variation=%d flat_normal=%d flat_normal_y=%d at tolerance %.4f (2 of 255 steps)"),
					*E.Name, FirstMip, E.W, E.H, S.Texels, S.Min[0], S.Min[1], S.Min[2], S.Min[3], S.Max[0], S.Max[1], S.Max[2], S.Max[3],
					TexCorruptPure::Uniform::IsUniformUnder(ERule::AnyChannelRange, S, Tol) ? 1 : 0,
					TexCorruptPure::Uniform::IsUniformUnder(ERule::NormalXY, S, Tol) ? 1 : 0,
					TexCorruptPure::Uniform::IsUniformUnder(ERule::NormalY, S, Tol) ? 1 : 0, Tol);
			}
		}

		EVerdict Query(UTexture2D* Tex, int32 FirstMip, int32 W, int32 H, ERule Rule, FString& OutDetail)
		{
#if !ANOMALY_SHADERS
			OutDetail = TEXT("not_built");
			return EVerdict::NotChecked;
#else
			if (!Tex || Rule == ERule::None)
			{
				return EVerdict::NotChecked;
			}
			const FKey K{ Tex, FirstMip };
			FEntry* E = Cache().Find(K);
			if (!E || E->Weak.Get() != Tex)
			{
				Enqueue(Tex, FirstMip, W, H);
				return EVerdict::Pending;
			}
			switch (E->State)
			{
			case EState::Queued:
			case EState::InFlight:
				return EVerdict::Pending;
			case EState::Failed:
				OutDetail = E->Fail;
				return EVerdict::Unmeasurable;
			default:
				break;
			}
			return TexCorruptPure::Uniform::IsUniformUnder(Rule, E->Stats, TexCorruptPure::Uniform::TolRaw) ? EVerdict::Uniform : EVerdict::Varying;
#endif
		}

		bool Kick(UTexture2D* Tex)
		{
#if !ANOMALY_SHADERS
			return false;
#else
			int32 First = 0;
			int32 W = 0;
			int32 H = 0;
			if (!Tex || Tex->IsCurrentlyVirtualTextured() || !ResidentTop(Tex, First, W, H))
			{
				return false;
			}
			return Enqueue(Tex, First, W, H);
#endif
		}

		int32 KickForActor(AActor* Actor)
		{
			int32 Added = 0;
			if (!Actor)
			{
				return 0;
			}
			TInlineComponentArray<UMeshComponent*> Meshes(Actor);
			TSet<UTexture2D*> Seen;
			for (UMeshComponent* Mesh : Meshes)
			{
				if (!Mesh)
				{
					continue;
				}
				for (int32 I = 0; I < Mesh->GetNumMaterials(); ++I)
				{
					UMaterialInterface* Mat = Mesh->GetMaterial(I);
					if (!Mat)
					{
						continue;
					}
					TArray<UTexture*> Used;
					Mat->GetUsedTextures(Used, EMaterialQualityLevel::Num, true, GMaxRHIFeatureLevel, true);
					for (UTexture* T : Used)
					{
						UTexture2D* T2 = Cast<UTexture2D>(T);
						if (T2 && !Seen.Contains(T2))
						{
							Seen.Add(T2);
							Added += Kick(T2) ? 1 : 0;
						}
					}
				}
			}
			return Added;
		}

		void Pump()
		{
			TArray<FPublished> Got;
			{
				FScopeLock Lock(&PublishedCS());
				Got = MoveTemp(Published());
				Published().Reset();
			}
			for (FPublished& P : Got)
			{
				const FKey* KP = IdToKey().Find(P.Id);
				if (!KP)
				{
					continue;
				}
				const FKey K = *KP;
				IdToKey().Remove(P.Id);
				GInFlight = FMath::Max(0, GInFlight - 1);
				FEntry* E = Cache().Find(K);
				if (!E || E->Id != P.Id)
				{
					continue;
				}
				if (P.bOk && TexCorruptPure::Uniform::DecodeStats(P.Raw, (uint32)E->W * (uint32)E->H, E->Stats))
				{
					E->State = EState::Done;
					++GMeasured;
					if (TexCorruptPure::Uniform::IsUniformUnder(ERule::AnyChannelRange, E->Stats, TexCorruptPure::Uniform::TolRaw))
					{
						++GUniformUv;
					}
					LogMeasured(*E, K.FirstMip);
				}
				else if (E->Retries < MaxRetries)
				{
					++E->Retries;
					E->State = EState::Queued;
					Queue().Add(K);
				}
				else
				{
					E->State = EState::Failed;
					E->Fail = P.bOk ? FString(TEXT("decode")) : P.Fail;
					++GFailed;
					UE_LOG(LogAnomaly, Warning,
						TEXT("texcorrupt: UNIFORM-STATS '%s' resident top mip %d (%dx%d) could not be measured (%s). A slot whose every ")
						TEXT("required texture is unmeasured or uniform is refused texture_uniform; an unmeasured texture is never assumed to vary."),
						*E->Name, K.FirstMip, E->W, E->H, *E->Fail);
				}
			}

#if ANOMALY_SHADERS
			int32 Kicks = 0;
			while (Queue().Num() > 0 && Kicks < MaxKicksPerPump && GInFlight < MaxInFlight)
			{
				const FKey K = Queue()[0];
				Queue().RemoveAt(0);
				FEntry* E = Cache().Find(K);
				if (!E || E->State != EState::Queued)
				{
					continue;
				}
				UTexture2D* Tex = E->Weak.Get();
				if (!Tex)
				{
					Cache().Remove(K);
					continue;
				}
				FTextureResource* Res = Tex->GetResource();
				if (!Res)
				{
					E->State = EState::Failed;
					E->Fail = TEXT("no_resource");
					++GFailed;
					continue;
				}
				E->State = EState::InFlight;
				E->Id = GNextId++;
				IdToKey().Add(E->Id, K);
				++GInFlight;
				++Kicks;
				const uint64 Id = E->Id;
				const int32 W = E->W;
				const int32 H = E->H;
				ENQUEUE_RENDER_COMMAND(AnomalyTexStatsKick)([Res, Id, W, H](FRHICommandListImmediate& RHICmdList)
				{
					KickRT(RHICmdList, Res, Id, W, H);
				});
			}
			if (GInFlight > 0)
			{
				ENQUEUE_RENDER_COMMAND(AnomalyTexStatsPoll)([](FRHICommandListImmediate&)
				{
					PollRT();
				});
			}
#endif
		}

		int32 NumPending()
		{
			int32 N = 0;
			for (const TPair<FKey, FEntry>& P : Cache())
			{
				if (P.Value.State == EState::Queued || P.Value.State == EState::InFlight)
				{
					++N;
				}
			}
			return N;
		}

		int32 NumMeasured()
		{
			return GMeasured;
		}

		void ResetCache()
		{
			Queue().Reset();
			for (TPair<FKey, FEntry>& P : Cache())
			{
				if (P.Value.State == EState::Queued)
				{
					P.Value.State = EState::Failed;
					P.Value.Fail = TEXT("reset");
				}
			}
		}

		FString DescribeCounters()
		{
			return FString::Printf(TEXT("textures=%d measured=%d uniform_no_spatial_variation=%d failed=%d pending=%d in_flight=%d"),
				Cache().Num(), GMeasured, GUniformUv, GFailed, NumPending(), GInFlight);
		}
	}

	int32 KickUniformMeasurements(UWorld* World, const FString& TargetName)
	{
		if (!World)
		{
			return 0;
		}
		int32 Added = 0;
		const TArray<TWeakObjectPtr<AActor>> Actors = TargetName.IsEmpty()
			? AnomalyViewport::GetVisibleRenderableActorsReadOnly(World)
			: AnomalyTargeting::FindActorsMatching(World, TargetName.StartsWith(TEXT("=")) ? TargetName : (TEXT("=") + TargetName));
		for (const TWeakObjectPtr<AActor>& Weak : Actors)
		{
			Added += Uniform::KickForActor(Weak.Get());
		}
		Uniform::Pump();
		return Added;
	}

	int32 NumUniformMeasurementsPending()
	{
		return Uniform::NumPending();
	}

	FString DescribeUniformMeasurements()
	{
		return Uniform::DescribeCounters();
	}
}
