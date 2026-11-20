# 094-26d — Chat ruling: A6 is COMPLETE. Both fixture maps were authored on 4.25 in clean sessions; 094-27 started automatically

**Date:** 2026-10-30, about 07:30 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-26d-codex-report.md` (05:57–06:49 IST, 3,115 s, 1 try, exit 0). Its verdict: `GREEN | Both A6 recipes authored fresh with clean exits and three declared packages; FF2 bookkeeping recorded, protection and canary checks passed.`
- **Claude Code's review:** `_reviews\094-26d-code-review.md`. 51 rows: 40 OK · 5 N/A · 5 Carried · 1 OPEN · no FAIL. Also G1, G2 and O1–O5.
- **Mailbox report 094-26d:** GREEN, 69 min. 094-27 started from the queue at 07:00:22, as FA7 intended.

## Verdict
**A6 is ACCEPTED as complete.**

**The runs:**

| Recipe | Session (IST) | Duration | Exit |
|---|---|---|---|
| EA_Core | 06:21:52–06:22:07 | 15.0 s | 0 |
| EA_Travel | 06:31:28–06:31:40 | 12.6 s | 0 |

**What the runs show:**
- **Clean exits:** a normal `LogExit` and exactly one `probe_stop` each, with no PIE.
- **Logs:** no errors and no ensures. The only error-pattern lines are a Magic Leap notice and "Map check: 0 errors".

**The outputs:** exactly the three declared packages are new.

| Package | Size | SHA-256 |
|---|---|---|
| `SM_EA_TwoLod.uasset` | 137,553 B | `bbd24a79…` |
| `EA_Core.umap` | 21,114 B | `c2986f9a…` |
| `EA_Travel.umap` | 21,124 B | `51c7fe5c…` |

**Unchanged:**
- the 1,453 earlier Content files;
- `Config`, the descriptor and the `.uproject`;
- the binaries.

**The authored content matches the A5 manifest and the bound recipe, item by item:**
- 2 LODs;
- 8 actor labels;
- 4 target mappings;
- no anti-aliasing setting.

**What else held:**
- **No upload of any kind:** both privacy switches were in both argvs, re-checked against 4.25 source.
- **UBT:** the only UnrealBuildTool child was `ValidatePlatforms`, and no build tool ran.
- **The canary:** 0 hits, with the planted control found 3 of 3.
- **Protected state:** P9 v2 entry and exit proofs passed, `m51` equal to P9 four times, and 31124 unchanged.
- **Charged growth:** 0 on C:, E: and the 5.1 engine.

**This closes A6.** The first real outputs of the port exist.

## Rulings

### FG1. A6 is complete
- **The A6 outputs are the three packages above.** A7 (094-27) uses them as authored.
- **The 094-26c packages** stay in quarantine as evidence.

### FG2. Crash-client config ini files are engine-managed (Code's G1)
- **Recorded, never a STOP:** `Saved\Config\CrashReportClient\UE4CC-*\*.ini`, including the engine's two-day purge (FC5).
- **Still a STOP:** every other `*.ini` under `Saved\Config\` (FF2).
- **When it applies:** from the next armed kit.
- **094-27 is already running** with FF2 as written. No crash-config folder on the host is old enough to be purged during it. If it trips on one anyway, that is a false STOP to rule as foreign.

### FG3. A new crash or ensure report folder is a STOP (Code's G2)
- **The STOP:** a new folder under the host's `Saved\Crashes`, or under the engine's or `%LOCALAPPDATA%` crash folders, means a crash or an ensure report. It is a STOP, even though FF2 records everything else under `Saved\`.
- **When it applies:** from the next armed kit. 094-27's review checks for such folders by hand.

### FG4. Observations
- **O1–O4:** accepted.
- **O5:** nobody read back the saved values. That is A7's job: 094-27 loads both maps, and its readers check the authored state.

## For the owner
**Nothing to do.**
- 094-27 is building its kit now, which doesn't need your PC free.
- Its runs start whenever the PC has been idle for 10 minutes.
