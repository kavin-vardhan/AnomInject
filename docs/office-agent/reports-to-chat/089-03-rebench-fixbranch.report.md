# REPORT for 089-03-rebench-fixbranch.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 089-03-rebench-fixbranch.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-30 02:23:25 | finished: 2026-09-30 04:10:45 | duration: 107 min | exit code: 0
context at last turn: 385770 tokens (39% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\089-03-rebench-fixbranch.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**1. Plain summary for the owner**
- The labels are now shown in sync with the picture for three anomalies: stuck_low_mip, blinking and lod_popping. That holds with anti-aliasing on and off, and in both tick orders for blinking and lod_popping. All 24 runs went through on the first try.
- camera_clipping with anti-aliasing off still can't be judged by the existing gate. A proposed alternative reading passes, but chat has to rule on whether to use it.
- An older checker (086-01) still flags a few stuck_low_mip events as late; each carries its own "scene drift" note. That is left open.

**2. Per-anomaly table**

The three tools, side by side, are E1 (the frozen 089-03 evaluator), E2 (the 086-01 checker) and the office kit (read as shipped, plus KIT-GT against the game-time null).

| anomaly / leg | E1 | E2 (086-01) | Kit | Class |
|---|---|---|---|---|
| stuck_low_mip B0 | PASS 16. Onset +2 ×16. Offset +5 ×15, +6 ×1 | PASS 6, FAIL 5, CONFOUNDED 5 | Office: PASS 4, CENSORED 12. KIT-GT: PASS 16 | **SYNC-PROVEN** (E1 + KIT-GT) |
| stuck_low_mip B3 | PASS 8. Onset +2 ×8. Offset +4 ×2, +5 ×5, +6 ×1 | PASS 3, FAIL 2, CONFOUNDED 3 | Office: PASS 3, CENSORED 5. KIT-GT: PASS 8 | **SYNC-PROVEN** |
| stuck_low_mip B9 (AA off) | PASS 16. Onset +2 ×16. Offset +0 ×16 | PASS 11, FAIL 5 (predicted scene drift) | Office: PASS 4, CENSORED 12. KIT-GT: PASS 16 | **SYNC-PROVEN** |
| B0L (timing lever, should fail) | FAIL 18, CENSORED 3 | FAIL 19 | KIT-GT: FAIL 18 | Can-fail proven |
| P1 (purity lever, should fail) | Purity FAIL | — | — | Can-fail proven |
| blinking REAL_BL_SYN | PASS 10, +0 / +0. The old 084-09 evaluator still reads FAIL 10 (known G405 artefact) | PASS 10 (2 with a censored edge) | PASS 10 | **SYNC-PROVEN** |
| blinking REAL_BL_NAT | PASS 10, +0 / +0 | PASS 10 (3 with a censored edge) | PASS 10 | **SYNC-PROVEN** |
| lod_popping, 8 gate legs | PASS 23 judged per leg, +0 / +0 | PASS 23 | PASS 23 | **SYNC-PROVEN** |
| MASK55A | Mask gate PASS; E1 PASS 16 (onset +3 ×16) | PASS 6, FAIL 5, CONFOUNDED 5 | Office: PASS 4, CENSORED 12. KIT-GT: PASS 16 | **SYNC-PROVEN** |
| camera_clipping, AA off | As-built gate UNJUDGEABLE (F 0, P 0) | Camera moved | No event judged | **UNJUDGEABLE** |
| Part 0, packages A and B | PASS every check | | | PASS |

stuck_low_mip specifics:
- **Partial-frame histogram (onset, steady events):** 2 frames on every event in B0, B3 and B9, and 3 on every event in MASK55A. Warm-up events show 4–5. Offsets are mostly 0, with at most 3 events at 1.
- **Flagged-invisible frames:** B0 9, B3 5, B9 0, MASK55A 10.
- **OffFrames-16 read-back:** reads 16 on B0, B3, B0L, P1 and MASK55A. B9 reads 0; it is the AA-off leg and was not declared an OffFrames-16 leg.
- **Null coverage:** the 1,800-frame null runs from 0.034 s to 61.934 s of game time and covers all five legs that need it. No event anywhere was left unjudgeable.

lod_popping specifics:
- The relit fixture renders: median frame mean 131.05, and 9 of 9 eligible events were measurable in both premise runs.
- Both variants (N and D) pass 23/23 with 0 off frames under every condition: natural and synthetic tick order, AA on and off.
- Dithered fade: 0 of 46 D edges faded, AA on or off. There are no D-fade findings.

MASK55A specifics:
- Valid on the first attempt: peak live tags 2, recycles 6.
- Aliasing: 0 shared TAG-OWNERS frames and 0 prior collisions.
- Masked fraction: 199 of 199 labelled frames, with 0 unavailable-mask (G295) frames.

camera_clipping, AA off, both readings:
- **As-built gate: UNJUDGEABLE.** Its sphere-proxy can-fail reads not proven, because the gate itself can't be judged.
- **Proposed confirmed-hold reading: PASS.** F 0, P 621,854, 0 confirmed frames disagreeing, edges 0 / 0. The 20 unconfirmed prop frames (140–159) all read D 0.

**3. Disagreements between the tools, per event (unresolved)**
- **E2 fails events that E1 and KIT-GT pass.** Every one carries E2's own "scene drift" offset note:
  - B0 ords 3, 8, 12, 13, 16 (start frames 251, 602, 881, 951, 1161)
  - B3 ords 3, 4 (503, 783)
  - B9 ords 1, 2, 6, 7, 11 (111, 181, 461, 531, 811)
  - MASK55A ords 3, 8, 12, 13, 16
- **The office kit censors offsets that KIT-GT passes:** 12 of 16 on B0, B9 and MASK55A, and 5 of 8 on B3. Neither version fails any event; this is the G396 shape the README predicted.
- **E1 as banked vs the new E1 on REAL_BL_SYN:** FAIL 10 vs PASS 10 on the same session. This is the predicted G405 difference.
- **camera_clipping:** the as-built gate and the proposed reading disagree (UNJUDGEABLE vs PASS).

**4. Pacing, idle waits, prompts and other processes**
- **Pacing:** `speed_ratio` stayed between 1.0000 and 1.0430 on every leg.
- **Idle:** the PC had been idle 8,630 s at launch. The harness's person gate then waited 16 times, 8–52 s each (about 9 min in total). Each wait followed an attempt shorter than 60 s, which fits our own game launch resetting the input clock, but that is not established. No input appeared outside our launches.
- **Prompts:** 0 Windows Security prompts were raised or closed, and the firewall-rule count stayed 16 → 16 on both Part 0 launches.
- **Other processes seen, all untouched:** `UnrealTraceServer.exe`, and unrelated `python.exe` processes (uv-managed tools and a HeistCrewUE MCP server).

**5. Staging at the end**
The M53 set is back in place, confirmed by my own hashes after the harness's restore: exe `2FCDF059`, utoc `20DA6F98`, ucas `534C5863`, pak `FD766B7B`, global.utoc `462B8AC6`, global.ucas `BB05CF99`. No game or editor is running. The harness's postflight passed, and the freeze check still read OK after evaluation.

**6. Commits pushed**
`d1ab068` on `fix/m52-label-timing` (from `ef4cc16`, now equal to origin): the journal `docs/sessions/2026-09-30-089-03-rebench.md` plus the status block. It was committed from the `_r84_host` worktree, so `m51` (`53bf725`), `master`, tags, `ToCodex\` and `E:\<personal-folder>` were not touched. Evidence is in `_reviews\089-03-evidence\run\` (start with `report.md`); the banks are `D:\IA_BankOverflow\M52S10_*`.

**7. NEEDS-DECISION**
1. **camera_clipping, AA off:** should the proposed confirmed-hold reading (PASS) become how this leg is judged, since the as-built gate stays UNJUDGEABLE?
2. **E2's scene-drift FAILs on B0, B3 and MASK55A:** these were not predicted for those legs. They need to be classed either as a confound of E2's own reference (as 089-01 ruled for B9) or as something to investigate.
api-equivalent cost (not billed on Max): $6.79 | turns: 49 | running total across all briefs: $3,786.14
