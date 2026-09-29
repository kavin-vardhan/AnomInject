#include "Anomalies/TexCorruptCore.h"

#include "AnomalyInjectorLog.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyTargeting.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/Package.h"
#include "UObject/UObjectIterator.h"

namespace AnomalyTexCorrupt
{
#if !UE_BUILD_SHIPPING
	namespace
	{
		struct FAssetSlotMid
		{
			TWeakObjectPtr<UStaticMesh> Mesh;
			TWeakObjectPtr<UAnomalyInjectorSubsystem> Holder;
			TArray<TWeakObjectPtr<UStaticMeshComponent>> Users;
			TArray<int32> SlotIndices;
			TArray<UMaterialInterface*> Originals;
			TArray<UMaterialInstanceDynamic*> Mids;
			FString ActorName;
		};

		FAssetSlotMid GAssetSlotMid;

		void MarkUsersDirty()
		{
			for (const TWeakObjectPtr<UStaticMeshComponent>& W : GAssetSlotMid.Users)
			{
				if (UStaticMeshComponent* C = W.Get())
				{
					C->MarkRenderStateDirty();
				}
			}
		}
	}
#endif

	void RestoreBenchAssetSlotMid(const TCHAR* Context)
	{
#if UE_BUILD_SHIPPING
		(void)Context;
#else
		UStaticMesh* Mesh = GAssetSlotMid.Mesh.Get();
		if (!Mesh && GAssetSlotMid.Mids.Num() == 0)
		{
			GAssetSlotMid = FAssetSlotMid();
			return;
		}
		int32 Restored = 0;
		int32 LeftAlone = 0;
		if (Mesh)
		{
			TArray<FStaticMaterial>& Materials = Mesh->GetStaticMaterials();
			for (int32 k = 0; k < GAssetSlotMid.SlotIndices.Num(); ++k)
			{
				const int32 i = GAssetSlotMid.SlotIndices[k];
				if (Materials.IsValidIndex(i) && Materials[i].MaterialInterface.Get() == GAssetSlotMid.Mids[k])
				{
					Materials[i].MaterialInterface = GAssetSlotMid.Originals[k];
					++Restored;
				}
				else
				{
					++LeftAlone;
				}
			}
			MarkUsersDirty();
		}
		if (UAnomalyInjectorSubsystem* Holder = GAssetSlotMid.Holder.Get())
		{
			for (UMaterialInstanceDynamic* Mid : GAssetSlotMid.Mids)
			{
				Holder->TexCorruptLetGo(Mid);
			}
			for (UMaterialInterface* Original : GAssetSlotMid.Originals)
			{
				Holder->TexCorruptLetGo(Original);
			}
		}
		UE_LOG(LogAnomaly, Warning,
			TEXT("IAI.Bench.TexCorruptAssetSlotMid: RESTORED mesh asset '%s' (%s) - %d slot(s) put back, %d left alone because ")
			TEXT("they no longer held our MID."),
			*GetPathNameSafe(Mesh), Context, Restored, LeftAlone);
		GAssetSlotMid = FAssetSlotMid();
#endif
	}

#if !UE_BUILD_SHIPPING
	namespace
	{
		int32 ParseInt(const TArray<FString>& Args, int32 Fallback)
		{
			return (Args.Num() >= 1 && Args[0].IsNumeric()) ? FCString::Atoi(*Args[0]) : Fallback;
		}

		const TCHAR* GFixtureRoot = TEXT("/Game/CaptureBenchTexCorrupt/");

		void ApplyAssetSlotMid(UWorld* World, const FString& ActorName)
		{
			RestoreBenchAssetSlotMid(TEXT("re-armed"));
			UAnomalyInjectorSubsystem* Injector = ResolveInjector(World);
			const TArray<TWeakObjectPtr<AActor>> Matches = AnomalyTargeting::FindActorsMatching(World, FString(TEXT("=")) + ActorName);
			AActor* Actor = Matches.Num() > 0 ? Matches[0].Get() : nullptr;
			if (!Actor || !Injector)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("IAI.Bench.TexCorruptAssetSlotMid: REFUSED - '%s' matched no live actor. NOTHING WAS CHANGED; a lever that ")
					TEXT("silently matches nothing is a clean null, so this says so."),
					*ActorName);
				return;
			}
			TArray<UStaticMeshComponent*> Comps;
			Actor->GetComponents<UStaticMeshComponent>(Comps);
			UStaticMesh* Mesh = nullptr;
			for (UStaticMeshComponent* C : Comps)
			{
				if (C && C->GetStaticMesh())
				{
					Mesh = C->GetStaticMesh();
					break;
				}
			}
			if (!Mesh || !Mesh->GetPathName().StartsWith(GFixtureRoot))
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("IAI.Bench.TexCorruptAssetSlotMid: REFUSED - '%s' uses mesh '%s', which is not under %s. The lever writes a ")
					TEXT("MID into a MESH ASSET's material array, so it only ever touches the fixture's own duplicate mesh. NOTHING ")
					TEXT("WAS CHANGED."),
					*ActorName, *GetPathNameSafe(Mesh), GFixtureRoot);
				return;
			}
			TArray<FStaticMaterial>& Materials = Mesh->GetStaticMaterials();
			GAssetSlotMid.Mesh = Mesh;
			GAssetSlotMid.Holder = Injector;
			GAssetSlotMid.ActorName = ActorName;
			for (int32 i = 0; i < Materials.Num(); ++i)
			{
				UMaterialInterface* Original = Materials[i].MaterialInterface;
				if (!Original)
				{
					continue;
				}
				UMaterialInstanceDynamic* Mid = UMaterialInstanceDynamic::Create(Original, GetTransientPackage());
				if (!Mid)
				{
					continue;
				}
				Injector->TexCorruptHold(Original);
				Injector->TexCorruptHold(Mid);
				GAssetSlotMid.SlotIndices.Add(i);
				GAssetSlotMid.Originals.Add(Original);
				GAssetSlotMid.Mids.Add(Mid);
				Materials[i].MaterialInterface = Mid;
			}
			for (TObjectIterator<UStaticMeshComponent> It; It; ++It)
			{
				UStaticMeshComponent* C = *It;
				if (C && C->GetWorld() == World && C->GetStaticMesh() == Mesh)
				{
					GAssetSlotMid.Users.Add(C);
				}
			}
			MarkUsersDirty();
			UE_LOG(LogAnomaly, Warning,
				TEXT("IAI.Bench.TexCorruptAssetSlotMid -> ARMED on '%s': %d slot(s) of mesh ASSET '%s' now hold a runtime MID, and ")
				TEXT("%d using component(s) were marked render-state dirty. The next m53 fire at it must refuse host_mid ")
				TEXT("where=asset_slot kind=mid (plan R5, Delta-1). BENCH DEVICE; restored when turned off, at run end and at world ")
				TEXT("teardown."),
				*ActorName, GAssetSlotMid.Mids.Num(), *Mesh->GetPathName(), GAssetSlotMid.Users.Num());
		}

		void Echo(const TCHAR* Command)
		{
			UE_LOG(LogAnomaly, Warning, TEXT("%s -> levers now [%s]. BENCH DEVICE: console only, default off, never in a client payload."),
				Command, *DescribeLevers());
		}

		FAutoConsoleCommand GNoApplyCmd(
			TEXT("IAI.Bench.TexCorruptNoApply"),
			TEXT("BENCH DEVICE (m53). 1 = the matched null: the same recipe, reservation, allocation, draws and MIDs, and ONLY the ")
			TEXT("slot commit (transaction step 7) skipped. 2 = the no-allocation null for G-COLL: the decision tree and the ")
			TEXT("reservation arithmetic run and nothing is allocated, drawn or committed. Both label the event with condition_held ")
			TEXT("false. Usage: IAI.Bench.TexCorruptNoApply <0|1|2>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
			{
				const int32 V = ParseInt(Args, -1);
				if (V < 0 || V > 2)
				{
					UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.TexCorruptNoApply <0|1|2>"));
					return;
				}
				Levers().NoApply = V;
				Echo(TEXT("IAI.Bench.TexCorruptNoApply"));
			}));

		FAutoConsoleCommand GWrongCopyCmd(
			TEXT("IAI.Bench.TexCorruptWrongCopy"),
			TEXT("BENCH DEVICE (m53, plan R12.2). A deliberately wrong copy that must FAIL G-ID on the rows it is assigned to: ")
			TEXT("chanswap (R<->B), srgbtwice (double sRGB encode), mipshift (level m reads mip m+1), mipgen (skip the per-mip ")
			TEXT("copies and let the engine regenerate), texelshift (one texel along U only, plan A), normal (skip the .rg ")
			TEXT("re-encode), alpha (opacity 1), noclear (skip the clear before a redraw). ")
			TEXT("Usage: IAI.Bench.TexCorruptWrongCopy <none|chanswap|srgbtwice|mipshift|mipgen|texelshift|normal|alpha|noclear>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
			{
				static const TPair<const TCHAR*, EWrongCopy> Table[] = {
					{ TEXT("none"), EWrongCopy::None }, { TEXT("chanswap"), EWrongCopy::ChanSwap },
					{ TEXT("srgbtwice"), EWrongCopy::SrgbTwice }, { TEXT("mipshift"), EWrongCopy::MipShift },
					{ TEXT("mipgen"), EWrongCopy::MipGen }, { TEXT("texelshift"), EWrongCopy::TexelShift },
					{ TEXT("normal"), EWrongCopy::Normal }, { TEXT("alpha"), EWrongCopy::Alpha },
					{ TEXT("noclear"), EWrongCopy::NoClear } };
				if (Args.Num() >= 1)
				{
					for (const TPair<const TCHAR*, EWrongCopy>& Row : Table)
					{
						if (Args[0].Equals(Row.Key, ESearchCase::IgnoreCase))
						{
							Levers().WrongCopy = Row.Value;
							Echo(TEXT("IAI.Bench.TexCorruptWrongCopy"));
							return;
						}
					}
				}
				UE_LOG(LogAnomaly, Warning,
					TEXT("Usage: IAI.Bench.TexCorruptWrongCopy <none|chanswap|srgbtwice|mipshift|mipgen|texelshift|normal|alpha|noclear>"));
			}));

		FAutoConsoleCommand GIdentityCmd(
			TEXT("IAI.Bench.TexCorruptIdentity"),
			TEXT("BENCH DEVICE (m53 S1). Selects the identity mode for the next m53 fire: every output mip is the source's own ")
			TEXT("mip redrawn unchanged. It applies only to a fire with no mode argument; without it (or the tile probe, or identity ")
			TEXT("redraw) such a fire is refused mode_invalid:no_mode. Usage: IAI.Bench.TexCorruptIdentity <0|1>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
			{
				Levers().bIdentity = ParseInt(Args, 0) != 0;
				Echo(TEXT("IAI.Bench.TexCorruptIdentity"));
			}));

		FAutoConsoleCommand GIdentityRedrawCmd(
			TEXT("IAI.Bench.TexCorruptIdentityRedraw"),
			TEXT("BENCH DEVICE (m53 S1, G-RD). Identity with a FORCED redraw of every level of every output on every frame, each ")
			TEXT("draw preceded by its clear (skipped only by WrongCopy noclear). Scratch is kept for the event. ")
			TEXT("Usage: IAI.Bench.TexCorruptIdentityRedraw <0|1>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
			{
				Levers().bIdentityRedraw = ParseInt(Args, 0) != 0;
				Echo(TEXT("IAI.Bench.TexCorruptIdentityRedraw"));
			}));

		FAutoConsoleCommand GTileProbeCmd(
			TEXT("IAI.Bench.TexCorruptTileProbe"),
			TEXT("BENCH DEVICE (m53 S1, G-BIND only, plan Delta-3). A fixed x2 or x4 UV tile on uv_corruption, TARGETED FIRE ONLY ")
			TEXT("(ignored on an auto-pool pick), in no mode list and not in the pool. Output mip m holds source mip m+log2(N) ")
			TEXT("repeated NxN. Usage: IAI.Bench.TexCorruptTileProbe <0|2|4>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
			{
				const int32 V = ParseInt(Args, -1);
				if (V != 0 && V != 2 && V != 4)
				{
					UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.TexCorruptTileProbe <0|2|4>"));
					return;
				}
				Levers().TileProbe = V;
				Echo(TEXT("IAI.Bench.TexCorruptTileProbe"));
			}));

		FAutoConsoleCommand GForceMissingAssetCmd(
			TEXT("IAI.Bench.TexCorruptForceMissingAsset"),
			TEXT("BENCH DEVICE (m53, G-REASON E1). The decision tree treats the corruptor materials and the noise texture as ")
			TEXT("unresolved, so every fire refuses assets_unavailable. Usage: IAI.Bench.TexCorruptForceMissingAsset <0|1>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
			{
				Levers().bForceMissingAsset = ParseInt(Args, 0) != 0;
				Echo(TEXT("IAI.Bench.TexCorruptForceMissingAsset"));
			}));

		FAutoConsoleCommand GFailStepCmd(
			TEXT("IAI.Bench.TexCorruptFailStep"),
			TEXT("BENCH DEVICE (m53, G4). Fails transaction step <n> (2..6) ONCE on the next fire, to prove the rollback: no slot ")
			TEXT("touched, created bytes held in the two-frame pending ledger, the rest un-reserved, nothing leaked: the ")
			TEXT("TEXCORRUPT-LEDGER kind=rollback reading two rendered frames later reads live=0 pending=0. For step 2 an ")
			TEXT("optional <ordinal> fails that allocation in plan order AFTER creating its resource (a created-then-rejected ")
			TEXT("target); without it step 2 fails before any allocation. 0 disarms. ")
			TEXT("Usage: IAI.Bench.TexCorruptFailStep <0|2|3|4|5|6> [ordinal]"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
			{
				const int32 V = ParseInt(Args, -1);
				if (V != 0 && (V < 2 || V > 6))
				{
					UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.TexCorruptFailStep <0|2|3|4|5|6> [ordinal]"));
					return;
				}
				int32 Ordinal = -1;
				if (Args.Num() >= 2)
				{
					if (V != 2 || !Args[1].IsNumeric() || FCString::Atoi(*Args[1]) < 0)
					{
						UE_LOG(LogAnomaly, Warning, TEXT("IAI.Bench.TexCorruptFailStep: [ordinal] is a non-negative integer and applies to step 2 only."));
						return;
					}
					Ordinal = FCString::Atoi(*Args[1]);
				}
				Levers().FailStep = V;
				Levers().FailAllocOrdinal = Ordinal;
				Echo(TEXT("IAI.Bench.TexCorruptFailStep"));
			}));

		FAutoConsoleCommand GForeignReplaceCmd(
			TEXT("IAI.Bench.TexCorruptForeignReplace"),
			TEXT("BENCH DEVICE (m53, G4). Two ticks after the next Apply, sets a foreign material (the engine default) on the ")
			TEXT("first committed slot; the revert must leave it alone and log left-to-game. One-shot. ")
			TEXT("Usage: IAI.Bench.TexCorruptForeignReplace <0|1>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
			{
				Levers().bForeignReplace = ParseInt(Args, 0) != 0;
				Echo(TEXT("IAI.Bench.TexCorruptForeignReplace"));
			}));

		FAutoConsoleCommand GCollateralDetailCmd(
			TEXT("IAI.Bench.TexCorruptCollateralDetail"),
			TEXT("BENCH DEVICE (m53, G-COLL, plan fix B). Logs one TEXCORRUPT-COLL line per sample (Apply, every labelled frame, ")
			TEXT("2 frames after revert) listing every collateral texture's path, Apply-time and current resident level, so two legs ")
			TEXT("can be paired texture by texture. Usage: IAI.Bench.TexCorruptCollateralDetail <0|1>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
			{
				Levers().bCollateralDetail = ParseInt(Args, 0) != 0;
				Echo(TEXT("IAI.Bench.TexCorruptCollateralDetail"));
			}));

		FAutoConsoleCommand GCommitDelayCmd(
			TEXT("IAI.Bench.TexCorruptCommitDelay"),
			TEXT("BENCH DEVICE (m53 DC, B-M53 can-fail). Every m53 event still APPLIES and is labelled on its frame, but the slot ")
			TEXT("commit (the corrupted MID going onto the slots) runs <n> engine frames later, so the picture's first edge is <n> ")
			TEXT("frames late against the label. 0 = off (byte-identical). Usage: IAI.Bench.TexCorruptCommitDelay <0..60>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
			{
				const int32 V = ParseInt(Args, -1);
				if (V < 0 || V > 60)
				{
					UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.TexCorruptCommitDelay <0..60>"));
					return;
				}
				Levers().CommitDelay = V;
				Echo(TEXT("IAI.Bench.TexCorruptCommitDelay"));
				if (V > 0)
				{
					UE_LOG(LogAnomaly, Warning,
						TEXT("IAI.Bench.TexCorruptCommitDelay -> %d: every m53 picture edge at onset is now %d frame(s) LATE against its ")
						TEXT("label, on purpose. Never in a client payload."),
						V, V);
				}
			}));

		FAutoConsoleCommand GRestoreDelayCmd(
			TEXT("IAI.Bench.TexCorruptRestoreDelay"),
			TEXT("BENCH DEVICE (m53 DC, B-M53 can-fail). Every m53 revert still ends the event on its frame, but the slot restore ")
			TEXT("and the render-target release run <n> engine frames later, so the picture's last edge is <n> frames late against ")
			TEXT("the label. 0 = off (byte-identical). Usage: IAI.Bench.TexCorruptRestoreDelay <0..60>"),
			FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
			{
				const int32 V = ParseInt(Args, -1);
				if (V < 0 || V > 60)
				{
					UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.TexCorruptRestoreDelay <0..60>"));
					return;
				}
				Levers().RestoreDelay = V;
				Echo(TEXT("IAI.Bench.TexCorruptRestoreDelay"));
				if (V > 0)
				{
					UE_LOG(LogAnomaly, Warning,
						TEXT("IAI.Bench.TexCorruptRestoreDelay -> %d: every m53 picture edge at offset is now %d frame(s) LATE against ")
						TEXT("its label, on purpose. Never in a client payload."),
						V, V);
				}
			}));

		FAutoConsoleCommandWithWorldAndArgs GAssetSlotMidCmd(
			TEXT("IAI.Bench.TexCorruptAssetSlotMid"),
			TEXT("BENCH DEVICE (m53, plan R5 Delta-1). Writes a runtime MID into the material array of the named actor's static ")
			TEXT("MESH ASSET, refusing any mesh outside /Game/CaptureBenchTexCorrupt/, and marks its users render-state dirty. The ")
			TEXT("next m53 fire at it must refuse host_mid where=asset_slot. 0 restores. ")
			TEXT("Usage: IAI.Bench.TexCorruptAssetSlotMid <actor|0>"),
			FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
			{
				if (Args.Num() < 1)
				{
					UE_LOG(LogAnomaly, Warning, TEXT("Usage: IAI.Bench.TexCorruptAssetSlotMid <actor|0>"));
					return;
				}
				if (Args[0] == TEXT("0"))
				{
					RestoreBenchAssetSlotMid(TEXT("turned off"));
					return;
				}
				ApplyAssetSlotMid(World, Args[0]);
			}));

		FAutoConsoleCommandWithWorldAndArgs GCensusCmd(
			TEXT("IAI.Bench.TexCorruptCensus"),
			TEXT("m53 G0 eligibility census, READ-ONLY. Runs the decision tree for both families on the named actor, or on every ")
			TEXT("visible renderable actor, and logs every slot's Raw/Asset/Resolved/Effective material and every texture entry ")
			TEXT("with its disposition. It changes nothing and counts nothing into run_summary. ")
			TEXT("Usage: IAI.Bench.TexCorruptCensus [actor]"),
			FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
			{
				RunCensus(World, Args.Num() >= 1 ? FString(TEXT("=")) + Args[0] : FString());
			}));
	}
#endif
}
