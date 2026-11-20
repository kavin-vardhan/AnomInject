# 079-08 — chat's disposition of Codex's third merge review (2026-09-20)

**All six findings ACCEPTED. `1cbc07e` is NOT merged.**

**Ruling — the verdict concept is withdrawn.** F1/F2/F4 show, with accurate masks and known RGB timing, that frame differencing cannot attribute a change on the target to the anomaly (lighting, occlusion, animation, neighbouring runs all qualify). Three rounds of narrowing the verdict boundary each produced a new valid counterexample; the boundary was the mistake. From 079-09 the tool reports pixel EVIDENCE and one sound negative: per-edge observations (TRANSITION k / NO-TRANSITION / UNASSESSABLE), run outcomes CONSISTENT (does not establish cause) / OFFSET-NOTE (for human review, not a failure) / NO-TRACE (masks on every frame of the span, every pair observed, no change on the target anywhere — the only FAIL) / PARTIAL / UNASSESSABLE / READING (bbox-only). PASS, SHIFT and NOT-VISIBLE no longer exist. F3 (aggregation), F4 (association: one transition serves one edge, labelled-frame bounds only), F5 (explicit mask-id mismatch ⇒ unassessable), F6 (scorer uses run-level eligibility) fixed as specified. Wording and G263 corrected accordingly.

**Process change for this unit (owner's decision, 2026-09-20):** Codex implements the spec (`_reviews/079-09-spec-for-codex-consistency-semantics.md`) on `fix/verifier-field-validity`; Claude Code performs the independent review afterwards with its own fixtures; chat remains orchestrator; the mailbox is not used by Codex. Rationale: the tests have been written from the implementer's mental model three times; the reviewer who found the cases should build against them, and a different reviewer should try to break the result.

— chat Claude, orchestrator
