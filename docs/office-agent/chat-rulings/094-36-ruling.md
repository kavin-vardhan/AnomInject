# 094-36 — Chat ruling: the 4.25 port's capture works on a real scene (no tagging interference, census at scale, exact restore, player view, 5.1 labels). Real-scene grading found anomaly-design issues shared with 5.1, so it moves off the port's path. 094-37 finishes the port (parity fixes, a capture self-check line, D: cleanup, a short street-map smoke) and writes the office capture card

**Date:** 2026-11-01, about 17:50 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-36r:** NEEDS-DECISION. It ran 13:11–16:15 IST, with one street-map session from 15:11 to 15:23 (6,336 frames, 0 dropped).
- **Codex's report:** `_reviews\094-36-codex-report.md`.
- **Code's review:** `_reviews\094-36-code-review.md`. Code recomputed everything from the raw files.

## Verdict
**What the port achieved on the real map (accepted):**
- **Census parity:**
  - batches stay tagged until their own readback, with at most 2 in flight, as in 5.1;
  - 141, 116 and 92 drawn actors at three poses;
  - the ledger was never exhausted, and the reserved value 203 was never assigned.
- **No interference from our tagging:**
  - 094-35's wall strip is gone: 0 changed pixels on every census-on frame.
  - Code's switch test: the first census-on frame, before any actor is tagged, already sits at the census-on level. The step at the switch is no larger than the drift within the census-off half, and the stable pose shows no step.
  - So the three formal failures are scene drift.
- **Custom depth:**
  - 1,980 components are equal before and after;
  - 6,336 mid-session samples obey the ownership rule;
  - the game's own custom-depth actors were never touched.
- **The view:** 6,336 of 6,336 frames equal the player's view.
- **The annotations:** schema 2.1, with 56 keys in order. The affected and injected sets agree with the rows on all 36 events.
- **Blinking's hidden-frame masks on the real map:** IoU 1.000, precision 1.000, spill 0.
- **Safety:** none.

**What the real-scene grading showed:**
1. **My interference gate** (one reference, ≤ 0.001) cannot hold on a scene that drifts. It is the same lesson for the third time, so the gate is withdrawn rather than redesigned.
2. **Four anomaly-design issues that apply to 5.1 too,** pending FU3's parity check:
   - **blinking:** after a reveal, the target area settles over 3–4 frames (from 8 % to 4.5 %) while it is labelled clean;
   - **lighting:** the change ramps over 13 frames, and the receiver mask covers 99 % of the frame (wall, tree, ground and sky), so precision is at most 0.20;
   - **LOD:** the swaps on these props change 0.004–0.011 % of the target, which is invisible;
   - **missing_object:** the targets were wind-animated bushes and an awning, which can't be graded against a moving background.
3. **Port gaps:**
   - G500, a bench ordering bug (the fix is unbuilt);
   - `coverage_pct` is −1 (the fix is unbuilt);
   - an infinite far plane is written as 0, where 5.1 writes 1,000,000;
   - there is no lost-batch timeout (5.1 has `LostAfterTicks` 8, plus the no-pass and polluted discard), so one lost readback would stall the census for good;
   - an event adopts the census's value instead of asserting its own. The masks stay correct.

## Rulings

### FU1. Accepted on stock 4.25, on a real scene
**What is accepted:**
- census at scale, and its parity with 5.1;
- no tagging interference, on the switch-test evidence;
- the exact custom-depth restore, with the game's own actors untouched;
- player-view capture;
- 5.1's annotation schema and frame sets.

**Together with C1–C4,** the 4.25 capture path is accepted for the office smoke, with FU3's finishing fixes.

### FU2. Real-scene label-free grading moves off the port's path
*Why:* its failures are about anomaly design and evaluation method on real scenes. They apply equally to 5.1, and they don't block running the port at the office.

**They are logged as dataset-quality items for both engines (FUTURE):**
- **D1:** a settle tail after blinking's reveal, and after any revert. Either label the settling frames, or hold the label until the image settles.
- **D2, lighting:** the ramp; and a mask of the receiver pixels that actually change, rather than whole receivers.
- **D3, LOD:** pick only meshes and LODs whose swap is visible at the current screen size.
- **D4, missing_object:** targets in animated foliage can't be graded against pose references.
- **D5, evaluation:** real scenes need same-time counterfactuals, not pose references: two runs of the same seed, one clean and one injected. This is the planned replay capture.

The 5.1 data may carry D1–D3 too. Flag them for a later look.

### FU3. 094-37 finishes the port
- **Build the unbuilt fixes:** `05c55e2` (G500) and `2b9dc19` (`coverage_pct`).
- **The far plane:** write 5.1's 1,000,000 for an infinite far plane in `annotation.json`.
- **The census:** port 5.1's lost-batch timeout, and its no-pass and polluted discard.
- **The event-adopts-census-value deviation stays.** FT2 allowed it, and the masks are correct. Log it in gotchas.
- **A parity check, by reading, citing 5.1,** of each smoke anomaly's apply and revert path:
  - blinking's hide and reveal calls;
  - lighting's change, and any ramp;
  - LOD's chosen model;
  - missing_object's hide.

  If 4.25 differs from 5.1, fix it. If it doesn't, D1–D3 are confirmed as shared.
- **A one-line capture self-check,** logged at capture start and again after 300 frames. It reports:
  - the viewport class and size;
  - the backbuffer format;
  - the RHI;
  - whether `PostRenderViewFamily` fired (with a count), and whether that was before the UI;
  - the tick mode;
  - the census state, and whether any readback was lost.

  *Why:* the office card reads it.
- **A fresh build budget:** FM1's 3 attempts per target.

### FU4. D: cleanup, approved by the owner (11-01, about 17:40), as step 0 of 094-37
- **Scope:** the folders of finished jobs under `D:\IA_Europa\Receipts`, from `09403` to `09433c` and from `CodeReview09403` to `CodeReview09433c`.
- **Remove:**
  - picture and frame data;
  - ledger databases;
  - logs and archives;
  - any other file of 1 MiB or more.
- **Keep:**
  - `P9`;
  - `09434` onward, and their `CodeReview` folders;
  - every path that the current kits, tools and tests reference. Find these first, by searching the kit scripts, `tools/europa`, the launch kits and the relay run folder;
  - FH2's real probe files;
  - 09427e's contact sheets;
  - 09427b's crash analysis;
  - small summaries: `.json`, `.md`, `.txt` and `.log` files under 1 MiB.
- **The receipt:**
  - `D:\IA_Europa\Receipts\_cleanup-094-37\removed.tsv`, listing each file's path, size and modified time;
  - plus a summary: the space freed, and D:'s free space before and after.
- **Nothing outside that scope is touched.**

### FU5. 094-37's smoke
**The session:**
- one street-map session, from the player's view;
- seeded auto-injection 4242, with the four smoke anomalies;
- 1280×720;
- no label-free grading.

**The gates:**
- each anomaly fires at least 3 times;
- every affected frame has a mask, and clean frames have none (FQ3);
- **`annotation.json`:** schema 2.1, with the sets agreeing with the rows, the far plane as in 5.1, and a valid `coverage_pct`;
- **the census:** more than 55 drawn actors at some pose, no exhaustion and no lost batch. A focused test covers the timeout;
- **custom depth:** restored exactly, with the game's own actors untouched;
- the wall pose's strip stays unchanged with the census on;
- the view equals the player's view;
- the self-check line is present and sane;
- no crash, no upload, and the Paris host unchanged.

**The rest:**
- one retry slot;
- **the window for real sessions:** until 07:00 IST on 11-02.

### FU6. After the smoke passes, Code's two documents (same job)
- **The pre-push scan:** the commits since `4f46a9c`, checked for secrets, identifiers, codenames and binaries.
- **The office capture-smoke card for Europa's fork,** extending the D-lite card:
  - pinned to the passing commit;
  - the build;
  - a short seeded capture on an ordinary level;
  - typed reads only, under the HARD RULE: the self-check line, the event counts, the frame and mask counts, whether the annotation file is present, and "crash: none";
  - the expected value for each read, and what to do if one differs.

## For the owner
- **Nothing to do now.**
- **The port's capture works on a real scene.** The flicker is fixed and proven.
- **The real-map grading found four ways some bugs don't look right on real scenes.** They're also true on 5.1, and they're logged for later.
- **Next:** the last port fixes, the D: cleanup, a quick smoke run, and your office test card.
