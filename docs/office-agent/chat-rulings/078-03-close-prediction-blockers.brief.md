# SESSION: new-default
# NAME: 078 m51 bounded prediction corrections
# EFFORT: xhigh
# MODEL: claude-opus-5
# ACK-VERDICT: Codex read 078-02 in full and rejected its soft writer budget, delayed stale expiry, and incomplete gate/provenance deliverables. This releases only the bounded documentation corrections below; no implementation or leg.

# 078-03 — close specific prediction blockers, fresh session

You are Code; Codex orchestrates, chat Claude reviews. The previous Code session reached 74% context, so this is a fresh session under the operating contract. **Do not restart the source audit or restate the milestone.** Read the contract, the latest CLAUDE status only, this brief, and the active AMENDMENT 1 sections named below. Read the actual current source/engine/script interfaces only where needed to settle a concrete item.

Expected state: plugin repo `D:/IntrusiveAnomalies/StackOBot/Plugins/AnomalyInjector`, branch `m51` at `adb0d4d12f3b751bece396499bedd50d00fdeb76`; master `ac13700d99fe116056f5aa11611f8931a563a3f2`; source backup `be16977c0802f57457ec68edcf8d0f96c12c13d9`. Prediction file `docs/predictions/2026-09-06-m51-release-hardening.md` SHA256 `725CD30FF5A6F55BB6B96990BF194319DA4D665471F8DBBC8921ED119D55B50B`. Ten owner handoffs untracked, no tracked WIP. Verify; preserve any discrepancy.

**Scope:** read-only inspection/hashing plus documentation-only edits/commit on m51. NO source/tool/fixture/engine edit, implementation, build, cook, test/self-test, capture, staging, branch/worktree change, push, merge, tag, office data/action, mailbox maintenance. Use PowerShell. The runner writes your report. Codenames only; no office material leaves its PC. One brief in flight.

## The following are corrections to an unapproved draft, not a request for permission

### C1 — expire by render identity, not by readback capacity

AMENDMENT 1 A1.6 introduces `StaleArmMaxTicks=32`. **Rejected.** Thirty-two is a pending-readback capacity, not an allowance to render an older snapshot. Remove that new knob. An unpublished arm from an earlier game frame is already stale for a later scene. Sweep those arms on the GT before the next admission/cap check; keep the publish-time game-frame equality check as well. Published GPU work is not expired by this sweep. Record the exact frame identity, same-session terminal guard and exactly-once capacity retirement.

A1.8.1 also predicts NoStaleArmSweep alone prevents recovery. With the publish-time rejection/retirement kept, that conclusion need not follow: it can retire the backlog when rendering resumes. Correct the falsifier to bypass **all stale-request capacity-retirement routes during the test interval**, while retaining the identity guard (never deliberately label a later render with an old snapshot). Name its exact scope and expected pending/cancel/drop/commit signatures. Restore normal cleanup at test teardown so an intended failure does not hang FinishRun.

WithholdWorldRender stays the real no-render device, with independently observed capture opportunities, restored prior flag and >=3 counted events after resumption. ForceMiss stays separate; count key misses for accepted session capture requests, not unrelated non-wanted keys/preview families. The archived B8AC65D8 has neither new levers nor the cap; do not claim it runs them.

### C2 — a bounded writer must actually reject excess admission

A1.7's soft reservation policy is **rejected**. Measured maximum compression ratio is not a bound on future content. Allowing over-budget admission/completion reverses R6. A release default also cannot depend on a calibration capture that has not been approved or run.

Specify a deterministic admission policy with defaults **N=2, byte cap=256 MiB**, including in-service jobs. Use a conservative source/format/dimension-based reservation for the buffers the plugin owns (raw, masks, conversion/resampling, encoded output); verify the needed encoder/output-bound assumptions against the actual local implementation. Account codec scratch separately if the API cannot expose it, with an explicit bounded worker count/input-size contract and an honest statement of what the byte cap does and does not bound. Do not claim a hard cap on all process/GPU/allocator memory.

There is **no oversized admission exception**. Invalid configuration can be refused at StartRun; an accepted capture request whose job cannot fit is a counted failure/incomplete session, never silently admitted over budget. An encoder/output limit breach is a controlled failure, never truncation or overrun-and-success. Give valid deterministic limits/defaults, validation and deadline rules; do not invent an unknown EncoderReserveFactor to be priced later. If the current codec API prevents a defensible bound, identify the exact source reason and a bounded implementation alternative as an OPEN implementation prerequisite, rather than declaring the design complete.

The saturation device should be one reproducible runtime worker-delay/pressure control compatible with valid settings, with a numeric load/delay/duration and expected drops. A StartRun configuration refusal is not a successful in-capture backpressure test. The 600 requested frames / first 60 timing-only warmup / 30 fps / <=1.05 speed-ratio natural test remains; never lower the bar after seeing a result.

### C3 — repair completeness and fallback contradictions

A1.5 requires a mask "iff armed", but C/E can be armed and intentionally unmeasured, and sparse empty masks can legitimately have no PNG. Define `mask_write_required` from the actual effective artifact policy **before attempting the write**, distinct from requested/armed status. A failed required write cannot clear that obligation by downgrading the row. Unknown/empty cases with no promised PNG are not artifact failures.

Separate prepublication readiness from final publication success. Compute candidate completeness from reconciled frame/artifact receipts; stage and publish metadata with summary last; notify success only after all required metadata commits succeed. Do not say the reconciliation has observed the summary's write before serializing that summary. An absent/failed summary is a failed session to the auditor.

Preserve the TWO ledgers: requested injection history and committed observations. R1's ZERO-measured fallback means `affected_frames = injected_frames` **also on an incomplete session**, so A1.5's blanket claim that affected_frames always excludes dropped requests is false. Explicitly distinguish observable membership from fallback membership. Missing-image membership in injection history/fallback is recorded unknown/incomplete, not a successful visible annotation. The auditor still fails the incomplete session; it must not manufacture an R1 contradiction by demanding the observable-only rule on a fallback. Do not erase the requested ledger or silently flip the agreed schema meaning to solve this.

Define the actual membership sampler (frozen per-fire labelled/active condition) from existing code, rather than replacing it with the vague claim "an injection exists"; preserve the agreed meanings. Decline a second simultaneous capture context explicitly if the shared key-ring/tag state cannot safely isolate it; do not merely notice interference after pixels were captured.

### C4 — supply the missing executable gate/provenance manifest

A1.9 still gives script **names** rather than executable paths/arguments, and A1.11 gives broad leg classes rather than exact per-leg allowlists. A1.13 explicitly defers **existing** Lyra full hashes, despite the report saying full hashes were supplied. These are uncompleted deliverables, not reviewer-approval questions.

Create one documentation-only manifest (Markdown table or structured JSON under `docs/predictions/`) with one row/object per runnable recipe and each fixture/order expansion. For every required F1-F16/R1-R8/regression/throughput/known-answer case give:
- unique recipe ID; fixture and target type; exact existing absolute script path plus supported command arguments (inspect CLI/argparse); full capture config and expected event validity;
- for a NEW script/lever, explicitly `planned, not implemented`, a concrete proposed CLI contract and the later implementation dependency; do not pretend it exists;
- expected artifacts/counters/auditor exit and falsifier, plus stop condition;
- exact artifact/field additions or value/membership differences for P-C7 subjects, or an explicit reason the recipe is not a P-C7 comparison. Row `visible_positive`, bbox fields and per-fire render-state/identity additions must not be omitted where the agreed fixes change their values. Avoid global "allow all changes" lists.

No recipe may send new ReadbackDelayTicks, destroy-target devices, etc. to an archived A executable. Use in-candidate bypass pairs or an explicitly available external controller; otherwise mark that comparison OPEN. An ordinary input variation is not automatically a falsifier of the fix: `NoTagRetire` does not manufacture cross-attribution; making both fires pending does not test the frame-wide-shader-attribution bug; synth order alone is not a bypass of the fixed global sampler. State the precise defect each falsifier introduces and which checker it must trip, or mark the missing device honestly.

Record existing **full absolute paths and all 64 hash characters** for the archived current and historical StackOBot sets and Lyra plugin/host/project manifests and relevant loaded modules. Resolve manifest entries to actual DLL names (including any suffix); include the relevant engine/project provenance. Hashes of future candidate binaries still belong to the later freeze step. Missing editor A-side archives remain a named prerequisite; use the approved in-candidate M-F4 route, not a fabricated baseline. Do not copy unqualified numeric claims such as vetoed_events 8->0 across unnamed legs.

## Deliver only the corrections and manifest

Append **AMENDMENT 2**, explicitly superseding the affected A1 clauses, without copying the entire 1,000-line plan again. Reference the new manifest as the single execution inventory. Keep this amendment focused (approximately <=250 lines, excluding the gate manifest); no pilot praise, long retelling, or claim that every requirement is met unless the manifest demonstrates it. Add a short 078-03 journal and compact CLAUDE status, plus dated contract §9 corrections if needed. Preserve history and owner files. No code change.

Self-check these concrete acceptance cases in the DOCUMENT, not by running tests: one prior-game-frame unpublished arm expires before new admission; published delayed readback survives; oversize job cannot be admitted over cap; zero measured + failed RGB keeps fallback semantics but fails integrity; armed invalid mask has no invented PNG requirement; NoStaleSweep alone is not claimed to prevent every other cancellation route; every existing hash is full length; every runnable recipe has an actual/proposed executable command and a classified falsifier.

Commit only the named docs with Conventional Commit. Verify source/shader/tool diff vs ac13700 empty, refs unchanged except m51 docs, no binary/worktree mutations. Final report: exact commit/hash, paths, C1-C4 dispositions with actual remaining gaps, no-implementation confirmation; end `VERDICT: NEEDS-DECISION` for Chat's written review. Codex will read the whole report. If a concrete architectural fork cannot be settled from source, return it with the evidence instead of inventing agreement.
