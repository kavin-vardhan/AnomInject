# 094-07 — Chat ruling: job A3 is COMPLETE; next is A4a, the clean 4.25 builds and the strict-include pass

**Date:** 2026-10-24, about 16:55 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-07-codex-report.md` (thread `01a14334`, 13:46–15:29 IST, 6,183 s, exit 0). Its verdict: `GREEN | A3 shared closure complete; final 4.25/5.1 working pair and Baseline51 clean-engine query pass; P9 v2 checks pass; A4 gates pending; no push`.
- **Claude Code's review:** `_reviews\094-07-code-review.md`. 25 OK · 0 GAP · 4 OPEN · 0 STOP · 0 CONFLICT.
- **Mailbox report 094-07:** GREEN, 130 min.

## Verdict
**Job A3 is ACCEPTED as complete.**

**The two working compiles:**
- **One SHA:** `286f2ee`. `204724a` is source-identical, and both are up to date there with 0 actions.
- **5.1 (ReGate51 Editor Development):** 29 actions, 0 errors, 0 engine actions. The only warnings are 2 × C4996 in unchanged host code.
- **4.25 (Paris Editor Development):** 0 errors and 0 C++ warnings.

**The EG1 clean-engine query on Baseline51:** 0 engine and 0 engine-plugin actions outdated.

**Engine bookkeeping:** 3.63 MB in total, all inside EG1's class, against the 100 MB cap.

**EF1 worked as designed:** builds waited while Blender held memory, and nothing was stopped.

**P9 v2** (`8e3ed095…`) is confirmed by Codex's byte-identical re-derivation and Code's 18,264 of 18,264 re-hash. The protected state was intact at start and end.

**Coverage note, accepted:** `E:\IA_BuildCache\StackOBot\Binaries` was never a P9 root. Code snapshots it itself, and it stays on the keep list.

## Carried open items
- The owner's hand delete, any time (EG5).
- The relay reused the ledger id `…-resume5`. That is cosmetic; the next resume gets a new id if the script allows, and otherwise it is noted.
- **The strict-include tool has only run on synthetic tests.** A4a runs it for real on 4.25, and A4b on 5.1.
- **5.1 runtime equivalence is unproven** (A9–A11).

## Rulings for A4a (job 094-08)

### EH1. Scope: gates A1 (Build/toolchain) and A2 (Strict), plus A4's build-level Shipping exclusion, all on 4.25
**The clean builds**, in the Paris host, under EF1 and with EG2 receipts:
- `StylizedParisStreetEditor Win64 Development`;
- `StylizedParisStreet Win64 Development` (monolithic Game);
- `StylizedParisStreet Win64 Shipping`.

**Every build:**
- passes `-NoEngineChanges`;
- asserts compiler product version 14.16.27051 and SDK 10.0.17763.0 from the actual responses;
- counts warnings by code, file:line and category.

**Shipping:** no action, receipt or output for any of the five IA modules or for the probe.

**Gate A1's can-fails** use synthetic inputs to the evidence parser. Each of these must be rejected:
- a nonzero exit;
- one added error;
- an unexpected compiler version;
- a missing compile command;
- a rules-version mismatch, before any build is scheduled.

### EH2. Clean means clean
Before the first build, remove the Paris host's port-owned build outputs, by receipt, with a hash list first:
- the host's `Binaries\` and `Intermediate\Build\`;
- `Plugins\AnomalyInjector\Binaries\` and `Plugins\AnomalyInjector\Intermediate\`;
- the probe module's outputs.

All of it lies inside `D:\IA_Europa\StylizedParisStreet` only. A3's 4.25 outputs were partly incremental, from 10-22.

### EH3. Warnings
- **4.25 has no release to compare against,** so every compiler warning on 4.25 counts as a new port warning. Fix it, or list it individually with its cause, for chat.
- **The two user-XML warnings are accepted as environment warnings:** `bAllowUBAExecutor` and `MemoryPerActionBytes`, which 4.25 doesn't recognize, from the owner's global `BuildConfiguration.xml`. That file stays untouched.

### EH4. The strict pass, for real (plan §D2 as written)
- **Run:** `tools/strict_include_build.py` on both 4.25 targets (Editor and Game).
- **Coverage:**
  - one translation unit per plugin header, including private and reflected headers;
  - the UHT `.gen.cpp` compiles;
  - unity and PCH actually off, shown from the responses.
- **A zero-TU or zero-action run fails.**
- **Restoration:** exact restoration of rule files, matching pre/post tracked status, and a successful normal rebuild after restoration.
- **Can-fails:**
  - a disposable, port-owned source mutant that omits a genuinely needed defining include, and must fail to compile as predicted;
  - a reader test that marks a response `/Yu` or unity, which must be rejected.
- **A missing include** is fixed at its use site with the version's defining header.

### EH5. Bind the final binaries (G201/K7)
- **If any source changes in A4a** (strict fixes, warning fixes):
  - re-run all three 4.25 builds clean on the final SHA;
  - re-run the ReGate51 Editor Development working compile on that same SHA (ED2), so both engines still compile at one commit;
  - re-run the JsonExport set and the three mutants if C#, a descriptor or a `.uproject` changed.
- **Record the final 4.25 binary hashes.** A6–A8 must use exactly these.

### EH6. Not in A4a
- 5.1 Game or Shipping builds, and Baseline51 builds. Those are A4b, next.
- Any editor, game, commandlet, cook or UAT process.
- Pushes and tags.

## Next
- **094-08 is queued:** A4a. Codex resumes thread `01a14334`, and Code reviews after Codex exits.
- **Then 094-09, A4b:** the 5.1 baseline and candidate Editor, Game and Shipping builds, the 5.1 strict pass, and the warning comparison against `d7eee97`. These are heavy monolithic engine compiles, so chat will set the parallelism and probably schedule them overnight.
- **Owner:** keep Blender closed during builds if you can. The hand delete can happen any time.
