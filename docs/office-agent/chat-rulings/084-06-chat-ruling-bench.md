# 084-06 — Chat ruling on tonight's label-sync bench (RED): four anomalies proven on real content, the mask fix proven; m52 onset shows partial frames; camera_clipping over-labels hollow meshes; lod_popping and B-CC unjudgeable. Next: 084-07 grows by three items; a harness brief follows

Date: 2026-09-29 ~03:10 IST · Author: chat Claude (owner offline, per his instruction) · Branch `fix/m52-label-timing` `b40a7f4` · Build `E9FF019A` on container `67EA1FE0`

## Accepted
- **The run:** 31/31 legs accepted on attempt 1; the pacing is clean. The recovery after G355 is sound (the harness's own restore, postflight PASS, the m53 hashes re-verified).
- **SYNC-PROVEN, now on realistic MainWorld content, anti-aliasing on and off (B-REAL 10/10 legs at 0/0; the 086-01 checker agrees):**
  - blinking;
  - missing_object;
  - corrupted_texture.

  This adds MainWorld evidence to 086-01's synthetic-heavy proof. missing_object is no longer "synthetic only".
- **missing_texture: SYNC-PROVEN on the targeted legs** (B-REAL 20/20). The one MASK55 borderline event is ruled below.
- **The mask-ID fix is proven in runtime:** 119 events, 586/586 labelled frames masked, 64 recycles, 0 exhausted, 0 aliasing frames.
  - Codex's F4 path was not exercised (peak live IDs = 1), so F4 is still fixed in source by 084-07.
- **The m52 offset is exact** (0 on 16/16, anti-aliasing off). The owner's "blur lasts ~5 frames after the label" is fixed.
- **Can-fail levers behaved:** B0L fails timing, P1 fails purity. Purity passes on all fix legs.
- **G354 / G355 / G356** accepted as found.

## Rulings

### 1. m52 onset (anti-aliasing off): the first two labelled frames show 9–22 % of the blur
Two readings are possible, and they need different fixes. **084-07 decides between them from the banked render records first, offline, before any source edit:**
- **(a) Partial application.** The held set is only partly at its held level on those frames: one texture of several has dropped, or the streamer is part-way down the chain. The label is right that the event has started, but the picture shows only part of the effect.
  - **Fix:** a **residency-derived `transition` flag** with reason `partial`. It is set on every frame, either edge, where the event's render record shows the held set between baseline and the held level, **with or without temporal AA**, alongside the TAA frame-count flags.
  - This is the same schema the client already gets for TAA, and it doesn't depend on how big the intermediate step happens to be. That matters, because it varies: ~70 % in 084-03, 9–22 % tonight.
- **(b) A render-to-picture lag.** The record says the whole held set is at the held level, but the picture still shows 9–22 %. The record then runs ahead of the image, and that is a real desync.
  - **Fix:** the label follows the image, i.e. the frame the swapped resource is first sampled. Code proposes the mechanism with the evidence.

The same analysis must explain why the level of those frames fell from ~70 % (084-03) to 9–22 % (tonight). Chat's leading hypothesis is multi-texture drop order, not a code change.

**Guard against the flag excusing a desync (not circular):** the gate still judges every unflagged frame from pixels, at 0 off. It also **reports, per edge, the count of partial-flagged frames and their measured effect level.**
- A partial-flagged frame measured at < 10 % on the onset side is counted as **label-early**, not excused.
- More than 3 partial-flagged frames on any edge is a FAIL to investigate.

### 2. camera_clipping over-labels hollow geometry (MainWorld 200/200 labelled, nothing visibly clipped)
Bounds are a superset of the triangles, so a bounds-only test can't be the label. **The label becomes: bounds as the broad phase, then triangle-level confirmation.**
- For each candidate that passes the oriented-box SAT, cast a grid of ray segments **inside the slab only** (from the baseline near plane to the anomalous near plane, along view rays through the candidate's slab footprint) with **complex** traces against **that component** (`LineTraceComponent`, `bTraceComplex`).
  - A hit means geometry is in the slab, so the frame is positive.
  - Report the hit fraction as `camera_clipping.clipped_ray_fraction`.
  - Code chooses the grid density against a cost budget of **≤ 0.5 ms mean per labelled frame** and states what it can miss (sub-grid slivers).
- **Candidates that can't be confirmed at triangle level** (no query collision, no complex data, skinned bodies that only have physics-asset shapes, over a per-frame candidate cap): the SAT result stands, **and the frame carries a per-frame uncertainty flag** that clients can filter on. Use the `transition` key's pattern and a reason (e.g. `camera_clipping_unconfirmed`). The label is never silently a guess.
- **Landscape:** confirm with the heightfield trace; it is exact.
- **Unit tests both ways:** a hollow box around the eye (must be 0 and not flagged, when collision exists), a wall in the slab (1), a thin pole between grid rays (documented miss or caught by the footprint grid), a NoCollision hollow mesh (SAT positive, flagged), and F2's scaled instances.
- **The acceptance bar on the next bench:**
  - B-CC-MW's 200 frames agree with the pixel evidence (no over-label);
  - the B-CC camera-schedule legs pass at 0 off from pixels on both edges.

### 3. B-CC cannot run on container `67EA1FE0` (09-04, descriptor without `AnomalyBench`)
**084-07 ends with a cook of `fix/m52-label-timing`** after its source fixes. Cooking is a commandlet: no launch, no focus, allowed by day.
- The result is a complete staged set for the fix branch, archived, with a descriptor that lists `AnomalyBench` as a bench-only module.
- Nothing is staged by 084-07; staging happens in the next bench brief.
- **G354 consequence for delivery:** the delivery packaging checklist must prove the shipped descriptor/modules match the intended set (levers out; nothing silently missing).

### 4. lod_popping: UNJUDGEABLE (0 measurable events under AA, 4 with AA off)
The effect is too faint on the current fixture. It still goes to the owner's Thursday decision if it is unproven then. **The harness brief (084-08):**
- builds a high-contrast LOD fixture: targets on MainWorld (or cooked fixture meshes) whose forced LOD differs strongly from LOD0 at the bench camera distance, selected by a measured pre-check;
- runs ≥ 16 judged events per condition (AA on and off, both tick orders).
- The m30 gate stays banned as evidence.

### 5. The MASK55 missing_texture borderline (+1 at onset, threshold at 97 % of depth, reference inside a 4-frame gap after the previous event)
**084-08 re-evaluates it offline with an uncontaminated reference** (the post-event clean frames, and the pre-event frames beyond the previous event's settle).
- If it passes, it is a **reference confound**. A dated amendment is declared **before** the re-evaluation and applied **uniformly to every type**: a minimum clean-reference span, below which an event is CONFOUNDED, counted, and reported per type, never silently dropped. missing_texture stays SYNC-PROVEN.
- If it still fails, it is a real +1 and goes into the next source round.
- The office kit (087-01) gets the same rule.

### 6. The harness fixes (G355, G356): ACCEPTED, done in 084-08
- The restore catches every error and retries with backoff.
- The null calibrates from its own sharpness series, not from events this build no longer writes.
- The m52 AA-on legs (B0 12 censored, B3 5 censored) are re-run on the next bench after G356.

### 7. The `anomaly_present` rows with no labelling event (MASK55 128 blinking, REAL_BL 56 per leg, LOD 80 per leg, B0L 345)
Blinking's rows are the documented F1 shape: the event is active, the object is in its visible phase, and `visible_positive = 0`. B0L is the lever.
- **084-07 classifies every row family** (folded into F3).
- **lod_popping's 80 rows per leg must be explained.** Any family that isn't "event active, not visible this frame, visibility fields say so" is a defect, fixed in 084-07.
- The client readme names each allowed family.

## Status per delivered anomaly after 084-06
| anomaly | status | next |
|---|---|---|
| blinking, missing_object, corrupted_texture | **SYNC-PROVEN** (synthetic + MainWorld, AA on/off) | none |
| missing_texture | **SYNC-PROVEN** on targeted legs; 1 auto-pool event pending the reference check | 084-08 offline |
| stuck_low_mip | offset **exact**; onset partial frames | 084-07 analysis + fix; re-bench |
| camera_clipping | **over-labels hollow geometry** | 084-07 triangle confirmation + cook; B-CC re-bench |
| lod_popping | UNJUDGEABLE | 084-08 fixture; re-bench |
| masks (all types) | recycling **proven**; F4 path unexercised | 084-07 F4 in source |

## Next
1. **084-07 (xhigh, fresh session), released now.** No game launch, so it runs at night too.
   - Items 1, 2, 3 and 7 above, plus F2, F4, F5, F3, defaults 3/8, and readme semantics.
   - Rebuild, then **cook**, then the Codex re-check.
2. **085-01 (m53 DC plan, bench-free)**, then **084-08** (harness: G355, G356, the LOD fixture, the missing_texture reference check, B-CC legs on the new cook, and the m52 re-run legs), then **085-02** (m53 source).
3. **Tuesday night:** the re-bench of everything above on the 084-07 build.
