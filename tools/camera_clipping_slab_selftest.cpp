#include "../Source/AnomalyInjector/Public/AnomalyNearClipSlab.h"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <random>
#include <string>
#include <vector>

using namespace AnomalyNearClipSlab;

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

struct FCamera
{
	FV3 Origin;
	FV3 Forward;
	FV3 Right;
	FV3 Up;
	double TanH = 1.0;
	double TanV = 0.5625;
	double N0 = 10.0;
	double N1 = 100.0;
};

static FCamera DefaultCamera()
{
	FCamera C;
	C.Origin = MakeV3(0.0, 0.0, 0.0);
	C.Forward = MakeV3(1.0, 0.0, 0.0);
	C.Right = MakeV3(0.0, 1.0, 0.0);
	C.Up = MakeV3(0.0, 0.0, 1.0);
	const FTanExtents T = ComputeTanExtents(90.0, 16.0 / 9.0, false, 1280, 720, AxisMaintainX, false);
	C.TanH = T.TanH;
	C.TanV = T.TanV;
	return C;
}

static FSlab SlabOf(const FCamera& C)
{
	return BuildSlab(C.Origin, C.Forward, C.Right, C.Up, C.TanH, C.TanV, C.N0, C.N1);
}

static bool InSlab(const FCamera& C, const FSlab& S, const FV3& P)
{
	return PointInsideSlab(S, C.Origin, C.Forward, C.Right, C.Up, C.TanH, C.TanV, C.N0, C.N1, P);
}

static bool BruteForceIntersects(const FCamera& C, const FSlab& S, const FBox3& B, int N)
{
	if (S.bEmpty)
	{
		return false;
	}
	for (int i = 0; i <= N; ++i)
	{
		for (int j = 0; j <= N; ++j)
		{
			for (int k = 0; k <= N; ++k)
			{
				const double U = -1.0 + 2.0 * i / N;
				const double V = -1.0 + 2.0 * j / N;
				const double W = -1.0 + 2.0 * k / N;
				const FV3 P = Add(Add(Add(B.Center, Mul(B.Axis[0], U * B.Half[0])), Mul(B.Axis[1], V * B.Half[1])),
					Mul(B.Axis[2], W * B.Half[2]));
				if (InSlab(C, S, P))
				{
					return true;
				}
			}
		}
	}
	const double N0 = C.N0 > 0.0 ? C.N0 : 0.0;
	for (int i = 0; i <= N; ++i)
	{
		for (int j = 0; j <= N; ++j)
		{
			for (int k = 0; k <= N; ++k)
			{
				const double Depth = N0 + (C.N1 - N0) * i / N;
				const double X = (-1.0 + 2.0 * j / N) * C.TanH * Depth;
				const double Y = (-1.0 + 2.0 * k / N) * C.TanV * Depth;
				const FV3 P = Add(Add(Add(C.Origin, Mul(C.Forward, Depth)), Mul(C.Right, X)), Mul(C.Up, Y));
				if (PointInsideBox(B, P))
				{
					return true;
				}
			}
		}
	}
	return false;
}

struct FScenePrimitive
{
	std::string Name;
	FBox3 Box;
	FPrimitiveFlags Flags;
	bool bCollidesVisibility = true;
	bool bOwnedByPawn = false;
};

static bool NewSlabLabel(const FCamera& C, const std::vector<FScenePrimitive>& Scene)
{
	const FSlab S = SlabOf(C);
	for (const FScenePrimitive& P : Scene)
	{
		if (ClassifyPrimitive(P.Flags) != EPrimitiveVerdict::Counted)
		{
			continue;
		}
		if (BoxIntersectsSlab(S, P.Box))
		{
			return true;
		}
	}
	return false;
}

static bool NewSlabLabelWorldAabbOnly(const FCamera& C, const std::vector<FScenePrimitive>& Scene)
{
	const FSlab S = SlabOf(C);
	for (const FScenePrimitive& P : Scene)
	{
		if (ClassifyPrimitive(P.Flags) != EPrimitiveVerdict::Counted)
		{
			continue;
		}
		FV3 Mn, Mx;
		BoxAabb(P.Box, Mn, Mx);
		const FBox3 Aabb = MakeAxisAlignedBox(Mul(Add(Mn, Mx), 0.5), Mul(Sub(Mx, Mn), 0.5));
		if (BoxIntersectsSlab(S, Aabb))
		{
			return true;
		}
	}
	return false;
}

static bool NewSlabLabelFrustumRemoved(const FCamera& C, const std::vector<FScenePrimitive>& Scene)
{
	for (const FScenePrimitive& P : Scene)
	{
		if (ClassifyPrimitive(P.Flags) != EPrimitiveVerdict::Counted)
		{
			continue;
		}
		if (BoxIntersectsBall(P.Box, C.Origin, C.N1))
		{
			return true;
		}
	}
	return false;
}

static bool LegacySphereProxy(const FCamera& C, const std::vector<FScenePrimitive>& Scene)
{
	if (C.N1 <= 0.0)
	{
		return false;
	}
	for (const FScenePrimitive& P : Scene)
	{
		if (!P.bCollidesVisibility || P.bOwnedByPawn)
		{
			continue;
		}
		if (BoxIntersectsBall(P.Box, C.Origin, C.N1))
		{
			return true;
		}
	}
	return false;
}

static bool PictureTruth(const FCamera& C, const std::vector<FScenePrimitive>& Scene)
{
	const FSlab S = SlabOf(C);
	for (const FScenePrimitive& P : Scene)
	{
		if (ClassifyPrimitive(P.Flags) != EPrimitiveVerdict::Counted)
		{
			continue;
		}
		if (BruteForceIntersects(C, S, P.Box, 40))
		{
			return true;
		}
	}
	return false;
}

static FScenePrimitive Cube(const std::string& Name, const FV3& Center, double Half)
{
	FScenePrimitive P;
	P.Name = Name;
	P.Box = MakeAxisAlignedBox(Center, MakeV3(Half, Half, Half));
	return P;
}

static FScenePrimitive WallFacingCamera(const std::string& Name, double FaceDepth)
{
	FScenePrimitive P;
	P.Name = Name;
	P.Box = MakeAxisAlignedBox(MakeV3(FaceDepth + 5.0, 0.0, 0.0), MakeV3(5.0, 600.0, 400.0));
	return P;
}

struct FCase
{
	std::string Name;
	std::vector<FScenePrimitive> Scene;
	FCamera Camera;
	bool bExpectedTruth = false;
	bool bExpectedLegacy = false;
};

static void RunCases()
{
	std::vector<FCase> Cases;
	const FCamera Cam = DefaultCamera();

	{
		FCase C; C.Name = "wall_face_150_in_front"; C.Camera = Cam; C.Scene.push_back(WallFacingCamera("wall", 150.0));
		C.bExpectedTruth = false; C.bExpectedLegacy = false; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "wall_face_50_in_front"; C.Camera = Cam; C.Scene.push_back(WallFacingCamera("wall", 50.0));
		C.bExpectedTruth = true; C.bExpectedLegacy = true; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "wall_face_97.5_edge_in"; C.Camera = Cam; C.Scene.push_back(WallFacingCamera("wall", 97.5));
		C.bExpectedTruth = true; C.bExpectedLegacy = true; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "wall_face_102.5_edge_out"; C.Camera = Cam; C.Scene.push_back(WallFacingCamera("wall", 102.5));
		C.bExpectedTruth = false; C.bExpectedLegacy = false; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "behind_wall_face_50_behind"; C.Camera = Cam;
		FScenePrimitive W; W.Name = "wall"; W.Box = MakeAxisAlignedBox(MakeV3(-55.0, 0.0, 0.0), MakeV3(5.0, 600.0, 400.0));
		C.Scene.push_back(W);
		C.bExpectedTruth = false; C.bExpectedLegacy = true; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "beside_collider_lateral_70"; C.Camera = Cam;
		C.Scene.push_back(Cube("beside", MakeV3(0.0, 70.0, 0.0), 10.0));
		C.bExpectedTruth = false; C.bExpectedLegacy = true; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "noncolliding_prop_depth_50"; C.Camera = Cam;
		FScenePrimitive P = Cube("prop", MakeV3(50.0, 0.0, 0.0), 10.0); P.bCollidesVisibility = false;
		C.Scene.push_back(P);
		C.bExpectedTruth = true; C.bExpectedLegacy = false; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "pawn_owned_mesh_depth_50"; C.Camera = Cam;
		FScenePrimitive P = Cube("pawn_mesh", MakeV3(50.0, 0.0, 0.0), 10.0); P.bOwnedByPawn = true;
		P.Flags.bOwnedByViewActor = true;
		C.Scene.push_back(P);
		C.bExpectedTruth = true; C.bExpectedLegacy = false; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "first_person_arms_only_owner_see_depth_40"; C.Camera = Cam;
		FScenePrimitive P = Cube("arms", MakeV3(40.0, 10.0, -15.0), 12.0); P.bOwnedByPawn = true;
		P.Flags.bOwnedByViewActor = true; P.Flags.bOnlyOwnerSee = true; P.bCollidesVisibility = false;
		C.Scene.push_back(P);
		C.bExpectedTruth = true; C.bExpectedLegacy = false; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "pawn_body_owner_no_see_depth_50"; C.Camera = Cam;
		FScenePrimitive P = Cube("body", MakeV3(50.0, 0.0, 0.0), 30.0); P.bOwnedByPawn = true;
		P.Flags.bOwnedByViewActor = true; P.Flags.bOwnerNoSee = true;
		C.Scene.push_back(P);
		C.bExpectedTruth = false; C.bExpectedLegacy = false; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "hidden_collider_depth_50"; C.Camera = Cam;
		FScenePrimitive P = Cube("hidden", MakeV3(50.0, 0.0, 0.0), 10.0); P.Flags.bVisible = false;
		C.Scene.push_back(P);
		C.bExpectedTruth = false; C.bExpectedLegacy = true; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "closer_than_baseline_near_depth_2_to_8"; C.Camera = Cam;
		C.Scene.push_back(Cube("tiny", MakeV3(5.0, 0.0, 0.0), 3.0));
		C.bExpectedTruth = false; C.bExpectedLegacy = true; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "above_vertical_extent_depth_50"; C.Camera = Cam;
		C.Scene.push_back(Cube("above", MakeV3(50.0, 0.0, 40.0), 5.0));
		C.bExpectedTruth = false; C.bExpectedLegacy = true; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "rotated_rod_aabb_overlaps_obb_does_not"; C.Camera = Cam;
		FScenePrimitive P; P.Name = "rod";
		const double S2 = std::sqrt(0.5);
		P.Box.Center = MakeV3(40.0, 80.0, 0.0);
		P.Box.Axis[0] = MakeV3(S2, S2, 0.0);
		P.Box.Axis[1] = MakeV3(-S2, S2, 0.0);
		P.Box.Axis[2] = MakeV3(0.0, 0.0, 1.0);
		P.Box.Half[0] = std::sqrt(40.0 * 40.0 + 40.0 * 40.0) / 2.0;
		P.Box.Half[1] = 2.0;
		P.Box.Half[2] = 2.0;
		C.Scene.push_back(P);
		C.bExpectedTruth = false; C.bExpectedLegacy = true; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "fx_system_depth_50_excluded"; C.Camera = Cam;
		FScenePrimitive P = Cube("fx", MakeV3(50.0, 0.0, 0.0), 10.0); P.Flags.bIsFxSystem = true; P.bCollidesVisibility = false;
		C.Scene.push_back(P);
		C.bExpectedTruth = false; C.bExpectedLegacy = false; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "null_leg_near_equals_baseline"; C.Camera = Cam; C.Camera.N1 = C.Camera.N0;
		C.Scene.push_back(WallFacingCamera("wall", 50.0));
		C.bExpectedTruth = false; C.bExpectedLegacy = false; Cases.push_back(C);
	}
	{
		FCase C; C.Name = "rotated_camera_yaw_180_wall_ahead_50"; C.Camera = Cam;
		C.Camera.Forward = MakeV3(-1.0, 0.0, 0.0); C.Camera.Right = MakeV3(0.0, -1.0, 0.0);
		FScenePrimitive W; W.Name = "wall"; W.Box = MakeAxisAlignedBox(MakeV3(-55.0, 0.0, 0.0), MakeV3(5.0, 600.0, 400.0));
		C.Scene.push_back(W);
		C.bExpectedTruth = true; C.bExpectedLegacy = true; Cases.push_back(C);
	}

	std::printf("%-44s truth legacy new aabbOnly noFrustum\n", "case");
	int LegacyWrong = 0;
	int NewWrong = 0;
	int AabbWrong = 0;
	int NoFrustumWrong = 0;
	for (const FCase& C : Cases)
	{
		const bool Truth = PictureTruth(C.Camera, C.Scene);
		const bool Legacy = LegacySphereProxy(C.Camera, C.Scene);
		const bool New = NewSlabLabel(C.Camera, C.Scene);
		const bool AabbOnly = NewSlabLabelWorldAabbOnly(C.Camera, C.Scene);
		const bool NoFrustum = NewSlabLabelFrustumRemoved(C.Camera, C.Scene);
		std::printf("%-44s %5d %6d %3d %8d %9d\n", C.Name.c_str(), Truth, Legacy, New, AabbOnly, NoFrustum);
		Check(Truth == C.bExpectedTruth, "brute-force oracle matches the designed truth: " + C.Name);
		Check(Legacy == C.bExpectedLegacy, "legacy sphere model reads as designed: " + C.Name);
		Check(New == Truth, "NEW slab label agrees with picture truth: " + C.Name);
		LegacyWrong += (Legacy != Truth) ? 1 : 0;
		NewWrong += (New != Truth) ? 1 : 0;
		AabbWrong += (AabbOnly != Truth) ? 1 : 0;
		NoFrustumWrong += (NoFrustum != Truth) ? 1 : 0;
	}
	std::printf("disagreements with truth: legacy %d, new %d, new-with-world-aabb-only %d, new-with-frustum-removed %d (of %d)\n",
		LegacyWrong, NewWrong, AabbWrong, NoFrustumWrong, (int)Cases.size());
	Check(NewWrong == 0, "new slab label has zero disagreements");
	Check(LegacyWrong >= 8, "legacy sphere proxy disagrees on the behind/beside/non-colliding/pawn/hidden/near/above/rod/null cases");
	Check(AabbWrong >= 1, "a world-AABB-only narrow phase is caught by the rotated rod case (the fine phase is load-bearing)");
	Check(NoFrustumWrong >= 3, "removing the frustum is caught by the behind/beside/above cases (the frustum is load-bearing)");

	auto FindCase = [&Cases](const std::string& Name) -> const FCase* {
		for (const FCase& C : Cases) { if (C.Name == Name) { return &C; } }
		return nullptr;
	};
	for (const char* Name : { "behind_wall_face_50_behind", "noncolliding_prop_depth_50" })
	{
		const FCase* C = FindCase(Name);
		Check(C != nullptr, std::string("case exists: ") + Name);
		if (C)
		{
			const bool Truth = PictureTruth(C->Camera, C->Scene);
			Check(LegacySphereProxy(C->Camera, C->Scene) != Truth, std::string("BOTH WAYS: legacy sphere proxy FAILS ") + Name);
			Check(NewSlabLabel(C->Camera, C->Scene) == Truth, std::string("BOTH WAYS: new slab test PASSES ") + Name);
		}
	}
}

static FV3 RandomUnit(std::mt19937_64& Rng)
{
	std::normal_distribution<double> N(0.0, 1.0);
	FV3 V = MakeV3(N(Rng), N(Rng), N(Rng));
	const double L = std::sqrt(LengthSq(V));
	return Mul(V, 1.0 / (L > 0.0 ? L : 1.0));
}

static FBox3 RandomBox(std::mt19937_64& Rng)
{
	std::uniform_real_distribution<double> Pos(-80.0, 180.0);
	std::uniform_real_distribution<double> Lat(-160.0, 160.0);
	std::uniform_real_distribution<double> H(0.5, 45.0);
	FBox3 B;
	B.Center = MakeV3(Pos(Rng), Lat(Rng), Lat(Rng) * 0.6);
	const FV3 A0 = RandomUnit(Rng);
	FV3 T = RandomUnit(Rng);
	FV3 A1 = Cross(A0, T);
	while (LengthSq(A1) < 1e-6)
	{
		T = RandomUnit(Rng);
		A1 = Cross(A0, T);
	}
	A1 = Mul(A1, 1.0 / std::sqrt(LengthSq(A1)));
	const FV3 A2 = Cross(A0, A1);
	B.Axis[0] = A0;
	B.Axis[1] = A1;
	B.Axis[2] = A2;
	B.Half[0] = H(Rng);
	B.Half[1] = H(Rng) * 0.3;
	B.Half[2] = H(Rng);
	return B;
}

static double OverlapOnAxis(const FSlab& S, const FBox3& B, const FV3& Axis)
{
	const double L = std::sqrt(LengthSq(Axis));
	if (L <= 0.0)
	{
		return 1e30;
	}
	const FV3 A = Mul(Axis, 1.0 / L);
	double SMin = Dot(S.Corners[0], A);
	double SMax = SMin;
	for (int i = 1; i < 8; ++i)
	{
		const double P = Dot(S.Corners[i], A);
		SMin = P < SMin ? P : SMin;
		SMax = P > SMax ? P : SMax;
	}
	const double C = Dot(B.Center, A);
	double R = 0.0;
	for (int i = 0; i < 3; ++i)
	{
		R += std::fabs(Dot(B.Axis[i], A)) * B.Half[i];
	}
	const double Lo = (C - R) > SMin ? (C - R) : SMin;
	const double Hi = (C + R) < SMax ? (C + R) : SMax;
	return Hi - Lo;
}

static double MinSatOverlap(const FSlab& S, const FBox3& B)
{
	double M = 1e30;
	for (int i = 0; i < 5; ++i) { const double O = OverlapOnAxis(S, B, S.FaceAxes[i]); M = O < M ? O : M; }
	for (int i = 0; i < 3; ++i) { const double O = OverlapOnAxis(S, B, B.Axis[i]); M = O < M ? O : M; }
	for (int e = 0; e < 6; ++e)
	{
		for (int i = 0; i < 3; ++i)
		{
			const FV3 A = Cross(S.EdgeDirs[e], B.Axis[i]);
			if (LengthSq(A) <= 1e-12 * LengthSq(S.EdgeDirs[e]) * LengthSq(B.Axis[i])) { continue; }
			const double O = OverlapOnAxis(S, B, A);
			M = O < M ? O : M;
		}
	}
	return M;
}

static void RunRandomCrossCheck()
{
	std::mt19937_64 Rng(86022ull);
	const FCamera C = DefaultCamera();
	const FSlab S = SlabOf(C);
	int SampleHitSatMiss = 0;
	int SatHitSampleMissCoarse = 0;
	int SatHitSampleMissFine = 0;
	double WorstUnconfirmedOverlap = 0.0;
	int Hits = 0;
	const int N = 20000;
	for (int i = 0; i < N; ++i)
	{
		const FBox3 B = RandomBox(Rng);
		const bool Sat = BoxIntersectsSlab(S, B);
		const bool Sample = BruteForceIntersects(C, S, B, 8);
		Hits += Sat ? 1 : 0;
		if (Sample && !Sat)
		{
			++SampleHitSatMiss;
		}
		if (Sat && !Sample)
		{
			++SatHitSampleMissCoarse;
			if (!BruteForceIntersects(C, S, B, 60))
			{
				++SatHitSampleMissFine;
				const double O = MinSatOverlap(S, B);
				WorstUnconfirmedOverlap = O > WorstUnconfirmedOverlap ? O : WorstUnconfirmedOverlap;
			}
		}
	}
	std::printf("random cross-check: %d boxes, SAT hits %d, sample-hit-but-SAT-miss %d, SAT-hit-coarse-sample-miss %d, "
		"still-unconfirmed-at-61^3 %d (largest minimum SAT overlap among them %.3f cm)\n", N, Hits, SampleHitSatMiss,
		SatHitSampleMissCoarse, SatHitSampleMissFine, WorstUnconfirmedOverlap);
	Check(SampleHitSatMiss == 0, "SAT never misses an intersection a sample point proves");
	Check(SatHitSampleMissFine <= N / 2000, "SAT-hit cases unconfirmed at fine sampling are rare");
	Check(WorstUnconfirmedOverlap < 3.4, "every unconfirmed SAT hit is a grazing contact thinner than the sampling pitch");
	Check(Hits > N / 10 && Hits < N * 9 / 10, "the random set exercises both outcomes");
}

static void RunTanExtents()
{
	const double Eps = 1e-9;
	FTanExtents T = ComputeTanExtents(90.0, 16.0 / 9.0, false, 1280, 720, AxisMaintainX, false);
	Check(std::fabs(T.TanH - 1.0) < Eps && std::fabs(T.TanV - 0.5625) < Eps, "MaintainX 90deg 1280x720 -> tan 1.0 / 0.5625");
	T = ComputeTanExtents(90.0, 16.0 / 9.0, false, 1280, 720, AxisMajor, false);
	Check(std::fabs(T.TanH - 1.0) < Eps && std::fabs(T.TanV - 0.5625) < Eps, "MajorAxis landscape behaves as MaintainX");
	T = ComputeTanExtents(90.0, 16.0 / 9.0, false, 1280, 720, AxisMaintainY, false);
	Check(std::fabs(T.TanV - 0.5625) < Eps && std::fabs(T.TanH - 1.0) < Eps, "MaintainY at the POV aspect equals MaintainX");
	T = ComputeTanExtents(90.0, 16.0 / 9.0, false, 1280, 1024, AxisMaintainY, false);
	Check(std::fabs(T.TanV - 0.5625) < Eps && std::fabs(T.TanH - 0.5625 * 1280.0 / 1024.0) < Eps, "MaintainY keeps the vertical extent");
	T = ComputeTanExtents(90.0, 16.0 / 9.0, false, 1280, 1024, AxisMaintainY, true);
	Check(std::fabs(T.TanV - 1.0) < Eps, "legacy MaintainY uses the raw half FOV vertically");
	T = ComputeTanExtents(90.0, 2.39, true, 1280, 720, AxisMaintainX, false);
	Check(std::fabs(T.TanH - 1.0) < Eps && std::fabs(T.TanV - 1.0 / 2.39) < Eps, "constrained aspect uses the POV aspect");
	T = ComputeTanExtents(90.0, 16.0 / 9.0, false, 720, 1280, AxisMajor, false);
	Check(T.TanV > T.TanH, "MajorAxis portrait maintains the vertical axis");
}

static void RunClassify()
{
	FPrimitiveFlags F;
	Check(ClassifyPrimitive(F) == EPrimitiveVerdict::Counted, "default primitive counts");
	F.bOwnedByViewActor = true;
	Check(ClassifyPrimitive(F) == EPrimitiveVerdict::Counted, "a pawn-owned mesh counts (the old proxy ignored the pawn)");
	F.bOwnerNoSee = true;
	Check(ClassifyPrimitive(F) == EPrimitiveVerdict::HiddenFromViewer, "owner-no-see mesh of the view actor is not drawn for it");
	F = FPrimitiveFlags(); F.bOnlyOwnerSee = true;
	Check(ClassifyPrimitive(F) == EPrimitiveVerdict::HiddenFromViewer, "only-owner-see mesh of another actor is not drawn");
	F.bOwnedByViewActor = true;
	Check(ClassifyPrimitive(F) == EPrimitiveVerdict::Counted, "only-owner-see mesh of the view actor counts");
	F = FPrimitiveFlags(); F.bOwnerHidden = true;
	Check(ClassifyPrimitive(F) == EPrimitiveVerdict::NotRendered, "hidden owner is not rendered");
	F = FPrimitiveFlags(); F.bRenderInMainPass = false;
	Check(ClassifyPrimitive(F) == EPrimitiveVerdict::NotRendered, "main-pass-off primitive (m45 hide) is not rendered");
	F = FPrimitiveFlags(); F.bHasSceneProxy = false;
	Check(ClassifyPrimitive(F) == EPrimitiveVerdict::NotRendered, "no scene proxy is not rendered");
	F = FPrimitiveFlags(); F.bSceneCaptureOnly = true;
	Check(ClassifyPrimitive(F) == EPrimitiveVerdict::NotRendered, "scene-capture-only is not rendered in the main view");
	F = FPrimitiveFlags(); F.bIsFxSystem = true;
	Check(ClassifyPrimitive(F) == EPrimitiveVerdict::FxExcluded, "FX systems are excluded");
	F = FPrimitiveFlags(); F.MinDrawDistance = 100.0; F.DistanceSqToView = 50.0 * 50.0;
	Check(ClassifyPrimitive(F) == EPrimitiveVerdict::InsideMinDrawDistance, "inside min draw distance is not drawn");
	F.DistanceSqToView = 150.0 * 150.0;
	Check(ClassifyPrimitive(F) == EPrimitiveVerdict::Counted, "beyond min draw distance counts");
}

static void RunSlabShape()
{
	const FCamera C = DefaultCamera();
	FSlab S = SlabOf(C);
	Check(!S.bEmpty, "slab 10..100 is not empty");
	Check(std::fabs(S.AabbMin.X - 10.0) < 1e-9 && std::fabs(S.AabbMax.X - 100.0) < 1e-9, "slab depth range 10..100");
	Check(std::fabs(S.AabbMax.Y - 100.0) < 1e-9 && std::fabs(S.AabbMax.Z - 56.25) < 1e-9, "slab far half extents 100 x 56.25");
	S = BuildSlab(C.Origin, C.Forward, C.Right, C.Up, C.TanH, C.TanV, 100.0, 10.0);
	Check(S.bEmpty, "an anomalous near below the baseline gives an empty slab");
	S = BuildSlab(C.Origin, C.Forward, C.Right, C.Up, C.TanH, C.TanV, 10.0, 10.0);
	Check(S.bEmpty, "an anomalous near equal to the baseline gives an empty slab");
	S = SlabOf(C);
	const FBox3 Inside = MakeAxisAlignedBox(MakeV3(50.0, 0.0, 0.0), MakeV3(1.0, 1.0, 1.0));
	Check(BoxIntersectsSlab(S, Inside), "a box fully inside the slab intersects");
	const FBox3 Enclosing = MakeAxisAlignedBox(MakeV3(0.0, 0.0, 0.0), MakeV3(5000.0, 5000.0, 5000.0));
	Check(BoxIntersectsSlab(S, Enclosing), "a box enclosing the whole slab intersects (the hollow-mesh false-positive class)");
	Check(PointInsideBox(Enclosing, C.Origin), "and it contains the eye, which the diagnostic counts");
}

static void RunBenchmark()
{
	std::mt19937_64 Rng(7ull);
	std::vector<FBox3> Boxes;
	for (int i = 0; i < 4096; ++i)
	{
		Boxes.push_back(RandomBox(Rng));
	}
	const FCamera C = DefaultCamera();
	const int Iter = 2000000;
	volatile int Sink = 0;
	auto T0 = std::chrono::steady_clock::now();
	FSlab S = SlabOf(C);
	for (int i = 0; i < Iter; ++i)
	{
		Sink += BoxIntersectsSlab(S, Boxes[i & 4095]) ? 1 : 0;
	}
	auto T1 = std::chrono::steady_clock::now();
	const double NsPerSat = std::chrono::duration<double, std::nano>(T1 - T0).count() / Iter;
	T0 = std::chrono::steady_clock::now();
	for (int i = 0; i < Iter; ++i)
	{
		FV3 Mn, Mx;
		BoxAabb(Boxes[i & 4095], Mn, Mx);
		Sink += AabbOverlap(S.AabbMin, S.AabbMax, Mn, Mx) ? 1 : 0;
	}
	T1 = std::chrono::steady_clock::now();
	const double NsPerAabb = std::chrono::duration<double, std::nano>(T1 - T0).count() / Iter;
	T0 = std::chrono::steady_clock::now();
	for (int i = 0; i < Iter / 10; ++i)
	{
		S = BuildSlab(C.Origin, C.Forward, C.Right, C.Up, C.TanH, C.TanV, 10.0, 100.0 + (i & 1));
		Sink += S.bEmpty ? 1 : 0;
	}
	T1 = std::chrono::steady_clock::now();
	const double NsPerBuild = std::chrono::duration<double, std::nano>(T1 - T0).count() / (Iter / 10);
	std::printf("BENCH ns per call: exact SAT box-vs-slab %.1f, AABB broad phase %.1f, slab build %.1f (sink %d)\n",
		NsPerSat, NsPerAabb, NsPerBuild, (int)Sink);
	Check(NsPerSat < 5000.0, "exact SAT stays in the microsecond range");
}

int main()
{
	RunTanExtents();
	RunClassify();
	RunSlabShape();
	RunCases();
	RunRandomCrossCheck();
	RunBenchmark();
	std::printf("camera_clipping slab selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
