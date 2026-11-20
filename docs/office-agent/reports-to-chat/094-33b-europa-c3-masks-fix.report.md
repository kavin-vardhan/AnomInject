# REPORT for 094-33b-europa-c3-masks-fix.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-33b-europa-c3-masks-fix.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-01 03:32:09 | finished: 2026-11-01 04:22:12 | duration: 50 min | exit code: 0
context at last turn: 530926 tokens (53% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-33b-europa-c3-masks-fix.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner (plain):**
- Yes, the masks now line up on 4.25: each one sits exactly on the right object, and the labels still match the picture frame for frame.
- Two of the four anomalies (lighting and LOD) still fail a scoring rule that expects all the changed pixels to fall inside the mask. In this test scene the glow around the bright objects and the old object's outline also change, so no mask can cover them.
- Chat needs to decide how lighting and LOD should be scored. Nothing for the owner to do.

**C3** (Codex exit 0 after 30 min; feature `d43635d`; both session slots used). I recomputed every number from the raw files, and each equals Codex's:

| Anomaly | Frames | Metric against threshold | Result |
|---|---:|---|---|
| blinking drawn / hidden | 21 / 21 | IoU 1.000 / 1.000 (≥0.90); spill 0 (≤0.01) | PASS |
| missing_object | 42 | IoU 1.000; spill 0 | PASS |
| lod_corruption | 42 | IoU 1.000, spill 0, but coverage 0.0493–0.0494 (≥0.95) | **FAIL** |
| lighting_mismatch | 42 | 0 outside receivers, spill 0, but coverage 0.5645–0.5647 (≥0.95) | **FAIL** |

- **Why coverage fails:**
  - In the lighting frame, all 344,769 uncovered changed pixels are untagged backdrop glow.
  - In the LOD frame, 43,750 uncovered pixels are the removed cube's footprint, 31,721 are floor shadow, and the rest is glow.
  - Stricter change levels still give only 0.80 (lighting) and 0.24 (LOD), so this is not a threshold artifact.
- **IoU 1.000 only shows the shader agrees with a CPU recompute of the same input.** The reference comes from the same stencil and depth readback the shader uses.
- **The negatives, run by me on the real masks:**
  - The one-frame shift fails 6 frames per anomaly, all at event edges.
  - The wrong target fails 42/42 for blinking, missing_object and LOD.
  - For lighting, the wrong target passes the geometry checks (0/42), because lighting has no IoU term. Only coverage could catch it, and coverage already fails on the real masks.
- **C2 holds on the same session:**
  - The C2 reader passes 240/240 frames with 0 mismatches on both slots, and its three label-shift negatives fail.
  - My own label-free check also finds 0 mismatches.
  - The two slots are byte-identical.
- **The descriptor fix is correct:**
  - The citations match 4.25 source (`RendererInterface.h:109-110`, `ShaderParameterStruct.cpp:496`, `D3D11Commands.cpp:2320`).
  - My engine-check test refuses the old descriptor and accepts the new one.
  - No other descriptor on the 4.25 mask path has the same mistake.
  - New likely cause of 094-33's crash: the old flag also trips a fatal check at `RHIUtilities.h:603`. This is not proven, because 094-33's log has no assert text.

**Safety:**
- **Crash: none.** One handled ensure occurred in slot 01 (the missing copy-source flag). The process continued, and slot 02 has none after the fix.
- **Upload: none.** The ensure never reached the crash client, and no new crash folder exists.
- **The Paris host:** Content, Config, the descriptor and the `.uproject` are hash-equal, before against after, from my own lists.
- **Binaries:** they changed only during the builds, and the manifest is 32/32 equal.
- **Git:** `m51` equals P9 at start, at launch and at exit. Nothing was pushed or tagged.

**NEEDS-DECISION:**
1. **How to score lighting and LOD.** FO3's coverage rule cannot pass on this glowing, saturated scene. One option is a run with bloom and auto-exposure off, like the AA-off reference used on 5.1. Lowering a threshold was not done.
2. **Masks on frames labelled clean.** C3 writes the blinking mask on the drawn frames, which are labelled as no anomaly. FO3 asks for exactly that, but 5.1's `m44` rule forbids it, so it needs a ruling before any office capture.

My review is in `_reviews\094-33b-code-review.md`, and Codex's report is collected as `_reviews\094-33b-codex-report.md`.

Several MCP connectors are not authorized in this session; this job did not need them.
api-equivalent cost (not billed on Max): $12.49 | turns: 90 | running total across all briefs: $5,361.19
