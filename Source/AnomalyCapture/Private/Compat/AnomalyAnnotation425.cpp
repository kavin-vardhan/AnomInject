#include "AnomalyAnnotation425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "Dom/JsonObject.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
namespace AnomalyAnnotation425
{
    TSharedPtr<FJsonValue> LabelNum(double V) { return MakeShared<FJsonValueNumber>(V); }
    TArray<TSharedPtr<FJsonValue>> LabelVec3(double X,double Y,double Z) { return {LabelNum(X),LabelNum(Y),LabelNum(Z)}; }
	bool WriteSessionAnnotation(const FString& RunDir, const FSessionAnnotation& A)
	{
		TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
		Root->SetNumberField(TEXT("label_schema"), 2);
		Root->SetNumberField(TEXT("label_schema_minor"), 1);
		Root->SetStringField(TEXT("session_id"), A.SessionId);
		TSet<int32> TransitionFrames;
		for (const TPair<FString, FSessionTransition>& KV : A.Transitions)
		{
			for (int32 Frame : KV.Value.Frames) { TransitionFrames.Add(Frame); }
		}
		Root->SetNumberField(TEXT("transition_frame_count"), TransitionFrames.Num());

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
			O->SetStringField(TEXT("event_id"), E.EventId);
			TArray<TSharedPtr<FJsonValue>> TransitionIndices;
			TSharedRef<FJsonObject> TransitionReasons = MakeShared<FJsonObject>();
			if (const FSessionTransition* T = A.Transitions.Find(E.EventId))
			{
				TArray<int32> Sorted = T->Frames;
				Sorted.Sort();
				for (int32 Frame : Sorted) { TransitionIndices.Add(LabelNum(Frame)); }
				for (const TPair<FString, int32>& KV : T->Reasons) { TransitionReasons->SetNumberField(KV.Key, KV.Value); }
			}
			O->SetArrayField(TEXT("transition_frames"), TransitionIndices);
			O->SetObjectField(TEXT("transition_reasons"), TransitionReasons);
			O->SetStringField(TEXT("anomaly_type"), E.AnomalyType);
			O->SetStringField(TEXT("anomaly_subtype"), E.AnomalySubtype);
			if (!E.StuckMipRoute.IsEmpty()) { O->SetStringField(TEXT("stuck_mip_route"), E.StuckMipRoute); }

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
#endif
