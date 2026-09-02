# 2026-09-02 — session 068 — `P9` TASK 1: the transition-driver and toggle-line-anchor source read

> 🧭 **What this session is.** Journal 067 §17.6 handed forward **TASK 1**, the source read that was
> never executed because the brief carrying it was never delivered. This is that read, and nothing
> else. **Docs-only. No source edit, no build, no leg, no `m38` work, no tag.**
>
> ⛔ **DISCIPLINE, STATED ONCE AND HELD THROUGHOUT §1: NO MECHANISM, NO LEAD AND NO LIKELY-CAUSE FOR
> `P9` (B) APPEARS ANYWHERE BELOW.** Every row is a factual mapping with `file:line`. Where a
> structural fact sits close to something that could be *read* as a mechanism, it is marked ⛔ and the
> reason it is **not** one is given. The frame-saga discipline (`G120`) governs: an observation and
> its explanation are separate claims, and a scope decision may rest only on the observation.

---

## §1 THE SOURCE READ

### §1.0 Scope, provenance, and the line-number note

**Tree read: `master` at `be4dd1b`** (`git rev-parse HEAD` == `git rev-parse origin/master`), working
tree clean apart from the owner's four untracked `docs/CHAT-HANDOFF-*.md`. **Every plugin `file:line`
below is at `be4dd1b`.** Engine citations are the source engine at `Engine/Source/...`, UE 5.1.

⚠ **LINE-NUMBER DRIFT AGAINST JOURNAL 067 §15 — VERIFIED, NOT A CONTRADICTION.** §15's citations into
`AnomalyCaptureSubsystem.cpp` were correct **at `d257f7b^`** (pre-`m37`) and were checked there this
turn by reading that blob directly. `m37` (`d257f7b`) inserted the ceiling knob, so the same sites are
now lower in the file. The **facts are unchanged**; only the numbers moved.

| §15 said | at `be4dd1b` | delta | what is there |
|---|---|---|---|
| `:533` | **`:539`** | +6 | `UAnomalyCaptureSubsystem::Tick` |
| `:555` | **`:561`** | +6 | `SampleDeferredActiveState();` |
| `:664` | **`:670`** | +6 | `FinalizeArmedLabel();` |
| `:2094` | **`:2155`** | +61 | `const uint64 RequestId = ++CaptureRequestSerial;` |
| `:2111` | **`:2172`** | +61 | `Async->SveCapturer->ArmWanted(RequestId);` |
| `:2294` | **`:2355`** | +61 | `Snap->FirePos.Reset();` |
| `:2497` | **`:2558`** | +61 | `Active = (FActor && FActor->IsHidden()) ? 1 : 0;` |
| `:1917` / `:1919` / `:1921-1922` | **`:1978` / `:1980` / `:1982-1983`** | +61 | PNG name · labels record · `AccumulateFrameEvents` |
| `:3171` | **`:3232`** | +61 | `Ev->ActiveByIndex.Add(SessionIndex, (uint8)Active);` |
| `:3281-3286` | **`:3342-3347`** | +61 | the `TOGGLING-SUBSET` log |

📌 **The brief asked me to confirm the `Snap->SessionIndex` stamping site. CONFIRMED, and it is
stronger than "one line apart":** the labels.jsonl image name is built from `Snap->SessionIndex` at
**`:1978`**, the record from the same `Snap` at **`:1980`**, and `AccumulateFrameEvents` is handed
`Snap->SessionIndex` at **`:1982-1983`** — **three consecutive statements in one loop iteration over
one `Snap`.** §15.2's "impossible by construction" holds at `be4dd1b`.

**Model validation, done before anything below was written.** The tick-by-tick reconstruction in
§1.1 was checked against a **banked bench artifact** —
`_bench_sessions_bank\P9_P9_B\session_20260902-112711`, `run.json` `pre=4 positive=8 post=4 settle=2`
(i.e. `IAI.Capture.Config 2 4 8 4 0`), the config `C-1`/`C-3` use:

- `annotation.json` blink events read **`[16,17,21,22]`** and **`[40,41,45,46]`** — the certified
  `(n, n+1, n+5, n+6)` cadence at `n = 16` and `n = 40`. The reconstruction predicts exactly this.
- The same leg's `corrupted_texture` event (a `FireWindow`-source anomaly) reads **`[27..34]`, 8
  frames** — which the reconstruction also predicts exactly, including *both* of its edges.
- `labels.jsonl` rows: `session_index` **15** carries the fire (`anomaly_present=true`), **16..22**
  carry it, and **23 carries no anomaly row at all** (`anomaly_present=false`). Predicted.

⇒ **The model reproduces three independent artifact facts it was not fitted to.** That is why §1.1's
driver table is offered as a reading of the code rather than a guess about it.

---

### §1.1 (1a) THE FOUR FLIPS — which code path drives each

#### §1.1.0 The first fact, because it changes the shape of the answer

🚨 **`FAnomaly_Blinking::Apply` NEVER HIDES ANYTHING.** `Anomaly_Blinking.cpp:10-71` resolves targets,
resolves the half-period, and sets `FramesSinceToggle = 0` (`:64`), `bHiddenPhase = false` (`:65`),
`bActive = Targets.Num() > 0` (`:66`). **There is no `SetActorHiddenInGame` call anywhere in
`Apply`.** ⇒ **the FIRST hide is not driven by `Apply`; it is driven by the `Tick` toggle like the
two interior flips.** (Contrast `missing_object`, which hides inside `Apply`.)

There are exactly **two** sites in the whole file that change actor visibility:

| site | file:line | call |
|---|---|---|
| **the `Tick` while-loop toggle** | `Anomaly_Blinking.cpp:91` | `Actor->SetActorHiddenInGame(bHiddenPhase);` |
| **`Revert`** | `Anomaly_Blinking.cpp:106` | `Actor->SetActorHiddenInGame(false);` |

#### §1.1.1 The engine-tick reconstruction for one burst

Config `2 4 8 4 0` (`SettleFrames K = 2`, `PreFrames 4`, `PositiveFrames 8`, `PostFrames 4`),
`HalfPeriodFrames = 3` (`Anomaly_Blinking.h:28`, compiled; the effective value is echoed — see §1.4).
`T0` is the tick on which `BeginFire()` runs. `n` is the first session index of the Positives phase.

Per engine tick the order is: **capture subsystem `Tick`**, then **injector subsystem `Tick`**
(ordering discussed in §1.2.iii).

| tick | capture subsystem `Tick` (`AnomalyCaptureSubsystem.cpp:539-672`) | injector tick → `FAnomaly_Blinking::Tick` | armed `session_index` |
|---|---|---|---|
| **T0** | last frame of the preceding capturing phase armed (`si = n−1`); `PhaseFramesLeft` hits 0 → **`BeginFire()`** (`:613` LeadIn or `:650` PostGap) → `Apply` | `FramesSinceToggle` 0→1 | **n−1** |
| T1 | `SettleAfterFire`, **no capture** (`:616-619`) | 1→2 | — |
| **T2** | `SettleAfterFire` ends → `Phase = Positives`, **no capture** | 2→3 ⇒ **FLIP 1 → HIDDEN** | — |
| **T3** | `Positives`: arm | 0→1 | **n** |
| T4 | arm | 1→2 | n+1 |
| **T5** | arm | 2→3 ⇒ **FLIP 2 → VISIBLE** | **n+2** |
| T6 | arm | 0→1 | n+3 |
| T7 | arm | 1→2 | n+4 |
| **T8** | arm | 2→3 ⇒ **FLIP 3 → HIDDEN** | **n+5** |
| T9 | arm | 0→1 | n+6 |
| **T10** | arm; `PhaseFramesLeft` hits 0 → **`BeginRevert()`** (`:623`) ⇒ **FLIP 4 → VISIBLE**; *then* `FinalizeArmedLabel()` (`:670`) | `bActive == false` ⇒ `Tick` returns at `:75-78` | **n+7** |
| T11 | `SettleAfterRevert` | — | — |

**The sampled active bit.** `SampleDeferredActiveState()` runs **first** in the capture `Tick`
(`:561`) and fills the *previous* armed frame's `FireActive` from `FActor->IsHidden()` **now**
(`:2520-2562`, the read at **`:2558`**). `blinking`'s state source is **`ActorHidden`**
(`AnomalyCaptureSubsystem.cpp:255`). So `active(frame armed at T)` = the hidden state **after**
injector tick `T`:

| frame | sampled at | state after that tick's injector tick | `ActiveByIndex` |
|---|---|---|---|
| n−1 | T1 | visible (flip 1 is at T2) | 0 → **AMBER** |
| **n** | T4 | HIDDEN | **1 → RED** |
| **n+1** | T5 | HIDDEN | **1 → RED** |
| n+2 | T6 | visible | 0 → AMBER |
| n+3 | T7 | visible | 0 → AMBER |
| n+4 | T8 | visible | 0 → AMBER |
| **n+5** | T9 | HIDDEN | **1 → RED** |
| **n+6** | T10 | HIDDEN | **1 → RED** |
| n+7 | T11 | — | **no row for this fire at all** (see below) |

⇒ `frame_indices` = **`{n, n+1, n+5, n+6}`** — the certified cadence, derived rather than assumed,
and confirmed against `[16,17,21,22]` in the banked artifact.

🚨 **`n+7` IS NOT "LABELLED VISIBLE" — IT IS OUTSIDE THE EVENT ENTIRELY.** On T10 the switch calls
`BeginRevert()` (`:623`) which clears `LiveFires` (`AnomalyAutoInjectorSubsystem.cpp:697-710`), and
`FinalizeArmedLabel()` runs **after** the switch (`:670`) and reads `Auto->GetLiveFires()` (`:2342`)
— **now empty**. So `Snap->Fires` for `n+7` is empty: no `labels.jsonl` anomaly row for that fire, no
`AffectedFrames` entry, no `ActiveByIndex` entry. Confirmed in the artifact (`session_index 23`,
`anomaly_present=false`). ⚠ **This is async-path behaviour.** The sync fallback builds its record
inline at `:2211-2212`, **before** `BeginRevert`, so it would include the fire; Bates runs the async
SVE path.

#### §1.1.2 THE DRIVER TABLE — the answer to 1a

| flip | labels place it at | driver | file:line | call made |
|---|---|---|---|---|
| **1 — first hide** | visible→hidden **at n** | **`Tick` while-loop toggle**, on tick **T2**, an **uncaptured settle tick** | `Anomaly_Blinking.cpp:80-97`, the call at **`:91`** | `Actor->SetActorHiddenInGame(true)` |
| **2 — mid-event show** | hidden→visible **at n+2** | **`Tick` while-loop toggle**, on tick **T5**, the **arm tick of n+2**, *after* the arm | `Anomaly_Blinking.cpp:80-97`, the call at **`:91`** | `Actor->SetActorHiddenInGame(false)` |
| **3 — second hide** | visible→hidden **at n+5** | **`Tick` while-loop toggle**, on tick **T8**, the **arm tick of n+5**, *after* the arm | `Anomaly_Blinking.cpp:80-97`, the call at **`:91`** | `Actor->SetActorHiddenInGame(true)` |
| **4 — final show** | event ends after **n+6** | **`Revert()`**, on tick **T10**, the **arm tick of n+7**, *after* the arm | `Anomaly_Blinking.cpp:100-113`, the call at **`:106`** | `Actor->SetActorHiddenInGame(false)` |

**Flip 4's call chain, in full:**
`UAnomalyCaptureSubsystem::Tick` `:623` → `BeginRevert()` `:2129-2137` → `:2133`
`Auto->RevertAllLiveFires()` → `AnomalyAutoInjectorSubsystem.cpp:697-710`, `:705`
`Injector->RevertAnomaly(Fire.Id)` → `AnomalyInjectorSubsystem.cpp:472-490`, `:486` `(*Found)->Revert()`
→ `FAnomaly_Blinking::Revert()` `Anomaly_Blinking.cpp:100-113`, the hide-clear at `:106`.

**Flip 1/2/3's call chain, in full:**
`UAnomalyInjectorSubsystem::Tick` `AnomalyInjectorSubsystem.cpp:184-218`, the dispatch loop at
`:193-199` (`Pair.Value->Tick(DeltaTime)` at `:197`) → `FAnomaly_Blinking::Tick`
`Anomaly_Blinking.cpp:73-98` → the `while` at `:81-97`, the hide/show at `:91`.

**Event start, for completeness:** `BeginFire()` `:2106-2127` → `:2117`
`Auto->TryFireSpecific(...)` (`AnomalyAutoInjectorSubsystem.cpp:407-476`, `:456`
`Injector->ApplyAnomaly`) or `Auto->TryFireOnce()` → `AnomalyInjectorSubsystem.cpp:445-470`, `:454`
`(*Found)->Apply(...)` → `FAnomaly_Blinking::Apply` — **which arms the phase machine and hides
nothing.**

#### §1.1.3 SAME SITE OR DIFFERENT SITES — the explicit answer the brief asks for

> **The two OUTER flips do NOT share a call site. The two INTERIOR flips share a call site with the
> FIRST outer flip, not with each other exclusively.**

- **Flips 1, 2 and 3 are the SAME statement**, `Anomaly_Blinking.cpp:91`, reached through the same
  chain, differing only in the boolean argument.
- **Flip 4 is a DIFFERENT statement**, `Anomaly_Blinking.cpp:106`, reached through a different chain
  (capture FSM → auto-injector → injector → `Revert`).

📌 **So the two partitions do not coincide, and this is stated as arithmetic:**

| partition | grouping |
|---|---|
| **by call site** | `{1, 2, 3}` at `:91` · `{4}` at `:106` |
| **by observed agreement on Bates** (ledger §8.6a) | `{1, 4}` match · `{2, 3}` each +1 late in pixels |

⛔ **NO INFERENCE IS DRAWN FROM THAT NON-COINCIDENCE. It is not evidence for or against anything.**

#### §1.1.4 ⛔ ONE STRUCTURAL ASYMMETRY, RECORDED AS STRUCTURE AND EXPLICITLY NOT AS A LEAD

Reading the table by *when the driver runs relative to the arm of the first frame it affects*:

| flip | driver runs on | relative to that frame's arm |
|---|---|---|
| 1 | tick **T2** — an **uncaptured** settle tick | strictly **before** (a whole tick earlier) |
| 2 | tick **T5** — the **arm tick** of `n+2` | **after** the arm, same tick |
| 3 | tick **T8** — the **arm tick** of `n+5` | **after** the arm, same tick |
| 4 | tick **T10** — the **arm tick** of `n+7` | **after** the arm, same tick |

**Why flip 1 differs is pure arithmetic of the shipped config:** the injector ticks once on the
`Apply` tick and once per settle tick, so it has accumulated `1 + K = 1 + 2 = 3` ticks by the last
settle tick, and `HalfPeriodFrames` is also **3**. At any other `(K, half-period)` pair flip 1 lands
somewhere else. It is a coincidence of `2 4 8 4 0` + half-period 3, not a designed property.

⛔ **THIS IS NOT OFFERED AS A MECHANISM FOR (B), AND HERE IS THE MEASURED REASON IT CANNOT BE ONE AS
IT STANDS: the bench has exactly this structure and reads 16/16 ALIGNED** (journal 067 §12.6). An
arm-tick toggle still reaches that same tick's render, because
`FRendererModule::BeginRenderingViewFamilies` calls `World->SendAllEndOfFrameUpdates()` before
kicking off the view families — `SceneRendering.cpp:4528`, whose own comment reads *"Guarantee that
all render proxies are up to date before kicking off a BeginRenderViewFamily."* — and the redraw runs
after `UWorld::Tick` within the same `FEngineLoop::Tick` iteration (`GEngine->Tick` at
`LaunchEngineLoop.cpp:5363`). That is the `F-1` guarantee already on this project's record.
**⇒ the asymmetry above is a property of the plugin present on BOTH hosts; it is a map, not a
candidate.**

⚠ One further engine detail worth having in the map: `AActor::SetActorHiddenInGame` is guarded —
`Actor.cpp:4556-4563`, `if (IsHidden() != bNewHidden)` — so **a set to the value already held is a
complete no-op and marks nothing dirty.** Flip 4 therefore does real work only when the event ends
in the hidden phase (which, at this config, it always does).

---

### §1.2 (1b) THE CLOCKS

#### (i) What calls `FAnomaly_Blinking::Tick`, and how often relative to a world tick

- **Caller:** `UAnomalyInjectorSubsystem::Tick`, `AnomalyInjectorSubsystem.cpp:184-218`; the dispatch
  loop is `:193-199` and the call is `:197`, guarded only by `Pair.Value->IsActive()` (`:195`).
- **Cadence:** `UAnomalyInjectorSubsystem` is a `UTickableWorldSubsystem`
  (`AnomalyInjectorSubsystem.h:12`). It does not override `GetTickableTickType`, a tick group, or any
  priority. ⇒ **exactly once per world tick**, unconditionally — **not** per captured frame, **not**
  per rendered frame, **not** per burst.
- ⚠ **`DeltaSeconds` is ignored.** `FAnomaly_Blinking::Tick(float DeltaSeconds)`
  (`Anomaly_Blinking.cpp:73`) never reads its parameter. `FramesSinceToggle` counts **calls**
  (`:80`), and the flip condition is `while (FramesSinceToggle >= HalfPeriodFrames)` (`:81`). **The
  blink clock is a call counter with no time in it at all** — this is the `m23`/`P3a` fix (seconds →
  frames) still in force.

#### (ii) How the label side derives its indices

| artifact | derived from | file:line |
|---|---|---|
| the arm | `Snap.FrameCounter = GFrameCounter`; `Snap.SessionIndex = SessionFrameIndex`; then `++SessionFrameIndex` | `AnomalyCaptureSubsystem.cpp:2157`, `:2158`, `:2180` |
| PNG name | `Actual_Frames/frame_%05d.png` from `Snap->SessionIndex` | `:1978` |
| `labels.jsonl` row | `BuildLabelRecordForSnapshot(*Snap, …)` → `BuildFrameLabelRecord(… Snapshot.FrameCounter, Snapshot.SessionIndex …)` → `frame_index` and `session_index` fields | `:1980`; `AnomalyLabelWriter.cpp:367-372`, fields at `:45-46` |
| `frame_indices` | `AccumulateFrameEvents(…, Snap->SessionIndex, …)` → `Ev->ActiveByIndex.Add(SessionIndex, Active)` → at `FinishRun` the keys with `Active == 1`, sorted | `:1982-1983`; `:3232`; `:3289-3339` (`ActiveIdx` built `:3291-3299`, adopted `:3322`, sorted `:3338`) |

✅ **CONFIRMED, and the brief's `~:1917-1922` maps to `:1978-1983` at `be4dd1b`.**
📌 **`labels.jsonl.frame_index` IS `GFrameCounter` at arm time.** Not a derived index, not a
re-count — the raw engine frame counter, carried by value in the snapshot.

#### (iii) One clock or two? — **TWO, AND THEY ARE NEVER CONVERTED**

| clock | unit | increments at | file:line |
|---|---|---|---|
| **A — the toggle clock** `FramesSinceToggle` | **one world tick** in which blinking is active | `++FramesSinceToggle` | `Anomaly_Blinking.cpp:80` |
| **B — the frame clock** `SessionFrameIndex` | **one armed captured frame** | `++SessionFrameIndex` | `AnomalyCaptureSubsystem.cpp:2180` (async) / `:2255` (sync) |
| *(C — the run-accounting counter)* `CaptureGameTicks` | one world tick while `bRunBegun` | `++CaptureGameTicks` | `:550` |

**Where are A and B reconciled? NOWHERE ARITHMETICALLY.** There is no code anywhere in the plugin
that converts ticks to frames or frames to ticks for labelling purposes. They meet at exactly **one**
place, and it is a **sample, not a conversion**: `SampleDeferredActiveState` (`:2520-2562`) reads
`FActor->IsHidden()` (`:2558`) — the *state* clock A produced — at a moment scheduled by clock B (one
capture tick after the arm). **The only coupling between the two clocks is the value of a boolean
read at a time chosen by the other clock.**

**Clock B advances only in capturing phases.** `CaptureCurrentFrame()` is called from `LeadIn`
(`:612`), `Positives` (`:622`) and `PostGap` (`:632`) only. `SettleAfterFire` (`:616-619`) and
`SettleAfterRevert` (`:626-629`) tick clock A and clock C but **not** clock B.

#### (iv) `ticks_per_captured_frame = 1.3556` — as arithmetic only

**Definition:** `capture_game_ticks / total_frames`, `AnomalyLabelWriter.cpp:548-549`
(numerator `TickPin->GameTicks` = `CaptureGameTicks`, `AnomalyCaptureSubsystem.cpp:550`). Closed as a
non-finding in journal 067 §11.4; the bench reproduces `1.3556` exactly (122 / 90).

**Run-wide, the arithmetic is:**

| quantity | value at `2 4 8 4 0`, half-period 3 |
|---|---|
| a 3-tick half-period, expressed in **run-average captured frames** | `3 / 1.3556` = **2.213** |
| a full 6-tick period, in run-average captured frames | **4.426** |
| an 8-frame Positives window, in ticks | `8 × 1.3556` = **10.84** |

🚨 **BUT THE RUN-WIDE RATIO IS NOT THE RATIO INSIDE AN EVENT WINDOW, AND THAT IS THE WHOLE POINT OF
QUOTING IT HERE.** Per burst the FSM spends **12 capturing ticks** (8 Positives + 4 PostGap) and
**4 non-capturing ticks** (2 + 2 settle), giving a structural **16 / 12 = 1.3333**; the run-wide
`1.3556` is that plus the `LeadIn` and `DrainTail` edges. **Inside the Positives phase every tick
captures, so the local ratio is exactly `1.0`:** 3 ticks = 3 captured frames.

⇒ **the observed cadence is 2 hidden / 3 visible / 2 hidden not because 3 ticks ≈ 2.2 frames, but
because one hidden tick of each 3-tick hidden phase is spent on a NON-CAPTURING tick** — T2 for the
first hidden phase (a settle tick), and T10 for the second (the tick whose frame is dropped from the
event by flip 4, §1.1.1). This is measured, not asserted: the artifact reads `[16,17,21,22]`.

⛔ **STATED AS ARITHMETIC, NOT AS INTERPRETATION.** `ticks_per_captured_frame` is a run-average and
is **not** a per-window conversion factor; using it as one would be wrong by construction.

---

### §1.3 (1c) THE VERBOSE TOGGLE LINE AND WHAT ITS PREFIX CARRIES

#### §1.3.1 The line, quoted exactly

`Source/AnomalyInjector/Private/Anomalies/Anomaly_Blinking.cpp:95-96`:

```
		UE_LOG(LogAnomaly, Verbose, TEXT("blinking toggle -> %s (%d actors)."),
			bHiddenPhase ? TEXT("HIDDEN") : TEXT("VISIBLE"), Affected);
```

Emitted text is therefore exactly one of:

```
blinking toggle -> HIDDEN (1 actors).
blinking toggle -> VISIBLE (1 actors).
```

✅ **CONFIRMED: the line stamps ONLY the phase word and the actor count.** No frame index, no session
index, no tick count, no `FramesSinceToggle`, no `RequestId`, no target name, no run-relative
anything. `Affected` (`:86`, `:92`) counts targets whose weak pointer resolved this tick — it is a
target count, not an index.

⚠ It is emitted **inside** the `while` loop (`:81-97`), so a half-period shorter than one tick would
print more than one line per tick. At `HalfPeriodFrames >= 1` (`Anomaly_Blinking.h:29`, clamped
`[1..600]`) the loop body runs **at most once per tick**.

#### §1.3.2 The engine prefix — what Bates' sealed build already prints

The line's identity comes entirely from the **engine's log-line prefix**, which the plugin does not
control and does not need to.

| element | value | engine `file:line` |
|---|---|---|
| composition of the prefix | `FOutputDeviceHelper::AppendFormatLogLine` | `Runtime/Core/Private/Misc/OutputDeviceHelper.cpp:10-76` |
| **UTC timestamp with milliseconds** | `[%Y.%m.%d-%H.%M.%S:%s]` | `OutputDeviceHelper.cpp:29` (the `ELogTimes::UTC` case, `:28-31`) |
| **the bracketed frame counter** | `[%3llu]` of **`GFrameCounter % 1000`** | `OutputDeviceHelper.cpp:30` |
| category + verbosity | `LogAnomaly: Verbose: ` (verbosity is printed because it is not `Log`) | `OutputDeviceHelper.cpp:53-63`, the verbosity clause `:58-62` |
| the log **file** device routes through it | `FOutputDeviceFile::Serialize` → `FormatCastAndSerializeLine` → the same builder with `GPrintLogTimes` | `Runtime/Core/Private/Misc/OutputDeviceFile.cpp:535`, `:557`; `OutputDeviceHelper.cpp:110-123` |

**Is it on by default?** The prefix is driven by `GPrintLogTimes`, whose value comes from the console
variable **`log.Timestamp`, declared with default `1`** = `ELogTimes::UTC`:
`Runtime/Launch/Private/LaunchEngineLoop.cpp:5678-5687` (default `1` at `:5680`), pushed into
`GPrintLogTimes` by `CVarLogSinkFunction` at `:5700-5718` (`case 1: GPrintLogTimes = ELogTimes::UTC;`
at `:5713`), the sink being called from the engine tick at `:5156`.

⚠ **`GPrintLogTimes`'s own C++ initialiser is `ELogTimes::None`** (`CoreGlobals.cpp:386`); the UTC
value arrives via the cvar sink. Overrides that a host build can apply, all of them engine-standard
and all of them **outside our reach on a sealed build**: `[LogFiles] LogTimes=` in `Engine.ini`
(`LaunchEngineLoop.cpp:5731-5762`, `CheckForPrintTimesOverride`, called at `:5947`) and the
command-line switches `-LOGTIMES` / `-NOLOGTIMES` / `-LOCALLOGTIMES` / `-LOGTIMESINCESTART` /
`-LOGTIMECODE` (`:5764-5787`).

✅ **MEASURED ON THIS BOX'S PACKAGED DEVELOPMENT BUILD, so this is a reading and not only a source
default.** From `Builds\BenchGate\Windows\StackOBot\Saved\Logs\StackOBot.log`:

```
[2026.09.02-10.07.33:125][  4]LogAnomaly: blinking: matched 1 actor(s) for '=StaticMeshActor_49' at half-period 3 frame(s).
```

⇒ the prefix is present, UTC with milliseconds, followed by `[  4]` = `GFrameCounter % 1000`. In the
same file, `Select-String 'blinking toggle ->'` returns **0 hits** — the direct confirmation that the
toggle line is absent from a run that did not raise `LogAnomaly` to `Verbose`.

**So the expected shape of a toggle line on Bates, using only what its build already prints:**

```
[YYYY.MM.DD-HH.MM.SS:mmm][fff]LogAnomaly: Verbose: blinking toggle -> HIDDEN (1 actors).
```

#### §1.3.3 🎯 THE ANCHOR — and it is exact, not approximate

**`GFrameCounter` is incremented at `LaunchEngineLoop.cpp:5568`, which is AFTER `GEngine->Tick(...)`
at `:5363`.** Therefore **everything executed inside one `UWorld::Tick` — the capture subsystem's arm
and the injector subsystem's toggle alike — observes the SAME `GFrameCounter` value**, and so does
the log device when it serialises the line, since that happens on the game thread inside the same
loop iteration.

⇒ **the toggle line's bracketed number equals `labels.jsonl.frame_index % 1000` of the frame armed on
that same tick.**

✅ Cross-checked against the artifact: in `session_20260902-112711`, `session_index` 15→`frame_index`
20, 16→23, …, 22→29, 23→30, 24→33. **Contiguous inside a capturing phase, jumping by `K = 2` across
each settle boundary** — exactly the reconstruction in §1.1.1.

⚠ **The modulo is the only lossy part, and it is not lossy in practice:** a 90-frame capture spans
well under 1000 engine frames, so within one bundle the 3-digit field is unique unless the run
straddles a multiple of 1000, in which case the wrap is visible as a decreasing sequence and
`labels.jsonl`'s full `frame_index` resolves it.

⚠ **A render-thread line's `[fff]` is NOT its own render frame.** `GFrameCounter` is the game
thread's counter; a line serialised from the render thread prints whatever value the game thread has
reached. This matters only for §1.4's `keyed frame … submitted` line, which carries its own
`rtframe=` and its own `id=` and therefore does not need the prefix.

---

### §1.4 (1d) THE BRACKETING LINES

Every line below is **`Log` verbosity** (i.e. present without raising anything) on `LogAnomaly` or
`LogAnomalyCapture`, and is emitted per captured frame or per event. Column **"no-flags?"** answers:
*is it present on a Bates run with census OFF and mask OFF, which is what `C-1`/`C-3` run?*

#### §1.4.1 Per captured frame

| line (exact format string) | file:line | thread | carries | no-flags? | bound |
|---|---|---|---|---|---|
| `Capture(sve): SVE-WANT-TRACE arm %d/%d requestId=%llu gameFrame=%llu pendingAfter=%d` | `AnomalySveCapturer.cpp:39-41` | **game**, inside the arm | **`requestId` AND `gameFrame` (= `GFrameCounter`)** | ✅ yes | 🚨 **FIRST 64 ONLY** (`HandshakeTraceLimit = 64`, `AnomalySveCapturer.h:38`; gate `:32-35`) |
| `Capture(sve): SVE-WANT-TRACE publish %d/%d familyFrame=%u wanted=%d requestId=%llu pendingBefore=%d` | `AnomalySveCapturer.cpp:68-70` | **render** | `familyFrame`, `requestId` | ✅ yes | 🚨 **FIRST 64 ONLY** |
| `Capture(sve): keyed frame id=%llu submitted (rtframe=%u, fmt=%d, rect=%dx%d, dualPath=%d).` | `AnomalySveCapturer.cpp:151-153` | **render** | **`id` (= `RequestId`)**, `rtframe` | ✅ yes | **unbounded — one per submitted frame** |

📌 **`RequestId` ↔ `session_index` is a constant offset within a run.** Both are minted in the same
block: `RequestId = ++CaptureRequestSerial` (`:2155`) and `Snap.SessionIndex = SessionFrameIndex`
(`:2158`), then `++SessionFrameIndex` (`:2180`). `SessionFrameIndex` is reset to 0 at `StartRun`
(`:1357`); **`CaptureRequestSerial` is not reset** (`AnomalyCaptureSubsystem.h:222`, member
initialiser `0`), so the offset is `1` for the first run of a process and larger afterwards — but it
is **constant within any one run** and is recoverable by pairing a single `arm` trace line with its
`labels.jsonl` row. ⚠ It holds only while every frame takes the async path; a sync fallback bumps
`SessionFrameIndex` without minting a `RequestId` (`:2255`) and announces itself at `:2185`
(*Verbose*).

#### §1.4.2 Per event

| line | file:line | thread | marks | no-flags? |
|---|---|---|---|---|
| `blinking: matched %d actor(s) for '%s' at half-period %d frame(s).` | `Anomaly_Blinking.cpp:68-69` | game | **event START**, and it **reports the EFFECTIVE half-period** | ✅ yes |
| `IAI.Apply '%s' -> %s.` | `AnomalyInjectorSubsystem.cpp:468` | game | event START (applied / not applied) | ✅ yes |
| `Auto.FireSpecific: '%s' on '%s' -> %s.` (targeted) | `AnomalyAutoInjectorSubsystem.cpp:473-474` | game | event START + target name | ✅ yes |
| `Auto.Fire: '%s' on '%s' -> %s.` (auto-pool) | `AnomalyAutoInjectorSubsystem.cpp:369-370` | game | event START + target name | ✅ yes |
| **`IAI.Revert '%s' -> reverted.`** | `AnomalyInjectorSubsystem.cpp:488` | game | **event END — this is flip 4's own receipt** | ✅ yes |
| `Capture: burst %d fired nothing (zero-match / empty) — negatives only.` | `AnomalyCaptureSubsystem.cpp:2122-2123` | game | a burst that did not fire | ✅ yes |

⚠ **`FAnomaly_Blinking::Revert()` itself logs NOTHING** (`Anomaly_Blinking.cpp:100-113`). Flip 4's
only log evidence is `IAI.Revert 'blinking' -> reverted.` from its caller.

🎯 **Consequence, and it is a directly checkable expectation for the bundle: at `2 4 8 4 0` with
half-period 3, ONE blinking event produces EXACTLY THREE toggle lines, in the order
`HIDDEN · VISIBLE · HIDDEN`, followed by `IAI.Revert 'blinking' -> reverted.`** A fourth toggle line,
a different order, or a different count means the half-period or the burst config differs from the
assumption — which the `blinking: matched … at half-period N frame(s).` line reports independently.

#### §1.4.3 Once per run (frames the whole bundle)

`=== Capture run STARTED: …` (`:1675`) · `=== Capture run FINISHED: …` (`:3030`) ·
`Capture: RESOLUTION DELTA (3-rect) …` (`:1946`, first drained frame) ·
`Capture(m28): MEASURED FROM THE FIRST WRITTEN FRAME …` (`:1850`) ·
`Capture: grab point EFFECTIVE = …` (`:1509`) ·
`Capture: TOGGLING-SUBSET id=… positives=%d of %d fire-active frame(s)` (`:3342-3347`, **once per
toggling event at `FinishRun`** — carries the counts, not the indices) ·
`Capture(sve): SVE-WANT-SUMMARY …` (`:3013`) · `Capture(sve): key ring …` (`:3005`) ·
`Capture(tickpin): …` (`:1584` / `:1593` / `:1600`).

#### §1.4.4 ⛔ ABSENT on a no-flags Bates run — do not look for them

Every `Census: …` line (census OFF) and every `Capture(mask): M23 / M26S1 / M26S2 / M26S3 / M27 /
M36 …` line, `VETOED-OBJECT`, and `DUAL-PATH COMPARE` (mask OFF / `DualPathReadback` off). These are
the densest per-frame lines the bench log carries, and **they are exactly the ones a no-flags run
will not have.** Their absence is expected, not a fault.

#### §1.4.5 ORDERING GUARANTEES — asked for explicitly, answered honestly

| pair | same thread? | same tick? | ordered? |
|---|---|---|---|
| `SVE-WANT-TRACE arm` ↔ the toggle line | **yes, game** | **yes** (the arm is in the capture subsystem's `Tick`, the toggle in the injector's, both inside one `UWorld::Tick`) | **YES — arm first, toggle second**, and the two carry the **same** `GFrameCounter` |
| `blinking: matched …` / `IAI.Apply` / `Auto.Fire*` ↔ the toggle line | yes, game | the fire lines are on **T0**, the first toggle on **T2** | ordered by tick, two ticks apart |
| `IAI.Revert 'blinking'` ↔ the toggle line | yes, game | revert on **T10**, last toggle on **T8** | ordered by tick |
| `keyed frame … submitted` ↔ anything on the game thread | **NO — render thread** | no | ⛔ **NOT ORDERED.** Interleaving in the file is arbitrary. Use its `id=` field, never its position or its `[fff]` prefix |
| `SVE-WANT-TRACE publish` ↔ game-thread lines | **NO — render thread** | no | ⛔ not ordered; use `requestId=` |

🚨 **THE ORDERING THAT UNDERPINS ROW 1 IS REGISTRATION ORDER, NOT AN ARCHITECTURAL GUARANTEE, AND
THAT IS STATED RATHER THAN ASSUMED.** `UAnomalyCaptureSubsystem` (`AnomalyCaptureSubsystem.h:14`),
`UAnomalyInjectorSubsystem` (`AnomalyInjectorSubsystem.h:12`) and `UAnomalyAutoInjectorSubsystem`
(`AnomalyAutoInjectorSubsystem.h:35`) are all plain `UTickableWorldSubsystem`s; **none** overrides
`GetTickableTickType`, a tick group, or any priority. The relative order is `FTickableGameObject`
registration order. The evidence that capture ticks first is (a) `m20`, which measured
`annotation(G) == pixels(G-1)` on every blink edge and produced `SampleDeferredHidden` /
`SampleDeferredActiveState` precisely to cope with it, and (b) this turn's reconstruction, which
reproduces `[16,17,21,22]`, `[27..34]` and the empty `session_index 23` row only under that order.
⛔ **Recorded as a property of the read, not as a candidate for anything.**

---

### §1.5 (1e) CONCLUSION — WHAT `C-3` CAN AND CANNOT DISCRIMINATE

#### §1.5.1 Per interior flip

| | **flip 2 — mid-event show** (labels `n+2`, eye `n+3`) | **flip 3 — second hide** (labels `n+5`, eye `n+6`) |
|---|---|---|
| **can the bundle place the toggle on a specific captured frame index?** | **YES** — provided the engine prefix is present | **YES** — same condition |
| **by what join** | toggle line's `[fff]` = `GFrameCounter % 1000` ⇒ matched against `labels.jsonl.frame_index % 1000` from `C-3(b)` | identical |
| **resolution** | **EXACT captured-frame index**, one tick = one frame inside the Positives phase (local ratio 1.0, §1.2.iv) | identical |
| **corroborating join, if the prefix is missing** | `SVE-WANT-TRACE arm … gameFrame=…` immediately preceding the toggle line, **if the frame index is ≤ 64** | identical |
| **fallback if both are missing** | **tick-level ORDER only** — the toggle can be placed *after* a named arm line and *before* the next, but not on a frame index | identical |

**What makes it indeterminate — all four are readable from the bundle itself:**

1. **The engine prefix is absent** (`[LogFiles] LogTimes=None` or `-NOLOGTIMES` on that build). Then
   there is no timestamp/frame bracket at all and the only anchor left is ordering.
2. **The event sits beyond captured frame ~64**, so `SVE-WANT-TRACE arm` has stopped
   (`HandshakeTraceLimit = 64`) and the prefix is the *sole* anchor.
3. **`Log LogAnomaly Verbose` did not take**, so there are no toggle lines. ⚠ **That is a READ, not a
   failure** — record it exactly as *"`Log LogAnomaly Verbose` was set and no `blinking toggle ->`
   line appeared"* and send the bundle anyway (`G197` family; already on the card).
4. **The run straddles a `GFrameCounter` multiple of 1000** — visible as a decreasing `[fff]`
   sequence; `labels.jsonl`'s full `frame_index` resolves it.

⚠ **One thing `C-3` CANNOT do, stated so nobody expects it:** it cannot see the **pixels**. It places
the *toggle call*; the eye places the *pixels*; `labels.jsonl` + `frame_indices` place the *label*.
The bundle is a three-way comparison precisely because no single artifact in it observes more than
one of those three.

⚠ **And one thing it cannot do for flip 1 or flip 4:**
- **Flip 1** fires on an **uncaptured settle tick** (T2, §1.1.4). Its toggle line's `[fff]` therefore
  matches **no `labels.jsonl` row at all** — it falls in the `K`-tick gap between `frame_index` of
  `n−1` and of `n`. That is expected and is itself a positive check (`frame_index(n) − frame_index(n−1)`
  should be `K + 1 = 3`).
- **Flip 4** emits **no toggle line**. Its only receipt is `IAI.Revert 'blinking' -> reverted.`, which
  lands on the arm tick of `n+7`.

#### §1.5.2 Label-side facts the bundle should also confirm — free cross-checks, no new commands

These follow from the code and are **host-independent** (labels are, per ledger §8.6a). If any of
them fails on Bates, that is itself a finding and should be **reported raw**.

| expectation | source |
|---|---|
| `n − 1` has a `labels.jsonl` row for the fire and is **not** in `frame_indices` ⇒ **AMBER, never RED** | `:670` `FinalizeArmedLabel` runs after `BeginFire`; sampled visible at T1 |
| **`n + 7` has NO row for the fire at all ⇒ NEITHER red NOR amber** | `BeginRevert` at `:623` precedes `FinalizeArmedLabel` at `:670`; artifact `session_index 23`, `anomaly_present=false` |
| `frame_index` is contiguous across `n … n+7` and jumps by `K + 1 = 3` at each phase boundary | `:2157`; artifact 15→20, 16→23 … 23→30, 24→33 |
| exactly **3** toggle lines per event, `HIDDEN · VISIBLE · HIDDEN` | §1.4.2 |
| `blinking: matched … at half-period N frame(s).` reports the **effective** half-period for that build | `Anomaly_Blinking.cpp:68-69`; resolution at `:35-58` via `AnomalyDefaults::GetHalfPeriodFrames` |

#### §1.5.3 THE PRE-DECLARED THREE-WAY COMPARISON, RESTATED IN THESE TERMS

For **each interior flip**, read the toggle line's anchored captured-frame index `f_toggle` and
compare it with the labels' first frame of the new phase (`n+2` for flip 2, `n+5` for flip 3) and the
eye's first frame of the new phase (`n+3`, `n+6`).

| outcome | reading in the card's words | what it means structurally |
|---|---|---|
| **`f_toggle` = `n+2` / `n+5`** — the log sides with **LABELS** | **the PIXELS are the outlier on this host** | the toggle executed on the tick the labels imply, and the divergence sits **downstream of the visibility call** |
| **`f_toggle` = `n+3` / `n+6`** — the log sides with the **EYE** | **the SAMPLING / LABELLING side is the outlier on this host** | the toggle executed a tick later than the labels imply, and the divergence sits **between the toggle and the label** |
| **mixed across the two flips, or the anchor is unavailable (§1.5.1)** | ⛔ **report raw. Classify nothing.** | — |

⛔ **NO BRANCH IS ASSERTED AND NONE IS PREFERRED. These outcomes name WHERE the divergence sits. They
never name WHY.** ⛔ **Nothing in §1.1–§1.4 is offered as a prior on which branch will fire** — the
bench, which has the identical plugin structure, reads 16/16 ALIGNED, so the plugin structure alone
predicts neither branch.

---

## §2 WHAT WAS NOT DONE, NAMED

- ⛔ **No source file was edited, nothing was built, no leg was run, nothing was cooked, nothing was
  tagged.** Staged bench exe remains `6C80E872`; container unchanged.
- ⛔ **`m38` was not started.** It remains the approved-but-unimplemented plan at journal 067 §16 and
  is the next session's TASK 2.
- ⛔ **No Bates artifact was read** — none exists here. Every measured cross-check in §1 is from this
  box's own bench log and banked bench sessions.
- ⚠ **The `log.Timestamp` value on Bates was not measured** and cannot be from here. §1.3.2 gives the
  engine default (`1` = UTC) and the two override routes; **the bundle answers it directly** — either
  the toggle lines carry a `[…][fff]` prefix or they do not.
- ⚠ **Half-period on Bates was not measured.** §1.4.2's `blinking: matched … at half-period N` line
  reports it in the bundle.

## §3 COMMIT (TASK 1)

Docs-only, one commit: **`3e14385`** `docs(p9): transition-driver and toggle-line anchor source read`.
Files: this journal · `docs/office-rdp-card.md` (a **C-3 READ GUIDE** subsection — read instructions
only, no new steps and no new commands) · `CLAUDE.md` (the TASK-1 pointer flipped to DONE).
**No tag.** ✅ **ACCEPTED by chat, no changes.**

---

## §4 BRIEF 2 — THE TWO DECISIONS, RULED

### §4.1 🔻 JOURNAL 067 §16.4's GATE-(i) MARKER SET IS **SUPERSEDED**

⛔ **Journal 067 is a RECORD and is NOT edited.** The supersession is recorded here, which is where a
reader arriving at `m38` will be.

**What §16.4 asked for and why it could not stand, measured at `be4dd1b`:**

| §16.4 marker | exists? | evidence |
|---|---|---|
| `Census: BEGIN` | ✅ exists — but **only when the census is ON** | `AnomalyCensus.cpp:159`, inside `FAnomalyCensus::Begin` |
| `M36 STENCIL RESERVATION` | ✅ exists — but **inside the mask/census setup block** | `AnomalyCaptureSubsystem.cpp:1483` |
| **`M23 ARM`** | 🔴 **DOES NOT EXIST ANYWHERE** | the mask tokens are `M23 CVAR` (`AnomalyMaskMeasure.cpp:56`), `M23 PASS` (`AnomalyMaskSceneViewExtension.cpp:198`), `M23 REDUCE` (`:401`) |

⇒ as written, gate (i) named one token that does not exist and two that a **no-flags** run does not
emit. **Chat ruled the replacement**, and the ruling also fixes the configuration:

> **Gate (i) runs on the NO-FLAGS configuration — census OFF, mask OFF.** That is the client-shaped
> run, which is what `m38` exists for. **Required markers, all unconditional there:**
> `=== Capture run STARTED` · `grab point EFFECTIVE` · at least one fire line
> (`Auto.Fire` / `Auto.FireSpecific` / `IAI.Apply`) · at least one `IAI.Revert` · and, **as the file's
> own last line**, the close marker.
> Whether **`=== Capture run FINISHED`** lands INSIDE the file depends on the close ordering chosen —
> **PREDICT it from the code before the leg and record the outcome; it is an observation, not a
> pass/fail.**
> A second observation leg with census ON confirming `Census: BEGIN` appears is **optional, recorded
> if cheap, not a gate.**

### §4.2 ✅ `C-3` GAINS AN EARLY-EVENT ANCHOR PREREQUISITE

Ruled **ADD IT**. Card `C-3` prerequisite **4** now says: anchor on an event inside the first ~64
captured frames, so both anchors are available instead of one.

**Computed from the card's actual run config** (`IAI.Capture.Config 2 4 8 4 0`, 90 frames), rather
than assumed. Captured frames `0..89`; lead-in `0-3`; each burst's Positives window is 8 frames:

| burst | event window (session index) | inside the 64-arm trace? |
|---|---|---|
| 1 | **4-11** | ✅ |
| 2 | **16-23** | ✅ |
| 3 | **28-35** | ✅ |
| 4 | **40-47** | ✅ |
| 5 | **52-59** | ✅ |
| 6 | 64-71 | ⛔ entirely outside |
| 7 | 76-83 | ⛔ |
| 8 | 88-89 (frame-cap truncated) | ⛔ |

📌 **The brief said "the first three bursts"; the computed answer is the first FIVE**, and the card
carries the computed one because the brief asked for it to be computed. The bound is
`Handshake.TracedArms < HandshakeTraceLimit` with `HandshakeTraceLimit = 64`
(`AnomalySveCapturer.h:38`, gate `AnomalySveCapturer.cpp:32-35`), and arm *k* is session index
*k−1*, so arms 1..64 cover session indices **0..63**. ✅ `Handshake` is reset per run
(`AnomalySveCapturer.cpp:87-93`, called from `AnomalyCaptureSubsystem.cpp:1400`), so the budget is
per-run and not per-process.

### §4.3 CLAUDE.md gains the mailbox / headless note

Two lines under the workflow rules: briefs arrive as files via
**`D:\IntrusiveAnomalies\_mailbox`** (outside every repo, never written into, never staged), and in
a headless run **the final message is the report**.

---

## §5 `m38` — THE RUN-SCOPED SESSION LOG. **BUILT, GATED, SHIPPED.**

> **Pre-declared gates: `docs/predictions/2026-09-02-m38-run-log.md`, commit `ba0982c`, written and
> committed BEFORE any source existed.** Read that file before reading any result here.
> **Plan: journal 067 §16**, unchanged except gate (i)'s marker set (§4.1) and the new gate (v).

### §5.1 Build identity — `G121`'s quartet, and only the exe half moves

| | |
|---|---|
| **predecessor** | **`6C80E872`** (m37), 241,036,800 B — already archived as `_binary_baselines\StackOBot.exe.m37-census-defaults-6C80E872` and **re-hashed at the archive before the swap** (`A62`): reads `6C80E872`. |
| **new staged exe** | **`F2FA6BCD`**, 241,061,376 B, archived as `_binary_baselines\StackOBot.exe.m38-runlog-F2FA6BCD`, hash-verified **at the archive** after the copy. |
| **container** | ⛔ **UNCHANGED. NO COOK.** Code-only hot-swap (`G103`). |
| **census** | still **compiled OFF**; `master` stays client-inert. |

**A44 on the STAGED artifact, both encodings** (`§8.2`):

| symbol | ascii | utf16 |
|---|---|---|
| `anomaly_log.txt` | 0 | **7** |
| `IAI.Capture.RunLogVerbose` | 0 | **7** |
| `RUNLOG-VERBOSE-PROBE` | 0 | **4** |
| `Capture(runlog)` | 0 | **11** |
| `RunLogDefault` | 0 | **3** |
| *`IsHideTypeAnomaly`* (the runbook's example control) | 0 | **0** |

⚠ **The control reads 0 because it is the KNOWN-STALE one** — renamed at session 053 and already flagged
in the status block. The scan is **not blind**: five new symbols match in UTF-16 and none in ASCII,
which is the expected shape. 📌 The runbook's §8.2 example control should be replaced with a live
symbol; **filed, not fixed** (it is a docs edit inside a gating turn).

### §5.2 Requirements (1)–(8), each with where it is met

| # | requirement | met at |
|---|---|---|
| 1 | `FOutputDevice`, filters **`LogAnomaly` + `LogAnomalyCapture` only**, writes `anomaly_log.txt` beside `annotation.json` | class `AnomalyRunLog.h:10`; filter `AnomalyRunLog.cpp:23-26` (`AcceptsCategory`) applied at `:100`; path built `AnomalyCaptureSubsystem.cpp:1594` |
| 2 | registered at `StartRun` **after `RunDir` exists** | `StartRunLog()` called at `AnomalyCaptureSubsystem.cpp:1339`, immediately after `LastRunDir = RunDir;` and **after** the `MakeDirectory` success check at `:1319-1333`; `GLog->AddOutputDevice` at `:1606` |
| 3 | flush + `GLog->RemoveOutputDevice` on **every** teardown path | `EndRunLog()` at `:1652`; `RemoveOutputDevice` **before** `Close()` at `:1678-1679`. Four call sites: `FinishRun` cancelled branch `:3300` (before `DeleteDirectory` `:3301`) · `FinishRun` last statement `:3423` · `Deinitialize` `:503` · destructor `AnomalyRunLog.cpp:19-22` |
| 4 | `FCriticalSection` around the write, callable from any thread | `AnomalyRunLog.h:34` (`CS`); `CanBeUsedOnAnyThread`/`CanBeUsedOnMultipleThreads` both true at `:26-27`; every entry point takes `FScopeLock` |
| 5 | verbosity never silently changed; separate knob raises **and restores**, both echoed | raise `AnomalyCaptureSubsystem.cpp:1636-1648`; restore `:1655-1667`; the knob is `SetRunLogVerbose` `:1712` |
| 6 | delivery default mirrors `run.json` (auto = `!bDeliveryMode`), three-state `−1/0/1`, console **and** ini | `ResolveRunLogEffective` `:1521-1542` (auto branch `:1539-1540`); ini `RunLogDefault` `:441-446`; console `IAI.Capture.RunLog` `:3874` |
| 7 | `StartRun` echo states **state + path**, loud both ways | ON `:1608-1609`; OFF `:1629-1631`; open-failure `:1620-1625` |
| 8 | post-`EndRun` writer lines go to the main log only — stated | in the echo `:1610-1613`; as the file's **last line** `:1679`; and in the CLOSED line `:1683-1687`, which itself says it is in the main log only |

⛔ **NO ARTIFACT FIELD WAS ADDED.** Gate (iv) measures it: `annotation.json` **48 keys** and
`run_summary.json` **48 keys**, both **IDENTICAL** across the binary change. `P6` does not move.

🎯 **ONE IMPLEMENTATION CHOICE THE PLAN DID NOT SPECIFY, MADE DELIBERATELY AND STATED IN THE FILE
ITSELF: the run log formats every line with `ELogTimes::UTC` rather than with `GPrintLogTimes`**
(`AnomalyRunLog.cpp:100`). ⇒ **the `[GFrameCounter % 1000]` prefix is present in `anomaly_log.txt`
unconditionally, whatever a host's `log.Timestamp` is set to.** That is `P9`'s anchor (journal 068
§1.3.3), and this makes it survive a host that has log timestamps switched off. It changes **no global
state** — it is this file's own format — and the file's header line says so in words.

### §5.3 GATE (i) — normal leg, NO-FLAGS. ✅ **PASS**

⚠ **A finding, and `A48` is what caught it.** The first attempt at this gate issued **no console
commands at all** — and the `EFFECTIVE FOR THIS RUN` echo read **`mask ON … from DefaultGame.ini
[AnomalyCapture] bMaskMeasureDefault`**. **On this bench, "issue no flags" and "the no-flags
configuration" are DIFFERENT THINGS**, because the project ini turns the mask on. That leg
(`M38_G1_NOFLAGS`) is kept as a **mask-ON** observation; the gate was re-run with
`IAI.Capture.Mask 0` to reach the ruled state. 📌 **Read the echo, never the invocation.**

**Leg `M38_G1_MASKOFF`** — census OFF, mask OFF, delivery OFF, run log auto ⇒ ON.
`pose_match=True`, bbox exactly `CALIB_BBOX`, accepted attempt 1. File **48,245 B, 287 lines.**

| required marker | count | |
|---|---|---|
| `=== Capture run STARTED` | 1 | ✅ |
| `Capture: grab point EFFECTIVE` | 1 | ✅ |
| fire line (`Auto.FireSpecific` / `IAI.Apply`) | 8 / 8 | ✅ |
| `IAI.Revert` | 8 | ✅ |
| close marker **as the file's own last line** | 1, and it **is** line 287 | ✅ |

**Also as predicted:** first line is the `#` header · categories present are **exactly**
`LogAnomalyCapture` (246) and `LogAnomaly` (39) and nothing else · `Census: BEGIN`, `M23 PASS`,
`M23 REDUCE`, `M26S*`, `M27 VETO`, `M36 STENCIL` all **0**.

🎯 **OBSERVATION `P-FIN` — CONFIRMED. `=== Capture run FINISHED` IS INSIDE THE FILE** (count 1), as
predicted from the close ordering. Recorded, **not graded**.

✅ **OPTIONAL ITEM, DONE BECAUSE IT WAS CHEAP — leg `M38_OBS_CENSUS_ON`** (`IAI.Capture.Census 1`,
mask ON from the ini): **810 lines, 241,587 B**, containing `Census: BEGIN` ×1, `Census: SUMMARY` ×1,
`M36 STENCIL RESERVATION` ×1, `M23 PASS` ×105, and the close marker. ⇒ journal 067 §16.4's two
surviving markers **do** appear — they were **conditional**, exactly as §4.1 recorded, and that is why
they could not gate a no-flags run.

### §5.4 GATE (ii) — delivery, both directions (`G96`). ✅ **PASS**

| leg | echo, read from the log | file set | verdict |
|---|---|---|---|
| **`M38_G2A_DELIV_AUTO`** | `run log OFF (auto, from delivery=on) - NO anomaly_log.txt will be written.` | `annotation.json` · `labels.jsonl` · `run_summary.json` — **no `anomaly_log.txt`** | ✅ **ABSENT** |
| **`M38_G2B_DELIV_FORCED`** | `run log ON (forced ON, from IAI.Capture.RunLog) -> …\anomaly_log.txt` | the same three **+ `anomaly_log.txt` (87,753 B)** | ✅ **PRESENT** |

⚠ Both legs also confirm the rest of the delivery set is untouched: `run.json` and
`selection_provenance.json` stay suppressed, `labels.jsonl` is still written.

### §5.5 GATE (iii) — the handle-leak test, **done by literally deleting the folder**. ✅ **PASS, both legs**

New harness `CaptureBench/tools/m38_gate3_abort.ps1` (local-only repo). **The session is banked BEFORE
the delete**, so proving the folder can be removed never destroys the artifact that proves it.

| leg | how it ended | file | close marker | `run FINISHED` | **`Remove-Item -Recurse -Force`** |
|---|---|---|---|---|---|
| **`M38_G3_STOP`** | `capture_stop` over the control server's WebSocket, **162 frames in**, reply `{"type":"capture_stopped","running":false,…}` | 63,419 B / **391 lines**, readable | ✅ present, **and it is the last line** | ✅ present | ✅ **SUCCEEDED** |
| **`M38_G3_KILL`** | **hard `Stop-Process -Force`, 152 frames in.** `FinishRun` never ran | 54,273 B / **356 lines**, readable | ⛔ **ABSENT — correctly** | ⛔ absent | ✅ **SUCCEEDED** |

🎯 **The kill leg's last line is a COMPLETE, UNTORN line** (`Capture(sve): keyed frame id=156
submitted …`) — the per-line write-through policy (§0.1 of the predictions) doing exactly what it was
declared to do. **Nothing logged before the kill was lost.**

### §5.6 GATE (iv) — `P-C7` shape, re-anchored at `F2FA6BCD`. ✅ **PASS**

**Pair, pose-matched and same-config:** `M38_BASE` (pre-m38 **`6C80E872`**) vs `M38_G1_NOFLAGS`
(m38 **`F2FA6BCD`**). Both `pose_match=True`, `modal_rot=(0,0,0)`, bbox **exactly**
`(0.0, 485.2, 306.1, 234.8)` = `CALIB_BBOX`, both accepted on attempt 1.

**`m36_s1_pc7_check.py` — and it PROVED ITSELF AGAINST A KNOWN ANSWER FIRST (`A53`/`G96`):** proof 1
(A vs A) clean, proof 2 (A vs perturbed-A) reported 2 differences. Then:

| # | check | result |
|---|---|---|
| 1 | `annotation.json` keyset | **IDENTICAL, 48 keys** — the client key set did not move |
| 2 | event set | **IDENTICAL, 8 events** |
| 3 | `run_summary.json` keyset | **IDENTICAL, 48 keys — zero added keys** |
| 4 | `run_summary` values outside the run-unique set | **identical** |
| 5 | frame count | **identical, 90** |
| 6 | `census_*` keys | **none either side** |

**`compare_sessions.py`:** `annotation.json` differs on `/session_id` + `/video/path` **only**;
`run_summary.json` on `speed_ratio`, `game_clock_speed_ratio`, `sustained_wall_fps` **only**;
`run.json` on `session_id` + `start_time_utc` **only**. All six are in the declared run-unique set.
`Actual_Frames`: **90 vs 90, names EQUAL**, bytes differ on 90/90 — **REPORTED, NOT GATED**, and
expected twice over (`A47`, plus the CaptureBench marker changes every frame by construction).

⚠ **`labels.jsonl` FIRST READ SHOWED 98 DIFFERING FIELDS, AND THAT WAS THE INSTRUMENT, NOT THE
BUILD.** `compare_sessions.py` compares **positionally**, and `G162` records that labels.jsonl **row
order varies run to run** (async writer completion order) — *the control pair exhibits it too*. The
sound instrument is `G161`'s: **key by `session_index`**. Re-read that way:

> **90 rows both sides · key sets IDENTICAL · total field differences across all 90 rows = 90, and
> every one of them is `/t_wall`.**

🎯 **That is TIGHTER than pre-declared.** The predictions allowed `frame_index`, `t` and `t_wall`;
the measurement shows **only `t_wall`** — `frame_index` and `t` are identical, because both legs
started at `start_frame=1` under a paced fixed timestep.

**THE COMPARATOR HALF — ✅ NO COMPARATOR NEEDED EDITING, AND THE BASELINE IS RE-VERIFIED.** Every
`.py`/`.ps1` in `CaptureBench/tools` was re-checked for directory enumeration: `compare_sessions.py:67`
lists **`Actual_Frames` only**; `m36_s1_pc7_check.py:65` globs `Actual_Frames/frame_*.png`;
`a54_oracle.py:213`, `p9_hidden_set.py:224`, `h5_pixel_change.py:40`, `cure_measurement_table.py:52`,
`frame_stats.py:26`, `decode_marker.py:42`, `compare_traces.py:9` glob `Actual_Frames`;
`resolution_delta.py:57` lists a frames dir; the bank sweeps glob for named files;
`run_leg.ps1` banks with `Copy-Item -Recurse` (the new file rides along) and counts `Actual_Frames`
only; `prune_verify.ps1` builds a **symmetric** manifest of two copies. **Nothing enumerates a session
ROOT.**
✅ **And it was exercised, not just read:** `m36_s1_pc7_check.py`, `compare_sessions.py`,
`p9_hidden_set.py`, and (via `run_leg.ps1`) `eval_leg.py` and `check_pose.py` all ran **on sessions
that contain `anomaly_log.txt`**, and all behaved normally.

### §5.7 GATE (v) — the verbosity knob, both directions, in one leg. ✅ **PASS**

**Leg `M38_G5_VERBOSE`** — mask OFF, `IAI.Capture.RunLogVerbose 1`, targeted `blinking`.
⛔ Deliberately **not** run with the harness's `-VerboseAnomalyLog`, which would have raised
`LogAnomaly` externally and made the restore probe meaningless.

**PART 1 — the toggle lines, and this is TASK 1's PRE-DECLARED EXPECTATION MEETING A MEASUREMENT FOR
THE FIRST TIME.** 23 `blinking toggle ->` lines. Interleaved with the fire and revert lines the
sequence is:

```
APPLY HIDDEN VISIBLE HIDDEN REVERT   x 7
APPLY HIDDEN VISIBLE        REVERT   x 1   (the frame-cap-truncated 8th burst)
```

🎯 **Seven complete events, each exactly `HIDDEN · VISIBLE · HIDDEN` followed by
`IAI.Revert 'blinking' -> reverted.` — 21 lines — plus 2 from the truncated burst = 23.** The
prediction was "21 graded, plus 0–2 read-not-graded"; the measurement is **21 + 2**. ⇒ **journal 068
§1.1's driver map is CONFIRMED IN A LOG: three `Tick`-driven toggles at `Anomaly_Blinking.cpp:91`,
then `Revert()` at `:106`.** ⛔ Still no mechanism for `P9` (B) — this is the bench, and the bench
agrees with its labels.

**PART 2 — the restore, proven BOTH WAYS.** Echoes:
`VERBOSITY RAISED - LogAnomaly Log(5) -> Verbose(6) FOR THIS RUN ONLY` and
`VERBOSITY RESTORED - LogAnomaly is back to Log(5)`.

⚠ **A NAIVE `Select-String` COUNTED `RUNLOG-VERBOSE-PROBE raised=1` TWICE, AND THE PREDICTION SAID
ONCE. THE BUILD IS RIGHT AND MY FIRST COUNT WAS WRONG** — `G142`'s shape, a defect in the checker
found while reporting a pass. The second hit is line **314**, which is **my own `VERBOSITY RESTORED`
echo quoting the token in its explanatory text**. Counting only lines whose message **is** the probe:

| probe | emissions | means |
|---|---|---|
| `RUNLOG-VERBOSE-PROBE raised=1` (line 5, `LogAnomaly: Verbose:`) | **1** | the raise took |
| `RUNLOG-VERBOSE-PROBE raised=0` | **0** | **the restore took** — the identical call is suppressed |

📌 **Lesson, and it is mine: an echo that QUOTES its own evidence token corrupts the naive count of
that token.** The fix is to count emissions, not mentions; the wording is otherwise worth keeping,
because it explains the proof to a reader who has only the file.

### §5.8 Two environmental findings, recorded without attribution

1. ⚠ **THE GATE-(iv) BASELINE LEG FAILED `B1` THREE TIMES, THEN PASSED ON ATTEMPT 1.** The three
   failures ran with **~3.6 GB physical memory free** (an editor at 4.56 GB resident — `G97`'s
   permanent environmental fact); the accepted run was after the build, at **~8.1 GB free**, and
   landed on `modal_rot=(0,0,0)` with the bbox **exactly** equal to `CALIB_BBOX`. **Every attempt is
   banked** (`M38_BASE_try1..try3` from the first invocation). ⛔ **ASSOCIATION ONLY — THE CAUSE IS
   NOT ESTABLISHED AND IS NOT ATTRIBUTED** (`G123`: a gate that fails safe still misleads if its
   label names a cause it has not established). The discriminator print showed non-uniform ratios
   with `modal_rot` displaced, which the harness's own table calls the A47 shape rather than
   resolution scope — **that is a reading of the numbers, not a mechanism.**
2. ⚠ **`G141` FIRED ON ME, AND THE MECHANICAL DIFFSTAT CHECK IS THE ONLY REASON IT DID NOT SHIP.**
   Fixing a compile error with `Get-Content -Raw … | Set-Content -Encoding utf8` **added a UTF-8 BOM**
   to `AnomalyCaptureSubsystem.cpp` (first bytes `239,187,191`), and the diffstat went from pure
   insertions to `248 insertions / 1 deletion`. Stripped with `File.WriteAllBytes`; the diffstat
   returned to **247 insertions / 0 deletions**. **The rule already existed and I broke it anyway;
   the pre-commit diffstat habit (`G115`) is what caught it.**

### §5.9 What was NOT done, named

- ⛔ **NO TAG.** Highest remains `m30`. The office batch is now
  `m31 → m33 → m34 → m35 → m36 → m37 → m38`.
- ⛔ **No cook.** Container unchanged; `m38` reaches an office host only when that host's build is next
  updated, and **Bates is sealed**.
- ⛔ **No ini key was added to any shipping config.** `RunLogDefault` and `bRunLogVerboseDefault` are
  *read* if present; neither is written into `DefaultGame.ini` here.
- ⚠ **The `[AnomalyCapture] RunLogDefault` INI ROUTE IS UNTESTED** — only the console route and the
  shared resolve/echo path are proven (`G88`'s standing caveat, the same one `m27` carried for
  `bMaskMeasureDefault`). The auto and console branches are both measured; the ini branch is the same
  code path with a different source string.
- ⚠ **Runbook §8.2's A44 example control `IsHideTypeAnomaly` is STALE** — filed, not fixed.
