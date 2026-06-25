#include "AnomalyPreviewCapture.h"

#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "UnrealClient.h"
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

		if (!Viewport->ReadPixels(OutPixels) || OutPixels.Num() == 0)
		{
			return false;
		}

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
