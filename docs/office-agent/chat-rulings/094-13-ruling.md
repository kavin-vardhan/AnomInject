# 094-13 — Chat ruling: UBT's own PATH prefix is admitted; rebuild before querying; an intent clause so benign stock behaviour no longer costs a round; finish A5

**Date:** 2026-10-25, about 20:30 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-13-codex-report.md` (18:21–19:17 IST, 3,341 s, exit 0). Its verdict: `NEEDS-DECISION | A5 incomplete: stock UBT PATH augmentation fails EL1; 0 compile actions; 109 prior artifacts equal, 4.25 probe PDB missing after query cleanup`.
- **Claude Code's review:** `_reviews\094-13-code-review.md`. 29 OK · 6 GAP (from the stop) · 4 OPEN · 0 stop violations.

## Verdict
**EM1 is done.** The stray file was deleted by receipt, and `m51` equals P9's record entry for entry, including its 15 untracked files.

**Done this round:**
- the EL1/EM2 denylist (`CLAUDECODE` blocked), with 26/26 synthetic tests and a real fresh-child test;
- the module comparison and the three Shipping mutants. Code's re-run differs from the accepted objects only inside the probe module;
- docstrings removed;
- EA11;
- the prediction addendum, committed before the query, which predicted this very stop.

**The stop was correct, and the rule was over-strict (mine again).** UBT's `VCEnvironment` prepends the selected MSVC and Windows SDK folders to `PATH` (`VCEnvironment.cs:141–184`) on both engines. Code's own reader confirms it: 3 toolchain folders, followed by the unchanged 60-entry base.

**Side effect:** UBT's query step deleted the outdated bound 4.25 probe PDB and `.lib` (`BuildMode.cs:321`). That is stock behaviour, and 5.1 does the same (`:568`).

**Protected state:** P9 v2 18,264 of 18,264; 0 launches; the canary found 0 times; the engine trees unchanged.

## Rulings

### EN1. UBT's toolchain environment is admitted (amends EL1 (b))
**Inside UBT's children** (checked through the makefile reader), UBT may set up the selected toolchain:
- a **prefix** on `PATH`, whose entries are exactly the selected MSVC and Windows SDK folders for that engine (matching the response files), with the **base `PATH` unchanged as the suffix**;
- any `INCLUDE`, `LIB`, `LIBPATH`, `WindowsSdk*` or `VC*` variables it sets, provided every entry points into the selected MSVC or Windows SDK folders. Record them as additions.

**Still required everywhere:**
- no blocked name, and no canary;
- every other non-blocked base variable present and unchanged.

**At the wrapper-to-UBT boundary,** EL1 (b) stays exactly as written: the base unchanged.

**EL1 restarts with two fresh D2 attempts.** This is a ruling, not a harness fix.

### EN2. Rebuild before you query
**UBT deletes outdated outputs before it writes an action plan.** So for every host:
- rebuild first;
- run that host's clean-engine query only after its rebuild;
- record any query-time deletion of outdated, port-owned outputs (as with the 4.25 probe PDB and `.lib`). That deletion is accepted stock behaviour.

The rebuilds then recreate the outputs, and the manifests are re-bound.

### EN3. Ratified: Codex's removal of the three identical 161-byte duplicate probe stub files
They were declared in its predictions and removed by hash receipt, and they were needed for the first probe compile.

### EN4. Code's self-test writes
Code may write its self-test re-run outputs under `D:\IA_Europa\Temp\CodeReview09414\` as well as `Receipts\CodeReview09414\`.

### EN5. The intent clause (standing, from now on)
**When a chat rule conflicts with stock, benign tool behaviour documented in engine source,** such as a UBT environment change, a stale-output cleanup, or a cache rewrite:
- **record it, with its source citation;**
- **apply the narrowest reading that keeps the rule's intent;**
- **continue, and report it for ratification.**

**The narrowest reading must keep the intent:**
- no blocked names or secrets;
- no loss of base variables;
- no write outside the allowed places;
- no change to protected state.

**This clause never covers, and these stay hard STOPs:**
- P9 or protected-state differences;
- engine-tree writes outside EG1;
- secret or canary exposure;
- any machine, security or global setting;
- any editor or game launch in a no-launch job;
- a floor breach.

## Next
- **094-14 is queued: A5's finish,** under EN1–EN5:
  - EL1 with EN1;
  - the 6 rebuilds, the 4.25 one recreating the deleted PDB and `.lib`;
  - the post-rebuild makefile proof;
  - the EI4 query, after the rebuilds;
  - re-binding the manifests;
  - the owner kit (the `.bat` and the card draft), dry-run only.
- **The owner card** must also say: answer "No" to any "rebuild modules?" prompt.
