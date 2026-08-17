# AnomalyInjector — canonical context

Personal research project **"GDP: Anomaly Injection"** (intrusive UE5 track). This is a
UE5 plugin that injects **labeled visual anomalies** (graphics bugs — missing objects,
lighting mismatch, LOD corruption, blinking, etc.) into UE5 games, to generate synthetic
training data for bug-detection ML. It is **game-agnostic** (public UE APIs only) and is
tested on Stack O Bot. A separate non-intrusive tool exists elsewhere and is out of scope.

This file is the **canonical entry point**. The folder it lives in is its own git repository
and is the single source of truth for the project.

## Current status — keep this current; it is the cold-start "you are here"
- **STANDING CONVENTION (owner directive, 2026-07-29): this Current-status block is REFRESHED AT EVERY MILESTONE
  CLOSE — same discipline as the session journals.** It is the cold-start contract: if it says "in flight / not
  committed", a fresh session believes it. Rationale: stale docs have now caused a real miss twice (the m20 "Bug A"
  slipped because annotation.json's path sat outside validation scope; and this block claimed m21 was uncommitted
  for six commits after it had shipped). A status refresh is a standalone `docs:` commit, never folded into feature work.
- 🚧 **S3 STARTED (2026-08-18) — S3a PLANNED, NOT IMPLEMENTED. No capture code written.** → journal
  `docs/sessions/2026-08-18-037-s3a-plan-stage-renumber-h4-filed.md`.
  ⚠ **STAGE RENUMBERING — A NUMBER MOVED, READ THIS BEFORE TRUSTING ANY OLDER DOC:**
  **`S3`** = B′ into `AnomalyCapture` behind a **default-OFF** switch, **COLOUR ONLY**, full ratio ×
  config matrix on the **real paced path** — *this is where ratio-independence is DISCHARGED* (unchanged);
  **`S4` = the BACKBUFFER DEMOTION to the UI-on option + defaults flip + client config — `S4` WAS
  "depth" and NO LONGER IS (it was S5)**; **DEPTH is PARKED and UNNUMBERED** (`SceneDepthTexture` in
  `PrePostProcessPass_RenderThread`, FP32, + the typed FP16/FP32 path) — **parked, NOT deleted**,
  revivable if the ML side wants it or the H4/stencil lane needs a cheap instrument. **THERE IS NO HOLE
  AT S4 — the number moved down.** Any earlier text calling depth "S4" or the demotion "S5" is superseded.
  **S3 SPLITS INTO TWO GATED TURNS (structural, not a suggestion): `S3a` = implementation (land B′
  behind the switch, prove switch-OFF inert, prove the loud-miss guard fires ON THE PRODUCTION PATH);
  `S3b` = the full ratio × config matrix.** Bundling them would let a validation miss halt a turn that
  also holds uncommitted code. **S3a plan = 3 new files (`AnomalySveKeyRing`, `AnomalySceneViewExtension`,
  `AnomalySveCapturer`) + `Renderer`/Renderer-private in `AnomalyCapture.Build.cs` (sanctioned: architecture.md
  deferred it to Stage 3) + `IAI.Capture.SVE` (default 0, GConfig `bSveCaptureDefault`) + `IAI.Capture.SVE.ForceMiss`
  + `run_summary.capture_path`; sliced into 3 gated commits; NO tag until after S3b.**
  **THE SEAM IS `FAnomalyCapturedFrame::RequestId`** — B′ swaps the PRODUCER of that id and touches no
  consumer (`PendingSnapshots`, label record, accumulator, writer, `labels.jsonl`, `annotation.json` all untouched).
  **PRE-DECLARED PREDICTION (before the matrix exists): ratio-independence HOLDS at every ratio incl. deep and
  pacing-off** — B′ keys by IDENTITY, not order, so the arm→present race has no positional step left to fail on.
  **IF THE MATRIX GOES RED THAT IS A DESIGN FAILURE OF B′, NOT A BUG — it means REDESIGN, not patch.**
  🚨 **S3 GOING GREEN DOES NOT CLOSE `P1`.** P1 has never been reproduced and you cannot demonstrate a fix
  for something you cannot summon. A clean matrix proves the new path does not carry the OLD race; it is NOT
  evidence it cures her defect. **P1 stays OPEN after S3**; leads unchanged: **H1** (GPU load, no lever exists)
  and the **delivery-mode gap**. Free-run debts: **A10 discharges in S3a** (nominal paced leg only);
  **key-ring-under-stall and A11 wait for S3b** (`ForceMiss` counters are synthetic, so they cannot honestly
  discharge A11).
- ⚠ **A47 AMENDED (2026-08-18): the bifurcation is in camera ROTATION, not position.** Measured: eye position
  invariant at `(-1500,0,260)` on **369/369** banked gate samples; rotation modal `(0,0,0)` on 278/369.
  **A47's original per-leg-bbox ruling is UNCHANGED.** New clause: **inter-actor occlusion is invariant across
  the bifurcation** (it depends only on eye position + static geometry; rotation changes only frustum membership).
  ⚠ **DO NOT GENERALISE — this holds because gate-level targets are all STATIC and the player start is fixed;
  it FAILS in any level with motion.**
- **HYPOTHESIS LEDGER:** **H1** GPU-load starvation — OPEN, no lever exists. **H2 — RETIRED-UNKNOWN**
  (appears nowhere in this repo; history unrecoverable; **never re-mint this number** — the entry exists only so
  nobody reclaims it). **H3** auto-exposure active — OPEN, likely, unconfirmed. **H4** occlusion-blind labelling —
  OPEN, named, NOT adopted (see its bullet below).
- **`P6` WIDENS to "annotation.json field-contract defects" — NO NEW NUMBER (A61 applied to phenomena).**
  Three instances: **`node.bounds`** — **SETTLED** (editor-only frustum cube; contract ruling locked; parked as a
  milestone candidate); **`camera.path`** — **OPEN**, naming/contract; **`coverage_pct` = 0 while `coverage_ratio` > 0**
  — **OPEN, PREDICTED FROM SOURCE, NOT MEASURED** (a source read, never observed in any artifact; manifests **only**
  under H4's exact condition, i.e. contingent on an unconfirmed hypothesis).
- ✅ **make_gate_level.py FOOTGUN DEFUSED — `CaptureBench` `8dad64e`** (tools edit, **probe untouched**; the freeze
  is not invoked). The script deleted the asset at `LEVEL_PATH` before authoring, so running it unmodified
  **destroyed the frozen `CB_GateLevel`** that every banked leg and every A54 calibration is measured against. It
  now **refuses by default**, names what is at stake, points at the sibling-level route, and yields only to
  `--allow-overwrite-frozen`. Verified three ways (default refuses / override yields / sibling passes). **G99.**
- ✅ **m23 "P3-fix" SHIPPED — commit `2f74799`, TAGGED `m23`, tag pushed and remote-confirmed.**
  → journal `docs/sessions/2026-08-16-034-m23-p3-fix-and-the-oracle-saga.md` (§7 = the smoke addendum).
  **OWNER PLAY-GATE SMOKE PASSED** in **PIE / StackOBot MainWorld** (`session_20260817-132214`, 90 frames,
  1068×604, fps 30) — verified from disk: 8 blink events, **gapped cadence byte-exact to the historical
  shape**, `manifested: true` 8/8, `non_manifested_events: 0`, **zero** occurrences of the 8-consecutive
  fabrication shape; the tail event `[88,89]` is TRUNCATED per the A50 addendum. First confirmation in
  **real gameplay content** rather than synthetic `CB_GateLevel`. ⚠ **A PIE smoke is an owner sanity gate,
  NOT packaged evidence — G76 stands; m23's certification evidence remains the packaged BenchGate legs.**
- ✅ **`P6` BOUNDS SIDE — SETTLED 2026-08-17 (diagnosis only, NO code change).** → journal
  `docs/sessions/2026-08-17-035-p6-bounds-settled-and-auditor-premise-halt.md`.
  In `annotation.json`, `camera.path` equals the anomaly node's path and `camera.global_position`
  equals `node.bounds.origin` to 13 s.f. **The camera block is NOT mis-sourced** — `labels.jsonl` shows
  all 90 frames reporting the same `view.origin`, a genuine `PC->GetPlayerViewPoint`
  (`AnomalyViewport.cpp:404-408` → `ViewRing` → `AnomalyCaptureSubsystem.cpp:1417`). **The 1010 cube is
  `UDrawFrustumComponent`**, auto-created on the OWNING ACTOR by `UCameraComponent::OnRegister`
  (`CameraComponent.cpp:118-152`): `UpdateDrawFrustum` sets `FrustumStartDist=10` +
  `FrustumDrawDistance=1000` ⇒ `FrustumEndDist=1010` (`:203-212`), and `CalcBounds`
  (`DrawFrustumComponent.cpp:164-167`) returns a box **centred on the camera** with extent
  `(1010,1010,1010)`. It is a `UPrimitiveComponent`, so `GetComponentsBoundingBox(true)`
  (`AnomalyCaptureSubsystem.cpp:159-164`) admits it via **`bNonColliding`** — visualisation-only and
  hidden-in-game do not exclude it. It **contains** the capsule and mesh, so the union is unchanged and
  the centre lands exactly on the camera (**containment, not compromise**).
  ⚠ **CORRECTION — the earlier "unions spring arm / camera / collision" wording is STRUCTURALLY
  IMPOSSIBLE and is struck:** the union iterates `UPrimitiveComponent` only (`Actor.cpp:1685`), and
  spring arms and camera components are `USceneComponent`s. **1010 is a hardcoded editor visualisation
  constant, not geometry.**
  ⚠ **CLIENT-IMPACT DOWNGRADED — this is EDITOR/PIE-ONLY.** The creation is behind
  `WITH_EDITORONLY_DATA`, and the packaged game target defines it **0** (measured in
  `Intermediate\Build\Win64\StackOBot\Development\...\Definitions.*.h`). **Prediction on the record:** a
  packaged capture of a camera-bearing pawn gives order-of-capsule bounds, not 1010. **NOT yet
  measured — the corpus is confounded** (every packaged node with a bounds field is a camera-less
  `StaticMeshActor`); the confirmation run is **DEFERRED by owner ruling**, and the gate level must NOT
  be mutated to enable it. `camera.path` is the **view-target actor** path (`ResolveCameraPath`
  `:102-116`), equal to the node path here only because the anomaly fired on the player pawn — an open
  naming/contract question.
  🔒 **CONTRACT DECISION — RULED AND LOCKED, NOT IMPLEMENTED, DO NOT RELITIGATE:** `node.bounds` must be
  **render-relevant bounds** — the union over components that contribute drawn pixels — never a
  whole-actor union admitting collision capsules and visualisation primitives, and it must reuse the
  existing renderable definition (`IsRenderableComponent`, static-or-skinned, **G33**) so label geometry
  and selection geometry agree on what "the object" is. **Parked as a milestone candidate.** Residual
  that survives even packaged: the capsule is still unioned in, so `node.bounds ≠ mesh bounds` in both
  configs — by a capsule, not by 1010.
  **New rules A59** (MCP-bridge provenance: echo `Paths.project_dir()` + engine version or the
  measurement is not attributed to this project), **A60** (a quantity absent from the artifact is
  operator-supplied or the claim is UNDECIDABLE — never reconstructed, never defaulted, never replaced
  by a weaker test reported as the original), **A61** (a new shape earns a diagnostic tag, never a new
  verdict bucket). **New gotchas G97** (the bridge attaches to whichever editor is listening — a second
  UE project on this box, `HeistCrewUE`/5.7.4, silently captured it; permanent environmental fact) and
  **G98** (`AffectedFrames` is a PROJECTION-FILTERED SET, not a frame range — see the auditor entry).
- ⛔ **DELIVERED-SESSION FABRICATION AUDITOR — CANCELLED 2026-08-18 (not paused). NEVER IMPLEMENTED.**
  → journal `docs/sessions/2026-08-18-036-auditor-cancelled-and-h4-occlusion-recon.md`.
  Owner constraint: **there is no client communication channel in either direction**, so the audit's
  output has **no consumer**. A cold reader who finds the approved plan in journal 035 is looking at
  work that is **closed, not outstanding**. **KEPT AS REFERENCE ONLY** (all in journal 035): the
  verified schema mapping, the three schema traps, the located control sessions, **G98**, and the
  shipped-default observation. **NEG2 stays banked** — that rescue was correct regardless (G92).
  🚫 **STRUCK FROM THE STANDING PLAN — struck, NOT deferred: the office-machine `target_fps` audit and
  the precautionary "cap VideoFps at 30" client note. NO CLIENT-FACING ACTION ITEM SURVIVES ANYWHERE.**
  Any older doc that still lists one — including `CHAT-HANDOFF-s2-i10-and-m23-p3-fix.md` §8 — is
  superseded by this line.
- ⚠ **NEW OPEN HYPOTHESIS `H4` — occlusion-blind labelling. NAMED, NOT ADOPTED, NEVER OBSERVED.**
  (H1 and H3 are minted; H2 appears nowhere, and numbers are never reused, so H4 is next free.)
  **The selection path is occlusion-aware and the label path is not:**
  `IsComponentRenderableVisibleInternal` (`AnomalyViewport.cpp:165-181`) = renderable ∧ poll-radius ∧
  frustum ∧ **`IsUnoccluded`**, while `ProjectActorBoundsToScreenRect` (`:653-685`, called at
  `AnomalyCaptureSubsystem.cpp:1438`) runs **no trace at all** ⇒ a target on-screen but fully occluded
  is **labelled positive while contributing no pixels**. `IsUnoccluded` traces centre+8 corners and
  passes on ANY clear sample, so "fully occluded" = **9/9 blocked**. **Routed to
  `feature/stencil-capture`** — that branch's premise (report actual pixel contribution before hiding)
  is its cure, so H4 **strengthens a locked ruling** rather than opening a lane. **Pre-declared test:**
  a target fully occluded for a whole event window, labelled, pixels unchanged. **RECON DONE, READ-ONLY,
  NO TEST RUN:** `CB_GateLevel` holds **7 targets that are fully occluded AND on-screen** under the
  rigorous cube-occluder-only floor (26/144 realistic, 52/144 upper bound) — **no scene mutation
  needed**, and `make_gate_level.py` **deletes `LEVEL_PATH` before authoring**, so a sibling level must
  rename first. **A47 is a ROTATION bifurcation, not a position one — measured: camera position
  invariant at `(-1500,0,260)` on 369/369 banked gate samples, modal rotation `(0,0,0)` on 278/369** ⇒
  occlusion is stable across the bifurcation by construction (rotation only changes frustum
  membership). Verified name map: **`StaticMeshActor_K` ⇔ grid spawn `n = K−1`** (3 data points).
  Targeted fire **bypasses occlusion** (`TryFireSpecific` has no viewport predicate) **only while
  `IAI.SetViewportScoping` is OFF (its default)** — ON, the anomaly's own `Apply` re-filters and
  selects nothing. Auto-pool **does** exclude occluded actors. **Only path (b)** — fire at an
  already-occluded actor — is producible; **path (a)** (becomes occluded mid-window) is impossible here
  (all-STATIC actors, invariant camera). **A54 would read this as ABSENT — the same verdict as P3 —
  so it detects the symptom, not the cause;** the cause is already instrumented in
  `selection_provenance.json`, ⚠ **but for a fully occluded target `EvaluateSelectionProvenance`
  early-outs**, so the sidecar reads `valid:false` + `0/0` (not 9-blocked) and **`annotation.json`
  ships `coverage_pct = 0` with `coverage_ratio > 0`** — observation only, not a designed
  discriminator. Test design is chat-side; nothing runs until it returns.
- ⛔ *(superseded — kept for the record)* **AUDITOR HALTED AT ITS SOURCE-PREMISE GATE (2026-08-17).** The instrument is meant to decide
  whether the client's already-delivered **pre-m23** sessions carry P3b-fabricated windows. Its
  window-blind design rests on "a gap is mechanically impossible for a fabricated event". **First half
  confirmed** (the pre-m23 fallback does emit `Ev.AffectedFrames` verbatim — `git show m23^`);
  **second half FALSIFIED → G98:** `AffectedFrames` is accumulated only on frames passing
  `ProjectActorBoundsToScreenRect` (`AnomalyViewport.cpp:653-685`), which fails on an invalid view, no
  static/skinned bounds, a box entirely behind the camera, or a rect off-screen — so a target that
  leaves frame mid-window **gaps** the set. **0/1,367 events show it, but that null is CONFOUNDED —
  every banked leg is static-camera; the client captures moving-camera gameplay.** Blast radius covers
  the windowed path too: a fabricated-but-gapped event reads as "strict gapped subset" = GENUINE-SHAPED,
  i.e. **a false blessing in the dangerous direction**. Halted for a chat verdict; no in-turn repair.
  **Also banked this turn:** control #3 (`NEG2\session_20260816-183524`, guard fired 8/8) rescued out of
  the archive-wipe path (**G92**) into `_bench_sessions_bank` (95 files, SHA-256 verified). **Control #4
  (delivery-ON) still does not exist** — 0 of 74 banked sessions has `delivery_mode=true`.
  **Shipped-default observation (report-only, NOT wired in):** the burst window is `PositiveFrames = 8`,
  hardcoded at `AnomalyCaptureSubsystem.h:182`, changeable **only** by the `IAI.Capture.Config` console
  command — no dashboard command, no `config.json` key, no ini (`GConfig` reads only
  `bDeliveryModeDefault` / `ContentClockDefault` / `bFocusGateDefault`). **The window is NOT obtainable
  from the client** (owner constraint: not possible), which is why the design is window-blind.
- ⚠ **m23 as-built (unchanged):**
  **PRODUCTION CHANGED — the "production byte-unchanged" invariant of S2 RETIRES HERE**; from m23 on it is
  *production changes only via approved milestone plans*. 8 files, +103/−34. `IAnomaly` untouched;
  `labels.jsonl` untouched; auto-pool (`TryFireOnce`) untouched.
  **P3a** — blink's half-period is now in **FRAMES** (default **3** = the previous 30 fps cadence,
  byte-exact) instead of seconds, so the toggle no longer depends on `VideoFps`.
  **P3b** — hide-type identity comes from the anomaly **ID** (`IsHideTypeAnomaly`), never from the
  sampling outcome; zero sampled-hidden ⇒ **zero positives** + `manifested:false` + loud warning +
  `non_manifested_events`; an **unregistered id** falls back loudly rather than silently.
  **Targeted-fire args** — tokens after the target on `IAI.Capture.Start` forward verbatim to the
  anomaly's parser (no tokens = byte-identical to before). It exists because **the guard is untestable
  without it**.
  **THE GUARD IS PROVEN BOTH WAYS, and that pair IS the certification:** forced non-manifestation
  (`half_period_frames 40`) → **8/8 events `manifested:false`, zero `frame_indices`, counter 8**, verified
  in the annotation rows; the 30 fps control → guard **silent**, counter 0, cadence byte-exact.
  **CERTIFICATION SCOPE: 30 fps CERTIFIED, floor-robust** (12 decidable ALIGNED / 0 non-ALIGNED under
  BOTH candidate floors). **60 fps FLOOR-BLOCKED — deferred, NOT failed. ≥90 fps P5-BLOCKED.**
  ⚠ `positive_frames` stays **fire-active** and is unchanged by design — **fire-active ≠ manifested**;
  the client artifact carries neither per-frame flag (delivery gates `labels.jsonl` entirely), so
  event-level `manifested` is the only channel to the client.
  New rules **A57** (floor-robustness: certify only what is invariant across all defensible calibration
  constructions; a set that *brackets* a regime without *containing* it yields no floor) and **A58**
  (diff-isolation rules are invariants-to-preserve, never confinement predictions; when a brief
  contradicts itself take the conservative branch and flag it). New gotcha **G96** — oracle blindness is
  exposed only by known-answer controls, three instances one principle.
  **P5's founding instrument is assigned** (the blend-ladder from the certified 30 fps leg) and is also
  the eventual source of DA60's deferred floor. Not built.
- 🔴 **P3 DIAGNOSED — MECHANISM ADOPTED (superseded by m23 above; kept for the diagnosis record).** → journal
  `docs/sessions/2026-08-16-033-p3-mechanism-adopted.md` (2026-08-16). **P3 is TWO stacked defects:**
  **P3a (timing)** — `FAnomaly_Blinking` accumulates forwarded tick dt (`Anomaly_Blinking.cpp:58-69`)
  against `HalfPeriodSeconds = 0.5/Hz` (:44, default 5 Hz ⇒ 0.100 s), and under capture that dt **is**
  `1/VideoFps` (`AnomalyCaptureSubsystem.cpp:672`, clamp :442). When a window's ticks × (1/fps) never
  reach the half-period the toggle never fires and **the actor is never hidden** (:76) — the pixels are
  CORRECT, the scene truly has no anomaly. `Revert` resets the accumulator (:85-98), so no phase carries.
  **P3b (labelling, anomaly-agnostic, the amplifier)** — `:1466` infers `bHideType` from the sampling
  OUTCOME (`HiddenIdx.Num() > 0`) and `:1467` **silently substitutes `Ev.AffectedFrames`**, turning a
  non-event into a full-window block of positives. **Fingerprint: genuine hide sets are GAPPED
  (`[4,5,9,10]`); the fallback shape is CONSECUTIVE (`[3..10]`).** → **G94**.
  **DISCRIMINATORS:** D-A fps bisection — a **THRESHOLD between 90 and 120 fps, not a gradient**
  (60fps 12 ALIGNED, 90fps 6 ALIGNED, 120fps 13 ABSENT) with **zero SHIFTED events anywhere**; a
  survey-derived 60–90 expectation was stated in advance and **FALSIFIED**. D-B grab-point test — true
  dual capture, marker-matched, one oracle: **backbuffer and SceneColor agree to four decimals, both show
  the object VISIBLE during labelled windows ⇒ P3 is scene-level and grab-point-independent; THE SVE
  MIGRATION WOULD NOT CURE IT.** D-C — `missing_object` (hides in `Apply`) **MANIFESTS at 120 fps, 8
  ALIGNED** ⇒ P3a is pinned on the toggle clock. **G95** (a second capturer's write load starves the
  production writer; overlapping captures need the focus gate managed).
  **FIX DIRECTION (plan only, no code yet):** **F-LABEL** — zero sampled-hidden ⇒ **zero positives**, row
  kept with additive `manifested:false` + loud warning + session counter; hide-type identity from
  existing routing, never from sample outcomes; `IAnomaly` LOCKED. **F-BLINK** — half-period in **FRAMES**
  (integer), **default 3**, reproducing today's 30 fps cadence exactly; 60/90 fps cadence changes and that
  is accepted. **SEQUENCING RULING: P3-fix lands and validates BEFORE S3** — client-facing dataset
  poisoning outranks the internal migration, and D-B proves the SVE is orthogonal.
  **OPEN, evidence-only: P4-CANDIDATE** — the D-C leg shows 41/96 shift-0 mismatches while all 8 event
  edges are ALIGNED (a tail-length disagreement), **distinct from P3 and from P1, not conflated**.
  New rules **A53** (any oracle/analysis change re-verifies against one known-ALIGNED and one
  known-ABSENT control first) and the **A50 TRUNCATED addendum** (events cut by the frame cap are
  classified TRUNCATED and excluded from taxonomy counts).
- 🔴 **THE REPRODUCTION ITSELF — "P3": at `VideoFps` 120/240 a hide-type anomaly window is LABELLED IN
  `annotation.json` BUT NEVER APPEARS IN THE PIXELS. 49/49 events across four legs, zero manifested.**
  → `docs/sessions/2026-08-16-032-s2-high-fps-sweep-and-p3-reproduction.md` (2026-08-16).
  99 labelled-positive frames per leg, target plainly visible in every one (eyes-confirmed) —
  **dataset-poisoning severity.** **DO NOT CAPTURE HIDE-TYPE ANOMALIES ABOVE 30 fps until this closes**;
  treat any existing high-fps session as suspect. Legs: HF1 120fps ratio 1.6916, HF2 240fps 3.8262,
  HF3 120fps+26ms 3.3684, HF4 240fps+24ms 6.1092 — **all ABSENT**.
  ⚠ **NOT the m21 stale-present mechanism**, and three discriminators say so (**A51** signature kit):
  identity `label.frame_index == marker gfc` **590/590 diff 0**; adjacent-duplicate scan **0/149
  byte-identical AND 0/149 near-identical** on every leg (frames are fresh); claimed-hidden frames at
  **−1.8..+3.5 robust sigmas** vs **+22..+29** at 30 fps. Perfect pairing, fresh frames, anomaly state
  never reaching the rendered scene. **NO MECHANISM CLAIM.**
  ⚠ **`VideoFps`-SCOPED, NOT RATIO-SCOPED (A52):** HF1 at ratio 1.69 and HF4 at 6.11 are equally ABSENT
  while the 30-fps legs at ratio 3.0027/3.4840 were perfectly ALIGNED — same target/seed/anomaly/binary.
  🔎 **m21 ARCHAEOLOGY — the residual was P3 filed as P2, and its attribution was confounded.** Journal
  027's R7 (`blinking@240`, "pixels never show the hide at all") **is P3**; R3/R6 were also at **240**.
  Every m21 residual run was at `VideoFps 240`, so "deep starvation (ratio ≳3) → stale scene" conflated
  ratio with fps — **our 30-fps deep legs hit ratio 3.0027/3.4840 and were clean on all three
  discriminators.** Journal 027's "the staleness is change-type-dependent" inference is SUPERSEDED.
  **THREE PHENOMENA, tracked separately from here:** **P1** the client's one-frame shift @ratio≈1.2,
  30fps — **NOT reproduced** (12 legs); **P2** stale/duplicate present — **signature absent** here;
  **P3** labelled hide never manifests — **REPRODUCED**, fps-dependent.
  New rules **A48** (config echo — report the EFFECTIVE value from independent read-backs, never the one
  requested), **A49** (pre-declared regime windows where A40's bands don't apply), **A50** (per-event
  taxonomy ALIGNED/SHIFTED(N)/**ABSENT**; ABSENT counts as reproduction and the oracle must be *able* to
  say it), **A51**, **A52**. New gotcha **G93** (`FocusGate 0` + high fps corrupts the camera — neither
  alone does; keep the focus gate ON above 30 fps). **Also measured: above the box's sustainable capture
  rate `speed_ratio` stops being a dial and becomes a readout** — natural frame cost 12.7–17.8 ms here, so
  ratios 1.5–2.1 (120fps) and 3.2–4.1 (240fps) occur with ZERO induced load, and the nominal/client bands
  are unreachable there. Still open and NOT closed by this: **H1** (GPU-load — both levers are CPU
  busy-waits), the **delivery-mode gap** (every I10 leg ran delivery OFF), **A47**.
- **IN FLIGHT — SVE capture migration, stage S2 (render-thread keying design). Production BYTE-UNCHANGED;
  production still captures via the BACKBUFFER; no S3 work started.** → journals
  `docs/sessions/2026-08-06-030-s2-keying-design-and-gate-environment.md` and
  **`docs/sessions/2026-08-16-031-s2-i10-game-lever-and-render-lever-provenance.md` (latest)**.
  **I10 GAME-LEVER LEGS DONE — THE CLIENT'S DEFECT DOES NOT REPRODUCE UNDER GAME-THREAD STARVATION.**
  Six targeted single-anomaly hide-type legs (blink on one measured-prominent actor) across every required
  A40 band — nominal 1.0000, mild 1.0558, **client 1.2148 and 1.2342**, **deep 3.0027**, **pacing-off
  0.3312** — gave **44 hide events, 494 frames, ZERO misaligned frames**, with a live positive control (a
  one-frame shift costs 19–30 mismatches per leg). An independent identity check agrees:
  `labels.jsonl frame_index == the marker's decoded GFrameCounter in the pixels` on **532/534** frames, in
  every regime. **Three of the four frozen predictions FAILED** (client/deep/pacing-off all predicted −1);
  by the pre-declared rule the clean CLIENT band is a **MAJOR RESULT** — her defect has a different
  mechanism. ⚠ **This licenses exactly one claim** — "does not reproduce under GAME-THREAD starvation, this
  box, this level, VideoFps 30" — and **no mechanism claim**. **Deep starvation stays OPEN**: ratio 3.0 was
  reached by a *game-thread* stall, which by the concurrency model never starves the renderer, so it cannot
  have tested the present-side m21 residual. The high-`VideoFps` (120/240) legs are still owed.
  **RENDER LEVER FIXED AND CHARACTERISED.** Root cause was **binary provenance, not code** (G92): the fix
  *was* compiled at 11:53:58, 36 s before the legs — but never **staged**, so the package kept serving a
  2026-08-06 exe. Re-staged, A44-verified by string scan, and it fires with **zero probe edits**
  (`fired=780` at 40 ms; 0 at 0 ms). ⇒ **`speed_ratio` is NOT blind to render-side starvation** — the near-miss
  major finding is refuted. Sweep `0→1.000 · 20→1.000 · 30→1.116 · 40→1.407 · 70→2.308 · 110→3.507`;
  **one model for both levers**, `frame_time ≈ max(1/VideoFps, stall + residual)`, residual **1.3 ms game /
  6.9 ms render**, knee **32.0 / 26.4 ms**. ⚠ **Corollary: the ratio cannot attribute** — a client's 1.2 says
  frame time ≈ 40 ms and nothing about which thread starved. **The counter story is CORRECTED**: `163dd12`'s
  counters were never in the binary, so "stall_fired=0" was never a reading — ratio arithmetic made that
  catch. *"A counter that never printed is not a counter that printed 0."*
  **RENDER I10 LEGS ALSO CLEAN — six for six in-band on the FIRST attempt, zero retries** (nominal
  1.0000, mild 1.0815, **client 1.2145 / 1.3071**, **deep 3.4840**, pacing-off 1.4317): 43 events,
  480 frames, **0 misalignments**, identity **520/520**. Bands were declared chat-side from the sweep
  before any leg ran; the defect outcome was **UNPREDICTED**.
  ⇒ **COMBINED across both I10 sets: 87 hide events · 974 oracle frames · 0 misalignments · 1052/1054
  identity frames at diff 0. "CPU starvation breaks the arm→present pairing" is REFUTED for this
  instrument** — nominal through deep, either thread, pacing-off from both sides. ⚠ **Pacing-off is
  TWO regimes**: game-set L5 free-ran FAST (0.3312), render-set R5 was render-limited and ran SLOW
  (1.4317) — the slow one is closer to a struggling client box. **Still NOT ruled out and named in
  advance:** H1 (GPU-load shape — no lever exists for it), the high-`VideoFps` 120/240 pacing-ON shape
  where the m21 residual was actually seen, and **the DELIVERY-MODE GAP — both I10 sets ran delivery
  OFF because the oracle needs the `labels.jsonl` bbox that delivery suppresses, while the client
  captures in delivery mode; closing it needs a delivery-compatible oracle.**
  Precisions on the record: the analysis-window start is a **wall-clock ~570 ms luminance ramp**, not a
  frame count (5 frames at 116 ms/frame, 16 at 36 ms/frame); `fired=N` is **process-lifetime and
  quantised to 60** by the `%60` log filter, so it reads "the lever executed", never "N times during
  capture"; the A47 camera bifurcation has now occurred **once in twelve legs** and is NOT retired by
  these six.
  ✅ **A20 item 4 was DISCHARGED by `fbf8ad1`** (the `GameDefaultMap` bullet in PRE-DELIVERY-CHECKLIST
  §1 — that commit calls it the "delivery-checklist guard"); the debt had been carried forward wrongly
  by the handoff, journal 030 and early drafts of 031. Struck.
  New standing rules **A44** (prove the change is in the binary; string scan, not timestamp), **A45**
  (a valid marker read is a strictly increasing series — the decoder confidently misreads markerless
  frames), **A46** (kill by process name + assert an idle box; the 217 KB launcher trap already produced one
  wrong ratio), **A47** (per-leg bbox — the gate level is content-deterministic, NOT camera-deterministic).
  New gotchas **G90/G91/G92**. Banked sessions were moved out of the package before re-staging, to
  `D:\IntrusiveAnomalies\_bench_sessions_bank` (1347.2 MB) — the archive step wipes that tree.
  **B′ is LOCKED**: a key-only ring — the game thread publishes at **`BeginRenderViewFamily`** (the ONLY
  hook where `ViewFamily.FrameNumber` is assigned; `SetupViewFamily` still reports `UINT_MAX`), the
  render-thread pass looks up by `View.Family->FrameNumber`, the label snapshot never crosses threads, and a
  lookup miss is loud (counted, warned, frame dropped — never labelled by guess). Measured: counters advance
  1:1, ring round-trip 100%, forced-miss 26/26 warned.
  **Gate environment now exists and is characterised** — synthetic bench level `CB_GateLevel`, cooked to
  `Builds\BenchGate` (**NOT deliverable**), reached by command-line map argument so the host project config
  is never touched. See **G87–G89** for the whole saga (packaged runs always boot MainMenu; loose config
  beside a package is ignored; black-level, exposure-pin and cost-model corrections).
  **Game-thread stall→ratio table BANKED** (dev-box instrument, not a portable spec): knee 30→34 ms and
  sharp, ratio **3.004 at 99 ms**, `frame_time ≈ stall + 1.3 ms`. *(Re-verified on the rebuilt binary:
  0 ms→1.0000, 99 ms→3.0242. ⚠ Those banked legs ran **marker-OFF** — the decoder's constant-`0`@row-105
  false signature — so they carry no frame-identity evidence of their own; an A17/A19 input.)*
  **Render-thread lever now WORKS and is characterised — see the 031 entry above.**
  **Open and NOT drifting toward "fine":** deep starvation (m21 residual never reproduced, and the I10 deep
  leg could not test it — game-thread stalls do not starve the renderer), the latch lifetime rule
  (unbuilt), the captured-view matrix equality check (ViewRing/`ViewLagFrames` deletion is blocked on it).
  **Nothing shows the migration FIXES the client's defect** — I10-game has now run and came back CLEAN,
  which means the defect's mechanism is still unlocated; a render-side reproduction is the next place to
  look, and it must exist before any fix can be shown to remove it.
- **Prior milestone (as-built): m22 — blink subtype pin + annotation traceability + selection provenance —
  COMPLETE (tagged `m22`, pushed).**
  Three commits on `master`: `af8d937` `feat(blinking)` + `03a51d5` `feat(capture)` + `28bc6f1` `feat(capture)`.
  **(1) Subtype pin** — the per-event subtype derivation is DELETED; `blink` now emits
  `anomaly_subtype = "disappear_reappear"` **always**, however many toggles the event contains. **Blink's own
  behaviour is UNCHANGED and is CORRECT** — multi-toggle within a window is intended, not a defect. `"flicker"`
  leaves the blink family entirely and is **reserved for the future separate `flickering` class** (unbuilt);
  the `anomaly_subtype` FIELD is retained so that class has a slot and the client file shape doesn't churn twice.
  **(2) G81 FIXED** — `affected_frames.frame_count` was a SPAN (`end-start+1`, reporting 7 for 4 real indices on
  gapped events); it is now a TRUE COUNT = `len(frame_indices)`. The span stays recoverable from
  `start_frame`/`end_frame`. Confined to `WriteSessionAnnotation`; the m18-validated labels/range-builder path is
  untouched. **(3) B1 traceability** — `affected_objects.nodes[]` gains `asset_name`, `component_class` and
  `bounds{origin,extent}` so auto-named `StaticMeshActor_###` level actors are identifiable (verified:
  `StaticMeshActor_0` → `SM_SlopeWarpLandscape`). ⚠ **Anchor-frame semantics — sampled ONCE at the event's first
  captured frame, NOT per-frame truth** (documented in architecture.md). **(4) B2 selection provenance** — new
  `AnomalyViewport::EvaluateSelectionProvenance`; `coverage_pct` → **annotation.json (client-visible, both
  modes)**, occlusion samples passed/total + poll distance → new **`selection_provenance.json` sidecar
  (internal, suppressed in delivery mode)**. **OBSERVATIONAL ONLY, enforced STRUCTURALLY: the selection path has
  ZERO edits** (both `AnomalyViewport.cpp` hunks are pure insertions; the auto-injector/selector are untouched),
  so the early-out boolean still decides selection. Gates: seeded selection identity (seed 4242, 8 events,
  two runs byte-identical), delivery suppression, `frame_count == len(frame_indices)` on all 10 events, blink
  subtype pinned on `runs=2` events, and the m20 trailing reappear frame still present.
  **TWO DEVIATIONS from the approved plan, owner accept/reject pending:** B2 is a standalone evaluator rather
  than an opt-in out-param threaded through `ClassifyRenderableVisibleLive` (so "observational" holds by
  construction and costs 9 traces per EVENT, not per candidate per poll — but there is no runtime ON/OFF toggle,
  so that gate is discharged structurally); and the internal half went to a finish-time sidecar rather than
  `run.json`, because `run.json` is written at StartRun **before any event exists**.
  **CLIENT-FACING:** `frame_count` and blink `anomaly_subtype` change value in files the client already receives.
  → `docs/sessions/2026-07-29-029-m22-subtype-pin-traceability-provenance.md`.
- **Previous HEAD before m22 = `ed2b851`. Latest TAG = `m21` = `a2c3127`** (m22 is not tagged yet).
  **Post-m21, untagged, on `master`** (all shipped/pushed): `3e5e455` + `a4a8862` docs(client-readme) — Setup/Run flow,
  shared captures folder, ffmpeg troubleshooting; `3a46c1f` fix(control-server) — reply `{type:"error",
  code:"bad_token"}` before rejecting a peer; `9c46ef5` docs(gotchas) **G83** — 5.1 `INetworkingWebSocket` has no
  `Close`, the error reply is the only bad-token signal; `6d01bc9` docs(delivery) — PRE-DELIVERY-CHECKLIST + client
  docs migrated to a runtime `config.json`; `ed2b851` docs(delivery) — desktop-app delivery (WebView2, SmartScreen,
  Python-for-encoder) + **G85**. The **dashboard M1–M3 (Tauri `Dashboard.exe`)** work lives in the SEPARATE AnomDash
  repo, not here.
- **NEXT MAJOR TRACK (approved 2026-07-29): the SVE capture migration** — see
  `docs/sessions/2026-07-29-028-capturebench-s1-and-traceability-plan.md`.
  **NEW LOAD-BEARING REQUIREMENT: label correctness must be RATIO-INDEPENDENT.** A real client session at
  `speed_ratio` **1.2** produced a confirmed **−1** on a `missing_object` boundary (effect frame 50, annotation 51) —
  the **1.1–1.5 band that was never measured** (m21 validated ratio≈1.0 exact and ratio≳3 residual; this sat between
  the two measured points). Clients capture on their own hardware and **will not be asked to discard or re-capture
  sessions**. ⇒ **The `speed_ratio ≤ ~1.05` SHIP RULE IS DEMOTED from correctness gate to INTERNAL TELEMETRY**, and
  retires entirely once the SVE migration lands. **The SCENE-IDENTITY-MARKER proposal is SUPERSEDED and dead** —
  the SVE grab point solves the same problem structurally (it reads the frame being rendered, so there is no
  arm→present matching to get wrong). ⚠ **That dead proposal once claimed the name "m22"; it was NEVER tagged,
  and the m22 number is REASSIGNED to the blink-subtype/traceability milestone below. Refer to the dead
  proposal by NAME (scene-identity marker), never by number.** **UI decision: UI on/off ships as a GRAB-POINT CHOICE — SVE = UI-free,
  backbuffer = UI-on. No UI-isolation work** (SDR has no isolated UI layer: `GetCompositeUIRenderTarget()` is
  HDR-gated, `SlateRHIRenderer.cpp:980-991`). **Ground-truth contract AMENDED: UI presence is per-run config**
  (proposed delivered default UI-off, pending client sign-off). 8-bit delivered color stands; the typed FP16/FP32 path
  lands **with depth**. **`Plugins/CaptureBench/` is a PERMANENT non-shipping perf-regression harness** (own plugin,
  gated `CAPTURE_BENCH`, never ships; it is NOT part of this repo).
- **STANDING TEST BASELINE (owner directive, 2026-07-15): validate against a LOCAL PACKAGED BUILD under
  `D:\IntrusiveAnomalies\StackOBot\Builds`, not just PIE.** The editor masks packaged-only behavior — both
  first-smoke-test bugs (packaged black preview; missing_texture stuck revert) were invisible in PIE. A package runs
  fully headless: `StackOBot.exe -windowed -ExecCmds="IAI.Server.Start, ..."` + the control server's own WS surface as
  the driver; iterative cook + exe hot-swap = ~1 min edit→validate loop. See **G76**.
- **Latest milestone (as-built): m21 — deterministic arm→present pairing — COMPLETE (commit `a2c3127`, tagged `m21`,
  pushed) (2026-07-15).** *(Corrected 2026-07-29: this entry previously read "IN FLIGHT … NO COMMIT this turn" and
  stayed that way for six subsequent commits. m21 shipped.)* The owner's −1 (annotation/labels one earlier than
  pixels) was **not delivery mode and not content clock** — both refuted by A/B + code (the clock only reaches the fps
  stamp; the dev box was already running WALL: no `[AnomalyCapture]` ini section, engine default Wall since m15). And
  it was never an annotation bug: **labels.jsonl shifts too.** **Real variable = the rate regime:** paced+sustainable
  (`speed_ratio`≈1) = exact; starved (ratio≫1) OR pace-off (ratio 0.38, running FAST) = content lags index by one —
  the m11 pacer's sleep was the only thing making the old pairing correct, and every m18/m19/m20 validation ran in
  that one masking regime. **Measured (STEP 1):** arm-id→consume-rtframe delta = d=2 (99%) at ratio≈1, d=1 (100%)
  starved/pace-off, **MIXED 88/12 within one starved run** → no fixed delta exists; pairing must be by identity/order.
  **Fix (~18 lines, `AnomalyFrameCapturer.{h,cpp}` only):** arm registration now rides the render-thread command
  stream (`ENQUEUE_RENDER_COMMAND`, weak-ptr guarded) → FIFO-ordered after present(N−1), before present(N) → "next
  present wins" deterministic in every regime; `armRt` telemetry added to the armed-frame log. Preview tee inherits
  (shared class). **Post-fix:** pace-off **FIXED** (0/100, was −1); ratio 1.05 exact; ratio≈1 byte-identical
  (pattern/cadence match m20); pairing telemetry `consume−armRt=0` 100/100 even at ratio 3–5. **⚠ RESIDUAL EXPOSED
  (open → proposed m22): under DEEP starvation (ratio≳3) the presented backbuffer can carry a STALE SCENE** — a
  one-time mid-run event permanently shifts content −1 (missing_texture) while pairing stays perfect, and
  render-STATE changes are worse: an 8-tick hide window (game-state-proven via annotation) **never appeared in any
  presented frame** (blinking@240, visually confirmed). No arm-side pairing can fix content the present never
  contained → the proposed **scene-identity marker**. **⚠ SUPERSEDED 2026-07-29 — that proposal is DEAD and was
  never tagged; the SVE migration replaces it** (it reads the frame being rendered, so no arm→present matching exists
  to get wrong). *(It once claimed the name "m22"; that number now means the blink-subtype/traceability milestone.)* **The old SHIP RULE (`run_summary.speed_ratio ≤ ~1.05 & paced` → trust) is DEMOTED to internal
  telemetry** — it is NO LONGER a correctness gate, because a client session at ratio **1.2** shifted anyway and
  clients will not re-capture. Honest gates as of m21: G2/G3/G4/G6(ratio≈1) GREEN; **G1/G5 (deep starve) NOT green** —
  improved but residual, deferred with evidence. G82.
  → `docs/sessions/2026-07-15-027-m21-arm-present-pairing.md`.
- **Latest milestone (as-built): m20 — annotation.json labeling — COMPLETE (commit `c01a214`, tagged `m20`, pushed)
  (2026-07-15).**
  Three reported annotation bugs; measured against PIXEL ground truth in a package. **One was NOT a bug and two had
  different mechanisms than first diagnosed** — the fixes below are what the evidence supports (the brief's own
  pixel-exact gates are what prove it). **Bug A (missing_texture range shifted one earlier) = NOT A BUG, no code change.**
  annotation is NOT a separate path from labels.jsonl: `BuildLabelRecordForSnapshot` (:800) and `AccumulateFrameEvents`
  (:802) are ADJACENT LINES fed by the SAME m18-corrected `Snap`. Measured: annotation `[3..10]` == pixels == labels, zero
  disagreement; index 3 ↔ `frame_00003.png` ↔ visibly checkered. **Cause of the field report = 0-BASED vs 1-BASED
  numbering** (pipeline is 0-based, `frame_%05d.png` + encode_watcher :17-18; a 1-based player shows index 15 as "frame
  16" = the exact reported +1). **The tell: pre-m18 the range ran one LATE and accidentally matched a 1-based read; m18
  made it 0-based-exact and "broke" that read.** Shifting +1 would RE-INTRODUCE the m18 bug in the client deliverable →
  **OPEN owner question (blocks any change): which artifact was "actual 16..20" read from? Decisive test = open
  frame_00015.png vs frame_00016.png; the first corrupted one IS start_frame.** If the client's tooling is 1-based that's
  a SPEC change (all indices + docs + slicer contract), never a sample shift. **Bug B (blinking "tail clip") = blinking's
  hidden state was ONE GAME TICK STALE** — there is no range end to fix (`frame_indices` is emitted verbatim,
  AnomalyLabelWriter.cpp:345-355). Measured `annotation(G) == pixels(G-1)` on every blink edge: blinking toggles in the
  INJECTOR's tick (AnomalyInjectorSubsystem.cpp:169-173), a different tickable running AFTER capture, so m18's
  end-of-our-Tick sample predates it. (`missing_object` immune — hides in `Apply` inside our Tick. ⇒ hide-type splits:
  Apply/Revert-driven = correct; self-ticking = stale. This is the m18/G78 predicted risk, now measured.) **FIX: new
  `SampleDeferredHidden()` fills `FireHidden` at the TOP of the next Tick** (before `ProcessCompletedFrames`; also at
  `FinishRun` top). **Zero blast radius on labels.jsonl — `FireHidden` never reaches it.** **Bug C = TWO independent
  causes, NOT B's root** (a uniform shift preserves transition counts, so B's fix changes C by nothing): (C1) the
  transition count was ORDER-DEPENDENT (accumulated on arrival, but `Drain_RenderThread` appends REVERSE →
  out-of-order → spurious transitions → "flicker") → now `TMap HiddenByIndex` + derive hidden set AND transitions from
  the SORTED keys at write time; (C2) `MapAnomalyToClient` else-branch never set a subtype → now mirrors the type.
  *Note: "single vanish → flicker" did NOT reproduce — `DefaultHz=5.0f` (6-frame period) means the default 8-frame window
  genuinely holds ~2 hidden blocks, so "flicker" is CORRECT there.* **Gates G1–G6 GREEN in a LOCAL PACKAGE, pixel-exact:**
  G1 annotation==pixels==labels `[3..10]` (already true on m19); G2 blinking annotation `{4,5,9,10}` == pixels `{4,5,9,10}`
  exactly (was `{4,5,6,10}`), tail frame included; G3 single-vanish window → **14/14 `disappear_reappear`**, default
  multi-toggle → `flicker` (derivation intact, still data-driven), missing_texture → `missing_texture`; G5 **labels.jsonl
  BYTE-IDENTICAL to m19** (same pattern/cadence/counts — m18 undisturbed); G6 overlay clean (65 present / 65 boxes / 0
  present-no-box). **PARKED (owner D1): the shared range-builder refactor** — single source of truth so this off-by-one
  class can't recur; deferred post-delivery (touches the m18-validated path). *Note: annotation + labels already share one
  snapshot and one accumulator, so the drift risk is lower than feared — it's an emit-layer consolidation.* **ALSO PARKED
  (found, unapproved): `frame_count` is a SPAN not a count** (`End-Start+1`, AnomalyLabelWriter.cpp:349) — measured 7 vs 4
  indices for gapped hide-type sets; ships in the client deliverable; owner decision. G81. **SHIPPED** — one
  `fix(capture)` commit `c01a214` + tag `m20`, pushed; dashboard untouched. *(Post-ship: the "Bug A" question was
  pursued through two more owner hypotheses — delivery mode, then content clock — both refuted; the real root is the
  m21 arm→present pairing race above, which also explains why the owner's box showed −1 while this box did not.)*
  → `docs/sessions/2026-07-15-026-m20-annotation-labeling.md`.
- **Latest milestone (as-built): m19 — preview backbuffer tee + targeting defaults — COMPLETE (commit `c8aa3fa`, tagged
  `m19`, pushed) (2026-07-15).**
  Fixes the **black dashboard preview in ANY packaged build** (delivery-gating: no client build had a working preview).
  **Two premise corrections, both evidence-backed (docs must not restate the myths):** (1) the preview was **NEVER
  editor-gated** — no `WITH_EDITOR` guard exists; the only guard is `ANOMALY_CONTROL_SERVER` (= not-Shipping), so the
  packaged build compiled AND RAN it; (2) frames were **NOT "never generated"** — 117 frames/20 s were generated,
  encoded and SENT, each **exactly 15027 B**, decoding to a valid 1280×720 image of mean **0.00** = BLACK. ⇒ **a
  "frame counter increments" gate PASSES on the broken build; gate on PIXELS.** Cause: a packaged viewport has no
  render target → `ReadPixels` zero-fills and reports success (G79). **Fix:** the preview tees off the same
  `OnBackBufferReadyToPresent` stream as capture via its **own** `FAnomalyFrameCapturer` instance (new
  `FAnomalyPreviewTee`) — it cannot share capture's grab (m16 makes them mutually exclusive in time; the capturer's
  arm/queue are single-consumer). Capturer class **unmodified**. `UAnomalyCaptureSubsystem` owns the tee + a 3-method
  facade (`PreviewPump`/`PreviewArm`/`PreviewPoll`) so the control server gains **no render deps**. **m16 suppression
  gates the ARM, not just the send** (pump still drains-and-discards so an in-flight readback can't leak). Encode
  off-thread; WS send stays on the game thread; **no `FlushRenderingCommands` left in the path**; `ViewEpoch` stamped
  at arm. `ConvertTightToBGRA` promoted to `namespace AnomalyLabel` so capture + preview share ONE conversion.
  **Dashboard UNCHANGED** (AIF1 format identical). **Gates in a LOCAL PACKAGE:** G1 preview WORKS — pixel-verified,
  ~5.8/s, JPEGs ~71 KB and 60/60 **distinct** (vs the constant-15027 B black fingerprint), real scene decoded, and the
  `InRHITexture` ensure is GONE; G2 suppression — **13.2 s capture running → 0 preview frames**, 51 after; G4 capture
  **byte-identical to m18** (100 frames / 65 pos / same pattern / same cadence / same annotation `[3..10]` / 9 events);
  G5 format `fmt=18` `PF_B8G8R8A8` correct on StackOBot; G6 no render-thread stall, ~5% fps delta **inside the noise
  floor** (baseline itself 68–105 fps). **G3 (PIE) OWNER-PENDING** — needs the editor; same hook/rect/capturer class
  that capture already proves in PIE. **HONEST PERF (no overclaim): m19 does NOT speed up capture** — m16 already
  suppressed preview during captures; the win is a working packaged preview + no ~6 Hz game-thread flush OUTSIDE
  capture (structural; unmeasurable on the light local scene). **KNOWN LIMITATION — Concorde/HDR format is an owner
  post-push check** (mirrors m17): `ConvertTightToBGRA`'s `default:` branch returns BLACK, so any title whose captures
  are correct will preview correctly (Concorde's captures are good ⇒ inference, not measurement); a true HDR
  `PF_FloatRGBA` swapchain would look washed, not black. **Exact place to add a format/tonemap step if needed:
  `AnomalyLabel::ConvertTightToBGRA` — it now fixes capture AND preview at once**; the `Preview(tee): first backbuffer
  frame (fmt=…)` log makes it a 5-second check.
  **BUNDLED INTO m19 (owner) — NEW TARGETING DEFAULTS: coverage `0 → 6`%, poll radius `0 → 1800` cm (18 m), auto-pool
  default-enabled `{missing_object, blinking, missing_texture} → {blinking, missing_texture}`.** All three were
  **hardcoded engine constants** (`GPollRadius`/`GMinScreenCoveragePct` in `AnomalyViewport.cpp`; a new
  `GAutoPoolDefaultEnabled` consumed by `AnomalyAutoInjectorSubsystem::Initialize`) — **not** ini-backed (GConfig
  remains a follow-up option). **The ENGINE IS AUTHORITATIVE and the dashboard has NO defaults of its own** — its
  sliders/checkboxes are pure snapshot mirrors (`session.pollRadius`, `session.minScreenCoverage`, `auto.pool[id]` ←
  `IsAnomalyEnabled`), so **zero dashboard changes were needed and the dashboard repo stays untouched at `f978f1b`**;
  adding a UI-side default would have created a second source of truth that could drift. `missing_object` stays
  **selectable, just not default-on**; `SetAllAnomaliesEnabled(true)` still means all of `GAutoPool`. **UNITS:** poll
  radius is **cm** (18 m = 1800); coverage is **percent 0–100** (6% = `6.0f`). **Verified in a FRESH PACKAGED session,
  no dashboard:** `1800.0 cm (cull ON)` / `6.00% (cull ON)` / `enabled pool (2): blinking, missing_texture`; snapshot
  parity confirmed. **6% was measured, not guessed** — the Bot is 9.98% of the viewport (= `annotation.json`
  `coverage_ratio: 0.10026`), so 10%/12% CULL THE TEST SCENE'S HERO CHARACTER; 6% keeps it (15→5 targets, Bot in).
  **⚠ The MainMenu set collapses to 1 under the POLL radius (not coverage) — a MENU-MAP ARTIFACT:** the poll cull is
  pawn-relative (G34) and a menu map's pawn is nowhere near the menu camera; in a real gameplay level the pawn IS the
  player. **The poll default therefore cannot be judged in this map — owner sanity check wanted in a gameplay level /
  on Concorde.** G80. **SHIPPED** — one `feat(capture)` commit `c8aa3fa` + tag `m19`, pushed; dashboard untouched
  (`f978f1b`) because it has no defaults of its own. → `docs/sessions/2026-07-15-025-m19-preview-backbuffer-tee.md`.
- **Prior milestone (as-built): m18 — burst-boundary label alignment (async label stamp → end of tick) — COMPLETE
  (commit `4559c8c`, tagged `m18`, pushed) (2026-07-15).** Fixes the ~17% label/pixel misalignment found while validating m17 —
  same dataset-poisoning class, and it hit EVERY anomaly type. **Mechanism:** `CaptureCurrentFrame()` sampled the label
  from `GetLiveFires()` at mid-tick N, but `BeginFire()`/`BeginRevert()` run LATER IN THE SAME `Tick`, and the **async**
  grab (default) returns the render of the ARM TICK ITSELF (frame N) → the frame armed on a transition tick renders the
  POST-transition world while its label described the PRE-transition one. Measured: pixels positive [3..10] vs labels
  positive [4..11] → **the label span ran one frame LATE**; fire edge = labeled clean / pixels anomalous (**false
  negative — a "clean" example containing the bug**), revert edge = labeled positive / pixels clean (false positive).
  Scale = **2/(PositiveFrames+PostFrames) = 16.7%** at defaults (17/100 measured), independent of settle-K and pre.
  **⚠ DIRECTION NOTE: the brief's diagnosis ("pixels change on N+1 → shift the span LATER") was the exact inverse of the
  measurement; the fix shifts the span one frame EARLIER.** The render thread's lag is a THREAD lag, not a state lag — a
  change at end-of-tick-N lands in frame N's own render. **Fix (async-only; `AnomalyCaptureSubsystem.{h,cpp}` only):**
  the arm stores the snapshot WITHOUT the fire state; new **`FinalizeArmedLabel()`** (last statement of `Tick`, after
  the phase switch) fills `Fires`/`FireHidden`/`FirePos` post-transition. **Hide vs non-hide needs no special-casing** —
  `anomaly_present`, the bbox/`bbox_valid`, `AffectedFrames` (non-hide `frame_indices`) and `HiddenIndices` (hide-type)
  all derive from that ONE sample, so every surface shifts coherently and `visible_positive` stays consistent by
  construction. **Sync is untouched and was already correct** (its `ReadPixels` returns the PREVIOUS frame — the same
  fact L=0 rests on); **phase timing, settle-K, L=0 and the view ring are byte-unchanged.** **Gates G1–G6 GREEN in a
  LOCAL PACKAGE:** 0/100 mismatches (was 17/100); hide-type `frame_indices`=[3..10]=hidden frames; non-hide
  `frame_indices`=[3..10] non-empty; **frame cadence byte-identical pre/post** (proves settle-K/phase timing untouched),
  positives 64→65 = exactly +9 FN −8 FP corrected; `verify_capture.py` clean (65 present / 65 boxes / 0 present-no-box)
  + visual confirmation at both corrected edges. **SHIPPED** — one `fix(capture)` commit `4559c8c` + tag `m18`, pushed
  (carried the m17-Concorde docs flip too). **OPEN (same root, NOT fixed):** the VIEW half — async grabs
  camera N while the ring yields camera N-1 → bbox predicted 1 frame stale under camera motion (unmeasured; static-camera
  scene; `IAI.Capture.ViewLag` is the knob G41 reserved for it). Also open: bbox projected from LIVE actor bounds at
  record-build time (wrong for moving targets); `blinking` toggle edges vs tickable order; **sync capture writes BLACK
  frames in a package** (same `ReadPixels` root as m19). G78.
  → `docs/sessions/2026-07-15-024-m18-label-alignment.md`.
- **Prior milestone (as-built): m17 — missing_texture revert hardening for runtime/modular-character materials —
  COMPLETE (commit `e2c6dd2`, tagged `m17`, pushed; on top of `m16` `84dfa52`) (2026-07-15).
  ✅ CONFIRMED ON THE REAL TITLE — no open items.** The owner pulled + rebuilt m17 on the office box and verified
  `missing_texture` apply → revert on Concorde's **actual `FWMasterSkeletalMeshComponent`**: the body reverts clean both
  for an **immediate** revert and for the **CHURN** case (apply → ~30 s play, character system re-creates the body
  mid-hold → revert — the case that previously stuck). The slot reset STICKS on the real merged/master-pose proxy; the
  character system does not re-assert the checker back. **The D4 question is RESOLVED and the modular-proxy follow-up is
  NOT needed** (G77 closed; its outcome map is retained as regression history only). Also validated on the LOCAL
  StackOBot repro in a package: stuck revert clears immediate + after-churn; `revert_all` clears; regression
  byte-identical on plain props/`SKM_Bot` incl. a real game MID restored as the same object; the our-material-only guard
  leaves a game-re-asserted MID untouched. Fixes the confirmed Concorde bug (body-only stuck corruption;
  `IAI.RevertAll` also failed): the anomaly restored through a saved component ptr + a saved original material ptr, and
  on characters whose own runtime logic re-creates the component and/or its MIDs (Concorde's
  `FWMasterSkeletalMeshComponent`) BOTH went stale during the hold → the stale-skip silently skipped → corruption
  persisted while frames were labeled CLEAN (dataset contamination). **Fix (only `Anomaly_MissingTexture.{h,cpp}`;
  `IAnomaly`/injector/other anomalies/capture loop byte-unchanged; no new dep; catalog stays 8):** Apply additionally
  records each slot's **owning actor + component `FName`** (+ the applied checker, since `Revert()` has no world
  access); Revert **re-finds the live component** (saved ptr → else same-named on the owner), **guards** (touch a slot
  only if it still holds our checker, incl. a MID whose parent chain reaches it — never stomp a material the game
  re-took), restores the saved original if alive **else resets to the mesh built-in default** (`SetMaterial(i,
  nullptr)`) so the game re-takes ownership, then **sweeps** every live mesh component of each touched actor for
  leftover checker (catches corruption copied onto a successor component we never captured). Every revert now logs
  `restored/default-reset/left-to-game/unresolved/swept/re-found` — **no more silent failures**. G74–G77.
  **D4 finding (repro-derived, since CONFIRMED on the real component):** the correct restore target is *whatever
  components are live on the actor at revert time* (not "the master" vs "the sub-parts") — one revert handled a master +
  master-posed sub-part with different dispositions; the restore must NOT survive the character system's next
  re-assertion (yielding the slot IS correct). Limits (untested rather than known-broken): per-actor only (sub-parts on
  a *different* actor are out of reach); a swept successor can only be default-reset.
  **Gates G1–G5 all GREEN in a LOCAL PACKAGE** (`Builds\MidRepro`, headless via WS): immediate + after-churn revert
  clean; `revert_all` clean; guard leaves a game-re-asserted MID untouched; **regression byte-identical on plain
  static/skeletal content incl. a real game MID**; targeted capture reverts within every burst (verified at pixel
  level via PNG decode). **Repro harness is a VALIDATION ASSET, NOT in this repo:**
  `D:\IntrusiveAnomalies\StackOBot\Source\StackOBot\MidReproActor.{h,cpp}` (project game module) +
  `SOB.MidRepro.*` console commands; the plugin repo tracks zero test files. **SHIPPED** — one
  `fix(missing-texture)` commit + tag `m17`, pushed (pushed BEFORE the Concorde test on purpose: the fix must reach
  the office box via GitHub before the real component can be exercised). Comment stripper run pre-commit: 0 changed /
  59 no-change. → `docs/sessions/2026-07-15-023-m17-missing-texture-revert.md`.
- *(The burst-boundary label misalignment found while validating m17 is now **m18**, above — built this session,
  awaiting review. Note the fix landed on the label-stamp timing, NOT on the phase transition: the anomalies already
  fire at the right time.)*
- **DEFERRED to m18 (diagnosed + locally reproduced, NOT started): packaged black dashboard preview.** Title-independent
  (repros on local StackOBot package). The preview's `FViewport::ReadPixels` reads a game-viewport render target that
  **does not exist in a package** (packaged viewports render straight to the swapchain; `GetRenderTargetTexture()` is
  null → D3D12 `RHIReadSurfaceData` zero-fills and returns "success" → ~6 valid all-black 15 KB JPEGs/s on the wire,
  frame counter ticking). PIE has a separate RT → works → editor masked it. Fix shape: TEE the preview off the
  **backbuffer** stream the capture path already proves out (`OnBackBufferReadyToPresent` + GPU readback, throttled +
  JPEG off-thread) — this also IS the long-deferred async-preview upgrade (kills the synchronous
  `FlushRenderingCommands` game-thread stall). Must gate **arming** on `IsCaptureActive()` (preserve G73) and use its
  OWN capturer instance (arm-match + Completed queue are single-consumer). **Delivery-gating: the preview is black in
  ANY client package.**
- **Prior milestone (as-built): m16 — three capture-delivery fixes — COMPLETE (tagged `m16` `84dfa52`, dashboard
  `f978f1b`) (2026-07-13).**
  (1) **Client token auto-populate** — `AnomalyControlServerSubsystem::StartListening` reads `[AnomalyControlServer] Token`
  from `DefaultGame.ini` via GConfig (present → fixed token; absent/empty → the existing random per-session GUID + log line,
  so the owner in-editor is byte-unchanged). The dashboard bakes a matching `VITE_CONTROL_TOKEN` (via `.env`) and
  auto-connects with zero client copy-paste; also persists the last-used token in localStorage so the owner stops re-pasting.
  Static shared secret, localhost-only tradeoff owner-accepted (G71). (2) **Focus-gated capture start** — a Start ARMS
  immediately but holds the first frame until the game window has foreground focus (new `ECapturePhase::ArmedPending`
  resolved in `Tick`; focus = `FViewport::IsForegroundWindow`); the timing bundle (StartFrame/manifest/fixed-timestep) is
  deferred out of `StartRun` into new `BeginActualRun` at focus-in; cancel-before-focus writes nothing + deletes the empty
  session dir (`bRunBegun` guard). Skipped when there is no game window (headless/**Simulate → bridge gates don't deadlock**);
  `IAI.Capture.FocusGate <0|1>` override + `[AnomalyCapture] bFocusGateDefault` + 30 s safety timeout (G72). (3)
  **Preview-pause hardening** — the control server's `PushFrames` suppresses live-preview JPEG generation while a capture is
  active (engine-side, immediate, no snapshot round-trip), so the synchronous preview `ReadPixels` can't drag sustained fps
  (G73). A single `bRunning`/`IsCaptureActive()` signal (true arm→finish, armed-pending included) drives both the focus-gate
  and the preview suppression. **Catalog stays 8; no new module dep** (GConfig=Core; focus via Engine `FViewport`;
  AnomalyCapture already links Slate in non-Shipping). **Gates:** dashboard `npm run build` GREEN (tsc+vite; baked-token
  inlining verified); owner-gate GREEN in-editor on real hardware; plugin compiled clean. **SHIPPED** — plugin `84dfa52`
  tag `m16` pushed, dashboard `f978f1b` pushed (untagged per dash precedent), both trees clean.
  → `docs/sessions/2026-07-13-022-m16-capture-delivery-fixes.md`, `docs/client-delivery.md`.
  *(Field note from the first packaged smoke test: the m16 focus-gate + preview suppression are sound, but the preview
  itself is black in ANY packaged build for an unrelated reason — see the m18 entry above; the G73 suppression is not
  implicated.)*
- **Prior milestone (as-built): Content-clock default reverted to WALL — COMPLETE (tagged `m15`) (2026-07-13).**
  Small settle-milestone on top of m14: flips the `IAI.Capture.ContentClock` **default back to `wall`** (m14 had briefly
  shipped `game` on an owner override pending validation). RESOLVED by the owner testing wall vs game on the actual office
  machine: **the client titles (Until Dawn/Concorde) are WALL-clock** — wall gives correct-SPEED video (length varies with
  real capture duration = correct for wall-clock content); the earlier Fps 120/240 "slow motion" was an extreme-forced-ratio
  artifact, not game-clock evidence. Wall default is client-safe (a `game` default would play their real-time-clock videos
  ~2× FAST = the Issue-2 regression). **StackOBot is game-clock → set `game` via its build's `DefaultGame.ini [AnomalyCapture]
  ContentClockDefault=game`.** One-line code change (`EContentClock` member init `Game→Wall`; GConfig-absent fallback follows)
  + doc correction to the settled state (journal 021 closes journal 020's open item). Re-verified: fresh session, no ini key
  → clock=wall. The m14 machinery (game/wall stamp branches, warnings, setting, run_summary `content_clock`) is otherwise
  unchanged. Catalog stays 8. → `docs/sessions/2026-07-13-021-m15-content-clock-default-wall.md`, `docs/capture-fps.md`.
- **Prior milestone (as-built): Content-clock-aware fps stamp — COMPLETE (tagged `m14`) (2026-07-13).**
  Fixes game-clock captures playing `speed_ratio`× SLOW (the m11 honest stamp always stamped the sustained wall rate,
  which is correct for real-time content but wrong for game-clock content under fixed step, where every frame is an exact
  `1/target` game-slice → the natural stamp is TARGET). New setting **`IAI.Capture.ContentClock <game|wall>`** (mid-run
  guarded), **default `game` at m14 → REVERTED to `wall` in m15 (see above)**, packaged default `DefaultGame.ini
  [AnomalyCapture] ContentClockDefault` (GConfig at Initialize; same mechanism as delivery mode). **game** = stamp TARGET at
  any ratio (a high ratio only means the LIVE capture ran slow — perf issue, not a video defect); **wall** = unchanged m11
  behavior (ratio>tol → sustained). `run_summary.json` gains `content_clock`; annotation client-clean. (m14's "mixed-clock
  UNRESOLVED / client FAST-risk OPEN" note is now CLOSED by m15: the client titles tested wall-clock; default is wall;
  StackOBot uses game via ini.) Only AnomalyCapture (stamp branch + warnings + setting) + the run_summary field changed; fixed timestep / pacing
  / labeling / ground-truth UNCHANGED. All 5 gates + end-to-end mp4 GREEN (game 60→2.0s natural; wall→sustained fractional;
  ini default; mid-run guard; bad-token reject) + default-flip re-verify. G70. Catalog stays 8.
  → `docs/sessions/2026-07-13-020-m14-content-clock.md`, `docs/capture-fps.md`.
- **Prior milestone (as-built): Client delivery mode — COMPLETE (tagged `m12`) (2026-07-12).**
  A capture DELIVERY MODE for shipping the plugin to an external client who runs capture in their own build (no
  post-processing between their capture and them → whatever capture writes IS what the client gets). `bDeliveryMode`
  **default OFF** (full fidelity, byte-identical to m11 except the D3 annotation change). Console
  `IAI.Capture.Delivery <0|1>` (mid-run guarded); packaged default read at Initialize via GConfig from the project
  `Config/DefaultGame.ini` `[AnomalyCapture] bDeliveryModeDefault=True` (GConfig caches at startup → edit needs an
  editor restart; console overrides per session, no SaveConfig; chose GConfig over UDeveloperSettings to avoid a new
  dep/UCLASS — G69). **ON writes ONLY** `Actual_Frames/` + `Video_Clip/` + `run_summary.json` + `annotation.json`;
  **suppresses** `labels.jsonl` + `run.json` (never created — label record still COMPUTED, uniform path; threaded
  `bWriteLabels` through FJob→EncodeAndWriteFrame async + CaptureLabeledShot→AppendRecordAndImage sync, image always
  written). run_summary kept (encode_watcher's done-signal); seed lives only in run.json → delivery withholds it →
  session NOT client-reproducible (intended — G68). **D3 (both modes, always):** removed `schema_version` +
  per-anomaly `source_id` from annotation.json (+ dead-field tidy). **D4:** run_summary gains a `delivery_mode` bool.
  Manual `IAI.Capture.Shot` UNAFFECTED. Our QA tools (overlay_watcher.py/verify_capture.py) no-op on delivery
  sessions BY DESIGN (need labels.jsonl — G67); encode_watcher unaffected. No new module dep (GConfig=Core); no
  dashboard change (packaging-time decision, console+config only). All 5 bridge gates GREEN (OFF regression / ON
  file-set + end-to-end mp4 / GConfig default / mid-run guard / annotation strip; both async+sync; 0 drops); fully
  bridge-verifiable, no owner eyeball. Catalog stays 8. Files: AnomalyCaptureSubsystem.{h,cpp},
  AnomalyLabelWriter.{h,cpp}, AnomalyAsyncWriter.{h,cpp}. → `docs/sessions/2026-07-12-018-m12-delivery-mode.md`,
  `docs/client-delivery.md`.
- **Prior milestone (as-built): Capture pacing + honest fps stamping — COMPLETE (tagged `m11`) (2026-07-11).**
  Fixes the Issue-2 office "2x-fast mp4" on real-time-clock-driven client games. **Two-clock model (G64):** fixed
  timestep (m10-era) pins only the GAME clock; real-time-driven content (client sequencer/audio-synced scenes) runs
  on the WALL clock, so with fixed-step alone the mp4 plays fast by `VideoFps / sustained_wall_fps` (StackOBot is
  game-clock-driven → was always exact). **Fix = real-time pacing** `IAI.Capture.Pace <0|1>` **default ON**: a
  drift-free coarse-sleep+spin at the top of `UAnomalyCaptureSubsystem::Tick` holds every tick to ≥ `1/VideoFps` wall
  → game == wall == video clock, correct for BOTH families (UE's own limiter is bypassed under fixed timestep, so
  ours is the only pacer — G65). **Fallback = one-sided honest stamp:** every armed frame wall-stamped (`t_wall` per
  labels.jsonl row, both async+sync); at finalize `speed_ratio = wallSpan/gameSpan` (same first/last armed frames,
  settle gaps cancel), `sustained = VideoFps/ratio`; ratio > 1.02 → `annotation.video.fps` = sustained (fractional,
  3dp; encode watcher float-parses) + warnings; else fps = VideoFps exactly (never stamp faster-than-target).
  `video.target_fps` always written; `run.json` += target_fps/paced; `run_summary.json` += target_fps/
  sustained_wall_fps/speed_ratio/stamped_fps/paced. NO frame dup / NO VFR (1:1 mapping inviolate). WS
  `capture_stopped`/`capture_status` carry `{targetFps,stampedFps,speedRatio,paced}`; dashboard shows a post-run
  badge on fallback (own untagged feat commit). All 5 bridge gates + owner eyeball GREEN (G-P1 paced 30 exact int
  stamp; G-P2 throttled 60 → 58.055 fractional end-to-end via encode_watcher+ffprobe; G-P3 Pace-0 keeps 30; G-P4
  zero drops; G-P5 sync t_wall coherent). Warm-up + background-editor-throttle skew measurements (G66). Deferred to
  possible m11.1: hitch-robust median ratio (2% constexpr tol stands). Catalog stays 8.
  → `docs/sessions/2026-07-11-017-m11-capture-pacing.md`, `docs/capture-fps.md` (rewritten).
- **IN FLIGHT (branch `feature/stencil-capture` off `master` `d4a77db`, NOT on `master`): Occlusion-correct stencil bounding boxes + async unified capture.**
  Multi-stage; **Stage 1 COMPLETE, owner re-eyeball GREEN (2026-06-30), committed on the branch** (`refactor(capture)`, no tag). A new
  **quarantined `AnomalyCapture` module** (gated `ANOMALY_CAPTURE`; render/`RHI`/`RenderCore`/`Slate`/`SlateCore`/`ApplicationCore` + a
  `bBuildEditor`-only `UnrealEd` dep, all compiled OUT of Shipping — `Renderer`/Renderer-private deferred to Stage 3) extracted from
  `AnomalyControlServer`: the m7 capture (`UAnomalyCaptureSubsystem` + `AnomalyLabelWriter` + `AnomalyPreviewCapture`) **MOVED** there with
  its own log cat `LogAnomalyCapture`; `AnomalyControlServer` now **depends on** `AnomalyCapture` (DAG: core ← AnomalyCapture ← ControlServer).
  New **async, non-blocking capture** that grabs the REAL player frame (**game UI IN**): `FAnomalyFrameCapturer` hooks
  `FSlateRenderer::OnBackBufferReadyToPresent` (post-Slate), clips to the game-viewport rect (FFrameGrabber `TargetWindowPtr`+`CaptureRect`
  pattern → no editor chrome even in docked PIE), stages an `FRHIGPUTextureReadback`, the render thread does only the lock-copy-out, and a
  **thread-pool `FAnomalyAsyncWriter`** does convert+encode+write OFF the game thread (G53 — fixes a per-frame stall/animation judder). Frame↔state
  carry keyed by submit `GFrameCounter`; `IAI.Capture.Async <0|1>` falls back to the sync `ReadPixels` path. **Only OUR overlays** are suppressed
  for a run via a generalized core flag `AnomalyViewport::SetOverlaysSuppressed` (poll-radius sphere + selector HUD/box + auto HUD + the heartbeat
  **actively evicted** each tick, G54); the PIE mouse-control-label is disabled **per-PIE-session** at subsystem `Initialize` (G55). A `DrainTail`
  FSM phase makes clean burst-count runs **0-drop** (G56). The m7 projected label box is UNCHANGED this stage (the stencil box is Stage 3; color and
  stencil are now two grab points joined by frame id — G52). **Clean 5.1 Dev-Editor compile (exit 0); core dep set unchanged (render deps quarantined);
  `IAnomaly` untouched; catalog stays 8.** Gotchas **G52–G57**; journal `docs/sessions/2026-06-30-015-stencil-capture-stage1.md`.
  **Next:** Stage 2 — custom-stencil tagging (`r.CustomDepth 3`, set/restore), then Stage 3 (stencil/depth SVE + occlusion-correct box), Stage 4 (multi-actor + docs + tag).
- **Latest milestone (as-built): Targeted capture modes + pre-run clean slate + entry-point parity — COMPLETE (tagged `m10`) (built 2026-07-10, closed 2026-07-11).**
  Capture runs fire in **targeted** mode (`IAI.Capture.Start [outDir] [png|jpeg] [seed] [maxFrames] [anomaly] [targetActor]`,
  `""` placeholders — G60; WS `capture_start {anomaly, target}`; new `UAnomalyAutoInjectorSubsystem::TryFireSpecific` — exact
  `=`-match, keeps all m6 guards, visibility-independent G61) or **auto-pool** (unchanged). `run.json` records
  `mode`/`target_anomaly`/`target_actor`. **Clean slate:** StartRun reverts auto live fires + `Injector->RevertAllActive()`
  (no unlabeled contamination — G63; contam gate green, `IAI.DumpActive`=0 after start). **Parity:** StartRun/FinishRun own the
  auto-injector pause/resume for BOTH entry points (`bDeinitializing` teardown guard — G62; WS-local pause/resume deleted).
  Dashboard is capture-first (Targeted/Auto-pool toggle; auto panel → "Capture pool"; Inject/Arg panels deleted; own feat
  commit in the dashboard repo). Also landed in the close turn: the m9-era follow-on `fix(capture)` (`6d4eb01` — client-shaped
  `affected_frames` object + seedless `session_<ts>` naming) and the previously-untracked `docs/capture-fps.md` (`16a5c19`).
  **NOTE:** "m10" in some earlier notes meant the untagged fixed-timestep capture-fps cluster (`c5d58b0`/`500eac7`/`417833a`) —
  that naming is CORRECTED: m10 = this milestone; the approved capture-pacing/honest-fps plan = **m11** (next).
  Catalog stays 8. → `docs/sessions/2026-07-11-016-m10-targeted-capture.md`.
- **Latest as-built (post-m8, NO tag — both on `master`, 2026-06-22): Screen-coverage candidate cull + its dashboard slider.**
  **(1) Cull** (commit `a96f8bb`) — an optional **actor-level** cull on the renderable-visible set in `AnomalyViewport`: an actor
  is an injectable target iff its on-screen footprint (the **clamped projected union** of its renderable-visible component bounds)
  covers **≥ P%** of the viewport. `IAI.SetMinScreenCoverage <pct>` (plain world-independent cmd; `P <= 0` = OFF = byte-identical) +
  `IAI.DumpCoverage` (ascending-coverage tuning diagnostic). Applied to **both** live entry points
  (`GetVisibleRenderableActors` + `GetVisibleRenderableActorInfos`) through a new shared per-actor classifier
  **`ClassifyRenderableVisibleLive`** (OFF = byte-identical in result **and** cost via the kept first-match short-circuit; ON = one
  union pass, no double tracing) so the `IAI.DumpVisible` set-identity gate holds with the cull ON. Reuses the clamped
  `ProjectBoundsToScreenRect` fed the visible-component union (NOT the m7 type-only/unclamped projector). Touches only
  `AnomalyViewport.{h,cpp}` + docs; **no `IAnomaly`/dep change**. Gotcha **G51**; journal
  `docs/sessions/2026-06-22-014-screen-coverage-cull.md`. **(2) Dashboard slider** (plugin `81bc841` + dashboard repo `8c148b6`) —
  surfaced as a **throttled live slider** on the Tier-2 dashboard via a new `AnomalyControlServer` WS command
  `set_min_screen_coverage {pct}` (→ `SetMinScreenCoveragePct`) + a `session.minScreenCoverage` snapshot field (→ `GetMinScreenCoveragePct`),
  cloned from the poll-radius precedent; server stays compiled out of Shipping. Session-level live value only — **NOT** per-actor
  `FRenderableActorInfo` (still deferred). New dashboard `src/lib/throttle.ts` (~10/sec + authoritative send on release; numeric %
  snapshot-bound). **No new anomaly — catalog stays 8** (cull = targeting infrastructure, slider = UI). No tag, no version bump
  (same framing as the poll-radius pair). FF-merged into `master` in both repos; review branches deleted.
- **Latest milestone (as-built): Missing-Texture Anomaly — COMPLETE (tagged `m8`, VersionName 0.9.0) (2026-06-21).**
  New **8th** anomaly **`missing_texture`** (object-scoped, `Private/Anomalies/Anomaly_MissingTexture.{h,cpp}`): per-component
  `UMeshComponent::SetMaterial` swaps every renderable static/skeletal mesh slot to a plugin-**shipped Lit gray/white UV-checker**
  material (per-component override = object isolation, never touches the shared mesh/material asset; per-slot original +
  `bWasExplicitOverride` captured for an exact revert; skip stale). **First `Content/` asset in the plugin** — cook guarantee =
  a **CDO hard-ref** (`ConstructorHelpers::FObjectFinder` → non-transient `UPROPERTY TObjectPtr` on `UAnomalyInjectorSubsystem`)
  + flip `"CanContainContent": true`; **no host `DefaultGame.ini`** (G45). Found the hard way: the cook runs on **editor** binaries
  (rebuild before cooking — G47); 5.1 **IoStore** puts cooked assets in `.ucas`/`.utoc`, not `.pak` (verify by runtime load — G48).
  Material declares all mesh **usage flags** (skeletal/nanite/ISM/morph/spline) or it renders **default-gray** at runtime (G49).
  Reproducible authoring via `tools/create_missing_texture_materials.py`. Wired: `Register()`; `GetAuthoredSpec` (Object, **no
  args**); added to the selector `GAnomalyChoices` + the auto `GAutoPool` (`NumPoolKeys` 4→5, key `5`/`pool5`). **`IAnomaly`
  untouched; deps `Core/CoreUObject/Engine/InputCore`; catalog 7→8.** Gates driven green over the bridge: G-Compile (DumpCatalog=8),
  G-Apply (static multi-slot `SM_Ramp3` + skeletal `SKM_Bot`, exact revert incl. both override branches), G-Isolation
  (`SM_RockFlats_02`/`M_Rock` sibling untouched), G-BornComplete (selector cycle+inject, auto FireOnce, a 14-burst capture run).
  **DEFERRED — the flat-magenta variant + a `mode` arg:** unlit-emissive magenta lit the Lumen scene ("glowed" onto neighbours);
  the canonical fix is Lit base-colour but the owner is revisiting the look (G50). One `feat(missing-texture)` commit, tagged
  **`m8`**. **NOTE:** the unreal-mcpython bridge was unstable this session (crashed the editor on some calls) — the lit-checker
  live render is owner-eyeball-pending. → `docs/sessions/2026-06-21-013-missing-texture.md`.
- **Prior milestone (as-built): Labeled Frame-Capture + 2D BBox Labeling — COMPLETE (tagged `m7`, VersionName 0.8.0)
  (2026-06-20).** A capture/labeling layer producing an ML-friendly **labeled image sequence** from a LIVE
  auto-injection run (labels = the injector's OWN ground truth, L1). New **`UAnomalyCaptureSubsystem`** + `AnomalyLabelWriter`
  **housed in the `AnomalyControlServer` module** (reuses its game-viewport capture primitive + ImageWrapper; gated by
  `ANOMALY_CONTROL_SERVER`, compiled out of Shipping — dataset capture is a dev/research activity in a packaged
  Development/Test build, never retail Shipping). Drives the m6 deterministic core in **capture-driven bursts**
  (`[pre] → FireOnce → [settle K] → [positives] → RevertAllLiveFires → [settle K] → [post]`, looped); per captured frame
  writes `frame_<GFrameCounter>.png` (opaque, native res) + a JSONL label record + `run.json`/`run_summary.json`, all
  same-tick + `GFrameCounter`-stamped (exact image↔label alignment). The 2D bbox projects the fired actor's PERSISTED
  bounds (works when hidden). **Three sanctioned core exposures only — `IAnomaly`/injector/anomalies/leaf-helpers/`=`-match/
  `GetVisibleRenderableActors` byte-clean:** `AnomalyViewport::ProjectActorBoundsToScreenRect` (type-only bounds union,
  NOT `IsVisible`-gated — G38); `FAutoLiveFireInfo` widened with `TargetActor` + `StartFrame`; `RevertAllLiveFires()`
  exposed (keeps `GetLiveFires()` accurate). **No new dep; catalog stays 7.** Settle-K SYMMETRIC at both boundaries (G37);
  view-lag **L=0 validated** (tickables tick before the camera update → the view already lags 1 frame; L=0 = "1 render-frame
  back", FPS-invariant — G41); **`visible_positive`** = present + a valid box (off-screen-during-hold frames kept as hard
  negatives — G42). `tools/verify_capture.py` overlays boxes (Pillow). Gates 1/2/3 GREEN incl. owner moving-eyeball.
  **Closed as TWO commits (Plan A):** `ff1be3c` `feat(control-server): Slice-1 dashboard …` (the parallel track's
  uncommitted Slice-1 WIP promoted first, NO tag — it owns the shared capture primitive) + the m7 commit on top (tagged
  **`m7`**). The async backbuffer capture path (`OnBackBufferReadyToPresent` + GPU readback) is the documented superseder,
  REQUIRED before framerate-bug anomalies + for exact-under-motion (G40). → `docs/sessions/2026-06-20-012-frame-capture-labeling.md`,
  `docs/post-m7-capture-labeling-handoff.md`.
- **Prior as-built (post-m6 viewport fixes, 2026-06-20):** two surgical, owner-locked fixes to the shared
  **renderable-visible set** in `AnomalyViewport` (the one source of truth consumed by the M5 selector, the m6
  auto-injector, AND — new since m6 — the control-server **A4 read-back** `GetVisibleRenderableActorInfos` + the
  `IAI.DumpVisible` set-identity gate). **(1) VFX removed (G33):** dropped the `UFXSystemComponent` clause from
  `IsRenderableComponent` → the set is **SM ∥ SK only** (reverses the G29/R1 VFX inclusion; HARD REMOVE). The `=name`
  console escape hatch still reaches VFX actors (it bypasses the predicate). **(2) Changeable poll-radius distance
  cull (G34):** `IAI.SetPollRadius <cm>` adds an optional cull — actor in the set iff renderable AND within R of the
  **player pawn** (sphere-approx bounds metric) AND in-frustum AND unoccluded; `R <= 0` disables it (default OFF,
  byte-identical); applied identically at both live entry points (DumpVisible MATCH preserved); a dev debug sphere
  visualizes R around the live pawn. **Only `AnomalyViewport.{h,cpp}` touched** (+ docs); `IAnomaly`/injector/anomalies/
  selector/auto/control-server cores untouched; **no new dep**, no `.uplugin` bump. **Clean Development-Editor compile
  on 5.1 (exit 0)** before each commit (control-server module re-links clean against the changed header). **Two atomic
  path-scoped commits, NO tag:** `9bbd398` `fix(viewport): remove VFX from renderable-visible set` +
  `<fix2>` `feat(viewport): add changeable poll-radius distance cull` (the uncommitted control-server WIP in the tree
  was left untouched). Owner smoke-test pending. → `docs/sessions/2026-06-20-011-viewport-vfx-removal-poll-radius.md`.
- **Prior as-built (control server, in flight):** the Tier-2 runtime control surface is under construction — committed
  slices `2645236` (transport spike: WS server + auth + loopback gate + backbuffer→JPEG) and `4c05344` (core read-back
  / A4: `GetVisibleRenderableActorInfos` + `FRenderableActorInfo`), plus uncommitted WIP. Separate `AnomalyControlServer`
  module with its own log category (G32). Not yet journaled as a milestone.
- **Latest milestone (as-built):** **Automatic Injection — COMPLETE (committed `41ba104`, tagged `m6`) (2026-06-19).** New
  **separate** `UAnomalyAutoInjectorSubsystem`
  (`Public/AnomalyAutoInjectorSubsystem.h` + `Private/AnomalyAutoInjectorSubsystem.cpp`, `UTickableWorldSubsystem`,
  Game+PIE) that auto-fires the **4** object-scoped anomalies **randomly on the renderable objects currently on-screen**
  (drawn from `AnomalyViewport::GetVisibleRenderableActors` + applied via the `=` exact-match token), each
  **auto-reverting** after a randomized hold. **Concurrent but collision-free by construction** (no coordinator) via two
  invariants: **(i)** one live fire per id (the registry's one-instance-per-id) + **(ii)** **one anomaly per actor**
  (`OVERRIDE-1` — subsumes both conflict groups *and* the hide-masks-LOD case, so **no id→group table**; supersedes the
  planning turn's per-group guard). All randomness from **one seeded `FRandomStream`** (console-settable seed, default
  time-based) on a **fixed draw protocol** independent of apply-result (R-SEED). **Explicit-core / thin-shell split (as
  m4/m5):** the deterministic core `AdvanceTime`/`TryFireOnce` is bridge-driveable as `IAI.Auto.Step`/`IAI.Auto.FireOnce`
  **without real time and without Enable/Run**; two thin shells drive it — the `IAI.Auto.*` console (bridge gate) +
  raw-input poll (keys `1-4`/`J`/`K`, distinct from the selector's) + a right-anchored immediate-mode HUD (eyeball).
  **Two switches, both default OFF → dormant → existing gates byte-identical:** `IAI.Auto.Enable <0|1>` (HUD/keys) and
  `IAI.Auto.Run <0|1>` (firing; forced OFF when !Enabled). Fires **auto-revert** after a randomized hold (R-LIFE;
  `IAI.Auto.Persist` flag, default off). **Self-scoping** — does NOT touch `IAI.SetViewportScoping` (warns if it is ON);
  no view → fire nothing (never blind). **Manual selector/console injection of a pool id during an auto run is
  unsupported → warn-not-block (R-COEXIST).** **No `IAnomaly`/injector/anomaly/leaf-helper change; no new dep**
  (`FRandomStream` = Core; deps stay `Core/CoreUObject/Engine/InputCore`); **catalog stays 7** (orchestration over the
  existing catalog). VersionName → **0.7.0**. **Clean Development-Editor compile on 5.1 (exit 0).** **Bridge state-gates
  GREEN (MainWorld Simulate):** deterministic headless fire + `=` exact-match (1 of 21 EnergyOrb siblings hit),
  auto-revert on hold-elapse, collision-free concurrent (3 distinct ids × 3 distinct actors, no 4th fire — invariants
  (i)+(ii)+cap), seed-reproducible target, OFF-regression byte-identical (`SM_Ramp`→2), both coexistence warnings fire
  without blocking. **Owner real-Play eyeball green — ACCEPTED.** → `docs/sessions/2026-06-19-010-auto-injection.md`.
- **Prior milestone (as-built):** **Object Selector + Inject UI (minimal) — COMPLETE (committed `aa2a3a4`, tagged `m5`) (2026-06-19).**
  A new **separate** `UAnomalySelectorSubsystem` (`Public/AnomalySelectorSubsystem.h` + `Private/AnomalySelectorSubsystem.cpp`,
  `UTickableWorldSubsystem`, Game+PIE only) that lets the player **select a visible on-screen object** (Tab-cycle over the
  **renderable-visible set** — frustum AND occlusion AND renders-to-screen) and **inject** one of the four object-scoped anomalies on it (default args), then
  revert — calling the existing injector's public `ApplyAnomaly`/`RevertAnomaly`. **Explicit-core / thin-shell split (as m4):**
  public methods `AdvanceSelection`/`SelectPrevious`/`CycleAnomalyChoice`/`InjectSelected`/`RevertSelected` + readbacks
  `GetSelectedActorName`/`GetVisibleActorNames`/`GetAnomalyChoice` are the bridge-gatable surface; two thin shells drive them —
  the `IAI.Selector.*` console commands (bridge gate) and per-tick **raw input polling** + an **immediate-mode HUD**
  (real-Play eyeball). Targeting is made exact by a new **`=` sentinel** in `AnomalyTargeting::FindActorsMatching`
  (leading `=` → `GetName().Equals(IgnoreCase)`; substring path **byte-identical** with no `=`); `InjectSelected` passes
  `"=" + GetName()` so it hits only the selected actor (the **only** leaf-helper change — additive; verify-item 5 pre-authorized).
  HUD = `UDebugDrawService::Register("Game", …)` (host-blind, no game HUD class — G25) drawing a visible-names list + an
  anomaly list + a `DrawDebugBox`/label on the selection; input = `WasInputKeyJustPressed`/`IsInputKeyDown` raw key state
  (no host mappings — G26); defaults Tab/Shift+Tab/C/G/H, rebindable via `IAI.SelectorBind`. Activation **`IAI.SelectorUI <0|1>`,
  default OFF → dormant → existing gates byte-identical.** **First dep since M0: `InputCore`** (FKey/EKeys; transitive via Engine,
  declared for IWYU) — **no Slate/UMG** (immediate-mode). **Renderable-target filter folded in** (m5 follow-on): the selector's
  visible set means **renderable-visible** — new additive `AnomalyViewport::IsRenderableComponent` (`IsVisible()` + a
  static/skeletal/`UFXSystemComponent` base-type allowlist; VFX caught with no Niagara dep) excludes volumes/spawn-points/
  debug/landscape (the m4 visibility funcs stay byte-identical); a HUD `LastInjectResult` line surfaces the AMB-2 zero-match;
  `GetVisibleRenderableActors` returns empty on no-view (offer nothing, never blind). This is the set **auto-injection** will
  consume (gotcha G29). **No `IAnomaly` change, injector subsystem + all 7 anomalies untouched;
  catalog stays 7.** VersionName → **0.6.0**. **Clean Development-Editor compile on 5.1 (exit 0).** Combined gate **green**
  over the bridge (MainWorld Simulate): selection cycles the name-sorted renderable-visible set; `=` exact-match inject
  hits exactly the selected actor (1 of 17 prefix-siblings); the renderable filter excludes RVTVolume / PlayerStart /
  GameplayDebugger / zero-instance-grass LandscapeStreamingProxy while keeping meshes + foliage + NiagaraActors +
  RoomBuilderSquare *(NiagaraActors/VFX were later removed from the set — G33, 2026-06-20)*;
  zero-match (Niagara + `lod_corruption`) surfaced; OFF-regression byte-identical (`SM_Ramp`→2,
  `=SM_Ramp2…`→1). **Owner real-Play eyeball green — ACCEPTED.** → `docs/sessions/2026-06-19-009-selector-inject-ui.md`.
- **Prior milestone (as-built):** **Viewport-Visibility Layer — COMPLETE (committed `7c34275`, tagged `m4`) (2026-06-18).**
  New shared helper **`AnomalyViewport`** (`Public/AnomalyViewport.h` + `Private/AnomalyViewport.cpp`,
  AnomalyTargeting/Args/Lod convention) = "is this object visible to the player" via **frustum AND occlusion**
  over an explicit view spec `FAnomalyViewInfo` (deterministic, synthetic-view-gatable) + a thin live resolver
  `GetActiveViewInfo` (first local player's POV; treat-as-unscoped + warn on no view). Occlusion backend (AMB-V1)
  = **multi-sample camera-to-bounds line trace** (`ECC_Visibility`, center+8 corners), private behind the
  backend-agnostic API; `GetLastRenderTimeOnScreen()` is the documented live backend for the future
  capture/live-injection milestone (.cpp-only swap — G22). New opt-in toggle **`IAI.SetViewportScoping <0|1>`
  (default OFF)** + diagnostic **`IAI.TestVisibility`** (synthetic-gate driver). The **4** object-scoped
  primitive-backed anomalies (`missing_object`, `flicker`, `lod_corruption`, `lod_popping`) consult the toggle and
  route through `AnomalyViewport` only when ON; `lighting_mismatch` + the two globals are excluded by design.
  **No `IAnomaly` change, no new module dependency** (frustum/traces/camera = Engine, `FReversedZPerspectiveMatrix` =
  Core; both locks held). **Clean Development-Editor compile on 5.1 (exit 0)**; over the bridge (MainWorld Simulate):
  synthetic frustum gate (behind→out, far→in, in-cone→in — reversed-Z VP validated, G24), synthetic occlusion gate
  (controlled wall: blocked→0 / clear→1 at frustum=1), and **OFF-is-byte-identical regression** (`missing_object`
  + `lod_corruption` round-trips M-identical, ListAnomalies still 7) all **green**. Catalog unchanged at **7**.
  VersionName → **0.5.0**. → `docs/sessions/2026-06-18-008-viewport-visibility-layer.md`.
- **Prior as-built:** **Refactor — "GDP" prefix removed from the plugin — COMPLETE + COMMITTED `351c7e8` (2026-06-18).**
  Pure mechanical rename, **no behavior change**: module/plugin/folder/`Build.cs`/`.uplugin` `GDPAnomalyInjector`→`AnomalyInjector`;
  `UGDPAnomalyInjectorSubsystem`→`UAnomalyInjectorSubsystem`; `IGDPAnomaly`→`IAnomaly`; `FGDPAnomaly_*`→`FAnomaly_*`;
  API macro `GDPANOMALYINJECTOR_API`→`ANOMALYINJECTOR_API`; log category `LogGDPAnomaly`→`LogAnomaly`;
  helpers `GDPTargeting/GDPArgs/GDPLod`→`AnomalyTargeting/AnomalyArgs/AnomalyLod`; console commands `GDP.*`→`IAI.*`.
  Project identity **"GDP: Anomaly Injection"** retained (code-prefix strip only; copyright/`CreatedBy` unchanged). Clean
  Development-Editor compile on 5.1 (exit 0) + light bridge re-gate green (module loads under the new name, `IAI.ListAnomalies`
  lists the **7** sorted under `LogAnomaly`, `IAI.Apply/Revert missing_object SM_Ramp` round-trips). One `refactor:` commit, **no tag**;
  bridge/host unchanged (G21). → `docs/sessions/2026-06-18-007-rename-strip-gdp-prefix.md`.
- **Prior milestone:** **M3 — LOD breadth fill — COMPLETE (committed `c54351a`, tagged `m3`).**
  `lod_corruption` extended to **static OR skeletal** meshes (same ID — one "LOD corruption" category; mesh
  type is an implementation detail), new ticking **`lod_popping`** (flicker mechanics), and a new shared
  helper **`AnomalyLod`** (`Public/AnomalyLod.h`+`Private/AnomalyLod.cpp`) absorbing the static/skeletal forced-LOD
  dispatch (2 consumers). Registry lists **7** (sorted). **No `IAnomaly` change** (M1 lock held again)
  and **no new module dependency**. Clean Development-Editor compile on 5.1 (exit 0); all 9 state gates
  driven green over the bridge in a `MainWorld` Simulate session — incl. the static **regression**
  (M2-identical), the **heterogeneous** apply (`lod_corruption Bot` = 1 static + 2 skinned in one apply),
  `lod_popping` oscillation, re-apply no-leak, RevertAll, teardown. **The Bot is single-LOD → skeletal
  anomalies are state-validated, no Bot visual** (G20). VersionName → 0.4.0.
  → `docs/sessions/2026-06-13-006-m3-lod-breadth.md`.
- **Prior as-built:** **M2.5 (UE 5.1 port) + M2.6 (bridge sever) — COMPLETE (2026-06-10).** **UE 5.1 is now
  the canonical engine** (the two real target games are on 5.1). Host = `D:\IntrusiveAnomalies\StackOBot`
  (natively-5.1); source engine = 5.1 at `D:\UESource\UnrealEngine`. The six anomalies compile clean on 5.1
  with **zero plugin-source changes** (all 7 port watch-items unchanged; only host-target build constants
  changed — G17), and all **10** stage gates were re-driven **green over the MCP bridge** + owner-confirmed
  visuals (flicker blink, magenta movable sun, near-clip). The `unreal-mcpython` bridge was ported to 5.1 by
  **severing its `BehaviorTreeEditor` dependency** (G8) — costs only the 2 BT-authoring tools.
  → `docs/sessions/2026-06-10-005-m2.5-m2.6-5.1-port-bridge-sever.md`.
- **Earlier:** **M2 — Breadth Round 1 — COMPLETE (all 8 stage gates passed).**
  Adds two shared helpers — **A1** `AnomalyTargeting::FindComponentsMatching<T>` (component targeting) and
  **A3** `AnomalyArgs` (parse/clamp/warn) — and three anomalies: `lighting_mismatch` (component, ULightComponent),
  `lod_corruption` (component, UStaticMeshComponent, static-only), `camera_clipping` (global near-clip).
  Registry lists **6** (sorted). **No `IAnomaly` change was needed — the M1 lock held.** Clean headless
  compile + gates 2–7 verified live in PIE `MainWorld` (unreal-mcpython bridge + owner eyeball, 2026-06-09).
  → `docs/sessions/2026-06-09-004-m2-breadth-round-1.md`, `docs/architecture.md`.
- **Resolved (M3):** **AMB-1 → skinned LOD count via `USkinnedMeshComponent::GetNumLODs()`** (runtime
  render-data count — the analog of static `GetNumLODs()`; not the asset's authored `GetLODNum()`) — G19.
  **AMB-2 → single tagged capture record keyed to the common base `UMeshComponent`** + `Cast<>` dispatch in
  `AnomalyLod` (not two typed lists); this is what lets one apply span a heterogeneous static+skeletal set.
  **AMB-3 → `lod_popping` default 2 Hz, ceiling 30 Hz.** Supersedes G16's static-only scope.
- **Resolved (M2):** **AMB-M2-1 → defer A2/`AnomalyCvar`** — near-clip is a console *command* + the
  `GNearClippingPlane` global, not an `IConsoleVariable`, so `camera_clipping` is self-contained (no
  `RenderCore` dep); AnomalyCvar lands with its first real cvar consumer (G13). **AMB-M2-2 → static-only
  `lod_corruption`** was the M2 stopgap; **resolved in M3** (static + skeletal via `AnomalyLod`, G19). M2 ships 2 helpers (A1, A3).
- **Resolved (M1):** **AMB-3 → capture-baseline** — `time_dilation` Revert restores the pre-Apply value.
  Generalized in M2 to the **per-target/global state-capture convention** (see architecture.md). G11.
- **In flight:** the **Tier-2 runtime control server** (`AnomalyControlServer` module — WS transport + A4 read-back;
  committed `2645236`/`4c05344` + uncommitted WIP in the tree; not yet journaled as a milestone). The two post-m6
  viewport fixes (G33 VFX removal + G34 poll-radius cull) are committed (`9bbd398` + `<fix2>`, no tag); owner smoke-test
  pending. **Next action:** finish the control-server slice, then the High-priority new bug types (born viewport-aware
  AND auto-injectable). The `flicker→blinking` rename is **DONE** (`refactor(blinking)`, no tag). Also still queued: a new `flickering` anomaly (scene-region / light toggling; handoff §2.3),
  region-darkening (§2.4), the selector's screen-X ordering polish. Bridge/host stay unversioned (G8 unchanged).
- Milestones: M0 (`…-001`), M1 (`…-003`), M2 (`…-004`), M2.5+M2.6 (`…-005`), M3 (`…-006`) fully passed
  + tagged; rename refactor (`…-007`) committed `351c7e8` (no tag); **Viewport-Visibility Layer (`…-008`) committed
  `7c34275`, tagged `m4`**; **Object Selector + Inject UI (`…-009`) committed `aa2a3a4`, tagged `m5`**;
  **Automatic Injection (`…-010`) committed `41ba104`, tagged `m6`**; viewport VFX-removal + poll-radius (`…-011`,
  no tag); **Labeled Frame-Capture + 2D BBox Labeling (`…-012`) tagged `m7`** (control-server Slice-1 promoted first
  as `ff1be3c`, no tag); **Missing-Texture (`…-013`) tagged `m8`**; screen-coverage cull + slider (`…-014`, no tag);
  **multi-anomaly session capture tagged `m9`** (`88f519c`); fixed-timestep capture-fps (`c5d58b0`/`500eac7`, no tag,
  `docs/capture-fps.md`); **targeted capture (`…-016`) tagged `m10`**; **capture pacing + honest fps stamping
  (`…-017`) tagged `m11`**; **client delivery mode (`…-018`) tagged `m12`**; **confirmation-bounded dashboard optimism
  (`…-019`, AnomDash, no tag)**; **content-clock-aware fps stamp (`…-020`) tagged `m14`**; **content-clock default → wall
  (`…-021`) tagged `m15`**; **three capture-delivery fixes (`…-022`) tagged `m16`** (`84dfa52`);
  **missing_texture revert hardening (`…-023`) tagged `m17`** (`e2c6dd2`; validated on the local repro **and confirmed
  on Concorde's real `FWMasterSkeletalMeshComponent`**, immediate + churn — G77 closed); **burst-boundary label
  alignment (`…-024`) tagged `m18`** (`4559c8c`); **preview backbuffer tee + targeting defaults (`…-025`) tagged `m19`**
  (`c8aa3fa`; G3/PIE + Concorde format = owner post-push checks); **annotation.json labeling (`…-026`) tagged `m20`**
  (`c01a214`); **deterministic arm→present pairing (`…-027`) = m21, BUILT + package-gated, NOT yet committed/tagged**
  (fixes the −1 in pace-off + mild-overrun regimes; deep-starvation residual → proposed m22 scene-identity marker).

## Documentation system — how these docs fit together (read in this order)
- **CLAUDE.md** (this file) — canonical context, environment, invariants, workflow rules, and the
  **Current status** above. Start here.
- **[docs/architecture.md](docs/architecture.md)** — **living** current-as-built design reference
  + the **anomaly catalog**. "The whole picture in one read." Describes only what is in the code
  *now*; forward plans live in the journals, never here.
- **[docs/onboarding.md](docs/onboarding.md)** — what this is, how the work is run, where things live.
- **[docs/setup-runbook.md](docs/setup-runbook.md)** — **living** recipe to build/run from scratch.
- **[docs/gotchas.md](docs/gotchas.md)** — **append-only** non-obvious lessons (G1, G2, …).
- **[docs/sessions/](docs/sessions/)** — one journal per session, `YYYY-MM-DD-NNN-slug.md`: the
  chronological record (Goal / What done / Problem→Resolution / Deviations / State / Hand-off) and
  the home for milestone **plans** and **design decisions** (including open/blocking ones).

## Environment
- Engine: **source-built UE 5.1** (Release-5.1) at `D:\UESource\UnrealEngine`, registered to the
  `.uproject`'s `EngineAssociation` GUID `{B34F356C-4AE7-256A-F0E1-318A632BB902}` under
  `HKCU\Software\Epic Games\Unreal Engine\Builds`. (Originally validated on source-built UE 5.4.4 — see
  the Engine support note in architecture.md. After any engine re-sync, **rebuild ShaderCompileWorker** — G18.)
- Host project: **StackOBot** at `D:\IntrusiveAnomalies\StackOBot` (natively-5.1; the old 5.4 host at
  `D:\Unreal Projects\StackOBot` is retired).
- Plugin in-tree at `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\` (its own git repo, `master`).
- Windows, MSVC. Build target: **StackOBotEditor / Development / Win64**. Host-target build constants:
  `BuildSettingsVersion.V2` / `EngineIncludeOrderVersion.Unreal5_1` (G17).
- Functional smoke tests run in **PIE via the `unreal-mcpython` MCP bridge** (host tooling, NOT part of
  this repo — see gotcha G8; on 5.1 its `BehaviorTreeEditor` dependency is severed). State/log reads close
  the non-visual gates; the owner eyeballs visuals.

## Architecture (current as-built: M0 — full detail in docs/architecture.md)
- One **Runtime** module `AnomalyInjector`, `LoadingPhase = Default`, `EnabledByDefault: true`.
- Build.cs deps: `Core`, `CoreUObject`, `Engine` (later may add `Renderer`, `RenderCore`, `RHI`,
  `Slate`, `InputCore`).
- Core injector = a `UTickableWorldSubsystem` (`UAnomalyInjectorSubsystem`) — auto-ticks,
  world-scoped, gives `GetWorld()`. Restricted to **Game + PIE** worlds via `DoesSupportWorldType`
  (never the editor preview world).
- Control surface = console commands via `FAutoConsoleCommandWithWorldAndArgs`, module-scoped,
  resolving the subsystem from the world the console passes in, null-guarded.
- M0 anomaly = ONE hardcoded hide (`IAI.HideActor` / `IAI.ShowAllActors`). The general anomaly
  **interface + registry is the M1 design** (see Current status + journal 002), not yet in code.

## Invariants (do not violate)
- **Source carries NO comments — by deliberate convention.** Every source file (C++ `.h/.cpp`, C#
  `.Build.cs`, Python, `.bat`) is kept comment-free, *including* the top-of-file copyright/banner header.
  **Do NOT add comments; strip any before committing.** (Feature work keeps re-introducing them — first
  stripped in `d4a77db`, re-stripped 2026-07-08 after the `AnomalyCapture` module re-added them.) Enforced
  with a deterministic, byte-preserving stripper kept in the workspace root alongside the two repos
  (`_strip_comments.py`): run `python _strip_comments.py <repo-root>`. It removes only comments while
  preserving every other byte — all string/char/template/regex literal contents, CRLF endings, and the BOM
  (idempotent; validated byte-identical against the original strip). Put rationale and design notes in commit
  messages, `docs/`, and the session journals — **never in code.** `LICENSE.txt` and the `.uplugin` JSON are
  intentionally exempt (not source).
- **Plugin stays game-agnostic.** The `AnomalyInjector` module may depend only on
  `Core`/`CoreUObject`/`Engine` (later `Renderer`/`RenderCore`/`RHI`/`Slate`/`InputCore`)
  and must **never `#include` or reference host game-module types** (e.g. anything from the
  `StackOBot` module). Host-specific buildability lives in the project, never in the plugin.
- **Matching is label-free.** Targeting matches by actor Name or Class only.
  `GetActorLabel()` is editor-only and absent in cooked builds — `ListActors` may print the
  label (guarded by `WITH_EDITOR`) but nothing matches on it.

## Workflow & doc-maintenance rules
- **Two-Claude split.** Design decisions come from an orchestrating "chat Claude" and are
  ferried by Kavin (project owner). The implementing Claude implements. Genuine design forks
  or ambiguities are surfaced back (listed standalone), not improvised.
- **Reports to Chat go in a copy block.** Any status/gate/handoff report meant to be ferried
  back to the orchestrating chat Claude must be emitted as a single fenced code block (so the
  owner can copy-paste it verbatim). Applies to stage-gate results, plan summaries, and any
  "report back" deliverable.
- **Plan-before-code.** A new milestone's first response is a file-by-file plan only; no
  implementation until approved.
- **Commits — Conventional Commits.** Prefixes: `feat:` (new anomaly or capability), `fix:` (bug),
  `docs:` (doc-only), `refactor:` (no behavior change), `chore:` (build/tooling). Scope anomaly-specific
  changes, e.g. `feat(blinking): …`. **Tag each milestone** with `git tag m<N>` after its commit so
  milestones diff cleanly (`m1..m2`, and a changelog can be auto-derived later). The git repo is the
  plugin folder (`master`); host scaffolding lives outside it and is not committed here. **Before every
  commit, run the comment stripper (see Invariants) — the source must stay comment-free.**
- **PUSH — CODE OWNS PUSHES (standing rule, 2026-07-29; SUPERSEDES the old "owner owns remote pushes").**
  When work is committed and gated, **push it yourself, including tags.** Do not wait for the owner to push —
  the old rule added a round trip and nothing else. **KEEP:** before pushing, report `git status` +
  `git log origin/master..master` so what went up is on the record — that is a **LOG, not an approval
  request**; report it and push **in the same turn**. (Rationale: two tracks share this repo and entangled
  once — G43 — so the record matters; the gate does not.) **KEEP:** **never force-push on a rejection** —
  stop and flag it to the owner. **Auth stays with the owner; never handle credentials.**
  ⚠ **This does NOT dissolve owner-owned QUALITY gates.** An owner Play-gate/eyeball smoke still comes
  **before** a tag when one is required — the hold there is the **smoke gate**, not push ownership.
- **Doc discipline — leave the docs able to (a) cold-start a fresh session and (b) explain the
  whole plugin to any UE dev.** When you start or advance a milestone you MUST, before the session
  closes:
  1. Update **Current status** (above) — the single "you are here" marker (latest as-built /
     in flight / open decisions / next action).
  2. Update **docs/architecture.md** to match the new as-built state, including the **anomaly
     catalog** — describe current code only, never aspirational.
  3. Write/append the **session journal** under `docs/sessions/` (history + the milestone plan +
     design decisions, including any open/blocking decisions).
  4. **Append** new lessons to `docs/gotchas.md` (never delete; supersede).
  5. Keep `docs/setup-runbook.md` and `docs/onboarding.md` current with the build/run steps and
     the control surface as they actually are.
  - Division of labor: **architecture.md = current state** ("what it is"); **journals = history +
    plans** ("how we got here / where we're going"); **runbook = repro**; **gotchas = lessons**.
