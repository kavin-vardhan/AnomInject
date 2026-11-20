# 094-06 — Chat ruling: the STOP was right; the 5.1 build bookkeeping becomes an allowed class with a clean-engine proof; P9 v2 is cut now so the owner's delete can't trip a job

**Date:** 2026-10-24, about 13:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-06-codex-report.md` (thread `01a14334`, 10:05–10:42 IST, exit 0). Its verdict: `NEEDS-DECISION | A3 stopped at EF2 on inherited 5.1 engine metadata outside ED4; protected binary/config hashes match; no repository edits, cleanup, compile or push in 094-06; final compile pair pending`.
- **Claude Code's review:** `_reviews\094-06-code-review.md`. 17 OK · 2 GAP · 6 OPEN · 1 STOP (ED4) · 0 CONFLICT.
- **Mailbox report 094-06:** NEEDS-DECISION, 69 min.

## Verdict
- **The 10-22 crash was memory, not hardware.** The event log shows three Windows low-memory warnings naming three Blender processes (about 22 GB each), then a Start-menu restart at 17:11:32. There was no blue screen and no WHEA error. 10-18 was an unclean stop, also with no hardware error.
- **The recovery checks pass:** all 25,866 protected hashes, `git fsck`, and the open ledger's integrity check. Code's independent re-hash matched 19,290 of 19,290 files.
- **The STOP was correct.** The run cut off on 10-22 left 1,023 files (1.91 MB) in the 5.1 engine tree, written 17:05:55–17:06:04:
  - 653 `LiveCodingInfo.json`, all byte-identical to v1;
  - 368 UHT `Timestamp` (140 with new content);
  - `ActionHistory.bin`;
  - one `UbtRuns` marker.
  
  400 of them sit under engine plugins, outside ED4's root. No engine binary, generated C++, source, content, config, shader or program file changed.
- **No source edit, cleaning or compile happened in 094-06.** `feat/europa-port` is still at `5bd368b`.

## Rulings

### EG1. The 5.1 build bookkeeping is an allowed class (it replaces ED4's 5.1 clause)
**Why:** Codex traced each file type to stock UBT code:
- `LiveCodingInfo.json` is written whenever a target supports Live Coding (`UEBuildModuleCPP.cs:745`);
- UHT `Timestamp` files are written for every module not marked read-only, engine plugins included (`ExternalExecution.cs:855`);
- the `UbtRuns` marker is created at start and removed in a `finally` (`UnrealBuildTool.cs:596/710`), so a killed run leaves one behind.

Every 5.1 build writes these. They are bookkeeping, not engine changes.

**Allowed:**
- These files under `D:\UESource\UnrealEngine\Engine\Intermediate\Build\` and under every `Engine\Plugins\**\Intermediate\Build\`:
  - UHT `Timestamp` files;
  - `LiveCodingInfo.json`;
  - `ActionHistory.bin`;
  - dependency and source-file caches;
  - makefiles;
  - UHT manifests.
- UBT's own `Engine\Intermediate\UbtRuns\<pid>_<hash>` markers.

**How it is held:**
- 100 MB in total per job, with per-operation receipts (path, size, mtime, hash).
- The same rule covers a run that fails, is stopped by EF1, or is cut off by a restart. Its bookkeeping counts in this class and cap, and nothing else is excused.

**Still a STOP:**
- any change to engine or engine-plugin binaries (`.exe .dll .pdb .lib .obj .modules .target`);
- any change to generated C++ (`.gen.cpp`, `.generated.h`, `.init.gen.cpp`);
- any change to Source, Content, Config, Shaders or Programs;
- any engine or engine-plugin compile or link action.

**The 1,023 inherited files are accepted under this class.**
- Restore nothing.
- The `42980` marker stays until UBT removes it itself, and that removal is in-class.

**New can-fail: the engine is still clean for other projects.**
- At exit, run one action-plan query, with no compile: Baseline51 (the `d7eee97` control), `StackOBotEditor Win64 Development`, with `-NoEngineChanges` and `-WriteOutdatedActions`.
- The outdated list must contain **0 engine and 0 engine-plugin actions**. If it doesn't, that is a STOP.
- This query is allowed despite ED2's "no Baseline51 build". Its own writes stay in Baseline51 or in EG1's class.

### EG2. Receipts (Code's GAP)
- **Every build receipt** hashes each produced binary (DLL, EXE, PDB), and records the effective `/std` and PCH flags from the response files.
- **Re-run the 4.25 compile on the final SHA.** EF2's fallback applies, because the 17:01 receipt cannot verify its DLLs.

### EG3. `tools/europa/tooling_self_tests.py`
- Fix it to the current build-mode guard.
- Make `--output` mandatory, so it can never write into an old job's folder. Don't retire it.

### EG4. The 12 empty folders
Code created them in 094-06 under `D:\IA_Europa\Receipts\09405\routing` and `D:\IA_Europa\Temp\09405`. Code removes exactly those, by receipt, at the start of 094-07. They are its own scratch, on D:.

### EG5. P9 v2 now, independent of the owner's delete (EE7 continued)
**Why:** the owner's hand delete has to wait for a gap between jobs, and that gap keeps being missed. Cutting v2 now removes the coupling.

**What Code does:**
- At the start of 094-07, write P9 v2 as v1 minus every entry under the roots on `code-receipts-094-05\cleanup\OWNER-DELETE-LIST.md`, **whether or not they have been deleted yet**. Those roots are:
  - the 14 `_r*` folders;
  - the listed `_binary_baselines` items;
  - the contents of `StackOBot\Builds`, `Intermediate` and `Saved`.
- Use the same files and receipt as 094-05 Step 2.
- The keep list stays protected.

**What Codex does:** re-derive v2 and run its entry and exit proofs against it.

**What changes for the owner:** from now on he can delete those roots at any time, even while a job runs.

**What stays protected:**
- Everything else.
- Code checks `D:\IntrusiveAnomalies\StackOBot\Content` and `D:\IA_BankOverflow` against their snapshots at its start and again after Codex exits. A delete that went through the junction would still be caught, as RED.

### EG6. Finish A3 (094-06's order, otherwise unchanged)
1. EF1, the memory gate, with its self-tests.
2. EF2's ReGate51 cleaning, inside ReGate51 only.
3. **The 8 first-include errors** (CameraClipping, ChangeCase, CorruptedTexture, LodPopping, MissingTexture, StuckLowMip, TexCorrupt, TimeDilation).
   - Codex's proposal is accepted: put each file's own header first, then the version include, keeping the outer 5.1 implementation guard.
   - Each own header must be safe to parse on 4.25, either portable or guarded itself, so that C3 holds.
4. The 5.1 working compile, to 0 errors.
5. The 4.25 re-run on the same final SHA (EG2).
6. EG1's Baseline51 clean-engine query.
7. Exit.

## Next
- **094-07 is queued.** Code does EG4 and EG5 and its start checks, then resumes Codex to finish A3. Code reviews after Codex exits.
- **Owner:**
  - do the hand delete whenever suits you, even during the job;
  - keep Blender closed while builds run, or the memory gate waits;
  - don't install the Visual Studio 17.14 update.
