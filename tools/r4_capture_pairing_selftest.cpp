#include "../Source/AnomalyInjector/Public/AnomalyFrozenGeometry.h"
#include "../Source/AnomalyInjector/Public/AnomalyLabelSync.h"

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace AnomalyFrozenGeometry;
using namespace AnomalyLabelSync;

static int GFailures = 0;
static int GChecks = 0;

static void Check(bool bOk, const std::string& What)
{
	++GChecks;
	if (!bOk)
	{
		++GFailures;
		std::printf("FAIL %s\n", What.c_str());
	}
}

struct FVec2
{
	double X = 0.0;
	double Y = 0.0;
	FVec2() {}
	FVec2(double InX, double InY) : X(InX), Y(InY) {}
};

struct FBoxT
{
	double Min[3] = { 0.0, 0.0, 0.0 };
	double Max[3] = { 0.0, 0.0, 0.0 };
};

struct FViewT
{
	double CamX = 0.0;
	bool bValid = true;
};

struct FActorT
{
	bool bAlive = true;
	bool bHasBox = true;
	FBoxT Box;
};

struct FFireT
{
	int Actor = -1;
	bool bNamed = true;
	bool bWholeFrame = false;
};

struct FRect
{
	bool bOnScreen = false;
	FVec2 Min;
	FVec2 Max;
};

static FBoxT MakeBox(double X, double Y, double Z, double Half)
{
	FBoxT B;
	B.Min[0] = X - Half; B.Min[1] = Y - Half; B.Min[2] = Z - Half;
	B.Max[0] = X + Half; B.Max[1] = Y + Half; B.Max[2] = Z + Half;
	return B;
}

static bool ProjectBox(const FViewT& V, const FBoxT& B, FVec2& OutMin, FVec2& OutMax)
{
	OutMin = FVec2();
	OutMax = FVec2();
	if (!V.bValid)
	{
		return false;
	}
	double MinX = 1.0e30, MinY = 1.0e30, MaxX = -1.0e30, MaxY = -1.0e30;
	int InFront = 0;
	for (int C = 0; C < 8; ++C)
	{
		const double Px = (C & 1) ? B.Max[0] : B.Min[0];
		const double Py = (C & 2) ? B.Max[1] : B.Min[1];
		const double Pz = (C & 4) ? B.Max[2] : B.Min[2];
		const double D = Px - V.CamX;
		if (D <= 1.0e-6)
		{
			continue;
		}
		const double Sx = 0.5 + 0.5 * (Py / D);
		const double Sy = 0.5 - 0.5 * (Pz / D);
		MinX = Sx < MinX ? Sx : MinX; MaxX = Sx > MaxX ? Sx : MaxX;
		MinY = Sy < MinY ? Sy : MinY; MaxY = Sy > MaxY ? Sy : MaxY;
		++InFront;
	}
	if (InFront == 0)
	{
		return false;
	}
	OutMin = FVec2(MinX, MinY);
	OutMax = FVec2(MaxX, MaxY);
	return (OutMax.X > 0.0) && (OutMin.X < 1.0) && (OutMax.Y > 0.0) && (OutMin.Y < 1.0);
}

static const FActorT* Live(const std::vector<FActorT>& World, const FFireT& F)
{
	if (F.Actor < 0 || F.Actor >= (int)World.size() || !World[(size_t)F.Actor].bAlive)
	{
		return nullptr;
	}
	return &World[(size_t)F.Actor];
}

static TFrozen<FBoxT> SampleFire(const std::vector<FActorT>& World, const FFireT& F)
{
	const FActorT* A = Live(World, F);
	return Freeze(F.bWholeFrame, A != nullptr, F.bNamed, A != nullptr && A->bHasBox, A ? A->Box : FBoxT());
}

static FRect CompleteFrozen(const TFrozen<FBoxT>& G, const FViewT& FrozenView)
{
	FRect R;
	R.bOnScreen = Resolve(G, FrozenView,
		[](const FViewT& V, const FBoxT& B, FVec2& Mn, FVec2& Mx) { return ProjectBox(V, B, Mn, Mx); }, R.Min, R.Max);
	return R;
}

static FRect CompleteLiveLegacy(const std::vector<FActorT>& World, const FFireT& F, const FViewT& View)
{
	FRect R;
	const FActorT* A = Live(World, F);
	if (F.bWholeFrame || (!A && !F.bNamed))
	{
		R.Max = FVec2(1.0, 1.0);
		R.bOnScreen = true;
		return R;
	}
	if (A && A->bHasBox)
	{
		R.bOnScreen = ProjectBox(View, A->Box, R.Min, R.Max);
	}
	return R;
}

static bool Same(const FRect& A, const FRect& B)
{
	return A.bOnScreen == B.bOnScreen && std::fabs(A.Min.X - B.Min.X) < 1e-12 && std::fabs(A.Min.Y - B.Min.Y) < 1e-12
		&& std::fabs(A.Max.X - B.Max.X) < 1e-12 && std::fabs(A.Max.Y - B.Max.Y) < 1e-12;
}

static bool Member(const FRect& R)
{
	return IsAnnotationMemberPaired(EAnnotationPolicy::FireWindow, true, R.bOnScreen, true, false, false);
}

struct FCase
{
	const char* Name;
	bool bOldRuleMustDisagree;
};

template <class TMutate>
static void RunCase(const FCase& C, std::vector<FActorT> World, const FFireT& F, const FViewT& View, TMutate&& Mutate)
{
	const FRect Truth = CompleteLiveLegacy(World, F, View);
	const TFrozen<FBoxT> G = SampleFire(World, F);
	Mutate(World);
	const FRect Record = CompleteFrozen(G, View);
	const FRect Old = CompleteLiveLegacy(World, F, View);
	Check(Same(Record, Truth), std::string("R4 async ") + C.Name + ": the record's box and on-screen bit are the sample's");
	Check(Member(Record) == Member(Truth), std::string("R4 async ") + C.Name + ": annotation membership is the sample's");
	if (C.bOldRuleMustDisagree)
	{
		Check(!Same(Old, Truth), std::string("R4 async ") + C.Name + ": the old completion-time rule disagrees (the case has teeth)");
	}
	else
	{
		Check(Same(Old, Truth), std::string("R4 async ") + C.Name + ": nothing moved, so the old rule agrees too");
	}
}

static void TestAsyncFreeze()
{
	const FViewT View;
	std::vector<FActorT> World(1);
	World[0].Box = MakeBox(1000.0, 0.0, 0.0, 50.0);
	FFireT F;
	F.Actor = 0;

	RunCase({ "static target", false }, World, F, View, [](std::vector<FActorT>&) {});
	RunCase({ "target moves off screen after the sample", true }, World, F, View,
		[](std::vector<FActorT>& W) { W[0].Box = MakeBox(1000.0, 5000.0, 0.0, 50.0); });
	RunCase({ "target moves behind the camera after the sample", true }, World, F, View,
		[](std::vector<FActorT>& W) { W[0].Box = MakeBox(-1000.0, 0.0, 0.0, 50.0); });
	RunCase({ "target destroyed after the sample", true }, World, F, View,
		[](std::vector<FActorT>& W) { W[0].bAlive = false; });
	RunCase({ "target loses its renderable bounds after the sample", true }, World, F, View,
		[](std::vector<FActorT>& W) { W[0].bHasBox = false; });
	RunCase({ "target mesh grows after the sample", true }, World, F, View,
		[](std::vector<FActorT>& W) { W[0].Box = MakeBox(1000.0, 0.0, 0.0, 400.0); });

	std::vector<FActorT> Off(1);
	Off[0].Box = MakeBox(1000.0, 5000.0, 0.0, 50.0);
	RunCase({ "target enters the view after the sample", true }, Off, F, View,
		[](std::vector<FActorT>& W) { W[0].Box = MakeBox(1000.0, 0.0, 0.0, 50.0); });

	std::vector<FActorT> Dead(1);
	Dead[0].bAlive = false;
	RunCase({ "named target already gone at the sample", false }, Dead, F, View, [](std::vector<FActorT>&) {});
	FFireT Unnamed = F;
	Unnamed.bNamed = false;
	RunCase({ "unnamed target already gone at the sample is whole-frame", false }, Dead, Unnamed, View, [](std::vector<FActorT>&) {});
	RunCase({ "unnamed target destroyed after the sample keeps its box", true }, World, Unnamed, View,
		[](std::vector<FActorT>& W) { W[0].bAlive = false; });

	FFireT Whole = F;
	Whole.bWholeFrame = true;
	RunCase({ "whole-frame fire", false }, World, Whole, View, [](std::vector<FActorT>& W) { W[0].bAlive = false; });

	FViewT Bad;
	Bad.bValid = false;
	RunCase({ "invalid frozen view", false }, World, F, Bad, [](std::vector<FActorT>&) {});

	const TFrozen<FBoxT> Unsampled{};
	const FRect R = CompleteFrozen(Unsampled, View);
	Check(!R.bOnScreen && !Member(R) && R.Max.X == 0.0 && R.Max.Y == 0.0,
		"R4 async: a fire with no sample (added after the sample point) is off screen, never a member");

	Check(DecideAtSample(false, true, true, true) == ESource::Box && DecideAtSample(false, true, true, false) == ESource::None
		&& DecideAtSample(false, false, true, false) == ESource::None && DecideAtSample(false, false, false, false) == ESource::WholeFrame
		&& DecideAtSample(true, true, true, true) == ESource::WholeFrame,
		"R4 async: the sample decides whole-frame / box / none from the state at the sample only");
	Check(std::string(DescribeSource(ESource::Box)) == "box" && std::string(DescribeSource(ESource::WholeFrame)) == "whole_frame"
		&& std::string(DescribeSource(ESource::None)) == "none", "R4 async: source names");
}

static void TestSyncUnpaired()
{
	Check(std::string(DescribeReasonBit(7)) == "capture_unpaired" && ReasonCaptureUnpaired == 128 && NumReasons == 8,
		"R4 sync: capture_unpaired is the eighth reason bit (128)");
	Check(ReasonsOrLegacy(ReasonCaptureUnpaired) == ReasonCaptureUnpaired
		&& (ReasonsOrLegacy(ReasonCaptureUnpaired) & ReasonTemporal) == 0
		&& ReasonsOrLegacy(ReasonCaptureUnpaired | ReasonTemporal) == (ReasonCaptureUnpaired | ReasonTemporal),
		"R4 sync: capture_unpaired is never rewritten as a legacy temporal (anti-aliasing) flag");
	Check(DecideUnpairedEntry(EEntryEmit::Normal, true) == EEntryEmit::TransitionOnly
		&& DecideUnpairedEntry(EEntryEmit::TransitionOnly, true) == EEntryEmit::TransitionOnly
		&& DecideUnpairedEntry(EEntryEmit::Suppress, true) == EEntryEmit::Suppress
		&& DecideUnpairedEntry(EEntryEmit::Normal, false) == EEntryEmit::Normal,
		"R4 sync: an unpaired frame's entries are written transition-only; a suppressed one stays suppressed");
	const EAnnotationPolicy Policies[] = { EAnnotationPolicy::FireWindow, EAnnotationPolicy::ActorHidden,
		EAnnotationPolicy::AnomalyState, EAnnotationPolicy::RenderHeldWindow };
	bool bNeverMember = true;
	bool bPairedUnchanged = true;
	for (EAnnotationPolicy P : Policies)
	{
		for (int Bits = 0; Bits < 16; ++Bits)
		{
			const bool A = (Bits & 1) != 0, S = (Bits & 2) != 0, I = (Bits & 4) != 0, N = (Bits & 8) != 0;
			bNeverMember = bNeverMember && !IsAnnotationMemberPaired(P, A, S, I, N, true);
			bPairedUnchanged = bPairedUnchanged && IsAnnotationMemberPaired(P, A, S, I, N, false) == IsAnnotationMemberGated(P, A, S, I, N);
		}
	}
	Check(bNeverMember, "R4 sync: an unpaired frame is never an annotation member under any policy");
	Check(bPairedUnchanged, "R4 sync: a paired frame's membership is exactly the 090-09 gated rule");

	EEntryEmit Modes[3] = { EEntryEmit::Normal, EEntryEmit::Normal, EEntryEmit::Suppress };
	Check(FramePresent(Modes, 3, 3), "R4 sync: the frame before the flag reads present");
	for (EEntryEmit& M : Modes)
	{
		M = DecideUnpairedEntry(M, true);
	}
	Check(!FramePresent(Modes, 3, 3) && Modes[2] == EEntryEmit::Suppress,
		"R4 sync: after the flag the frame is not present (unlabelled, dropped) and nothing is invented");
}

int main()
{
	TestAsyncFreeze();
	TestSyncUnpaired();
	std::printf("r4_capture_pairing_selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
