# REPORT for 068-06-m40-implement-and-gate.md
brief: 068-06-m40-implement-and-gate.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: medium
started: 2026-09-02 20:21:29 | finished: 2026-09-02 20:40:26 | duration: 19 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\068-06-m40-implement-and-gate.live.txt

---- FINAL MESSAGE (Claude Code) ----
**1. Status.** `m40` is implemented, gated and pushed. **Commit `0864e7a`** (7 files, 321 insertions / 15 deletions; the only source deletions are the moved `SampleDeferredActiveState()` call and the extracted dispatch loop), pushed to `origin/master`, **no tag**. **Staged bench exe `C0AD3F91`** (241,069,056 B, built == staged); **container quartet UNCHANGED** (`utoc 2A66CA57` · `ucas A7EF9B12` · `pak D8009AD7`) — code-only hot-swap, no cook (`G103`). Predecessor **`F2FA6BCD`** archived and recorded as m40's load-bearing A-side. Comment stripper: 0 changed / 93 no-change.

**2. The four legs.** Reader `p9_hidden_set.py` unmodified, `SEP_RATIO 5.0` untouched; config `2 4 8 4 0`, 90 frames, seed 4242, targeted `blinking` on `StaticMeshActor_49`, paced 30 fps, census OFF, `Log LogAnomaly Verbose`. Every attempt banked incl. two `A63` pose discards.

| leg | binary | lever | predicted | measured | Δ | |
|---|---|---|---|---|---|---|
| **L1** control | `F2FA6BCD` | OFF | all counted ALIGNED, exit 0, `{n,n+1,n+5,n+6}` | **7/7 ALIGNED, exit 0** — `[4,5,9,10]`…`[76,77,81,82]` | **+2** ×8 | ✅ **PASS** |
| **L2** reproduction | `DC16710D` ⚠ | **ON** | all counted P9-SHAPE, `k=0`, residual both directions, exit 1 | **7/7 P9-SHAPE, exit 1**, `k=+0`, claimed `{n,n+1,n+5,n+6}` vs observed `{n,n+1,n+2,n+6}`, **missing `[n+5]` extra `[n+2]`** on every event | **+3** ×8 | ✅ **PASS** |
| **L3** the fix | `C0AD3F91` | **ON** | all counted ALIGNED, exit 0, `{n,n+1,n+2,n+6}` | **7/7 ALIGNED, exit 0** — `[4,5,6,10]`…`[76,77,78,82]` | **+3** ×8 | ✅ **PASS** |
| **L4** inertness | `C0AD3F91` | OFF | all counted ALIGNED, byte-identical to L1 | **7/7 ALIGNED, exit 0**; `frame_indices` **identical**, `labels.jsonl` **0 row diffs** (sorted by `session_index`, `G162`) | **+2** ×8 | ✅ **PASS** |

🎯 **L3 is the load-bearing one: the lever was still ON and the toggle lines still read `+3` — the toggles did not move back — yet the labels followed the pixels.** The label sample is what changed. The pre-declared cap-truncated 8th event read `TRUNCATED` on all four legs (7 counted each, above the ≥3 floor); no cap change.

**3. Supporting gates — all pass.**

| gate | measured |
|---|---|
| `m20` pixel-vs-label, new exe | L4's 7/7 ALIGNED **is** that comparison ✅ |
| `P6` | annotation **48 = 48** flattened, identical; `run_summary` **37 = 37**, 0 added/removed ✅ |
| `P-C7` re-anchor | L1↔L4 census-OFF artifacts identical ✅ re-anchored to `C0AD3F91` |
| client-inert | no ini read added, no artifact field, lever console-only ✅ |
| `A44` both encodings | `SynthTickOrder` ascii 0 / **utf16 5**; `SYNTH TICK ORDER` ascii 0 / **utf16 1** ✅ |
| `G139` echo | L4 reads `= off (compiled default off)`; L2/L3 read `= ON` ✅ |
| `m38` run log | all four legs: close marker is the file's last line, one `FINISHED` each ✅ |
| `G103` | container quartet unchanged ✅ |

**4. `LEVELTICK_TimeOnly` — reading and choice.** `OnWorldTickEnd` is broadcast **unconditionally for every `TickType`** (`LevelTick.cpp:1814`), but **a tickable does not tick when `TickType == LEVELTICK_TimeOnly`** (`Tickable.cpp:143-148`) — so on such a tick neither subsystem runs, nothing is armed, and `bHasDeferredActive` is never set. The flag is set and consumed inside one `UWorld::Tick` call, so no cross-tick leakage exists. **Choice: no filter** — `SampleDeferredActiveState`'s own `if (!bHasDeferredActive) return;` already makes the handler a no-op on exactly the ticks a filter would exclude, which is the structural rather than conditional form, and it matches the `m26` mask handler. **L1↔L4 byte-identity is the proof, not the argument.**

**5. Failures: all pass.** Two things worth naming rather than burying:

- 🚨 **A gate-set gap was found and closed, not by a failing leg.** `run_leg.ps1` force-kills the process (`:293`, `A46`), so **`Deinitialize` — where both new delegate handles are removed — was untested by all four legs**, which is precisely the lifecycle risk the plan named. A fifth graceful-shutdown leg (`M40_TEARDOWN_GRACEFUL`, banked) closes it: capture finished, 30 PNGs written, then `Game engine shut down` → `Object subsystem successfully closed.` → `Exiting.`, **exit code 0**, and **zero** `Fatal error` / `Assertion failed` / `Ensure condition failed` / `Critical error` lines.
- ⚠ **L2 was not executable as the predictions file specified it.** `F2FA6BCD` has no lever — the lever is m40 code. L2's substance ("lever ON against the *shipped sampler position*") was preserved by building an ephemeral **lever-only intermediate `DC16710D`** (lever present, `SampleDeferredActiveState()` still at the top of `Tick`), running L2 on it, then moving the call and building `C0AD3F91` for L3/L4. **The closed predictions file was not edited**; the correction rides journal 068 §11.1 (`P-C2` route), and `DC16710D` is recorded in `_binary_baselines\README.md` so the L2 result can be attributed to a build.

**6. NEEDS-DECISION — one.** **Ratify the L2 deviation.** It departs from the letter of a committed predictions file (binary identity), was forced (the leg is otherwise unexecutable), and I executed it rather than stopping because stopping would have made Route A unsatisfiable — but it is a departure from a pre-declaration and that is yours to bless. If you would rather it were re-run differently, say so; the intermediate is reconstructible by moving one call. Everything else: none.
api-equivalent cost (not billed on Max): $26.61 | turns: 69 | running total across all briefs: $68.27
