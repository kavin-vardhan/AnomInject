#include "AnomalyHiddenClass.h"

#include "AnomalyInjectorLog.h"

#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"

namespace AnomalyHiddenClass
{
	namespace
	{
		struct FSavedPrimitive
		{
			TWeakObjectPtr<UPrimitiveComponent> Component;
			uint8 bRenderInMainPass : 1;
			uint8 bRenderInDepthPass : 1;
			uint8 bCastShadow : 1;
			uint8 bCastContactShadow : 1;
			uint8 bAffectDynamicIndirectLighting : 1;
			uint8 bAffectDistanceFieldLighting : 1;
			uint8 bVisibleInRayTracing : 1;
			uint8 bReceivesDecals : 1;
		};

		struct FSavedActor
		{
			TArray<FSavedPrimitive> Primitives;
		};

		TMap<TWeakObjectPtr<AActor>, FSavedActor> GHidden;
		int32 GHideMode = 1;
		bool GOmitShadowSilencing = false;
	}

	void SetHideMode(int32 InMode)
	{
		GHideMode = (InMode != 0) ? 1 : 0;
	}

	int32 GetHideMode()
	{
		return GHideMode;
	}

	const TCHAR* DescribeHideMode()
	{
		return (GHideMode != 0)
			? TEXT("1 (m45: main-pass-off, custom depth KEPT so the would-be silhouette is still measurable)")
			: TEXT("0 (pre-m45: SetActorHiddenInGame - no custom depth, no mask on hidden frames)");
	}

	void SetOmitShadowSilencing(bool bInOmit)
	{
		GOmitShadowSilencing = bInOmit;
	}

	bool IsOmitShadowSilencing()
	{
		return GOmitShadowSilencing;
	}

	void Hide(AActor* Actor)
	{
		if (!Actor)
		{
			return;
		}

		if (GHideMode == 0)
		{
			Actor->SetActorHiddenInGame(true);
			return;
		}

		if (GHidden.Contains(Actor))
		{
			return;
		}

		FSavedActor Saved;
		for (UActorComponent* AC : Actor->GetComponents())
		{
			UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(AC);
			if (!Prim)
			{
				continue;
			}

			FSavedPrimitive S;
			S.Component = Prim;
			S.bRenderInMainPass = Prim->bRenderInMainPass ? 1 : 0;
			S.bRenderInDepthPass = Prim->bRenderInDepthPass ? 1 : 0;
			S.bCastShadow = Prim->CastShadow ? 1 : 0;
			S.bCastContactShadow = Prim->bCastContactShadow ? 1 : 0;
			S.bAffectDynamicIndirectLighting = Prim->bAffectDynamicIndirectLighting ? 1 : 0;
			S.bAffectDistanceFieldLighting = Prim->bAffectDistanceFieldLighting ? 1 : 0;
			S.bVisibleInRayTracing = Prim->bVisibleInRayTracing ? 1 : 0;
			S.bReceivesDecals = Prim->bReceivesDecals ? 1 : 0;
			Saved.Primitives.Add(S);

			Prim->bRenderInMainPass = false;
			Prim->bRenderInDepthPass = false;
			if (!GOmitShadowSilencing)
			{
				Prim->SetCastShadow(false);
				Prim->bCastContactShadow = false;
			}
			Prim->bAffectDynamicIndirectLighting = false;
			Prim->bAffectDistanceFieldLighting = false;
			Prim->bVisibleInRayTracing = false;
			Prim->bReceivesDecals = false;
			Prim->MarkRenderStateDirty();
		}

		if (Saved.Primitives.Num() > 0)
		{
			GHidden.Add(Actor, MoveTemp(Saved));
		}
	}

	void Show(AActor* Actor)
	{
		if (!Actor)
		{
			return;
		}

		if (GHideMode == 0 && !GHidden.Contains(Actor))
		{
			Actor->SetActorHiddenInGame(false);
			return;
		}

		FSavedActor Saved;
		if (!GHidden.RemoveAndCopyValue(Actor, Saved))
		{
			Actor->SetActorHiddenInGame(false);
			return;
		}

		for (const FSavedPrimitive& S : Saved.Primitives)
		{
			UPrimitiveComponent* Prim = S.Component.Get();
			if (!Prim)
			{
				continue;
			}
			Prim->bRenderInMainPass = S.bRenderInMainPass != 0;
			Prim->bRenderInDepthPass = S.bRenderInDepthPass != 0;
			Prim->SetCastShadow(S.bCastShadow != 0);
			Prim->bCastContactShadow = S.bCastContactShadow != 0;
			Prim->bAffectDynamicIndirectLighting = S.bAffectDynamicIndirectLighting != 0;
			Prim->bAffectDistanceFieldLighting = S.bAffectDistanceFieldLighting != 0;
			Prim->bVisibleInRayTracing = S.bVisibleInRayTracing != 0;
			Prim->bReceivesDecals = S.bReceivesDecals != 0;
			Prim->MarkRenderStateDirty();
		}
	}

	bool IsLogicallyHidden(const AActor* Actor)
	{
		if (!Actor)
		{
			return false;
		}
		if (Actor->IsHidden())
		{
			return true;
		}
		return GHidden.Contains(const_cast<AActor*>(Actor));
	}

	bool IsAnyLogicallyHidden()
	{
		return GHidden.Num() > 0;
	}

	void RestoreAll()
	{
		TArray<TWeakObjectPtr<AActor>> Keys;
		GHidden.GetKeys(Keys);
		for (const TWeakObjectPtr<AActor>& Weak : Keys)
		{
			if (AActor* Actor = Weak.Get())
			{
				Show(Actor);
			}
		}
		GHidden.Reset();
	}
}
