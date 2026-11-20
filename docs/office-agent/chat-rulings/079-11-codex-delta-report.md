# 079-11 — implementation clarification required

**NEEDS-DECISION before implementation.** The supplied R6 rules do not determine
an implementation that can satisfy their new two-run fixture. I have read the
specification and current source, verified the starting state, and requested two
clarifications. No verifier, scorer, fixture or repository documentation has
been changed. No new measurements, calibration sweep or implementation commit
has been made. The previous round's results remain historical evidence.

## 1. The conflict is in the specified search domain and unchanged outer edges

Authority: [079-11 specification](D:/IntrusiveAnomalies/_reviews/079-11-codex-delta-gap-ordering.md).
R6.1 collects transitions in `G union both facing windows`. R6.2 assigns the
earliest and latest. R6.5 says **“Non-facing edges (A.onset, B.end) unchanged.”**

For the requested fixture A=[4..5], B=[8..9], window=4, and a session spanning
frames 0..99, the existing inclusive labelled bounds produce:

| Edge | Labelled transition | Search window |
|---|---:|---|
| A.onset | 4 | [1..6] |
| A.end | 6 | [4..8] |
| B.onset | 8 | [6..10] |
| B.end | 10 | [8..14] |

These bounds follow directly from
[the existing window construction](D:/IntrusiveAnomalies/_reviews/079-10-frozen/verify_capture.py:910).
A normal two-run blink has RGB transitions at 4, 6, 8 and 10. Thus:

- The facing-window union is [4..10], containing **all four transitions**.
  Earliest/latest over that union gives **4 and 10**, not the requested 6 and 8.
- A.onset's unchanged window contains 4 and 6. B.end's unchanged window contains
  8 and 10. With equal-amplitude blink transitions, both fail the existing
  dominance test. Changing only the facing edges cannot make both runs CONSISTENT.
- This is also visible in the retained pinned readings: the first natural blink's
  outer onset competes for frame 6, and its outer end is ambiguous. Those are
  existing measurements, not new results for 079-11.

The issue therefore remains even if the new facing-edge assignment works exactly
as intended. It is a conflict between R6.1/R6.5 and the requested fixture answer,
not a reason to adjust a threshold after seeing a new bank result.

## 2. “Dominant transitions” needs a definition for more than one candidate

[The current detector](D:/IntrusiveAnomalies/_reviews/079-10-frozen/verify_capture.py:888)
selects one largest above-tau change and requires its ratio to the second largest
to be at least 1.5. Two equal, separated blink transitions in the same window do
not both qualify; the window is ambiguous.

The clarification choices sent to the owner are:

1. Preserve the literal union and outer-edge rules and report prediction misses,
   or revise gap selection and recheck the outer edges after assignment.
2. Count separate locally dominant peaks, or collect only TRANSITION observations
   produced by the existing single-winner detector.

Proposed local-peak definition for review: each selected frame exceeds tau and
has at least the existing 1.5 ratio against its adjacent-frame change values.
This is a proposed new definition, not a rule already present in the source.

## 3. Concrete proposed correction, if the intended fixture answers govern

This proposal requires a ruling because it changes R6.1 and R6.5:

1. For a facing pair, restrict the union to transition indices strictly between
   the non-facing labelled edges: `A.start < k < B.end + 1`. In the supplied
   example this retains 6 and 8 and excludes 4 and 10. The facing windows still
   admit the actual onset 8 when B is labelled late at 9.
2. After assigning the two facing transitions, remove those assigned indices
   from the candidate sets of the two outer-edge searches and remeasure those
   searches. Their labelled windows, baselines, tau, cap and coverage guards
   stay fixed. This is an explicit exception to “non-facing edges unchanged.”
3. Define multiple candidate peaks before implementation; retain missing-mask,
   RGB, identity, baseline and cap refusals. Preserve single-run duplicate
   refusal when no inter-run assignment applies.

Under this proposal the original `15 -> 16` ordering fixture would need a new
predeclared answer: its two facing transitions are 12 and 15; B.onset has delta
-1. Removing those assignments from the outer searches leaves 10 and 17. That
predicts OFFSET-NOTE instead of its 079-10 PARTIAL result. This is a source-based
prediction, not a new measured result or an approved change.

Two additional wording issues should be resolved in the ruling:

- The actual duplicate guard groups by `(target, mode)`; different targets do
  not currently compete for ownership of the same frame. R6.5's statement that
  refusal “still applies” across different targets does not describe the source.
  A new cross-target refusal would be a separate behavior change.
- “Other four legs byte-identical” conflicts with mandatory new gap-disclosure
  lines when ordering applies between their successive same-target runs, and
  with header changes if the rederived cap moves. Clarify whether this means
  identical observations/numbers with newly required disclosures allowed.

No part of this proposal has been implemented or silently substituted for R6.

## 4. Verified state and remaining work

- Feature base/local-and-origin tracking ref:
  `e6351ce396c53ede5a7acd8186497811776e9e34`.
- Initial and restored checkout: m51
  `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`.
- Master/origin-master: `ac13700d99fe116056f5aa11611f8931a563a3f2`.
- CaptureBench HEAD: `472a409dc70ae78d01a38d08fc3bddb15fe41c58`.
- Tracked trees clean; pre-existing untracked files preserved. The 079-10 frozen
  source hashes were verified. The temporary switch to the feature branch was
  read-only. No commit, push, mailbox, Code brief, build, capture, merge or tag.

Evidence: [079-11 preflight](D:/IntrusiveAnomalies/_reviews/079-11-evidence/preflight.json).
After clarification: append and commit AMENDMENT 2 before implementation/results;
implement the ruled algorithm; run the full fixture suite, ten bank sessions,
scorer sweep and exit contracts; commit/push only the feature branch; restore m51;
replace this pending report with the completed readings and preserve this note.

**Chat review REQUIRED for the R6 semantic clarification before implementation.**
The reason is the concrete conflict in the supplied specification, not a general
permission requirement or an automatic approval rejection. The already authorized
implementation and feature push do not need separate approval once the rule is
resolved. The next mandatory checkpoint after implementation remains completed
evidence and Claude Code's independent review before merge. Codex remains the
implementer for this unit; Chat orchestrates. All release/campaign holds remain.

First task clock: **2026-09-20 10:05:11 UTC**. Final boundary: **2026-09-20 10:14:26 UTC**.
Elapsed wall time: **9m 15s**, including source inspection and the clarification report.
[Boundary evidence](D:/IntrusiveAnomalies/_reviews/079-11-evidence/final/boundary.json).
No monetary/token charge is inferred. The same coherent workstream can continue;
no fresh Codex session or context-setting change is recommended.

VERDICT: NEEDS-DECISION
