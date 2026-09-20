# 079-09 predictions — consistency observations

Written before implementation/results on 2026-09-20. Base: `1cbc07e` on
`fix/verifier-field-validity`. Authority: the owner's implementation request and
`D:/IntrusiveAnomalies/_reviews/079-09-spec-for-codex-consistency-semantics.md`.
Chat orchestrates; Codex implements; Claude Code independently reviews afterward.
No merge, tag, build, capture, Source/Shaders/measure_label_offset edit or mailbox use.

## P1–P6 — supplied predictions, retained before measurements

**P1:** Every 079-05/07/08 fixture becomes permanent with its new known answer.
lighting_aligned => OFFSET-NOTE (or CONSISTENT when the lighting step coincides
with the label), ring note, never FAIL; lighting_late_onset => OFFSET-NOTE, never
CONSISTENT; occluder_aligned => CONSISTENT or OFFSET-NOTE, never FAIL;
missing_mask61 => UNASSESSABLE(mask missing at 61); no_detected_end => PARTIAL;
foreign_mask_value => UNASSESSABLE(mask id 222 not present); the 15->16 ordering
fixture => both competing edges UNASSESSABLE(claimed by two edges) or correct
delta, never a wrong delta; mixed_run => READING and zero scorer cells.
Old NOT-VISIBLE cases => NO-TRACE. One missing interior-span mask => UNASSESSABLE,
never NO-TRACE.

**P2:** Six pinned bench legs: four CONSISTENT events each, NO FAILURE FOUND
(4 consistent ...), tau=0.0040, 64 observed edges, exit 0.

**P3:** A2L_LEGA, LYRA_SMOKE_01, A1L_LEGA: no NO-TRACE, full per-edge lines,
outcomes as measured. M50L_LG9: six READING, UNREAD-BBOX-ONLY.

**P4:** Scorer v3 on the three masked sessions; immutable denominator first;
recovered/wrong/no-transition/unassessable/unsupported; cap rederived.
READING/UNASSESSABLE runs supply no scored cells.

**P5:** Batch collision protection and exit contract survive. Only NO-TRACE can
produce label-pixel exit2; report-only suppresses2 and preserves execution error3.

**P6:** The independent reviewer supplies accurate-mask/known-timing fixtures;
outcomes follow spec section1. A NO-TRACE when pixels changed within the span is
the remaining P1 class named by the supplied spec.

## Interpretations and conflicts recorded before results

1. Sections1–2 govern computation. `lighting_late_onset` has the lighting
   transition at the late label and a correctly timed clear; section1 requires
   CONSISTENT, while P1 forbids it. Keep the prediction and report the conflict.
2. Every compared masked pair uses both actual masks, including baseline pairs.
   A boundary snapshot is never copied into missing frames. Banked captures may
   lack masks on clean frames; P2 remains unmodified.
3. Nominal +/-window searches are clipped to session bounds and the preceding/
   following labelled edge frames, inclusively. Overlap allows duplicate-claim
   detection. Detected frames never clip windows. Masked edges and bbox readings
   have separate bound/claim groups for each target.
4. Missing boundary mask files produce a bbox READING run, preserving P1 mixed_run.
   A delivered invalid-ID mask is UNASSESSABLE, never silently a bbox fallback.
   Missing interior masks invalidate the affected edge; a missing span mask blocks
   NO-TRACE. Absent and malformed inputs remain distinguishable in diagnostics.
5. Two observed edges with an off-label TRANSITION yield OFFSET-NOTE. A zero-delta
   TRANSITION plus NO-TRANSITION yields PARTIAL. Two NO-TRANSITION edges need the
   complete span check/eligible class for NO-TRACE; otherwise UNASSESSABLE(reason).
   Unresolved mask IDs forbid NO-TRACE.
6. The original edge x perturbation key set is the fixed planned denominator.
   READING/UNASSESSABLE runs return no scored cells; their keys remain in an explicit
   unscored ledger. Scored plus unscored must equal planned. Rates use planned N.
7. NO-TRACE uses the specified d > tau test, not bitwise equality. The existing
   one-pixel fixture (d=0.0004 < tau=0.004) can satisfy it. This conflicts with an
   absolute interpretation of P6's "pixels did change". Expose thresholds and
   report the conflict; do not tune a fixture or threshold to change the outcome.
8. Print ring change for every TRANSITION. A ring fraction above the edge tau is
   annotated "whole-region change (lighting/camera?)". Diagnostic only; no outcome
   depends on it. An unavailable ring reads n/a.

## Regression assertions before the first implementation test

- Original bbox fixtures remain READING/exit0; exact-mask aligned controls are
  CONSISTENT; offset observations alone never cause exit2.
- Lighting/occlusion/motion/dilation/lag never fail solely because of an offset.
- Missing RGB/masks, empty/unreadable masks, explicit ID mismatch, insufficient
  baseline, unsatisfiable tau, oversized regions and ambiguous/duplicate transitions
  produce UNASSESSABLE observations with reasons.
- No-signal plus one zero-delta transition remains PARTIAL at every aggregation level.
- Duplicate-claim invalidation affects every claimant; bbox runs impose no masked bounds.
- Complete blank controls exercise NO-TRACE/exit2; an interior above-threshold
  transition, unknown/non-pixel class or unresolved ID blocks NO-TRACE.
- Numeric cap zero differs from NO ADMISSIBLE ENVELOPE.
- Batch retains both colliding-flattened-path reports; forced hash collision writes
  no reports; an execution error remains3 with report-only.

**Chat review NOT REQUIRED to implement this approved unit. Chat review REQUIRED
for specification conflicts, independent Claude Code review and evidence before
merge.** The next mandatory Chat checkpoint is disposition of the implementation
report and independent review. Client/Section G, m51 and campaign holds remain.
