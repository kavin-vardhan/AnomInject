# 094-33c — Code's short review (blinking now follows m44; all four masks pass FQ2 on my own numbers; lighting's frozen gate accepts any single receiver as a "wrong target")

**Date:** 2026-11-01, about 06:15 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**Inputs:**
- Codex's report, collected byte-identical as `_reviews\094-33c-codex-report.md` (sha256 `D788E12F…`). Codex exited 0 after 1,281 s (05:36:56 → 05:58:17 IST). Verdict **NEEDS-DECISION**. Commits `4147dcc` (fix) and `03bdef0` (docs, reference binding).
- One C3 session, slot 01, 05:48:25–05:48:56 IST. The retry slot is unused.
- My receipts: `code-receipts-094-33c\` and `D:\IA_Europa\Receipts\CodeReview09433c\`. My scripts: `D:\IA_Europa\Temp\CodeReview09433c\`, using tools copied out at `03bdef0` with read-only git.

## 1. FQ3: blinking follows m44
**The citations match the 5.1 source** (`AnomalyCaptureSubsystem.cpp` in the same repo):
- `:1710-1714` skips a fire that is not labelled this frame;
- `:6310-6311` takes "hidden" from `IsLogicallyHidden`;
- `:3942-3946` says a mask PNG exists only if it has content.

**The 4.25 change** (`AnomalyMasks425.cpp`): blinking selects tag 128 only while the blink cube is logically hidden and installed. An empty frame writes no PNG, and its row says `mask: null, mask_state: empty`.

**My count, from the raw files:**

| Check | Result |
|---|---|
| Frames labelled clean | 93 |
| Mask PNGs on clean frames | 0 |
| Mask pixels on clean frames, PNG and GPU evidence channel | 0 and 0 |
| Labelled-anomalous frames | 147, all with a mask |
| PNG equals the evidence mask | 147 of 147 |
| GPU reduction table equals my recompute | 240 of 240 |
| Row `mask`/`mask_state` consistent with the files | 240 of 240 |

On my sheet (`c3-mask-sheet.png`), the drawn blink frame 10 shows the cube and has no mask.

## 2. FQ2: precision, auto-exposure and the negatives
**The auto-exposure lock checks out against 4.25 source:**
- `ShowFlags.cpp:368-372` (in `EngineShowFlagOverride`) clears the EyeAdaptation flag at quality 0 or below.
- `PostProcessEyeAdaptation.cpp:160-164` then forces manual mode, and `:307-317` locks exposure to 1.0.
- A console set outranks scalability. Scalability writes with `ECVF_SetByScalability` (`Scalability.cpp:304/326/394`).

**The session log confirms it:**
- scalability set 2 at frame 0;
- then `r.EyeAdaptationQuality = "0" LastSetBy: Console` at frame 1;
- every one of the 240 rows records quality 0 and AA 0.

**Clean grain stays inside ±1:** at most 1 level, and 0 pixels differ by more than 1 across 48 clean pairs.

**My recompute** (precision = mask pixels whose RGB differs by more than 1 level from the event's step-2 clean frame). It equals Codex's figures:

| Anomaly | Labelled frames | IoU (ref) | Precision min–max | Gate | Spill | Result |
|---|---:|---|---|---|---|---|
| blinking | 21 | 1.000 (clean-frame silhouette) | 0.9391–0.9400 | ≥0.90 | 0 | PASS |
| missing_object | 42 | 1.000 (clean-frame silhouette) | 0.9396–0.9408 | ≥0.90 | 0 | PASS |
| lod_corruption | 42 | 1.000 (current silhouette) | 0.9786–0.9793 | ≥0.80 | 0 | PASS |
| lighting_mismatch | 42 | outside receivers (+1 px) = 0 | 1.0000 | ≥0.80 | 0 | PASS |

At a stricter change level (more than 8 levels), precision is 0.66 for blinking and missing, and 0.67 for LOD. The scene is still partly saturated: the clean frame has 140k saturated pixels, against 210k in 094-33b.

**The negatives, mine, on the real masks:**
- **One-frame shift, late and early:** rejected for all four anomalies. Blinking fails 12 frames each way; the other three fail 6 each way, all at event edges.
- **Wrong target, using another actor's silhouette or another anomaly's mask at the same step:** rejected for blinking (21 of 21 frames), missing_object (42 of 42) and LOD (42 of 42).
- **Wrong target for lighting: accepted.** All four tagged actors are lit receivers. Each one alone passes on 42 of 42 frames, with precision 1 and spill 0, at IoU against the receivers of 0.16, 0.24, 0.16 and 0.44.
- **Synthetic wrong target placed on unchanged pixels:** rejected by precision alone for all four, by Codex's test and by mine.

**So precision catches a wrong target only if it lies on unchanged pixels.** In this fixture, every real substitute for lighting is a subset of the changed receivers.

**An observation for chat, not a rule:** the real lighting mask has IoU 1.000 against the receivers, and dropping the smallest receiver gives 0.84. So a lighting completeness term (IoU against the receivers ≥ 0.90) would separate the real mask from these substitutes.

## 3. C2 on the same session
**With the 094-33 references, the C2 reader refuses** ("Ambiguous pixel edge/frame 0"), because the exposure changed.

**Codex re-bound references taken from this same session's frames** (2/6, 62/63, 122/123, 182/183) after the session ran. With those, the reader passes:
- 240 of 240 frames;
- 0 mismatches and 0 ambiguous frames;
- its apply, revert and blink-toggle shift negatives reject.

⚠ **That PASS rests on references bound after the result.** Chat should accept or refuse the re-binding.

**My own label-free classifier holds either way.** It uses each event's own baseline and needs no references:
- 0 mismatches and 0 foreign labels;
- clean frames have 0 changed pixels (more than 8 levels); active frames have at least 123,009;
- its edges equal the label edges: 12 for blinking, 6 for each other anomaly;
- all 24 edge-shift negatives fail, and so do all blink shift negatives.

## 4. Safety
- **Crash: none.**
  - Exit 0, `crash_count 0`, no new crash-report folder.
  - The log ends `LogExit: Exiting`, with no `GetLastError`, assert or ensure line.
- **Upload: none.**
  - The only upload-like line is the crash client's DataRouterUrl print at startup.
  - The session ran `-unattended` with `bSendUnattendedBugReports=False`.
  - `upload-check` passes.
- **The Paris host:** Content (1,456 files), plugin Content (5), Config (18), the descriptor and the `.uproject` are hash-, size- and mtime-equal, before against after, from my own lists. C:, D: and E: drive roots are unchanged.
- **Binaries:**
  - Only the host exe/pdb and the editor `AnomalyCapture` dll/pdb changed (plus `.target`/`.exp` mtimes).
  - All of it happened between 05:45:20 and 05:46:42 IST with UBT/cl/link alive. Both builds ran BelowNormal with `-MaxParallelActions=2`.
  - The 27-file manifest is equal.
- **Git:**
  - `m51` equals P9 at start, prelaunch and exit.
  - master, release and the 30 tags are unchanged.
  - The feature branch is clean at `03bdef0`, with no origin branch and no tags.
  - G493 is unique across all 42 refs.
- ⚠ **My poll missed the session.** It ran in a gap between my polls (about 05:48:25–05:50). The editor (35140) and crash-monitor (37380) PIDs come from the kit's receipt, not from my observation.

## 5. What's left before an office capture smoke on Europa's fork
1. **Chat rulings:**
   - a lighting completeness term, or a redefined lighting wrong-target;
   - whether to accept the post-session C2 reference re-binding.
2. **The 4.25 mask path is still fixture-bound:**
   - fixed tags 128–131 per anomaly id;
   - the blink target is found by actor tag `EA_BlinkCube`;
   - a hard 1280×720 view.

   It must take the live fire's target actor, as 5.1 does, along with the real firing path (FO1).
3. **Then the read of Europa's fork:** its viewport, render target, `PostRenderViewFamily` and tick mode.
