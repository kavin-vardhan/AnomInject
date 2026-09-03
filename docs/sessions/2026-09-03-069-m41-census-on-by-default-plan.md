# Session 069 — `m41` "census ON by default": PLAN ONLY

**Date:** 2026-09-03 · **Bootstrap SHA:** `0bd1a6d` (== `origin/master`, verified) · **Effort:** high
**Scope of this session:** plan + predictions. **No source change, no build, no leg.** Implementation is
a separate brief after approval.

**Owner ruling being planned against:** the selection census ships **ON by default**.

---

## §1. THE `m41` PLAN

### §1.0 What `m41` is, in one paragraph

`m41` makes the m36/m37 selection census the shipped default rather than an opt-in, and lands four
changes that a source review said are preconditions for that: a **translucent-only exclusion that no
longer has a custom-depth loophole**, a **host post-process preflight** that says out loud whether
anything on the host reads CustomDepth/CustomStencil, **verdict expiry measured against the census's
own cycle length** instead of a fixed 12 ticks, and a **coverage assertion** that counts selection
candidates the census has never seen. It changes **no rendered pixel**, **no `annotation.json` field**,
and adds **three `run_summary` counters** on top of the existing twelve — all inside the `census_*`
block that is emitted only when the census is effective. What it does change is **which actors get
selected on a host where bounds and drawn pixels disagree**, which is the entire point.

⛔ **Not in `m41`: tag persistence.** See §1.6 — its premise was checked against 5.1 engine source
this session and the *correctness* half of it does not survive that check. It becomes `m42`, and
`m42`'s first task is a measurement, not an implementation.

---

### §1.1 ITEM A — THE DEFAULT FLIP

#### A.1 The coupling, stated exactly

`bCensusEffective = bCensus && bMaskMeasure && bAsyncCapture` — `AnomalyCaptureSubsystem.cpp:1480`.

| term | compiled default | ini key | today's delivered value |
|---|---|---|---|
| `bAsyncCapture` | **true** (`AnomalyCaptureSubsystem.h:287`) | none | true |
| `bMaskMeasure` | **false** (m27 ruling) | `bMaskMeasureDefault` | **True** — already in the client ini since m27 (`client-delivery.md:299-303`) |
| `bCensus` | **false** | `bSelectionCensusDefault` | absent ⇒ off |

Two consequences, and they pull in different directions:

1. **For the client, flipping `bSelectionCensusDefault` alone is sufficient** — the mask is already ON
   in the delivered cook and has been since m27.
2. **For any host with no ini — including this bench — a census compiled-ON beside a mask compiled-OFF
   is a census that can never run.** The existing warning at `:1502` ("census was REQUESTED but is
   INACTIVE") would then fire **on every run of the shipped default**. That inverts the loud-inert rule
   into noise: a warning that always fires is a warning nobody reads.

#### A.2 Recommendation — flip BOTH compiled defaults to `true`

1. **Coherence.** A compiled default that cannot take effect under its sibling's compiled default is
   not a default; it is a trap for the next host that has no ini.
2. **Safety, and this is the strongest argument.** Today, if the cooked `DefaultGame.ini` ever loses
   `bMaskMeasureDefault`, the client silently gets **m25 labelling** — invisible anomalies back, with
   no artifact difference except the provenance line nobody reads. That is precisely the `G139` failure
   `m27` exists to make *visible*. Flipping the compiled default removes it **structurally**: a lost
   ini key becomes a provenance downgrade, not a disabled cure.
3. **The reason for `false` has lapsed.** `m27` compiled it `false` because the cure was new and
   unmeasured. It has since run on Bates (Sections A and B) and on Concorde, and it is the delivered
   configuration in every client build since `m27`.

**Costs, named:**

- **The mask's own cost has never been isolated on any host.** `G-M6` (the hook-cost prior) was never
  run. ⛔ **Do not quote S3's `+1.5352 ms/engine frame` as the mask's cost** — both S3 sides ran
  mask-ON, so that figure is the **census above an already-ON mask**, unpaced, 1080p, dev box. Any host
  with no ini now pays a mask cost that this project has never measured.
- **Bench comparability boundary.** Every banked bench leg that ran mask-OFF stops being comparable to
  a default leg. This is a `G140`-class boundary; it is declared at the `m41` SHA in the predictions
  file. In practice `run_leg.ps1` passes explicit `-ExecCmds`, so the harness pins it — but the
  boundary is written down rather than relied upon.
- **`G-R7(ii)` becomes a HARD delivery precondition for the next client cook.** See §1.1.5, verbatim.

**Fallback if the mask flip is refused:** flip `bCensus` only, **and** gate the `:1502` warning so it
fires only when the census was requested from ini or console — never from the compiled default — with
a Log-level line on the compiled-default path. → **NEEDS-DECISION 1.**

#### A.3 File-by-file

| file | change |
|---|---|
| `Source/AnomalyCapture/Public/AnomalyCaptureSubsystem.h` | `bCensus` init `false → true`; `bMaskMeasure` init `false → true`; add `bool bCensusFromConsole = false;` |
| `AnomalyCaptureSubsystem.cpp:365-370` | unchanged logic; ini still overrides, still sets `bCensusFromIni` |
| `AnomalyCaptureSubsystem.cpp:917-935` (`SetCensus`) | set `bCensusFromConsole = true` |
| `AnomalyCaptureSubsystem.cpp:1046-1053` (`DescribeCensusSource`) | 🚨 **must-fix, not cosmetic.** It currently returns `"COMPILED DEFAULT (off) or IAI.Capture.Census"`. After the flip that string is **false** — the compiled default is ON, so an `off` reading can only come from console or ini. New form: console → `"IAI.Capture.Census (console)"`; ini → the ini key; else → `"COMPILED DEFAULT (on)"`. **This is `G139`'s own failure mode living inside `G139`'s fix.** |
| `AnomalyCaptureSubsystem.cpp:457-463` and `:1456-1466` (mask echoes) | same correction for the mask: `"COMPILED DEFAULT (off)…"` → provenance-exact, three branches |
| `AnomalyCaptureSubsystem.cpp:1500-1507` | keep the warning; it now genuinely means "someone turned the mask off" |
| `docs/client-delivery.md` | the ini block below + a paragraph saying the keys are now redundant-but-explicit |
| `docs/PRE-DELIVERY-CHECKLIST.md` | the `G-R7(ii)` box (§1.1.5) |
| `CLAUDE.md` | status refresh; **and the `+11 census_*` correction** (§1.1.4) |

#### A.4 Client ini block, and the key-set delta

```ini
[AnomalyCapture]
bMaskMeasureDefault=True
bDeliveryModeDefault=True
bSelectionCensusDefault=True
CensusMinDrawnCoveragePctDefault=0.5
CensusMaxDrawnCoveragePctDefault=25.0
CensusMaxVerdictAgeTicksDefault=12
bCensusExcludeTranslucentDefault=True
```

⚠ With the compiled defaults flipped these become **redundant-but-explicit**. **Keep them** — they are
what makes the StartRun provenance echo read `ini`, which is the only way a reader can confirm the key
reached the cook. The doc must say they are belt-and-braces, or someone will "clean up" a redundant key
and re-open the `G88` question. ⚠ `CensusMaxVerdictAgeTicksDefault`'s meaning changes with item D —
sequence that doc line after D.

**`run_summary.json` key-set delta when the census is effective — 12 today → 15 at `m41`:**

existing 12 (`AnomalyLabelWriter.cpp:517-528`): `census_frames` · `census_cycles` · `census_candidates` ·
`census_zero` · `census_below_floor` · `census_above_ceiling` · `census_excluded_translucent` ·
`census_fires_fallback_all` · `census_unmeasurable_nanite` · `census_unmeasurable_tag_failed` ·
`census_unmeasurable_hidden` · `census_unmeasurable_not_yet_measured`
new 3: **`census_host_pp_customdepth_readers`** (C) · **`census_fires_partial_fallback`** (D) ·
**`census_fires_unseen_candidates`** (E)

📌 **CORRECTION TO THE LIVE RECORD.** `CLAUDE.md` says "+11 `census_*` keys" in two places and the m36
subset gate measured "exactly the 11". **`m37` added `census_above_ceiling`, making it 12.** The `m41`
subset gate expects **15**. The journals are records and are not retro-edited; `CLAUDE.md`'s status
block is corrected at the docs commit.

⛔ **`annotation.json` does NOT move.** `P6` unmoved, measured, as at m36 and m37.

#### A.5 The mask-default and `G-R7(ii)` statement — VERBATIM, to be carried into the docs

> **`bCensusEffective = census && mask && async`, so shipping the census ON requires the mask ON.** The
> mask's compiled default is `false` today; the client ini has set `bMaskMeasureDefault=True` since
> `m27`, so the delivered cook already runs it. `m41` flips the compiled default to `true` as well, so
> that a lost ini key downgrades provenance instead of silently restoring `m25` labelling. **The cost
> of the census above an already-ON mask is S3's measured +1.5352 ms per engine frame (1080p,
> unpaced, dev box); at the shipped paced 30 fps the dev box absorbed it entirely — that is headroom,
> not free.** The mask's own cost has never been isolated on any host. **`G-R7(ii)` — the
> physical-only hitch + throughput gate on master's own cook — is therefore a HARD DELIVERY
> PRECONDITION for the next client cook, not a merge precondition and not optional.** Nothing ships to
> the client off master until it passes there.

#### A.6 Comparability

- **`P-C7` (census OFF ≡ pre-census picker, byte-identical) stays gated and is re-anchored on the
  `m41` build.** The inert path is **structural**: with `bCensusEffective` false the provider is never
  registered (`AnomalyCaptureSubsystem.cpp:1834-1870` is guarded by it, cleared at `:3322`), so
  `CensusQuery` is null and the selection loop in `AnomalyAutoInjectorSubsystem.cpp:277-312` is the
  pre-m36 code. Measured, not asserted.
- **Census-ON becomes the baseline from the `m41` SHA.** The bisect `IAI.Capture.Census 0` still
  reaches every banked census-OFF leg. Banked *auto-pool* legs from before `72d6dd5` were already
  non-comparable to any census-ON leg (`G140`); nothing new is lost.

#### A.7 Gates and predictions

| gate | leg | prediction |
|---|---|---|
| **A-G1** inert-when-OFF (`P-C7` re-anchor) | `m41` build, `IAI.Capture.Census 0`, pose-matched against a census-OFF control | `frame_indices` identical · `labels.jsonl` 0 row diffs · **no `census_*` key present** · `run_summary` differs only by the declared run-unique trio |
| **A-G2** default provenance, **both directions** (`G96`) | (a) no ini, no console; (b) `IAI.Capture.Census 0` | (a) echo reads census **ON**, source **`COMPILED DEFAULT (on)`**, mask **ON** same; (b) reads **off**, source **`IAI.Capture.Census (console)`**. ⚠ **Both branches must name a single source — a disjunction here is the defect** |
| **A-G3** key-set subset | census-ON leg vs a pre-`m41` census-OFF leg | delta is **exactly the 15 `census_*` keys**, nothing else |
| **A-G4** `P6` unmoved | same leg | `annotation.json` 48/48, added 0 removed 0 |

⚠ **Honest limit on A: the bench cannot certify this item.** Max drawn coverage on this bench is ≈6 %,
so the 25 % ceiling never bites and floor 0.5 admits nearly everything measured non-zero — selection may
be **nearly unchanged here**. **Bates is the instrument (item G).**

---

### §1.2 ITEM B — THE TRANSLUCENT LOOPHOLE

#### B.1 Current behaviour, from source — the brief's description is confirmed exactly

`AnomalyCensus.cpp:40-64`, `ComponentSlotsAllTranslucentWithoutOptIn` returns **false** — "not
translucent-only" — the moment **any** slot's material answers `IsTranslucencyWritingCustomDepth()`
true. So a fog card whose material has *Allow Custom Depth Writes* ticked:

1. is classified **Measurable** (`ClassifyCandidate:109-137`);
2. is tagged, writes custom depth, and the mask counts its **geometric silhouette** — a fog card is
   large, so it reads `MEASURED_NONZERO` at a healthy `drawnPct`;
3. clears the floor, clears the ceiling, is selected, is hidden — and **nothing visibly changes**,
   because the card contributes little or no visible colour;
4. 🚨 **and the armed-frame veto does not catch it either, for the same reason: both the census and the
   veto read the SAME custom-depth silhouette. They are not independent checks here.**

That fourth point is why a knob-level fix at the census layer is the right shape and a veto-level fix
is not — see B.3.

#### B.2 The rule and its file-by-file

**Ruling:** translucent-only ⇒ `EXCLUDED(translucent)` **regardless of custom-depth writes**, by
default. A new knob restores the old behaviour for hosts that want glass-type targets.

| file | change |
|---|---|
| `AnomalyCensus.h` | `FAnomalyCensusParams` += `bool bIncludeTranslucentCustomDepthWriters = false;` |
| `AnomalyCensus.cpp:40-64` | rename → `ComponentSlotsAllTranslucent(Prim, bAllowCustomDepthOptIn)`; the `IsTranslucencyWritingCustomDepth()` early-return becomes conditional on that argument |
| `AnomalyCensus.cpp:84-138` | `ClassifyCandidate` gains the flag and forwards it |
| `AnomalyCensus.cpp:158-169` (`Begin` banner) | echo `includeTranslucentWriters=%d` |
| `AnomalyCaptureSubsystem.h` | `bool bCensusIncludeTranslucentWriters = false;` + `bool bCensusTranslucentWritersFromIni/FromConsole` |
| `AnomalyCaptureSubsystem.cpp:389-394` | ini `bCensusIncludeTranslucentCustomDepthWritersDefault` |
| `AnomalyCaptureSubsystem.cpp` (new setter, beside `:982`) | `IAI.Capture.CensusTranslucentWriters <0|1>` — mid-run guarded, out-of-range refused, provenance echoed |
| `AnomalyCaptureSubsystem.cpp:1481-1499` | the **existing** `EFFECTIVE FOR THIS RUN` census line gains `includeTranslucentWriters=%d(%s)` — one line, not a new one |
| `AnomalyCaptureSubsystem.cpp:993-998` + `:4270-4280` | help text must state the two knobs' relationship (below) |
| `AnomalyCaptureSubsystem.cpp:1836-1842` | thread the flag into `CensusParams` |

**Two knobs, one question each — say it in the help text or they will be confused:**
`IAI.Capture.CensusTranslucent` decides **whether translucent-only candidates are excluded at all**.
`IAI.Capture.CensusTranslucentWriters` decides **whether a custom-depth-writing translucent still
counts as translucent-only**. The second is only consulted when the first is ON.

⚠ **Naming deviation, stated:** the ini key takes the brief's name verbatim
(`bCensusIncludeTranslucentCustomDepthWritersDefault`); the console command shortens to
`IAI.Capture.CensusTranslucentWriters` to sit in the existing `IAI.Capture.Census*` family.

#### B.3 Should the armed-frame VETO apply the same class rule?

**No — and I agree with chat's lean, with a reason and a named residual.**

- The veto is `manifested && MEASURED_ZERO`, and `m26`'s whole discipline is that it deletes **only** on
  a measured zero, with **no ratio and no class rule**. Adding "or translucent" makes the veto delete
  on a *class test*; if that test is wrong it destroys data, which is the dangerous direction.
- The census layer **refuses to select**, producing no event at all — the safe direction, and the
  census's stated job ("UPSTREAM of selection only; the armed-frame measurement and the zero-only veto
  are unchanged and remain the backstop").
- ⚠ **Named residual, recorded not queued:** with B in place a translucent-only target can still reach
  an event via the **fallback path** (unseen/expired) or via **targeted fire**, and the veto will not
  catch it because it reads non-zero. That is a limitation of `m41`, not a hole to plug by widening the
  veto. If it turns out to matter it earns its own milestone and its own gate.

#### B.4 Gate B-G1 — both directions (`G96`)

A bench fixture: one actor, a single **translucent-blend** material slot with *Allow Custom Depth
Writes* ON, in frustum at the settled bench pose.

- **knob OFF (the new default):** the `CYCLE n NOT-MEASURED` listing names it `EXCLUDED(translucent)`;
  `census_excluded_translucent` ≥ 1; it never appears in an `Auto.Fire: CENSUS '<name>' -> Eligible`
  line.
- **knob ON:** the same actor reads `MEASURED_NONZERO` with a plausible `drawnPct`, and
  `census_excluded_translucent` is one lower.

**Failure branch, pre-declared:** if it reads `EXCLUDED(translucent)` with the knob **ON**, the fixture
is wrong, not the code — check `GetBlendMode()` (a *Masked* material is not translucent) and
`IsTranslucencyWritingCustomDepth()` on the resolved material. **Report both flags; do not adjust the
fixture until the cause is named.**

⚠ **Fixture cost, stated up front.** No such actor exists on the bench today, and **`CB_GateLevel` is
FROZEN (`G99`)**. Three routes: (a) a sibling level; (b) a **bench-only runtime spawn**
(`IAI.Bench.SpawnTranslucentProbe`, default absent, console-only, never in a client payload) — no
cooked asset changes, `make_gate_level.py` never invoked; (c) run the check on **Bates**, where the fog
card is the actual motivating instance. **Recommendation: (b) for the both-directions proof here, plus
(c) as the confirmation on the real instance in item G.** → **NEEDS-DECISION 2.**

---

### §1.3 ITEM C — HOST POST-PROCESS PREFLIGHT

#### C.1 The Shipping answer — and a correction to the question

**`ANOMALY_CAPTURE = 0` in Shipping, so the census does not exist there at all. Shipping is moot.** The
real constraint is a **cooked build with `WITH_EDITORONLY_DATA = 0`**, which is what a client capture
build (packaged Development/Test) is. In that build:

- ⛔ **Expression walking is unavailable** — `UMaterial::Expressions` /
  `UMaterialExpressionSceneTexture::SceneTextureId` are editor-only data, stripped by the cook.
- ✅ **A runtime read exists, and it is the right instrument.**
  `FMaterialCompilationOutput::UsedSceneTextures` is a **`LAYOUT_FIELD(uint32, ...)`**
  (`Engine/Public/MaterialShared.h:735`) — a *serialized* field of the frozen shader map, declared
  **outside** the `#if WITH_EDITOR` block, therefore **present in a cooked build**. It is exposed as
  **`FMaterialShaderMap::UsesSceneTexture(uint32 TexId)`** (`MaterialShared.h:1382`, public, after the
  editor block closes at `:1379`).
  `ESceneTextureId::PPI_CustomDepth = 13`, `PPI_CustomStencil = 25`
  (`Engine/Public/MaterialSceneTextureId.h:45, 69`).
- **Access path, game thread:** `UMaterialInterface::GetMaterialResource(FeatureLevel)` → `FMaterial*`
  → `GetGameThreadShaderMap()` (`MaterialShared.h:2097`, public) → `UsesSceneTexture(PPI_CustomDepth)`
  / `(PPI_CustomStencil)`. A `UMaterialInstanceDynamic` resolves to its parent's shader map, so MIDs
  are covered correctly and for free.

⇒ **C is implementable in a cooked build. The honest-fallback branch the brief asked for is not
taken.** The limits are stated instead (C.4), and the owner's one-glance card check is **retained as a
second instrument** — not as a fallback.

#### C.2 Sources enumerated at StartRun

1. `TActorIterator<APostProcessVolume>` over the capture world → `Settings.WeightedBlendables.Array[]`
   (record `bEnabled`, `Priority`, `BlendWeight`, unbound flag).
2. `PC->PlayerCameraManager->GetCachedPostProcessBlends(OutSettings, OutWeights)`
   (`Camera/PlayerCameraManager.h:381`, **public**) — the accumulated per-frame blend list (camera
   component PP + camera modifiers). One authoritative source; do **not** hand-walk `UCameraComponent`s
   and risk a second, disagreeing enumeration.
3. Each blendable `UObject*` → `Cast<UMaterialInterface>` → the shader-map query above.

#### C.3 The line, and the field

```
Capture(census): HOST-PP CUSTOM-DEPTH READERS = 0 (scanned V volume(s), C camera blend(s), M material(s))
Capture(census): HOST-PP CUSTOM-DEPTH READERS = N [<Name>(depth), <Name>(stencil), ...] (scanned V/C/M) - WARNING
```

`= 0` at **Log**, `= N` at **Warning**, naming each material and which texture it reads. Printed
**unconditionally whenever the census is effective** — loud either way.
`run_summary` += **`census_host_pp_customdepth_readers`** (int), inside the `census_*` block.

🔑 **The `scanned V/C/M` counts are load-bearing and are not decoration.** A `= 0` with `scanned 0/0/0`
is **blindness**, not a clean read. They are what make the zero readable — `G96` applied to a preflight.

#### C.4 Limits — these go in the line's own text and in the doc, unsoftened

1. It detects that a material **samples** CustomDepth/CustomStencil. It does **not** prove the sample
   changes a pixel (a material may sample and multiply by zero). **`N > 0` is a "look at this", not a
   defect.**
2. It cannot see a reader **outside the material system** — a host C++ scene-view extension or custom
   render pass, a Niagara or UMG reader, a decal. ⛔ **`N = 0` does NOT mean "nothing on this host
   reads custom depth."**
3. It is a **snapshot at StartRun**. A blendable added mid-run (`AddOrUpdateBlendable`, a camera
   modifier that starts later) is missed.
4. Weight-0 and disabled volumes are reported as present — deliberately; reporting a disabled reader
   is the safe direction.
5. The shader map queried is the world's feature level's.

#### C.5 Gate C-G1 — split into a cheap always-runnable half and an expensive half

- **C-G1a (the reader mechanism, always runnable):** assert the query correctly reports a texture id a
  **stock** PP material does use — essentially every PP material reads `PPI_PostProcessInput0`. This
  proves the enumeration + shader-map path are alive without authoring anything. **Prediction:** the
  scanned counts are non-zero and `PostProcessInput0` reads true on at least one enumerated material.
- **C-G1b (the CustomDepth branch, both directions):** a PP material that samples
  `SceneTexture: CustomDepth`, applied through an `APostProcessVolume` blendable. Present → `= 1`,
  named, at Warning. Removed → `= 0` at Log with scanned counts non-zero.

⚠ **C-G1b needs a cooked asset** — a SceneTexture expression must be *authored*, so it cannot be a pure
runtime spawn, and this gate therefore **needs a cook** (every census gate so far ran on a code-only
hot-swap). Routes: (a) plugin `Content/` (⛔ wrong — it would ship in the plugin); (b) **host-project
bench content, cooked only into the BenchGate build**; (c) skip the positive direction here and take it
on Bates, where a real reader may exist. **Recommendation: (b).** → **NEEDS-DECISION 3.**
📌 The a/b split means that even if C-G1b is deferred, the detector is **not** shipping with an unproven
mechanism — C-G1a covers the scan; only the CustomDepth bit-position would be unproven, and that is a
much smaller unproven surface than "the whole preflight".

---

### §1.4 ITEM D — VERDICT EXPIRY RELATIVE TO CYCLE LENGTH

#### D.1 Current behaviour and why it bites

`AnomalyCensus.cpp:298-305`: `Age = GFrameCounter - MeasuredAtTick; if (Age > MaxVerdictAgeTicks) →
"expired"`, knob default **12**. Cycle length is already computed at `CloseCycle`
(`CycleTicks = GFrameCounter - CycleStartTick`, `:748`) and already logged. On Bates a cycle is ≈4 ticks
— fine. On a host with enough candidates that one cycle exceeds 12 ticks, the **earliest-measured**
candidates of each cycle expire before the cycle even closes ⇒ selection biases toward whatever was
measured last. **The only loud signal today fires on ALL-fallback**, so *partial* bias is silent.

#### D.2 New semantics

```
FreshnessWindow = max(MaxVerdictAgeTicks, LastCompletedCycleTicks + Margin)
Margin = FAnomalyCensus::LostAfterTicks (8)
```

- `Margin` **reuses an existing constant** rather than minting one: `LostAfterTicks` is already "how
  long a batch may be in flight before it is declared lost", which is exactly the extra latency a
  fresh verdict can legitimately carry.
- `LastCompletedCycleTicks` is the value already computed at `:748`, stored into a new member at
  `CloseCycle`. **Zero until the first cycle closes**, in which case the window is just the knob —
  unchanged behaviour, and WaitCensus already defers the first fire until a cycle completes.
- **The knob keeps its name and becomes the absolute FLOOR of the window** — it can only make the
  window larger than the cycle-relative term, never smaller.
- ⚠ **Semantics change of a shipped knob**, so `G139` applies: the StartRun echo and the console help
  must print the **effective window**, not just the knob value.

🚨 **NEEDS-DECISION 4 — "the knob kept as an absolute cap" has two readings and one of them undoes the
fix.** If the knob is a true **upper bound** on the window, then on the 300-candidate host it exists
for, the window is clamped back to 12 and nothing changes. **Recommendation: `max()` (knob = floor),
plus the existing `[0,600]` clamp acting as a runaway ceiling on the whole window** so a stalled cycle
cannot produce an unbounded freshness window. Both readings are "an absolute cap"; only one of them
works.

#### D.3 The per-fire line (shared with E) and the counters

```
Auto.Fire: census consulted=N eligible=E excluded=X fallback=F expired=Y unseen=U (window=W ticks) -> <id> on <target>
```

at **Log** verbosity, one line per fire attempt whenever a provider is registered and `Consulted > 0`.

- Counting `expired` and `unseen` needs them distinguishable from ordinary fallback. The reason strings
  exist, but `Reason` is a `const TCHAR*` to a literal and cross-TU pooling is not guaranteed ⇒
  ⛔ **never compare it by pointer.** **Instead: `FAnomalyCensusOpinion` gains `bool bExpired` and
  `bool bUnseen`** — additive to a struct in the lower module, no enum change, no contract widening.
- The existing ALL-fallback Warning stays.
- **The fire-report callback widens:** `FAnomalyCensusFireReportFn` becomes
  `TFunction<void(int32 Consulted, int32 Fallback, int32 Unseen)>`; `NoteFireAllFallback(bool)` becomes
  `NoteFire(Consulted, Fallback, Unseen)`, incrementing `FiresFallbackAll` (Fallback == Consulted),
  **`FiresPartialFallback`** (Fallback > 0) and **`FiresUnseenCandidates`** (Unseen > 0).

📌 **NEEDS-DECISION 5 (minor, naming):** `census_fires_unseen_candidates` reads ambiguously — fires, or
candidates? **Recommendation: make all three `census_fires_*` fields FIRE COUNTS** (they then read as a
consistent trio), and put the per-fire **candidate** count in the log line only. Consistency wins over
a marginally more informative field name.

| file | change |
|---|---|
| `AnomalyCensus.h` | `+ uint64 LastCompletedCycleTicks = 0;` · counters `+ FiresPartialFallback`, `+ FiresUnseenCandidates`, `+ HostPpCustomDepthReaders` · `NoteFire(...)` replaces `NoteFireAllFallback(bool)` · `int32 GetFreshnessWindowTicks() const` |
| `AnomalyCensus.cpp:298-305` | the window expression + `Out.bExpired` |
| `AnomalyCensus.cpp:695-760` | store `LastCompletedCycleTicks`; the `CYCLE n DONE` line gains `window=` |
| `AnomalyCensus.cpp:189-198` (`SUMMARY`) | the three new counters |
| `AnomalyInjector/Public/AnomalyCensusProvider.h` | `FAnomalyCensusOpinion` `+ bExpired`, `+ bUnseen`; `FAnomalyCensusFireReportFn` signature |
| `AnomalyAutoInjectorSubsystem.cpp:274-331` | count `expired`/`unseen`; emit the Log line; call the widened report |
| `AnomalyLabelWriter.{h,cpp}` | the three new `run_summary` fields |
| `AnomalyCaptureSubsystem.cpp:958-979`, `:4240-4260` | `IAI.Capture.CensusMaxAge` help says **floor of the window**, not a fixed age |

#### D.4 Gates and predictions

- **D-G1 (long cycle, no early expiry) — and the A-side problem is solved in advance.** Bench-only
  `IAI.Bench.CensusBatchCap <n>` (default 0 = off) caps `HalfCap`; at `n=2` with ≈60 bench candidates a
  cycle stretches to ≈30 ticks, well past 12. 🚨 **To show "before" you need the old semantics, and the
  lever is `m41` code — that is exactly `m40`'s L2 problem, which cost an unarchived intermediate
  binary. Do not repeat it:** ship a second bench-only console `IAI.Bench.CensusFixedExpiry <0|1>`
  (default 0) forcing the pre-`m41` fixed window, so **A and B sides run on ONE binary.**
  **Prediction:** A-side `expired > 0` on early-batch candidates, candidate pool skewed to
  late-measured actors; B-side `expired = 0` with `window ≈ cycleTicks + 8`. The `consulted=` line
  present on **every** fire in both.
- **D-G2 (no regression at normal cycle length).** Standard bench leg, no lever. **Prediction:
  `window = 12`** (the bench's 1–3-tick cycles put `lastCycleTicks + 8` under the knob), `expired = 0`,
  and the leg is otherwise identical to A-G1's census-ON control — i.e. **byte-inert where the cycle is
  short**, which is every host measured so far.

---

### §1.5 ITEM E — PREFILTER ⊇ FIRE-VISIBLE-SET

#### E.1 Source finding — the two predicates are NOT mismatched

- **prefilter** (`AnomalyViewport.cpp:864-909`): ∃ component with `IsRenderableComponent` ∧ poll-radius
  ∧ `IsInFrustum`.
- **fire** (`GetVisibleRenderableActors:835-862` → `ClassifyRenderableVisibleLive:450-467` →
  `IsComponentRenderableVisibleInternal:255-268`): ∃ component with `IsRenderableComponent` ∧
  poll-radius ∧ `IsInFrustum` ∧ **`IsUnoccluded`**, then (coverage > 0) the union must pass
  `PassesScreenCoverage`.

Both go through the same `IsRenderableComponent` `G33` chokepoint (foliage exclusion, `m27` name
patterns, ISM instance count, static-or-skinned), the same `ResolvePollOrigin`, the same `GPollRadius`,
and the same frustum (`BuildFrustum:183-188` is byte-identical to the inline construction at `:845-847`).
⇒ **At the same tick and the same view, fire-visible ⊆ prefilter, structurally.**

🔑 **So an "unseen" candidate is not a predicate bug — it is TEMPORAL.** `Entries` is rebuilt from a
prefilter snapshot taken at `StartCycle` (`AnomalyCensus.cpp:385`) and carries verdicts only for actors
present in the **new** list. An actor that entered the frustum or the radius after cycle start has **no
entry**, and `QueryActor`'s `!Found` branch (`:257-262`) returns `FallbackBounds / "not_yet_measured"`
— **today indistinguishable from "entry exists, measurement pending".** That indistinguishability is the
defect worth fixing: it is the same shape as D, a silent recency bias made visible.

Two further routes, named so they are not mistaken for the first: (i) `Prefiltered.Num() == 0` (no view)
leaves `Entries` empty and **every** actor unseen; (ii) `HeldElsewhere` (already tagged, or no renderable
component) **creates** an entry with `NotYetMeasured` and so reads as `not_yet_measured`, **not** unseen
— correct, and precisely why the two reasons must be separate strings.

#### E.2 Implementation, and the one thing not to do

`QueryActor`'s `!Found` branch sets `Reason = TEXT("unseen")` and `bUnseen = true`. **The decision does
not change** — still `FallbackBounds`; only its explanation does. The consumer counts it (§1.4.3) and
`census_fires_unseen_candidates` reports it.

⛔ **Do NOT "align the two functions" by adding occlusion or coverage to the prefilter.** That would
shrink the census's measured set to exactly the set bounds-based selection already trusts — which
defeats the census's purpose (it exists to measure things bounds wrongly admits) and would blind it to
an actor that becomes unoccluded between cycles. **The prefilter being WIDER is correct by design.**
Stated explicitly because "align the two functions" is the brief's phrasing and the aligned direction is
the wrong one.

#### E.3 Gates and predictions

- **E-G1 (the reading):** two standard bench legs, census-ON, settled camera — the paced default and the
  B-1-leg-2 shape. **Prediction: `census_fires_unseen_candidates = 0`**, because the camera is settled
  and the level static, so the prefilter set is constant across cycles. **A non-zero here is a finding**
  — most likely `Prefiltered.Num() == 0` on the fire's own tick. Report verbatim; do not adjust.
- **E-G2 (prove-it-can-fail, `G96`) — required.** A counter whose whole job is to be believed when it
  says zero on a client host cannot ship with an unproven zero. Bench-only
  `IAI.Bench.CensusDropEntry <n>` (default 0) omits every *n*th actor from the prefilter list at
  `StartCycle`; those actors are still seen by the fire path and have no entry.
  **Prediction: at `n=2`, `unseen ≈ half the consulted set` on every fire.**
  ⛔ **Do not accept "0 on the bench, detector unproven" as a pass.**

---

### §1.6 ITEM F — PERSIST TAGS / ROTATE VALUES IN PLACE → **VERDICT: `m42`**

#### F.1 The premise is CONFIRMED from 5.1 engine source

- `UPrimitiveComponent::SetRenderCustomDepth` → `MarkRenderStateDirty()` — **and only when the value
  actually changes** (`PrimitiveComponent.cpp:4075-4082`). A full render-state recreate.
- `UPrimitiveComponent::SetCustomDepthStencilValue` → `SceneProxy->SetCustomDepthStencilValue_GameThread`
  (`PrimitiveComponent.cpp:4084-4097`) → an `ENQUEUE_RENDER_COMMAND` writing **one scalar** on the proxy
  (`PrimitiveSceneProxy.cpp:964-984`). **No recreate.**

⇒ **"the value-set is an in-place proxy update; the flag flip is the full recreate" is verified, not
assumed.** Rotating values is cheap. The optimisation is real.

#### F.2 But the stated CORRECTNESS motivation does not survive the same read

The brief promotes the one-frame TSR ghost/shimmer from a cost item to **the** correctness motivation,
via "each tag flips the render proxy → motion vectors reset for a frame". In 5.1:

- Previous-transform / velocity state lives in **`FSceneVelocityData`**, keyed by
  **`FPrimitiveComponentId`**, and its own class comment reads: *"Tracks primitive transforms so they
  will be persistent across rendering state recreates."* (`ScenePrivate.h:2410-2413`; accessors
  `GetComponentPreviousLocalToWorld:2425`, `UpdateTransform:2443`). The component id is **stable across
  a recreate**.
- `UpdateTransform` is guarded by `check(Proxy->HasDynamicTransform())` — most census candidates are
  **static** and have no velocity to lose in the first place.
- `m26`'s banked `F-1` finding already established that the deferred recreate is flushed **inside the
  same frame's** `BeginRenderingViewFamilies`, so the primitive is **not missing** from the frame.

⇒ 🚨 **The named mechanism is NOT SUPPORTED by the 5.1 source.** ⛔ **It is not refuted as a *symptom*:**
GPU-Scene and static-draw-list churn on a recreate are real, and their per-frame pixel effect has
**never been measured here**. Per the observation-vs-mechanism invariant (`G120`), the concern stands as
**unmeasured**; the mechanism does **not** stand as stated.

#### F.3 Why `m42`, in order of weight

1. **Its justification is now unmeasured.** Building a tag-lifetime redesign to fix an unmeasured pixel
   effect is the `ReservedStencilMax` mistake again — a change targeting a symptom whose mechanism
   source refutes. **`m42`'s FIRST task is a measurement:** does a census flip on a captured frame move
   a pixel? Instrument: two legs on **one** binary, census-ON vs census-ON-with-flips-suppressed at a
   matched pose, read with the grid/luma instrument under `G125`'s marker discipline (strict cross-run
   byte identity is known-unobtainable here).
2. **It reverses a closed ruling.** m36's *"tag-lifetime rules, closed by construction: a batch stays
   tagged until collected/LOST; values never reused in flight."* Persisting tags makes every candidate
   permanently tagged, so `IsAnyComponentTagged` — the `HeldElsewhere` guard at `AnomalyCensus.cpp:90`
   and the arm-time skip at `:648` — degenerates to always-true, and the event mask's tagging of a
   fired target always collides with a live census tag (`TagOvertaken`). That is an allocator redesign,
   not an optimisation.
3. **It makes item C's hazard permanent instead of intermittent.** With tags persisted, ~60 actors
   write custom depth on **every** frame of the run. Parking values at 0 hides them from our own mask
   (`Stencil < ReservedBase → 0`, `AnomalyVisibleMask.usf:26-30`) but **not from the custom-depth
   buffer**, which is exactly what a host post-process reads. ⇒ **C must land and read `= 0` on the
   target host before F is even safe to consider.** That is an ordering *reason*, not a preference.
4. `m41` already moves the client default plus three behaviours. One variable at a time.

#### F.4 What `m41` carries from F

**Nothing in code. One correction in the docs:** the PERSIST-TAGS note in `CLAUDE.md` and the ledger is
amended so it records the §1.6.1 confirmation **and** the §1.6.2 non-support, so the next reader does
not build on a refuted mechanism.

---

### §1.7 ITEM G — BATES VALIDATION, `office-rdp-card.md` **SECTION E**

Editor/PIE over RDP on the `m41` build, same route as Section D (pull → `Build.bat StackOBotEditor` →
Play). The packaged Bates build stays untouched.

**E-0. PASS CONDITIONS — pre-declared, read before running anything.**
(a) the StartRun echo reads census **ON** with source **`COMPILED DEFAULT (on)`** and mask **ON**;
(b) the `HOST-PP CUSTOM-DEPTH READERS =` line is present **with its scanned counts non-zero**;
(c) `aboveCeiling ≥ 1` with an `ABOVE-CEILING` line naming the landscape-class actor;
(d) the fog-card actor appears in the `NOT-MEASURED` listing as `EXCLUDED(translucent)`;
(e) an `Auto.Fire: census consulted=…` line on **every** fire;
(f) eye list: anomaly visible at ≥ the m36 leg-2 rate (~90 %), **no repeat of the same 2–3 targets**,
**no pitch-black frames**.

⚠ **(c) and (d) can legitimately come back different and still be results.** `aboveCeiling = 0` ⇒ the
landscape actor's drawn coverage moved (different window/letterbox) — report the histogram verbatim. The
fog card reading `MEASURED_NONZERO` ⇒ its material is **not** translucent-blend (likely Masked), so B's
rule does not reach it — **a finding that changes B's scope, not a failed gate.**

**E-1. Update the box** — mirrors D-1: `git status` clean → pull → confirm the SHA → rebuild the
**EDITOR** target (runbook §8.6 STEP 3.5 is not optional).

**E-2. The run.** `blinking` **TICKED** (P9 closed, mitigation lifted). **`IAI.Capture.RunLog 1` FIRST**
— Bates runs delivery mode, so `m38`'s run log is auto-OFF there (`G210`); forcing it puts the whole
read in `anomaly_log.txt` beside `annotation.json` instead of console scrollback. Then
`IAI.Capture.Config 2 4 8 4 0` and `IAI.Capture.Start "" png 4242 90`.

**E-3. The reads, in order.** (1) the two `EFFECTIVE FOR THIS RUN` census/mask lines, whole; (2) the
`HOST-PP` line, whole; (3) `CYCLE n DONE` + `DRAWN-COVERAGE histogram` + `ABOVE-CEILING` +
`NOT-MEASURED` from one settled cycle; (4) every `Auto.Fire: census consulted=` line; (5) the 15
`census_*` keys from `run_summary.json`; (6) the eye list — **target name + visible yes/no per event**,
written down during the run.

**E-4. Two things that must not happen.** ⛔ **Do not issue `IAI.Capture.Census` or `IAI.Capture.Mask`**
— the leg's entire purpose is that the *compiled defaults* do it, and typing either destroys the read.
⛔ Do not re-run to a green.

**E-5.** D-5's ceiling read is **absorbed into E-3(3)** and D-5 is marked superseded, not duplicated.

**Predictions for G.** Eligible-set size lands between m36 leg-2's ~8 and that minus the fog card and
the landscape actor. `vetoed_events` **0** (as on both m36 legs). **`census_fires_unseen_candidates > 0`
on Bates** — unlike the bench, because the PIE camera moves; that is the counter's first real reading
and it is **expected, not a fault**. `census_fires_partial_fallback > 0` for the same reason.

---

### §1.8 ORDER, COMMIT SHAPE, AND WHAT `m41` DOES NOT DO

**Order: A → B → C → D → E.** A and B first because they are what the owner's dataset needs and B
changes selection on the client; C–E are instrumentation and a semantics fix, and none of them changes
which actors are selected on a short-cycle host.

**Commits:**
1. `docs(m41)` — `docs/predictions/2026-09-03-m41-census-on-by-default.md` (pre-declared gates,
   predictions, failure branches) + this journal. **Before any source.**
2. `feat(census): m41 - census on by default (+ translucent rule, host-PP preflight, cycle-relative expiry, coverage assertion)`
   ⚠ the parenthetical **drops "tag persistence"** per §1.6.
3. `docs` — `CLAUDE.md` status refresh (incl. the 11→12→15 correction and the PERSIST-TAGS amendment) ·
   `client-delivery.md` ini block + census section · `invisible-anomaly-mechanisms.md` census entry ·
   `office-rdp-card.md` Section E · `gotchas.md`.

⛔ **No tags.** The office batch stays `m31 → m33 → m34 → m35 → m36 → m37 → m38 → m40` (+ `m39`, + `m41`).

**Gotchas `m41` is likely to mint, named now so they are not rediscovered:**
(i) an ini key that duplicates the compiled default stops being a correctness dependency and becomes a
provenance readout — say so in the doc, or someone deletes the "redundant" key;
(ii) `FMaterialCompilationOutput::UsedSceneTextures` is a serialized `LAYOUT_FIELD`, so material
scene-texture usage **is** readable in a cooked build — expression walking is the editor-only route, not
the only route;
(iii) a "requested but inactive" warning becomes noise the moment the requested state is the compiled
default.

**`m41` explicitly does NOT:** change a rendered pixel · move `annotation.json` · add a client-facing
*setting* (the ini keys are defaults, not UI) · touch the armed-frame veto's rule · change the
prefilter's predicate · persist tags · tag anything.

---

### §1.9 OPEN DECISIONS (carried to the report)

1. **Flip the mask's compiled default too, or census only + gate the warning?** (§1.1.2)
2. **B's fixture route** — bench runtime spawn vs sibling level vs Bates-only. (§1.2.4)
3. **C-G1b's fixture** — host-project bench content (needs a cook) vs deferring the positive direction
   to Bates. (§1.3.5)
4. **D's "absolute cap"** — floor (`max()`, recommended) or upper bound (undoes the fix). (§1.4.2)
5. **`census_fires_unseen_candidates`** — fire count (recommended, consistent trio) or candidate count.
   (§1.4.3)

---

# Session 069 §2 — `m41` IMPLEMENTED, GATED AND SHIPPED

**Continues** `2026-09-03-069-m41-census-on-by-default-plan.md` (§1 = the plan, `47acfe6`).
Covers briefs 069-02 (implement) and 069-03 (diagnose) and 069-04 (close).
**Pre-declared gates:** `docs/predictions/2026-09-03-m41-census-on-by-default.md` (`2543674`) +
**ADDENDUM 1** (`8fa807d`, written and committed BEFORE the runs it describes).

---

## §2.0 Outcome in one paragraph

`m41` ships the selection census **ON by compiled default**, with the mask's compiled default flipped
with it, the translucent custom-depth loophole closed, a host post-process preflight added, verdict
expiry made cycle-relative, and a coverage assertion that counts candidates the census never saw. It
changes **no rendered pixel** and **no `annotation.json` field**; `run_summary`'s census block goes
**12 → 15 keys**, emitted only when the census is effective. The campaign stopped once on a failed
gate, and **that stop paid for itself**: it exposed a real defect (§2.2) that a gate written without
`scanned` counts would have shipped behind a green tick.

---

## §2.1 The five rulings, as implemented

1. **Both compiled defaults flip ON.** The client ini has carried `bMaskMeasureDefault=True` since
   `m27`, so the delivered cook already ran the mask; flipping the compiled default means a lost ini
   key **downgrades provenance** instead of silently restoring `m25` labelling. The three provenance
   describers now have **three exact branches each** and the pre-`m41`
   `"COMPILED DEFAULT (off) or IAI.Capture.X"` disjunction is deleted — after the flip it was *false*,
   and it was `G139`'s own failure mode living inside `G139`'s fix.
2. **B's fixture is a bench-only runtime spawn.** It **refused** — see §2.4.
3. **`C-G1b` defers to the client cook** as a required pre-delivery gate; `C-G1a` ran here.
4. **`Window = max(knob, LastCompletedCycleTicks + LostAfterTicks)`**, knob = floor.
   🚨 **Carve-out found while implementing, and pre-declared before it was measured:**
   `IAI.Capture.CensusMaxAge 0` is documented as *"0 expires everything and is the `P-C11` loud-inert
   control"*. Under a bare `max()` a knob of 0 would have yielded a window of `cycleTicks + 8` and
   **`P-C11`'s lever would have silently stopped working.** `m41` special-cases knob `<= 0` to a
   window of 0. **A shipped gate lever was one line away from being retired by accident.**
5. **`census_fires_unseen_candidates` is a FIRE count**, so the three `census_fires_*` fields read as
   a consistent trio; the candidate count lives in the per-fire log line.

---

## §2.2 🚨 THE `C-G1a` STOP FOUND A REAL DEFECT — the G96 discipline paying for itself

`C-G1a` was written with an explicit clause: *a `= 0` with `scanned 0/0/0` is **BLINDNESS, NOT A CLEAN
READ** and is a FAILURE.* On `CB_GateLevel` the preflight returned exactly that, the gate failed on its
own written terms, and the campaign stopped without a fix in the same turn.

**Diagnosing it produced two findings, and only one of them was the fixture.**

**(i) FIXTURE — measured first, by two reads that do not use the suspect scan.**
`CB_GateLevel` authors **no `APostProcessVolume`** (its authoring script `make_gate_level.py` spawns
only `StaticMeshActor`, one skeletal actor, `DirectionalLight`, `SkyLight`, `SkyAtmosphere`,
`PointLight`, `PlayerStart`), and the camera blend cache **only ever holds camera-MODIFIER pushes and
is emptied every update** (`ApplyCameraModifiers` calls `ClearCachedPPBlends()` as its first statement,
`PlayerCameraManager.cpp:281`; `AddCachedPPBlend` at `:300-305` is called only from modifier code),
while the `-unattended` pawn is a `SpectatorPawn` with no camera component running no modifier.
⇒ **`V = 0` and `C = 0` are the TRUE answers on that level. Fixture, not code.**

**(ii) 🚨 CODE — and it would have shipped.** The engine assembles a view's post-process from **THREE**
sources (`LocalPlayer.cpp:866-881`): volumes · the cached blends the engine itself comments as
*"CameraAnim override"* (`:870-878`) · and **`View->OverridePostProcessSettings(ViewInfo.PostProcessSettings,
ViewInfo.PostProcessBlendWeight)` under the comment `// CAMERA OVERRIDE` (`:881`)**. The first cut of
the preflight scanned sources 1 and 2 and **missed source 3 — which is where a `UCameraComponent`'s
`PostProcessSettings` actually arrive, i.e. the most ordinary way a host applies a full-screen
effect.** A host doing exactly that would have been reported as a confident `= 0`.

📌 **The lesson, and it is the transferable one: the `scanned` counts are what turned a confident zero
into a question.** A preflight that had printed only `READERS = 0` would have been green on both
levels and would have shipped the missing source. **`G96` is usually about proving a detector can
fire; here it caught a detector that was looking in the wrong place.**

**(iii) The `M = 0` follow-up was NOT attributed until it was measured.** `MainWorld` returned
`V=1, C=1, M=0`, and the addendum had pre-declared that shape as a code defect. Rather than assert it,
a **discriminator** was added — *blendable ENTRIES* reported separately from resolved *MATERIALS* — and
it returned **`entries = 0`**: those settings carry no blendable at all. ⇒ **content, not a broken
walk.** The pre-declared failure branch was **refuted by measurement**, which is a better outcome than
being confirmed by assumption.

---

## §2.3 `P-C7 v2` — the comparator rule, and why it is stronger than "0 row diffs"

**The problem.** `A-G1` demanded `labels.jsonl` **0 row diffs** against a pre-`m41` control. Measured
**90/90 rows differing**, with extras beyond the same-binary run-unique set (`t_wall` alone):
`t`, `frame_index`, `view`, `anomalies`.

⛔ **Widening the run-unique set to excuse this was REFUSED** — `P30` already ruled that route the
laundering shape, and re-running the control until it agreed would have been exactly
"re-run to agreement".

**`P-C7 v2`, journaled here, predictions file untouched:**
- absolute counters (`t`, `frame_index`) are compared as **DELTAS** and must be **ONE constant across
  every row**;
- `view` and pose-derived label fields must be **identical after that constant is removed**, OR differ
  by a **single constant pose delta** that is itself constant across rows;
- everything else **byte-identical**; the run-unique set stays **`{t_wall}`**.

🔑 **This is STRONGER than "0 row diffs", not weaker: it forbids DRIFT, which "0 row diffs" only
forbids incidentally.** A drifting settle pose passes neither, but a rigid translation — which carries
no information about behaviour — passes v2 and fails v1 for the wrong reason.

**§2.3.1 Where the offset came from — measured, not argued.**
The source diff contains nothing that consumes ticks before the first arm (the preflight is inside the
`bCensusEffective` guard and its absence from the census-OFF leg's log is the confirmation; the new
per-fire line is inside `if (CensusQuery …)`; console registration and log strings consume no ticks).
So the ruling's fallback applied: **a variance read, one extra launch of each binary, all values
reported, no leg discarded, no matching pair picked.**

| leg | binary | `start_frame` | arm@si0 | arm@si89 | span |
|---|---|---|---|---|---|
| `M41_OFF` | m41 `5C073AC9` | 1 | 1 | 120 | **119** |
| `M41_OFF_B` | m41 `5C073AC9` | 1 | 1 | 120 | **119** |
| `M41_OFF_C` | m41 `5C073AC9` | 1 | 1 | 120 | **119** |
| `M41_M40_CTRL` | m40 `C0AD3F91` | 4 | **5** | 124 | **119** |
| `M41_M40_CTRL_B` | m40 `C0AD3F91` | 1 | **1** | 120 | **119** |

🔑 **The m40 binary produced BOTH 5 and 1. The `−4` offset is run-to-run STARTUP VARIANCE present
WITHIN the m40 binary itself — it is not a property of `m41` and not a code difference.** The arm
**span is 119 on all five legs**: the whole run is rigidly translated, its internal structure identical.

**§2.3.2 The verdict, both pairs reported.**

| pair | `frame_index` Δ | `t` Δ | `view.rot` Δ | other fields | `anomalies` rows differing | v2 |
|---|---|---|---|---|---|---|
| `M41_OFF` vs `M41_M40_CTRL` | **−4**, one constant ✅ | **−0.164644**, one constant ✅ | **5 distinct values**, yaw drifting 0 → −0.175 → −0.35 → −0.525 ❌ | byte-identical ✅ | 48/90 | **FAIL on the pose conjunct** |
| `M41_OFF` vs `M41_M40_CTRL_B` | **0** ✅ | **0.000000** ✅ | **0,0,0** ✅ | byte-identical ✅ | **0/90** | ✅ **PASS** |
| `M41_OFF` vs `M41_OFF_C` (same binary) | 0 ✅ | 0 ✅ | 0,0,0 ✅ | byte-identical ✅ | 0/90 | ✅ PASS |

⚠ **The failing pair is NOT pose-matched, and `A64` already governs that case:** two legs can each pass
their own gate and still sit in different admissible poses, and the pair-level pose match is a
**precondition of the comparison**, not part of its verdict. The `CTRL` leg's camera was still yawing
through the settle tail — the `A47` rotation axis. **Both legs are banked; neither was discarded.**

🎯 **The pose-matched cross-binary pair returns BYTE-IDENTITY of `labels.jsonl` except `t_wall`.** That
is a stronger result than `A-G1` originally asked for. ⇒ **`A-G1` PASSES.**

**§2.3.3 The substance, recorded separately from the comparator.** The **event set is IDENTICAL across
all five census-OFF legs on BOTH binaries** — same six events, same types, same targets, same spans,
same `frame_indices`, string-equal — **and DIFFERS on the census-ON leg.** Census OFF ≡ the pre-census
picker; census ON changes selection. That pairing is the census's own positive control and it costs
nothing extra to state.

---

## §2.4 Gates — predicted vs measured

| gate | predicted | measured | verdict |
|---|---|---|---|
| `A-G1` / `m40` L4-shape | 0 row diffs | v2: all deltas **0** on the pose-matched pair; event sets identical | ✅ **PASS** (v2) |
| `A-G2(a)` census | `COMPILED DEFAULT (on)` | exactly that, single source | ✅ |
| `A-G2(a)` mask | ini reading satisfies it (ruling 2) | `from DefaultGame.ini [AnomalyCapture] bMaskMeasureDefault` | ✅ |
| `A-G2(b)` | `off` from `IAI.Capture.Census (console)` | exactly that, single string | ✅ |
| `A-G3` | exactly 15 `census_*` | ON 52 keys vs OFF 37 = **+15, all `census_*`**, non-census extras **NONE**, removed **NONE** | ✅ |
| `A-G4` (`P6`) | added 0 / removed 0 | identical | ✅ |
| census-OFF inertness | no keys, no lines | 0 keys · 0 per-fire lines · no HOST-PP line | ✅ |
| `B-G1` | both directions or honest absence | lever **REFUSED**: 3 materials present, all `blendMode=0 translucent=0`; 2 absent | ⚠ **UNRUNNABLE HERE** — rides the cook |
| `C-G1a(b)` probe | 3 targets | engine `BufferVisualization/*` **NOT PRESENT in this container**; opaque control `sceneTextures=0` | ⚠ **PARTIAL** — positive bit folded into `C-G1b` |
| `C-G1a(a)` MainWorld | `V/C ≥ 1` and `M ≥ 1` | `V=1 C=1 entries=0 M=0`, readers 0 | ⚠ prediction not met; **its failure branch REFUTED** (content, not code) |
| `D-G1` A-side | `expired>0`, window 12 | 5/5 fires `expired=3/3`, `eligible=0`, `fires_fallback_all=**5**`, cycle 41–47 ticks | ✅ defect reproduced |
| `D-G1` B-side | `expired=0` every fire | `window=49/55` (=cycle+8) ✅, `fires_fallback_all=**0**` ✅, eligible **10/15** vs 0/15 ✅; `expired>0` on 3 of 5 | ⚠ **corrected prediction** (§2.5) |
| `D-G2` | window 12 | 12–14 on 4–6-tick cycles | ⚠ corrected prediction, by design |
| `E-G1` | unseen 0 | **0** | ✅ |
| `E-G2` | unseen ≈ half consulted | **7/7 fires `unseen=3/3`**, counter=7, lever omitted 39 of 77 | ✅ counter proven able to fire |
| `m38` run log | closes cleanly | present + close marker on **all legs** | ✅ |
| `A44` both encodings | all new strings | all present utf16, ascii 0; positive and negative controls both correct | ✅ |

---

## §2.5 Corrected predictions and named residuals

- **`D-G2`**: the window reads **12–14**, not a flat 12 — the bench's cycles are 4–6 ticks and the rule
  is `max(12, cycle+8)`. **My prediction was wrong; the code is right.**
- **`D-G1` B-side**: `expired = 0 on every fire` was predicted; 3 of 5 fires show 1–2. The decisive
  statistic moved as designed (`fires_fallback_all` **5 → 0**, eligibility **0/15 → 10/15**, one
  binary, one cap, one seed). **Residual, named and NOT tuned:** at a synthetic 41–47-tick cycle the
  `LostAfterTicks = 8` margin does not cover cycle-to-cycle variance. ⛔ **It is not tuned on a bench
  regime manufactured by `CensusBatchCap 2`.** → ledger watch item: **if a real host shows `expired>0`
  with `window>12`, that margin is the first knob to look at.**
- **`B-G1` and `C-G1a`'s positive halves are UNRUNNABLE on this container** and ride the client cook as
  `C-G1b` + the translucent-probe fixture. **Recorded as unrunnable, never as passed.**
- **Exe archiving gap:** `2BF9E1B9` and `7616F144` were overwritten in staging without being archived.
  Stated, not hidden; bounded (rebuildable from history, and the only result taken on `2BF9E1B9` is
  banked in `M41_M41_ON_A`'s own log). Recorded in `_binary_baselines\README.md`.

---

## §2.6 Environment at close

Staged bench exe **`5C073AC9`** (241,122,816 B). Predecessor **`C0AD3F91`** archived and hash-verified
before the swap — it is **`m41`'s A-side** and stays load-bearing. Container quartet **UNCHANGED**
(`2A66CA57` / `A7EF9B12` / `D8009AD7`) — code-only hot-swap, **no cook** (`G103`). Legs banked under
`_bench_sessions_bank\M41_*` (11 dirs, every attempt kept). **No tag.**

---

# §3. `m43` — TARGET ID MASK: PLAN, AND WHY IT STOPS AT THE PLAN

**Date:** 2026-09-03 · **Bootstrap:** `HEAD == origin/master == db2f49b`, verified · **Mode:** plan → gate.
**Owner ruling being planned against:** the client wants an instance mask of the **anomaly targets only**,
in this week's Bates delivery.

⛔ **VERDICT: STOPPED AT THE PLAN. The spec is sound in shape but FOUR of its assumptions are refuted or
contradicted by the source, and two of those change what the client actually receives.** Per the brief's
own rule — *any assumption the source refutes → STOP, do not implement around it* — nothing was built.
No predictions file was written either: the gate set cannot be pre-declared until D1 and D6 are ruled,
because two of the gates as written are unsatisfiable against the design as written.

---

## §3.1 What the spec gets RIGHT — verified, so it is not re-litigated

| spec | verified against source | verdict |
|---|---|---|
| S1 pixel source | `AnomalyVisibleMask.usf` → `MaskRT`, created `PF_R8_UINT` at `SceneColor.ViewRect.Size()` (`AnomalyMaskSceneViewExtension.cpp:124-133`) | ✅ R8, view-rect sized, occlusion-correct by construction |
| S1 "no new shader" | the pass and shader already exist; only a readback + a writer are added | ✅ |
| S2 filter is NEEDED | the shader emits **any** stencil `>= ReservedBase` (`AnomalyVisibleMask.usf:26-30`); only the CPU reduce filters by assigned set. So census tags **do** land in an event frame's RT | ✅ S2 is necessary, not belt-and-braces |
| S2 host values excluded | shader floor at `ReservedBase` + the m36 host reservation | ✅ |
| S3 rect | mask RT size == view rect; the m35 readback copies the view sub-rect into a plugin-owned W×H texture at (0,0), so picture == view rect | ✅ **at native output height** (see D4) |
| S5 requires mask ON | `bMaskMeasure` gates the whole block (`AnomalyCaptureSubsystem.cpp:734`) | ✅ and it is ON by `m41` default |
| gate (ii) feasibility | the m34 GPU table and the surface are both derivable on one armed frame | ✅ and it is the right load-bearing gate |

---

## §3.2 🚨 D1 — COVERAGE. The mask arms PER EVENT, NOT PER FRAME. Measured: ~21 masks for 90 frames.

**`FAnomalyMaskMeasure::MaxArmsPerEvent = 4`** (`AnomalyMaskMeasure.h:44`), and `ArmIfMeasurable`
returns after arming **one** record per tick (`AnomalyMaskMeasure.cpp:205-250`). The mask was never a
per-frame instrument — `m26` sized it to answer *"did this target draw anything during its window"*,
which needs a handful of samples, not ninety.

**Measured on the banked `m41` leg `M41_M41_ON_A` (90 captured frames, 6 events):**

| quantity | value |
|---|---|
| captured frames | **90** |
| `M23 PASS` render-pass executions | **99** |
| `Census: ARM` (census batches) | **78** |
| ⇒ **event mask arms** | **≈ 21** |

⇒ **Under S3 as written, `target_mask/` would hold ~21 PNGs for a 90-frame session — about 23 % coverage
— and ~69 rows would carry `mask_file: null`.**

🚨 **Gate (i) as written is UNSATISFIABLE.** It requires `frames_unavailable == 0 on a healthy async
leg`; on a healthy leg that counter is **~69 by construction**. The gate and the design contradict each
other, so one of them is wrong and it is not for me to pick which.

⚠ **And this is the part that reaches the client.** They asked for an instance mask to train on. A
directory covering 23 % of frames, with the gaps unexplained, is not what "an instance mask of the
anomaly targets" means to someone building a segmentation set.

**Three resolutions, with their real costs — chat/owner's call:**

- **(a) Arm the mask on every fire-active captured frame** (raise/remove `MaxArmsPerEvent` for this
  path). 🚨 **This is the dangerous one and I will not take it unprompted:** the number of frames
  contributing to the `m26` measurement feeds `MEASURED_ZERO` / `MEASURED_NONZERO`, which feeds **the
  veto**. Changing the arming cadence changes the inputs of a **shipped labelling path** — a behaviour
  change wearing an artifact change's clothes.
- **(b) Keep the cadence; ship masks only where they exist**, and document the coverage honestly
  (`mask_file: null` is already the honest signal). Cheapest, zero risk to the veto, and probably not
  what the client wants.
- **(c) A SEPARATE arm budget for the target mask**, independent of `m26`'s: same shader, same pass,
  its own `RequestId` space and its own counter, so `m26`'s `framesContributed` is untouched.
  ⚠ It costs one mask pass + one full-surface readback **per captured frame** rather than ≤4 per event.
  **Recommended if the client wants real coverage** — it is the only option that gives per-frame masks
  without touching the veto's inputs.

---

## §3.3 🚨 D6 — A HIDDEN TARGET PRODUCES NO FILE, NOT A BLANK ONE. Gate (iii) contradicts S3.

`ArmIfMeasurable` refuses on a hidden target — `if (Actor->IsHidden()) { ++R.SkippedHidden; continue; }`
(`AnomalyMaskMeasure.cpp:225-229`). That is **`LOCK-1`, a deliberate and proven guard**: a hidden target
must never be measured, or `m26` would manufacture a `MEASURED_ZERO` and the veto would delete a good
event.

⇒ On a `blinking`-hidden or `missing_object` frame there is **no mask arm, therefore no readback,
therefore — per S3 — no PNG at all.**

**Gate (iii) expects a PNG containing zero pixels of that value. The design as written produces no
file.** Those are different artifacts and they mean different things to a consumer:
- **blank PNG** = "measured, and the target was not visible" — real ground truth, and for a hide-type
  anomaly it is *the most informative frame in the dataset*;
- **no file / `null`** = "not measured" — an absence of evidence.

🔑 **This is the same distinction `m26` fought for at the event level (`NOT_MEASURED` ≠ `MEASURED_ZERO`)
and it now recurs at the frame level.** ⚠ **A hide-type anomaly's hidden frames are exactly the frames a
segmentation consumer needs**, so under S3 the client would get masks for the frames where the target is
visible and nothing for the frames where it vanished — the inverse of what the anomaly is about.

**Resolution requires a ruling:** emit an explicit **all-zero PNG** on fire-active frames where the
target is known-hidden (cheap — no readback needed, the state is already known game-side and `m40`
already samples it at `OnWorldTickEnd`), or accept the gap and re-word gate (iii). ⛔ **Do NOT arm the
mask on a hidden tick to obtain it — that is precisely what `LOCK-1` forbids.**

---

## §3.4 D2 — the surface readback is NOT enqueued under the shipped default. Cost, not a blocker.

`if (Mode != EAnomalyMaskReduceMode::Gpu) { … FRHIGPUTextureReadback … }`
(`AnomalyMaskSceneViewExtension.cpp:174-178`). The shipped default is **`gpu`**, which by design reads
back a **5 KB per-tag table, not the surface** — that is exactly what `m34` was built to achieve.

S1 says "ALSO enqueue the texture readback", so this is **not a deviation** — but it must be stated
plainly: **`m43` re-introduces a full W×H `PF_R8_UINT` surface readback per armed frame, which is the
cost `m34` removed.** At 1280×720 that is ~0.9 MB per armed frame; at the client's resolution,
proportionally more. S7's cost measurement is therefore mandatory, not optional, and it interacts with
D1: under resolution (c) it is paid on **every** captured frame.

## §3.5 D3 — the join key is `GFrameCounter`, not the capture's RequestId. Wording, not design.

Event arms use **`ArmIfMeasurable(Sve, GFrameCounter)`** (`AnomalyCaptureSubsystem.cpp:744`) — the
mask's `RequestId` **is `GFrameCounter`**. The captured frame's `RequestId` is a **plugin-owned monotonic
serial** minted at the capture arm site (`m31`'s fix). **They are different id spaces.**

✅ The join nonetheless exists and is exact: `labels.jsonl.frame_index` **is** the arm-time
`GFrameCounter`, so `mask RequestId == frame_index`. That is the same join `P9` used and proved
(704/704). **S1 should read "keyed by `GFrameCounter`, joined to the label row by `frame_index`"** — the
current wording would send an implementer to the wrong id.

## §3.6 D4 — `m28` output height silently breaks the rect equality. Latent, must be handled or refused.

The mask RT is **view-rect sized**; `Actual_Frames` are **resampled on write** when
`CaptureOutputHeightDefault != 0` (`m28`). The client ini carries no such key today, so picture == view
rect and S3 holds — **but the moment anyone sets an output height, the mask and the picture disagree in
size and the artifact is silently wrong.**

⚠ **A label mask must never be filtered.** If it is resampled at all it must be **nearest-neighbour**;
bilinear would invent stencil values that were never assigned to anything. **`m43` must either implement
nearest-neighbour resampling for the mask or REFUSE LOUDLY when `EffectiveOutputHeight != 0`.** Refusing
is the safer default and is one line.

## §3.7 D7 — "multiple live targets keep their distinct values" is UNVERIFIED, and may be structurally rare

`ArmIfMeasurable` arms **one record per tick** and `RestoreActor` untags after collection, so on a
typical armed frame **only one event's target carries a tag**. Two targets can overlap only while a
second event's tag is still applied awaiting its own collection. **S2's multi-value claim is therefore
not wrong, but it is not the common case and it has not been measured.** It should not be written into
client documentation as a feature until a leg shows two distinct non-zero values in one PNG.

---

## §3.8 What `m43` would look like once D1/D6 are ruled — the parts that are NOT in doubt

Recorded so the next brief starts from here rather than re-deriving it.

- **New file `AnomalyTargetMask.{h,cpp}`** in `AnomalyCapture`: owns the per-frame R8 buffer, the
  event-tag filter (S2), the `mask_map.json` accumulator and the PNG encode hand-off. No new shader, no
  new render pass.
- **`AnomalyMaskSceneViewExtension`**: enqueue the surface readback additionally when the target mask is
  on (one `if`, joining the existing `Mode != Gpu` condition); carry the drained R8 buffer out through
  `FAnomalyMaskResult` (it already carries `ViewRectSize` and the per-tag table).
- **`AnomalyAsyncWriter`**: a second encode job type — 8-bit grayscale PNG, `target_mask/frame_NNNNN.png`,
  numbered by **`session_index`** so it sorts with `Actual_Frames` (⚠ **not** by `frame_index`; `G161`
  and `client-readme.md` already forbid joining on `frame_index`).
- **`AnomalyLabelWriter`**: `mask_value` on the anomaly row, `mask_file` (string or `null`) on the frame
  row — **exactly two keys**, `annotation.json` untouched; `run_summary` gains exactly
  `target_mask_frames_written` and `target_mask_frames_unavailable`.
- **Knob** `bTargetMaskDefault` / `IAI.Capture.TargetMask` with the three-branch provenance echo the
  `m41` describers already establish; **delivery mode does not suppress it**; when the mask pass is off
  the echo reads `TARGET MASK OFF (mask off)` and nothing is written.
- **OFF is byte-inert** and is gated by `P-C7 v2`, the comparator `m41` established.
- **Gate (ii) stays the load-bearing one**: per armed frame, per live event, the PNG's pixel count for
  that value **==** the `m34` GPU table's `Count` for that tag. It ties the delivered mask to the exact
  silhouette the labels were judged on, and nothing weaker should be accepted in its place.

---

## §3.9 NEEDS-DECISION — four, and the first two are blocking

1. 🚨 **Coverage (D1).** ~21 masks per 90 frames under the spec as written, and gate (i) is
   unsatisfiable. Pick (a) raise `m26`'s arm budget — **⛔ touches the veto's inputs**, (b) accept 23 %
   and document, or (c) **a separate arm budget for the target mask** (recommended: full coverage,
   `m26` untouched, cost paid per frame).
2. 🚨 **Hidden frames (D6).** A hidden target produces **no file**, not a blank one, because `LOCK-1`
   refuses to arm on a hidden tick — and hidden frames are the ones a hide-type anomaly is *about*.
   Emit an explicit all-zero PNG from the known game-side hidden state, or re-word gate (iii).
3. **`m28` interaction (D4).** Implement nearest-neighbour mask resampling, or refuse loudly when
   `EffectiveOutputHeight != 0`. **Refusing is recommended.**
4. **Cost (D2/S7).** `m43` re-introduces the full-surface readback `m34` removed. Confirm that is
   accepted, and note it compounds with resolution (c) of D1.

📌 **Not blocking, for the record:** D3 (join is `frame_index`, fix the wording) and D7 (the multi-value
claim is unmeasured — keep it out of client docs until a leg shows it).

---

# §4. `m43` — TARGET ID MASK: FOUR ATTEMPTS, AND WHAT EACH STOP BOUGHT

**Shipped 2026-09-03.** One `feat(capture): m43 - target ID mask; one mask render serves all pending
arms` on `master`. Pre-declared gates: `docs/predictions/2026-09-03-m43-target-mask.md` +
**ADDENDUM 2** (the shared pass) + **ADDENDUM 3** (the race and liveness). ⛔ None of the three was
edited after a measurement existed.

---

## §4.0 The headline, and it is not the mask

`m43` delivers a per-frame instance mask of the anomaly targets. **But the change that matters most is
the one it needed in order to work at all:** the shared mask pass now serves **every** pending arm from
**one** render, and that repairs a **latent defect in shipped `m41`** where the census starved `m26`'s
arms through a single-slot FIFO — coupling **`framesContributed`, a veto input**, to census cycle
length. See §4.5.

---

## §4.1 Attempt 1 — the wrong-blank stop. **A WRONG BLANK IS WORSE THAN A MISSING FILE.**

Gate `(i)` failed: `16 + 71 = 87` of 90. Cause: only the *shared-arm* half of ruling `R1` was built, so
`m26`'s `MaxArmsPerEvent = 4` capped coverage, and **every frame `m26` declined was written as a BLANK
mask.**

🚨 **A blank asserts MEASURED AND NOTHING VISIBLE.** 71 frames were written that way and most had a
visible target, so the artifact would have stated ground truth that is **false**. ⇒ **the lesson that
travels: a wrong blank is worse than a missing file** — an absent file says "no information", a blank
file says something specific and untrue, and a segmentation consumer cannot tell the difference.

## §4.2 Attempt 2 — the single-pass FIFO starvation, found by arithmetic

Adding a per-frame own arm moved coverage only 16 → 26. The arithmetic said why:
**90 captured frames · 106 mask passes · `m26` 24 arms · census 72 arms.** The pass renders once per
frame and consumed exactly one arm, so `96` of the `106` slots were already spoken for and the target
mask got **10**. Confirmed to the unit by attempt 1 (16 shared) + 10 own = 26.

🔑 **Lesson: a single pass served FIFO starves whichever consumer arrives last, and the starvation is
invisible unless you count arms against passes.**

## §4.3 Attempt 3 — the peek/take race, and a mask that contradicted its own labels

The shared pass (ruling **(a)**) landed and proved itself — but two residuals remained.

**(1)** `unavailable = 3` at `session_index` 29, 43, 89. All three had `M23 ARM` on their own tick, so
the render happened. **The shared path was the cause:** the block ran
`EnqueueDrain()` → `ServiceTargetMask()` (peek) → `CollectResults()` (take, removes), and if the render
thread published between the peek and the take, the target mask never saw its own result. Sporadic by
construction — hence three scattered frames.

**(2) 🚨 `session_index` 23 and 47 carried mask CONTENT while their label row said `anomalies = 0`.**
`m26`'s records **outlive their fire window**, so "`m26` armed" is not "a target is live", and the
filter used `BuildBaseTagSet()` — every record tag ever — so an ended event's lingering tag was never
zeroed.

⇒ **the same failure family as attempt 1: the artifact stating something untrue.** A mask that
contradicts its own labels is worse than a missing mask.

## §4.4 Attempt 4 — liveness from the labels' own source; the own-arm always

- **The race is removed, not timed around:** the target mask **always arms its own `RequestId`**, which
  costs nothing once one render serves every arm. `unavailable` **3 → 0**.
- **Liveness and the filter come from `Auto->GetLiveFires()` — the same call `FinalizeArmedLabel`
  builds the label row from**, in the same tick, with `LiveFires` mutated only in the injector's `Tick`
  which has already run. ⇒ **mask and labels cannot disagree by construction, WITHOUT depending on the
  `OnWorldTickEnd` multicast order** — that is the assumption `P9` was made of and it is not repeated.
  Frames with `anomalies = 0` carrying content: **2 → 0**.

## §4.5 🚨 THE LATENT `m41` DEFECT — measured, controlled, fixed

| leg | census arms | `m26` arms | served | **UNSERVED** | lag min/max/mean |
|---|---|---|---|---|---|
| `m41`, census **ON** (shipped default) | 78 | 24 | 22 | **2** | 1 / **3** / 1.86 |
| `m41`, census **OFF** | 0 | 24 | 24 | 0 | 1 / 1 / 1.00 |
| `m41`, census OFF (2nd) | 0 | 24 | 24 | 0 | 1 / 1 / 1.00 |
| **`m43`**, census **ON** | 97 | 24 | **24** | **0** | **1 / 1 / 1.00** |

The census-OFF pair is the control that proves the cause. Effect on a veto input, measured:
`StaticMeshActor_49@116` **`framesContributed` 2 → 4**.

⚠ **LATENT — no verdict was ever observed to change.** Both `m41` legs had identical event sets and
`vetoed_events` 0. ⛔ **"The veto was wrong" is NOT established and must not be written.** What is
established is that a veto input was coupled to an unrelated subsystem's cadence.

## §4.6 Gate `D` — PASS-WITH-READING, and the predicate that was over-strict

Census `tagOvertaken` **0 → 1**, attributed by the same-binary control (target mask OFF → 0).
`framesPolluted` **0**, `batchesLost` **0**, cycle histogram identical, verdict set identical.

⚖ **Ruling (`P-C2` precedent): PASS-WITH-READING.** The counter is the one the census built for exactly
this class — its own text calls a re-tag by the event mask *"the expected case"*.
📌 **My predicate "`tagOvertaken` unchanged or lower" was OVER-STRICT for a counter designed to absorb
this. The corrected predicate: "`tagOvertaken` may rise by the number of live-target re-tags;
`framesPolluted`, `batchesLost` and the verdict set must not move."** The addendum is **not edited**;
the correction lives here (`P-C2` route).

⚠ **And a mechanism of mine was refuted by a later measurement.** I attributed gate `D` to *"the target
mask's tag/restore cycle opens windows in which a live target is untagged"*. The `tagFlips` counter
added afterwards reads **0 on every bench leg** — the target mask never tagged anything itself, because
`m26` already had the live target tagged. ⇒ **that mechanism does not stand**; the perturbation comes
from the extra arms changing census batch timing. **Recorded as refuted, not quietly dropped.**

## §4.7 Limitations shipped with `m43`, stated

- **Only anomaly targets appear.** Not a mask of every object in the scene.
- **Translucent-only targets never appear** — excluded at selection (`m41` item B) and unable to write
  custom depth.
- **Nanite targets are invisible to it** (`G134`), the same limit the `m26` measurement has.
- **Multi-target frames are UNVERIFIED** (`D7`): no bench leg has shown two distinct non-zero values in
  one PNG. Client docs say "one value per anomaly target present in the frame".
- **Output height ≠ 0 refuses outright.** Nearest-neighbour mask resampling is a named follow-up.
- **Per-frame tag/restore churn on a live target** queues a deferred proxy recreate. **`tagFlips = 0`
  measured on every bench leg**, so it is zero here; **its pixel effect is UNMEASURED, not shown
  harmless.** `m42` is its fix, measurement-first.

## §4.8 What each stop bought, in one line each

1. **The wrong blank** — caught before it shipped an artifact asserting a falsehood about 71 frames.
2. **The FIFO starvation** — found a latent defect in the *client's current build*, not just in `m43`.
3. **The peek/take race + the label-contradicting mask** — caught 2 frames whose mask disagreed with
   their own labels.
4. **The over-strict predicate** — corrected in the open, and a mechanism of mine refuted by its own
   follow-up measurement.

⇒ **Four stops, four findings, and two of them were in shipped code rather than in the new feature.**

---

# §5. OWNER-FOUND BUILD DEFECT — `LogAnomaly` was never exported, and the bench structurally could not see it

**2026-09-03, session 069 brief 11.** Found by the owner on the Bates **editor** build, not here.

## §5.1 The defect

`Source/AnomalyInjector/Public/AnomalyInjectorLog.h` declared:

```cpp
DECLARE_LOG_CATEGORY_EXTERN(LogAnomaly, Log, All);
```

with **no module export**. Since **`m38` (`7c06c6c`)** the `AnomalyCapture` module logs to `LogAnomaly`
(the run-log probe lines and `AnomalyRunLog.cpp`), and `m38`'s verbosity raise references the category
**object** across the module boundary. `AnomalyCapture.Build.cs:335` does list `"AnomalyInjector"` as a
dependency — the dependency was never the problem; the **symbol visibility** was.

**Measured, pre-fix, on the working tree at `8bd32a0`:**

```
Module.AnomalyCapture.cpp.obj : error LNK2001: unresolved external symbol
"struct FLogCategoryLogAnomaly LogAnomaly" (?LogAnomaly@@3UFLogCategoryLogAnomaly@@A)
D:\...\UnrealEditor-AnomalyCapture.dll : fatal error LNK1120: 1 unresolved externals
```

`StackOBotEditor Win64 Development` → **exit code 6**.

**Fix — one line, the standard UE idiom** (`CORE_API DECLARE_LOG_CATEGORY_EXTERN` in `CoreGlobals.h`):

```cpp
ANOMALYINJECTOR_API DECLARE_LOG_CATEGORY_EXTERN(LogAnomaly, Log, All);
```

`StackOBotEditor Win64 Development` → **exit code 0**.

## §5.2 🚨 WHY THE BENCH COULD NOT HAVE CAUGHT IT — and why that matters more than the fix

**Every bench gate in this project runs the PACKAGED Development build, which is MONOLITHIC.** In a
monolithic link there are no DLL boundaries, so `ANOMALYINJECTOR_API` expands to nothing and a missing
export is **invisible by construction**. The **editor** target is **modular** — one DLL per module —
and is the only configuration in which the symbol has to cross a boundary.

⇒ **This class of defect cannot be caught by any amount of packaged-build gating.** Not "was not
caught" — **could not be**.

🚨 **And it would have blocked the client cook.** The cook runs on **editor** binaries (`G47`, card
`A-3` step 3.5), so the next cook would have failed at link, after the cook window had opened.
**`m38` shipped it, and `m40`, `m41` and `m43` all shipped on top of it, none of them able to see it.**

## §5.3 THE NEW PERMANENT GATE — both targets, every feat milestone

**Every feat milestone now builds BOTH the packaged Development target AND the Editor target on the
bench, and both must exit 0.** Added to the milestone gate template in `CLAUDE.md`, to
`PRE-DELIVERY-CHECKLIST.md` §1.1 (editor build exit 0 *before* the cook), and as `G221`.

**The gate is proven BOTH WAYS on this tree** (`G96`), which is why the pre-fix build was run first and
its linker line captured: **fail at the pre-fix tree (exit 6, LNK2001/LNK1120), pass with the one-line
fix (exit 0)**. ⛔ A gate that has only ever passed would not have been worth adding.

## §5.4 The other cross-module symbols — audited, and the linker is the authority

Grepped the injector module's public surface: **79 `ANOMALYINJECTOR_API` occurrences across 10 headers**.
`UAnomalyInjectorSubsystem` and `UAnomalyAutoInjectorSubsystem` carry `class ANOMALYINJECTOR_API`;
`AnomalyViewport.h` exports 28 symbols including `IsRenderableComponent`,
`GetVisibleRenderableActors`, `GetCensusPrefilterActors` and `ProjectActorBoundsToScreenRect`;
`AnomalyDefaults.h` 35; `AnomalyArgs`, `AnomalyLod`, `AnomalyTargeting`, `AnomalyCensusProvider` and
`AnomalySelectorSubsystem` all export what the capture module uses.

🔑 **But the decisive check is not the grep — it is the linker.** The pre-fix editor link reported
**exactly ONE** unresolved external, and the post-fix link reported none. ⇒ **every other cross-module
symbol the capture module touches was already exported, proven by the tool whose job that is.**

## §5.5 `P-C7` — the packaged build is inert, shown by behaviour rather than by hash

The packaged target is monolithic, so the macro expands to nothing there and the change should be a
true no-op. ⚠ **The exe hash moved anyway** — `AD543F42` → `0EF535DC`, **identical size
(241,169,920 B)** — which is `G201` in action: an artifact hash is not content identity.

**So inertness was shown by behaviour.** Against the `m43` control leg under `P-C7 v2`:
`frame_index` Δ **0**, `t` Δ **0**, `view` **identical**, and every label field byte-identical
**except `anomalies`** — where the **only** differing sub-field was **`mask_value`** (223 vs 227 on the
first event).

**That was not accepted as an explanation; it was controlled.** A second leg on the *same* post-fix
binary was run and compared to the first:

| leg | binary | first-event tag set |
|---|---|---|
| `M43G_BASE` | `AD543F42` (pre-fix) | `223,225,226,227,228,254` |
| `M44_EXPORT` | `0EF535DC` (post-fix) | `223,**224**,226,227,228,254` |
| `M44_EXPORT_B` | `0EF535DC` (post-fix) | `223,**225**,226,227,228,254` |

⇒ **the same-binary pair differs in `mask_value` by the same 23 rows, and the post-fix second run
reproduces the PRE-FIX tag set exactly.** `mask_value` is **run-unique**: stencil tag allocation draws
from the shared ledger and depends on census claim ordering, which is timing-sensitive.
**The export change is exonerated.**

📌 **Client-facing consequence, now measured rather than assumed:** `mask_value` is **stable within a
session and NOT stable across sessions**. That is exactly why `mask_map.json` is per-session and why the
client docs say to key on `mask_value` **together with** the event/frame range, never on the value
alone. The doc line was written on principle at `m43`; it is now backed by a measurement.
