# 078-17 working review notes — NOT a disposition

SUPERSEDED by the completed `_reviews/078-17-codex-independent-review-and-request-to-chat.md`. Use that final review and its exact final commit/draft/report pins.

Snapshot `_reviews/078-17-draft-first-read.md` SHA2563C0E99E0C1D456AB0F1C109575A169869DED2C029F62A9573728C60BAE3B7C01 read in FULL (all1426lines, count verified). Final report/commit and post-snapshot delta still required. Do not dispatch or claim completion from these notes.

R1: broader seal/carrier/lifetime/retention holds are now honest. Proposed rule1 FindOrAdd/second attach and rule10 live repeat refusal still need an explicit distinction. Rule9 says the TSharedRef/membership-thread arrangement is where code already is, though existing code only takes StateCS to move PendingArms. Existing lock does not establish the missing carrier/publication. No implementation-ready portion inferred.

R2: improvements preserve terminal logical-credit release, source ForceMiss preallocation path, later notifications as diagnostics, shared CPU owner and withdrawn watermark/per-index aggregation. Remaining candidate findings to recheck against FINAL:
- B2 Submitted still means merely readback exists (owner3 now scheduled-pass owner). B4row6 cancellation BEFORE submit says owner3 never began and safe, excluding the very allocated/scheduled-but-not-GPU-enqueued window B1 says needs protection. B4row15 attributes owner3 retirement to BlockUntilGPUIdle; scheduled pointer ownership still needs graph execution/discard lifetime proof. All remain implementation blockers.
- B2 Cancelled(generation) still says completion for prior generation, contrary to corrected B2note/B4row8 diagnostic-only rule. Define first terminal at run cancellation separately from later stale delivery.
- B3 physical identity adds an undefined repeat_view_submits_adjustment. One consumed arm/two submits needs a NEGATIVE correction, or move nonnegative repeat count to consumed side. Define population, sign and units; no evidence exists for a new counter. Allocated/discarded-without-enqueue work needs its own complete physical balance.
- B4row9/D4 N-LC2 claim terminal same-session late completion reachable ONLY after submitted cancellation. Delayed duplicate notification after successful Committed/Resolved is also a terminal notification case; prescribed post-submit cancel device is one route, not proof of exclusivity.
- B1/B3/B4 blanket census no-writer/no-image/no-row wording is SOURCE FALSE for optional bench dump: see078-17-codex-source-checks.md full callback trace. Ordinary reduction versus debug dump must be distinguished, both physical accounting paths included. No implementation authorized by this finding.

R3: broad update/enqueue, collection/join, semantic/lifetime and oracle holds now explicit. C3 struct still labels NoAA AUTHORITATIVE although following paragraph declares convention OPEN. Demote placeholder rather than silently choose client meaning. No known answers or geometry closure supplied.

R6: all schedules explicitly UNSCHEDULABLE; missing floors/clocks/restore/device now listed. Preserve these as honest holds. N-LC2 exclusive-path assertion inherits R2 issue. R4/R5/R7 corrections appear preserved, including separate releaseT and Codex's own wording correction; manifest unchanged pending final hash.

Draft grew rather than becoming a concise prerequisite/accounting contract. Recommend next unit normalize normative rules/tables, remove superseded pseudo-protocol detail or classify it consistently, instead of another layer of addenda. Do not dispatch a second unit in this turn. Final design remains NEEDS-DECISION even if scoped docs delivery completes. **Chat review NOT REQUIRED for source-backed docs correction; next mandatory coherent design before implementation.**
