#pragma once

#include <cmath>

namespace AnomalyLodTriggerRule
{
	enum class EMode : unsigned char { Periodic = 0, DistanceThreshold = 1, ViewEntry = 2, Auto = 3 };
	enum class EFrameClass : unsigned char { Negative = 0, Positive = 1, Subvisible = 2 };
	enum class ESwapCause : unsigned char { None = 0, Distance = 1, ViewEntry = 2, HoldCap = 3 };
	enum class EState : unsigned char { Armed = 0, Labelled = 1, Swapped = 2, Released = 3 };
	enum class ETickAction : unsigned char { None = 0, ReleaseVisible = 1, ReHold = 2, ReleaseInvisible = 3 };

	static constexpr int MotionEstimateFrames = 8;

	inline const char* DescribeMode(EMode Mode)
	{
		return Mode == EMode::Periodic ? "periodic"
			: Mode == EMode::DistanceThreshold ? "distance_threshold"
			: Mode == EMode::ViewEntry ? "view_entry" : "auto";
	}

	inline const char* DescribeCause(ESwapCause Cause)
	{
		return Cause == ESwapCause::Distance ? "distance"
			: Cause == ESwapCause::ViewEntry ? "view_entry"
			: Cause == ESwapCause::HoldCap ? "hold_cap" : "none";
	}

	inline const char* DescribeState(EState State)
	{
		return State == EState::Labelled ? "labelled"
			: State == EState::Swapped ? "swapped"
			: State == EState::Released ? "released" : "armed";
	}

	inline const char* DescribeFrameClass(EFrameClass Class)
	{
		return Class == EFrameClass::Positive ? "positive" : Class == EFrameClass::Subvisible ? "subvisible" : "negative";
	}

	struct FSettings
	{
		float CoverageMinPct = 7.0f;
		int TriangleRatio = 4;
		float ReleaseMargin = 0.10f;
		float Hysteresis = 0.85f;
		float MinClosingSpeedCmPerS = 50.0f;
		float MaxApproachAngleDeg = 30.0f;
		int AheadFrames = 90;
		float ViewEntryMarginDeg = 20.0f;
		float ViewEntryMarginMaxDeg = 120.0f;
		float MinYawRateDegPerS = 30.0f;
		int EntryFrames = 4;
		int HoldCapFrames = 150;
		bool bDownPop = true;
	};

	inline int EngineLodFromScreenSize(const float* ScreenSizes, int NumEntries, float BoundsScreenSize, float Scale, int MinLod)
	{
		const float Radius = BoundsScreenSize * 0.5f;
		const float RadiusSquared = Radius * Radius;
		for (int LodIndex = NumEntries - 1; LodIndex >= 0; --LodIndex)
		{
			const float MeshRadius = ScreenSizes[LodIndex] * Scale * 0.5f;
			if (MeshRadius * MeshRadius > RadiusSquared)
			{
				return LodIndex > MinLod ? LodIndex : MinLod;
			}
		}
		return MinLod;
	}

	inline int FinestContrastLod(const int* Triangles, int NumLods, int HeldLod, int Ratio)
	{
		if (HeldLod <= 0 || HeldLod >= NumLods || Ratio < 1)
		{
			return -1;
		}
		for (int LodIndex = HeldLod - 1; LodIndex >= 0; --LodIndex)
		{
			if ((long long)Triangles[HeldLod] * Ratio <= (long long)Triangles[LodIndex])
			{
				return LodIndex;
			}
		}
		return -1;
	}

	inline bool ContrastPasses(const int* Triangles, int NumLods, int HeldLod, int EngineLod, int Ratio)
	{
		return Ratio >= 1 && EngineLod >= 0 && EngineLod < HeldLod && HeldLod < NumLods
			&& (long long)Triangles[HeldLod] * Ratio <= (long long)Triangles[EngineLod];
	}

	inline float ScreenSizeForEngineLodAtMost(const float* ScreenSizes, int NumEntries, int Lod, float Scale)
	{
		if (Lod < 0 || Lod + 1 >= NumEntries)
		{
			return -1.0f;
		}
		return ScreenSizes[Lod + 1] * Scale;
	}

	inline float ReleaseScreenSize(const float* ScreenSizes, int NumEntries, const int* Triangles, int NumLods, int HeldLod,
		float Scale, const FSettings& Settings)
	{
		const int Contrast = FinestContrastLod(Triangles, NumLods, HeldLod, Settings.TriangleRatio);
		const float Threshold = ScreenSizeForEngineLodAtMost(ScreenSizes, NumEntries, Contrast, Scale);
		return Threshold > 0.0f ? Threshold * (1.0f + Settings.ReleaseMargin) : -1.0f;
	}

	inline bool FreezeIsInvisible(int EngineLod, int TemporalLod0, int TemporalLod1)
	{
		return EngineLod >= 1 && TemporalLod0 == EngineLod && TemporalLod1 == EngineLod;
	}

	inline bool TemporalAgree(int TemporalLod0, int TemporalLod1)
	{
		return TemporalLod0 >= 0 && TemporalLod0 == TemporalLod1;
	}

	inline EFrameClass ClassifyFrame(bool bHeld, bool bOnScreen, int HeldLod, int EngineLod, float CoveragePct, float CoverageMinPct)
	{
		if (!bHeld || !bOnScreen || EngineLod < 0 || EngineLod >= HeldLod)
		{
			return EFrameClass::Negative;
		}
		return CoveragePct >= CoverageMinPct ? EFrameClass::Positive : EFrameClass::Subvisible;
	}

	struct FDistanceArmInput
	{
		int EngineLod = -1;
		int TemporalLod0 = -1;
		int TemporalLod1 = -1;
		float BoundsScreenSize = 0.0f;
		float ReleaseScreenSize = -1.0f;
		float CoveragePct = 0.0f;
		float DistanceCm = 0.0f;
		float FrameSeconds = 0.0f;
		float ClosingSpeedCmPerS = 0.0f;
		float ApproachAngleDeg = 180.0f;
	};

	inline float CoverageScreenSize(float BoundsScreenSize, float CoveragePct, float CoverageMinPct)
	{
		if (BoundsScreenSize <= 0.0f)
		{
			return -1.0f;
		}
		if (CoveragePct >= CoverageMinPct)
		{
			return BoundsScreenSize;
		}
		if (CoveragePct <= 0.0f)
		{
			return -1.0f;
		}
		return BoundsScreenSize * std::sqrt(CoverageMinPct / CoveragePct);
	}

	inline float DistanceLookAheadScreenSize(float ReleaseScreenSize, float CoverageSize)
	{
		return ReleaseScreenSize > CoverageSize ? ReleaseScreenSize : CoverageSize;
	}

	inline const char* DistanceArmRefusal(const FDistanceArmInput& In, const FSettings& Settings, int& OutFramesAhead,
		float* OutLookAheadScreenSize = nullptr)
	{
		OutFramesAhead = -1;
		if (OutLookAheadScreenSize)
		{
			*OutLookAheadScreenSize = -1.0f;
		}
		if (!FreezeIsInvisible(In.EngineLod, In.TemporalLod0, In.TemporalLod1))
		{
			return In.EngineLod < 1 ? "at_finest_lod" : "mid_dither";
		}
		if (In.ReleaseScreenSize <= 0.0f)
		{
			return "no_contrast_lod";
		}
		const float CoverageSize = CoverageScreenSize(In.BoundsScreenSize, In.CoveragePct, Settings.CoverageMinPct);
		if (CoverageSize <= 0.0f)
		{
			return "no_coverage";
		}
		const float Target = DistanceLookAheadScreenSize(In.ReleaseScreenSize, CoverageSize);
		if (OutLookAheadScreenSize)
		{
			*OutLookAheadScreenSize = Target;
		}
		if (Target <= In.BoundsScreenSize)
		{
			return "already_past_release";
		}
		if (In.ClosingSpeedCmPerS < Settings.MinClosingSpeedCmPerS || In.FrameSeconds <= 0.0f || In.DistanceCm <= 0.0f)
		{
			return "not_approaching";
		}
		if (In.ApproachAngleDeg > Settings.MaxApproachAngleDeg)
		{
			return "off_motion_axis";
		}
		const float TargetDistanceCm = In.DistanceCm * In.BoundsScreenSize / Target;
		const float Frames = (In.DistanceCm - TargetDistanceCm) / (In.ClosingSpeedCmPerS * In.FrameSeconds);
		OutFramesAhead = (int)(Frames + 0.999f);
		if (OutFramesAhead > Settings.AheadFrames)
		{
			return "too_far_ahead";
		}
		return nullptr;
	}

	struct FViewEntryArmInput
	{
		float AngleOutsideFrustumDeg = -1.0f;
		bool bYawTowardTarget = false;
		float YawRateDegPerS = 0.0f;
		float FrameSeconds = 0.0f;
		bool bShadowInView = false;
		bool bContrastAtHeld = false;
		float CoverageAsIfInViewPct = 0.0f;
	};

	inline float ViewEntryMarginForRate(const FSettings& Settings, float YawRateDegPerS, float FrameSeconds)
	{
		const float Ahead = YawRateDegPerS * FrameSeconds * (float)Settings.AheadFrames;
		const float Floored = Ahead > Settings.ViewEntryMarginDeg ? Ahead : Settings.ViewEntryMarginDeg;
		return Floored < Settings.ViewEntryMarginMaxDeg ? Floored : Settings.ViewEntryMarginMaxDeg;
	}

	inline const char* ViewEntryArmRefusal(const FViewEntryArmInput& In, const FSettings& Settings, int& OutFramesAhead)
	{
		OutFramesAhead = -1;
		if (In.AngleOutsideFrustumDeg <= 0.0f)
		{
			return "already_in_view";
		}
		if (!In.bYawTowardTarget || In.YawRateDegPerS < Settings.MinYawRateDegPerS || In.FrameSeconds <= 0.0f)
		{
			return "not_turning_toward";
		}
		if (In.AngleOutsideFrustumDeg > ViewEntryMarginForRate(Settings, In.YawRateDegPerS, In.FrameSeconds))
		{
			return "too_far_outside_view";
		}
		if (In.bShadowInView)
		{
			return "lod_shadow_in_view";
		}
		if (!In.bContrastAtHeld)
		{
			return "no_contrast_lod";
		}
		if (In.CoverageAsIfInViewPct < Settings.CoverageMinPct)
		{
			return "too_small_in_view";
		}
		const float Frames = In.AngleOutsideFrustumDeg / (In.YawRateDegPerS * In.FrameSeconds);
		OutFramesAhead = (int)(Frames + 0.999f);
		if (OutFramesAhead > Settings.AheadFrames)
		{
			return "too_far_ahead";
		}
		return nullptr;
	}

	struct FFrameInput
	{
		bool bOnScreen = false;
		int EngineLod = -1;
		int TemporalLod0 = -1;
		int TemporalLod1 = -1;
		float BoundsScreenSize = 0.0f;
		float CoveragePct = 0.0f;
		bool bContrast = false;
		bool bRetreating = false;
	};

	struct FTracker
	{
		EMode Mode = EMode::DistanceThreshold;
		EState State = EState::Armed;
		bool bHeld = true;
		int HeldLod = 1;
		float ReleaseScreenSize = -1.0f;
		float SwapScreenSize = -1.0f;
		float FirstPositiveScreenSize = -1.0f;
		int PositiveFramesSeen = 0;
		int FramesSinceFirstPositive = -1;
		bool bCapReached = false;
		bool bReheld = false;
		bool bWrongSeen = false;
		bool bHasLast = false;
		FFrameInput Last;
		EFrameClass LastClass = EFrameClass::Negative;
		ESwapCause PendingCause = ESwapCause::None;
		bool bPendingDown = false;
	};

	struct FObservation
	{
		EFrameClass Class = EFrameClass::Negative;
		bool bSwap = false;
		ESwapCause Cause = ESwapCause::None;
		bool bDown = false;
		int FromLod = -1;
		int ToLod = -1;
	};

	inline FObservation ObserveFrame(FTracker& T, const FFrameInput& In, const FSettings& Settings)
	{
		FObservation O;
		O.Class = ClassifyFrame(T.bHeld, In.bOnScreen, T.HeldLod, In.EngineLod, In.CoveragePct, Settings.CoverageMinPct);
		if (T.PendingCause != ESwapCause::None)
		{
			if (In.bOnScreen && In.EngineLod >= 0 && In.EngineLod != T.HeldLod)
			{
				O.bSwap = true;
				O.Cause = T.PendingCause;
				O.bDown = T.bPendingDown;
				O.FromLod = T.bPendingDown ? In.EngineLod : T.HeldLod;
				O.ToLod = T.bPendingDown ? T.HeldLod : In.EngineLod;
			}
			T.PendingCause = ESwapCause::None;
			T.bPendingDown = false;
		}
		if (T.FramesSinceFirstPositive >= 0)
		{
			++T.FramesSinceFirstPositive;
		}
		if (O.Class == EFrameClass::Positive || O.Class == EFrameClass::Subvisible)
		{
			T.bWrongSeen = true;
		}
		if (O.Class == EFrameClass::Positive)
		{
			++T.PositiveFramesSeen;
			if (T.FramesSinceFirstPositive < 0)
			{
				T.FramesSinceFirstPositive = 1;
				T.FirstPositiveScreenSize = In.BoundsScreenSize;
			}
			T.State = EState::Labelled;
		}
		else if (T.bHeld)
		{
			T.State = EState::Armed;
		}
		else if (T.State != EState::Released)
		{
			T.State = EState::Swapped;
		}
		if (Settings.HoldCapFrames > 0 && T.FramesSinceFirstPositive >= Settings.HoldCapFrames && O.Class == EFrameClass::Positive)
		{
			T.bCapReached = true;
		}
		T.Last = In;
		T.LastClass = O.Class;
		T.bHasLast = true;
		return O;
	}

	struct FArmClock
	{
		int WaitedFrames = 0;
		bool bRestarted = false;
	};

	inline bool StepArmClock(FArmClock& Clock, bool bWrongSeen, int TimeoutFrames)
	{
		if (bWrongSeen && !Clock.bRestarted)
		{
			Clock.bRestarted = true;
			Clock.WaitedFrames = 0;
		}
		++Clock.WaitedFrames;
		return Clock.WaitedFrames >= TimeoutFrames;
	}

	inline bool IsHoldingEventOpen(const FTracker& T)
	{
		return T.bHeld && T.bHasLast && T.LastClass == EFrameClass::Positive && !T.bCapReached && T.Mode != EMode::Periodic;
	}

	inline ETickAction DecideTick(FTracker& T, const FSettings& Settings)
	{
		if (!T.bHasLast || T.State == EState::Released || T.Mode == EMode::Periodic)
		{
			return ETickAction::None;
		}
		const FFrameInput& In = T.Last;
		const bool bAgree = TemporalAgree(In.TemporalLod0, In.TemporalLod1);
		if (T.bHeld)
		{
			if (T.Mode == EMode::DistanceThreshold && T.LastClass == EFrameClass::Positive && In.bContrast && bAgree
				&& T.ReleaseScreenSize > 0.0f && In.BoundsScreenSize >= T.ReleaseScreenSize
				&& In.BoundsScreenSize >= T.FirstPositiveScreenSize * (1.0f + Settings.ReleaseMargin))
			{
				T.bHeld = false;
				T.SwapScreenSize = In.BoundsScreenSize;
				T.PendingCause = ESwapCause::Distance;
				T.bPendingDown = false;
				T.State = EState::Swapped;
				return ETickAction::ReleaseVisible;
			}
			if (T.Mode == EMode::ViewEntry && T.LastClass == EFrameClass::Positive && bAgree
				&& T.PositiveFramesSeen >= Settings.EntryFrames)
			{
				T.bHeld = false;
				T.SwapScreenSize = In.BoundsScreenSize;
				T.PendingCause = ESwapCause::ViewEntry;
				T.bPendingDown = false;
				T.State = EState::Swapped;
				return ETickAction::ReleaseVisible;
			}
			if (T.Mode == EMode::DistanceThreshold && T.bReheld && T.LastClass == EFrameClass::Negative && In.bOnScreen
				&& In.EngineLod == T.HeldLod && bAgree)
			{
				T.bHeld = false;
				T.bReheld = false;
				T.State = EState::Swapped;
				return ETickAction::ReleaseInvisible;
			}
			return ETickAction::None;
		}
		if (T.Mode == EMode::DistanceThreshold && Settings.bDownPop && T.SwapScreenSize > 0.0f && In.bOnScreen && In.bRetreating
			&& bAgree && In.EngineLod >= 0 && In.EngineLod < T.HeldLod && In.CoveragePct >= Settings.CoverageMinPct
			&& In.BoundsScreenSize < T.SwapScreenSize * Settings.Hysteresis)
		{
			T.bHeld = true;
			T.bReheld = true;
			T.PendingCause = ESwapCause::Distance;
			T.bPendingDown = true;
			return ETickAction::ReHold;
		}
		return ETickAction::None;
	}
}
