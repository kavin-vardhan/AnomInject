# 2026-09-01 — session 065 — m36 S2: selection wired to the census, and the S2 gate set run

**Status at close: S2 IS BUILT, GATED AND COMMITTED as `72d6dd5` on `feature/selection-census`
(parent `344a9c9`). Every S2 gate ran. No gate failed. TWO PREDICTIONS were found to be
DEFECTIVE AS WRITTEN — neither is a build defect and neither was quietly relabelled; both are
in §5 for a chat ruling. NOT PUSHED, NO TAG, NO COOK. Binary `CBBF6644`, staged and archived.**

🔑 **`72d6dd5` IS THE `G140` BOUNDARY SHA.** Wiring the census changes the candidate set, so the
same seed picks different targets across it and **every banked auto-pool run is non-comparable to
a census-ON leg from here on**. What is NOT lost: census **OFF** remains byte-identical to the old
picker (P-C7, re-verified post-S2 below), so every banked run stays comparable to any census-OFF
leg. Census-ON is a **NEW baseline starting at this commit**, not a lost one. ⛔ **If P-C7 ever
fails after `72d6dd5`, THAT is the door closing, and it is a stop.**

## §1 What S2 is, and the one structural decision inside it

Selection now consults MEASURED DRAWN PIXELS before the bounds path. S1 built the census as a
rolling measure-only pass; S2 makes it decide.

🚨 **THE PROVIDER IS INVERTED, AND IT HAD TO BE.** `AnomalyCapture` **depends on**
`AnomalyInjector`, so `TryFireOnce` cannot call the census directly — that edge does not exist and
adding it would be a dependency cycle. So `AnomalyCapture` **registers a provider** into the
auto-injector at census `Begin` and **clears it** at `End`. The contract lives in the LOWER module
(`AnomalyInjector/Public/AnomalyCensusProvider.h`: an opinion enum plus three `TFunction` types) so
both sides see it without either depending on the other. `IAnomaly` untouched;
`GetVisibleRenderableActors/-Infos` untouched; the dashboard picker stays on the bounds path (R8).

**That inversion is also why the door is cheap to hold:** with no provider registered the selection
path is literally the pre-S2 code.

The rule, per candidate per fire: `MEASURED_ZERO` → excluded categorically · `MEASURED_NONZERO` →
eligible iff `pct >= CensusMinDrawnCoveragePct` · translucent → EXCLUDED when the knob is on,
fallback when off · nanite / tag_failed / hidden / not_yet_measured / **EXPIRED** → the bounds path
decides. A fire whose entire consulted pool was fallback logs `CENSUS ALL-FALLBACK` and increments
`census_fires_fallback_all` — the fire is valid; what is not valid is reading it as evidence the
census selected anything.

## §2 THE FLOOR, as ruled by chat this session

- **P-C2 is the gate OF the floor** and runs at its pre-declared pair: **6.0** (Cube eligible and
  selectable) and **10** (Cube refused). Unchanged.
- **Every OTHER census-ON leg runs at `CensusMinDrawnCoveragePct = 0.5`** so the pool is realistic
  and selection is actually exercised. The floor is a leg parameter there, not a prediction.
- Every leg header states its floor. **Knob default stays 6.0.** No amendment to the predictions
  file — nothing predicted changes.
- **P-C8 runs TARGETED**, so selection and floor do not enter it.

## §3 PRE-FLIGHT — two instrument gaps found BEFORE any verdict was read

Both were found by reading a banked S1 leg rather than by running a gate and interpreting it.

**GAP 1 — the per-cycle `DRAWN-COVERAGE` listing capped at 60 entries, sorted by drawn pixels
DESCENDING.** On the 77-candidate bench that truncated the tail, and **the tail is where every
`MEASURED_ZERO` lives**. `P-C1`'s control IS a zero, so its verdict was unreadable, and an absence
read off that line would not have been a reading at all. Measured on the banked `M36_S1HYG2` log:
`StaticMeshActor_100` absent, line ends `(+17 more)`.

**GAP 2 — no per-candidate name for the NOT_MEASURABLE / EXCLUDED classes.** `P-C3` needs
`SM_Ramp2` named nanite on EVERY cycle and `P-C4` needs `BP_SplineSpawn_C` named translucent AND
never measured; only per-class COUNTS existed.

⇒ **Fix, log-only, inside `CloseCycle` so it cannot execute at all with the census off:** the cap
is raised to 512 and a companion **`Census: CYCLE n NOT-MEASURED`** line names every unmeasured
candidate with its verdict and its `timesMeasured` — the positive evidence that no census tag was
ever spent on it. **GAP 3** (the per-candidate fire-time decision is `Verbose`) needed no code: the
gate legs carry `Log LogAnomaly Verbose`.

## §4 THE GATE LEDGER — every leg on `CBBF6644`, floors stated per leg

Anchor re-run first: **P-C7 (the door) RE-VERIFIED POST-S2** on a pose-matched census-OFF pair,
`M36_S2_POSEDISC_A` (pre-S2 `02C1DFA2`) vs `M36_S2_POSEDISC_B` (S2 `CBBF6644`): annotation keyset
48/48, event set identical (8 events), run_summary keyset identical, **no `census_*` keys in either**,
values identical outside the declared run-unique set, frames 90/90. The checker self-proved both
ways first (A-vs-A must pass; A-vs-perturbed-A must fail).

| gate | leg (floor) | result |
|---|---|---|
| **P-C1** | `PC1_PC2_F6` (6.0), 400 frames, auto-pool | **PASS.** `StaticMeshActor_100` **MEASURED_ZERO on 91/91 cycles**, drawn px 0..0; **fired on 0** of **25** fires (≥20 required). Companion: it is in the prefiltered set on **91/91** cycles (absent from 0). Listings complete — absence is a reading. |
| **P-C2** | `PC1_PC2_F6` (6.0) + `PC2_COMP_F10` (10) | **PASS on all substantive conjuncts; ONE LITERAL CONJUNCT NOT MET — see §5.1.** Cube ELIGIBLE at 6.0, fired 25/25, 66,832–66,878 px = 7.23–7.26 % (the banked band). Companion at floor 10: **EXCLUDED-below_floor ×7, ZERO fires**, `no candidate survived the census (3 consulted, 3 excluded, 0 fallback)` ×7 — **the floor knob proven able to refuse.** |
| **P-C3** | `PC3_PC4_MW` (0.5) + `PC3_COMP_NANITE0` | **PASS.** `SM_Ramp2` → `NOT_MEASURABLE(nanite)` on **90/90** cycles, **never** MEASURED_ZERO, `timesMeasured 0` throughout (no census tag ever spent); `census_unmeasurable_nanite ≥ 1` on 90/90 (27–31). Companion with `r.Nanite 0` **echoed live** (`r.Nanite = "0"`): nanite count → **0**, `SM_Ramp2` **measured on 31/31 cycles** at 24,942–30,768 px, fire-time **ELIGIBLE ×8**. The classifier reads the HOST's Nanite state, not the asset. |
| **P-C4** | `PC3_PC4_MW` (0.5) + `PC4_COMP_TRANS0` | **PASS, both knob directions.** With `ExcludeTranslucent 1`: `BP_SplineSpawn_C` → `EXCLUDED(translucent)` on **90/90** cycles **without being measured** (`timesMeasured 0`), `census_excluded_translucent` = 1 on 90/90, **0 fires**. Prefilter-vacuity companion: it IS consulted at fire time (15 decisions), so it was in the set. Companion with the knob OFF: it **IS measured, 52/52 cycles, MEASURED_ZERO 52/52**, excluded via the zero rule instead (`EXCLUDED-zero ×8`), still 0 fires — the banked 8/8 zero reproduced. |
| **P-C5** | all 12 census legs | **PASS.** `census_candidates ≥ 1` on **473 of 473** counted cycles. |
| **P-C8** | `PC8_TARGETED` (0.5), targeted `_49` | **PASS on both named criteria.** `MASK-REDUCE COMPARE` **105 lines, 103 IDENTICAL, 0 FIRST-DIFF**; `vetoed_events` (0) == `VETOED-OBJECT` line count (0). Event count 8 and `mask_nopass_discards` 0 match both the census-OFF leg and the banked m35 Build-B leg `M35_GM4_CTRL49`. B1 PASSED (bbox exactly `CALIB_BBOX`). ⚠ Observation reported, not explained: the 8 per-event `maxCount`s are byte-identical to the census-OFF leg on **6 of 8** events; the other two differ by **+19 and −21** — **opposite signs**, so a "census tags steal pixels" story is NOT supported by the sign pattern. Both series sit in the banked band bar one value 2 px under its low endpoint. **CAUSE NOT ESTABLISHED.** |
| **P-C10** | `PC10_COARM` (0.5), targeted `_73` so the Cube stays visible | **PASS.** `coArmOnly=1` echoed. Census armed **21** times, **`pendingBefore ≥ 1` on 21 of 21** (max 2) — the delayed pop was forced on EVERY census arm, far exceeding the companion's "at least one". The Cube measured on **7/7** cycles in its own band (66,635–66,878) and **no other actor carried a count in that band on any cycle** — the count landed on the Cube's row and on no other row. |
| **P-C11** | `PC11_MAXAGE0` (0.5) + two default-cap legs | **PASS.** At `MaxVerdictAgeTicks 0`: **8 `CENSUS ALL-FALLBACK` warnings, `census_fires_fallback_all = 8`, 8 fires** — every fire loud, every fire-time decision a fallback (not_yet_measured ×3, expired ×10, hidden ×11), zero ELIGIBLE. Companion at the default cap 12: **0 warnings and counter 0 on TWO independent legs** (floor 0.5 and floor 6.0). |
| **P-C12** | `PC12_RESV_ON` / `PC12_RESV_OFF` (both 10.0, so nothing fires and the census is undisturbed) | **PASS, both directions, quantified.** Host tag applied to `StaticMeshActor_73` value 250 outside the plugin map. ON: `M36 STENCIL RESERVATION ON - reserved=1 [ 250 ]`, `hostReserved=1 assignable=54`; **no candidate but the genuine one reads ≥40,000 px on any of 30 cycles.** OFF: `reserved=0 assignable=55`, and **`StaticMeshActor_61` reads 71,579–71,945 px where it reads 23,198–23,358 with reservation ON**. The difference of means is **71,814 − 23,270 = 48,544 px**, and `_73`'s own count is **48,381–48,597** — the host actor's pixels, landing on whichever candidate holds 250. Corroborating: the batch tag range is `227-254` when 250 is reserved and `227-253` when it is not, i.e. the reserved leg must reach 254 to obtain the same 27 usable values. |
| **P-C13** | `PC13_PILLAR` (0.5), packaged MainWorld, `IAI.Bench.Letterbox 1.0` | **conjunct 1 PASS (the point of the gate) · conjunct 2 PASS but THIN · conjunct 3 REFUTED — see §5.2.** Lever APPLIED and echoed (`bConstrainAspectRatio 0->1, aspect 1.7778->1.0000`); `READBACK-LAYOUT rect=(280,0)-(1000,720) picture=720x720` — the non-zero X origin. **Derived `frame_px` = 518,304 / 518,447 (≈ 720×720 = 518,400), NOT 921,600**, against **921,521 / 921,697 on the zero-origin control leg.** The denominator moved with the RECT: the census is rect-relative. Conjunct 2: 43 valid boxes, **0 outside their frame rect** — ⚠ but only **1** of those sits on a genuinely 720×720 frame. |
| **run_summary subset** | ON vs OFF | **PASS.** Delta is **exactly the 11 pre-declared `census_*` keys**, nothing removed, on a declared gate leg. **P6 did not move: annotation keyset 48/48 identical between census ON and OFF.** |

**Cross-leg hygiene, all 12 legs:** `framesNoPass 0 · framesPolluted 0 · batchesLost 0` on every
one; `tagOvertaken` 0–3 (the event mask re-tagging a fresh fire, the expected case); **zero
`CENSUS-HYGIENE DIFF` lines anywhere** — P-C6 re-confirmed free at S2.

## §5 THE TWO PREDICTION DEFECTS — reported, NOT amended into the predictions file

⛔ The predictions file permits appending only **before any measurement against S2 exists**. It now
does, so nothing was appended there. These are for a chat ruling.

### §5.1 P-C2's two halves are mutually exclusive on an S2 leg

P-C2 asks for `MEASURED_NONZERO` **on every census cycle** AND for the control to be **selectable
by the seeded draw**. On the main leg the Cube is measured on **8 of 91** cycles — because at floor
6.0 it is the ONLY eligible candidate, so it is fired on continuously and a hide anomaly makes it
`NOT_MEASURABLE(hidden)` for the other 83. **Selection picking it is what stops it being measured.**

This is not a build defect and the dangerous direction never occurred: **MEASURED_ZERO 0 of 91.**
And the conjunct IS demonstrated, on P-C2's own companion leg, where the floor refuses the Cube so
nothing fires on it: **MEASURED_NONZERO on 30/30 cycles.** The mechanism is measured rather than
argued — on the same legs, `StaticMeshActor_100`, which is never fired on, is measured on **91/91**
and **26/26**.

📌 **Recorded as PASS on all substantive conjuncts with this reading stated; the literal "every
cycle" reading is unobtainable on any leg that also satisfies "selectable".** Chat rules whether
the wording is corrected for future milestones.

### §5.2 P-C13 conjunct 3 predicts the share RISES; it FELL, and the arithmetic says it must

Conjunct 3: *"the known-visible control's drawn share is computed AGAINST THE RECT, so its share
rises relative to the same object on an un-letterboxed leg."* Measured, same two actors:
**3.183 % → 1.792 %** and **2.263 % → 1.648 %** (ratios 0.563 and 0.728). It fell.

**Why the prediction was wrong:** it assumed the drawn pixel count is unchanged between the legs.
It is not — pillarboxing to aspect 1.0 also CROPS the view, and those actors' drawn pixels fell by
0.317× and 0.410×, harder than the denominator's 0.5625×. **The direction of the share is not
determined by the denominator alone.**

✅ **The diagnostic the conjunct exists for is stated in the prediction itself — *"a share that is
unchanged between the two legs is the tell that the denominator never moved"* — and that tell did
NOT fire.** Conjunct 1 measures the denominator directly and unambiguously (518,400 vs 921,600), so
**P-C13's actual claim is established by conjunct 1 and does not rest on conjunct 3.**

⚠ **Honest limit on conjunct 2:** the MainWorld pillarbox leg is HETEROGENEOUS, exactly as m35's
G-M8 recorded — 16 frames at 720×720, then the aspect walks through eight further widths
(782…1218), then 66 frames at 1280×720 as the intro-camera→Bot switch drops the constraint. So of
the 43 boxes checked, only **1** lies on a genuinely offset frame. **n=1 is stated, not smoothed.**

## §6 A SIX-ATTEMPT B1 STREAK, and the discriminator that refused to blame the build

The first door-anchor leg on `CBBF6644` failed the B1 pose gate **6 consecutive attempts** —
against a recorded base rate of about 2-in-5. Run conditions were identical on every attempt
(`start_frame 1`, `speed_ratio ≈ 1.0`, 90 frames); only the settled camera rotation differed.

Rather than re-roll to a green, the pre-S2 binary `02C1DFA2` was swapped in **hash-verified** and
run on the identical payload: it reached the calibrated pose exactly (`modal_rot (0,0,0)`,
bbox `(0.0, 485.2, 306.1, 234.8)`) on attempt 2. Then `CBBF6644` was swapped back and **reached the
same calibrated pose exactly on the next attempt**. ⇒ **the streak is NOT build-linked; both
binaries reach the calibration pose. CAUSE NOT ESTABLISHED**, and the S2 delta over the prior
binary is log-only code that cannot execute with the census off.

📌 **New harness control born from it:** `run_leg.ps1 -RequireModalRotZero`. B1 is scoped to
`StaticMeshActor_49` (G117), so on an auto-pool leg it correctly declares itself NOT APPLICABLE —
which left the A47 bifurcation **uncontrolled on exactly the legs whose candidate set depends on
where the camera settled**. A47 as amended measured the bifurcation in ROTATION with eye position
invariant on 369/369 gate samples, which is target-independent and therefore available where B1 is
not. **Proven both ways against banked known answers before its first verdict was read.**
⛔ Deliberately NOT applied to MainWorld legs — that measurement is a property of the gate level,
and applying it to a map that settles at `(0,-40,0)` would be G117's error on a new axis.

## §7 Gotcha candidates (filed at milestone end; listed here so they survive)

1. **A truncated result is an absence-of-finding only as good as the surface actually read** —
   G136's shape, pagination edition. Two instances this arc: a search result truncated at 25 that
   made me re-implement an existing console command; and a LOG LINE capped at 60 entries whose sort
   order put every zero in the truncated tail, which would have made P-C1 unreadable.
2. **`struct Foo*` written inside a namespace declares a NEW type in that namespace**, not a
   reference to the global one. It compiles at the declaration and fails at the point of use with
   "use of undefined type". Fix: a global forward declaration plus `::Foo`.
3. **A validity gate that is honestly NOT APPLICABLE still leaves its variable uncontrolled.**
   B1 saying "not applicable" on an auto-pool leg is correct and is not the same as that leg's pose
   being controlled; a target-independent criterion existed and had to be reached for.
4. **The harness banks the process log into the `_try<N>` directory, not the accepted-leg
   directory** — a reader that looks only in the accepted dir finds no log and can conclude the leg
   produced none.

## §7.5 S3 — P-C9, THE COST. NUMBERS ONLY. NO THRESHOLD, AND NONE IS PROPOSED.

**The instrument had to be finished first.** P-C9 names a **per-cycle** line and only a run TOTAL
existed; a total over cycles is a MEAN, and a mean would hide exactly the spikiness worth looking
for (the mask pass has a recorded hitching finding, session 054). `CloseCycle` now emits
`tagBlockMs / overTicks / perTickMs / flagFlips` per cycle as deltas of the existing cumulative
counters. Log-only, inside `CloseCycle`, so it cannot execute with the census off. Binary
**`70F6B72C`** (archived), and **`P-C7` was RE-ANCHORED to it before any S3 leg ran** — census OFF
still byte-identical to the pre-S2 leg.

**Leg set, declared before execution:** 1920×1080 · CB_GateLevel · auto-pool · pose controlled by
`-RequireModalRotZero` · **A = census OFF, B = census ON at floor 0.5** · order **DISCARD, A₁, B₁,
B₂, A₂** (G186) · **pacing OFF** on all five · **both sides carry `IAI.Capture.Mask 1` and only
`IAI.Capture.Census` differs**, so the delta is the CENSUS and not the mask. 77 candidates,
`framesNoPass / framesPolluted / batchesLost` all 0 on every census leg.

| leg | per captured frame | per engine frame | per MP | sustained fps | speed_ratio | game_clock_ratio |
|---|---|---|---|---|---|---|
| DISCARD (declared) | 15.5531 | 11.1631 | 7.5005 | 85.97 | 0.3490 | 0.3490 |
| A₁ OFF | 16.4389 | 11.7989 | 7.9277 | 81.34 | 0.3688 | 0.3688 |
| B₁ ON | 18.8494 | 13.5290 | 9.0902 | 70.93 | 0.4229 | 0.4229 |
| B₂ ON | 18.8727 | 13.5457 | 9.1014 | 70.85 | 0.4234 | 0.4234 |
| A₂ OFF | 17.0051 | 12.2053 | 8.2008 | 78.63 | 0.3815 | 0.3815 |

- **Per CAPTURED frame:** A mean **16.7220** (spread 0.5662) · B mean **18.8610** (spread 0.0233) ·
  **B−A = +2.1390 ms**, against a worst within-build spread of 0.5662 ⇒ **the difference EXCEEDS
  the instrument's resolution.** Unlike `G-M6`, this cost is MEASURABLE.
- **Per ENGINE frame:** A **12.0021** · B **13.5373** · **B−A = +1.5352 ms**, worst spread 0.4064.
- **Per MEGAPIXEL:** A 8.0643 · B 9.0958 · **B−A = +1.0316 ms/MP.**
- ⚠ **EXTRAPOLATION, LABELLED AS ONE AND NOT A MEASUREMENT:** at Concorde's 3200×2000 (6.40 MP)
  that is **6.60 ms per captured frame IF the cost scaled linearly with pixels.** It is an
  assumption — the reduce is per-pixel but the tag block is per-candidate, so the two halves cannot
  both scale that way.
- ✅ **The A,B,B,A design earned its keep (G186).** The box drifted slower across the session
  (15.55 → 16.44 → 18.85 → 18.87 → 17.01), so A's two legs bracket B's two in time and A's mean is
  centred rather than biased. In `A,B,A,B` the drift would have flattered B.

🚨 **THE COMPONENT ACCOUNTING IS THE FINDING, AND IT VINDICATES S1'S WARNING EXACTLY.** The timed
tag block totals **9.64 ms per run** = **0.0778 ms per engine frame**, against a measured cost of
**1.5352 ms per engine frame**. ⇒ **only 5.1 % of the census's cost is INSIDE the block the
instrument times; 94.9 % is OUTSIDE it** — the deferred proxy recreates (1,187–1,259 flag flips per
run) and the extra render-side mask passes. **Quoting `tagBlockMs` as "the census's cost" would
under-read it about twentyfold.** Per-cycle distribution, census-ON legs: `tagBlockMs` min 0.2998 /
median 0.5610–0.6063 / max 0.9067; `perTickMs` min 0.0333 / median ~0.078 / max 0.2244 — **no spike
of hitching scale in the timed block.**

✅ **THE PACED PAIR ANSWERS THE DELIVERY QUESTION, AND IT ANSWERS IT CLEANLY.** Same box, same
resolution, `Pace 1`: per-captured-frame **44.5693 ms on BOTH legs, identical to four decimals**;
per engine frame **32.7824 on both**; `speed_ratio` 1.0000005 vs 1.0000006. ⇒ **at the shipped
30 fps the pacer absorbs the census entirely — it costs nothing observable in wall time.** That
reproduces journal 061 finding 1 exactly and is why the measuring legs had to run pacing OFF.
⛔ It does **not** mean the census is free: it means the box had headroom at 1920×1080. A host
without that headroom would show it in `speed_ratio`, which is the existing instrument for it.

📌 **Observation recorded, CAUSE NOT ESTABLISHED:** cycles per engine frame differ between the two
pacing regimes — 16–17 cycles / 124 engine frames unpaced vs 26 / 121 paced. Reported as a number;
no mechanism is claimed.

## §8 NOT done, named

- **NO TAG. NO COOK.**
- **P-C9 is REPORTED, NOT GATED, exactly as pre-declared. No threshold exists anywhere in the
  tooling or the docs, and none is recommended.**
- The Bates reading sheet is unrun (it is owner-run on a sealed host; the RDP card covers it).
- `master` untouched. `feature/stencil-capture` untouched. No force-push. No ratio, no threshold.
- Binary chain this session: `02C1DFA2` (pre-S2, archived) → `E046D1CA` (S2 pre-instrument,
  archived) → **`CBBF6644`** (S2 final, archived as
  `_binary_baselines\StackOBot.exe.m36-s2-CBBF6644`, 241,025,536 B). Container UNCHANGED — code-only
  hot-swap, G103. A44 green both encodings on the staged artifact, with pre-existing tokens as the
  positive control.
  → **`70F6B72C`** (S3 per-cycle cost line; archived as
  `_binary_baselines\StackOBot.exe.m36-s3-70F6B72C`, 241,026,048 B). `P-C7` re-anchored to it.
- Harness: CaptureBench `28249c5` (`m36_pc9_cost.py` added).
- Commits: `72d6dd5` S2 (**the G140 boundary**) · `dd5ed05` docs · `f9cb764` S3 cost line.

## §9 Evidence bank

`M36_S2_POSEDISC_A` (pre-S2 door A-side) · `M36_S2_POSEDISC_B` (S2 door B-side) ·
`M36_S2_DOORB_try1..3` and the second set (the B1 streak, every attempt banked) ·
`M36_S2_PROBE_AUTO` (declared PROBE, not a gate) · `M36_PC1_PC2_F6` · `M36_PC2_COMP_F10` ·
`M36_PC3_PC4_MW` · `M36_PC3_COMP_NANITE0` · `M36_PC4_COMP_TRANS0` · `M36_PC8_TARGETED` ·
`M36_PC10_COARM` · `M36_PC11_MAXAGE0` · `M36_PC12_RESV_ON` · `M36_PC12_RESV_OFF` ·
`M36_PC13_PILLAR` — all under `_bench_sessions_bank\` with their process logs.
Checkers: `m36_s1_pc7_check.py` (self-proving, reused for the door) and the new
`m36_s2_census_check.py` (self-proving on every invocation).
