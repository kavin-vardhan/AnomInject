# 094-02 — Chat ruling: the stop was right and nothing was touched; the write proof becomes a targeted no-admin check; C: must recover before A1 resumes

**Date:** 2026-10-16, about 15:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-02-codex-report.md` (thread `01a14334`, Astra max, full access, 13:28–14:48 IST, 4,798 s, exit 0). Its verdict: `NEEDS-DECISION | A1 write proof blocked; no hosts, worktrees or commits; A2 not executed`.
- **Claude Code's review:** `_reviews\094-02-code-review.md`. 5 OK · 1 GAP (EA4) · 9 OPEN (not reached) · 0 CONFLICT. Its independent P9, engine-tree and routing checks are all clean.
- **Mailbox report 094-02:** NEEDS-DECISION, 95 min, $13.27 API-equivalent.
- **The owner's question (15:20):** "Why is C: being touched??"

## Verdict
- **The stop was correct.** Codex could not produce the proof it set itself, and it did not escalate permissions or relax the proof on its own.
- **Nothing was changed:**
  - **J2 (P9):** unchanged. Codex's full baseline covered 707,775 files / 582.9 GB, and its exit check found 0 differences. Code independently matched 19,290 of 19,290 files.
  - **Both engine trees and the Paris original:** 0 writes in the job window.
  - **Hosts, worktrees, branches and commits:** none created.
- **The port wrote nothing to C:.** Code's in-window scans of every C: path the port could touch found 0 port writes. The only `%LOCALAPPDATA%\UnrealEngine` changes belong to the owner's UE 5.7 editor.
- **C: still fell from 12.84 to 10.28 GiB during the job.** None of the measured folders explains it:
  - `.codex` 24.70 → 24.71 GB;
  - `%LOCALAPPDATA%\Temp` 12.65 GB, flat;
  - NVIDIA cache 4.75 GB, flat;
  - `C:\ClaudeTemp` 4.80 GB, flat;
  - `%LOCALAPPDATA%\UnrealEngine` 2.55 → 2.53 GB;
  - `C:\pagefile.sys` 512 MB, unchanged.

  The cause is outside the port and still unidentified. EB4 measures it.

## Rulings

### EB1. The EA4 write proof: a targeted, declared check that needs no admin
EA4 never required system-wide tracing. The ETW and USN attempts were Codex's own choice, and both need elevation.

**The proof standard is now:**
1. **Per-operation inventories** (path, size, mtime) of the EA4 exception paths, before and after every operation, enforcing the byte budgets: 250 MB on C:, 50 MB on E:.
2. **In-window modified-file scans:**
   - both engine trees (`E:\EpicGames\UE_4.25`, `D:\UESource\UnrealEngine`);
   - the Paris original;
   - `%LOCALAPPDATA%\UnrealEngine` and `%LOCALAPPDATA%\UnrealBuildTool`;
   - `%PROGRAMDATA%\Epic`;
   - the drive roots of C: and E: (top level only);
   - in `%TEMP%`, only UE-named entries (`UnrealShaderWorkingDir`, `UnrealBuildTool*`, `UE4*`, `Unreal*`). Other apps write there constantly.
3. **Receipts of every child command line,** showing the redirect arguments (`-UserDir`, `-ShaderWorkingDir`, `-DDC`, `-abslog`, `-Log=`, `uebp_*`, TEMP/TMP).
4. **Free space is a floor monitor only,** never evidence of attribution in either direction.
5. **No ETW, no USN journal reads, no elevation, no attempt at any of them.**

**What counts as a breach:**
- A write in a scanned location that the port caused and the exceptions don't name is a STOP.
- Writes there by other processes (the owner's apps, UE 5.7, Windows) are recorded as foreign, not port-attributed.

**The orchestration tools' own folders** (`<USERPROFILE>\.claude`, `<USERPROFILE>\.codex`, `C:\ClaudeTemp`) are measured each job and reported as tooling overhead, outside the port budget.

### EB2. C: headroom before resuming
- **To start**, the next port job needs **C: of 13 GiB or more** and **E: of 52 GiB or more**, measured on the resolved volumes.
- **If either is short:** the brief polls every 5 minutes for up to 3 hours without launching Codex, then reports NEEDS-DECISION "C: headroom".
- **Floors and warning margins are unchanged** (EA4). A breach mid-job is still a STOP.
- **The owner is asked to free space** (chat's recommendation: Windows "Temporary files" cleanup, about 12.7 GB).
  - **Do not touch `<USERPROFILE>\.codex`.** It holds the Codex threads both J2 and this port resume from.

### EB3. Resume scope
- **Resume the same Codex thread (`01a14334`).**
- **Entry check: the per-job EA8 entry proof only.**
  - It covers git state, protected inventories (path, size, mtime) and full hashes of protected binaries and configs, against today's full baseline (`full-sha256.jsonl` `db884f1e…`).
  - **The 49-minute full hash is not repeated.** It runs again only at the final Phase A job.
- **Then continue A1 under EB1:**
  - the hosts and worktrees, per EA3;
  - routing and its ledger;
  - the idle gate moved from draft into port tooling;
  - commit the reviewed predictions draft on `feat/europa-port` before the first UBT run.
- **Then A2:**
  - review the 22-file draft, then apply it;
  - the EA11 check in the real worktree;
  - both UBTs compiling the shared C# rules;
  - the toolset selection (14.38.33130 / 14.16.27051) shown from the logs;
  - the JsonExport matrices on both engines;
  - the removed-blacklist mutant.
- **The 094-02 STOP rules carry over unchanged.** The new EB1 standard replaces only the proof method.

### EB4. A C: space audit, sizes only
At the start and end of the next job, Code records the sizes of:
- the top-level folders on C:;
- `C:\Windows\SoftwareDistribution`, `C:\Windows\Temp` and `C:\ProgramData`;
- `<USERPROFILE>\AppData\Local`, `Roaming` and `LocalLow`, each with its top 15 children by size;
- `hiberfil.sys`, `pagefile.sys` and `swapfile.sys`;
- `$Recycle.Bin` (the total only).

Rules for the audit:
- **Measure, never delete.** Access-denied entries are recorded as such.
- **Goal:** tell the owner what is eating C:, including today's unexplained drop of about 2.6 GiB.

### EB5. Process notes
- **Accepted:** Codex stopping the logman session it had started, with its 8 KB ETL left on D:.
- **Allowed through the receipts:** the RoomGenerator junction harness fix (D2 attempt 1 on that problem).
- **CaptureBench `e64d6f9`** is confirmed from `docs/sessions/2026-09-30-089-02b-m53-rebench-prep.md:76`.
- **Not owner items:** the connector-authorization notes in Code's report are unrelated to GDP.

## Next
- **094-03 is queued:** Codex resumes thread `01a14334` under EB1–EB5 for A1 and A2. Code reviews after Codex exits (W3).
  - It starts only once C: is at 13 GiB or more (EB2).
- **Owner:** clear Windows temporary files (Settings → System → Storage → Temporary files). Leave `.codex` alone.
