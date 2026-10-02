#include "../Source/AnomalyCapture/Private/AnomalyMaskServe.h"

#include <cstdio>
#include <string>
#include <vector>

using namespace AnomalyMaskServe;

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

struct FSim
{
	unsigned long long NextSeq = 1;
	std::vector<unsigned long long> Pending;
	std::vector<std::vector<unsigned long long>> Served;

	unsigned long long Arm()
	{
		const unsigned long long Seq = NextSeq++;
		Pending.push_back(Seq);
		return Seq;
	}

	unsigned long long BeginFamily() const { return BoundForFamily(NextSeq); }

	std::vector<unsigned long long> Render(bool bHasBound, unsigned long long Bound)
	{
		std::vector<unsigned long long> Serve;
		std::vector<unsigned long long> Keep;
		for (unsigned long long Seq : Pending)
		{
			(ArmBelongsToFamily(Seq, bHasBound, Bound) ? Serve : Keep).push_back(Seq);
		}
		Pending = Keep;
		Served.push_back(Serve);
		return Serve;
	}
};

int main()
{
	{
		FSim S;
		const unsigned long long A = S.Arm();
		const unsigned long long B0 = S.BeginFamily();
		const std::vector<unsigned long long> R0 = S.Render(true, B0);
		Check(R0.size() == 1 && R0[0] == A, "lockstep: a family serves the arm made before its BeginRenderViewFamily");
		Check(S.Pending.empty(), "lockstep: nothing left pending");
	}
	{
		FSim S;
		const unsigned long long A = S.Arm();
		const unsigned long long BoundN = S.BeginFamily();
		const unsigned long long B = S.Arm();
		const std::vector<unsigned long long> RN = S.Render(true, BoundN);
		Check(RN.size() == 1 && RN[0] == A, "render lag: frame N's render serves only frame N's arm");
		Check(S.Pending.size() == 1 && S.Pending[0] == B, "render lag: frame N+1's arm stays pending");
		const unsigned long long BoundN1 = S.BeginFamily();
		const std::vector<unsigned long long> RN1 = S.Render(true, BoundN1);
		Check(RN1.size() == 1 && RN1[0] == B, "render lag: frame N+1's own render serves its arm");
		Check(S.Pending.empty(), "render lag: no arm left behind");
	}
	{
		FSim S;
		const unsigned long long A = S.Arm();
		const unsigned long long BoundN = S.BeginFamily();
		const std::vector<unsigned long long> None = S.Render(true, 0);
		Check(None.empty() && S.Pending.size() == 1, "a family whose bound predates every arm serves nothing");
		const std::vector<unsigned long long> RN = S.Render(true, BoundN);
		Check(RN.size() == 1 && RN[0] == A, "an older arm is still served by a later family");
	}
	{
		FSim S;
		S.Arm();
		S.Arm();
		const std::vector<unsigned long long> R = S.Render(false, 0);
		Check(R.size() == 2 && S.Pending.empty(), "a family with no bound serves every pending arm (the pre-090-10f2 behaviour)");
	}
	{
		FSim S;
		const unsigned long long A = S.Arm();
		const unsigned long long M = S.Arm();
		const unsigned long long BoundN = S.BeginFamily();
		const unsigned long long Next = S.Arm();
		const std::vector<unsigned long long> RN = S.Render(true, BoundN);
		Check(RN.size() == 2 && RN[0] == A && RN[1] == M, "two arms made in one tick are both served by that tick's family");
		Check(S.Pending.size() == 1 && S.Pending[0] == Next, "the next tick's arm waits");
	}
	Check(BoundForFamily(1) == 0, "no arm yet: bound 0");
	Check(BoundForFamily(0) == 0, "bound never underflows");
	Check(ArmBelongsToFamily(5, true, 5), "an arm at the bound belongs");
	Check(!ArmBelongsToFamily(6, true, 5), "an arm past the bound does not belong");

	std::printf("mask_serve_selftest: %d check(s), %d failure(s)\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
