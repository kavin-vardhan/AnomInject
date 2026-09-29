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
	FString AssetName;
	FString ComponentClass;
	float Distance = 0.0f;

	FVector2D ScreenMin = FVector2D::ZeroVector;
	FVector2D ScreenMax = FVector2D::ZeroVector;
	bool bRectValid = false;
};

struct FAnomalyNearClipSlabResult
{
	bool bEvaluated = false;
	bool bSlab = false;
	bool bNearOverridden = false;
	bool bNotPerspective = false;
	float BaselineNear = 0.0f;
	float AnomalousNear = 0.0f;
	float EffectiveNear = 0.0f;
	int32 SlabPrimitives = 0;
	int32 EyeInsideBox = 0;
	int32 Candidates = 0;
	int32 Enumerated = 0;
	int32 BoxFallbacks = 0;
	int32 InstancesTested = 0;
	int32 FxExcluded = 0;
	double Micros = 0.0;
	FString FirstHit;
	bool bSatPositive = false;
	bool bUnconfirmed = false;
	float ClippedRayFraction = 0.0f;
	int32 ConfirmCandidates = 0;
	int32 ConfirmTraces = 0;
	int32 ConfirmHits = 0;
	int32 ConfirmMisses = 0;
	int32 ConfirmUnconfirmable = 0;
	int32 ConfirmOverCap = 0;
	int32 ConfirmTraceCapped = 0;
	int32 ConfirmNoRay = 0;
	int32 ConfirmTooFewValid = 0;
	int32 ConfirmFullSlabFallbacks = 0;
	int32 ConfirmInvalidSegments = 0;
	int32 LandscapeCandidates = 0;
	int32 SkinnedUnconfirmable = 0;
	int32 NoCollisionUnconfirmable = 0;
	int32 NoComplexUnconfirmable = 0;
	int32 WeldedUnconfirmable = 0;
	int32 InstanceTransformUnconfirmable = 0;
	int32 HismTreeQueries = 0;
	int32 IsmFullScans = 0;
	double ConfirmMicros = 0.0;
	FString FirstConfirmedHit;
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

	ANOMALYINJECTOR_API bool IsTranslucentOnlyComponent(const UPrimitiveComponent* Component,
		bool bAllowCustomDepthOptIn);

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

	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> GetCensusPrefilterActors(UWorld* World);

	ANOMALYINJECTOR_API TArray<FRenderableActorInfo> GetVisibleRenderableActorInfos(UWorld* World);

	ANOMALYINJECTOR_API bool ProjectActorBoundsToScreenRect(
		const FAnomalyViewInfo& View, const AActor* Actor, FVector2D& OutMin, FVector2D& OutMax);

	ANOMALYINJECTOR_API float GetActorScreenCoveragePct(UWorld* World, const AActor* Actor);

	ANOMALYINJECTOR_API float GetActorPollDistanceCm(UWorld* World, const AActor* Actor);

	ANOMALYINJECTOR_API bool IsGeometryWithinNearClipRadius(UWorld* World);

	ANOMALYINJECTOR_API bool EvaluateNearClipSlab(UWorld* World, float BaselineNear, float AnomalousNear,
		FAnomalyNearClipSlabResult& Out);

	ANOMALYINJECTOR_API float ComputeBoundsScreenSizeForActiveView(UWorld* World, const FVector& BoundsOrigin, float SphereRadius);

	ANOMALYINJECTOR_API void ResetTargetExclusionStats();

	ANOMALYINJECTOR_API int32 GetTargetExclusionCount();

	ANOMALYINJECTOR_API int32 GetTranslucentOnlyExclusionCount();

	struct ANOMALYINJECTOR_API FReadOnlyEnumerationScope
	{
		FReadOnlyEnumerationScope();
		~FReadOnlyEnumerationScope();
		FReadOnlyEnumerationScope(const FReadOnlyEnumerationScope&) = delete;
		FReadOnlyEnumerationScope& operator=(const FReadOnlyEnumerationScope&) = delete;
	};

	ANOMALYINJECTOR_API bool IsReadOnlyEnumeration();

	ANOMALYINJECTOR_API bool IsRenderableComponentReadOnly(const UPrimitiveComponent* Component);

	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> GetVisibleRenderableActorsReadOnly(UWorld* World);

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
