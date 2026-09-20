# 079-09 — consistency observations, implemented by Codex

## Authority and starting state

The owner requested implementation of
`D:/IntrusiveAnomalies/_reviews/079-09-spec-for-codex-consistency-semantics.md`.
This unit explicitly swaps roles: Codex implements; Claude Code independently
reviews afterward; Chat orchestrates. No mailbox use or review brief was issued.

Started from `fix/verifier-field-validity` at
`1cbc07e0fb21224f6a136871270766d8155481ce`. Original checkout: m51
`53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`; master/origin-master
`ac13700d99fe116056f5aa11611f8931a563a3f2`. CaptureBench starts at
`472a409dc70ae78d01a38d08fc3bddb15fe41c58`; its permitted scorer was already
untracked and remains a local review artifact. That repository has no remote.
Existing untracked handoffs/tools are preserved. No Source, Shaders or
measure_label_offset.py edit, merge, tag, build, cook, capture or owner-app change.

Predictions commit **`2a38d159a5402a363736d7854bcee287c2aec711`** precedes the
implementation commit. It creates
`docs/predictions/2026-09-20-verifier-consistency-semantics.md` and adds a dated
pointer to the previous predictions, preserving the supplied P1–P6 and the
conflicts identifiable before measurements. The implementation report records
the final feature commit, push verification and restored checkout.

## Implementation

- `tools/verify_capture.py`: explicit edge observations, run outcomes and event
  counts. CONSISTENT and OFFSET-NOTE do not establish cause; only a complete
  whole-span NO-TRACE returns code 2. Report-only preserves execution errors.
- Every comparison uses actual masks from both frames, their union and exact
  requested identity. Zero/unresolved sole-value fallback is tagged and cannot
  support NO-TRACE. RGB dimensions and coverage are checked. Label rows retain
  the declared session extent when endpoint RGB files are missing.
- The signal counts changes in any RGB channel: equal-luminance colour changes
  cannot disappear through grayscale conversion. Ring measurements are diagnostic.
- All windows use labelled bounds before detection; duplicate transition claims
  invalidate all claimants. Bbox and masked runs have separate bound/claim groups.
- Run eligibility is exported. CaptureBench scorer v3 returns no scored cells
  for READING/UNASSESSABLE runs, preserving their original planned keys in a
  separate unscored ledger. Rates retain the complete denominator. Its
  `IAI_VERIFIER_TOOLS` override permits review against the frozen feature tool
  after the original checkout is restored.
- README Step 6, the module header and Section G's prospective note use the new
  semantics. G263 receives a dated correction, G264 records single-edge ownership.
  The historical Section G reading and earlier prediction bodies remain intact.

Eligible producer IDs are `missing_object`, `blink`/`blinking`, `missing_texture`,
`corrupted_texture`, `lod_popping`, `lod_corruption`. Read
`AnomalyCaptureSubsystem.cpp:257` (`MapAnomalyToClient`), `:278`
(`ResolveAnomalyActiveSource`), `:4620` (`IsFireLabelledThisFrame`) and the
injector headers' `GetId()` implementations. The type list excludes time dilation,
lighting mismatch, camera clipping and unknown IDs. Type membership does not
independently prove that a particular injection must produce a visible change.

## Validation and the limits of closure

The permanent suite passes **83 cases** and contains the 079-05/07/08 fixtures, the 15-to-16 ordering case,
whole-span negatives and missing-mask counterexamples. Additional regressions
cover RGB dimensions, equal-luminance colour changes and missing endpoint RGB.
The last two endpoint cases were discovered during the final source audit:
clipping to existing files had hidden a missing first/last RGB frame and allowed
NO-TRACE. Using the union of labelled and delivered indices preserves that missing
coverage, so both cases become PARTIAL with an unassessable affected edge.

Four separate batch/region contract tests prove flattened-path report collisions
retain both readings, a forced identifier collision writes no reports, execution
error 3 survives report-only alongside successful sessions, and the union includes
a changed pixel present only in the previous mask. The batch self-test dependency
is stubbed in those focused tests; the complete image self-test runs separately.

Ten bank sessions were read in full. Six pinned legs: four UNASSESSABLE events
each, all **64 edges unassessable**, tau unavailable, exit 0. A2L_LEGA: seven
UNASSESSABLE; LYRA_SMOKE_01 and A1L_LEGA: six each; M50L_LG9: six READING and
UNREAD-BBOX-ONLY. None produces NO-TRACE. This is lost coverage, not label evidence.

The three-session calibration has **572 fixed planned keys**: A2L 208, Lyra 208,
A1L 156. Both guard states have zero scored cells and 572 unscored keys (392 from
UNASSESSABLE runs, 180 from READING runs). There is zero positive recovery support.
The derived cap is **UNDETERMINED, N=0 eligible baseline cells**. The earlier
`0.00014` cap is retained explicitly as a legacy heuristic, not revalidated.

The complete 78-group guard-off/on sweep used the retained calibration snapshot.
The only subsequent runtime change preserves missing endpoint RGB coverage. All
**39 calibration shadow inputs** were rebuilt and their old/new session bounds
were verified identical, so this change cannot alter their windows or sweep
readings. The final candidate then passed all 83 fixtures, the four contracts,
the scorer self-test, four scored-class controls, three CLI exit checks, and a
fresh reading of the ten unperturbed bank sessions. Snapshot hashes and the
small source diff are retained; no full-sweep claim is silently transferred to
an unrelated source revision.

The full report and exact readings are under
`D:/IntrusiveAnomalies/_reviews/079-09-codex-implementation-report.md`,
`079-09-evidence/` and `079-09-frozen/`. The report includes final test counts,
source hashes, exact per-P lines, finding-by-finding locations, commands and refs.

## Decisions still required

1. **P1 lighting contradicts section 1.** In `lighting_late_onset`, a stronger
   lighting transition coincides with the late label; both selected deltas are
   zero. Fixed semantics require CONSISTENT, while P1 forbids it. The tool follows
   the fixed rule and makes no claim that the known late label is correct.
2. **P2 is missed under strict coverage.** The six pinned legs lack the masks
   needed around the windows/baseline; the former 64 readable-edge claim depended
   on substituting a boundary mask. Calibration therefore cannot deliver P4's
   requested numeric rederivation. No constant or fixture was tuned to hide this.
3. **P6's absolute negative contradicts the threshold rule.** The accurate-mask
   one-pixel fixture changes d=0.0004, below tau=0.004, and meets the specified
   NO-TRACE rule. Output/docs state the threshold qualification; an absolute
   claim that no pixels changed is unsupported. Producer class membership adds
   a second assumption, not a proof of visible effect.
4. Boundary-mask absence is interpreted as READING to preserve P1 `mixed_run`;
   missing masks within an otherwise masked window remain UNASSESSABLE. Two
   NO-TRANSITION edges whose span cannot support NO-TRACE give an UNASSESSABLE
   run, potentially a PARTIAL event because its edges were observed. Original
   planned keys stay in an unscored ledger when run eligibility supplies zero
   scored cells. These interpretations were predeclared and need review.

## Disposition, ownership and measurement

**NEEDS-DECISION.** Implementation of the fixed rules is complete and the feature
branch is pushed; this is not merge approval or campaign/release GREEN. The
original m51 checkout is restored at the final boundary. No independent review
has been claimed: Claude Code owns that next review under the role swap.

**Chat review NOT REQUIRED for the authorized implementation and feature push.**
**Chat review REQUIRED for the above conflicts, independent Code review and
evidence before merge.** The next mandatory Chat checkpoint is disposition of
this report and the independent review. Section G, client and m51 campaign holds
remain. The same Codex workstream can continue; no new session or context-setting
change is recommended.

First recorded task clock: **2026-09-20 05:42:55 UTC (11:12:55 IST)**. Exact closing
clock and elapsed wall time are recorded in the external implementation report.
This interval includes review, coding, tests, documentation, commit and push;
it is not a CPU-time measure. Monetary charge/token billing is unavailable to
this task, so no cost figure is invented. No paid service, build or capture was
launched for this implementation.
