# 094-39 — Chat ruling: the smoke passes every gate on EA_Core except one: missing_object never fired. The cause is the bench driver re-seeding per hold. LOD swaps were still invisible (the coarsest LOD was already drawn). 094-40 fixes both, re-runs the smoke, then redacts, scans and pushes

**Date:** 2026-11-02, about 04:10 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-39:** NEEDS-DECISION. It ran 01:05–02:44 IST.
- **Codex's report:** `_reviews\094-39-codex-report.md`.
- **Code's review:** `_reviews\094-39-code-review.md`.

## Verdict
**Accepted:**
- **FW3's 2-LOD guard** works both ways: single-LOD meshes are rejected, and two-LOD meshes are accepted.
- **FW4's per-frame `pre_ui` proof:**
  - the marker is drawn on frames 1712–1719, and the post-UI controls read 8 of 8 cells;
  - the same frames' captures read 1 of 8 (the black cell on black), and 0 of 2,352 captures contain the marker.
- **The constants** are labelled `build_constant`.
- **The self-check line is measured and sane** (frame 300):
  `stage=300 frames=300 size=1280x720 format=18 RHI=D3D11 PostRenderViewFamily=315 viewport=SceneViewport client=GameViewportClient viewport_size=1280x720 backbuffer_format=18 slate=1 pie=0 pre_ui=1 ui_draws=8 ui_controls=8 ui_positive=8 ui_checked=300 ui_hits=0 ui_paired=8 tick=OnWorldPostActorTick425 tick_source=build_constant tick_type=2 tick_type_source=build_constant tick_calls=1664 fixed_step=1 fixed_dt=0.033333333 fixed_rate=0 fixed_fps=30.000000 smoothing=0 census=on batches=0 lost=0 no_pass=0 polluted=0`
- **The EA_Core smoke passes every other gate:**
  - masks on all 414 affected frames, and none on 1,938 clean frames;
  - `annotation.json` at schema 2.1, with the sets agreeing on 48 events and the far plane at 1,000,000;
  - the census: nothing exhausted, lost, no-pass or polluted;
  - custom depth restored, with the game's own actors untouched;
  - 2,352 of 2,352 frames from the player's view;
  - no crash and no upload.
- **The builds passed:** Editor attempt 3 and Game attempt 1.

**Short:**
1. **missing_object fired 0 times.** The 4.25 bench driver restarts the seeded injector for every hold, and 5.1 re-seeds on every start. So on EA_Core's fixed view the six holds replay the same eight picks, and missing_object is never one of them.
2. **The LOD swaps changed 0 target pixels in all 12 events.** At that distance the engine already draws the coarsest LOD.

## Rulings

### FX1. Accepted: FW3, FW4, the measured self-check line, the builds, and every EA_Core smoke gate except missing_object's coverage

### FX2. The bench driver starts and seeds the injector once per session, not once per hold
*Why:*
- 5.1 re-seeds on every start, so per-hold restarts replay the same picks;
- the office card starts it once.

### FX3. Auto-fired `lod_corruption` forces a LOD different from the one currently drawn
- **The rule:** force the coarsest LOD. If the coarsest is already drawn, force LOD0.
- **How the current LOD is known:** predicted on the game thread from screen size, the way the engine computes it.
- **A new gate:** every LOD event changes target pixels.
- *Why:* 5.1 never auto-fires `lod_corruption` (this is a 4.25 addition), and an auto event that changes nothing is a mislabelled frame.
- Log it with FW3, as a deliberate addition.

### FX4. The office card uses the shipped selection floor, 6 % (5.1 parity)
- The 0.5 % floor is for EA_Core bench runs only.
- *Why:* the office test should exercise what will actually run.

### FX5. Lighting masks exclude the game's own custom-depth actors (G498)
- This is an accepted limitation.
- The card says so in one line.

### FX6. 094-40
- **The work:** FX2 and FX3, then a rebuild with a fresh FM1 budget.
- **The smoke:**
  - EA_Core at the 8 GiB line, two slots. The street map isn't required: it was proven in 094-36.
  - **The gates:** FV5's EA_Core set, at least 3 fires of each anomaly, and every LOD event changing target pixels.
  - **The window for real sessions:** until 07:00 IST on 11-02.
- **If GREEN:**
  1. FV6's redaction (now 25 commits carry the listing; backup ref and tree-diff proof);
  2. Code's pre-push scan;
  3. if the scan is clean, Code pushes `feat/europa-port` (the owner's call, 11-02);
  4. Code writes the office card, saying "already pushed: fetch and check out <commit>".
- **If not GREEN:** Code pushes only `4f46a9c` to `feat/europa-port`, so the D-lite card works on an office day.
- **The push rules:**
  - only `feat/europa-port`, with no tags, no backup refs and no force;
  - confirmed with `ls-remote`;
  - if it isn't a fast-forward, stop and report.

## For the owner
- **Nothing to do.**
- **Almost there:** everything passed except one bug that never got picked, because of how the test was replaying. That's fixed in the next run.
- **Code pushes when ready,** as you asked.
