# 081-01 — `m55` "measured change evidence": the plan

**2026-09-20. PLAN ONLY.** No source, shader, tool, fixture or schema change. No build, no cook, no
capture, no leg, no tag, no merge. Two documentation files on a new branch.

Deliverable: `docs/predictions/2026-09-20-m55-change-evidence.md`. **It goes to an independent Codex
review before any implementation**, and §12 of it carries six questions that are not decided here.

---

## §1 The state this was written against

- `master` **`031a103`** (080-06, the `m26` veto-arm-window merge). Branch **`feat/m55-change-evidence`**
  cut from it.
- **`m51` `53bf725` was never touched and the main checkout was never moved.** The work was done in a
  `git worktree` at `D:\IntrusiveAnomalies\_m55_plan`, outside the StackOBot tree — shared-tree rule 2
  covers doc-only commits on another branch, and putting the worktree outside `Plugins/` avoids UE's
  recursive `.uplugin` scan finding a duplicate. Leaving the checkout as it was found is therefore a
  property of the method, not something to remember at the end.
- `m55` is the **named** next unit: `080-04`/`080-05` parked `stuck_low_mip`'s default-on decision
  *pending `m55`*, and `G270` is the finding that escalated it.

## §2 What the plan says, in five lines

Measure, per captured frame and per tagged target, how many of the target's **own mask pixels** changed
against the **previous captured frame's delivered colour**, plus the mean per-pixel magnitude, plus the
same two statistics over the rest of the frame as a paired control. Publish all of it as additive
measurements. Latch a per-event `change_evidence` verdict at onset, and let `observable` consult it —
where the measurement is sound, and admitting wherever it is not.

## §3 The six things the source read settled

1. **Where the mask is made and where the delivered pixels are grabbed are two different post-process
   passes.** The mask subscribes to `EPostProcessingPass::Tonemap`
   (`AnomalyMaskSceneViewExtension.cpp:97-101`); the colour capture subscribes to
   `EPostProcessingPass::VisualizeDepthOfField` (`AnomalySceneViewExtension.cpp:72`); the enum order is
   `SSRInput, MotionBlur, Tonemap, FXAA, VisualizeDepthOfField`. ⇒ the mask is produced **two
   subscribable passes before the picture the client receives**, and the delivered colour is
   **post-FXAA** while the mask's own hook is pre-FXAA. This is what decided where `m55` runs.
2. **`m49` reads no colour at all.** `target_pixels` and `target_drawn_pixels` come from custom stencil +
   custom depth + scene depth (`AnomalyVisibleMask.usf:31,39-40,44,51`). ⇒ `m55` would be the **first
   colour-derived label evidence** in the producer. The one existing colour read is `m48`'s exposure dip,
   and it is CPU-side, whole-frame and subsampled (`AnomalyCaptureSubsystem.cpp:4061-4063`).
3. **The identity pin `m55` needs already exists.** The colour arm stamps
   `Snap.SessionIndex = SessionFrameIndex` (`:4372`) and the target-mask arm for the same frame is bound
   at the same tick (`:4397-4398`, consumed under `TargetMaskArmedTick == GFrameCounter` at `:891`), with
   the mask result keyed to a session index at `:1249`. Colour and mask for one session index are the
   same rendered frame.
4. **But the mask pass does not run on every rendered frame** — the SVE is active only while arms are
   pending (`AnomalyMaskSceneViewExtension.cpp:49-53`), whereas the colour callback runs on exactly the
   captured frames (`AnomalySceneViewExtension.cpp:107-110`). ⇒ **retention must ride the captured-frame
   cadence, not the arm cadence**, or the previous frame at an event's onset — the last pre-roll frame,
   which carries no labelled fire and therefore no target-mask arm — would never have been retained. That
   is the single constraint that chose the mechanism.
5. **The colour SVE already builds the exact texture `m55` wants to retain.** `OwnTexture`, the W×H
   sub-rect copy at (0,0) that becomes the PNG (`:139-145`). Extracting it costs a refcount, not a copy.
6. **`lighting_mismatch` has no mask and cannot be given one by this unit.** It matches
   `ULightComponent` by substring (`Anomaly_LightingMismatch.cpp:48`); the tag is written onto
   `UPrimitiveComponent`s; a light actor has none. ⇒ it already ships `target_pixels: -1` and
   `observable: null`, and `m55` changes nothing for it. A future `m53` must bring its own region source.

## §4 The correction to the brief, and why it is the most important line in the plan

The brief asks that `change_frac_target - change_frac_control` be **what the label carries**.

**`G260` (079-02/03) built that subtraction, gated it, and had it refuted by its own gates in both
directions at once:** it does not cancel camera motion (parallax is content-weighted), and it destroys
real signal on a still camera — a pinned bench onset read `d_region = 0.02286` against
`d_ring = 0.02872`, so the subtracted quantity went **negative at the exact frame the anomaly started**.
Its premise — that the effect is confined to the region — is false for these anomalies, and this project
had already measured it false three milestones earlier: `m26`'s `A35`/`A-4` found hiding `SM_Ramp2`
changed **more outside its own bbox than inside** (peak-OUT `0.2955` vs peak-IN `0.1785`). `G258` is the
same family.

The brief's justification for reviving it — *same frame pair, same render, no attribution claim* —
removes a **pairing** confound. `G260` was never about pairing. Both of its failure modes are properties
of the **scene**, and moving the measurement in-engine repairs neither.

⇒ The plan **publishes both fractions and subtracts neither**, and uses the control as a **refusal**
input, which is `G260`'s own stated rule: *where the surroundings cannot be a control, measure the
confound and refuse above a threshold.* 📌 **This costs the reviewer nothing** — both fields ship, so a
subtractive rule stays one arithmetic step away in any consumer if chat overrules, with the banked
evidence to test it on. What the plan declines to do is bake the subtraction into the label, which is
the irreversible half. Recorded as **Q1**.

## §5 The design finding that shaped the rule

**A temporal difference measures transitions, not states — and `observable` is a per-frame field.**

Every steady-state class in the catalogue (all of them except `lod_popping`) produces identical
consecutive frames once the anomaly has engaged. So the brief's literal D3 — `observable = current rule
AND this frame's change >= floor` — would mark the onset frame observable and **every subsequent held
frame not observable**, emptying `affected_frames` down to a single frame for every texture- and
hide-class event in the dataset.

The resolution is that change evidence is an **event-level** property, latched at onset and consumed per
frame. And it needs no lookahead, which matters because `labels.jsonl` rows are emitted progressively
from the drain (`AnomalyCaptureSubsystem.cpp:3997-4048`) and cannot be retro-edited: **the onset is the
first labelled frame, and the onset's change is measured on that very frame**, so the latch resolves
exactly when the first row that needs it is built.

`m52`'s `080-06` window is a gift here rather than an obstacle — it already puts the labelled window at
the first measurably-held frame, so the frame pair straddling it is exactly unheld→held.

## §6 Two cross-branch facts found while bootstrapping, recorded so they are not rediscovered

- 🚨 **`m51`'s `F1` is NOT on `master`.** `git merge-base --is-ancestor 6713f51 master` **exits 1**, and
  neither `bAwaitingTargetMask` nor `TargetMaskWaitTicks` appears in `master`'s
  `AnomalyCaptureSubsystem.cpp`. `m55` built here therefore does not interact with `F1` **as code**. It
  interacts with it as a **design** twice — `m55` adds a second per-frame readback against `R7`'s open
  writer-byte bound, and it adds a second outcome a frame can be waiting on, to which `F1`'s rule (*wait
  only for an outcome that can exist*) must extend. Neither blocks `m55`; both need reconciling at
  whatever merge brings the two together.
- 🚨 **Every GPU route needs a FULL COOK** (`G129`) — extending the existing reduce as much as adding a
  pass, because both move a global shader parameter struct. That retires a container quintet
  byte-unchanged since `m49` Phase B and every binary↔container pairing in `_binary_baselines`. Because
  that may be refused, the plan costs a **CPU fallback with identical semantics and no cook** (§7.4)
  rather than treating the cook as settled. Recorded as **Q2**.

## §7 What the plan deliberately does not do

- It does not choose a single floor or `tau`. §8 is a **derivation procedure** — two-sided against
  negative frame pairs for `tau`, and against banked legs whose visibility is known from **outside** the
  artifact for the class floors, with `G260`'s corollary honoured (an onset read from the delivered masks
  is circular, because `m49` A1 defines `affected_frames` as the observable subset). All values are
  predeclared in an amendment **before** the implementation gate.
- It does not flip `GAutoPoolDefaultEnabled`, raise `MaxCoAffected`, tune the `m52` ratio, or build S2′.
- It does not move `annotation.json` (`P6` intact) or `label_schema` (`m51` owns the bump to 3).
- It does not let `change_*` become an input to the `m26` veto. The 077 ruling removed a veto built on
  `target_drawn_pixels` for exactly this reason; `G258` is the standing warning.
- It does not claim causation. `G263`'s dated correction — *masks identify the pixels, not the cause* —
  is not repealed by moving the measurement in-engine, and the plan says so in its own §0.

## §8 State at close

`master` `031a103` untouched. `m51` `53bf725` untouched; main checkout never moved. Branch
`feat/m55-change-evidence` carries two docs commits and is pushed. The worktree at
`D:\IntrusiveAnomalies\_m55_plan` is removed at close; the branch remains. CaptureBench not touched. No
binary staged, no container touched, no bench process started.

## §9 Next

**Codex reviews the plan.** Then chat rules on §12's Q1–Q6 — the subtraction, the cook, whether
`observable` changes in `m55` at all or in a follow-up, `K`, the light class, and the `stuck_low_mip`
default-on decision that has been waiting on this unit.

⛔ **Do not implement, do not cook, do not run a leg, do not flip a default, and do not start `m53`/`m54`
or S2′ — none of them unprompted.**
