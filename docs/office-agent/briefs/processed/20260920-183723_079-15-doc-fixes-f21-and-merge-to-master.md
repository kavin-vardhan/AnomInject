# NAME: 079-14 independent review of the Codex-built verifier
# MODEL: claude-opus-5
# EFFORT: high
# ACK-VERDICT: 079-14 UNDECLARED = the harness did not parse "VERDICT: CHANGES REQUIRED" (not in its vocabulary); chat read the review — no P1, two doc-only changes — and rules them fixed in this brief, then merge.

# 079-15 — apply the 079-14 doc fixes + adopt F21, then MERGE `fix/verifier-field-validity` into master. Resume session e303d99e (58%).

## 1. Fixes (all accepted by chat)
F1 `docs/verifier-characterisation.md`: add a run-outcome column to the wrong-cell table and the sentence "24 of the 314 scored cells read CONSISTENT while carrying a known one-frame label shift — a CONSISTENT run does not confirm the label"; same sentence in README Step 6's CONSISTENT row.
F2 Remove internal identifiers from client-facing text: `docs/client-readme.md` lines ~216 and ~270 (and any other `079-`, `Section G`, `F2`, `_reviews` mention in that file), and the runtime header line printed by `verify_capture.py:~1113` — the boundary-mask explanation stays, phrased for a client ("masks outside the labelled span are extrapolated from the nearest labelled frame and disclosed on each line").
F3 Restore the `⚠` at `verify_capture.py:55` (UTF-8 without BOM; G115/G141 — verify the file's encoding is unchanged elsewhere).
F6 Missing spaces before numerals in reader-facing strings (16+ places Code listed) — fix all; selftest expected strings updated accordingly with each change listed.
F21 Adopt Code's fixture as a permanent selftest case (`event_mixed_fabricated_run`: run [10..15] fabricated + run [24..24] real ⇒ event NO-TRACE, exit 2), plus the M15 mutation as a contract test if the harness supports it.
F4/F5/F7 noted, no change: characterisation doc SHA line reworded to name the repo blob (LF) and state the CRLF working-copy hash beside it; F5 stays a documented rule; F7 handled at m51 resume.

## 2. Gate before merge
Selftest (95) + contracts (22/23) OK; pinned bench ×6 = 64/64 zero-delta, 24 CONSISTENT, `tau=0.0040`; four bank sessions line-identical to 079-14's bank readings except the header line changed by F2; CLI exits 2/0/3. If anything else moves, STOP — NEEDS-DECISION.

## 3. Merge
`git merge --no-ff fix/verifier-field-validity` into `master` (master is at `ac13700`, the branch's base — a no-ff merge keeps the unit's history in one commit); message `feat(verifier): consistency semantics — observations, NO-TRACE, readings; closes 079` with the Conventional-Commits body listing the 079-01..14 briefs and the Codex/Code roles. Push master. NO tag (tags batch at the office). Do NOT touch `m51` (its CLAUDE.md conflict is handled when m51 resumes). Keep the branch (do not delete).
Docs: journal `2026-09-20-079-15-doc-fixes-and-merge.md`; CLAUDE.md status block on master updated (verifier unit CLOSED; semantics one-liner; pointer to characterisation doc); OPERATING-CONTRACT entry (it lives on m51 — write the entry text into the journal and note it is to be appended to the contract at m51 resume).

## 4. Report
Diff summary, gate readings, merge SHA, `master == origin/master`, and `VERDICT: GREEN` on its own last line (or NEEDS-DECISION).
