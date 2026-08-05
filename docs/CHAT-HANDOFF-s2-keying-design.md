# Chat handoff — S2 (render-thread keying design) + the gate-level saga

**Session date:** 2026-08-06
**Plugin repo:** `AnomalyInjector` (GitHub: AnomInject) — HEAD `2365808`, clean, pushed
**Bench repo:** `CaptureBench` — `10267cb` + uncommitted work, **local-only, no remote**
**Audience:** a cold reader — fresh chat, Claude Code, or a collaborator.

Production `AnomalyInjector` / `AnomalyCapture` are **BYTE-UNCHANGED** through this entire
session. Production still captures via the backbuffer. No S3 work has started.

This doc carries what is *not* in the repo. For operational detail read the docs under
**Pointers**.

---

## 1. What this session was for

S2 of the SVE capture migration: design the render-thread frame↔state keying model. Today
labels key to a game-thread `GFrameCounter` at arm time; an SVE has no arming step and runs on
the render thread, so the mapping from "this rendered frame" back to "the game-thread anomaly
state snapshot that produced it" had to be **designed, not ported**.

The session split into two halves. The first half designed and measured the keying model and
succeeded. The second half discovered that **every packaged validation this project has ever
run was in a menu**, and has been building a trustworthy gate environment ever since.

---

## 2. Current state

### The keying design — B′, LOCKED

**B′ = frame-number-keyed ring, key-only.** The game thread publishes at
`BeginRenderViewFamily`; the render-thread pass looks up by `View.Family->FrameNumber`.

Key properties, all deliberate:

- **Only the identifier crosses threads.** The label snapshot never leaves the game thread.
  This was a refinement of Code's proposed Option B, forced by the D1 finding (below).
- **Matching stays where it is today** — `ProcessCompletedFrames` on the game thread, same
  shape as the current backbuffer path.
- **A lookup miss is loud.** Counted, warned, frame dropped — never written with a guessed
  label. Silent failure was the disqualifying property for every rejected option.
- Options A (payload on the view family) and D (arm-style, render-thread ordered) were
  rejected: A has no sanctioned payload slot on stock 5.1; D still pairs by *order*, not
  identity, and fails silently by construction.

### What was measured and settled

| Finding | Status |
|---|---|
| `BeginRenderViewFamily` runs on the game thread AFTER `FrameNumber` is assigned | MEASURED — the design rests on this |
| `SetupViewFamily` returns `FrameNumber` = UINT_MAX (not yet assigned) | MEASURED — near-miss, see §3.1 |
| Scene frame number and `GFrameCounter` advance 1:1 (constant +2 startup offset) | MEASURED, free-run only — on the paced re-run list |
| Key ring round-trips 100% under game-thread and render-thread stall | MEASURED, free-run only — on the paced re-run list |
| Loud-failure detection actually detects (26/26 forced misses) | MEASURED |
| TAA jitter present at the capture pass, sub-pixel (max ~0.375 px); `GetProjectionNoAAMatrix()` exists | MEASURED + READ — use the NoAA matrix |
| Marker oracle decodes 100% at both grab points | MEASURED |
| DrawDebug* line-batcher primitives **DO** survive into SceneColor | MEASURED |
| Canvas/UMG overlays are **ABSENT** from SceneColor | MEASURED — see §3.2 |
| Deterministic stall→ratio lever exists; ratio ≥3 reached for the first time | MEASURED, **but scene-bound — see §3.3** |

### The gate environment — in progress, blocking everything downstream

- `CB_GateLevel` exists at `/Game/CaptureBenchGate/CB_GateLevel`, authored **headlessly by
  script** (`Plugins/CaptureBench/tools/make_gate_level.py`) so it is reproducible.
- Reached via **command-line map argument** in a bench-only package at
  `Builds\BenchGate`. **This package is NOT deliverable** — it contains a synthetic map and a
  restricted cook. The client artifact remains `Builds\Windows`.
- **Two open problems:** the level is **overexposed** (blown white, targets barely legible)
  and **too heavy** (29.5 ms natural cost vs the 18–25 ms target).
- **Uncommitted in CaptureBench:** the A25 luminance floor (working, live-tested twice) and
  the re-author script. **No remote on that repo — commit first thing next session.**

---

## 3. Decisions made this session, with rationale

### 3.1 The measure-then-design rule paid for itself twice

**The `SetupViewFamily` near-miss.** There is an obvious-looking earlier game-thread hook. Its
`FrameNumber` is UINT_MAX — not yet assigned. Picking it on intuition would have keyed every
label to a constant, silently. Nothing about it looks wrong until a client reports garbage.

**The gate level rendered nothing.** A script-authored level was never opened in an editor, so
lighting was never built; static-mobility meshes with unbuilt lighting render black, and there
was no sky actor, so even the background was zero. It benchmarked at 7.07 ms and that number
was reported as "the level is lighter than the menu." It was measuring a scene drawing
**nothing**. Withdrawn.

Both were caught by measuring rather than reasoning. The second was caught because **the owner
looked at the screen** — see §3.6.

### 3.2 The UI-off premise is now a picture, not an argument

Comparing the same frame at both grab points: the backbuffer capture contains the UMG menu and
the Canvas debug text; the SceneColor capture contains neither. This was free evidence from
files already on disk.

Consequence — **G54 splits cleanly**:
- **Canvas/HUD-family suppression becomes UNNECESSARY** under an SVE.
- **DrawDebug-family suppression remains REQUIRED** (line-batcher primitives are scene
  geometry — the marker itself is one and it appears in SceneColor).

This also independently validates reason 2 of the migration (client wants UI excluded).

### 3.3 MainMenu — the discovery that reshaped the session

**Packaged runs always boot MainMenu, and the redirect is ACTIVE.** A command-line map override
is honoured and then the title immediately re-opens MainMenu. A loose `Config` file beside a
package is **silently ignored** (the cooked config inside the pak wins).

**So MainMenu was not a near-miss — it was the only level any packaged validation could ever
have run in.** That includes the m22 gates, the S1 packaged matrix, and B3'.

Consequence: **the pending A17/A19 audit defaults every packaged result to WEAKENED**, and each
must argue its way out. Structural findings will survive; scene-dependent ones will not.

Also confirmed scene-dependent: the I5 calibration table (measured in a near-idle menu), the
`views=1` finding, and — flagged by Code unprompted — its own deep-starvation non-reproduction.

### 3.4 F4 (bench gate level) chosen over three cheaper options

- **F1 (drive the PLAY button)** — highest fidelity, but couples the whole gate matrix to one
  sample project's UI blueprint. We are not validating StackOBot; it is a stand-in.
- **F2 (re-cook with a different default map)** — full cook per iteration, and MainWorld is an
  uncontrolled scene we do not own.
- **F3 (PIE/Standalone)** — **rejected precisely because it was cheapest.** G76 exists because
  the editor has masked packaged-only behaviour before. Taking a G76 exception on the legs that
  measure ratio behaviour would reintroduce the S1 "is this even the same regime?" doubt a third
  time.
- **F4** — gives the pinned, stable, **owned** gate scene A13 demanded independently.

**The honest cost of F4:** the gate level is synthetic and will not exercise everything a real
title does. Accepted, because the gate measures **frame↔label alignment across ratio regimes**,
which is a property of the capture path, not of scene content. **Where scene content matters —
the invisible-anomaly / selection-quality work — a synthetic level is the wrong instrument.**
Do not mistake the gate level for a general validation environment.

The route that worked was **C3**: cook the gate map into a package alongside MainMenu and reach
it by command-line argument. **No host-project config was ever touched.** The redirect was
MainWorld-specific; a level with no blueprint logic has nothing to bounce it. Verified by
6,443/6,443 ticks in `CB_GateLevel` with zero MainMenu loads — the strict criterion, not "it
loaded."

### 3.5 Sequencing inverted — reproduction before characterisation

Original plan: characterise deep starvation (I4), then reproduce the client's defect (I10).
**Code flagged the dependency runs the other way and was right** — I4 could not characterise a
failure it could not summon. The starvation test came back clean because **the bug never
happened**, to either grab point.

That result is **uninformative for three independent reasons**: the bench free-runs (wrong
regime); the stall shape was CPU-only and game-thread (wrong shape); and the scene was a
near-idle menu with almost no GPU work (wrong content). **It does not drift toward "probably
fine."**

### 3.6 Two owner observations overturned Code measurements

Both were visible on screen while the logs looked fine:

1. "The game always stays in the main menu" → §3.3, the whole retroactive audit.
2. "The game screen was completely black" → §3.1, the withdrawn 7.07 ms benchmark.

**The systemic response** is the A25 luminance floor: a machine check for "did this frame
render anything," computed at drain time, failing loudly and invalidating the leg. Built
*before* the level fix it guards, deliberately.

### 3.7 A25 passed a useless frame — correctly — and that is the point

The overexposed level scored luminance 188.6 and passed. That is A25 behaving to spec: it
separates *rendered* from *did not render*, nothing more. **The visual inspection step is
therefore NOT redundant with the numeric one**, and this is the proof. A31 (a flatness/variance
floor) will close the blown-white half; eyes remain required regardless.

Related and permanent: **marker decode success is NOT evidence the scene rendered.** The
decoder thresholds cells relative to its own reference cells, so it decodes perfectly on a
black frame. A gate leg could pass its frame-identity check on an empty picture.

### 3.8 Overexposure is a correctness problem, not a cosmetic one

I10's oracle is "at which frame does the object visibly disappear in the pixels." That requires
targets **visually distinguishable from their background**. And the client's number-one
complaint is labelled-but-invisible anomalies — a gate level where anomalies are hard to see is
the wrong instrument for that project.

Pass condition for the level is therefore **both**: 18–25 ms natural cost **and** targets
clearly legible by eye.

### 3.9 Auto-exposure is a prerequisite, not a parallel check

Config is silent at both layers, so the engine default applies — auto-exposure **on**
(READ, not yet MEASURED; H3 is likely but unconfirmed).

**Why it blocks light tuning:** auto-exposure makes tuning a feedback loop. Raise an intensity,
adaptation compensates, the image looks similar. Values tuned against a moving target are not
values. **Pin exposure before changing a single light value.**

Two concerns, and the second is the dangerous one:
- **Determinism** — adaptation adapts over frames, so "deterministic" runs may not be.
- **Oracle contamination** — hiding a large bright object may trigger global re-adaptation,
  which a pixel oracle reads as a frame-wide change at exactly the injection moment. This
  directly threatens I10.

Concern 2 needs its **own** test, run after pinning, to verify the pin solved it.

---

## 4. Amendment index (A8–A33)

Standing rulings from this session, by number, so they can be cited rather than re-derived.

| # | Ruling |
|---|---|
| A8 | Latch lifetime rule — the latch stores the `GFrameCounter` it was set in; consumed only on match. Identity all the way through; no positional step survives anywhere |
| A9 | `VideoFps=30` pinned across the S3 matrix; the calibration table is fps-bound; never scale it arithmetically |
| A10 | I1's counter relationship inherits I3's free-run limitation — on the paced re-run list |
| A11 | One clean observation of non-zero ring counters still owed |
| A12/A16 | Level question settled from artifacts; free G54 evidence from existing files |
| A13 | Calibration table is scene-bound; pin the gate level like fps is pinned |
| A14/H1 | GPU-load starvation shape — **untested hypothesis**, not written into any design |
| A15/A26 | Scene-dependence re-audit; second axis added — "did anyone verify the frames had content?" |
| A17/A19 | Retroactive audit, paper only, default classification **WEAKENED** |
| A20 | Mechanical revert protocol for host-config edits; **item 4 (pre-cook GameDefaultMap check in the delivery checklist) is unconditional and still owed** |
| A21 | Bench package output separated and named so it cannot be confused with the client artifact |
| A22–A24 | Verify rendering before re-authoring; load target 18–25 ms; cost from rendering work, not actor count |
| A25 | Luminance floor — required, loud, leg-invalidating. Leg validity is now **two** conditions (ratio band + luminance) |
| A27 | **Eyes, then number, then benchmark.** In that order |
| A28 | Converge the level empirically, ≤5 attempts, report the sweep |
| A29 | Overexposure is a correctness problem |
| A30/H3 | Auto-exposure — likely on, unconfirmed; two tests needed |
| A31 | Flatness/variance floor beside the luminance floor |
| A32 | Pin and record scene / fps / **resolution** / machine with the table; it is a dev-box instrument, not a portable spec |
| A33 | Auto-exposure is a **prerequisite** for light tuning, not a parallel check |

---

## 5. Forward plan

**Immediate — finish the gate environment.** Everything downstream is defined as "in the gate
level," so nothing else can honestly run.

1. Commit the uncommitted CaptureBench work (local-only repo, no remote).
2. **A30** — settle auto-exposure. Test 1: is adaptation active? Test 2 (after pinning): does
   hiding a bright object shift whole-frame luminance? If test 2 fails even with exposure
   pinned, **stop and report** — the oracle needs rethinking.
3. **A31** flatness floor, **N3** script self-check (assert asset exists, log actor count vs
   expected — the exit-255 trap presented as success while leaving the old umap in place).
4. **A28** converge the level: 18–25 ms **and** legible. Warm cook is 1.3 min, so iterate.
5. **A32** record conditions, then the stall→ratio calibration in the converged level.
6. **A17/A19** retroactive audit, both axes, paper only.
7. **I2** re-measure in the gate level.
8. **I10** — reproduce the client's defect on the **current backbuffer path** at the
   recalibrated stalls, predictions restated **verbatim** before results. Then high-fps legs
   (120/240, pacing ON — deliberately breaks A9 for the specific purpose of reproducing a known
   historical failure). Then A14's GPU-load leg.

**Then S3 onward.** Note the stages were renumbered this session:

- **S3** — land B′ in `AnomalyCapture` behind a default-OFF switch, **colour only**. Run the
  full ratio × config matrix on the real paced path. **This is where ratio-independence is
  actually discharged.**
- **S4** — depth (FP32 SceneDepth) + the typed FP16/FP32 path, on a keying model already green.
- **S5** — demote the backbuffer path to the UI-on option, flip defaults, client config.

Rationale for the reorder: depth is a feature, gates are the requirement. A feature must not
ride on an unvalidated keying model.

**Unblocked and independent of all of the above:** the client reply (drafted in substance last
session, still unsent), the environmental/invisible-object selection fix, resolution selection
+ JPEG + defaults profile.

---

## 6. Open vs locked

### Locked

- **B′** as the keying design; key-only ring; snapshot never crosses threads; all three
  state-fill points stay put.
- Key minted at `BeginRenderViewFamily` and **nowhere else**.
- A8 latch lifetime rule — identity matching throughout.
- Never assert on a client-reachable path (A3); log and continue.
- Stage renumbering: S3 = gates on the real path, S4 = depth, S5 = backbuffer demotion.
- F4 gate level; C3 route; bench package non-deliverable.
- CaptureBench stays **local-only, permanently**. Settled, do not re-raise.
- `feature/stencil-capture`: **do not rebase.** Merge-base pre-m9, 34 commits of divergence.
  Mine its Stage 3a SVE mask pass for the pattern and re-implement on current master.
- `IAnomaly` LOCKED. Ratio-independence required. Ship rule is telemetry only.

### Open

- **A4 Condition 1 — the VP equality check is UNSATISFIED.** ViewRing / `ViewLagFrames`
  deletion is contingent on it. Nothing gets deleted until it passes, and keep a bisect switch
  when it does.
- **Deep starvation** — open, and uninformative for three independent reasons (§3.5).
- **H1** (GPU-load starvation shape) — untested hypothesis.
- **H3** (auto-exposure active) — likely, unconfirmed.
- Whether the client's 1.2-band −1 actually resolves under the SVE — unmeasurable until the
  S3 gates.
- Gate level convergence — overexposed and too heavy.
- Everything client-facing from last session: reply unsent, invisible-anomaly fix unscoped,
  resolution/JPEG/defaults unbuilt.

---

## 7. Corrections — discard this stale understanding

- **"S3 = depth"** → renumbered. S3 is now the real-path gates; depth moved to S4.
- **"The packaged tests ran in a gameplay level"** → **false.** Every packaged validation this
  project has ever run was in MainMenu, and there was no way out.
- **"CB_GateLevel natural cost is 7.07 ms / lighter than the menu"** → **withdrawn.** It was
  measuring a scene rendering nothing.
- **"The SVE makes overlay suppression moot"** → **half true, now precise.** Canvas/HUD yes,
  DrawDebug* no.
- **"SVE has a readback-latency advantage"** (C1) and **"SVE captures at ViewRect so screen
  percentage desyncs resolution"** (C2) → both remain **false**, from last session.
- **"CaptureBench needs a remote"** → owner ruling, local-only, settled.
- **"Marker decode success means the capture is good"** → **no.** The decoder is
  self-referential and decodes on a black frame.
- **"A25 passing means the frame is usable"** → **no.** It separates rendered from not-rendered
  only. A25 passed a blown-white frame, correctly.

### New gotchas landed this session

**G87** — packaged runs always boot MainMenu; **MainMenu looks like gameplay** (that is the
part that cost us, not the config line); the redirect is active, not a startup artifact.
**G88** — a loose `Config` file beside a package is ignored; the cooked config in the pak wins;
**it fails silently.**
**G89** — the gate level: how it is rebuilt, what is in it, how it is reached, and its scope
limit. Pending updates listed in §8.

Two 5.1 Python API traps, recorded: `new_level` refuses when the asset exists **and leaves the
old umap in place** (a silent no-op that presents as success); lights are accessed via
`.light_component`, not the typed accessors.

---

## 8. Debts carried — none silently dropped

- **A20 item 4** — pre-cook `GameDefaultMap` check in the client delivery checklist.
  Unconditional. The hazard exists regardless of which route we took, because anyone reading
  G88 will reach for the project-config workaround and nothing in git would catch it.
- **G89 update** — C3 success + command-line map invocation; cook times (warm = 1.3 min); the
  black-level incident; the dynamic-lighting requirement; both Python API traps; the
  overexposure incident; the A25-passed-a-useless-frame note beside the detector's design
  description; the auto-exposure finding once settled.
- **Uncommitted CaptureBench work** — A25 luminance floor + re-author script. Local-only repo.
- `CHAT-HANDOFF-m10-m21.md` still absent from the repo; `CHAT-HANDOFF-m22-and-sve-s1.md`
  landed at `f6e16fe`.

---

## 9. Pointers

In the plugin repo: `CLAUDE.md` (Current-status), `docs/gotchas.md` (G87–G89 new; G43, G54,
G76, G80, G86 relevant), `docs/sessions/2026-07-29-028-*` (S1 evaluation) and `029-*` (m22),
`docs/capture-fps.md` (ship-rule framing superseded), `docs/architecture.md`,
`docs/client-delivery.md`, `docs/CHAT-HANDOFF-m22-and-sve-s1.md`.

In CaptureBench (local-only): `tools/make_gate_level.py`, the SVE probe sources, the marker
implementation and its Python decoder.

**Standing lesson, unchanged and reinforced twice this session:** chat Claude must not write a
confident mechanism into a brief without Code's measurement. And the corollary this session
added — **a benchmark on unverified content is worthless.** Eyes, then number, then benchmark.
