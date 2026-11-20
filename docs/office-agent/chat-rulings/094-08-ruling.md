# 094-08 — Chat ruling: A4a is COMPLETE; A4b (the 5.1 builds) is next, with parallel builds under a commit-based memory gate, and Code clears the build cache first

**Date:** 2026-10-24, about 22:15 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-08-codex-report.md` (thread `01a14334`, 16:54–21:48 IST, 17,608 s, exit 0). Its verdict: `GREEN | A4a clean 4.25 trio and strict pass complete on 489c828; same-source ReGate51 Editor passes; P9 v2 and ledgers pass; A4b pending; no push`.
- **Claude Code's review:** `_reviews\094-08-code-review.md`. 36 OK · 0 GAP · 7 OPEN · 0 STOP.
- **Mailbox report 094-08:** GREEN, 319 min, about 2.4 h of it waiting for memory while Blender was open.
- **The owner (10-24, about 18:45 IST):** "Make code delete them" (the build cache).

## Verdict
**A4a is ACCEPTED as complete, at `489c828`.**

**Gate A1 (Build/toolchain), 4.25:**
- three clean builds: Editor 35 actions, Game 16, Shipping 5;
- all exit 0, with 0 errors, 0 compiler or linker warnings, and 0 engine actions;
- 14.16.27051 and SDK 10.0.17763.0, read from the response files;
- the only warnings are the two accepted user-XML lines.

**Gate A2 (Strict):**
- 77 of 77 headers as their own translation units, plus 64 `.cpp` and 11 generated files;
- 152 compiles with PCH and unity off;
- the missing-include mutant failed as predicted (C2027);
- exact restoration and a clean normal rebuild.

**Shipping:** no IA or probe module, shown four independent ways.

**The one source change:** a 4.25-only `WeakObjectPtr.h` include. ReGate51 recompiled clean on the same commit.

**Final 4.25 manifest:** 27 files, re-hashed 27 of 27 by Code. **A6–A8 must use exactly this manifest.**

**Protected state:** P9 v2 18,264 of 18,264; the protected state unchanged; 5.1 bookkeeping 1.30 MB, all inside EG1.

## Carried open items
- **The `AnomalyInjector.uplugin` staging line in the Shipping receipt** carries no code. Packaged exclusion is A8/A11a's job.
- **5.1 runtime parity** is A9–A11.
- **5.1 strict:** A4b.

## Rulings for A4b (job 094-09)

### EI1. Scope
**Builds,** all `Win64`, under EF1 as amended by EI2, with EG2 receipts, `-NoEngineChanges`, and 14.38.33130 asserted from the responses:

| Host | Builds |
|---|---|
| Baseline51 (`d7eee97`, the control) | `StackOBotEditor Development`; `StackOBot Development` (monolithic) |
| ReGate51 (`489c828`) | `StackOBotEditor Development` (clean); `StackOBot Development`; `StackOBot Shipping` |

**Not built: Baseline51 Shipping.** Our plugin is in neither Shipping build. The release's exclusion was qualified at delivery, and the descriptor matrices equal release. The candidate's Shipping build proves the port's exclusion, and skipping the control's saves one full engine compile.

**R1 evidence:**
- warnings compared by code, file:line and category between Baseline51 and ReGate51, for Editor and Game Development. No new or worse warning in the candidate.
- the **generated** defines in non-Shipping: `ANOMALY_CAPTURE=1`, `ANOMALY_CONTROL_SERVER=1`, `ANOMALY_SHADERS=1`, and the bench profile enabled;
- the tick-pin compile setting equal to the baseline's;
- ReGate51 Shipping with no IA or probe action, receipt or output.

**The 5.1 strict pass on ReGate51,** both targets, as plan §D2. Its can-fails, exact restoration and normal rebuild after are as in A4a.

**EG1's clean-engine query at exit** (EI4).

### EI2. Memory and parallelism (amends EF1)
**Why:**
- The 10-22 crash was commit exhaustion. Free commit was 3.4 GiB and the page file was at its maximum, held mostly by Blender.
- A shortage of physical memory alone causes paging, which is slow but does not crash the PC.
- At one action at a time, each 5.1 monolithic build (about 840 actions, by Code's measurement) would take many hours.

**The gate:**
- **Start:** free commit 16 GiB or more, and free physical 4 GiB or more, on three polls about 20 s apart.
- **In-run stop:** free commit below 6 GiB on two consecutive 5 s samples stops the port's own UBT tree, by PID. Physical memory is no longer an in-run stop.
- **Parallelism:** `-MaxParallelActions=4` for every A4b build. UBT's own memory limit may lower it, never raise it.
- **The rest of EF1 is unchanged:** a 4-hour wait limit, a stopped run is not a D2 attempt, and a stopped run's outputs are cleaned before the next run.

### EI3. Counting engine work in a monolithic build
**Classify actions by where their outputs go.**
- **Any action writing under either engine tree is a STOP.**
- **Actions that compile engine source into the host's own `Intermediate`** are expected for monolithic targets. Report them as a count ("engine-source actions in host").

EG1's bookkeeping class is unchanged.

### EI4. EG1's clean-engine query cadence
Required at the exit of every job that runs any 5.1 build: Baseline51 Editor Development, an action-plan query only, with 0 engine and 0 engine-plugin actions.

### EI5. Time limit for A4b
- **The limit is 14 hours of wall-clock time from Codex's start,** instead of 9 hours of work, because A4b is mostly compile time.
- Stop at clean points between builds. If A4b isn't finished by the limit, stop at the last clean point and end NEEDS-DECISION "A4b continues".
- D2 and the two-attempt compile-error rule are unchanged.

### EI6. Tooling scope (Code's OPEN 6)
- **Make the job folder and the target allow-list parameters per job.** A4b's allow-list is the five builds above, the strict runs and the clean-engine query.
- **The memory-gate self-tests** run against a test folder that Code can also use, so Code can reproduce all of them.

### EI7. The build cache: Code deletes it (the owner's request)
Code does this before launching Codex, following `code-receipts-094-05\cleanup\OWNER-DELETE-LIST.md`:

1. **Remove the 3 junctions non-recursively.** Check that each target is intact, then re-scan the delete set for any remaining reparse point. If one remains, stop the cleanup.
2. **Delete the junction-free folders recursively.**
   - The roots: the 14 `_r*` folders; the listed `_binary_baselines` items; the contents of `StackOBot\Builds`, `Intermediate` and `Saved`.
   - **Keep:**
     - `release-2026-10-05-23098fa-218BA4C7`;
     - `StackOBot\Binaries`;
     - every junction-target folder itself.
   - **Never use PowerShell `Remove-Item -Recurse`.**
3. **Verify afterwards:**
   - StackOBot Content against its snapshot;
   - `D:\IA_BankOverflow` against v1;
   - the release baseline's 8 hashes;
   - E: free space.
4. **If Code's own safety hook refuses any delete:**
   - don't rephrase or disguise the same delete;
   - stop the cleanup, record it, and continue with the port job;
   - the owner then deletes by hand.

P9 v2 already excludes these roots, so the cleanup never trips a port STOP.

## Next
- **094-09 is queued:** Code does EI7, then resumes Codex for A4b. Code reviews after Codex exits. It is expected to run overnight.
- **Owner:** keep Blender closed while A4b runs. The new gate tolerates more, but Blender plus four parallel compiles is the risky mix.
