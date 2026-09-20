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


## AMENDMENT 1 — 079-10 rulings and predictions (2026-09-20, before implementation/results)

Authority: the owner requested implementation of
`D:/IntrusiveAnomalies/_reviews/079-10-codex-delta-boundary-masks-and-rulings.md`.
Base `7d5b6e919f9bca3506b9057a56a4349ea300ae64`, same feature branch and scope;
Codex implements, Claude Code independently reviews next, Chat orchestrates.
The original predictions and their measured misses above remain historical.

### Rulings now governing the computation

- R1 permits a missing target mask OUTSIDE a run to be extrapolated from that
  same run's nearest labelled boundary: onset before the span, last labelled
  frame after it. Actual delivered target masks always take precedence. Missing
  masks inside the span are still unassessable. Both members of a compared pair
  independently select their actual/extrapolated masks; their union remains the
  measurement region. Every observation records each extrapolation source and
  signed frame delta, including masks used to learn its baseline. A run prints
  the largest absolute extrapolation used. NO-TRACE may use extrapolated edge
  pairs, with that fact stated on its line.
- R2 accepts CONSISTENT when a zero-delta transition coincides with a high ring
  change, with a run-level caveat and a count of consistent events with a caveat.
  **P1's original "lighting_late_onset never CONSISTENT" prediction was wrong:**
  the rule selects the stronger lighting transition at the labelled frame; it
  cannot identify which cause produced that transition. The ring is diagnostic.
- R3 defines NO-TRACE as no target change ABOVE the printed noise floor over the
  full span/windows. The one-pixel fixture remains NO-TRACE: 1/2500=0.0004, below
  tau=0.004. P6's wrong-NO-TRACE class now means an in-span or edge-window pair
  exceeded tau, not merely that a nonzero number of pixels changed.
- R4 derives the event token from run outcomes only: NO-TRACE > OFFSET-NOTE >
  PARTIAL > CONSISTENT. UNASSESSABLE/READING runs remain visible in coverage
  counts but cannot promote an event on the strength of their edge observations.
  With no assessable outcome, all-reading events are READING; otherwise they are
  UNASSESSABLE. `blank_missing_interior` is now an UNASSESSABLE event.
- R5 retains the class allowlist as a producer contract, inclusive labelled
  bounds/duplicate-claim refusal and their coverage cost, RGB-channel differences,
  session extent from labels plus RGB files, and batch/exit contracts.

### Implementation interpretations declared before tests

1. `N_BASE` is the verifier's existing `BASELINE_MAX_FRAMES = 24`, not the separate
   measure_label_offset tool's baseline count. Maximum extrapolation distance is
   `edge_window + 24` (default 28) from this run's span, inclusive. No boundary
   template is used beyond that distance; unavailable baseline pairs are skipped,
   and the existing minimum of three valid baseline pairs still applies.
2. Missing files/references, an empty valid PNG, or a valid PNG without this
   target's requested ID constitute absence outside the span. Corrupt/unreadable
   PNGs or invalid dimensions remain errors, not permission to replace evidence.
   An unresolved ID with multiple possible values remains unassessable. A real
   target mask that exists is never replaced, even if it disagrees with the
   boundary shape. A boundary template must itself be valid; no recursive fallback.
3. Target identity for actual/boundary masks is matched to the SAME target/run.
   A sole unrelated event entry on an off-span row must not supply that target's
   mask ID. Explicit in-span ID mismatch remains unassessable; unresolved
   sole-value fallback remains tagged and excludes NO-TRACE.
4. If a run has some delivered in-span masks, it stays in mask mode even when one
   boundary file is missing: the missing in-span frame must refuse that edge under
   R1. Thus the old `mixed_run` fixture with only frame67 missing becomes PARTIAL,
   with two eligible edge cells (one unassessable). A truly bbox-only run remains
   READING and supplies zero scored cells. This is a disclosed change to the old
   boundary-based mode selector, required by R1's in-span rule.
5. Summary CONSISTENT n (c with caveat) counts events whose final token is
   CONSISTENT and which contain a caveated CONSISTENT run. Every run still prints
   its own caveat. A separate session run-coverage line prevents a CONSISTENT event
   with an UNASSESSABLE sibling run from hiding that missing coverage under R4.
6. The calibration procedure and immutable planned key set stay unchanged. The
   rederived numeric cap or NO ADMISSIBLE ENVELOPE becomes the configured default;
   if derivation has no usable data, retain an explicitly unvalidated old value
   and report NEEDS-DECISION. No threshold is chosen to make the bench pass.
   Final bank readings use the final configured default. Semantic fixture controls
   use an explicit permissive cap (1.0), with separate numeric/zero/sentinel guard
   regressions, so they test the observation rules even if calibration refuses all.

### P1–P6 for this delta

**P1:** Keep the permanent 83 fixtures with R1–R4's changed known answers and add
regressions for boundary-only masks; actual masks overriding extrapolation;
missing interior/onset/end masks; the exact extrapolation distance boundary;
an unrelated off-span target; unreadable masks; CONSISTENT with a lighting caveat;
and aggregation of CONSISTENT plus UNASSESSABLE / PARTIAL siblings. The entire
self-test and batch/exit contracts must pass. Wrong-NO-TRACE above tau is forbidden.

**P2 (supplied, unchanged):** Six pinned legs: every edge observed (64), each
leg has four CONSISTENT events, tau=0.0040, NO FAILURE FOUND (4 consistent ...),
exit0. Extrapolation is printed, never passed off as an actual delivered mask.
Inclusive windows and duplicate-claim guards remain enabled; their accepted
short-run coverage cost may still conflict with this prediction. Report a miss;
do not change the bounds or dominance threshold to force agreement.

**P3:** A2L_LEGA, LYRA_SMOKE_01 and A1L_LEGA: full per-edge lines, no NO-TRACE
expected. Also retain the M50L_LG9 bbox-only reading as a regression.

**P4:** Three-session scorer v3: print immutable 572-key denominator before
scoring, retain scored and unscored ledgers, report all five scored classes,
positive support and refusal reasons. Cells are expected to score now. Rederive
and report the cap or NO ADMISSIBLE ENVELOPE exactly as measured; no tuning.

**P5:** Report-name collision protection and execution-error precedence remain;
NO-TRACE alone produces2, report-only suppresses2 and preserves3.

**P6:** Independent Code review remains pending; the negative's falsifier is an
above-tau in-span/window pair under the selected actual/extrapolated regions.
The documented producer-class assumption and extrapolation limitation remain.

**Disposition rule from 079-10:** GREEN only if P2 reads as supplied AND the
self-test passes; NEEDS-DECISION otherwise. GREEN for this delta is not merge,
client, campaign or release approval.

**Chat review NOT REQUIRED** for this authorized delta implementation and push.
**Chat review REQUIRED** for the completed evidence and independent review before
merge; that is the next mandatory checkpoint. No mailbox, merge, tag, build,
capture, Source/Shaders/measure_label_offset edit or m51/master edit.


## AMENDMENT 2 — 079-12 local peaks and ordered nearest assignment (2026-09-20, before implementation/results)

Authority: owner supplied `D:/IntrusiveAnomalies/_reviews/079-12-codex-ruling-monotone-nearest-assignment.md`.
R6 in 079-11 is withdrawn. Its clarification report remains unchanged. Base is
`e6351ce396c53ede5a7acd8186497811776e9e34` on `fix/verifier-field-validity`.
Codex implements; Claude Code independently reviews next; Chat orchestrates.
This amendment is committed before implementation or any new fixture/bank/sweep results.

### R6-prime computation and interpretations

1. Keep per-edge regions, tau, inclusive labelled bounds and every hard refusal
   (missing/corrupt RGB or masks, identity, baseline, cap and session coverage).
   A local peak k has d(k)>tau and d(k)>=1.5*d(k-1), d(k)>=1.5*d(k+1).
   Interpret "observed range" as this edge's existing scanned window [lo..hi];
   neighbours outside that window count as zero. No new neighbouring input is
   read outside the window to decide peak membership. Threshold comparison stays
   strict, the two peak comparisons inclusive. No global winner/dominance test.
2. Group all runs' labelled edges by (target, mode), order by labelled frame,
   onset before end at a tied frame, then event index and run ordinal for a
   deterministic tie. Each edge subtracts previously assigned frame numbers
   from its own peaks, chooses nearest to its nominal frame, tie earlier, then
   consumes that frame. Hard-refused/truncated edges consume nothing. Keep the
   labelled-bound window calculation; never clip it using a detected frame.
3. A selected peak yields TRANSITION; remaining available alternatives print on
   that same line. A window with no peaks yields NO-TRANSITION. A nonempty peak
   set entirely consumed by earlier edges yields the specified UNASSESSABLE
   reason with the consumed frames. Structured detail retains original peaks,
   candidates available at this edge's turn, and previously consumed peaks.
4. If either run window held >1 peak, disclose both assignments on the run line
   as nearest of the candidates available at that turn (none when unavailable).
   Same-target/mode scope and onset-first single-run handling apply everywhere.
   Retain full-span above-tau checking: no local peak is NOT by itself NO-TRACE.
5. Follow the explicitly specified ordered greedy walk without an additional
   ordering constraint. Exclusion establishes uniqueness, but does not generally
   prove increasing assigned frames when edge regions/tau yield different peak
   sets. Do not silently add a last-selected-frame floor. Add a narrow regression
   documenting this limitation and report it for Chat's semantic disposition.
6. All R1-R5 rules remain except the old single-winner and duplicate-claim
   refusals, now expressly replaced. Ring remains diagnostic at the SELECTED
   peak; event/run aggregation, NO-TRACE and batch/exit rules stay unchanged.
   Reuse AMENDMENT 1's cap derivation and cap=1 semantic fixture controls.

### Predeclared readings and known specification conflict

- Supplied blink control: labels [4..5],[8..9], peaks4/6/8/10 -> assignments
  4/6/8/10, all delta0, both runs CONSISTENT. B.onset label9 -> assignment8,
  delta-1, B.end10 delta0; event OFFSET-NOTE. Use actual masked RGB fixtures.
- Supplied missing-gap prediction is preserved: without peak6, A.end
  NO-TRANSITION, B.onset8, A PARTIAL/B CONSISTENT. **This contradicts the specified
  walk.** Under unchanged windows A.onset takes4; A.end [4..8] can still take8
  (delta+2); B.onset [6..10] then takes10 (delta+2); B.end [8..14] has only consumed
  8/10 and refuses. Literal expected result: A OFFSET-NOTE, B PARTIAL, event
  OFFSET-NOTE. A missing-6 fixture will contain exactly peaks4/8/10, with stable
  pairs thereafter. Do not reserve8 for B or invent a gap-local no-peak rule.
  Record the supplied prediction as a miss if this reading is confirmed.
- 079-07/08 adjacent late second onset, true10..11 and15..16, labels10..11 and
  16..16: take10/12/15/17, B.onset delta-1. New expected OFFSET-NOTE replaces
  PARTIAL. A.end must be12 (delta0), never15 (the old END-SHIFT(+3) class).
- Adjacent aligned changes UNASSESSABLE -> CONSISTENT; adjacent early B onset
  changes UNASSESSABLE -> OFFSET-NOTE; mixed bbox/mask late B onset is expected
  PARTIAL -> OFFSET-NOTE under the separate mode walks.
- Single-frame c08 aligned remains UNASSESSABLE: adjacent equal changes60/61
  suppress each other as local peaks, and full-span change blocks NO-TRACE.
  c08 late changes PARTIAL -> OFFSET-NOTE (end61 at window boundary, delta-1);
  c08 early changes UNASSESSABLE -> OFFSET-NOTE (onset60 at boundary, delta+1).
  These explicitly use the observed-window interpretation above.
- Lighting, occluder and every other existing fixture retain their old outcome
  unless local-peak membership or the ordered walk changes it. Before changing
  any further expected string, record the first observed mismatch and explain
  its cause; list EVERY changed event/run/edge expected reading against the
  frozen 079-10 evidence. Such changes are measured corrections, not retroactive
  predictions. Missing-data/cap/identity guards must still take precedence.

### P1-P6 and disposition

P1: retain all 90 prior fixtures, with disclosed R6-prime changes, and add the
three ruled blink controls. Add meaningful local-peak/nearest/tie/exclusion,
scope, hard-refusal, disclosure and ordering-limit contracts. No wrong-NO-TRACE.
P2-prime: all six pinned legs, 64/64 edges observed, four CONSISTENT events per
leg, tau0.0040. Compare the four non-blink legs' observations and numbers to
079-10; header/cap/new disclosure text may differ.
P3: rerun A2L_LEGA, LYRA_SMOKE_01, A1L_LEGA and M50L_LG9; show complete readings.
P4: rerun the unchanged 572 planned-key scorer sweep, both guard settings;
derive and install the measured cap without tuning; retain all ledgers and
report scored classes, unscored reasons and original-edge positive support.
P5: retain batch collision tests and direct CLI 2/0/3 exit contracts.
P6: independent Code review remains pending; preserve producer-contract,
noise-floor and extrapolation limitations and disclose assignment limitations.

Report `_reviews/079-12-codex-delta-report.md` with the changed-expectation table,
per-P evidence, source/artifact hashes, diff summary and measured wall time.
GREEN only if P2-prime AND self-test readings match the supplied predictions;
otherwise NEEDS-DECISION, even if regressions for the specified algorithm pass.
Commit/push the feature only, restore m51, preserve 079-11's report and all
protected/unrelated files. No mailbox, brief, build, cook, capture, merge or tag.

**Chat review NOT REQUIRED** for this authorized implementation and feature push.
**Chat review REQUIRED** for the resulting evidence, prediction conflicts and
independent Code review before merge; this is the next mandatory checkpoint.
Client/Section G, m51 campaign and release holds remain.
