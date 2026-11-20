# 084-01 — Chat ruling on the m52 label-timing diagnosis: the fix contract, restore handling, timeout, the wrong-object bug, and the sequence

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `fix/m52-label-timing` `6405efb` (off `master` `b5f15a3`)
Governing rule: `085-00` ADDENDUM. **Label sync is the product: labels and masks match the picture on both edges, 0 frames off, measured from pixels, with gates that fail on a broken build.**

## Accepted
- **Both root causes are established from source and bank, and Codex agrees independently (ROOT-CAUSE-ESTABLISHED; consult AGREED-WITH-CORRECTIONS):**
  - **onset** reads the game-thread mirror `GetNumResidentMips()`, which updates only after the render-thread swap;
  - **offset:** `BeginRevert` drops the fire and `Revert()` goes inactive in the same tick, while stream-in is still in flight;
  - **masks and `observable`** share the same switch.
- **Bank:** blur 1 frame before the label on 43 of 43 events; **5–14 blurred, clean-labelled frames after the label** on 42 of 42.
- **The original m52 gates were circular (onset) and blind to pixels (offset).** Chat approved them, and chat owns that.
- **Withdrawn claims,** correctly: the fade explanation, "always ≥ 1 frame late", "all textures switch together".

## Decisions
1. **The fix contract, with the label window driven by render-side truth:**
   - `stuck_low_mip`'s label, target masks, `stuck_mip.held` and `observable`-eligibility come from **the render-thread residency record for the exact captured frame** (`FStreamableTextureResource::GetState()`, read in the capture pass): **held = any held texture was drawn with fewer mips than baseline.**
   - **The event outlives `Revert`** until that record shows every held texture back at baseline. Masks stay armed through that trailing window.
   - **A configurable post-restore settle tail** (`IAI.StuckMip.SettleTailFrames`, **default 0**) is built in now. Its value is set from part B's measurement: if the slow final recovery is temporal history (anti-aliasing), the tail covers its measured convergence. Otherwise it stays 0.
   - **Visibility stays separate:** m55's per-frame change evidence and `observable` report whether the change is actually visible. Content with no visible difference (the G270 class) is a *visibility* question, not a sync one.
   - **The sync gate on measurable content:** every pixel-visible blurred frame is inside the labelled window (**0 unlabelled visible frames**), and no labelled frame precedes the first render-held frame.
2. **Restore handling: gate the next burst on the restore.** Capture continues. No overlapping m52 events.
3. **Timeout:** frames stay labelled held for as long as the render record says held. The event is marked **"restore unresolved"**, further m52 fires are refused with a named reason, and the run is not stopped. Nothing is ever silently labelled clean.
4. **The wrong-object bug (the owner's glove/wall observation) is IN this fix.**
   - Confirm the cause from source first. The likely one: the co-affected check counts only the visible set at fire time, and/or misses component types.
   - Then apply the owner's purity rule over the **whole loaded world:** a texture with any other user component (any type: static, skeletal, ISM/HISM, landscape, decals where applicable) is refused.
   - **Gate:** for every fired event, an independent engine-side enumeration in the harness shows **exactly one** user component of the held texture. Can-fail: the old rule on a known shared texture must be caught.
   - A CaptureBench fixture for it (small object plus off-screen wall) goes into the next fixture cook with m53. Until then the gate uses StackOBot's own shared textures.
5. **Part B is AUTHORISED as tonight's bench window, and it measures the baseline AND the fix build in the same window:**
   - baseline: the archived m55-merge exe `E0BE6F0A` with container `67EA1FE0`;
   - fix: the new build;
   - legs: B0, B3 and B5 on both builds, B9 (anti-aliasing off) on the baseline, with B7 if time allows;
   - hash-verified swaps, restoring the m53 staged set afterwards.
6. **CLAUDE.md status block:** update it in 084-02.

## Sequence
1. **084-02 (xhigh, fresh session; compiling allowed by day, no game launch):**
   - the fix source (items 1–4);
   - builds (Game and Editor, 0 warnings);
   - offline and unit proofs;
   - the harness for part B, with both-ways gates including the doctored-label negatives (onset +2, offset −5) and a lever that restores today's timing;
   - a Codex source review through the Relay, collected and not acted on.
2. **084-03 (high, tonight, bench):** part B on both builds, the gates, and the settle-tail measurement.
3. **Chat rules,** then the fix is merged to `master` after Codex's evidence check. Then comes the label-sync audit of every other delivered anomaly.
