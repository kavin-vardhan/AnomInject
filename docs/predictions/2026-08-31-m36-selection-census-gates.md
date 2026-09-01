# m36 — selection census: pre-declared predictions and gates

**Committed BEFORE any m36 code exists that can measure (chat ruling: predictions first, as their
own `docs:` commit).** Branch `feature/selection-census`, cut from `feature/mask-gpu-reduce` tip
`784d31f` (R1). Milestone number m36 confirmed free before use: no `m36` in tags, tracked files,
the two untracked CHAT-HANDOFF docs, or CaptureBench (R0).

**What m36 is:** candidate selection based on pixels actually drawn on screen, not bounds. The m26
mask pass + m34 per-tag GPU reduce are reused at SELECTION time as a rolling multi-target CENSUS
upstream of `TryFireOnce`. The armed-frame measurement and the zero-only veto are UNCHANGED and
remain the backstop. `annotation.json` field set unchanged (P6). `run_summary` gains census
counters only (S2). Stencil range stays 200–255 with 255 the residual detector, never mintable by
any allocator (R7).

## The selection rule (per candidate, per fire)

- `MEASURED_ZERO` → excluded. Categorical.
- `MEASURED_NONZERO` → eligible iff `100 * drawn_px / frame_px >= CensusMinDrawnCoveragePct`
  (NEW knob, R3 — initialised 6.0 in v1; deliberately NOT `GMinScreenCoveragePct`, whose operand
  stays the bounds rect for the NOT_MEASURABLE fallback path).
- `NOT_MEASURABLE(reason)` → SPLIT policy (R2):
  - `nanite`, `tag_failed`, `hidden`, `not_yet_measured` → today's bounds path decides
    (occlusion trace + bounds coverage), counted per reason.
  - `translucent` (every slot of every renderable component translucent without
    `AllowTranslucentCustomDepthWrites`) → EXCLUDED from selection, not fallback
    (`Census.ExcludeTranslucent`, default 1, echoed; counter `census_excluded_translucent`).
- Verdict freshness: usable iff age ≤ `Census.MaxVerdictAgeTicks` (default 12, echoed). Expired ⇒
  `not_yet_measured` until re-measured. First fire defers in `WaitCensus` ≤ 12 ticks until census
  cycle 1 completes (R6); all later fires consume the rolling table, never defer.
- Every fire logs verdict age-at-use per candidate consulted; a fire whose pool is ENTIRELY
  not_yet_measured/expired logs a WARNING and increments `census_fires_fallback_all` (R5).

## Tag discipline (R4 + R10) — stated here because P-C10/P-C12 gate it

- A batch's tags stay ON until that batch's results are COLLECTED or declared LOST — never
  untagged earlier. Tag values are never reused while any batch holding them is in flight
  (ledger-exclusive claims; a batch WAITS rather than reuses; ≤2 batches in flight, each capped
  at half the assignable pool so the second batch always has values).
- Host reservation (R10): at StartRun, every primitive component with `bRenderCustomDepth == true`
  and a `CustomDepthStencilValue` in 200..254 that the plugin did not tag RESERVES that value for
  the whole run — never assigned by the census OR the event allocator. Reserved set + count echoed
  at StartRun. No per-cycle rescan in v1 (`Census.Reservation` exists as the off-lever for the
  P-C12 companion only).

## Predictions

Every equality gate carries a vacuity companion. Markers: **[BENCH]** = needs this box's harness
and artifacts; **[BATES-TYPABLE]** = decidable on a sealed office host from typed counts (StartRun
echo + run_summary values + warning-line counts read off the screen) plus an eye judgment.

- **P-C1 [BENCH]** Known-occluded control (`StaticMeshActor_100`, CB_GateLevel — fully occluded
  9/9 by `_86`, banked; the bench analog of Bates' underground rocks): NEVER selected across N=20
  census-ON fires; census reads `MEASURED_ZERO` on every census cycle in which it is prefiltered.
  Companion: `_100` IS in the prefiltered set on those cycles (banked: 97.6 % on-screen, poll
  1031.9 cm < 1800) — else void.
- **P-C2 [BENCH]** Known-visible control (`StaticMeshActor_49`, Cube — banked drawn 66,843–66,878
  px = 7.23–7.25 % of a 921,600-px frame, above the 6.0 floor by construction): `MEASURED_NONZERO`
  on every census cycle; ELIGIBLE under `CensusMinDrawnCoveragePct=6.0`; selectable by the seeded
  draw. Companion: with `CensusMinDrawnCoveragePct 10`, the same target is INELIGIBLE
  (below_floor) — the floor knob proven able to refuse.
- **P-C3 [BENCH]** `SM_Ramp2` (known-Nanite control, G134): `NOT_MEASURABLE(nanite)` on EVERY
  cycle, NEVER `MEASURED_ZERO`, no census tag ever spent on it; `census_unmeasurable_nanite ≥ 1`
  per counted cycle. Companion (classifier reads the HOST's Nanite state, not the asset): a leg
  launched with `-ExecCmds="r.Nanite 0"` classifies `SM_Ramp2` measurable and the census MEASURES
  it (verdict is MEASURED_*, not nanite).
- **P-C4 [BENCH] (amended per R2)** `BP_SplineSpawn_C` (`SM_GenericPlane`, banked
  `translucentSlots=1/1`): with `Census.ExcludeTranslucent 1` (default) it is
  `EXCLUDED-translucent` WITHOUT being measured — no census tag spent, `census_excluded_translucent
  ≥ 1`, zero fires on it. Companion (both directions of the knob): with `Census.ExcludeTranslucent
  0` it IS measured, and at the bench pose reads `MEASURED_ZERO` (banked 8/8) — excluded via the
  zero rule instead; still zero fires. Second companion (prefilter vacuity): it IS in the
  prefiltered set (banked poll_distance −19405.5 — the radius can never cull it).
- **P-C5 [BATES-TYPABLE]** Vacuity floor: `census_candidates ≥ 1` on every census cycle counted as a
  pass; a cycle with 0 candidates counts toward NO prediction.
- **P-C6 [BENCH]** Tag hygiene, both directions (G96): after ANY census cycle and at FinishRun,
  the set of components with `bRenderCustomDepth == true` and their stencil values are
  byte-identical to the pre-run snapshot (per-cycle check excludes components legitimately tagged
  at check time; the final check excludes nothing). Companion: with the gate-only leak probe
  (`IAI.Capture.CensusLeakProbe 1`) the final check REPORTS A DIFF naming the leaked component —
  the instrument proven able to fail.
- **P-C7 [BENCH]** Census OFF: artifact-identical to the parent-tip build at the same seed/map/
  config — control pair against `733FE83C` (the parent-tip staged binary): event set identical by
  `(target, anomaly_type, start_frame)`, `annotation.json` keyset 48/48, `run_summary` keyset
  UNCHANGED (census_* keys NOT emitted when OFF), frames count identical. Run-unique fields per
  the established 54-field control-pair method; log lines are NOT part of this gate (the census
  echo line exists either way).
- **P-C8 [BENCH]** Backstop untouched: on a census-ON leg, the armed-frame mask lines (M23
  PASS/REDUCE) and the zero-only veto behave identically on the SAME events as the banked m35
  Build-B gate legs (`COMPARE IDENTICAL` where `MaskReduce both`; `vetoed_events` == VETOED-OBJECT
  line count).
- **P-C9 [BENCH]** Cost: REPORTED, NOT GATED, in v1. Instrument pre-declared: per census tick, the
  tag/untag/arm block's game-thread ms and the proxy-recreate count (flag flips counted at tag
  time); per cycle, a summary line; bench A/B census ON vs OFF at 1920x1080, order A,B,B,A after
  one declared discard leg (G186), t_wall normalised per ENGINE frame (journal 059 §1), pacing per
  standard leg noting the pacer masks hook cost (journal 061 finding 1). Numbers to chat; chat
  rules. No threshold anywhere.
- **P-C10 [BENCH]** Attribution control (R4): on a leg that forces delayed pops — gate-only
  `IAI.Capture.CensusCoArm 1` makes the census arm ONLY on ticks where the event mask also armed,
  so the census arm is always queued BEHIND an event arm and pops on the NEXT view family — the
  known-visible Cube's count lands on the Cube's row on every cycle and on no other row.
  Companion: the census ARM log shows `pendingBefore ≥ 1` on at least one census arm (positive
  evidence a delayed pop occurred), else void.
- **P-C11 [BENCH gate / BATES-TYPABLE reading]** Loud-inert control (R5): with
  `Census.MaxVerdictAgeTicks 0`, EVERY fire logs the all-fallback WARNING and
  `census_fires_fallback_all == fires`. Companion: same leg shape at the default cap (12),
  warning count == 0 and `census_fires_fallback_all == 0`.
- **P-C12 [BENCH]** Reservation control (R10): before StartRun, gate-only
  `IAI.Capture.CensusHostTag <actor> 250` sets stencil 250 + `bRenderCustomDepth` on an untracked
  bench actor that draws ≥1 px at the gate pose (set OUTSIDE the plugin's tag map, so it reads as
  host-set). With reservation ON (default): the StartRun echo lists 250 reserved, 250 is never
  assigned during the run, and that actor's pixels appear in NO candidate's count. Companion
  (G96, the instrument can fail): with `Census.Reservation 0`, the pixels DO land on whichever
  candidate is assigned 250 (observed as that candidate's count including the host actor's
  pixels / a first-diff vs the reservation-ON leg).

## Stage mapping (stop-on-failure; no same-turn fix on a failed gate)

- **S1** (build; census OFF by default; measuring core only, selection NOT wired):
  clean compile · **P-C7** · **P-C6** (census ON, hygiene + leak-probe legs).
- **S2** (wire TryFireOnce provider, WaitCensus, run_summary keys; PRECONDITION: m35 CLOSED on the
  parent): **P-C1 · P-C2 · P-C3(+companion) · P-C4(+companions) · P-C5 · P-C8 · P-C10 · P-C11 ·
  P-C12**. run_summary subset-gate delta pre-declared = exactly the census_* keys:
  `census_frames, census_cycles, census_candidates, census_zero, census_below_floor,
  census_excluded_translucent, census_fires_fallback_all, census_unmeasurable_nanite,
  census_unmeasurable_tag_failed, census_unmeasurable_hidden, census_unmeasurable_not_yet_measured`.
- **S3**: **P-C9** numbers to chat.

## Bates reading sheet (owner-run, sealed box, results typed back + eye judgment)

The Bates leg cannot run any home checker. What it CAN return, and what each reading means:

1. StartRun echo (photo or typed): `census=ON` + provenance, `floor=6.00`, `maxVerdictAgeTicks=12`,
   `excludeTranslucent=1`, `reservation=1 reserved=N [values]`. Expectation: line present and
   census ON — else the leg is VOID by the G139 rule (a census that silently didn't run reads as
   a clean null).
2. `run_summary.json` typed values: `census_candidates` (P-C5 satisfied iff ≥ 1),
   `census_unmeasurable_nanite` — **expected 0 on Bates (the addendum: Bates has no Nanite at all); a
   non-zero here means the classifier is misfiring and the leg reports it rather than hiding it**,
   `census_zero`, `census_excluded_translucent`, `census_fires_fallback_all` (expected 0 on a
   healthy leg; non-zero = the R5 loud-inert path fired — report, do not re-run to a green),
   `vetoed_events` (the banked Bates band is 12–15 per run; the census-ON expectation is a marked
   drop, REPORTED as a number, not gated — the veto stays the backstop and any residual vetoes
   are the backstop working).
3. Eye judgment: the selected targets are things a viewer can see change (the product definition).
   One typed list of fired target names + "visible: yes/no" per event is sufficient.
4. FAILURE IS FAST still applies (journal 061 §12): a Bates run that survives its first armed frame's
   execution has cleared the crash class; declaring m36 working on Bates still needs the 90-frame
   floor.

Bench-only by construction (cannot be typed back): P-C1–C4 controls, P-C6 hygiene byte-diff,
P-C7 control pair, P-C8 COMPARE lines, P-C10 attribution — these certify at home; Bates certifies
the product outcome via the sheet above.

---

## AMENDMENT — 2026-09-01, BEFORE ANY `S2` LEG RUNS. Tightening, nothing above withdrawn.

**Why appending is permitted:** no measurement against `S2` exists yet. `P-C1`–`P-C12` are unchanged.

### P-C13 [BENCH, and the shape of the Bates host] — the census must be RECT-RELATIVE, not viewport-relative

On a **packaged `MainWorld` leg with the pillarbox lever** (`G193` — the lever applies there because
the view target is camera-bearing, unlike `CB_GateLevel`'s `SpectatorPawn`):

1. **`frame_px` equals the VIEW-RECT area, not the viewport's.** At `IAI.Bench.Letterbox 1.0` on a
   1280×720 viewport the measured rect is `(280,0)-(1000,720)` ⇒ **`frame_px` = 720×720 = 518,400**,
   **NOT** 1280×720 = 921,600. A census that reports the viewport's area is measuring a frame it did
   not capture, and every share it computes is wrong by the ratio of the two.
2. **Every drawn bbox lies INSIDE the rect** — no box may extend into the pillarbox bars, in
   rect-local coordinates.
3. **The known-visible control's drawn share is computed AGAINST THE RECT**, so its share *rises*
   relative to the same object on an un-letterboxed leg, by construction. A share that is unchanged
   between the two legs is the tell that the denominator never moved.

🚨 **COMPANION, AND IT IS THE POINT OF PUTTING THIS IN WRITING (`G192`'s shape):** **the same leg at
origin `(0,0)` is NOT GATE-BEARING for this prediction.** At a zero origin the rect and the viewport
are the same rectangle, so rect-relative and viewport-relative arithmetic **produce identical
numbers** — a passing zero-origin leg would be comparing an expression to itself and would pass
whether the census is rect-relative or not. **Only the offset leg counts.** This is `G-M9`'s lesson
carried forward before it can be repeated: identify where the two candidate behaviours degenerate
into one, and make sure the gate is not standing on that point.

⇒ Run `P-C13` on the **pillarbox** leg. The zero-origin leg may be run alongside as a control, and it
is reported as a control, never as the pass.
