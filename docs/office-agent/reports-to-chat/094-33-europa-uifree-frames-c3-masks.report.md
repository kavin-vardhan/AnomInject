# REPORT for 094-33-europa-uifree-frames-c3-masks.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-33-europa-uifree-frames-c3-masks.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-01 02:03:28 | finished: 2026-11-01 03:10:48 | duration: 67 min | exit code: 0
context at last turn: 536744 tokens (54% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-33-europa-uifree-frames-c3-masks.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

FO2 passed: 4.25 frames are now UI-free and the C1 and C2 gates pass again. C3 produced no masks: its session crashed at the first mask step. This was the run's first crash under FI4, there was no upload sign and no Paris change, so by your definitions this is NEEDS-DECISION, not RED. Codex called it RED because C3 failed. Codex exited 0 after 3,166 s (02:09 → 03:02 IST).

**2. For the owner**
- Frames on 4.25 are now clean like on 5.1: the green "[IAI] … ticking" text is gone and the picture is otherwise unchanged.
- Labels still line up with the picture frame for frame.
- Masks are not done yet. One texture setting in the new mask code was wrong and the game stopped at the first mask frame. The fix is one line, but this job had no build or session attempts left to try it.

**3. FO2**
- **Hook:** colour is now copied in `PostRenderViewFamily_RenderThread`. 4.25 calls it at `SceneRendering.cpp:3044` (inside `RenderFinish`), and the HUD, debug canvas and stats are drawn afterwards (`GameViewportClient.cpp:1528` → `:1624` / `:1672` / `:1695`). The 5.1 path is untouched; no fallback and no backbuffer-format override were used.
- **Heartbeat:**
  - My check finds 0 heartbeat pixels in all 612 C1 and C2 frames; the controls show 1,358–1,377.
  - The new frames are byte-identical to 094-32's same-numbered frames everywhere outside the old text box: 60/60 PIE, 60/60 Game, 480/480 C2. The 12 travel frames differ by at most 1 level, which is the engine's grain.
  - I opened 3 frames per session: the text is gone and the scene behind it is intact.
- **C1 re-run, PASS:**
  - PIE: 60 frames, indices 0–59, 0 black, format 18, D3D11, 0 dropped; check region mean 0.497 against the Phase A reference.
  - Game: same counts; region mean 0.478 against the same-map engine read.
  - Both exited normally.
- **C2 re-run, PASS:**
  - The reader (re-run by me) and my label-free classifier both find 0 mismatches over 480 frames, and their edges agree.
  - Per anomaly and tick order: 3 applies and 3 reverts, plus 6 blink toggles for blinking.
  - All six of my one-frame shifts (early and late, at apply, revert and blink toggles) and Codex's three kept negative files fail as they should.

**4. C3**

| Anomaly | Measured | Threshold | Verdict |
|---|---|---|---|
| blinking | 0 masks | IoU ≥ 0.90 drawn and hidden | not measured |
| missing_object | 0 masks | IoU ≥ 0.90 | not measured |
| lod_corruption | 0 masks | IoU ≥ 0.90, changed-pixel coverage ≥ 0.95 | not measured |
| lighting_mismatch | 0 masks | coverage ≥ 0.95, receivers ≤ 1 % | not measured |

- **Shaders:** they compiled at boot (the smoke reached 0 pending and exited normally), and the session found the reduce shader. No boot compile failure. The kit's "shader compile failure" flag is its broad log matcher, as Codex says.
- **The failure:** the mask textures put the shader-read flag in `Flags`, but 4.25 checks `TargetableFlags`. I confirmed this against `RendererInterface.h:109-110` and `ShaderParameterStruct.cpp:496-503`.
- **Where I differ from Codex:** Codex says no engine exception is confirmed. The log's last line (`Runtime.log:638`) is `Windows GetLastError`, which 4.25 prints only on its fatal-error/assert path (`WindowsErrorOutputDevice.cpp:30-46`). So the session did crash at frame 93, then hung until the kit stopped it at 181 s.
- **Thresholds:** frozen and not lowered; the reader's values equal FO3's.
- **Negatives, re-run by me:**
  - Codex's synthetic test passes.
  - On my own masks cut from the kept C2 frames: a one-frame shift fails at every edge, early and late; the wrong target gives IoU 0, spill 1, FAIL.
  - These only prove the check can fail; they are not C3 evidence. There was no mask to open.

**5. Safety**
- **Crash:** one, the C3 session, as above. No crash folder was written.
- **Uploads:** none. All 7 upload checks pass, and the CrashClient logs show only the startup DataRouter URL.
- **Paris host:** Content (1,456 files), plugin Content (5), Config (18), the descriptor and the `.uproject` are hash-, size- and mtime-equal between my 02:06 and 03:03 lists. Host Source is unchanged.
- **m51:** HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the same 15 untracked entries, at start, launch and exit. master, release and the 30 tags are unchanged.
- **Binaries:**
  - All 13 changed files fall inside Codex's game-03 and editor-03 build windows.
  - The manifest is 27/27 equal.
  - All builds used 2 parallel actions; the lowest free commit was 4.81 GiB.
  - ⚠ My poll was not running from 02:39 to 02:51, so for the C3-window builds and the C3 smoke I rely on Codex's receipts.
- **feat/europa-port:** five commits ending `8d1b2e4`, clean, no origin branch, no tags, nothing pushed.
- **FO4 PASS:** `docs/gotchas.md` is the old file verbatim plus G489–G490 appended; the restored line is byte-equal to the original.

**6. NEEDS-DECISION**
1. **Authorize one more build pair and one C3 session.** The fix is to move the shader-read flag into `TargetableFlags`. The real-session window closes at 12:00 IST today, and FM1's three starts per target and the C3 slot are spent.
2. **Accept FO2 as done:** pre-UI capture, re-run C1 and C2, and the heartbeat check.
3. **Count the C3 crash** as this run's first FI4 crash.

Before an office capture smoke on Europa's fork: the fixed C3 run, then the real firing path (FO1), then a read of how the fork handles its viewport and its `PostRenderViewFamily` call.

Files are in D:\IntrusiveAnomalies\_reviews\:
- 094-33-code-review.md
- 094-33-codex-report.md (collected byte-identical)
api-equivalent cost (not billed on Max): $13.72 | turns: 108 | running total across all briefs: $5,348.70
