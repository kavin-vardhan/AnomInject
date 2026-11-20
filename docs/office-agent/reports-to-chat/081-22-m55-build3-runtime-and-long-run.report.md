# REPORT for 081-22-m55-build3-runtime-and-long-run.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-22-m55-build3-runtime-and-long-run.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-24 04:33:48 | finished: 2026-09-24 07:32:30 | duration: 179 min | exit code: 0
context at last turn: 630586 tokens (63% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-22-m55-build3-runtime-and-long-run.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION. Build 3 finally ran in the engine and three of the four owed legs passed. The fourth (d), evidence-OFF byte identity against pre-m55, failed its declared gate in native tick order, so the stop rule fired and the long-run readings (Part 2) were not started. The boundary is restored and proven.

## 2. Plain summary for the owner
- The other project's Unreal editors ran until about 07:10. The PC was quiet for only five minutes (07:13–07:18), and every Part 1 test fitted inside that window.
- The normal 1080p test passed: all 20 measurements present, and the checker agrees with every one.
- Cancelling a capture before the game window has focus now leaves nothing behind. A real split-screen game gets its normal pictures and masks exactly as before m55, and the new feature correctly marks those frames "unsupported".
- In the like-for-like comparison with the old build, pictures, masks and labels match. One annotation number, `coverage_pct` (how much of the screen the target covers), differs slightly (by at most 0.47 %) on three events in normal tick order.
- By the rules written before the test, that counts as a failure, so the long ON/OFF tests did not run. Chat needs to decide how to treat it.
- Another editor from the other project started at 07:18:28, 22 seconds after the last test finished.

## 3. Waiting log
- **04:34:27 IST, first check:** four UE 5.7 <other-home-project> editors (PIDs 153756, 96312, 76640, 104420, started 01:32–02:31) were running. They used about 1 core each; PID 104420 used about 2 cores and had a window titled "Error".
- **Polls 1–18** (≤ 9 min each, 60 s checks, every check logged to `environment-events.json`) were all NOT-QUIET.
  - Poll 18 crashed once: a child PowerShell failed with "Thread failed to start" while the commit charge was about 71.7 of 85 GB.
  - The check now retries and counts a failed check as not quiet; it was re-run as poll 18b.
- **Quiet from 07:13:27** (second consecutive quiet check), after **2 h 39 min** of waiting, about 66 % of the 4-hour budget.
- **Part 1 ran 07:13:34–07:18:06.** No foreign process appeared during any leg (runtime checks at startup, before `capture_start` and every second).
- **New foreign editor PID 97980** (<other-home-project>, `/Engine/Maps/Templates/OpenWorld`) started at 07:18:28 and is still running. There were no mid-leg interruptions.

## 4. Part 1

| Leg | Att. | Result | Key numbers | Oracle |
|---|---|---|---|---|
| (a) `A_SOLID1080_N`: 081-17R solid1080 recipe, 90 frames | 1 | **PASS** | See (a) below | **exit 0**, 20/20/20/0 |
| (b) `F1_CANCEL`: window minimised, start, stop after about 3 s | 1 | **PASS** | See (b) below | — |
| (c) `F2_TWOVIEW`: `DebugCreatePlayer 1`, CB 1080p, order b3/pre/pre/b3 | 1 each | **EXERCISED, PASS** | See (c) below | build 3 exit 0 (nothing to compare) |
| (d) native order: MainWorld 081-12 recipe, evidence OFF, order pre/b3/b3/pre | 1 each | **FAIL** (declared deep-diff gate) | See (d) below | — |
| (d) synthetic tick order | 1 each | **PASS** | All rules pass, key presence OK | — |

**(a) 1080p smoke**
- 20/20 pairs measured, 5 events; onset changed-pixel fraction (gt8) 1.0 on all five against a control of 0.009–0.013.
- Budget refusals 0/0/0, invariants 0, census self-check clean, 71 frozen masks verified.
- `change_multi_view_families`, `change_unsupported_completion` and `change_view_rejected` are all 0; the EFFECTIVE line reads `gate=14(from console)`.
- High-water 66,392,064 B, identical to build 2. Latency p50/p95/max 386/430/436 ms, `speed_ratio` 1.0006, 90 PNGs, foreground 79/79.

**(b) Cancel before focus**
- The game was never foreground (51 samples); `capture_status` read running=true, frames=0.
- The session folder existed before the stop (empty `Actual_Frames`).
- The log has one `CANCELLED before focus` line and one `STAGE-DISCARDED … issued=0` line.
- There was no session folder and there were zero files 5 s after the stop and again after the process exited; only the empty capture root remains.

**(c) Two-view family**
- Delivered frames are 1920×540, one half of a horizontal split screen.
- Build 3: `change_multi_view_families` 120 and `change_view_rejected` 150. All 30 m55 pair rows are `unsupported_delivery`, with 0 measured.
- Legacy output equals pre-m55 on every compared field, including an identical `SVE-WANT-SUMMARY`: 90 marks, 90 wanted, 180 submits (the pre-m55 multi-view double readback), 90 frames.
- Pending arms on every leg: `pendingWantedAtEnd` 0, `maxPendingDepth` 1.

**(d) Native order**
- Every mask file is byte-identical, and the whole 081-12 field set is equal on all 6 pairs.
- Evidence OFF was read back on build 3, and key presence is OK: build 3 adds or removes no JSON path.
- 9 JSON paths differ between build 3 and pre-m55 on all four cross pairs and on neither same-binary pair; 3 of them fall outside the historical run-unique set as well.

Observed values on the paths that differ in native order:

| Path | pre-m55 D_N1 / D_N4 | build 3 D_N2 / D_N3 | all 4 synthetic legs (both binaries) | 081-12 bank (6 m55 legs) |
|---|---|---|---|---|
| `annotation/anomalies[2]/coverage_pct` | 8.6927 / 8.6927 | 8.6518 / 8.6518 | 8.6518 | 8.6518 |
| `anomalies[6]` / `anomalies[7]` | 9.25045 / 9.25495 (both legs) | 9.24997 / 9.25479 | same as build 3 | 9.2500 / 9.2548 |
| `end_frame` / `capture_game_ticks` | 122 | 121 | 122 | 121 or 122 within one binary |
| `key_ring_published` / `consumed` | 121 | 120 | 121 | 120 or 121 within one binary |
| `key_ring_wrapped`, `ticks_per_captured_frame` | 57, 1.3556 | 56, 1.3444 | 57, 1.3556 | vary within one binary |

- The pre-m55 pair also differs from itself on events 3 and 5. The anchor frames, occlusion samples, poll distance and label camera rows are identical on every leg.
- `coverage_pct` is computed from the **live** view when the capture drain processes the anchor frame, not from the frame's own view. m55 changes the drain code path. That is a candidate explanation only; the cause is not established.
- The binary the 081-12 comparisons called the predecessor, `AEBD09EA`, is actually the **m55 stage-1** build. So this is the first time a pre-m55 binary has run this recipe, and every m55 build on record reads 8.6518 at event 2.

## 5. Part 2
**NOT RUN, because of the stop rule.** L1, L2, L3 and L4 (Lyra) and S1 and S2 (StackOBot) were never launched, and the Lyra modules were never staged. The PC has been busy again since 07:18:28 in any case. The harness is ready: runners, drivers, and the analyzer re-validated against 081-17R's known answers. The 081-18 predictions are unchanged.

## 6. Deviations and harness changes
- **Comparator hole found before any leg.** I tested the (c)/(d) comparator on 081-12's banked legs first. The declared rule missed a difference present in only one build-3 leg. **AMENDMENT 1** was appended to the predictions before any leg and only tightens the gate:
  - key presence: build 3 may add or remove no JSON path;
  - native order only: a strict rule whose run-unique set comes from non-build-3 same-binary pairs.

  Known-good passes; both must-fail cases are caught. New gotcha G286.
- **Preflight** has two declared external updates:
  - chat's scorecard file (edited at 04:33);
  - 081-21's `postflight.log`, whose manifest entry is the hash of an empty file (081-21 hashed it before its own output arrived).
- **Runners** were derived from 081-18's by asserted replacements. They add a foreign-editor check.
- **Lyra runner (not used this round):** it now records a post-capture L2 failure instead of retrying the leg; 081-18's driver would have retried, against its own brief.
- **INVALID-FOREIGN attempts** count toward the 3 attempts per leg. None occurred.
- **Docs commit** `2531227` on `feat/m55-change-evidence`, pushed: journal, predictions copy, status block, G286. No source, build, merge, tag or cook.
- **Memory:** the foreign-editor note now records tonight's timings.
- **Environment:** several MCP connectors need authorization and the PDF viewer failed to connect; none was needed.

## 7. Boundary proof
`_reviews/081-22-evidence/postflight.json`: **"PASS: boundary restored (main m51 53bf725, Lyra caa68c6 with original 9 modules, staged exe 002805CF = build 3, every binary, container, prior artifact and bank file byte-identical to preflight, scratch worktree removed)"**, `problems: []`. It verified 2,387 prior artifacts and 14,031 prior bank files, and hashed 3,285 new bank files. Remote: feature at `2531227`, master at `031a103`.

## 8. NEEDS-DECISION items
1. **The (d) native-order difference.** Chat can either:
   - accept `coverage_pct` and the six counters as run-variable, since `coverage_pct` is sampled from the live view at drain time, and add them to the declared noise set; or
   - have it diagnosed first, for example with more pre-m55 native legs or by sampling provenance from the arm-time view.

   It is client-visible (`annotation.json`, both delivery modes) and at most 0.47 % relative.
2. **Then Part 2 is re-briefed.** Its harness is ready. The quiet window tonight was 5 minutes, so the PC-sharing decision still stands open.

## 9. Wall-clock (IST)

| Step | Window |
|---|---|
| Reading, preflight (1 declared rerun), harness, comparator self-test and amendment | 04:34–04:58 |
| Waiting for a quiet PC (18 + 1 polls) | 04:34:27–07:13:27 (2 h 39 min) |
| Leg (a) | 07:13:34–07:14:03 |
| Leg (b) | 07:14:09–07:14:24 |
| Leg (c), 4 launches | 07:14:34–07:15:30 |
| Leg (d), 8 launches | 07:15:58–07:18:06 |
| (d) analysis, docs, push, postflight | 07:18–07:32 |

In total about 3 h. Active work was about 45 min, of which Part 1 took 4.5 min for 14 launches.
api-equivalent cost (not billed on Max): $19.97 | turns: 144 | running total across all briefs: $2,116.43
