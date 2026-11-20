# REPORT for 094-27c-europa-a6-camera-fix-reauthor-runtime-run.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-27c-europa-a6-camera-fix-reauthor-runtime-run.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-30 15:40:08 | finished: 2026-10-31 02:17:50 | duration: 638 min | exit code: 0
context at last turn: 689941 tokens (69% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-27c-europa-a6-camera-fix-reauthor-runtime-run.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION. Both test maps were rebuilt with the camera fixed, and all 18 runtime legs ran. Only the load test (A3) passed. Two defects in our own test code caused most of the failures, and there was one crash, caused by our probe. Nothing protected changed, nothing was uploaded, and the canary appears nowhere. Codex called the run RED; under this brief's definitions it is NEEDS-DECISION.

**Plain summary for the owner**
1. Last night Unreal rebuilt the two test maps (19:11 and 19:28), then ran 18 short test sessions between 19:48 and 01:42. The camera now looks down properly.
2. Only the lighting anomaly produced a full set of good pictures. Two of our own bugs spoiled several other tests, and one test crashed.
3. Nothing Unreal is running now; nothing needs closing.

**Start state (15:42 IST)**
- `m51` matched P9: HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the same 15 untracked entries. It matched again before launch and after the job.
- 094-27b's report and review show no protected change and no upload.
- The protected-state check passed at the start and again after the job.
- The feature branch was at `0a02987`, clean, with the three old A6 packages byte-equal to 094-26d.
- 4.25 manifest: 27 files (`c501895c…`). EditorRuns held `31124` and `48368`. The Epic launcher was running.
- Free space: D: 47.3 GiB, E: 286.5 GiB.

**The Codex job**
- Thread `01a14334`, run folder `…\runs\2026-10-16-094-01-europa-phase-a-plan`, resumed on the first try.
- Ran 15:46:49 → 02:04:36 IST (37,066 s), exit 0, no retries, no foreign drive-root entries.
- Its pauses were all waits for an idle PC or for headroom: D: dipped twice as the page file grew (to 32.1 and 26.8 GiB), and memory was low under your DaVinci, Blender and python jobs.
- Codex's verdict: `RED | Two authoring sessions; 18 runtime sessions; A3 PASS, A5 FAIL, A6 FAIL, A7 FAIL, A8 FAIL; evidence retained; no push.`

**The fixes and the run-up**
- **FI2:** my own scan of all 93 tool scripts finds 0 positional `Rotator` calls in anything either kit runs. The only two are in the 5.1 stress recipe (`author_51_stress_fixture.py:33,35`), which neither kit runs. The stub now has `Rotator(roll=0.0, pitch=0.0, yaw=0.0)`, and the old camera line, run through the harness, gives roll −12 with 0 saves and 1 exit request.
- **FI3:** I re-ran the harness can-fails and they pass: one exit request, no load after a failed check, the callback unregistered on its first raise. Codex's crash-cause line citations are correct. The Niagara crash cause stays unconfirmed. New evidence: the real Load leg's guard ignored 10 re-entrant ticks, so the re-entry is now observed.
- **FI4:** my can-fails against the kit's own functions passed 15/15. A first crash continues the run, a second ends it, and a session-limit stop is not counted as a crash.
- **Suites:** I re-ran the runtime suites: FI 37, FH 34, readers 22 and runtime 122 cases pass, and all 138 real-file mutations are rejected. The four fixture suites need a job config, so I relied on Codex's results for those.
- **Rehearsal, armings and live readings:** both 10-group rehearsals passed, and both armings are for 09427c. The runtime arming binds the new package hashes. All 18 dry runs passed with 0 launches, and my own 3-poll live compare matched the kit's gate.
- **The recipe's save calls changed.** FI2 said the bindings stay the same, but Codex switched to `new_blank_map` + `save_map` so that nothing is saved before the camera check. These run the same engine code `new_level` used internally (`EditorLevelLibrary.cpp:534-545`), and both real sessions worked.

**The authoring**

| Recipe | Gate wait | Start–end (IST) | Duration | Exit | Camera read-back |
|---|---|---|---|---|---|
| EA_Core | 73 s | 19:11:46–19:12:23 | 36.6 s | 0 | pitch −12, yaw 0, roll 0 (passed before any save) |
| EA_Travel | 536 s | 19:28:26–19:28:39 | 12.4 s | 0 | pitch −12, yaw 0, roll 0 (passed before any save) |

New outputs: `SM_EA_TwoLod.uasset` 137,553 B `c4fb3976…`, `EA_Core.umap` 23,168 B `3c9c34dc…`, `EA_Travel.umap` 21,124 B `6b7c9dd6…`. By my own before/after lists, nothing else under Content, Config, the descriptor or the `.uproject` changed.

**The runtime run, per leg** (every leg was admitted after ≥10 min idle)

| Leg | Start–end (IST) | Duration | Exit | Reader |
|---|---|---|---|---|
| Smoke | 19:48:42–19:48:55 | 12.8 s | 0 | PASS |
| Load | 19:50:58–19:51:13 | 15.1 s | 0 | PASS |
| WorldsPIE | 19:53:22–19:54:36 | 74.6 s | 0 | FAIL (no probe stop) |
| WorldsGame | 20:08:35–20:09:03 | 27.3 s | 125 | FAIL (memory stop) |
| GC | 20:20:55–20:21:02 | 6.7 s | 3 (crash) | FAIL |
| A7 blinking positive | 20:23:03–20:23:15 | 12.7 s | 125 | FAIL (probe-file error) |
| A7 lighting positive | 20:25:09–20:25:34 | 25.0 s | 0 | PASS |
| A7 lod positive | 20:37:45–20:38:13 | 27.5 s | 0 | FAIL |
| A7 missing_object positive | 20:50:55–20:51:25 | 29.7 s | 0 | FAIL |
| A7 blinking NoApply | 21:03:46–21:04:13 | 26.8 s | 0 | FAIL |
| A7 lighting NoApply | 21:33:01–21:33:27 | 25.5 s | 0 | FAIL |
| A7 lod NoApply | 21:45:32–21:45:52 | 20.5 s | 0 | PASS |
| A7 missing_object NoApply | 21:57:58–21:58:18 | 20.6 s | 0 | PASS |
| A7 blinking SkipRestore | 22:10:31–22:10:55 | 24.5 s | 0 | FAIL |
| A7 lighting SkipRestore | 23:55:09–23:55:46 | 37.0 s | 125 | FAIL (probe-file error) |
| A7 lod SkipRestore | 01:13:34–01:13:49 | 15.3 s | 125 | FAIL (probe-file error) |
| A7 missing_object SkipRestore | 01:15:27–01:20:31 | 303.5 s | 125 | FAIL (session limit) |
| Admission | 01:32:00–01:42:00 | 600.4 s | 125 | FAIL (session limit) |

**The gates**

| Gate | Reader | Mine |
|---|---|---|
| A3 | PASS | Agree: 42 of 42 saved values checked on each map, the camera (−12, 0, 0) included; one load per map, one exit |
| A5 | FAIL | Agree |
| A6 | FAIL | Agree: confirmed probe defect, `NewObject<UObject>` at `EuropaPhaseAProbeDriver.cpp:437` asserts in `-game` (`UObjectGlobals.cpp:2282`) |
| A7 | FAIL | Agree; my counts from the raw rows equal the reader's on all 12 legs (below) |
| A8 | FAIL | Agree |

A7 per anomaly (valid / applied / reverted):
- lighting_mismatch: 5 / 6 / 6, PASS.
- lod_corruption: 6 / 6 / 6, but no visible change in any picture.
- missing_object: 6 / 6 / 6, rejected for clean-frame noise.
- blinking: 0 / 0 / 0 (its leg was stopped after 12.7 s).

**The pictures**
- **Index:** `D:\IA_Europa\Receipts\09427c\contact-sheet-index.json`. There is 1 counted sheet of the 4 expected, plus 3 diagnostic sheets marked NOT COUNTED (lod_corruption, missing_object, and blinking from its SkipRestore control).
- **The counted sheet** is `runtime-attempts\A7_-lighting_mismatch-positive\01\lighting_mismatch-contact-sheet.png`. All 15 pictures exist, are 1280×720, belong to their events and have an anti-aliasing echo of 0.
- **My look at it:** the view is level and slightly downward, the dimming shows during each event, and the after frames match the before frames. But the lit frames are blown out to white.

**My review** (`_reviews\094-27c-code-review.md`, 51 rows: 49 OK, 1 GAP, plus the gate section)
- **G1, blocking:** the kit's monitor reads `probe.jsonl` while the probe writes it with an exclusive handle (`EuropaPhaseAWorldProbe.cpp:106` vs `runtime_monitor.py:160-166`).
  - Seen on 3 legs: the kit's own read failed with a permission error, so the kit stopped the leg (exit 125).
  - Strong candidate, not directly seen, on 3 more: the probe disables itself silently when its own write fails (`:107-109`). That fits WorldsPIE (probe went quiet ~186 frames before a clean exit) and missing_object SkipRestore and Admission (quiet ~290 s and ~593 s before their session limits).
  - Codex did not identify this cause.
- **G2, blocking for A6:** the GC probe line above needs a C++ change and a build.
- **G3:** lod_corruption and missing_object count 0 despite 6 valid events each; the saturated lighting may be hiding the change.
- **G4:** three control legs failed only because the kit's monitoring did not cover the start of their event window.
- **Observations:**
  - **Canary scan:** Codex's scan ended GAP on 4 unreadable `C:\Windows` files (access denied, outside the FE2 and FI5 classes). My own scan of our files found 0 matches, and my planted control was found 3 of 3.
  - **Bindings:** the recipe's changed save calls (above) ran without a smoke session first.
  - **WorldsGame** was stopped because your jobs used up memory.
  - **No trace of the GC crash on disk:** no crash folder and no EditorRuns marker; only exit code 3 and the crash monitor's log.
- **My independent results:**
  - EU1 audit: 510 commands, 509 through the wrapper, 1 direct read-only, 0 direct writes.
  - Exactly 20 Unreal launches (2 authoring, 18 runtime).
  - Every UnrealBuildTool child was ValidatePlatforms only.
  - The privacy switches were on every argv, and there were 0 upload lines in any log.
  - No new crash folders, the evidence seals are intact, and charged C: growth was 40 MB.
  - No push and no tags.

**Owner actions**
- **Eye check:** open the lighting_mismatch contact sheet named above. Each row should look slightly downward on the cubes, dimmer in the middle column, and the same left and right.
- **Optionally,** open `lod_corruption-diagnostic-contact-sheet.png` and `missing_object-diagnostic-contact-sheet.png` in the same folder pattern, and say whether you can see the anomaly in the middle column.
- **On screen:** nothing Unreal is open.

**NEEDS-DECISION**
1. The verdict itself: Codex called it RED, but no RED condition occurred (one runtime crash and failed gates count as NEEDS-DECISION in this brief).
2. G1: fix both sides of the probe-file conflict before any re-run. The probe would write with shared read access and stop disabling itself silently; the monitor would retry when it hits a sharing error.
3. G2: the GC probe fix needs a C++ build. Should it come in the same job as G1?
4. G3 and G4: whether to re-light the fixture or re-calibrate the picture readers, and how to fix the control legs' monitoring coverage.
5. The 4 access-denied `C:\Windows` files: whether to widen FE2/FI5 to cover them.

Separately: Google Calendar and the plugin connectors need authorizing in claude.ai connector settings or via `/mcp`, and the meshy and PDF-viewer servers failed to connect. This job didn't use any of them.
api-equivalent cost (not billed on Max): $26.28 | turns: 203 | running total across all briefs: $5,266.20
