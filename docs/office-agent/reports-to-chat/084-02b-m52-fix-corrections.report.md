# REPORT for 084-02b-m52-fix-corrections.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-02b-m52-fix-corrections.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 16:05:55 | finished: 2026-09-28 17:14:07 | duration: 68 min | exit code: 0
context at last turn: 781900 tokens (78% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-02b-m52-fix-corrections.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Plain summary for the owner**
- I corrected everything the ruling asked for (F1–F7, Shipping, resource replacement, purity scope) and rebuilt it (exe `FD621B27`); tests pass. The label gate now also fails when labels run past the last blurry frame. Nothing was launched, staged or focused.
- Codex reviewed the correction and still says **CHANGES-REQUIRED**, and that the build is **not fit for tonight's bench** under "correct first". It says the zero-tail residual is not its reason. Chat must decide whether tonight's bench runs on this build or waits for another correction pass.

**Each correction and where it lives**
- **F1:** a confirming baseline counts only if no stream work was pending and no texture was still restoring when the frame was armed. The flag is set in `FinalizeRenderTruthArm`; `FTrail::Process` applies it.
- **F2:** trail updates go through an ordered per-event cursor (`FTrail::NoteArmed`/`Receive`).
  - Out-of-order frames wait (`ORDER HOLD` in `ProcessCompletedFrames`).
  - A missing request counts as unknown and breaks the run (`ResolveTrailGaps`).
  - Closure is final only at a fence covering the requests in flight; a late held or unknown receipt reopens the event (`HandleTrailReopens`).
- **F3:** a fire with no watch is unknown, so it is labelled. The watch is re-frozen before the frame is rendered (`ExtendRenderWatch` from `FinalizeRenderTruthArm`), and each event's texture set is cached (`ResolveEventTextures`).
- **F5:** a live window cut by stop or frame cap gets a trail and is counted. Attached trails carry over to the next run with their reservation and refusal (`CarryAttachedTrails`/`AdoptCarriedTrails`). Holds reverted outside a render-truth run are adopted there too.
- **F6:** m55 never gets a provisional negative (`ResolveObserve`, `FlushObserveQueue`), and m55 closure waits for the flush (`FinishRun`).
- **F7:** the deferred mask's age carries across service (`ServiceTargetMask`).
- **Shipping:** the legacy-purity flag, its accessors and its branch are under `#if !UE_BUILD_SHIPPING` (`Anomaly_StuckLowMip.cpp`, `AnomalyStuckMipStats.h`). Checked in source only; Shipping was not built.
- **Resource replacement:** a render resource that changed identity since Apply reads unknown and is logged (`ClassifyTextureSample`, `ComputeRenderMembership`).
- **Purity scope:**
  - At Apply it counts all loaded levels, active or not, and components registered or not (`BuildWorldTextureUsers`, `InPurityScope`).
  - During the hold a monitor reverts immediately on a new user and flags `stuck_mip.contaminated = 1`. UE 5.1 has no component-registered hook and the object-creation hook is unsafe mid-game (G346), so the monitor diffs new components every tick, plus level-added and material-change events.
  - `TEXUSERS` uses the same scope through its own independent traversal.

**Unit tests:** `m52_window_selftest` has **93 checks, 0 failures** (MSVC `/W4 /WX`). Each correction fails on a verbatim copy of the 084-02 logic and passes on the corrected logic. That includes:
- F2's cross-batch order, gap, fence and reopen;
- F1's pending-stream hold;
- F3's unknown fire;
- F5's survival across stop and start;
- F6, F7 and resource replacement;
- purity: an inactive-level user, an unregistered user, and a user appearing mid-hold (revert plus contaminated flag).

**Harness**
- **Two-sided gate (i):** the label window must start at the first pixel-visible frame and end at the last. Isolated visible frames outside that window are reported, not required (G345). The first dry run caught that change being needed on real data.
- **Four can-fail cases on real banked pixels**, each built as an on-disk session copy (rewritten labels, written mask PNGs) and judged against a frozen oracle. On baseline event 577 (pixel window 507–521):
  - the repaired window passes all gates;
  - onset +2, offset −5, early-onset over-label and late-offset over-label all **fail**;
  - the late-offset case passes the old window gate alone, which is Codex's counterexample reproduced on real data.
- **Residual measurement:** per event, signed residuals before and after the render record, for AA as delivered and AA off (`B9_FIX` is now required), plus the closure rule that would give 0 on both edges.
- **Purity gate (iii):** now requires the new scope (else FAIL-SCOPE) and matches each fire's texture enumeration to its held textures (else FAIL-JOIN).
- **Checks run:** `084-03-gateselftest.py` passed 15/15. The dry run exited 0; the baseline reproduces the diagnosis (label 1 frame late, 7 unlabelled blurry frames after).

**Build**
- Game and Editor both exit 0, with 0 plugin warnings. The first Game attempt failed at exit 6 (an `#if` inside a log macro), fixed in source.
- Exe `FD621B27`, archived at `_binary_baselines\m52fix-FD621B27\` and pinned by name in the harness. The A44 string scan finds every new token.
- Nothing was staged; the staged exe still reads `2FCDF059`.

**Codex mini-delta (1,105 s): CHANGES-REQUIRED**, saved to `_reviews\084-02b-codex-m52-minidelta.md` and not acted on.
- **Resolved:** F3, F7, Shipping (source), resource replacement, the Apply-time purity scope, and the two-sided comparator.
- **F1 partial:** stream-out work the engine has queued but not yet started is not fenced.
- **F2 partial:** a reopen loses the mask and m55 continuity, and leaves the reservation and refusal stuck on.
- **F4 partial:** the mid-hold monitor misses a material set on an unregistered component that then registers, and a texture-parameter change on a dynamic material. Its per-tick cost is unmeasured.
- **F5 partial:** inherited frames get no mask at the start of the next run, and a non-render-truth run in between drops the carried ownership.
- **F6 partial:** the 64-tick fallback can disagree with a late label on a live frame.
- **Gate:** a missing frame or engine-frame jump next to the edge is not censored; a later visible run is called "isolated"; missing masks can pass; and contamination does not affect the purity verdict.

**084-03 invocation:** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-03-window.py live`. It runs 12 required legs in about **50–65 minutes (up to about 95 with retries)** and banks about 20 GB on D:. The fix legs may fail gate (i) by exactly the residual, as the ruling expects.

**Commits pushed** (`fix/m52-label-timing`): `d798900` (source and tests) and `90dfa6f` (journal, status block, G345–G346). No tag or merge; `master` is still `b5f15a3` and the main checkout is on `m51` `53bf725`.

**NEEDS-DECISION**
1. Given Codex's verdict, does 084-03 run tonight on `FD621B27` (ordinary-path measurement plus the residual), or after another correction pass on its F1, F2, F4, F5, F6 and gate findings?
2. **Mid-hold user detection:** UE 5.1 has no hook for the two routes Codex found (material set before registration, texture-parameter changes on a dynamic material). The alternatives are a broader per-tick check, an engine change, or ruling them out of scope.
3. **Harness gaps:** whether the gap censoring, repeated-visible-run, missing-mask and contamination-in-purity checks must be fixed before the bench.

Several MCP connectors (GitHub, Slack and others) need authorising via `claude mcp` or claude.ai connector settings, and the PDF viewer failed to connect. Nothing here depended on them.
api-equivalent cost (not billed on Max): $30.35 | turns: 197 | running total across all briefs: $3,078.38
