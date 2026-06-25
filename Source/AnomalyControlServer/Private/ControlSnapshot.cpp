#include "ControlSnapshot.h"
#include "ControlProtocol.h"

#include "Engine/World.h"
#include "Engine/GameViewportClient.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

#include "AnomalyInjectorSubsystem.h"
#include "AnomalyAutoInjectorSubsystem.h"
#include "AnomalySelectorSubsystem.h"
#include "AnomalyViewport.h"
#include "AnomalyCatalogTypes.h"
#include "AnomalyCaptureSubsystem.h"

namespace
{
	TSharedPtr<FJsonValue> Num(double V) { return MakeShared<FJsonValueNumber>(V); }
	TSharedPtr<FJsonValue> Str(const FString& V) { return MakeShared<FJsonValueString>(V); }

	TArray<TSharedPtr<FJsonValue>> Vec3(double X, double Y, double Z)
	{
		return { Num(X), Num(Y), Num(Z) };
	}

	FString Serialize(const TSharedRef<FJsonObject>& Root)
	{
		FString Out;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
		FJsonSerializer::Serialize(Root, Writer);
		return Out;
	}

	void GetViewportPx(UWorld* World, int32& OutX, int32& OutY)
	{
		OutX = 0;
		OutY = 0;
		if (UGameViewportClient* GV = World ? World->GetGameViewport() : nullptr)
		{
			FVector2D Size = FVector2D::ZeroVector;
			GV->GetViewportSize(Size);
			OutX = (int32)Size.X;
			OutY = (int32)Size.Y;
		}
	}

	TSharedRef<FJsonObject> ArgSpecToJson(const FAnomalyArgSpec& Arg)
	{
		TSharedRef<FJsonObject> O = MakeShared<FJsonObject>();
		O->SetStringField(TEXT("name"), Arg.Name);
		O->SetStringField(TEXT("type"), ToString(Arg.Type));
		O->SetBoolField(TEXT("required"), Arg.bRequired);
		O->SetStringField(TEXT("default"), Arg.Default);
		if (Arg.bHasMin) { O->SetNumberField(TEXT("min"), Arg.Min); }
		if (Arg.bHasMax) { O->SetNumberField(TEXT("max"), Arg.Max); }
		if (Arg.Options.Num() > 0)
		{
			TArray<TSharedPtr<FJsonValue>> Opts;
			for (const FString& Opt : Arg.Options) { Opts.Add(Str(Opt)); }
			O->SetArrayField(TEXT("options"), Opts);
		}
		return O;
	}
}

namespace ControlSnapshot
{
	FString BuildSnapshotJson(UWorld* World, uint32 Epoch)
	{
		TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
		Root->SetNumberField(TEXT("v"), ControlProtocol::Version);
		Root->SetStringField(TEXT("type"), TEXT("snapshot"));
		Root->SetNumberField(TEXT("t"), World ? World->GetTimeSeconds() : 0.0);
		Root->SetNumberField(TEXT("epoch"), (double)Epoch);

		UAnomalyInjectorSubsystem* Inj = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
		UAnomalyAutoInjectorSubsystem* Auto = World ? World->GetSubsystem<UAnomalyAutoInjectorSubsystem>() : nullptr;
		UAnomalySelectorSubsystem* Sel = World ? World->GetSubsystem<UAnomalySelectorSubsystem>() : nullptr;

		{
			FAnomalyViewInfo View;
			const bool bValid = AnomalyViewport::GetActiveViewInfo(World, View);
			int32 VX = 0, VY = 0;
			GetViewportPx(World, VX, VY);

			TSharedRef<FJsonObject> V = MakeShared<FJsonObject>();
			V->SetArrayField(TEXT("origin"), Vec3(View.Origin.X, View.Origin.Y, View.Origin.Z));
			V->SetArrayField(TEXT("rot"), Vec3(View.Rotation.Pitch, View.Rotation.Yaw, View.Rotation.Roll));
			V->SetNumberField(TEXT("fovDeg"), View.HorizontalFOVDeg);
			V->SetNumberField(TEXT("aspect"), View.AspectRatio);
			V->SetArrayField(TEXT("viewportPx"), { Num(VX), Num(VY) });
			V->SetBoolField(TEXT("valid"), bValid);
			Root->SetObjectField(TEXT("view"), V);
		}

		{
			TArray<TSharedPtr<FJsonValue>> Arr;
			if (World)
			{
				for (const FRenderableActorInfo& Info : AnomalyViewport::GetVisibleRenderableActorInfos(World))
				{
					TSharedRef<FJsonObject> O = MakeShared<FJsonObject>();
					O->SetStringField(TEXT("name"), Info.ActorName);
					O->SetStringField(TEXT("class"), Info.ClassName);
					O->SetStringField(TEXT("comp"), Info.ComponentType);
					O->SetNumberField(TEXT("dist"), Info.Distance);
					O->SetArrayField(TEXT("rect"), {
						Num(Info.ScreenMin.X), Num(Info.ScreenMin.Y), Num(Info.ScreenMax.X), Num(Info.ScreenMax.Y) });
					O->SetBoolField(TEXT("rectValid"), Info.bRectValid);
					Arr.Add(MakeShared<FJsonValueObject>(O));
				}
			}
			Root->SetArrayField(TEXT("visible"), Arr);
		}

		{
			TArray<TSharedPtr<FJsonValue>> Arr;
			if (Inj)
			{
				TMap<FName, EAnomalyScope> ScopeById;
				for (const FAnomalyCatalogEntry& E : Inj->GetAnomalyCatalog()) { ScopeById.Add(E.Id, E.Scope); }

				TMap<FName, FString> AutoTargetById;
				if (Auto)
				{
					for (const FAutoLiveFireInfo& F : Auto->GetLiveFires()) { AutoTargetById.Add(F.Id, F.Target); }
				}

				for (const FActiveAnomalyInfo& A : Inj->GetActiveAnomalies())
				{
					TSharedRef<FJsonObject> O = MakeShared<FJsonObject>();
					O->SetStringField(TEXT("id"), A.Id.ToString());

					TArray<TSharedPtr<FJsonValue>> ArgsArr;
					for (const FString& S : A.Args) { ArgsArr.Add(Str(S)); }
					O->SetArrayField(TEXT("args"), ArgsArr);

					const bool bAuto = AutoTargetById.Contains(A.Id);
					O->SetStringField(TEXT("source"), bAuto ? TEXT("auto") : TEXT("manual"));

					FString Target;
					if (bAuto)
					{
						Target = AutoTargetById[A.Id];
					}
					else if (const EAnomalyScope* Scope = ScopeById.Find(A.Id))
					{
						if (*Scope != EAnomalyScope::Global && A.Args.Num() > 0)
						{
							Target = A.Args[0].StartsWith(TEXT("=")) ? A.Args[0].Mid(1) : A.Args[0];
						}
					}
					O->SetStringField(TEXT("target"), Target);
					O->SetNumberField(TEXT("tActive"), A.SecondsActive);
					Arr.Add(MakeShared<FJsonValueObject>(O));
				}
			}
			Root->SetArrayField(TEXT("active"), Arr);
		}

		{
			TSharedRef<FJsonObject> O = MakeShared<FJsonObject>();
			if (Auto)
			{
				O->SetBoolField(TEXT("enabled"), Auto->IsEnabled());
				O->SetBoolField(TEXT("running"), Auto->IsRunning());
				O->SetNumberField(TEXT("seed"), Auto->GetSeed());

				float MinV = 0.0f, MaxV = 0.0f;
				Auto->GetIntervalRange(MinV, MaxV);
				O->SetNumberField(TEXT("intervalMin"), MinV);
				O->SetNumberField(TEXT("intervalMax"), MaxV);
				Auto->GetHoldRange(MinV, MaxV);
				O->SetNumberField(TEXT("holdMin"), MinV);
				O->SetNumberField(TEXT("holdMax"), MaxV);
				O->SetNumberField(TEXT("maxConcurrent"), Auto->GetMaxConcurrent());
				O->SetBoolField(TEXT("persist"), Auto->GetPersist());

				TSharedRef<FJsonObject> Pool = MakeShared<FJsonObject>();
				if (Inj)
				{
					for (const FAnomalyCatalogEntry& E : Inj->GetAnomalyCatalog())
					{
						if (E.Scope == EAnomalyScope::Object)
						{
							Pool->SetBoolField(E.Id.ToString(), Auto->IsAnomalyEnabled(E.Id));
						}
					}
				}
				O->SetObjectField(TEXT("pool"), Pool);

				TArray<TSharedPtr<FJsonValue>> LiveFires;
				for (const FAutoLiveFireInfo& F : Auto->GetLiveFires())
				{
					TSharedRef<FJsonObject> L = MakeShared<FJsonObject>();
					L->SetStringField(TEXT("id"), F.Id.ToString());
					L->SetStringField(TEXT("target"), F.Target);
					L->SetNumberField(TEXT("secondsRemaining"), F.SecondsRemaining);
					LiveFires.Add(MakeShared<FJsonValueObject>(L));
				}
				O->SetArrayField(TEXT("liveFires"), LiveFires);
			}
			Root->SetObjectField(TEXT("auto"), O);
		}

		{
			TSharedRef<FJsonObject> O = MakeShared<FJsonObject>();
			O->SetBoolField(TEXT("viewportScoping"), Inj ? Inj->IsViewportScopingEnabled() : false);
			O->SetBoolField(TEXT("selectorHud"), Sel ? Sel->IsUIEnabled() : false);
			O->SetBoolField(TEXT("autoHud"), Auto ? Auto->IsEnabled() : false);
			const double Dt = World ? World->GetDeltaSeconds() : 0.0;
			O->SetNumberField(TEXT("fps"), Dt > 0.0 ? 1.0 / Dt : 0.0);
			O->SetNumberField(TEXT("activeCount"), Inj ? Inj->GetActiveAnomalyCount() : 0);
			O->SetNumberField(TEXT("pollRadius"), AnomalyViewport::GetPollRadius());
			O->SetNumberField(TEXT("minScreenCoverage"), AnomalyViewport::GetMinScreenCoveragePct());
			Root->SetObjectField(TEXT("session"), O);
		}

		{
			TSharedRef<FJsonObject> O = MakeShared<FJsonObject>();
			bool bRunning = false;
			int32 Frames = 0, Seed = 0;
			FString RunDir;
			if (UAnomalyCaptureSubsystem* Cap = World ? World->GetSubsystem<UAnomalyCaptureSubsystem>() : nullptr)
			{
				Cap->GetStatus(bRunning, Frames, RunDir, Seed);
			}
			O->SetBoolField(TEXT("running"), bRunning);
			O->SetNumberField(TEXT("framesWritten"), Frames);
			O->SetStringField(TEXT("runDir"), RunDir);
			O->SetNumberField(TEXT("seed"), Seed);
			Root->SetObjectField(TEXT("capture"), O);
		}

		return Serialize(Root);
	}

	FString BuildCatalogJson(UWorld* World)
	{
		TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
		Root->SetNumberField(TEXT("v"), ControlProtocol::Version);
		Root->SetStringField(TEXT("type"), TEXT("catalog"));

		TArray<TSharedPtr<FJsonValue>> Arr;
		if (UAnomalyInjectorSubsystem* Inj = World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr)
		{
			for (const FAnomalyCatalogEntry& E : Inj->GetAnomalyCatalog())
			{
				TSharedRef<FJsonObject> O = MakeShared<FJsonObject>();
				O->SetStringField(TEXT("id"), E.Id.ToString());
				O->SetStringField(TEXT("description"), E.Description);
				O->SetStringField(TEXT("usage"), E.Usage);
				O->SetStringField(TEXT("scope"), ToString(E.Scope));

				TArray<TSharedPtr<FJsonValue>> ArgsArr;
				for (const FAnomalyArgSpec& Arg : E.Args)
				{
					ArgsArr.Add(MakeShared<FJsonValueObject>(ArgSpecToJson(Arg)));
				}
				O->SetArrayField(TEXT("args"), ArgsArr);
				Arr.Add(MakeShared<FJsonValueObject>(O));
			}
		}
		Root->SetArrayField(TEXT("entries"), Arr);
		return Serialize(Root);
	}
}
