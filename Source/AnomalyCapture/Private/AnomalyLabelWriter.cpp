#include "AnomalyLabelWriter.h"

#include "AnomalyCaptureLog.h"

#if ANOMALY_CAPTURE

#include "AnomalyPreviewCapture.h"
#include "AnomalyViewport.h"
#include "AnomalyAutoInjectorSubsystem.h"
#include "AnomalyCensus.h"
#include "AnomalyLabelSync.h"
#include "AnomalyCaptureSubsystem.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "CoreGlobals.h"
#include "HAL/IConsoleManager.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformTime.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Math/Float16Color.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Policies/PrettyJsonPrintPolicy.h"

namespace
{
	TSharedPtr<FJsonValue> LabelNum(double V) { return MakeShared<FJsonValueNumber>(V); }

	TArray<TSharedPtr<FJsonValue>> LabelVec3(double X, double Y, double Z)
	{
		return { LabelNum(X), LabelNum(Y), LabelNum(Z) };
	}

	AnomalyLabelSync::EEntryEmit EmitAt(const TArray<uint8>* Modes, int32 Index)
	{
		return (Modes && Modes->IsValidIndex(Index))
			? (AnomalyLabelSync::EEntryEmit)(*Modes)[Index] : AnomalyLabelSync::EEntryEmit::Normal;
	}

	bool TransitionAt(const TArray<uint8>* Modes, const TArray<uint8>* Transition, int32 Index)
	{
		const AnomalyLabelSync::EEntryEmit Mode = EmitAt(Modes, Index);
		if (Mode == AnomalyLabelSync::EEntryEmit::TransitionOnly)
		{
			return true;
		}
		return Mode == AnomalyLabelSync::EEntryEmit::Normal
			&& Transition && Transition->IsValidIndex(Index) && (*Transition)[Index] != 0;
	}

	AnomalyLabel::FLabelEntryCounts CountEntries(int32 NumFires, const TArray<uint8>* Modes, const TArray<uint8>* Transition,
		const TArray<FAutoLiveFireInfo>* TransitionFires)
	{
		AnomalyLabel::FLabelEntryCounts C;
		for (int32 i = 0; i < NumFires; ++i)
		{
			const AnomalyLabelSync::EEntryEmit Mode = EmitAt(Modes, i);
			if (Mode == AnomalyLabelSync::EEntryEmit::Normal)
			{
				C.bPresent = true;
			}
			else if (Mode == AnomalyLabelSync::EEntryEmit::Suppress)
			{
				++C.Suppressed;
			}
			if (TransitionAt(Modes, Transition, i))
			{
				++C.TransitionEntries;
			}
		}
		if (TransitionFires)
		{
			C.TransitionEntries += TransitionFires->Num();
		}
		return C;
	}

	FString BuildFrameLabelRecord(const TArray<FAutoLiveFireInfo>& Fires,
		const FAnomalyViewInfo& View, int32 W, int32 H, uint64 FrameIndex, int32 SessionIndex, double TimeSeconds,
		double WallSeconds, const FString& ImageName, int32& OutNumLabels,
		bool bTargetMask = false, const FString& MaskFileRel = FString(), const TArray<int32>* MaskValues = nullptr,
		AnomalyLabel::EAnomalyMaskState MaskState = AnomalyLabel::EAnomalyMaskState::Unmeasured,
		int32 ShadersPending = 0, int32 AnomalyMaterialsIncomplete = 0, bool bExposureDip = false,
		const TArray<int32>* TargetPixels = nullptr, const TArray<uint8>* Observable = nullptr,
		const TArray<FIntRect>* DrawnBounds = nullptr, const TArray<int32>* TargetDrawnPixels = nullptr,
		bool bExposureDipScopeExcluded = false, const TArray<FAnomalyTelemetry>* Telemetry = nullptr,
		const TArray<uint8>* EntryEmit = nullptr, const TArray<uint8>* EntryTransition = nullptr,
		const TArray<FAutoLiveFireInfo>* TransitionFires = nullptr,
		const AnomalyLabel::FCameraClipFrameDiag* CameraClip = nullptr)
	{
		OutNumLabels = 0;

		TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
		Root->SetNumberField(TEXT("frame_index"), (double)FrameIndex);
		Root->SetNumberField(TEXT("session_index"), (double)SessionIndex);
		Root->SetNumberField(TEXT("t"), TimeSeconds);
		Root->SetNumberField(TEXT("t_wall"), WallSeconds);
		Root->SetStringField(TEXT("image"), ImageName);
		Root->SetNumberField(TEXT("width"), W);
		Root->SetNumberField(TEXT("height"), H);
		const AnomalyLabel::FLabelEntryCounts Counts = CountEntries(Fires.Num(), EntryEmit, EntryTransition, TransitionFires);
		Root->SetBoolField(TEXT("anomaly_present"), Counts.bPresent);
		if (Counts.TransitionEntries > 0)
		{
			Root->SetBoolField(TEXT("transition_present"), true);
		}

		TArray<TSharedPtr<FJsonValue>> Anoms;
		auto EmitEntry = [&](const FAutoLiveFireInfo& F, int32 FireIndex, bool bTransition, bool bSetsPresent)
		{
			TSharedRef<FJsonObject> O = MakeShared<FJsonObject>();
			O->SetStringField(TEXT("id"), F.Id.ToString());
			O->SetStringField(TEXT("target_name"), F.Target);
			if (bTargetMask)
			{
				const int32 MaskValue =
					(MaskValues && MaskValues->IsValidIndex(FireIndex)) ? (*MaskValues)[FireIndex] : 0;
				O->SetNumberField(TEXT("mask_value"), MaskValue);
			}

			const int32 Px = (TargetPixels && TargetPixels->IsValidIndex(FireIndex))
				? (*TargetPixels)[FireIndex] : AnomalyLabel::GTargetPixelsUnmeasured;
			O->SetNumberField(TEXT("target_pixels"), Px);

			const int32 DrawnPx = (TargetDrawnPixels && TargetDrawnPixels->IsValidIndex(FireIndex))
				? (*TargetDrawnPixels)[FireIndex] : AnomalyLabel::GTargetPixelsUnmeasured;
			O->SetNumberField(TEXT("target_drawn_pixels"), DrawnPx);

			const uint8 Obs = (Observable && Observable->IsValidIndex(FireIndex))
				? (*Observable)[FireIndex] : (uint8)AnomalyLabel::EObservable::Unmeasured;
			if (Obs == (uint8)AnomalyLabel::EObservable::Unmeasured)
			{
				O->SetField(TEXT("observable"), MakeShared<FJsonValueNull>());
			}
			else
			{
				O->SetBoolField(TEXT("observable"), Obs == (uint8)AnomalyLabel::EObservable::True);
			}

			O->SetNumberField(TEXT("seconds_remaining"), F.SecondsRemaining);
			O->SetNumberField(TEXT("start_frame"), (double)F.StartFrame);

			FVector2D Min(FVector2D::ZeroVector);
			FVector2D Max(FVector2D::ZeroVector);
			bool bValid = false;
			if (F.bWholeFrameExtent || (F.TargetActor.Get() == nullptr && F.Target.IsEmpty()))
			{
				Min = FVector2D(0.0, 0.0);
				Max = FVector2D(1.0, 1.0);
				bValid = true;
			}
			else if (const AActor* Actor = F.TargetActor.Get())
			{
				bValid = AnomalyViewport::ProjectActorBoundsToScreenRect(View, Actor, Min, Max);
			}
			O->SetBoolField(TEXT("bbox_valid"), bValid);

			O->SetArrayField(TEXT("bbox_norm"), { LabelNum(Min.X), LabelNum(Min.Y), LabelNum(Max.X), LabelNum(Max.Y) });

			const double X0 = FMath::Clamp((double)Min.X * W, 0.0, (double)W);
			const double Y0 = FMath::Clamp((double)Min.Y * H, 0.0, (double)H);
			const double X1 = FMath::Clamp((double)Max.X * W, 0.0, (double)W);
			const double Y1 = FMath::Clamp((double)Max.Y * H, 0.0, (double)H);
			O->SetArrayField(TEXT("bbox_px"), { LabelNum(X0), LabelNum(Y0), LabelNum(X1 - X0), LabelNum(Y1 - Y0) });

			const FIntRect Drawn = (DrawnBounds && DrawnBounds->IsValidIndex(FireIndex))
				? (*DrawnBounds)[FireIndex] : FIntRect();
			if (Drawn.Width() > 0 && Drawn.Height() > 0)
			{
				O->SetArrayField(TEXT("bbox_drawn_px"),
					{ LabelNum(Drawn.Min.X), LabelNum(Drawn.Min.Y),
					  LabelNum(Drawn.Width()), LabelNum(Drawn.Height()) });
			}
			else
			{
				O->SetField(TEXT("bbox_drawn_px"), MakeShared<FJsonValueNull>());
			}

			if (Telemetry && Telemetry->IsValidIndex(FireIndex))
			{
				const FAnomalyTelemetry& T = (*Telemetry)[FireIndex];
				for (const TPair<FString, int32>& KV : T.Ints)
				{
					O->SetNumberField(KV.Key, (double)KV.Value);
				}
				for (const TPair<FString, double>& KV : T.Floats)
				{
					O->SetNumberField(KV.Key, KV.Value);
				}
				for (const TPair<FString, bool>& KV : T.Bools)
				{
					O->SetBoolField(KV.Key, KV.Value);
				}
				for (const TPair<FString, FString>& KV : T.Strings)
				{
					O->SetStringField(KV.Key, KV.Value);
				}
				for (const TPair<FString, TArray<FAnomalyTelemetryFields>>& KV : T.Arrays)
				{
					TArray<TSharedPtr<FJsonValue>> Entries;
					Entries.Reserve(KV.Value.Num());
					for (const FAnomalyTelemetryFields& Rec : KV.Value)
					{
						TSharedRef<FJsonObject> E = MakeShared<FJsonObject>();
						for (const TPair<FString, int32>& RV : Rec.Ints)
						{
							E->SetNumberField(RV.Key, (double)RV.Value);
						}
						for (const TPair<FString, double>& RV : Rec.Floats)
						{
							E->SetNumberField(RV.Key, RV.Value);
						}
						for (const TPair<FString, bool>& RV : Rec.Bools)
						{
							E->SetBoolField(RV.Key, RV.Value);
						}
						for (const TPair<FString, FString>& RV : Rec.Strings)
						{
							E->SetStringField(RV.Key, RV.Value);
						}
						Entries.Add(MakeShared<FJsonValueObject>(E));
					}
					O->SetArrayField(KV.Key, Entries);
				}
			}

			if (bTransition)
			{
				O->SetNumberField(TEXT("transition"), 1);
			}
			if (bValid && bSetsPresent)
			{
				++OutNumLabels;
			}
			Anoms.Add(MakeShared<FJsonValueObject>(O));
		};
		for (int32 FireIndex = 0; FireIndex < Fires.Num(); ++FireIndex)
		{
			const AnomalyLabelSync::EEntryEmit Mode = EmitAt(EntryEmit, FireIndex);
			if (Mode == AnomalyLabelSync::EEntryEmit::Suppress)
			{
				continue;
			}
			EmitEntry(Fires[FireIndex], FireIndex, TransitionAt(EntryEmit, EntryTransition, FireIndex),
				Mode == AnomalyLabelSync::EEntryEmit::Normal);
		}
		if (TransitionFires)
		{
			for (const FAutoLiveFireInfo& F : *TransitionFires)
			{
				EmitEntry(F, INDEX_NONE, true, false);
			}
		}
		Root->SetArrayField(TEXT("anomalies"), Anoms);

		Root->SetBoolField(TEXT("visible_positive"), Counts.bPresent && (OutNumLabels > 0));

		if (bTargetMask)
		{
			const bool bPresent = (MaskState == AnomalyLabel::EAnomalyMaskState::Present) && !MaskFileRel.IsEmpty();
			if (bPresent)
			{
				Root->SetStringField(TEXT("mask_file"), MaskFileRel);
			}
			else
			{
				Root->SetField(TEXT("mask_file"), MakeShared<FJsonValueNull>());
			}
			Root->SetStringField(TEXT("mask_state"), AnomalyLabel::DescribeMaskState(MaskState));
		}

		if (bExposureDip)
		{
			Root->SetBoolField(TEXT("exposure_dip"), true);
			Root->SetStringField(TEXT("exposure_dip_scope"),
				bExposureDipScopeExcluded ? TEXT("frame_minus_targets") : TEXT("frame"));
		}

		if (AnomalyMaterialsIncomplete > 0)
		{
			Root->SetStringField(TEXT("render_state"), TEXT("shaders_pending"));
			Root->SetNumberField(TEXT("anomaly_materials_incomplete"), AnomalyMaterialsIncomplete);
			Root->SetNumberField(TEXT("shader_jobs_pending"), ShadersPending);
		}

		if (CameraClip && CameraClip->bPresent)
		{
			Root->SetBoolField(TEXT("camera_clipping.slab"), CameraClip->bSlab);
			Root->SetBoolField(TEXT("camera_clipping.sphere_proxy"), CameraClip->bSphereProxy);
			Root->SetNumberField(TEXT("camera_clipping.slab_primitives"), CameraClip->SlabPrimitives);
			Root->SetNumberField(TEXT("camera_clipping.eye_inside_box"), CameraClip->EyeInsideBox);
			if (CameraClip->bNearOverridden)
			{
				Root->SetBoolField(TEXT("camera_clipping.near_overridden"), true);
			}
		}

		TSharedRef<FJsonObject> V = MakeShared<FJsonObject>();
		V->SetArrayField(TEXT("origin"), LabelVec3(View.Origin.X, View.Origin.Y, View.Origin.Z));
		V->SetArrayField(TEXT("rot"), LabelVec3(View.Rotation.Pitch, View.Rotation.Yaw, View.Rotation.Roll));
		V->SetNumberField(TEXT("fovDeg"), View.HorizontalFOVDeg);
		V->SetNumberField(TEXT("aspect"), View.AspectRatio);
		V->SetBoolField(TEXT("valid"), View.bValid);
		Root->SetObjectField(TEXT("view"), V);

		FString Out;
		const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Out);
		FJsonSerializer::Serialize(Root, Writer);
		return Out;
	}

	bool AppendRecordAndImage(const FString& OutputDir, const TArray<uint8>& ImageBytes,
		const FString& Record, const FString& ImageRelName, FString& OutImagePath, FString& OutSidecarPath, bool bLog,
		bool bWriteLabels)
	{
		const FString ImagePath = FPaths::Combine(OutputDir, ImageRelName);
		const FString SidecarPath = FPaths::Combine(OutputDir, TEXT("labels.jsonl"));

		IFileManager::Get().MakeDirectory(*FPaths::GetPath(ImagePath), true);
		if (!FFileHelper::SaveArrayToFile(ImageBytes, *ImagePath))
		{
			if (bLog)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture: failed to write image '%s'."), *ImagePath);
			}
			return false;
		}

		if (bWriteLabels)
		{
			FFileHelper::SaveStringToFile(Record + TEXT("\n"), *SidecarPath,
				FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM, &IFileManager::Get(), FILEWRITE_Append);
		}

		OutImagePath = ImagePath;
		OutSidecarPath = SidecarPath;
		return true;
	}

}

namespace AnomalyLabel
{
	static FColor DecodeTightPixel(EPixelFormat Format, const uint8* P)
	{
		switch (Format)
		{
		case PF_B8G8R8A8:
			return FColor(P[2], P[1], P[0], 255);
		case PF_R8G8B8A8:
			return FColor(P[0], P[1], P[2], 255);
		case PF_A2B10G10R10:
		{
			const uint32 V = *reinterpret_cast<const uint32*>(P);
			const uint8 R = (uint8)(((V >> 0) & 0x3FF) >> 2);
			const uint8 G = (uint8)(((V >> 10) & 0x3FF) >> 2);
			const uint8 B = (uint8)(((V >> 20) & 0x3FF) >> 2);
			return FColor(R, G, B, 255);
		}
		case PF_FloatRGBA:
		{
			const FFloat16* H16 = reinterpret_cast<const FFloat16*>(P);
			auto ToByte = [](const FFloat16& In) -> uint8
			{
				return (uint8)FMath::Clamp(FMath::RoundToInt(In.GetFloat() * 255.0f), 0, 255);
			};
			return FColor(ToByte(H16[0]), ToByte(H16[1]), ToByte(H16[2]), 255);
		}
		default:
			return FColor(0, 0, 0, 255);
		}
	}

	void ConvertTightToBGRA(EPixelFormat Format, int32 BytesPerPixel, const TArray<uint8>& RawBytes,
		int32 W, int32 H, TArray<FColor>& OutPixels)
	{
		OutPixels.SetNumUninitialized(W * H);
		const uint8* Base = RawBytes.GetData();

		for (int32 i = 0; i < W * H; ++i)
		{
			OutPixels[i] = DecodeTightPixel(Format, Base + (int64)i * BytesPerPixel);
		}
	}

	void ComputeSubsampledMeanLumaSplit(EPixelFormat Format, int32 BytesPerPixel, const TArray<uint8>& RawBytes,
		int32 W, int32 H, int32 Stride, const TArray<uint8>* ExclusionMask,
		double& OutLumaAll, double& OutLumaExcl)
	{
		OutLumaAll = -1.0;
		OutLumaExcl = -1.0;

		if (W <= 0 || H <= 0 || BytesPerPixel <= 0 || Stride <= 0)
		{
			return;
		}
		if ((int64)RawBytes.Num() < (int64)W * (int64)H * (int64)BytesPerPixel)
		{
			return;
		}

		const bool bHaveMask = ExclusionMask && (int64)ExclusionMask->Num() >= (int64)W * (int64)H;
		const uint8* Excl = bHaveMask ? ExclusionMask->GetData() : nullptr;

		const uint8* Base = RawBytes.GetData();
		double Sum = 0.0;
		double SumExcl = 0.0;
		int64 N = 0;
		int64 NExcl = 0;
		for (int32 y = 0; y < H; y += Stride)
		{
			const uint8* Row = Base + (int64)y * (int64)W * (int64)BytesPerPixel;
			const uint8* ExclRow = Excl ? (Excl + (int64)y * (int64)W) : nullptr;
			for (int32 x = 0; x < W; x += Stride)
			{
				const FColor C = DecodeTightPixel(Format, Row + (int64)x * BytesPerPixel);
				const double L = 0.299 * (double)C.R + 0.587 * (double)C.G + 0.114 * (double)C.B;
				Sum += L;
				++N;
				if (!ExclRow || ExclRow[x] == 0)
				{
					SumExcl += L;
					++NExcl;
				}
			}
		}
		OutLumaAll = (N > 0) ? (Sum / (double)N) : -1.0;
		OutLumaExcl = (NExcl > 0) ? (SumExcl / (double)NExcl) : OutLumaAll;
	}

	double ComputeSubsampledMeanLuma(EPixelFormat Format, int32 BytesPerPixel, const TArray<uint8>& RawBytes,
		int32 W, int32 H, int32 Stride)
	{
		double All = -1.0;
		double Excl = -1.0;
		ComputeSubsampledMeanLumaSplit(Format, BytesPerPixel, RawBytes, W, H, Stride, nullptr, All, Excl);
		return All;
	}
	void DeriveOutputSize(int32 SrcW, int32 SrcH, int32 TargetH, int32& OutW, int32& OutH, bool& bOutNeedsResample)
	{
		OutW = SrcW;
		OutH = SrcH;
		bOutNeedsResample = false;

		if (SrcW <= 0 || SrcH <= 0 || TargetH <= 0 || TargetH >= SrcH)
		{
			return;
		}

		const int32 SnappedH = FMath::Max(2, 2 * ((TargetH + 1) / 2));
		if (SnappedH >= SrcH)
		{
			return;
		}

		int32 DerivedW = FMath::RoundToInt((double)SnappedH * (double)SrcW / (double)SrcH);
		DerivedW = FMath::Max(2, 2 * ((DerivedW + 1) / 2));
		if (DerivedW > SrcW)
		{
			DerivedW = FMath::Max(2, (SrcW / 2) * 2);
		}

		if (DerivedW == SrcW && SnappedH == SrcH)
		{
			return;
		}

		OutW = DerivedW;
		OutH = SnappedH;
		bOutNeedsResample = true;
	}

	bool ResampleAndEncodeBGRA(AnomalyPreview::EImageFormat Format, const TArray<FColor>& Pixels,
		int32 SrcW, int32 SrcH, int32 OutW, int32 OutH, TArray<uint8>& OutBytes, bool& bOutResampled)
	{
		bOutResampled = false;

		if (SrcW <= 0 || SrcH <= 0 || Pixels.Num() < SrcW * SrcH)
		{
			return false;
		}

		if (OutW <= 0 || OutH <= 0 || (OutW == SrcW && OutH == SrcH))
		{
			return AnomalyPreview::EncodePixels(Format, Pixels, SrcW, SrcH, OutBytes);
		}

		TArray<FColor> Scaled;
		Scaled.SetNumUninitialized(OutW * OutH);

		const double ScaleX = (double)SrcW / (double)OutW;
		const double ScaleY = (double)SrcH / (double)OutH;
		const FColor* Base = Pixels.GetData();

		for (int32 Dy = 0; Dy < OutH; ++Dy)
		{
			const double SrcY0 = (double)Dy * ScaleY;
			const double SrcY1 = (double)(Dy + 1) * ScaleY;
			const int32 Y0 = FMath::Clamp((int32)FMath::FloorToDouble(SrcY0), 0, SrcH - 1);
			const int32 Y1 = FMath::Clamp((int32)FMath::CeilToDouble(SrcY1) - 1, 0, SrcH - 1);

			for (int32 Dx = 0; Dx < OutW; ++Dx)
			{
				const double SrcX0 = (double)Dx * ScaleX;
				const double SrcX1 = (double)(Dx + 1) * ScaleX;
				const int32 X0 = FMath::Clamp((int32)FMath::FloorToDouble(SrcX0), 0, SrcW - 1);
				const int32 X1 = FMath::Clamp((int32)FMath::CeilToDouble(SrcX1) - 1, 0, SrcW - 1);

				double AccR = 0.0, AccG = 0.0, AccB = 0.0, AccWeight = 0.0;
				for (int32 Sy = Y0; Sy <= Y1; ++Sy)
				{
					const double WeightY = FMath::Min(SrcY1, (double)(Sy + 1)) - FMath::Max(SrcY0, (double)Sy);
					if (WeightY <= 0.0)
					{
						continue;
					}
					const FColor* Row = Base + (int64)Sy * SrcW;
					for (int32 Sx = X0; Sx <= X1; ++Sx)
					{
						const double WeightX = FMath::Min(SrcX1, (double)(Sx + 1)) - FMath::Max(SrcX0, (double)Sx);
						if (WeightX <= 0.0)
						{
							continue;
						}
						const double Weight = WeightX * WeightY;
						const FColor& C = Row[Sx];
						AccR += (double)C.R * Weight;
						AccG += (double)C.G * Weight;
						AccB += (double)C.B * Weight;
						AccWeight += Weight;
					}
				}

				FColor& Out = Scaled[(int64)Dy * OutW + Dx];
				if (AccWeight > 0.0)
				{
					Out = FColor(
						(uint8)FMath::Clamp(FMath::RoundToInt(AccR / AccWeight), 0, 255),
						(uint8)FMath::Clamp(FMath::RoundToInt(AccG / AccWeight), 0, 255),
						(uint8)FMath::Clamp(FMath::RoundToInt(AccB / AccWeight), 0, 255),
						255);
				}
				else
				{
					Out = FColor(0, 0, 0, 255);
				}
			}
		}

		bOutResampled = true;
		return AnomalyPreview::EncodePixels(Format, Scaled, OutW, OutH, OutBytes);
	}

	bool CaptureLabeledShot(UWorld* World, const FString& OutputDir, AnomalyPreview::EImageFormat Format,
		const FAnomalyViewInfo& ProjectionView, const FString& ImageRelName, int32 SessionIndex,
		double WallSeconds, int32 TargetOutputHeight, FString& OutImagePath, FString& OutSidecarPath,
		int32& OutNumLabels, int32& OutNativeW, int32& OutNativeH, int32& OutWrittenW, int32& OutWrittenH,
		bool& bOutResampled, bool bLog, bool bWriteLabels)
	{
		OutNumLabels = 0;
		OutNativeW = 0;
		OutNativeH = 0;
		OutWrittenW = 0;
		OutWrittenH = 0;
		bOutResampled = false;
		if (!World)
		{
			return false;
		}

		TArray<FAutoLiveFireInfo> Fires;
		if (UAnomalyAutoInjectorSubsystem* Auto = World->GetSubsystem<UAnomalyAutoInjectorSubsystem>())
		{
			Fires = Auto->GetLiveFires();
		}

		TArray<FColor> Pixels;
		int32 W = 0, H = 0;
		if (!AnomalyPreview::CaptureGameViewportRaw(World, Pixels, W, H))
		{
			if (bLog)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture: game-viewport capture failed (no game viewport?)."));
			}
			return false;
		}

		OutNativeW = W;
		OutNativeH = H;

		int32 OutW = W, OutH = H;
		bool bNeedsResample = false;
		DeriveOutputSize(W, H, TargetOutputHeight, OutW, OutH, bNeedsResample);

		TArray<uint8> ImageBytes;
		if (!ResampleAndEncodeBGRA(Format, Pixels, W, H, OutW, OutH, ImageBytes, bOutResampled))
		{
			if (bLog)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture: encode failed for '%s' (%dx%d -> %dx%d)."),
					*ImageRelName, W, H, OutW, OutH);
			}
			return false;
		}

		const FString Record = BuildFrameLabelRecord(Fires, ProjectionView, OutW, OutH, GFrameCounter, SessionIndex,
			World->GetTimeSeconds(), WallSeconds, ImageRelName, OutNumLabels);

		if (!AppendRecordAndImage(OutputDir, ImageBytes, Record, ImageRelName, OutImagePath, OutSidecarPath, bLog, bWriteLabels))
		{
			return false;
		}

		OutWrittenW = OutW;
		OutWrittenH = OutH;
		return true;
	}

	FString BuildLabelRecordForSnapshot(const FCaptureSnapshot& Snapshot, int32 Width, int32 Height,
		const FString& ImageName, int32& OutNumLabels)
	{
		return BuildFrameLabelRecord(Snapshot.Fires, Snapshot.View, Width, Height,
			Snapshot.FrameCounter, Snapshot.SessionIndex, Snapshot.TimeSeconds, Snapshot.WallSeconds, ImageName, OutNumLabels,
			Snapshot.bTargetMask, Snapshot.MaskFileRel, &Snapshot.MaskValues, Snapshot.MaskState,
			Snapshot.ShadersPending, Snapshot.AnomalyMaterialsIncomplete, Snapshot.bExposureDip,
			&Snapshot.TargetPixels, &Snapshot.Observable, &Snapshot.DrawnBounds,
			&Snapshot.TargetDrawnPixels, Snapshot.bExposureDipScopeExcluded, &Snapshot.Telemetry,
			&Snapshot.EntryEmit, &Snapshot.EntryTransition, &Snapshot.TransitionFires, &Snapshot.CameraClip);
	}

	FLabelEntryCounts CountLabelEntries(const FCaptureSnapshot& Snapshot)
	{
		return CountEntries(Snapshot.Fires.Num(), &Snapshot.EntryEmit, &Snapshot.EntryTransition, &Snapshot.TransitionFires);
	}

	bool EncodeAndWriteFrame(const FString& OutputDir, AnomalyPreview::EImageFormat OutFormat,
		const TArray<uint8>& RawBytes, EPixelFormat SrcFormat, int32 BytesPerPixel, int32 Width, int32 Height,
		int32 OutWidth, int32 OutHeight, const FString& ImageRelPath, const FString& Record,
		FCriticalSection& JsonlLock, bool bWriteLabels, bool& bOutResampled,
		TSharedPtr<const TArray<FColor>, ESPMode::ThreadSafe>* CanonicalPixels)
	{
		bOutResampled = false;

		if (Width <= 0 || Height <= 0 || BytesPerPixel <= 0 || RawBytes.Num() < (int64)Width * Height * BytesPerPixel)
		{
			return false;
		}

		TArray<FColor> Pixels;
		ConvertTightToBGRA(SrcFormat, BytesPerPixel, RawBytes, Width, Height, Pixels);
		if (CanonicalPixels) { *CanonicalPixels = MakeShared<const TArray<FColor>, ESPMode::ThreadSafe>(MoveTemp(Pixels)); }
		const TArray<FColor>& EncoderPixels = CanonicalPixels ? **CanonicalPixels : Pixels;

		TArray<uint8> ImageBytes;
		if (!ResampleAndEncodeBGRA(OutFormat, EncoderPixels, Width, Height, OutWidth, OutHeight, ImageBytes, bOutResampled))
		{
			return false;
		}

		const FString ImagePath = FPaths::Combine(OutputDir, ImageRelPath);

		IFileManager::Get().MakeDirectory(*FPaths::GetPath(ImagePath), true);
		if (!FFileHelper::SaveArrayToFile(ImageBytes, *ImagePath))
		{
			return false;
		}

		if (bWriteLabels)
		{
			FScopeLock Lock(&JsonlLock);
			FFileHelper::SaveStringToFile(Record + TEXT("\n"), *FPaths::Combine(OutputDir, TEXT("labels.jsonl")),
				FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM, &IFileManager::Get(), FILEWRITE_Append);
		}

		return true;
	}

	bool WriteTargetMaskMap(const FString& RunDir, const TArray<FTargetMaskMapEntry>& Entries)
	{
		TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
		Root->SetStringField(TEXT("type"), TEXT("target_mask_map"));
		Root->SetStringField(TEXT("note"),
			TEXT("One entry per (mask_value, event). Stencil tag values are REUSED across events, so a "
				 "value alone does not identify an event - key on mask_value together with the frame range. "
				 "m49-A2: first_frame/last_frame are now scoped to THIS EVENT. Before m49-A2 they were keyed "
				 "on the stencil VALUE alone, so two events sharing a reused value reported one merged range "
				 "for both - which broke the very disambiguation this note asks you to perform. -1 on both "
				 "means the event's tag was never counted non-zero in any measured mask. "
				 "Values are the 8-bit pixel values in target_mask/frame_NNNNN.png; 0 is background. Only "
				 "ANOMALY TARGETS appear - this is not a mask of every object in the scene."));

		TArray<TSharedPtr<FJsonValue>> Arr;
		for (const FTargetMaskMapEntry& E : Entries)
		{
			TSharedRef<FJsonObject> O = MakeShared<FJsonObject>();
			O->SetNumberField(TEXT("mask_value"), E.MaskValue);
			O->SetStringField(TEXT("event_id"), E.EventId);
			O->SetStringField(TEXT("target_name"), E.TargetName);
			O->SetStringField(TEXT("anomaly_type"), E.AnomalyType);
			O->SetNumberField(TEXT("first_frame"), E.FirstFrame);
			O->SetNumberField(TEXT("last_frame"), E.LastFrame);
			Arr.Add(MakeShared<FJsonValueObject>(O));
		}
		Root->SetArrayField(TEXT("entries"), Arr);

		FString Out;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
		FJsonSerializer::Serialize(Root, Writer);

		IFileManager::Get().MakeDirectory(*RunDir, true);
		return FFileHelper::SaveStringToFile(Out, *FPaths::Combine(RunDir, TEXT("mask_map.json")),
			FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}

	bool WriteSelectionProvenance(const FString& RunDir, const TArray<FProvenanceRecord>& Records)
	{
		TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
		Root->SetStringField(TEXT("type"), TEXT("selection_provenance"));

		TArray<TSharedPtr<FJsonValue>> Arr;
		for (const FProvenanceRecord& R : Records)
		{
			TSharedRef<FJsonObject> O = MakeShared<FJsonObject>();
			O->SetStringField(TEXT("anomaly_id"), R.AnomalyId);
			O->SetStringField(TEXT("target"), R.Target);
			O->SetNumberField(TEXT("anchor_index"), R.AnchorIndex);
			O->SetBoolField(TEXT("valid"), R.bValid);
			O->SetNumberField(TEXT("coverage_pct"), R.CoveragePct);
			O->SetNumberField(TEXT("occlusion_samples_passed"), R.OcclusionSamplesPassed);
			O->SetNumberField(TEXT("occlusion_samples_total"), R.OcclusionSamplesTotal);
			O->SetNumberField(TEXT("poll_distance"), R.PollDistance);
			Arr.Add(MakeShared<FJsonValueObject>(O));
		}
		Root->SetArrayField(TEXT("events"), Arr);

		FString Out;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
		FJsonSerializer::Serialize(Root, Writer);

		IFileManager::Get().MakeDirectory(*RunDir, true);
		return FFileHelper::SaveStringToFile(Out, *FPaths::Combine(RunDir, TEXT("selection_provenance.json")),
			FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}

	bool WriteRunManifest(const FString& RunDir, const FRunManifest& M)
	{
		TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
		Root->SetStringField(TEXT("type"), TEXT("run_manifest"));
		Root->SetNumberField(TEXT("schema_version"), SchemaVersion);
		Root->SetNumberField(TEXT("seed"), M.Seed);
		Root->SetNumberField(TEXT("settle_frames"), M.SettleFrames);
		Root->SetNumberField(TEXT("view_lag_frames"), M.ViewLagFrames);
		Root->SetNumberField(TEXT("pre_frames"), M.PreFrames);
		Root->SetNumberField(TEXT("positive_frames"), M.PositiveFrames);
		Root->SetNumberField(TEXT("post_frames"), M.PostFrames);
		Root->SetNumberField(TEXT("burst_count"), M.BurstCount);
		Root->SetNumberField(TEXT("frame_cap"), M.FrameCap);
		Root->SetStringField(TEXT("session_id"), M.SessionId);
		Root->SetArrayField(TEXT("viewport"), { LabelNum(M.ViewportW), LabelNum(M.ViewportH) });
		Root->SetStringField(TEXT("format"), M.Format);
		Root->SetNumberField(TEXT("start_frame"), (double)M.StartFrame);
		Root->SetStringField(TEXT("start_time_utc"), M.StartTimeUtc);
		Root->SetStringField(TEXT("mode"), M.Mode);
		Root->SetStringField(TEXT("target_anomaly"), M.TargetAnomaly);
		Root->SetStringField(TEXT("target_actor"), M.TargetActor);
		Root->SetNumberField(TEXT("target_fps"), M.TargetFps);
		Root->SetBoolField(TEXT("paced"), M.bPaced);

		FString Out;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
		FJsonSerializer::Serialize(Root, Writer);

		IFileManager::Get().MakeDirectory(*RunDir, true);
		return FFileHelper::SaveStringToFile(Out, *FPaths::Combine(RunDir, TEXT("run.json")),
			FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}

	bool WriteRunSummary(const FString& RunDir, int32 TotalFrames, int32 PositiveFrames, int32 BurstsDone,
		int32 ZeroMatchBursts, uint64 EndFrame,
		int32 TargetFps, double SustainedWallFps, double SpeedRatio, double StampedFps, double GameClockSpeedRatio, bool bPaced, bool bDeliveryMode,
		const FString& ContentClock, int32 NonManifestedEvents, const FString& CapturePath,
		const FRingTelemetry* Ring,
		int32 MaskProbeArms, int32 MaskResidualDiscards, int32 MaskNoPassDiscards,
		int32 VetoedEvents, int32 TranslucentVetoes, int32 TranslucencyUnknownVetoes,
		const FTickPinTelemetry* TickPin, int32 PatternExcludedTargets,
		const FReadbackLayoutTelemetry* ReadbackLayout,
		const ::FAnomalyCensusCounters* Census,
		const FTargetMaskTelemetry* TargetMask,
		const FShaderReadinessTelemetry* ShaderReadiness,
		int32 FramesExposureDip, const FObservabilityTelemetry* Observability,
		int32 TranslucentOnlyExcludedTargets, int32 UnmeasurableTargetsAdmitted,
		int32 TargetDrawnPixelsMeasured, int32 FramesDrawnUnexpected, int32 FramesExposureDipSuppressed,
		const FStuckMipTelemetry* StuckMip, const TSharedPtr<FJsonObject>& ChangeSummary,
		const FLabelSyncTelemetry* LabelSync, const ::FCameraClipRunAccum* CameraClip)
	{
		TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
		Root->SetStringField(TEXT("type"), TEXT("run_summary"));
		Root->SetNumberField(TEXT("schema_version"), SchemaVersion);
		Root->SetNumberField(TEXT("total_frames"), TotalFrames);
		Root->SetNumberField(TEXT("positive_frames"), PositiveFrames);
		Root->SetNumberField(TEXT("bursts_done"), BurstsDone);
		Root->SetNumberField(TEXT("zero_match_bursts"), ZeroMatchBursts);
		Root->SetNumberField(TEXT("end_frame"), (double)EndFrame);
		Root->SetNumberField(TEXT("target_fps"), TargetFps);
		Root->SetNumberField(TEXT("sustained_wall_fps"), SustainedWallFps);
		Root->SetNumberField(TEXT("speed_ratio"), SpeedRatio);
		Root->SetNumberField(TEXT("stamped_fps"), StampedFps);
		Root->SetNumberField(TEXT("game_clock_speed_ratio"), GameClockSpeedRatio);
		Root->SetBoolField(TEXT("paced"), bPaced);
		Root->SetBoolField(TEXT("delivery_mode"), bDeliveryMode);
		Root->SetStringField(TEXT("content_clock"), ContentClock);
		Root->SetNumberField(TEXT("non_manifested_events"), NonManifestedEvents);

		Root->SetStringField(TEXT("capture_path"), CapturePath);

		Root->SetNumberField(TEXT("mask_probe_arms"), MaskProbeArms);
		Root->SetNumberField(TEXT("mask_residual_discards"), MaskResidualDiscards);
		Root->SetNumberField(TEXT("mask_nopass_discards"), MaskNoPassDiscards);
		Root->SetNumberField(TEXT("vetoed_events"), VetoedEvents);
		Root->SetNumberField(TEXT("translucent_vetoes"), TranslucentVetoes);
		Root->SetNumberField(TEXT("translucency_unknown_vetoes"), TranslucencyUnknownVetoes);
		Root->SetNumberField(TEXT("pattern_excluded_targets"), PatternExcludedTargets);
		Root->SetNumberField(TEXT("translucent_only_excluded_targets"), TranslucentOnlyExcludedTargets);
		Root->SetNumberField(TEXT("unmeasurable_targets_admitted"), UnmeasurableTargetsAdmitted);
		if (StuckMip)
		{
			Root->SetNumberField(TEXT("stuck_mip_fires_applied"), StuckMip->FiresApplied);
			Root->SetNumberField(TEXT("stuck_mip_textures_held"), StuckMip->TexturesHeld);
			Root->SetNumberField(TEXT("stuck_mip_frames_held"), StuckMip->FramesHeld);
			Root->SetNumberField(TEXT("stuck_mip_refused_shared"), StuckMip->RefusedShared);
			Root->SetNumberField(TEXT("stuck_mip_refused_not_streamable"), StuckMip->RefusedNotStreamable);
			Root->SetNumberField(TEXT("stuck_mip_refused_virtual"), StuckMip->RefusedVirtual);
			Root->SetNumberField(TEXT("stuck_mip_refused_imperceptible"), StuckMip->RefusedImperceptible);
			Root->SetNumberField(TEXT("stuck_mip_refused_too_small_for_ratio"), StuckMip->RefusedTooSmallForRatio);
			Root->SetNumberField(TEXT("stuck_mip_refused_no_eligible_textures"), StuckMip->RefusedNoEligibleTextures);
			Root->SetNumberField(TEXT("stuck_mip_refused_not_restored"), StuckMip->RefusedNotRestored);
			Root->SetNumberField(TEXT("stuck_mip_refused_already_held"), StuckMip->RefusedAlreadyHeld);
			Root->SetNumberField(TEXT("stuck_mip_hold_timeouts"), StuckMip->HoldTimeouts);
			Root->SetNumberField(TEXT("stuck_mip_restore_timeout"), StuckMip->RestoreTimeouts);
			Root->SetNumberField(TEXT("stuck_mip_restore_frames_max"), StuckMip->RestoreFramesMax);
			Root->SetNumberField(TEXT("stuck_mip_textures_awaiting_restore"), StuckMip->TexturesAwaitingRestore);
			Root->SetNumberField(TEXT("stuck_mip_onset_preroll_max"), StuckMip->OnsetPrerollMax);
			Root->SetNumberField(TEXT("stuck_mip_revert_on_destroy"), StuckMip->RevertOnDestroy);
			Root->SetNumberField(TEXT("stuck_mip_unverified_at_teardown"), StuckMip->UnverifiedAtTeardown);
			if (StuckMip->FiresApplied > 0 || StuckMip->RenderRecordFrames > 0 || StuckMip->TrailsOpened > 0
				|| StuckMip->RestoreInheritedAtStart > 0 || StuckMip->RestoreCarriedAtEnd > 0)
			{
				Root->SetStringField(TEXT("stuck_mip_label_source"), StuckMip->LabelSource);
				Root->SetNumberField(TEXT("stuck_mip_render_record_frames"), StuckMip->RenderRecordFrames);
				Root->SetNumberField(TEXT("stuck_mip_render_held_frames"), StuckMip->RenderHeldFrames);
				Root->SetNumberField(TEXT("stuck_mip_render_unknown_frames"), StuckMip->RenderUnknownFrames);
				Root->SetNumberField(TEXT("stuck_mip_render_record_missing_frames"), StuckMip->RenderRecordMissingFrames);
				Root->SetNumberField(TEXT("stuck_mip_trailing_frames"), StuckMip->TrailingFrames);
				Root->SetNumberField(TEXT("stuck_mip_trailing_labelled_frames"), StuckMip->TrailingLabelledFrames);
				Root->SetNumberField(TEXT("stuck_mip_settle_tail_frames"), StuckMip->SettleTailFrames);
				Root->SetNumberField(TEXT("stuck_mip_settle_tail_setting"), StuckMip->SettleTailSetting);
				Root->SetNumberField(TEXT("stuck_mip_trails_opened"), StuckMip->TrailsOpened);
				Root->SetNumberField(TEXT("stuck_mip_trails_closed"), StuckMip->TrailsClosed);
				Root->SetNumberField(TEXT("stuck_mip_restore_unresolved"), StuckMip->RestoreUnresolved);
				Root->SetNumberField(TEXT("stuck_mip_restore_unresolved_at_end"), StuckMip->RestoreUnresolvedAtEnd);
				Root->SetNumberField(TEXT("stuck_mip_gt_mirror_disagree_frames"), StuckMip->GtMirrorDisagreeFrames);
				Root->SetNumberField(TEXT("stuck_mip_mask_deferred_dropped"), StuckMip->MaskDeferredDropped);
				Root->SetNumberField(TEXT("stuck_mip_refused_shared_world"), StuckMip->RefusedSharedWorld);
				Root->SetNumberField(TEXT("stuck_mip_refused_baseline_pending"), StuckMip->RefusedBaselinePending);
				Root->SetNumberField(TEXT("stuck_mip_restore_tracked_while_busy"), StuckMip->RestoreTrackedWhileBusy);
				Root->SetNumberField(TEXT("stuck_mip_purity_enumeration_ms_max"), StuckMip->PurityEnumerationMsMax);
				Root->SetNumberField(TEXT("stuck_mip_purity_inactive_level_users"), StuckMip->PurityInactiveLevelUsers);
				Root->SetNumberField(TEXT("stuck_mip_purity_unregistered_users"), StuckMip->PurityUnregisteredUsers);
				Root->SetNumberField(TEXT("stuck_mip_hold_contaminations"), StuckMip->HoldContaminations);
				Root->SetNumberField(TEXT("stuck_mip_contaminated_frames"), StuckMip->ContaminatedFrames);
				Root->SetNumberField(TEXT("stuck_mip_resource_replaced_frames"), StuckMip->ResourceReplacedFrames);
				Root->SetNumberField(TEXT("stuck_mip_trail_reopens"), StuckMip->TrailReopens);
				Root->SetNumberField(TEXT("stuck_mip_trail_missing_frames"), StuckMip->TrailMissingFrames);
				Root->SetNumberField(TEXT("stuck_mip_unwatched_after_close"), StuckMip->UnwatchedAfterClose);
				Root->SetNumberField(TEXT("stuck_mip_watch_refrozen_frames"), StuckMip->WatchRefrozenFrames);
				Root->SetNumberField(TEXT("stuck_mip_watch_missing_frames"), StuckMip->WatchMissingFrames);
				Root->SetNumberField(TEXT("stuck_mip_order_held_frames"), StuckMip->OrderHeldFrames);
				Root->SetNumberField(TEXT("stuck_mip_live_window_cut"), StuckMip->LiveWindowCut);
				Root->SetNumberField(TEXT("stuck_mip_restore_carried_at_end"), StuckMip->RestoreCarriedAtEnd);
				Root->SetNumberField(TEXT("stuck_mip_restore_inherited_at_start"), StuckMip->RestoreInheritedAtStart);
				Root->SetNumberField(TEXT("stuck_mip_observe_unresolved"), StuckMip->ObserveUnresolved);
				Root->SetNumberField(TEXT("stuck_mip_hold_monitor_scans"), StuckMip->HoldMonitorScans);
				Root->SetNumberField(TEXT("stuck_mip_hold_monitor_ms_mean"), StuckMip->HoldMonitorMsMean);
				Root->SetNumberField(TEXT("stuck_mip_hold_monitor_ms_p95"), StuckMip->HoldMonitorMsP95);
				Root->SetNumberField(TEXT("stuck_mip_hold_monitor_ms_max"), StuckMip->HoldMonitorMsMax);
				Root->SetNumberField(TEXT("stuck_mip_hold_monitor_components_walked_max"), StuckMip->HoldMonitorComponentsWalkedMax);
				Root->SetNumberField(TEXT("stuck_mip_hold_monitor_unregistered_watched_max"), StuckMip->HoldMonitorUnregisteredWatchedMax);
				Root->SetNumberField(TEXT("stuck_mip_hold_monitor_registration_rejudges"), StuckMip->HoldMonitorRegistrationRejudges);
				Root->SetNumberField(TEXT("stuck_mip_hold_monitor_dirty_requeued"), StuckMip->HoldMonitorDirtyRequeued);
				Root->SetNumberField(TEXT("stuck_mip_streamer_fences"), StuckMip->StreamerFences);
				Root->SetNumberField(TEXT("stuck_mip_streamer_fence_ms_max"), StuckMip->StreamerFenceMsMax);
				Root->SetNumberField(TEXT("stuck_mip_streamer_fence_incomplete"), StuckMip->StreamerFenceIncomplete);
				Root->SetNumberField(TEXT("stuck_mip_restore_held_for_streamer_plans"), StuckMip->RestoreHeldForStreamerPlans);
				Root->SetNumberField(TEXT("stuck_mip_trail_detaches"), StuckMip->TrailDetaches);
				Root->SetNumberField(TEXT("stuck_mip_trail_detaches_at_next_fire"), StuckMip->TrailDetachesAtNextFire);
				Root->SetNumberField(TEXT("stuck_mip_grace_frames"), StuckMip->GraceFrames);
				Root->SetNumberField(TEXT("stuck_mip_reopens_in_grace"), StuckMip->ReopensInGrace);
				Root->SetNumberField(TEXT("stuck_mip_reopens_after_detach"), StuckMip->ReopensAfterDetach);
				Root->SetNumberField(TEXT("stuck_mip_reopen_unrecoverable_frames"), StuckMip->ReopenUnrecoverableFrames);
				Root->SetNumberField(TEXT("stuck_mip_reopen_crosstalk_suppressed"), StuckMip->ReopenCrossTalkSuppressed);
				Root->SetNumberField(TEXT("stuck_mip_forced_authority_frames"), StuckMip->ForcedAuthorityFrames);
				Root->SetNumberField(TEXT("stuck_mip_late_receipt_after_force"), StuckMip->LateReceiptAfterForce);
				Root->SetNumberField(TEXT("stuck_mip_inherited_mask_records"), StuckMip->InheritedMaskRecords);
			}
		}
		Root->SetNumberField(TEXT("frames_exposure_dip"), FramesExposureDip);
		Root->SetNumberField(TEXT("frames_exposure_dip_suppressed"), FramesExposureDipSuppressed);
		Root->SetNumberField(TEXT("target_drawn_pixels_measured"), TargetDrawnPixelsMeasured);
		Root->SetNumberField(TEXT("frames_drawn_unexpected"), FramesDrawnUnexpected);

		if (Observability)
		{
			Root->SetNumberField(TEXT("observable_frames"), Observability->ObservableFrames);
			Root->SetNumberField(TEXT("frames_condition_lost"), Observability->FramesConditionLost);
			Root->SetNumberField(TEXT("observable_min_pixels"), Observability->ObservableMinPixels);
		}

		if (Census)
		{
			Root->SetNumberField(TEXT("census_frames"), Census->CensusFrames);
			Root->SetNumberField(TEXT("census_cycles"), Census->Cycles);
			Root->SetNumberField(TEXT("census_candidates"), Census->Candidates);
			Root->SetNumberField(TEXT("census_zero"), Census->Zero);
			Root->SetNumberField(TEXT("census_below_floor"), Census->BelowFloor);
			Root->SetNumberField(TEXT("census_above_ceiling"), Census->AboveCeiling);
			Root->SetNumberField(TEXT("census_excluded_translucent"), Census->ExcludedTranslucent);
			Root->SetNumberField(TEXT("census_fires_fallback_all"), Census->FiresFallbackAll);
			Root->SetNumberField(TEXT("census_fires_partial_fallback"), Census->FiresPartialFallback);
			Root->SetNumberField(TEXT("census_fires_unseen_candidates"), Census->FiresUnseenCandidates);
			Root->SetNumberField(TEXT("census_host_pp_customdepth_readers"), Census->HostPpCustomDepthReaders);
			{
				TArray<TSharedPtr<FJsonValue>> ReaderNames;
				for (const FString& Name : Census->HostPpCustomDepthReaderNames)
				{
					ReaderNames.Add(MakeShared<FJsonValueString>(Name));
				}
				Root->SetArrayField(TEXT("census_host_pp_customdepth_reader_names"), ReaderNames);
			}
			Root->SetNumberField(TEXT("census_unmeasurable_nanite"), Census->UnmeasurableNanite);
			Root->SetNumberField(TEXT("census_unmeasurable_tag_failed"), Census->UnmeasurableTagFailed);
			Root->SetNumberField(TEXT("census_unmeasurable_hidden"), Census->UnmeasurableHidden);
			Root->SetNumberField(TEXT("census_unmeasurable_not_yet_measured"), Census->NotYetMeasured);
		}

		if (ShaderReadiness)
		{
			Root->SetNumberField(TEXT("shader_prewarm_ms"), ShaderReadiness->PrewarmMs);
			Root->SetNumberField(TEXT("shader_prewarm_incomplete"), ShaderReadiness->PrewarmIncomplete);
			Root->SetNumberField(TEXT("frames_shaders_pending"), ShaderReadiness->FramesShadersPending);
		}

		if (TargetMask)
		{
			Root->SetNumberField(TEXT("target_mask_frames_measured"), TargetMask->Measured);
			Root->SetNumberField(TEXT("target_mask_frames_hidden_blank"), TargetMask->HiddenBlank);
			Root->SetNumberField(TEXT("target_mask_frames_unavailable"), TargetMask->Unavailable);
		}

		if (Ring)
		{
			Root->SetNumberField(TEXT("key_ring_published"), Ring->Published);
			Root->SetNumberField(TEXT("key_ring_consumed"), Ring->Consumed);
			Root->SetNumberField(TEXT("key_ring_missed"), Ring->Missed);
			Root->SetNumberField(TEXT("key_ring_wrapped"), Ring->Wrapped);
			Root->SetNumberField(TEXT("key_ring_corrupted"), Ring->Corrupted);
			Root->SetNumberField(TEXT("wanted_matches"), Ring->WantedMatches);
		}

		if (TickPin)
		{
			Root->SetBoolField(TEXT("tickpin_compiled"), TickPin->bCompiled);
			Root->SetBoolField(TEXT("tickpin_applied"), TickPin->bApplied);
			Root->SetNumberField(TEXT("tickpin_saved"), TickPin->Saved);
			Root->SetNumberField(TEXT("tickpin_reasserts"), TickPin->Reasserts);
			Root->SetNumberField(TEXT("capture_game_ticks"), TickPin->GameTicks);
			Root->SetNumberField(TEXT("ticks_per_captured_frame"),
				TotalFrames > 0 ? ((double)TickPin->GameTicks / (double)TotalFrames) : 0.0);
		}

		if (ReadbackLayout)
		{
			TSharedRef<FJsonObject> L = MakeShared<FJsonObject>();
			L->SetNumberField(TEXT("source_extent_w"), ReadbackLayout->SourceExtentX);
			L->SetNumberField(TEXT("source_extent_h"), ReadbackLayout->SourceExtentY);
			L->SetNumberField(TEXT("rect_min_x"), ReadbackLayout->RectMinX);
			L->SetNumberField(TEXT("rect_min_y"), ReadbackLayout->RectMinY);
			L->SetNumberField(TEXT("rect_max_x"), ReadbackLayout->RectMaxX);
			L->SetNumberField(TEXT("rect_max_y"), ReadbackLayout->RectMaxY);
			L->SetNumberField(TEXT("picture_w"), ReadbackLayout->W);
			L->SetNumberField(TEXT("picture_h"), ReadbackLayout->H);
			L->SetNumberField(TEXT("buffer_height"), ReadbackLayout->BufferHeight);
			L->SetNumberField(TEXT("row_pitch_in_pixels"), ReadbackLayout->RowPitchInPixels);
			L->SetNumberField(TEXT("pixel_format"), ReadbackLayout->Format);
			Root->SetObjectField(TEXT("readback_layout"), L);
		}

		if (LabelSync)
		{
			Root->SetStringField(TEXT("label_aa_method"), LabelSync->AaMethod);
			Root->SetNumberField(TEXT("label_aa_method_cvar"), LabelSync->AaMethodValue);
			Root->SetBoolField(TEXT("label_temporal_aa"), LabelSync->bTemporalAa);
			Root->SetNumberField(TEXT("label_transition_on_frames"), LabelSync->OnFrames);
			Root->SetNumberField(TEXT("label_transition_off_frames"), LabelSync->OffFrames);
			Root->SetNumberField(TEXT("label_transition_hide_frames"), LabelSync->HideFrames);
			Root->SetNumberField(TEXT("label_transition_on_frames_cvar"), LabelSync->OnFramesConfigured);
			Root->SetNumberField(TEXT("label_transition_off_frames_cvar"), LabelSync->OffFramesConfigured);
			Root->SetNumberField(TEXT("label_transition_hide_frames_cvar"), LabelSync->HideFramesConfigured);
			Root->SetNumberField(TEXT("label_transition_entries"), LabelSync->TransitionEntries);
			Root->SetNumberField(TEXT("label_transition_frames"), LabelSync->TransitionFrames);
			Root->SetNumberField(TEXT("label_entries_suppressed"), LabelSync->SuppressedEntries);
			Root->SetNumberField(TEXT("label_transition_out_of_order"), LabelSync->OutOfOrderFrames);
			Root->SetNumberField(TEXT("mask_tag_recycles"), LabelSync->MaskTagRecycles);
			Root->SetNumberField(TEXT("mask_tag_peak_live"), LabelSync->MaskTagPeakLive);
			Root->SetNumberField(TEXT("mask_tag_exhausted"), LabelSync->MaskTagExhausted);
		}

		if (CameraClip && CameraClip->FramesEvaluated > 0)
		{
			const double Frames = (double)CameraClip->FramesEvaluated;
			Root->SetStringField(TEXT("camera_clipping_label_rule"), TEXT("view_slab_render_bounds_v1"));
			Root->SetNumberField(TEXT("camera_clipping_baseline_near"), CameraClip->BaselineNear);
			Root->SetNumberField(TEXT("camera_clipping_anomalous_near"), CameraClip->AnomalousNear);
			Root->SetNumberField(TEXT("camera_clipping_frames_evaluated"), CameraClip->FramesEvaluated);
			Root->SetNumberField(TEXT("camera_clipping_slab_positive_frames"), CameraClip->SlabPositiveFrames);
			Root->SetNumberField(TEXT("camera_clipping_sphere_proxy_positive_frames"), CameraClip->SphereProxyPositiveFrames);
			Root->SetNumberField(TEXT("camera_clipping_slab_only_frames"), CameraClip->SlabOnlyFrames);
			Root->SetNumberField(TEXT("camera_clipping_sphere_proxy_only_frames"), CameraClip->ProxyOnlyFrames);
			Root->SetNumberField(TEXT("camera_clipping_eye_inside_box_frames"), CameraClip->EyeInsideBoxFrames);
			Root->SetNumberField(TEXT("camera_clipping_near_overridden_frames"), CameraClip->NearOverriddenFrames);
			Root->SetNumberField(TEXT("camera_clipping_box_fallbacks"), CameraClip->BoxFallbacks);
			Root->SetNumberField(TEXT("camera_clipping_fx_excluded_max"), CameraClip->FxExcludedMax);
			Root->SetNumberField(TEXT("camera_clipping_primitives_enumerated_mean"), CameraClip->EnumeratedTotal / Frames);
			Root->SetNumberField(TEXT("camera_clipping_candidates_mean"), CameraClip->CandidatesTotal / Frames);
			Root->SetNumberField(TEXT("camera_clipping_instances_tested_total"), (double)CameraClip->InstancesTestedTotal);
			Root->SetNumberField(TEXT("camera_clipping_eval_ms_total"), CameraClip->MicrosTotal / 1000.0);
			Root->SetNumberField(TEXT("camera_clipping_eval_us_mean"), CameraClip->MicrosTotal / Frames);
			Root->SetNumberField(TEXT("camera_clipping_eval_us_max"), CameraClip->MicrosMax);
			TArray<TPair<FString, int32>> Hits;
			for (const TPair<FString, int32>& Hit : CameraClip->FirstHits)
			{
				Hits.Add(Hit);
			}
			Hits.Sort([](const TPair<FString, int32>& A, const TPair<FString, int32>& B)
			{
				return A.Value != B.Value ? A.Value > B.Value : A.Key < B.Key;
			});
			TArray<TSharedPtr<FJsonValue>> HitValues;
			for (int32 i = 0; i < Hits.Num() && i < 16; ++i)
			{
				HitValues.Add(MakeShared<FJsonValueString>(FString::Printf(TEXT("%s=%d"), *Hits[i].Key, Hits[i].Value)));
			}
			Root->SetArrayField(TEXT("camera_clipping_first_hits"), HitValues);
		}

		FString Out;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
		FJsonSerializer::Serialize(Root, Writer);

		if (ChangeSummary.IsValid())
		{
			for (const auto& Field : ChangeSummary->Values) { Root->SetField(Field.Key, Field.Value); }
			Out.Reset();
			const auto ChangeWriter = TJsonWriterFactory<>::Create(&Out);
			FJsonSerializer::Serialize(Root, ChangeWriter);
		}
		return FFileHelper::SaveStringToFile(Out, *FPaths::Combine(RunDir, TEXT("run_summary.json")),
			FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}

	bool WriteSessionAnnotation(const FString& RunDir, const FSessionAnnotation& A)
	{
		TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
		Root->SetNumberField(TEXT("label_schema"), 2);
		Root->SetStringField(TEXT("session_id"), A.SessionId);

		{
			TSharedRef<FJsonObject> V = MakeShared<FJsonObject>();
			V->SetStringField(TEXT("path"), A.Video.VideoPath);
			V->SetStringField(TEXT("frames_dir"), A.Video.FramesDir);
			V->SetArrayField(TEXT("resolution"), { LabelNum(A.Video.ResolutionW), LabelNum(A.Video.ResolutionH) });
			V->SetNumberField(TEXT("fps"), A.Video.Fps);
			V->SetNumberField(TEXT("target_fps"), A.Video.TargetFps);
			V->SetNumberField(TEXT("total_frames"), A.Video.TotalFrames);
			Root->SetObjectField(TEXT("video"), V);
		}

		TArray<TSharedPtr<FJsonValue>> Arr;
		for (const FSessionEvent& E : A.Events)
		{
			TSharedRef<FJsonObject> O = MakeShared<FJsonObject>();
			O->SetStringField(TEXT("anomaly_type"), E.AnomalyType);
			O->SetStringField(TEXT("anomaly_subtype"), E.AnomalySubtype);

			{
				TSharedRef<FJsonObject> AF = MakeShared<FJsonObject>();
				int32 Start = 0, End = 0;
				const int32 Count = E.FrameIndices.Num();
				if (Count > 0)
				{
					Start = E.FrameIndices[0];
					End = E.FrameIndices.Last();
				}
				AF->SetNumberField(TEXT("start_frame"), Start);
				AF->SetNumberField(TEXT("end_frame"), End);
				AF->SetNumberField(TEXT("frame_count"), Count);
				AF->SetNumberField(TEXT("span_frame_count"), Count > 0 ? (End - Start + 1) : 0);
				TArray<TSharedPtr<FJsonValue>> Idx;
				for (int32 F : E.FrameIndices) { Idx.Add(LabelNum(F)); }
				AF->SetArrayField(TEXT("frame_indices"), Idx);
				O->SetObjectField(TEXT("affected_frames"), AF);
			}

			{
				TSharedRef<FJsonObject> IF = MakeShared<FJsonObject>();
				int32 Start = 0, End = 0;
				const int32 Count = E.InjectedFrameIndices.Num();
				if (Count > 0)
				{
					Start = E.InjectedFrameIndices[0];
					End = E.InjectedFrameIndices.Last();
				}
				IF->SetNumberField(TEXT("start_frame"), Start);
				IF->SetNumberField(TEXT("end_frame"), End);
				IF->SetNumberField(TEXT("frame_count"), Count);
				IF->SetNumberField(TEXT("span_frame_count"), Count > 0 ? (End - Start + 1) : 0);
				TArray<TSharedPtr<FJsonValue>> Idx;
				for (int32 F : E.InjectedFrameIndices) { Idx.Add(LabelNum(F)); }
				IF->SetArrayField(TEXT("frame_indices"), Idx);
				O->SetObjectField(TEXT("injected_frames"), IF);
			}

			O->SetStringField(TEXT("bbox_source"), E.BboxSource);
			O->SetNumberField(TEXT("observable_frame_count"), E.ObservableFrameCount);
			O->SetNumberField(TEXT("unmeasured_frame_count"), E.UnmeasuredFrameCount);
			O->SetBoolField(TEXT("observability_measured"), E.bObservabilityMeasured);
			O->SetBoolField(TEXT("manifested"), E.bManifested);
			O->SetNumberField(TEXT("coverage_ratio"), E.CoverageRatio);
			O->SetNumberField(TEXT("coverage_pct"), E.CoveragePct);

			{
				TSharedRef<FJsonObject> Obj = MakeShared<FJsonObject>();
				Obj->SetNumberField(TEXT("count"), E.Nodes.Num());
				Obj->SetNumberField(TEXT("primary_index"), E.PrimaryIndex);
				TArray<TSharedPtr<FJsonValue>> NodeArr;
				for (const FSessionNode& N : E.Nodes)
				{
					TSharedRef<FJsonObject> NO = MakeShared<FJsonObject>();
					NO->SetStringField(TEXT("name"), N.Name);
					NO->SetStringField(TEXT("path"), N.Path);
					NO->SetArrayField(TEXT("global_position"), LabelVec3(N.GlobalPosition.X, N.GlobalPosition.Y, N.GlobalPosition.Z));
					NO->SetStringField(TEXT("asset_name"), N.AssetName);
					NO->SetStringField(TEXT("component_class"), N.ComponentClass);
					{
						TSharedRef<FJsonObject> B = MakeShared<FJsonObject>();
						B->SetArrayField(TEXT("origin"), LabelVec3(N.BoundsOrigin.X, N.BoundsOrigin.Y, N.BoundsOrigin.Z));
						B->SetArrayField(TEXT("extent"), LabelVec3(N.BoundsExtent.X, N.BoundsExtent.Y, N.BoundsExtent.Z));
						NO->SetObjectField(TEXT("bounds"), B);
					}
					NodeArr.Add(MakeShared<FJsonValueObject>(NO));
				}
				Obj->SetArrayField(TEXT("nodes"), NodeArr);
				O->SetObjectField(TEXT("affected_objects"), Obj);
			}

			{
				TSharedRef<FJsonObject> Cam = MakeShared<FJsonObject>();
				Cam->SetStringField(TEXT("path"), E.CamPath);
				Cam->SetArrayField(TEXT("global_position"), LabelVec3(E.CamPosition.X, E.CamPosition.Y, E.CamPosition.Z));
				Cam->SetNumberField(TEXT("near"), E.CamNear);
				Cam->SetNumberField(TEXT("far"), E.CamFar);
				Cam->SetArrayField(TEXT("rotation"), LabelVec3(E.CamRotation.Pitch, E.CamRotation.Yaw, E.CamRotation.Roll));
				Cam->SetNumberField(TEXT("fov_deg"), E.CamFovDeg);
				Cam->SetNumberField(TEXT("aspect"), E.CamAspect);
				O->SetObjectField(TEXT("camera"), Cam);
			}

			{
				TSharedRef<FJsonObject> Eng = MakeShared<FJsonObject>();
				Eng->SetNumberField(TEXT("ticks_msec"), (double)E.TicksMsec);
				Eng->SetStringField(TEXT("name"), E.EngineName);
				Eng->SetStringField(TEXT("version"), E.EngineVersion);
				Eng->SetStringField(TEXT("project"), E.EngineProject);
				O->SetObjectField(TEXT("engine"), Eng);
			}

			{
				TSharedRef<FJsonObject> Mask = MakeShared<FJsonObject>();
				Mask->SetBoolField(TEXT("provided"), E.bMaskProvided);
				O->SetObjectField(TEXT("mask"), Mask);
				TSharedRef<FJsonObject> Depth = MakeShared<FJsonObject>();
				Depth->SetBoolField(TEXT("provided"), false);
				O->SetObjectField(TEXT("depth"), Depth);
			}

			Arr.Add(MakeShared<FJsonValueObject>(O));
		}
		Root->SetArrayField(TEXT("anomalies"), Arr);

		FString Out;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
		FJsonSerializer::Serialize(Root, Writer);

		IFileManager::Get().MakeDirectory(*RunDir, true);
		return FFileHelper::SaveStringToFile(Out, *FPaths::Combine(RunDir, TEXT("annotation.json")),
			FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}
}


static FAutoConsoleCommandWithWorldAndArgs GCaptureShotCmd(
	TEXT("IAI.Capture.Shot"),
	TEXT("Capture ONE labeled frame (PNG default) + append a JSONL label record. ")
	TEXT("Usage: IAI.Capture.Shot [outDir] [png|jpeg] [outputHeight]  (default outDir: ")
	TEXT("<ProjectSaved>/AnomalyCaptures/manual). outputHeight 0 or omitted = NATIVE; a value below the frame's ")
	TEXT("own height downscales the WRITTEN image only (width derived from the frame's aspect, both snapped even); ")
	TEXT("a value at or above it is NOT an upscale and yields native. This one-shot takes its height from the ")
	TEXT("argument alone and does NOT consult the run-level precedence chain."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
		[](const TArray<FString>& Args, UWorld* World)
		{
			const FString Dir = (Args.Num() > 0 && !Args[0].IsEmpty())
				? Args[0]
				: FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("AnomalyCaptures"), TEXT("manual"));

			AnomalyPreview::EImageFormat Format = AnomalyPreview::EImageFormat::PNG;
			if (Args.Num() > 1 && (Args[1].Equals(TEXT("jpeg"), ESearchCase::IgnoreCase) || Args[1].Equals(TEXT("jpg"), ESearchCase::IgnoreCase)))
			{
				Format = AnomalyPreview::EImageFormat::JPEG;
			}

			const int32 TargetOutputHeight = (Args.Num() > 2 && !Args[2].IsEmpty()) ? FCString::Atoi(*Args[2]) : 0;

			FAnomalyViewInfo View;
			AnomalyViewport::GetActiveViewInfo(World, View);

			const TCHAR* Ext = (Format == AnomalyPreview::EImageFormat::PNG) ? TEXT("png") : TEXT("jpg");
			const FString ShotName = FString::Printf(TEXT("frame_%llu.%s"), GFrameCounter, Ext);

			FString ImagePath, SidecarPath;
			int32 NumLabels = 0;
			int32 NativeW = 0, NativeH = 0, WrittenW = 0, WrittenH = 0;
			bool bResampled = false;
			if (AnomalyLabel::CaptureLabeledShot(World, Dir, Format, View, ShotName, 0, FPlatformTime::Seconds(),
				TargetOutputHeight, ImagePath, SidecarPath, NumLabels, NativeW, NativeH, WrittenW, WrittenH, bResampled))
			{
				UE_LOG(LogAnomalyCapture, Log,
					TEXT("Capture.Shot: wrote '%s' - native %dx%d -> output %dx%d, resample %s (%d valid bbox ")
					TEXT("label(s)); record appended to '%s'."),
					*ImagePath, NativeW, NativeH, WrittenW, WrittenH,
					bResampled ? TEXT("YES") : TEXT("no - native"), NumLabels, *SidecarPath);
			}
			else
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture.Shot: failed (run inside a Game/PIE world with a live viewport)."));
			}
		}));

#endif
