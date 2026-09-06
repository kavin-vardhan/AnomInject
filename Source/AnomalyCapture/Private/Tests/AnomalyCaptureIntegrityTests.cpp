#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && ANOMALY_CAPTURE
#include "AnomalyAsyncWriter.h"
#include "AnomalyLabelWriter.h"
#include "AnomalyMaskMeasure.h"
#include "AnomalyStencilTag.h"
#include "AnomalySveKeyRing.h"
#include "AnomalySveCapturer.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

namespace
{
constexpr uint32 Flags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;
TSharedPtr<FJsonObject> Parse(const FString& Text)
{
	TSharedPtr<FJsonObject> Result;
	FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Result);
	return Result;
}
FAnomalyAsyncWriter::FJob TinyFrame(const FString& Dir, int32 Index)
{
	FAnomalyAsyncWriter::FJob Job;
	Job.OutputDir = Dir; Job.SessionIndex = Index;
	Job.Width = Job.Height = Job.OutWidth = Job.OutHeight = 2;
	Job.SrcFormat = PF_B8G8R8A8; Job.BytesPerPixel = 4;
	Job.RawBytes.Init(255, 16);
	Job.ImageRelPath = FString::Printf(TEXT("Actual_Frames/frame_%05d.png"), Index);
	Job.Record = TEXT("{}");
	return Job;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVisibilityContractTest, "AnomalyInjector.CaptureIntegrity.VisibilityContract", Flags)
bool FVisibilityContractTest::RunTest(const FString&)
{
	AnomalyLabel::FCaptureSnapshot Snapshot;
	FAutoLiveFireInfo Fire; Fire.Id = TEXT("missing_texture"); Fire.Target = TEXT("destroyed-target");
	Snapshot.Fires.Add(Fire);
	Snapshot.FireLabelled.Add(1);
	AnomalyLabel::FTargetGeometry Geometry;
	Geometry.bRectValid = true; Geometry.ScreenMin = FVector2D(.2, .3); Geometry.ScreenMax = FVector2D(.4, .5);
	Snapshot.TargetGeometry.Add(Geometry);
	int32 PositiveCount = 0;
	for (uint8 State : {uint8(0), uint8(1), uint8(2)})
	{
		Snapshot.Observable = {State};
		const auto Row = Parse(AnomalyLabel::BuildLabelRecordForSnapshot(Snapshot, 100, 100, TEXT("frame.png"), PositiveCount));
		if (!TestTrue(TEXT("JSON parsed"), Row.IsValid())) { return false; }
		TestEqual(TEXT("Only proven observability is positive"), Row->GetBoolField(TEXT("anomaly_present")), State == 1);
		TestEqual(TEXT("Positive count agrees with row"), PositiveCount, State == 1 ? 1 : 0);
		const auto Target = Row->GetArrayField(TEXT("anomalies"))[0]->AsObject();
		TestTrue(TEXT("Frozen box survives absent actor"), Target->GetBoolField(TEXT("bbox_valid")));
		TestEqual(TEXT("Frozen X coordinate"), Target->GetArrayField(TEXT("bbox_px"))[0]->AsNumber(), 20.0);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPixelLayoutTest, "AnomalyInjector.CaptureIntegrity.PixelLayout", Flags)
bool FPixelLayoutTest::RunTest(const FString&)
{
	TArray<uint8> Bytes; Bytes.Init(255, 16);
	TArray<FColor> Pixels;
	AnomalyLabel::ConvertTightToBGRA(PF_Unknown, 4, Bytes, 2, 2, Pixels);
	TestEqual(TEXT("Unsupported format cannot become a black image"), Pixels.Num(), 0);
	AnomalyLabel::ConvertTightToBGRA(PF_B8G8R8A8, 8, Bytes, 2, 2, Pixels);
	TestEqual(TEXT("Wrong stride rejected"), Pixels.Num(), 0);
	Bytes.SetNum(15);
	AnomalyLabel::ConvertTightToBGRA(PF_B8G8R8A8, 4, Bytes, 2, 2, Pixels);
	TestEqual(TEXT("Truncated source rejected"), Pixels.Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWriterCommitTest, "AnomalyInjector.CaptureIntegrity.WriterCommit", Flags)
bool FWriterCommitTest::RunTest(const FString&)
{
	const FString Dir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation/AnomalyIntegrity"), FGuid::NewGuid().ToString());
	auto Writer = MakeShared<FAnomalyAsyncWriter, ESPMode::ThreadSafe>();
	auto Job = TinyFrame(Dir, 7);
	Job.MaskRelPath = TEXT("target_mask/frame_00007.png"); Job.MaskBytes = {200, 0, 0, 0};
	TestTrue(TEXT("Job admitted"), Writer->Enqueue(MoveTemp(Job)));
	TestTrue(TEXT("Writer joined"), Writer->FlushPending(10.0));
	TestTrue(TEXT("Frame committed"), Writer->GetCommittedFrames().Contains(7));
	TestTrue(TEXT("Onset mask exists before commit"), IFileManager::Get().FileExists(*FPaths::Combine(Dir, TEXT("target_mask/frame_00007.png"))));

	const FString BadDir = FPaths::Combine(Dir, TEXT("bad"));
	IFileManager::Get().MakeDirectory(*FPaths::Combine(BadDir, TEXT("labels.jsonl")), true);
	Writer->Enqueue(TinyFrame(BadDir, 8));
	TestTrue(TEXT("Failed job joined"), Writer->FlushPending(10.0));
	TestFalse(TEXT("Label append failure cannot commit frame"), Writer->GetCommittedFrames().Contains(8));
	TestEqual(TEXT("Failed frame counted"), Writer->GetDropped(), 1);
	TestEqual(TEXT("Successful frame count unchanged"), Writer->GetFramesWritten(), 1);
	for (int32 Index = 20; Index < 40; ++Index)
	{
		TestTrue(TEXT("Burst exceeding queue capacity applies backpressure"), Writer->Enqueue(TinyFrame(Dir, Index)));
	}
	TestTrue(TEXT("Burst writer joined"), Writer->FlushPending(10.0));
	TestEqual(TEXT("Burst preserves every frame"), Writer->GetFramesWritten(), 21);
	Writer->FlushPending(-1.0);
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTagRetirementTest, "AnomalyInjector.CaptureIntegrity.TagRetirement", Flags)
bool FTagRetirementTest::RunTest(const FString&)
{
	FAnomalyStencilTagLedger Ledger;
	FAnomalyMaskMeasure Measure;
	Measure.BeginRun(&Ledger);
	for (int32 I = 0; I < 200; ++I)
	{
		const auto* Record = Measure.FindOrAddRecord(TEXT("missing_texture"), TEXT("target"), I + 1, nullptr);
		TestTrue(TEXT("Sequential events keep receiving measurable IDs past event 55"), Record->Tag >= 200 && Record->Tag <= 254);
		Measure.RetireInactiveRecords({});
		TestEqual(TEXT("Retired lease returned"), Ledger.EventClaimed.Num(), 0);
	}
	TestEqual(TEXT("Retirement preserves event history"), Measure.GetRecords().Num(), 200);
	Measure.EndRun();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSingleUseRenderKeyTest, "AnomalyInjector.CaptureIntegrity.SingleUseRenderKey", Flags)
bool FSingleUseRenderKeyTest::RunTest(const FString&)
{
	AnomalySveKeyRing::Reset();
	AnomalySveKeyRing::PublishKey(1234, 10, true);
	AnomalySveKeyRing::FKeyEntry Entry;
	TestTrue(TEXT("First view consumes capture key"), AnomalySveKeyRing::LookupKey(1234, Entry));
	TestFalse(TEXT("Second callback cannot duplicate capture"), AnomalySveKeyRing::LookupKey(1234, Entry));
	AnomalySveKeyRing::Reset();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FStaleArmTest, "AnomalyInjector.CaptureIntegrity.StaleArm", Flags)
bool FStaleArmTest::RunTest(const FString&)
{
	FAnomalySveCapturer Capturer;
	Capturer.ArmWanted(42);
	uint64 Request = 0;
	TestFalse(TEXT("A later render must not consume an earlier tick's label"), Capturer.ConsumeWantedForPublish(1, Request, GFrameCounter + 1));
	Capturer.ArmWanted(43);
	TestTrue(TEXT("Same-tick render consumes its label"), Capturer.ConsumeWantedForPublish(2, Request, GFrameCounter));
	TestEqual(TEXT("Fresh request identity preserved"), Request, uint64(43));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProjectionClippingTest, "AnomalyInjector.CaptureIntegrity.ProjectionClipping", Flags)
bool FProjectionClippingTest::RunTest(const FString&)
{
	FAnomalyViewInfo View;
	View.bValid = true; View.bHasProjectionMatrix = true;
	// An orthographic reversed-Z view: identity clips z > 1 at its near plane.
	View.ViewProjectionMatrix = FMatrix::Identity;
	FVector2D Min, Max;
	TestFalse(TEXT("Box entirely in front of near plane is clipped"), AnomalyViewport::ProjectWorldBoundsToScreenRect(View,
		FBox(FVector(-.5, -.5, 2), FVector(.5, .5, 3)), Min, Max));
	TestTrue(TEXT("Near-plane crossing retains clipped edges"), AnomalyViewport::ProjectWorldBoundsToScreenRect(View,
		FBox(FVector(-.5, -.5, .5), FVector(.5, .5, 2)), Min, Max));
	TestEqual(TEXT("Orthographic left edge"), Min.X, .25);
	TestEqual(TEXT("Orthographic right edge"), Max.X, .75);
	return true;
}
#endif
