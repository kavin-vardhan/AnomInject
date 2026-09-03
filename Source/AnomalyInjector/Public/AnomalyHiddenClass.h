#pragma once

#include "CoreMinimal.h"

class AActor;

namespace AnomalyHiddenClass
{
	ANOMALYINJECTOR_API void SetHideMode(int32 InMode);
	ANOMALYINJECTOR_API int32 GetHideMode();
	ANOMALYINJECTOR_API const TCHAR* DescribeHideMode();

	ANOMALYINJECTOR_API void SetOmitShadowSilencing(bool bInOmit);
	ANOMALYINJECTOR_API bool IsOmitShadowSilencing();

	ANOMALYINJECTOR_API void SetOmitDepthPassSilencing(bool bInOmit);
	ANOMALYINJECTOR_API bool IsOmitDepthPassSilencing();

	ANOMALYINJECTOR_API void Hide(AActor* Actor);
	ANOMALYINJECTOR_API void Show(AActor* Actor);

	ANOMALYINJECTOR_API bool IsLogicallyHidden(const AActor* Actor);
	ANOMALYINJECTOR_API bool IsAnyLogicallyHidden();
	ANOMALYINJECTOR_API void RestoreAll();
}
