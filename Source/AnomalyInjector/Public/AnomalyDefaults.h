#pragma once

#include "CoreMinimal.h"

namespace AnomalyDefaults
{
	inline constexpr int32 BlinkingHalfPeriodCompiled = 3;
	inline constexpr int32 LodPoppingHalfPeriodCompiled = 8;
	inline constexpr int32 HalfPeriodMin = 1;
	inline constexpr int32 HalfPeriodMax = 600;

	ANOMALYINJECTOR_API FString DescribeBlinkingHalfPeriod();
	ANOMALYINJECTOR_API FString DescribeLodPoppingHalfPeriod();

	ANOMALYINJECTOR_API int32 GetHalfPeriodFrames(const TCHAR* IniKey, int32 CompiledDefault,
		int32 MinFrames, int32 MaxFrames, const TCHAR* AnomalyName);

	ANOMALYINJECTOR_API FString Describe(const TCHAR* IniKey, int32 CompiledDefault,
		int32 MinFrames, int32 MaxFrames, const TCHAR* AnomalyName);

	ANOMALYINJECTOR_API const TCHAR* BlinkingHalfPeriodKey();
	ANOMALYINJECTOR_API const TCHAR* LodPoppingHalfPeriodKey();
	ANOMALYINJECTOR_API const TCHAR* SectionName();

	ANOMALYINJECTOR_API bool SetConsoleOverride(const TCHAR* IniKey, int32 Frames,
		int32 MinFrames, int32 MaxFrames, const TCHAR* AnomalyName);
	ANOMALYINJECTOR_API void ClearConsoleOverride(const TCHAR* IniKey);
}
