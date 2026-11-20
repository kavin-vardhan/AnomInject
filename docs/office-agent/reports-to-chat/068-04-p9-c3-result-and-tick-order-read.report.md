# REPORT for 068-04-p9-c3-result-and-tick-order-read.md
brief: 068-04-p9-c3-result-and-tick-order-read.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-02 19:50:10 | finished: 2026-09-02 20:05:14 | duration: 15 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\068-04-p9-c3-result-and-tick-order-read.live.txt

---- FINAL MESSAGE (Claude Code) ----
**1. Status.** `C-3` answered. On Bates the toggle log sides with the **eye** on both interior flips — `HIDDEN [38]` = frame 28 = `n`, `VISIBLE [41]` = `n+3`, `HIDDEN [44]` = `n+6`, eye gone at 28/29/30/34 — so pixels, AA, capture path and overlay semantics are **cleared** and the labels `{28,29,33,34}` are the outlier; both free cross-checks (amber `n−1`, no box `n+7`) passed there. The result, join table and arithmetic are in ledger §8.6a (the superseded localization line is marked, not deleted); journal 068 gains §7 (brief + bench measurement), §8 (tick-order source read), §9 (fix plan); the card has C-3 marked done with its result, the four READ GUIDE corrections, the conditional arm-line step, and a Standing block saying nothing further is needed from Bates until a fix build exists; CLAUDE.md's `P9` block no longer tells a cold reader to run C-3. Commit **`623ca24`**, pushed, 4 files, 539 insertions / 17 deletions — every deletion is a line I replaced. ⛔ No source edit, no build, no leg, no cook, no tag.

**2. (b)/(c) — bench, measured vs expected.** Source: `M38_G5_VERBOSE\session_20260902-183933`, `run.json` = `settle 2 / pre 4 / positive 8 / post 4 / cap 90 / targeted blinking` — byte-for-byte C-3's config.

| burst | `n` | apply | `fi(n−1)` | T1 | uncaptured? | T2 | `fi(n+2)` | T3 | `fi(n+5)` | revert | `fi(n+7)` | Δ apply→T1 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 1 | 4 | 4 | **4** ✅ | 6 | ✅ | 9 | **9** ✅ | 12 | **12** ✅ | 14 | **14** ✅ | +2 |
| 2 | 16 | 20 | **20** ✅ | 22 | ✅ | 25 | **25** ✅ | 28 | **28** ✅ | 30 | **30** ✅ | +2 |
| 3 | 28 | 36 | **36** ✅ | 38 | ✅ | 41 | **41** ✅ | 44 | **44** ✅ | 46 | **46** ✅ | +2 |
| 4 | 40 | 52 | **52** ✅ | 54 | ✅ | 57 | **57** ✅ | 60 | **60** ✅ | 62 | **62** ✅ | +2 |
| 5 | 52 | 68 | **68** ✅ | 70 | ✅ | 73 | **73** ✅ | 76 | **76** ✅ | 78 | **78** ✅ | +2 |
| 6 | 64 | 84 | **84** ✅ | 86 | ✅ | 89 | **89** ✅ | 92 | **92** ✅ | 94 | **94** ✅ | +2 |
| 7 | 76 | 100 | **100** ✅ | 102 | ✅ | 105 | **105** ✅ | 108 | **108** ✅ | 110 | **110** ✅ | +2 |
| 8 (cap-cut) | 88 | 116 | **116** ✅ | 118 | ✅ | 121 | *no frame* | — | — | 122 | *none* | +2 |
| **Bates** | 28 | 30035 | **30035** ✅ | **30038 = `n`** | ❌ **captured** | 30041 = `n+3` | — | 30044 = `n+6` | — | `n+7` | — | **+3** |

**Loudly, as asked: the bench does NOT show `n / n+3 / n+6`.** It shows `uncaptured / n+2 / n+5` on every full burst, exactly as §1.1.1 modelled — the story does not change. **(c): `Δ = +2` on the bench (apply tick IS counted), `+3` on Bates (it is NOT).** With half-period 3 the arithmetic admits no other value. Burst 8 also confirms §6.4.2 live: `frame_indices [88,89]`, max `frame_index` 120, so toggle `[121]` has no labels row at all.

**3. (d) — tick order.** Chain: `UWorld::Tick` → `FTickableGameObject::TickObjects` (`LevelTick.cpp:1606`) → array order (`Tickable.cpp:134-164`) → drained from a `TSet` each tick (`:117-124`, set at `:15`, ctor `:83-95`, dtor `:97-103`) → `TSet` iterates its sparse array in index order (`Set.h:1397-1400`, `:1582-1636`) **but `Emplace` reuses a LIFO free list** (`Set.h:723-736` → `SparseArray.h:115-136`) → construction order from `World.cpp:8615` / teardown `:5085` → `SubsystemCollection.cpp:135-141` → `GetDerivedClasses` BFS over `ClassToChildListMap` (`UObjectHash.cpp:1417-1434`, `:1314-1343`, map `:361`). **No ordering control exists on this API:** none of the five subsystems overrides tick type/group/priority, and `FTickableGameObject` has **no prerequisite API** — that lives on `FTickFunction::AddPrerequisite` (`EngineBaseTypes.h:349`). 🚨 **Measured contradiction: on this box the injector subsystem is *constructed* before the capture subsystem (Initialize log order) yet *ticks* after it** — so construction order is not tick order, §1.4.5's "registration order" is incomplete, and this box is its own counter-example. (That supersedes my own §6.1.3 line calling that mechanism un-refuted; the free-list reuse at `SparseArray.h:115-136` plus world-transition churn is why.) ⇒ **yes, it can differ between hosts** — every input (module load order, the host's own and every plugin's `UWorldSubsystem` classes, UHT emission order, tickable churn across world transitions) is host-owned and undeclared.

**Arithmetic.** Injector-before-capture on `T0`: `Apply` runs inside the capture `Tick` (`:643`/`:680` → `:2314`), so the injector sees `bActive == false` and returns at `Anomaly_Blinking.cpp:75-78` — `T0` uncounted; `T1→1, T2→2, T3→3` ⇒ first toggle at `T3` = `n`'s arm tick. **Bates: apply 30035 = `fi(27)`; 30036/30037 absent from labels; toggles 30038 = `fi(28)`, 30041 = `fi(31)`, 30044 = `fi(34)`; pixels `{n,n+1,n+2,n+6}` (a toggle inside tick T reaches T's render, `SceneRendering.cpp:4528`) = eye gone 28,29,30,34; labels unchanged at `{n,n+1,n+5,n+6}` by §6's order-invariance; amber `n−1`, no box `n+7`. Every line fits.** Capture-first on `T0`: `T0` counted ⇒ first toggle at `T2`, uncaptured — **bench, all 8 bursts.** ✅ **One hypothesis fits every number on both hosts with no free parameters and nothing contradicts it — recorded as "consistent with", not asserted; Bates' order was not observed.** ⚠ And `apply → first toggle` **cannot** distinguish "ticked before `Apply`" from "did not tick" — both give +3.

**4. (e) Options.**

| | what | fixes labels on any host? | changes rendered pixels? | blast radius | main risk |
|---|---|---|---|---|---|
| **1** pin the order (1a capture drives injector · 1b `FTickFunction`+prerequisite · 1c pre-tick delegate) | make order deterministic | yes, by pinning | 🚨 **YES — on whichever host disagrees with the pin** | injector tick for **all** anomalies, in and out of capture | converts a label fix into a product-behaviour change on a client build |
| **2** ✅ sample active state at `FWorldDelegates::OnWorldTickEnd` (`LevelTick.cpp:1814`, after `TickObjects` `:1606`, before the draw) | **yes — order becomes irrelevant** | **NO** | one function (`SampleDeferredActiveState`) → `FireActive` → `frame_indices` | must not read as an `m18` regression (it is strictly later than m18's point); sync-fallback path still stale unless moved |
| **3** both | — | yes | yes | widest | bundles a pixel change with a label fix ⇒ unattributable |

**Recommendation: option 2.** It fixes the thing that is wrong (labels) and leaves the thing that is right (pixels) untouched on every host; it *removes* the dependency rather than pinning it, so no future host's plugin set can reintroduce this; and the hook, the `World != GetWorld()` guard and the "post-toggle by position, pre-draw same frame" argument are **already in this codebase and already proven** by `m26` (`AnomalyCaptureSubsystem.cpp:334`/`:704`/`:707`). Gates to re-pass: `m20`'s pixel-vs-label measurement, the 16/16 bench alignment, `P6` 48/48, `P-C7`, and — the discriminating one — **the bench's certified `(n, n+1, n+5, n+6)` cadence byte-identical, i.e. proven inert where the order already agrees.**

**5. NEEDS-DECISION**

1. 🚨 **Prove-it-can-fail (`G96`).** The bench is a `Δ=+2` environment, so option 2 can only be proven **inert** here — a gate that can only pass. **Route A:** add a default-OFF bench-only lever that synthesises `Δ=+3` (echoed at `StartRun` per `A48`; it synthesises the *symptom*, not the cause, and the gate file must say so). **Route B:** ship proven-inert and validate on Bates when its build next updates, with the pass condition pre-declared **before** the build ships (labels must equal the eye whether `Δ` reads +2 or +3). I lean **A** — this project has three prior instances of a guard that never fired.
2. **Scope of the fix:** does it also move the **sync-fallback** sample (`AnomalyCaptureSubsystem.cpp:2439`), which is one tick stale by the same mechanism and is *silent* on a `LogAnomaly`-only Verbose run? Fix it, or state it as a documented limitation.
3. **Milestone number.** The fix is unnumbered and unauthorised. `m39` is honest bbox (with `P-C13` conjunct 2 riding it); this needs its own number and a slot in the office batch — chat's call, not mine.
