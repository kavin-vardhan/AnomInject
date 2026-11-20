# 094-10 — Chat ruling: A5 didn't start; identifiers are not secrets (my EJ4 wording was too broad); fix the makefile reader; re-run A5

**Date:** 2026-10-25, about 12:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-10-codex-report.md` (thread `01a14334`, 08:43–09:00 IST, 1,008 s, exit 0). Its verdict: `RED | A5 incomplete: D2 environment-reader limit and EJ4 session-identifier receipt STOP; new identifiers redacted, ledger invalidated, P9 v2 and qualified binaries unchanged`.
- **Claude Code's review:** `_reviews\094-10-code-review.md`. 17 OK · 10 GAP (the unreached A5 items) · 7 OPEN · 1 STOP.
- **Mailbox report 094-10:** RED, 79 min.

## Verdict
**Nothing was harmed.** Code checked independently:
- P9 v2: 18,264 of 18,264;
- all 110 qualified binaries are equal;
- 0 engine writes and 0 launches;
- the protected state passes;
- refs are unchanged.

**No A5 work happened,** and no commit was made. `feat/europa-port` is still `d30cbe0`.

**Two stops:**
1. **The makefile-environment reader failed twice (D2).** Code found two concrete parser bugs:
   - it rejects a real variable named with a dot (`ASL.LOG`);
   - its greedy search swallows a length byte that happens to be a letter.
   
   All 8 real makefiles decode cleanly at the layout in the engine source.
2. **The closing ledger recorded session and thread identifiers in tooling paths.** Under my EJ4 wording ("credential-like", which included `CLAUDE_*`/`CODEX_*`), Codex treated them as credential values. It redacted both ledger databases and abandoned that ledger. **The wording was mine and too broad.** The stop was a correct reading of a bad rule.

## Rulings

### EK1. Secrets and identifiers (replaces EJ4's "credential-like" list)
**Secrets** are the values of variables whose names contain `TOKEN`, `SECRET`, `PASSWORD`, `PASSWD`, `KEY` (any API key), `AUTH` (an authorization header or token), `COOKIE` or `CREDENTIAL`. For example, `CLAUDE_CODE_MESSAGING_TOKEN`.
- A secret value must never be written to a new file.
- If one is, that is a STOP.
- Historical copies are redacted to `<redacted>`, with a hash log.
- A secret value is never printed.

**Identifiers are not secrets:** session ids, thread ids, PIDs, ports, entrypoint names and similar, such as `CLAUDE_CODE_SESSION_ID`, `CODEX_THREAD_ID` and `CLAUDE_CODE_SSE_PORT`.
- They may appear in receipts and paths.
- They trigger no stop and no redaction.

**Child processes still get neither class.** The EJ4 allowlist stays: build and launch children receive only the variables they need. No `CLAUDE_*`, `CODEX_*`, `ANTHROPIC_*` or `OPENAI_*` name, and no secret-named variable.

**What 094-10 already did stays as it is:**
- the redacted and abandoned 094-10 ledger;
- Code's self-redaction of its session id.

The next job opens a fresh ledger from P9 v2.

### EK2. Reading the build environment: fix the reader
**The design:**
- Parse the UBT makefile's environment block by the layout in the engine source: length-prefixed strings, any legal variable name including dots, and no greedy pattern search.
- Use it to prove the environment UBT will actually restore for compile children.

**The test:** all 8 existing makefiles must decode, giving 112–115 variables each, in agreement with Code's decode. A synthetic makefile with a dotted name and a letter-valued length byte must also decode.

**The D2 budget:** this corrected reader is a new, chat-specified design, so it gets its own two attempts. The 094-10 attempts don't count against it.

**The clean-up order (EJ4, as amended by EK1):**
1. Redact the **secret** values in the 3 `actions-1.json` receipts.
2. **Delete the 8 makefiles that hold a secret,** by receipt, so the next build regenerates them under the allowlisted environment.
3. After that next build, prove with the reader that the new makefiles hold no `CLAUDE_*`/`CODEX_*` or secret-named variable.

### EK3. Small items
- **Read-only git queries use `GIT_OPTIONAL_LOCKS=0`.** ReGate51's index was rewritten at 09:00:01 by a status refresh. That rewrite was content-equivalent and is accepted.
- **Code's count correction** (14 names per makefile, not 13) is noted.

## Next
- **094-11 is queued: A5 again,** with the same eight items as 094-09's "Next" list and the 094-10 request, under EK1–EK3 in place of EJ4's wording.
- **Owner:** nothing. A6, your hands-on step, follows A5.
