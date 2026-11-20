# REPORT for 079-14-independent-review-of-codex-verifier.md
verdict: UNDECLARED (no VERDICT line (2026-09-06 rule: fail closed - holds the next brief like NEEDS-DECISION)) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 079-14-independent-review-of-codex-verifier.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-20 17:21:28 | finished: 2026-09-20 18:03:36 | duration: 42 min | exit code: 0
context at last turn: 577771 tokens (58% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\079-14-independent-review-of-codex-verifier.live.txt

---- FINAL MESSAGE (Claude Code) ----
The review is complete and written to `_reviews/079-14-code-independent-review.md`. Repo untouched, scratch cleared, evidence preserved.

```
079-14 — INDEPENDENT REVIEW OF fix/verifier-field-validity (Claude Code as reviewer)

CANDIDATE HEAD  c1945b64060ef5c10ed5a379443340d7574db8d6  "fix(verifier): separate motion
caveats and revise peak selection".  local == origin.  master ac13700.  m51 53bf725 —
live checkout never switched, ZERO tracked modifications on exit, temporary worktree
removed, scratch cleared.  No commit / push / merge / tag / build / cook / capture.

REPORT: _reviews/079-14-code-independent-review.md (545 lines)
ARTIFACTS: _reviews/079-14-fixtures{,2,3}.py · -predeclared-round{1,2,3} ·
-mutation-probe.py · -probe-raw-d.py · 079-14-evidence/ (55 files)

VERDICT: CHANGES REQUIRED — two DOC-ONLY defects.  NO P1.  B1–B6 ALL HELD.

I COULD NOT PRODUCE A FALSE NO-TRACE.  35 fixtures, every expectation predeclared BEFORE
running, written from the §1 semantics, not from Codex's generators.  33/35 matched
exactly; the two misses were MINE (F13 — I hadn't applied R9's prominence rule to a
3-frame plateau; F15 — I wrote the fixture wrong).  Both confirm the tool.
The B1 probe: F01 places a full-target change (d=1.0000) deliberately outside both edge
windows so both edges legitimately read NO-TRANSITION.  Result:
  "UNASSESSABLE - whole-span change at frame 18 d=1.0000 > tau=0.0040"  exit 0.
NO-TRACE fired only where the pixels genuinely contained nothing (F05/F21/F22/F26/F30/F34).
§4.3 answered structurally: verify_capture.py:983 is the ONLY return R_NO_TRACE, reached
only after four guards; run["outcome"] assigned at one site; exit 2 gated solely on it.

REPRODUCED EXACTLY, NO MISMATCH: selftest 94 · contracts 22 · pinned bench 64/64 edges all
delta=+0, tau=0.0040, 24 CONSISTENT, exit 0 · the four bank sessions match Codex's
bank-validation.txt LINE FOR LINE · batch/CLI exits 2/0/3 · sweep 572/314/258,
218 recovered / 55 wrong / 2 no-transition / 39 unassessable.

I RE-RAN THE SWEEP RATHER THAN READING IT.  Using the branch-tip file (615b6c8f…), my
ledger matches Codex's CELL FOR CELL — "cells whose classification differs: 0" — and the
regenerated verifier-characterisation.md is IDENTICAL to the shipped one except ONE line
(the SHA).  The published tables are confirmed to come from this code.

EXTENSIONS BEYOND THE BRIEF
· Bank sweep, 143 mask-mode sessions, one per family: ZERO NO-TRACE, zero FAIL.
· 16-point mutation probe: the load-bearing guards ARE pinned — deleting the whole-span
  change check turns the selftest RED (as do the unresolved-id, span-error, class-allowlist
  and PARTIAL-promotion mutations); peak-ratio and 0.42-marker mutations are caught by the
  CONTRACTS, not the selftest, so both suites are needed.

FINDINGS
F1 CHANGES REQUIRED — the branch's most decision-relevant number is published only outside
   the repo.  24 of 314 scored cells read CONSISTENT while carrying a KNOWN one-frame label
   shift.  docs/verifier-characterisation.md is where the header (verify_capture.py:1116-7)
   sends readers, lists all 55 wrong cells, and has NO run-outcome column (:20).  The
   figure exists only at _reviews/079-13-codex-delta-report.md:74.  Fix: one column + one
   sentence in the doc and in README Step 6's CONSISTENT row.
F2 CHANGES REQUIRED — internal identifiers ship to the client.  client-readme.md:216 carries
   "This is not 079-08 F2's boundary-mask reuse…"; :270 carries "Section G remains held
   pending evidence and independent review."  That file is mapped to the bundle's README.md.
   The 079-08 F2 sentence is ALSO PRINTED AT RUNTIME on every client run
   (verify_capture.py:1113) — it is the second header line of all 45 readings I took.
F3 NOTE — encoding regression at verify_capture.py:55: master's ⚠ is now "Ã¢Å¡Â " (G115/G141).
F4 NOTE — the published verifier SHA256 (verifier-characterisation.md:170, badd0e3e…) names
   _reviews/079-13-sweep-tools/verify_capture.py, not anything in the repo (blob b9b4eb95…
   LF / 615b6c8f… CRLF).  Content IS identical (2252 lines, zero diff lines) and my re-run
   proves the tables are from this code — but 079-13 report L372's "exactly match byte-for-
   byte" is FALSE as written (32 bytes, mixed line endings).  G121's shape.
F5 NOTE — a NO-TRANSITION window can contain an above-tau pair and the edge line doesn't say
   so.  F13 measured through the gate's own _PairSignal: pair 11 d=1.000000 (run A's true
   end) is suppressed as a plateau interior.  Disclosed at rule level in the README, not at
   the point of reading.  Per §3 NOT blocking.
F6 NOTE — systematic missing space before numerals in reader-facing strings (exit2, code2,
   return3, floor0.004, default28, All94, and22 …), 16+ places.
F7 NOTE — merge-base with m51 is ac13700; merge-tree reports a CLAUDE.md CONFLICT.  Chat's
   sequencing call, not a branch change.

ONE COVERAGE GAP WORTH CLOSING (§4, M15).  _event_token's NO-TRACE-first precedence — the
rule that a NO-TRACE run can never be hidden by a healthy sibling run in the same event —
passes ALL 94 fixtures and ALL 22 contracts when deliberately broken.  Measured on my F21
(one event: run [10..15] fabricated, run [24..24] real):
   control  exit=2 tokens=[NO-TRACE]     M15  exit=0 tokens=[CONSISTENT]
A fabricated label passes silently.  F21 is ready to adopt as a permanent selftest case.
M7/M11 (whole-span tau=min, and the edge-window half of the NO-TRACE range) also survive
both suites; I could NOT build discriminators and say so — those are suite coverage gaps,
NOT demonstrated defects.  The code is correct as written.

SCOPE (B5): diff touches only CLAUDE.md, docs/* and the two tool files.  Source/, Shaders/,
measure_label_offset.py, Content/, WebClient/ UNCHANGED.  gotchas.md 441 added 0 removed.
Predictions file append-only on all four amendments (89/113/102/95 added, 0 removed) and
each predeclaration commit PRECEDES its implementation commit by author date
(11:16→12:01, 13:24→13:56, 15:55→16:23, 16:49→17:15).  CaptureBench 472a409 tracked-clean;
scorer SHA matches the published one.

COST: ~1 h 10 min wall, Opus 5 xhigh, single session, no sub-agents.  ~81 gate invocations.
Scorer re-run ~21 min; mutation probe ~11 min; bank sweep ~31 min (stopped deliberately at
143 — reported, not rounded up).  Currency cost unavailable to me.

Nothing here releases Section G, the client hold, the m51 campaign or the release hold, and
no output of this tool certifies a label.

VERDICT: CHANGES REQUIRED
```
api-equivalent cost (not billed on Max): $43.43 | turns: 150 | running total across all briefs: $1,654.82
