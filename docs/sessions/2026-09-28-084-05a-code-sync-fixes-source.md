# 084-05a — Label-sync fixes, source: G350, the temporal-AA `transition` flag, mask-ID recycling; rebuild

Date: 2026-09-28 (IST, late night) · Branch `fix/m52-label-timing` · base `44584e7` · source commit `05c7649` · exe **`2AC0523E`**
Spec: `_reviews/084-03-chat-ruling-bench-results.md` decisions 1–3 and 5. Compiling only: **nothing was launched, staged or
focused**; the `084-03-*` harness is untouched; main checkout still `m51` `53bf725`; staged bench exe still `2FCDF059`.

## 0. The answer

| item | what ships | where |
|---|---|---|
| **G350** | A `stuck_low_mip` (render-truth) entry is emitted **as positive only on a frame labelled for it**. Post-closure trailing bookkeeping and pre-onset rows no longer carry a positive entry; `anomaly_present`, `visible_positive` and `run_summary.positive_frames` follow the emitted positives. Every non-m52 row is byte-identical. | `ApplyRenderTruthToSnapshot` (per-fire `EntryEmit`), `BuildFrameLabelRecord`, `ProcessCompletedFrames` (`Job.bPositive`) |
| **`transition` flag** | Additive. Temporal AA (TAA or TSR) detected at run start; `IAI.Label.TransitionOnFrames` / `TransitionOffFrames` / `TransitionHideFrames`, `-1` = **2 / 8 / 1** under temporal AA, **0** without. m52: first K_on labelled frames get `transition: 1`; the K_off frames after a labelled frame get a transition-only entry that does **not** set `anomaly_present`. Hide types: the first K_hide captured frames after the object returns. Frame key `transition_present: true` only when set. | `ResolveLabelSyncForRun`, `StepHideTransitions`, `Add/ResolveDetachedTransitionCandidates`, pure `AnomalyLabelSync.h` |
| **Mask-ID recycling** | When the 55-value pool is otherwise exhausted, the value of the **oldest finished** event is reclaimed — fire ended, m52 trail detached with nothing in flight, every frame and target mask read back, m26 arms done — after restoring its components still carrying it. Runs that never exhaust allocate exactly as before. | `FAnomalyMaskMeasure::RefreshTagReleasability` / `ReclaimReleasableTag` / `AllocateTag`, `AnomalyStencilTag::RestoreComponentsCarrying`, capture `RefreshMaskTagReleasability` |
| **Gate (ii)'s 5 frames** | **All five are the G295 coalesced-arm signature.** Not fixed (not trivial). | §4 |

Selftest **189 checks, 0 failures** (59 new, each both ways). Game + Editor **exit 0, 0 compiler warnings**. Archived
`_binary_baselines\m52fix-2AC0523E\`.

## 1. G350 — only labelled frames are positive

- **Rule as built.** For every fire of a render-truth anomaly (`IsRenderTruthFire`: `stuck_low_mip` on an SVE + async run),
  `ApplyRenderTruthToSnapshot` records an emission mode per fire from the frame's render membership:
  member ⇒ `Normal`; not a member ⇒ `TransitionOnly` inside K_off of a previous member, else `Suppress`
  (`AnomalyLabelSync::DecideRenderTruthEntry`). `BuildFrameLabelRecord` skips `Suppress` entries, emits `TransitionOnly` entries
  with `transition: 1`, and sets `anomaly_present` iff at least one `Normal` entry exists (`FramePresent`).
  `visible_positive` counts boxes only on entries that set `anomaly_present`.
- **Continuity is untouched (F2).** The trailing fire stays in `Snap.Fires`, so mask gating, m55, the trail cursor, the
  annotation accumulator and every counter see exactly what they saw before; only the labels.jsonl emission changes.
- **Consequence stated, not hidden:** the live event's pre-onset rows (fire applied, render not yet held) lose their entry too.
  That is the ruling's literal rule ("only anomalies labelled on that frame"). `positive_frames` now equals the count of rows
  with `anomaly_present: true`; on m52 runs it drops the pre-onset and post-closure rows; on every other run it is unchanged.
- **Non-m52 byte identity — the source argument.** The three new snapshot arrays (`EntryEmit`, `EntryTransition`,
  `TransitionFires`) are written in three places only: `ApplyRenderTruthToSnapshot` sizes `EntryEmit`/`EntryTransition` only
  when the frame has a render-truth result (an m52 fire); `StepHideTransitions` writes only when a hide type returns under
  temporal AA; `ResolveDetachedTransitionCandidates` adds only m52 candidates. With the arrays empty, `CountEntries` returns
  `bPresent = Fires.Num() > 0` and no transition, the lambda emits the same entries in the same order with the same fields
  (`bSetsPresent` true for all, no `transition` key), `OutNumLabels` and `visible_positive` are the old expressions, and
  `Job.bPositive` equals `Fires.Num() > 0`. JSON field insertion order is unchanged. **Under temporal AA the only change on a
  non-m52 row is additive and confined to hide-type return frames** (`transition` on an entry, `transition_present`, or a
  transition-only entry after the event ended); with AA off those rows are byte-identical as well.
- **Unit proof (logic level):** `TestG350PostClosure` — on the 44584e7 rule an attached m52 event puts `anomaly_present` and an
  entry on all 34 unlabelled frames (2 pre-onset + 32 post-closure); on the new rule `anomaly_present` is exactly the 12
  labelled frames at 0/0 and at 2/8, and with no modes the present rule equals `Fires.Num() > 0` for n = 0..5. The JSON writer
  itself needs the engine, so the byte-level claim rests on the source argument above.

## 2. The `transition` flag

- **Detection:** `ResolveLabelSyncForRun` (called in `StartRun` after the m52 echo) reads
  `GetDefaultAntiAliasingMethod(World->FeatureLevel)` — the engine's own resolution of `r.AntiAliasingMethod`
  (`SceneUtils.cpp:41`) — and the raw cvar for the log. Temporal = TAA (2) or TSR (4). Echo line
  `=== Capture(labelsync): EFFECTIVE FOR THIS RUN …`; `run_summary` gains `label_aa_method`, `label_aa_method_cvar`,
  `label_temporal_aa`, `label_transition_{on,off,hide}_frames` (effective) and their `_cvar` values, plus
  `label_transition_entries`, `label_transition_frames`, `label_entries_suppressed`, `label_transition_out_of_order`.
- **Values:** CVar default `-1` = the provisional 2 / 8 / 1; an explicit value is used (clamped to 64) **only under temporal
  AA**; without it every value is 0. The final values are a lock amendment (CVar, no rebuild) per the ruling.
- **m52:** per event, `FEventTransitionTrack` (fixed memory: the 64 smallest member SIs and the 128 most recent) observes each
  drained frame. A member whose count of earlier members is below K_on gets `transition: 1` (still `Normal`). A non-member
  whose previous member is at most K_off captured frames back gets a `TransitionOnly` entry. After the trail **detaches**
  (the event leaves `Snap.Fires`), `AddDetachedTransitionCandidates` carries the event as a candidate at arm and
  `ResolveDetachedTransitionCandidates` decides at drain, so the K_off entries survive a detach at the next fire. An inherited
  (carried) trail starts with its onset already past.
- **Hide types (blinking, missing_object):** `StepHideTransitions` runs in `SampleDeferredActiveState` (game thread, capture
  order): a hidden → visible step flags the next K_hide captured frames; inside a `blinking` burst that is the burst's own
  entry, after the event ends it is a transition-only entry built from the last live fire info (`seconds_remaining` 0).
- **The gate the ruling asks for** (every unlabelled pixel-visible frame carries `transition`, 0/0 elsewhere) is modelled in
  `TestTransitionGate` with 084-03's measured shape (onset +2, clear +1…+7): 44584e7 fails 7/7, the new logic passes 7/7, and
  it still fails at clear +9 or onset +3, so the model can fail on the new logic too.

## 3. Mask-ID recycling

- **Why lazy (reclaim only at exhaustion), not release-at-end:** the census allocates from the same ledger (`IsFree`), and
  with the census on (the default since m41) its batches decide which targets pass selection. Returning values to the free
  pool the moment an event ends would change census batches — and so seeded selection — on **every** census-on run, including
  every short run that never nears the ceiling. Reclaiming only when `AllocateTag` finds no free value keeps every
  non-exhausting run identical, and the value never leaves `EventClaimed`, so the census never sees it. → **G351.**
- **Eligibility** (`AnomalyLabelSync::IsTagReleasable`, refreshed every tick after the m26 arm and before the target-mask
  records): tag non-zero and not already recycled; fire not live (session globals count as live); no m52 trail that is
  attached, or detached with a noted request not yet processed (`TrailBlocksRelease` — that is the only way a detached trail
  can re-attach); no pending snapshot containing the event (its frames are all drained, i.e. read back); no pending
  target-mask request carrying the value; no m26 arm in flight; and m26 cannot arm it again (arms exhausted, actor gone,
  unmeasurable, tag failed, or a deferred-onset record whose fire has ended — `ArmIfMeasurable` arms any record with arms
  left, live or not, which is how `missing_object` gets its post-revert arms).
- **Reclaim:** oldest-releasable first (`PickRecycleVictim`), refused if any other unrecycled record still holds the value;
  the victim's components still carrying the value are restored (`RestoreComponentsCarrying`, which leaves a census value or
  a new event's value alone), the record is marked `bTagRecycled` (never armed again) and the value moves to the new record.
  Log `Capture(mask): TAG RECYCLED …`; per-run `TAG POOL SUMMARY`; `run_summary.mask_tag_recycles`, `mask_tag_peak_live`
  (most values held by not-yet-releasable events at once), `mask_tag_exhausted` (events that still got none).
- **Across run boundaries:** records and `EventClaimed` reset per run; a carried trail gets a fresh record in the new run and
  is not releasable while attached. The live `TAG-OWNER VIOLATION` check (`OnWorldTickEndMask`) reads the plugin's own tag
  map and will see any alias however produced; `mask_map.json` already keys events by value and frame range.
- **Unit proof:** 90 events on a 55-value pool — 44584e7 leaves the last 35 without a mask (280 labelled frames), the new
  logic tags all 90 with 35 recycles and no frame with two in-flight events under one value; 40 events → 0 recycles,
  allocation identical. The no-aliasing can-fail is a deliberately broken variant (release at fire end, before read-back):
  it aliases on a 1-value pool and recycles a carried trail's value while attached on a 3-value pool; the ruled logic does
  neither. 44584e7 never recycles, so it cannot fail the no-alias case — it fails the coverage case.

## 4. Gate (ii)'s 5 unmeasured-mask frames — the G295 signature

The five labelled m52 rows with `target_pixels -1` in 084-03 are B3_FIX si 1171, B9_FIX si 194, B7_FIX si 126, 613 and 615.
**Each has `TARGET MASK UNAVAILABLE … pixels=0` in its leg log**, which is `ServiceTargetMask`'s line for a target arm that
was served by a render whose pixels went to another arm. B9_FIX si 194 in full: `M23 PASS id=…105 servedArms=3
ids=[ …105 205 …106 ]` — the target arms of si 193 (`…105`) and si 194 (`…106`) were served by one mask render — then
`MASK-SERVED si=194 … servingToken=194 … owner=…105 captureArms=2` and the UNAVAILABLE line. That is G295 exactly.
**Not fixed:** the coalesced render belongs to si 193's colour family, so copying its pixels to si 194 would pair a mask with
the wrong picture; the real fix (a readback per target arm, or re-arming on the next render) is G295's filed FUTURE item and
is not trivial. The honest `-1` stands (m49 rule); it costs coverage, not correctness.

## 5. Unit tests (`tools/m52_window_selftest.cpp`, MSVC `/W4 /WX`, standalone) — 189 checks, 0 failures

| test | on the 44584e7 behaviour (or a broken variant) | on the new logic |
|---|---|---|
| `TestG350PostClosure` | `anomaly_present` + entry on 34 unlabelled frames | present on exactly the 12 labelled frames (0/0 and 2/8); transitions {4,5} and {16..23}; empty modes == `Fires.Num() > 0` |
| `TestTransitionTrackOrder` | — | dedupe, out-of-order counted, K_off boundary exact, inherited onset, bounded memory at 300 members |
| `TestAaResolve` | — | TAA/TSR temporal; 2/8/1 defaults; 0 without temporal AA even if set; clamp 64 |
| `TestTransitionGate` | 7/7 FAIL (no flag) | 7/7 PASS; FAILS at clear +9 and onset +3 |
| `TestHideReturn` | residual frame FAILS the gate | blinking in-window and post-revert returns, missing_object, K=0/1/2, ends-visible |
| `TestTagRecycling` | 35 of 90 untagged; release-at-revert aliases, and recycles a carried trail while attached | 90/90 tagged, 35 recycles, 0 alias; 0 recycles on a 40-event run; carried trail safe; 200 events + carry clean |

The legacy side is a transcription of 44584e7's behaviour: `anomaly_present = Fires.Num() > 0` with every fire emitted
(`AnomalyLabelWriter.cpp:58` and the entry loop at 44584e7), no transition key, and `AllocateTag` returning 0 at exhaustion
(`AnomalyMaskMeasure.cpp:82-122` at 44584e7).

## 6. Builds

- Host `E:\IA_BuildCache\_r84_host\StackOBot` (branch worktree, no Content junction), edited in place.
- **Game:** a first attempt failed in 19 s at C2039 (`UWorld::GetFeatureLevel` does not exist in 5.1; `FeatureLevel` is a
  member) — fixed; its log was overwritten, and it printed no compiler warning. Rebuild **exit 0**, 3 actions
  (`Module.AnomalyCapture.cpp`, link, metadata), 65.0 s.
- **Editor:** **exit 0**, 10 actions (AnomalyBench, AnomalyControlServer, AnomalyCapture compiled and linked modularly), 38.2 s.
- **Warnings: 0** compiler warnings in either log (the one `warning` text match is UHT's `-WarningsAsErrors` flag).
- Exe **`2AC0523E26B9B75EB3FE542F25CF99A25207CEB3E5A2968FA6351E1628205125`**, 241,992,192 B, archived with both logs and a
  README at `_binary_baselines\m52fix-2AC0523E\` (hash verified at the archive). Code-only; pairs with container `67EA1FE0`.
- A44 (UTF-16): all 15 new tokens present (list in the archive README); controls `IAI.Bench.StuckMipNoHold` 5,
  `IAI.Capture.TargetMask` 6, unchanged from `B725678B`.
- Comment stripper 0 of 117 changed; CRLF files stay CRLF, the two LF files (pure headers' style, selftest) stay LF; diffstat
  +1,437 / −17, no encoding rewrite. **Not staged.**

## 7. Deviations and limits, stated

- **G350 scope:** the rule is applied to render-truth (m52) entries only. Applying it to every anomaly would change
  `blinking`'s visible in-window rows (fire-active since m23) and break the byte-identity the brief requires; that is a
  separate client-visible decision if wanted.
- **The hide-type K is its own CVar** (`IAI.Label.TransitionHideFrames`), per "K_off for hide types is 1, configurable".
- **Explicit CVar values apply only under temporal AA.** A value set for TAA delivery must not flag frames on an AA-off box.
- **AA detection is once per run, from the console value** (as the engine's default path resolves it). A project that turns
  AA off only via a show flag or camera setting still reports the cvar's method; a mid-run change is not tracked. Documented.
- **K_off is streaming:** it flags non-member frames within K_off after **any** labelled frame, so an interior gap (a flap)
  is flagged too; "after the last labelled frame" cannot be known in capture order without look-ahead. K_on counts the
  event's first K_on labelled frames only.
- **Recycling is reclaim-at-exhaustion** rather than release-at-end (§3), chosen to keep every non-exhausting run identical.
  If the pool is exhausted by more than 55 simultaneously unfinished events, an event still gets no value (counted, honest).
- **Not covered:** the synchronous (non-async) capture path emits no transition keys; the bench lever
  `IAI.Bench.StuckMipLegacyTiming` has no render truth, so m52 rows there are emitted as before and get no transitions.
- **Runtime evidence: none.** Every claim here is source plus offline unit tests.

## 8. State and hand-off

- Branch `fix/m52-label-timing`: `05c7649` (source + tests) and the docs commit. No tag, no merge. `master` `b5f15a3`.
- **For 084-05b (harness):** the transition-aware gate should treat a frame as exempt only for the event whose entry carries
  `transition`, and FAIL any unlabelled pixel-visible frame without it. The 086-01 checker's reference selection by
  `anomaly_present` now works on fix legs, but transition-only entries do not set it, so reference frames should also exclude
  `transition_present` rows. The > 55-event mask leg reads `mask_tag_recycles` (> 0), `mask_tag_exhausted` (0), the absence of
  `TAG-OWNER VIOLATION`, and a mask on every labelled frame (G295's rare `-1` excepted and counted).
- **Read first on the next bench:** `Capture(labelsync): EFFECTIVE FOR THIS RUN`, `label_aa_method` / `label_temporal_aa`,
  `label_entries_suppressed`, `label_transition_entries`, `TAG RECYCLED` lines.
