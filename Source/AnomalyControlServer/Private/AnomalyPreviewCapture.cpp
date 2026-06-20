// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "AnomalyPreviewCapture.h"

#include "AnomalyControlServerLog.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/SlateRenderer.h"
#include "Widgets/SWindow.h"
#include "RHICommandList.h"
#include "RenderingThread.h"
#include "Modules/ModuleManager.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

FAnomalyPreviewCapture::~FAnomalyPreviewCapture()
{
	Stop();
}

void FAnomalyPreviewCapture::Start()
{
	if (bRunning)
	{
		return;
	}
	if (!FSlateApplication::IsInitialized())
	{
		UE_LOG(LogAnomalyServer, Warning, TEXT("Preview: Slate not initialized — no preview frames this session."));
		return;
	}
	FSlateRenderer* Renderer = FSlateApplication::Get().GetRenderer();
	if (!Renderer)
	{
		UE_LOG(LogAnomalyServer, Warning, TEXT("Preview: no Slate renderer — no preview frames."));
		return;
	}
	Handle = Renderer->OnBackBufferReadyToPresent().AddRaw(this, &FAnomalyPreviewCapture::OnBackBufferReady);
	bRunning = true;
	UE_LOG(LogAnomalyServer, Log, TEXT("Preview: backbuffer capture armed (OnBackBufferReadyToPresent)."));
}

void FAnomalyPreviewCapture::Stop()
{
	if (!bRunning)
	{
		return;
	}
	if (FSlateApplication::IsInitialized())
	{
		if (FSlateRenderer* Renderer = FSlateApplication::Get().GetRenderer())
		{
			Renderer->OnBackBufferReadyToPresent().Remove(Handle);
		}
	}
	Handle.Reset();
	bRunning = false;

	// Ensure no in-flight render-thread callback is mid-execution before we (potentially) get destroyed.
	FlushRenderingCommands();

	FScopeLock Lock(&Mutex);
	PendingPixels.Empty();
	bHasPending = false;
}

void FAnomalyPreviewCapture::RequestCapture(TWeakPtr<SWindow> InTargetWindow)
{
	// Set the target before arming the request. A benign race remains (the render thread may read the
	// previous target for one frame); acceptable for the spike.
	TargetWindow = InTargetWindow;
	bCaptureRequested = true;
}

void FAnomalyPreviewCapture::OnBackBufferReady(SWindow& Window, const FTexture2DRHIRef& BackBuffer)
{
	// RENDER THREAD. Only grab when the game thread has requested a frame.
	if (!bCaptureRequested)
	{
		return;
	}

	// Window targeting: if we have a valid target (the game viewport window), only grab that window's
	// backbuffer so we never capture an editor window. If the target couldn't be resolved, accept any.
	TSharedPtr<SWindow> Target = TargetWindow.Pin();
	if (Target.IsValid() && Target.Get() != &Window)
	{
		return; // wait for the game window to present
	}

	if (!BackBuffer.IsValid())
	{
		return;
	}

	FRHITexture2D* Tex2D = BackBuffer->GetTexture2D();
	if (!Tex2D)
	{
		return;
	}

	// Consume the request only once we have a usable backbuffer to read.
	bCaptureRequested = false;

	const FIntPoint Size = Tex2D->GetSizeXY();
	if (Size.X <= 0 || Size.Y <= 0)
	{
		return;
	}

	FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();
	TArray<FColor> Pixels;
	RHICmdList.ReadSurfaceData(
		BackBuffer.GetReference(),
		FIntRect(0, 0, Size.X, Size.Y),
		Pixels,
		FReadSurfaceDataFlags(RCM_UNorm, CubeFace_PosX));

	if (Pixels.Num() == 0)
	{
		return;
	}

	FScopeLock Lock(&Mutex);
	PendingPixels = MoveTemp(Pixels);
	PendingSize = Size;
	bHasPending = true;
}

bool FAnomalyPreviewCapture::TakeEncodedJpeg(TArray<uint8>& OutJpeg, int32& OutWidth, int32& OutHeight, int32 Quality)
{
	TArray<FColor> Pixels;
	FIntPoint Size = FIntPoint::ZeroValue;
	{
		FScopeLock Lock(&Mutex);
		if (!bHasPending)
		{
			return false;
		}
		Pixels = MoveTemp(PendingPixels);
		Size = PendingSize;
		bHasPending = false;
	}

	if (Pixels.Num() == 0 || Size.X <= 0 || Size.Y <= 0)
	{
		return false;
	}

	IImageWrapperModule& Module = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
	TSharedPtr<IImageWrapper> Wrapper = Module.CreateImageWrapper(EImageFormat::JPEG);
	if (!Wrapper.IsValid())
	{
		return false;
	}

	// Backbuffer readback is BGRA8. JPEG has no alpha channel, so a zero alpha in the backbuffer is harmless.
	if (!Wrapper->SetRaw(Pixels.GetData(), (int64)Pixels.Num() * sizeof(FColor), Size.X, Size.Y, ERGBFormat::BGRA, 8))
	{
		return false;
	}

	const TArray64<uint8>& Compressed = Wrapper->GetCompressed(Quality);
	if (Compressed.Num() == 0)
	{
		return false;
	}

	OutJpeg.SetNumUninitialized(Compressed.Num());
	FMemory::Memcpy(OutJpeg.GetData(), Compressed.GetData(), Compressed.Num());
	OutWidth = Size.X;
	OutHeight = Size.Y;
	return true;
}
