# SESSION: new-default
# NAME: 078 m51 complete F1 candidate campaign
# EFFORT: xhigh
# MODEL: claude-opus-5
# ACK-VERDICT: Codex read 078-04's entire report. Its inferred GREEN is rejected: no declared verdict, candidate build and B-side legs unfinished, no journal. The session reached 89% context. This fresh session completes ONLY the already authorized F1 campaign.

# 078-05 — finish the existing F1 work; no new source changes

You are Code. Use Windows PowerShell and read this brief, the complete 078-04 report, the current M-F1 manifest record and the narrow F1 diff. Do not reread the 1,500-line milestone narrative or the entire historical CLAUDE file. One brief in flight; do not touch watcher/config/scripts. Codex owns the queue. Keep normal Opus xhigh. Finish with a literal VERDICT and a journal, not an interim progress message used as final output. Background tasks die when this CLI session exits; **do not end while a required build or leg is running**. Roll safely at 70%: bank outputs and journal first, explicitly report unfinished work rather than inferred success.

Repo `D:/IntrusiveAnomalies/StackOBot/Plugins/AnomalyInjector`, branch m51 at `09c636f75d8234e1183d324b416ae02389bfef53`. F1 source `6713f51`, original predictions `53d42c2`, appended native-control predictions `09c636f`. Master ac13700 and source backup be16977 unchanged. Ten owner handoffs preserved, no tracked WIP. Read `D:/IntrusiveAnomalies/_mailbox/to_chat/078-04-f1-implementation-and-gate.report.md` in full; its actual content, not GREEN header, governs.

## Accepted decisions; scope held

Codex accepts preserving the current effective-mask predicate: the mask extension has independent arming, so do NOT add a bSveCapture restriction. This corrects an overstrong premise in my 078-04 brief. Backbuffer runtime remains OPEN, not proved working by source alone.

The added TargetMask=1 / Mask=0 native control is useful evidence of the same F1 defect and may be completed under this follow-up. It does NOT retrospectively certify the resized-output attempts that failed B1. Keep the OutputHeight=360 runs as readings with their actual runner verdicts, plus the native control with B1 active. No B1 tolerance or calibration change. Do not claim all nonzero OutputHeight values must fail B1: at/above native height the writer can keep native output size. That broader statement in 09c636f is incorrect; the measured mismatch applies to the actual 360-height recipe.

No schema changes: keep existing meanings and serialized keys. In particular do not implement Chat's `injected_frames intersect committed` proposal. PNG writer-bound work remains held; see `D:/IntrusiveAnomalies/_reviews/078-04-codex-disposition-of-chat-review.md` only if needed. No lifecycle/geometry/lease/writer/control work, no shader/cook/fixture/engine edits, no Lyra work, no push/merge/tag. Production source is FROZEN at 6713f51. A candidate failure needing code changes ends this campaign for review.

## Recover the interrupted build before anything else

078-04 launched:
`D:/UESource/UnrealEngine/Engine/Build/BatchFiles/Build.bat StackOBot Win64 Development -project=D:/IntrusiveAnomalies/StackOBot/StackOBot.uproject -waitmutex`

The background output `C:/ClaudeTemp/claude/D--IntrusiveAnomalies-StackOBot-Plugins-AnomalyInjector/<uuid>/tasks/bcx4tjd16.output` now reads **[killed]**. At 17:08 the project executable had changed size during linking; there was no successful build result. **Treat it as interrupted/possibly partial, never as a valid candidate.** Preserve the evidence in `D:/IntrusiveAnomalies/_bench_sessions_bank/M51_F1_EVIDENCE` before the old temp output vanishes.

Inspect process/output state; do not overlap builds. Recover the same source build using the proven code-only route, ensuring a completed link and successful UBT exit, with FULL build output persisted directly to the evidence directory. If an interrupted target could be mistaken for up-to-date, preserve/quarantine that exact output (verified absolute path confined to project Binaries) and force its necessary build action. No recursive cleans or engine rebuild. This is recovery of an UNSTAGED interrupted candidate, not restaging a same-source second binary. Do not fabricate success from file existence or timestamp. The required packaged game target is sufficient; an unrelated editor-target build is not needed for this packaged campaign.

Staged runtime A-side is still expected B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8. Confirm staged/archive identity and frozen Phase B container hashes. After successful build, archive/hash the F1 candidate and stage it ONCE with that unchanged container. Pin source commit, command/config, full executable hash and container hashes in the evidence. Do not execute any prior partial output.

## Finish only the B-side runs and approved smoke

A-side is already banked; do not repeat it. Unique attempts are eight: three resized natural + three resized synth + native mask-off natural + native mask-off synth. The extra native `*_try1` and accepted bank copies represent the same attempts, NOT independent runs. They consistently have rows/images 0..32, 33 written, 57 logged drops, 3 events. The 18–30 predicted band was missed; preserve that historical prediction and its recorded miss.

Run the four B-side M-F1 configurations already named in the manifest: OutputHeight360 natural/synth and native TargetMask1/Mask0 natural/synth. Preserve the resized harness failures as scoped readings; do not call their B1 passes. Native controls must satisfy B1, >=3 events in BOTH orders, rows/images 0..89 exactly, no missing/duplicate/orphan indices, no NEVER RESOLVED/finalize-drop lines, correct unmeasured representation, unchanged schema/key shape. Independently derive expected request count from run logs/config; counters alone are insufficient. Read `visible_positive` with its EXISTING bbox-based meaning: F1 does not implement the later observable-only schema correction.

Complete B-side ONSET/PAIRING/G7 smoke on effective masks/native output, both orders. The inherited smoke recipe omits enabling the pairing probe and abbreviates checker commands: resolve that BEFORE those legs. Use actual existing knob/CLI and full paths, with live probe evidence. Include 100% and 50% pairing in BOTH orders as released by 078-04; the manifest's natural-only 50% arm is incomplete. Amend only the smoke subrecord, commit predictions before any new smoke leg, do not alter past predictions. No new harness/gate-tool implementation. Existing m44 tools: `D:/IntrusiveAnomalies/StackOBot/Plugins/CaptureBench/tools/m44_gates.py` and `m44_pairing_probe.py`; inspect their actual setup requirements and verdict/exit behavior. A known marginal historical pairing result is not automatically a PASS or proven unrelated to this binary; report it separately and NEEDS-DECISION if the declared checker fails. No threshold changes or unapproved retries until a result happens to pass.

Preserve owner UnrealEditor PID25324 and any other owner process. Use only the packaged runner after checking its broad StackOBot stop cannot hit unrelated processes. No new watcher. No office material/data/actions.

## Evidence and completion

Existing evidence reader `D:/IntrusiveAnomalies/_bench_sessions_bank/M51_F1_EVIDENCE/read_f1_legs.ps1` may be reused/corrected as offline evidence tooling, not shipping code. Inspect it first; validate distinct physical attempts vs accepted copies, row/image sets, anomaly fields, schema/metadata, pixel dimensions, request/drop diagnostics and checker outputs. Save the readings and logs, not just prose. Do not equate exit 0 with gate PASS.

Write the missing 078-04/05 campaign journal, compact CLAUDE status and dated operating-contract section9 entry. Only documentation/evidence edits authorized. Report source/prediction/evidence commits, changed source paths, successful build log and exact source/binary/container hashes, per-attempt results, new/old artifact deltas, and all remaining OPEN/unrun/refused gates. Correct the report's duplicate-counting “10 runs” claim. Include the workflow incident: 078-04 ended early, its background build was killed and its GREEN was merely inferred; no mailbox script change here.

End with **VERDICT: GREEN** only if all requirements of this bounded F1 phase actually completed and its released checkers passed. Otherwise **VERDICT: NEEDS-DECISION** or RED with exact failed check and banked evidence. No source fix or extra campaign after a candidate failure. Stop after the final report for Chat's evidence review.
