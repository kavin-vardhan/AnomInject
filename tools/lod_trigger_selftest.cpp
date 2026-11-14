#ifndef LOD_TRIGGER_HEADER
#define LOD_TRIGGER_HEADER "../Source/AnomalyInjector/Public/AnomalyLodTriggerRule.h"
#endif
#include LOD_TRIGGER_HEADER

#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

using namespace AnomalyLodTriggerRule;

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

static bool Is(const char* Got, const char* Want)
{
	return Got && std::strcmp(Got, Want) == 0;
}

static const float GFixtureSS[8] = { 1.0f, 0.05f, 0, 0, 0, 0, 0, 0 };
static const int GFixtureTris[2] = { 960, 48 };
static const float GRockSS[8] = { 1.0f, 0.5f, 0.25f, 0.1f, 0, 0, 0, 0 };
static const int GRockTris[4] = { 4000, 2000, 400, 90 };

static FFrameInput Frame(bool bOnScreen, int EngineLod, float BoundsScreenSize, float CoveragePct, bool bContrast = true,
	bool bRetreating = false, int T0 = -2, int T1 = -2)
{
	FFrameInput In;
	In.bOnScreen = bOnScreen;
	In.bInFrustum = bOnScreen;
	In.EngineLod = EngineLod;
	In.TemporalLod0 = T0 == -2 ? EngineLod : T0;
	In.TemporalLod1 = T1 == -2 ? EngineLod : T1;
	In.BoundsScreenSize = BoundsScreenSize;
	In.CoveragePct = CoveragePct;
	In.bContrast = bContrast;
	In.bRetreating = bRetreating;
	return In;
}

static void TestEngineFormula()
{
	Check(EngineLodFromScreenSize(GFixtureSS, 8, 0.04f, 1.0f, 0) == 1, "fixture far is LOD1");
	Check(EngineLodFromScreenSize(GFixtureSS, 8, 0.06f, 1.0f, 0) == 0, "fixture near is LOD0");
	Check(EngineLodFromScreenSize(GFixtureSS, 8, 0.05f, 1.0f, 0) == 0, "equal screen size leaves the coarse LOD (strict >)");
	Check(EngineLodFromScreenSize(GFixtureSS, 8, 0.0499f, 1.0f, 0) == 1, "just below the switch keeps LOD1");
	Check(EngineLodFromScreenSize(GFixtureSS, 8, 0.06f, 2.0f, 0) == 1, "scale 2 keeps the coarse LOD at twice the size");
	Check(EngineLodFromScreenSize(GFixtureSS, 8, 0.5f, 1.0f, 1) == 1, "MinLOD clamps up");
	Check(EngineLodFromScreenSize(GRockSS, 8, 0.05f, 1.0f, 0) == 3, "rock tiny is LOD3");
	Check(EngineLodFromScreenSize(GRockSS, 8, 0.2f, 1.0f, 0) == 2, "rock 0.2 is LOD2");
	Check(EngineLodFromScreenSize(GRockSS, 8, 0.4f, 1.0f, 0) == 1, "rock 0.4 is LOD1");
	Check(EngineLodFromScreenSize(GRockSS, 8, 0.9f, 1.0f, 0) == 0, "rock 0.9 is LOD0");
	Check(EngineLodFromScreenSize(GRockSS, 8, 1.5f, 1.0f, 0) == 0, "larger than every screen size is LOD0");
}

static void TestContrast()
{
	Check(FinestContrastLod(GFixtureTris, 2, 1, 4) == 0, "fixture LOD1 contrasts with LOD0 (48*4 <= 960)");
	Check(FinestContrastLod(GRockTris, 4, 3, 4) == 2, "rock held LOD3 contrasts first with LOD2 (90*4 <= 400)");
	Check(FinestContrastLod(GRockTris, 4, 1, 4) == -1, "rock held LOD1 has no contrast LOD (2000*4 > 4000)");
	Check(FinestContrastLod(GRockTris, 4, 0, 4) == -1, "held LOD0 has nothing finer");
	Check(FinestContrastLod(GRockTris, 4, 2, 10) == 0, "ratio 10: held LOD2 contrasts only with LOD0");
	Check(ContrastPasses(GRockTris, 4, 3, 2, 4), "contrast 3 vs 2 passes");
	Check(!ContrastPasses(GRockTris, 4, 3, 3, 4), "same LOD never contrasts");
	Check(ContrastPasses(GRockTris, 4, 2, 1, 4), "2 vs 1 passes (400*4 <= 2000)");
	Check(!ContrastPasses(GRockTris, 4, 2, 1, 6), "2 vs 1 fails at ratio 6 (400*6 > 2000)");
	Check(ContrastPasses(GRockTris, 4, 2, 0, 4), "2 vs 0 passes");
	Check(!ContrastPasses(GRockTris, 4, 1, 0, 4), "1 vs 0 fails");
	Check(ContrastPasses(GFixtureTris, 2, 1, 0, 20), "fixture ratio 20 passes exactly (48*20 = 960)");
	Check(!ContrastPasses(GFixtureTris, 2, 1, 0, 21), "fixture ratio 21 fails");
}

static void TestThresholds()
{
	FSettings S;
	Check(ScreenSizeForEngineLodAtMost(GFixtureSS, 8, 0, 1.0f) == 0.05f, "fixture switch to LOD0 at 0.05");
	Check(ScreenSizeForEngineLodAtMost(GRockSS, 8, 2, 1.0f) == 0.1f, "rock LOD<=2 from 0.1");
	Check(ScreenSizeForEngineLodAtMost(GRockSS, 8, -1, 1.0f) < 0.0f, "no contrast LOD gives no threshold");
	const float R = ReleaseScreenSize(GFixtureSS, 8, GFixtureTris, 2, 1, 1.0f, S);
	Check(R > 0.0549f && R < 0.0551f, "fixture release = 0.05 * 1.10 got " + std::to_string(R));
	const float RockR = ReleaseScreenSize(GRockSS, 8, GRockTris, 4, 3, 1.0f, S);
	Check(RockR > 0.1099f && RockR < 0.1101f, "rock held 3 release = 0.1 * 1.10 got " + std::to_string(RockR));
	Check(ReleaseScreenSize(GRockSS, 8, GRockTris, 4, 1, 1.0f, S) < 0.0f, "rock held 1 has no release");
}

static void TestFreezeAndClassify()
{
	Check(FreezeIsInvisible(1, 1, 1), "freeze at LOD1 with agreeing samples is invisible");
	Check(!FreezeIsInvisible(0, 0, 0), "freezing LOD0 is not a held coarse LOD");
	Check(!FreezeIsInvisible(1, 1, 0), "mid-dither freeze refused");
	Check(!FreezeIsInvisible(2, 1, 2), "a sample disagreeing with the engine LOD refuses");
	Check(ClassifyFrame(true, true, true, 1, 0, 9.0f, 7.0f, true) == EFrameClass::Positive, "held, on screen, wrong, big = positive");
	Check(ClassifyFrame(true, true, true, 1, 0, 3.0f, 7.0f, true) == EFrameClass::Subvisible, "held, wrong, small = subvisible");
	Check(ClassifyFrame(true, true, true, 1, 0, 7.0f, 7.0f, true) == EFrameClass::Positive, "coverage equal to the minimum is positive");
	Check(ClassifyFrame(true, false, false, 1, 0, 9.0f, 7.0f, true) == EFrameClass::Negative, "off screen is negative");
	Check(ClassifyFrame(true, true, true, 1, 1, 9.0f, 7.0f, true) == EFrameClass::Negative, "held equals engine is negative");
	Check(ClassifyFrame(true, true, true, 1, 2, 9.0f, 7.0f, true) == EFrameClass::Negative, "engine coarser than held is negative");
	Check(ClassifyFrame(false, true, true, 1, 0, 9.0f, 7.0f, true) == EFrameClass::Negative, "released is negative");
	Check(ClassifyFrame(true, true, true, 1, -1, 9.0f, 7.0f, true) == EFrameClass::Negative, "unknown engine LOD is negative");
	Check(ClassifyFrame(true, false, true, 1, 0, 9.0f, 7.0f, true) == EFrameClass::Occluded,
		"held, in the frustum, engine finer, every visibility trace blocked = occluded (flagged, never a plain negative)");
	Check(ClassifyFrame(true, false, true, 1, 1, 9.0f, 7.0f, true) == EFrameClass::Negative, "in the frustum but held equals engine = negative");
	Check(ClassifyFrame(false, false, true, 1, 0, 9.0f, 7.0f, true) == EFrameClass::Negative, "released and occluded = negative");
	Check(IsFlaggedClass(EFrameClass::Occluded) && IsFlaggedClass(EFrameClass::Subvisible) && !IsFlaggedClass(EFrameClass::Positive),
		"occluded and subvisible are the flagged classes");	Check(ClassifyFrame(true, true, true, 1, 0, 9.0f, 7.0f, false) == EFrameClass::Subvisible,
		"held, wrong, big but without triangle contrast = subvisible (a faint pop is not a usable positive)");
	Check(ClassifyFrame(true, true, true, 3, 2, 30.0f, 7.0f, false) == EFrameClass::Subvisible, "rock held 3 vs engine 2 with no contrast = subvisible");}

static void TestArmRules()
{
	FSettings S;
	int Ahead = 0;
	FDistanceArmInput D;
	D.EngineLod = 1; D.TemporalLod0 = 1; D.TemporalLod1 = 1; D.BoundsScreenSize = 0.03f; D.ReleaseScreenSize = 0.055f;
	D.CoveragePct = 9.0f; D.DistanceCm = 1000.0f; D.FrameSeconds = 1.0f / 30.0f; D.ClosingSpeedCmPerS = 225.0f; D.ApproachAngleDeg = 5.0f;
	float LookAhead = 0.0f;
	Check(DistanceArmRefusal(D, S, Ahead, &LookAhead) == nullptr && Ahead == 61,
		"approach arms 61 frames ahead (1000 cm to 545 cm at 7.5 cm/frame), got " + std::to_string(Ahead));
	Check(LookAhead > 0.0549f && LookAhead < 0.0551f, "covered target: the look-ahead is the contrast release size");
	FDistanceArmInput Slow = D; Slow.ClosingSpeedCmPerS = 20.0f;
	Check(Is(DistanceArmRefusal(Slow, S, Ahead), "not_approaching"), "slow approach refused");
	FDistanceArmInput Away = D; Away.ClosingSpeedCmPerS = -100.0f;
	Check(Is(DistanceArmRefusal(Away, S, Ahead), "not_approaching"), "retreating camera refused");
	FDistanceArmInput Side = D; Side.ApproachAngleDeg = 45.0f;
	Check(Is(DistanceArmRefusal(Side, S, Ahead), "off_motion_axis"), "target off the motion axis refused");
	FDistanceArmInput Far = D; Far.ClosingSpeedCmPerS = 50.0f;
	Check(Is(DistanceArmRefusal(Far, S, Ahead), "too_far_ahead"), "release 273 frames ahead at 50 cm/s refused");
	FDistanceArmInput CovSmall = D; CovSmall.CoveragePct = 1.0f;
	Check(DistanceArmRefusal(CovSmall, S, Ahead, &LookAhead) == nullptr && Ahead == 83,
		"coverage-limited target arms 83 frames ahead (looks ahead to the 7 % size), got " + std::to_string(Ahead));
	Check(LookAhead > 0.0793f && LookAhead < 0.0795f, "look-ahead = the size at which coverage reaches 7 % (0.03 x sqrt 7), got " + std::to_string(LookAhead));
	FDistanceArmInput Tiny = D; Tiny.CoveragePct = 0.1f;
	Check(Is(DistanceArmRefusal(Tiny, S, Ahead), "too_far_ahead"), "coverage 0.1 %: the 7 % size is out of reach, refused at the arm");
	FDistanceArmInput NoCov = D; NoCov.CoveragePct = 0.0f;
	Check(Is(DistanceArmRefusal(NoCov, S, Ahead), "no_coverage"), "no coverage refused");
	Check(CoverageScreenSize(0.1f, 7.0f, 7.0f) == 0.1f, "coverage already at the minimum: the current size");
	const float Cov4 = CoverageScreenSize(0.1f, 1.75f, 7.0f);
	Check(Cov4 > 0.1999f && Cov4 < 0.2001f, "a quarter of the coverage needs twice the screen size");
	Check(CoverageScreenSize(0.1f, 0.0f, 7.0f) < 0.0f, "zero coverage has no coverage size");
	FDistanceArmInput C3;
	C3.EngineLod = 1; C3.TemporalLod0 = 1; C3.TemporalLod1 = 1; C3.BoundsScreenSize = 0.0428f; C3.ReleaseScreenSize = 0.055f;
	C3.CoveragePct = 0.1083f; C3.DistanceCm = 2075.9f; C3.FrameSeconds = 1.0f / 30.0f; C3.ClosingSpeedCmPerS = 224.7f; C3.ApproachAngleDeg = 1.7f;
	Check(Is(DistanceArmRefusal(C3, S, Ahead), "too_far_ahead") && Ahead > 230,
		"review C3 at si 51 (copy 2076 cm away, 0.11 % coverage): refused at the arm instead of arming and timing out, " + std::to_string(Ahead) + " frames ahead");
	FDistanceArmInput Fine = D; Fine.EngineLod = 0; Fine.TemporalLod0 = 0; Fine.TemporalLod1 = 0;
	Check(Is(DistanceArmRefusal(Fine, S, Ahead), "at_finest_lod"), "target at LOD0 refused");
	FDistanceArmInput Dither = D; Dither.TemporalLod1 = 0;
	Check(Is(DistanceArmRefusal(Dither, S, Ahead), "mid_dither"), "mid-dither refused");
	FDistanceArmInput Past = D; Past.BoundsScreenSize = 0.06f; Past.CoveragePct = 9.0f;
	Check(Is(DistanceArmRefusal(Past, S, Ahead), "already_past_release"), "already past the release refused");
	FDistanceArmInput NoContrast = D; NoContrast.ReleaseScreenSize = -1.0f;
	Check(Is(DistanceArmRefusal(NoContrast, S, Ahead), "no_contrast_lod"), "no contrast refused");

	FViewEntryArmInput V;
	V.AngleOutsideFrustumDeg = 10.0f; V.bYawTowardTarget = true; V.YawRateDegPerS = 60.0f; V.FrameSeconds = 1.0f / 30.0f;
	V.bContrastAtHeld = true; V.CoverageAsIfInViewPct = 12.0f;
	Check(ViewEntryArmRefusal(V, S, Ahead) == nullptr && Ahead == 5, "turn arms with 5 frames ahead, got " + std::to_string(Ahead));
	FViewEntryArmInput In = V; In.AngleOutsideFrustumDeg = -3.0f;
	Check(Is(ViewEntryArmRefusal(In, S, Ahead), "already_in_view"), "in view refused");
	FViewEntryArmInput Near = V; Near.AngleOutsideFrustumDeg = 25.0f;
	Check(ViewEntryArmRefusal(Near, S, Ahead) == nullptr && Ahead == 13,
		"25 degrees outside at 60 deg/s arms (time-based margin), got " + std::to_string(Ahead));
	FViewEntryArmInput Wide = V; Wide.AngleOutsideFrustumDeg = 125.0f;
	Check(Is(ViewEntryArmRefusal(Wide, S, Ahead), "too_far_outside_view"), "125 degrees outside refused (the 120 degree cap)");
	FViewEntryArmInput SlowWide = V; SlowWide.YawRateDegPerS = 30.0f; SlowWide.AngleOutsideFrustumDeg = 95.0f;
	Check(Is(ViewEntryArmRefusal(SlowWide, S, Ahead), "too_far_outside_view"), "95 degrees outside at 30 deg/s refused (margin 90)");
	FViewEntryArmInput SlowIn = SlowWide; SlowIn.AngleOutsideFrustumDeg = 60.0f;
	Check(ViewEntryArmRefusal(SlowIn, S, Ahead) == nullptr && Ahead == 60, "60 degrees outside at 30 deg/s arms 60 frames ahead");
	FViewEntryArmInput Wrong = V; Wrong.bYawTowardTarget = false;
	Check(Is(ViewEntryArmRefusal(Wrong, S, Ahead), "not_turning_toward"), "turning away refused");
	FViewEntryArmInput Lazy = V; Lazy.YawRateDegPerS = 10.0f;
	Check(Is(ViewEntryArmRefusal(Lazy, S, Ahead), "not_turning_toward"), "slow turn refused");
	FViewEntryArmInput Shadow = V; Shadow.bShadowInView = true;
	Check(Is(ViewEntryArmRefusal(Shadow, S, Ahead), "lod_shadow_in_view"), "shadow in view refused");
	FViewEntryArmInput Small = V; Small.CoverageAsIfInViewPct = 2.0f;
	Check(Is(ViewEntryArmRefusal(Small, S, Ahead), "too_small_in_view"), "too small in view refused");
}

static void TestViewEntryWindow()
{
	FSettings S;
	const float Dt = 1.0f / 30.0f;
	const float M30 = ViewEntryMarginForRate(S, 30.0f, Dt);
	Check(M30 > 89.99f && M30 < 90.01f, "margin at 30 deg/s = 3 s of turn = 90 deg, got " + std::to_string(M30));
	Check(ViewEntryMarginForRate(S, 60.0f, Dt) == S.ViewEntryMarginMaxDeg, "margin at 60 deg/s is capped at 120 deg");
	Check(ViewEntryMarginForRate(S, 3.0f, Dt) == S.ViewEntryMarginDeg, "margin at 3 deg/s is floored at 20 deg");
	const int BurstCycle = 2 + 8 + 2 + 4;
	const int MotionLag = MotionEstimateFrames + 2;
	const float Rates[4] = { S.MinYawRateDegPerS, 45.0f, 60.0f, 120.0f };
	for (float Rate : Rates)
	{
		FViewEntryArmInput V;
		V.bYawTowardTarget = true; V.YawRateDegPerS = Rate; V.FrameSeconds = Dt; V.bContrastAtHeld = true; V.CoverageAsIfInViewPct = 12.0f;
		int Window = 0;
		int Ahead = 0;
		for (float Angle = 170.0f; Angle > 0.0f; Angle -= Rate * Dt)
		{
			V.AngleOutsideFrustumDeg = Angle;
			if (ViewEntryArmRefusal(V, S, Ahead) == nullptr)
			{
				++Window;
			}
		}
		Check(Window > BurstCycle + MotionLag, "view-entry arming window at " + std::to_string((int)Rate) + " deg/s is " + std::to_string(Window)
			+ " frames, longer than one default burst cycle (" + std::to_string(BurstCycle) + ") plus the motion-estimate lag ("
			+ std::to_string(MotionLag) + ")");
		if (Rate == S.MinYawRateDegPerS)
		{
			Check(Window >= 2 * BurstCycle, "at the minimum yaw rate the window spans two burst cycles, got " + std::to_string(Window));
		}
	}
}

static void TestApproach()
{
	FSettings S;
	FTracker T;
	T.Mode = EMode::DistanceThreshold;
	T.HeldLod = 1;
	T.ReleaseScreenSize = ReleaseScreenSize(GFixtureSS, 8, GFixtureTris, 2, 1, 1.0f, S);
	FObservation O = ObserveFrame(T, Frame(true, 1, 0.04f, 2.0f), S);
	Check(O.Class == EFrameClass::Negative && T.State == EState::Armed, "frozen far: negative, armed");
	Check(DecideTick(T, S) == ETickAction::None, "no release while held equals engine");
	O = ObserveFrame(T, Frame(true, 0, 0.051f, 3.0f), S);
	Check(O.Class == EFrameClass::Subvisible, "wrong but small: subvisible");
	Check(!IsHoldingEventOpen(T), "subvisible does not hold the event open");
	Check(DecideTick(T, S) == ETickAction::None, "no release while subvisible");
	O = ObserveFrame(T, Frame(true, 0, 0.053f, 8.0f), S);
	Check(O.Class == EFrameClass::Positive && T.State == EState::Labelled, "wrong and big: positive, labelled");
	Check(IsHoldingEventOpen(T), "positive holds the event open");
	Check(DecideTick(T, S) == ETickAction::None, "below the release margin: still held");
	O = ObserveFrame(T, Frame(true, 0, 0.0590f, 8.4f, true, false, 0, 1), S);
	Check(DecideTick(T, S) == ETickAction::None, "past the release but mid-dither: release deferred");
	O = ObserveFrame(T, Frame(true, 0, 0.056f, 8.6f), S);
	Check(O.Class == EFrameClass::Positive, "still positive before the release");
	Check(DecideTick(T, S) == ETickAction::None && T.bHeld,
		"past the contrast release size but within 10 % of the first positive size: still held (the margin applies to the later threshold)");
	Check(T.FirstPositiveScreenSize > 0.0529f && T.FirstPositiveScreenSize < 0.0531f, "first positive screen size recorded");
	O = ObserveFrame(T, Frame(true, 0, 0.0585f, 8.9f), S);
	Check(DecideTick(T, S) == ETickAction::ReleaseVisible && !T.bHeld, "release at 1.1 x the first positive size");
	Check(T.SwapScreenSize > 0.0584f && T.SwapScreenSize < 0.0586f, "swap screen size recorded");
	O = ObserveFrame(T, Frame(true, 0, 0.060f, 9.0f), S);
	Check(O.bSwap && O.Cause == ESwapCause::Distance && !O.bDown && O.FromLod == 1 && O.ToLod == 0, "swap frame: distance up 1->0");
	Check(O.Class == EFrameClass::Negative && T.State == EState::Swapped, "swap frame is negative");
	Check(!IsHoldingEventOpen(T), "swapped does not hold open");
	O = ObserveFrame(T, Frame(true, 0, 0.06f, 9.4f), S);
	Check(!O.bSwap, "a swap is recorded once");
	Check(DecideTick(T, S) == ETickAction::None, "approaching after the swap: nothing");

	O = ObserveFrame(T, Frame(true, 0, 0.052f, 8.0f, true, true), S);
	Check(DecideTick(T, S) == ETickAction::None, "retreat inside the hysteresis band: no re-hold");
	O = ObserveFrame(T, Frame(true, 0, 0.047f, 7.5f, true, true), S);
	Check(DecideTick(T, S) == ETickAction::ReHold && T.bHeld, "retreat past 0.85 of the swap size: re-hold (down-pop)");
	O = ObserveFrame(T, Frame(true, 0, 0.046f, 7.4f, true, true), S);
	Check(O.bSwap && O.bDown && O.Cause == ESwapCause::Distance && O.FromLod == 0 && O.ToLod == 1, "down-pop swap recorded 0->1");
	Check(O.Class == EFrameClass::Positive, "after a down-pop the frames are positive");
	O = ObserveFrame(T, Frame(true, 1, 0.045f, 7.3f, true, true), S);
	Check(O.Class == EFrameClass::Negative, "engine coarsened to the held LOD: negative");
	Check(DecideTick(T, S) == ETickAction::ReleaseInvisible && !T.bHeld, "hold matches the engine: invisible release");
	O = ObserveFrame(T, Frame(true, 1, 0.044f, 7.2f, true, true), S);
	Check(!O.bSwap, "an invisible release is not a swap");

	FTracker U;
	U.Mode = EMode::DistanceThreshold;
	U.HeldLod = 1;
	U.ReleaseScreenSize = 0.055f;
	ObserveFrame(U, Frame(true, 0, 0.056f, 9.0f), S);
	ObserveFrame(U, Frame(true, 0, 0.062f, 9.5f), S);
	Check(DecideTick(U, S) == ETickAction::ReleaseVisible, "release fires");
	O = ObserveFrame(U, Frame(true, 1, 0.04f, 4.0f), S);
	Check(!O.bSwap, "a release whose next frame already shows the held LOD is not a visible swap");
	O = ObserveFrame(U, Frame(true, 0, 0.06f, 9.0f), S);
	Check(!O.bSwap, "a later engine change is not a plugin swap");
}

static void TestNoDownPopWhenOff()
{
	FSettings S;
	S.bDownPop = false;
	FTracker T;
	T.HeldLod = 1;
	T.bHeld = false;
	T.SwapScreenSize = 0.056f;
	T.State = EState::Swapped;
	ObserveFrame(T, Frame(true, 0, 0.04f, 7.5f, true, true), S);
	Check(DecideTick(T, S) == ETickAction::None, "down-pop switch off: no re-hold");
	S.bDownPop = true;
	ObserveFrame(T, Frame(true, 0, 0.04f, 7.5f, true, false), S);
	Check(DecideTick(T, S) == ETickAction::None, "not retreating: no re-hold even below the band");
	ObserveFrame(T, Frame(true, 0, 0.04f, 3.0f, true, true), S);
	Check(DecideTick(T, S) == ETickAction::None, "too small for a visible down-pop: no re-hold");
}

static void TestViewEntry()
{
	FSettings S;
	FTracker T;
	T.Mode = EMode::ViewEntry;
	T.HeldLod = 1;
	FObservation O = ObserveFrame(T, Frame(false, 0, 0.3f, 0.0f), S);
	Check(O.Class == EFrameClass::Negative && T.State == EState::Armed, "off screen: negative, armed");
	Check(DecideTick(T, S) == ETickAction::None, "no release off screen");
	O = ObserveFrame(T, Frame(true, 0, 0.3f, 2.0f), S);
	Check(O.Class == EFrameClass::Subvisible, "partly entered: subvisible");
	for (int i = 1; i <= 4; ++i)
	{
		O = ObserveFrame(T, Frame(true, 0, 0.3f, 12.0f), S);
		Check(O.Class == EFrameClass::Positive, "entered frame " + std::to_string(i) + " positive");
		const ETickAction A = DecideTick(T, S);
		Check((i < 4) ? A == ETickAction::None : A == ETickAction::ReleaseVisible,
			"view entry releases after exactly 4 positive frames, frame " + std::to_string(i));
	}
	O = ObserveFrame(T, Frame(true, 0, 0.3f, 12.0f), S);
	Check(O.bSwap && O.Cause == ESwapCause::ViewEntry && O.FromLod == 1 && O.ToLod == 0, "view-entry swap recorded");
	Check(T.PositiveFramesSeen == 4, "four positive frames before the snap");
}

static void TestTurnAwayAndCap()
{
	FSettings S;
	FTracker T;
	T.Mode = EMode::ViewEntry;
	T.HeldLod = 1;
	ObserveFrame(T, Frame(true, 0, 0.3f, 12.0f), S);
	ObserveFrame(T, Frame(true, 0, 0.3f, 12.0f), S);
	FObservation O = ObserveFrame(T, Frame(false, 0, 0.3f, 0.0f), S);
	Check(O.Class == EFrameClass::Negative && T.bHeld && T.State == EState::Armed, "turned away before the snap: held, armed, negative");
	Check(!IsHoldingEventOpen(T), "turned away: the event is not held open");
	Check(DecideTick(T, S) == ETickAction::None, "no visible release while unseen");
	FFrameInput Glass = Frame(false, 0, 0.3f, 12.0f);
	Glass.bInFrustum = true;
	O = ObserveFrame(T, Glass, S);
	Check(O.Class == EFrameClass::Occluded && !IsHoldingEventOpen(T) && T.bHeld, "behind glass: occluded, held, does not hold the event open");
	Check(DecideTick(T, S) == ETickAction::None, "no release while occluded");
	FSettings Cap;
	Cap.HoldCapFrames = 5;
	FTracker C;
	C.Mode = EMode::DistanceThreshold;
	C.HeldLod = 1;
	C.ReleaseScreenSize = 1.0f;
	for (int i = 1; i <= 4; ++i)
	{
		ObserveFrame(C, Frame(true, 0, 0.06f, 9.0f), Cap);
		Check(IsHoldingEventOpen(C), "below the cap the event is held open, frame " + std::to_string(i));
	}
	ObserveFrame(C, Frame(true, 0, 0.06f, 9.0f), Cap);
	Check(C.bCapReached && !IsHoldingEventOpen(C), "the cap closes the event after 5 frames");

	FTracker P;
	P.Mode = EMode::Periodic;
	ObserveFrame(P, Frame(true, 0, 0.06f, 9.0f), S);
	Check(!IsHoldingEventOpen(P), "periodic never holds the event open");
	Check(DecideTick(P, S) == ETickAction::None, "periodic is not driven by the trigger");
}

struct FSimTarget
{
	double X, Y, Z, R;
	float Sizes[8];
	int Tris[8];
	int NumLods;
};

struct FSimPose
{
	double X, Y, Z, Yaw;
};

typedef FSimPose (*FSimPath)(int);

struct FSimFrame
{
	double D;
	float S;
	float Cov;
	bool bOn;
	int Eng;
};

static const double GPi = 3.14159265358979323846;
static const double GM11 = 16.0 / 9.0;

static FSimFrame SimMeasure(const FSimPose& P, const FSimTarget& T)
{
	FSimFrame F;
	const double DX = T.X - P.X, DY = T.Y - P.Y, DZ = T.Z - P.Z;
	F.D = std::sqrt(DX * DX + DY * DY + DZ * DZ);
	F.S = (float)(2.0 * 0.5 * GM11 * T.R / (F.D > 1.0 ? F.D : 1.0));
	const double C = std::cos(-P.Yaw * GPi / 180.0), Sn = std::sin(-P.Yaw * GPi / 180.0);
	double U0 = 1e9, U1 = -1e9, V0 = 1e9, V1 = -1e9;
	int N = 0;
	for (int I = 0; I < 8; ++I)
	{
		const double PX = DX + ((I & 1) ? T.R : -T.R), PY = DY + ((I & 2) ? T.R : -T.R), PZ = DZ + ((I & 4) ? T.R : -T.R);
		const double FX = PX * C - PY * Sn, FY = PX * Sn + PY * C;
		if (FX <= 1.0)
		{
			continue;
		}
		const double U = (FY / FX + 1.0) / 2.0, V = (1.0 - PZ * GM11 / FX) / 2.0;
		U0 = U < U0 ? U : U0; U1 = U > U1 ? U : U1; V0 = V < V0 ? V : V0; V1 = V > V1 ? V : V1;
		++N;
	}
	F.bOn = N > 0 && U1 > 0.0 && U0 < 1.0 && V1 > 0.0 && V0 < 1.0;
	const double W = (U1 < 1.0 ? U1 : 1.0) - (U0 > 0.0 ? U0 : 0.0), H = (V1 < 1.0 ? V1 : 1.0) - (V0 > 0.0 ? V0 : 0.0);
	F.Cov = (N > 0 && W > 0.0 && H > 0.0) ? (float)(W * H * 100.0) : 0.0f;
	F.Eng = EngineLodFromScreenSize(T.Sizes, 8, F.S, 1.0f, 0);
	return F;
}

static double SimNorm(double A)
{
	while (A > 180.0) { A -= 360.0; }
	while (A < -180.0) { A += 360.0; }
	return A;
}

struct FSimResult
{
	int ArmedAt = -1;
	int FirstPositive = -1;
	int Labelled = 0;
	int Swap = -1;
	int TimeoutAt = -1;
	int ClockRestartAt = -1;
	int BeginFires = 0;
	std::string Refusals;
};

static FSimResult Simulate(FSimPath Path, const FSimTarget& T, EMode Mode, const FSettings& S, int Frames, int TimeoutFrames = 90,
	bool bRestart = true)
{
	const float Dt = 1.0f / 30.0f;
	FSimResult R;
	enum { Lead, SFire, Pos, SRev, Post } Phase = Lead;
	int Left = 4;
	bool bArmed = false;
	FTracker Tr;
	FArmClock Clock;
	bool bWindow = false;
	int AppliedAt = -1;
	EFrameClass PrevClass = EFrameClass::Negative;
	bool PrevHoldOpen = false;
	bool bHaveEval = false;
	FFrameInput PrevInput;
	double DistHistory[64] = { 0 };
	auto BeginFire = [&](int Tick)
	{
		++R.BeginFires;
		const FSimPose Now = Path(Tick - 1), Old = Path(Tick - 10);
		const FSimFrame F = SimMeasure(Now, T);
		const FSimFrame FO = SimMeasure(Old, T);
		const char* Why = nullptr;
		int Ahead = 0;
		int Held = -1;
		float Release = -1.0f;
		if (Mode == EMode::DistanceThreshold)
		{
			FDistanceArmInput D;
			D.EngineLod = F.Eng; D.TemporalLod0 = F.Eng; D.TemporalLod1 = F.Eng; D.BoundsScreenSize = F.S;
			D.ReleaseScreenSize = ReleaseScreenSize(T.Sizes, 8, T.Tris, T.NumLods, F.Eng, 1.0f, S);
			D.CoveragePct = F.Cov; D.DistanceCm = (float)F.D; D.FrameSeconds = Dt;
			D.ClosingSpeedCmPerS = (float)((FO.D - F.D) / (9.0 * Dt));
			const double TX = Now.X - Old.X, TY = Now.Y - Old.Y, GX = T.X - Now.X, GY = T.Y - Now.Y;
			const double TL = std::sqrt(TX * TX + TY * TY), GL = std::sqrt(GX * GX + GY * GY);
			D.ApproachAngleDeg = (TL > 0.0 && GL > 0.0) ? (float)(std::acos((TX * GX + TY * GY) / (TL * GL)) * 180.0 / GPi) : 180.0f;
			Why = F.bOn ? DistanceArmRefusal(D, S, Ahead) : "not_in_view";
			Held = F.Eng;
			Release = D.ReleaseScreenSize;
		}
		else
		{
			FViewEntryArmInput V;
			const double Bearing = std::atan2(T.Y - Now.Y, T.X - Now.X) * 180.0 / GPi;
			const double Delta = SimNorm(Bearing - Now.Yaw);
			const double AngR = std::asin(T.R / F.D < 1.0 ? T.R / F.D : 1.0) * 180.0 / GPi;
			V.AngleOutsideFrustumDeg = (float)((Delta < 0 ? -Delta : Delta) - 45.0 - AngR);
			const double Rate = SimNorm(Now.Yaw - Old.Yaw) / (9.0 * Dt);
			V.YawRateDegPerS = (float)(Rate < 0 ? -Rate : Rate);
			V.bYawTowardTarget = Rate != 0.0 && ((Delta > 0.0) == (Rate > 0.0));
			V.FrameSeconds = Dt;
			V.bContrastAtHeld = true;
			V.CoverageAsIfInViewPct = 12.0f;
			Why = ViewEntryArmRefusal(V, S, Ahead);
			Held = T.NumLods - 1;
		}
		if (Why)
		{
			if (R.Refusals.find(Why) == std::string::npos)
			{
				R.Refusals += std::string(R.Refusals.empty() ? "" : ",") + Why;
			}
		}
		else
		{
			bArmed = true;
			R.ArmedAt = R.ArmedAt < 0 ? Tick : R.ArmedAt;
			AppliedAt = Tick;
			Tr = FTracker();
			Tr.Mode = Mode;
			Tr.HeldLod = Held;
			Tr.ReleaseScreenSize = Release;
			Clock = FArmClock();
			bHaveEval = false;
		}
		Phase = SFire;
		Left = 2;
	};
	auto BeginRevert = [&]()
	{
		bArmed = false;
		Phase = SRev;
		Left = 2;
	};
	for (int Tick = 0; Tick < Frames; ++Tick)
	{
		DistHistory[Tick % 64] = SimMeasure(Path(Tick), T).D;
		if (Phase == Lead)
		{
			if (--Left <= 0) { BeginFire(Tick); }
		}
		else if (Phase == SFire)
		{
			if (--Left <= 0) { Phase = Pos; Left = 8; bWindow = false; }
		}
		else if (Phase == Pos)
		{
			if (Left > 0)
			{
				if (bArmed && !bWindow && PrevClass != EFrameClass::Positive)
				{
					const bool bBegun = Tr.bWrongSeen || PrevClass == EFrameClass::Subvisible;
					const bool bWasRestarted = Clock.bRestarted;
					if (StepArmClock(Clock, bRestart && bBegun, TimeoutFrames))
					{
						R.TimeoutAt = Tick;
						BeginRevert();
					}
					if (!bWasRestarted && Clock.bRestarted)
					{
						R.ClockRestartAt = Tick;
					}
				}
				else
				{
					bWindow = true;
					--Left;
				}
			}
			if (Phase == Pos && Left <= 0 && !(bArmed && PrevHoldOpen))
			{
				BeginRevert();
			}
		}
		else if (Phase == SRev)
		{
			if (--Left <= 0) { Phase = Post; Left = 4; }
		}
		else if (Phase == Post)
		{
			if (--Left <= 0) { BeginFire(Tick); }
		}
		if (bArmed && Tick > AppliedAt && bHaveEval)
		{
			ObserveFrame(Tr, PrevInput, S);
			DecideTick(Tr, S);
		}
		if (bArmed && Tick >= AppliedAt)
		{
			const FSimFrame F = SimMeasure(Path(Tick), T);
			FFrameInput In;
			In.bOnScreen = F.bOn; In.bInFrustum = F.bOn; In.EngineLod = F.Eng; In.TemporalLod0 = F.Eng; In.TemporalLod1 = F.Eng;
			In.BoundsScreenSize = F.S; In.CoveragePct = F.Cov;
			In.bContrast = ContrastPasses(T.Tris, T.NumLods, Tr.HeldLod, F.Eng, S.TriangleRatio);
			In.bRetreating = Tick >= 8 && (F.D - DistHistory[(Tick - 8) % 64]) / (8.0 * Dt) >= S.MinClosingSpeedCmPerS;
			FTracker Copy = Tr;
			const FObservation O = ObserveFrame(Copy, In, S);
			if (O.Class == EFrameClass::Positive)
			{
				++R.Labelled;
				if (R.FirstPositive < 0) { R.FirstPositive = Tick; }
			}
			if (O.bSwap && R.Swap < 0) { R.Swap = Tick; }
			PrevClass = O.Class;
			PrevHoldOpen = IsHoldingEventOpen(Copy);
			PrevInput = In;
			bHaveEval = true;
		}
		else
		{
			PrevClass = EFrameClass::Negative;
			PrevHoldOpen = false;
		}
	}
	return R;
}

static FSimPose PathReviewC3(int K)
{
	const double X = K < 20 ? -600.0 : (K < 287 ? -600.0 + 7.5 * (K - 20) : -600.0 + 7.5 * 267);
	return FSimPose{ X, 0.0, 200.0, 0.0 };
}

static FSimPose PathBenchApproach(int K)
{
	double X = 600.0 + 7.5 * (K > 20 ? K - 20 : 0);
	X = X < 1450.0 ? X : 1450.0;
	return FSimPose{ X, 0.0, 200.0, 0.0 };
}

static FSimPose PathBenchTurnIn(int K)
{
	double Yaw = -160.0 + 2.0 * (K > 30 ? K - 30 : 0);
	Yaw = Yaw < 10.0 ? Yaw : 10.0;
	return FSimPose{ 0.0, 0.0, 200.0, Yaw };
}

static FSimPose PathReviewTurnIn(int K)
{
	const double Yaw = K < 85 ? -100.0 + 2.0 * (K > 30 ? K - 30 : 0) : 10.0;
	return FSimPose{ 0.0, 0.0, 200.0, Yaw };
}

static void TestBenchSimulation()
{
	FSettings S;
	S.EntryFrames = 30;
	const FSimTarget Copy = { 1700.0, -60.0, 200.0, 50.0, { 1.0f, 0.05f, 0, 0, 0, 0, 0, 0 }, { 960, 48, 0, 0, 0, 0, 0, 0 }, 2 };
	const FSimTarget Second = { 1700.0, -60.0, 200.0, 50.0, { 1.0f, 0.3f, 0, 0, 0, 0, 0, 0 }, { 960, 48, 0, 0, 0, 0, 0, 0 }, 2 };
	const FSimTarget FixN = { 850.0, -340.0, 200.0, 230.0, { 1.0f, 0.05f, 0, 0, 0, 0, 0, 0 }, { 960, 48, 0, 0, 0, 0, 0, 0 }, 2 };
	const FSimResult Old = Simulate(PathReviewC3, Copy, EMode::DistanceThreshold, S, 287);
	Check(Old.ArmedAt < 0 && Old.TimeoutAt < 0, "review C3 path, copy 0.05 switch: never armed, so no burst times out with nothing shown (refusals "
		+ Old.Refusals + ")");
	Check(Old.Refusals.find("too_far_ahead") != std::string::npos, "review C3 path: refused too_far_ahead while the coarse LOD is still drawn");
	const FSimResult C3 = Simulate(PathBenchApproach, Second, EMode::DistanceThreshold, S, 200);
	Check(C3.ArmedAt == 51, "bench approach (second pair, switch 0.3, path end 257 cm): armed at si 51, got " + std::to_string(C3.ArmedAt));
	Check(C3.FirstPositive == 128 && C3.Labelled == 5 && C3.Swap == 133 && C3.TimeoutAt < 0,
		"bench approach triggers: first positive 128, 5 labelled, swap 133; got " + std::to_string(C3.FirstPositive) + " / "
		+ std::to_string(C3.Labelled) + " / " + std::to_string(C3.Swap) + " / timeout " + std::to_string(C3.TimeoutAt));
	const FSimTarget Slow = { 1700.0, -60.0, 200.0, 50.0, { 1.0f, 0.2f, 0, 0, 0, 0, 0, 0 }, { 960, 48, 0, 0, 0, 0, 0, 0 }, 2 };
	const FSimResult Restart = Simulate(PathBenchApproach, Slow, EMode::DistanceThreshold, S, 200, 60, true);
	Check(Restart.ArmedAt == 51 && Restart.ClockRestartAt > 0 && Restart.TimeoutAt < 0 && Restart.Labelled > 0 && Restart.Swap > 0,
		"switch 0.2, arm timeout 60: the clock restarts at the first flagged frame and the event triggers; restart "
		+ std::to_string(Restart.ClockRestartAt) + ", first positive " + std::to_string(Restart.FirstPositive) + ", swap " + std::to_string(Restart.Swap));
	const FSimResult NoRestart = Simulate(PathBenchApproach, Slow, EMode::DistanceThreshold, S, 200, 60, false);
	Check(NoRestart.TimeoutAt > 0 && NoRestart.FirstPositive < 0,
		"the same leg without the restart times out (at " + std::to_string(NoRestart.TimeoutAt) + ") before the first positive frame");
	const FSimResult TurnOld = Simulate(PathReviewTurnIn, FixN, EMode::ViewEntry, S, 85);
	Check(TurnOld.ArmedAt < 0, "review turn_in (start 18.6 deg outside): no BeginFire lands in the 5-frame window, never armed");
	const FSimResult Turn = Simulate(PathBenchTurnIn, FixN, EMode::ViewEntry, S, 230);
	Check(Turn.ArmedAt == 51 && Turn.FirstPositive == 69 && Turn.Labelled == 30 && Turn.Swap == 99,
		"bench turn_in (start -160, 2 deg/frame): armed 51, first positive 69, 30 labelled, swap 99; got " + std::to_string(Turn.ArmedAt) + " / "
		+ std::to_string(Turn.FirstPositive) + " / " + std::to_string(Turn.Labelled) + " / " + std::to_string(Turn.Swap));
}

static void TestArmClock()
{
	FArmClock C;
	int TimedOutAt = -1;
	for (int F = 1; F <= 200 && TimedOutAt < 0; ++F)
	{
		if (StepArmClock(C, F >= 55, 90))
		{
			TimedOutAt = F;
		}
	}
	Check(C.bRestarted && TimedOutAt == 55 + 89, "the arm clock restarts once at the first wrong frame (55) and times out 90 frames later, got "
		+ std::to_string(TimedOutAt));
	FArmClock N;
	int NoWrong = -1;
	for (int F = 1; F <= 200 && NoWrong < 0; ++F)
	{
		if (StepArmClock(N, false, 90))
		{
			NoWrong = F;
		}
	}
	Check(!N.bRestarted && NoWrong == 90, "with no wrong frame the timeout is unchanged (90)");
}

static void TestDescribe()
{
	Check(std::strcmp(DescribeCause(ESwapCause::Distance), "distance") == 0, "cause distance");
	Check(std::strcmp(DescribeCause(ESwapCause::ViewEntry), "view_entry") == 0, "cause view_entry");
	Check(std::strcmp(DescribeCause(ESwapCause::HoldCap), "hold_cap") == 0, "cause hold_cap");
	Check(std::strcmp(DescribeMode(EMode::DistanceThreshold), "distance_threshold") == 0, "mode distance_threshold");
	Check(std::strcmp(DescribeMode(EMode::ViewEntry), "view_entry") == 0, "mode view_entry");
	Check(std::strcmp(DescribeMode(EMode::Periodic), "periodic") == 0, "mode periodic");
	Check(std::strcmp(DescribeState(EState::Labelled), "labelled") == 0, "state labelled");
}

int main()
{
	TestEngineFormula();
	TestContrast();
	TestThresholds();
	TestFreezeAndClassify();
	TestArmRules();
	TestViewEntryWindow();
	TestApproach();
	TestNoDownPopWhenOff();
	TestViewEntry();
	TestTurnAwayAndCap();
	TestArmClock();
	TestBenchSimulation();
	TestDescribe();
	std::printf("lod_trigger_selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
