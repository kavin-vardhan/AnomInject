# 094-12 — Chat ruling: a stray 7-byte file stopped A5 at entry; Code removes it by receipt; A5's finish re-runs unchanged

**Date:** 2026-10-25, about 18:00 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-12-codex-report.md` (657 s, exit 0). Its verdict: `NEEDS-DECISION | A5 blocked at EA8 entry by the disclosed extra untracked m51 file; no implementation or builds; 110 prior artifacts preserved`.
- **Claude Code's review:** `_reviews\094-12-code-review.md`. 16 OK · 8 GAP (all from the stop) · 0 stop violations.

## Verdict
**The stop was correct,** under EA8 ("on any difference, STOP; never an automatic restore").

**The cause:** a background shell call of Code's failed to enter its folder. Its trailing `echo` then wrote `ek1-prescan-console.txt` (7 bytes, `exit=1`) into the protected `m51` checkout, which is the watcher's working folder. That took the untracked count from 15 to 16.

**Nothing else changed:**
- P9 v2: 18,264 of 18,264;
- the three manifests are unchanged;
- 0 launches, 0 commits;
- the canary was found 0 times in 110 new or changed files.

**A5 has not moved.** EL1's two D2 attempts are unspent.

## Rulings

### EM1. Remove the stray file, and stop it happening again
- **Code deletes exactly** `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\ek1-prescan-console.txt`, by receipt.
  - Verify its SHA-256 first: `CF205DBB8CEA84897B488ABCC281BF96698D5E94B1096B16657B4CABA9082A22`.
  - Then confirm the `m51` checkout shows 15 untracked entries and is otherwise unchanged.
- **The watcher's working folder is the protected checkout.** So Code:
  - uses absolute output paths, inside its own receipts folder, for every write;
  - makes background commands fail closed: no trailing write when a `cd` fails;
  - checks, before launching Codex, that `m51`'s status equals the recorded baseline (15 untracked). **A difference means Code stops itself; it does not launch.**

### EM2. `CLAUDECODE` joins EL1's denylist
`CLAUDE_*` doesn't match `CLAUDECODE`, which has no underscore. It is an identifier, not a secret, but children don't need it, so it is blocked too.

## Next
**094-13 is queued:** the 094-12 request again, unchanged apart from EM2, after Code's EM1 removal. Code reviews after Codex exits.
