# 094-21 — Chat ruling: the probe fix is built and bound; A6's second half (scripted fixture authoring) is authorized in principle, and 094-22 arms it, dry-run only

**Date:** 2026-10-28, about 04:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-21-codex-report.md` (01:49–03:49 IST, 7,223 s, exit 0). Its verdict: `GREEN | EV5 fixed and compiled in six targets; static mutants, EI4, 110-artifact bindings, P9 and scans pass; no runtime launch`.
- **Claude Code's review:** `_reviews\094-21-code-review.md`. 43 OK · 8 N/A · 1 GAP (G1, not blocking) · 2 OPEN (carried) · 0 STOP · 0 stop violations.
- **The owner:** the PC is free overnight, until morning.

## Verdict
**ACCEPTED.**

**EV5, the exit-path fix:**
- root changes go only through a guarded `ChangeRoot`;
- `Stop()` makes no UObject access after the exit purge. Code walked every path from the source;
- `ReleaseGC` and `ScreenshotDone` skip at purge, with a receipt;
- world receipts are keyed by address;
- one probe source in all three hosts, with no production change.

**The static check** now also refuses:
- `LoadClass` and `StaticLoadClass`;
- `TObjectIterator` and `TObjectRange`;
- `FindFirstObject`;
- root changes outside the helper.

It passes on all four sources, with 25/25 self-tests each. Code's own 21 mutants were all caught.

**The builds:**
- all six rebuilds exited 0 on the first try, with 0 engine actions;
- EI4 found 0 actions;
- 0 disk pauses: D: never fell below 73.54 GiB.

**The re-bound manifests,** re-hashed by Code (110 of 110):
- 27 files: `c501895c…`;
- 39 files: `b05e6f8c…`;
- 44 files: `3865fe0c…`.

Only probe-linked outputs changed. Every plugin DLL is byte-identical.

**Protected state:**
- P9 v2 18,264 of 18,264;
- `m51` equal to P9;
- the protected-state check PASS;
- the canary found 0 times in 2,858 files;
- the full EK1 scan: 43,037 files, 0 secret values;
- the command audit: 117 wrapped commands, 1 direct read-only, 0 direct write-capable;
- 0 launches.

**Still open:** the clean exit is proven from source only (Code's O1). The next runtime leg proves it.

## Rulings

### EW1. The fixed probe binaries are the bound set
- **A7 onward uses exactly these binaries** (ES4).
- A6's owner-leg evidence stays bound to the 094-15 binaries.
- Every kit is re-bound to these manifests before its next use. The spent 09417 owner kit refuses them for now, which is correct.

### EW2. A6's second half is authorized in principle
**What it does:** builds `EA_Core` and `EA_Travel` in the 4.25 Paris port host from the A5 recipes, by script, unattended.
- 094-22 arms it, dry-run only.
- A later job runs it.

**What it may write:**
- only the declared new fixture packages, listed by exact path before the run, in a dedicated fixture folder in the Paris port host's Content;
- nothing is ever overwritten;
- every existing Content file is re-hashed before and after, and none may change.

**What the run must show:**
- **A clean exit:** the probe's exit rows are present (the "skipped" cleanup row and `probe_stop`). A crash is a STOP.
- **Nothing beyond authoring:** no PIE, cook or packaging, and no production plugin change.
- **No `.uproject` or config file change.**
  - If the recipes need the stock Python Editor Script Plugin, it is enabled for that run only, on the command line, with nothing saved, provided 4.25 supports that.
  - Otherwise 094-22 ends NEEDS-DECISION, with the exact options.

### EW3. The automated kit (built in 094-22, dry-run only)
**The gate: the measured profile, in full.**
- human idle for 300 s;
- at least 6 GiB of free physical memory;
- GPU use at most 15 %;
- no foreign Unreal process;
- three passing polls.

**The launcher (EU3):**
- it must already be running;
- the kit waits with a plain message if it isn't, and never starts it;
- the launcher's own processes don't hold the gate; they are recorded.

**Uploads (EU4):** every argv carries `-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False`. A kit argv without it refuses.

**Unattended crashes, a pre-approved rule.** Use `-unattended` only if Codex's 4.25 source reading shows all three of:
- with the switch, there is no upload;
- no dialog can block the run;
- under unattended, the known prompts (rebuild modules, convert or resave, check-out, save on exit) take their non-destructive default.

Otherwise, run without `-unattended`. At the run's time limit, stop only our own editor tree, including its crash-reporter child, by PID.

**Code's G1: wrapper metadata stays out of children.**
- `EUROPA_BOOTSTRAP_PROOF`, and any other wrapper metadata variable, is kept out of every build and runtime child. It is recorded only in the wrapper's own receipt.
- EN1's strict byte test must pass again on the next regenerated makefiles.
- The six current makefiles are regenerable caches. They keep the variable until their next EO3 refresh.

**Code's O2: widen the static check** to the 8 related calls Code listed (for example `StaticFindFirstObject`, `FObjectIterator`, and a plain dereference in `Stop`), with a mutant for each.

**Carried items, built here:**
- ER1's G3: each refusal shows its own reason;
- ER3: a bare `UnrealBuildTool.exe`, or a dotnet UBT, holds the measured gate;
- the 8.3 short-name and junction alias tests.

**Attempts:**
- a fresh job root per attempt;
- a launched lock;
- at most one editor session per recipe, so two sessions;
- a per-session time limit, which Codex proposes.

**No launch in 094-22.** It ends with a dry run of the armed kit, on throwaway slots.

### EW4. Carried
- EO1's reader budget;
- A9;
- EJ1, EJ3 and EJ5;
- O3, for A8;
- EV6's glass baseline, for A7;
- Code's O3: physical memory dipped to about 0.1 GiB during the 5.1 game links. That is paging, which is allowed. Commit stayed at 14.3 GiB or more.

## Next
**094-22 is queued now:** arm the automated fixture-authoring leg (EW2 and EW3), dry-run only.

**The real run follows** in the next free overnight window, once chat has ruled on 094-22.

**For the owner,** after 094-22 has finished (chat will say when):
1. restart the PC;
2. double-click `start_watcher.bat`;
3. start the Epic Games Launcher normally, and leave it running.
