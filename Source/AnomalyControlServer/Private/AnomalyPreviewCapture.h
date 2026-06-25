#pragma once

#include "CoreMinimal.h"

class UWorld;

namespace AnomalyPreview
{
	enum class EImageFormat : uint8 { PNG, JPEG };

	bool CaptureGameViewportRaw(UWorld* World, TArray<FColor>& OutPixels, int32& OutWidth, int32& OutHeight);

	bool CaptureGameViewportEncoded(UWorld* World, EImageFormat Format, TArray<uint8>& OutBytes,
		int32& OutWidth, int32& OutHeight, int32 JpegQuality = 90);

	bool CaptureGameViewportJpeg(UWorld* World, TArray<uint8>& OutJpeg, int32& OutWidth, int32& OutHeight, int32 Quality = 60);
}
