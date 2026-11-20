# REPORT for 094-31b-europa-c1-finish-light-build.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-31b-europa-c1-finish-light-build.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-31 20:59:21 | finished: 2026-11-01 00:37:08 | duration: 218 min | exit code: 0
context at last turn: 506099 tokens (51% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-31b-europa-c1-finish-light-build.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:** not yet. The plugin now builds on 4.25, and in the standalone game it saved all 60 requested frames, correctly numbered, at 1280×720 with none dropped. But every one of those frames is solid black, so it isn't capturing the picture yet. The PIE (play-in-editor) session never produced a usable frame, and both sessions have used up their retry budgets.

**The build**
- **First build:** 2 compile errors, both in AnomalyCapture at `AnomalyFrameCapturer.cpp:406`.
  - C2065: `GPixelFormats` is not declared.
  - C2789 on `BPP`, a knock-on from the first.
  - AnomalyInjector and EuropaPhaseAProbe: 0 errors. My own count of `UBT.log` agrees.
- **Fix:** an include of `RenderUtils.h` that applies only on 4.25 (`024e5fc`).
- **Result:** Editor attempt 2 and Game attempt 1 both finished with 0 errors and 0 warnings.
- **FM1:** Editor used 2 of its 3 attempts, Game used 1 of 3.
  - Start readings were 8.26, 8.27 and 8.17 GiB free commit; the lowest reading during a build was 5.85 GiB, so the 3 GiB stop never fired.
  - I saw every UnrealBuildTool running at below-normal priority with `-MaxParallelActions=2`.
- The runtime kit's manifest is re-bound: all 27 files match what's on disk.
- **The diff:**
  - The 5.1 paths are intact going by the diff; nothing was compiled on 5.1 this job.
  - SHADERS and CONTROL_SERVER are still 0 on 4.25.
  - The readback check that drops padded, undersized and 0×0 maps (FL3) is in place, and its negative test passes in both targets. That test is a set of compile-time checks on the rule; no bad map was fed in at runtime.
  - The new gotcha number G488 is present.

**The sessions**
- **Smoke 01 and 02:** all six bindings pass, normal exit.
- **PIE 01 (EA_Core):** 0 of 60 frames. Stock 4.25 gave a 10-bit backbuffer (format 18), which the 4.25 capture code refuses; 1 dropped, normal exit.
- **PIE 02:** stopped by the kit's own child-process guard before any capture (exit 125). 0 frames.
- **Game 01:** the same guard stop. 0 frames.
- **Game 02 (EA_Travel), launched with an 8-bit backbuffer override:**
  - 60 frames, indices 0–59, none missing or duplicated, 1280×720, format 2, D3D11, 0 dropped, 9.1 s, normal exit.
  - **The pixel check failed:** all 60 files are byte-identical and every pixel is RGB 0. Frames 0, 30 and 59 each show a solid black image with nothing in it.
  - The cause isn't established. The tonemapper adds slightly different grain to every frame (094-27d), so 60 identical files suggest the copy or readback is returning zeros rather than the scene really being dark.

**Safety:** none. No crashes, no new crash folders, and no upload signs. The Paris host's Content, Config, descriptor and `.uproject` match my before and after hashes exactly. The binaries changed only while UnrealBuildTool was running (21:20–21:26). `m51` is unchanged by git (HEAD `53bf725`, index `526aba7f`, the 15 untracked entries). `feat/europa-port` is at `32c5bf5`, clean, with nothing pushed and no tags.

At the start, 094-31's Codex and its build-wrapper processes were still running. I stopped 14 of them, each matched by PID, name and start time; no build had started.

**NEEDS-DECISION**
1. **Fresh session slots.** Both the PIE and Game retry budgets are spent. A new diagnostic session would need to record pixel checksums at the source, the copy and the staging texture to find where the black comes from.
2. **10-bit backbuffer.** Every C1 launch now forces an 8-bit backbuffer, through a setting that applies only to that process; no Config file changed. Without it, stock 4.25's 10-bit backbuffer gives 0 frames. The 5.1 path already converts that format (`AnomalyLabelWriter.cpp:444`), and Europa's fork's backbuffer format is unknown. The choice is between adding 10-bit support on 4.25 and keeping the forced 8-bit setting.

The review is in `_reviews\094-31b-code-review.md`, and Codex's report is collected at `_reviews\094-31b-codex-report.md`.
api-equivalent cost (not billed on Max): $14.01 | turns: 109 | running total across all briefs: $5,321.08
