# 095-00 — Chat ruling: back to the main line; the client's three review topics become units A / B / C

**Date:** 2026-11-13, about 12:45 IST · **Author:** chat Claude (orchestrator) · **Status:** binding until superseded

**Inputs:**
- The owner's photo of the client's annotation-quality review summary (a table of three priority topics for discussion [client wording paraphrased]), for a capture session dated 2026-09-09. Only that table came home; no session data did.
- Claude Code's read-only review of 2026-11-11, saved verbatim at `_reviews\095-00-code-capture-review-2026-11-11.md` (sections C1–C5, H1–H7, D1–D7, O1–O11, decisions a–d).
- Branch topology and release-line source, read by chat today (read-only git).

**Owner instructions (2026-11-13):**
- Move to the main branch and tackle the three feedback topics.
- **No Codex** until he says otherwise.
- Claude Code runs **Opus 5.5 at high to xhigh**; `max` only when truly required.
- Chat orchestrates through the mailbox.

---

## 0. The three topics, as the client wrote them [client wording paraphrased]

| # | Topic | What they said |
|---|---|---|
| 1 | Frame skips, perhaps from performance cost [client wording paraphrased] | [client wording paraphrased] They saw the first corrupted texture at frame 675, then a big jump in time or camera position from frame 675 to 676. They offered heavy processing as one possible cause, without claiming it. They asked us to line up frame timestamps and capture cadence against the moment of injection, thread timing, and CPU and GPU traces. |
| 2 | LOD pops should have a spatial cause [client wording paraphrased] | [client wording paraphrased] A LOD pop should happen when the camera crosses a sensible distance, or when the player turns toward a part of the scene that was not in view before. A mesh that changes with no change in distance, visibility or view should not serve as a typical LOD example. |
| 3 | Terrain disappearance as a kind of blink [client wording paraphrased] | [client wording paraphrased] Terrain that disappears leaves a hole in the ground surface, which looks different from an ordinary object blink. They want it kept under blink with its own terrain subtype, and suggested a name such as `blink/terrain_disappear_reappear`. |

**Intake caveat.** The reviewed session predates the M3 release (5 Oct). Which host and build produced it is not known yet. The owner reads that at the office (§7). Nothing below depends on the answer: every cause named in unit A is still in the delivered release.

## 1. Branch rulings ("move to main")

**Facts (chat, read-only git, today):**
- `master` = `b5f15a3` (2026-09-26). It is a strict ancestor of `release/2026-10-05` = `d7eee97`, which is 251 commits ahead.
- `d7eee97` is what the client holds as M3 on 5.1, and it is the base of both `fix/followup-capture` (+54, 31 unpushed) and `feat/europa-port` (+237).
- The primary checkout (`D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`, the watcher's working folder) still sits on `m51` = `53bf725`, held since 2026-09-15 and about 27,000 lines behind. Code therefore cold-starts from a stale `CLAUDE.md`.

**Rulings:**
- **M1. `master` is fast-forwarded to `d7eee97` and pushed.** Fast-forward only. Main becomes "what the client has on 5.1" again.
- **M2. The primary checkout moves from `m51` to `master`.** The `m51` branch ref stays exactly where it is; nothing on it is lost. Its 15 untracked files are copied to receipts first.
- **M3. Each unit gets its own branch off `master`** and merges back only when its gates are green: `fix/m56-capture-continuity` (A), `feat/m57-lod-spatial-trigger` (B), `feat/m58-terrain-blink-subtype` (C). Code confirms m56–m58 are unused, or takes the next free numbers and says so.
- **M4. Frozen, do not touch:** `fix/followup-capture` and every J2 worktree, host and archive (093 handoff, P9); `feat/europa-port` and `D:\IA_Europa`; `research/natural-causes` policy (093-02 P10). No tags, ever, from Code.
- **M5. The 4.25 build gets these fixes later,** by merging `master` into `feat/europa-port` once each unit is green on 5.1. Not started until ruled.

## 2. Unit A — capture continuity ("no silent frame skips")

**The standard.** During a run, every engine tick produces exactly one published frame, in order. If a frame is ever lost, the session's own files say so. A gap that the data hides is the worst outcome, because it sits exactly on the label boundaries.

**Answers to Code's four questions:**
- **(a) The settle skip goes. Ruling: capture-and-flag.**
  - Settle frames are written like any other frame. The burst state machine keeps deciding *labels*; it no longer decides *whether a frame exists*.
  - Settle frames carry the existing schema-2.1 transition flag with a new reason value (proposed `settle`), so the client's rule "drop transition frames from positives and negatives" already covers them.
  - **Why not just skip them, as today:** cutting frames at every onset and offset teaches a model the cut instead of the anomaly, breaks real-time video, and hides a discontinuity where the labels change. Chat already ruled this way for natural events (093-02 P3). It now applies to every anomaly.
  - **Why flag instead of trusting settle 0 outright:** every gate on record ran with settle 2. The flag is the safe direction. The unit also *measures* whether settle frames are pixel-exact, so a later ruling can drop the flag with evidence.
- **(b) Yes, carry `6713f51` (the m51 mask-wait fix) now,** as its own commit on the unit branch. Re-implement the same logic if it does not apply cleanly. Gates: the pre-fix build reproduces the drop in both trigger shapes (output height non-zero; mask measure off); the fixed build writes every frame; the never-run backbuffer leg (M-F1b) runs.
- **(c) Yes, blocking back-pressure is accepted.** Under the forced fixed timestep it costs wall time, never a frame and never game time. Never drop by design.
- **(d) Owner read,** see §7.

**Further rulings:**
- **A1. Continuity is checkable from the data alone.** Each label row gains the tick index since run start and a wall-clock time since run start (additive keys, schema minor bump). Consecutive rows differ by exactly one tick on a healthy run.
- **A2. One honest drop count.** `run_summary` gains a single `frames_dropped` total with the missing tick indices and per-cause counts. Write and encode failures are counted and loud. `pending_at_stop` is fixed. A dropped frame is never silently renumbered away.
- **A3. The fps stamp follows captured frames,** so the assembled video is real time again. `ticks_per_captured_frame` reads exactly 1 on a healthy run; that is the headline gate.
- **A4. Throughput comes second and is measured, not read.** Order: continuity and accounting first (A-1), then cost work (A-2: writer queue, census period, snapshots during capture, run-log flushing, readback pooling, per-tick anomaly walks). Each cost change lands as its own commit with a before/after wall-time reading and pixel-identical output.
- **A5. The 675→676 jump: no mechanism is claimed.** Two families fit: a cadence gap at the event edge (C1), or a wall-clock hitch at injection while the player's input keeps arriving in real time. A bench leg measures per-tick wall time across onsets per anomaly type, including first-use cost of `corrupted_texture`. After A1, every session carries this evidence itself.
- **A6. C4 and C5 exist only on the frozen follow-up branch.** They are recorded for J2's resume, not fixed here.
- **A7. Security and hygiene findings (O1 control server on all interfaces, O2 hotkeys during capture) are queued as a separate small unit,** not mixed into A.

**Gate set for A-1 (label sync is the product):**
1. Continuity: tick delta is 1 on every consecutive row; frame count equals tick count. PIE and staged, paced and unpaced, both tick orders.
2. Edges from pixels: every delivered anomaly's onset and offset are 0 frames off on AA-off captures, with settle frames now present. No unflagged frame disagrees with its label.
3. Every labelled frame has its measured mask.
4. Can-fails: the release build fails gate 1 (about 1.33 ticks per frame); a deliberately dropped frame shows up in `frames_dropped` and as a tick gap; a shifted-label mutant fails gate 2.
5. No change to label or mask meaning on non-settle frames.

## 3. Unit B — `lod_popping` gets a spatial trigger

**Today (release):** a timer. The target snaps between its current LOD and its worst LOD every 8 frames while the camera does anything or nothing. The client is right that this is not what a shipped LOD bug looks like. Chat's own 093-00 draft already listed it as "partly real".

**Direction (binding intent; Code plans the mechanics):**
- **B1. The pop is caused by the player's own movement.** Two triggers:
  - **Distance crossing.** The target is held at a low LOD until the camera crosses a threshold distance, then snaps to its proper LOD (and the reverse on retreat, with hysteresis). The threshold is placed where the swap is clearly visible on screen, and just ahead along the camera's current motion so pops stay frequent.
  - **View entry.** A target just outside the view is prepared at a low LOD while unseen. When the player turns and it enters the view, it shows the low LOD for a few frames and then snaps.
- **B2. No movement, no event.** An armed target that never triggers within its window is released without a label and does not count as an anomaly. Every labelled event has something visible on screen.
- **B3. The id stays `lod_popping`.** The subtype names the trigger (proposed `distance_threshold`, `view_entry`). Additive keys record the trigger distance and the frame where the swap happened.
- **B4. The timer version leaves the default auto pool.** It stays reachable from the console for tests only.
- **B5. Label and mask rules do not loosen:** multi-LOD targets only, visible contrast at the moment of the pop, measured masks, 0-frame edges.
- **B6. Look first.** Before any gate campaign the owner sees bench clips of both triggers and says whether they read as the real thing. The bench needs a scripted camera path (test-only; the plugin still does not move the player).

## 4. Unit C — terrain blink subtype

- **C1.** When a `blinking` event's target is terrain, the label reads `anomaly_type: blink`, `anomaly_subtype: terrain_disappear_reappear` (the client's string). All other blink events stay `disappear_reappear`.
- **C2. Terrain detection is game-agnostic and adds no module dependency:** engine landscape actors and components, recognised by class name through the class chain. An optional ini list of extra class or name patterns covers hosts whose ground is ordinary meshes; default empty.
- **C3. Eligibility does not change in this unit.** The coverage band that keeps frame-filling terrain hides out stays as it is. Widening it for terrain is an owner and client call (§7).
- **C4. Every reader of `anomaly_subtype` is inventoried** (host tools, verifier, annotation export, dashboard Library and timeline) so the new value does not break or hide a session.
- **C5. Gate:** a landscape fixture produces the new subtype; a non-terrain target is unchanged; removing the detection fails the gate; readme and schema note updated.

## 5. Sequence (one brief in flight)

1. **095-01 (high):** M1 and M2, rebuild, one baseline capture on `master` with the cadence readings. Released today.
2. **095-02 (xhigh):** file-by-file plans for A-1 and C, read-only.
3. **C implemented and gated** (small, quick), then **A-1**, then **A-2**.
4. **B:** plan, bench clips for the owner, then gates.
5. Merge each green unit to `master`; then the 4.25 forward-port (M5); then one office pull.

## 6. Process without Codex

- The review gate stays chat reading every report before the next brief.
- At checkpoints chat names, an **independent review is a fresh Claude Code side session** (`# SESSION: new`, Opus 5.5, xhigh, read-only), never the session that wrote the change.
- Unchanged: plan before code, gates that can fail, report-and-stop on a failed gate, codenames only, office data never comes home, never kill by process name.

## 7. Open, for the owner

- 🔴 **Office read (numbers only):** which host and which build made the reviewed session, plus its `run_summary` cadence numbers and the event that covers frame 675.
- 🔴 **Is there more in the review than the three-topic table?** If so, photos of the rest.
- **Product, not urgent:** whether large terrain hides (above today's coverage ceiling) should be allowed for the new terrain subtype. Default until told otherwise: no change.
- **Client-facing:** the per-topic plan the owner promised the client on 2026-11-06. Chat drafts it from this ruling once 095-02's plans confirm the mechanics.

## 8. Corrections to carry

- "Main" was stale: `master` lagged the delivered release by 251 commits. After M1 it does not.
- The Bates reading `ticks_per_captured_frame = 1.3556`, recorded as unexplained, fits the settle skip (16 ticks per 12 frames ≈ 1.33). Not proven for that host.
- `D:\IA_BankOverflow` no longer exists (owner deleted it 2026-11-03); older pointers into it are dead.
