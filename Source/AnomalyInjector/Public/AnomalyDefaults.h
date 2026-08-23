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

	ANOMALYINJECTOR_API const TCHAR* ExcludedTargetPatternsKey();
	ANOMALYINJECTOR_API const TArray<FString>& GetExcludedTargetPatterns();
	ANOMALYINJECTOR_API FString DescribeExcludedTargetPatterns();
	ANOMALYINJECTOR_API void SetExcludedTargetPatternsOverride(const TArray<FString>& Patterns);
	ANOMALYINJECTOR_API void ClearExcludedTargetPatternsOverride();
	ANOMALYINJECTOR_API FString ExcludedTargetPatternsSource();

	inline constexpr float LodPoppingMaxDistanceCompiled = 200.0f;
	inline constexpr float MaxDistanceMin = 0.0f;
	inline constexpr float MaxDistanceMax = 1000000.0f;

	ANOMALYINJECTOR_API const TCHAR* LodPoppingMaxDistanceKey();
	ANOMALYINJECTOR_API float GetLodPoppingMaxDistanceCm();
	ANOMALYINJECTOR_API FString DescribeLodPoppingMaxDistance();
	ANOMALYINJECTOR_API bool SetLodPoppingMaxDistanceOverride(float Cm);
	ANOMALYINJECTOR_API void ClearLodPoppingMaxDistanceOverride();

	inline constexpr float LodPoppingMinCoverageCompiled = 7.0f;
	inline constexpr float MinCoverageMin = 0.0f;
	inline constexpr float MinCoverageMax = 100.0f;

	ANOMALYINJECTOR_API const TCHAR* LodPoppingMinCoverageKey();
	ANOMALYINJECTOR_API float GetLodPoppingMinCoveragePct();
	ANOMALYINJECTOR_API FString DescribeLodPoppingMinCoverage();
	ANOMALYINJECTOR_API bool SetLodPoppingMinCoverageOverride(float Pct);
	ANOMALYINJECTOR_API void ClearLodPoppingMinCoverageOverride();

	inline constexpr float CameraClippingTriggerRadiusCompiled = 200.0f;
	inline constexpr float TriggerRadiusMin = 1.0f;
	inline constexpr float TriggerRadiusMax = 1000000.0f;

	ANOMALYINJECTOR_API const TCHAR* CameraClippingTriggerRadiusKey();
	ANOMALYINJECTOR_API float GetCameraClippingTriggerRadiusCm();
	ANOMALYINJECTOR_API FString DescribeCameraClippingTriggerRadius();
	ANOMALYINJECTOR_API bool SetCameraClippingTriggerRadiusOverride(float Cm);
	ANOMALYINJECTOR_API void ClearCameraClippingTriggerRadiusOverride();
}
