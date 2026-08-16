# Chat handoff — S2 gate environment (converged + verified) and I10 setup

**Session date:** 2026-08-16
**Plugin repo:** `AnomalyInjector` — HEAD `fbf15b1`, clean, pushed. **Production BYTE-UNCHANGED.**
**Bench repo:** `CaptureBench` — `163dd12`, clean, **local-only (permanent ruling), FROZEN for I10.**
**Audience:** a cold reader picking up mid-S2, immediately before the I10 game-lever legs.

**Read `CHAT-HANDOFF-s2-keying-design.md` first** — it carries the B′ keying design, the
MainMenu discovery, the F4/C3 gate-level route, and amendments A8–A33. This doc carries
A34–A43 and everything that changed since. Amendments are cited by number in verdicts; a fresh
chat must be able to resolve them.

---

## 1. Where we are, in one paragraph

B′ is locked and unchanged. Production still captures via the backbuffer; the client's
1.2-band −1 remains unfixed. The gate environment is **done and verified** — converged,
legible, exposure pinned and proven inert, self-checking, with four leg-validity conditions.
The game-thread stall lever is **calibrated and banked**; the render-thread lever is **dead
(twice) and undiagnosed**. The next action is a **pre-flight check (A43)**, then the **I10
game-lever legs** — the measurement this entire stage has been waiting for.

---

## 2. The state of the instruments

### Gate level — DONE. Stop tuning it.

`CB_GateLevel`, 152 actors, script-authored (`make_gate_level.py`) with an N3 self-check
(asserts asset exists + actor count matches; the exit-255 trap silently left a stale umap
once). Reached by command-line map argument in the BenchGate package (**not deliverable**).

**Pass conditions (A28's 18–25 ms band is RETIRED — see §3.2):**
1. Legible by eye — verified: shapes individually distinguishable, shading on curves,
   shadows against a grid floor. A hide window is plainly resolvable by pixel inspection.
2. Passes all four A31 validity conditions.
3. Deterministic and reproducible from script.
4. Natural cost **RECORDED as a property, not targeted**: **8.52 ms** (W0, capture-only).

**Exposure is PINNED for every gate run:** `r.DefaultFeature.AutoExposure 0` +
`r.EyeAdaptationQuality 0`. Auto-exposure was active by engine default and drifted a
deterministic level ~5.75 luminance levels over 12 frames (A30 Test 1). The pin is
**verified inert**, not merely configured (A30 Test 2, control-run method).

### Leg validity — FOUR conditions, every leg, or the leg is INVALID

1. Achieved `speed_ratio` in its declared band (A6/A40)
2. Luminance floor: mean ≥ 2.0/255 (catches black / did-not-render)
3. Flatness floor: sd ≥ 5.0 (catches uniform/flat)
4. Clip ceiling: ≤35% of pixels ≥250 (catches blown-out; calibrated against the healthy
   level's 9.2%)

All crude, loud, leg-invalidating; none chases image quality. Portability caveat recorded:
they assume a deliberately lit, deliberately varied scene. **A27's visual step remains
required regardless** — a partially blown frame passes all four.

### Game-thread stall table — BANKED (VideoFps 30, 1280×720, exposure pinned)

| stall (ms) | ratio | A40 band |
|---|---|---|
| 0–30 | 1.000 | nominal |
| 34 | 1.239 | **client band** |
| 39 | 1.204 | **client band** |
| 76 / 85 | 2.314 / 2.589 | (gap — kept as data) |
| 99 | 3.004 | **deep** |

- **Knee: 30→34 ms, SHARP.** Concurrency model: `frame_time ≈ stall + 1.3 ms` — the render
  thread runs in parallel, so **scene cost never enters a game-thread stall**. The model
  retroactively fits the old MainMenu table too (60→1.847 vs model 1.84; 105→3.198 vs 3.19).
  Two scenes, one model. Sole outlier: the 34 ms knee-region row — which is why that region
  is binned, not fitted.
- **Mild band (1.02–1.10] is UNCOVERED and stays that way** — its window is inside the
  1–2 ms run-to-run noise. Best-effort per A40; no I10 leg depends on it.

### Render-thread stall lever — DEAD, TWICE, UNDIAGNOSED

First build only ran inside CaptureBench's SVE (bench-capture-gated → never fired under
production capture). Rebuild moved it to an `ENQUEUE_RENDER_COMMAND` from the probe's world
tick — **and it still does not fire: STALL_FIRED = 0 at all four values.**

**The execution counter (A41) caught it on first use.** Without the counter, the evidence
pattern (four identical 1.000s) was exactly the "speed_ratio is blind to render starvation"
major finding — which would have been **wrong** and would have redirected I10. Diagnosis is
deliberately deferred (not same-turn — that is how this project's misdiagnoses happened).
Candidates, in order: does the world-tick delegate itself register and fire under production
capture (log at registration AND at issue); is the counter/log gating the bug rather than the
stall; does the capture path bypass or flush it. Plus one edge case: the stall runs with a
0 ms duration (value not plumbed) while the counter sits inside an `if (ms > 0)` guard. The
arithmetic constraint: at 110 ms a firing stall would force ratio ≈3.3 by the model; ratio
read 1.000 — counter and ratio independently agree it never executed, so "counter broken,
stall firing" should not get equal time.

---

## 3. Decisions this session, with rationale (A34–A43)

### 3.1 A30 Test 2 — passed via CONTROL RUN; two findings came out of it

The first attempt was confounded (auto-pool mixing, shadow leakage) and Code **refused to
convert an unclean test into a pass**. The control run (same seed, same level, same cadence,
zero fires — a *targeted zero-match* run, which gives identical burst cadence with no
positives) settled it: non-event frames agree to 0.3%; event frames diverge 12× more.
**No exposure-driven global shift. The pixel oracle is safe from adaptation.**

- **A35 — shadow leakage is a FINDING, not noise.** Hiding an object removes its cast shadow,
  brightening pixels **outside** its bbox (+1.66 mean at event frames, up to +4.67). Real
  game behaviour, present in every client capture. **Consequence 1, adopted: I10's oracle
  keys on pixels INSIDE the target's bbox**, never whole-frame stats. Consequence 2, journal
  note only: shadow contribution changes how *visible* a hidden object is — directly adjacent
  to the client's #1 complaint; belongs to the stencil workstream, not this stage.
- **A36 — standing rule:** every gate leg states **targeted vs auto-pool, and why**.
  Auto-pool mixing has silently shaped a measurement twice.

### 3.2 The recalibration corrections — one of them was chat-Claude's error

- **Correction 1 (Code's):** the "23.7 ms converged" natural cost was contaminated by PNG
  encode (Write 1). Clean figure: **8.52 ms — the gate level is still lighter than MainMenu.**
  "A28 converged" was withdrawn; the level-weight problem had been *measured away*, not fixed.
- **Correction 2 (chat-Claude's error, on record):** A13/A24's "heavier level → honest knee"
  rationale assumed stall and scene cost are **additive**. They are not — the render thread
  runs concurrently, so a game-thread stall **bypasses the scene entirely**. The knee sits at
  ~32 ms regardless of scene weight. The level-weight chase was spent on a wrong model.
- **Consequence — the 18–25 ms band is RETIRED, not renumbered.** Inventing a replacement
  number would stack a third assumption on two dead ones. The level is judged by the four
  §2 conditions and its cost is recorded, not targeted.
- **Silver lining:** the trip produced the exposure pin, the legibility fix, and the A31
  conditions — all needed regardless.

### 3.3 A40 — legs classified by ACHIEVED ratio into PRE-DECLARED bands

The 1.05 target is unreachable by dialling (sharp knee + noise ≥ window), and a leg that
lands at 1.000 while *labelled* 1.05 is a **silently passing leg**. So: bands were declared
**before** any I10 run — nominal [1.00–1.02], mild (1.02–1.10] best-effort, **client
[1.15–1.35]**, **deep ≥2.80**, pacing-off its own category. A valid leg landing in a gap is
kept as data but fills no requirement. **Required I10 coverage: nominal, client, deep,
pacing-off.** Declaring bands in advance is what stops binning from becoming goalpost-moving.

### 3.4 A42 — I10 SPLITS; the miscall was chat-Claude's

A39 originally held I10 for both levers ("the fix is small, waiting costs a turn"). The fix
is 0-for-2 and undiagnosed; the cost is open-ended. **Revised: game-lever legs run NOW on the
frozen instrument (CaptureBench 163dd12, zero probe edits between calibration and
measurement); render legs remain REQUIRED, deferred.** The freeze is also why the render
diagnosis sequences *after* the game legs — the fix touches the same probe module.

**Conditional priority after I10-game:** defect REPRODUCES → render lever is
important-not-blocking; S3 prep may begin against the game-lever recipe. Game legs CLEAN →
**the render lever becomes the critical path immediately** — her shape may be render-side,
and that is then the only untested hypothesis standing.

An I10-game result licenses only "reproduces (or not) under GAME-THREAD starvation" — never a
mechanism claim.

### 3.5 A43 — PRE-FLIGHT before any leg: does the marker survive PRODUCTION capture?

The marker has **only ever been decoded from CaptureBench's own capturer**. Every I10 leg
decodes it from **production capture** PNGs — and production capture has previously and
deliberately kept a DrawDebug visual out of saved frames (the poll-radius sphere: absent from
captures, visible live). The marker is a DrawDebugSolidBox. If that suppression catches the
family, the oracle is blind in exactly the captures I10 needs, and every prior decode success
is silent on it.

**One production-capture run, marker on, decode it.** Decodes → proceed in the same turn.
Absent → **STOP**; the remedy is a design change (likely a probe-driven mesh actor;
production stays byte-unchanged) and that is a chat-side call.

---

## 4. I10 — the specification, frozen

**Question:** does the client's −1 defect reproduce on the **current backbuffer path** under
calibrated game-thread starvation?

**Legs:** nominal (stall 0) · client band (run BOTH 34 and 39 — knee-region noise, two
in-band legs beat one) · deep (99) · pacing-OFF (Pace 0, stall 0). Classification by achieved
ratio into A40 bands; if both client attempts land in gaps, max 3 extra attempts, then stop
and report rather than grind.

**Method:** targeted, single-anomaly, hide-type (blink) on a named CB_ target (A36/A37 —
state targeted and why). Oracle keys **inside** the target bbox (A35). Marker on every frame
(and marker decode is NOT evidence the scene rendered — A31 covers that). All four validity
conditions per leg. Enough bursts that the boundary comparison rests on **≥3 hide events**
per leg. Signature per event: the frame index where the target disappears **in the pixels**
vs the index **annotation.json claims**. A32 conditions recorded in the same report.

**PREDICTIONS — restate VERBATIM with this band mapping before showing any result:**

> nominal band: clean · CLIENT band: shows the −1 · DEEP band: −1 or worse ·
> pacing-OFF: shows the −1 · **if the CLIENT band is CLEAN, that is a major result meaning
> her defect has a different mechanism — reported as such, not as a partial failure.**

Render legs are **UNPREDICTED, now and forever** — a prediction written after seeing the
game-lever results is worthless. State that plainly when they eventually run.

**Why I10 cannot really disappoint:** reproduces → we have the recipe and S3 proves the fix
against it. Clean → we've learned her defect has a different mechanism *before* building the
fix on the wrong theory. Both outcomes move the project. The only bad outcome was trusting a
broken rig — which is what the last four turns eliminated.

---

## 5. Open vs locked

### Locked (do not relitigate)
- B′; key minted at `BeginRenderViewFamily` only; A8 latch rule; A3 never-assert.
- Stage renumbering: S3 = B′ behind default-OFF switch + gates on the real paced path;
  S4 = depth; S5 = backbuffer demotion.
- Gate level DONE under §2 conditions — stop tuning. Exposure pin cvars. A31 thresholds +
  the 9.2% reference. A40 bands. A35 oracle scope. A36 targeted/auto-pool rule.
- A41: any stall lever carries an execution counter, reported per run.
- CaptureBench local-only, FROZEN at 163dd12 until the I10 game legs are done.
- `feature/stencil-capture`: do not rebase; mine Stage 3a on current master later.

### Open
- **I10 game legs — next action after the A43 pre-flight.**
- **Render lever dead, undiagnosed** (twice). Priority is conditional per A42/§3.4.
- **A4 Condition 1 (VP equality) UNSATISFIED** — ViewRing/ViewLagFrames deletion contingent
  on it; keep a bisect switch when it lands.
- A11: one clean non-zero ring-counter observation. Mild band uncovered (accepted).
- Deep starvation still open — uninformative for three independent reasons; **it does not
  drift toward "probably fine."** H1 (GPU-load shape) untested; the render lever is the only
  lever where scene weight matters at all, which *raises* H1's eventual priority.
- A17/A19 retroactive audit — paper only, both axes (menu-bound? content verified?).
  Blocks nothing; sequenced after I10-game.
- I2 re-measure in the gate level.
- Client-facing (all pre-dating this session): reply unsent; invisible-anomaly fix unscoped;
  resolution/JPEG/defaults unbuilt.

---

## 6. Corrections — discard this stale understanding

- **"Gate level converged at 23.7 ms inside the 18–25 band"** → WITHDRAWN. Encode-contaminated.
  True cost 8.52 ms; the band itself is retired.
- **"A heavier gate level makes the stall table honest"** → WRONG (chat-Claude's model). A
  game-thread stall bypasses the scene; the knee is scene-independent.
- **"The knee sits at budget − natural cost"** → wrong for the same reason. It sits at
  ~32 ms at VideoFps 30 regardless of scene.
- **"The render lever works"** → it has never fired. Twice built, zero executions, undiagnosed.
- **"Marker decode success in prior runs covers I10"** → NO. All prior decodes were through
  CaptureBench's capturer; I10 decodes through production capture. That is A43.
- **"ratio 1.05 is a reachable leg"** → not by dialling; the band is best-effort (A40).
- **"The overexposure was auto-exposure"** → refuted; it was light intensity. (Auto-exposure
  was a *different*, real problem: determinism.)
- **"Shadow effects in Test 2 are noise"** → reframed as A35, a permanent property of what
  I10 measures.

---

## 7. Debts — none silently dropped

- **A20 item 4** — pre-cook `GameDefaultMap` check in the client delivery checklist.
  Unconditional; anyone reading G88 will reach for the project-config workaround.
- **G89 update** — now owes: both recalibration corrections; the concurrency model; the
  retired 18–25 band and why a light level is acceptable; knee steepness + mild-band
  unreachability; the A40 bands; the render-lever defect ×2 + execution-counter rule + the
  counter-catch incident; the Test 2 control method; the A35 quantification + oracle-scope
  ruling; the A36 rule; the A42 revision; the A43 result whichever way it lands; the
  old-table cross-check.
- `CHAT-HANDOFF-m10-m21.md` still absent from the repo.

---

## 8. Pointers

Plugin repo: `CLAUDE.md`, `docs/gotchas.md` (G87–G89), journals 028/029,
`docs/CHAT-HANDOFF-s2-keying-design.md` (the companion to this doc),
`docs/CHAT-HANDOFF-m22-and-sve-s1.md`. CaptureBench (local-only): `tools/make_gate_level.py`,
the probe sources, the marker decoder.

**Standing lessons, now with receipts:** measure then design (two chat-Claude models died to
measurement this session). Eyes, then number, then benchmark. A test that never summoned its
subject proves nothing — and the A41 counter is what catches it *on the way in*. Marker
decode ≠ scene rendered. Both sides put their own errors on the record; the corrections
sections are where a cold reader should look first.
