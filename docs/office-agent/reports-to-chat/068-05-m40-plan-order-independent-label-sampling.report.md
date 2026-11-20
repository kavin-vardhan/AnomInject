# REPORT for 068-05-m40-plan-order-independent-label-sampling.md
brief: 068-05-m40-plan-order-independent-label-sampling.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: high
started: 2026-09-02 20:06:50 | finished: 2026-09-02 20:18:03 | duration: 11 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\068-05-m40-plan-order-independent-label-sampling.live.txt

---- FINAL MESSAGE (Claude Code) ----
**1. Status.** `m40` is planned and pre-declared; nothing is built. Journal 068 gains **§10** (file-by-file, three traps, lifecycle, out-of-scope), the pre-declaration lands at `docs/predictions/2026-09-02-m40-order-independent-label-sampling.md` **before any m40 source exists**, ledger §8.6a gains the **FIX APPROVED** block with the sync-fallback limitation and its detection route, and CLAUDE.md's milestone map gains `m40` plus R3's "numbers are identities, not an order". Commit **`2c73e70`**, pushed, 4 files, 490 insertions / **1** deletion (the office-batch line I replaced). ⛔ No source edit, no build, no leg, no cook, no tag. **One deviation from R1 — see §5.**

**2. File-by-file.**

| # | file | site | change |
|---|---|---|---|
| 1 | `AnomalyCapture/Public/AnomalyCaptureSubsystem.h` | private, beside `MaskWorldTickEndHandle` | +1 handle `SampleWorldTickEndHandle`, +1 method `OnWorldTickEndSample(UWorld*, ELevelTick, float)` |
| 2 | `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | `Initialize`, beside **`:334`** | register a **second, independent** `OnWorldTickEnd` handle via `AddUObject` |
| 3 | ″ | `Deinitialize`, beside **`:527`** | symmetric `Remove` |
| 4 | ″ | **`Tick` `:591`** | **REMOVE** the `SampleDeferredActiveState()` call — this is the behavioural change |
| 5 | ″ | new handler beside **`:704`** | `if (World != GetWorld() \|\| !bRunning) return; SampleDeferredActiveState();` — guard mirrors **`:707`**. No logging |
| 6 | ″ | `SampleDeferredActiveState` **`:2717-2759`** | **body unchanged** — only its call site moves |
| 7 | ″ | `FinishRun` **`:2986`** | keep, now a no-op safety net |
| 8 | ″ | `FinalizeArmedLabel` **`:2519-2562`** | unchanged; still sets the flag at `:2560-2561` |
| 9 | ″ | `StartRun`, beside the m28/m36/m38 echoes | +1 **unconditional** lever echo. Log only, no `run_summary` field |
| 10 | `AnomalyInjector/Public/AnomalyInjectorSubsystem.h` | | +`bSynthTickOrder`, +`static IsSynthTickOrderEnabled(UWorld*)` (the `:43` pattern), +`DispatchAnomalyTicks(float)` |
| 11 | `AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp` | `Tick` **`:193-199`** | extract the dispatch loop verbatim; `Tick` calls it only when the lever is OFF |
| 12 | ″ | new `OnWorldPreActorTick` handler + handle | when lever ON and `World == GetWorld()`, drives the dispatch; returns immediately when OFF |
| 13 | ″ | beside `IAI.SetViewportScoping` **`:687-700`** | +1 `FAutoConsoleCommandWithWorldAndArgs` for the lever, with the loud set-time echo |

Consumption path unchanged: `FireActive` (`:2757`) → `AccumulateFrameEvents` (`:2179-2180`) → `ActiveByIndex` (`:3432`) → `frame_indices`. `labels.jsonl`'s `anomaly_present` comes from `Snap->Fires` and is untouched.

**3. The four legs** (reader `p9_hidden_set.py`, `SEP_RATIO 5.0` frozen; config `2 4 8 4 0`, 90 frames, seed 4242, targeted blinking, `RunLogVerbose 1`; validity floor ≥3 counted events; the cap-truncated 8th event is **predicted** UNDECIDABLE/uncounted).

| leg | binary | lever | predicted reader | predicted sets | Δ apply→toggle |
|---|---|---|---|---|---|
| **L1** control | `F2FA6BCD` | OFF | all counted **ALIGNED**, exit 0 | `{n,n+1,n+5,n+6}` both | **+2**, first toggle uncaptured |
| **L2** 🎯 reproduction | `F2FA6BCD` | **ON** | all counted **P9-SHAPE**, `best_k=0`, residual `{n+2, n+5}` both directions, exit 1 | claimed `{n,n+1,n+5,n+6}` · observed `{n,n+1,n+2,n+6}` | **+3**, first toggle **on `frame_index(n)`** |
| **L3** the fix | post-fix | **ON** | all counted **ALIGNED**, exit 0 | `{n,n+1,n+2,n+6}` both | **+3, unchanged** |
| **L4** inertness | post-fix | OFF | all counted **ALIGNED**, exit 0 | `{n,n+1,n+5,n+6}`, **byte-identical to L1** (sorted by `session_index`, `G162`) | **+2** |

L2 is the reader's own `--d-unit` fixture (ii) "Bates pair" (`p9_hidden_set.py:354`), so the branch is known-answer tested on literal sets. **L3's Δ staying +3 is what makes it a proof:** the lever is still on, the toggles have not moved back, so the *label sample* is what changed. 🛑 **If L2 does not reproduce, the lever is wrong, not the theory — stop.** Supporting gates: `m20` pixel-vs-label re-run, `P6` 48/48, `P-C7` re-anchored on L4, client-inert, `A44` both encodings, `G103` no cook.

**4. The lever.** `IAI.Bench.SynthTickOrder <0|1>`, compiled default **OFF**, **console only — no ini key, no dashboard command**. When ON, `UAnomalyInjectorSubsystem`'s anomaly-dispatch loop is skipped in `Tick` and driven instead from `FWorldDelegates::OnWorldPreActorTick` (`LevelTick.cpp:1468`, before `TickObjects` at `:1606`), putting the injector genuinely ahead of the capture subsystem every tick. That reproduces **both** consequences at once — the apply tick goes uncounted (`Δ=+3`, since `Apply` runs later inside the capture `Tick`) **and** the shipped sampler now sits after that tick's dispatch — giving pixels `{n,n+1,n+2,n+6}` against labels `{n,n+1,n+5,n+6}`: **all four Bates numbers including the revert boundary.** Echoes, both unconditional:

*Set-time:* `IAI.Bench.SynthTickOrder -> ON. BENCH-ONLY SYNTHESIS. The injector's anomaly dispatch now runs at OnWorldPreActorTick, i.e. BEFORE the capture subsystem, for every world tick. This SYNTHESISES THE SYMPTOM of a host on which the two subsystems tick in the opposite relative order; it does NOT reproduce that host's cause and is NOT evidence about it. Labels and pixels will disagree by design. NEVER ship a capture taken with this ON.`

*Every `StartRun`, on or off:* `Capture(bench): m40 SYNTH TICK ORDER = off (compiled default). When ON, the injector's anomaly dispatch is relocated to OnWorldPreActorTick to synthesise the SYMPTOM of a reversed subsystem tick order. It is a bench device: console-only, no ini key, never in a client payload. A session captured with it ON has labels that deliberately disagree with its pixels.` — the **`off`** line is the load-bearing one (`G139`).

**5. NEEDS-DECISION**

1. 🔻 **RATIFY OR REVERT — R1's suggested lever mechanism was refuted before any code, and I substituted one.** *"The injector does not count the tick on which `Apply` ran"*, worked through the tick reconstruction, moves the **pixels and the labels by the same one tick**, so both sides of the comparison shift together and the reader reads **ALIGNED** — it reproduces nothing. (The Δ is a *co-symptom* of the reordering, not its label-side cause.) I adopted the `OnWorldPreActorTick` relocation instead, which is still a symptom synthesis and is still bench-only/default-OFF/console-only per R1's other constraints. **Pre-registered here in the `m33` AMENDMENT-1 shape: ratify, or tell me to revert to A and accept that L2 cannot reproduce.**
2. **Implementation detail deliberately left open and named:** whether `OnWorldTickEndSample` should ignore `LEVELTICK_TimeOnly`. The existing mask handler does not filter on `TickType`. To be settled **by reading, not guessing**, during implementation, with the answer journaled.
3. **The truncated 8th event** is pre-declared as expected `UNDECIDABLE`/uncounted rather than a failure. If you want it counted, that needs a different frame cap and re-baselined banked controls — flagging rather than assuming.

Stopping here — implementation awaits your approval.
api-equivalent cost (not billed on Max): $10.58 | turns: 18 | running total across all briefs: $41.66
