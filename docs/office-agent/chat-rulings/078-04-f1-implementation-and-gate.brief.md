# NAME: 078 m51 F1 effective mask resolution
# EFFORT: xhigh
# MODEL: claude-opus-5
# ACK-VERDICT: Codex read the complete 078-03 report and Chat's 078-03 review. Chat explicitly approves only F1 implementation and its bounded StackOBot campaign. The milestone plan remains CHANGES REQUIRED. This brief releases only the work below; all other findings remain held.

# 078-04 — implement and measure F1 only

Continue the existing session (last report 51% context); do not reread the whole milestone narrative. Read this brief, Chat review section 3, relevant current source, M-F1 manifest record and existing harness contracts. Code implements and measures; Codex orchestrates. Use Windows PowerShell, not the broken Bash path. No mailbox maintenance. Runner writes the final report.

Expected repo `D:/IntrusiveAnomalies/StackOBot/Plugins/AnomalyInjector`, branch `m51`, HEAD `760ad9473aa977f2789fb4a43777f0a9d11133c7`, master `ac13700`, source backup `be16977`. Ten untracked owner handoffs preserved. Chat review `D:/IntrusiveAnomalies/_reviews/078-03-chat-predictions-review.md`, SHA256 `2DCB7B257F3B733FF544458727F764BA18812AAFAB4D8AD6D50F06C19142CF38`. Verify state and report discrepancies without overwriting them.

## Scope and technical rulings

Implement **F1 only**: a requested mask that is intentionally unavailable must not hold completed RGB frames waiting for an outcome that will never exist. Keep the requested-mask label contract separate from whether this snapshot awaits a real outcome. Add a per-snapshot awaiting flag and per-snapshot hold counter; retire subsystem-wide `TargetMaskHoldTicks` and its reset. Derive awaiting from effective availability, including actual SVE-path availability. Review `bTargetMaskEffective` construction: it currently does not include `bSveCapture`, so merely copying it is insufficient for the backbuffer case. Prevent impossible mask arming as well as impossible waiting. Initialize unmeasured results honestly when not awaiting. Existing valid-mask behavior, wait bound, Phase B DrawnCount and exposure operands must survive. Do not rewrite the drain, schema, writer, lifecycle, geometry, tag leases or event membership.

Likely source footprint: `AnomalyCaptureSubsystem.cpp`, its public header (remove old member), and `AnomalyLabelWriter.h` (snapshot fields). Any additional production file must be justified as necessary for F1 before editing; if it belongs to another finding, STOP and report. No new runtime lever required. No shader/engine/fixture/content edits, no cook, no Lyra checkout/build/leg, no push/merge/tag. Keep changes under ANOMALY_CAPTURE as appropriate. Preserve disabled-build compilation structure by inspection.

**Do not implement Chat's table-row 6 proposal** `injected_frames := RequestedIndices intersect committed`: it changes the already agreed meaning. R1 and the two-ledger disposition in 078-03 remain in force; any alternative belongs to a separately agreed migration. Do not change `observability_complete` semantics either. Chat's PNG formula is a proposal to verify in the later writer tranche, not approval of a proved bound. Neither topic blocks F1.

## First: write the exact F1 predictions, before any leg

Edit only the M-F1 manifest record to describe this **intermediate schema-2/F1 candidate**, its A/B runs and the approved regression-smoke dependencies. Add a short dated disposition to the predictions file (target <=50 lines). Leave unrelated manifest records untouched and explicitly held. A narrow F1 snapshot does not implement m51 schema 3: **do not require or emit future `capture_complete`, `requested_frames`, `written_frame_indices`, schema_version or other new milestone fields.** Check completeness from existing rows, image filenames, summary and request/arm/finalize logs. Preserve label_schema 2. Remove M-F1's misleading whole-milestone allowlist in favor of this actual change's predeclared differences.

Write full supported commands for StackOBot natural/synth, frozen A-side and candidate B-side, with unique bank names. No `...`, imaginary options or immediate commands described as mid-capture. Explicitly set/read back SVE=1, mask requested ON, mask-measure ON, census ON, OutputHeight=360, 1280x720 input, PNG, Delivery=0, Pace=1, Fps=30, seed=4242, MaxFrames=90, targeted blinking / StaticMeshActor_49. Verify actual console command names and effective readings from the current tree; do not invent a knob. Use existing runner's capture schedule unless a required minimum needs a predeclared adjustment. Lyra is OPEN-until-recipe in this tranche, not waived or passed. Backbuffer handling is implemented and inspected, but **M-F1b runtime leg is not released** by this brief.

Predict the actual F1 failure signature from source: on A, intentionally unavailable masks cause bounded repeated holds and unresolved/dropped requests; do not assume zero rows or exact dropout counts without derivation. On B, no wait for an impossible outcome, no associated NEVER RESOLVED/finalize-drop signature, full expected requested RGB/row index sets, output height 360, no promised mask PNG, and anomaly rows retain the existing unmeasured representation (-1/null/unmeasured as actually serialized). No observable-positive assertion from an unavailable mask. Freeze exact expected schema-2 keys and their omission rules from source. Compare per-index results and explicitly explain new rows/events recovered from A-side drops; do not require identical A/B event sets where F1 recovers them.

Certification requires >=3 counted events; a defective A-side is a falsifier, not a passing certification leg. Record its event attempts/counts honestly. Any natural B leg below the minimum is INVALID. Bank every attempt, respect existing A63/B1 comparability rules, do not silently lower them or discard failures.

Also pin the exact B-side ONSET/PAIRING/G7 smoke recipes before their first leg, both orders, with effective masks ON and native output so the probes have a subject. Use the existing m44_gates.py and m44_pairing_probe.py; inspect their actual verdict behavior and consume printed gates as well as exit codes. The normal 100/50 pairing arms may share the applicable ONSET/G7 runs. No broader regression campaign, new auditor, new P-C7 adapter, or new harness implementation in this brief. Read-only deterministic analysis of produced artifacts may be saved with the evidence; it is not a new shipping tool.

Commit the bounded predictions before the first leg. The reviewer has approved this executable elaboration; no second narrative review loop is needed unless a premise fails or a new design decision is required.

## Implementation, build, provenance and legs

Use the existing CaptureBench runners and proven Windows code-only build/stage procedure. Read the relevant prior build journal rather than inventing a packaging route. Preserve the frozen container. Archive and hash every existing binary before replacement; A-side must remain byte-identical in `_binary_baselines`.

A-side executable is **B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8**, with Phase B container set pinned in manifest.provenance. Validate all container hashes. Run the two A-side F1 legs while that original executable is still staged, then build and stage the new F1 candidate once and run the two B-side F1 legs plus the approved regression smoke. This avoids needless A/B swapping. Pin source commit, build command/configuration, full candidate executable hash and unchanged container hashes in the evidence. Never re-stage a same-source rebuild as a new candidate. If a shader/cook is unexpectedly necessary, STOP; do not expand scope.

Commit F1 source separately with a Conventional Commit. No runtime leg before its own predictions. One candidate binary for all B-side runs in this brief. If a failed run requires a code correction, stop the campaign and report; do not change code beneath the frozen hash or regrade the prior legs. No merge-level/release-readiness claim from these smokes. All remaining gates stay OPEN.

Process ownership: existing UnrealEditor PID 25324 was running at dispatch. Preserve it and other owner sessions; do not use an editor runner or kill it. Only manipulate processes started for this campaign. The packaged runner contains broad StackOBot process stops: verify no unrelated StackOBot process is present before using it; if one appears, do not kill it. Bench focus acquisition may occur through the approved runner; do not weaken its focus/pose validity gates to make a leg pass. Reuse the existing single watcher PID 14448; no additional watcher.

## Return a compact, fully evidenced report

Journal + CLAUDE status + a short dated operating-contract section 9 entry recording the F1-only phase and all other work held. Do not append another whole-plan amendment. Preserve owner docs. Report:

- prediction/source/evidence commits; exact changed production paths and why;
- prediction hash and A/B executable/container hashes, build result and commands;
- a table for every attempt: bank path, order, effective config, event count, requested/log-derived count, RGB/row indices, missing/duplicate/orphan indices, mask-state/value counts, wait/drop diagnostics and checker verdicts;
- A/B findings separated from causal inference; regression smoke outputs, and any INVALID/FAIL/OPEN case;
- whether the actual production diff stays confined to F1, and what remains untested (including M-F1b and Lyra).

Read results rather than echoing the expected table. End with VERDICT: GREEN only for this bounded F1 phase if all released checks pass; otherwise NEEDS-DECISION/RED with exact blocker and banked evidence. GREEN here is not m51 approval. Stop after reporting for Chat's evidence review; do not queue or start lifecycle work.
