# REPORT for 094-40-europa-port-finish-take4.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-40-europa-port-finish-take4.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-02 04:03:23 | finished: 2026-11-02 05:03:31 | duration: 60 min | exit code: 0
context at last turn: 562009 tokens (56% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-40-europa-port-finish-take4.live.txt

---- FINAL MESSAGE (Claude Code) ----
```
VERDICT: GREEN | FX2 and FX3 hold. The EA_Core smoke passes every gate by my own numbers. FV6 is verified, the scan is clean, feat/europa-port = d1cdc7b is pushed (ls-remote equal), and the office capture card is written.

FOR THE OWNER
- Yes, the 4.25 port is ready for the office capture test. The last bug (missing_object never firing) and the invisible LOD swaps are both fixed, and every check passed on the first try.
- Code pushed the tested branch to GitHub as you asked: commit d1cdc7b, with your drive-folder list removed from the history first.
- The office card is written. You do nothing at home.

THE FIXES (my review: _reviews\094-40-code-review.md)
- Codex: exit 0, 1,612 s (04:06:10 to 04:33:03), one resume try. Its report is collected byte-identical as _reviews\094-40-codex-report.md (sha256 0BEC402D...).
- FX2: the 4.25 bench now starts and seeds the injector once per session. Wanted = !Tail && (Started || Eligible), at AnomalyCaptureLabels425.cpp:93. Runtime log: one "IAI.Auto.Run -> ON (seed 4242)" and one OFF; 69 draws numbered 1-69 with no restart; the old repeating 8-draw block is gone. The shared 5.1 SetRunning is unchanged.
- FX3 (the LOD fix): on 4.25 auto picks only, PredictDrawnLod425 predicts the drawn LOD on the game thread from the player's CalcSceneView, using stock ComputeStaticMeshLOD. ForcedModel then forces the coarsest LOD, or LOD0 if the coarsest is already drawn. FW3's 2-LOD guard is kept.
  - I checked the 4.25 citations against E:\EpicGames\UE_4.25: SceneManagement.cpp:451-461 and :543-568, StaticMeshRender.cpp:151-174 and :2081-2150.
  - The parity inventory (09438-auto-parity.json, 09440_amendments) and G510/G511 log it as a deliberate 4.25 addition.
- The LOD sweep: 19 of 19 LOD events pass. Each predicts drawn=1 of 2 and forces model 1 (LOD0, the sphere). On every one of the 13 affected frames per event, 14,455-14,475 of the 15,022 target-mask pixels differ by more than 2 levels from the preceding capture.
- Builds: Editor attempt 1 (56 s) and Game attempt 1 (75 s), both at 08fe652, exit 0, 0 errors, BelowNormal priority, 2 parallel actions. The new Injector DLL and exe contain the Auto.LodChoice log format.

SELF-CHECK LINES (verbatim in the Codex report)
- start: frames=1, 1280x720, format 18, D3D11, PostRenderViewFamily 5, synth 0, pre_ui 1, ui 4/4/4, ui_hits 0, ui_paired 1, fixed_step 1, fps 30, census bench_off, lost/no_pass/polluted 0.
- stage 300: frames=300, PostRenderViewFamily 316, ui 8/8/8, ui_checked 300, ui_hits 0, ui_paired 8, tick_calls 1630, census on, lost/no_pass/polluted 0.
- tick, tick_source, tick_type and tick_type_source are build constants.

THE SMOKE (EA_Core, as FX6 requires; slot 01, 04:20-04:22 IST; bench-only 0.5% selection floor; retry slot unused)
- Fires (each needs 3 or more): PASS. blinking 22, lighting_mismatch 13, lod_corruption 19, missing_object 15.
- Every LOD event changes target pixels: PASS, 19 of 19.
- Masks: PASS. All 731 affected frames have target pixels, and each PNG equals the raw mask. The 1,621 clean frames have no PNG and 0 mask pixels.
- I opened one mask per anomaly:
  - blinking: the hidden cube.
  - lighting: floor and two cubes.
  - LOD: the forced sphere.
  - missing_object: the hidden cube's silhouette.
- annotation.json: PASS. Schema 2.1; the frame sets agree on all 69 events; far plane 1,000,000; coverage valid on 56 events, and the 13 lighting events are -1 with valid=false.
- Census: PASS. Peak 3 drawn; 0 exhausted, lost, no-pass or polluted.
- Custom depth: PASS. 14 components restored; the game's own actors untouched.
- Player view: PASS on 2,352 of 2,352 frames.
- Pre-UI proof: PASS. The 8 control frames show the UI marker on 8/8 cells; every capture shows 1/8.
- Frames: 2,352 written, 0 dropped. No crash, no upload.

FV6, SCAN, PUSH, CARD
- FV6:
  - Backup ref refs/backup/europa-pre-fv6-09440 points at 08fe652 and is local only.
  - 26 commits were rewritten (47a6972 to 08fe652). Each rewritten tree differs only in docs/europa/09434-live-capture-receipts.json, with the same author, date and message. 39 earlier commits are unchanged.
  - Source, Shaders and the descriptor are equal between the built commit 08fe652 and d1cdc7b.
  - The pre-rewrite file held all 189 drive-root names; the redacted file holds 4 ordinary path words (Game, Cpp, Builds, Config).
- Scan: clean (_reviews\094-40-prepush-scan.md), covering abb240b..d1cdc7b (67 commits).
  - No secrets and no control-server token.
  - No e-mail addresses, GitHub account, machine name or Windows SIDs. One identity on every commit, the same as on origin/master.
  - No codenames: the only scrub hits are the permanent Build.cs exclusion lines.
  - No drive-root listing (at most 7 common words in any file), no binaries, no blob over 1 MB.
  - Carried from 094-29: the non-secret profile path at tools/europa/write_ledger.py:22.
- Push: "* [new branch] feat/europa-port". ls-remote shows d1cdc7b956db..., equal to the local tip. No tags, no backup ref, no force.
- Card (_reviews\094-40-office-card-europa-capture.md):
  - Pins d1cdc7b (new clone, or fetch + checkout --detach on an existing one).
  - Steps: build, the D-lite eyeball smoke, then one seeded capture: IAI.Capture.C5 with seed 4242 and the four anomalies, from the player's own view (-IAICaptureFallback), at the shipped 6% selection floor.
  - Includes the one-line FX5 lighting limitation.
  - Needs about 1.5 GB of disk (5 GB free asked) and 12 GB of available memory.
  - Typed reads R1-R23, each with its home value and whether it is pass/fail or a reading:
    - both self-check lines, field by field (class names typed only as standard or custom);
    - the 6.00% selection-floor line;
    - fires per anomaly;
    - frame and mask counts;
    - written=2352 and dropped=0;
    - the native receipt result (PASS);
    - whether annotation.json is present;
    - LogExit count;
    - crash.

SAFETY
- No crash: exit 0, the crash client reports ExitCode 0, no new Saved\Crashes entry.
- No upload sign in either session log.
- Paris Content (1,456 files), plugin Content, Config, the descriptor and the .uproject are hash-, size- and mtime-equal by my own before/after hashes.
- Binaries changed only inside the two build windows.
- Nothing deleted: 0 files or folders gone, 0 reparse points.
- m51 equals P9 at every check. master b5f15a3, release/2026-10-05 d7eee97 and the 30 tags are unchanged.
- My feature-state gate's final run fails only on "origin branch absent", which the push makes true; that is expected.

NEEDS-DECISION: none.
- Note for the office: the shipped 6% floor on a real level has not run at home (at 6%, 094-39's EA_Core run fired lighting only). The card treats any zero fire count as a finding, not a stop.
```
api-equivalent cost (not billed on Max): $14.64 | turns: 100 | running total across all briefs: $5,485.34
