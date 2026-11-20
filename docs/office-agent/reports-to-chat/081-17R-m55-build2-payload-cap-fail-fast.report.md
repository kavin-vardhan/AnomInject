# REPORT for 081-17R-m55-build2-payload-cap-fail-fast.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-17R-m55-build2-payload-cap-fail-fast.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-23 23:50:34 | finished: 2026-09-24 00:59:19 | duration: 69 min | exit code: 0
context at last turn: 652198 tokens (65% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-17R-m55-build2-payload-cap-fail-fast.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

## 1. Verdict
**GREEN.** Steps A–F are all done.
- **Step D:** all four StackOBot 1080p twins passed on their first attempt.
- **Step E:** Lyra G270 at 300 frames reached the **PASS** branch.
- **Stage 3 is not started.** The 70-leg requalification brief comes first.

Full report: `_reviews\081-17-code-m55-build2-report.md` (sha `b1170a3a9f92…`).

## 2. Plain summary for the owner
- The memory cap is now 256 MiB, and the plugin records exactly what it is holding whenever memory peaks or runs out.
- It took one build, which succeeded first time with no warnings. Every StackOBot 1080p test passed with every required measurement present and no memory refusals.
- The Lyra 1080p test also passed: five events, every required measurement present, nothing refused.
- Worth knowing: Lyra used 197 MB, about three quarters of the new cap. It passed, but the headroom on a real game at 1080p is modest, and 1440p would probably not fit.
- Another project's Unreal editor started on this machine partway through. I didn't touch it and waited 39 minutes for it to close before building.

## 3. What changed
All changes are on `feat/m55-change-evidence` and pushed. No merge, tag or cook.

| Commit | What |
|---|---|
| `db9e74d` | Predictions amendment, pushed before the build and before any result. |
| `9c75abe` | The built source: `AnomalyChangeStage.{h,cpp}` (+146/−5) and the field reference. |
| `fcfde07` | Journal `docs/sessions/2026-09-24-081-17-code-m55-build2.md`, measured readings added to the field reference, G282 addendum, `CLAUDE.md` status block. |

The source change, and nothing else:
- **Cap:** the compiled default of `IAI.Capture.ChangeMaxBytes` goes from 64 MiB to 256 MiB (268,435,456). Admission, the completion clock and every refusal rule are unchanged from 5649a6d.
- **Diagnostic (i):** every refused colour or mask reservation, including bench gate 10, writes one `BUDGET-EXCEEDED` log line and adds a `budget_census` array to that index's pair row.
- **Diagnostic (ii):** each new high-water point is recorded after the buffer's owner is known.
  - `HIGH-WATER` log lines are throttled to one per colour-sized rise.
  - One `HIGH-WATER-PEAK` line is written at closure.
  - The run summary gains `change_bytes_peak_census`.
- **What the census records:**
  - bytes held;
  - distinct colours, split into pending / previous / phase-reference / in-hand, with a shared buffer counted once;
  - masks and their bytes, and any unaccounted bytes;
  - cursor and latest index;
  - open phases;
  - what the head is waiting on.
- **In-hand tracking:** `Work()` now keeps a pointer to the item it is finalizing so the census can see it. Nothing else depends on it.
- There is no closure-bound field and no formula-based warning. No comments were added.

## 4. Build identity
- **Game exe:** `8e64fa39…0650` (**8E64FA39**), 241,763,328 B. The built, staged and archived copies (`_binary_baselines\StackOBot.exe.m55-stage2-8E64FA39`) matched before every leg.
- **Editor modules:** archived to `m55-stage2-r17-main-editor-9c75abe`.
- **Lyra modules:** archived to `m55-stage2-r17-lyra-48b9e048`.
- **Cooked containers:** all five unchanged.
- **A44 scan:** all four new tokens are present in the staged exe and the Lyra DLL; the invented control token is absent.
- **Build times:** StackOBot 113 s, StackOBotEditor 85 s, LyraEditor 61 s, all exit 0.

## 5. Leg table
"Budget" means the refusal counter / `BUDGET-EXCEEDED` log lines / rows carrying `budget_census`.

| Leg | Attempt | Fixture | Measured / required | Budget | High-water (bytes) | Peak census | Latency ms p50/p95/max | Result |
|---|---|---|---|---|---|---|---|---|
| NULL1080 N | 1 | valid, foreground 86/86 | 20/20, 5 events | 0/0/0 | 51,867,648 | 4 colours + 9 masks | 296/334/350 | PASS |
| NULL1080 S | 1 | valid, foreground 86/86 | 20/20, 5 events | 0/0/0 | 64,315,392 | 5 colours + 11 masks | 334/434/455 | PASS |
| SOLID1080 N | 1 | valid, foreground 83/83 | 20/20, 5 events | 0/0/0 | 66,392,064 | 5 colours + 12 masks | 387/446/474 | PASS |
| SOLID1080 S | 1 | valid, foreground 87/87 | 20/20, 5 events | 0/0/0 | 70,545,408 | 5 colours + 14 masks | 390/476/512 | PASS |
| Lyra G3 A1 | 1 | invalid before capture: the host respawned after placement | — | — | — | — | — | fixture attempt 1 of 3 |
| Lyra G3 A2 | 2 | valid, foreground 218/218, L1 0.115 cm / 0°, L2 pre and post passed | 20/20, 5 events | 0/0/0 | 196,992,000 | 22 colours (21 pending) + 7 masks | 1191/1551/1629 | **PASS** |

On every leg:
- the startup line reports `maxBytes=268435456(from compiled)`;
- retained bytes are 0, all mismatch, persistence and row-drop counters are 0, and there were no closure timeouts, out-of-order timeouts or late results;
- the census accounting closes: 0 unaccounted bytes and no unowned colour.

## 6. Readings
**Refusal reasons across every captured frame:**
- StackOBot: 1 `first_frame` and 18–19 `mask_payload_missing`.
- Lyra: 1 `first_frame` and 264 `mask_payload_missing`.
- The `mask_payload_missing` frames are unlabelled frames, which have no mask; none of them is a required measurement.

**Onset vs whole-frame control, unsubtracted:**
- **Null twins:** target 0.00004–0.00023, control 0.0044–0.0048.
- **Solid twins:** target 1.0 on all ten onsets, control 0.009–0.013.
- **Lyra `stuck_low_mip` (five events), target vs control:**
  - 0.584 vs 0.530
  - 0.027 vs 0.069
  - 0.00009 vs 0.0013
  - 0.0011 vs 0.0042
  - 0.1015 vs 0.0976

**Why G270 passed:** its first valid capture had 5 events, every phase had 4 of 4 pairs measured, and nothing was refused or timed out.

**Lyra memory, facts only:**
- Across the capture the peak climbs from 8 MB to 197 MB.
- The gap between the newest frame and the frame being processed grows from 12 to 53, and the colour buffers waiting behind it grow from 1 to 21.
- At 18 of the 20 peak readings, the frame at the front of the queue already had every input it needed but had not been processed yet.
- The measurement worker and the PNG writer share the same thread pool. That is a possible explanation, but I have not established it (G120) and did not chase it.

**Predictions I got wrong (readings, not gates):**
- StackOBot high-water: predicted 67–120 MB; three of four legs came in lower.
- StackOBot latency p50: predicted 400–550 ms; actual 296–390.
- StackOBot peak masks: predicted at least 10; the null N leg had 9.
- Lyra high-water: predicted 60–140 MB; actual **197 MB**.

## 7. Deviations and harness faults
1. **My preflight checker was wrong on its first run.** It resolved bank paths from the wrong directory. I fixed it once, before any bench action, and the re-run found 0 mismatches.
2. **Another project's editor (not ours) ran mid-round.**
   - A UE 5.7 editor for HeistCrewUE (PID 17236) started at 00:02:25.
   - My build script refused at its idle check before doing anything.
   - I waited, never touched it, and it closed on its own at about 00:42. Nothing ran while it was open.
3. **Date mismatch.** The predictions header is dated 09-24, but it was committed at 23:59:53 on 09-23.
4. **Harness copies.** I copied Codex's 081-15 scripts under 081-17 names and changed only:
   - the evidence and bank paths;
   - the binary manifest;
   - the added pass checks;
   - a guard against reusing a leg label.

   No harness fixes were needed once the legs started.
5. **Lyra attempt 1 was fixture-invalid** because the host respawned. Under the 081-14 rules it counts as fixture attempt 1 of 3.
6. **The feature branch's earlier m55 code contains comments**, which the repo convention forbids. I left them, since removing them is outside this build's scope. The comment stripper must run before merge.
7. **Scratch files.** My temporary folder was removed; the drafts are kept in `081-17-evidence\drafts\`.

## 8. Boundary proof
`postflight.json`: **"PASS: boundary restored (main m51 53bf725, Lyra caa68c6 with original 9 modules)"**, `problems: []`. It covers:
- 1,684 prior artifacts and 13,015 prior bank files re-checked;
- CURRENT.md unchanged;
- the remote feature branch at `fcfde07` and master at `031a103`;
- no engine processes left running.

The main checkout on `m51` still holds the build-2 Game and Editor binaries.

## 9. NEEDS-DECISION items
None. These are for your attention only:
- **(a) Lyra headroom.** Lyra used 73% of the cap with a growing queue behind a ready frame. The cause isn't established, and 1440p would likely exceed the cap.
- **(b) Refusal logging is unproven at runtime.** No refusal happened, so diagnostic (i) never ran. The next brief's G10 and `ChangeMaxBytes 1` legs have to show it working.
- **(c) One extra summary key.** The next brief's legacy comparisons will see exactly one added `run_summary` key, `change_bytes_peak_census`.
- **(d) Work remaining.** 70 legs remain on build 2.

## 10. Wall-clock per step (IST)

| Step | Duration | Notes |
|---|---|---|
| A | about 4 min | 23:51–23:55 |
| B | about 5 min | 23:55–23:59 |
| C | about 47 min | 8 min of real work plus 39 min waiting for the other editor |
| D | about 3 min | |
| E | about 5 min | |
| F | about 6 min | |
| **Total** | **about 68 min** | about 29 min without the wait; 6 launches, 5 captures, 1 build |

Environment note: several MCP connectors (Figma, GitHub, Slack and others) need authorization through their connector settings or `claude mcp`, and the pdf-viewer server failed to connect. None of them were needed for this round.
api-equivalent cost (not billed on Max): $19.26 | turns: 131 | running total across all briefs: $2,021.82
