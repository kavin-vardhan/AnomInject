# P24 — fault (ii) diagnosis, pre-declared BEFORE any measurement

**Written and committed before the instrumented build exists and before any leg runs.**
Session 2026-08-20, journal PART TWENTY-FOUR. Follows `p18`/`p21`/`p23` discipline.

## The question

Custom depth is NOT PRODUCED on exactly half the armed frames (15/30), in a FIXED per-burst
pattern — arm1 DUMMY · arm2 REAL · arm3 REAL · arm4 DUMMY — identical across all seven full
events (M-2, journal §164). `r.CustomDepth` is 3 at the pass point on all 30 armed frames
(M-1/M-3), so the cvar is exonerated. **WHY is the pass not produced on those frames?**

## The proximate fact, already established

`RenderCustomDepthPass` returned false for those frames ⇒ `StencilDummy` (1×1, 255) was bound
(`SceneTextures.cpp:959`, D-3). The 1×1-vs-view-sized extent is the discriminator, decided in
advance at P23.

## The engine chain, READ FROM SOURCE THIS SESSION (5.1, D:\UESource)

1. `CustomDepthRendering.cpp:131-148` — `FSceneRenderer::RenderCustomDepthPass` renders a view
   ONLY if `View.ShouldRenderView() && View.bHasCustomDepthPrimitives`; if no view qualifies it
   returns false.
2. `SceneVisibility.cpp:2470-2474` — `bHasCustomDepthPrimitives` is set ONLY inside compute-
   relevance, which runs ONLY over primitives in the view's VISIBLE set, when
   `ViewRelevance.bRenderCustomDepth` is true.
3. ⇒ with the mode fixed at 3, "custom depth not produced" ⇔ **no VISIBLE primitive wanted
   custom depth that frame.**

## Candidates — to DISTINGUISH, not to assume

| # | candidate | what it claims | what distinguishes it |
|---|---|---|---|
| **C-A** | no primitive carries `bRenderCustomDepth` at that moment | write side: the tag property was lost | `VerifyPendingTags` read-backs (per tick); D-2 already exonerated the write side on `P20_M26S1_RAMP` (`collisions=0`, property verified, still zero). A write-side FirstCollisionDetail on the new leg would re-open it |
| **C-B** | the tagged primitive is NOT IN THE VISIBLE SET on exactly those frames | the target was HIDDEN for the frame the mask armed against — the arm gate's `IsHidden()` read is ONE TICK STALE relative to the rendered state, because `FAnomaly_Blinking::Tick` (driven by `UAnomalyInjectorSubsystem::Tick`) toggles hidden AFTER `UAnomalyCaptureSubsystem::Tick` has already armed, and the toggle IS flushed into the same frame's render (F-1) | **M-4 below** |
| **C-C** | the pass is skipped for a reason unrelated to our tagging | some per-burst-position engine behaviour | if M-4's hidden sample does NOT track the DUMMY frames, C-B is out and C-C is live |

**C-B is the recorded LEAD (blinking half-period 3, m20 one-tick-stale hidden sample),
formalised. It is NOT a claim until the measurement below says so.** The last three candidate
repairs this milestone (stencil range, tag/arm separation, the cvar) each died when their
premise was checked first; this one gets the same treatment.

### C-B's worked prediction (stated so it can FAIL)

With the check one tick stale — C(t) = H(t−1) — and blinking phase locked to the burst
(half-period 3), the observed arm ticks {6,10,11,12} (mod 16) imply hidden phases {6,7,8} and
{12,13,14}, visible {9,10,11}: armed frame 6 renders HIDDEN (dummy), 10 and 11 render VISIBLE
(real), 12 renders HIDDEN (dummy). **That reproduces D-R-R-D exactly.** If the check were
same-tick fresh, no armed frame could render hidden and there would be no dummies at all —
already contradicted by the standing observation, but the direct per-frame sample below is the
evidence, not this arithmetic.

## M-4 — the measurement (log-only instrumentation)

Per ARMED frame, on the game thread at `FCoreDelegates::OnEndFrame` — i.e. AFTER every
subsystem/anomaly tick and AFTER the frame's render commands were enqueued, so the sampled
state is the state the frame rendered:

```
Capture(mask): M24 ENDFRAME id=<RequestId> target=... tag=... hiddenAtArm=0 hiddenAtEndOfFrame=<0|1>
```

Joined offline against the existing per-frame pass log (`M23 PASS id=... customStencilExtent=WxH`)
on the same RequestId. `hiddenAtArm` is 0 by construction (LOCK-1 refuses hidden arms); it is
printed so the stale-read claim is visible in one line.

## Predictions and REFUTERS

| # | statement |
|---|---|
| **P1** | every DUMMY armed frame (extent 1×1) has `hiddenAtEndOfFrame=1` |
| **P2** | every REAL armed frame (view-sized extent) has `hiddenAtEndOfFrame=0` |
| **P3** | per full event the position pattern of `hiddenAtEndOfFrame` is {1,0,0,1} |
| **R-A** | 🚨 ANY dummy frame with `hiddenAtEndOfFrame=0` ⇒ **C-B REFUTED** — the target was visible at render and the pass still did not run; report, attribute nothing, HALT |
| **R-B** | 🚨 ANY real frame with `hiddenAtEndOfFrame=1` ⇒ the sample does not discriminate; **nothing is established**; HALT |
| **R-C** | 🚨 control: a `missing_texture` leg on the same target (a NON-hide anomaly — the target is never hidden in-window) still produces ANY dummy armed frame ⇒ **the hidden-state mechanism is REFUTED as the sole cause** |

## Branch table

| branch | reading | verdict |
|---|---|---|
| **B1** | P1 ∧ P2 ∧ P3 on the blinking leg AND zero dummies on the missing_texture leg | **C-B ESTABLISHED**: the arm gate's hidden read is one tick stale; the mask arms against frames that render the target hidden; hidden ⇒ not in the visible set ⇒ no view has custom-depth primitives ⇒ pass not produced ⇒ dummy. REPORT AND STOP — no fix this turn |
| **B2** | R-A fires | C-B refuted. Report the frames, attribute nothing. C-C is live and UNDESIGNED |
| **B3** | R-B fires | instrument does not discriminate. Nothing established |
| **B4** | R-C fires | hidden-state is not the sole cause; report the split |
| **B5** | mixed / partial | report exactly what was seen, attribute nothing, HALT |
| **B6** | leg invalid (A63/B1) | discard, bank, re-run — how-it-ran, not what-it-showed |

## Task-1 expectations on the same legs (the frame-scoped discard fix), stated in advance

- Blinking control leg, full events: `arms=4 resolved=4 framesDiscarded=2 framesContributed=2`,
  `state=MEASURED_NONZERO`, `pctOfFrame` ≈ 7.2–7.3 (the banked rect is 7.80). The final event
  may be truncated by the 90-frame cap (D-5); its resolved frames follow P1/P2 and if EVERY
  resolved frame is discarded it must read `NOT_MEASURED`, never `MEASURED_ZERO`.
- `missing_texture` leg: `framesDiscarded=0 framesContributed=4`, `MEASURED_NONZERO` on every
  full event.
- 🚨 **NEITHER leg is F-6 validation.** F-6 needs BOTH controls (incl. `SM_Ramp2`) NON-ZERO with
  `collisions=0` and the 255 detector proven live BOTH WAYS. Fault (ii) is not fixed this turn,
  so `collisions=0` is not even reachable on the blinking leg. The `H5` legs STAY BLOCKED.
