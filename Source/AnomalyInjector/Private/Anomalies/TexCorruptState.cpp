#include "Anomalies/TexCorruptCore.h"
#include "Anomalies/TexCorruptPure.h"

#include "AnomalyDefaults.h"
#include "AnomalyInjectorLog.h"
#include "HAL/IConsoleManager.h"
#include "HAL/ThreadSafeCounter.h"
#include "Misc/ConfigCacheIni.h"

namespace AnomalyTexCorrupt
{
	namespace Why
	{
		const TCHAR* AssetsUnavailable = TEXT("assets_unavailable");
		const TCHAR* CorruptorNotReady = TEXT("corruptor_not_ready");
		const TCHAR* Dxt5NormalHost = TEXT("dxt5_normal_host");
		const TCHAR* RuntimeLodBias = TEXT("runtime_lod_bias");
		const TCHAR* ModeInvalid = TEXT("mode_invalid");
		const TCHAR* NoMesh = TEXT("no_mesh");
		const TCHAR* SlotEmpty = TEXT("slot_empty");
		const TCHAR* SlotTranslucent = TEXT("slot_translucent");
		const TCHAR* HostMid = TEXT("host_mid");
		const TCHAR* NaniteOverride = TEXT("nanite_override");
		const TCHAR* ShaderMapUnavailable = TEXT("shader_map_unavailable");
		const TCHAR* ShaderMapIncomplete = TEXT("shader_map_incomplete");
		const TCHAR* DefaultMaterialPath = TEXT("default_material_path");
		const TCHAR* NoTextures = TEXT("no_textures");
		const TCHAR* VirtualTexture = TEXT("virtual_texture");
		const TCHAR* UnsupportedType = TEXT("unsupported_type");
		const TCHAR* ExcludedGroup = TEXT("excluded_group");
		const TCHAR* TextureNotParameter = TEXT("texture_not_parameter");
		const TCHAR* UnsupportedEncoding = TEXT("unsupported_encoding");
		const TCHAR* MipChainShape = TEXT("mip_chain_shape");
		const TCHAR* HeldByStuckLowMip = TEXT("held_by_stuck_low_mip");
		const TCHAR* ResourceNotReady = TEXT("resource_not_ready");
		const TCHAR* StreamingPending = TEXT("streaming_pending");
		const TCHAR* NotFullyResident = TEXT("not_fully_resident");
		const TCHAR* Transformable = TEXT("transformable");
		const TCHAR* NonSpatialExempt = TEXT("non_spatial_exempt");
		const TCHAR* NoNormalMap = TEXT("no_normal_map");
		const TCHAR* NormalUnconnected = TEXT("normal_unconnected");
		const TCHAR* BelowSizePolicy = TEXT("below_size_policy");
		const TCHAR* MapSetOverCap = TEXT("map_set_over_cap");
		const TCHAR* NoEligibleSlot = TEXT("no_eligible_slot");
		const TCHAR* OverBudget = TEXT("over_budget");
		const TCHAR* RtAllocFailed = TEXT("rt_alloc_failed");
		const TCHAR* DrawPreconditionFailed = TEXT("draw_precondition_failed");
		const TCHAR* ParamReadbackMismatch = TEXT("param_readback_mismatch");
		const TCHAR* Qualified = TEXT("qualified");
		const TCHAR* NotRequired = TEXT("not_required");
	}

	namespace Param
	{
		const FName SrcColor(TEXT("SrcColor"));
		const FName SrcData(TEXT("SrcData"));
		const FName SrcNormal(TEXT("SrcNormal"));
		const FName NoiseNormal(TEXT("NoiseNormal"));
		const FName SrcKind(TEXT("SrcKind"));
		const FName SrcMip(TEXT("SrcMip"));
		const FName NoiseMip(TEXT("NoiseMip"));
		const FName UvScale(TEXT("UvScale"));
		const FName UvOffsetU(TEXT("UvOffsetU"));
		const FName UvOffsetV(TEXT("UvOffsetV"));
		const FName UvSwap(TEXT("UvSwap"));
		const FName ScrambleOn(TEXT("ScrambleOn"));
		const FName ScrambleK(TEXT("ScrambleK"));
		const FName ScrambleAInv(TEXT("ScrambleAInv"));
		const FName ScrambleB(TEXT("ScrambleB"));
		const FName NormalSignX(TEXT("NormalSignX"));
		const FName NormalSignY(TEXT("NormalSignY"));
		const FName FlatMix(TEXT("FlatMix"));
		const FName NoiseAmp(TEXT("NoiseAmp"));
		const FName DbgChanSwap(TEXT("DbgChanSwap"));
		const FName DbgSrgbTwice(TEXT("DbgSrgbTwice"));
		const FName DbgTexelShift(TEXT("DbgTexelShift"));
		const FName TexelSizeU(TEXT("TexelSizeU"));
		const FName TexelSizeV(TEXT("TexelSizeV"));
		const FName DbgSkipNormalEncode(TEXT("DbgSkipNormalEncode"));
		const FName DbgOpaque(TEXT("DbgOpaque"));
	}

	const TCHAR* LexFamily(EFamily Family)
	{
		return Family == EFamily::UV ? TEXT("uv") : TEXT("normal");
	}

	const TCHAR* LexClass(EClass Class)
	{
		switch (Class)
		{
		case EClass::Colour: return TEXT("colour");
		case EClass::Data:   return TEXT("data");
		case EClass::Normal: return TEXT("normal");
		default:             return TEXT("none");
		}
	}

	const TCHAR* LexMode(EMode Mode)
	{
		switch (Mode)
		{
		case EMode::Identity:       return TEXT("identity");
		case EMode::IdentityRedraw: return TEXT("identity_redraw");
		case EMode::TileProbe:      return TEXT("tile_probe");
		default:                    return TEXT("none");
		}
	}

	const TCHAR* LexWrongCopy(EWrongCopy Fault)
	{
		switch (Fault)
		{
		case EWrongCopy::ChanSwap:   return TEXT("chanswap");
		case EWrongCopy::SrgbTwice:  return TEXT("srgbtwice");
		case EWrongCopy::MipShift:   return TEXT("mipshift");
		case EWrongCopy::MipGen:     return TEXT("mipgen");
		case EWrongCopy::TexelShift: return TEXT("texelshift");
		case EWrongCopy::Normal:     return TEXT("normal");
		case EWrongCopy::Alpha:      return TEXT("alpha");
		case EWrongCopy::NoClear:    return TEXT("noclear");
		default:                     return TEXT("none");
		}
	}

	const TArray<FName>& UvCorruptorScalars()
	{
		static const TArray<FName> List = {
			Param::SrcKind, Param::SrcMip, Param::UvScale, Param::UvOffsetU, Param::UvOffsetV, Param::UvSwap,
			Param::ScrambleOn, Param::ScrambleK, Param::ScrambleAInv, Param::ScrambleB,
			Param::DbgChanSwap, Param::DbgSrgbTwice, Param::DbgTexelShift, Param::TexelSizeU, Param::TexelSizeV,
			Param::DbgSkipNormalEncode, Param::DbgOpaque };
		return List;
	}

	const TArray<FName>& UvCorruptorTextures()
	{
		static const TArray<FName> List = { Param::SrcColor, Param::SrcData, Param::SrcNormal };
		return List;
	}

	const TArray<FName>& NormalCorruptorScalars()
	{
		static const TArray<FName> List = {
			Param::SrcMip, Param::NoiseMip, Param::NormalSignX, Param::NormalSignY, Param::FlatMix, Param::NoiseAmp,
			Param::DbgChanSwap, Param::DbgTexelShift, Param::TexelSizeU, Param::TexelSizeV, Param::DbgSkipNormalEncode };
		return List;
	}

	const TArray<FName>& NormalCorruptorTextures()
	{
		static const TArray<FName> List = { Param::SrcNormal, Param::NoiseNormal };
		return List;
	}

	namespace
	{
		struct FIntKnob
		{
			const TCHAR* Command;
			const TCHAR* IniKey;
			int32 Compiled;
			int32 Min;
			int32 Max;
			int32 Override = 0;
			bool bOverride = false;
			bool bResolved = false;
			int32 Value = 0;
			const TCHAR* Source = TEXT("compiled");
		};

		FIntKnob GMaxRtBytes{ TEXT("IAI.Anomaly.TexCorruptMaxRtBytes"), TEXT("TexCorruptMaxRtBytesDefault"),
			FKnobs::MaxRtBytesCompiled, FKnobs::MaxRtBytesMin, FKnobs::MaxRtBytesMax };
		FIntKnob GMinTexturePx{ TEXT("IAI.Anomaly.TexCorruptMinTexturePx"), TEXT("TexCorruptMinTexturePxDefault"),
			FKnobs::MinTexturePxCompiled, FKnobs::MinTexturePxMin, FKnobs::MinTexturePxMax };
		FIntKnob GMaxTextures{ TEXT("IAI.Anomaly.TexCorruptMaxTextures"), TEXT("TexCorruptMaxTexturesDefault"),
			FKnobs::MaxTexturesCompiled, FKnobs::MaxTexturesMin, FKnobs::MaxTexturesMax };

		int32 KnobGet(FIntKnob& K)
		{
			if (K.bOverride)
			{
				return K.Override;
			}
			if (!K.bResolved)
			{
				K.bResolved = true;
				K.Value = K.Compiled;
				K.Source = TEXT("compiled");
				int32 FromIni = 0;
				if (GConfig && GConfig->GetInt(AnomalyDefaults::SectionName(), K.IniKey, FromIni, GGameIni))
				{
					if (FromIni < K.Min || FromIni > K.Max)
					{
						UE_LOG(LogAnomaly, Warning,
							TEXT("texcorrupt: DefaultGame.ini [%s] %s = %d is out of range [%d..%d]; REFUSED, not clamped, ")
							TEXT("so the compiled default %d stands."),
							AnomalyDefaults::SectionName(), K.IniKey, FromIni, K.Min, K.Max, K.Compiled);
					}
					else
					{
						K.Value = FromIni;
						K.Source = TEXT("ini");
					}
				}
				UE_LOG(LogAnomaly, Log, TEXT("texcorrupt: %s = %d (%s)."), K.Command, K.Value, K.Source);
			}
			return K.Value;
		}

		FString KnobDescribe(FIntKnob& K)
		{
			const int32 V = KnobGet(K);
			return FString::Printf(TEXT("%d(%s)"), V, K.bOverride ? TEXT("console") : K.Source);
		}

		void KnobCommand(FIntKnob& K, const TArray<FString>& Args)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: %s <n|default>  (current: %s, range [%d..%d])"),
					K.Command, *KnobDescribe(K), K.Min, K.Max);
				return;
			}
			if (Args[0].Equals(TEXT("default"), ESearchCase::IgnoreCase))
			{
				K.bOverride = false;
			}
			else if (Args[0].IsNumeric())
			{
				const int64 Requested = FCString::Atoi64(*Args[0]);
				if (Requested < K.Min || Requested > K.Max)
				{
					UE_LOG(LogAnomaly, Warning,
						TEXT("%s: %lld is out of range [%d..%d]; REFUSED, not clamped. The previous value stands."),
						K.Command, Requested, K.Min, K.Max);
				}
				else
				{
					K.Override = (int32)Requested;
					K.bOverride = true;
				}
			}
			else
			{
				UE_LOG(LogAnomaly, Warning, TEXT("%s: '%s' is not a whole number (or 'default')."), K.Command, *Args[0]);
			}
			UE_LOG(LogAnomaly, Log, TEXT("%s: EFFECTIVE READ-BACK = %s. Precedence: console > DefaultGame.ini [%s] %s > compiled %d."),
				K.Command, *KnobDescribe(K), AnomalyDefaults::SectionName(), K.IniKey, K.Compiled);
		}

		FLevers GLevers;
		FLedger GLedger;
		FRunStats GStats;
		FThreadSafeCounter GTripwire;

		FAutoConsoleCommand GTexCorruptMaxRtBytesCmd(
			TEXT("IAI.Anomaly.TexCorruptMaxRtBytes"),
			TEXT("m53: the run-wide cap on live m53 render-target bytes, both ids together (outputs, drift snapshots and ")
			TEXT("per-mip scratch). Over the cap an event is REFUSED over_budget, never downsampled. Compiled 134217728 ")
			TEXT("(128 MiB); ini [AnomalyInjector] TexCorruptMaxRtBytesDefault; out of [1 MiB, 1 GiB] is refused. ")
			TEXT("Usage: IAI.Anomaly.TexCorruptMaxRtBytes <bytes|default>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args) { KnobCommand(GMaxRtBytes, Args); }));

		FAutoConsoleCommand GTexCorruptMinTexturePxCmd(
			TEXT("IAI.Anomaly.TexCorruptMinTexturePx"),
			TEXT("m53: the size POLICY (A5): a qualifying slot needs at least one required texture at or above this size ")
			TEXT("on both axes. It is a policy, not a claim that a small texture is constant; small textures inside a ")
			TEXT("qualifying set are transformed with it. Compiled 64; ini [AnomalyInjector] TexCorruptMinTexturePxDefault. ")
			TEXT("Usage: IAI.Anomaly.TexCorruptMinTexturePx <px|default>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args) { KnobCommand(GMinTexturePx, Args); }));

		FAutoConsoleCommand GTexCorruptMaxTexturesCmd(
			TEXT("IAI.Anomaly.TexCorruptMaxTextures"),
			TEXT("m53: the cap on a slot's required map set (A6). Over the cap the slot is REFUSED map_set_over_cap; it is ")
			TEXT("never trimmed. Compiled 8; ini [AnomalyInjector] TexCorruptMaxTexturesDefault. ")
			TEXT("Usage: IAI.Anomaly.TexCorruptMaxTextures <n|default>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args) { KnobCommand(GMaxTextures, Args); }));
	}

	int32 GetMaxRtBytes() { return KnobGet(GMaxRtBytes); }
	FString DescribeMaxRtBytes() { return KnobDescribe(GMaxRtBytes); }
	int32 GetMinTexturePx() { return KnobGet(GMinTexturePx); }
	FString DescribeMinTexturePx() { return KnobDescribe(GMinTexturePx); }
	int32 GetMaxTextures() { return KnobGet(GMaxTextures); }
	FString DescribeMaxTextures() { return KnobDescribe(GMaxTextures); }

	FLevers& Levers()
	{
		return GLevers;
	}

	FString DescribeLevers()
	{
		return FString::Printf(TEXT("noapply=%d wrongcopy=%s identity=%d identity_redraw=%d tile_probe=%d force_missing_asset=%d ")
			TEXT("fail_step=%d fail_alloc_ordinal=%d foreign_replace=%d collateral_detail=%d"),
			GLevers.NoApply, LexWrongCopy(GLevers.WrongCopy), GLevers.bIdentity ? 1 : 0, GLevers.bIdentityRedraw ? 1 : 0,
			GLevers.TileProbe, GLevers.bForceMissingAsset ? 1 : 0, GLevers.FailStep, GLevers.FailAllocOrdinal,
			GLevers.bForeignReplace ? 1 : 0, GLevers.bCollateralDetail ? 1 : 0);
	}

	FLedger& Ledger()
	{
		return GLedger;
	}

	FRunStats& FStatsAccess::Mutable()
	{
		return GStats;
	}

	void CountReason(TMap<FString, int32>& Map, const FString& Key)
	{
		Map.FindOrAdd(Key)++;
	}

	FThreadSafeCounter& TripwireCounter()
	{
		return GTripwire;
	}

	int64 ChainBytes(int32 W, int32 H, int32 M)
	{
		return (int64)TexCorruptPure::ChainBytes(W, H, M);
	}

	int64 ScratchBytes(int32 W, int32 H, int32 M)
	{
		return (int64)TexCorruptPure::ScratchBytes(W, H, M);
	}

	void ResetRunStats()
	{
		GStats = FRunStats();
		GLedger.Tick(GFrameCounter);
		GLedger.Peak = GLedger.Live + GLedger.PendingSum();
		GStats.RtBytesPeak = GLedger.Peak;
		GTripwire.Reset();
	}

	FRunStats GetRunStats()
	{
		FRunStats Out = GStats;
		Out.RtMipMismatch = GTripwire.GetValue();
		Out.RtBytesPeak = FMath::Max(Out.RtBytesPeak, GLedger.Peak);
		return Out;
	}

	const TArray<FString>& AllFinalReasons()
	{
		static const TArray<FString> List = {
			Why::AssetsUnavailable, Why::CorruptorNotReady, Why::Dxt5NormalHost, Why::RuntimeLodBias,
			Why::ModeInvalid, Why::NoMesh, Why::HostMid, Why::NaniteOverride, Why::ShaderMapUnavailable,
			Why::ShaderMapIncomplete, Why::DefaultMaterialPath, Why::NoTextures, Why::VirtualTexture,
			Why::UnsupportedType, Why::ExcludedGroup, Why::TextureNotParameter, Why::UnsupportedEncoding,
			Why::MipChainShape, Why::HeldByStuckLowMip, Why::ResourceNotReady, Why::StreamingPending,
			Why::NotFullyResident, Why::NoNormalMap, Why::NormalUnconnected, Why::BelowSizePolicy,
			Why::MapSetOverCap, Why::NoEligibleSlot, Why::OverBudget, Why::RtAllocFailed,
			Why::DrawPreconditionFailed, Why::ParamReadbackMismatch };
		return List;
	}

	const TArray<FString>& AllRollbackSteps()
	{
		static const TArray<FString> List = { TEXT("2"), TEXT("3"), TEXT("4"), TEXT("5"), TEXT("6") };
		return List;
	}

	bool IsTexCorruptId(FName Id)
	{
		static const FName Uv(TEXT("uv_corruption"));
		static const FName Normal(TEXT("normal_corruption"));
		return Id == Uv || Id == Normal;
	}

	FString FBinding::DispositionKey() const
	{
		if (bNonSpatialExempt)
		{
			return Why::NonSpatialExempt;
		}
		if (Step == 0)
		{
			return bRequired ? FString(Why::Transformable) : FString(Why::NotRequired);
		}
		return Sub.IsEmpty() ? Reason : FString::Printf(TEXT("%s:%s"), *Reason, *Sub);
	}

	FString FSlot::DispositionKey() const
	{
		if (IsQualified())
		{
			return Why::Qualified;
		}
		if (Reason == Why::HostMid)
		{
			return FString::Printf(TEXT("%s:%s:%s"), *Reason, *Where, *Kind);
		}
		return Sub.IsEmpty() ? Reason : FString::Printf(TEXT("%s:%s"), *Reason, *Sub);
	}

	FString FTreeResult::FinalKey() const
	{
		return bApply ? FString(TEXT("APPLY")) : (Sub.IsEmpty() ? Reason : FString::Printf(TEXT("%s:%s"), *Reason, *Sub));
	}
}
