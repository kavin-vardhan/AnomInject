# REPORT for 095-03-e-drive-cleanup-rebuild-baseline.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-03-e-drive-cleanup-rebuild-baseline.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-11-13 12:51:01 | finished: 2026-11-13 13:02:52 | duration: 12 min | exit code: 0
context at last turn: 485956 tokens (49% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-03-e-drive-cleanup-rebuild-baseline.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**1. For the owner (plain words)**
- I deleted 257 large files, mostly old Europa game builds, debug symbols and big evidence files. That freed **10.9 GiB on E:** (21.2 → 31.0 GiB free). The Europa package you use was checked afterwards and is intact: all 34 files match their hashes, and it starts and quits cleanly.
- The test game was **not rebuilt**. E: needs about 65 GiB free before the build can run, and it has 31. Most of the old evidence, about 31 GiB, is small picture files under 1 MB each, and the brief's "keep every file of 1 MB or less" rule protects them.
- No baseline capture was run, so there are still no frame-skip numbers.

**2. Part A**

Freed space per folder (GiB, before → after):

| Folder | Before | After |
|---|---:|---:|
| 09444e-evidence | 1.055 | 0.437 |
| 09444f-evidence | 0.003 | 0.003 |
| 09445-evidence | 2.587 | 1.345 |
| 09445r-evidence | 13.316 | 12.064 |
| 09446-evidence | 5.925 | 5.287 |
| 09446r-evidence | 9.097 | 8.448 |
| 09446s-evidence | 3.516 | 3.504 |
| 09447-evidence | 0.907 | 0.003 |
| 09447t-evidence | 0.975 | 0.005 |
| 09447t-package | 2.207 | 0.003 |
| 09447u-evidence | 0.391 | 0.001 |
| 09447u-package | 2.207 | 2.207 (kept: card rule; hardlinks to the current package) |
| 09448-evidence | 0.391 | 0.000 |
| 09448-package | 4.414 | 4.154 (66 of 68 files are hardlinks to the current package) |
| 09448b-evidence | 1.018 | 0.036 |
| 09448b-package | 2.207 | 2.077 (33 of 34 files are hardlinks to the current package) |
| 09448c-evidence | 0.347 | 0.000 |
| 09448c-package | 2.207 | 2.207 (the current package) |

The table sums each hardlink separately. Space actually freed was 10.95 GiB.

Free space:

| Drive | Before | After |
|---|---:|---:|
| E: | 21.20 | 31.01 |
| C: | ~20.1 | 20.11 |
| D: | ~37 | 36.97 |

Manifests:
- **Inventory:** 171,665 files.
- **Deleted:** 257 files, 0 errors. 243 went in the first pass. The other 14 were skipped there because their hardlink partner had just been deleted; I re-checked each one by file ID, size and last-write time, then deleted it. 54 empty folders were removed. Manifest SHA-256 starts `049cdf09`.
- **Kept, over 1 MB:**
  - the current package (34 files) and 132 hardlinks to it;
  - the `09447u-package` exe, because the 094-47 card names that package;
  - 2 `labels.jsonl` test fixtures, in folders that `reader_tests09446.py` and the selftests read.
- **Kept, 1 MB or less:** 171,239 files, 31.13 GiB. That is PNG 25.88, `.zlib` 4.86, `.u32` 0.28.
- **Checks afterwards:** all 171,408 kept files are present with the same size and timestamp; none of the deleted files remain. Nothing was running from these folders before the delete.
- **Tool references:** I scanned 2,079 scripts and configs. Every path into these folders comes from a finished per-job script. Only the folders read by test scripts were kept, as 094-49 did.

**Kept, unsure:** none.

**Largest other folders on E:** (read-only; nothing deleted):

| Folder | GiB | What it is |
|---|---:|---|
| `E:\<personal-folder>` | 160.9 | personal video and photos |
| `E:\<personal-folder>` | 89.7 | the owner's other home projects |
| `E:\AI` | 83.7 | ComfyUI install |
| `E:\Saily` | 64.2 | personal media |
| `E:\EpicGames` | 60.6 | the UE 4.25 and 5.7 installs |
| `E:\Unreal Projects` | 57.1 | <other-home-project> 30.8, Patreon 14.0, StackOBot 4.8 (looks like the retired 5.4 host), StylizedParisStreet 4.1 (protected) |
| `E:\SailyIphone` | 40.7 | phone photos |
| `E:\IA_Fixtures\Outline425` | 37.7 | Europa outline fixture; its `Receipts` folder is 34.4 |
| `E:\CodexData` | 36.4 | Codex home; sessions 21.6 |
| `E:\UnityProjects` | 31.9 | <other-home-project> Unity project |

The 0944* folders still hold 41.8 GiB after this cleanup.

**3. Build:** not run, because the gate failed. Gate arithmetic: 31.01 GiB free, minus 14.54 for the rebuild (the measured figure from runbook 8.6 / G130, treated as GiB), minus about 1 for the B1 and B2 sessions, leaves about 15.5. The floor is 50. E: would need about 65.5 GiB free. I did not compare the host and fixture with the release proof; that is Part B.

**4. Baseline:** not run. No predictions were written and no legs launched.

**5. Safety:**
- Nothing was killed, no process was running from the scope, and no git write was made. The primary checkout is still `master` = `ebc34ed`, the same as origin, with the same 15 untracked files.
- I left the following untouched: the primary host's E: build folders (`E:\IA_BuildCache\StackOBot`), `_binary_baselines`, `_bench_sessions_bank`, `D:\IA_Europa`, and every never-touch item in the brief. The J2 folders `_r53_src`, `_r53_host`, `_r52p_src` and `_r933` do not exist on E:.
- **Finding:** D: is below its 50 GiB floor at 36.97 (it was 51.36 at 095-01). The cause is the Windows page file: `D:\pagefile.sys` grew to 34.57 GiB at 12:44. It is not project data, and I did not touch it.

**Decisions chat must make:**
- **(a) How E: reaches about 65.5 GiB.** The options:
  - Lift KEEP rule 3 for frames and masks (`.png`, `.zlib`, `.u32`) inside `0944*-evidence`. That adds about 31.1 GiB and gives about 62 GiB, still about 3.5 short.
  - The outline fixture's `E:\IA_Fixtures\Outline425\Receipts` (34.4 GiB).
  - Owner-side space, since most of E: is personal media.
  - A lower E: floor for this one build. With today's 31 GiB the build would leave about 15.5.
- **(b) D: below its floor because of the page file.** Does D:'s 50 GiB floor apply to this build? The page file is a machine setting, so only the owner can change it.
- **(c) Release `09447u-package`?** I kept it only because the 094-47 card names it. It is 33 hardlinks to the current package plus one 133 MiB exe, so deleting it frees only about 0.13 GiB.
- **(d) Re-issue Part B** as written once (a) and (b) are settled.

Receipts are in `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-03\`; open `exit-receipt-095-03.txt` first.
api-equivalent cost (not billed on Max): $7.92 | turns: 39 | running total across all briefs: $5,973.39
