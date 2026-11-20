# REPORT for 094-45-europa-textures-paris-then-cameraclip.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-45-europa-textures-paris-then-cameraclip.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-04 14:49:02 | finished: 2026-11-04 16:14:37 | duration: 86 min | exit code: 0
context at last turn: 601013 tokens (60% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-45-europa-textures-paris-then-cameraclip.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION. Stage 1 failed its texture gate on the first Paris leg (D3D11), so nothing was pushed and Stage 2 (camera_clipping) never started.

**For the owner:**
- 🟡 The textures still fail on the Paris test level. Labels and timing are exact, but on some objects part of the swap looks identical to the original surface, or sits in a pitch-black shadow, so the strict pixel test can't pass. Nothing was pushed, so the default set on the office branch is unchanged.
- ⚪ Camera clipping wasn't tested; it waits for the textures.
- The office card stays `_reviews\094-43-office-card-europa.md`.

**How it ran.** I resumed thread `01a14334` at 15:15:19 IST, after the blackout; my wrapper refused any earlier launch. The resume prompt holds the 094-44 request, GL1–GL5 verbatim, the scope line, my context with the handshake, and the 094-43r implementer block. It ran to 16:07:58 (3,159 s, exit 0, one try). Codex stopped on its own failed gates before Stage 1 was ready, so it never wrote `STAGE1-READY.txt` and I wrote no handshake file. My review is `_reviews\094-45-code-review.md`; Codex's report is `_reviews\094-45-codex-report.md`, byte-equal to `report.md`.

**Stage 1**
- **GL3 (`96d8f01`) passes.** One predicate changed: an owned stop with an empty reason is no longer rejected. All 9 tests pass when I run them from a copy, and each of my three mutants fails at least one (old empty-reason rule, PID check removed, running check removed). Replaying 094-44f's receipt through the new code gives `owned-stop`; that receipt is byte-unchanged.
- **GL2's view lock (`93e7435`, `422ac34`) works.**
  - The bench helper holds `EA_Camera` at (−550, 0, 480), pitch −12, FOV 50, and every probe row I read used that one camera.
  - It adds a bench-only probe to the plugin's capture code (Shipping excluded, `-IAIBench` and EA_Core only, auto-injection off). The office plugin copy will carry it but it stays inert there.
  - My recompute of target size on screen: EA_HideCube 8.5059 %, EA_BlinkCube 8.5059 %, EA_LOD 12.8707 %, all centred in frame. This matches Codex exactly.
  - **EA_Floor (23.492 %) is not in the helper's list,** so it was never previewed, yet the capture fired 7 checker and 4 pink events on it.
- **GK2 re-qualified in `-game` at −5 EV.** The three cubes pass every check: means 143.86 / 144.42 / 99.80, 0 saturated pixels, pink hue passes, checker gaps 121.6 / 121.8 / 41.6. No `-game` sweep was needed.
- **D3D11 1280×720.** H4 passes:
  - 2,352 frames, 0 drops; 69 events (blink 15, lod_corruption 19, missing_texture 22, corrupted_texture 13);
  - masks equal the affected set (780/780); census and custom depth clean; `unavailable` 0;
  - all 19 LOD events change the target; upload check PASS.
- **Texture gates on that leg:**
  - **G1 and G2 pass.** Every texture fire changes its target, nothing changes outside the masks, all 140 edge frames are clean, and residual is 0 after all 35 reverts.
  - **G5 passes:** `r.CustomDepth` reads 1 throughout, 0 custom-depth components, polluted 0.
  - **G3 fails.** Whole-frame IoU by target:

    | | EA_LOD | EA_HideCube | EA_BlinkCube | EA_Floor |
    |---|---|---|---|---|
    | checker (missing_texture) | 1.0000 | 0.4926 | 0.4935 | 0.4065 |
    | pink (corrupted_texture) | 1.0000 | 0.9862 | 0.9862 | 0.8131 |

- **Why G3 fails** (measured; no mechanism claimed):
  - On the cubes, 39,771 of 78,390 pixels change by 2 levels or less: one checker grey renders at the original surface's value.
  - On the floor, 40,450 pixels are pure black (0,0,0) in both frames, outside every other object. These are the cubes' shadows, not a mask over-claim.
  - The `-game` previews already showed a checker change of only 0.49 before any fire.
- **Not run:** D3D12 1280×720 and D3D12 1279×719.
- **C: per session:** 20.46 → 20.47 GiB (setup) and 20.45 → 20.45 GiB (scored). No drop near 1 GiB.

**Stage 2:** not started. camera_clipping is still compiled out on 4.25 and not in the pool.

**Pushes and card:** none. `ls-remote` still reads `8df75de`. Local `feat/europa-port` is `2d93e67` (six local commits) on top of the texture port `8e59a56`, which changes the default set, so it stays local. No card change.

**Safety:** no findings.
- No crash (both sessions exit 0, crash count 0, no dumps) and no upload sign.
- The Paris host's own Content (1,456 files), Config (18), descriptor and `.uproject` are hash-equal before and after. Plugin Content is unchanged.
- Binaries changed only while builds ran. The 4.25 and 5.1 material `.uasset`s and the fixture files are hash-equal.
- Nothing deleted, no reparse points, P9 hashes unchanged.
- m51 matched P9 at start, before launch, at exit and at the final check. The spike branches are still local only.

**Decisions for chat**
1. **G3 on EA_Core.** As the bench scene stands, whole-frame IoU can't reach 0.98 for the checker except on EA_LOD, nor for pink on the floor. My recommendation: keep G3 as it is and use GK2's own fallback as runtime-only bench settings.
   - a baseline material on the EA_Core targets that looks unlike both checker greys;
   - shadowless fill lights, so no target area is black;
   - one new pre-fire check: each swap must change at least 98 % of its target's silhouette in the `-game` preview. That would have stopped this leg before any fire (it read 0.49).
2. **EA_Floor.** Either add it to the helper's previews and every pre-fire check, or keep the bench from selecting it.
3. **Next job.** Rerun Stage 1 (D3D11, then both D3D12 legs) with the same handshake, then Stage 2 as written.
api-equivalent cost (not billed on Max): $18.47 | turns: 135 | running total across all briefs: $5,741.37
