// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "AnomalyLabelWriter.h"

#include "AnomalyControlServerLog.h"

#if ANOMALY_CONTROL_SERVER

#include "AnomalyPreviewCapture.h"
#include "AnomalyViewport.h"               // ProjectActorBoundsToScreenRect / GetActiveViewInfo / FAnomalyViewInfo
#include "AnomalyAutoInjectorSubsystem.h"  // FAutoLiveFireInfo / GetLiveFires

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "CoreGlobals.h"                   // GFrameCounter (the shared frame-index stamp)
#include "HAL/IConsoleManager.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	// File-unique names: UE unity builds concatenate .cpp files into one TU, so an anonymous-namespace
	// helper named identically to one in a sibling file (ControlSnapshot.cpp's Num/Vec3) is an ODR clash.
	TSharedPtr<FJsonValue> LabelNum(double V) { return MakeShared<FJsonValueNumber>(V); }

	TArray<TSharedPtr<FJsonValue>> LabelVec3(double X, double Y, double Z)
	{
		return { LabelNum(X), LabelNum(Y), LabelNum(Z) };
	}

	/**
	 * Build one JSONL label record (a single condensed line) for a captured frame. anomaly_present is the
	 * TEMPORAL label (any live fire this frame); each anomalies[] entry carries the SPATIAL label — the pixel
	 * bbox [x,y,w,h] (clamped to the frame) plus the unclamped normalized rect and bbox_valid. A degenerate
	 * spatial label (off-screen / behind camera / actor gone) sets bbox_valid=false but the frame + temporal
	 * label are still recorded (never drop the frame for a bad box).
	 */
	FString BuildFrameLabelRecord(UWorld* World, const TArray<FAutoLiveFireInfo>& Fires,
		const FAnomalyViewInfo& View, int32 W, int32 H, uint64 FrameIndex, const FString& ImageName, int32& OutNumLabels)
	{
		OutNumLabels = 0;

		TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
		Root->SetNumberField(TEXT("frame_index"), (double)FrameIndex);
		Root->SetNumberField(TEXT("t"), World ? World->GetTimeSeconds() : 0.0);
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

			// Unclamped normalized rect [minX, minY, maxX, maxY] (resolution-independent; may exceed [0,1]).
			O->SetArrayField(TEXT("bbox_norm"), { LabelNum(Min.X), LabelNum(Min.Y), LabelNum(Max.X), LabelNum(Max.Y) });

			// Pixel bbox [x, y, w, h], clamped to the frame [0,W]x[0,H].
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

		// VISIBLE POSITIVE (the detection-relevant positive) = an anomaly is applied in game-state this frame
		// AND at least one fire projects to an on-screen box. anomaly_present (game-state truth) stays separate:
		// under camera motion a fired actor can leave the viewport DURING its hold -> anomaly_present=true with
		// every bbox_valid=false (anomaly active but off-screen). Those are kept (legitimate hard negatives), not
		// dropped; consumers filter on visible_positive for the detection set. NOTE: the in-frustum-but-OCCLUDED
		// sub-case (on-screen yet hidden behind geometry) is NOT caught by bounds projection — that is the
		// deferred GetLastRenderTimeOnScreen refinement (G22).
		Root->SetBoolField(TEXT("visible_positive"), (Fires.Num() > 0) && (OutNumLabels > 0));

		// View used for the projection (lets a consumer re-project / verify).
		TSharedRef<FJsonObject> V = MakeShared<FJsonObject>();
		V->SetArrayField(TEXT("origin"), LabelVec3(View.Origin.X, View.Origin.Y, View.Origin.Z));
		V->SetArrayField(TEXT("rot"), LabelVec3(View.Rotation.Pitch, View.Rotation.Yaw, View.Rotation.Roll));
		V->SetNumberField(TEXT("fovDeg"), View.HorizontalFOVDeg);
		V->SetNumberField(TEXT("aspect"), View.AspectRatio);
		V->SetBoolField(TEXT("valid"), View.bValid);
		Root->SetObjectField(TEXT("view"), V);

		// Condensed (single-line) — one JSON object per line = JSONL.
		FString Out;
		const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Out);
		FJsonSerializer::Serialize(Root, Writer);
		return Out;
	}
}

namespace AnomalyLabel
{
	bool CaptureLabeledShot(UWorld* World, const FString& OutputDir, AnomalyPreview::EImageFormat Format,
		const FAnomalyViewInfo& ProjectionView,
		FString& OutImagePath, FString& OutSidecarPath, int32& OutNumLabels, bool bLog)
	{
		OutNumLabels = 0;
		if (!World)
		{
			return false;
		}

		// (1) Same-tick ground truth: snapshot the live fires on this game-thread call (L3/Q6 — image and
		//     label reflect one frame). The PROJECTION view is supplied by the caller — it must match the
		//     captured pixels (the render trails the game thread, so a moving burst passes the view from L
		//     frames ago, not the current view; S3). An invalid view => bbox_valid=false, frame still written.
		TArray<FAutoLiveFireInfo> Fires;
		if (UAnomalyAutoInjectorSubsystem* Auto = World->GetSubsystem<UAnomalyAutoInjectorSubsystem>())
		{
			Fires = Auto->GetLiveFires();
		}
		const FAnomalyViewInfo& View = ProjectionView;

		// (2) Synchronous game-viewport capture (native resolution, no downscale).
		TArray<uint8> ImageBytes;
		int32 W = 0, H = 0;
		if (!AnomalyPreview::CaptureGameViewportEncoded(World, Format, ImageBytes, W, H))
		{
			if (bLog)
			{
				UE_LOG(LogAnomalyServer, Warning, TEXT("Capture: game-viewport capture failed (no game viewport?)."));
			}
			return false;
		}

		// (3) Stamp + filenames (image and record share the GFrameCounter).
		const uint64 FrameIndex = GFrameCounter;
		const TCHAR* Ext = (Format == AnomalyPreview::EImageFormat::PNG) ? TEXT("png") : TEXT("jpg");
		const FString ImageName = FString::Printf(TEXT("frame_%llu.%s"), FrameIndex, Ext);
		const FString ImagePath = FPaths::Combine(OutputDir, ImageName);
		const FString SidecarPath = FPaths::Combine(OutputDir, TEXT("labels.jsonl"));

		// (4) Ensure dir, write the image, append the label record line.
		IFileManager::Get().MakeDirectory(*OutputDir, /*Tree=*/true);
		if (!FFileHelper::SaveArrayToFile(ImageBytes, *ImagePath))
		{
			if (bLog)
			{
				UE_LOG(LogAnomalyServer, Warning, TEXT("Capture: failed to write image '%s'."), *ImagePath);
			}
			return false;
		}

		const FString Record = BuildFrameLabelRecord(World, Fires, View, W, H, FrameIndex, ImageName, OutNumLabels);
		FFileHelper::SaveStringToFile(Record + TEXT("\n"), *SidecarPath,
			FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM, &IFileManager::Get(), FILEWRITE_Append);

		OutImagePath = ImagePath;
		OutSidecarPath = SidecarPath;
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
		Root->SetArrayField(TEXT("viewport"), { LabelNum(M.ViewportW), LabelNum(M.ViewportH) });
		Root->SetStringField(TEXT("format"), M.Format);
		Root->SetNumberField(TEXT("start_frame"), (double)M.StartFrame);
		Root->SetStringField(TEXT("start_time_utc"), M.StartTimeUtc);

		FString Out;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
		FJsonSerializer::Serialize(Root, Writer);

		IFileManager::Get().MakeDirectory(*RunDir, /*Tree=*/true);
		return FFileHelper::SaveStringToFile(Out, *FPaths::Combine(RunDir, TEXT("run.json")),
			FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}

	bool WriteRunSummary(const FString& RunDir, int32 TotalFrames, int32 PositiveFrames, int32 BurstsDone,
		int32 ZeroMatchBursts, uint64 EndFrame)
	{
		TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
		Root->SetStringField(TEXT("type"), TEXT("run_summary"));
		Root->SetNumberField(TEXT("schema_version"), SchemaVersion);
		Root->SetNumberField(TEXT("total_frames"), TotalFrames);
		Root->SetNumberField(TEXT("positive_frames"), PositiveFrames);
		Root->SetNumberField(TEXT("bursts_done"), BurstsDone);
		Root->SetNumberField(TEXT("zero_match_bursts"), ZeroMatchBursts);
		Root->SetNumberField(TEXT("end_frame"), (double)EndFrame);

		FString Out;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
		FJsonSerializer::Serialize(Root, Writer);

		return FFileHelper::SaveStringToFile(Out, *FPaths::Combine(RunDir, TEXT("run_summary.json")),
			FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}
}

// ----------------------------------------------------------------------------------------------------
// IAI.Capture.Shot — Stage-1 single-shot console surface (the full IAI.Capture.* set lands with the
// burst subsystem in Stage 2). Module-scoped, mirroring the injector/auto console pattern.
// ----------------------------------------------------------------------------------------------------

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

			// Manual single shot: project with the CURRENT view (intended for a still pose; the L-frame
			// delayed-view correction is the burst path's concern, where the camera moves).
			FAnomalyViewInfo View;
			AnomalyViewport::GetActiveViewInfo(World, View);

			FString ImagePath, SidecarPath;
			int32 NumLabels = 0;
			if (AnomalyLabel::CaptureLabeledShot(World, Dir, Format, View, ImagePath, SidecarPath, NumLabels))
			{
				UE_LOG(LogAnomalyServer, Log, TEXT("Capture.Shot: wrote '%s' (%d valid bbox label(s)); record appended to '%s'."),
					*ImagePath, NumLabels, *SidecarPath);
			}
			else
			{
				UE_LOG(LogAnomalyServer, Warning, TEXT("Capture.Shot: failed (run inside a Game/PIE world with a live viewport)."));
			}
		}));

#endif // ANOMALY_CONTROL_SERVER
