# REPORT for 068-03-task1-adversarial-reread.md
brief: 068-03-task1-adversarial-reread.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-02 19:13:31 | finished: 2026-09-02 19:40:29 | duration: 27 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\068-03-task1-adversarial-reread.live.txt

---- FINAL MESSAGE (Claude Code) ----
**Report.**

`git rev-parse HEAD` == `origin/master` at bootstrap: **`970bf1d`** (as expected). Commit pushed: **`92c186a`**, one file, 538 insertions / 0 deletions, the four untracked owner `CHAT-HANDOFF-*.md` untouched.

## R1–R5

| | verdict | evidence |
|---|---|---|
| **R1** flip table | **SPLIT — table ✅ / §1.4.5 evidence (b) 🔻 REFUTED** | Table re-derived from the FSM at `970bf1d` (`AnomalyCaptureSubsystem.cpp:639-698`, `:2322-2323`) and holds statement for statement. But **`frame_indices` is *exactly* order-invariant**: reversing the two subsystems shifts every flip +1 tick, and `SampleDeferredActiveState` at `:591` shifts its read +1 tick with it, so `label(T)` is unchanged for every `T` at every config. `[27..34]` and the empty `session_index 23` are decided *inside* the capture `Tick` (`:653` before `:700`) and never consult the injector. ⇒ none of the three artifact facts can establish the order. Only `m20`'s **pixel-vs-label** measurement can — and it does, decisively (under injector-first there would have been no `m20` bug). The **pixel column is not order-invariant**; the outer flips are unaffected either way. ⛔ No mechanism claimed; the `{n,n+1,n+2,n+6}` shape is generic to *any* one-tick offset (`m20`'s pre-fix *labels* had it from a different cause), so it carries no location information. Separately: my own suspicion that tickable order was pointer-hash order is **refuted** — `TSet` iterates insertion order (`Set.h:1397-1400`), so §1.4.5's "registration order" is right. |
| **R2** two clocks | **🔻 REFUTED (wording)** | Three meeting points, not one. `ComputeNominalGameSpan()` **`:2808-2815`** multiplies a *world-tick* count by `1/VideoFps` (the *captured-frame* period) — a genuine tick→seconds conversion. `ProjectionView()` **`:2280-2288`** reads a tick-indexed `ViewRing` at a frame-clock moment and writes it into the label snapshot at `:2358` (inert at the default `ViewLagFrames = 0`; the console command calls the unit "frames", `:810`). §1's *"for labelling purposes"* qualifier survives; the unqualified clause and "exactly one place" do not. "Local ratio 1.0 inside Positives" has one exception — the frame-cap check at `:624-637`. |
| **R3** prefix anchor | **✅ COULD NOT REFUTE — and now measured** | `GFrameCounter++` at `LaunchEngineLoop.cpp:5568` is after `:5363` and is the only mutation reachable in a run (swept Runtime; the other three are high-res screenshot / preload / commandlet). No arm outside `UWorld::Tick` (`CaptureCurrentFrame` has exactly 3 call sites). **The join itself had never been measured** — §1.3.3's cross-check was `labels.jsonl`'s own series. It is now: `SVE-WANT-TRACE arm` self-reports `gameFrame=` *and* takes a prefix, so `[fff] == gameFrame % 1000` is a known-answer control — **704/704 across 11 packaged logs, 0 divergent**. A real deferral path exists (`OutputDeviceRedirector.cpp:614`; `FOutputDeviceLine` `:73-83` carries `Time` but no frame counter; `ThreadLoop` `:426` can steal primary-thread status) but `TryStartWriterThread` has **no caller** in the engine. Narrow refutation: the frame bracket survives **four of five** `ELogTimes` modes (`OutputDeviceHelper.cpp:24/30/35/39`), so §1.3.2's dated "expected shape" is over-specific. |
| **R4** `n+7` / 3 toggle lines | **`n+7` ✅ / "three lines" 🔻 REFUTED, inside `C-3`'s own config** | `n+7` holds on the async path (no `return` between `:653` and `:700`); scope caveat — `AppendSessionGlobalFires` `:2543` can repopulate it if a global is held. **"Exactly three toggle lines per event" is false for the last event of a 90-frame run**: `BurstCount = 0`, so the frame cap trips mid-Positives (`:624-631`), `BeginRevert` never runs, and the injector keeps toggling into `DrainTail` with no captured frame behind those lines. **Measured: 235 banked 90-frame sessions, every blink final event reads `frame_indices = [88,89]`.** Also: the sync fallback reads its active bit inline at `:2439` (pre-`m20` one-tick-stale) and announces on **`LogAnomalyCapture`**, not `LogAnomaly` — so on a `C-3` bundle it is **silent**; `SVE-WANT-SUMMARY`'s `marksIssued` vs `framesWritten` is the detector. |
| **R5** three-way table | **🔻 one mis-naming exists** | Row 2 (`f_toggle = n+3/n+6`) is read as *"the **sampling/labelling** side is the outlier"*. Per R1, a one-tick change in when the toggle runs relative to the arm produces exactly that with the labelling code behaving **identically to the bench** — so row 2's *structural clause* survives but its *naming* is narrower than its evidence. It should name the **interval**, not a subsystem. |

## Corrections §1 needs (full exact wording in journal §6.6; the load-bearing two)

🔴 **These two are on `docs/office-rdp-card.md`'s `C-3` READ GUIDE, which is what gets read at the office — I did not edit the card.**

1. **READ GUIDE item 5, last sentence + §1.5.1's flip-1 bullet** — reclassify:
   > *Flip 1 fires on an uncaptured settle tick under the order measured here, so its `[fff]` should match NO `labels.jsonl` row — it should fall in the `K`-tick gap, and `frame_index(n) − frame_index(n−1)` should be `K+1 = 3`. ⚠ **THIS IS A DISCRIMINATOR, NOT AN INVARIANT.** If flip 1's `[fff]` instead lands ON `frame_index(n)`, that is a READING and not a fault — record it and send it raw.*

2. **READ GUIDE item 5 + §1.4.2/§1.5.2** — bound the rule:
   > *Exactly three toggle lines per event holds for every FULL burst. ⛔ It does NOT hold for the LAST event of a 90-frame run … Anchor on bursts 1–5, as prerequisite 4 already requires, and do not count toggle lines on the last event or on an event truncated by `-First 20`.*

The other seven (§1.4.5 evidence, §1.3.2 prefix shape, §1.3.3 known-answer control, §1.2(iii) restatement, §1.5.3 row 2, §1.5.1 mode-3 `Heartbeat` control, §1.5.2 `n+7` scoping) are journal-side and are written out verbatim in §6.6.

## NEEDS-DECISION

1. **Amend the `C-3` READ GUIDE with corrections 1 and 2?** Time-sensitive — the card is what the owner uses, item 5 as written would send a reader who counts toggle lines on a truncated event to conclude *"Bates' half-period differs"*, and item 5 currently spends flip 1's anchor (the single read that separates §1.5.3's two rows) as an "expected" check. I made no card edit; the brief scoped me to journal §6.
2. **Add the `SVE-WANT-TRACE arm` prefix check (`[fff] == gameFrame % 1000`) as a stated `C-3` read step?** It needs no new command — the line is already in the bundle — but it is currently framed only as a "second anchor", not as the validity check on the primary one.

⛔ No mechanism, lead or prior for `P9` (B) anywhere. No scope decision changed — `C-3` runs exactly as carded. No source edit, no build, no leg, no cook, no tag; staged bench exe unchanged.
