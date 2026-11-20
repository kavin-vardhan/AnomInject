094-11 — IMPLEMENT Europa Phase A job A5 (again): probe, readers and fixture readiness, with no editor or game launch, under chat's ruling 094-10 (EK1–EK3) and the 094-10 request. Stop at the end of A5. Requested by chat Claude (the orchestrator) on 2026-10-25.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING, in this order
1. _reviews\094-10-ruling.md (EK1–EK3). It is new. EK1 replaces EJ4's "credential-like" list.
2. _reviews\094-10-request.md: its ORDER, STOP RULES and DELIVERABLE, unchanged except as EK1–EK3 amend them.
3. _reviews\094-09-ruling.md (EJ1–EJ3, EJ5 and the "Next" items 1–8), and _reviews\094-08-ruling.md (EI2).
4. The earlier rulings, then your plan and the frozen predictions, as amended.

WHAT CHANGED SINCE YOUR LAST TURN
- Your two stops were correct readings. The second came from my over-broad EJ4 wording.
- EK1: only secret-named variables are secrets: names containing TOKEN, SECRET, PASSWORD, PASSWD, KEY, AUTH, COOKIE or CREDENTIAL.
  - Identifiers (session and thread ids, PIDs, ports) are not secrets. They need no stop and no redaction.
  - Children still receive neither class (allowlist).
- EK2: fix the makefile reader by the engine-source layout:
  - length-prefixed strings, and names that may contain dots;
  - tested on all 8 real makefiles (112–115 variables each) and on a synthetic case with a dotted name and a letter-valued length byte.
  It is a new design with its own two D2 attempts.
- EK2's clean-up order:
  1. redact the secret values in the 3 actions-1.json receipts;
  2. delete the 8 secret-bearing makefiles, by receipt;
  3. after the next build, prove with the reader that the regenerated makefiles hold no CLAUDE_*, CODEX_* or secret-named variable.
- EK3: read-only git queries use GIT_OPTIONAL_LOCKS=0.
- Your 094-10 ledger stays abandoned. Open a fresh one from P9 v2.

ORDER
As in 094-10-request.md, steps 1–10, with step 2 (EJ4) carried out as EK1–EK2. Do the rebuilds of step 8 after the makefile deletion, so the proof in EK2 step 3 rides on them.

STOP RULES
As in 094-10-request.md, except that "any credential value appearing in a new file" now reads "any secret value (EK1) appearing in a new file". The D2 budget for the corrected reader starts fresh. The 9-hour work limit applies; at a clean point, end NEEDS-DECISION "A5 continues". No launches. No push. No tags.

DELIVERABLE
As in 094-10-request.md, plus:
- the corrected reader's test results;
- the EK2 clean-up receipts;
- the post-rebuild makefile proof.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
