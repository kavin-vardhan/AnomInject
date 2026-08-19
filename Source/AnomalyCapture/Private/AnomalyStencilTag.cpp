#include "AnomalyStencilTag.h"

#if ANOMALY_CAPTURE

#include "AnomalyViewport.h"

#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "HAL/IConsoleManager.h"

namespace
{
	struct FPriorStencilState
	{
		bool bRenderCustomDepth = false;
		int32 CustomDepthStencilValue = 0;
	};

	TMap<TWeakObjectPtr<UPrimitiveComponent>, FPriorStencilState> GTaggedComponents;

	int32 GCustomStencilRefCount = 0;
	int32 GSavedCustomDepthValue = 0;
}

namespace AnomalyStencilTag
{
	int32 TagActor(AActor* Actor, int32 StencilValue)
	{
		if (!Actor)
		{
			return 0;
		}

		const uint8 Value = (uint8)FMath::Clamp(StencilValue, ReservedStencilBase, ReservedStencilMax);

		int32 Tagged = 0;
		TInlineComponentArray<UPrimitiveComponent*> Prims;
		Actor->GetComponents(Prims);
		for (UPrimitiveComponent* Prim : Prims)
		{
			if (!AnomalyViewport::IsRenderableComponent(Prim))
			{
				continue;
			}

			const TWeakObjectPtr<UPrimitiveComponent> Key(Prim);
			if (!GTaggedComponents.Contains(Key))
			{
				FPriorStencilState Prior;
				Prior.bRenderCustomDepth = Prim->bRenderCustomDepth;
				Prior.CustomDepthStencilValue = Prim->CustomDepthStencilValue;
				GTaggedComponents.Add(Key, Prior);
			}

			Prim->SetCustomDepthStencilValue(Value);
			Prim->SetRenderCustomDepth(true);
			++Tagged;
		}
		return Tagged;
	}

	bool VerifyActorStillTagged(const AActor* Actor, int32 StencilValue, FString& OutDetail)
	{
		OutDetail.Reset();
		if (!Actor)
		{
			OutDetail = TEXT("actor gone");
			return false;
		}

		const uint8 Value = (uint8)FMath::Clamp(StencilValue, ReservedStencilBase, ReservedStencilMax);

		int32 Checked = 0;
		int32 Intact = 0;
		TInlineComponentArray<UPrimitiveComponent*> Prims;
		const_cast<AActor*>(Actor)->GetComponents(Prims);
		for (const UPrimitiveComponent* Prim : Prims)
		{
			if (!AnomalyViewport::IsRenderableComponent(Prim))
			{
				continue;
			}
			++Checked;
			if (Prim->bRenderCustomDepth && Prim->CustomDepthStencilValue == (int32)Value)
			{
				++Intact;
			}
			else if (OutDetail.IsEmpty())
			{
				OutDetail = FString::Printf(TEXT("component '%s' reads bRenderCustomDepth=%d value=%d, expected 1/%d"),
					*Prim->GetName(), Prim->bRenderCustomDepth ? 1 : 0, Prim->CustomDepthStencilValue, (int32)Value);
			}
		}

		if (Checked == 0)
		{
			OutDetail = TEXT("no renderable component to tag");
			return false;
		}
		return Intact == Checked;
	}

	void RestoreActor(AActor* Actor)
	{
		if (!Actor)
		{
			return;
		}

		TInlineComponentArray<UPrimitiveComponent*> Prims;
		Actor->GetComponents(Prims);
		for (UPrimitiveComponent* Prim : Prims)
		{
			const TWeakObjectPtr<UPrimitiveComponent> Key(Prim);
			if (const FPriorStencilState* Prior = GTaggedComponents.Find(Key))
			{
				Prim->SetCustomDepthStencilValue(Prior->CustomDepthStencilValue);
				Prim->SetRenderCustomDepth(Prior->bRenderCustomDepth);
				GTaggedComponents.Remove(Key);
			}
		}
	}

	void RestoreAll()
	{
		for (TPair<TWeakObjectPtr<UPrimitiveComponent>, FPriorStencilState>& Pair : GTaggedComponents)
		{
			if (UPrimitiveComponent* Prim = Pair.Key.Get())
			{
				Prim->SetCustomDepthStencilValue(Pair.Value.CustomDepthStencilValue);
				Prim->SetRenderCustomDepth(Pair.Value.bRenderCustomDepth);
			}
		}
		GTaggedComponents.Empty();
	}

	bool IsAnyTagged()
	{
		return GTaggedComponents.Num() > 0;
	}

	void EnableCustomStencil()
	{
		if (GCustomStencilRefCount++ == 0)
		{
			if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepth")))
			{
				GSavedCustomDepthValue = CVar->GetInt();
				CVar->Set(3, ECVF_SetByCode);
			}
		}
	}

	void DisableCustomStencil()
	{
		if (GCustomStencilRefCount > 0 && --GCustomStencilRefCount == 0)
		{
			if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepth")))
			{
				CVar->Set(GSavedCustomDepthValue, ECVF_SetByCode);
			}
		}
	}
}

#endif
