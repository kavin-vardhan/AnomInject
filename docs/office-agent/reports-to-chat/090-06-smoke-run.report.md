# REPORT for 090-06-smoke-run.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 090-06-smoke-run.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-30 19:03:11 | finished: 2026-10-01 06:31:26 | duration: 688 min | exit code: 0
context at last turn: 386494 tokens (39% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\090-06-smoke-run.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Not idle: the 090-06 smoke did not run.** The PC was never idle long enough before 06:30 IST, so no leg started. I staged nothing that persisted, and the staged files are still the pre-run M53 pairing, verified by hash.

**1. Plain summary for the owner**
- **Does a label now stop when the effect stops?** Not measured tonight; no leg ran.
- **Are Nanite objects skipped, with every label masked?** Not measured tonight.
- **Is blurry-texture (stuck_low_mip) sync unchanged?** Not measured tonight. A foreign UE 5.7 editor was open almost all night (19:04–06:30 IST), so the idle gate never let a leg start.

**2. Per-leg table**
All 22 capture legs plus `F1_RETAKE_OLD_CANFAIL` read **NOT-RUN (not idle)**: 0 attempts, no onset/offset, no tool readings. The declared predictions in README §4 are untouched. Part 0 (P0) is **NOT-RUNNABLE**, as declared: there is no package with the 090-05 code, and `dbghelp.dll` can't be created on this box.

**3. Can-fail and F1's cost**
- The can-fail on OLD did not run, so it is **not proven tonight**.
- F1's cost was not measured: no `Capture(F1): EFFECT-INSTALLED CHECK` line exists, so there is no mean or max to compare with the 0.05 / 0.5 ms threshold.

**4. Nanite**
Not run. There are no `refused_nanite` values, no `reason=nanite` lines and no masked-labelled frame counts.

**5. Tool disagreements**
None, because nothing was evaluated.

**6. Pre-run checks** (harness `preflight`, live mode, 19:04 IST; repeated at 22:11)
- **Freeze:** OK.
- **Selftest:** 83/83.
- **NEW pin `97D292F8`:** matches its archive in both `E:\IA_BuildCache\_binary_baselines\m53-09005-97D292F8\` and `D:\IntrusiveAnomalies\_binary_baselines\m53-09005-97D292F8\` (242,480,128 B).
- **OLD pin `45FD344C`:** matches its archive (242,454,528 B).
- **A44 string scan:** NEW has every F1 and Nanite token; OLD has none of them.
- **Container and maps:** DC2 container OK; all five maps present.
- **Main checkout:** `m51` at `53bf725`.
- **Only problem:** the idle gate.

**7. Idle waits**
- I polled the harness's own `idle_gate()` once a minute (723 reads), never killing or touching anything. The only foreign processes seen were `UnrealEditor.exe` (`E:\EpicGames\UE_5.7`, three different PIDs over the night) and `blender.exe` (19–33 % GPU SM, in 95 reads). The PC was also in active use most of the night.
- **Finding (harness behaviour):** in live mode a non-idle preflight exits **10 immediately**. The 90-minute wait applies only per leg and at staging. That is why I polled before invoking `-Live`, instead of re-invoking it repeatedly.
- **Window 1 (22:11 IST, the only `-Live` call):**
  - The gate read idle twice (22:09–22:10), so I launched.
  - A new UE 5.7 editor (PID 31020) started within about 30 s. The harness stopped with `NOT-IDLE at staging NEW`, **exit 4**, before copying anything.
  - Its restore wrote `receipt-99-restore-prerun-FAILED.json`, because the restore is also idle-gated. Nothing needed restoring: the staged six still equal receipt-00, and **postflight PASS**.
  - After that I required three consecutive idle reads before launching again. That never happened again before the deadline.
- **Prompts closed:** none.
- **Disk** (no legs ran, so the drop is not ours):

| drive | 19:04 IST | 06:30 IST |
|---|---|---|
| C: | 32.8 GB | 33.9 GB |
| D: | 136.8 GB | 97.7 GB |
| E: | 105.8 GB | 75.5 GB |

- No `M90S06*` directory exists beside the exe or in `D:\IA_BankOverflow`, and no StackOBot process is running.

**8. Staging at the end** (unchanged from pre-run, set "M53")
| file | hash |
|---|---|
| `StackOBot.exe` | `2FCDF059` |
| `StackOBot-Windows.utoc` | `20DA6F98` |
| `StackOBot-Windows.ucas` | `534C5863` |
| `StackOBot-Windows.pak` | `FD766B7B` |
| `global.utoc` | `462B8AC6` |
| `global.ucas` | `BB05CF99` |

**9. Commits pushed**
None. With no leg run, the brief's stop rule applied, so I didn't write the journal or status block. No git writes were made; the checkout stays `m51` `53bf725`.

**10. NEEDS-DECISION**
1. **Rescheduling:** the smoke needs a window with no UE 5.7 editor and no blender. That means the owner closing them, or a night when they are already closed.
2. **Harness:** should a live preflight that fails only on the idle gate wait up to 90 min, as README §6 implies, instead of exiting 10 at once? It would be a declared fix iteration. I didn't make it, because the harness is frozen and nothing was sampled.
3. **Evidence directory:** `090-06-evidence\run\` now holds window 1's artifacts, including the `-FAILED` restore receipt that restored nothing. Should it be cleared before the next window, or kept and resumed?
api-equivalent cost (not billed on Max): $10.30 | turns: 95 | running total across all briefs: $4,057.32
