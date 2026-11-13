#include "AnomalyTerrain.h"

#include "AnomalyTerrainRule.h"
#include "AnomalyDefaults.h"
#include "AnomalyInjectorLog.h"

#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkinnedAsset.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"
#include "HAL/IConsoleManager.h"

namespace
{
	using FTerrainText = AnomalyTerrainRule::TText<TCHAR>;
	using FTerrainPart = AnomalyTerrainRule::TPart<TCHAR>;
	using FTerrainObject = AnomalyTerrainRule::TObject<TCHAR>;

	bool GTerrainBenchClassRuleInert = false;
	bool GTerrainBenchRefusalOff = false;
	int32 GTerrainUnlabelableRefusals = 0;

	const TCHAR* TerrainLandscapeActorClassPath()
	{
		return TEXT("/Script/Landscape.LandscapeProxy");
	}

	const TCHAR* TerrainLandscapeComponentClassPath()
	{
		return TEXT("/Script/Landscape.LandscapeComponent");
	}

	FTerrainText TerrainView(const FString& S)
	{
		FTerrainText T;
		T.Ptr = *S;
		T.Len = S.Len();
		return T;
	}

	void AppendTerrainClassChain(const UObject* Obj, TArray<FString>& Out)
	{
		for (const UClass* C = Obj ? Obj->GetClass() : nullptr; C; C = C->GetSuperClass())
		{
			Out.Add(C->GetName());
		}
	}

	FString TerrainMeshAssetName(const UPrimitiveComponent* Component)
	{
		if (const UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Component))
		{
			if (const UStaticMesh* Mesh = SMC->GetStaticMesh())
			{
				return Mesh->GetName();
			}
		}
		else if (const USkinnedMeshComponent* SKC = Cast<USkinnedMeshComponent>(Component))
		{
			if (const USkinnedAsset* Asset = SKC->GetSkinnedAsset())
			{
				return Asset->GetName();
			}
		}
		return FString();
	}

	bool IsTerrainClassResolvable(const TCHAR* Path, const char* ExpectedName)
	{
		const UClass* Found = FindObject<UClass>(nullptr, Path);
		return Found != nullptr && Found->GetName() == FString(ANSI_TO_TCHAR(ExpectedName));
	}

	struct FTerrainClassRuleState
	{
		bool bActorClass = false;
		bool bComponentClass = false;
		bool bForcedInert = false;

		bool IsActive() const
		{
			return !bForcedInert && bActorClass && bComponentClass;
		}
	};

	FTerrainClassRuleState ResolveTerrainClassRule()
	{
		FTerrainClassRuleState S;
		S.bActorClass = IsTerrainClassResolvable(TerrainLandscapeActorClassPath(), AnomalyTerrainRule::LandscapeActorClass());
		S.bComponentClass = IsTerrainClassResolvable(TerrainLandscapeComponentClassPath(), AnomalyTerrainRule::LandscapeComponentClass());
		S.bForcedInert = GTerrainBenchClassRuleInert;
		return S;
	}

	struct FTerrainPartStore
	{
		FString Name;
		FString OwnClass;
		FString Asset;
		TArray<FString> Chain;
	};
}

namespace AnomalyTerrain
{
	FTerrainVerdict Classify(const AActor* Actor)
	{
		FTerrainVerdict Out;
		if (!Actor)
		{
			return Out;
		}

		const FString ActorName = Actor->GetName();
		TArray<FString> ActorChain;
		AppendTerrainClassChain(Actor, ActorChain);

		TArray<FTerrainPartStore> Stores;
		TArray<UPrimitiveComponent*> Prims;
		Actor->GetComponents<UPrimitiveComponent>(Prims);
		Stores.Reserve(Prims.Num());
		for (const UPrimitiveComponent* Prim : Prims)
		{
			if (!Prim)
			{
				continue;
			}
			FTerrainPartStore& S = Stores.AddDefaulted_GetRef();
			S.Name = Prim->GetName();
			S.Asset = TerrainMeshAssetName(Prim);
			AppendTerrainClassChain(Prim, S.Chain);
			S.OwnClass = S.Chain.Num() > 0 ? S.Chain[0] : FString();
		}

		TArray<FTerrainText> ActorChainViews;
		ActorChainViews.Reserve(ActorChain.Num());
		for (const FString& C : ActorChain)
		{
			ActorChainViews.Add(TerrainView(C));
		}
		TArray<TArray<FTerrainText>> PartChainViews;
		PartChainViews.SetNum(Stores.Num());
		TArray<FTerrainPart> Parts;
		Parts.SetNum(Stores.Num());
		for (int32 i = 0; i < Stores.Num(); ++i)
		{
			for (const FString& C : Stores[i].Chain)
			{
				PartChainViews[i].Add(TerrainView(C));
			}
			Parts[i].Name = TerrainView(Stores[i].Name);
			Parts[i].Asset = TerrainView(Stores[i].Asset);
			Parts[i].Chain = PartChainViews[i].Num() > 0 ? PartChainViews[i].GetData() : nullptr;
			Parts[i].ChainNum = PartChainViews[i].Num();
		}

		FTerrainObject Obj;
		Obj.Name = TerrainView(ActorName);
		Obj.Chain = ActorChainViews.Num() > 0 ? ActorChainViews.GetData() : nullptr;
		Obj.ChainNum = ActorChainViews.Num();
		Obj.Parts = Parts.Num() > 0 ? Parts.GetData() : nullptr;
		Obj.NumParts = Parts.Num();

		const TArray<FString>& Patterns = AnomalyDefaults::GetTerrainPatterns();
		TArray<FTerrainText> PatternViews;
		PatternViews.Reserve(Patterns.Num());
		for (const FString& P : Patterns)
		{
			PatternViews.Add(TerrainView(P));
		}

		const FTerrainClassRuleState ClassRule = ResolveTerrainClassRule();
		const AnomalyTerrainRule::FVerdict V = AnomalyTerrainRule::Classify(Obj, ClassRule.IsActive(),
			PatternViews.Num() > 0 ? PatternViews.GetData() : nullptr, PatternViews.Num());

		Out.bTerrain = AnomalyTerrainRule::IsTerrain(V);
		Out.bEngineLandscape = AnomalyTerrainRule::IsEngineLandscape(V);
		Out.bRefuseBlink = AnomalyTerrainRule::RefusesBlink(V, IsBlinkRefusalEnabled());
		if (!Out.bTerrain)
		{
			return Out;
		}

		const bool bPartValid = Stores.IsValidIndex(V.Part);
		if (V.Rule == AnomalyTerrainRule::ERule::Class)
		{
			if (V.Field == AnomalyTerrainRule::EField::ComponentClass && bPartValid)
			{
				Out.Rule = FString::Printf(TEXT("class:%s@component=%s"), *Stores[V.Part].OwnClass, *Stores[V.Part].Name);
			}
			else
			{
				Out.Rule = FString::Printf(TEXT("class:%s"), ActorChain.Num() > 0 ? *ActorChain[0] : TEXT("?"));
			}
			return Out;
		}

		FString Value;
		switch (V.Field)
		{
		case AnomalyTerrainRule::EField::Actor: Value = ActorName; break;
		case AnomalyTerrainRule::EField::ActorClass: Value = ActorChain.Num() > 0 ? ActorChain[0] : FString(); break;
		case AnomalyTerrainRule::EField::Component: Value = bPartValid ? Stores[V.Part].Name : FString(); break;
		case AnomalyTerrainRule::EField::ComponentClass: Value = bPartValid ? Stores[V.Part].OwnClass : FString(); break;
		case AnomalyTerrainRule::EField::Asset: Value = bPartValid ? Stores[V.Part].Asset : FString(); break;
		default: break;
		}
		Out.Rule = FString::Printf(TEXT("pattern:%s@%s=%s"),
			Patterns.IsValidIndex(V.Pattern) ? *Patterns[V.Pattern] : TEXT("?"),
			ANSI_TO_TCHAR(AnomalyTerrainRule::DescribeField(V.Field)), *Value);
		return Out;
	}

	bool IsClassRuleActive()
	{
		return ResolveTerrainClassRule().IsActive();
	}

	FString DescribeClassRule()
	{
		const FTerrainClassRuleState S = ResolveTerrainClassRule();
		if (S.IsActive())
		{
			return FString::Printf(TEXT("ACTIVE[actor IsA %s | component IsA %s]"),
				ANSI_TO_TCHAR(AnomalyTerrainRule::LandscapeActorClass()),
				ANSI_TO_TCHAR(AnomalyTerrainRule::LandscapeComponentClass()));
		}
		if (S.bForcedInert)
		{
			return FString(TEXT("INERT[forced by IAI.Bench.TerrainClassRuleInert - BENCH DEVICE]"));
		}
		return FString::Printf(TEXT("INERT[%s %s, %s %s - the class rule matches NOTHING this run]"),
			TerrainLandscapeActorClassPath(), S.bActorClass ? TEXT("found") : TEXT("NOT FOUND"),
			TerrainLandscapeComponentClassPath(), S.bComponentClass ? TEXT("found") : TEXT("NOT FOUND"));
	}

	FString DescribeRules()
	{
		return FString::Printf(
			TEXT("class=%s | patterns=%s source=%s | refusal=%s | subtype=%s on a terrain blink, %s otherwise"),
			*DescribeClassRule(),
			*AnomalyDefaults::DescribeTerrainPatterns(),
			*AnomalyDefaults::TerrainPatternsSource(),
			IsBlinkRefusalEnabled()
				? TEXT("ON(blinking on engine landscape is refused terrain_unlabelable)")
				: TEXT("OFF(IAI.Bench.TerrainRefusalOff - BENCH DEVICE; engine-landscape blinks fire with no mask and a wrong box)"),
			ANSI_TO_TCHAR(AnomalyTerrainRule::TerrainBlinkSubtype()),
			ANSI_TO_TCHAR(AnomalyTerrainRule::DefaultBlinkSubtype()));
	}

	bool IsBlinkRefusalEnabled()
	{
		return !GTerrainBenchRefusalOff;
	}

	void ResetRunStats()
	{
		GTerrainUnlabelableRefusals = 0;
	}

	void NoteUnlabelableRefusal(const AActor* Actor, const FString& Rule)
	{
		++GTerrainUnlabelableRefusals;
		UE_LOG(LogAnomaly, Warning,
			TEXT("REFUSED-TERRAIN actor='%s' rule=%s reason=%s - blinking is refused on ENGINE LANDSCAPE because such an ")
			TEXT("event cannot be labelled to the product standard today: no component of a landscape tile is tagged for ")
			TEXT("the mask, and the box covers the grass, a point or nothing, never the terrain. Mesh terrain matched by a ")
			TEXT("pattern is NOT refused. Counted in run_summary.terrain_unlabelable_refusals."),
			*GetNameSafe(Actor), *Rule, ANSI_TO_TCHAR(AnomalyTerrainRule::UnlabelableReason()));
	}

	int32 GetUnlabelableRefusalCount()
	{
		return GTerrainUnlabelableRefusals;
	}

	FString TerrainBlinkSubtype()
	{
		return FString(ANSI_TO_TCHAR(AnomalyTerrainRule::TerrainBlinkSubtype()));
	}

	FString DefaultBlinkSubtype()
	{
		return FString(ANSI_TO_TCHAR(AnomalyTerrainRule::DefaultBlinkSubtype()));
	}

	FString UnlabelableReason()
	{
		return FString(ANSI_TO_TCHAR(AnomalyTerrainRule::UnlabelableReason()));
	}
}

#if !UE_BUILD_SHIPPING
static FAutoConsoleCommand GBenchTerrainRefusalOffCmd(
	TEXT("IAI.Bench.TerrainRefusalOff"),
	TEXT("BENCH DEVICE, console only, default 0 - no ini key, never in a client payload. 1 turns OFF the m58 refusal of ")
	TEXT("blinking on engine landscape (terrain_unlabelable), so gate C-G2 can show what such an event carries: subtype ")
	TEXT("terrain_disappear_reappear with rule class:<ActorClass>, target_pixels -1, no mask and a box that is not the ")
	TEXT("terrain. Takes effect on the next blinking Apply. Usage: IAI.Bench.TerrainRefusalOff <0|1>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.TerrainRefusalOff <0|1>  (currently %d)"), GTerrainBenchRefusalOff ? 1 : 0);
			return;
		}
		GTerrainBenchRefusalOff = FCString::Atoi(*Args[0]) != 0;
		UE_LOG(LogAnomaly, Warning,
			TEXT("IAI.Bench.TerrainRefusalOff -> %d. BENCH DEVICE, console only, never in a client payload. EFFECTIVE ")
			TEXT("READ-BACK: refusal %s."),
			GTerrainBenchRefusalOff ? 1 : 0, AnomalyTerrain::IsBlinkRefusalEnabled() ? TEXT("ON") : TEXT("OFF"));
	}));

static FAutoConsoleCommand GBenchTerrainClassRuleInertCmd(
	TEXT("IAI.Bench.TerrainClassRuleInert"),
	TEXT("BENCH DEVICE, console only, default 0 - no ini key, never in a client payload. 1 forces the m58 terrain CLASS ")
	TEXT("rule INERT, as if the landscape classes could not be found, so gate C-G2's can-fail leg can prove that removing ")
	TEXT("the detection fails the gate: the run banner must read class=INERT, no target is refused, and a landscape blink ")
	TEXT("reads disappear_reappear. Usage: IAI.Bench.TerrainClassRuleInert <0|1>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.TerrainClassRuleInert <0|1>  (currently %d)"),
				GTerrainBenchClassRuleInert ? 1 : 0);
			return;
		}
		GTerrainBenchClassRuleInert = FCString::Atoi(*Args[0]) != 0;
		UE_LOG(LogAnomaly, Warning,
			TEXT("IAI.Bench.TerrainClassRuleInert -> %d. BENCH DEVICE, console only, never in a client payload. EFFECTIVE ")
			TEXT("READ-BACK: class rule %s."),
			GTerrainBenchClassRuleInert ? 1 : 0, *AnomalyTerrain::DescribeClassRule());
	}));
#endif
