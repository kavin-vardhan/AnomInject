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

struct FFire
{
	std::string Key;
	EAnnotationPolicy Policy = EAnnotationPolicy::FireWindow;
	bool bLabelled = false;
	bool bUnpaired = false;
};

struct FFrame
{
	int SI = 0;
	std::vector<FFire> Fires;
	bool bUnpaired = false;
};

struct FEntryOut
{
	std::string Key;
	bool bGone = false;
	EEntryEmit Emit = EEntryEmit::Normal;
	unsigned short Reasons = 0;
};

struct FSim
{
	struct FSlot
	{
		bool bLabelled = false;
	};
	std::map<std::string, FSlot> Slots;
	int FramesFlagged = 0;
	std::map<int, std::vector<FEntryOut>> Flags;

	void Step(bool bPieWorld, const FFrame& Frame)
	{
		std::set<std::string> Seen;
		bool bFlagged = false;
		for (const FFire& F : Frame.Fires)
		{
			Seen.insert(F.Key);
			const bool bLabelledNow = !Frame.bUnpaired && F.bLabelled;
			const auto It = Slots.find(F.Key);
			const bool bWas = It != Slots.end() && It->second.bLabelled;
			if (DecidePieEndSettle(bWas, true, bLabelledNow) == EPieSettleAction::FlagPresent)
			{
				FEntryOut E;
				E.Key = F.Key;
				E.Emit = DecidePieSettleEntry(EEntryEmit::Normal);
				E.Reasons = ReasonPieEndSettle;
				Flags[Frame.SI].push_back(E);
				bFlagged = true;
			}
			if (TrackPieEndSettle(bPieWorld, F.Policy, bLabelledNow))
			{
				Slots[F.Key].bLabelled = true;
			}
			else
			{
				Slots.erase(F.Key);
			}
		}
		for (auto It = Slots.begin(); It != Slots.end();)
		{
			if (Seen.count(It->first))
			{
				++It;
				continue;
			}
			if (DecidePieEndSettle(It->second.bLabelled, false, false) == EPieSettleAction::FlagGone)
			{
				FEntryOut E;
				E.Key = It->first;
				E.bGone = true;
				E.Emit = EEntryEmit::TransitionOnly;
				E.Reasons = ReasonPieEndSettle;
				Flags[Frame.SI].push_back(E);
				bFlagged = true;
			}
			It = Slots.erase(It);
		}
		if (bFlagged)
		{
			++FramesFlagged;
		}
	}
};

static FFrame Fr(int SI, std::vector<FFire> Fires, bool bUnpaired = false)
{
	FFrame F;
	F.SI = SI;
	F.Fires = Fires;
	F.bUnpaired = bUnpaired;
	return F;
}

static FFire Fw(const std::string& Key, bool bLabelled, EAnnotationPolicy Policy = EAnnotationPolicy::FireWindow)
{
	FFire F;
	F.Key = Key;
	F.bLabelled = bLabelled;
	F.Policy = Policy;
	return F;
}

static std::vector<FFrame> BurstGone(const std::string& Key, int First, int Last, int Tail, EAnnotationPolicy Policy)
{
	std::vector<FFrame> Out;
	for (int si = First - 2; si < First; ++si)
	{
		Out.push_back(Fr(si, {}));
	}
	for (int si = First; si <= Last; ++si)
	{
		Out.push_back(Fr(si, { Fw(Key, true, Policy) }));
	}
	for (int si = Last + 1; si <= Last + Tail; ++si)
	{
		Out.push_back(Fr(si, {}));
	}
	return Out;
}

static FSim Run(bool bPie, const std::vector<FFrame>& Frames)
{
	FSim S;
	for (const FFrame& F : Frames)
	{
		S.Step(bPie, F);
	}
	return S;
}

static std::vector<int> FlaggedSIs(const FSim& S)
{
	std::vector<int> Out;
	for (const auto& KV : S.Flags)
	{
		Out.push_back(KV.first);
	}
	return Out;
}

static std::string Str(const std::vector<int>& V)
{
	std::string S = "[";
	for (size_t i = 0; i < V.size(); ++i)
	{
		S += (i ? "," : "") + std::to_string(V[i]);
	}
	return S + "]";
}

static void TestReasonBit()
{
	Check(ReasonPieEndSettle == 256 && NumReasons == 9 && std::string(DescribeReasonBit(8)) == "pie_end_settle",
		"reason: pie_end_settle is the ninth reason bit (256)");
	const unsigned short Others[] = { ReasonTemporal, ReasonHideReturn, ReasonPartial, ReasonCameraUnconfirmed, ReasonUnresolved,
		ReasonEffectInterrupted, ReasonNaniteUnmaskable, ReasonCaptureUnpaired };
	bool bDisjoint = true;
	for (unsigned short O : Others)
	{
		bDisjoint = bDisjoint && (O & ReasonPieEndSettle) == 0;
	}
	Check(bDisjoint, "reason: pie_end_settle shares no bit with any other reason");
	Check(ReasonsOrLegacy(ReasonPieEndSettle) == ReasonPieEndSettle && (ReasonsOrLegacy(ReasonPieEndSettle) & ReasonTemporal) == 0,
		"reason: pie_end_settle is never rewritten as a legacy temporal (anti-aliasing) flag");
	Check(ReasonsOrLegacy((unsigned short)(ReasonPieEndSettle | ReasonEffectInterrupted)) == (ReasonPieEndSettle | ReasonEffectInterrupted),
		"reason: pie_end_settle keeps a second reason beside it");
}

static void TestEntry()
{
	Check(DecidePieSettleEntry(EEntryEmit::Normal) == EEntryEmit::TransitionOnly
		&& DecidePieSettleEntry(EEntryEmit::TransitionOnly) == EEntryEmit::TransitionOnly
		&& DecidePieSettleEntry(EEntryEmit::Suppress) == EEntryEmit::Suppress,
		"entry: a flagged entry is written transition-only (never labelled, never a member); a suppressed one stays suppressed");
	Check(!IsEntryLabelled(DecidePieSettleEntry(EEntryEmit::Normal) == EEntryEmit::Normal, EAnnotationPolicy::FireWindow, true, true, true),
		"entry: an on-screen, installed fire-window entry flagged pie_end_settle is not labelled");
}

static void TestPieGone()
{
	const FSim S = Run(true, BurstGone("uv@100|A", 4, 11, 6, EAnnotationPolicy::FireWindow));
	Check(FlaggedSIs(S) == std::vector<int>{ 12 }, "PIE gone: the first frame after the last labelled frame (12) and only it, got "
		+ Str(FlaggedSIs(S)));
	const auto It = S.Flags.find(12);
	Check(It != S.Flags.end() && It->second.size() == 1 && It->second[0].bGone && It->second[0].Key == "uv@100|A"
		&& It->second[0].Reasons == ReasonPieEndSettle && It->second[0].Emit == EEntryEmit::TransitionOnly,
		"PIE gone: one transition-only entry for that event, reason pie_end_settle");
	Check(S.FramesFlagged == 1 && S.Slots.empty(), "PIE gone: one frame counted, no state left");
}

static void TestPiePresentUnlabelled()
{
	std::vector<FFrame> F;
	for (int si = 0; si < 4; ++si) { F.push_back(Fr(si, { Fw("mt@7|B", true) })); }
	for (int si = 4; si < 7; ++si) { F.push_back(Fr(si, { Fw("mt@7|B", false) })); }
	const FSim S = Run(true, F);
	Check(FlaggedSIs(S) == std::vector<int>{ 4 }, "PIE present: a live but unlabelled entry is flagged once at 4, got " + Str(FlaggedSIs(S)));
	Check(S.Flags.count(4) && !S.Flags.at(4)[0].bGone && S.Flags.at(4)[0].Emit == EEntryEmit::TransitionOnly,
		"PIE present: the live entry itself becomes transition-only");
}

static void TestPieTwoRuns()
{
	std::vector<FFrame> F;
	F.push_back(Fr(3, {}));
	for (int si = 4; si <= 6; ++si) { F.push_back(Fr(si, { Fw("ct@9|C", true) })); }
	for (int si = 7; si <= 8; ++si) { F.push_back(Fr(si, { Fw("ct@9|C", false) })); }
	for (int si = 9; si <= 10; ++si) { F.push_back(Fr(si, { Fw("ct@9|C", true) })); }
	for (int si = 11; si <= 14; ++si) { F.push_back(Fr(si, {})); }
	const FSim S = Run(true, F);
	Check(FlaggedSIs(S) == std::vector<int>({ 7, 11 }), "PIE two runs: each label run's first following frame, got " + Str(FlaggedSIs(S)));
}

static void TestStaged()
{
	const FSim A = Run(false, BurstGone("uv@100|A", 4, 11, 6, EAnnotationPolicy::FireWindow));
	Check(A.Flags.empty() && A.FramesFlagged == 0 && A.Slots.empty(), "staged: a game world never flags (gone), got " + Str(FlaggedSIs(A)));
	std::vector<FFrame> F;
	for (int si = 0; si < 4; ++si) { F.push_back(Fr(si, { Fw("mt@7|B", true) })); }
	for (int si = 4; si < 7; ++si) { F.push_back(Fr(si, { Fw("mt@7|B", false) })); }
	const FSim B = Run(false, F);
	Check(B.Flags.empty(), "staged: a game world never flags (present), got " + Str(FlaggedSIs(B)));
}

static void TestOtherPolicies()
{
	const EAnnotationPolicy P[] = { EAnnotationPolicy::ActorHidden, EAnnotationPolicy::AnomalyState, EAnnotationPolicy::RenderHeldWindow };
	const char* N[] = { "actor_hidden", "anomaly_state", "render_held_window" };
	for (int i = 0; i < 3; ++i)
	{
		const FSim S = Run(true, BurstGone("x@1|D", 4, 9, 4, P[i]));
		Check(S.Flags.empty(), std::string("PIE: a ") + N[i] + " event is never flagged, got " + Str(FlaggedSIs(S)));
	}
}

static void TestConcurrent()
{
	std::vector<FFrame> F;
	for (int si = 0; si < 5; ++si) { F.push_back(Fr(si, { Fw("uv@1|A", true), Fw("nm@2|B", true) })); }
	F.push_back(Fr(5, {}));
	F.push_back(Fr(6, {}));
	const FSim S = Run(true, F);
	Check(FlaggedSIs(S) == std::vector<int>{ 5 } && S.Flags.at(5).size() == 2 && S.FramesFlagged == 1,
		"PIE concurrent: two events ending together are both flagged on one frame, counted once");
}

static void TestUnpaired()
{
	std::vector<FFrame> F;
	for (int si = 0; si < 4; ++si) { F.push_back(Fr(si, { Fw("uv@1|A", true) })); }
	F.push_back(Fr(4, { Fw("uv@1|A", true) }, true));
	F.push_back(Fr(5, { Fw("uv@1|A", true) }));
	F.push_back(Fr(6, {}));
	const FSim S = Run(true, F);
	Check(FlaggedSIs(S) == std::vector<int>({ 4, 6 }), "PIE unpaired: a sync frame after a label is flagged, and so is the frame after "
		"the next label run, got " + Str(FlaggedSIs(S)));
}

static void TestNeverOnLabelled()
{
	std::vector<FFrame> F;
	for (int si = 0; si < 10; ++si) { F.push_back(Fr(si, { Fw("uv@1|A", true) })); }
	const FSim S = Run(true, F);
	Check(S.Flags.empty(), "PIE: a frame that is still labelled is never flagged, got " + Str(FlaggedSIs(S)));
}

int main()
{
	TestReasonBit();
	TestEntry();
	TestPieGone();
	TestPiePresentUnlabelled();
	TestPieTwoRuns();
	TestStaged();
	TestOtherPolicies();
	TestConcurrent();
	TestUnpaired();
	TestNeverOnLabelled();
	std::printf("pie_end_settle_selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
