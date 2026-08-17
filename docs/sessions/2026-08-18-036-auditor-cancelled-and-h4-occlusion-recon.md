# 2026-08-18 — 036 — the auditor is cancelled; H4 (occlusion-blind labelling) opened and reconnoitred

**Docs-only turn.** No production code changed, no probe touched (`CaptureBench` stays at `163dd12`),
no scene mutated, no build, no packaged run, no test executed.

---

## 1. The delivered-session fabrication auditor is CANCELLED

**Not paused — cancelled.** Owner constraint: **there is no client communication channel in either
direction**, so the audit's output has no consumer. Nothing was ever implemented — the file
`Plugins/CaptureBench/tools/audit_delivered_session.py` was never created, and nothing about it is
committed anywhere.

A cold reader will find an approved plan in journal 035 with no outcome. **This is that outcome.**
Do not pick it up as outstanding work.

**Kept as reference, NOT as pending work** (all already banked in journal 035):

- the verified schema mapping for `annotation.json` / `run_summary.json` / `run.json`;
- the three schema traps — `start_frame`/`end_frame` are min/max of the *claimed set* and not a
  window; `anomaly_type` is client-mapped; the two different `positive_frames`;
- the located control sessions (`I10HF\HF1_nat120`, `I10\L1_nominal`, `NEG2`);
- **G98**, which is the durable finding and outlives the tool entirely;
- the shipped-default observation: the burst window is `PositiveFrames = 8`, hardcoded at
  `AnomalyCaptureSubsystem.h:182`, reachable only via the `IAI.Capture.Config` console command.

**The NEG2 rescue stands.** It was correct regardless of the cancellation: the sole copy of the
guard-fired control was sitting in the tree the archive step wipes (**G92**). It is now at
`_bench_sessions_bank\NEG2\session_20260816-183524`, verified byte-for-byte.

### 1.1 STRUCK from the standing plan — struck, not deferred

- the **office-machine `target_fps` audit** of delivered client sessions;
- the precautionary **"cap VideoFps at 30" client note**.

**No client-facing action item survives anywhere in the plan.** Older documents — including
`CHAT-HANDOFF-s2-i10-and-m23-p3-fix.md` §8, which still lists the `target_fps` audit as an
owner-lane item gating client comms — are **superseded by this entry**.

### 1.2 Chat-side error on the record

Chat-Claude overrode the handoff's standing `target_fps` approach as "measuring a proxy", and built
**two turns of design** on the claimed-set **shape fingerprint** instead. The fingerprint turned out
to be unsound (**G98**: a gap is not mechanically impossible for a fabricated event, so shape cannot
separate fabricated from genuine); the proxy was the right instrument all along.

The design died to measurement, which is the system working. But the record should show that a
**correct standing plan was driven past**, and that the replacement was two turns of work that
produced one durable gotcha and no instrument. Both halves belong in the record.

*(Both instruments are moot now — the audit has no consumer at all. That does not retire the
lesson.)*

---

## 2. H4 — occlusion-blind labelling. OPEN HYPOTHESIS, NAMED NOT ADOPTED

**Number assignment:** the ledger holds **H1** (GPU-load starvation shape) and **H3** (auto-exposure
active). **H2 appears nowhere in this repo.** Because its history cannot be verified from here and
numbers are **never reused** (the m22 renumber hazard), the next free number is **H4**.

> **H4 — the label path is occlusion-blind while the selection path is not, so a target that is
> on-screen but fully occluded is labelled positive while contributing no pixels.**

**Mechanism, read from source. Never observed producing an instance. Not adopted, not a cause of
anything, not to be written into any design or fixed.**

| path | predicate |
|---|---|
| **selection** — `IsComponentRenderableVisibleInternal` (`AnomalyViewport.cpp:165-181`) | renderable ∧ poll-radius ∧ frustum ∧ **`IsUnoccluded`** |
| **labelling** — `ProjectActorBoundsToScreenRect` (`:653-685`), called at `AnomalyCaptureSubsystem.cpp:1438` | static/skinned bounds union ∧ projects ∧ intersects screen — **no trace, no occlusion** |

`IsUnoccluded` (`:109-144`) traces `ECC_Visibility` from the view origin to the bounds **centre plus
8 corners**, ignoring only the target's own owner, and returns *unoccluded* as soon as **any one**
sample is clear. "Fully occluded" therefore means **9 of 9 rays blocked**.

**Routed to `feature/stencil-capture`.** That branch's premise — report actual pixel contribution
before hiding — is the cure for exactly this. H4 **strengthens an existing locked ruling** and does
not open a lane.

**Pre-declared test, stated now, before any instrument exists:** a target fully occluded for an
entire event window, labelled positive, with pixels showing no change across the window.

---

## 3. Recon findings (read-only)

### 3.1 The gate level does contain stably occluded on-screen targets

`CB_GateLevel` is script-authored by `CaptureBench/tools/make_gate_level.py`, so its geometry is
exactly known without opening anything: a 12×12 grid at 200 cm spacing, `x,y ∈ [-1100, 1100]`,
`z = 60 + ((ix·7 + iy·13) mod 5)·25`, uniform scale 1.2 on 100-unit primitives ⇒ **bounds half-extent
60 on every target**, cycling cube/sphere/cylinder/cone by `(ix+iy) mod 4`. Verified against banked
artifacts: `StaticMeshActor_49` reports origin `(-1100,-300,110)` and extent `(60,60,60)`, matching
the model exactly.

**Actor-name mapping, verified on three independent banked data points** (`StaticMeshActor_49`,
`_73`, `_85`): **`StaticMeshActor_K` ⇔ grid spawn index `n = K−1`** (the floor takes `_0`). This
matters because targeting is label-free — the console needs the engine name, not the editor label.

**The A47 bifurcation is in ROTATION, not position.** Across **369** gate-level event camera samples
in the bank the position is **invariant at `(-1500, 0, 260)`** — the `CB_PlayerStart` — on
**369/369**. What varies is orientation: modal `(0,0,0)` on **278/369 (75.3 %)**, the remainder
scattered in yaw and pitch.

**This is the load-bearing consequence:** occlusion between two actors depends only on the **eye
position** and the geometry, both invariant. Camera rotation changes only *frustum membership*.
**So any occlusion relationship here is stable across the A47 bifurcation by construction**, and the
only thing the bifurcation can disturb is whether the target is on screen.

Computed occlusion (9-sample model identical to `IsUnoccluded`; target sampling uses the bounds AABB
corners, which is **exact** for every shape since the engine samples `Component->Bounds`):

| occluder model | fully occluded | …and on-screen at the settled pose |
|---|---|---|
| all occluders as AABB — **upper bound** | 52 / 144 | 50 |
| true primitive collision shapes — realistic | 26 / 144 | 25 |
| **cube occluders only — rigorous floor** (box collision *is* the AABB) | 8 / 144 | **7** |

The floor row needs no approximation of curved collision and is the number to trust. On-screen share
across the full observed rotation spread, for the cube-occluded set:

```
CB_Target_004 -> StaticMeshActor_5     (-300,-1100,135)   329/369  89.2%
CB_Target_010 -> StaticMeshActor_11     (900,-1100, 60)   350/369  94.9%
CB_Target_021 -> StaticMeshActor_22     (700, -900, 85)   350/369  94.9%
CB_Target_023 -> StaticMeshActor_24    (1100, -900, 60)   350/369  94.9%
CB_Target_032 -> StaticMeshActor_33     (500, -700,110)   350/369  94.9%
CB_Target_099 -> StaticMeshActor_100   (-500,  500, 60)   350/369  94.9%
CB_Target_138 -> StaticMeshActor_139    (100, 1100, 60)   348/369  94.3%
CB_Target_134 -> StaticMeshActor_135   (-700, 1100,110)    37/369  10.0%   <- EXCLUDE
```

### 3.2 Reachability — targeted fire bypasses occlusion, with one conditional

`TryFireSpecific` (`AnomalyAutoInjectorSubsystem.cpp:258-306`) resolves its target through
`AnomalyTargeting::FindActorsMatching(World, "=" + ActorName)` — **no viewport predicate of any
kind**. Its only guards are `MaxConcurrent`, `IsIdLive` and `IsActorLive`. Occlusion is not
consulted, confirmed from source rather than assumed.

**The conditional, and it is a real gate:** each anomaly's own `Apply` re-resolves its targets —
`Anomaly_Blinking.cpp:48-50` uses `AnomalyViewport::FindVisibleActorsMatching` (frustum ∧
`IsUnoccluded`) **when `IAI.SetViewportScoping` is ON**, and plain targeting when OFF. The default is
**OFF**, so targeted fire reaches an occluded actor — **but a run with scoping ON would silently
select nothing.**

**The auto-pool selector does exclude occluded actors:** it draws from `GetVisibleRenderableActors`,
which routes through `IsComponentRenderableVisibleInternal` and therefore through `IsUnoccluded`.

### 3.3 Mutation — not required

**An occlusion test can be run WITHOUT mutating `CB_GateLevel`.** Seven already-occluded, already-
on-screen targets exist in the level as it stands and as it is already cooked into `Builds\BenchGate`.
The level is not touched, not re-authored and not re-cooked.

Cost of a sibling level, if one were ever wanted: cheap in script, **expensive in practice**. It needs
the 5.1 StackOBot editor running (the bridge currently reaches a different project — **G97**), then a
cook, then a stage to `Builds\BenchGate`, and **staging wipes the `Saved` tree (G92)** so the bank
must be refreshed first. ⚠ **Footgun:** `make_gate_level.py` **deletes the asset at `LEVEL_PATH`
before authoring** (`:14-15`) — running it unmodified would destroy the frozen `CB_GateLevel`. A
sibling must change `LEVEL_PATH` first.

### 3.4 The two manifestation paths

- **(b) targeted fire on an already-occluded actor — PRODUCIBLE NOW, zero mutation.** Pick one of the
  seven, fire `blinking` or `missing_object` at it by engine name, viewport scoping OFF. Cost is one
  packaged run.
- **(a) selected while visible, becomes occluded during the window — NOT PRODUCIBLE in this level.**
  Every target is `ComponentMobility.STATIC`, there are no moving actors, and the camera position is
  invariant across all 369 banked samples. Occlusion state therefore **cannot change** during a
  window. Producing (a) requires motion the gate level does not have — a moving occluder, a moving
  target, or a driven camera — i.e. the sibling-level cost above **plus** a motion mechanism that
  does not exist yet.

(a) is the shape that would occur in a real client capture; (b) is the one we can afford.

### 3.5 Oracle fit — A54 reads the symptom, and the cause is already instrumented

**A54 as it stands would read this**, and would return **ABSENT**: the target is fully occluded, so
hiding it changes no pixels, and the bbox — projected from bounds by the occlusion-blind projector —
covers the *occluder's* static pixels. Local contrast against the event-flank frames finds nothing.

**But ABSENT is exactly P3's verdict too.** A54 is a *symptom* detector here, not a cause
discriminator; on its own it cannot separate "labelled hide never manifested" from "labelled hide was
invisible because the target was behind something". **No new oracle needs building** — the missing
half already ships:

`selection_provenance.json` (internal sidecar, suppressed in delivery) records per event
`occlusion_samples_passed` / `occlusion_samples_total` and a `valid` flag. Banked example from
`L1_nominal`: `occlusion_samples_passed: 9, occlusion_samples_total: 9, valid: true`.

⚠ **And there is a subtlety worth knowing before the test is designed.**
`EvaluateSelectionProvenance` (`AnomalyViewport.cpp:540-570`) calls `CollectRenderableVisibleUnion`,
which filters through `IsComponentRenderableVisibleInternal` — **occlusion-aware** — and **returns
false** when nothing survives. For a fully occluded target it therefore returns early with the
struct left at defaults. Two consequences:

1. the sidecar reports **`valid: false`** with `0/0` occlusion samples — itself a clean occlusion
   signal, but *not* a "9 blocked of 9" reading;
2. **`annotation.json`'s `coverage_pct` would be 0** for that event (it is `Ev.Provenance.CoveragePct`)
   while **`coverage_ratio` would be non-zero** (it comes from the occlusion-blind projector).

That `coverage_pct == 0` with `coverage_ratio > 0` asymmetry **ships to the client in delivery mode**.
Recorded as an observation only — it is not a designed discriminator and nothing is built on it.

**A56 note:** A54 is camera-certifiability-gated (modal-crop coverage ≥ 90 % of label rows, ≤ 3
distinct bboxes). The six targets at 94.9 % clear it comfortably; `StaticMeshActor_5` at 89.2 % sits
on the line and should not be the one chosen.

---

## 4. State

- Plugin `master`: docs-only this turn. `CaptureBench` `163dd12`, untouched.
- Bank: **74** session dirs.
- `docs/CHAT-HANDOFF-s2-i10-and-m23-p3-fix.md` is now **tracked** (`fa22a1d`) — the cold-start chain
  is intact. `CHAT-HANDOFF-m10-m21.md` was never written and remains a separate standing debt; do not
  reconstruct it.
- **H4 open, named, not adopted.** Test design is chat-side. Nothing runs until it comes back.
