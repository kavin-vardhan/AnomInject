#include "AnomalyFrameCapturer.h"

#if ANOMALY_CAPTURE

#include "AnomalyCaptureLog.h"

#include "RenderingThread.h"
#include "RHICommandList.h"
#include "RHIResources.h"
#include "PixelFormat.h"
#include "CoreGlobals.h"
#include "HAL/IConsoleManager.h"

#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Rendering/SlateRenderer.h"

static int32 GAnomalyReadbackGuardInflateRows = 0;
static FAutoConsoleVariableRef GAnomalyReadbackGuardInflateCVar(
	TEXT("IAI.Bench.ReadbackGuardInflate"),
	GAnomalyReadbackGuardInflateRows,
	TEXT("BENCH KNOB, not a product setting - inflate by N rows the row count the readback bounds guard ")
	TEXT("checks against the mapped buffer height, so the guard can be proven to FIRE and DROP on demand. ")
	TEXT("It only ever causes DROPS: when it fires the row loop never runs, so it cannot produce a wrong ")
	TEXT("frame. A guard that has never fired is not a guard. Default 0. Compiled out of Shipping with the ")
	TEXT("rest of AnomalyCapture."),
	ECVF_Default);

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
			TEXT("bufferHeight=%d rowPitchInPixels=%d fmt=%d — THE INVARIANT AT THIS SITE: the readback is a ")
			TEXT("WHOLE-TEXTURE readback of a PLUGIN-OWNED picture-sized texture, so the drain indexes ")
			TEXT("sub-rect-locally (row y at y*rowPitch) and the sub-rect origin is applied in OUR copy pass, ")
			TEXT("NEVER in the drain. That is deliberate: engine readback staging layout DIFFERS BY UE VERSION ")
			TEXT("— 5.1 allocates FULL-SOURCE-SIZE staging and copies the sub-rect to ITS OWN POSITION ")
			TEXT("(RHIGPUReadback.cpp:156 and :172), while 5.2+ allocates RECT-SIZED staging and never sets ")
			TEXT("DestPosition so it lands at 0,0 (5.2.1 :158-165/:180-188, 5.3.2 :152-159/:187-204). No single ")
			TEXT("indexing is correct on both, and no version check is trustworthy on a fork, so we own the ")
			TEXT("texture instead. bufferHeight is therefore expected to equal PICTURE height on every engine ")
			TEXT("here BY CONSTRUCTION — it is no longer the layout discriminator. What still discriminates a ")
			TEXT("host is sourceExtent vs rect: rect inside sourceExtent means the view rect and the texture ")
			TEXT("agree, and an EXTENT-CLAMP line instead of this one means they do not. Emitted once per run, ")
			TEXT("from the first drained frame."),
			PathName, SourceExtent.X, SourceExtent.Y, Rect.Min.X, Rect.Min.Y, Rect.Max.X, Rect.Max.Y,
			Rect.Width(), Rect.Height(), BufferHeight, RowPitchInPixels, (int32)Format);
	}

	bool CheckDrainBounds(const TCHAR* PathName, uint64 RequestId, const FIntRect& Rect,
		int32 W, int32 H, int32 RowPitchInPixels, int32 BufferHeight, FThreadSafeCounter& DropCounter)
	{
		const int32 InflateRows = FMath::Max(0, GAnomalyReadbackGuardInflateRows);
		const int32 CheckedH = H + InflateRows;

		const bool bHeightOk = BufferHeight >= CheckedH;
		const bool bPitchOk = RowPitchInPixels >= W;
		if (bHeightOk && bPitchOk)
		{
			return true;
		}

		const int32 Drops = DropCounter.Increment();

		ensureMsgf(false,
			TEXT("Capture(%s): readback bounds guard FIRED id=%llu rect=(%d,%d)-(%d,%d) W=%d H=%d ")
			TEXT("checkedH=%d rowPitchInPixels=%d bufferHeight=%d inflateRows=%d"),
			PathName, RequestId, Rect.Min.X, Rect.Min.Y, Rect.Max.X, Rect.Max.Y, W, H,
			CheckedH, RowPitchInPixels, BufferHeight, InflateRows);

		UE_LOG(LogAnomalyCapture, Error,
			TEXT("Capture(%s): READBACK-GUARD FIRED — FRAME DROPPED, NOT WRITTEN (drop %d this run). ")
			TEXT("id=%llu rect=(%d,%d)-(%d,%d) W=%d H=%d checkedH=%d rowPitchInPixels=%d bufferHeight=%d ")
			TEXT("inflateRows=%d. heightOk=%d pitchOk=%d. The row loop would have read outside the mapped ")
			TEXT("readback buffer, which is an access violation, so the frame is dropped instead. A NON-ZERO ")
			TEXT("inflateRows means this was the bench knob IAI.Bench.ReadbackGuardInflate deliberately ")
			TEXT("provoking the guard — that is the guard's proof-by-breaking and NOT a defect. With ")
			TEXT("inflateRows=0 this line is a REAL fault: report it with these numbers."),
			PathName, Drops, RequestId, Rect.Min.X, Rect.Min.Y, Rect.Max.X, Rect.Max.Y, W, H,
			CheckedH, RowPitchInPixels, BufferHeight, InflateRows, bHeightOk ? 1 : 0, bPitchOk ? 1 : 0);

		return false;
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
	const FIntRect Requested = Arm.Rect;
	FIntRect Rect = Requested;
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

	if (Rect != Requested)
	{
		const int32 Drops = ClampDrops.Increment();
		UE_LOG(LogAnomalyCapture, Error,
			TEXT("Capture(async): EXTENT-CLAMP FIRED — FRAME DROPPED, NOT WRITTEN (clamp drop %d this run). ")
			TEXT("id=%llu requested rect=(%d,%d)-(%d,%d) but the backbuffer is only %dx%d, so the requested ")
			TEXT("region is NOT INSIDE THE SOURCE. Capturing the clamped region instead would silently ")
			TEXT("deliver a DIFFERENT picture than the label describes, so the frame is dropped. The view rect ")
			TEXT("and the source texture disagree about their coordinate space — report these numbers."),
			Drops, Arm.RequestId, Requested.Min.X, Requested.Min.Y, Requested.Max.X, Requested.Max.Y, BBW, BBH);
		return;
	}

	const int32 W = Rect.Width();
	const int32 H = Rect.Height();
	const EPixelFormat SrcFormat = BackBuffer->GetFormat();

	FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();

	if (!OwnSubRect.IsValid() || OwnSize != FIntPoint(W, H) || OwnFormat != SrcFormat)
	{
		const FRHITextureCreateDesc OwnDesc =
			FRHITextureCreateDesc::Create2D(TEXT("AnomalyBackbufferSubRect"), W, H, SrcFormat)
			.SetFlags(ETextureCreateFlags::ShaderResource)
			.SetInitialState(ERHIAccess::CopyDest);
		OwnSubRect = RHICreateTexture(OwnDesc);
		OwnSize = FIntPoint(W, H);
		OwnFormat = SrcFormat;

		UE_LOG(LogAnomalyCapture, Log,
			TEXT("Capture(async): plugin-owned sub-rect texture (re)created %dx%d fmt=%d — the readback is a ")
			TEXT("WHOLE-TEXTURE readback of this, so the drain never applies a sub-rect origin. Flags here are ")
			TEXT("ShaderResource with an explicit CopyDest initial state, DELIBERATELY WITHOUT ")
			TEXT("RenderTargetable, and that differs on purpose from the SVE path's RDG texture, which MUST ")
			TEXT("carry RenderTargetable: an RDG texture is allocated TRANSIENTLY and the D3D12 transient ")
			TEXT("allocator derives its initial state only from ALLOW_RENDER_TARGET / ALLOW_DEPTH_STENCIL / ")
			TEXT("ALLOW_UNORDERED_ACCESS, asserting on a ShaderResource-only texture ")
			TEXT("(D3D12TransientResourceAllocator.cpp:8-26; the flag translation is D3D12Texture.cpp:557-560). ")
			TEXT("This texture is committed, never transient, so it does not need the flag and does not take ")
			TEXT("the requires-initialization path that comes with it."),
			W, H, (int32)SrcFormat);
	}

	if (!OwnSubRect.IsValid())
	{
		UE_LOG(LogAnomalyCapture, Error,
			TEXT("Capture(async): could not create the plugin-owned %dx%d fmt=%d sub-rect texture for frame ")
			TEXT("id=%llu — FRAME DROPPED, NOT WRITTEN. No fallback to a sub-rect readback exists on purpose: ")
			TEXT("that path is only correct on some engine versions."),
			W, H, (int32)SrcFormat, Arm.RequestId);
		return;
	}

	FRHICopyTextureInfo CopyInfo;
	CopyInfo.SourcePosition = FIntVector(Rect.Min.X, Rect.Min.Y, 0);
	CopyInfo.DestPosition = FIntVector(0, 0, 0);
	CopyInfo.Size = FIntVector(W, H, 1);
	RHICmdList.CopyTexture(BackBuffer.GetReference(), OwnSubRect.GetReference(), CopyInfo);

	RHICmdList.Transition(FRHITransitionInfo(OwnSubRect.GetReference(), ERHIAccess::CopyDest, ERHIAccess::CopySrc));

	TUniquePtr<FRHIGPUTextureReadback> Readback = MakeUnique<FRHIGPUTextureReadback>(TEXT("AnomalyColorReadback"));
	Readback->EnqueueCopy(RHICmdList, OwnSubRect.GetReference());

	RHICmdList.Transition(FRHITransitionInfo(OwnSubRect.GetReference(), ERHIAccess::CopySrc, ERHIAccess::CopyDest));

	FInFlight Item;
	Item.RequestId = Arm.RequestId;
	Item.Readback = MoveTemp(Readback);
	Item.Rect = Rect;
	Item.SourceExtent = FIntPoint(BBW, BBH);
	Item.Format = SrcFormat;
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

			if (!AnomalyReadback::CheckDrainBounds(*PathName, Item.RequestId, Item.Rect, W, H,
				RowPitchInPixels, BufferHeight, GuardDrops))
			{
				Item.Readback->Unlock();
				InFlight.RemoveAt(i);
				continue;
			}

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
				const uint8* SrcRow = Base + (int64)y * RowPitchInPixels * BPP;
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
