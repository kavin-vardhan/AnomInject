#include "AnomalyStencilTag.h"

#if ANOMALY_CAPTURE

#include "AnomalyViewport.h"

#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "EngineUtils.h"
#include "Engine/World.h"
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

bool FAnomalyStencilTagLedger::IsAssignable(uint8 Value) const
{
	const int32 Top = (BenchPoolLimit > 0)
		? FMath::Min(AnomalyStencilTag::AssignableStencilMax,
			AnomalyStencilTag::ReservedStencilBase + BenchPoolLimit - 1)
		: AnomalyStencilTag::AssignableStencilMax;

	return Value >= (uint8)AnomalyStencilTag::ReservedStencilBase
		&& Value <= (uint8)Top
		&& !HostReserved.Contains(Value);
}

bool FAnomalyStencilTagLedger::IsFree(uint8 Value) const
{
	return IsAssignable(Value) && !EventClaimed.Contains(Value) && !CensusClaimed.Contains(Value);
}

int32 FAnomalyStencilTagLedger::NumFree() const
{
	int32 N = 0;
	for (int32 V = AnomalyStencilTag::ReservedStencilBase; V <= AnomalyStencilTag::AssignableStencilMax; ++V)
	{
		if (IsFree((uint8)V))
		{
			++N;
		}
	}
	return N;
}

int32 FAnomalyStencilTagLedger::NumAssignable() const
{
	int32 N = 0;
	for (int32 V = AnomalyStencilTag::ReservedStencilBase; V <= AnomalyStencilTag::AssignableStencilMax; ++V)
	{
		if (IsAssignable((uint8)V))
		{
			++N;
		}
	}
	return N;
}

void FAnomalyStencilTagLedger::Reset()
{
	HostReserved.Reset();
	EventClaimed.Reset();
	CensusClaimed.Reset();
}

namespace AnomalyStencilTag
{
	FString JoinValues(const TArray<uint8>& Values)
	{
		TArray<FString> Parts;
		Parts.Reserve(Values.Num());
		for (uint8 V : Values)
		{
			Parts.Add(FString::FromInt((int32)V));
		}
		return FString::Join(Parts, TEXT(","));
	}

	FString JoinValues(const TSet<uint8>& Values)
	{
		TArray<uint8> Sorted = Values.Array();
		Sorted.Sort();
		return JoinValues(Sorted);
	}

	int32 TagActor(AActor* Actor, int32 StencilValue)
	{
		return TagActor(Actor, StencilValue, nullptr);
	}

	int32 TagActor(AActor* Actor, int32 StencilValue, int32* OutFlagFlips)
	{
		if (OutFlagFlips)
		{
			*OutFlagFlips = 0;
		}
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

			if (OutFlagFlips && !Prim->bRenderCustomDepth)
			{
				++(*OutFlagFlips);
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

	bool IsAnyComponentTagged(const AActor* Actor)
	{
		if (!Actor)
		{
			return false;
		}
		TInlineComponentArray<UPrimitiveComponent*> Prims;
		const_cast<AActor*>(Actor)->GetComponents(Prims);
		for (UPrimitiveComponent* Prim : Prims)
		{
			if (GTaggedComponents.Contains(TWeakObjectPtr<UPrimitiveComponent>(Prim)))
			{
				return true;
			}
		}
		return false;
	}

	FString ForgetOneTaggedComponentOfActor(AActor* Actor)
	{
		if (!Actor)
		{
			return FString();
		}
		TInlineComponentArray<UPrimitiveComponent*> Prims;
		Actor->GetComponents(Prims);
		for (UPrimitiveComponent* Prim : Prims)
		{
			const TWeakObjectPtr<UPrimitiveComponent> Key(Prim);
			if (GTaggedComponents.Contains(Key))
			{
				GTaggedComponents.Remove(Key);
				return Prim->GetName();
			}
		}
		return FString();
	}

	void GetTaggedComponents(TSet<TWeakObjectPtr<UPrimitiveComponent>>& Out)
	{
		Out.Reset();
		for (const TPair<TWeakObjectPtr<UPrimitiveComponent>, FPriorStencilState>& Pair : GTaggedComponents)
		{
			Out.Add(Pair.Key);
		}
	}

	void GetTaggedActorsByValue(TMap<uint8, TArray<FString>>& Out)
	{
		Out.Reset();
		for (const TPair<TWeakObjectPtr<UPrimitiveComponent>, FPriorStencilState>& Pair : GTaggedComponents)
		{
			const UPrimitiveComponent* Prim = Pair.Key.Get();
			if (!Prim || !Prim->bRenderCustomDepth)
			{
				continue;
			}
			const int32 V = Prim->CustomDepthStencilValue;
			if (V < ReservedStencilBase || V > ReservedStencilMax)
			{
				continue;
			}
			const AActor* Owner = Prim->GetOwner();
			const FString Name = Owner ? Owner->GetName() : TEXT("(no owner)");
			TArray<FString>& Names = Out.FindOrAdd((uint8)V);
			Names.AddUnique(Name);
		}
	}

	FString DescribeOwnership(const TMap<uint8, TArray<FString>>& ByValue)
	{
		TArray<uint8> Values;
		ByValue.GetKeys(Values);
		Values.Sort();
		TArray<FString> Parts;
		Parts.Reserve(Values.Num());
		for (uint8 V : Values)
		{
			Parts.Add(FString::Printf(TEXT("%d=%s"), (int32)V, *FString::Join(ByValue[V], TEXT("|"))));
		}
		return FString::Join(Parts, TEXT(", "));
	}

	TSet<uint8> SnapshotHostReservedValues(UWorld* World)
	{
		TSet<uint8> Reserved;
		if (!World)
		{
			return Reserved;
		}
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor)
			{
				continue;
			}
			TInlineComponentArray<UPrimitiveComponent*> Prims;
			Actor->GetComponents(Prims);
			for (const UPrimitiveComponent* Prim : Prims)
			{
				if (!Prim || !Prim->bRenderCustomDepth)
				{
					continue;
				}
				const int32 V = Prim->CustomDepthStencilValue;
				if (V < ReservedStencilBase || V > AssignableStencilMax)
				{
					continue;
				}
				if (GTaggedComponents.Contains(TWeakObjectPtr<UPrimitiveComponent>(const_cast<UPrimitiveComponent*>(Prim))))
				{
					continue;
				}
				Reserved.Add((uint8)V);
			}
		}
		return Reserved;
	}

	void SnapshotCustomDepthEnabled(UWorld* World, TMap<TWeakObjectPtr<UPrimitiveComponent>, int32>& Out)
	{
		Out.Reset();
		if (!World)
		{
			return;
		}
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor)
			{
				continue;
			}
			TInlineComponentArray<UPrimitiveComponent*> Prims;
			Actor->GetComponents(Prims);
			for (UPrimitiveComponent* Prim : Prims)
			{
				if (Prim && Prim->bRenderCustomDepth)
				{
					Out.Add(TWeakObjectPtr<UPrimitiveComponent>(Prim), Prim->CustomDepthStencilValue);
				}
			}
		}
	}

	int32 DiffCustomDepthSnapshots(
		const TMap<TWeakObjectPtr<UPrimitiveComponent>, int32>& Before,
		const TMap<TWeakObjectPtr<UPrimitiveComponent>, int32>& After,
		const TSet<TWeakObjectPtr<UPrimitiveComponent>>* Exclude,
		FString& OutFirstDiff, int32* OutOursDiffs, int32* OutHostDiffs)
	{
		OutFirstDiff.Reset();
		int32 Diffs = 0;
		if (OutOursDiffs) { *OutOursDiffs = 0; }
		if (OutHostDiffs) { *OutHostDiffs = 0; }

		auto InReservedRange = [](int32 V)
		{
			return V >= ReservedStencilBase && V <= ReservedStencilMax;
		};
		auto Attribute = [&](int32 A, int32 B)
		{
			if (InReservedRange(A) || InReservedRange(B))
			{
				if (OutOursDiffs) { ++(*OutOursDiffs); }
			}
			else if (OutHostDiffs)
			{
				++(*OutHostDiffs);
			}
		};

		auto NameOf = [](const TWeakObjectPtr<UPrimitiveComponent>& Key) -> FString
		{
			if (const UPrimitiveComponent* Prim = Key.Get())
			{
				const AActor* Owner = Prim->GetOwner();
				return FString::Printf(TEXT("%s/%s"), Owner ? *Owner->GetName() : TEXT("?"), *Prim->GetName());
			}
			return TEXT("(stale component)");
		};

		for (const TPair<TWeakObjectPtr<UPrimitiveComponent>, int32>& Pair : Before)
		{
			if (!Pair.Key.IsValid() || (Exclude && Exclude->Contains(Pair.Key)))
			{
				continue;
			}
			const int32* NowValue = After.Find(Pair.Key);
			if (!NowValue)
			{
				++Diffs;
				Attribute(Pair.Value, Pair.Value);
				if (OutFirstDiff.IsEmpty())
				{
					OutFirstDiff = FString::Printf(TEXT("%s lost bRenderCustomDepth (was value %d)"), *NameOf(Pair.Key), Pair.Value);
				}
			}
			else if (*NowValue != Pair.Value)
			{
				++Diffs;
				Attribute(Pair.Value, *NowValue);
				if (OutFirstDiff.IsEmpty())
				{
					OutFirstDiff = FString::Printf(TEXT("%s stencil value %d -> %d"), *NameOf(Pair.Key), Pair.Value, *NowValue);
				}
			}
		}
		for (const TPair<TWeakObjectPtr<UPrimitiveComponent>, int32>& Pair : After)
		{
			if (!Pair.Key.IsValid() || (Exclude && Exclude->Contains(Pair.Key)))
			{
				continue;
			}
			if (!Before.Contains(Pair.Key))
			{
				++Diffs;
				Attribute(Pair.Value, Pair.Value);
				if (OutFirstDiff.IsEmpty())
				{
					OutFirstDiff = FString::Printf(TEXT("%s gained bRenderCustomDepth (value %d)"), *NameOf(Pair.Key), Pair.Value);
				}
			}
		}
		return Diffs;
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
