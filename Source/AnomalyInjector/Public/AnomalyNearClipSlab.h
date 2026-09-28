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

	inline FBox3 MakeInstanceBox(const FV3& MeshCenter, const FV3& MeshHalf, const FV3& UnitX, const FV3& UnitY,
		const FV3& UnitZ, const FV3& Scale, const FV3& Translation)
	{
		FBox3 B;
		const FV3 Scaled = MakeV3(MeshCenter.X * Scale.X, MeshCenter.Y * Scale.Y, MeshCenter.Z * Scale.Z);
		B.Center = Add(Translation, Add(Add(Mul(UnitX, Scaled.X), Mul(UnitY, Scaled.Y)), Mul(UnitZ, Scaled.Z)));
		B.Axis[0] = UnitX;
		B.Axis[1] = UnitY;
		B.Axis[2] = UnitZ;
		B.Half[0] = std::fabs(MeshHalf.X) * std::fabs(Scale.X);
		B.Half[1] = std::fabs(MeshHalf.Y) * std::fabs(Scale.Y);
		B.Half[2] = std::fabs(MeshHalf.Z) * std::fabs(Scale.Z);
		return B;
	}

	struct FAffine34
	{
		FV3 Row[3];
		FV3 Origin;
	};

	inline FAffine34 MakeAffine(const FV3& RowX, const FV3& RowY, const FV3& RowZ, const FV3& Origin)
	{
		FAffine34 M;
		M.Row[0] = RowX;
		M.Row[1] = RowY;
		M.Row[2] = RowZ;
		M.Origin = Origin;
		return M;
	}

	inline FV3 AffineVector(const FAffine34& M, const FV3& V)
	{
		return Add(Add(Mul(M.Row[0], V.X), Mul(M.Row[1], V.Y)), Mul(M.Row[2], V.Z));
	}

	inline FV3 AffinePoint(const FAffine34& M, const FV3& P)
	{
		return Add(AffineVector(M, P), M.Origin);
	}

	inline FAffine34 MulAffine(const FAffine34& First, const FAffine34& Then)
	{
		FAffine34 C;
		for (int i = 0; i < 3; ++i)
		{
			C.Row[i] = AffineVector(Then, First.Row[i]);
		}
		C.Origin = AffinePoint(Then, First.Origin);
		return C;
	}

	inline FV3 NormalizeOr(const FV3& V, const FV3& Fallback)
	{
		const double L = std::sqrt(LengthSq(V));
		return L > 1e-12 ? Mul(V, 1.0 / L) : Fallback;
	}

	inline FBox3 MakeMatrixBox(const FV3& LocalCenter, const FV3& LocalHalf, const FAffine34& M)
	{
		FBox3 B;
		B.Center = AffinePoint(M, LocalCenter);
		const double H[3] = { std::fabs(LocalHalf.X), std::fabs(LocalHalf.Y), std::fabs(LocalHalf.Z) };
		const FV3 E[3] = { Mul(M.Row[0], H[0]), Mul(M.Row[1], H[1]), Mul(M.Row[2], H[2]) };
		int Order[3] = { 0, 1, 2 };
		for (int a = 0; a < 3; ++a)
		{
			for (int b = a + 1; b < 3; ++b)
			{
				if (LengthSq(E[Order[b]]) > LengthSq(E[Order[a]]))
				{
					const int T = Order[a];
					Order[a] = Order[b];
					Order[b] = T;
				}
			}
		}
		const FV3 A0 = NormalizeOr(E[Order[0]], MakeV3(1.0, 0.0, 0.0));
		FV3 A1 = MakeV3(0.0, 0.0, 0.0);
		for (int k = 1; k < 3 && LengthSq(A1) == 0.0; ++k)
		{
			const FV3 Cand = Sub(E[Order[k]], Mul(A0, Dot(E[Order[k]], A0)));
			if (LengthSq(Cand) > 1e-18 * (LengthSq(E[Order[k]]) + 1.0))
			{
				A1 = NormalizeOr(Cand, MakeV3(0.0, 0.0, 0.0));
			}
		}
		if (LengthSq(A1) == 0.0)
		{
			const FV3 Probe = std::fabs(A0.X) < 0.9 ? MakeV3(1.0, 0.0, 0.0) : MakeV3(0.0, 1.0, 0.0);
			A1 = NormalizeOr(Sub(Probe, Mul(A0, Dot(Probe, A0))), MakeV3(0.0, 1.0, 0.0));
		}
		const FV3 A2 = NormalizeOr(Cross(A0, A1), MakeV3(0.0, 0.0, 1.0));
		B.Axis[0] = A0;
		B.Axis[1] = A1;
		B.Axis[2] = A2;
		for (int k = 0; k < 3; ++k)
		{
			B.Half[k] = std::fabs(Dot(E[0], B.Axis[k])) + std::fabs(Dot(E[1], B.Axis[k])) + std::fabs(Dot(E[2], B.Axis[k]));
		}
		return B;
	}

	inline bool InstanceBoxMayTouchSlab(const FSlab& S, const FBox3& InstanceBox)
	{
		if (S.bEmpty)
		{
			return false;
		}
		FV3 Min;
		FV3 Max;
		BoxAabb(InstanceBox, Min, Max);
		return AabbOverlap(S.AabbMin, S.AabbMax, Min, Max) && BoxIntersectsSlab(S, InstanceBox);
	}

	inline bool LegacyIsmQueryKeepsInstance(const FSlab& S, const FV3& InstanceTranslation, const FV3& MeshHalf)
	{
		if (S.bEmpty)
		{
			return false;
		}
		const FV3 E = MakeV3(std::fabs(MeshHalf.X), std::fabs(MeshHalf.Y), std::fabs(MeshHalf.Z));
		return AabbOverlap(S.AabbMin, S.AabbMax, Sub(InstanceTranslation, E), Add(InstanceTranslation, E));
	}

	inline FV3 ViewRayDir(double ScreenU, double ScreenV, const FV3& Forward, const FV3& Right, const FV3& Up,
		double TanH, double TanV)
	{
		return Add(Add(Forward, Mul(Right, ScreenU * TanH)), Mul(Up, ScreenV * TanV));
	}

	inline bool ClipSegmentToBox(const FBox3& B, const FV3& P0, const FV3& P1, double& OutT0, double& OutT1)
	{
		double T0 = 0.0;
		double T1 = 1.0;
		const FV3 D = Sub(P1, P0);
		const FV3 Rel = Sub(P0, B.Center);
		for (int i = 0; i < 3; ++i)
		{
			const double O = Dot(Rel, B.Axis[i]);
			const double Dir = Dot(D, B.Axis[i]);
			const double H = B.Half[i];
			if (std::fabs(Dir) < 1e-12)
			{
				if (std::fabs(O) > H)
				{
					return false;
				}
				continue;
			}
			double Ta = (-H - O) / Dir;
			double Tb = (H - O) / Dir;
			if (Ta > Tb)
			{
				const double Swap = Ta;
				Ta = Tb;
				Tb = Swap;
			}
			T0 = Ta > T0 ? Ta : T0;
			T1 = Tb < T1 ? Tb : T1;
			if (T0 > T1)
			{
				return false;
			}
		}
		OutT0 = T0;
		OutT1 = T1;
		return true;
	}

	inline FV3 LerpV3(const FV3& A, const FV3& B, double T)
	{
		return Add(A, Mul(Sub(B, A), T));
	}

	struct FScreenRect
	{
		double U0 = -1.0;
		double V0 = -1.0;
		double U1 = 1.0;
		double V1 = 1.0;
		bool bFull = true;
	};

	inline FScreenRect BoxFootprint(const FBox3& B, const FV3& Eye, const FV3& Forward, const FV3& Right, const FV3& Up,
		double TanH, double TanV)
	{
		FScreenRect R;
		double U0 = 1e300;
		double V0 = 1e300;
		double U1 = -1e300;
		double V1 = -1e300;
		for (int SX = -1; SX <= 1; SX += 2)
		{
			for (int SY = -1; SY <= 1; SY += 2)
			{
				for (int SZ = -1; SZ <= 1; SZ += 2)
				{
					const FV3 C = Add(Add(Add(B.Center, Mul(B.Axis[0], SX * B.Half[0])), Mul(B.Axis[1], SY * B.Half[1])),
						Mul(B.Axis[2], SZ * B.Half[2]));
					const FV3 D = Sub(C, Eye);
					const double Depth = Dot(D, Forward);
					if (Depth <= 1e-3)
					{
						return R;
					}
					const double U = Dot(D, Right) / (Depth * TanH);
					const double V = Dot(D, Up) / (Depth * TanV);
					U0 = U < U0 ? U : U0;
					V0 = V < V0 ? V : V0;
					U1 = U > U1 ? U : U1;
					V1 = V > V1 ? V : V1;
				}
			}
		}
		R.bFull = false;
		R.U0 = U0 < -1.0 ? -1.0 : U0;
		R.V0 = V0 < -1.0 ? -1.0 : V0;
		R.U1 = U1 > 1.0 ? 1.0 : U1;
		R.V1 = V1 > 1.0 ? 1.0 : V1;
		return R;
	}

	struct FConfirmConfig
	{
		int GridX = 16;
		int GridY = 9;
		int FootX = 4;
		int FootY = 4;
		int MinFootprintRays = 6;
		int MaxCandidates = 16;
		int MaxTraces = 320;
		double MinTraceLength = 1e-3;
	};

	static constexpr double EngineTraceMinLength = 1e-4;

	inline bool PrepareConfirmSegment(const FV3& RayP0, const FV3& RayP1, double T0, double T1, double MinLength,
		FV3& OutA, FV3& OutB, bool& bOutFullSlab)
	{
		bOutFullSlab = false;
		OutA = LerpV3(RayP0, RayP1, T0);
		OutB = LerpV3(RayP0, RayP1, T1);
		if (LengthSq(Sub(OutB, OutA)) > MinLength * MinLength)
		{
			return true;
		}
		if (LengthSq(Sub(RayP1, RayP0)) > MinLength * MinLength)
		{
			OutA = RayP0;
			OutB = RayP1;
			bOutFullSlab = true;
			return true;
		}
		return false;
	}

	static constexpr int MaxConfirmCandidates = 64;

	struct FConfirmCandidate
	{
		FBox3 Box;
		bool bConfirmable = true;
	};

	enum class ECandidateOutcome : unsigned char
	{
		NotNeeded = 0,
		Hit = 1,
		Miss = 2,
		Unconfirmable = 3,
		OverCandidateCap = 4,
		TraceCapped = 5,
		NoRay = 6,
		TooFewValidTraces = 7
	};

	inline const char* DescribeOutcome(ECandidateOutcome O)
	{
		switch (O)
		{
		case ECandidateOutcome::Hit:              return "hit";
		case ECandidateOutcome::Miss:             return "miss";
		case ECandidateOutcome::Unconfirmable:    return "unconfirmable";
		case ECandidateOutcome::OverCandidateCap: return "over_candidate_cap";
		case ECandidateOutcome::TraceCapped:      return "trace_capped";
		case ECandidateOutcome::NoRay:            return "no_ray";
		case ECandidateOutcome::TooFewValidTraces: return "too_few_valid_traces";
		default:                                  return "not_needed";
		}
	}

	struct FConfirmResult
	{
		bool bPositive = false;
		bool bUnconfirmed = false;
		int GlobalRays = 0;
		int GlobalRaysHit = 0;
		int Traces = 0;
		int Hits = 0;
		int Misses = 0;
		int Unconfirmable = 0;
		int OverCap = 0;
		int TraceCapped = 0;
		int NoRay = 0;
		int TooFewValid = 0;
		int FullSlabFallbacks = 0;
		int InvalidSegments = 0;
		double ClippedRayFraction = 0.0;
	};

	template <typename TraceFn>
	FConfirmResult ConfirmSlab(const FV3& Eye, const FV3& Forward, const FV3& Right, const FV3& Up, double TanH,
		double TanV, double NearBaseline, double NearAnomalous, const FConfirmCandidate* Candidates, int NumCandidates,
		ECandidateOutcome* OutOutcome, const FConfirmConfig& Cfg, TraceFn&& Trace)
	{
		FConfirmResult Res;
		const double N0 = NearBaseline > 0.0 ? NearBaseline : 0.0;
		const double N1 = NearAnomalous;
		const int Cap = Cfg.MaxCandidates < MaxConfirmCandidates ? Cfg.MaxCandidates : MaxConfirmCandidates;
		int Foot[MaxConfirmCandidates] = {};
		int Valid[MaxConfirmCandidates] = {};
		bool bCapped[MaxConfirmCandidates] = {};
		const int NeededValid = Cfg.MinFootprintRays > 0 ? Cfg.MinFootprintRays : 1;
		for (int c = 0; c < NumCandidates; ++c)
		{
			if (c >= Cap)
			{
				OutOutcome[c] = ECandidateOutcome::OverCandidateCap;
			}
			else
			{
				OutOutcome[c] = Candidates[c].bConfirmable ? ECandidateOutcome::NotNeeded : ECandidateOutcome::Unconfirmable;
			}
		}
		const int Limit = NumCandidates < Cap ? NumCandidates : Cap;
		bool bAnyHit = false;
		const int Gx = Cfg.GridX > 0 ? Cfg.GridX : 1;
		const int Gy = Cfg.GridY > 0 ? Cfg.GridY : 1;
		for (int Iy = 0; Iy < Gy; ++Iy)
		{
			for (int Ix = 0; Ix < Gx; ++Ix)
			{
				const double U = -1.0 + (2.0 * Ix + 1.0) / Gx;
				const double V = -1.0 + (2.0 * Iy + 1.0) / Gy;
				const FV3 Dir = ViewRayDir(U, V, Forward, Right, Up, TanH, TanV);
				const FV3 P0 = Add(Eye, Mul(Dir, N0));
				const FV3 P1 = Add(Eye, Mul(Dir, N1));
				++Res.GlobalRays;
				bool bRayHit = false;
				for (int c = 0; c < Limit; ++c)
				{
					if (!Candidates[c].bConfirmable)
					{
						continue;
					}
					double T0 = 0.0;
					double T1 = 0.0;
					if (!ClipSegmentToBox(Candidates[c].Box, P0, P1, T0, T1))
					{
						continue;
					}
					++Foot[c];
					if (bRayHit)
					{
						continue;
					}
					FV3 SegA;
					FV3 SegB;
					bool bFullSlab = false;
					if (!PrepareConfirmSegment(P0, P1, T0, T1, Cfg.MinTraceLength, SegA, SegB, bFullSlab))
					{
						++Res.InvalidSegments;
						continue;
					}
					if (Res.Traces >= Cfg.MaxTraces)
					{
						bCapped[c] = true;
						continue;
					}
					++Res.Traces;
					++Valid[c];
					Res.FullSlabFallbacks += bFullSlab ? 1 : 0;
					if (Trace(c, SegA, SegB))
					{
						bRayHit = true;
						bAnyHit = true;
						OutOutcome[c] = ECandidateOutcome::Hit;
					}
				}
				if (bRayHit)
				{
					++Res.GlobalRaysHit;
				}
			}
		}
		if (!bAnyHit)
		{
			for (int c = 0; c < Limit && !bAnyHit; ++c)
			{
				if (!Candidates[c].bConfirmable || Valid[c] >= Cfg.MinFootprintRays || Cfg.FootX <= 0 || Cfg.FootY <= 0)
				{
					continue;
				}
				const FScreenRect Rect = BoxFootprint(Candidates[c].Box, Eye, Forward, Right, Up, TanH, TanV);
				if (!Rect.bFull && (Rect.U0 > Rect.U1 || Rect.V0 > Rect.V1))
				{
					continue;
				}
				for (int Iy = 0; Iy < Cfg.FootY && !bAnyHit; ++Iy)
				{
					for (int Ix = 0; Ix < Cfg.FootX && !bAnyHit; ++Ix)
					{
						const double U = Rect.U0 + (Rect.U1 - Rect.U0) * (Ix + 0.5) / Cfg.FootX;
						const double V = Rect.V0 + (Rect.V1 - Rect.V0) * (Iy + 0.5) / Cfg.FootY;
						const FV3 Dir = ViewRayDir(U, V, Forward, Right, Up, TanH, TanV);
						const FV3 P0 = Add(Eye, Mul(Dir, N0));
						const FV3 P1 = Add(Eye, Mul(Dir, N1));
						double T0 = 0.0;
						double T1 = 0.0;
						if (!ClipSegmentToBox(Candidates[c].Box, P0, P1, T0, T1))
						{
							continue;
						}
						++Foot[c];
						FV3 SegA;
						FV3 SegB;
						bool bFullSlab = false;
						if (!PrepareConfirmSegment(P0, P1, T0, T1, Cfg.MinTraceLength, SegA, SegB, bFullSlab))
						{
							++Res.InvalidSegments;
							continue;
						}
						if (Res.Traces >= Cfg.MaxTraces)
						{
							bCapped[c] = true;
							continue;
						}
						++Res.Traces;
						++Valid[c];
						Res.FullSlabFallbacks += bFullSlab ? 1 : 0;
						if (Trace(c, SegA, SegB))
						{
							bAnyHit = true;
							OutOutcome[c] = ECandidateOutcome::Hit;
						}
					}
				}
			}
		}
		for (int c = 0; c < NumCandidates; ++c)
		{
			if (c < Limit && Candidates[c].bConfirmable && OutOutcome[c] == ECandidateOutcome::NotNeeded && !bAnyHit)
			{
				if (bCapped[c])
				{
					OutOutcome[c] = ECandidateOutcome::TraceCapped;
				}
				else if (Foot[c] == 0)
				{
					OutOutcome[c] = ECandidateOutcome::NoRay;
				}
				else if (Valid[c] < NeededValid)
				{
					OutOutcome[c] = ECandidateOutcome::TooFewValidTraces;
				}
				else
				{
					OutOutcome[c] = ECandidateOutcome::Miss;
				}
			}
			switch (OutOutcome[c])
			{
			case ECandidateOutcome::Hit:              ++Res.Hits; break;
			case ECandidateOutcome::Miss:             ++Res.Misses; break;
			case ECandidateOutcome::Unconfirmable:    ++Res.Unconfirmable; break;
			case ECandidateOutcome::OverCandidateCap: ++Res.OverCap; break;
			case ECandidateOutcome::TraceCapped:      ++Res.TraceCapped; break;
			case ECandidateOutcome::NoRay:            ++Res.NoRay; break;
			case ECandidateOutcome::TooFewValidTraces: ++Res.TooFewValid; break;
			default: break;
			}
		}
		const bool bUncertain = Res.Unconfirmable + Res.OverCap + Res.TraceCapped + Res.NoRay + Res.TooFewValid > 0;
		Res.bPositive = bAnyHit || bUncertain;
		Res.bUnconfirmed = !bAnyHit && bUncertain;
		Res.ClippedRayFraction = Res.GlobalRays > 0 ? (double)Res.GlobalRaysHit / (double)Res.GlobalRays : 0.0;
		return Res;
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
