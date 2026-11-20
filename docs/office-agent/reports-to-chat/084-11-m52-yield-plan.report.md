# REPORT for 084-11-m52-yield-plan.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-11-m52-yield-plan.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 17:11:39 | finished: 2026-09-29 17:37:22 | duration: 26 min | exit code: 0
context at last turn: 583091 tokens (58% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-11-m52-yield-plan.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Plain summary for the owner.** `stuck_low_mip` fires nothing on the office host (Deimos) because a texture is held only if exactly one object in the whole loaded level uses it. When the picked object fails that test, the whole burst is wasted. Option A lets a small group of objects that share a texture (up to 8) blur together, each with its own mask and label, and adds a picker filter so bursts aren't wasted. On our own level it barely changes the count. Your office log's refusal counts will tell us whether it helps there.

**1. Sharing and yield today, under A (K = 4/8/16) and under B**

How I measured:
- I ran two read-only editor commandlet scans on `_r84_host`, with temporary junctions to the D: content. The D: content manifest was identical before and after (`6143FE1C…`, 4,966 entries), and nothing was saved.
- I checked the scan against the runtime purity lines in the 084-03/06 fix-branch logs. User counts match exactly up to 7 users (for example `T_HoloGrid_M` 7, `T_black` 2, `T_rock_02_*` 1). Above 200 users the scan reads 2–6 % low, because the runtime also counts spawned components.

MainWorld sharing:
- **Users per streamable texture** (29 of 38 textures): 1 user → 7 textures, 2 → 3, 3–4 → 3, 5–8 → 2, 9–16 → 2, 17–64 → 6, more than 64 → 6. Every texture above 16 users has Nanite users.
- **Components that sample a texture** (610): 445 Nanite, 88 maskable, 42 decals, 28 translucent with no custom-depth writes, 7 text/widget.
- **Renderable actors:** 342 on the whole map, 244 of them entirely Nanite.

Targets that pass (A = admit shared textures up to K users and refuse any texture with an unmaskable user, Nanite included; A′ = the same, but Nanite users are labelled without a mask):

| rule | whole map (of 342) | settled camera, sharing only (of 17) | settled camera, sharing + ratio |
|---|---|---|---|
| today (K = 1) | 2 | 2 | 1 (rock A) |
| A, K = 4 / 8 | 2 / 2 | 2 | 1 |
| A, K = 16 | 3 | 2 | 1 |
| A′, K = 8 / 16 / 32 | 6 / 7 / 26 | – | – |
| B (today's rule plus a picker filter) | 2 | – | 1 |

- **The settled-camera set** is the 17 targets the auto-pool actually picked in 490 banked sessions.
- **Today's measured fire rate** at home is 9 fires in 24 bursts, all on rock A.
- **A second wall, separate from sharing:** at the engine floor the top mip is 64 px, so the 8× ratio gate needs a target at least 512 px on screen. 4 of the 9 most-picked targets are smaller.
- **`baseline_pending`** appeared 0 times in the 13 home legs, all with a settled camera. For a moving camera I reasoned from source rather than measured.
- **`CB_GateLevel`** yields 0 under every rule: its 145 targets use a material with no textures (G135).

**2. Option A design (A-core for Thursday)**
- **Admission:**
  - Hold texture t if all of its world users are maskable and the affected set, including t's users, stays at K = 8 components or fewer.
  - Textures are admitted greedily, fewest users first.
  - K = 1 reproduces today exactly, which gives a byte-level regression check.
- **Masks:**
  - The target keeps its current actor-level value.
  - Every other affected component gets its own value, set on that component only.
  - I chose one value per component, not one per event, so each entry gets its own pixel count and drawn box, and the m50 one-value-one-actor gate keeps its meaning.
  - Recycling (from 084-05a) frees all of an event's values together. If the pool runs out when the fire arms, the fire is refused rather than partly masked.
- **Labels:** one entry per affected component on the frames where it is drawn, with its own box. `labelled` comes from the existing annotation authority, so all entries of one event agree. `annotation.json` just gets more nodes; no field is added. New keys appear only on co-affected entries, so every other anomaly's output is byte-identical.
- **Timing:** unchanged. The per-texture render residency record drives every entry; the `partial` / `unresolved` / `taa` flags are worked out per entry from the textures that entry uses.
- **Picker filter:** a new `IsTargetEligible` hook runs before the target is drawn. Without it A cannot raise the fire rate. With K = 1 this filter is option B.
- **After Thursday (A-ext):**
  - A user that appears mid-hold, if maskable and within K, is masked from the frame it is detected. A late detection flags the previous frame as contaminated.
  - Above K, the hold reverts and the frames are flagged contaminated, as today.
  - A `baseline_pending` texture gets a bounded settle-wait (default 8 frames) before it is dropped. Its baseline is still read only after its stream settles.
- **Maskable:** static and spline meshes, instanced meshes, skeletal meshes, foliage as a co-user. Landscape is probably maskable but not yet verified.
- **Not maskable:** Nanite (on 5.1), translucent materials without custom-depth writes, decals, Niagara/Cascade particles, text and widget components.
- **Not visible to the check at all:** UI/Slate brushes and post-process materials. A world texture used there would blur without a label; this is stated as a remaining limit.
- **Unchanged:** the m53 exclusion works per anomaly family and ignores targets, so it already covers co-affected objects.

**3. Gates and their can-fails**
- **G-COAFF:** on a new sibling fixture level, `CB_CoAffFixture` (AA off, native order), pixels show that every object that visibly blurs has a mask and a label, and the 086-01 wrong-object count is 0. Can-fail: a new bench lever that labels only the target must push the wrong-object count above 0.
- **G-YIELD:** on MainWorld at 100 % and 50 %, at least half of all bursts produce an event, and at least one event lists rock B as a co-affected entry. Can-fail: turning off the picker filter must drop back to the 9-in-24 shape.
- **G-CAP:** a texture with K+1 users is refused as over cap. Can-fail: raising K by one makes it hold.
- **G-UNMASK:** fixture cells with a decal user and a translucent user are both refused.
- **G-K1:** with K = 1, the field set and events match `02d15a4`.
- **G-MASKVAL:** no tag-owner violations, the pool is never exhausted, and forcing a small pool refuses fires instead of masking them partly.
- **G-BYTE:** a leg of another anomaly type is byte-identical.
- **Sync gates:** unchanged — the 084-06/084-09 set must still read 0 off at 50 %.

**4. Schedule to Thursday, risk, fallback**
- **Tue 18:30–22:15 (084-12a):** A-core source, self-tests, the mutant lever, both builds. The main risk in the build is this: the first time the label writer emits more than one entry per event.
- **Tue 22:30:** the night bench, as already planned.
- **Wed 08:00–10:30:** Codex review and a fix round (084-12b).
- **Wed 10:30–14:00 (084-13):** fixture, cook and harness.
- **Wed 14:00–16:00 (084-14):** the bench.
- **Wed 16:30:** ruling, delivery cook, and the 088-01 rehearsal (office-check kit merged first, then A).
- **Risk:** the last m52 fix needed four Codex rounds, so I put A-core being ready for Thursday at about 40 %.
- **Fallback 1:** option B, about 3 hours, but it may also yield 0 at the office.
- **Fallback 2:** ship `stuck_low_mip` available but off by default, with a readme line saying it rarely or never fires on shared-texture content.
- If the office counts show nearly every refused texture with 9 or more users, go straight to fallback 2. A per-target proxy texture is named in the plan as the approach that would work there; it is not planned.

**5. Commits pushed**
- `ebe856d` on `fix/m52-label-timing` (`02d15a4..ebe856d`; local and origin match).
- It contains `docs/predictions/2026-09-29-m52-yield.md`, the journal `docs/sessions/2026-09-29-084-11-m52-yield-plan.md`, the status block, and gotcha G397: in an editor commandlet, textures read as 32×32 until async compilation finishes. My first scan made that mistake and I re-ran it.
- Evidence is in `_reviews\084-11-evidence\`. `m51` (still at `53bf725`), `master`, the m53 branch and all tags are untouched.

**6. NEEDS-DECISION**
1. **Build A-core now, or wait for the office counts?** Plan §1.1 lists in advance what each count means. The deciding one is the spread of the N values in the log's `shared_world - N user component(s)` lines.
2. **Nanite co-users:** refuse them (my recommendation for Thursday, because pixels can't prove a label with no mask), or label them with `-1` (A′)?
3. **K = 8:** home content gives the same result for K = 4, 8 and 16, and nothing suggests 8 is wrong. Revisit only if the office N values cluster between 9 and 16.
4. **Accept the picker filter as part of A.**
5. **The ratio wall (≥ 512 px on screen) is reported, not tuned.** If the office's `too small for the ratio` count dominates, neither A nor B helps.

Note: the project's `CLAUDE.md` codename rule lists "Deimos" as retired (G222). I used it as the brief gives it.
api-equivalent cost (not billed on Max): $17.46 | turns: 85 | running total across all briefs: $3,673.66
