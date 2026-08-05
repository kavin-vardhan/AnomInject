# Chat handoff — m22 (client feedback fixes) + SVE capture-method S1

**Session date:** 2026-07-29
**Plugin repo:** `AnomalyInjector` (GitHub: AnomInject)
**Branch:** master · `b210a52` → `ac02bb3`
**Tag at close:** m22 (applied by Code after the owner's Play-gate smoke)
**Audience:** a cold reader — fresh chat, Claude Code, or a collaborator.

This doc carries what is *not* in the repo. For operational detail read the repo docs
listed under **Pointers** at the end.

---

## 1. Why this session happened

The external client returned **feedback on the first delivered build**. That feedback drove
everything here. Her report, condensed:

**Positives (bank these — they validate the core contract):** setup and README were easy;
the dashboard was intuitive; no performance problems on a 4090; and critically —
*when an anomaly is clearly visible, the corresponding annotation is findable and the class
is generally correct.* That is the labeling contract validated by an outside party.

**Problems, in her words:**
1. Some anomalies were **almost invisible or not visible at all** — annotations present, no
   visible change on screen.
2. Objects in annotations are named `StaticMeshActor_###`, so **she cannot identify which
   object was hit**.
3. A **frame-boundary lag**: a `missing_object` effect began at frame 50, annotation said 51.
4. The **`flicker` subtype was confusing** — she expected repeated appear/disappear to be
   called "flickering", and found the type/subtype split unhelpful.
5. Feature requests: **output-resolution selection**, **PNG → JPEG** for storage, and an
   **agreed default settings profile**.
6. One low-priority note about a two-monitor requirement (she flagged it as unlikely to matter).

---

## 2. Current state

### Shipped this session — m22 (4 commits, tagged, pushed)

| Commit | Content |
|---|---|
| `af8d937` | `feat(blinking)` — subtype pin + G81 frame_count fix |
| `03a51d5` | `feat(capture)` — annotation traceability (asset name, class, bounds) |
| `28bc6f1` | `feat(capture)` — selection provenance (coverage, occlusion, distance) |
| `ac02bb3` | `docs(m22)` — journal 029 + Current-status refresh + m22 number reassignment |

Plus two earlier docs commits in the same session: `89b8240` (CLAUDE.md Current-status
refresh) and `b210a52` (journal 028 + G86).

**What m22 actually changes:**

- **Subtype pin.** The per-event `anomaly_subtype` *derivation* is deleted. `blink` now always
  emits `disappear_reappear`, however many toggles the event contains. The value `flicker`
  leaves the blink family entirely and is reserved for the future `flickering` class. The
  `anomaly_subtype` **field is retained** in the schema.
- **G81 fixed.** `frame_count` was a *span* (e.g. 7 for 4 real indices); it is now
  `len(frame_indices)` — a true count. The span remains recoverable from
  `start_frame`/`end_frame`. Scope guard held: `frame_count` lives only in
  `WriteSessionAnnotation`, so the m18-validated labels/range-builder path was **not touched**
  and the parked shared-range-builder refactor stays parked.
- **B1 traceability.** `nodes[]` gains `asset_name`, `component_class`, and
  `bounds{origin,extent}`. Verified in-package: `StaticMeshActor_0` now also reports
  `SM_SlopeWarpLandscape` / `StaticMeshComponent`. This closes the client's identification gap.
  Bounds are sampled at the **anchor frame only** — documented in `architecture.md`, and *not*
  per-frame truth for moving actors.
- **B2 selection provenance.** `coverage_pct` → `annotation.json` (client-visible, both modes);
  occlusion samples passed/total + poll distance → `selection_provenance.json`
  (internal, delivery-suppressed).

**Gates, all green on a local package (game-target build + exe hot-swap, G76):** subtype pinned
on the two previously-"flicker" events; `frame_count == len(frame_indices)` 10/10; non-blink
subtypes unchanged; m20 trailing reappear frame still present; B1 fields populated; **seeded
selection identity byte-identical across two runs at seed 4242**; delivery-mode suppression
correct.

### Two client-facing value changes (must appear in client notes)

Both change to the *expected* value, in files she already receives:

- `affected_frames.frame_count` — span → true count
- `blink anomaly_subtype` — `"flicker"` → `"disappear_reappear"`

### Not shipped, deliberately

- **SVE capture migration** — evaluated and approved, **not implemented**. See §4.
- Client-facing feature requests (resolution selection, JPEG, defaults profile) — not started.
- The environmental/invisible-object selection fix — not started; this is the client's
  highest-value open complaint.

---

## 3. Decisions made this session, with rationale

These are the "why"s a fresh session must not relitigate.

### 3.1 Ratio-independence is now a load-bearing requirement (owner, firm)

The previous **ship rule** — `speed_ratio ≤1.05` is safe to deliver, `≳1.1` do not ship — was
an internal correctness gate. The client captures **on her own hardware**; her session ran at
**1.2** and carried the −1 lag.

**Owner ruling: we do not tell clients their machine is too slow or that their data must be
discarded.** Label correctness must hold at **any** ratio.

Consequences:
- The ship rule is **demoted** from correctness gate to internal telemetry, and retires entirely
  when the SVE migration lands.
- A previously-proposed feature — surfacing a trust flag / live ratio indicator to the client —
  was **withdrawn**. It made the client manage *our* limitation.
- The m21 fix is ratio-dependent by construction, so this requirement is only genuinely
  discharged by the migration (§4).

### 3.2 Blink behaviour does NOT change — only the label

Initially over-scoped by chat Claude into a behaviour change plus an investigation. **Corrected
by the owner:** blink behaving as a multi-toggle within a capture window is *correct*, not a
defect. Only the subtype label was wrong.

Root-cause chain that was considered and rejected as a behaviour fix: default blink genuinely
multi-toggles (observed hidden indices `[4,5,9,10]` = vanish, return, vanish, return), so the
derivation labeling it `flicker` was *correct*. The complaint is vocabulary, not logic.

Because behaviour is unchanged, **gapped events persist**, so G81 did *not* self-resolve and was
fixed explicitly instead (§2).

### 3.3 `anomaly_subtype` field retained (Option A over Option B)

Two options were put to the owner: (A) keep the field, pin the value; (B) drop the field until
there is more than one subtype. **Owner chose A** — smaller change to what the client already
receives, leaves a slot for the future `flickering` class, avoids churning the schema twice.

### 3.4 The taxonomy renegotiation with the client is RETIRED

An earlier plan was to propose a flattened taxonomy (three flat types, no subtypes) for her
sign-off. **No longer needed.** Pinning the value makes `blink` mean what she already expected.
We are not asking her to accept new vocabulary.

**Comms note that still stands:** she reads repeated appear/disappear as "flickering". When we
reply, state plainly that `blink` covers object disappearance (single *or* repeated), and
`flickering` will be a *separate* class for lighting / scene-region effects. Otherwise she hits
the same confusion twice.

### 3.5 Push rule changed (supersedes the older rule)

**Code now runs commits AND pushes, including tags.** The old "owner owns any remote push" rule
is retired — it added a round trip and nothing else. Code still *reports* `git status` +
`git log origin/master..master` before pushing, **for the record, not for approval**
(per-track hygiene, G43). Never force-push on rejection — flag to the owner. Auth stays with
the owner.

**Distinct and still owner-owned:** the **Play-gate smoke test** before tagging. That hold is
the smoke gate, not push permission.

### 3.6 Accepted deviations from the approved B1/B2 plan

- **D1 — B2 built as a standalone evaluator**, not an opt-in out-param threaded through
  `ClassifyRenderableVisibleLive`. **Accepted, and better than specified.** The threaded version
  made "observational only" true by *discipline* (a null-default param a future caller could
  misuse); the standalone version makes it true by *construction* — zero selection-path edits,
  proven by diff (pure insertions, 0 lines removed). Cost also dropped to ~9 traces per fired
  event instead of per-candidate-per-poll.
  **The literal ON/OFF A/B gate is formally WITHDRAWN** and replaced by (a) structural diff proof
  and (b) same-seed selection-identity determinism. A cold reader should not think a gate was
  skipped. **Do not add the runtime toggle back** — it would restore the misuse risk the
  redesign removed.
- **D2 — internal provenance goes to a `selection_provenance.json` sidecar**, not `run.json`.
  **Accepted.** `run.json` is written at `StartRun`, before any event exists; per-event data
  cannot live there without changing its write timing — real risk for a cosmetic preference.
  The sidecar is delivery-suppressed, which was the actual requirement.

### 3.7 m22 number reassigned

The old **m22 proposal (deep-starvation scene-identity SVE marker) is dead** — superseded by the
SVE migration, and it was never tagged. The number is reused for this milestone. CLAUDE.md now
refers to the dead proposal **by name, not by number**.

### 3.8 Standing convention added

The **CLAUDE.md Current-status block is refreshed at every milestone close**, same discipline as
session journals. Added because the block had gone stale (it claimed m21 was uncommitted when
m21 was tagged and an ancestor of HEAD) — the second time stale docs caused a real miss.

---

## 4. The SVE capture migration — approved, S1 complete, NOT implemented

This is the largest forward item and the most important thing for a cold reader to understand
correctly.

### What it is

Replace the capture grab point: **`OnBackBufferReadyToPresent` → a SceneViewExtension (SVE)
post-processing pass**. Evaluated in a throwaway `CaptureBench` plugin. **Production
`AnomalyInjector` / `AnomalyCapture` are byte-unchanged.**

### Why we're doing it — three reasons, in order of weight

1. **It structurally eliminates the m18/m20/m21/m22 bug class.** The SVE reads the frame *while
   it is being rendered*. There is no "arm now, consume whatever present comes next" step — and
   that step is the mechanism behind the entire frame-alignment bug family, **including the −1
   the client hit at ratio 1.2**. This is the reason we committed.
2. **The client wants UI excluded from captures.** SVE grabs SceneColor *before* the Slate UI
   composite, so it is inherently UI-free. Code verified in engine source that **SDR builds have
   no isolated UI layer** (`GetCompositeUIRenderTarget()` is HDR-gated), so UI isolation is
   impossible — the toggle must be a **choice of grab point** (SVE = UI off, backbuffer = UI on).
3. **Performance**, which is real but is *not* the reason.

### What S1 measured (10 rows: standalone, packaged, and starve legs)

- **Hook cost: 80–173× cheaper, and flat.** SVE max never exceeded 0.31 ms in any run.
  Backbuffer spiked to 7.3 / 7.4 / 11.3 / **18.2 ms**. At 105 fps the whole frame budget is
  9.5 ms, so a 7.4 ms spike eats ~78% of a frame; 18.2 ms is longer than an entire 60 fps frame.
  Cause is structural: the backbuffer path calls `EnqueueCopy` on the **immediate** command list
  inside the present hook; the SVE appends a **deferred** pass to the render graph.
- **100% coverage, zero gaps, zero stale-content duplicates in all 10 rows**, both methods,
  14.6 → 108 fps. **Neither hook drops or repeats frames.** Important reframe: any frame gaps
  seen in production are an **arming-cadence** problem, not a capture-method problem.
- **Throughput is a tie** (backbuffer marginally ahead in two of three). *State this precisely —
  overclaiming here would be easy and wrong.*
- **Inline PNG encode is the throughput ceiling** (W1 46–55 fps vs W0 102–105 fps at 720p).
  Validates the existing host-ffmpeg offload. Never inline-encode per frame.

### ⚠️ The limitation that bounds all of the above — do not lose this

**CaptureBench free-runs.** It has no fixed timestep and no m11 pacer. Production's `speed_ratio`
regime is defined by the pacer + fixed timestep interacting with wall clock. A2's "starvation" is
frame-time overrun of a *free-running* game — **not that regime**.

**Therefore: the client's 1.2-band −1 lag can be neither reproduced nor refuted by this harness.
The 10 green rows must NEVER be cited as evidence that the 1.2 band is fine.** That question
belongs to the S2/S4 gates driven through the **production** capture path with pacing engaged.

Also honest: the ≥3× starve target was **not reached** (2.05× max) — `r.setres` is silently
clamped to desktop resolution.

### Corrections Code made to its own earlier reporting (both accepted)

- **C1:** the PIE-observed "SVE has 0-frame readback latency vs backbuffer's 1" was a **PIE
  artifact**. At real frame rates both sit at 1.73–2.00 frames. **There is no readback-depth
  advantage.**
- **C2:** the earlier warning that "SVE captures at `SceneColor.ViewRect` so screen percentage
  desyncs it from output resolution" is **measured false** — at SP170 and SP320 the SVE frame
  stayed exactly 1920×1080. The grab point sits **after** the upsample, i.e. at output
  resolution. **That migration caveat is withdrawn** — which also de-risks the client's
  resolution-selection request.

### Answered design questions (locked)

- **Q1 — UI on/off ships as grab-point choice** (SVE = off, backbuffer = on). The backbuffer path
  is **kept**, not retired, because it is the only way to offer UI-on in SDR. This **amends the
  previously-locked ground-truth contract** ("game UI in"): UI presence becomes **per-run
  config**, proposed delivered default **UI-off**, pending client sign-off.
- **Q2 — 8-bit delivered color stands.** The typed FP16/FP32 path lands *with* depth at S3.
  (Source is 10-bit `PF_A2B10G10R10`, truncated to 8-bit BGRA today.)
- **Q3 — migration opens now**, and supersedes the dead m22 scene-identity-marker proposal.
- **Q4 — CaptureBench is kept permanently** as a non-shipping perf-regression harness.
- **Not repeating** the single-observation packaged W1 backbuffer collapse (13.70 vs 40.28 fps) —
  production never inline-encodes, so it is a bench artifact, not a decision input.
- **Anomaly-arg passthrough on `IAI.Capture.Start` deferred to S2**, scoped to the actual gate.
  It was justified only by exercising the `disappear_reappear` branch, which the subtype pin
  makes the only branch.

### Staging (approved)

- **S1 — evaluation. ✅ COMPLETE** (journal 028).
- **S2 — design the render-thread frame↔state keying model. ❌ NOT STARTED. This is the real
  work and the real risk.** Today labels key to a game-thread `GFrameCounter` at arm time. An SVE
  runs on the render thread with `GFrameNumberRenderThread` and **no arming step**, so a new
  deterministic mapping from "this rendered frame" back to "the game-thread anomaly state
  snapshot that produced it" must be **designed, not ported**. Everything m7/m18/m21 established
  about label alignment needs re-validation.
- **S3 — add depth** (`SceneDepthTexture` in `PrePostProcessPass_RenderThread`, FP32), still in
  CaptureBench.
- **S4 — integrate into `AnomalyCapture` behind a switch**, with existing label-alignment gates
  re-driven green **before** the backbuffer path is retired.

### The framing that must survive into S2

**The SVE removes the arm→present race architecturally, but the owner's ratio-independence
requirement is only DISCHARGED when the new keying model is designed and gated across ratio
regimes on the paced production path.** S2 is where that requirement is met or missed — it is not
just the risky part of the migration.

**Until S4 lands, production still captures via the backbuffer and the client's 1.2-band −1 lag
remains unfixed.**

### Known migration costs

- The SVE needs the **Renderer private include path** (`FPostProcessMaterialInputs`,
  `FScreenPassTexture`, a `class FViewInfo;` forward declaration). Compiles on **stock 5.1** — no
  engine patch — but private headers shift between engine versions = ongoing maintenance.
  `Renderer` is already on the allowed-dependency list; stays quarantined out of Shipping.
- **ExportTextures as a whole is not portable** (its full feature set needs a patched engine).
  **We take the pattern, never the plugin.**
- **Overlay suppression (G54) may partly become moot — must be verified, not assumed.** Canvas/HUD
  overlays are composited post-scene and should be absent from SceneColor, but `DrawDebug*`
  line-batcher primitives **are scene geometry and would still appear**.
- **Knock-on surfaces to re-examine:** m19 preview tee (currently a second backbuffer capturer),
  m16 focus gate (a game-thread concept), m16 preview/capture exclusivity. Delivery mode, pacing
  and content-clock are believed unaffected (stamping/write concerns, not grab-point concerns).

---

## 5. Client-facing work — open, none started

**Highest priority: the invisible-anomaly problem (her complaints 1 and 2).** For training data a
labeled-but-invisible sample is **worse than a missing label** — it teaches the detector that
"nothing changed" is an anomaly.

Two sub-problems:
- **(a) Selection quality.** If an object passed the 12% coverage threshold *and* the occlusion
  trace and still is not visible, one of those checks is lying. Known suspects: large
  backdrop/skybox meshes; unlit or translucent materials; partially-occluded targets slipping the
  sampled line-trace; and the confirmed **environmental-blueprint-actor** case (light/fog volumes
  carrying a token qualifying mesh component, so they pass the filter — the label is *accurate*,
  the change is not *visible*).
- **(b) Traceability.** ✅ Addressed by B1 this session.

**The principled cure for (a) is the parked `feature/stencil-capture` branch.** A real stencil
mask reports how many pixels an actor actually contributes *before* we hide it — coverage stops
being an estimate. The client's #1 complaint is exactly what that branch was built to solve. Note
it still carries the pre-rename `flicker` strings and must absorb the rename on its next rebase.

**Diagnostic now available:** B2's provenance. One observation logged from the m22 gate —
`coverage_pct 100` alongside `occlusion 4/9 samples passed`. On a menu-map landscape mesh that is
unremarkable, but it is **the shape this complaint would take**: full coverage score with a
majority of occlusion samples blocked. Hypothesis to test once provenance from real gameplay
levels exists. **Not acted on.**

### Feature requests (agreed in principle, not built)

- **Output resolution selection.** Worth building; a bigger storage lever than JPEG (4K→1080p is a
  4× pixel cut vs JPEG's ~80%, and they compound). **Load-bearing risk:** labels must scale —
  `bbox_norm` is already normalized 0–1 so geometry survives, but `bbox_px` and coverage derive
  from output resolution. This **deliberately revises the v1 principle "native res, no
  downscale"** at client request. C2 above removes the resolution-semantics objection.
- **PNG → JPEG.** Agreed in principle. **PNG stays the archival/ground-truth format; JPEG becomes
  a delivery-mode option** with configurable quality. **We** run the A/B, not the client: does
  q90–95 measurably degrade the detection signal? Magenta `missing_texture` will be fine; LOD
  seams and subtle lighting mismatches are where compression could eat the evidence.
- **Agreed default settings profile.** Send a concrete proposal to react to, not an open question:
  resolution, format + quality, capture fps, coverage threshold, poll radius, enabled anomaly
  pool, and UI-on/off once the migration lands.
- **Two-monitor note.** Low priority. Honest answer if pushed: on a single monitor the
  **console-command path** (`IAI.Capture.*`) avoids the focus dance entirely (the m16 focus gate
  waits for game-window focus before the first frame). One README paragraph, no code.

### Client reply — not yet sent, contents settled

1. Frame-boundary issue: root-caused to capture timing under load; structural fix in progress.
   **Do not mention ratios or ask her to manage them.**
2. Object identification: fixed — annotations now carry the mesh asset name.
3. Subtype: `blink` now always reports `disappear_reappear`; explain that `blink` covers single
   *or* repeated disappearance and that `flickering` will be a separate class for lighting /
   scene-region effects.
4. Invisible anomalies: acknowledge honestly as a known limitation, describe the diagnostic work
   and the stencil direction.
5. Defaults + resolution + JPEG: proposals to sign off.
6. Send **more samples**, including a `missing_texture` session — that path was assembled
   correctly by construction but has never been empirically gate-tested end to end.

---

## 6. Open vs locked

### Locked (do not relitigate)

- Ratio-independence is required; the ship rule is telemetry only.
- Blink behaviour unchanged; subtype pinned to `disappear_reappear`; field retained.
- `flicker` reserved for the future separate `flickering` class (scene-region / light toggling).
- `frame_count` = `len(frame_indices)`.
- SVE adopted as primary color grab point; backbuffer path **kept** for UI-on.
- UI presence is per-run config via grab-point choice; proposed delivered default UI-off.
- CaptureBench kept permanently, non-shipping.
- Code owns pushes; the owner owns the Play-gate smoke.
- D1 (standalone evaluator) and D2 (sidecar) accepted; the A/B toggle gate is withdrawn.
- `IAnomaly` interface remains **LOCKED** — unbroken since M1, through every milestone.

### Open

- **S2's keying model** — entirely undesigned. The whole risk of the migration.
- **Whether the 1.2 band actually resolves** under the SVE — unmeasurable until S2/S4 gates.
- **Deep starvation (ratio ≳3)** — the old m22 proposal is dead; the SVE is expected to subsume it
  by construction, but this is *reasoned*, not demonstrated.
- **The environmental/invisible-object selection fix** — root cause confirmed, fix not scoped.
- Resolution selection, JPEG option, defaults profile — agreed in principle, unbuilt.
- Client reply — drafted in substance, unsent.
- The `feature/stencil-capture` rebase (must absorb the `flicker` → `blinking` rename).
- Restore the user's original auto seed after a capture, or leave as capture's? (Still defaulted
  to leave-as-is.)

---

## 7. Corrections — things that changed this session

Stale understanding to discard:

- **"The owner owns remote pushes"** → **retired.** Code pushes.
- **"m22 = deep-starvation scene-identity SVE marker"** → **dead proposal**, never tagged. m22 now
  means *this* milestone. Refer to the dead proposal by name.
- **"Ship rule: don't deliver above ratio 1.05"** → demoted to internal telemetry; never surfaced
  to clients as their problem.
- **"SVE has a readback-latency advantage"** → **false**, PIE artifact (C1).
- **"SVE captures at ViewRect so screen percentage desyncs resolution"** → **false**, measured;
  grab point is after the upsample (C2).
- **"blink's multi-toggle behaviour is a defect"** → **wrong**, chat Claude's over-scope; behaviour
  is correct, only the label was wrong.
- **"G81 will self-resolve once blink is single-toggle"** → moot; blink didn't change, so G81 was
  fixed explicitly.
- **"m21 is built but not committed/tagged"** (CLAUDE.md) → **false**; m21 = `a2c3127`, tagged, an
  ancestor of HEAD. Status block refreshed.
- **Chat Claude misattributed the earlier subtype bug** as owner misremembering. It was **real,
  raised by the owner, and fixed in m20**. Three distinct things share the name: the internal
  class rename (done), Bug C (real, fixed in m20), and the vocabulary design (addressed by m22).
- **Ground-truth contract amended:** "game UI in" is no longer absolute; UI presence is per-run
  config.
- **"Native res, no downscale"** — being deliberately revised at client request.

### New environment gotcha — G86

A **Visual Studio update to MSVC 14.42** (pulled in by the owner installing **UE 5.7** for an
unrelated game) (a) silently invalidated the **entire UBT action cache** — the compiler version is
part of the action key, which was the real cause of a mystery 2517-action rebuild — and (b) broke
UE 5.1 engine Core outright (`__has_feature` undefined in `ConcurrentLinearAllocator.h`).

**Fix:** pinned `<CompilerVersion>14.38.33130</CompilerVersion>` in
`%APPDATA%\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml`. **This file is outside the
repo** — a change to the owner's global UE config, one commented XML block, reversible.

**Client impact: none.** The trigger was the owner's own 5.7 install, and the delivered build is
monolithic and does not recompile the engine. **Do not add this to client delivery docs.**

### Office-machine-only work

The client's session artifacts live on a **different machine** and cannot be brought to the dev
box. Any check needing them is **office-only** and should be batched:

- Inspecting the client's session folder / annotation.json
- The `speed_ratio` audit of already-delivered sessions
- The Concorde HDR preview-format check (pending since m19)

**Consequence:** the B3 client-file subtype check was cancelled and replaced with a local smoke.
That smoke **partially passed** — the `flicker` direction confirmed (2/2 events, m20 fix intact,
no regression), but the `disappear_reappear` direction **could not be exercised** because blink's
`DefaultHz=5.0` is unreachable through `IAI.Capture.Start` (no anomaly-arg passthrough). m20's G3
gate did prove that branch (14/14); this session did not re-prove it. **Moot after m22** — the pin
makes `disappear_reappear` the only branch.

**Also observed:** the package boots into MainMenu, where the only blink target was
`StaticMeshActor_0` (the m19/G80 menu-map artifact) — so B3' did **not** run in a real gameplay
level.

---

## 8. Forward plan

**Immediate — S2, in a fresh session.** Design the render-thread frame↔state keying model.
Nothing ships before it is settled and gated the way m18/m21 were. Gates must cover **starved
regimes** and the **wall + delivery** client config (not game-mode-on-StackOBot, which is why bugs
slipped before). Then S3 (depth), then S4 (production integration behind a switch, existing label
gates re-driven green before retiring the backbuffer).

**In parallel / after:**
1. Send the client reply (§5) — unblocked, depends on nothing.
2. Environmental/invisible-object selection fix — her highest-value open complaint. Gather
   provenance data from real gameplay levels first.
3. Resolution selection + JPEG option + defaults profile.
4. `feature/stencil-capture` rebase and revival — the principled cure for the coverage estimate,
   and it converges with the SVE work (Stage 3a already built an SVE mask pass).

---

## 9. Pointers — what a cold reader should have Code read

In the plugin repo:

- `CLAUDE.md` — Current-status block (refreshed this session; now trustworthy).
- `docs/sessions/2026-07-29-028-capturebench-s1-and-traceability-plan.md` — the S1 evaluation and
  the B1/B2 plan.
- `docs/sessions/2026-07-29-029-*.md` — m22 implementation journal.
- `docs/gotchas.md` — G86 at the tail; G76 (packaged exe hot-swap), G80 (menu-map artifact), G81
  (now fixed), G54 (overlay suppression) all relevant here.
- `docs/capture-fps.md` — fps, pacing, fixed timestep, content-clock, and the ship rule.
  **Note:** its ship-rule framing is now superseded by §3.1 above.
- `docs/architecture.md` — anchor-frame bounds semantics; the provenance sidecar.
- `docs/client-delivery.md` — delivery shape.
- Previous chat handoff: `CHAT-HANDOFF-m10-m21.md` — the frame-index saga and its standing lessons.

**Standing lesson that governs the next session:** chat Claude must **not** write a confident
mechanism into a brief without Code's measurement. The frame-alignment family has been
misdiagnosed **three times** from plausible-but-untested theories. **Measure, then design.**
