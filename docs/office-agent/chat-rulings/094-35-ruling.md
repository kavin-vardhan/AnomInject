# 094-35 — Chat ruling: C5's new code works (census at scale, exact restore, player view, 5.1 schema), but nothing could be scored. My 5 % pre-pass rule was ill-posed for a real outdoor scene, and the run found a real 4.25 issue: our census makes part of the picture flicker. 094-36 fixes both and re-runs C5

**Date:** 2026-11-01, about 10:50 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-35:** NEEDS-DECISION. It ran 09:12–10:21 IST, with one street-map session from 09:37 to 09:45.
- **Codex's report:** `_reviews\094-35-codex-report.md`.
- **Code's review:** `_reviews\094-35-code-review.md`. Code recomputed everything from the raw files and split the causes label-free.

## Verdict
**What C5's code achieved (accepted):**
- **The census:**
  - 5.1's 0.5 % floor and 25 % ceiling (both inclusive), its batches, and the `SetCensusProvider` seam;
  - actors that enter the view later are eligible;
  - 168, 125 and 116 distinct drawn actors at three poses;
  - the ledger was never exhausted;
  - the ceiling correctly excluded a wall that covered 93 % of the frame.
- **Custom depth:**
  - 1,980 components are equal before and after;
  - all 3,996 mid-session samples obey the ownership rule;
  - the reserved value 203 was never handed out.
- **The view:** 3,996 of 3,996 frames equal the player's own view (`DefaultPawn_0`), with zero deltas.
- **The annotations:**
  - 5.1's 56 keys, in 5.1's order;
  - "affected" means injected with target mask pixels above 0, which is 5.1's meaning.
- **Off by default:** the bench camera and the street driver both need `-IAIBench`.
- **Safety:** none. The Paris host (street map included) and `m51` are unchanged.

**What failed:**
1. **My pre-pass rule.** All six poses were dropped, at 26–41 % unstable pixels, for two scene reasons:
   - **the grain:** the tonemapper grain has a 7-frame period and spans 2 levels, so the "more than 1 level" tolerance counted the grain itself;
   - **real motion:** wind materials, a fog panner and the sky. Even phase-locked, 8–13 % of pixels move more than 8 levels at five of the poses.

   A 5 % cap cannot hold for an outdoor scene. This is the FO3 lesson again: I froze a metric without checking that it was well-posed for the scene.
2. **A real 4.25 finding.** At the wall pose, a strip of 3.84 % of the frame (rows 686–719) flips every frame, in lockstep with our census.
   - **What happens:** on 4.25 the census restores and re-tags every tagged actor on every captured frame, event targets included. Each tag change rebuilds the actor's render proxy, which flips which of two coplanar surfaces (the street and the wall) wins.
   - **What 5.1 does:** it holds each batch until its readback, and keeps event targets tagged.
   - **Not yet proven:** the census-off control is the proof.
3. **Values in `annotation.json`:**
   - `coverage_ratio` is a frame ratio, where 5.1 writes a screen share;
   - `bbox_source` is always "drawn";
   - the camera's far plane is fixed.
4. **The census re-tags an actor whose custom depth the game already uses:** the value 203 actor gets our 200 for one frame per cycle. 5.1 does the same. It breaks FS2's principle.

## Rulings

### FT1. Accepted: C5's census, ledger restore, player-view capture and schema, on the evidence above
C5's anomaly gates are not measured yet.

### FT2. 4.25 census parity, and proof that the census changes no pixels
- **Parity:**
  - hold each batch until its own readback resolves;
  - keep event targets tagged for the whole event;
  - both as 5.1 does (`AnomalyCensus.cpp:629-699`, cited).
- **The game's own custom-depth actors:**
  - Never change the custom-depth state of an actor the game already has custom depth on.
  - Such actors are not targets.
  - This deviates from 5.1 on purpose. Log it in gotchas and as a 5.1 follow-up.
  - *Why:* a game that reads custom depth must never see ours.
- **The interference gate, at every pose:**
  - A clean hold with the census on must match the census-off reference: no frame may be active by the C2 rule on stable pixels.
  - Report the per-frame maximum of changed stable pixels.
- **If parity alone doesn't pass:**
  - 4.25-only reductions in re-tagging are allowed, as long as 5.1's floor and ceiling hold. For example, don't re-measure actors whose coverage is already known for an unchanged view.
  - Tagging every candidate for the whole session is not allowed.
- **Last resort, narrowly:**
  - A residual region counts as z-fighting only with evidence that two coplanar surfaces trade places there (for example, two actors alternately winning custom depth at equal depth).
  - Those pixels join the pose's unstable map before the scored pass. They are counted in the report and logged in gotchas.
  - Any other residual fails the gate.

### FT3. A pre-pass rule that is well-posed on a real scene
It replaces FS2's 5 % rule and is frozen here, before any measurement.
- **The grain:**
  - Turn it off for scoring through a runtime-only setting cited from 4.25 source, never a saved asset.
  - If 4.25 has no such setting, phase-match the references to the grain's period.
- **Per pose, in order:**
  1. a census-off clean hold of at least 240 frames, which gives the reference and the unstable map;
  2. a census-on clean hold of at least 240 frames, which gives the interference gate.
- **Unstable pixels:**
  - a pixel is unstable if it changes by more than 1 level during the census-off hold, dilated by 2 px;
  - unstable pixels are excluded from every C2 and C3 count.
- **The self-check:**
  - a map built from the first half of the census-off hold must leave the second half with no active frame by the C2 rule;
  - a pose that fails is dropped.
- **The poses:**
  - a pose is usable if at least 50 % of its pixels are stable;
  - poses may pitch down to keep the sky small;
  - at least 6 poses, all fixed before the run.
- **The events:**
  - an event is scored if both its edges fall in scored hold frames, and at least 50 % of its target's visible pixels are stable;
  - other events are counted, but not scored.
- **The rest of FS2 stands:**
  - C2 on the affected label;
  - C3 by FQ2, FQ3 and FR2;
  - both negatives;
  - at least 3 scored events of each anomaly;
  - a scored late-entering target, meaning an actor outside the view at the scored pass's first frame;
  - the census, custom-depth, view and annotation gates.

### FT4. `annotation.json` values carry 5.1's meaning
- `coverage_ratio` is 5.1's screen share.
- `bbox_source` is set as 5.1 sets it.
- The camera's far plane comes from the view.
- Cite 5.1 for each.

### FT5. FR3's reader checks the freeze against the first scored frame's write time
It no longer checks against the moment auto-injection first turns on.

### FT6. 094-36: implement FT2–FT5, rebuild, and re-run C5
- Implement FT2–FT5, and rebuild under FM1.
- Re-run C5 in one street-map session.
- One retry slot.
- The window for real sessions: until 07:00 IST on 11-02.

## For the owner
- **Nothing to do.**
- **The new code handles a big, real scene,** but nothing could be graded yet: my grading rule was too strict for a scene with wind, fog and sky.
- **The run caught a real old-engine problem:** our object tagging makes part of the picture flicker. 094-36 fixes it, and proves the fix by comparing against a run with the tagging switched off.
