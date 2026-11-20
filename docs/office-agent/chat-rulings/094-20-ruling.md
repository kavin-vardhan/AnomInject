# 094-20 — Chat ruling: the probe fix and the disk pause are accepted; tonight's job closes one more exit path, widens the check, and runs the six rebuilds with right-sized gates

**Date:** 2026-10-28, about 01:40 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-20-codex-report.md` (15:37–00:46 IST, 32,959 s, exit 0). Its verdict: `NEEDS-DECISION | Nine-hour limit during a foreign disk pause; probe fix committed but uncompiled; six builds, EI4, rebinding, EU5 and full EK1 pending; EU2 pause-write violation disclosed`.
- **Claude Code's review:** `_reviews\094-20-code-review.md`. 25 OK · 9 N/A · 2 GAP rows · 4 OPEN rows · 0 STOP · 0 stop violations.
- **The owner:** the PC is free overnight, until morning.

## Verdict
**EU2 works as ruled.**
- There were 17 real pauses, about 2.6 hours in total.
- All 17 were correctly judged foreign: the port's own D: growth was at most 139 MiB each time.
- 16 resumed. The last one was still open when the 9-hour limit came.
- Code drove the real supervisor through all four can-fails, and a mutant that ignores the port's own growth was caught.

**The probe fix is accepted as source,** at `1722348`. It is not compiled yet.
- Guarded per-level actor lists replace the actor iterator.
- Both `FindObject` lookups move to world init, behind exit and GC guards, and are kept as weak pointers.
- Cleanup at exit writes a "skipped" row.
- The fix is identical in all three hosts, with no production edit.
- Codex's 9 mutants and Code's 5 are all caught.

**The command audit:** 191 commands.
- 187 went through the wrapper.
- 4 were direct and read-only; they ran before the wrapper existed.
- 0 were direct and write-capable.

**Protected state:**
- P9 v2 18,264 of 18,264;
- `m51` equal to P9;
- the protected-state check PASS;
- the canary found 0 times;
- 0 launches;
- the manifests unchanged, so they still describe the old binaries.

**Why no builds ran:**
- free commit memory stayed at 6–15 GiB, against EI2's 16 GiB start. Windows' `wlanext.exe` alone held about 9 GB and kept growing;
- D: kept dropping by about 22 GiB at a time, from a writer nobody could identify.

## Rulings

### EV1. Codex's edit during a pause (Code's G1)
- **It is a recorded deviation.** No host file changed.
- **From now on, nothing is edited while a pause is open.**
- **The command audit flags any edit inside a pause window.**

### EV2. Report-only work after the 9-hour limit is allowed
Writing the report and the closeout receipts may continue past the limit. No new work may start.

### EV3. D: thresholds relaxed (amends ED1)

| D: | Old | New |
|---|---|---|
| start | 70 GiB | **40 GiB** |
| warn | 55 GiB | **30 GiB** |
| floor | 50 GiB | **25 GiB** |

**Why:**
- the rest of Phase A writes little to D:: incremental builds, receipts, runtime logs and the DDC, well under 20 GB in total;
- D:'s free space now sits between 44 and 75 GiB, with foreign dips of about 22 GiB.

**Unchanged:** EU2's foreign pause, and the C: and E: thresholds.

### EV4. EI2's start gate for tonight's six rebuilds (094-21 only)
**The start gate is now:**
- free commit memory at least **10 GiB** (it was 16);
- free physical memory at least 4 GiB;
- both on three polls.

**Unchanged:**
- the in-run stop at free commit below 6 GiB;
- `-MaxParallelActions=4`.

**Why:** each leg had 4–5 actions in 094-15. The 16 GiB gate was sized for full builds running next to three Blenders.

**Full builds keep the 16 GiB gate.**

### EV5. Close one more exit path, and widen the check, before the rebuilds (Code's O1 and G3)
**O1: no UObject access after the engine's exit purge, in any path.**
- That includes `Stop()`'s `RemoveFromRoot` on raw pointers.
- Unroot in a pre-exit hook, or skip when the engine is purging.

**G3: the static check also refuses, in teardown paths:**
- `LoadClass`;
- `StaticLoadClass`;
- `TObjectIterator`;
- `FindFirstObject`;
- root changes after the purge.

Each gets a mutant.

**Unchanged:**
- one probe source, using `ENGINE_MAJOR_VERSION` for engine differences;
- no production edit;
- D2 applies.

### EV6. The texture (EU5, Code's finding)
**What Code found:**
- **The texture:** `T_Vetro02_N`, the glass normal map under `MM_Glass` and six glass instances.
- **The only one:** it is the one multi-tile texture among 698.
- **Stock behaviour:** 4.25 builds only its first tile without virtual texturing.
- **Host content:** it is unchanged from the original project.
- **No effect on labels:** it renders the same way in clean and injected frames.

**The ruling:**
- **Record it as a known 4.25 host-content baseline.** No content change.
- **A7's eye check** treats glass normals as baseline.
- **Any cross-engine pixel comparison** masks glass, or uses shots where glass is minor.

### EV7. Carried
- **For the arming job:**
  - EU3;
  - EU4;
  - ER1's G3;
  - ER3;
  - the alias tests.
- **For the owner:** the clean launcher restart.
- **Unchanged:** EO1's reader budget, A9, EJ1, EJ3 and EJ5, and O3 (for A8).

## Next
**094-21 is queued now, overnight.**
1. EV5;
2. the six rebuilds, under EV3 and EV4;
3. EI4;
4. re-binding the manifests;
5. the full EK1 scan;
6. the command audit.

**Then 094-22:** arming the automated fixture-authoring leg, dry-run only.

**For the owner, tomorrow, between jobs only:**
- restart the PC. That clears the Wi-Fi driver's memory growth (`wlanext.exe`) and shrinks the swap file;
- then double-click `start_watcher.bat`;
- then start the Epic Games Launcher normally, and leave it running.
