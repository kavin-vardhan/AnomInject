#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "UObject/StrongObjectPtr.h"
#include "Templates/SharedPointer.h"
#include "RHIDefinitions.h"
#include "AnomalyPrivateVT.generated.h"

class FTextureResource;
struct FAnomalyVTPrefetch;
class UMaterialInstanceDynamic;

UCLASS(Transient)
class UAnomalyPrivateVT final : public UTexture
{
	GENERATED_BODY()

public:
	bool Initialize(UTexture2D* InSource, int32 InDrop, FString& OutReason);
	bool PollPrefetch(FString& OutReason);
	int32 PrefetchAttempts() const;
	int32 PrefetchPages() const;
	void ReleasePrivate();
	virtual FTextureResource* CreateResource() override;
	virtual ETextureClass GetTextureClass() const override { return ETextureClass::TwoD; }
	virtual EMaterialValueType GetMaterialType() const override { return MCT_TextureVirtual; }
	virtual bool IsCurrentlyVirtualTextured() const override;
	virtual float GetSurfaceWidth() const override { return Width; }
	virtual float GetSurfaceHeight() const override { return Height; }
	virtual float GetSurfaceDepth() const override { return 0; }
	virtual uint32 GetSurfaceArraySize() const override { return 0; }
	virtual TextureAddress GetTextureAddressX() const override { return AddressU; }
	virtual TextureAddress GetTextureAddressY() const override { return AddressV; }
	virtual void BeginDestroy() override;
	int32 GetFirstMip() const { return FirstMip; }
	int32 GetDrop() const { return Drop; }
	int32 GetTileSize() const { return TileSize; }
	static int32 LiveResources();
	static int32 LiveProducers();
#if !UE_BUILD_SHIPPING
	void BenchSkipRequests() { bSkipRequests = true; }
#endif

private:
	TStrongObjectPtr<UTexture2D> SourcePin;
	TSharedPtr<FAnomalyVTPrefetch, ESPMode::ThreadSafe> Prefetch;
	struct FVirtualTextureBuiltData* BorrowedData = nullptr;
	int32 FirstMip = 0;
	int32 Drop = 0;
	int32 Width = 0;
	int32 Height = 0;
	int32 TileSize = 0;
	TextureAddress AddressU = TA_Wrap;
	TextureAddress AddressV = TA_Wrap;
#if !UE_BUILD_SHIPPING
	bool bSkipRequests = false;
#endif
};

bool AnomalyPrefetchVTMaterial(UMaterialInstanceDynamic* Material, const TArray<UAnomalyPrivateVT*>& Targets,
	ERHIFeatureLevel::Type FeatureLevel, FString& OutReason);

#if !UE_BUILD_SHIPPING
bool AnomalyProxyVTBenchNoBind();
#endif
