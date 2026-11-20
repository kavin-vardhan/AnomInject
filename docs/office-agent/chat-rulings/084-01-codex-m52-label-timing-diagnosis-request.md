# Request to Codex (via the GDP Relay): independent diagnosis of stuck_low_mip (m52) label-timing errors (chat Claude, 2026-09-28)

**Role:** independent diagnostician.
- Read-only. Write the report to `review.md`; it will be copied to `_reviews/084-01-codex-m52-label-timing-diagnosis.md`.
- Claude Code is diagnosing the same symptom in parallel, with bench measurements. **You are deliberately not shown its work.** Chat will reconcile the two.
- End with a `VERDICT:` line: ROOT-CAUSE-ESTABLISHED / PARTIAL / NOT-ESTABLISHED.

**Symptom (the owner's own observations on an office host running `master` `b5f15a3`; no office data is available)**
- **Onset: labels and target masks start about 2 frames late.** The blur is visible from frame 102, but `labels.jsonl` rows and target masks start at 104. Labels and masks agree with each other.
- **Offset: inconsistent.** Some events switch off correctly. On others the blur is **still visible for about 5 frames after the label switches off**.
- **`corrupted_texture` on the same host shows no desync.** So the general mask/picture pairing defect (m51, G295) is not the primary explanation. Test that inference rather than assume it.

**Inputs**
- **Repo `master` at `b5f15a3`:**
  - `Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp` and `Public/AnomalyStuckMipStats.h`;
  - `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`: `ResolveAnomalyActiveSource` (`stuck_low_mip` → `AnomalyState`), `HasDeferredOnset`, the deferred-onset and restore timeouts (~`:1763`, `:3663`, `:6289`), how the labelled window opens and closes, and how target-mask arms are issued for the window;
  - `AnomalyLabelWriter.cpp`;
  - the m26 veto gating from 080-06;
  - the m52 design and journals `docs/predictions/2026-09-20-m52-stuck-low-mip.md` and `docs/sessions/2026-09-2*-080-*`; gotchas G260–G270.
- **Engine source** `D:\UESource\UnrealEngine\Engine\Source\Runtime\...`, for the texture streamer and the render-thread mip update: when a stream-out or stream-in becomes **visible in the rendered frame**, relative to when `GetNumResidentMips()` / `GetCachedLODBias` report it on the game thread, and relative to the frame the capture stamps.
- **Banked m52 sessions,** if any exist under `D:\IntrusiveAnomalies\_bench_sessions_bank\` (for example `M52D_*`): per-frame PNGs, `labels.jsonl`, `anomaly_log.txt` and `game.log`. Measure the actual visible onset and offset per event against the label window, from pixels, with a null or pre-onset reference.

**Questions**
1. **Onset.** Which signal opens the labelled window, and at which frame does the rendered picture actually change?
   - Explain the ~2-frame lag mechanically: the game-thread vs render-thread vs streaming-update ordering, and the capture stamp.
   - Is the lag constant, or content- and load-dependent?
2. **Offset.** Which signal closes the window? Why would the blur stay visible for ~5 frames after the label closes on **some** events but not others (restore path, stream-in latency, "already at baseline" skip, restore timeout, destroy/revert paths)?
   - Name the condition that separates the correct events from the late ones.
3. **Masks.** Do masks follow the same window, so both are wrong together? Would a fix to the window move both?
4. **The fix.** Propose a design that makes the labelled window **equal the frames where the anomaly is visible**, from engine-side truth, not from a timing guess. For example: open on the first frame whose rendered mip for the held texture is below baseline; close on the first frame whose rendered mip is back to baseline for every held texture.
   - What source-level signal gives that truth?
   - What is the cheapest bench gate that can falsify it (a can-fail), per frame, both edges, including the 5-frame-tail case?
5. **Blast radius.** Does the same pattern (a window keyed to a game-thread state that the picture lags or leads) affect any other anomaly with deferred onset or a staged restore?

Put depth where the evidence is. Cite file:line, and bank paths with frame numbers.
