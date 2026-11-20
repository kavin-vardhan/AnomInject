# REPORT for 094-45r-europa-textures-paris-then-cameraclip.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-45r-europa-textures-paris-then-cameraclip.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-04 16:18:07 | finished: 2026-11-04 19:22:37 | duration: 185 min | exit code: 0
context at last turn: 686589 tokens (69% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-45r-europa-textures-paris-then-cameraclip.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION. Stage 1 (the textures) passed by my own recompute and is pushed. Stage 2 (camera_clipping) failed its D4 gate on a 1–3 pixel technicality, so it is not pushed.

**For the owner:**
- 🟢 The checker and pink textures now pass the strict test on Paris on all three runs (D3D11, D3D12, and D3D12 at 1279×719). They are pushed (`c5d0919`) and are in the default set: blinking, lod_corruption, missing_texture, corrupted_texture.
- 🟡 Camera clipping works: it labelled exactly the three near-wall stretches, every one of those frames is visibly clipped, and the test with the near plane at its normal value labelled nothing. But on 914 frames that should be identical to the comparison run, 1–3 pixels differ by 3 brightness levels, so the strict rule fails. Chat must decide. It is not pushed.
- Office card to use: `_reviews\094-45r-office-card-europa.md` (replaces card 094-43).

**Stage 1: textures on Paris (EA_Core), pushed**
- **Bench changes (test machine only, never saved):**
  - all four test objects, including the floor, get a mid-blue base material;
  - the point light is turned off and six fill lights that cast no shadows are added;
  - the host project's own files are hash-equal before and after.
- **Brightness recalibrated to −1.875 EV:**
  - all 15 brighter settings fail; the deciding check is the floor's pink hue at −1.75;
  - the old −5 EV setting is now too dark under the fills.
- **Pre-fire check:** both swaps change 100% of every object's silhouette, so GM2's fallback was never needed.
- **Camera:** EA_Camera is held on every frame of every run.

| Run | Frames / drops | Events (blink / LOD / checker / pink) | Texture IoU min | Changed px outside masks | Edge frames changed | Revert residue | `unavailable` |
|---|---|---|---|---|---|---|---|
| D3D11 1280×720 | 2352 / 0 | 15 / 19 / 22 / 13 | 1.000 | 0 | 0 of 140 | 0 of 35 | 0 |
| D3D12 1280×720 | 2352 / 0 | 15 / 19 / 22 / 13 | 1.000 | 0 | 0 of 140 | 0 of 35 | 0 |
| D3D12 1279×719 | 2352 / 0 | 15 / 19 / 22 / 13 | 1.000 | 0 | 0 of 140 | 0 of 35 | 0 |

- **Every earlier H4 gate passes on all three runs:** 780 masks equal 780 affected frames, and all 19 LOD events change their object (at least 67,886 px).
- **Clean on all three runs:** census errors, custom-depth components, "Default Material" warnings and fatal log lines are all zero, and `r.CustomDepth` reads 1, 1, 1.
- **C: per session:** changes between −0.026 and +0.034 GiB.
- **Pre-push scan** (`_reviews\094-45r-stage1-prepush-scan.md`): clean, apart from the owner profile path already public on origin.
- **Push:** a fast-forward of `8df75de..c5d0919` (26 commits) at 18:22. `ls-remote` returns `c5d0919`; no tags, backup refs, spike branches or force.

**Stage 2: camera_clipping, local `c706b7c`, not pushed**
- **Port vs the release line (`d7eee97`):**
  - the anomaly class differs only by the 4.25 camera-cache compatibility call;
  - the config keys and the slab self-test are byte-equal; the self-test passes 154 checks;
  - 55 of the 56 release keys are present.
- **D4 on D3D11 (Codex's numbers and mine agree):**
  - **Spans:** three, frames 624–911, 1200–1487 and 1776–2063; each labelled frame changes at least 902,398 px against the control.
  - **Can-fail:** the run at the normal near plane labels 0 frames.
  - **Stop:** the near plane is restored to 10 in both runs.
  - **Failure:** 914 of 1,488 unlabelled frames have 1–3 px differing by exactly 3 levels, at 8 fixed screen positions. Every unlabelled frame also differs by ±1–2 levels on about 40% of its pixels. The cause is not established.
- **Not run:** the D3D12 camera runs and a Paris regression with the route requested.
- **D5 holds:** camera_clipping is never fired by the auto pool, and the defaults are unchanged.

**Safety:** none.
- No crashes or upload signs in any of the 7 sessions.
- The Paris host's own Content, Config, descriptor and `.uproject` are hash-equal, and the 4.25 and 5.1 materials are unchanged.
- Binaries changed only during builds; the fixture, P9 and `m51` all check equal.
- **One deletion, by receipt:** a temporary helper script I had just created in my memory folder (receipt: `code-receipts-094-45r\delete-receipt.txt`).

**Decisions chat must make:**
1. **D4's "every unlabelled frame matches":** adopt a defined tolerance, or first run a control pair at the same near plane to measure how much two sessions differ on their own.
2. **The Paris regression:** should the next job run the default routes plus camera_clipping on both RHIs? Stage 2 changes the capture path every run uses: `run_summary.json` is now written on every run, and every fire gains a `whole_frame` key. Codex's runs used only the camera_clipping route, so even a passing run would not have tested this.
3. **Key differences from 5.1:** the run-summary counter `label_transition_camera_clipping_unconfirmed_entries` is absent on 4.25, and three `session_global_*` keys are added. Are these acceptable as compatibility differences?

Files are in D:\IntrusiveAnomalies\_reviews:
- 094-45r-code-review.md
- 094-45r-codex-report.md
- 094-45r-stage1-prepush-scan.md
- 094-45r-office-card-europa.md
api-equivalent cost (not billed on Max): $26.61 | turns: 189 | running total across all briefs: $5,767.98
