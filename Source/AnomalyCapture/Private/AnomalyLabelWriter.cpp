#include "AnomalyLabelWriter.h"

#include "AnomalyCaptureLog.h"

#if ANOMALY_CAPTURE

#include "AnomalyPreviewCapture.h"
#include "AnomalyViewport.h"
#include "AnomalyAutoInjectorSubsystem.h"

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

	FString BuildFrameLabelRecord(const TArray<FAutoLiveFireInfo>& Fires,
		const FAnomalyViewInfo& View, int32 W, int32 H, uint64 FrameIndex, int32 SessionIndex, double TimeSeconds,
		double WallSeconds, const FString& ImageName, int32& OutNumLabels)
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
		Root->SetBoolField(TEXT("anomaly_present"), Fires.Num() > 0);

		TArray<TSharedPtr<FJsonValue>> Anoms;
		for (const FAutoLiveFireInfo& F : Fires)
		{
			TSharedRef<FJsonObject> O = MakeShared<FJsonObject>();
			O->SetStringField(TEXT("id"), F.Id.ToString());
			O->SetStringField(TEXT("target_name"), F.Target);
			O->SetNumberField(TEXT("seconds_remaining"), F.SecondsRemaining);
			O->SetNumberField(TEXT("start_frame"), (double)F.StartFrame);

			FVector2D Min(FVector2D::ZeroVector);
			FVector2D Max(FVector2D::ZeroVector);
			bool bValid = false;
			if (const AActor* Actor = F.TargetActor.Get())
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

			if (bValid)
			{
				++OutNumLabels;
			}
			Anoms.Add(MakeShared<FJsonValueObject>(O));
		}
		Root->SetArrayField(TEXT("anomalies"), Anoms);

		Root->SetBoolField(TEXT("visible_positive"), (Fires.Num() > 0) && (OutNumLabels > 0));

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
	void ConvertTightToBGRA(EPixelFormat Format, int32 BytesPerPixel, const TArray<uint8>& RawBytes,
		int32 W, int32 H, TArray<FColor>& OutPixels)
	{
		OutPixels.SetNumUninitialized(W * H);
		const uint8* Base = RawBytes.GetData();

		for (int32 i = 0; i < W * H; ++i)
		{
			const uint8* P = Base + (int64)i * BytesPerPixel;
			FColor& Out = OutPixels[i];

			switch (Format)
			{
			case PF_B8G8R8A8:
				Out = FColor(P[2], P[1], P[0], 255);
				break;
			case PF_R8G8B8A8:
				Out = FColor(P[0], P[1], P[2], 255);
				break;
			case PF_A2B10G10R10:
			{
				const uint32 V = *reinterpret_cast<const uint32*>(P);
				const uint8 R = (uint8)(((V >> 0) & 0x3FF) >> 2);
				const uint8 G = (uint8)(((V >> 10) & 0x3FF) >> 2);
				const uint8 B = (uint8)(((V >> 20) & 0x3FF) >> 2);
				Out = FColor(R, G, B, 255);
				break;
			}
			case PF_FloatRGBA:
			{
				const FFloat16* H16 = reinterpret_cast<const FFloat16*>(P);
				auto ToByte = [](const FFloat16& In) -> uint8
				{
					return (uint8)FMath::Clamp(FMath::RoundToInt(In.GetFloat() * 255.0f), 0, 255);
				};
				Out = FColor(ToByte(H16[0]), ToByte(H16[1]), ToByte(H16[2]), 255);
				break;
			}
			default:
				Out = FColor(0, 0, 0, 255);
				break;
			}
		}
	}
	bool CaptureLabeledShot(UWorld* World, const FString& OutputDir, AnomalyPreview::EImageFormat Format,
		const FAnomalyViewInfo& ProjectionView, const FString& ImageRelName, int32 SessionIndex,
		double WallSeconds, FString& OutImagePath, FString& OutSidecarPath, int32& OutNumLabels, bool bLog,
		bool bWriteLabels)
	{
		OutNumLabels = 0;
		if (!World)
		{
			return false;
		}

		TArray<FAutoLiveFireInfo> Fires;
		if (UAnomalyAutoInjectorSubsystem* Auto = World->GetSubsystem<UAnomalyAutoInjectorSubsystem>())
		{
			Fires = Auto->GetLiveFires();
		}

		TArray<uint8> ImageBytes;
		int32 W = 0, H = 0;
		if (!AnomalyPreview::CaptureGameViewportEncoded(World, Format, ImageBytes, W, H))
		{
			if (bLog)
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture: game-viewport capture failed (no game viewport?)."));
			}
			return false;
		}

		const FString Record = BuildFrameLabelRecord(Fires, ProjectionView, W, H, GFrameCounter, SessionIndex,
			World->GetTimeSeconds(), WallSeconds, ImageRelName, OutNumLabels);

		return AppendRecordAndImage(OutputDir, ImageBytes, Record, ImageRelName, OutImagePath, OutSidecarPath, bLog, bWriteLabels);
	}

	FString BuildLabelRecordForSnapshot(const FCaptureSnapshot& Snapshot, int32 Width, int32 Height,
		const FString& ImageName, int32& OutNumLabels)
	{
		return BuildFrameLabelRecord(Snapshot.Fires, Snapshot.View, Width, Height,
			Snapshot.FrameCounter, Snapshot.SessionIndex, Snapshot.TimeSeconds, Snapshot.WallSeconds, ImageName, OutNumLabels);
	}

	bool EncodeAndWriteFrame(const FString& OutputDir, AnomalyPreview::EImageFormat OutFormat,
		const TArray<uint8>& RawBytes, EPixelFormat SrcFormat, int32 BytesPerPixel, int32 Width, int32 Height,
		const FString& ImageRelPath, const FString& Record, FCriticalSection& JsonlLock, bool bWriteLabels)
	{
		if (Width <= 0 || Height <= 0 || BytesPerPixel <= 0 || RawBytes.Num() < (int64)Width * Height * BytesPerPixel)
		{
			return false;
		}

		TArray<FColor> Pixels;
		ConvertTightToBGRA(SrcFormat, BytesPerPixel, RawBytes, Width, Height, Pixels);

		TArray<uint8> ImageBytes;
		if (!AnomalyPreview::EncodePixels(OutFormat, Pixels, Width, Height, ImageBytes))
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
		int32 TargetFps, double SustainedWallFps, double SpeedRatio, double StampedFps, bool bPaced, bool bDeliveryMode,
		const FString& ContentClock)
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
		Root->SetBoolField(TEXT("paced"), bPaced);
		Root->SetBoolField(TEXT("delivery_mode"), bDeliveryMode);
		Root->SetStringField(TEXT("content_clock"), ContentClock);

		FString Out;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
		FJsonSerializer::Serialize(Root, Writer);

		return FFileHelper::SaveStringToFile(Out, *FPaths::Combine(RunDir, TEXT("run_summary.json")),
			FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}

	bool WriteSessionAnnotation(const FString& RunDir, const FSessionAnnotation& A)
	{
		TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
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
				int32 Start = 0, End = 0, Count = 0;
				if (E.FrameIndices.Num() > 0)
				{
					Start = E.FrameIndices[0];
					End = E.FrameIndices.Last();
					Count = End - Start + 1;
				}
				AF->SetNumberField(TEXT("start_frame"), Start);
				AF->SetNumberField(TEXT("end_frame"), End);
				AF->SetNumberField(TEXT("frame_count"), Count);
				TArray<TSharedPtr<FJsonValue>> Idx;
				for (int32 F : E.FrameIndices) { Idx.Add(LabelNum(F)); }
				AF->SetArrayField(TEXT("frame_indices"), Idx);
				O->SetObjectField(TEXT("affected_frames"), AF);
			}
			O->SetNumberField(TEXT("coverage_ratio"), E.CoverageRatio);

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
				Mask->SetBoolField(TEXT("provided"), false);
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
	TEXT("Usage: IAI.Capture.Shot [outDir] [png|jpeg]  (default outDir: <ProjectSaved>/AnomalyCaptures/manual)"),
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

			FAnomalyViewInfo View;
			AnomalyViewport::GetActiveViewInfo(World, View);

			const TCHAR* Ext = (Format == AnomalyPreview::EImageFormat::PNG) ? TEXT("png") : TEXT("jpg");
			const FString ShotName = FString::Printf(TEXT("frame_%llu.%s"), GFrameCounter, Ext);

			FString ImagePath, SidecarPath;
			int32 NumLabels = 0;
			if (AnomalyLabel::CaptureLabeledShot(World, Dir, Format, View, ShotName, 0, FPlatformTime::Seconds(), ImagePath, SidecarPath, NumLabels))
			{
				UE_LOG(LogAnomalyCapture, Log, TEXT("Capture.Shot: wrote '%s' (%d valid bbox label(s)); record appended to '%s'."),
					*ImagePath, NumLabels, *SidecarPath);
			}
			else
			{
				UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture.Shot: failed (run inside a Game/PIE world with a live viewport)."));
			}
		}));

#endif
