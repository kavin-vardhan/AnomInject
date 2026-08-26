#include "CoreMinimal.h"

#if ANOMALY_CAPTURE

#include "AnomalyCaptureLog.h"

#include "Camera/CameraComponent.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "UnrealClient.h"

namespace AnomalyLetterbox
{
	static TWeakObjectPtr<UCameraComponent> SavedComponent;
	static bool bHasSaved = false;
	static bool bSavedConstrain = false;
	static float SavedAspect = 0.0f;

	static UCameraComponent* ResolveViewTargetCamera(UWorld* World, FString& OutWhere)
	{
		OutWhere.Reset();

		if (!World || !(World->IsGameWorld()))
		{
			OutWhere = TEXT("no game/PIE world");
			return nullptr;
		}

		APlayerController* PC = World->GetFirstPlayerController();
		if (!PC)
		{
			OutWhere = TEXT("no local PlayerController");
			return nullptr;
		}

		AActor* ViewTarget = PC->GetViewTarget();
		if (!ViewTarget)
		{
			OutWhere = TEXT("PlayerController has no view target");
			return nullptr;
		}

		UCameraComponent* Cam = ViewTarget->FindComponentByClass<UCameraComponent>();
		if (!Cam)
		{
			OutWhere = FString::Printf(TEXT("view target '%s' has no UCameraComponent"), *ViewTarget->GetName());
			return nullptr;
		}

		OutWhere = FString::Printf(TEXT("%s / %s"), *ViewTarget->GetName(), *Cam->GetName());
		return Cam;
	}

	static void ReportPredictedRect(UWorld* World, float Aspect)
	{
		UGameViewportClient* VPClient = World ? World->GetGameViewport() : nullptr;
		FViewport* Viewport = VPClient ? VPClient->Viewport : nullptr;
		if (!Viewport)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(bench): LETTERBOX cannot echo the predicted view rect - no FViewport. The lever may ")
				TEXT("still have applied; read the READBACK-LAYOUT line from the capture instead."));
			return;
		}

		const FIntPoint Size = Viewport->GetSizeXY();
		const FIntRect Full(0, 0, Size.X, Size.Y);
		const FIntRect Constrained = Viewport->CalculateViewExtents(Aspect, Full);

		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(bench): LETTERBOX predicted view rect at aspect %.4f - viewport %dx%d, constrained ")
			TEXT("(%d,%d)-(%d,%d) = %dx%d, minY=%d. minY MUST be > 0 for this lever to be exercising anything; ")
			TEXT("a minY of 0 means the lever is a NO-OP and any clean result from it is an artifact of ")
			TEXT("insulation, not evidence."),
			Aspect, Size.X, Size.Y,
			Constrained.Min.X, Constrained.Min.Y, Constrained.Max.X, Constrained.Max.Y,
			Constrained.Width(), Constrained.Height(), Constrained.Min.Y);
	}

	static void Apply(const TArray<FString>& Args, UWorld* World)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Usage: IAI.Bench.Letterbox <aspect|off>   e.g. IAI.Bench.Letterbox 2.39"));
			return;
		}

		FString Where;
		UCameraComponent* Cam = ResolveViewTargetCamera(World, Where);

		if (Args[0].Equals(TEXT("off"), ESearchCase::IgnoreCase))
		{
			if (!bHasSaved)
			{
				UE_LOG(LogAnomalyCapture, Log,
					TEXT("Capture(bench): LETTERBOX off - nothing to revert, the lever was never applied."));
				return;
			}

			UCameraComponent* Target = SavedComponent.Get();
			if (!Target)
			{
				UE_LOG(LogAnomalyCapture, Warning,
					TEXT("Capture(bench): LETTERBOX off - the camera component it was applied to is GONE (PIE ")
					TEXT("restarted or the view target changed). Nothing was reverted; the saved state is being ")
					TEXT("dropped. Restart PIE to be certain the camera is stock."));
				bHasSaved = false;
				SavedComponent.Reset();
				return;
			}

			Target->bConstrainAspectRatio = bSavedConstrain;
			Target->AspectRatio = SavedAspect;
			UE_LOG(LogAnomalyCapture, Log,
				TEXT("Capture(bench): LETTERBOX REVERTED on %s - bConstrainAspectRatio=%d aspect=%.4f (restored)."),
				*Target->GetName(), bSavedConstrain ? 1 : 0, SavedAspect);

			bHasSaved = false;
			SavedComponent.Reset();
			return;
		}

		if (!Cam)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(bench): LETTERBOX REFUSED - %s. This lever only acts on a live PIE/Game view ")
				TEXT("target's camera component; it changes nothing on disk and nothing is saved."), *Where);
			return;
		}

		const float Aspect = FCString::Atof(*Args[0]);
		if (!(Aspect > 0.01f) || Aspect > 100.0f)
		{
			UE_LOG(LogAnomalyCapture, Warning,
				TEXT("Capture(bench): LETTERBOX REFUSED - aspect '%s' parsed to %.4f, which is out of the ")
				TEXT("accepted range (0.01, 100]. REFUSED, not clamped."), *Args[0], Aspect);
			return;
		}

		if (!bHasSaved || SavedComponent.Get() != Cam)
		{
			SavedComponent = Cam;
			bSavedConstrain = Cam->bConstrainAspectRatio;
			SavedAspect = Cam->AspectRatio;
			bHasSaved = true;
		}

		Cam->bConstrainAspectRatio = true;
		Cam->AspectRatio = Aspect;

		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(bench): LETTERBOX APPLIED on %s - bConstrainAspectRatio 0->1, aspect %.4f->%.4f. ")
			TEXT("RUNTIME ONLY: this writes to the live component instance in this PIE session, touches no asset ")
			TEXT("and saves nothing. Revert with IAI.Bench.Letterbox off."),
			*Where, SavedAspect, Aspect);

		ReportPredictedRect(World, Aspect);
	}
}

static FAutoConsoleCommandWithWorldAndArgs GAnomalyLetterboxCmd(
	TEXT("IAI.Bench.Letterbox"),
	TEXT("BENCH LEVER, not a product setting - constrain the live view target camera's aspect ratio so the ")
	TEXT("rendered picture is LETTERBOXED inside the render target, giving a view rect with a NON-ZERO Min.Y. ")
	TEXT("This is the standing control for view-rect-origin bugs in the capture readback path. Runtime only: it ")
	TEXT("edits the PIE component instance, never an asset, and saves nothing. Compiled out of Shipping with the ")
	TEXT("rest of AnomalyCapture. Usage: IAI.Bench.Letterbox <aspect|off>   e.g. IAI.Bench.Letterbox 2.39"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&AnomalyLetterbox::Apply));

#endif
