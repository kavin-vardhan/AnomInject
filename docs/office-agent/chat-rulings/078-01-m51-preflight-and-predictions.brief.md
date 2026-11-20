# SESSION: new-default
# NAME: 078 m51 preflight and predictions
# EFFORT: xhigh
# MODEL: claude-opus-5
# ACK-VERDICT: fix/capture-label-integrity is m51 source material (chat ruling 2026-09-06); not merged as a unit

# 078-01 — m51 preflight, integration design, and predictions for independent review

Issued by Codex as the incoming orchestrator, authorized by the owner's 2026-09-06 handoff and "Have a look here and proceed". Chat Claude is the adversarial reviewer and issues no briefs during this pilot. This is the first bounded step of m51, not permission to run the whole milestone in one turn.

## 0. Scope and stop boundary

This brief authorizes read-only source/evidence inspection, backup push of the existing source branch, creation of the m51 branch, and narrowly scoped documentation commits. NO implementation, cherry-pick, rebase, source edits, tool edits, builds, cooks, captures, test execution, binary staging, worktree changes, master merge, tags, or office actions. Do not start m51 legs, including baseline/control legs, before the predictions have received written adversarial review and a follow-up brief releases execution.

This separation is deliberate: the current handoff requires reviewer approval before the first leg, and the architectural/failure contracts need to be reviewable before Code encodes them. Return at the end with NEEDS-DECISION for the predictions review. An apparently green planning run is not implementation acceptance.

Office data never leaves the office PC. Codenames only: Concorde, Bates, Europa. Inspect local sample fixtures and their evidence only. No temporary files in the mailbox; the runner owns its operational files. Do not write the final report yourself: your final message is the report. The checkout belongs to you only for this brief. No broad adds, stashes, cleanup, resets, force pushes, or changes to the owner's untracked handoffs.

## 1. Cold start and state verification

Read, in order:
1. docs/OPERATING-CONTRACT.md (authoritative, including role boundaries); docs/CHAT-HANDOFF-077-m49-complete-m51-codex-pilot.md.
2. CLAUDE.md current status block; docs/sessions/2026-09-04-077-m49-phase-b.md, particularly sections 3, 4, 7 and 8; docs/predictions/2026-09-04-m49-phase-b.md INCLUDING both amendments; gotchas G236–G258, and any older gotcha specifically referenced by the gates you plan.
3. D:/IntrusiveAnomalies/_reviews/2026-09-06-review-of-fix-capture-label-integrity.md INCLUDING the amended rulings in section 5; D:/IntrusiveAnomalies/_reviews/2026-09-06-external-release-review.md (all sixteen findings).
4. docs/client-readme.md section 8; relevant architecture/runbook sections; the existing gate scripts, read-only. Read actual code at both ac13700 and be16977 rather than assuming historical line references are current.

Give the five-line state summary before action. Expected main state: master == origin/master == ac13700d99fe116056f5aa11611f8931a563a3f2; source branch fix/capture-label-integrity == be16977c0802f57457ec68edcf8d0f96c12c13d9, based on 21c1e0d; Lyra detached at 4a4bfcc. Verify the remote directly, do not rely solely on a remote-tracking ref. If conflicting work or state is found, preserve it and report the discrepancy.

Before changing branches or creating new commits, back up source material with a normal push of refs/heads/fix/capture-label-integrity to the same branch on origin. Verify the remote SHA. If already identical, record it and do not repush. If the remote differs, STOP; no overwrite. This source-backup push is specifically authorized by the handoff; it is the only push in this brief.

Verify staged/archived artifacts by their real paths and full hashes using _binary_baselines/README.md. Expected staged bench exe prefix B8AC65D8, size 241400320; Phase B container prefixes 67EA1FE0/2CEFB8F4/E03C6610 plus A16A18A8/C70ECDAA. Keep the A159EDFD + m46 archive intact. Record CaptureBench revision and Lyra plugin/worktree and binary provenance. Do not rebuild or stage A9B10CC0 just because it is a newer link. No blanket recursive bank scan; inspect the manifests and specifically needed anchors. Snapshot the known untracked-file inventory.

If the verified main checkout is the expected master and m51 does not exist, create m51 from that exact commit. If m51 exists, inspect and report before using it; never reset it. Source branch and master remain intact.

## 2. Orchestrator rulings and design constraints

All amended R1–R8 are binding. This brief does not reopen a client-facing meaning.

Correction to the handoff: section 1's first bullet still states that hide-class observable requires DrawnCount == 0. That sentence is superseded by the same handoff's corrections, G258, Phase B prediction amendments, and journal 077 section 8. DrawnCount / target_drawn_pixels and frames_drawn_unexpected remain diagnostics, never label predicates. Record the correction in journal 078; do not retro-edit the handoff/history.

Integration strategy: m51 is based on post-Phase-B master. Port the useful be16977 changes; do not merge its old base into master or replace shared files wholesale. Preserve RT1, reduce stride/indices, DrawnCount propagation, frames_drawn_unexpected, and BOTH operands of the target-excluded exposure detector. Merely preserving exposure_dip_scope's field name is not acceptance. Keep shaders unchanged unless you identify a necessary change, price its cook/rebaseline cost, and obtain a later explicit scope release.

Design the implementation around these boundaries, testing the design against existing code before endorsing it:
- Immutable frame snapshot identified by session generation and session index, with same-family color/mask pairing and frozen per-fire conditions/identity/geometry.
- Every accepted request reaches exactly one terminal success/failure/cancellation outcome. Rejection of a stale color arm must notify capture, release its PendingSnapshots capacity and corresponding mask/view bookkeeping, and permit future healthy captures. An obsolete completion cannot resurrect it or decrement capacity twice.
- Encoding may run on N workers, default 2, with an ini setting and byte accounting that includes in-service jobs. A bounded wait of at most one configured capture-frame period occurs without holding a lock needed by a worker. Define the default byte budget, maximum single-job handling, and accounting of raw/mask/encoded buffers; price these at 1080p and high-BPP layouts. Readback cap defaults to 32 with an ini setting. Do not present either default as throughput-proven before measurement.
- A frame completion receipt must contain the FINAL measurement disposition, including a downgraded mask failure. A missing mask must not leave a prebuilt positive observation in annotation.json while its label row says unmeasured. Keep valid RGB on mask failure; count masks_dropped. Image/label failure means no successful frame commit; count writer_drops as appropriate. An actual artifact-write failure marks capture_complete false even if RGB survives. Intentionally unavailable measurement (Nanite/mask disabled/resize) is not itself an artifact-write failure. Declare this distinction in predictions.
- Final annotation and summary derive from terminal committed receipts, after all accepted work is joined. Session writers are not reused/reset while active. Concurrent encoding does not require sorted JSONL delivery: the existing client contract joins by session_index. Define safe serialized label append/receipt publication without serializing all encodes or blocking on an index that was cancelled.
- Per-fire shader readiness is sampled at the relevant frame boundary and used by capture AND auditor. A pending texture material cannot suppress a co-active hide. Preserve Phase B diagnostics and document any new attribution keys explicitly.
- R1: zero measured => legacy injected-window fallback; partly measured => observable subset; observability_measured remains Measured > 0; observability_complete is new. Keep anomaly_present's existing meaning. Document the already-approved empty-window -1 sentinel explicitly rather than hiding its value change behind "additive" prose. Do not silently redefine manifested or injected membership: inspect baseline semantics and declare any bug-fix differences per leg.
- R2/R3/R4: retain all-Nanite detection and legacy name-or-class matching, add full-path support and ambiguity logging; WebSocket refusal stays, console mutations remain available and counted with session index. Distinguish external mutations from the auto-injector's own operations; log attempts/outcomes so a no-op cannot masquerade as a successful can-fail lever. The auditor must not fail solely on external_mutations.
- Historical tag records outlive their live leases. Demonstrate how release/reuse and delayed render/readback snapshots stay isolated, without reviving the rejected blanket-quarantine design. Account for renderer proxy updates, host reservations, and census headroom.

If these constraints conflict in the actual implementation, report a concrete failure case and a bounded alternative; do not quietly pick a different contract.

## 3. Required plan and predictions

Create docs/sessions/2026-09-06-078-m51-preflight.md and docs/predictions/2026-09-06-m51-release-hardening.md. They must be a concrete implementation/repro plan, not a copied list of gate names. Mark predictions DRAFT — NOT APPROVED FOR EXECUTION. No new empirical result is claimed in this planning turn.

Include:
1. A finding-by-finding F1–F16 table: current ac13700 behavior with exact source references; what be16977 supplies; what must change under rulings; planned files/commit; regression and can-fail evidence required. Verify all sixteen, not just the previous spot checks.
2. A conflict map and an implementation sequence. Keep R1–R8 identifiable as separate named commits; propose how imported work is organized without temporarily treating reversed semantics as accepted. Include stale global counters, vestigial hold counter, Published overflow, and no-op/GC/preview/pixel-layout work. The alleged held-buffer const-copy is WITHDRAWN; do not invent a fix for it.
3. A truth table for schema fallback, partial measurement, per-fire shader pending, mask-write downgrade, missing RGB/JSONL, and global/sync paths. Include final per-frame fields, event fields, counters and auditor result. Unknown is never zero. The integrity auditor must implement the amended schema, not be16977's superseded strict-positive schema.
4. A lifetime/state diagram or table for arm -> render -> readback -> measurement -> encoding -> commit/cancellation -> finalization. Show lock/thread ownership, generation checks, single terminal accounting, stale cancellation, capacity release, and final GPU/resource retirement. Bound Published growth through explicit outcomes, never silent clearing of live batches.
5. A reproducible gate matrix with command/script references, config, fixture, tick order, input/control, minimum three counted events for certification, expected numbers/relations, independent observation, can-fail signature, known-answer checks of the checker, artifacts and exact stop condition. List tools/levers that do not yet exist and precisely what they must observe. Do not turn an unavailable gate into a pass or silently replace it with a unit test.
6. Full existing regression set from operating contract section 5, including PB-5 in BOTH directions; all F1–F16 coverage; F3 >55 sequential measured events; F4 wrong-world/view rejection; F5 stop/start with pending work; F7/F8 artifact failure and late worker completion; F9 invalid pass vs measured zero; F10 globals in both orders; F11 projection cases; F12 GC preserving original pointer; F13 preview recovery; F14 sync parity; F15 console vs WebSocket; F16 unsupported formats.
7. R8 must distinguish reversed subsystem tick order from multiple game ticks with no render. Inspect tickpin_compiled on each fixture. Do not claim synth order alone tests decoupling. Plan a fixture that causes > readback-cap missed render opportunities, then resumes rendering and proves cancellation/capacity recovery; report unrendered_ticks separately from writer/readback drops. A host where tick pin is not compiled cannot count as the applied-pin arm of this gate.
8. Throughput: 1920x1080 native PNG, mask and census enabled, enough duration to expose steady-state pressure, both orders and both fixtures. Declare frame target, event count, zero natural writer/readback drops, no missing labels, memory/queue and wait-time measurements, timing comparison, and genuine can-fail saturation. Determine the actual duration/acceptance thresholds from existing baseline requirements, not from our 640x360 smokes.
9. P-C7 v3: enumerate added keys and intentional changes PER LEG. Existing anchors require invariance except explicitly declared bug-fix cases. Preserve absolute-counter constant-offset and tag-bijection rules. A projected actor AABB does not generally equal a visible silhouette bbox: the F2 moving-target test needs an independent same-frame projection reference, or a purpose-built unoccluded fixture with justified tolerance, not a universal false equality.
10. Binary provenance: a fixed, hashed candidate executable/DLL+container set per fixture from the same plugin source revision, used for all that fixture's campaign. StackOBot and Lyra cannot literally share a host executable. State this explicitly while preserving the one-binary discipline within each fixture. Freeze A-side bytes; preserve every discard and classify it under existing rules; no after-the-fact binary substitutions.
11. Every failure lever is expected to make the tested checker fail or report the specified unknown/incomplete state. The campaign passes by detecting that response. Deliberately incomplete failure sessions must NOT be required to pass --integrity-only; natural valid schema-3 sessions must pass. List expected auditor exits per leg so these two obligations cannot contradict each other.
12. Cost/scope estimate and remaining technical forks for Codex, separating proposed assumptions from established readings. Do not include office tests or M3 client negotiations in local execution.

## 4. Documentation and review boundary

Commit only the new journal, draft predictions, an accurate compact CLAUDE.md status update (m49 remains complete; m51 is planning, reviewer pending), and the owner-seeded docs/OPERATING-CONTRACT.md, which this brief explicitly authorizes adding to version control. Preserve its body; append a dated section-9 entry for the pilot/preflight and the handoff-predicate correction. Existing CHAT-HANDOFF documents remain untracked and unmodified. Do not rewrite client docs as if implementation has shipped.

Use Conventional Commits on m51. No master/source-branch changes and no push of m51 in this brief. Verify the final diff is documentation-only and the staged/archived bench and Lyra binaries/checkouts are unchanged. Return pointers; Codex will read the report in full and prepare the written reviewer request under _reviews. Implementation and all legs require a later brief after written predictions review. There is no automatic continuation.

## 5. Final message (complete, self-contained)

1. Plain-language rundown, up to eight lines.
2. Preflight table: main/source/remote/CaptureBench/Lyra refs; original source backup; full staged/archived artifact hashes and provenance; untracked-file disposition.
3. F1–F16 source-verification and proposed integration table, plus any refuted premise.
4. Architecture decisions and the gate matrix summary, especially cancellation recovery, write-failure receipts, per-fire shaders, and Phase B exposure preservation.
5. Exact documentation commit, paths, prediction hash, changed files, checkout state; confirmation no implementation/build/leg/cook/staging/merge/tag occurred.
6. NEEDS-DECISION: written adversarial review of predictions is required, followed by any genuine technical forks with evidence and your preferred resolution.
7. End with exactly: VERDICT: NEEDS-DECISION
