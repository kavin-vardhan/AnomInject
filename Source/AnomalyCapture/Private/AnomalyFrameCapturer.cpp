#include "AnomalyFrameCapturer.h"

#if ANOMALY_CAPTURE

#include "AnomalyCaptureLog.h"

#include "RenderingThread.h"
#include "RHICommandList.h"
#include "RHIResources.h"
#include "PixelFormat.h"
#include "CoreGlobals.h"

#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Rendering/SlateRenderer.h"

namespace AnomalyReadback
{
	void NoteLayoutOnce(FCriticalSection& Guard, FAnomalyReadbackLayout& Layout, const TCHAR* PathName,
		const FIntPoint& SourceExtent, const FIntRect& Rect, int32 RowPitchInPixels, int32 BufferHeight,
		EPixelFormat Format)
	{
		{
			FScopeLock Lock(&Guard);
			if (Layout.bValid)
			{
				return;
			}
			Layout.bValid = true;
			Layout.SourceExtent = SourceExtent;
			Layout.Rect = Rect;
			Layout.W = Rect.Width();
			Layout.H = Rect.Height();
			Layout.BufferHeight = BufferHeight;
			Layout.RowPitchInPixels = RowPitchInPixels;
			Layout.Format = (int32)Format;
		}

		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(%s): READBACK-LAYOUT sourceExtent=%dx%d rect=(%d,%d)-(%d,%d) picture=%dx%d ")
			TEXT("bufferHeight=%d rowPitchInPixels=%d fmt=%d — engine readback staging layout differs by UE ")
			TEXT("version, so read it, never assume it: UE 5.1 allocates FULL-SOURCE-SIZE staging and copies ")
			TEXT("the sub-rect to ITS OWN POSITION (expect bufferHeight == sourceExtent.y), while UE 5.2+ ")
			TEXT("allocates RECT-SIZED staging and copies to 0,0 (expect bufferHeight == picture height). ")
			TEXT("This line is emitted once per run, from the first drained frame."),
			PathName, SourceExtent.X, SourceExtent.Y, Rect.Min.X, Rect.Min.Y, Rect.Max.X, Rect.Max.Y,
			Rect.Width(), Rect.Height(), BufferHeight, RowPitchInPixels, (int32)Format);
	}
}

FAnomalyFrameCapturer::~FAnomalyFrameCapturer()
{
	UnregisterBackbufferHook();
}

FAnomalyReadbackLayout FAnomalyFrameCapturer::GetReadbackLayout() const
{
	FScopeLock Lock(&LayoutCS);
	return Layout;
}

void FAnomalyFrameCapturer::ResetReadbackLayout()
{
	FScopeLock Lock(&LayoutCS);
	Layout = FAnomalyReadbackLayout();
}

void FAnomalyFrameCapturer::SetLayoutPathName(const FString& InName)
{
	FScopeLock Lock(&LayoutCS);
	LayoutPathName = InName;
}

void FAnomalyFrameCapturer::RegisterBackbufferHook()
{
	if (BackBufferHandle.IsValid() || !FSlateApplication::IsInitialized())
	{
		return;
	}
	if (FSlateRenderer* Renderer = FSlateApplication::Get().GetRenderer())
	{
		BackBufferHandle = Renderer->OnBackBufferReadyToPresent().AddRaw(
			this, &FAnomalyFrameCapturer::OnBackBufferReadyToPresent_RenderThread);
	}
}

void FAnomalyFrameCapturer::UnregisterBackbufferHook()
{
	if (BackBufferHandle.IsValid() && FSlateApplication::IsInitialized())
	{
		if (FSlateRenderer* Renderer = FSlateApplication::Get().GetRenderer())
		{
			Renderer->OnBackBufferReadyToPresent().Remove(BackBufferHandle);
		}
	}
	BackBufferHandle.Reset();

	FlushRenderingCommands();
}

void FAnomalyFrameCapturer::ArmForCapture(uint64 RequestId, SWindow* TargetWindow, const FIntRect& CaptureRect)
{
	TWeakPtr<FAnomalyFrameCapturer, ESPMode::ThreadSafe> WeakSelf = AsShared();
	FArm Arm;
	Arm.RequestId = RequestId;
	Arm.Window = TargetWindow;
	Arm.Rect = CaptureRect;

	ENQUEUE_RENDER_COMMAND(AnomalyCaptureArmRegister)(
		[WeakSelf, Arm](FRHICommandListImmediate&) mutable
		{
			if (TSharedPtr<FAnomalyFrameCapturer, ESPMode::ThreadSafe> Self = WeakSelf.Pin())
			{
				Arm.RegisteredRtFrame = GFrameNumberRenderThread;
				FScopeLock Lock(&Self->StateCS);
				Self->PendingArms.Add(Arm);
			}
		});
}

int32 FAnomalyFrameCapturer::NumPendingApprox() const
{
	FScopeLock Lock(&StateCS);
	return PendingArms.Num();
}

void FAnomalyFrameCapturer::OnBackBufferReadyToPresent_RenderThread(SWindow& SlateWindow, const FTexture2DRHIRef& BackBuffer)
{
	FArm Arm;
	bool bHaveArm = false;
	{
		FScopeLock Lock(&StateCS);
		if (PendingArms.Num() > 0 && PendingArms[0].Window == &SlateWindow)
		{
			Arm = PendingArms[0];
			PendingArms.RemoveAt(0);
			bHaveArm = true;
		}
	}

	if (!bHaveArm || !BackBuffer.IsValid())
	{
		return;
	}

	const int32 BBW = (int32)BackBuffer->GetSizeX();
	const int32 BBH = (int32)BackBuffer->GetSizeY();
	FIntRect Rect = Arm.Rect;
	Rect.Min.X = FMath::Clamp(Rect.Min.X, 0, BBW);
	Rect.Min.Y = FMath::Clamp(Rect.Min.Y, 0, BBH);
	Rect.Max.X = FMath::Clamp(Rect.Max.X, 0, BBW);
	Rect.Max.Y = FMath::Clamp(Rect.Max.Y, 0, BBH);
	if (Rect.Width() <= 0 || Rect.Height() <= 0)
	{
		UE_LOG(LogAnomalyCapture, Warning, TEXT("Capture(async): empty viewport rect for frame id=%llu (backbuffer %dx%d) — skipped."),
			Arm.RequestId, BBW, BBH);
		return;
	}

	TUniquePtr<FRHIGPUTextureReadback> Readback = MakeUnique<FRHIGPUTextureReadback>(TEXT("AnomalyColorReadback"));

	FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();
	Readback->EnqueueCopy(RHICmdList, BackBuffer.GetReference(),
		FResolveRect(Rect.Min.X, Rect.Min.Y, Rect.Max.X, Rect.Max.Y));

	FInFlight Item;
	Item.RequestId = Arm.RequestId;
	Item.Readback = MoveTemp(Readback);
	Item.Rect = Rect;
	Item.SourceExtent = FIntPoint(BBW, BBH);
	Item.Format = BackBuffer->GetFormat();
	InFlight.Add(MoveTemp(Item));

	UE_LOG(LogAnomalyCapture, Log,
		TEXT("Capture(async): armed frame id=%llu submitted (rtframe=%u, armRt=%u, fmt=%d, rect=%dx%d)."),
		Arm.RequestId, GFrameNumberRenderThread, Arm.RegisteredRtFrame, (int32)Item.Format, Rect.Width(), Rect.Height());
}

void FAnomalyFrameCapturer::EnqueueDrain()
{
	TWeakPtr<FAnomalyFrameCapturer, ESPMode::ThreadSafe> WeakSelf = AsShared();

	ENQUEUE_RENDER_COMMAND(AnomalyCaptureDrain)(
		[WeakSelf](FRHICommandListImmediate& RHICmdList)
		{
			if (TSharedPtr<FAnomalyFrameCapturer, ESPMode::ThreadSafe> Self = WeakSelf.Pin())
			{
				Self->Drain_RenderThread();
			}
		});
}

void FAnomalyFrameCapturer::Drain_RenderThread()
{
	for (int32 i = InFlight.Num() - 1; i >= 0; --i)
	{
		FInFlight& Item = InFlight[i];
		if (!Item.Readback.IsValid() || !Item.Readback->IsReady())
		{
			continue;
		}

		int32 RowPitchInPixels = 0;
		int32 BufferHeight = 0;
		void* Src = Item.Readback->Lock(RowPitchInPixels, &BufferHeight);
		if (Src && RowPitchInPixels > 0)
		{
			const int32 W = Item.Rect.Width();
			const int32 H = Item.Rect.Height();
			const int32 BPP = GPixelFormats[Item.Format].BlockBytes;

			FString PathName;
			{
				FScopeLock NameLock(&LayoutCS);
				PathName = LayoutPathName;
			}
			AnomalyReadback::NoteLayoutOnce(LayoutCS, Layout, *PathName, Item.SourceExtent, Item.Rect,
				RowPitchInPixels, BufferHeight, Item.Format);

			FAnomalyCapturedFrame Frame;
			Frame.RequestId = Item.RequestId;
			Frame.Width = W;
			Frame.Height = H;
			Frame.Format = Item.Format;
			Frame.BytesPerPixel = BPP;
			Frame.RawBytes.SetNumUninitialized((int64)W * H * BPP);

			const uint8* Base = static_cast<const uint8*>(Src);
			for (int32 y = 0; y < H; ++y)
			{
				const uint8* SrcRow = Base + ((int64)(Item.Rect.Min.Y + y) * RowPitchInPixels + Item.Rect.Min.X) * BPP;
				FMemory::Memcpy(Frame.RawBytes.GetData() + (int64)y * W * BPP, SrcRow, (int64)W * BPP);
			}

			Item.Readback->Unlock();

			{
				FScopeLock Lock(&CompletedCS);
				Completed.Add(MoveTemp(Frame));
			}
		}
		else if (Src)
		{
			Item.Readback->Unlock();
		}

		InFlight.RemoveAt(i);
	}
}

bool FAnomalyFrameCapturer::PopCompleted(FAnomalyCapturedFrame& Out)
{
	FScopeLock Lock(&CompletedCS);
	if (Completed.Num() == 0)
	{
		return false;
	}
	Out = MoveTemp(Completed[0]);
	Completed.RemoveAt(0);
	return true;
}

#endif
