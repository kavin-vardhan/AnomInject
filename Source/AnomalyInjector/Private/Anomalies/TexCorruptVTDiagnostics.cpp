#include "Anomalies/TexCorruptVTDiagnostics.h"
#include "Anomalies/TexCorruptCore.h"
#include "AnomalyInjectorLog.h"
#include "Components/MeshComponent.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "MaterialShared.h"
#include "Materials/MaterialInterface.h"
#include "PixelFormat.h"
#include "TextureResource.h"

namespace AnomalyTexCorrupt
{
	namespace
	{
		int32 FormatBit(EPixelFormat Format)
		{
			switch (Format)
			{
			case PF_BC5: return 1;
			case PF_DXT5: return 2;
			case PF_BC7: return 4;
			case PF_DXT1: return 8;
			case PF_B8G8R8A8: return 16;
			default: return 32;
			}
		}

		int32 AddressClass(TextureAddress Address)
		{
			switch (Address)
			{
			case TA_Wrap: return 1;
			case TA_Clamp: return 2;
			case TA_Mirror: return 3;
			default: return 0;
			}
		}

		struct FComponentReading
		{
			TArray<FString> Keys;
			bool bRefused = false;
			bool bUnassessed = false;
			bool bTouching = false;
			bool bFormatsComplete = true;
			int32 Required = 0;
			int32 Formats = 0;
		};

		FString Histogram(const TMap<int32, int64>& Counts, int32 First = 0, int32 Last = -1)
		{
			TArray<int32> Keys;
			if (Last >= First)
			{
				for (int32 Key = First; Key <= Last; ++Key) { Keys.Add(Key); }
			}
			else
			{
				Counts.GetKeys(Keys);
				Keys.Sort();
			}
			FString Text;
			for (int32 Key : Keys)
			{
				Text += FString::Printf(TEXT("%s%d:%lld"), Text.IsEmpty() ? TEXT("") : TEXT(","), Key, Counts.FindRef(Key));
			}
			return Text.IsEmpty() ? FString(TEXT("0:0")) : Text;
		}
	}

	void AccumulateVTDiagnostics(UWorld* World, const FTreeResult& Result, FVTDiagnostics& Counts)
	{
		if (!World) { return; }
		TMap<const UMeshComponent*, FComponentReading> Components;
		TArray<FString> SlotKeys;
		for (const FSlot& Slot : Result.Slots)
		{
			for (const FString& Key : Slot.AllKeys) { SlotKeys.AddUnique(Key); }
		}
		TArray<FString> GlobalKeys;
		for (const FString& Key : Result.EventKeys)
		{
			if (!SlotKeys.Contains(Key) && !Key.StartsWith(Why::PartialFootprint) && !Key.StartsWith(Why::NoEligibleSlot))
			{
				GlobalKeys.AddUnique(Key);
			}
		}
		for (const FSlot& Slot : Result.Slots)
		{
			if (Slot.bUntouched || !Slot.Comp) { continue; }
			FComponentReading& Component = Components.FindOrAdd(Slot.Comp);
			Component.bRefused |= !Slot.IsQualified() || GlobalKeys.Num() != 0;
			Component.bUnassessed |= Slot.bUnassessed;
			for (const FString& Key : GlobalKeys) { Component.Keys.AddUnique(Key); }
			for (const FString& Key : Slot.AllKeys) { Component.Keys.AddUnique(Key); }
			const FMaterialResource* Resource = Slot.Resolved ? Slot.Resolved->GetMaterialResource(World->FeatureLevel) : nullptr;
			const FMaterialShaderMap* Map = Resource ? Resource->GetGameThreadShaderMap() : nullptr;
			if (!Map)
			{
				++Counts.UnreadSlots;
				Component.bFormatsComplete = false;
				continue;
			}
			const FUniformExpressionSet& Set = Map->GetUniformExpressionSet();
			for (const FMaterialVirtualTextureStack& Stack : Set.GetVTStacks())
			{
				Component.bTouching = true;
				++Counts.StackLayers.FindOrAdd(static_cast<int32>(Stack.GetNumLayers()));
				if (Stack.IsPreallocatedStack())
				{
					++Counts.PreallocatedStacks;
					Component.bFormatsComplete = false;
				}
				else { ++Counts.StreamingStacks; }
			}
			const int32 Num = Set.GetNumTextures(EMaterialTextureParameterType::Virtual);
			for (int32 Index = 0; Index < Num; ++Index)
			{
				++Counts.Bindings;
				Component.bTouching = true;
				UTexture* Texture = nullptr;
				Set.GetGameThreadTextureValue(EMaterialTextureParameterType::Virtual, Index, Slot.Resolved, *Resource, Texture);
				const UTexture2D* Tex = Cast<UTexture2D>(Texture);
				const FTextureResource* TexResource = Tex ? Tex->GetResource() : nullptr;
				const FVirtualTexture2DResource* VT = TexResource ? TexResource->GetVirtualTexture2DResource() : nullptr;
				++Counts.Tiles.FindOrAdd(VT ? static_cast<int32>(VT->GetTileSize()) : 0);
				++Counts.Borders.FindOrAdd(VT ? static_cast<int32>(VT->GetBorderSize()) : -1);
				++Counts.AddressX.FindOrAdd(Tex ? AddressClass(Tex->AddressX) : 0);
				++Counts.AddressY.FindOrAdd(Tex ? AddressClass(Tex->AddressY) : 0);
				bool bRequired = Result.bProxyBlur || Result.Family == EFamily::UV || (Tex && Tex->IsNormalMap());
				for (const FBinding& Binding : Slot.Bindings)
				{
					if (Binding.Type == EMaterialTextureParameterType::Virtual && Binding.Index == Index)
					{
						bRequired = Binding.bRequired;
						break;
					}
				}
				if (!Tex) { Component.bFormatsComplete = false; }
				if (!bRequired) { continue; }
				++Counts.RequiredBindings;
				++Component.Required;
				if (Set.GetTextureParameter(EMaterialTextureParameterType::Virtual, Index).GetParameterName().IsNone())
				{
					Component.Keys.AddUnique(Why::TextureNotParameter);
				}
				if (!VT || VT->GetNumLayers() == 0)
				{
					Component.bFormatsComplete = false;
					++Counts.BindingFormats.FindOrAdd(0);
					continue;
				}
				int32 Formats = 0;
				for (uint32 Layer = 0; Layer < VT->GetNumLayers(); ++Layer)
				{
					const int32 Bit = FormatBit(VT->GetFormat(Layer));
					Formats |= Bit;
					++Counts.LayerFormats.FindOrAdd(Bit);
				}
				++Counts.BindingFormats.FindOrAdd(Formats);
				Component.Formats |= Formats;
			}
		}
		for (const TPair<const UMeshComponent*, FComponentReading>& Pair : Components)
		{
			const FComponentReading& Component = Pair.Value;
			++Counts.Components;
			Counts.Touching += Component.bTouching ? 1 : 0;
			if (Component.Required > 0)
			{
				++Counts.RequiredComponents;
				if (Component.bFormatsComplete && Component.Formats != 0)
				{
					++Counts.ComponentFormats.FindOrAdd(Component.Formats);
				}
				else { ++Counts.IncompleteFormats; }
			}
			bool bOnlyVT = Component.bRefused && Component.Keys.Num() > 0;
			bool bOnlyStack = bOnlyVT;
			for (const FString& Key : Component.Keys)
			{
				bOnlyVT &= Key == Why::VirtualTexture || Key.StartsWith(FString(Why::VirtualTexture) + TEXT("/"));
				bOnlyStack &= Key == CensusKey(Why::VirtualTexture, TEXT("stack_layout"));
			}
			Counts.OnlyVT += bOnlyVT ? 1 : 0;
			Counts.OnlyVTUnassessed += bOnlyVT && Component.bUnassessed ? 1 : 0;
			Counts.OnlyStack += bOnlyStack ? 1 : 0;
			Counts.OnlyStackUnassessed += bOnlyStack && Component.bUnassessed ? 1 : 0;
		}
	}

	void PrintVTDiagnostics(const FVTDiagnostics& C, int32 Family)
	{
		UE_LOG(LogAnomaly, Display, TEXT("IAI-VT-CENSUS v1 family=%d components=%lld vt_components=%lld bindings=%lld required_components=%lld required_bindings=%lld unread_slots=%lld"),
			Family, C.Components, C.Touching, C.Bindings, C.RequiredComponents, C.RequiredBindings, C.UnreadSlots);
		UE_LOG(LogAnomaly, Display, TEXT("IAI-VT-CENSUS v1 family=%d only_vt=%lld only_vt_unassessed=%lld only_stack=%lld only_stack_unassessed=%lld"),
			Family, C.OnlyVT, C.OnlyVTUnassessed, C.OnlyStack, C.OnlyStackUnassessed);
		UE_LOG(LogAnomaly, Display, TEXT("IAI-VT-CENSUS v1 family=%d streaming=%lld preallocated=%lld stacks=%s"),
			Family, C.StreamingStacks, C.PreallocatedStacks, *Histogram(C.StackLayers, 1, 8));
		UE_LOG(LogAnomaly, Display, TEXT("IAI-VT-CENSUS v1 family=%d layers_bc5=%lld layers_dxt5=%lld layers_bc7=%lld layers_dxt1=%lld layers_bgra8=%lld layers_other=%lld"),
			Family, C.LayerFormats.FindRef(1), C.LayerFormats.FindRef(2), C.LayerFormats.FindRef(4), C.LayerFormats.FindRef(8), C.LayerFormats.FindRef(16), C.LayerFormats.FindRef(32));
		UE_LOG(LogAnomaly, Display, TEXT("IAI-VT-CENSUS v1 family=%d binding_sets=%s component_sets=%s incomplete_components=%lld"),
			Family, *Histogram(C.BindingFormats), *Histogram(C.ComponentFormats), C.IncompleteFormats);
		UE_LOG(LogAnomaly, Display, TEXT("IAI-VT-CENSUS v1 family=%d tiles=%s borders=%s address_x=%s address_y=%s"),
			Family, *Histogram(C.Tiles), *Histogram(C.Borders), *Histogram(C.AddressX, 0, 3), *Histogram(C.AddressY, 0, 3));
	}
}
