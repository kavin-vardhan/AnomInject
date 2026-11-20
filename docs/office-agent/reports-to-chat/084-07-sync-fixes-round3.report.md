# REPORT for 084-07-sync-fixes-round3.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-07-sync-fixes-round3.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 03:04:21 | finished: 2026-09-29 03:46:14 | duration: 42 min | exit code: 0
context at last turn: 734707 tokens (73% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-07-sync-fixes-round3.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION. All of 084-07's source work is done, built with 0 warnings, archived and pushed. Two decisions are chat's before the 084-08 harness is written (item 9).

**2. Summary for the owner**
- The stuck-texture (m52) blur at the start of an event is a partial effect, not a timing error. The object's textures blur one at a time, and how visible the first frames look depends on which texture goes first.
- Those frames are now flagged as partial.
- camera_clipping no longer labels a frame just because the camera sits inside an object's bounding box. It checks the actual surfaces.
- All five review findings in scope are fixed. Nothing was launched, cooked or staged.

**3. m52 onset: reading (a), partial application**
- I joined each frame's per-texture render record with the 084-06 evaluator's measured blur. The held set is the rock's normal, diffuse and AORM (roughness) maps; baseline 11 mips, held level 7. On all 16 judged events in both banks, labelled frames 0–1 have only part of the set held:
  - 084-03: normal map at 7, the other two at 11 → **0.66–0.78** of full depth.
  - 084-06: diffuse and AORM at 7, normal map at 11 → **0.06–0.23**.
  - All three held → 0.91–1.00.
- **70 % vs 9–22 %:** on this rock the normal map carries most of the sharpness drop, and the drop order flipped between the two builds. It also flips within a run: each run's warm-up event has the other order. Why the order varies is not established.
- **Fix:** the render record now carries each texture's held level. Any labelled frame whose set sits between baseline and the held level gets `transition_reason: partial`, on either edge, with or without anti-aliasing. `run_summary` lists the partial frames per edge (onset / mid / offset) and per event, so every flagged frame is identifiable.

**4. camera_clipping**
- **As built:** the bounds test is now only the first pass. For each object it passes, a 16×9 grid of rays is traced only inside the clipped zone, against that object's own collision triangles; landscape uses its height field. Small objects also get a 4×4 grid over their own footprint. A hit labels the frame, and `camera_clipping.clipped_ray_fraction` is reported per frame.
- **Objects that can't be checked:** skinned characters, no collision, simplified collision, over 16 objects or 320 traces. The bounds result stands and the frame is flagged `camera_clipping_unconfirmed`, never silently guessed.
- **Cost:** the grid-and-clipping overhead alone is 16 µs per frame at 16 objects. The trace cost itself can only be measured in-engine, so it is instrumented in `run_summary` for the next bench, not measured.
- **What it can miss:** a sliver thinner than the ray spacing that belongs to a large object, such as a thin pipe inside a room mesh.

**5. The other findings**
- **F2:** instanced meshes now use each instance's full transform (scale, rotation, mesh offset); the old query dropped scaled instances. `AnomalyViewport.cpp` plus `AnomalyNearClipSlab.h`.
- **F3:** the sync path wrote its label row before adding session-wide anomalies. It also read `IsHidden()`, which is always false for our hide anomalies. It now writes the row and the counts from one frame snapshot.
  - Delivered labels: not affected in practice. 0 of 88 banked sessions contain a sync row; it is reachable only through a fallback or an explicit console setting.
- **F4:** retiring a mask ID now clears every object we tagged with it, including ones whose custom depth the game switched off (value restored, the game's switch left alone). If anything still holds the value afterwards, it is set aside instead of reused.
- **F5:** unfinished transition and hide-return history now carries into the next capture run.
- **Row families (ruling 7):** every `anomaly_present` row outside its event's frame list is "event active, effect not applied this frame":
  - blinking's visible phases (MASK55 128 rows, REAL_BL 56 per leg);
  - lod_popping between pops (80 per leg): fire active on frames 3–18, forced LOD only on 8–15;
  - B0L's 345 rows, which are the timing lever.
- **The ruling's premise was wrong:** these rows all read `visible_positive: true`, not 0. Entries now carry a `labelled` field, and `visible_positive` requires a labelled entry with a box. The client readme lists each allowed family.
- **Transition defaults:** now 3 / 8 / 1 in source under temporal anti-aliasing, and 0 without it.

**6. Unit tests (both ways)**
- m52 window selftest: **220 checks, 0 failures** (31 new).
- camera_clipping selftest: **118 checks, 0 failures** (25 new).
- Each new rule is tested against its broken variant (for example, bounds-only over-labels the hollow box, and the old instance query drops the scaled instance).
- Moved to the next bench because they need the engine: trace cost ≤ 0.5 ms, landscape height-field confirmation, the fast query for hierarchical instanced meshes (HISM), and a sync-leg event check.

**7. Build**
- Final exe **`FF41BFF3`** (242,093,056 B), built from `8b8b4d0`. Game and Editor both built with 0 warnings.
- An intermediate exe `54BEC386` was replaced after I fixed a stale rule name in a log line; it was not archived.
- The string scan finds every new string in the exe, both old strings absent, and the reference strings present. The comment stripper changed nothing.
- Archived at `_binary_baselines\m52fix-FF41BFF3\`, hash re-checked there. Not staged, not cooked.

**8. Commits**
- `8b8b4d0` (source and tests) and `a6a4893` (journal `docs/sessions/2026-09-29-084-07-sync-fixes-round3.md`, status block, client readme, architecture, gotchas G357–G360).
- Pushed `b40a7f4..a6a4893`; local equals origin. `m51` (`53bf725`), `master` (`b5f15a3`) and tags are untouched.
- Context at the end: roughly 30 % of the window (estimate; there is no exact meter).

**9. NEEDS-DECISION**
1. **Ruling 1's "< 10 % ⇒ label-early" rule versus proven partial frames.** In 084-06, 6 of 16 events had both onset frames at 6–9 % of depth, although the render record shows diffuse and AORM already held. If that texture order repeats, the gate counts them label-early. Should the rule stand, or should it defer to the render record?
2. **Warm-up events read 5 partial onset frames in both banks,** which trips the "> 3 frames on an edge ⇒ investigate" check. Should 084-08 keep excluding warm-up events, or should these count?

`visible_positive` now reads false on those non-manifest rows, so 084-08's harness and `verify_capture.py` must be checked against the new rule before the bench.

Several connectors (Gmail, Calendar and a set of plugin servers) need authorization in their settings before they can be used, and the PDF viewer server failed to connect. None of these were needed for this work.
api-equivalent cost (not billed on Max): $32.76 | turns: 262 | running total across all briefs: $3,272.57
