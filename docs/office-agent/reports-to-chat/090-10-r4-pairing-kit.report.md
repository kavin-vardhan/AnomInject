# REPORT for 090-10-r4-pairing-kit.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 090-10-r4-pairing-kit.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-01 09:44:36 | finished: 2026-10-01 12:26:49 | duration: 162 min | exit code: 0
context at last turn: 767442 tokens (77% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\090-10-r4-pairing-kit.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Plain summary for the owner:** A label's box and its on-screen check now describe the exact picture they belong to, even if the object moves or disappears before the frame is saved. Frames from the old synchronous capture path are now marked and dropped (they are never labelled), and the readme says that path is not for delivery. The office checker now stops on a missing label line or a damaged image instead of quietly passing. Both branches compiled cleanly and are pushed; no game was launched.

**A: R4 async (what is frozen, and where)**
- **Where it is frozen:** at the tick-end sample (`SampleDeferredActiveState` → `FreezeSampleGeometry`, and also `FinishRun`'s final sample).
- **What is frozen:** each fire's renderable bounds box plus a source flag (`whole_frame` / `box` / `none`). The rule is a new UE-free header, `AnomalyFrozenGeometry.h`. The same is done for the transition-entry boxes and the stuck-mip candidate boxes.
- **Also frozen there, beyond the ruling:** the fire position, and each event's first-frame identity: node path, asset, component class, node bounds, selection provenance (including `coverage_pct`) and camera path.
- **At readback completion,** three things project the frozen box with the frozen view through the new `AnomalyViewport::ProjectBoxToScreenRect`. The old live projection now calls this same function, so the math is identical.
  - `FillAnnotationInputs`: the on-screen test behind `labelled` and annotation membership.
  - `BuildFrameLabelRecord`: `bbox_px` / `bbox_norm` / `bbox_valid`.
  - `AccumulateFrameEvents`: `coverage_ratio`.
- **One live read remains at completion:** the m26 mask record's actor pointer in `AccumulateFrameEvents` (`FindOrAddRecord`). It is safe because it only stores a handle for later mask arming and computes nothing for this frame's label; the record usually already exists, in which case the pointer is ignored.
- **Tests both ways:**
  - New `r4_capture_pairing_selftest` (46 checks): the target moves off screen, goes behind the camera, is destroyed, loses its bounds, grows, or enters the view between sample and completion. The record keeps the sample's box and membership, and the old rule is shown to disagree in every case where something moved.
  - New `tools/r4_pairing_gate.py` checks the real completion-path source for any live read. Its selftest builds 8 mutants of the real source, including 4 that read live bounds; all 8 fail and the real source passes.
  - Existing suites unchanged: 305, 154, 41 and 27 checks; the 15 earlier mutants plus 5 new header mutants all fail.

**B: R4 sync**
- **The flag:** new reason bit 128, `capture_unpaired`. It is set in the sync block of `CaptureCurrentFrame` (`bCaptureUnpaired` + `MarkCaptureUnpaired`).
- **What a sync frame looks like:**
  - Every entry is transition-only with the reason.
  - The row carries `capture_unpaired: true` and `transition_reason: ["capture_unpaired"]`.
  - There is no labelled entry and no mask.
  - The frame is never added to `annotation.json`.
- **Counted in** `run_summary.capture_unpaired_frames` and `label_transition_capture_unpaired_entries`, with one CAPTURE-UNPAIRED warning per run.
- **Readme line:** "The synchronous capture path is unsupported for delivery. The shipped defaults (asynchronous capture and scene-colour capture both on, no ini key needed) never reach it."
- **Tests both ways:** the selftest covers the reason bit, transition-only entries and never-a-member under all 4 policies. Mutants that leave sync frames labelled, keep them as members, or read the reason as anti-aliasing all fail, as do 3 gate mutants.
- **What knows the reason:**
  - `verify_capture --label-rule` knows it (31 → 51 cases; its mutants fail). A session made only of sync frames reads NOT JUDGED, exit 3.
  - The kit drops these frames, gives them no anti-aliasing excuse, censors a run edge next to one, and refuses an all-sync session.

**C: kit 1.3**
- **Missing labels row:** a row missing inside a labelled run, or filling the gap between two runs of the same event, makes the event unjudged ("labels row missing"). The stuck_low_mip path had the same silent skip and is fixed too.
- **Damaged PNG:** one strict PNG check runs before every full and partial decode on both decoders, so a truncated, bad-checksum or incomplete PNG is invalid on both and they agree. Pillow's crash on a bad checksum is now caught.
- **Tests:** the selftest goes from 54 to 73 checks and passes on both decoders. A mutant per fix fails, and the earlier kit mutants A, A2, B, C, `taa_flag` and `ignore` still fail.
- **Regression:** 8 real sessions, 91 events, 0 differences on Pillow and on stdlib; 0 of 3,624 real PNGs were flagged invalid.

**Builds (both branches)**

| | Fix branch | m53 |
| --- | --- | --- |
| Normal build | 0 warnings | 0 warnings |
| Strict-include | 0/0 | 0/0 |
| Final exe | `F636D0A7` | `85395644` |
| String scan vs 090-09 archive | 28 differences | 26 differences |
| Lever audit | 34/34 | 47/47 |
| C++ suites | 5/5, 20 mutants fail | the fix's 5/5 and its own 6/6; 20 + 23 mutants fail |
| Python suites | 18/18 | 26/26 |
| Archive (6/6 re-hashed) | `m52fix-09010-F636D0A7` | `m53-09010-85395644` |

Every string difference is this round's own strings, plus known one-byte artefacts (`pUAnomalyInjectorSubsystem` and `pDiagnostic` on the fix branch, `eIAI.Bench.StuckMipVirtualProbe` on m53).

**The pin for 090-11:** the m53 head exe is **`85395644`**. The head's code equals it: `git diff 54efd04 ff868b2 -- Source` is empty, and every later m53 commit is Python tools and docs only.

**Commits pushed**
- `fix/m52-label-timing` `189286a..b7da624`:
  - `5291059` R4 source, tests and readme
  - `d6dc703` checker
  - `1ea522b` kit 1.3
  - `0e3df52` docs: journal `docs/sessions/2026-10-01-090-10-r4-pairing-kit.md`, status block, architecture note, gotchas G436–G438
  - `b7da624` journal note
- `feat/m53-uv-normal-corruption` `31078c7..ff868b2`: merges `54efd04`, `bd6eb47`, `e4f2f80` (with the m53 status block) and `ff868b2`.
- `m51`, `master`, tags, `ToCodex\`, `E:\<personal-folder>` and `_reviews\090-06-*` were not touched.

**NEEDS-DECISION:** none. For chat's acceptance:
1. **A stale kit 1.2 does not reliably fail safe on `capture_unpaired` frames.** It fails the realistic case but can pass when the sync frame looks clean or also carries `temporal_aa`. Captures from this build must be checked with kit 1.3. With shipped defaults no frame is ever unpaired.
2. **Kit 1.3 censors rather than fails an edge beside a sync frame**, the same way it treats the Nanite end. So a label one frame late onto that edge reads CENSORED.
3. **Rule string bumped** to `…_v4_sample_bound`, plus a new `label_geometry_source` key. Neither tool depends on the exact string.
4. **More was frozen than the ruling named:** the event identity and fire position, as listed under A.
5. **Sync frames feed nothing in `annotation.json`,** so an event seen only on sync frames does not appear there.
6. **The single-shot `IAI.Capture.Shot` is not flagged.** It is not part of a capture run, and the ruling names only `Async 0` and the SVE-off fallback.
7. **The checker** also reads an older build's all-sync session as NOT JUDGED. None exists in the bank.

The pdf-viewer connector failed to connect, and Gmail, Calendar and several plugin connectors need authorising in their settings. None was needed here.
api-equivalent cost (not billed on Max): $88.53 | turns: 240 | running total across all briefs: $4,208.29
