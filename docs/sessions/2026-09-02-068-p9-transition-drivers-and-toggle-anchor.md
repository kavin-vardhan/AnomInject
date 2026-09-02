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

---

## §6 ADVERSARIAL RE-READ OF §1

> 🧭 **What this section is.** §1 is now **load-bearing**: the owner's Bates `C-3` bundle will be
> interpreted through it, and through the `C-3` READ GUIDE it produced. This section is a deliberate
> attempt to **BREAK** §1, claim by claim, from source — not to restate it. Each item R1–R5 ends in
> **REFUTED** with a concrete counter-example at `file:line`, or **COULD NOT REFUTE** with the
> specific evidence checked.
>
> ⛔ **DISCIPLINE, RESTATED AND HELD: NO MECHANISM FOR `P9` (B) IS PROPOSED, IMPLIED OR PREFERRED
> ANYWHERE BELOW.** This audits a **mapping**, not the bug. Where a structural fact sits close to
> something that could be read as a mechanism it is marked ⛔ and the reason it is not one is given.
> `G120` governs: an observation and its explanation are separate claims, and a scope decision may
> rest only on the observation. **Nothing here changes what `C-3` runs.**
>
> ⛔ **NO SOURCE FILE WAS EDITED. NOTHING WAS BUILT, COOKED, RUN OR TAGGED.** Every measurement below
> is a read of an artifact that already existed on this box.

### §6.0 Provenance, and a SECOND line-number drift — §1's citations no longer resolve on `master`

**Tree audited: `master` at `970bf1d`** (`git rev-parse HEAD` == `git rev-parse origin/master`), working
tree clean apart from the owner's four untracked `docs/CHAT-HANDOFF-*.md`.

⚠ **§1 was written at `be4dd1b` and says so. `m38` (`7c06c6c`) then added 247 lines to
`AnomalyCaptureSubsystem.cpp` and 18 to its header, so §1's citations into that file are now stale on
`master` — exactly the situation §1.0's own drift table was written to fix, recurring one commit
later.** ⛔ **§1 is a RECORD and is NOT retro-edited.** This is the forward mapping.

🚨 **THE OFFSET IS NOT UNIFORM (+30 … +204). Do not add a constant — grep the token.**

| token | §1 said (`be4dd1b`) | `970bf1d` |
|---|---|---|
| `void UAnomalyCaptureSubsystem::Tick` | `:539` | **`:569`** |
| `++CaptureGameTicks` | `:550` | **`:580`** |
| `SampleDeferredActiveState();` (in `Tick`) | `:561` | **`:591`** |
| `CaptureCurrentFrame()` — LeadIn · Positives · PostGap | `:612` · `:622` · `:632` | **`:642` · `:652` · `:662`** |
| `BeginFire()` call sites | `:613` · `:650` | **`:643` · `:680`** |
| `BeginRevert()` call site | `:623` | **`:653`** |
| `FinalizeArmedLabel();` (in `Tick`) | `:670` | **`:700`** |
| `SessionFrameIndex = 0` (`StartRun`) | `:1357` | **`:1389`** |
| `grab point EFFECTIVE` | `:1509` | **`:1542`** |
| `=== Capture run STARTED` | `:1675` | **`:1873`** |
| `Capture(m28): MEASURED FROM THE FIRST WRITTEN FRAME` | `:1850` | **`:2048`** |
| `RESOLUTION DELTA (3-rect)` | `:1946` | **`:2144`** |
| PNG name · labels record · `AccumulateFrameEvents` | `:1978` · `:1980` · `:1982-1983` | **`:2175` · `:2177` · `:2179-2180`** |
| `BeginFire()` definition | `:2106-2127` | **`:2303-2324`** |
| `BeginRevert()` definition | `:2129-2137` (`:2133`) | **`:2326-2334` (`:2330`)** |
| `RequestId = ++CaptureRequestSerial` · `Snap.FrameCounter` · `Snap.SessionIndex` | `:2155` · `:2157` · `:2158` | **`:2352` · `:2354` · `:2355`** |
| `SveCapturer->ArmWanted(RequestId)` | `:2172` | **`:2369`** |
| `++SessionFrameIndex` (async) | `:2180` | **`:2377`** |
| sync-fallback notice | `:2185` | **`:2382`** |
| sync `Fires = Auto->GetLiveFires()` | `:2211-2212` | **`:2408-2409`** |
| `++SessionFrameIndex` (sync) | `:2255` | **`:2452`** |
| `Auto->GetLiveFires()` in `FinalizeArmedLabel` | `:2342` | **`:2539`** |
| `Snap->FirePos.Reset()` | `:2355` | **`:2552`** |
| `FinalizeArmedLabel` definition | — | **`:2519-2562`** |
| `SampleDeferredActiveState` definition · the `IsHidden()` read | `:2520-2562` · `:2558` | **`:2717-2759` · `:2755`** |
| `Capture(sve): key ring` · `SVE-WANT-SUMMARY` | `:3005` · `:3013` | **`:3203` · `:3211`** |
| `=== Capture run FINISHED` | `:3030` | **`:3228`** |
| `Ev->ActiveByIndex.Add(...)` | `:3232` | **`:3432`** |
| `TOGGLING-SUBSET` log | `:3342-3347` | **`:3543-…`** |
| `CaptureRequestSerial` member (header) | `.h:222` | **`.h:231`** |
| `UAnomalyCaptureSubsystem : public UTickableWorldSubsystem` | `.h:14` | **`.h:15`** |

✅ **Every `AnomalyInjector`-module citation in §1 is UNCHANGED** — that module was not touched by
`m38`. Spot-verified at `970bf1d`: `Anomaly_Blinking.cpp:91` (the toggle), `:95-96` (the log line),
`:106` (the `Revert` hide-clear), `:68-69` (the matched line); `Anomaly_Blinking.h:28` / `:29`;
`AnomalyInjectorSubsystem.cpp:184-218` with the dispatch loop `:193-199` and the call `:197`, `:468`
(`IAI.Apply`), `:488` (`IAI.Revert … reverted`); `AnomalyAutoInjectorSubsystem.cpp:369-370`,
`:473-474`, `:697-710` with `:705`. ✅ **Every ENGINE citation in §1 verified exactly at UE 5.1** —
`OutputDeviceHelper.cpp:10-76`, `:29`, `:30`, `:53-63`; `OutputDeviceFile.cpp:535`, `:557`;
`CoreGlobals.cpp:386`; `LaunchEngineLoop.cpp:5156`, `:5363`, `:5568`, `:5678`, `:5680`, `:5713`,
`:5731`; `Actor.cpp:4556-4563`; `AnomalySveCapturer.h:38`.

---

### §6.1 R1 — THE FLIP TABLE AND THE TICK-ORDER CLAIM

#### §6.1.1 The table itself, under the stated order — **COULD NOT REFUTE**

The §1.1.1 reconstruction was re-derived from the FSM at `970bf1d` (`:639-698`) rather than from §1's
prose, and it reproduces §1's table statement for statement:

- `BeginFire` sets `Phase = SettleAfterFire; PhaseFramesLeft = SettleFrames` (`:2322-2323`), `SettleFrames`
  default **2** (`AnomalyCaptureSubsystem.h:277`), `PreFrames` 4 / `PositiveFrames` 8 / `PostFrames` 4
  (`.h:278-280`). `BeginRevert` is the same shape (`:2332-2333`).
- **`SettleAfterFire` never captures** — the case body is `--PhaseFramesLeft` then the transition
  (`:646-649`); `CaptureCurrentFrame()` appears at **exactly three sites**, `:642` (LeadIn), `:652`
  (Positives), `:662` (PostGap). ⇒ **T1 and T2 are uncaptured, and the tick that flips `Phase` to
  `Positives` is itself uncaptured.** §1's T2 is correct.
- `SampleDeferredActiveState()` is at `:591`, **before** the `ArmedPending` gate and before the switch,
  so it runs on **every** world tick while `bRunning` — the sample for a frame armed at `T` lands on
  world tick `T+1` whether or not `T+1` captures. §1's sample column is correct.
- Flip 4's chain re-walked in full: `:653 BeginRevert()` → `:2330 Auto->RevertAllLiveFires()` →
  `AnomalyAutoInjectorSubsystem.cpp:697-710` (`:705 Injector->RevertAnomaly`) → `Anomaly_Blinking.cpp:106`.
  `LiveFires.Reset()` at `:708` precedes `FinalizeArmedLabel()` at `:700`-of-the-next-statement order,
  so `Snap->Fires` for `n+7` is empty. **Verified in the artifact** (`session_20260902-112711`,
  `session_index 23`, `anomaly_present=false`).

⚠ **One completeness gap, not a defect.** §1.1.0 says there are "exactly two sites in the whole file
that change actor visibility" — true — but `Apply` itself calls `Revert()` at `Anomaly_Blinking.cpp:24`
when `bActive`, so `:106` has a **second reachable caller**. It cannot fire during a capture run:
`StartRun` calls `Auto->SetRunning(false)` (`:1286`), which stops `AdvanceTime`/`ServiceReverts`
(`AnomalyAutoInjectorSubsystem.cpp:121-123`, `:185-187`), and the FSM always reverts before the next
`BeginFire`. ⇒ **no competing flip-4 driver during a run. Could not refute; the table is complete on
that axis.**

#### §6.1.2 ⛔ Alternative orderings — flip 1 CAN land on a captured tick

The brief asks whether any ordering puts flip 1 somewhere other than an uncaptured settle tick. **Two
concrete cases:**

1. **`SettleFrames = 0`.** `BeginFire` leaves `PhaseFramesLeft = 0`; the next tick's
   `SettleAfterFire` case takes neither `if` branch and transitions immediately, still without
   capturing — so the injector has accumulated only `1 + 1 = 2` ticks by the first Positives tick and
   **flip 1 lands on the arm tick of `n`.** ⛔ **Outside `C-3`'s config** (`K = 2`), and §1.1.4 already
   states the general point ("at any other `(K, half-period)` pair flip 1 lands somewhere else").
2. **Reversed subsystem tick order** — see §6.1.3.

#### §6.1.3 🔻 **REFUTED — §1.4.5's EVIDENCE (b). The labels CANNOT see the tick order.**

§1.4.5 says the evidence that capture ticks first is *"(a) `m20` … and (b) this turn's reconstruction,
which reproduces `[16,17,21,22]`, `[27..34]` and the empty `session_index 23` row **only under that
order**."*

**Conjunct (b) is false. All three artifact facts hold under EITHER order.** Re-derived from the same
code, generally (not for one config):

- Reversing the two subsystems moves the injector's first counted tick from `T0` to `T1`, so **every
  flip shifts by exactly +1 tick.**
- `SampleDeferredActiveState` runs at the **top** of the capture `Tick`. Under capture-first it
  therefore samples *before* that tick's injector tick; under injector-first it samples *after* it.
  Writing `S_cf(T)` for the hidden state after injector tick `T` under capture-first:
  `label(frame armed at T)` is `S_cf(T)` under capture-first, and `S_rev(T+1) = S_cf(T)` under
  injector-first. **Identical, for every `T`, at every config.**
- ⇒ **`frame_indices` is EXACTLY ORDER-INVARIANT. The deferred sampler is order-compensating.**
- `[27..34]` (the `corrupted_texture` `FireWindow` window) and the empty `session_index 23` are decided
  **entirely inside `UAnomalyCaptureSubsystem::Tick`** — by `BeginFire` at `:643`/`:680` and
  `BeginRevert` at `:653` running before `FinalizeArmedLabel` at `:700` — and never consult the
  injector at all. **Neither carries any information about the order either.**

🚨 **AND THE PIXEL COLUMN IS *NOT* ORDER-INVARIANT.** A rendered frame shows the state at the end of
its own world tick, so reversing the order shifts the rendered transitions by one frame while leaving
every label untouched. Because flip 1 fires on an uncaptured tick and flip 4 fires inside the capture
`Tick` in both orders, **the two OUTER flips are unaffected and only the two INTERIOR flips move.**

⛔⛔ **THIS IS NOT A MECHANISM FOR `P9` (B), IS NOT A LEAD, AND IS NOT A PRIOR ON ANY `C-3` BRANCH.
Bates' tick order is UNMEASURED and nothing here says anything about it.** Two reasons it must not be
read that way, both from this project's own record:

- **The `{n, n+1, n+2, n+6}` shape is GENERIC TO ANY ONE-TICK OFFSET ANYWHERE IN THE CHAIN and
  therefore carries NO location information.** `m20` measured a pre-fix *label* set of exactly
  `{4,5,6,10}` — the same shape — from an entirely different cause (a sample taken one tick early;
  journal 026 §Bug B). A shape that many causes produce is not evidence for one of them.
- **Whatever the order is, it is a property of the plugin+engine present on BOTH hosts**, and this
  box reads 16/16 ALIGNED (journal 067 §12.6).

**What the refutation actually changes — and it is narrow and useful:**

| | before | after |
|---|---|---|
| what establishes the tick order | (a) `m20` **and** (b) the three artifact facts | **(a) alone.** (b) is vacuous |
| what CAN establish it | — | **only a pixel↔label comparison** — `m20`'s measurement, and the v1/v2 bench legs' 16/16 |
| where it is established | implied: "here" | **exactly and only where pixels have been compared to labels — i.e. this box.** It is **not** established on Bates |

✅ **Evidence (a) is decisive on this box, and stronger than §1 claims.** `m20` measured, in a package,
`annotation(G) == pixels(G−1)` with pre-fix annotation `{4,5,6,10}` against pixels `{4,5,9,10}`
(journal 026 §Bug B, `session_20260715-183542`). Under injector-first the pre-`m20` sample would have
been taken *after* the toggle and the labels would have **matched** the pixels — there would have been
no `m20` bug at all. **The bug's existence uniquely determines capture-before-injector here.**
⇒ **the mechanism §1.4.5 names — `FTickableGameObject` registration order — could not be refuted
either.** `FTickableStatics::NewTickableObjects` is a `TSet` (`Tickable.cpp:15`) drained in a batch at
`Tickable.cpp:119-123`, and `TSet` iterates its `TSparseArray Elements` in index order
(`Set.h:1397-1400`, `:1582-1636`), i.e. **insertion order** for an add-only set — so "registration
order" is the correct description and my first suspicion (that it was pointer-hash order) is **wrong
and is recorded as refuted.** No subsystem overrides `GetTickableTickType`, a tick group or a priority
(checked across all five: `AnomalyCaptureSubsystem.h:15`, `AnomalyInjectorSubsystem.h:12`,
`AnomalyAutoInjectorSubsystem.h:35`, `AnomalySelectorSubsystem.h:12`,
`AnomalyControlServerSubsystem.h:22`).

🎯 **THE CONSEQUENCE FOR THE BUNDLE, AND IT IS A FREE READ ALREADY IN IT.** §1.5.1 and READ GUIDE
item 5 both present flip 1's position as an **expectation**: *"the first hide … happens on a tick that
is deliberately not captured, and that is expected."* **It is not an invariant — it is a
DISCRIMINATOR.** If flip 1's toggle line `[fff]` falls in the `K`-tick gap between `frame_index(n−1)`
and `frame_index(n)`, the reading is one thing; if it lands **on** `frame_index(n)`, it is another.
**Both are readings, neither is a fault, and the card currently tells the reader that only the first
is worth noting.** ⇒ **§1.5.1's flip-1 bullet and READ GUIDE item 5's last sentence need re-wording
(exact text in §6.6).**

---

### §6.2 R2 — THE TWO CLOCKS

#### §6.2.1 🔻 **REFUTED (as worded) — there are THREE meeting points, and one IS an arithmetic conversion**

§1.2(iii) states: *"**Where are A and B reconciled? NOWHERE ARITHMETICALLY.** … They meet at exactly
**one** place, and it is a **sample, not a conversion**."*

**Counter-example 1 — an arithmetic tick→frame-period conversion.**
`UAnomalyCaptureSubsystem::ComputeNominalGameSpan()`, **`AnomalyCaptureSubsystem.cpp:2808-2815`**:

```
return (double)(TicksAtLastArm - TicksAtFirstArm) * (1.0 / (double)VideoFps);
```

`TicksAtFirstArm` / `TicksAtLastArm` are snapshots of **`CaptureGameTicks`** (§1's clock C, one per
world tick, `:580`) taken **at arm moments** by `StampArmWallClock` (`:2794-2806`, called from
`:2365` async / `:2450` sync). They are then multiplied by `1/VideoFps` — **the per-CAPTURED-FRAME
period**. ⇒ **this converts world ticks into seconds using the frame period as the tick period.** It
is the `m33` re-key; it feeds `speed_ratio`, `CheckEarlyPacingWarning` (`:2817-2833`) and the fps
stamp. ⚠ **§1's sentence carries the qualifier "for labelling purposes", and under that qualifier it
survives** — this conversion never reaches `frame_indices`. **The unqualified opening clause and the
"exactly one place" claim do not.**

**Counter-example 2 — a second sample point, and this one DOES reach a label field.**
`ProjectionView()`, **`:2280-2288`**: `const int32 Idx = ViewRing.Num() - 1 - ViewLagFrames;`.
`ViewRing` is fed once per **world tick** by `SampleViewThisTick()` (`:2290-2301`, called from `:617`,
including on non-capturing settle ticks), and `ProjectionView()`'s result is written straight into the
label snapshot at **`:2358`** (`Snap.View = ProjView`) where it drives the projected bbox,
`bbox_valid` and `coverage_ratio`. ⇒ **a TICK-indexed ring consumed at a FRAME-clock moment, and the
lag knob is counted in ticks while its console command calls them frames** —
`IAI.Capture.ViewLag: L=%d frame(s).` (`:810`). ⚠ **Inert today: `ViewLagFrames` default is `0`**
(`AnomalyCaptureSubsystem.h:283`), so `Idx` degenerates to the current tick's view. **The coupling is
structural, not currently active.**

⇒ **Verdict: REFUTED on the wording; the labelling-scoped claim survives.** The correct statement is
in §6.6.

#### §6.2.2 🔻 **REFUTED (narrowly) — "inside the Positives phase every tick captures" has ONE exception, and it is inside `C-3`'s own config**

§1.2(iv): *"**Inside the Positives phase every tick captures, so the local ratio is exactly `1.0`.**"*

The frame-cap check at **`:624-637`** runs **before** the phase switch:
`if (FrameCap > 0 && SessionFrameIndex >= FrameCap && Phase != Idle && Phase != DrainTail)` →
`Phase = DrainTail`. ⇒ **a Positives tick on which the cap trips does not capture, and `BeginRevert` is
never reached for that burst.** With `C-3`'s `IAI.Capture.Start "" png 4242 90 blinking …` and
`IAI.Capture.Config 2 4 8 4 0`, `BurstCount` is **0** (`.h:281`, unlimited) so the run **is** terminated
by the cap, and the eighth burst is the one that trips it. See §6.4 — the same fact refutes R4.

⛔ **This does not disturb §1.2(iv)'s headline**, which is that `ticks_per_captured_frame` (**`1.3556`**;
`AnomalyLabelWriter.cpp:547-549`, verified at `970bf1d`) is a **run average** and not a per-window
conversion factor. **That stands, and is the load-bearing half.**

---

### §6.3 R3 — THE PREFIX ANCHOR

#### §6.3.1 ✅ **COULD NOT REFUTE — and the one un-measured link is now MEASURED**

§1.3.3 derives the anchor from source: `GFrameCounter++` at `LaunchEngineLoop.cpp:5568` is **after**
`GEngine->Tick(...)` at `:5363`, so everything inside one `UWorld::Tick` sees one value. ✅ **Both
lines verified verbatim at UE 5.1, and `:5568` is the ONLY mutation of `GFrameCounter` in that file.**
A Runtime-wide sweep found only three other mutation sites, none reachable in a capture run:
`UnrealClient.cpp:1289` (`HighResScreenshotBeginFrame`, gated on `GIsHighResScreenshot`),
`PreLoadScreenManager.cpp:480`, `Commandlet.cpp:89`.

✅ **No capture arm happens outside `UWorld::Tick`:** `CaptureCurrentFrame()` has exactly three call
sites, all inside the phase switch (`:642`, `:652`, `:662`).

🚨 **BUT §1.3.3's ACTUAL JOIN — `[fff]` == `labels.jsonl.frame_index % 1000` — WAS NEVER MEASURED.**
§1.3.2 measured only that *a prefix exists*; §1.3.3's "✅ Cross-checked against the artifact"
(15→20, 16→23 …) is `labels.jsonl`'s **own internal** series, not a log-to-labels join. The join was
derived, not read.

✅ **IT IS NOW MEASURED, AND §1'S OWN §1.4.1 TABLE CONTAINS THE KNOWN-ANSWER CONTROL IT DID NOT USE.**
`Capture(sve): SVE-WANT-TRACE arm … gameFrame=%llu` (`AnomalySveCapturer.cpp:39-41`) is `Log`
verbosity, on the **game thread**, and prints `(uint64)GFrameCounter` **as a field of its own message**
while also receiving the engine's `[fff]` prefix. ⇒ **`[fff]` must equal `gameFrame % 1000` on that
line, or the prefix was not stamped at log time.** Measured over **all 11 packaged logs** under
`Builds\BenchGate\Windows\StackOBot\Saved\Logs\`:

> **704 arm lines · `[fff] == gameFrame % 1000` on 704 · 0 divergent · 0 without a prefix.**

⇒ **the anchor holds on this box as a measurement, not as a derivation** — and the same one-line check
runs on the Bates bundle for free, for every captured frame up to 64. **`A53`/`G96` shape: it turns a
silent failure mode into a read.**

#### §6.3.2 ⚠ **A REAL DEFERRAL PATH EXISTS, and a deferred line's `[fff]` IS WRONG**

The prefix is composed by `FOutputDeviceHelper::AppendFormatLogLine`, which reads the **global**
`GFrameCounter` **at format time** (`OutputDeviceHelper.cpp:24`, `:30`, `:35`, `:39`). Format time is
the *device's* `Serialize`, not the `UE_LOG` call. Two facts follow:

- ✅ `FOutputDeviceFile::Serialize` (`OutputDeviceFile.cpp:535`) formats on the **calling** thread and
  hands bytes to its `AsyncWriter` (`:557`) — **the async writer defers the byte write, not the
  formatting.** No hazard there.
- 🚨 `FOutputDeviceRedirector::Serialize` **queues** a line when the caller is not the primary logging
  thread, or when the primary lock is contended (`OutputDeviceRedirector.cpp:595-611` is the immediate
  path; **`:614`** is the queue). The queued payload, `FOutputDeviceLine` (**`:73-83`**), carries
  `Data`, `Category`, `Verbosity` and **`Time` — but NO frame counter.** A queued line is later
  serialized by `FlushBufferedLines` (`:443-453`) and stamped with `GFrameCounter` **as of the
  flush.** ⇒ **the timestamp survives a deferral; the frame bracket does not.**
- 🚨 The redirector can also run a **dedicated thread that makes itself the primary logging thread**
  (`ThreadLoop`, `:420-441`, the `PrimaryThreadId.store` at **`:426`**) — under which **game-thread
  lines take the queued path.**

✅ **Why it does not bite here, stated as a reading and not as a guarantee:** `TryStartWriterThread`
(`:557`) has **no caller anywhere in the engine source**, and the game thread is made primary at
`LaunchEngineLoop.cpp:1531`. Combined with the 704/704 measurement above, **the deferral path is not
active in a stock UE 5.1 packaged Windows build.** ⛔ It is recorded because §1 offers **no way to
detect it on a host we cannot instrument**, and §6.3.1's control is exactly that detector.

#### §6.3.3 🔻 **REFUTED (narrowly) — §1.3.2's "expected shape" is over-specific, and §1.5.1 mode 1 is under-specified**

§1.3.2 gives the expected Bates line as `[YYYY.MM.DD-HH.MM.SS:mmm][fff]LogAnomaly: Verbose: …`, and
the READ GUIDE repeats it with a dated example. **`AppendFormatLogLine` prints the `[%3llu]` frame
bracket in FOUR of the five `ELogTimes` modes** — `SinceGStartTime` `:24`, `UTC` `:30`, `Local` `:35`,
`Timecode` `:39` — and omits it only in `default:` (`None`, `:42-43`). The five values are
`0 None · 1 UTC · 2 SinceGStartTime · 3 Local · 4 Timecode` (`LaunchEngineLoop.cpp:5712-5716`).

⇒ **Two corrections, opposite in sign:**
- ✅ **§1.5.1 failure-mode 1 is CORRECT and in fact stronger than stated** — only `LogTimes=None` /
  `-NOLOGTIMES` removes the anchor; `-LOCALLOGTIMES`, `-LOGTIMESINCESTART` and `-LOGTIMECODE` all keep it.
- 🔻 **but the FIRST bracket is then a float (`[0012.34]`) or a timecode, not a date.** A reader
  matching the documented shape could report *"no prefix"* on a line whose anchor is present.
  **The rule that survives every mode is: the anchor is the LAST bracket before the category.**

⚠ **Two further prefix elements are host-switchable and §1 does not name them:** `GPrintLogCategory`
(`OutputDeviceHelper.cpp:46`) can drop `LogAnomaly: `, and `GPrintLogVerbosity` (`:58`, `:64`) can drop
`Verbose: `. Neither touches the bracket, and the payload `blinking toggle ->` survives both.
⚠ **And `log.Timestamp` is a console variable** (`LaunchEngineLoop.cpp:5678-5687`), so §1.3.2's
override list — ini + command line — misses the **console / `-ExecCmds`** route.

---

### §6.4 R4 — THE `n+7` CLAIM AND "EXACTLY THREE TOGGLE LINES"

#### §6.4.1 ✅ **`n+7` carries no row for the fire — COULD NOT REFUTE on the async path**

Re-verified: `BeginRevert()` at `:653` → `LiveFires.Reset()` (`AnomalyAutoInjectorSubsystem.cpp:708`);
`FinalizeArmedLabel()` at `:700` then reads `Auto->GetLiveFires()` at `:2539`, now empty. **There is no
`return` between `:653` and `:700`**, so the ordering cannot be bypassed. ⚠ **One scoped caveat §1
does not state:** `AppendSessionGlobalFires(Snap->Fires)` at **`:2543`** can repopulate `Snap->Fires`
for `n+7` **if a session-global anomaly is held** — the row would then exist for the *global*, not for
the blinking fire, and an overlay could draw a box at `n+7`. ⛔ **Not applicable to `C-3`** (targeted
`blinking`, no global), but the READ GUIDE's *"a box at `n+7` … is a finding"* should say
*"with no session-global held"*.

✅ §1's sync-fallback caveat is correct: the sync path builds `Fires` at `:2408-2409` from inside
`CaptureCurrentFrame` (called at `:652`, **before** `BeginRevert` at `:653`), so `n+7` **would** carry
a row there.

🆕 **AND THE SYNC PATH IS WORSE THAN §1 SAYS, IN A WAY THAT MATTERS FOR `C-3`.** Its active bit is read
**inline at `:2439`** (`ActiveNow.Add((FActor && FActor->IsHidden()) ? 1 : 0)`), i.e. **inside the
capture `Tick`, before that tick's injector tick** — it never sets `bHasArmedLabel` (`:2376` is
async-only) and so never uses `FinalizeArmedLabel`/`SampleDeferredActiveState`. ⇒ **a sync frame
reproduces the pre-`m20` one-tick-stale hidden set.** 🚨 **And its announcement at `:2382-2383` is
`LogAnomalyCapture, Verbose`, while `C-3` raises `LogAnomaly` — a DIFFERENT category. ⇒ on a `C-3`
bundle a per-frame sync fallback is SILENT.**
✅ **Bounded, and detectable anyway:** with SVE on, `:2350` reads `if (bUseSve || ComputeGameViewportCapture(...))`,
so the fallback needs a structural failure of `Async`/`SveCapturer`, not a transient rect miss; and
`SVE-WANT-SUMMARY` (`:3211`) is `Log` verbosity and reports `marksIssued` beside `framesWritten`.
⇒ **`marksIssued < framesWritten` is the in-bundle detector, and it is already collected.**

#### §6.4.2 🔻 **REFUTED — "EXACTLY THREE TOGGLE LINES PER EVENT" IS FALSE FOR THE LAST EVENT OF `C-3`'s OWN RUN**

`C-3` runs `IAI.Capture.Start "" png 4242 90 blinking StaticMeshActor_1246` at `2 4 8 4 0`.
Captured frames `0..89`; lead-in `0-3`; bursts every 12 frames ⇒ burst 8's Positives window would be
`88-95`. After arming `si = 89`, `SessionFrameIndex == 90 == FrameCap`, so the **next** tick takes
`:624-631` into `DrainTail` — **`BeginRevert` is never reached from the FSM for that burst.**
Consequences, all of which break the stated rule:

- the event has **2 frames, not the 4-frame `(n, n+1, n+5, n+6)` cadence**;
- there is **no `n+7`**, so §1.5.2's `n+7` cross-check is inapplicable to it;
- `bActive` stays true, so the injector **keeps toggling through `DrainTail`** and emits further
  `blinking toggle ->` lines that are anchored to **no captured frame at all**;
- flip 4 arrives from `FinishRun` (`:2984-2991`: `SampleDeferredActiveState()` then
  `RevertAllLiveFires()`), so the `IAI.Revert 'blinking' -> reverted.` receipt lands **after**
  `DrainTail`, not one tick after the last captured frame.

✅ **MEASURED, NOT ONLY DERIVED.** Scanning the banked session bank for 90-frame runs with an event
reaching index ≥ 87: **235 matches, and every blink one reads `frame_indices = [88,89]`** — a 2-frame
final event, across sessions from 2026-08-16 through 2026-09-02. (`FireWindow` anomalies read
`[87,88,89]`, i.e. `n−1` plus the two survivors, which independently confirms the `n−1` row exists.)

🚨 **THE HAZARD IS THE DIAGNOSTIC ATTACHED TO THE RULE, NOT THE COUNT.** §1.4.2 and READ GUIDE item 5
both say *"a different count or order means the half-period or the burst config differs from the
assumption"* — which, met on the final event, would send the reader to conclude **Bates' half-period
differs**, a false finding about the host.
✅ **Bounded in practice:** the card's `(a)` prints `Select-Object -First 20` toggle lines ≈ events 1–6
plus two lines of event 7, and prerequisite 4 already directs the anchor event into bursts 1–5. ⚠ **But
the last printed event is truncated by `-First 20`, so it too will show fewer than three.**

#### §6.4.3 ⚠ Two smaller R4 items

- **Half-period ≠ 3** genuinely changes the count, and §1's mitigation is correct: the effective value
  is reported by `blinking: matched … at half-period %d frame(s).` (`Anomaly_Blinking.cpp:68-69`),
  resolved through `AnomalyDefaults::GetHalfPeriodFrames` (`:35-37`). **Could not refute.**
- **Auto-pool vs targeted:** `C-3` is targeted, so every burst fires the same anomaly on the same
  actor. Under auto-pool a burst may fire a different id or none (`Capture: burst %d fired nothing`,
  `:2319-2320`) and the "3 lines per event" expectation would not even be well-posed. **Outside
  `C-3`'s case.**

---

### §6.5 R5 — AN OUTCOME §1.5.3 NAMES ONLY ONE READING FOR

The brief asks for an outcome the table classifies wrongly. **One exists, and it is in row 2.**

| row | `f_toggle` | §1.5.3's structural clause | §1.5.3's naming |
|---|---|---|---|
| 1 | `n+2` / `n+5` | divergence **downstream of the visibility call** | "the **PIXELS** are the outlier" |
| 2 | `n+3` / `n+6` | divergence **between the toggle and the label** | "the **SAMPLING / LABELLING** side is the outlier" |

🔻 **Row 2's structural clause survives; its NAMING does not.** §6.1.3 shows that a one-tick change in
*when the toggle runs relative to the arm* produces `f_toggle = n+3` **while the labelling code
behaves identically to the bench, byte for byte** — the deferred sampler is order-compensating, so
nothing about "sampling/labelling" would be anomalous. A reader given row 2 as written would be
pointed at the label pipeline, and the code admits at least one other reading in which the label
pipeline is not implicated at all. ⇒ **row 2 should name the LOCATION (an interval), not a SUBSYSTEM.**

⛔ **This is not an argument that row 2 will fire, nor a mechanism, nor a prior.** It is a statement
that row 2's label is narrower than its own evidence supports.

🎯 **AND THE BUNDLE ALREADY CONTAINS THE READ THAT SEPARATES THEM, AT ZERO EXTRA COST: flip 1's
anchor** (§6.1.3). §1.5.1 currently spends it as an "expected" check.

⚠ **One presentational trap, `G161`'s exact shape.** §1.5.3 compares `f_toggle` with `n+2` / `n+5`
without naming a space conversion, but the join in §1.5.1 is *"toggle `[fff]` against
`labels.jsonl.frame_index`"* while `n` lives in **`session_index`** space (`frame_indices` are session
indices). ✅ **READ GUIDE item 2 already says this explicitly** and is the safer text; **§1.5.3 alone
does not.**

⚠ **And one completeness item for §1.5.1 failure-mode 3.** Raising `LogAnomaly` to `Verbose` also
unmasks `Heartbeat; active anomalies: %d/%d; scoping: %s` every 2 s
(`AnomalyInjectorSubsystem.cpp:215`) — the only other `LogAnomaly, Verbose` line reachable on a
no-flags targeted `blinking` run (the other four are census/`camera_clipping`/`lod_popping`, all
absent). 🎯 **That makes it a free known-answer control on the verbosity switch itself:** it
distinguishes *"`Verbose` took and no toggle line appeared"* from *"`Verbose` did not take"* — which
§1.5.1 mode 3 asks the owner to record but gives no way to tell apart.

---

### §6.6 CORRECTIONS §1 NEEDS — exact wording

⛔ **§1 is a RECORD. Nothing above is retro-edited into it.** These are the replacement sentences, to
be carried wherever §1 is next used — and, for the two marked 🔴, into `docs/office-rdp-card.md`'s
`C-3` READ GUIDE, which is what the owner actually reads.

1. **§1.4.5, evidence list — replace conjunct (b).**
   > The evidence that capture ticks first is **`m20`, which measured `annotation(G) == pixels(G−1)`
   > on every blink edge in a package** — a comparison of **pixels against labels**, and the only kind
   > that can see the order. ⛔ **The reconstruction's agreement with `[16,17,21,22]`, `[27..34]` and
   > the empty `session_index 23` row is NOT evidence of the order: all three are order-invariant.**
   > **`frame_indices` is exactly order-invariant** — the deferred sampler at `:591` compensates —
   > **while the PIXEL column is not.** ⇒ **the order is established only where pixels have been
   > compared to labels, i.e. on this box. It is not established on Bates.**

2. 🔴 **§1.5.1, the flip-1 bullet, and READ GUIDE item 5's last sentence — reclassify.**
   > **Flip 1 fires on an uncaptured settle tick under the order measured here, so its `[fff]` should
   > match NO `labels.jsonl` row — it should fall in the `K`-tick gap, and
   > `frame_index(n) − frame_index(n−1)` should be `K + 1 = 3`. ⚠ THIS IS A DISCRIMINATOR, NOT AN
   > INVARIANT. If flip 1's `[fff]` instead lands ON `frame_index(n)`, that is a READING and not a
   > fault — record it and send it raw.**

3. 🔴 **§1.4.2 / §1.5.2 / READ GUIDE item 5 — bound the "three toggle lines" rule.**
   > **Exactly three toggle lines per event, `HIDDEN · VISIBLE · HIDDEN`, holds for every FULL burst.
   > ⛔ It does NOT hold for the LAST event of a 90-frame run: the frame cap (`:624-631`) sends the
   > run to `DrainTail` mid-Positives, `BeginRevert` never runs for that burst, its `frame_indices`
   > read `[88,89]` (measured on 235 banked sessions), and the injector keeps toggling into
   > `DrainTail` with no captured frame behind those lines. Anchor on bursts 1–5, as prerequisite 4
   > already requires, and do not count toggle lines on the last event or on an event truncated by
   > `-First 20`.**

4. **§1.3.2 / READ GUIDE item 1 — generalise the shape.**
   > **The anchor is the LAST bracket before the category, whatever precedes it.** The frame bracket
   > is printed in four of the five `ELogTimes` modes (`UTC`, `Local`, `SinceGStartTime`, `Timecode`)
   > and omitted only by `None`; under `SinceGStartTime` the first bracket is a seconds float, not a
   > date. **Report "no prefix" only if there is no bracket at all.** `log.Timestamp` is also settable
   > from the console / `-ExecCmds`, not just ini and command line.

5. **§1.3.3 — add the known-answer control, and say what it is for.**
   > **`[fff]` is stamped by the log device at SERIALIZE time from the global `GFrameCounter`
   > (`OutputDeviceHelper.cpp:30`), not captured at `UE_LOG` time.** `FOutputDeviceRedirector` can
   > queue a line (`OutputDeviceRedirector.cpp:614`) whose payload carries `Time` but no frame counter
   > (`:73-83`), in which case the bracket is stamped at flush. **Verify, do not assume:
   > `SVE-WANT-TRACE arm … gameFrame=N` self-reports its own frame, so `[fff] == gameFrame % 1000` on
   > that line is a direct check of the anchor. Measured here: 704/704 over 11 packaged logs, 0
   > divergent. Run the same check on the first arm line in the Bates bundle before trusting any
   > toggle-line join.**

6. **§1.2(iii) — restate precisely.**
   > **Clocks A and B are never reconciled FOR LABELLING PURPOSES.** They meet at **two** sample
   > points — `SampleDeferredActiveState`'s `IsHidden()` read (`:2755`) and `ProjectionView()`'s
   > tick-indexed `ViewRing` read (`:2280-2288`, inert at the default `ViewLagFrames = 0`) — and there
   > **is** one arithmetic tick→seconds conversion outside labelling,
   > `ComputeNominalGameSpan()` (`:2808-2815`), which multiplies a world-tick count by `1/VideoFps`.

7. **§1.5.3 row 2 — name the interval, not the subsystem.**
   > **`f_toggle` = `n+3` / `n+6` ⇒ the toggle executed one tick later than the labels imply, and the
   > divergence lies in the interval BETWEEN THE TOGGLE CALL AND THE LABEL SAMPLE.** ⛔ **That
   > interval contains more than the sampling code, so do not name a subsystem.**

8. **§1.5.1 failure-mode 3 — add the control.**
   > If no `blinking toggle ->` line appears, **check for `Heartbeat; active anomalies:` in the same
   > log** (`AnomalyInjectorSubsystem.cpp:215`, `LogAnomaly, Verbose`, every 2 s). **Present ⇒
   > `Verbose` took and the toggle lines are genuinely absent. Absent ⇒ `Verbose` did not take.**

9. **§1.5.2 `n+7` row — scope it.** Add *"with no session-global anomaly held"* (`:2543`).

---

### §6.7 WHAT COULD NOT BE REFUTED — the positive list

Stated explicitly, because a review that reports only its hits is not a review.

| §1 claim | checked against | verdict |
|---|---|---|
| `Apply` never hides; flips 1–3 are the same statement at `Anomaly_Blinking.cpp:91` | whole file re-read at `970bf1d` | ✅ holds |
| flip 4 is `:106`, reached `:653` → `:2330` → `AnomalyAutoInjectorSubsystem.cpp:705` → `AnomalyInjectorSubsystem.cpp:486` | chain re-walked | ✅ holds |
| the call-site partition `{1,2,3}·{4}` and the observed-agreement partition `{1,4}·{2,3}` do not coincide, **with no inference drawn** | — | ✅ holds, and the ⛔ is correctly placed |
| `FramesSinceToggle` counts world-tick CALLS; `DeltaSeconds` ignored | `Anomaly_Blinking.cpp:73-81` | ✅ holds |
| the toggle loop body runs at most once per tick at `HalfPeriodFrames ≥ 1` | `:80-97`, `Anomaly_Blinking.h:29` | ✅ holds |
| the toggle line stamps only the phase word and the actor count | `:95-96` | ✅ holds |
| `labels.jsonl.frame_index` **is** `GFrameCounter` at arm time | `:2354` beside `:2355`, `:2377` | ✅ holds |
| the three stamping statements are consecutive in one loop iteration over one `Snap` | `:2175` / `:2177` / `:2179-2180` | ✅ holds |
| `GFrameCounter++` is after `GEngine->Tick`, and is the sole mutation in the loop | `LaunchEngineLoop.cpp:5568` vs `:5363`; Runtime-wide sweep | ✅ holds |
| `log.Timestamp` default is `1` = UTC | `:5678-5687`, `:5713` | ✅ holds |
| a render-thread line's `[fff]` is not its own render frame; use `id=` / `requestId=` | `AnomalySveCapturer.cpp:68-70`, `:151-153` | ✅ holds |
| `SVE-WANT-TRACE arm` stops at 64 | `AnomalySveCapturer.h:38` | ✅ holds |
| `SetActorHiddenInGame` to the held value is a no-op | `Actor.cpp:4556-4563` | ✅ holds |
| no subsystem overrides `GetTickableTickType`, a tick group or a priority | all five headers | ✅ holds |
| "registration order" is the right description of tickable order | `Tickable.cpp:15`, `:119-123`; `Set.h:1397-1400`, `:1582-1636` | ✅ holds — **my own pointer-hash counter-claim is REFUTED and recorded as such** |
| §1.1.4's asymmetry is arithmetic of `(K=2, half-period 3)`, not a designed property, and is **not** a lead | re-derived | ✅ holds |
| `ticks_per_captured_frame` is a run average, not a per-window factor | `AnomalyLabelWriter.cpp:547-549` | ✅ holds |
| no competing flip-4 driver during a run (auto-injector paused) | `:1286`; `AnomalyAutoInjectorSubsystem.cpp:121-123`, `:185-187` | ✅ holds |

### §6.8 What this section does NOT do

- ⛔ **No mechanism, lead or likely-cause for `P9` (B).** The axis table in ledger §8.6a is unchanged;
  nothing is added to it and nothing is removed.
- ⛔ **No scope decision changes. `C-3` runs exactly as carded** — same steps, same commands, same
  prerequisites. Every correction in §6.6 is a change to **what to look at afterwards**.
- ⛔ **Nothing about Bates' tick order, log configuration or half-period is claimed.** All three are
  unmeasured here and all three are answered by the bundle itself.
- ⛔ **No source edit, no build, no leg, no cook, no tag.** Staged bench exe unchanged.
