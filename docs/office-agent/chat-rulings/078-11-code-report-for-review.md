# REPORT for 078-11-correct-pairing-prediction-draft.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-11-correct-pairing-prediction-draft.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 20:02:02 | finished: 2026-09-06 20:11:43 | duration: 10 min | exit code: 0
context at last turn: 509458 tokens (51% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-11-correct-pairing-prediction-draft.live.txt

---- FINAL MESSAGE (Claude Code) ----
All five corrections applied and committed. Nothing ran.

## Corrections, each mapped to its evidence

**1 — Attribution (`§9` superseded).** The claim *"if A also fails, the failure is not attributable to the candidate binary"* overclaimed and contradicted its own next sentence. Now: both-fail ⇒ **the failure occurs without `F1`, which neither isolates nor excludes an `F1` contribution**; control-passes ⇒ **consistent with**, time/state confounded. Neither branch is causal proof.

**2 — Historical receipts (`§2` superseded).** My "no recorded receipt" claim was **wrong**. Verified and hashed:

| banked `_tryN\_leg_game.log` | bytes | SHA-256 |
|---|---|---|
| `PB1F_F_PAIR_NAT_try1` | 564,449 | `A5960C07…F79B3` |
| `PB1F_F_PAIR_SYN_try1` | 554,349 | `DF914342…0AFE78` |
| `PB1F_F_PAIR_NAT_SP50_try1` | 585,903 | `87D80176…899C411` |
| `PB1F_F_PAIR_SYN_SP50_try1` | 564,499 | `7015B0B9…6ECF72F` |

**Line 1533 of all four reads `CaptureBench.Marker = "1"`**, with AE echoes at 1529–1532 and the **full launch command at lines 17 and 207** containing `CaptureBench.Marker 1`. `_leg_geometry.json` blindness stands — but blindness ≠ absence. Kept explicit: `MaxAttempts` is invocation-script evidence, not a runtime field; these logs establish **no** historical tool hashes; and the **accepted alias carries no `_leg_game.log`** (measured: `PB1F_F_PAIR_NAT` holds only the session dir + geometry, because `run_leg.ps1:491-494` copies the session, not the `_tryN` bank).

**3 — Read-back specificity (added to `§7`).** Identified as the `Capture(mask): M23 PASS` line: SP50 banked at `internalViewRect=(0,0)-(640,360)` vs `unscaledViewRect=1280x720` with `r.ScreenPercentage = "50"` at line 1542; **100 % read from the same rendered geometry** because no explicit `100` echo exists (only unrelated `r.TSR.History.ScreenPercentage` / `r.Bloom.ScreenPercentage`) — demanding one would be impossible. Added native artifact preconditions (40 PNGs, 40 rows, si 0..39, no duplicate/missing/zero-byte/orphan) explicitly **distinct from allowed mask no-data**, and a **runnable** exactly-one-session checker invocation, trialled read-only against `M51D_HE_RESIDENT_NAT` and reproducing its banked reading exactly.

**4 — Runner limits and honest retention.** `-ExtraExecCmds` is appended at `:224` **after** Fps/Config at `:219-220`, so override **is** possible — these nine commands simply don't. Retention corrected with the measured table: `no run_summary` (`:322-324`) and `rode focus timeout` (`:371-373`) `continue` **before** the `:408-418` bank copy ⇒ **no bank, no `_leg_game.log`**; pose (`:460-463`) and rotation (`:466-469`) discards **are** banked. Raw `<Label>_try<N>` dirs persist on unique paths and get preserved after the runner, with **any missing receipt enumerated by name**; the packaged log rotates on next launch. **No harness change released.** `§9`'s falsifier 3 no longer folds the three stop classes together — **REFUSED / INVALID-UNDECIDABLE / INVALID-(<3 events)** are restored as distinct, none releasing a control. Exit 3 is **unexpected, not impossible**; an actual refusal is always honoured.

**5 — Time.** "three hours earlier" removed; the A-side passes are **19:07:43** and **19:19:36**, no relative duration.

## Retained unchanged

Nine capture commands and their fixed order · `MaxAttempts 4` pose-invalidity budget · valid failures **never** retried · **at most ONE** conditional A control then STOP · predicate `PREVIOUS == 0 AND NEITHER == 0` · tolerances · even-frame blind spot · FPS/`speed_ratio` as telemetry only · every hold in `§10`.

## Commit and receipts

**`770386f`** — 4 files, 316 insertions / 2 deletions (both trailing-newline artifacts, verified re-added verbatim). `git diff --check` **CLEAN**.

| file | bytes | SHA-256 |
|---|---|---|
| `docs/predictions/2026-09-06-m51-frozen-pairing-recheck.md` | 33,342 | `FD398AE1CA496D706119C1DE3CBB0D8CAC9574443B2FF39BD009374B2EDB5675` |
| `docs/sessions/2026-09-06-078-10-11-m51-frozen-pairing-draft.md` | 4,088 | `31DA394F2BCFBF55EBFC6C291AF18C966C92341C371D9DDB13D2845F5BED1D74` |
| `docs/OPERATING-CONTRACT.md` | 35,211 | `1093FECD59E9A8CAAAB75664A49DA4EC7C7A742FCEFDCCDD26FE8CE62A36EAC1` |
| `CLAUDE.md` | 555,257 | `EECE136867542C35023EC3D682A0DC86670412524AA236117278AD444467D016` |

**CLAUDE.md was spliced byte-surgically**, not rewritten: no BOM, **CRLF 5886 → 5938 with zero bare LF**, non-ASCII 5681 → 5725 (additions only), **52 insertions / 0 deletions**, new block at line 14 and the 078-01→05 block intact at line 66. Commit used `--file` with an absolute message path; path-scoped adds only; no blanket add, amend, reset, stash, clean, push, merge or tag.

## Final state

`HEAD` `770386fe27040bef33fe9295890edfa78c8873bd` on `m51`; `master` == `origin/master` == `ac13700`; **`origin/m51` absent — nothing pushed**; no tags. `6713f51..HEAD -- Source Shaders` **EMPTY**; 10 handoffs preserved; nothing unstaged; only the four expected files changed. Staged runtime still `D50DDE78`, container all five unchanged, CaptureBench `472a409` tracked-clean, Lyra `4a4bfcc` clean. Zero `StackOBot` processes; **no background task is running.** The owner's editor is up and was not touched. No staging, build, cook, run, source, harness or tool change occurred.

Context: roughly **50–55 % of 1M** — below the 70 % rollover boundary.

**VERDICT: NEEDS-DECISION**
api-equivalent cost (not billed on Max): $6.10 | turns: 20 | running total across all briefs: $1,233.38
