#include "../Source/AnomalyCapture/Private/AnomalyActiveSource.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#ifndef SRC_MUTANT
#define SRC_MUTANT 0
#endif

using AnomalyActiveSource::ESource;
using AnomalyLabelSync::EAnnotationPolicy;

static int GChecks = 0;
static int GFailures = 0;

static void Check(bool bOk, const std::string& What)
{
	++GChecks;
	if (!bOk)
	{
		++GFailures;
		std::printf("FAIL %s\n", What.c_str());
	}
}

static bool ResolveUnderTest(const char* Id, ESource& Out)
{
	if constexpr (SRC_MUTANT == 1)
	{
		if (AnomalyActiveSource::SameId(Id, "uv_corruption"))
		{
			Out = ESource::AnomalyState;
			return true;
		}
	}
	return AnomalyActiveSource::Resolve(Id, Out);
}

static bool LabelledUnderTest(ESource Source, bool bRenderTruth, bool bActive, bool bOnScreen)
{
	if constexpr (SRC_MUTANT == 2)
	{
		return bActive;
	}
	else
	{
		return AnomalyActiveSource::IsLabelledMember(Source, bRenderTruth, bActive, bOnScreen);
	}
}

static std::string ReadText(const std::string& Path)
{
	std::ifstream In(Path, std::ios::binary);
	std::stringstream Ss;
	Ss << In.rdbuf();
	return Ss.str();
}

static std::string FunctionBody(const std::string& Text, const std::string& Signature)
{
	const size_t Start = Text.find(Signature);
	if (Start == std::string::npos)
	{
		return std::string();
	}
	const size_t Open = Text.find('{', Start);
	if (Open == std::string::npos)
	{
		return std::string();
	}
	int Depth = 0;
	for (size_t i = Open; i < Text.size(); ++i)
	{
		if (Text[i] == '{')
		{
			++Depth;
		}
		else if (Text[i] == '}' && --Depth == 0)
		{
			return Text.substr(Open, i - Open + 1);
		}
	}
	return std::string();
}

static bool Has(const std::string& Text, const std::string& Token)
{
	return !Text.empty() && Text.find(Token) != std::string::npos;
}

int main(int Argc, char** Argv)
{
	const char* M53Ids[] = { "uv_corruption", "normal_corruption" };
	for (const char* Id : M53Ids)
	{
		ESource Source = ESource::ActorHidden;
		const bool bKnown = ResolveUnderTest(Id, Source);
		const std::string Name(Id);
		Check(bKnown, Name + " is registered in the active-source table");
		Check(Source == ESource::FireWindow, Name + " resolves to FireWindow");
		Check(AnomalyActiveSource::PolicyFor(Source, false) == EAnnotationPolicy::FireWindow,
			Name + " annotation policy is fire_window");
		for (int a = 0; a < 2; ++a)
		{
			for (int o = 0; o < 2; ++o)
			{
				const bool bLabelled = LabelledUnderTest(Source, false, a != 0, o != 0);
				const std::string Cell = Name + " active=" + std::to_string(a) + " on_screen=" + std::to_string(o);
				Check(bLabelled == AnomalyLabelSync::IsAnnotationMember(EAnnotationPolicy::FireWindow, a != 0, o != 0),
					Cell + " labelled comes from IsAnnotationMember(FireWindow)");
				Check(bLabelled == (o != 0), Cell + " labelled == projected box on screen");
				Check(AnomalyLabelSync::IsEntryLabelled(true, AnomalyActiveSource::PolicyFor(Source, false), a != 0, o != 0) == bLabelled,
					Cell + " row labelled (IsEntryLabelled) agrees");
			}
		}
		const bool OnScreen[] = { false, true, true, false, true, true, true, false };
		const bool Active[] = { true, true, false, true, true, false, true, true };
		std::string Injected;
		std::string Members;
		for (int i = 0; i < 8; ++i)
		{
			if (LabelledUnderTest(Source, false, Active[i], OnScreen[i]))
			{
				Injected += std::to_string(i) + ",";
			}
			if (AnomalyLabelSync::IsAnnotationMember(EAnnotationPolicy::FireWindow, Active[i], OnScreen[i]))
			{
				Members += std::to_string(i) + ",";
			}
		}
		Check(Injected == Members && Injected == "1,2,4,5,6,", Name + " injected_frames equals the IsAnnotationMember set");
	}

	{
		ESource Source = ESource::FireWindow;
		Check(ResolveUnderTest("stuck_low_mip", Source) && Source == ESource::AnomalyState, "stuck_low_mip resolves to AnomalyState");
		Check(AnomalyActiveSource::PolicyFor(Source, true) == EAnnotationPolicy::RenderHeldWindow,
			"stuck_low_mip render-truth fire is render_held_window");
		Check(AnomalyActiveSource::PolicyFor(Source, false) == EAnnotationPolicy::AnomalyState,
			"stuck_low_mip without render truth is anomaly_state");
		Check(ResolveUnderTest("blinking", Source) && Source == ESource::ActorHidden, "blinking resolves to ActorHidden");
		Check(!ResolveUnderTest("not_an_anomaly", Source) && Source == ESource::FireWindow, "unknown id falls back to FireWindow, unknown");
	}

	std::string Root;
	if (Argc > 1)
	{
		Root = Argv[1];
	}
	else
	{
		const std::string Here(__FILE__);
		const size_t Slash = Here.find_last_of("/\\");
		Root = (Slash == std::string::npos ? std::string(".") : Here.substr(0, Slash)) + "/..";
	}
	const std::string Capture = ReadText(Root + "/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp");
	std::string Writer = ReadText(Root + "/Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp");
	if constexpr (SRC_MUTANT == 3)
	{
		const std::string From = "AnomalyLabelSync::IsAnnotationMember(Policy, bActive, bOnScreen)";
		const size_t At = Writer.find(From);
		if (At != std::string::npos)
		{
			Writer.replace(At, From.size(), "bActive");
		}
	}
	Check(!Capture.empty(), "capture source readable at " + Root);
	Check(!Writer.empty(), "label writer source readable at " + Root);

	const std::string Resolve = FunctionBody(Capture, "EAnomalyActiveSource ResolveAnomalyActiveSource(FName Id, bool& bOutKnownId)");
	Check(Has(Resolve, "AnomalyActiveSource::Table("), "capture ResolveAnomalyActiveSource builds its map from AnomalyActiveSource::Table");
	Check(Has(Capture, "using EAnomalyActiveSource = AnomalyActiveSource::ESource;"), "capture active-source enum is the pure header's");
	const std::string Policy = FunctionBody(Capture, "uint8 UAnomalyCaptureSubsystem::ResolveAnnotationPolicy(");
	Check(Has(Policy, "AnomalyActiveSource::PolicyFor(ResolveAnomalyActiveSource(F.Id, bKnownId), IsRenderTruthFire(F))"),
		"capture ResolveAnnotationPolicy calls AnomalyActiveSource::PolicyFor");
	const std::string Fill = FunctionBody(Capture, "void UAnomalyCaptureSubsystem::FillAnnotationInputs(");
	Check(Has(Fill, "Snap.FirePolicy[i] = ResolveAnnotationPolicy(Snap.Fires[i]);"), "FillAnnotationInputs takes each fire's policy from ResolveAnnotationPolicy");
	const std::string Accum = FunctionBody(Capture, "void UAnomalyCaptureSubsystem::AccumulateFrameEvents(");
	Check(Has(Accum, "Ev->MemberByIndex.Add(SessionIndex, AnomalyLabel::IsFireInAnnotation(&FirePolicy, &FireActive, &FireOnScreen, i)"),
		"AccumulateFrameEvents records membership through IsFireInAnnotation");
	const std::string Annot = FunctionBody(Capture, "void UAnomalyCaptureSubsystem::WriteSessionAnnotationFile(");
	Check(Has(Annot, "FrameIndices = MoveTemp(MemberIdx);") && Has(Annot, "Out.InjectedFrameIndices = FrameIndices;"),
		"annotation injected_frames is the member set");
	const std::string InAnnotation = FunctionBody(Writer, "bool IsFireInAnnotation(");
	Check(Has(InAnnotation, "return AnomalyLabelSync::IsAnnotationMember(Policy, bActive, bOnScreen);"),
		"IsFireInAnnotation returns AnomalyLabelSync::IsAnnotationMember");
	const std::string RowLabelled = FunctionBody(Writer, "bool IsSnapshotEntryLabelled(");
	Check(Has(RowLabelled, "AnomalyLabelSync::IsEntryLabelled("), "labels.jsonl row labelled comes from AnomalyLabelSync::IsEntryLabelled");

	std::printf("active source selftest (mutant %d): %d checks, %d failures\n", SRC_MUTANT, GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
