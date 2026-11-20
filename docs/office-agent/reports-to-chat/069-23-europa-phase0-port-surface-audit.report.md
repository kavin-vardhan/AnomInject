# REPORT for 069-23-europa-phase0-port-surface-audit.md
brief: 069-23-europa-phase0-port-surface-audit.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: high
started: 2026-09-03 15:39:42 | finished: 2026-09-03 15:46:11 | duration: 6 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-23-europa-phase0-port-surface-audit.live.txt

---- FINAL MESSAGE (Claude Code) ----
```
SESSION 069 / BRIEF 23 — EUROPA PHASE 0: PORT-SURFACE AUDIT DELIVERED.

STATE
  master  324ebad == origin/master  (pushed)
  READ-ONLY honoured: Source/ and Shaders/ verified CLEAN — one new doc, 290 lines,
  docs/europa/PHASE0-port-surface-4.25.md.  NO TAG.

THE DOC'S OWN LIMIT, STATED AT ITS TOP: there is no 4.25 tree on this bench. Every grade is either
read out of OUR code (certain) or from 4.x knowledge (not). PRESENT/CHANGED/ABSENT are predictions to
be checked; UNKNOWN rows carry the question that settles them. Nothing is guessed into a scope
decision (G120).

=== THE THREE FINDINGS THAT DECIDE THE PLAN ===
1. 🎯 THE CODE IS CONSERVATIVE C++ — measured, not assumed: `if constexpr` 0, `std::` 0, structured
   bindings 0, `FVector3f`/`FVector3d`/`UE::Math` 0, `TStringBuilder` 0, and no `CppStandard` set in
   any Build.cs. The 5.x double-precision/LWC migration is a NON-ISSUE in both directions.
2. THE INJECTOR HALF IS NEARLY PORTABLE. One structural change — `UTickableWorldSubsystem` is 4.26+,
   so `UWorldSubsystem` + `FTickableGameObject` (×3, AnomalyInjectorSubsystem.h:12,
   AnomalyAutoInjectorSubsystem.h:35, AnomalySelectorSubsystem.h:12). One real unknown:
   `FWorldDelegates::OnWorldTickEnd` (AnomalyCaptureSubsystem.cpp:349,350,557,562) — the m40 label
   anchor and m44's record-birth point. One contained unknown: LOD accessors, already isolated behind
   `AnomalyLod` since m3. One droppable flag: `bVisibleInRayTracing`.
3. THE CAPTURE HALF IS LARGE AND CONCENTRATED IN THREE THINGS:
   · `SubscribeToPostProcessingPass` (AnomalyMaskSceneViewExtension.cpp:91) is 5.0+ — ABSENT on 4.25;
     both the colour grab point and the mask pass need a different hook.
   · RDG inside an SVE hook — UNKNOWN; decides "port" vs "rewrite" for the mask pass.
   · 🚨 We reach into renderer-private `SceneRendering.h` for `FViewInfo::ViewRect` (:22,36,153,154,216,
     enabled by AnomalyCapture.Build.cs:355) — m46's internal rect. On a MODIFIED 4.25 engine this is
     the most likely hard failure.

=== TWO PIECES OF FREE GOOD NEWS ===
· The analysers (m44_gates, m44_pairing_probe, m45_iou, verify_capture) read ONLY the artifact
  contract — labels.jsonl, annotation.json, run_summary.json, target_mask/*.png. They are
  engine-independent and become the cross-engine gate for free.
· Nanite is absent on 4.25 ⇒ G134's "no hidden-class mask for Nanite targets" does not exist on
  Europa. Every target is potentially maskable — an improvement over the 5.1 host, not a regression.
· And two shipped fallbacks already exist in-tree and are gated: the backbuffer capture path
  (IAI.Capture.SVE 0) and the CPU reduce (IAI.Capture.MaskReduce cpu).

SIZE: injector half SMALL, 1–2 days (+2–3 if OnWorldTickEnd is absent). Capture half LARGE, 1.5–4
weeks, spread dominated by the three unknowns — an honest range, not padding.

ARCHITECTURE: I agree with the backend-interface shape and named four things I would do differently —
(1) TWO seams, not four (reduce and readback are implementation details of the mask backend, not
separate axes); (2) draw the seam ABOVE `FViewInfo` so shared code never sees a renderer-private type;
(3) `#if ENGINE_*_VERSION` only at declaration sites in the injector half, NEVER inside a gate-bearing
algorithm — that would be two engines running different logic behind one green gate table, G224's
shape; (4) declare the artifact contract byte-comparable across engines now, as the cheapest
cross-engine proof.

PHASE 1: injector-only compile, one anomaly fires, with pre-declared gates E1-G1..G6 including a
prove-it-can-fail (E1-G4: IAI.Apply on a non-existent actor must refuse and change nothing) and an
explicit "no artifact is produced or claimed" (E1-G6).

GATE MATRIX: doubles to 2 orders × 2 engines = 4 legs per alignment gate. Proposed mitigation, for
your ruling: alignment gates on both engines, pixel arbiters only on the engine being changed (G230's
scope rule already points this way). Hosting 4.25 here needs ~150–250 GB — fits on E: only — plus a
second registry Builds entry, and CB_GateLevel cannot be shared (it is a cooked 5.1 asset; Europa
needs its own frozen fixture).

=== NEEDS-DECISION ===
 1. WHAT IS THE 4.25 DELIVERABLE? Three coherent products: (a) injection only; (b) injection +
    backbuffer capture + labels, NO masks — uses only paths already shipped and gated, and needs
    almost none of the renderer questions; (c) full parity including masks — needs every renderer
    question answered first. ⛔ I would not commit to (c) before those answers, and (b) is a real
    product that (c) can be built on.
 2. DOES THE EUROPA TITLE ALREADY USE CUSTOM DEPTH/STENCIL? This is the one question that could
    invalidate the architecture rather than merely size it: the plugin reserves stencil 200–254 and
    m26's collision detectors assume nothing else writes there.
 3. DOES THE BENCH HOST THE 4.25 ENGINE, or does Europa work happen only on the office PC? That is
    the difference between gates I can run and gates only the owner can run.
```
api-equivalent cost (not billed on Max): $14.91 | turns: 11 | running total across all briefs: $505.06
