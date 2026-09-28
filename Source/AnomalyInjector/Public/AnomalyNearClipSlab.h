#pragma once

#include <cmath>

namespace AnomalyNearClipSlab
{
	struct FV3
	{
		double X = 0.0;
		double Y = 0.0;
		double Z = 0.0;
	};

	inline FV3 MakeV3(double X, double Y, double Z)
	{
		FV3 R;
		R.X = X;
		R.Y = Y;
		R.Z = Z;
		return R;
	}

	inline FV3 Add(const FV3& A, const FV3& B) { return MakeV3(A.X + B.X, A.Y + B.Y, A.Z + B.Z); }
	inline FV3 Sub(const FV3& A, const FV3& B) { return MakeV3(A.X - B.X, A.Y - B.Y, A.Z - B.Z); }
	inline FV3 Mul(const FV3& A, double S) { return MakeV3(A.X * S, A.Y * S, A.Z * S); }
	inline double Dot(const FV3& A, const FV3& B) { return A.X * B.X + A.Y * B.Y + A.Z * B.Z; }
	inline FV3 Cross(const FV3& A, const FV3& B)
	{
		return MakeV3(A.Y * B.Z - A.Z * B.Y, A.Z * B.X - A.X * B.Z, A.X * B.Y - A.Y * B.X);
	}
	inline double LengthSq(const FV3& A) { return Dot(A, A); }

	static constexpr int AxisMaintainY = 0;
	static constexpr int AxisMaintainX = 1;
	static constexpr int AxisMajor = 2;

	struct FTanExtents
	{
		double TanH = 1.0;
		double TanV = 1.0;
	};

	inline FTanExtents ComputeTanExtents(double FovDeg, double PovAspect, bool bConstrainAspect, int SizeX, int SizeY,
		int AxisConstraint, bool bLegacyMaintainY)
	{
		const double Pi = 3.14159265358979323846;
		const double Fov = FovDeg > 0.001 ? FovDeg : 0.001;
		const double TanHalf = std::tan(Fov * Pi / 360.0);
		FTanExtents R;
		if (bConstrainAspect || SizeX <= 0 || SizeY <= 0)
		{
			const double Aspect = PovAspect > 0.0 ? PovAspect
				: ((SizeX > 0 && SizeY > 0) ? (double)SizeX / (double)SizeY : 16.0 / 9.0);
			R.TanH = TanHalf;
			R.TanV = TanHalf / Aspect;
			return R;
		}
		const bool bMaintainX = (SizeX > SizeY && AxisConstraint == AxisMajor) || AxisConstraint == AxisMaintainX;
		if (bMaintainX)
		{
			R.TanH = TanHalf;
			R.TanV = TanHalf * (double)SizeY / (double)SizeX;
			return R;
		}
		double MatrixTan = TanHalf;
		if (PovAspect != 0.0 && !bLegacyMaintainY)
		{
			MatrixTan = TanHalf / PovAspect;
		}
		R.TanV = MatrixTan;
		R.TanH = MatrixTan * (double)SizeX / (double)SizeY;
		return R;
	}

	struct FSlab
	{
		bool bEmpty = true;
		FV3 Corners[8];
		FV3 FaceAxes[5];
		FV3 EdgeDirs[6];
		FV3 AabbMin;
		FV3 AabbMax;
	};

	inline FSlab BuildSlab(const FV3& Origin, const FV3& Forward, const FV3& Right, const FV3& Up,
		double TanH, double TanV, double NearBaseline, double NearAnomalous)
	{
		FSlab S;
		const double N0 = NearBaseline > 0.0 ? NearBaseline : 0.0;
		const double N1 = NearAnomalous;
		if (!(N1 > N0) || !(TanH > 0.0) || !(TanV > 0.0))
		{
			S.bEmpty = true;
			return S;
		}
		S.bEmpty = false;
		const double Depths[2] = { N0, N1 };
		int Index = 0;
		for (int D = 0; D < 2; ++D)
		{
			for (int SX = -1; SX <= 1; SX += 2)
			{
				for (int SY = -1; SY <= 1; SY += 2)
				{
					const double Depth = Depths[D];
					S.Corners[Index++] = Add(Add(Add(Origin, Mul(Forward, Depth)), Mul(Right, SX * TanH * Depth)),
						Mul(Up, SY * TanV * Depth));
				}
			}
		}
		const FV3 RayRight = Add(Forward, Mul(Right, TanH));
		const FV3 RayLeft = Sub(Forward, Mul(Right, TanH));
		const FV3 RayTop = Add(Forward, Mul(Up, TanV));
		const FV3 RayBottom = Sub(Forward, Mul(Up, TanV));
		S.FaceAxes[0] = Forward;
		S.FaceAxes[1] = Cross(Up, RayRight);
		S.FaceAxes[2] = Cross(Up, RayLeft);
		S.FaceAxes[3] = Cross(Right, RayTop);
		S.FaceAxes[4] = Cross(Right, RayBottom);
		S.EdgeDirs[0] = Right;
		S.EdgeDirs[1] = Up;
		S.EdgeDirs[2] = Add(Add(Forward, Mul(Right, TanH)), Mul(Up, TanV));
		S.EdgeDirs[3] = Sub(Add(Forward, Mul(Right, TanH)), Mul(Up, TanV));
		S.EdgeDirs[4] = Add(Sub(Forward, Mul(Right, TanH)), Mul(Up, TanV));
		S.EdgeDirs[5] = Sub(Sub(Forward, Mul(Right, TanH)), Mul(Up, TanV));
		S.AabbMin = S.Corners[0];
		S.AabbMax = S.Corners[0];
		for (int i = 1; i < 8; ++i)
		{
			const FV3& C = S.Corners[i];
			S.AabbMin = MakeV3(C.X < S.AabbMin.X ? C.X : S.AabbMin.X, C.Y < S.AabbMin.Y ? C.Y : S.AabbMin.Y,
				C.Z < S.AabbMin.Z ? C.Z : S.AabbMin.Z);
			S.AabbMax = MakeV3(C.X > S.AabbMax.X ? C.X : S.AabbMax.X, C.Y > S.AabbMax.Y ? C.Y : S.AabbMax.Y,
				C.Z > S.AabbMax.Z ? C.Z : S.AabbMax.Z);
		}
		return S;
	}

	struct FBox3
	{
		FV3 Center;
		FV3 Axis[3];
		double Half[3] = { 0.0, 0.0, 0.0 };
	};

	inline FBox3 MakeAxisAlignedBox(const FV3& Center, const FV3& HalfExtent)
	{
		FBox3 B;
		B.Center = Center;
		B.Axis[0] = MakeV3(1.0, 0.0, 0.0);
		B.Axis[1] = MakeV3(0.0, 1.0, 0.0);
		B.Axis[2] = MakeV3(0.0, 0.0, 1.0);
		B.Half[0] = std::fabs(HalfExtent.X);
		B.Half[1] = std::fabs(HalfExtent.Y);
		B.Half[2] = std::fabs(HalfExtent.Z);
		return B;
	}

	inline void BoxAabb(const FBox3& B, FV3& OutMin, FV3& OutMax)
	{
		double E[3] = { 0.0, 0.0, 0.0 };
		for (int i = 0; i < 3; ++i)
		{
			E[0] += std::fabs(B.Axis[i].X) * B.Half[i];
			E[1] += std::fabs(B.Axis[i].Y) * B.Half[i];
			E[2] += std::fabs(B.Axis[i].Z) * B.Half[i];
		}
		OutMin = MakeV3(B.Center.X - E[0], B.Center.Y - E[1], B.Center.Z - E[2]);
		OutMax = MakeV3(B.Center.X + E[0], B.Center.Y + E[1], B.Center.Z + E[2]);
	}

	inline bool AabbOverlap(const FV3& MinA, const FV3& MaxA, const FV3& MinB, const FV3& MaxB)
	{
		return MinA.X <= MaxB.X && MaxA.X >= MinB.X && MinA.Y <= MaxB.Y && MaxA.Y >= MinB.Y
			&& MinA.Z <= MaxB.Z && MaxA.Z >= MinB.Z;
	}

	inline bool AabbContains(const FV3& OuterMin, const FV3& OuterMax, const FV3& InnerMin, const FV3& InnerMax,
		double Slack)
	{
		return InnerMin.X >= OuterMin.X - Slack && InnerMin.Y >= OuterMin.Y - Slack && InnerMin.Z >= OuterMin.Z - Slack
			&& InnerMax.X <= OuterMax.X + Slack && InnerMax.Y <= OuterMax.Y + Slack && InnerMax.Z <= OuterMax.Z + Slack;
	}

	inline bool SeparatedOnAxis(const FSlab& S, const FBox3& B, const FV3& Axis)
	{
		double SMin = Dot(S.Corners[0], Axis);
		double SMax = SMin;
		for (int i = 1; i < 8; ++i)
		{
			const double P = Dot(S.Corners[i], Axis);
			SMin = P < SMin ? P : SMin;
			SMax = P > SMax ? P : SMax;
		}
		const double C = Dot(B.Center, Axis);
		double R = 0.0;
		for (int i = 0; i < 3; ++i)
		{
			R += std::fabs(Dot(B.Axis[i], Axis)) * B.Half[i];
		}
		return (C + R) < SMin || (C - R) > SMax;
	}

	inline bool BoxIntersectsSlab(const FSlab& S, const FBox3& B)
	{
		if (S.bEmpty)
		{
			return false;
		}
		for (int i = 0; i < 5; ++i)
		{
			if (SeparatedOnAxis(S, B, S.FaceAxes[i]))
			{
				return false;
			}
		}
		for (int i = 0; i < 3; ++i)
		{
			if (SeparatedOnAxis(S, B, B.Axis[i]))
			{
				return false;
			}
		}
		for (int e = 0; e < 6; ++e)
		{
			for (int i = 0; i < 3; ++i)
			{
				const FV3 A = Cross(S.EdgeDirs[e], B.Axis[i]);
				const double Scale = LengthSq(S.EdgeDirs[e]) * LengthSq(B.Axis[i]);
				if (LengthSq(A) <= 1e-12 * Scale)
				{
					continue;
				}
				if (SeparatedOnAxis(S, B, A))
				{
					return false;
				}
			}
		}
		return true;
	}

	inline bool PointInsideBox(const FBox3& B, const FV3& P)
	{
		const FV3 D = Sub(P, B.Center);
		for (int i = 0; i < 3; ++i)
		{
			if (std::fabs(Dot(D, B.Axis[i])) > B.Half[i])
			{
				return false;
			}
		}
		return true;
	}

	inline bool PointInsideSlab(const FSlab& S, const FV3& Origin, const FV3& Forward, const FV3& Right, const FV3& Up,
		double TanH, double TanV, double NearBaseline, double NearAnomalous, const FV3& P)
	{
		if (S.bEmpty)
		{
			return false;
		}
		const FV3 D = Sub(P, Origin);
		const double Depth = Dot(D, Forward);
		const double N0 = NearBaseline > 0.0 ? NearBaseline : 0.0;
		if (Depth < N0 || Depth > NearAnomalous)
		{
			return false;
		}
		return std::fabs(Dot(D, Right)) <= TanH * Depth && std::fabs(Dot(D, Up)) <= TanV * Depth;
	}

	inline bool BoxIntersectsBall(const FBox3& B, const FV3& Center, double Radius)
	{
		const FV3 D = Sub(Center, B.Center);
		double DistSq = 0.0;
		for (int i = 0; i < 3; ++i)
		{
			const double Outside = std::fabs(Dot(D, B.Axis[i])) - B.Half[i];
			if (Outside > 0.0)
			{
				DistSq += Outside * Outside;
			}
		}
		return DistSq <= Radius * Radius;
	}

	struct FPrimitiveFlags
	{
		bool bRegistered = true;
		bool bHasSceneProxy = true;
		bool bVisible = true;
		bool bOwnerHidden = false;
		bool bRenderInMainPass = true;
		bool bSceneCaptureOnly = false;
		bool bIsFxSystem = false;
		bool bOnlyOwnerSee = false;
		bool bOwnerNoSee = false;
		bool bOwnedByViewActor = false;
		double MinDrawDistance = 0.0;
		double DistanceSqToView = 0.0;
	};

	enum class EPrimitiveVerdict : unsigned char
	{
		Counted = 0,
		NotRendered = 1,
		HiddenFromViewer = 2,
		FxExcluded = 3,
		InsideMinDrawDistance = 4
	};

	inline EPrimitiveVerdict ClassifyPrimitive(const FPrimitiveFlags& F)
	{
		if (!F.bRegistered || !F.bHasSceneProxy || !F.bVisible || F.bOwnerHidden || !F.bRenderInMainPass
			|| F.bSceneCaptureOnly)
		{
			return EPrimitiveVerdict::NotRendered;
		}
		if ((F.bOnlyOwnerSee && !F.bOwnedByViewActor) || (F.bOwnerNoSee && F.bOwnedByViewActor))
		{
			return EPrimitiveVerdict::HiddenFromViewer;
		}
		if (F.bIsFxSystem)
		{
			return EPrimitiveVerdict::FxExcluded;
		}
		if (F.MinDrawDistance > 0.0 && F.DistanceSqToView < F.MinDrawDistance * F.MinDrawDistance)
		{
			return EPrimitiveVerdict::InsideMinDrawDistance;
		}
		return EPrimitiveVerdict::Counted;
	}
}
