#include "Anomalies/TexCorruptCore.h"
#include "Anomalies/TexCorruptPure.h"

#include "AnomalyInjectorLog.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyLod.h"
#include "AnomalyStuckMipStats.h"
#include "AnomalyViewport.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Components/StaticMeshComponent.h"
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
			UMaterialInterface* Link = Start;
			const TCHAR* Where = FirstWhere;
			for (int32 Depth = 0; Link && Depth < MaxParentDepth; ++Depth)
			{
				if (Link->IsA<UMaterialInstanceDynamic>())
				{
					OutWhere = Where;
					OutKind = TEXT("mid");
					return true;
				}
				if (Link->GetOutermost() == GetTransientPackage())
				{
					OutWhere = Where;
					OutKind = TEXT("transient_outer");
					return true;
				}
				if (!Link->HasAnyFlags(RF_WasLoaded))
				{
					OutWhere = Where;
					OutKind = TEXT("not_loaded");
					return true;
				}
				const UMaterialInstance* Instance = Cast<UMaterialInstance>(Link);
				if (!Instance)
				{
					break;
				}
				Link = Instance->Parent;
				Where = TEXT("parent");
			}
			return false;
		}

		bool NeedsUsage(const FSlot& S, UWorld* World, EMaterialUsage& OutUsage, const TCHAR*& OutName)
		{
			if (S.bSkinned)
			{
				OutUsage = MATUSAGE_SkeletalMesh;
				OutName = TEXT("skeletal_mesh");
				return true;
			}
			const UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(S.Comp);
			if (!SMC)
			{
				return false;
			}
			if (SMC->IsA<UInstancedStaticMeshComponent>())
			{
				OutUsage = MATUSAGE_InstancedStaticMeshes;
				OutName = TEXT("instanced_static_meshes");
				return true;
			}
			const UStaticMesh* Mesh = SMC->GetStaticMesh();
			const EShaderPlatform Platform = World ? GShaderPlatformForFeatureLevel[World->FeatureLevel] : GMaxRHIShaderPlatform;
			if (!SMC->bDisallowNanite && Mesh && Mesh->HasValidNaniteData() && UseNanite(Platform))
			{
				OutUsage = MATUSAGE_Nanite;
				OutName = TEXT("nanite");
				return true;
			}
			const FMeshMapBuildData* BuildData = SMC->LODData.Num() > 0 ? SMC->GetMeshMapBuildData(SMC->LODData[0]) : nullptr;
			if (BuildData && (BuildData->LightMap.IsValid() || BuildData->ShadowMap.IsValid()))
			{
				OutUsage = MATUSAGE_StaticLighting;
				OutName = TEXT("static_lighting");
				return true;
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
			ReadActiveBindings(World, S.Resolved, S.Bindings, bResOk, bSmOk, bComplete);
			if (!bResOk || !bSmOk)
			{
				FailSlot(S, RankS5, Why::ShaderMapUnavailable, bResOk ? TEXT("no_shader_map") : TEXT("no_resource"));
				return;
			}
			if (!bComplete)
			{
				FailSlot(S, RankS6, Why::ShaderMapIncomplete);
				return;
			}

			UMaterial* Root = S.Resolved->GetMaterial();
			EMaterialUsage Usage = MATUSAGE_SkeletalMesh;
			const TCHAR* UsageName = TEXT("");
			if (Root && Root->MaterialDomain == MD_Surface && NeedsUsage(S, World, Usage, UsageName))
			{
				bool bHas = true;
				Root->NeedsSetMaterialUsage_Concurrent(bHas, Usage);
				if (!bHas)
				{
					FailSlot(S, RankS7, Why::DefaultMaterialPath, UsageName);
					return;
				}
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

	void ReadActiveBindings(UWorld* World, UMaterialInterface* Resolved, TArray<FBinding>& OutBindings, bool& bOutResourceOk,
		bool& bOutShaderMapOk, bool& bOutComplete)
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
		if (!Res->IsGameThreadShaderMapComplete())
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
			if (!Res || !Res->GetGameThreadShaderMap() || !Res->IsGameThreadShaderMapComplete())
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
			RefuseEvent(Why::ModeInvalid,
				In.bModeArgGiven ? FString::Printf(TEXT("unknown:%s"), *In.ModeArg) : FString(TEXT("no_mode_in_s1")), TEXT("E6"));
			return;
		}

		TArray<TWeakObjectPtr<UMeshComponent>> Meshes = AnomalyLod::ResolveLodComponents(World, In.TargetQuery);
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
		if (Out.SlotsQualified == 0)
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
			TEXT("distinct_textures=%d scratch_classes=%d cap=%s levers=[%s]"),
			Tag, LexFamily(Result.Family), LexMode(Result.Mode), *Result.TargetQuery, *Result.FinalKey(),
			Result.EventStep.IsEmpty() ? TEXT("-") : *Result.EventStep, Result.Slots.Num(), Result.SlotsQualified,
			Result.RequiredBytes, Result.DistinctTextures, Result.ScratchClasses, *DescribeMaxRtBytes(), *DescribeLevers());
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
}
