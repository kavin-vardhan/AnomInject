# REPORT for 095-03b-e-drive-second-pass-rebuild-baseline.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-03b-e-drive-second-pass-rebuild-baseline.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-11-13 13:10:43 | finished: 2026-11-13 14:45:09 | duration: 94 min | exit code: 0
context at last turn: 566706 tokens (57% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-03b-e-drive-second-pass-rebuild-baseline.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

1. **For the owner (plain words)**
   - I permanently deleted 62.4 GiB of old Europa test frames and masks, plus large leftovers in the outline fixture's Receipts folder. E: went from 30.7 to 93.2 GiB free. The Europa package you use was checked before and after: all 34 files match their hashes, and it starts and quits cleanly.
   - The editor half of the test game rebuilt fine. I stopped the game half at step 392 of 764. Your Blender (about 21 GB), ComfyUI and Resolve were using nearly all the memory: it fell to 0–0.4 GiB free, and the build slowed to 3 steps in 9 minutes. It was also slowing your own work.
   - No baseline capture ran, so there are still no skipped-frame numbers. The legs need at least 6 GiB of free memory and a free GPU; the GPU was 38% busy.

2. **Part A**

   | Scope | Before (GiB) | After (GiB) |
   |---|---:|---:|
   | `0944*-evidence` (12 folders) | 31.13 | 3.01 |
   | `Outline425\Receipts` (8 job folders) | 34.40 | 0.16 |

   - Of what remains in `0944*-evidence`, 2.93 GiB is in `09445r-evidence`: the two picture folders `reader_tests09446.py` reads and a selftest folder, all kept whole.
   - **Delete manifest:** 280,945 files, 62.37 GiB, SHA-256 starts `21d89a25`. All were deleted, with 0 skips and 0 errors. Hardlink groups were all inside the delete set, so none was split. By type: PNG 47.3, `.zlib` 12.5, `.u32` 0.5 GiB, and in Receipts only, files over 1 MB (exe 1.75, jsonl 0.22, other 0.08 GiB).
   - **Kept:**
     - 23 folders read by test scripts or named as selftest or test folders: 15,150 files, 2.90 GiB.
     - 7,181 other files of 1 MB or less.
     - No card, GR6, GR6b or owner-kit reference points into either scope (scanned 2,276 scripts and 1,392 docs).
   - **Kept, unsure:** 31 files (33 MB) that a report, journal or finished per-job script names by path. They include `CrashReportClientEditor.exe` (18.5 MB), `UnrealBuildTool.exe` and `P3-coverage.json`.
   - **Checks afterwards:** all 22,362 kept files are present and unchanged, and no deleted file remains. No folder became empty; the 56 empty folders I found all date from Nov 3–4 and were left alone.
   - **Free space:** E: 30.70 → 93.24 GiB. C: about 20.1 GiB. D: swings between 15 and 43 GiB with the page file.

3. **Build, host and D:**
   - **The gate held at check time.** E: 93.24 − 14.54 − 1 = 77.7, above 50. D: read 37.4–37.7 for three samples, with one dip to 15.06.
   - **Editor target:** exit 0, 29 actions, 0 errors, 2 warnings. Both warnings are in the host's own `MidReproActor.cpp`; the plugin has none. Hashes:

     | File | SHA-256 (first 8) |
     |---|---|
     | AnomalyInjector DLL | 13DCDFFA |
     | AnomalyCapture DLL | 89018314 |
     | AnomalyControlServer DLL | DF269FB8 |
     | AnomalyShaders DLL | 79B7E535 |
     | AnomalyBench DLL | 0491AAB9 |
     | UnrealEditor-StackOBot DLL | 9612C9D6 |

   - **Game target:** 764 actions, 0 errors and 0 warnings up to step 392, then stopped by me by PID (UBT and its compilers only; no link was running). `StackOBot.exe` is unchanged at `002805CF` from 24 Sep, so the game exe still does not match `master`. A rebuild resumes from step 392.
   - **Host and fixture differences:** the release proof host `_r53_host` is gone.
     - Its `Content` was a junction to this host's `Content`.
     - Its recorded `DefaultGame.ini` hash `3CF789B92FDE` equals this host's today.
     - CaptureBench here is `e64d6f9`, clean apart from untracked tools. That is its last commit and predates the proof, but the proof host's CaptureBench revision is not recorded anywhere that survives.
     - This host also carries unreal-mcp and RoomGenerator; whether the proof host did is unknown.
     - Nothing needed aligning.
   - **What landed on D:** +0.06 GiB measured (the plugin's editor intermediates), against an estimate of about 1.1. E: took +7.9 GiB.

4. **Baseline:** not run. `predictions-095-03b.txt` was written before any launch. The leg runner (`run_leg95b.ps1`) and readings script are ready. They run the legs on the standalone editor with `-game`, because `Builds\BenchGate` is empty and no cook is allowed.

5. **Safety and decisions**
   - **Safety:**
     - During the build the D: page file grew to as much as 73.7 GB, and D: was below 30 GiB in 120 of 213 samples (minimum 15.0). That came from the owner's apps, not our writes.
     - No machine setting was changed and none of the owner's processes was touched.
     - No docs commit was made: `master` is still `ebc34ed`, the same as origin, and `m51` is still `53bf725`.
   - **Decisions chat must make:**
     - **(a) When to finish the Game build and legs.** They need a window with about 6 GiB or more of free memory and the GPU idle, which means Blender, ComfyUI and Resolve closed or quiet. A re-issue should be short: the Game build resumes from step 392, then B1 and B2, then the docs commit. E: no longer needs any cleanup.
     - **(b) The D: page file.** It moves 20–30 GB within minutes, so a D: floor checked once at the gate means little while the owner's apps are running. Only the owner can change the page file setting.
     - **(c) The 31 kept-unsure files (33 MB)** can be deleted later if chat wants; they are not worth a separate brief.

Everything is in `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-03b\`; open `exit-receipt-095-03b.txt` first.
api-equivalent cost (not billed on Max): $14.77 | turns: 92 | running total across all briefs: $5,988.16
