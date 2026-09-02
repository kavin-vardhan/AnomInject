# `m40` — ORDER-INDEPENDENT LABEL SAMPLING. PRE-DECLARED GATES AND PREDICTIONS.

> 🚨 **WRITTEN AND COMMITTED BEFORE ANY `m40` SOURCE EXISTS.** Read this file before reading any
> result. ⛔ **It is not amended after a measurement** — corrections ride the journal (the
> `P-C2` / `P-C13` route).
>
> **Plan:** journal 068 §10. **Approved:** session 068, brief 5 — option 2 of journal 068 §9.
> **Baseline binary `F2FA6BCD`** (`m38`). Container unchanged, **no cook**. Census stays compiled
> **OFF**. **No tag.**
>
> 🔢 **`m40` MAY SHIP BEFORE `m39`.** Milestone numbers are identities, not an order. `m39` remains
> honest bbox and `P-C13` conjunct 2 still rides it.

---

## 0. WHAT `m40` IS, IN ONE PARAGRAPH

The label's per-frame *active* bit is sampled at a point that is **before the injector's tick on one
host and after it on another**, so on a host where the injector ticks first the sampled bit describes
the *next* frame rather than the one being armed. `m40` moves that sample to
**`FWorldDelegates::OnWorldTickEnd`** (`LevelTick.cpp:1814`) — **after every tickable
(`FTickableGameObject::TickObjects` at `:1606`) and after `OnWorldPostActorTick` (`:1672`), and still
before the draw**. From there the sampled bit **is** what the renderer will draw for that same frame,
**whatever order the subsystems ticked in**. ⛔ **No rendered pixel changes on any host. Nothing about
tick order is pinned, asserted, or relied on.**

### 0.1 The arithmetic this rests on — declared, and it is the whole design

`label(frame armed at tick T)` today is the actor's hidden state read at the **top of the capture
subsystem's `Tick` on `T+1`**. That instant sits **before** the injector's tick of `T+1` when capture
ticks first, and **after** it when the injector ticks first. `m40` reads instead at the **end of world
tick `T`**, which is after every subsystem in both orders.

| case | pixels (eye) | labels (`frame_indices`) | verdict |
|---|---|---|---|
| bench, shipped, lever OFF | `{n, n+1, n+5, n+6}` | `{n, n+1, n+5, n+6}` | **ALIGNED** |
| Bates (measured `C-3`) | `{n, n+1, n+2, n+6}` | `{n, n+1, n+5, n+6}` | **P9-SHAPE** |
| **lever ON, pre-fix** | `{n, n+1, n+2, n+6}` | `{n, n+1, n+5, n+6}` | **P9-SHAPE — exact Bates reproduction** |
| **lever ON, post-fix** | `{n, n+1, n+2, n+6}` | `{n, n+1, n+2, n+6}` | **ALIGNED** |
| **lever OFF, post-fix** | `{n, n+1, n+5, n+6}` | `{n, n+1, n+5, n+6}` | **ALIGNED, byte-identical to shipped** |
| the fix applied to the Bates order | `{n, n+1, n+2, n+6}` | `{n, n+1, n+2, n+6}` | **ALIGNED** |

⛔ **The last row is a DERIVATION, not a prediction this file can gate** — Bates is sealed. It is the
claim §6 validates when that build is next updated.

---

## 1. THE LEVER

### 1.1 🔻 THE MECHANISM NAMED IN THE RULING DOES NOT REPRODUCE `P9`. STATED BEFORE ANY CODE.

Brief 5's `R1` suggested, as an example, *"the injector does not count the tick on which `Apply`
ran"*. **Worked through, that lever produces ALIGNED and reproduces nothing:**

| lever | pixels | labels | reproduces `P9`? |
|---|---|---|---|
| **A** — injector does not count the apply tick | `{n, n+1, n+2, n+6}` | `{n, n+1, n+2, n+6}` | ❌ **NO — ALIGNED** |
| **B** — injector's anomaly dispatch relocated to `OnWorldPreActorTick` | `{n, n+1, n+2, n+6}` | `{n, n+1, n+5, n+6}` | ✅ **YES — exact** |

**Why A fails.** Skipping the apply tick moves every toggle one tick later, so the **pixels** move —
but the shipped sampler reads *"state after the injector's tick of `T`"*, which moves by the same one
tick, so the **labels move with them**. The lever shifts both sides of the comparison and the reader
stays ALIGNED. ⇒ **the `apply → first toggle` `Δ` is a co-symptom of the reordering, not its
label-side cause; a lever built on the co-symptom synthesises the wrong half.**

**Why B works.** Running the dispatch at `OnWorldPreActorTick` (`LevelTick.cpp:1468`, **before**
`TickObjects` at `:1606`) puts the injector genuinely ahead of the capture subsystem **for every
tick**, which reproduces **both** consequences at once: the apply tick goes uncounted (`Δ = +3`,
because `Apply` runs later, inside the capture `Tick`), **and** the shipped sampler now sits *after*
the injector's tick, so the labels stay at `{n, n+1, n+5, n+6}` while the pixels move to
`{n, n+1, n+2, n+6}`. **All four Bates numbers, including the revert boundary.**

⛔ **THIS SYNTHESISES THE SYMPTOM, NOT THE CAUSE, AND THE GATE FILE SAYS SO HERE AND THE BUILD SAYS SO
AT RUN TIME.** Bates' ordering arises from ordinary `FTickableGameObject` order (journal 068 §8);
the lever reaches the same relative ordering **through a different mechanism (a delegate)**. It is
evidence that the fix removes *this* misalignment; **it is not evidence about what causes Bates'.**

### 1.2 The lever as specified

| | |
|---|---|
| **name** | `IAI.Bench.SynthTickOrder <0\|1>` |
| **default** | **OFF**, compiled |
| **route** | **CONSOLE ONLY.** ⛔ **No ini key. No dashboard command. No client-facing surface of any kind.** |
| **module** | `AnomalyInjector` (it relocates that module's own dispatch), read cross-module by `AnomalyCapture` through a `static … (UWorld*)` accessor — the `IsViewportScopingEnabled` pattern, `AnomalyInjectorSubsystem.h:43` |
| **effect when ON** | `UAnomalyInjectorSubsystem::Tick`'s anomaly dispatch loop (`AnomalyInjectorSubsystem.cpp:193-199`) is skipped, and the identical dispatch is driven from `FWorldDelegates::OnWorldPreActorTick` instead |
| **effect when OFF** | the loop runs exactly where it does today; the delegate handler returns immediately. **Behaviourally byte-inert** |
| **scope** | the dispatch only. The heartbeat, the overlay suppression and everything else in `Tick` stay where they are |

### 1.3 The echo — `A48` shape, and it is UNCONDITIONAL

⛔ **A diagnostic that can be silently on is a clean null waiting to be misread (`G139`, `A48`).** Two
read-backs, both unconditional:

**(a) at the moment it is set**, from the console command:

```
IAI.Bench.SynthTickOrder -> ON. BENCH-ONLY SYNTHESIS. The injector's anomaly dispatch now runs at
OnWorldPreActorTick, i.e. BEFORE the capture subsystem, for every world tick. This SYNTHESISES THE
SYMPTOM of a host on which the two subsystems tick in the opposite relative order; it does NOT
reproduce that host's cause and is NOT evidence about it. Labels and pixels will disagree by design.
NEVER ship a capture taken with this ON.
```

**(b) at every `StartRun`**, on its own line beside the existing `m28`/`m36`/`m38` echoes, **whether
it is on or off**:

```
Capture(bench): m40 SYNTH TICK ORDER = off (compiled default). When ON, the injector's anomaly
dispatch is relocated to OnWorldPreActorTick to synthesise the SYMPTOM of a reversed subsystem tick
order. It is a bench device: console-only, no ini key, never in a client payload. A session captured
with it ON has labels that deliberately disagree with its pixels.
```

⚠ **The `off` line is the load-bearing one** — it is what makes a normal run distinguishable from one
that silently had the lever on.

---

## 2. THE FOUR LEGS — PRE-DECLARED

**Reader: `CaptureBench/tools/p9_hidden_set.py`, unmodified.** `SEP_RATIO = 5.0` (`:202`) is
**FROZEN and is never retuned for this milestone or any other** — retuning it re-triggers `A53`.
Verdict vocabulary is the reader's own: `ALIGNED` · `SHIFTED(k)` · `P9-SHAPE` · `UNDECIDABLE`.
**Exit 0** iff every counted event is `ALIGNED`.

**Common leg configuration, identical across all four:** the `C-3` / `m38` gate-(v) configuration —
`IAI.Capture.Config 2 4 8 4 0`, 90 frames, seed 4242, targeted `blinking` on
`StaticMeshActor_49`, PNG, paced 30 fps, census OFF, **`IAI.Capture.RunLogVerbose 1`** so
`anomaly_log.txt` carries the toggle lines with their prefix. Harness: `CaptureBench/tools/run_leg.ps1`.
**`B1` is NOT APPLICABLE and is DECLARED so, never "passed"** (`G117`).

⚠ **VALIDITY FLOOR, declared in advance, not discovered:** a leg needs **≥ 3 COUNTED EVENTS** to be
evidence (the standing rule). The run produces **8 blink events**; the **8th is truncated by the frame
cap** (`frame_indices [88,89]`, journal 068 §6.4.2 / §7.3) and is **expected to land `UNDECIDABLE` or
uncounted — that is predicted here, not explained afterwards.** A leg below 3 counted is **INVALID,
never FAILED**, and is re-run.

### L1 — pre-fix binary `F2FA6BCD`, lever OFF · **THE CONTROL**

| | |
|---|---|
| **predicted reader** | **every counted event `ALIGNED`; tally contains no non-`ALIGNED` entry; exit 0** |
| **predicted sets** | claimed == observed == `{n, n+1, n+5, n+6}` on every full burst |
| **predicted toggle lines** | apply → first toggle **`Δ = +2`** on every full burst; first toggle on an **uncaptured** tick |
| **also asserted** | `frame_indices` byte-identical to the banked `M38_G5_VERBOSE` session's, and the certified cadence reproduced **byte-identically** |
| **what a failure means** | the environment moved, not the fix — **STOP; this leg touches no new code** |

### L2 — pre-fix binary `F2FA6BCD`, lever ON · 🎯 **THE BENCH REPRODUCTION OF `P9` (B)**

| | |
|---|---|
| **predicted reader** | **every counted event `P9-SHAPE`**, `best_k = 0` with a **non-empty** residual carrying differences in **BOTH** directions; exit 1 |
| **predicted sets** | claimed `{n, n+1, n+5, n+6}` · observed `{n, n+1, n+2, n+6}` · residual `{n+2, n+5}` |
| **predicted toggle lines** | apply → first toggle **`Δ = +3`**; first toggle lands **on `frame_index(n)`**, a **captured** tick — the discriminator from journal 068 §6.6 correction 2 |
| **cross-check** | this is the reader's own `--d-unit` fixture **(ii) "Bates pair"** (`p9_hidden_set.py:354`), so the branch is already known-answer tested on literal sets |
| 🛑 **FAILURE BRANCH, PRE-DECLARED** | **if `L2` does not reproduce, THE LEVER IS WRONG, NOT THE THEORY. STOP AND REPORT.** Do not retune the lever and re-run in the same turn; do not touch `SEP_RATIO`; do not proceed to `L3` |

### L3 — post-fix binary, lever ON · **THE PROOF THE FIX MOVED THE LABELS**

| | |
|---|---|
| **predicted reader** | **every counted event `ALIGNED`; exit 0** |
| **predicted sets** | claimed == observed == **`{n, n+1, n+2, n+6}`** — note this is the *displaced* cadence, and that is correct: the labels now follow the pixels |
| **predicted toggle lines** | apply → first toggle **STILL `Δ = +3`** |
| 🎯 **why the `Δ` matters** | **the lever is still on and the toggles have NOT moved back — so what changed is the LABEL SAMPLE, not the lever.** Without this, `L3` would be indistinguishable from "the lever stopped working" |

### L4 — post-fix binary, lever OFF · **INERTNESS**

| | |
|---|---|
| **predicted reader** | **every counted event `ALIGNED`; exit 0** |
| **predicted sets** | claimed == observed == `{n, n+1, n+5, n+6}` |
| **predicted artifacts** | **`frame_indices` and `labels.jsonl` byte-identical to `L1`** (after the declared run-unique field set: `session_id`, `video.path`, `speed_ratio`, `sustained_wall_fps`, and `labels.jsonl` row ORDER per `G162` — **compare sorted by `session_index`**) |
| **predicted toggle lines** | `Δ = +2`, first toggle on an uncaptured tick — identical to `L1` |
| **rides this leg** | **`P-C7` re-anchored to the post-fix binary** (census OFF byte-identity of the selection path — `m40` does not touch selection, so it must still hold) |

### 2.1 The four legs as one statement

> **`L1` says the bench is unchanged. `L2` says the defect can be produced here. `L3` says the fix
> removes it while the synthesised disorder is still present. `L4` says the fix does nothing where
> nothing was wrong.** ⛔ **`L3` without `L2` proves nothing** — that is the whole reason Route A was
> ruled, and `G96`'s three prior instances are why.

---

## 3. SUPPORTING GATES

| gate | condition | why it bites |
|---|---|---|
| **`m20` re-run** | on the post-fix binary, lever OFF: `annotation` hidden set **==** pixels hidden set on **every** blink edge | the founding gate for the deferred sampler (journal 026 §Bug B). A regression here is the `m20` bug returning, and `m40` moves exactly that code |
| **`P6` 48/48** | `annotation.json` **and** `run_summary.json` key sets unchanged, 0 added / 0 removed | `m40` changes a **value** on an affected host, never a **shape** |
| **`P-C7`** | census-OFF byte-identity re-anchored to the post-fix binary | rides `L4` |
| **client-inert** | **no new ini key; no new `run.json` / `annotation.json` / `run_summary.json` field; the lever is console-only** | the client build must be reachable only through the fix, never through the lever |
| **`A44`** | the new symbols present in the **staged** artifact, scanned in **both** encodings | a shader-free code change still has to reach the binary |
| **`G103`** | code-only hot-swap; **container unchanged, no cook** | `m40` adds no asset and no global shader |

⛔ **`G-M9`, `G-M7`/`G-M8`, and the mask/census gates are NOT implicated and are NOT re-run.** Recorded
so nobody spends a day on them.

---

## 4. LIFECYCLE — THE RISK THIS DESIGN ACTUALLY CARRIES

`FWorldDelegates::OnWorldTickEnd` is a **global, multicast, engine-lifetime** delegate. The failure
mode is a **dangling handle across a world transition**: the subsystem is torn down, the handle is
not removed, and the delegate fires into freed state or samples the wrong world.

**Mitigations, all three mirroring the `m26` mask hook that is already proven in this file:**

1. **Registered in `Initialize`, removed in `Deinitialize`** — beside
   `AnomalyCaptureSubsystem.cpp:334` and `:527`, with its **own** handle member.
2. **`AddUObject`**, so the delegate is weak against the `UObject` and will not fire on a destroyed
   subsystem — the same binding the mask hook uses.
3. **`if (World != GetWorld()) return;`** as the handler's first statement — the guard at `:707`.

**Predicted teardown behaviour, declared so it can be checked rather than assumed:**

- a `LoadMap` / world transition mid-session leaves **no** `OnWorldTickEnd` binding owned by the
  destroyed subsystem, and the incoming world's subsystem registers its own;
- an `IAI.Capture.Stop` between ticks loses **no** sample: the last armed frame was already sampled at
  its own tick's `OnWorldTickEnd`, and `FinishRun`'s existing `SampleDeferredActiveState()` call
  (`:2986`) is **kept as a no-op safety net**;
- **no new log line is emitted per tick.** ⛔ A per-tick line on a 90-frame run is noise that would
  swamp `anomaly_log.txt`, which `m38` exists to keep readable.

⚠ **ONE DETAIL DELIBERATELY LEFT TO IMPLEMENTATION, NAMED HERE:** `OnWorldTickEnd` broadcasts with an
`ELevelTick TickType`. The existing mask handler does not filter on it. Whether `m40`'s handler should
ignore `LEVELTICK_TimeOnly` is **an implementation question to settle by reading, not by guessing**,
and the answer goes in the journal.

---

## 5. STOP RULES

⛔ **Any leg fails ⇒ report and stop. Do not fix in the same turn. Do not re-run to a green.**

1. **`L2` does not reproduce** ⇒ the lever is wrong. **Stop.** (§2, `L2`.)
2. **`L1` is not byte-identical to the banked control** ⇒ the environment moved. **Stop**, and do not
   read `L2`–`L4` against it.
3. **`L4` is not byte-identical to `L1`** ⇒ the fix is **not** inert where the order already agrees.
   **Stop** — that is a defect in the fix, not a tuning matter.
4. **`P6` moves** ⇒ **stop.** No artifact field is in scope.
5. **`m20`'s gate regresses** ⇒ **stop.**
6. ⛔ **`SEP_RATIO` is never touched, in any branch, for any reason.**

---

## 6. BATES VALIDATION — FOR THE NEXT BUILD UPDATE

**Bates is sealed. `m40` reaches it only when that host's build is next updated.** Until then:
⛔ **`blinking` stays UNTICKED on any Bates run**, and **nothing further is asked of that box for
`P9`** (RDP card, Standing).

**Instrument: a re-run of the `C-3` bundle**, unchanged — same four commands, same READ GUIDE (as
corrected in journal 068 §6.6). **Pass condition, pre-declared here in Route-B's wording:**

> ✅ **PASS — the labels equal the eye, WHETHER `apply → first toggle` READS `+2` OR `+3`.**
> The `Δ` is now a *report*, not a gate: it says which order that build happens to have, and `m40`'s
> whole claim is that the answer no longer matters.

| observation on the post-`m40` Bates build | reading |
|---|---|
| `Δ = +3` **and** labels == eye | ✅ **PASS, and the strongest possible form** — the disorder is still there and the labels are right anyway |
| `Δ = +2` **and** labels == eye | ✅ PASS, ⚠ **weaker** — that build's order also changed, so it does not exercise the fix. **Say so; do not report it as the strong result** |
| labels ≠ eye | 🔴 **FAIL.** Report raw, classify nothing, and do **not** propose a second fix in the same message |

⛔ **No further Bates errand is created by this file.** The re-read costs three reads: `apply → first
toggle`, `frame_indices`, and one eye pass over `n−1 … n+7`.

---

## 7. COMMIT SHAPE

| step | commit | contents |
|---|---|---|
| 1 | **`docs(m40): plan and predictions - order-independent label sampling`** | this file + journal 068 §10 + the ledger's `FIX APPROVED` line. **Lands BEFORE any source exists** |
| 2 | **`feat(capture): m40 - order-independent label sampling`** | the fix, the lever, both echoes. **One commit** |
| 3 | *(if a gate forces a change)* | a **follow-up commit**, never an amend — the `m35` precedent |

⛔ **NO TAG.** Highest remains `m30`; the office batch becomes `m31 → m33 → m34 → m35 → m36 → m37 →
m38 → m40`, with **`m39` slotting in when it ships**. **`m40` does not wait for `m39`.**

**Scope statement to carry on the commit:**

> *`m40` makes the per-frame label's active bit independent of the order in which the capture and
> injector subsystems tick, by sampling it at the end of the world tick instead of at the top of the
> next capture tick. It changes no rendered pixel on any host, adds no artifact field, and adds no
> client-facing setting. On a host where the two already tick in the order this bench does, it is
> byte-identical. `IAI.Bench.SynthTickOrder` is a bench device that synthesises the SYMPTOM of the
> other order; it is console-only, default OFF, echoed unconditionally at `StartRun`, and must never
> be on in a delivered capture.*

---

## 8. WHAT THIS FILE DOES NOT CLAIM

- ⛔ **It does not claim to know why Bates' order differs.** Journal 068 §8's reading stays
  *"consistent with"*, and `m40` deliberately does not depend on it being right.
- ⛔ **It does not claim the lever reproduces Bates' CAUSE** — only its two measured consequences.
- ⛔ **It does not fix the sync-fallback path** (`AnomalyCaptureSubsystem.cpp:2439`), which samples
  inline and remains one tick stale by the same arithmetic. **Ruled out of `m40` deliberately**
  (brief 5, `R2`): no gate exercises that path today, and one variable at a time.
  🔎 **How a reader detects it:** a sync frame mints no `RequestId`, so
  **`SVE-WANT-SUMMARY`'s `marksIssued` falls below `framesWritten`** — a `Log`-verbosity line present
  with no flags. ⚠ Its own `Capture(async): … falling back to sync grab` notice is on
  **`LogAnomalyCapture`**, so a run that raised only `LogAnomaly` to `Verbose` **will not show it**.
- ⛔ **It does not certify anything at any other configuration** — one config, `2 4 8 4 0`, 30 fps.
- ⛔ **It does not move `P6`, does not touch selection, does not touch the census or the mask, and
  does not cook.**
