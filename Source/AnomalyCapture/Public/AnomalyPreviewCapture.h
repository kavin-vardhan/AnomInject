#pragma once

#include "CoreMinimal.h"

class UWorld;

namespace AnomalyPreview
{
	enum class EImageFormat : uint8 { PNG, JPEG };

	ANOMALYCAPTURE_API bool CaptureGameViewportRaw(UWorld* World, TArray<FColor>& OutPixels, int32& OutWidth, int32& OutHeight);

	ANOMALYCAPTURE_API bool EncodePixels(EImageFormat Format, const TArray<FColor>& Pixels, int32 Width, int32 Height,
		TArray<uint8>& OutBytes, int32 JpegQuality = 90);

	ANOMALYCAPTURE_API bool CaptureGameViewportEncoded(UWorld* World, EImageFormat Format, TArray<uint8>& OutBytes,
		int32& OutWidth, int32& OutHeight, int32 JpegQuality = 90);
}
