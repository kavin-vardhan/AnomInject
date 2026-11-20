# 094-34 — Chat ruling: C4 passes; capture runs through the real firing path on 4.25 at two resolutions. Next is C5, real-scene scale with 5.1 parity (census, custom-depth restore, player view, annotations), on the Paris street map with a moving view (094-35)

**Date:** 2026-11-01, about 09:25 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-34:** GREEN. It ran 07:03–08:22 IST.
- **Codex's report:** `_reviews\094-34-codex-report.md`.
- **Code's review:** `_reviews\094-34-code-review.md`. Code recomputed everything from the raw files.

## Verdict
**C4 passes:** two seeded (4242) auto-injection sessions, one at 1280×720 and one at 1600×900.
- **The frames:** 1,440 per session, 0 dropped.
- **The events per session:** blinking 19, lighting 11, LOD 16, missing_object 12.
- **C2:** 0 mismatches.
  - All 819 clean frames show 0 changed pixels.
  - The 154 pixel edges match the label edges exactly.
  - All 116 edge-shift tests fail.
- **C3:**
  - IoU 1.000 and spill 0 everywhere;
  - precision 0.937–1.000;
  - lighting's completeness 1.000;
  - clean frames carry no masks.
- **The negatives fail as required,** including FR2's lighting subsets. Lighting's real mask scores 1.000, the best single object 0.44, and all-but-the-smallest 0.84.
- **The same seed** fired the same 58 events.
- **The static check is clean:** no fixture names, no fixed tags, no hard resolution.
- **FR3's reference rule** was committed before the first session.
- **Safety:** no crash, no upload; the Paris host and `m51` are unchanged.

**Phase C works on stock 4.25 in the fixture scene,** with the plugin firing anomalies itself.

**What it doesn't yet do, per Code:**
1. **It tags every visible actor once, at session start.**
   - It fails when more actors are visible than the 55 free ledger values (200–254).
   - It never targets an actor that comes into view later.
   - It admits any drawn pixel, without 5.1's census floor and ceiling.
   - It leaves custom depth on for every tagged actor.
2. **`IAI.Capture.C4` forces the scene's first camera actor** rather than the player's view.
3. **The 4.25 `annotation.json` lacks 5.1's affected and injected frame sets.**

## Rulings

### FS1. C4 is accepted on stock 4.25

### FS2. C5: real-scene scale with 5.1 parity (094-35)
**The changes,** each citing the 5.1 code it ports:
- **The census:**
  - Port 5.1's census, through the existing `SetCensusProvider` seam, with its coverage floor and ceiling. Use whichever form 5.1 uses, rotating or per-event.
  - Actors that come into view later must be eligible.
  - More visible actors than free ledger values must never fail the session.
- **Custom depth:**
  - Our custom depth and stencil are on only for the targets currently tagged.
  - Each actor's prior state is restored exactly through the ledger.
  - *Why:* a game that reads custom depth must never see ours.
- **The camera:**
  - Capture uses the player's own view, its current view target.
  - Any forced camera is bench-only, and off by default.
- **The labels:**
  - `annotation.json` matches 5.1's schema, including the affected and injected frame sets.
  - "Affected" keeps 5.1's meaning: the anomaly is on screen.

**The test scene:** the Paris host's own street map, loaded read-only. It should show more than 55 actors at once, with real occlusion, which the fixture scene never had.

**The test protocol.** *Why:* C2 and C3 compare each frame with a clean reference. When the view moves, the reference must come from the same view.
- **The bench driver** is off by default.
  - It turns the player's view in fixed steps and holds at each pose.
  - The poses, and the hold, turn and settle lengths, are fixed before the run.
- **A clean pre-pass,** in the same game session, right before the scored pass: the same sweep with capture on and auto-injection off.
  - It doubles as the warm-up: let shader compiles and streaming finish first.
  - It records each pose's reference and its unstable pixels: pixels that change during the clean hold beyond the quantization-aware tolerance (animated foliage, for example).
  - A pose with more than 5 % unstable pixels is dropped, by rule, before the scored pass.
- **The scored pass:**
  - seeded auto-injection (4242), restricted to the four smoke anomalies;
  - 1280×720;
  - long enough for at least 3 events of each anomaly with both edges in scored frames.
- **The scoring:**
  - It uses hold frames after the settle frames, against that pose's reference, with unstable pixels excluded.
  - C2 uses the per-frame affected label.
  - Edges inside turns are counted, but not edge-scored.
- **The scoring settings,** as in C4: auto-exposure locked, and AA as in C4. If the fixed exposure clips the street map, lock it at its converged level instead, at runtime only, and say how.

**The gates:**
- **C2:** 0 mismatches by the label-free check.
- **C3:** FQ2, FQ3 and FR2. Both negatives fail.
- **The census:**
  - the visible count exceeds 55 at least once;
  - at least one scored event targets an actor that was not visible at session start;
  - no ledger-exhaustion failure.
- **Custom depth:**
  - **Setup:** before the pre-pass, the driver gives a few candidate actors a non-default custom-depth state at runtime. One of them uses a stencil value inside the ledger's range.
  - **The ledger never hands out that value.**
  - **At the end:** every candidate, those included, is exactly as it started.
  - **Mid-session samples:** the actors with custom depth on are exactly the tagged targets, plus those already on before the session.
- **The view:** each captured frame's view matches the player camera's point of view.
- **Annotations:** the schema matches 5.1's, and the affected and injected sets agree with the per-frame rows.

**The rest:**
- **FR3:** the poses, references and unstable maps are written down before the scored pass.
- FM1's build gate and FJ1's lean profile.
- One retry slot.
- **The window for real sessions:** until 07:00 IST on 11-02.

### FS3. After C5: the office capture smoke for Europa's fork
- **Pre-push:** a lean scan of the new commits (secrets, identifiers, codenames, binaries).
- **An office card,** extending the D-lite card:
  - build;
  - a short seeded auto-injection capture on an ordinary level;
  - typed reads only, under the HARD RULE;
  - pinned to the commit that passes C5.
- **The card also reads how Europa's fork renders,** from a one-line capture self-check: its viewport, its render target, whether `PostRenderViewFamily` fires, and its tick mode (Code's 094-34 item 3).
- **The D-lite injection card** stays valid for any office day before then.

## For the owner
- **Nothing to do.**
- **Capture works on 4.25 when the plugin picks its own targets.**
- **Next:** make it handle a full real scene with a moving camera, the way the office game will look.
