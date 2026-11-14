#include "../Source/AnomalyInjector/Public/AnomalyLabelSync.h"

#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <vector>

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

struct FTick
{
	int Tick = 0;
	bool bSettle = false;
	int Event = -1;
	bool bMember = false;
};

struct FSchedule
{
	std::vector<FTick> Ticks;
	std::vector<int> Onsets;
	std::vector<int> Reverts;
};

static FSchedule MakeSchedule(int K, int Pre, int Pos, int Post, int Total)
{
	FSchedule S;
	std::map<int, FTick> At;
	for (int T = 0; T < Total; ++T) { FTick X; X.Tick = T; At[T] = X; }
	int On = Pre - 1;
	int Ev = 0;
	while (On < Total)
	{
		S.Onsets.push_back(On);
		for (int T = On; T < On + K + Pos && T < Total; ++T) { At[T].Event = Ev; At[T].bMember = true; }
		for (int T = On + 1; T <= On + K && T < Total; ++T) { At[T].bSettle = true; }
		const int Rv = On + K + Pos;
		S.Reverts.push_back(Rv);
		for (int T = Rv + 1; T <= Rv + K && T < Total; ++T) { At[T].bSettle = true; }
		On = Rv + K + Post;
		++Ev;
	}
	for (auto& KV : At) { S.Ticks.push_back(KV.second); }
	return S;
}

enum class EMode { Release, M56, M56AsBuilt };

struct FFlags
{
	std::set<int> Temporal;
	std::set<int> Hide;
};

static FFlags RunTracks(const FSchedule& S, EMode Mode, int N)
{
	FFlags Out;
	std::map<int, FEventTransitionTrack> Tracks;
	std::map<int, FHideReturnTrack> Hides;
	std::set<int> Gone;
	int SI = 0;
	for (const FTick& X : S.Ticks)
	{
		if (Mode == EMode::Release && X.bSettle) { continue; }
		const bool bSettleArg = Mode == EMode::M56 && X.bSettle;
		const int Index = Mode == EMode::Release ? SI : X.Tick;
		if (X.Event >= 0 && Tracks.find(X.Event) == Tracks.end()) { Tracks[X.Event] = FEventTransitionTrack(); Hides[X.Event] = FHideReturnTrack(); }
		for (auto& KV : Tracks)
		{
			const bool bInFrame = KV.first == X.Event;
			bool bOn = false;
			bool bOff = false;
			KV.second.Observe(Index, bInFrame && X.bMember, N, N, bOn, bOff, bSettleArg);
			if (bOn || bOff) { Out.Temporal.insert(X.Tick); }
		}
		for (auto& KV : Hides)
		{
			const bool bInFrame = KV.first == X.Event;
			bool bFlag = false;
			if (bInFrame) { bFlag = StepHideLive(KV.second, true, N, bSettleArg); }
			else if (!HideTrackDone(KV.second) || !Gone.count(KV.first)) { Gone.insert(KV.first); bFlag = StepHideGone(KV.second, N, bSettleArg); }
			if (bFlag) { Out.Hide.insert(X.Tick); }
		}
		++SI;
	}
	return Out;
}

static std::string Join(const std::set<int>& S, int Limit)
{
	std::string R;
	int N = 0;
	for (int V : S)
	{
		if (N++ >= Limit) { R += " ..."; break; }
		R += (R.empty() ? "" : ",") + std::to_string(V);
	}
	return R.empty() ? "-" : R;
}

static std::set<int> NonSettle(const std::set<int>& Flags, const FSchedule& S)
{
	std::set<int> Settle;
	for (const FTick& X : S.Ticks) { if (X.bSettle) { Settle.insert(X.Tick); } }
	std::set<int> R;
	for (int T : Flags) { if (!Settle.count(T)) { R.insert(T); } }
	return R;
}

static bool Subset(const std::set<int>& A, const std::set<int>& B)
{
	for (int V : A) { if (!B.count(V)) { return false; } }
	return true;
}

int main()
{
	const int Pre = 4, Pos = 8, Post = 4, Total = 80;
	std::printf("settle window table: pre %d, positive %d, post %d, %d ticks; flags listed as tick indices, first 14 shown\n",
		Pre, Pos, Post, Total);
	std::printf("%-3s %-3s %-9s %-60s %-60s %s\n", "K", "N", "window", "release (settle ticks not captured)", "m56 fixed, non-settle ticks", "m56 as built lost");
	for (int K : { 2, 0 })
	{
		const FSchedule S = MakeSchedule(K, Pre, Pos, Post, Total);
		for (int N : { 1, 3, 40 })
		{
			const FFlags Rel = RunTracks(S, EMode::Release, N);
			const FFlags Fix = RunTracks(S, EMode::M56, N);
			const FFlags Old = RunTracks(S, EMode::M56AsBuilt, N);
			const std::string Tag = "K=" + std::to_string(K) + " N=" + std::to_string(N);
			for (int W = 0; W < 2; ++W)
			{
				const std::set<int>& R = W == 0 ? Rel.Temporal : Rel.Hide;
				const std::set<int>& F = W == 0 ? Fix.Temporal : Fix.Hide;
				const std::set<int>& O = W == 0 ? Old.Temporal : Old.Hide;
				const std::set<int> FixNon = NonSettle(F, S);
				std::set<int> Lost;
				for (int V : R) { if (!O.count(V)) { Lost.insert(V); } }
				std::printf("%-3d %-3d %-9s %-60s %-60s %s\n", K, N, W == 0 ? "on/off" : "hide", Join(R, 14).c_str(),
					Join(FixNon, 14).c_str(), Join(Lost, 8).c_str());
				const std::string What = Tag + (W == 0 ? " on/off" : " hide");
				Check(FixNon == R, What + ": the fixed build flags exactly the release frames on every non-settle tick");
				Check(Subset(R, F), What + ": every frame the release flagged is still flagged");
				if (K == 0)
				{
					Check(F == R && O == R, What + ": with K = 0 nothing changes");
				}
			}
		}
	}
	{
		const FSchedule S = MakeSchedule(2, Pre, Pos, Post, Total);
		const FFlags Rel = RunTracks(S, EMode::Release, 3);
		const FFlags Old = RunTracks(S, EMode::M56AsBuilt, 3);
		Check(!Subset(Rel.Temporal, Old.Temporal), "the as-built m56 arithmetic (settle frames counted) loses release on/off frames (the S5 defect)");
		Check(!Subset(Rel.Hide, Old.Hide), "the as-built m56 arithmetic loses release hide frames (the S5 defect)");
	}
	{
		FEventTransitionTrack T;
		bool bOn = false, bOff = false;
		T.Observe(10, true, 3, 3, bOn, bOff);
		T.Observe(11, false, 3, 3, bOn, bOff);
		Check(bOff, "a frame one tick after the last member is in a 3-frame off window");
		T.Observe(12, false, 3, 3, bOn, bOff, true);
		T.Observe(13, false, 3, 3, bOn, bOff, true);
		T.Observe(14, false, 3, 3, bOn, bOff);
		Check(bOff, "two settle frames do not use up the off window");
		Check(T.WindowDistance(10, 14) == 2, "window distance skips the settle frames in between");
		FEventTransitionTrack C = CarryTransitionTrack(T, 14);
		Check(C.WindowDistance(-5, -1) == 2, "rebasing keeps the settle frames in the window arithmetic");
	}
	std::printf("settle_window_selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
