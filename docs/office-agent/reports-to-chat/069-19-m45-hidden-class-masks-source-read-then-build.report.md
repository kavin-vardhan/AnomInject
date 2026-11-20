# REPORT for 069-19-m45-hidden-class-masks-source-read-then-build.md
brief: 069-19-m45-hidden-class-masks-source-read-then-build.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 13:59:06 | finished: 2026-09-03 14:13:58 | duration: 15 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-19-m45-hidden-class-masks-source-read-then-build.live.txt

---- FINAL MESSAGE (Claude Code) ----
```
SESSION 069 / BRIEF 19 — m45: MECHANISM CONFIRMED AND WORKING, BUT M45-G4 NOT OBTAINED → DOES NOT MERGE.

STATE
  master  d48e1ba == origin/master  (UNTOUCHED)
  branch  m45-hidden-class-masks-GATE-FAILED  cdfa8d7 == origin  (pushed, not merged)
  also    m44-f1-resolution-mapping-UNVALIDATED  2d66b90  (unchanged)
  exe     staged restored to 635A615A (the shipped m44 binary);
          m45 build 2F5301BC archived as .m45-GATE-G4-unobtained-2F5301BC
  NO TAG.

=== TASK A — SOURCE READ ===
A1 = YES, end-to-end. A bRenderInMainPass=false primitive still reaches the custom-depth pass:
  SceneVisibility.cpp:2470-2473  bRenderCustomDepth ⇒ bHasCustomDepthPrimitives + stencil registered
  :2634 / :3161 / :3166          gather condition is an OR with bRenderCustomDepth
  :2727-2730                     AddCommandsForMesh(..., EMeshPass::CustomDepth)
  CustomDepthRendering.cpp:148   pass runs iff View.bHasCustomDepthPrimitives
  CustomDepthRendering.cpp:265   processor gates ONLY on ShouldRenderCustomDepth()
  BasePassRendering.cpp:1831     base pass REFUSES when !ShouldRenderInMainPass()
  PrimitiveSceneProxy.h:613 + DepthRendering.cpp:883,887
                                 ShouldRenderInDepthPass = mainpass || depthpass → both false = off
  ⚠ TRAP: :2708 adds the mesh to the BasePass command list even with main-pass off (the enclosing
  condition is an OR). The PROCESSOR is what refuses it — reading only the gather says "it still draws".
A2 shadows CastShadow + bCastContactShadow (IsShadowCast, PrimitiveSceneProxy.cpp:1280-1283; the OLD
  hide relied on !DrawInGame at :1288) · Lumen bAffectDynamicIndirectLighting (:1161) · distance fields
  bAffectDistanceFieldLighting (:1166) · ray tracing bVisibleInRayTracing (:1118) · decals
  bReceivesDecals (:1083) · velocity falls out with the main pass (SceneVisibility.cpp:2639).
  🚨 Nanite remains impossible (G134) — unchanged limit.
A3 Correct by construction: AnomalyVisibleMask.usf:37 emits the tag iff CustomDeviceZ >= SceneDeviceZ −
  bias. With the target absent from scene depth, that is exactly the would-be-visible region, and real
  occluders still exclude it. No new code.
A4 Hardened: AnomalyCensus.cpp:88 and :722 now also test the logical hidden state.

=== THE ONE DESIGN POINT THAT MATTERS ===
The labels' notion of "hidden" was AActor::IsHidden(). Dropping SetActorHiddenInGame would have
silently emptied blinking's hidden set while the pixels stayed right. A logical-hidden registry now
backs every consumer: both label paths, m26's LOCK-1 guard and its three siblings
(AnomalyMaskMeasure.cpp:226,272,315,506 — without this m26 starts arming on hidden ticks and
framesContributed moves), and the census.

=== IDENTITY — AND THE FIRST INSTRUMENT WAS BLIND ===
At the DELIVERED configuration the comparator cannot answer the question:
  CONTROL  old-vs-old2 (identical config)   mean 9.1612 % of pixels differ, worst 15.3992 %
  TEST     old-vs-NEW                       mean 8.5619 %, worst 11.8074 %
  CAN-FAIL old-vs-NOSHADOW (violation)      mean 9.5381 %, worst 16.4272 %
The deliberate violation sits INSIDE the control's own band and the correct fix reads LOWER than the
control. Pose, frame alignment, event sets and blink hidden sets are identical across all three, so
this is per-run rendering nondeterminism, not A47 (G169).

With r.AntiAliasingMethod 0 + Lumen/GI/reflections off the floor collapses:
  CONTROL  old-vs-old2      0 of 60 frames differ   ← floor is genuinely ZERO, sensitivity one pixel
  TEST     old-vs-NEW       0 of 60 differ          ← M45-G1 PASSES
  CAN-FAIL old-vs-NOSHADOW  0 of 60 differ          ← M45-G4 NOT OBTAINED
Both levers PROVEN ENGAGED (engine log echoes HideMode -> 1 and HideOmitShadowSilencing -> ON at frame
1), so the omission really happened and the picture still did not move ⇒ this target casts no shadow
reaching the frame in CB_GateLevel. The FIXTURE cannot exhibit the class (G135's shape).
Mechanism works: 20 mask files under the new hide against 0 under the old.

=== WHY IT DOES NOT MERGE ===
M45-G4 is a required gate and a guard never shown to fire is not a guard (G96, four prior instances).
The identity claim is also narrower than it looks: proven at AA/GI off, which is NOT the delivered
configuration, where the instrument is blind. I deliberately did NOT run the remaining gates (G2
detail, G3 IoU, G6, MASK-TIE, ONSET, MASK-PICTURE-PAIRING) — a milestone that cannot ship does not
need a green table built around an unproven safety argument.

=== A MISTAKE I MADE AND FIXED ===
`git add docs` swept the owner's six untracked CHAT-HANDOFF-*.md into c9fed9a. Corrected in cdfa8d7
(git rm --cached); they are untracked again, still on disk, and master never had them. The invariant
names `git add -A`; a directory-scoped add hit the same trap. Path-scoped file lists only, from here.

=== NEEDS-DECISION ===
 D1. THE FALLBACK RULING. Options, in my order of preference:
     (a) Give CB_GateLevel a shadow-casting light so the target casts a visible shadow — then the
         can-fail leg is decisive at AA-off and M45-G4 becomes obtainable. Cheapest, no product
         change, and it makes the gate permanently meaningful. ⚠ It edits the frozen gate level (G99),
         so it needs your ruling — that is why I did not do it.
     (b) Declare the identity arbiter runs at AA-off BY DESIGN and find a different can-fail lever
         (e.g. omit bAffectDynamicIndirectLighting with Lumen ON, which does change pixels).
     (c) The pre-declared fallback: coarse projected-bbox masks flagged "coarse": true — needs m39's
         honest bbox first, so it is not available this week.
 D2. Identity at the DELIVERED configuration needs a within-frame comparator (G-M9's shape from m35),
     because cross-run temporal accumulation is 9 % here. Worth its own brief if you want the stronger
     claim; the AA-off arbiter is sound for "does the hide change what renders".
```
api-equivalent cost (not billed on Max): $20.96 | turns: 34 | running total across all briefs: $439.23
