# REPORT for 095-09b-m59-skeletal-switch.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-09b-m59-skeletal-switch.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-11-14 09:21:59 | finished: 2026-11-14 09:45:10 | duration: 23 min | exit code: 0
context at last turn: 926808 tokens (93% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-09b-m59-skeletal-switch.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**For the owner**
- `lod_corruption` and `lod_popping` now skip characters (skeletal meshes) by default, because their first and last labelled frames could be one frame off the picture and nobody has measured it yet.
- To get the old behaviour back on a host without rebuilding, type `IAI.Anomaly.LodAllowSkeletal 1` or set `LodAllowSkeletal=True` in the ini. The log then warns on every such event that its edge frames are unmeasured.

**Commits and branch tips**
- **`fix/m59-forced-lod-refusals` = `af1b08f`, pushed.**
  - `ce80eaa` is the code.
  - `af1b08f` is the docs: readme limitation bullet plus a settings table, architecture rows, G579 extended, journal §8, status block.
- **`integ/m56-m58` = `876c719`, pushed.**
  - `fba2820` is a true merge of `af1b08f`. One conflict, in the capture run banner line: I kept integ's line and added `lodSkeletal=`.
  - `876c719` is a status note only.
- **Checkout:** left on `integ/m56-m58`, no tracked changes. The plugin Editor DLLs were built from `fba2820`'s source.
- **What the code does:**
  - The pure rule checks hierarchical-instanced first, then skeletal, then single-LOD. The new reason is `skeletal_unmeasured`.
  - The switch is a console command plus an ini key in the style of the other LOD settings, default 0.
  - It is read once when a capture starts and held until it finishes, so a change made during a capture applies to the next one; the change message says so.
  - The run banner, which is also the run-config echo, shows `lodSkeletal=refused(compiled)` or `allowed(console)`, and a start-of-run log line repeats the value with its source.
  - With the switch on, behaviour is exactly the 095-09 behaviour, plus one warning per accepted skeletal component that its edge frames may be one frame off and that this is unmeasured.

**Compile and tests (same on both branches)**
- **Editor target**, Development Win64, 2 parallel actions, launched with about 10.4 GiB free: exit 0, 0 errors, 0 warnings. The new strings are in the built DLLs.
- **LOD selftest:** 63 checks, 0 failures. The five mutants all fail:

  | Mutant | Checks failed |
  |---|---|
  | hierarchical-instanced rule removed | 18 |
  | single-LOD threshold 1 | 3 |
  | switch ignored | 4 |
  | skeletal rule removed | 9 |
  | tie between reasons | 2 |

- **Pure C++ selftests:** 15 of 15 on the fix branch, 18 of 18 on integ. The m58 terrain mutants still fail 5 of 5.
- **Python suites (fix branch / integ):**

  | Suite | Fix branch | Integ |
  |---|---|---|
  | label rule | 65 | 94 |
  | pixel gate | 98 | 101 |
  | change oracle | 35 | 35 |
  | office kit | 93 | 102 |
  | unit tests | 49 | 55 |

  The exclusion gate and the label-offset tool pass on both.
- **Lever audit:** source and binary PASS (53 levers on the fix branch, 57 on integ); the lever-audit selftest passes 12 of 12.
- **Pre-push scans:** PASS on both branches. Receipts are in `code-receipts-095-09b\`.

**Amended prediction** (`predictions-095-09.txt` AMENDMENT 1, leg L59-7, appended before any leg; new SHA-256 `88D0EE80…C220E6`)
- **Target:** a targeted `lod_corruption` on the Bot in MainWorld (actor `BP_Bot_C_0`). It has:
  - two skeletal components: SKM_Bot (single-LOD) and SKM_Backpack (LOD count unknown);
  - one static component. The Bot blueprint references no static mesh asset, so I infer it has none; that is an offline reading, not observed.
- **Switch at 0:** both skeletal components are refused `skeletal_unmeasured` and the static one `single_lod`. Every component is refused, the dominant reason is `skeletal_unmeasured`, and no event is written. If the static component turns out to hold a multi-LOD mesh, an event is written on it alone; that disproves my inference, not the skeletal refusal.
- **Switch at 1:** SKM_Bot is refused `single_lod`. SKM_Backpack is refused `single_lod` if it has one LOD; otherwise it fires with the warning.
- **What the leg can prove:** that the default refusal and the switch both reach the run, because the reason flips from `skeletal_unmeasured` to `single_lod`. I put the skeletal check before the single-LOD check so that this flip shows up even on the single-LOD Bot. The switch-on run also reveals SKM_Backpack's LOD count.
- **What it cannot prove:** the one-frame lag itself. That needs a multi-LOD skeletal fixture.

**Safety**
- No Game build, launch, cook or Codex. No owner processes touched and nothing killed. No force, clean, reset, stash, squash or rebase.
- `master` (`ebc34ed`), `m51` (`53bf725`), the frozen branches, tags and the dashboard repo are all untouched.
- One slip, caught before any commit: my first resolution of the merge conflict re-encoded the capture source file. The diff stat showed it, and I restored the conflict and resolved it again byte-safely. The committed diff is exactly the 5 intended lines.
- E: has 16.58 GiB free, D: 63.27 GiB.

**Decisions for chat:** none.
api-equivalent cost (not billed on Max): $42.09 | turns: 90 | running total across all briefs: $6,265.28
