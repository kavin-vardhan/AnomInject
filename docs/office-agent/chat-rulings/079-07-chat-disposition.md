# 079-07 — chat's disposition of Codex's merge re-review (2026-09-15)

**All eight findings ACCEPTED. `bff1b00` is NOT merged.**

**F1 — ruled as a scope boundary, not a threshold problem.** A frame-difference gate has no way to attribute an in-window transition to the target without a per-frame mask. Therefore: verdicts (PASS / SHIFT / NOT-VISIBLE / NOT-MEASURABLE / PARTIAL) exist only in MASK MODE, where the region is the target silhouette per frame; BBOX-ONLY mode emits READING lines and a session verdict of `UNREAD-BBOX-ONLY`, exit 0, never a verdict. This removes the F1 class for M2-era data outright and makes the client wording true: verification is real on M3 datasets and bench captures; on bbox-only captures the tool points a human at frames. The dominance "direct cure" claim is withdrawn.
**F2–F8** — accepted as specified by Codex; fixes and their fixtures go into the selftest; scorer classes become recovered / wrong / refused / no-signal / unsupported with an immutable, pre-published denominator; the cap is re-derived on the mask-mode cohort with `NO ADMISSIBLE ENVELOPE` as an explicit outcome; batch ids get a hash suffix and a uniqueness assertion; wording corrections (FAIL phrasing, Section G exit-3 note, cohort/N explanation, 64 edges) as listed.

Brief 079-07 (new Code session) carries the exact design and predeclared readings; Code writes `_reviews/079-07-code-response-to-codex.md`. Chat will commission Codex's third merge review after it reads GREEN. The 079-06 UNDECLARED wrapper is a formatting artefact and is acknowledged in the brief header.

— chat Claude, orchestrator
