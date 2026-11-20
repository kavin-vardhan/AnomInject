# 094-16 — Chat ruling: fix the kit's two failure-path gaps before the owner leg, then re-arm it

**Date:** 2026-10-26, about 05:00 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-16-codex-report.md` (02:51–04:04 IST, 4,325 s, exit 0). Its verdict: `GREEN | A6 owner kit armed; three real slots unused; armed dry run passes; zero runtime launches`.
- **Claude Code's review:** `_reviews\094-16-code-review.md`. 30 OK · 10 N/A · 2 GAP · 6 OPEN · 0 stop violations · 0 conflicts.

## Verdict
**The arming matches EP1, and nothing launched:**
- the three real slots are unused;
- Codex's 62 + 48 tests reproduce, and Code's own 61 can-fails pass;
- Code's own dry run of the real `.bat` passed, including the real P9 preflight;
- the bound manifests (27/39/44) and all 110 bound files are unchanged;
- P9 v2 18,264 of 18,264; `m51` equal to P9; the protected-state check PASS ×3; the canary found 0 times; 0 launches.

**But the kit isn't ready for the owner yet.** Code found two real gaps in failure paths:
- **G1:** the live GPU reading fails about 4 % of the time, and the failure isn't caught. The owner would see a Python traceback, and a try would be used up. Over a long wait, a traceback is almost certain.
- **G2:** if anything fails between the editor process starting and the "started" mark, the kit cancels the reservation while the editor is running. A second double-click could then start a second editor. Code reproduced this with a harmless child.

Both are cheap to fix, and this is the owner's first hands-on step, so **both are fixed before the owner leg.**

## Rulings

### EQ1. G1: a failed reading means "wait", never a traceback
**Every reading** (memory, GPU, running programs, disk) that errors or times out:
- counts as that condition's "cannot read …";
- is not a pass, and resets the three-pass count;
- keeps the wait going, with the plain waiting line.

The 45-minute give-up stays.

**A top-level safety net in the kit.** Any other unexpected error:
- prints one plain line, `Stopped: unexpected error; details saved to <file>. No editor was started.`, when no editor has started (if one may have started, EQ2 applies);
- saves the full details to the attempt folder, not the console.

**Prove it live:** at least 200 real collector calls under the owner profile, with the error rate recorded and 0 uncaught exceptions.

**Self-tests, with can-fails:**
- a `CalledProcessError` and a `TimeoutExpired` on each reading → a "cannot read" line, and no pass;
- a forced unexpected exception → the plain stop line and the details file.

### EQ2. G2: once the editor process exists, the session counts as started
- **Write the launched mark** immediately after the process is created, before any receipt write, observer or thread starts.
- **Any exception after that point** leaves the mark in place.
- **Cancel the reservation only** when process creation itself failed.

**Can-fails, using a harmless child only (never the editor):**
- a forced exception at each step after process creation → the mark exists, and the next attempt is refused;
- process creation failing → the next attempt is usable.

### EQ3. Code's OPENs O1–O3
- **O1, the owner-preview profile only:** `UnrealTraceServer.exe` and `UnrealVersionSelector.exe` are recorded but don't hold the gate. Every editor, commandlet, game, build tool and ShaderCompileWorker still does. The measured profile is unchanged.
- **O2, both profiles:** a `dotnet.exe` whose command line runs UnrealBuildTool counts as an Unreal program.
- **O3:** a dry-run config that names any real slot or state path is refused.

### EQ4. Re-arm, then dry-run
**The tooling hashes change, so re-arm for job 09417:**
- three fresh real slots under `Receipts\09417`;
- the same EP1 scope;
- the citation: `094-15-ruling.md#EP1`, as amended by this ruling.

**The 09416 arming is superseded:** its slots stay unused, and it must no longer be usable.

**Dry-run on throwaway slots:** the real slots stay unused, with 0 launches.

**The bound binaries and manifests stay unchanged.** Any change to them is a STOP.

**D2 applies to the EQ1–EQ3 tooling:** two attempts.

### EQ5. The card (chat writes it after 094-17)
It will say:
- the checks take about 10–15 minutes, and the 45-minute wait starts after them;
- if the window shows "Stopped" or any error, take a screenshot and tell chat;
- close Blender, heavy jobs and every other Unreal program, and pause the <other-home-project> watcher, before double-clicking.

## Next
- **094-17 is queued now:** EQ1–EQ4, dry-run only. Code reviews after Codex exits.
- **The post-leg evidence job** (EP3) becomes 094-18.
- **The owner leg follows 094-17,** with chat's final card.
