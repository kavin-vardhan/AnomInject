// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "AnomalyPreviewCapture.h"

#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "UnrealClient.h"            // FViewport::ReadPixels / GetSizeXY
#include "Modules/ModuleManager.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

namespace AnomalyPreview
{
	bool CaptureGameViewportRaw(UWorld* World, TArray<FColor>& OutPixels, int32& OutWidth, int32& OutHeight)
	{
		OutPixels.Reset();
		OutWidth = 0;
		OutHeight = 0;

		UGameViewportClient* GameViewport = World ? World->GetGameViewport() : nullptr;
		if (!GameViewport || !GameViewport->Viewport)
		{
			return false;
		}
		FViewport* Viewport = GameViewport->Viewport;

		const FIntPoint Size = Viewport->GetSizeXY();
		if (Size.X <= 0 || Size.Y <= 0)
		{
			return false;
		}

		// Game-view-only, synchronous readback (default flags + full rect). Native resolution; no disk, no downscale.
		if (!Viewport->ReadPixels(OutPixels) || OutPixels.Num() == 0)
		{
			return false;
		}

		// Force opaque: the game backbuffer's alpha is not a meaningful opacity (typically ~0). PNG preserves
		// alpha, so without this the saved frame renders fully TRANSPARENT ("empty") in any alpha-honoring
		// viewer — even though the RGB pixels are correct. (JPEG has no alpha, so the dashboard path never
		// showed this.) A displayed-frame screenshot is opaque by definition. Idempotent.
		for (FColor& Px : OutPixels)
		{
			Px.A = 255;
		}

		OutWidth = Size.X;
		OutHeight = Size.Y;
		return true;
	}

	bool CaptureGameViewportEncoded(UWorld* World, EImageFormat Format, TArray<uint8>& OutBytes,
		int32& OutWidth, int32& OutHeight, int32 JpegQuality)
	{
		OutBytes.Reset();

		TArray<FColor> Pixels;
		if (!CaptureGameViewportRaw(World, Pixels, OutWidth, OutHeight))
		{
			return false;
		}

		const ::EImageFormat WrapperFormat = (Format == EImageFormat::PNG) ? ::EImageFormat::PNG : ::EImageFormat::JPEG;
		IImageWrapperModule& Module = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
		TSharedPtr<IImageWrapper> Wrapper = Module.CreateImageWrapper(WrapperFormat);
		if (!Wrapper.IsValid())
		{
			return false;
		}
		if (!Wrapper->SetRaw(Pixels.GetData(), (int64)Pixels.Num() * sizeof(FColor), OutWidth, OutHeight, ERGBFormat::BGRA, 8))
		{
			return false;
		}

		// PNG ignores the quality arg (lossless); JPEG uses it. 0 => the wrapper's default for the format.
		const int32 Quality = (Format == EImageFormat::PNG) ? 0 : JpegQuality;
		const TArray64<uint8>& Compressed = Wrapper->GetCompressed(Quality);
		if (Compressed.Num() == 0)
		{
			return false;
		}

		OutBytes.SetNumUninitialized(Compressed.Num());
		FMemory::Memcpy(OutBytes.GetData(), Compressed.GetData(), Compressed.Num());
		return true;
	}

	bool CaptureGameViewportJpeg(UWorld* World, TArray<uint8>& OutJpeg, int32& OutWidth, int32& OutHeight, int32 Quality)
	{
		return CaptureGameViewportEncoded(World, EImageFormat::JPEG, OutJpeg, OutWidth, OutHeight, Quality);
	}
}
