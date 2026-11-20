# REPORT for 068-01-kickoff-task1-source-read.md
brief: 068-01-kickoff-task1-source-read.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | effort: xhigh
started: 2026-09-02 17:45:16 | finished: 2026-09-02 18:07:42 | duration: 22 min | exit code: 
args: -p --output-format text --dangerously-skip-permissions --model claude-opus-5 --session-id <uuid> --effort xhigh

---- STDOUT (Claude Code final message) ----
Task 1 complete, committed and pushed. Here is the report.

---

# 1. STATE SUMMARY + VERIFICATION

**Verification: PASS.** `git rev-parse HEAD` == `git rev-parse origin/master` == **`be4dd1b`** at bootstrap; tree clean except the four untracked `docs/CHAT-HANDOFF-*.md`; no `.git/index.lock`. Nothing differed, so I proceeded.

1. `master` carries **m34 + m35 + m36 + m37**. Tip is now **`3e14385`** (this session's docs commit, pushed).
2. **Milestone map:** `m37` = census defaults **DONE** · `m38` = run-scoped session log **PLAN APPROVED, NOT IMPLEMENTED** · `m39` = honest bbox (`P-C13` c2 rides it).
3. **Staged bench exe `6C80E872`**; `D2BB25A5` still load-bearing as `P-C7`'s A-side, freeze retired. Container unchanged, no cook.
4. **NO TAGS** — highest `m30`; office batch `m31→m33→m34→m35→m36→m37` (+`m38` if landed).
5. **`P9` (A) BOUNDARY SMEAR = CLOSED**, temporal accumulation, by `C-1`'s pre-declared discriminator (menu AA/motion-blur off ⇒ partials vanished).
6. **`P9` (B) PHASE DISPLACEMENT = OPEN.** Clean binary, AA off: observed hidden **{n, n+1, n+2, n+6}** vs claimed **{n, n+1, n+5, n+6}**. Outer flips match; both interior flips +1 late in pixels; hidden count conserved 4-vs-4.
7. Every other axis excluded (AA both directions, census, mask, delivery, pacing, tick ratio, letterbox) ⇒ **localized to the Bates host build + content**. No mechanism claimed anywhere.
8. **TASK 1 existed because it was never executed** — the brief carrying it was never delivered (journal 067 §17.6, handoff §4). It is the layer below §15.1's enumeration and is what turns `C-3`'s three-way comparison into an attribution.
9. **TASK 1 is now DONE.** Docs-only; no source edit, no build, no leg, no `m38` work, no tag.
10. Bonus: the reconstruction was **validated against a banked bench artifact** before being written up — it predicts three facts it was not fitted to.

---

# 2. THE READ

## 1a — THE FOUR FLIPS

**First fact, because it changes the shape of the answer: `FAnomaly_Blinking::Apply` NEVER HIDES ANYTHING.** `Anomaly_Blinking.cpp:10-71` contains no `SetActorHiddenInGame` — it sets `FramesSinceToggle = 0` (`:64`), `bHiddenPhase = false` (`:65`), `bActive = true` (`:66`) and nothing more. **The first hide is therefore driven by the `Tick` toggle, not by `Apply`.** Only two sites in the whole file change visibility: `:91` (the `Tick` while-loop) and `:106` (`Revert`).

Reconstruction at config `2 4 8 4 0`, half-period 3. `T0` = the tick `BeginFire()` runs on; `n` = first Positives session index.

| flip | labels place it at | driver | file:line | call made |
|---|---|---|---|---|
| **1 — first hide** | visible→hidden **at n** | `Tick` while-loop toggle, tick **T2**, an **uncaptured settle tick** | `Anomaly_Blinking.cpp:80-97`, call at **`:91`** | `SetActorHiddenInGame(true)` |
| **2 — mid-event show** | hidden→visible **at n+2** | `Tick` while-loop toggle, tick **T5** = **arm tick of n+2**, *after* the arm | `Anomaly_Blinking.cpp:80-97`, call at **`:91`** | `SetActorHiddenInGame(false)` |
| **3 — second hide** | visible→hidden **at n+5** | `Tick` while-loop toggle, tick **T8** = **arm tick of n+5**, *after* the arm | `Anomaly_Blinking.cpp:80-97`, call at **`:91`** | `SetActorHiddenInGame(true)` |
| **4 — final show** | event ends after **n+6** | **`Revert()`**, tick **T10** = **arm tick of n+7**, *after* the arm | `Anomaly_Blinking.cpp:100-113`, call at **`:106`** | `SetActorHiddenInGame(false)` |

Flip 4's chain in full: `AnomalyCaptureSubsystem.cpp:623` `BeginRevert()` → `:2133` `Auto->RevertAllLiveFires()` → `AnomalyAutoInjectorSubsystem.cpp:705` `Injector->RevertAnomaly` → `AnomalyInjectorSubsystem.cpp:486` `(*Found)->Revert()` → `Anomaly_Blinking.cpp:106`.
Flips 1–3's chain: `AnomalyInjectorSubsystem.cpp:197` `Pair.Value->Tick(...)` → `Anomaly_Blinking.cpp:81-97` → `:91`.

**SAME SITE OR DIFFERENT — the explicit answer:**
- **Flips 1, 2 and 3 are the SAME STATEMENT** (`:91`), same chain, differing only in the boolean.
- **Flip 4 is a DIFFERENT STATEMENT** (`:106`) on a different chain.

| partition | grouping |
|---|---|
| by **call site** | `{1, 2, 3}` at `:91` · `{4}` at `:106` |
| by **observed agreement on Bates** | `{1, 4}` match · `{2, 3}` each +1 late |

⇒ **the two partitions do not coincide.** ⛔ **No inference is drawn from that.**

**One structural asymmetry, recorded as structure and explicitly NOT as a lead:** flip 1 fires on an **uncaptured** tick, strictly before the arm of the frame it first affects; flips 2, 3 and 4 fire on that frame's **own arm tick, after the arm**. Flip 1 differs purely by arithmetic of the shipped config — the injector has accumulated `1 + K = 3` ticks by the last settle tick and the half-period is also 3. ⛔ **It cannot be a mechanism for (B) as it stands, and the measured reason is given: the bench has exactly this structure and reads 16/16 ALIGNED**, because an arm-tick toggle still reaches that tick's render via `World->SendAllEndOfFrameUpdates()` at `SceneRendering.cpp:4528` ("Guarantee that all render proxies are up to date before kicking off a BeginRenderViewFamily"), with the redraw following `UWorld::Tick` in the same `FEngineLoop` iteration (`LaunchEngineLoop.cpp:5363`).

🚨 **A finding that is not a flip: `n+7` IS NOT "LABELLED VISIBLE" — IT CARRIES NO ANOMALY ROW AT ALL.** `BeginRevert()` (`:623`) clears `LiveFires` *before* `FinalizeArmedLabel()` (`:670`) reads them (`:2342`), so the last positive frame of every burst has an empty `Snap->Fires`: no `labels.jsonl` anomaly row, no `AffectedFrames`, no `ActiveByIndex`. ⚠ Async-path only; the sync fallback builds its record at `:2211-2212`, before the revert.

**Model validation (done before writing anything up), against `_bench_sessions_bank\P9_P9_B\session_20260902-112711`, `run.json` `pre=4 positive=8 post=4 settle=2`:**

| predicted | measured |
|---|---|
| blink cadence `{n, n+1, n+5, n+6}` | `[16,17,21,22]` and `[40,41,45,46]` ✅ |
| `corrupted_texture` (FireWindow source) window incl. **both** edges | `[27..34]`, 8 frames ✅ |
| last positive frame carries no row | `session_index 23`, `anomaly_present=false` ✅ |
| `n−1` carries the fire | `session_index 15`, `anomaly_present=true` ✅ |

Three facts it was not fitted to.

## 1b — THE CLOCKS

**(i) Who calls `FAnomaly_Blinking::Tick`, how often.** `UAnomalyInjectorSubsystem::Tick` (`AnomalyInjectorSubsystem.cpp:184-218`), dispatch loop `:193-199`, call at `:197`, guarded only by `IsActive()`. The subsystem is a plain `UTickableWorldSubsystem` (`AnomalyInjectorSubsystem.h:12`) ⇒ **exactly once per world tick** — not per captured frame, not per rendered frame. ⚠ **`DeltaSeconds` is ignored**: `FramesSinceToggle` counts **calls** (`:80`), flip condition `while (FramesSinceToggle >= HalfPeriodFrames)` (`:81`). The blink clock has no time in it (the `m23`/`P3a` fix still in force).

**(ii) The label side.** Arm: `Snap.FrameCounter = GFrameCounter` (`:2157`), `Snap.SessionIndex = SessionFrameIndex` (`:2158`), `++SessionFrameIndex` (`:2180`). Drain: PNG name from `Snap->SessionIndex` (`:1978`), record via `BuildLabelRecordForSnapshot` (`:1980` → `AnomalyLabelWriter.cpp:367-372`, fields `:45-46`), `AccumulateFrameEvents(..., Snap->SessionIndex, ...)` (`:1982-1983`) → `Ev->ActiveByIndex.Add(SessionIndex, Active)` (`:3232`) → at `FinishRun` the `Active==1` keys, sorted (`:3291-3299`, `:3322`, `:3338`). ✅ **Confirmed — and it is three consecutive statements over one `Snap`, not merely "one line apart".** `labels.jsonl.frame_index` **IS** `GFrameCounter` at arm time.

**(iii) Two clocks, and they are NEVER converted.**

| clock | unit | file:line |
|---|---|---|
| **A** `FramesSinceToggle` | one **world tick** while active | `Anomaly_Blinking.cpp:80` |
| **B** `SessionFrameIndex` | one **armed captured frame** | `AnomalyCaptureSubsystem.cpp:2180` (async) / `:2255` (sync) |
| *(C)* `CaptureGameTicks` | one world tick while running | `:550` |

**Reconciled nowhere arithmetically.** No code converts ticks↔frames for labelling. They meet at **one sample, not a conversion**: `SampleDeferredActiveState` (`:2520-2562`) reads `FActor->IsHidden()` at **`:2558`** — the state clock A produced, at a moment scheduled by clock B. Clock B advances only in `LeadIn` (`:612`), `Positives` (`:622`), `PostGap` (`:632`); the settle phases tick A and C but not B.

**The 1.3556 arithmetic.** Definition `capture_game_ticks / total_frames`, `AnomalyLabelWriter.cpp:548-549`. Run-wide: 3 ticks = **2.213** captured frames; a full period = **4.426**; an 8-frame window = **10.84** ticks. 🚨 **But the run-wide ratio is not the in-window ratio.** Per burst the FSM spends 12 capturing ticks and 4 non-capturing (2+2 settle) ⇒ structural **16/12 = 1.3333**, and `1.3556` is that plus the `LeadIn`/`DrainTail` edges. **Inside the Positives phase every tick captures ⇒ local ratio exactly 1.0, 3 ticks = 3 captured frames.** The 2-hidden/3-visible/2-hidden shape arises because **one hidden tick of each 3-tick hidden phase lands on a non-capturing tick** — T2 (settle) for the first, T10 (the frame dropped by flip 4) for the second. ⛔ **`ticks_per_captured_frame` is a run-average and is NOT a per-window conversion factor.**

## 1c — THE TOGGLE LINE AND ITS PREFIX

`Anomaly_Blinking.cpp:95-96`, quoted exactly:

```
		UE_LOG(LogAnomaly, Verbose, TEXT("blinking toggle -> %s (%d actors)."),
			bHiddenPhase ? TEXT("HIDDEN") : TEXT("VISIBLE"), Affected);
```

✅ **CONFIRMED: only the phase word and the actor count.** No frame index, no session index, no tick count, no `RequestId`, nothing run-relative. `Affected` (`:86`, `:92`) is a target count.

**The prefix — what Bates' sealed build already prints:**

| element | engine `file:line` |
|---|---|
| prefix builder | `Runtime/Core/Private/Misc/OutputDeviceHelper.cpp:10-76` |
| **UTC timestamp with ms** `[%Y.%m.%d-%H.%M.%S:%s]` | `OutputDeviceHelper.cpp:29` (UTC case `:28-31`) |
| **bracketed frame counter** `[%3llu]` of **`GFrameCounter % 1000`** | `OutputDeviceHelper.cpp:30` |
| category + verbosity (`LogAnomaly: Verbose: `) | `OutputDeviceHelper.cpp:53-63` |
| log **file** routes through it | `OutputDeviceFile.cpp:535`, `:557` → `OutputDeviceHelper.cpp:110-123` |
| **on by default** — `log.Timestamp` declared with default **`1` = UTC** | `LaunchEngineLoop.cpp:5678-5687` (default at `:5680`), pushed to `GPrintLogTimes` by the sink `:5700-5718` (`case 1:` at `:5713`), sink called from the engine tick `:5156` |
| ⚠ the C++ initialiser is `ELogTimes::None` | `CoreGlobals.cpp:386` — UTC arrives via the cvar sink |
| host overrides (outside our reach on a sealed build) | `[LogFiles] LogTimes=` `LaunchEngineLoop.cpp:5731-5762` (called `:5947`); `-LOGTIMES`/`-NOLOGTIMES`/… `:5764-5787` |

🎯 **THE ANCHOR IS EXACT.** `GFrameCounter++` is at `LaunchEngineLoop.cpp:5568`, **after** `GEngine->Tick(...)` at `:5363` ⇒ everything inside one `UWorld::Tick` — the capture arm and the injector toggle alike — sees the **same** `GFrameCounter`, and so does the log device serialising on the game thread. **⇒ the toggle line's `[fff]` == `labels.jsonl.frame_index % 1000` of the frame armed on that same tick.**

✅ **Measured on this box's packaged Development build**, not only source-read:
`[2026.09.02-10.07.33:125][  4]LogAnomaly: blinking: matched 1 actor(s) for '=StaticMeshActor_49' at half-period 3 frame(s).` — prefix present. Same file, `blinking toggle ->` returns **0 hits**: direct confirmation the toggle line is absent without `Verbose`. Also cross-checked in the artifact: `session_index` 15→`frame_index` 20, 16→23 … 23→30, 24→33 — contiguous inside a phase, jumping by `K+1 = 3` at each settle boundary, exactly as reconstructed.

⚠ **A render-thread line's `[fff]` is not its own render frame** (it prints the game thread's counter) — matters only for `keyed frame … submitted`, which carries its own `id=` and `rtframe=`.

## 1d — THE BRACKETING LINES

All `Log` verbosity. **"no-flags?"** = present on a Bates run with census OFF and mask OFF.

**Per captured frame:**

| line | file:line | thread | carries | no-flags? | bound |
|---|---|---|---|---|---|
| `Capture(sve): SVE-WANT-TRACE arm %d/%d requestId=%llu gameFrame=%llu pendingAfter=%d` | `AnomalySveCapturer.cpp:39-41` | **game**, in the arm | **`requestId` AND `gameFrame`** | ✅ | 🚨 **FIRST 64 ONLY** (`HandshakeTraceLimit=64`, `AnomalySveCapturer.h:38`; gate `:32-35`) |
| `Capture(sve): SVE-WANT-TRACE publish %d/%d familyFrame=%u wanted=%d requestId=%llu pendingBefore=%d` | `AnomalySveCapturer.cpp:68-70` | **render** | `familyFrame`, `requestId` | ✅ | 🚨 **FIRST 64 ONLY** |
| `Capture(sve): keyed frame id=%llu submitted (rtframe=%u, fmt=%d, rect=%dx%d, dualPath=%d).` | `AnomalySveCapturer.cpp:151-153` | **render** | **`id` (=`RequestId`)**, `rtframe` | ✅ | **unbounded** |

📌 **`RequestId` ↔ `session_index` is a constant offset within a run** (`:2155` / `:2158` / `:2180`). `SessionFrameIndex` resets at `StartRun` (`:1357`); **`CaptureRequestSerial` does not** (`AnomalyCaptureSubsystem.h:222`), so the offset is 1 for a process's first run and larger after — constant within any run, recoverable by pairing one arm-trace line with its `labels.jsonl` row. ⚠ Holds only while every frame takes the async path (a sync fallback bumps `SessionFrameIndex` at `:2255` without a `RequestId`; announced at `:2185`, *Verbose*).

**Per event:**

| line | file:line | marks | no-flags? |
|---|---|---|---|
| `blinking: matched %d actor(s) for '%s' at half-period %d frame(s).` | `Anomaly_Blinking.cpp:68-69` | **START**, and reports the **EFFECTIVE half-period** | ✅ |
| `IAI.Apply '%s' -> %s.` | `AnomalyInjectorSubsystem.cpp:468` | START | ✅ |
| `Auto.FireSpecific: '%s' on '%s' -> %s.` | `AnomalyAutoInjectorSubsystem.cpp:473-474` | START + target | ✅ |
| `Auto.Fire: '%s' on '%s' -> %s.` (auto-pool) | `AnomalyAutoInjectorSubsystem.cpp:369-370` | START + target | ✅ |
| **`IAI.Revert '%s' -> reverted.`** | `AnomalyInjectorSubsystem.cpp:488` | **END — flip 4's only receipt** | ✅ |
| `Capture: burst %d fired nothing …` | `AnomalyCaptureSubsystem.cpp:2122-2123` | a non-firing burst | ✅ |

⚠ **`FAnomaly_Blinking::Revert()` logs NOTHING** (`:100-113`). 🎯 **Checkable expectation: one event ⇒ exactly THREE toggle lines, `HIDDEN · VISIBLE · HIDDEN`, then `IAI.Revert 'blinking' -> reverted.`**

**Once per run:** `=== Capture run STARTED` (`:1675`) · `FINISHED` (`:3030`) · `RESOLUTION DELTA (3-rect)` (`:1946`) · `MEASURED FROM THE FIRST WRITTEN FRAME` (`:1850`) · `grab point EFFECTIVE` (`:1509`) · `TOGGLING-SUBSET … positives=%d of %d` (`:3342-3347`, per toggling event, counts not indices) · `SVE-WANT-SUMMARY` (`:3013`) · `key ring` (`:3005`) · `TICKPIN` (`:1584`/`:1593`/`:1600`).

⛔ **ABSENT on a no-flags run:** every `Census:` line and every `Capture(mask): M23 / M26S1 / M26S2 / M26S3 / M27 / M36` line, `VETOED-OBJECT`, `DUAL-PATH COMPARE`. These are the densest per-frame lines the bench log carries and they are exactly what a no-flags run will not have.

**Ordering guarantees:**

| pair | same thread? | same tick? | ordered? |
|---|---|---|---|
| `SVE-WANT-TRACE arm` ↔ toggle line | **yes, game** | **yes** | **YES — arm first, toggle second, same `GFrameCounter`** |
| fire lines ↔ first toggle | yes | no (T0 vs T2) | ordered by tick |
| `IAI.Revert` ↔ last toggle | yes | no (T10 vs T8) | ordered by tick |
| `keyed frame … submitted` / `publish` ↔ any game-thread line | **NO — render** | no | ⛔ **NOT ordered.** Join by `id=`/`requestId=`, never position or prefix |

🚨 **The capture-before-injector order is REGISTRATION ORDER, not an architectural guarantee — stated rather than assumed.** All three subsystems are plain `UTickableWorldSubsystem`s (`AnomalyCaptureSubsystem.h:14`, `AnomalyInjectorSubsystem.h:12`, `AnomalyAutoInjectorSubsystem.h:35`); **none** overrides tick type, group or priority. Evidence for the order: (a) `m20`, which measured `annotation(G) == pixels(G-1)` and produced `SampleDeferredActiveState` to cope; (b) this turn's reconstruction, which reproduces `[16,17,21,22]`, `[27..34]` and the empty `session_index 23` **only** under that order.

## 1e — WHAT `C-3` CAN AND CANNOT DISCRIMINATE

| | **flip 2** (labels `n+2`, eye `n+3`) | **flip 3** (labels `n+5`, eye `n+6`) |
|---|---|---|
| place the toggle on a specific captured frame? | **YES**, if the engine prefix is present | **YES**, same condition |
| join | toggle `[fff]` = `GFrameCounter % 1000` ↔ `labels.jsonl.frame_index % 1000` from `C-3(b)` | identical |
| **resolution** | **EXACT captured-frame index** (local tick:frame ratio is 1.0 inside Positives) | identical |
| backup anchor | `SVE-WANT-TRACE arm … gameFrame=` adjacent, **only if frame ≤ 64** | identical |
| fallback if both gone | **tick-level ORDER only** — after a named arm line, before the next; no frame index | identical |

**What makes it indeterminate — all four readable from the bundle itself:** (1) the prefix is absent (`LogTimes=None` / `-NOLOGTIMES` on that build) ⇒ only ordering survives; (2) the event sits beyond captured frame ~64 ⇒ the arm trace has stopped and the prefix is the sole anchor; (3) `Log LogAnomaly Verbose` did not take ⇒ no toggle lines — **a READ, not a failure**, already on the card; (4) the run straddles a `GFrameCounter` multiple of 1000 ⇒ visible as a decreasing `[fff]`, resolved by the full `frame_index`.

⚠ **What it cannot do at all:** `C-3` never sees the **pixels**. It places the *toggle call*; the eye places the *pixels*; `labels.jsonl` + `frame_indices` place the *label*. Three artifacts, one each — which is why it is a three-way comparison.
⚠ **Flip 1** fires on an **uncaptured** tick, so its toggle line matches **no `labels.jsonl` row** — it falls in the `K`-tick gap, and `frame_index(n) − frame_index(n−1) == K+1 == 3` is itself a positive check. **Flip 4** emits **no toggle line**; its receipt is `IAI.Revert`.

**Free label-side cross-checks (host-independent; a failure is a finding, report raw):** `n−1` ⇒ **AMBER, never RED** · **`n+7` ⇒ NO BOX AT ALL, neither red nor amber** · `frame_index` contiguous across the window, jumping by 3 at phase boundaries · exactly 3 toggle lines per event · the `at half-period N frame(s)` line reports that build's effective value.

**THE PRE-DECLARED THREE-WAY COMPARISON, RESTATED IN THESE TERMS.** For each interior flip, anchor the toggle line to a captured-frame index `f_toggle`:

| outcome | card's words | structurally |
|---|---|---|
| `f_toggle` = **`n+2` / `n+5`** (log sides with **LABELS**) | **the PIXELS are the outlier on this host** | the toggle ran on the tick the labels imply; divergence is **downstream of the visibility call** |
| `f_toggle` = **`n+3` / `n+6`** (log sides with **EYE**) | **the SAMPLING / LABELLING side is the outlier** | the toggle ran a tick later than the labels imply; divergence is **between toggle and label** |
| mixed across the two flips, or no usable anchor | ⛔ **report raw. Classify nothing.** | — |

⛔ **No branch is asserted or preferred. Outcomes name WHERE, never WHY.** ⛔ **Nothing in 1a–1d is a prior on which branch fires** — the bench has the identical plugin structure and reads 16/16 ALIGNED, so the structure alone predicts neither.

---

# 3. WHAT WAS WRITTEN WHERE

| | |
|---|---|
| **Journal (new)** | `docs/sessions/2026-09-02-068-p9-transition-drivers-and-toggle-anchor.md` — §1 is the read (§1.0 scope + line-number mapping + model validation · §1.1 flips · §1.2 clocks · §1.3 toggle line + prefix · §1.4 bracketing lines · §1.5 what C-3 can/cannot do), §2 what was not done, §3 commit |
| **Card — TOUCHED: YES** | `docs/office-rdp-card.md` gains **`#### 🆕 C-3 READ GUIDE`** inside the C-3 section, immediately before `### Standing`. **Read instructions only — no new steps, no new commands.** Six items: the prefix is the sole anchor (and say so if missing) · join (a)↔(b) on `frame_index` not `session_index` · the 64-frame trace bound + render-thread lines join by `id` · the two overlay cross-checks (amber at `n−1`, **no box at `n+7`**) · expect 3 toggle lines `HIDDEN·VISIBLE·HIDDEN` then the revert line, and copy the half-period line · the comparison in one sentence per branch |
| **`CLAUDE.md`** | The `P9` paragraph gains a **✅ TASK 1 DONE** block with the pointer, the findings in summary (Apply-never-hides · the two non-coinciding partitions · two clocks never converted · the exact prefix anchor · `n+7` carries no row · the 64-frame bound), and the note that journal 067 §17.6 and the handoff still list it outstanding **because both are records and are not retro-edited** |
| **Commit** | **`3e14385`** `docs(p9): transition-driver and toggle-line anchor source read` — 3 files, **644 insertions, 0 deletions**. Diffstat checked pre-commit (pure insertions, no encoding smell, `G115` clean); subject byte-checked for a BOM (`d,o,c` — clean, avoiding `6fb1215`'s defect class); path-scoped `git add`, the four untracked handoffs untouched |
| **Push** | **Pushed.** `be4dd1b..3e14385 master -> master`; `git rev-parse HEAD` == `origin/master` == `3e14385`; `origin/master..master` empty |
| **Not done** | No source edit, no build, no leg, no cook, no tag, no `m38` work. Staged exe still `6C80E872`, container unchanged. No Bates artifact was read — none exists here; every measured cross-check is bench-side |

⚠ **One correction to carry, verified rather than assumed:** journal 067 §15's `AnomalyCaptureSubsystem.cpp` line numbers were **correct at `d257f7b^`** (I read that blob directly) and are now **+6** in the early file / **+61** from `m37`'s ceiling knob onward. The stamping trio is **`:1978` / `:1980` / `:1982-1983`**; `ActiveByIndex.Add` is **`:3232`**; the `TOGGLING-SUBSET` log is `:3342-3347`. **Facts unchanged.** Full mapping table in journal 068 §1.0.

---

# 4. NEEDS-DECISION

**NEEDS-DECISION 1 — `m38` gate (i)'s marker list names a token that does not exist.** Journal 067 §16.4 gate (i) asks the run log to contain `Census: BEGIN`, `M36 STENCIL RESERVATION` and **`M23 ARM`**. Checked at `be4dd1b`: `Census: BEGIN` exists (`AnomalyCensus.cpp:159`) and `M36 STENCIL RESERVATION` exists (`AnomalyCaptureSubsystem.cpp:1483`), but **there is no `M23 ARM` anywhere** — the mask tokens are `M23 CVAR` (`AnomalyMaskMeasure.cpp:56`), `M23 PASS` (`AnomalyMaskSceneViewExtension.cpp:198`) and `M23 REDUCE` (`:401`). Separately, **all three of the named markers are census/mask-conditional**, so a "normal leg" run without those flags would produce none of them and gate (i) would fail on a healthy build. The plan has not yet become a predictions file, so this is fixable at pre-declaration with nothing to amend. ⛔ **I did not change the gate wording** — gates are chat's, and journal 067 §16 is an approved record. **Decision wanted: the replacement marker set and the flag state gate (i)'s "normal leg" runs under.**

**NEEDS-DECISION 2 — which event the owner should anchor `C-3` on.** The `SVE-WANT-TRACE arm` line, the only backup anchor for the toggle line, **stops after 64 captured frames**. A 90-frame run has events on both sides of that boundary. Choosing an event inside the first ~64 frames gives **two independent anchors** instead of one; beyond it the engine prefix is the sole anchor and a build with log timestamps off would leave `C-3` unable to place either interior flip. ⛔ **I did not add this to the card** — the brief said read instructions only, no new steps, and "pick an early event" is a step. **Decision wanted: whether to add that one-line instruction to `C-1`/`C-3`, or accept the single anchor.**

