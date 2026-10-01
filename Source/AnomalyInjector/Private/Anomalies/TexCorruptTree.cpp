#include "Anomalies/TexCorruptCore.h"
#include "Anomalies/TexCorruptPure.h"

#include "AnomalyAutoInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyLod.h"
#include "AnomalyStuckMipStats.h"
#include "AnomalyViewport.h"
#include "AnomalyDefaults.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/MeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/Ticker.h"
#include "EngineUtils.h"
#include "Engine/MapBuildDataRegistry.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkinnedAsset.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureDefines.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MaterialShared.h"
#include "RHI.h"
#include "RenderUtils.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "LocalVertexFactory.h"
#include "GPUSkinVertexFactory.h"
#include "Engine/InstancedStaticMesh.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "SceneInterface.h"
#include "StaticMeshResources.h"
#include "TextureResource.h"
#include "UObject/Package.h"

namespace AnomalyTexCorrupt
{
	namespace
	{
		constexpr int32 RankS3 = TexCorruptPure::Rank::S3;
		constexpr int32 RankS4 = TexCorruptPure::Rank::S4;
		constexpr int32 RankS5 = TexCorruptPure::Rank::S5;
		constexpr int32 RankS6 = TexCorruptPure::Rank::S6;
		constexpr int32 RankS7 = TexCorruptPure::Rank::S7;
		constexpr int32 RankS8 = TexCorruptPure::Rank::S8;
		constexpr int32 RankA2 = TexCorruptPure::Rank::A2;
		constexpr int32 RankA3 = TexCorruptPure::Rank::A3;
		constexpr int32 RankA5 = TexCorruptPure::Rank::A5;
		constexpr int32 RankA6 = TexCorruptPure::Rank::A6;
		constexpr int32 MaxParentDepth = 16;

		int32 ReadCVarInt(const TCHAR* Name, int32 Fallback)
		{
			if (IConsoleVariable* Var = IConsoleManager::Get().FindConsoleVariable(Name))
			{
				return Var->GetInt();
			}
			return Fallback;
		}

		float ReadCVarFloat(const TCHAR* Name, float Fallback)
		{
			if (IConsoleVariable* Var = IConsoleManager::Get().FindConsoleVariable(Name))
			{
				return Var->GetFloat();
			}
			return Fallback;
		}

		bool IsExcludedLodGroup(int32 Group)
		{
			switch (Group)
			{
			case TEXTUREGROUP_UI:
			case TEXTUREGROUP_Lightmap:
			case TEXTUREGROUP_Shadowmap:
			case TEXTUREGROUP_Terrain_Heightmap:
			case TEXTUREGROUP_Terrain_Weightmap:
			case TEXTUREGROUP_Bokeh:
				return true;
			default:
				return false;
			}
		}

		const TCHAR* LexTexType(EMaterialTextureParameterType Type)
		{
			switch (Type)
			{
			case EMaterialTextureParameterType::Standard2D: return TEXT("standard2d");
			case EMaterialTextureParameterType::Cube:       return TEXT("cube");
			case EMaterialTextureParameterType::Array2D:    return TEXT("array2d");
			case EMaterialTextureParameterType::ArrayCube:  return TEXT("arraycube");
			case EMaterialTextureParameterType::Volume:     return TEXT("volume");
			case EMaterialTextureParameterType::Virtual:    return TEXT("virtual");
			default:                                        return TEXT("unknown");
			}
		}

		const TCHAR* LexAssociation(EMaterialParameterAssociation A)
		{
			switch (A)
			{
			case EMaterialParameterAssociation::LayerParameter: return TEXT("layer");
			case EMaterialParameterAssociation::BlendParameter: return TEXT("blend");
			default:                                            return TEXT("global");
			}
		}

		bool FindRuntimeLink(UMaterialInterface* Start, const TCHAR* FirstWhere, FString& OutWhere, FString& OutKind)
		{
			const TCHAR* Kind = TEXT("");
			auto IsRuntime = [&Kind](UMaterialInterface* Link) -> bool
			{
				if (Link->IsA<UMaterialInstanceDynamic>())
				{
					Kind = TEXT("mid");
					return true;
				}
				if (Link->GetOutermost() == GetTransientPackage())
				{
					Kind = TEXT("transient_outer");
					return true;
				}
				if (!Link->HasAnyFlags(RF_WasLoaded))
				{
					Kind = TEXT("not_loaded");
					return true;
				}
				return false;
			};
			auto ParentOf = [](UMaterialInterface* Link) -> UMaterialInterface*
			{
				const UMaterialInstance* Instance = Cast<UMaterialInstance>(Link);
				return Instance ? Instance->Parent.Get() : nullptr;
			};
			int32 Depth = 0;
			const TexCorruptPure::EChainWalk Walk = TexCorruptPure::WalkChain(Start, MaxParentDepth, Depth, IsRuntime, ParentOf);
			if (Walk == TexCorruptPure::EChainWalk::Clean)
			{
				return false;
			}
			OutWhere = Depth == 0 ? FirstWhere : TEXT("parent");
			OutKind = Walk == TexCorruptPure::EChainWalk::RuntimeLink ? FString(Kind)
				: FString::Printf(TEXT("chain_limit_%d_unverified_tail"), MaxParentDepth);
			return true;
		}

		bool UsesNaniteProxy(const UStaticMeshComponent* SMC)
		{
			if (!SMC || SMC->bDisallowNanite || SMC->GetScene() == nullptr)
			{
				return false;
			}
#if WITH_EDITORONLY_DATA
			if (SMC->bDisplayNaniteFallbackMesh)
			{
				return false;
			}
#endif
			return UseNanite(SMC->GetScene()->GetShaderPlatform()) && SMC->HasValidNaniteData();
		}

		struct FUsageName
		{
			unsigned Bit;
			EMaterialUsage Usage;
			const TCHAR* Name;
		};

		bool FindUsageRefusal(const FSlot& S, UMaterial* Root, FString& OutSub)
		{
			TexCorruptPure::FUsageFacts F;
			TArray<bool> Lit;
			TArray<bool> HasBuild;
			TArray<bool> UsesSlot;
			if (S.bSkinned)
			{
				F.Kind = TexCorruptPure::EMeshKind::Skinned;
				const USkinnedMeshComponent* SK = Cast<USkinnedMeshComponent>(S.Comp);
				USkinnedAsset* Asset = SK ? SK->GetSkinnedAsset() : nullptr;
				FSkeletalMeshRenderData* RD = Asset ? Asset->GetResourceForRendering() : nullptr;
				F.bRenderData = RD != nullptr;
				if (RD)
				{
					for (int32 L = 0; L < RD->LODRenderData.Num(); ++L)
					{
						const FSkeletalMeshLODInfo* Info = Asset->GetLODInfo(L);
						const TArray<FSkelMeshRenderSection>& Sections = RD->LODRenderData[L].RenderSections;
						for (int32 SectionIndex = 0; SectionIndex < Sections.Num(); ++SectionIndex)
						{
							if (!Sections[SectionIndex].HasClothingData())
							{
								continue;
							}
							int32 Use = Sections[SectionIndex].MaterialIndex;
							if (Info && SectionIndex < Info->LODMaterialMap.Num() && Asset->IsValidMaterialIndex(Info->LODMaterialMap[SectionIndex]))
							{
								Use = FMath::Clamp(Info->LODMaterialMap[SectionIndex], 0, Asset->GetNumMaterials());
							}
							F.bSlotHasClothSection |= Use == S.SlotIndex;
						}
					}
					F.bHasMorphTargets = Asset->GetMorphTargets().Num() > 0;
				}
			}
			else if (S.bStatic)
			{
				F.Kind = TexCorruptPure::EMeshKind::Static;
				const UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(S.Comp);
				const UStaticMesh* Mesh = SMC ? SMC->GetStaticMesh() : nullptr;
				const FStaticMeshRenderData* RD = Mesh ? Mesh->GetRenderData() : nullptr;
				F.bRenderData = SMC && RD && RD->LODResources.Num() > 0;
				if (F.bRenderData)
				{
					F.bInstanced = SMC->IsA<UInstancedStaticMeshComponent>();
					F.bSpline = SMC->IsA<USplineMeshComponent>();
					F.bNanite = UsesNaniteProxy(SMC);
					F.bForceVolumetric = SMC->LightmapType == ELightmapType::ForceVolumetric;
					F.bLodsShareLighting = RD->bLODsShareStaticLighting;
					const int32 NumLods = RD->LODResources.Num();
					Lit.SetNumZeroed(NumLods);
					HasBuild.SetNumZeroed(NumLods);
					UsesSlot.SetNumZeroed(NumLods);
					for (int32 L = 0; L < NumLods; ++L)
					{
						const FMeshMapBuildData* BD = L < SMC->LODData.Num() ? SMC->GetMeshMapBuildData(SMC->LODData[L]) : nullptr;
						HasBuild[L] = BD != nullptr;
						Lit[L] = BD && (BD->LightMap.IsValid() || BD->ShadowMap.IsValid());
						for (const FStaticMeshSection& Section : RD->LODResources[L].Sections)
						{
							UsesSlot[L] = UsesSlot[L] || Section.MaterialIndex == S.SlotIndex;
						}
					}
					F.NumLods = NumLods;
					F.LodDataLit = Lit.GetData();
					F.LodDataHasBuildData = HasBuild.GetData();
					F.LodUsesSlot = UsesSlot.GetData();
				}
			}

			TexCorruptPure::EUsageGap Gap = TexCorruptPure::EUsageGap::None;
			const unsigned Required = TexCorruptPure::RequiredUsages(F, Gap);
			if (Gap != TexCorruptPure::EUsageGap::None)
			{
				OutSub = FString::Printf(TEXT("usage_undetermined:%s"), ANSI_TO_TCHAR(TexCorruptPure::LexUsageGap(Gap)));
				return true;
			}
			if (!Root)
			{
				OutSub = TEXT("usage_undetermined:no_root_material");
				return true;
			}
			if (Root->MaterialDomain != MD_Surface)
			{
				return false;
			}
			static const FUsageName Order[TexCorruptPure::Usage::Count] = {
				{ TexCorruptPure::Usage::SkeletalMesh, MATUSAGE_SkeletalMesh, TEXT("skeletal_mesh") },
				{ TexCorruptPure::Usage::Clothing, MATUSAGE_Clothing, TEXT("clothing") },
				{ TexCorruptPure::Usage::MorphTargets, MATUSAGE_MorphTargets, TEXT("morph_targets") },
				{ TexCorruptPure::Usage::InstancedStaticMeshes, MATUSAGE_InstancedStaticMeshes, TEXT("instanced_static_meshes") },
				{ TexCorruptPure::Usage::Nanite, MATUSAGE_Nanite, TEXT("nanite") },
				{ TexCorruptPure::Usage::SplineMesh, MATUSAGE_SplineMesh, TEXT("spline_mesh") },
				{ TexCorruptPure::Usage::StaticLighting, MATUSAGE_StaticLighting, TEXT("static_lighting") },
			};
			for (const FUsageName& U : Order)
			{
				if ((Required & U.Bit) == 0)
				{
					continue;
				}
				bool bHas = true;
				Root->NeedsSetMaterialUsage_Concurrent(bHas, U.Usage);
				if (!bHas)
				{
					OutSub = U.Name;
					return true;
				}
			}
			return false;
		}

		void GatherTextureFacts(FBinding& B)
		{
			UTexture2D* Tex = B.Tex2D;
			if (!Tex)
			{
				return;
			}
			B.bSRGB = Tex->SRGB != 0;
			B.bNormalMap = Tex->IsNormalMap();
			B.LODGroup = (int32)Tex->LODGroup;
			B.CinematicMips = Tex->NumCinematicMipLevels;
			B.CachedLODBias = Tex->GetCachedLODBias();
#if WITH_EDITOR
			B.bDefaultTexture = Tex->IsDefaultTexture();
#endif
			if (B.bDefaultTexture)
			{
				return;
			}
			if (const FTexturePlatformData* PD = Tex->GetPlatformData())
			{
				B.Format = PD->PixelFormat;
				B.M = PD->Mips.Num();
				if (B.M > 0)
				{
					B.W = PD->Mips[0].SizeX;
					B.H = PD->Mips[0].SizeY;
				}
				for (int32 m = 0; m < B.M; ++m)
				{
					B.LevelSizes.Add(FIntPoint(PD->Mips[m].SizeX, PD->Mips[m].SizeY));
				}
			}
			const FStreamableRenderResourceState& St = Tex->GetStreamableResourceState();
			if (St.IsValid())
			{
				B.ResidentLODs = St.NumResidentLODs;
				B.MaxLODs = St.MaxNumLODs;
				B.NonOptionalLODs = St.NumNonOptionalLODs;
				B.AssetLODBias = St.AssetLODBias;
				B.bStreams = St.bSupportsStreaming != 0;
			}
		}

		void FailBinding(FBinding& B, int32 Step, const TCHAR* InReason, const FString& Sub = FString())
		{
			B.Step = Step;
			B.Reason = InReason;
			B.Sub = Sub;
		}

		TexCorruptPure::EFmt ToPureFormat(EPixelFormat Format)
		{
			switch (Format)
			{
			case PF_DXT1:     return TexCorruptPure::EFmt::DXT1;
			case PF_DXT5:     return TexCorruptPure::EFmt::DXT5;
			case PF_BC7:      return TexCorruptPure::EFmt::BC7;
			case PF_B8G8R8A8: return TexCorruptPure::EFmt::BGRA8;
			case PF_BC4:      return TexCorruptPure::EFmt::BC4;
			case PF_G8:       return TexCorruptPure::EFmt::G8;
			case PF_BC5:      return TexCorruptPure::EFmt::BC5;
			default:          return TexCorruptPure::EFmt::Other;
			}
		}

		EClass ClassifyFormat(const FBinding& B, FString& OutSub)
		{
			const FString Fmt = GetPixelFormatString(B.Format);
			TexCorruptPure::EClassWhy Cause = TexCorruptPure::EClassWhy::Ok;
			switch (TexCorruptPure::ClassifyFormat(ToPureFormat(B.Format), B.bSRGB, B.bNormalMap, Cause))
			{
			case TexCorruptPure::EClassP::Colour: return EClass::Colour;
			case TexCorruptPure::EClassP::Data:   return EClass::Data;
			case TexCorruptPure::EClassP::Normal: return EClass::Normal;
			default:
				break;
			}
			switch (Cause)
			{
			case TexCorruptPure::EClassWhy::NormalMapNotBc5:  OutSub = FString::Printf(TEXT("normal_map_in_%s"), *Fmt); break;
			case TexCorruptPure::EClassWhy::Bc5NotNormalMap:  OutSub = TEXT("bc5_not_normal_map"); break;
			case TexCorruptPure::EClassWhy::SrgbSingleChannel: OutSub = FString::Printf(TEXT("srgb_single_channel_%s"), *Fmt); break;
			default:                                           OutSub = Fmt; break;
			}
			return EClass::None;
		}

		void EvaluateBinding(FBinding& B)
		{
			B.Step = 0;
			B.Reason = Why::Transformable;
			B.Sub.Reset();

			if (B.Type == EMaterialTextureParameterType::Virtual || (B.Tex2D && B.Tex2D->IsCurrentlyVirtualTextured()))
			{
				FailBinding(B, 1, Why::VirtualTexture, LexTexType(B.Type));
				return;
			}
			if (B.Type != EMaterialTextureParameterType::Standard2D || !B.Tex2D)
			{
				FailBinding(B, 2, Why::UnsupportedType,
					B.Type != EMaterialTextureParameterType::Standard2D ? FString(LexTexType(B.Type))
						: (B.Texture ? B.Texture->GetClass()->GetName() : FString(TEXT("null_texture"))));
				return;
			}
			if (IsExcludedLodGroup(B.LODGroup))
			{
				FailBinding(B, 3, Why::ExcludedGroup, FString::Printf(TEXT("group_%d"), B.LODGroup));
				return;
			}
			if (B.ParamName.IsNone())
			{
				FailBinding(B, 4, Why::TextureNotParameter);
				return;
			}
			if (B.bDefaultTexture)
			{
				FailBinding(B, 5, Why::ResourceNotReady, TEXT("compiling"));
				return;
			}
			if (!B.Tex2D->GetPlatformData())
			{
				FailBinding(B, 5, Why::ResourceNotReady, TEXT("no_platform_data"));
				return;
			}
			{
				FString Sub;
				B.Class = ClassifyFormat(B, Sub);
				if (B.Class == EClass::None)
				{
					FailBinding(B, 5, Why::UnsupportedEncoding, Sub);
					return;
				}
			}
			{
				TArray<int32> LevelW;
				TArray<int32> LevelH;
				for (const FIntPoint& L : B.LevelSizes)
				{
					LevelW.Add(L.X);
					LevelH.Add(L.Y);
				}
				const bool bShapeOk = B.LevelSizes.Num() == B.M
					&& TexCorruptPure::IsAdmittedChainShape(B.M, B.W, B.H, LevelW.GetData(), LevelH.GetData());
				if (!bShapeOk)
				{
					FailBinding(B, 6, Why::MipChainShape, FString::Printf(TEXT("M%d_%dx%d"), B.M, B.W, B.H));
					return;
				}
			}
			if (AnomalyStuckMip::IsTextureHeldOrRestoring(B.Tex2D))
			{
				FailBinding(B, 7, Why::HeldByStuckLowMip);
				return;
			}
			if (!B.Tex2D->GetResource() || B.Tex2D->HasPendingRenderResourceInitialization())
			{
				FailBinding(B, 8, Why::ResourceNotReady, TEXT("resource"));
				return;
			}
			if (B.Tex2D->HasPendingInitOrStreaming(false))
			{
				FailBinding(B, 8, Why::StreamingPending);
				return;
			}
			const FStreamableRenderResourceState& St = B.Tex2D->GetStreamableResourceState();
			if (!St.IsValid())
			{
				FailBinding(B, 8, Why::ResourceNotReady, TEXT("invalid_state"));
				return;
			}
			if (B.CinematicMips > 0)
			{
				FailBinding(B, 9, Why::RuntimeLodBias, TEXT("cinematic"));
				return;
			}
			if ((int32)St.AssetLODBias > 0 || (B.CachedLODBias - B.CinematicMips - (int32)St.AssetLODBias) > 0)
			{
				FailBinding(B, 9, Why::RuntimeLodBias, TEXT("per_texture"));
				return;
			}
			if (TexCorruptPure::StreamingBudgetPossible(St.bSupportsStreaming != 0,
				ReadCVarInt(TEXT("r.Streaming.UsePerTextureBias"), 1), ReadCVarFloat(TEXT("r.Streaming.MipBias"), 0.0f)))
			{
				FailBinding(B, 9, Why::RuntimeLodBias, TEXT("streaming_budget"));
				return;
			}
			if ((int32)St.MaxNumLODs != B.M)
			{
				FailBinding(B, 10, Why::NotFullyResident,
					(int32)St.MaxNumLODs < B.M ? TEXT("max_below_cooked") : TEXT("max_above_cooked"));
				return;
			}
			if (St.NumResidentLODs != St.MaxNumLODs)
			{
				const bool bOptional = St.NumNonOptionalLODs < St.MaxNumLODs && St.NumResidentLODs <= St.NumNonOptionalLODs;
				FailBinding(B, 10, Why::NotFullyResident, bOptional ? TEXT("optional_not_resident") : TEXT(""));
				return;
			}
		}

		void FailSlot(FSlot& S, int32 Rank, const FString& InReason, const FString& Sub = FString())
		{
			S.Step = Rank;
			S.Reason = InReason;
			S.Sub = Sub;
		}

		void EvaluateSlot(UWorld* World, const FTreeInputs& In, FSlot& S)
		{
			if (!S.Resolved)
			{
				S.bUntouched = true;
				S.Step = TexCorruptPure::Rank::S1;
				S.Reason = Why::SlotEmpty;
				return;
			}
			if (IsTranslucentBlendMode(S.Resolved->GetBlendMode()))
			{
				S.bUntouched = true;
				S.Step = TexCorruptPure::Rank::S2;
				S.Reason = Why::SlotTranslucent;
				return;
			}
			if (FindRuntimeLink(S.Resolved, S.Raw ? TEXT("override") : TEXT("asset_slot"), S.Where, S.Kind)
				|| (S.Effective && S.Effective != S.Resolved && FindRuntimeLink(S.Effective, TEXT("effective"), S.Where, S.Kind)))
			{
				FailSlot(S, RankS3, Why::HostMid, FString::Printf(TEXT("%s:%s"), *S.Where, *S.Kind));
				return;
			}
			if (S.bStatic)
			{
				const UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(S.Comp);
				if (SMC && SMC->UseNaniteOverrideMaterials() && S.Resolved->GetNaniteOverride() != nullptr)
				{
					FailSlot(S, RankS4, Why::NaniteOverride, GetNameSafe(S.Resolved->GetNaniteOverride()));
					return;
				}
			}

			bool bResOk = false;
			bool bSmOk = false;
			bool bComplete = false;
			FHostDrawReadiness Readiness;
			ReadActiveBindings(World, S.Resolved, S.Comp, S.Bindings, bResOk, bSmOk, bComplete, &Readiness);
			if (!bResOk || !bSmOk)
			{
				FailSlot(S, RankS5, Why::ShaderMapUnavailable, bResOk ? TEXT("no_shader_map") : TEXT("no_resource"));
				return;
			}

			UMaterial* Root = S.Resolved->GetMaterial();
			{
				FString UsageSub;
				if (FindUsageRefusal(S, Root, UsageSub))
				{
					FailSlot(S, RankS7, Why::DefaultMaterialPath, UsageSub);
					return;
				}
			}

			if (!bComplete)
			{
				const bool bPending = Readiness.Verdict == TexCorruptPure::EDrawReadiness::CompilePending;
				FailSlot(S, RankS6, bPending ? Why::ShaderMapIncomplete : Why::DrawShadersMissing,
					FString::Printf(TEXT("%s:%s"), *Readiness.VertexFactory, UTF8_TO_TCHAR(TexCorruptPure::LexDrawReadiness(Readiness.Verdict))));
				return;
			}

			if (S.Bindings.Num() == 0)
			{
				FailSlot(S, RankS8, Why::NoTextures);
				return;
			}

			for (FBinding& B : S.Bindings)
			{
				GatherTextureFacts(B);
				EvaluateBinding(B);
			}

			int32 RequiredCount = 0;
			for (FBinding& B : S.Bindings)
			{
				const bool bInFamily = (In.Family == EFamily::UV) || (B.Tex2D && B.bNormalMap);
				B.bRequired = bInFamily;
				if (bInFamily && !B.IsTransformable() && B.Tex2D && TexCorruptPure::IsNonSpatial(B.M, B.W, B.H))
				{
					B.bRequired = false;
					B.bNonSpatialExempt = true;
				}
				if (B.bRequired)
				{
					++RequiredCount;
				}
			}

			if (In.Family == EFamily::Normal && RequiredCount == 0)
			{
				FailSlot(S, RankA2, Why::NoNormalMap);
				return;
			}
			if (In.Family == EFamily::Normal && (!Root || !Root->IsPropertyConnected(MP_Normal)))
			{
				FailSlot(S, RankA3, Why::NormalUnconnected);
				return;
			}

			int32 Transformable = 0;
			TArray<int32> Steps;
			TArray<bool> Required;
			for (const FBinding& B : S.Bindings)
			{
				Steps.Add(B.Step);
				Required.Add(B.bRequired);
				if (B.bRequired && B.IsTransformable())
				{
					++Transformable;
				}
			}
			const int32 FailIndex = TexCorruptPure::PickFirstFailing(Steps.GetData(), Required.GetData(), Steps.Num());
			const FBinding* FirstFail = S.Bindings.IsValidIndex(FailIndex) ? &S.Bindings[FailIndex] : nullptr;
			if (FirstFail)
			{
				S.bPartial = Transformable > 0;
				FailSlot(S, TexCorruptPure::Rank::ForBindingStep(FirstFail->Step), FirstFail->Reason, FirstFail->Sub);
				return;
			}

			const int32 MinPx = GetMinTexturePx();
			bool bAnyAtPolicy = false;
			for (const FBinding& B : S.Bindings)
			{
				if (B.bRequired && B.W >= MinPx && B.H >= MinPx)
				{
					bAnyAtPolicy = true;
					break;
				}
			}
			if (!bAnyAtPolicy)
			{
				FailSlot(S, RankA5, Why::BelowSizePolicy, FString::Printf(TEXT("min_%d"), MinPx));
				return;
			}
			if (RequiredCount > GetMaxTextures())
			{
				FailSlot(S, RankA6, Why::MapSetOverCap, FString::Printf(TEXT("%d_of_%d"), RequiredCount, GetMaxTextures()));
				return;
			}
			S.Step = 0;
			S.Reason = Why::Qualified;
		}

		void ComputeRequirement(FTreeResult& Out)
		{
			TArray<TexCorruptPure::FReqTex> Req;
			for (const FSlot& S : Out.Slots)
			{
				if (!S.IsQualified())
				{
					continue;
				}
				for (const FBinding& B : S.Bindings)
				{
					if (B.bRequired && B.IsTransformable() && B.Tex2D)
					{
						TexCorruptPure::FReqTex& R = Req.AddDefaulted_GetRef();
						R.Id = (long long)(UPTRINT)B.Tex2D;
						R.W = B.W;
						R.H = B.H;
						R.M = B.M;
						R.bSRGB = (B.Class == EClass::Colour);
					}
				}
			}
			const TexCorruptPure::FRequirement R = TexCorruptPure::EventRequirement(Req.GetData(), Req.Num());
			Out.DistinctTextures = R.DistinctTextures;
			Out.ScratchClasses = R.ScratchClasses;
			Out.RequiredBytes = (int64)R.Total();
		}
	}
	UAnomalyInjectorSubsystem* ResolveInjector(UWorld* World)
	{
		return World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
	}

	bool CheckCorruptorContract(UMaterialInterface* Corruptor, const TArray<FName>& Scalars, const TArray<FName>& Textures,
		FString& OutMissing)
	{
		OutMissing.Reset();
		if (!Corruptor)
		{
			OutMissing = TEXT("corruptor_null");
			return false;
		}
		for (const FName& Name : Scalars)
		{
			float Value = 0.0f;
			if (!Corruptor->GetScalarParameterDefaultValue(FHashedMaterialParameterInfo(Name), Value))
			{
				OutMissing = Name.ToString();
				return false;
			}
		}
		for (const FName& Name : Textures)
		{
			UTexture* Value = nullptr;
			if (!Corruptor->GetTextureParameterDefaultValue(FHashedMaterialParameterInfo(Name), Value))
			{
				OutMissing = Name.ToString();
				return false;
			}
		}
		return true;
	}

	FHostDrawReadiness ReadHostDrawReadiness(UMaterialInterface* Material, const UPrimitiveComponent* Comp, UWorld* World)
	{
		FHostDrawReadiness Out;
		if (!Material || !World)
		{
			return Out;
		}
		FMaterialResource* Res = Material->GetMaterialResource(World->FeatureLevel);
		const FMaterialShaderMap* Map = Res ? Res->GetGameThreadShaderMap() : nullptr;
		if (!Map)
		{
			return Out;
		}
		Out.bWholeMapComplete = Res->IsGameThreadShaderMapComplete();
#if WITH_EDITOR
		Out.bCompileFinished = Res->IsCompilationFinished();
#else
		Out.bCompileFinished = true;
#endif
		const FVertexFactoryType* Types[3] = { nullptr, nullptr, nullptr };
		if (Cast<UInstancedStaticMeshComponent>(Comp))
		{
			Types[0] = &FInstancedStaticMeshVertexFactory::StaticType;
		}
		else if (Cast<USkinnedMeshComponent>(Comp))
		{
			Types[0] = &TGPUSkinVertexFactory<GPUSkinBoneInfluenceType::DefaultBoneInfluence>::StaticType;
			Types[1] = &TGPUSkinVertexFactory<GPUSkinBoneInfluenceType::UnlimitedBoneInfluence>::StaticType;
			Types[2] = FVertexFactoryType::GetVFByName(FHashedName(TEXT("FGPUSkinPassthroughVertexFactory")));
		}
		else if (Cast<USplineMeshComponent>(Comp))
		{
			Types[0] = FVertexFactoryType::GetVFByName(FHashedName(TEXT("FSplineMeshVertexFactory")));
		}
		else
		{
			Types[0] = &FLocalVertexFactory::StaticType;
		}
		int64 BestScore = -1;
		for (const FVertexFactoryType* Type : Types)
		{
			if (!Type)
			{
				continue;
			}
			const FMeshMaterialShaderMap* Mesh = Map->GetMeshShaderMap(Type);
			if (!Mesh)
			{
				continue;
			}
			int32 Shaders = 0;
			int32 Vs = 0;
			int32 Ps = 0;
			for (const TMemoryImagePtr<FShader>& Ptr : Mesh->GetShaders())
			{
				const FShader* Shader = Ptr.Get();
				if (!Shader)
				{
					continue;
				}
				++Shaders;
				const FShaderType* ShaderType = Shader->GetType(Map->GetPointerTable());
				if (!ShaderType)
				{
					continue;
				}
				const FTCHARToUTF8 TypeName(ShaderType->GetName());
				const int Kind = TexCorruptPure::ClassifyBasePassShaderTypeName(TypeName.Get());
				Vs += Kind == 1 ? 1 : 0;
				Ps += Kind == 2 ? 1 : 0;
			}
			const int64 Score = (Vs > 0 && Ps > 0 ? (int64)1 << 32 : 0) + ((int64)FMath::Min(Vs, Ps) << 16) + Shaders;
			if (Score > BestScore)
			{
				BestScore = Score;
				Out.VertexFactory = Type->GetName();
				Out.VertexFactoryShaders = Shaders;
				Out.BasePassVertexShaders = Vs;
				Out.BasePassPixelShaders = Ps;
			}
		}
		Out.Verdict = TexCorruptPure::JudgeDrawReadiness(Out.bCompileFinished, Out.VertexFactoryShaders > 0, Out.BasePassVertexShaders,
			Out.BasePassPixelShaders);
		static TSet<FString> Reported;
		if (!AnomalyViewport::IsReadOnlyEnumeration() && Reported.Num() < 256)
		{
			bool bAlready = false;
			Reported.Add(FString::Printf(TEXT("%s|%d"), *Material->GetPathName(), (int32)Out.Verdict), &bAlready);
			if (!bAlready)
			{
				const FString VerdictText = Out.IsReady() ? FString(TEXT("admitted"))
					: FString::Printf(TEXT("refused %s"), UTF8_TO_TCHAR(TexCorruptPure::LexDrawReadiness(Out.Verdict)));
				UE_LOG(LogAnomaly, Log,
					TEXT("TEXCORRUPT-SHADERMAP '%s' game_thread_complete=%d compilation_finished=%d vertex_factory=%s shaders=%d ")
					TEXT("base_pass_vs=%d base_pass_ps=%d -> %s. The whole-map flag is reported, never decided on: it stays 0 in the ")
					TEXT("editor for materials that render, and it is cached for the usages the map was compiled with. The base pass ")
					TEXT("draws a mesh with its material's vertex and pixel shaders for the component's vertex factory or falls back ")
					TEXT("to the default material (BasePassRendering.cpp:507 TryGetShaders), so those decide; an unfinished compile ")
					TEXT("refuses as shader_map_incomplete, missing base-pass shaders as draw_shaders_missing."),
					*GetNameSafe(Material), Out.bWholeMapComplete ? 1 : 0, Out.bCompileFinished ? 1 : 0, *Out.VertexFactory,
					Out.VertexFactoryShaders, Out.BasePassVertexShaders, Out.BasePassPixelShaders, *VerdictText);
			}
		}
		return Out;
	}

	bool CorruptorShadersReady(UMaterialInterface* Corruptor, UWorld* World)
	{
		if (!Corruptor || !World)
		{
			return false;
		}
		auto Complete = [Corruptor, World]()
		{
			FMaterialResource* Res = Corruptor->GetMaterialResource(World->FeatureLevel);
			return Res && Res->GetGameThreadShaderMap() && Res->IsGameThreadShaderMapComplete();
		};
		if (Complete())
		{
			return true;
		}
#if WITH_EDITOR
		Corruptor->EnsureIsComplete();
		return Complete();
#else
		return false;
#endif
	}

	void ReadActiveBindings(UWorld* World, UMaterialInterface* Resolved, const UPrimitiveComponent* Comp, TArray<FBinding>& OutBindings,
		bool& bOutResourceOk, bool& bOutShaderMapOk, bool& bOutComplete, FHostDrawReadiness* OutReadiness)
	{
		OutBindings.Reset();
		bOutResourceOk = false;
		bOutShaderMapOk = false;
		bOutComplete = false;
		if (!World || !Resolved)
		{
			return;
		}
		FMaterialResource* Res = Resolved->GetMaterialResource(World->FeatureLevel);
		if (!Res)
		{
			return;
		}
		bOutResourceOk = true;
		FMaterialShaderMap* ShaderMap = Res->GetGameThreadShaderMap();
		if (!ShaderMap)
		{
			return;
		}
		bOutShaderMapOk = true;
		const FHostDrawReadiness Readiness = ReadHostDrawReadiness(Resolved, Comp, World);
		if (OutReadiness)
		{
			*OutReadiness = Readiness;
		}
		if (!Readiness.IsReady())
		{
			return;
		}
		bOutComplete = true;

		const FUniformExpressionSet& Set = ShaderMap->GetUniformExpressionSet();
		for (uint32 t = 0; t < NumMaterialTextureParameterTypes; ++t)
		{
			const EMaterialTextureParameterType Type = (EMaterialTextureParameterType)t;
			const int32 Num = Set.GetNumTextures(Type);
			for (int32 i = 0; i < Num; ++i)
			{
				const FMaterialTextureParameterInfo& Entry = Set.GetTextureParameter(Type, i);
				FBinding& B = OutBindings.AddDefaulted_GetRef();
				B.Type = Type;
				B.Index = i;
				B.ParamName = Entry.GetParameterName();
				B.Info = FMaterialParameterInfo(Entry.ParameterInfo);
				UTexture* Tex = nullptr;
				Set.GetGameThreadTextureValue(Type, i, Resolved, *Res, Tex);
				B.Texture = Tex;
				B.Tex2D = Cast<UTexture2D>(Tex);
				B.TextureName = Tex ? Tex->GetPathName() : FString(TEXT("None"));
			}
		}
	}

	void EvaluateTree(UWorld* World, const FTreeInputs& In, FTreeResult& Out)
	{
		Out = FTreeResult();
		Out.Family = In.Family;
		Out.Mode = In.Mode;
		Out.TileN = In.TileN;
		Out.Attempt = In.Attempt;
		Out.TargetQuery = In.TargetQuery;

		auto RefuseEvent = [&Out](const TCHAR* InReason, const FString& Sub, const TCHAR* Step)
		{
			Out.bApply = false;
			Out.Reason = InReason;
			Out.Sub = Sub;
			Out.EventStep = Step;
		};

		UAnomalyInjectorSubsystem* Injector = ResolveInjector(World);
		UMaterialInterface* UvCorruptor = Injector ? Injector->GetTexCorruptUvMaterial() : nullptr;
		UMaterialInterface* NormalCorruptor = Injector ? Injector->GetTexCorruptNormalMaterial() : nullptr;
		UTexture2D* Noise = Injector ? Injector->GetTexCorruptNoiseTexture() : nullptr;

		if (!World || !Injector)
		{
			RefuseEvent(Why::AssetsUnavailable, TEXT("no_injector"), TEXT("E1"));
			return;
		}
		if (Levers().bForceMissingAsset)
		{
			RefuseEvent(Why::AssetsUnavailable, TEXT("bench_force_missing_asset"), TEXT("E1"));
			return;
		}
		if (!UvCorruptor || !NormalCorruptor || !Noise)
		{
			FString Missing;
			if (!UvCorruptor) { Missing += TEXT("M_CorruptTex_UV,"); }
			if (!NormalCorruptor) { Missing += TEXT("M_CorruptTex_Normal,"); }
			if (!Noise) { Missing += TEXT("T_CorruptTex_NoiseN,"); }
			Missing.RemoveFromEnd(TEXT(","));
			RefuseEvent(Why::AssetsUnavailable, FString::Printf(TEXT("missing:%s"), *Missing), TEXT("E1"));
			return;
		}
		{
			FString MissingParam;
			if (!CheckCorruptorContract(UvCorruptor, UvCorruptorScalars(), UvCorruptorTextures(), MissingParam))
			{
				RefuseEvent(Why::AssetsUnavailable, FString::Printf(TEXT("uv_corruptor_lacks:%s"), *MissingParam), TEXT("E1"));
				return;
			}
			if (!CheckCorruptorContract(NormalCorruptor, NormalCorruptorScalars(), NormalCorruptorTextures(), MissingParam))
			{
				RefuseEvent(Why::AssetsUnavailable, FString::Printf(TEXT("normal_corruptor_lacks:%s"), *MissingParam), TEXT("E1"));
				return;
			}
		}
		{
			UMaterialInterface* Needed = (In.Family == EFamily::UV) ? UvCorruptor : NormalCorruptor;
			FMaterialResource* Res = Needed->GetMaterialResource(World->FeatureLevel);
			if (!Res || !CorruptorShadersReady(Needed, World))
			{
				RefuseEvent(Why::CorruptorNotReady, GetNameSafe(Needed), TEXT("E2"));
				return;
			}
		}
		if (In.Family == EFamily::Normal && ReadCVarInt(TEXT("Compat.UseDXT5NormalMaps"), 0) != 0)
		{
			RefuseEvent(Why::Dxt5NormalHost, FString(), TEXT("E3"));
			return;
		}
		if (UTexture2D::GetGlobalMipMapLODBias() != 0.0f)
		{
			RefuseEvent(Why::RuntimeLodBias, TEXT("global_sampler"), TEXT("E4"));
			return;
		}
		if (TexCorruptPure::GlobalStreamingBias(ReadCVarInt(TEXT("r.Streaming.UsePerTextureBias"), 1), ReadCVarFloat(TEXT("r.Streaming.MipBias"), 0.0f)))
		{
			RefuseEvent(Why::RuntimeLodBias, TEXT("global_streaming"), TEXT("E5"));
			return;
		}
		if (!In.bCensus && In.Mode == EMode::None)
		{
			FString Sub = In.ModeRefusalSub;
			if (Sub.IsEmpty())
			{
				Sub = In.bModeArgGiven ? FString::Printf(TEXT("unknown:%s"), *In.ModeArg) : FString(TEXT("no_mode"));
			}
			RefuseEvent(Why::ModeInvalid, Sub, TEXT("E6"));
			return;
		}

		TArray<TWeakObjectPtr<UMeshComponent>> Meshes;
		if (AActor* Direct = In.TargetActor.Get())
		{
			TInlineComponentArray<UStaticMeshComponent*> Statics(Direct);
			for (UStaticMeshComponent* C : Statics)
			{
				Meshes.Add(C);
			}
			TInlineComponentArray<USkinnedMeshComponent*> Skinned(Direct);
			for (USkinnedMeshComponent* C : Skinned)
			{
				Meshes.Add(C);
			}
		}
		else
		{
			Meshes = AnomalyLod::ResolveLodComponents(World, In.TargetQuery);
		}
		if (UAnomalyInjectorSubsystem::IsViewportScopingEnabled(World))
		{
			FAnomalyViewInfo View;
			if (AnomalyViewport::GetActiveViewInfo(World, View))
			{
				Meshes = AnomalyViewport::FilterVisibleComponents(View, World, Meshes);
			}
		}
		Meshes.RemoveAll([](const TWeakObjectPtr<UMeshComponent>& W) { return !W.IsValid(); });
		if (Meshes.Num() == 0)
		{
			RefuseEvent(Why::NoMesh, FString(), TEXT("E7"));
			return;
		}
		if (!AnomalyDefaults::GetAllowNaniteTargets())
		{
			for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
			{
				const UMeshComponent* Comp = Weak.Get();
				if (Comp && (!AnomalyViewport::HasNaniteComponentProbe() || AnomalyViewport::ActorDrawsAnyNaniteReadOnly(Comp->GetOwner())))
				{
					RefuseEvent(Why::NaniteUnmaskable, FString(), TEXT("E7N"));
					return;
				}
			}
		}

		for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
		{
			UMeshComponent* Comp = Weak.Get();
			const int32 NumSlots = Comp->GetNumMaterials();
			for (int32 i = 0; i < NumSlots; ++i)
			{
				FSlot& S = Out.Slots.AddDefaulted_GetRef();
				S.Comp = Comp;
				S.Owner = Comp->GetOwner();
				S.CompName = Comp->GetFName();
				S.SlotIndex = i;
				S.Raw = Comp->OverrideMaterials.IsValidIndex(i) ? Comp->OverrideMaterials[i].Get() : nullptr;
				S.RawArrayLen = Comp->OverrideMaterials.Num();
				if (UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Comp))
				{
					S.bStatic = true;
					S.Asset = SMC->GetStaticMesh() ? SMC->GetStaticMesh()->GetMaterial(i) : nullptr;
				}
				else if (USkinnedMeshComponent* SK = Cast<USkinnedMeshComponent>(Comp))
				{
					S.bSkinned = true;
					USkinnedAsset* Asset = SK->GetSkinnedAsset();
					S.Asset = (Asset && Asset->GetMaterials().IsValidIndex(i)) ? Asset->GetMaterials()[i].MaterialInterface.Get() : nullptr;
				}
				S.Resolved = S.Raw ? S.Raw : S.Asset;
				S.Effective = Comp->GetMaterial(i);
				EvaluateSlot(World, In, S);
			}
		}

		TArray<int32> Ranks;
		TArray<bool> Untouched;
		TArray<bool> Qualified;
		for (const FSlot& S : Out.Slots)
		{
			Ranks.Add(S.Step);
			Untouched.Add(S.bUntouched);
			Qualified.Add(S.IsQualified());
			if (S.IsQualified())
			{
				++Out.SlotsQualified;
			}
		}
		const int32 EarliestIndex = TexCorruptPure::PickEarliestSlot(Ranks.GetData(), Untouched.GetData(), Qualified.GetData(), Ranks.Num());
		const FSlot* Earliest = Out.Slots.IsValidIndex(EarliestIndex) ? &Out.Slots[EarliestIndex] : nullptr;
		const TexCorruptPure::EFootprint Footprint = TexCorruptPure::JudgeFootprint(Out.SlotsQualified, Out.Slots.Num());
		if (Footprint == TexCorruptPure::EFootprint::NoneQualified)
		{
			if (Earliest)
			{
				RefuseEvent(*Earliest->Reason, Earliest->Sub, TEXT("V1"));
			}
			else
			{
				RefuseEvent(Why::NoEligibleSlot, Out.Slots.Num() == 0 ? FString(TEXT("no_slots")) : FString(TEXT("all_untouched")), TEXT("V1"));
			}
			return;
		}
		if (Footprint == TexCorruptPure::EFootprint::Partial)
		{
			const int32 FirstOut = TexCorruptPure::PickEarliestNonQualified(Ranks.GetData(), Qualified.GetData(), Ranks.Num());
			const FString FirstReason = Out.Slots.IsValidIndex(FirstOut) ? Out.Slots[FirstOut].Reason : FString(TEXT("unknown"));
			RefuseEvent(Why::PartialFootprint, FString::Printf(TEXT("%d/%d:%s"), Out.SlotsQualified, Out.Slots.Num(), *FirstReason),
				TEXT("V1P"));
			return;
		}

		ComputeRequirement(Out);
		const int64 Cap = GetMaxRtBytes();
		const int64 Available = Ledger().Available(Cap);
		if (!TexCorruptPure::Fits(Out.RequiredBytes, Cap, Ledger().Live, Ledger().PendingSum()))
		{
			RefuseEvent(Why::OverBudget, FString::Printf(TEXT("need_%lld_available_%lld_cap_%lld"), Out.RequiredBytes, Available, Cap),
				TEXT("V2"));
			return;
		}
		Out.bApply = true;
		Out.Reason = TEXT("APPLY");
	}

	bool IsEligibleTarget(UWorld* World, FName Id, AActor* Actor, FString& OutReason)
	{
		OutReason.Reset();
		static const FName Uv(TEXT("uv_corruption"));
		static const FName Normal(TEXT("normal_corruption"));
		if (!World || !Actor || (Id != Uv && Id != Normal))
		{
			OutReason = TEXT("not_texcorrupt");
			return false;
		}
		AnomalyViewport::FReadOnlyEnumerationScope ReadOnly;
		FTreeInputs In;
		In.Family = Id == Uv ? EFamily::UV : EFamily::Normal;
		In.bCensus = true;
		In.TargetActor = Actor;
		FTreeResult Result;
		EvaluateTree(World, In, Result);
		if (!Result.bApply)
		{
			OutReason = Result.Reason;
		}
		return Result.bApply;
	}

	void CountTreeDispositions(const FTreeResult& Result)
	{
		FRunStats& Stats = FStatsAccess::Mutable();
		for (const FSlot& S : Result.Slots)
		{
			CountReason(Stats.SlotDispositions, S.DispositionKey());
			if (S.bPartial)
			{
				++Stats.SlotsPartialSet;
			}
			for (const FBinding& B : S.Bindings)
			{
				CountReason(Stats.BindingDispositions, B.DispositionKey());
			}
		}
		if (!Result.bApply)
		{
			CountReason(Stats.RefusedByReason, Result.Reason);
		}
	}

	void LogTree(const FTreeResult& Result, const TCHAR* Tag, bool bVerboseBindings)
	{
		UE_LOG(LogAnomaly, Log,
			TEXT("TEXCORRUPT-%s family=%s mode=%s target='%s' final=%s step=%s slots=%d qualified=%d required_bytes=%lld ")
			TEXT("distinct_textures=%d scratch_classes=%d cap=%s levers=[%s]%s"),
			Tag, LexFamily(Result.Family), LexMode(Result.Mode), *Result.TargetQuery, *Result.FinalKey(),
			Result.EventStep.IsEmpty() ? TEXT("-") : *Result.EventStep, Result.Slots.Num(), Result.SlotsQualified,
			Result.RequiredBytes, Result.DistinctTextures, Result.ScratchClasses, *DescribeMaxRtBytes(), *DescribeLevers(),
			*Result.Attempt.Describe());
		for (const FSlot& S : Result.Slots)
		{
			UE_LOG(LogAnomaly, Log,
				TEXT("TEXCORRUPT-%s   slot %s.%s[%d] disposition=%s raw=%s(len %d) asset=%s resolved=%s effective=%s ")
				TEXT("nanite_routed=%d partial=%d bindings=%d"),
				Tag, *GetNameSafe(S.Owner), *S.CompName.ToString(), S.SlotIndex, *S.DispositionKey(),
				*GetNameSafe(S.Raw), S.RawArrayLen, *GetNameSafe(S.Asset), *GetNameSafe(S.Resolved), *GetNameSafe(S.Effective),
				(S.Effective != S.Resolved) ? 1 : 0, S.bPartial ? 1 : 0, S.Bindings.Num());
			if (!bVerboseBindings)
			{
				continue;
			}
			for (const FBinding& B : S.Bindings)
			{
				FString Levels;
				for (const FIntPoint& L : B.LevelSizes)
				{
					Levels += FString::Printf(TEXT("%dx%d "), L.X, L.Y);
				}
				Levels.TrimEndInline();
				UE_LOG(LogAnomaly, Log,
					TEXT("TEXCORRUPT-%s     binding type=%s idx=%d param='%s' assoc=%s layer=%d texture='%s' class=%s fmt=%s srgb=%d ")
					TEXT("normal_map=%d group=%d M=%d top=%dx%d levels=[%s] resident=%d max=%d nonoptional=%d asset_lod_bias=%d ")
					TEXT("cinematic=%d cached_lod_bias=%d streams=%d required=%d disposition=%s"),
					Tag, LexTexType(B.Type), B.Index, *B.ParamName.ToString(), LexAssociation(B.Info.Association), B.Info.Index,
					*B.TextureName, LexClass(B.Class), GetPixelFormatString(B.Format), B.bSRGB ? 1 : 0, B.bNormalMap ? 1 : 0,
					B.LODGroup, B.M, B.W, B.H, *Levels, B.ResidentLODs, B.MaxLODs, B.NonOptionalLODs, B.AssetLODBias,
					B.CinematicMips, B.CachedLODBias, B.bStreams ? 1 : 0, B.bRequired ? 1 : 0, *B.DispositionKey());
			}
		}
	}

	void RunCensus(UWorld* World, const FString& Query)
	{
		if (!World)
		{
			return;
		}
		TArray<FString> Targets;
		if (!Query.IsEmpty())
		{
			Targets.Add(Query);
		}
		else
		{
			for (const TWeakObjectPtr<AActor>& Weak : AnomalyViewport::GetVisibleRenderableActors(World))
			{
				if (const AActor* Actor = Weak.Get())
				{
					Targets.Add(FString(TEXT("=")) + Actor->GetName());
				}
			}
		}
		UE_LOG(LogAnomaly, Log,
			TEXT("TEXCORRUPT-CENSUS BEGIN targets=%d r.MipMapLODBias=%.3f r.Streaming.MipBias=%.3f r.Streaming.UsePerTextureBias=%d ")
			TEXT("Compat.UseDXT5NormalMaps=%d r.TextureStreaming=%d cap=%s min_px=%s max_textures=%s. READ-ONLY: the tree has no ")
			TEXT("side effect and the census counts nothing into run_summary."),
			Targets.Num(), UTexture2D::GetGlobalMipMapLODBias(), ReadCVarFloat(TEXT("r.Streaming.MipBias"), 0.0f),
			ReadCVarInt(TEXT("r.Streaming.UsePerTextureBias"), 1), ReadCVarInt(TEXT("Compat.UseDXT5NormalMaps"), 0),
			ReadCVarInt(TEXT("r.TextureStreaming"), 1), *DescribeMaxRtBytes(), *DescribeMinTexturePx(), *DescribeMaxTextures());
		for (const FString& Target : Targets)
		{
			for (const EFamily Family : { EFamily::UV, EFamily::Normal })
			{
				FTreeInputs In;
				In.Family = Family;
				In.bCensus = true;
				In.TargetQuery = Target;
				FTreeResult Result;
				EvaluateTree(World, In, Result);
				LogTree(Result, TEXT("CENSUS"), Family == EFamily::UV);
			}
		}
		UE_LOG(LogAnomaly, Log, TEXT("TEXCORRUPT-CENSUS END targets=%d"), Targets.Num());
	}

	namespace
	{
		struct FCensusCounts
		{
			int32 Eligible = 0;
			int32 Refused = 0;
			TMap<FString, int32> Reasons;
		};

		struct FOfficeCensusJob
		{
			TWeakObjectPtr<UWorld> World;
			bool bAll = false;
			bool bEnumerated = false;
			bool bStatsUnchanged = true;
			TArray<FString> Names;
			TArray<TWeakObjectPtr<AActor>> Actors;
			int32 Next = 0;
			int32 Gone = 0;
			FCensusCounts Counts[2];
			double StartSeconds = 0.0;
			double NextProgressSeconds = 0.0;
			double LastCallSeconds = 0.0;
			double MaxIntervalSeconds = 0.0;
			double MaxCallSeconds = 0.0;
			int32 Calls = 0;
		};

		TUniquePtr<FOfficeCensusJob>& ActiveCensus()
		{
			static TUniquePtr<FOfficeCensusJob> Job;
			return Job;
		}

		constexpr double CensusSliceSeconds = 0.004;
		constexpr double CensusMaxSeconds = 120.0;
		constexpr double CensusProgressSeconds = 2.0;
	}

	bool RunOfficeCensus(FOfficeCensusJob& Job)
	{
		AnomalyViewport::FReadOnlyEnumerationScope ReadOnly;
		const double CallStart = FPlatformTime::Seconds();
		if (Job.Calls > 0)
		{
			Job.MaxIntervalSeconds = FMath::Max(Job.MaxIntervalSeconds, CallStart - Job.LastCallSeconds);
		}
		Job.LastCallSeconds = CallStart;
		++Job.Calls;
		UWorld* World = Job.World.Get();
		if (!World && !Job.bEnumerated)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("IAI.TexCorrupt.Census: no world; nothing was counted."));
			return true;
		}

		struct FStatsSnapshot
		{
			int32 TargetExclusions = 0;
			int32 TranslucentExclusions = 0;
			uint32 RunStats = 0;
			uint32 UvModes = 0;
			uint32 NormalModes = 0;
			int32 PoolLiveFires = -1;
			TArray<FString> PoolIds;
		};
		auto TakeStatsSnapshot = [World]()
		{
			FStatsSnapshot S;
			S.TargetExclusions = AnomalyViewport::GetTargetExclusionCount();
			S.TranslucentExclusions = AnomalyViewport::GetTranslucentOnlyExclusionCount();
			S.RunStats = RunStatsDigest();
			S.UvModes = GetEnabledModeMask(EFamily::UV);
			S.NormalModes = GetEnabledModeMask(EFamily::Normal);
			if (World)
			{
				if (const UAnomalyAutoInjectorSubsystem* AutoInjector = World->GetSubsystem<UAnomalyAutoInjectorSubsystem>())
				{
					S.PoolLiveFires = AutoInjector->GetLiveFireCount();
					S.PoolIds = AutoInjector->GetEnabledIds();
				}
			}
			return S;
		};
		const FStatsSnapshot StatsBefore = TakeStatsSnapshot();

		TArray<FString>& Names = Job.Names;
		if (!Job.bEnumerated)
		{
			Job.bEnumerated = true;
			TArray<TPair<FString, TWeakObjectPtr<AActor>>> Found;
			if (Job.bAll)
			{
				for (TActorIterator<AActor> It(World); It; ++It)
				{
					AActor* Actor = *It;
					if (!Actor)
					{
						continue;
					}
					TInlineComponentArray<UPrimitiveComponent*> Prims(Actor);
					for (UPrimitiveComponent* Prim : Prims)
					{
						if (AnomalyViewport::IsRenderableComponentReadOnly(Prim))
						{
							Found.Emplace(Actor->GetName(), Actor);
							break;
						}
					}
				}
			}
			else
			{
				for (const TWeakObjectPtr<AActor>& Weak : AnomalyViewport::GetVisibleRenderableActorsReadOnly(World))
				{
					if (AActor* Actor = Weak.Get())
					{
						Found.Emplace(Actor->GetName(), Actor);
					}
				}
			}
			Found.Sort([](const TPair<FString, TWeakObjectPtr<AActor>>& L, const TPair<FString, TWeakObjectPtr<AActor>>& R)
			{
				return L.Key < R.Key;
			});
			for (const TPair<FString, TWeakObjectPtr<AActor>>& P : Found)
			{
				Names.Add(P.Key);
				Job.Actors.Add(P.Value);
			}
		}

		FCensusCounts* Counts = Job.Counts;
		const TCHAR* Stop = World ? nullptr : TEXT("world_gone");
		const double SliceEnd = FPlatformTime::Seconds() + CensusSliceSeconds;
		while (!Stop && Job.Next < Names.Num())
		{
			AActor* Actor = Job.Actors[Job.Next].Get();
			++Job.Next;
			if (!Actor)
			{
				++Job.Gone;
				continue;
			}
			for (int32 f = 0; f < 2; ++f)
			{
				FTreeInputs In;
				In.Family = f == 0 ? EFamily::UV : EFamily::Normal;
				In.bCensus = true;
				In.TargetActor = Actor;
				FTreeResult Result;
				EvaluateTree(World, In, Result);
				if (Result.bApply)
				{
					++Counts[f].Eligible;
				}
				else
				{
					++Counts[f].Refused;
					Counts[f].Reasons.FindOrAdd(Result.Reason)++;
				}
			}
			if (FPlatformTime::Seconds() >= SliceEnd)
			{
				break;
			}
		}
		const FStatsSnapshot StatsAfter = TakeStatsSnapshot();
		const bool bStatsUnchanged = Job.bStatsUnchanged && StatsBefore.TargetExclusions == StatsAfter.TargetExclusions
			&& StatsBefore.TranslucentExclusions == StatsAfter.TranslucentExclusions && StatsBefore.RunStats == StatsAfter.RunStats
			&& StatsBefore.UvModes == StatsAfter.UvModes && StatsBefore.NormalModes == StatsAfter.NormalModes
			&& StatsBefore.PoolLiveFires == StatsAfter.PoolLiveFires && StatsBefore.PoolIds == StatsAfter.PoolIds;
		Job.bStatsUnchanged = bStatsUnchanged;
		Job.MaxCallSeconds = FMath::Max(Job.MaxCallSeconds, FPlatformTime::Seconds() - CallStart);
		const double WorkMsMax = Job.MaxCallSeconds * 1000.0;
		const double FrameIntervalMsMax = Job.Calls > 1 ? Job.MaxIntervalSeconds * 1000.0 : -1.0;

		const double Elapsed = FPlatformTime::Seconds() - Job.StartSeconds;
		if (!Stop && Job.Next >= Names.Num())
		{
			Stop = TEXT("complete");
		}
		if (!Stop && Elapsed >= CensusMaxSeconds)
		{
			Stop = TEXT("time_limit");
		}
		if (!Stop)
		{
			if (Elapsed >= Job.NextProgressSeconds)
			{
				Job.NextProgressSeconds = Elapsed + CensusProgressSeconds;
				UE_LOG(LogAnomaly, Display, TEXT("IAI-TEXCORRUPT-CENSUS v1 progress scanned=%d of=%d seconds=%.1f"),
					Job.Next, Names.Num(), Elapsed);
			}
			return false;
		}

		const bool bAll = Job.bAll;
		UE_LOG(LogAnomaly, Display, TEXT("IAI-TEXCORRUPT-CENSUS v1 scope=%s candidates=%d cap_bytes=%d uv_modes=%s normal_modes=%s"),
			bAll ? TEXT("all") : TEXT("view"), Names.Num(), GetMaxRtBytes(), *DescribeModeSet(EFamily::UV, GetEnabledModeMask(EFamily::UV)),
			*DescribeModeSet(EFamily::Normal, GetEnabledModeMask(EFamily::Normal)));
		for (int32 f = 0; f < 2; ++f)
		{
			TArray<FString> Keys;
			Counts[f].Reasons.GetKeys(Keys);
			Keys.Sort([](const FString& L, const FString& R) { return L.Compare(R, ESearchCase::CaseSensitive) < 0; });
			FString Reasons;
			for (const FString& Key : Keys)
			{
				Reasons += FString::Printf(TEXT("%s%s:%d"), Reasons.IsEmpty() ? TEXT("") : TEXT(","), *Key, Counts[f].Reasons[Key]);
			}
			UE_LOG(LogAnomaly, Display, TEXT("IAI-TEXCORRUPT-CENSUS v1 id=%s eligible=%d refused=%d reasons=%s"),
				f == 0 ? TEXT("uv_corruption") : TEXT("normal_corruption"), Counts[f].Eligible, Counts[f].Refused,
				Reasons.IsEmpty() ? TEXT("-") : *Reasons);
		}
		UE_LOG(LogAnomaly, Display, TEXT("IAI-TEXCORRUPT-CENSUS v1 scanned=%d of=%d gone=%d stopped=%s seconds=%.1f"),
			Job.Next, Names.Num(), Job.Gone, Stop, Elapsed);
		UE_LOG(LogAnomaly, Display, TEXT("IAI-TEXCORRUPT-CENSUS v1 timing frames=%d work_ms_max=%.2f frame_interval_ms_max=%.1f"),
			Job.Calls, WorkMsMax, FrameIntervalMsMax);
		UE_LOG(LogAnomaly, Display, TEXT("IAI-TEXCORRUPT-CENSUS v1 end stats_unchanged=%d"), bStatsUnchanged ? 1 : 0);
		return true;
	}

	namespace
	{
		FAutoConsoleCommandWithWorldAndArgs GOfficeCensusCmd(
			TEXT("IAI.TexCorrupt.Census"),
			TEXT("m53 office census, READ-ONLY, in every build. Runs the uv_corruption and normal_corruption decision tree in census ")
			TEXT("mode (no allocation, draw or slot change; the mode step is skipped) with the effective cap and all-or-nothing, ")
			TEXT("over the auto-pool's candidate set now (name-sorted), or with 'all' over every renderable actor in the loaded ")
			TEXT("levels, and prints four IAI-TEXCORRUPT-CENSUS v1 lines of counts only: no actor, component, asset, path, map or ")
			TEXT("frame, and no sub-reasons. TIME-SLICED: about 4 ms of work per frame, a progress line every 2 s, and a hard stop ")
			TEXT("at 120 s that prints the counts so far with 'scanned=K of=N stopped=time_limit'; it never blocks a frame for long. ")
			TEXT("The end line reports the frames it spanned, its longest single frame of work (work_ms_max) and the longest ")
			TEXT("frame-to-frame interval while it ran (frame_interval_ms_max). ")
			TEXT("Usage: IAI.TexCorrupt.Census [all]"),
			FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
			{
				const bool bAll = Args.Num() >= 1 && Args[0].Equals(TEXT("all"), ESearchCase::IgnoreCase);
				if (Args.Num() >= 1 && !bAll)
				{
					UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.TexCorrupt.Census [all]"));
					return;
				}
				TUniquePtr<FOfficeCensusJob>& Job = ActiveCensus();
				if (Job.IsValid())
				{
					UE_LOG(LogAnomaly, Warning,
						TEXT("IAI.TexCorrupt.Census: a census is already running (%d of %d scanned); this request is ignored."),
						Job->Next, Job->Names.Num());
					return;
				}
				Job = MakeUnique<FOfficeCensusJob>();
				Job->World = World;
				Job->bAll = bAll;
				Job->StartSeconds = FPlatformTime::Seconds();
				Job->NextProgressSeconds = CensusProgressSeconds;
				UE_LOG(LogAnomaly, Display,
					TEXT("IAI-TEXCORRUPT-CENSUS v1 begin scope=%s slice_ms=%d max_seconds=%d - time-sliced across frames, READ-ONLY."),
					bAll ? TEXT("all") : TEXT("view"), (int32)(CensusSliceSeconds * 1000.0), (int32)CensusMaxSeconds);
				if (RunOfficeCensus(*Job))
				{
					Job.Reset();
					return;
				}
				FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([](float) -> bool
				{
					TUniquePtr<FOfficeCensusJob>& Active = ActiveCensus();
					if (!Active.IsValid())
					{
						return false;
					}
					if (RunOfficeCensus(*Active))
					{
						Active.Reset();
						return false;
					}
					return true;
				}));
			}));
	}
}
