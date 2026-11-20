# REPORT for 081-23-m55-requalification-prep.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-23-m55-requalification-prep.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-24 07:39:01 | finished: 2026-09-24 08:07:35 | duration: 29 min | exit code: 0
context at last turn: 604568 tokens (60% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-23-m55-requalification-prep.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Summary for the owner**
- The whole build-3 requalification campaign is prepared and tested. Nothing was launched in Unreal, and no build or bench leg ran.
- It needs **4 bench windows of 15–30 minutes each, about 1.5 hours of PC time in all**. The worst case for any one window is about 45 minutes.
- Each window is one command: `python _reviews\081-23-window.py c0` (then `c1`, `c2`, `c3`). It checks everything first, waits up to 9 minutes for the PC to be free, runs its legs, and restores the setup afterwards.
- If a window is interrupted, the next run picks up from the leg it stopped on. The first real failure stops the window and goes to chat.
- All the checking code was tried on saved runs from earlier rounds: 118 of 118 checks came out as expected, including exactly reproducing last round's legacy result.
- Two foreign UE 5.7 editors are open right now, so the PC is not free.

**Inventory: 92 legs** (6 long-run readings + 86 requalification legs)

| chunk | legs | what they prove | pass rule (summary) | compared against |
|---|---|---|---|---|
| c0 | Lyra L1–L4 at 1080p, 1800 frames (evidence ON/OFF/OFF/ON); Stack S1 ON, S2 OFF | whether memory and writer latency keep growing over a long run; evidence ON vs OFF | readings only (081-18 rules and predictions, unchanged) | ON vs OFF |
| c1 | G8, G9, G2, G15, `ChangeMaxBytes 1` (LOW), G10, count/wall can-fails — all both orders | the code paths build 3 changed; the refusal census (never run yet) | shared measurement gate + gate-specific checks, including §D's predicted build-3 outcomes | §D predictions |
| c1 | Stack 1080p null N, null S, solid S | full yield under the 256 MiB cap | 20/20 pairs, 5 events, onsets separate | build 2 twins |
| c1 | 20 legacy legs: CB N and S (pre, OFF, ON, ON, OFF, pre); MainWorld evidence-ON N and S (pre, ON, ON, pre) | legacy output never depends on m55 | 081-22 AMENDMENT 1 rules + the ruled timing-sampled set; with evidence ON only the `change_*` keys and the sidecar may be added | pre-m55 `0844220E` |
| c2 | 22 Stack 720 static recipes (both orders) + 2 moving (natural) + G1, G3–G7, G11–G13 both orders | Stage-2 measurement/finalisation and identity on build 3 | shared measurement gate + the 081-15 predicate for each recipe/gate | — |
| c3 | NoHold N and S on MainWorld; Lyra G270 (300 frames); Lyra 720 null/solid twins | zero-labelled onset; the G270 observation; scoped Lyra | G2 pose policy; 081-14 §3.4 branches + zero `budget_exceeded`; audit + census + oracle | 081-13/14, 081-17 |

The shared measurement gate is: the 081-08 invariants, zero `budget_exceeded` anywhere (except G10's forced refusal at SI 8 and the LOW control), a closed census, the build-3 counters at 0 with the EFFECTIVE line naming the requested gate, and the oracle exiting 0 with compared = measured.

Changes against Codex's 75:
- **−1:** the 1080p `solid1080` natural leg already passed on build 3 in 081-22 (a).
- **+4:** G8 and G9 in both orders. Build 3 rewrote exactly the code they exercise, and §D predicts a different G8 result.
- **+8 legacy:** the brief asks for both tick orders plus the evidence-ON halves. 081-22 (d) MainWorld evidence-OFF, both orders, is credited rather than re-run.
- **Dropped:** none. G14 stays covered by every measurement leg; the Lyra G1–G12 legs stay dropped by owner decision.

**Chunk plan** (fail-fast, resumable)

| window | legs | expected | worst |
|---|---|---|---|
| c0 long run (decides whether build 4 needs a scheduling change) | 6 | ~27 min | ~45 min |
| c1 riskiest first (build-3 paths, refusal census, 1080p, legacy) | 39 | ~20 min | ~40 min |
| c2 Stack 720 + remaining identity gates | 42 | ~25 min | ~45 min |
| c3 NoHold + Lyra | 5 | ~15 min | ~35 min |

Estimates come from the saved run timings: 10–25 s per StackOBot leg, 83–90 s per Lyra attempt, and about half of Lyra first attempts invalid.

**Dry run** (`081-23-evidence/dryrun/dryrun-results.json`): 118 of 118 as expected.

| evaluator | known-good passed | known-bad failed |
|---|---|---|
| measure_eval | 33/33 (every Stage-2 720 recipe, count/wall can-fails, the 1080p twins, 081-22 (a)) | 7/7 (including 081-15's 16/20 partial-yield leg) |
| identity_checks | 28/28 core checks on the saved Stage-1 V6 runs | 10/10 (the G2/G8 §D checks correctly fail on the old G2 shape, SI9 `mask_payload_missing`) |
| peak census / refusal census | 3/3 and 1/1 | 1/1 and 3/3 |
| legacy_compare | 6/6 | 8/8 |
| NoHold / Lyra G270 / Lyra twins | 3/3, 1/1, 2/2 | 1/1 each |
| oracle | exit 0 | exit 1 on a mutated row, exit 3 with no sidecar |
| fixture checks | 2/2 | 3/3 |
| c0 analyzer | output byte-identical (as JSON) to 081-22's self-test | — |

The key legacy result: 081-22 (d) native order reproduces its recorded FAIL exactly without the timing-sampled set (9 paths, 3 outside the historical set) and passes with it. The dry run also caught a harness bug before any use: my first comparator treated array entries as interchangeable (`anomalies[2]` counted as `anomalies[]`), which quietly let the event-2 difference through. It now compares exact paths, recorded as `G287`.

Leg types with no saved stand-in, so they are first proven in the bench windows:
- the refusal census in a real run (LOW, G10), tested only on synthetic data;
- the verbose PAIR trace (Stage-1 logs have none, so I built it from the sidecar);
- evidence-ON legacy against pre-m55, and CB legacy against pre-m55;
- G2, G8, G9 and G15 on build 3;
- 1800-frame runs for the c0 analyzer.

**What changed**
- Feature branch, pushed `2531227..a745ee5`, no tag:
  - `b27b082` — predictions (`docs/predictions/2026-09-24-m55-081-23-requalification.md`, byte-identical to `_reviews\081-23-evidence\predictions.md`, sha256 `a1b3c5b6…`).
  - `a745ee5` — the timing-sampled set with its reasons in `change-evidence-fields.md`, gotcha `G287`, the journal, and the CLAUDE.md status block.
- New files only under `_reviews\`:
  - `081-23-derive.py`, which made 12 derived runners by checked single-occurrence text replacements (recorded in `harness-derivation.json`);
  - `081-23-lib.py` (evaluators and runners), `081-23-chunk0..3.py`, `081-23-window.py`, `081-23-preflight.py`, `081-23-postflight.py`, `081-23-dryrun.py`;
  - `081-23-frozen\verify_capture.py` (the oracle copied byte-for-byte from the branch);
  - `081-23-evidence\prep-boundary.json`, which holds the hashes of 34 harness files; every preflight refuses to start if any changed.
- Boundary checked: main checkout still `m51` `53bf725` with no tracked changes; master `031a103`; Lyra `caa68c6`; staged exe 002805CF; scratch worktree removed; zero files changed in the bench bank or in any older `_reviews` file.

**NEEDS-DECISION:** none. For chat's attention: the §D build-3 predictions for G2 and G8 are hard gates, so a miss there stops window c1.

**Wall-clock** (07:39–08:06 IST, about 27 min)

| step | time |
|---|---|
| reading rulings, harness history and the bank | ~9 min |
| derivation, library, drivers, window/pre/postflight scripts | ~7 min |
| dry run (three passes of ~2.4 min each, with two harness fixes) | ~7 min |
| predictions, docs, two commits, push | ~4 min |
| boundary file, preflight smoke check, clean-up | ~2 min |
api-equivalent cost (not billed on Max): $14.18 | turns: 77 | running total across all briefs: $2,130.61
