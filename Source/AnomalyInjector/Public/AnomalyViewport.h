#pragma once

#include "CoreMinimal.h"
#include "AnomalyTargeting.h"

class UWorld;
class AActor;
class UPrimitiveComponent;


struct FAnomalyViewInfo
{
	FVector Origin = FVector::ZeroVector;

	FRotator Rotation = FRotator::ZeroRotator;

	float HorizontalFOVDeg = 90.0f;

	float AspectRatio = 16.0f / 9.0f;

	bool bValid = false;
};

struct FRenderableActorInfo
{
	TWeakObjectPtr<AActor> Actor;
	FString ActorName;
	FString ClassName;
	FString ComponentType;
	float Distance = 0.0f;

	FVector2D ScreenMin = FVector2D::ZeroVector;
	FVector2D ScreenMax = FVector2D::ZeroVector;
	bool bRectValid = false;
};

struct FSelectionProvenance
{
	float CoveragePct = -1.0f;
	int32 OcclusionSamplesPassed = 0;
	int32 OcclusionSamplesTotal = 0;
	float PollDistance = -1.0f;
	bool bValid = false;
};

namespace AnomalyViewport
{
	ANOMALYINJECTOR_API bool EvaluateSelectionProvenance(UWorld* World, const AActor* Actor, FSelectionProvenance& Out);

	ANOMALYINJECTOR_API bool GetActiveViewInfo(UWorld* World, FAnomalyViewInfo& OutView);

	ANOMALYINJECTOR_API bool IsComponentInFrustum(const FAnomalyViewInfo& View, const UPrimitiveComponent* Component);

	ANOMALYINJECTOR_API bool IsComponentVisible(const FAnomalyViewInfo& View, UWorld* World, const UPrimitiveComponent* Component);

	ANOMALYINJECTOR_API bool IsActorVisible(const FAnomalyViewInfo& View, UWorld* World, const AActor* Actor);

	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> FilterVisibleActors(
		const FAnomalyViewInfo& View, UWorld* World, const TArray<TWeakObjectPtr<AActor>>& In);

	template <typename T>
	TArray<TWeakObjectPtr<T>> FilterVisibleComponents(
		const FAnomalyViewInfo& View, UWorld* World, const TArray<TWeakObjectPtr<T>>& In)
	{
		TArray<TWeakObjectPtr<T>> Result;
		for (const TWeakObjectPtr<T>& Weak : In)
		{
			T* Component = Weak.Get();
			if (Component && IsComponentVisible(View, World, Component))
			{
				Result.Add(Weak);
			}
		}
		return Result;
	}

	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> FindVisibleActorsMatching(UWorld* World, const FString& Substring);


	ANOMALYINJECTOR_API bool IsRenderableComponent(const UPrimitiveComponent* Component);

	ANOMALYINJECTOR_API bool IsRenderableGeometryComponent(const UPrimitiveComponent* Component);

	ANOMALYINJECTOR_API bool GetActorRenderableBounds(const AActor* Actor, FBox& OutBox);

	ANOMALYINJECTOR_API bool IsActorRenderableVisible(const FAnomalyViewInfo& View, UWorld* World, const AActor* Actor);

	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> FilterRenderableVisibleActors(
		const FAnomalyViewInfo& View, UWorld* World, const TArray<TWeakObjectPtr<AActor>>& In);


	ANOMALYINJECTOR_API void SetPollRadius(float Radius);

	ANOMALYINJECTOR_API float GetPollRadius();

	ANOMALYINJECTOR_API void SetOverlaysSuppressed(bool bSuppressed);

	ANOMALYINJECTOR_API bool AreOverlaysSuppressed();


	ANOMALYINJECTOR_API void SetMinScreenCoveragePct(float Pct);

	ANOMALYINJECTOR_API float GetMinScreenCoveragePct();

	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> GetVisibleRenderableActors(UWorld* World);

	ANOMALYINJECTOR_API TArray<FRenderableActorInfo> GetVisibleRenderableActorInfos(UWorld* World);

	ANOMALYINJECTOR_API bool ProjectActorBoundsToScreenRect(
		const FAnomalyViewInfo& View, const AActor* Actor, FVector2D& OutMin, FVector2D& OutMax);

	ANOMALYINJECTOR_API float GetActorScreenCoveragePct(UWorld* World, const AActor* Actor);

	ANOMALYINJECTOR_API float GetActorPollDistanceCm(UWorld* World, const AActor* Actor);

	ANOMALYINJECTOR_API void ResetTargetExclusionStats();

	ANOMALYINJECTOR_API int32 GetTargetExclusionCount();

	template <typename T>
	TArray<TWeakObjectPtr<T>> FindVisibleComponentsMatching(UWorld* World, const FString& Substring)
	{
		TArray<TWeakObjectPtr<T>> Matched = AnomalyTargeting::FindComponentsMatching<T>(World, Substring);
		FAnomalyViewInfo View;
		if (!GetActiveViewInfo(World, View))
		{
			return Matched;
		}
		return FilterVisibleComponents<T>(View, World, Matched);
	}
}
