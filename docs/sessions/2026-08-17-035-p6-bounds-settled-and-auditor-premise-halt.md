# 2026-08-17 — 035 — P6 bounds settled; the fabrication auditor halts on its source premise

**Docs-only turn on `master`.** No production code changed. No probe touched — CaptureBench stays
at `163dd12` (a standalone analysis script was ruled not a probe edit; it was not written, see §4).

Two independent pieces of work: a read-only diagnosis that settled the **P6 bounds side**, and a
plan-gated build of the **delivered-session fabrication auditor** that **halted at its own
pre-declared source-premise gate before any code was written**.

---

## 1. P6 bounds — settled, with the adopted explanation corrected

**The question:** what produces the perfect 1010 cube reported as `BP_Bot_C_0`'s `node.bounds`?

**Outcome: component union CONFIRMED — but the previously adopted explanation named components that
cannot contribute, and the extent is not geometry at all.**

The chain, each link measured or read from the 5.1.1 engine source at `D:\UESource\UnrealEngine`
(`Build.version` verified `++UE5+Release-5.1`):

1. `AActor::GetComponentsBoundingBox` (`Actor.cpp:1681-1695`) iterates
   `ForEachComponent<UPrimitiveComponent>` and gates on
   `IsRegistered() && (bNonColliding || IsCollisionEnabled())`.
   ⇒ **A spring arm and a camera component can never contribute.** Both are `USceneComponent`, not
   `UPrimitiveComponent`. The earlier wording — "unions every component including non-rendering ones
   (spring arm, camera, collision)" — is **struck as structurally impossible**.
2. `UCameraComponent::OnRegister` (`CameraComponent.cpp:118-152`), inside `#if WITH_EDITORONLY_DATA`,
   creates two extra components **on the owning actor**: a `UStaticMeshComponent` proxy mesh and a
   **`UDrawFrustumComponent`** — and `UDrawFrustumComponent : public UPrimitiveComponent`
   (`DrawFrustumComponent.h:18`). `SetIsVisualizationComponent(true)`, `bHiddenInGame` and
   no-collision do **not** exclude it; the union checks registration only.
3. `UCameraComponent::UpdateDrawFrustum` (`:198-214`): `FrustumStartDist = 10.f`;
   `FrustumEndDist = FrustumStartDist + FrustumDrawDistance` with
   `const float FrustumDrawDistance = 1000.0f` (`:203`) ⇒ **`FrustumEndDist = 1010.0`**.
4. `UDrawFrustumComponent::CalcBounds` (`DrawFrustumComponent.cpp:164-167`) returns
   `FBoxSphereBounds(LocalToWorld.TransformPosition(ZeroVector), FVector(FrustumEndDist),
   FrustumEndDist)` ⇒ a box **centred exactly on the camera component's world location** with extent
   **(1010,1010,1010)**. The cube is perfect *by construction*; the centre is on the camera *by
   construction*.
5. That box strictly **contains** the pawn's capsule and skeletal mesh, so the union equals the
   frustum box unchanged.

**Why the "the centre should sit between camera and mesh" objection fails — and it was the right
objection to raise.** A union lands between two boxes only when neither contains the other. Here one
box swallows the other, so the result is **containment, not compromise**.

**Live corroboration** (see G97 for the provenance caveat — this measurement is from a *different*
project/engine and is used only as mechanism corroboration): on a registered `SceneCapture2D_0`
carrying the same component class, `get_actor_bounds(False)` (== `GetComponentsBoundingBox(true)`)
returned origin **exactly** the component's world location with extent **(1000,1000,1000)**, its
`DrawFrustumComponent_0` reported `frustum_end_dist = 1000.0` (scene captures set the end distance
flat — `SceneCaptureComponent.cpp:703` — hence 1000 there and 10+1000 for cameras), and the
colliding-only union came back **invalid/zero**, proving nothing on that actor has collision and that
**`bNonColliding = true` is what admits the frustum**. That is the one half of the original
explanation that survives.

**Artifact confirmation:** `session_20260817-132214`, 8/8 events — extent
`[1010.0000000000002, 1010, 1010]`, and `bounds.origin` equal to `camera.global_position` on all
three axes. `BP_Bot.uasset` carries `CameraComponent` / `SpringArmComponent` (`CameraBoom`,
`FollowCamera`).

**The number is a hardcoded editor visualisation constant, not a measured bound.** It is delivered
*through* a union, which is why this reads as a union bug, but no geometry produced 1010.

### 1.1 Editor-only — client impact drops, deferred not dropped

The creation in step 2 sits behind `WITH_EDITORONLY_DATA`, and the packaged game target defines it
**off** (measured: `Intermediate\Build\Win64\StackOBot\Development\...\Definitions.*.h` →
`#define WITH_EDITOR 0`, `#define WITH_EDITORONLY_DATA 0`; the editor target does not define it, i.e.
takes the default 1).

⇒ **Prediction on the record, not yet measured:** a packaged client build creates no `DrawFrustum`,
so `node.bounds` for a camera-bearing pawn should be capsule-∪-mesh, **order-of-capsule, not 1010**.

**The corpus is consistent but confounded, and is therefore NOT counted as evidence:** 1010 appears
in 8/8 PIE node rows and 0 of the other 1,376 rows, but **every** packaged node carrying a bounds
field is a `StaticMeshActor` with no camera at all. A corpus with no camera-bearing packaged target
cannot discriminate editor-only-ness. Named rather than banked.

**Confirmation run deferred by owner ruling** — urgency drops on the compile-time evidence, and the
gate level is frozen and must not be mutated to enable it. Residual risk to state when it does run:
a cooked asset carrying a *serialized* editor-only component.

### 1.2 The contract decision — RULED AND LOCKED (not implemented, do not relitigate)

`node.bounds` must be **render-relevant bounds**: the union over components that contribute drawn
pixels, **not** a whole-actor union admitting collision capsules and visualisation primitives. It
must reuse the existing renderable definition (`IsRenderableComponent`, static-or-skinned mesh, G33)
so that **label geometry and selection geometry agree on what "the object" is**.

Parked as a **milestone candidate**. Note the residual that survives even packaged: the whole-actor
union still admits the collision capsule, so `node.bounds ≠ mesh bounds` in *both* configs — just by
a capsule rather than by 1010. That is what the ruling fixes.

**P6 camera side is untouched:** `camera.path` is still the view-target actor path — a naming/contract
question, unaffected.

---

## 2. New standing rules

| # | Ruling |
|---|---|
| **A59** | **MCP-bridge provenance.** No measurement over the bridge is attributed to this project until `Paths.project_dir()` **and** the engine version are read back and stated with the result. A bridge connection is never evidence of *which* project answered. → **G97** |
| **A60** | A quantity absent from the artifact under test is **supplied explicitly by the operator**, **or** the analysis reports UNDECIDABLE for every claim depending on it. Never reconstructed from downstream artifacts, never defaulted, and **never replaced by a weaker test that is then reported as if it were the original one**. (Generalises A48.) |
| **A61** | A newly discovered **shape** does not earn a new verdict bucket. It gets a **diagnostic tag** on an existing verdict, or an existing amendment excludes it. |

**Gotchas landed: G97** (the bridge attaches to whichever editor is listening; a second UE project on
this box silently captures it — a permanent environmental fact, not an incident) and **G98** (see §4).

---

## 3. The auditor — what was approved

An office-machine instrument to decide whether the client's **already-delivered, pre-m23** sessions
carry P3b-fabricated label windows. Single standalone stdlib-only Python file, no CaptureBench
imports, so it copies as one file; reads a session folder, prints per-event verdicts; only a text
summary ever leaves that machine.

**Schema mapping, verified against 73 banked sessions and the writer source — not inferred:**

| semantic | field | file | present in delivery mode? |
|---|---|---|---|
| claimed positive set | `anomalies[i].affected_frames.frame_indices` | annotation.json | yes |
| anomaly id | `anomalies[i].anomaly_type` (client-mapped) | annotation.json | yes |
| provenance (post-m23) | `anomalies[i].manifested` | annotation.json | yes |
| provenance (post-m23) | `non_manifested_events` | run_summary.json | yes |
| target_fps | `target_fps` / `video.target_fps` | run_summary / annotation | yes |
| stamped fps | `video.fps` (m11 honest stamp) | annotation.json | yes |
| delivery mode | `delivery_mode` | run_summary.json | yes |
| **full declared window** | **`positive_frames`** | **run.json** | **NO — suppressed by m12** |

**Three schema traps, all measured, all destined for the README:**

1. `start_frame` / `end_frame` are **min/max of the CLAIMED set**, not a window
   (`AnomalyLabelWriter.cpp:375-383`). An auditor that trusts them computes `span == count` for every
   event and reads "contiguous" everywhere.
2. `anomaly_type` is **client-mapped** (`AnomalyCaptureSubsystem.cpp:167-179`): `blinking` → `blink`;
   every other id passes through unchanged. The internal id exists only in `run.json target_anomaly`.
3. `run_summary.positive_frames` is a **session total of fire-active frames**;
   `run.json positive_frames` is the **per-burst window length**. Same name, different files,
   different meanings — measured 59 vs 8 in one session.

**The window is not obtainable from the client** (owner constraint: not difficult, *not possible*).
Hence the approved design is **window-blind by default**, with `--window` optional: gapped ⇒
GENUINE-SHAPED; contiguous ⇒ SUSPECT-CONTIGUOUS (undecidable, and explicitly *not* an accusation);
FABRICATION-SHAPED is **never** emitted without a window.

---

## 4. THE HALT — the window-blind design's source premise is FALSE

The window-blind reduction rests on one premise, which the brief required be confirmed from source
before any code was written:

> the P3b fallback emits `AffectedFrames` verbatim, and `AffectedFrames` is **necessarily a
> contiguous frame range** — therefore a gap is mechanically impossible for a fabricated event.

**First half CONFIRMED. Second half FALSIFIED.** Full mechanism in **G98**. In short:
`AffectedFrames` is accumulated one index at a time and **only on frames passing a projection test**
(`ProjectActorBoundsToScreenRect`, `AnomalyViewport.cpp:653-685`), which fails on an invalid view, an
actor with no static/skinned mesh bounds, a bounds box entirely behind the camera, or a rect that
does not intersect the screen — plus an enclosing `TargetActor.Get()` validity check. Any of those
failing on an interior frame **gaps the set**. Event keying is per-fire
(`Id + StartFrame + Target`, `:1403-1404`), so cross-fire merging is *not* an additional source.

**Empirically unobserved but confoundedly so:** 0 of 1,367 non-empty events across four corpora has a
gapped `AffectedFrames`-verbatim set — because every banked leg is a **static-camera** run. The
client captures moving-camera gameplay, which is precisely the regime that can produce the gap.

**Blast radius is wider than the window-blind path.** With a window supplied, a fabricated-but-gapped
event has `count < window`, so it is not FABRICATION-SHAPED and falls to "strict gapped subset" =
GENUINE-SHAPED. **Both** the windowed and the window-blind discriminators would therefore *bless* it —
a false negative in the dangerous direction, the same failure mode Ruling C corrects for unknown
types.

**Halted per the pre-declared gate. No code written, no in-turn repair, no adjustment of the gapped
test to make a control pass.** Awaiting a chat verdict.

---

## 5. Shipped-default observation (report-only; deliberately NOT wired into anything)

- **Value: `PositiveFrames = 8`** — `Source/AnomalyCapture/Public/AnomalyCaptureSubsystem.h:182`
  (`int32 PositiveFrames = 8;`), a hardcoded member initialiser.
- **Only surface that can change it:** the `IAI.Capture.Config` console command
  (`AnomalyCaptureSubsystem.cpp:420-426`).
- **Cannot be changed by:** the dashboard (no burst/positive command exists in `AnomalyControlServer`
  at all), the shipped `config.json` (`controlToken` / `capturesRoot` / `serverUrl` only), or any ini
  — `GConfig` reads exactly three keys: `bDeliveryModeDefault`, `ContentClockDefault`,
  `bFocusGateDefault` (`:222-238`).
- **Corroboration:** 72 of 73 banked sessions record `run.json positive_frames = 8`; the single
  exception is the `NEG` control, deliberately set to 2.

⇒ Unless someone typed the console command in her build, delivered sessions ran a window of 8. This
is an **observation**, not a default, and the tool's behaviour without `--window` is unchanged by its
existence.

---

## 6. Step 0 — control #3 rescued from the wipe path

`Builds\BenchGate\Windows\StackOBot\Saved\NEG2\session_20260816-183524` (the forced-non-manifestation
run: `non_manifested_events = 8`, 8/8 `manifested:false`, empty `frame_indices`) was the **sole copy**
of the post-fix guard control, sitting under the tree the archive/stage step wipes (**G92**). Copied
to `D:\IntrusiveAnomalies\_bench_sessions_bank\NEG2\` and verified: 95 files / 67,576,264 bytes on
both sides, SHA-256 match on `annotation.json`, `run_summary.json`, `run.json`.

*(The brief named the folder `session_20260817-183524`; the session on disk is
`session_20260816-183524`. One session, dated the 16th.)*

---

## 7. State and hand-off

- **Production code: UNCHANGED.** Plugin `master` — this turn is docs-only.
- **CaptureBench: `163dd12`, untouched.** The auditor file was not created.
- **Bank: 74 session dirs** (was 73; `NEG2` added).
- **Controls 1 and 2 are identified and readable:** known-fabricated
  `I10HF\HF1_nat120\session_20260816-145719` (pre-m23, 120 fps, 13 blink events all `(8,8,CONTIG)`
  against window 8); known-genuine `I10\L1_nominal\session_20260816-122707` (30 fps, 7 events at
  `(4,7,GAPPED)` = the byte-exact `[4,5,9,10]`, plus one cap-truncated tail).
- **Control 4 (delivery-ON) still does not exist** — measured: 0 of 74 banked sessions has
  `delivery_mode = true`, and none exists anywhere in the four corpora. It must be produced.
- **Open, blocking the auditor:** the G98 verdict. Everything else in the plan (file layout, schema
  mapping, Rulings B/C/D, the printed-header rules, INDISCRIMINABLE as a permanent column) stands
  approved and unbuilt.
- **Open, unchanged:** P1, P5, 60 fps certification, the P6 packaged confirmation run, the P6
  render-relevant-bounds milestone candidate.
