#include "Anomalies/AnomalyPrivateVT.h"
#include "Engine/Texture2D.h"
#include "HAL/ThreadSafeCounter.h"
#include "RenderingThread.h"
#include "TextureResource.h"

namespace
{
	FThreadSafeCounter ResourceCount;
	FThreadSafeCounter ProducerCount;

	class FAnomalyPrivateVTResource final : public FVirtualTexture2DResource
	{
	public:
		FAnomalyPrivateVTResource(const UTexture2D* Source, FVirtualTextureBuiltData* Data, int32 FirstMip)
			: FVirtualTexture2DResource(Source, Data, FirstMip)
		{
			ResourceCount.Increment();
		}

		virtual ~FAnomalyPrivateVTResource() override
		{
			ResourceCount.Decrement();
		}

		virtual void InitRHI() override
		{
			FVirtualTexture2DResource::InitRHI();
			bCountedProducer = GetProducerHandle().IsValid();
			if (bCountedProducer) { ProducerCount.Increment(); }
		}

		virtual void ReleaseRHI() override
		{
			FVirtualTexture2DResource::ReleaseRHI();
			if (bCountedProducer) { ProducerCount.Decrement(); }
			bCountedProducer = false;
		}

#if WITH_EDITOR
		virtual void InitializeEditorResources(IVirtualTexture* InVirtualTexture) override {}
#endif

	private:
		bool bCountedProducer = false;
	};
}

bool UAnomalyPrivateVT::Initialize(UTexture2D* InSource, int32 InDrop, FString& OutReason)
{
	if (!InSource || !InSource->IsCurrentlyVirtualTextured() || InDrop < 0)
	{
		OutReason = TEXT("virtual_texture:source_type");
		return false;
	}
	FTexturePlatformData* Data = InSource->GetPlatformData();
	const FTextureResource* Resource = InSource->GetResource();
	const FVirtualTexture2DResource* VT = Resource ? Resource->GetVirtualTexture2DResource() : nullptr;
	if (!Data || !Data->VTData || !VT || !Resource->IsInitialized() || InSource->HasPendingInitOrStreaming(false))
	{
		OutReason = TEXT("virtual_texture:source_not_ready");
		return false;
	}
	const int32 SourceFirstMip = InSource->GetNumMips() - static_cast<int32>(VT->GetNumMips());
	const int32 MaximumDrop = FMath::CeilLogTwo(FMath::Max(VT->GetNumTilesX(), VT->GetNumTilesY()));
	if (SourceFirstMip < 0 || InDrop > MaximumDrop || VT->GetNumLayers() == 0 || VT->GetTileSize() == 0)
	{
		OutReason = TEXT("virtual_texture:invalid_layout");
		return false;
	}
	SourcePin.Reset(InSource);
	BorrowedData = Data->VTData;
	FirstMip = SourceFirstMip;
	Drop = InDrop;
	Width = FMath::Max(1, static_cast<int32>(VT->GetSizeX()) >> Drop);
	Height = FMath::Max(1, static_cast<int32>(VT->GetSizeY()) >> Drop);
	TileSize = VT->GetTileSize();
	AddressU = InSource->AddressX;
	AddressV = InSource->AddressY;
	SRGB = InSource->SRGB;
	Filter = InSource->Filter;
	LODGroup = InSource->LODGroup;
	CompressionSettings = InSource->CompressionSettings;
	VirtualTextureStreaming = true;
	NeverStream = true;
	UpdateResource();
	FRenderCommandFence Initialized;
	Initialized.BeginFence();
	Initialized.Wait();
	const FTextureResource* PrivateResource = GetResource();
	const FVirtualTexture2DResource* PrivateVT = PrivateResource ? PrivateResource->GetVirtualTexture2DResource() : nullptr;
	if (!PrivateVT || !PrivateVT->GetProducerHandle().IsValid() || PrivateVT->GetSizeX() != static_cast<uint32>(Width)
		|| PrivateVT->GetSizeY() != static_cast<uint32>(Height) || PrivateVT->GetNumLayers() != VT->GetNumLayers()
		|| PrivateVT->GetTileSize() != VT->GetTileSize() || PrivateVT->GetBorderSize() != VT->GetBorderSize())
	{
		OutReason = TEXT("virtual_texture:private_layout_mismatch");
		ReleasePrivate();
		return false;
	}
	for (uint32 Layer = 0; Layer < VT->GetNumLayers(); ++Layer)
	{
		if (PrivateVT->GetFormat(Layer) != VT->GetFormat(Layer))
		{
			OutReason = TEXT("virtual_texture:private_format_mismatch");
			ReleasePrivate();
			return false;
		}
	}
	return true;
}

FTextureResource* UAnomalyPrivateVT::CreateResource()
{
	return SourcePin.IsValid() && BorrowedData
		? new FAnomalyPrivateVTResource(SourcePin.Get(), BorrowedData, FirstMip + Drop) : nullptr;
}

bool UAnomalyPrivateVT::IsCurrentlyVirtualTextured() const
{
	const FTextureResource* Resource = GetResource();
	return Resource && Resource->GetVirtualTexture2DResource();
}

void UAnomalyPrivateVT::ReleasePrivate()
{
	if (GetResource())
	{
		ReleaseResource();
		FRenderCommandFence Released;
		Released.BeginFence();
		Released.Wait();
	}
	BorrowedData = nullptr;
	SourcePin.Reset();
}

void UAnomalyPrivateVT::BeginDestroy()
{
	ReleasePrivate();
	Super::BeginDestroy();
}

int32 UAnomalyPrivateVT::LiveResources() { return ResourceCount.GetValue(); }
int32 UAnomalyPrivateVT::LiveProducers() { return ProducerCount.GetValue(); }
