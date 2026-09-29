#include "Anomalies/TexCorruptCore.h"
#include "Anomalies/TexCorruptPure.h"

#include "AnomalyDefaults.h"
#include "AnomalyInjectorLog.h"
#include "HAL/IConsoleManager.h"
#include "HAL/ThreadSafeCounter.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Crc.h"

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
		const TCHAR* PartialFootprint = TEXT("partial_footprint");
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
		case EMode::Tile:           return TEXT("tile");
		case EMode::Scramble:       return TEXT("scramble");
		case EMode::Invert:         return TEXT("invert");
		case EMode::GreenFlip:      return TEXT("green_flip");
		default:                    return TEXT("none");
		}
	}

	bool ModeFamily(EMode Mode, EFamily& OutFamily)
	{
		switch (Mode)
		{
		case EMode::TileProbe:
		case EMode::Tile:
		case EMode::Scramble:
			OutFamily = EFamily::UV;
			return true;
		case EMode::Invert:
		case EMode::GreenFlip:
			OutFamily = EFamily::Normal;
			return true;
		default:
			return false;
		}
	}

	TexCorruptPure::EModeFamilyP ToPureFamily(EFamily Family)
	{
		return Family == EFamily::UV ? TexCorruptPure::EModeFamilyP::UV : TexCorruptPure::EModeFamilyP::Normal;
	}

	EMode FromPureMode(TexCorruptPure::EModeP Mode)
	{
		switch (Mode)
		{
		case TexCorruptPure::EModeP::Tile:      return EMode::Tile;
		case TexCorruptPure::EModeP::Scramble:  return EMode::Scramble;
		case TexCorruptPure::EModeP::Invert:    return EMode::Invert;
		case TexCorruptPure::EModeP::GreenFlip: return EMode::GreenFlip;
		default:                                return EMode::None;
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
			bool (*Valid)(int32) = nullptr;
			const TCHAR* Allowed = nullptr;
			int32 Override = 0;
			bool bOverride = false;
			bool bResolved = false;
			int32 Value = 0;
			const TCHAR* Source = TEXT("compiled");
		};

		bool IsPowerOfTwoKnob(int32 V)
		{
			return V > 0 && (V & (V - 1)) == 0;
		}

		bool KnobAdmits(const FIntKnob& K, int64 V)
		{
			return V >= K.Min && V <= K.Max && (!K.Valid || K.Valid((int32)V));
		}

		FString KnobRange(const FIntKnob& K)
		{
			return K.Allowed ? FString(K.Allowed) : FString::Printf(TEXT("[%d..%d]"), K.Min, K.Max);
		}

		FIntKnob GMaxRtBytes{ TEXT("IAI.Anomaly.TexCorruptMaxRtBytes"), TEXT("TexCorruptMaxRtBytesDefault"),
			FKnobs::MaxRtBytesCompiled, FKnobs::MaxRtBytesMin, FKnobs::MaxRtBytesMax };
		FIntKnob GMinTexturePx{ TEXT("IAI.Anomaly.TexCorruptMinTexturePx"), TEXT("TexCorruptMinTexturePxDefault"),
			FKnobs::MinTexturePxCompiled, FKnobs::MinTexturePxMin, FKnobs::MinTexturePxMax };
		FIntKnob GMaxTextures{ TEXT("IAI.Anomaly.TexCorruptMaxTextures"), TEXT("TexCorruptMaxTexturesDefault"),
			FKnobs::MaxTexturesCompiled, FKnobs::MaxTexturesMin, FKnobs::MaxTexturesMax };
		FIntKnob GTileN{ TEXT("IAI.Anomaly.TexCorruptTileN"), TEXT("TexCorruptTileNDefault"),
			FKnobs::TileNCompiled, FKnobs::TileNMin, FKnobs::TileNMax, &IsPowerOfTwoKnob, TEXT("{2, 4, 8, 16}") };
		FIntKnob GScrambleK{ TEXT("IAI.Anomaly.TexCorruptScrambleK"), TEXT("TexCorruptScrambleKDefault"),
			FKnobs::ScrambleKCompiled, FKnobs::ScrambleKMin, FKnobs::ScrambleKMax };

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
					if (!KnobAdmits(K, FromIni))
					{
						UE_LOG(LogAnomaly, Warning,
							TEXT("texcorrupt: DefaultGame.ini [%s] %s = %d is outside %s; REFUSED, not clamped, ")
							TEXT("so the compiled default %d stands."),
							AnomalyDefaults::SectionName(), K.IniKey, FromIni, *KnobRange(K), K.Compiled);
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
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: %s <n|default>  (current: %s, allowed %s)"),
					K.Command, *KnobDescribe(K), *KnobRange(K));
				return;
			}
			if (Args[0].Equals(TEXT("default"), ESearchCase::IgnoreCase))
			{
				K.bOverride = false;
			}
			else if (Args[0].IsNumeric())
			{
				const int64 Requested = FCString::Atoi64(*Args[0]);
				if (!KnobAdmits(K, Requested))
				{
					UE_LOG(LogAnomaly, Warning,
						TEXT("%s: %lld is outside %s; REFUSED, not clamped. The previous value stands."),
						K.Command, Requested, *KnobRange(K));
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

		struct FModeSetKnob
		{
			const TCHAR* Command;
			const TCHAR* IniKey;
			EFamily Family;
			uint32 Override = 0;
			bool bOverride = false;
			bool bResolved = false;
			uint32 Value = 0;
			const TCHAR* Source = TEXT("compiled");
		};

		FModeSetKnob GUvModes{ TEXT("IAI.Anomaly.TexCorruptUvModes"), TEXT("TexCorruptUvModesDefault"), EFamily::UV };
		FModeSetKnob GNormalModes{ TEXT("IAI.Anomaly.TexCorruptNormalModes"), TEXT("TexCorruptNormalModesDefault"), EFamily::Normal };

		FModeSetKnob& ModeKnobFor(EFamily Family)
		{
			return Family == EFamily::UV ? GUvModes : GNormalModes;
		}

		uint32 CompiledModeMask(EFamily Family)
		{
			return TexCorruptPure::DeliveredMask(ToPureFamily(Family));
		}

		bool ParseModeSetText(EFamily Family, const FString& Text, uint32& OutMask, FString& OutError)
		{
			const TexCorruptPure::FModeSetParse P = TexCorruptPure::ParseModeSet(*Text, ToPureFamily(Family));
			if (P.bOk)
			{
				OutMask = P.Mask;
				return true;
			}
			if (P.Why == TexCorruptPure::EModeArg::NoMode)
			{
				OutError = TEXT("empty value (write none for the empty set)");
			}
			else if (P.Why == TexCorruptPure::EModeArg::NotInDelivery && P.BadName)
			{
				OutError = FString::Printf(TEXT("not_in_delivery:%s"), ANSI_TO_TCHAR(P.BadName->Name));
			}
			else
			{
				OutError = FString::Printf(TEXT("%s:%s"), ANSI_TO_TCHAR(TexCorruptPure::LexModeArg(P.Why)), *Text.Mid(P.BadStart, P.BadLen));
			}
			return false;
		}

		uint32 ModeKnobGet(FModeSetKnob& K)
		{
			if (K.bOverride)
			{
				return K.Override;
			}
			if (!K.bResolved)
			{
				K.bResolved = true;
				K.Value = CompiledModeMask(K.Family);
				K.Source = TEXT("compiled");
				FString FromIni;
				if (GConfig && GConfig->GetString(AnomalyDefaults::SectionName(), K.IniKey, FromIni, GGameIni))
				{
					uint32 Mask = 0;
					FString Error;
					if (ParseModeSetText(K.Family, FromIni, Mask, Error))
					{
						K.Value = Mask;
						K.Source = TEXT("ini");
					}
					else
					{
						UE_LOG(LogAnomaly, Warning,
							TEXT("texcorrupt: DefaultGame.ini [%s] %s = '%s' is REFUSED (%s); the compiled set %s stands."),
							AnomalyDefaults::SectionName(), K.IniKey, *FromIni, *Error, *DescribeModeSet(K.Family, K.Value));
					}
				}
				UE_LOG(LogAnomaly, Log, TEXT("texcorrupt: %s = %s (%s)."), K.Command, *DescribeModeSet(K.Family, K.Value), K.Source);
			}
			return K.Value;
		}

		FString ModeKnobDescribe(FModeSetKnob& K)
		{
			const uint32 Mask = ModeKnobGet(K);
			return FString::Printf(TEXT("%s(%s)"), *DescribeModeSet(K.Family, Mask), K.bOverride ? TEXT("console") : K.Source);
		}

		void ModeKnobCommand(FModeSetKnob& K, const TArray<FString>& Args)
		{
			if (Args.Num() < 1)
			{
				UE_LOG(LogAnomaly, Warning, TEXT("Usage: %s <mode+mode|none|default>  (current: %s, delivered modes %s)"),
					K.Command, *ModeKnobDescribe(K), *DescribeModeSet(K.Family, CompiledModeMask(K.Family)));
				return;
			}
			const FString Text = FString::Join(Args, TEXT(""));
			if (Text.Equals(TEXT("default"), ESearchCase::IgnoreCase))
			{
				K.bOverride = false;
			}
			else
			{
				uint32 Mask = 0;
				FString Error;
				if (ParseModeSetText(K.Family, Text, Mask, Error))
				{
					K.Override = Mask;
					K.bOverride = true;
				}
				else
				{
					UE_LOG(LogAnomaly, Warning, TEXT("%s: '%s' is REFUSED (%s). The previous set stands."), K.Command, *Text, *Error);
				}
			}
			UE_LOG(LogAnomaly, Log,
				TEXT("%s: EFFECTIVE READ-BACK = %s. Precedence: console > DefaultGame.ini [%s] %s > compiled %s. The set drives the ")
				TEXT("auto-pool mode draw only; a targeted fire may name any delivered mode of the family."),
				K.Command, *ModeKnobDescribe(K), AnomalyDefaults::SectionName(), K.IniKey,
				*DescribeModeSet(K.Family, CompiledModeMask(K.Family)));
		}

		FLevers GLevers;
		FLedger GLedger;
		FRunStats GStats;
		FThreadSafeCounter GTripwire;
		TexCorruptPure::FAttemptOrdinals GOrdinals;

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

		FAutoConsoleCommand GTexCorruptTileNCmd(
			TEXT("IAI.Anomaly.TexCorruptTileN"),
			TEXT("m53: the tile factor N of uv_corruption mode tile (UvScale = N; output mip m reads source mip ")
			TEXT("min(m + log2 N, M - 1)). Compiled 8; ini [AnomalyInjector] TexCorruptTileNDefault; only 2, 4, 8 or 16 is ")
			TEXT("admitted, anything else is refused. Usage: IAI.Anomaly.TexCorruptTileN <2|4|8|16|default>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args) { KnobCommand(GTileN, Args); }));

		FAutoConsoleCommand GTexCorruptScrambleKCmd(
			TEXT("IAI.Anomaly.TexCorruptScrambleK"),
			TEXT("m53: the cell grid K of uv_corruption mode scramble (K x K cells, cell i drawn from a^-1 (i - b) mod K^2, the ")
			TEXT("pair derived per attempt from the capture seed and the attempt ordinal). Compiled 8; ini [AnomalyInjector] ")
			TEXT("TexCorruptScrambleKDefault; out of [2, 64] is refused. Usage: IAI.Anomaly.TexCorruptScrambleK <k|default>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args) { KnobCommand(GScrambleK, Args); }));

		FAutoConsoleCommand GTexCorruptUvModesCmd(
			TEXT("IAI.Anomaly.TexCorruptUvModes"),
			TEXT("m53: the uv_corruption modes the auto-pool may draw, +-joined (tile+scramble), or none. Compiled tile+scramble; ")
			TEXT("ini [AnomalyInjector] TexCorruptUvModesDefault. An unknown, other-family or not-delivered name is refused. ")
			TEXT("With none every auto attempt still draws once and refuses mode_invalid:no_mode_enabled. ")
			TEXT("Usage: IAI.Anomaly.TexCorruptUvModes <tile+scramble|tile|scramble|none|default>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args) { ModeKnobCommand(GUvModes, Args); }));

		FAutoConsoleCommand GTexCorruptNormalModesCmd(
			TEXT("IAI.Anomaly.TexCorruptNormalModes"),
			TEXT("m53: the normal_corruption modes the auto-pool may draw, +-joined (invert+green_flip), or none. Compiled ")
			TEXT("invert+green_flip; ini [AnomalyInjector] TexCorruptNormalModesDefault. An unknown, other-family or ")
			TEXT("not-delivered name is refused. With none every auto attempt still draws once and refuses ")
			TEXT("mode_invalid:no_mode_enabled. Usage: IAI.Anomaly.TexCorruptNormalModes <invert+green_flip|invert|green_flip|none|default>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args) { ModeKnobCommand(GNormalModes, Args); }));
	}

	int32 GetMaxRtBytes() { return KnobGet(GMaxRtBytes); }
	FString DescribeMaxRtBytes() { return KnobDescribe(GMaxRtBytes); }
	int32 GetMinTexturePx() { return KnobGet(GMinTexturePx); }
	FString DescribeMinTexturePx() { return KnobDescribe(GMinTexturePx); }
	int32 GetMaxTextures() { return KnobGet(GMaxTextures); }
	FString DescribeMaxTextures() { return KnobDescribe(GMaxTextures); }
	int32 GetTileN() { return KnobGet(GTileN); }
	FString DescribeTileN() { return KnobDescribe(GTileN); }
	int32 GetScrambleK() { return KnobGet(GScrambleK); }
	FString DescribeScrambleK() { return KnobDescribe(GScrambleK); }
	uint32 GetEnabledModeMask(EFamily Family) { return ModeKnobGet(ModeKnobFor(Family)); }
	FString DescribeEnabledModes(EFamily Family) { return ModeKnobDescribe(ModeKnobFor(Family)); }

	FString DescribeModeSet(EFamily Family, uint32 Mask)
	{
		TexCorruptPure::EModeP Modes[TexCorruptPure::NumModeNames];
		const int32 N = TexCorruptPure::DeliveredModesInOrder(ToPureFamily(Family), Mask, Modes, TexCorruptPure::NumModeNames);
		if (N == 0)
		{
			return TEXT("none");
		}
		FString Out;
		for (int32 i = 0; i < N; ++i)
		{
			Out += (i == 0 ? TEXT("") : TEXT("+"));
			Out += ANSI_TO_TCHAR(TexCorruptPure::LexModeP(Modes[i]));
		}
		return Out;
	}

	void EchoRunStartKnobs()
	{
		UE_LOG(LogAnomaly, Log,
			TEXT("texcorrupt: run-start knobs uv_modes=%s normal_modes=%s tile_n=%s scramble_k=%s cap=%s min_px=%s max_textures=%s ")
			TEXT("(console > DefaultGame.ini [%s] > compiled; the mode sets drive the auto-pool draw only)."),
			*DescribeEnabledModes(EFamily::UV), *DescribeEnabledModes(EFamily::Normal), *DescribeTileN(), *DescribeScrambleK(),
			*DescribeMaxRtBytes(), *DescribeMinTexturePx(), *DescribeMaxTextures(), AnomalyDefaults::SectionName());
	}

	int32 TakeAttemptOrdinal(EFamily Family)
	{
		return GOrdinals.Take(ToPureFamily(Family));
	}

	TexCorruptPure::FScramble DeriveScramble(uint32 Seed, int32 Ordinal, int32 K)
	{
		return TexCorruptPure::ScramblePair(Seed, (uint32)Ordinal, K,
			[](const unsigned char* Bytes, int Length) { return FCrc::MemCrc32(Bytes, Length, 0); });
	}

	FString FAttemptInfo::Describe() const
	{
		if (Ordinal < 0)
		{
			return FString();
		}
		FString Out = FString::Printf(TEXT(" ordinal=%d"), Ordinal);
		if (bScramble)
		{
			Out += FString::Printf(TEXT(" scramble_k=%d scramble_h=%u a=%d b=%d a_inv=%d"), Scramble.K, Scramble.H, Scramble.A,
				Scramble.B, Scramble.AInv);
		}
		return Out;
	}

	TArray<FString> GetAutoDrawModes(FName Id)
	{
		TArray<FString> Out;
		static const FName Uv(TEXT("uv_corruption"));
		static const FName Normal(TEXT("normal_corruption"));
		if (Id != Uv && Id != Normal)
		{
			return Out;
		}
		const EFamily Family = Id == Uv ? EFamily::UV : EFamily::Normal;
		TexCorruptPure::EModeP Modes[TexCorruptPure::NumModeNames];
		const int32 N = TexCorruptPure::DeliveredModesInOrder(ToPureFamily(Family), GetEnabledModeMask(Family), Modes,
			TexCorruptPure::NumModeNames);
		for (int32 i = 0; i < N; ++i)
		{
			Out.Add(ANSI_TO_TCHAR(TexCorruptPure::LexModeP(Modes[i])));
		}
		return Out;
	}

	FLevers& Levers()
	{
		return GLevers;
	}

	FString DescribeLevers()
	{
		return FString::Printf(TEXT("noapply=%d wrongcopy=%s identity=%d identity_redraw=%d tile_probe=%d force_missing_asset=%d ")
			TEXT("fail_step=%d fail_alloc_ordinal=%d foreign_replace=%d collateral_detail=%d commit_delay=%d restore_delay=%d"),
			GLevers.NoApply, LexWrongCopy(GLevers.WrongCopy), GLevers.bIdentity ? 1 : 0, GLevers.bIdentityRedraw ? 1 : 0,
			GLevers.TileProbe, GLevers.bForceMissingAsset ? 1 : 0, GLevers.FailStep, GLevers.FailAllocOrdinal,
			GLevers.bForeignReplace ? 1 : 0, GLevers.bCollateralDetail ? 1 : 0, GLevers.CommitDelay, GLevers.RestoreDelay);
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
		GOrdinals.Reset();
		EchoRunStartKnobs();
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
			Why::MapSetOverCap, Why::NoEligibleSlot, Why::PartialFootprint, Why::OverBudget, Why::RtAllocFailed,
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
