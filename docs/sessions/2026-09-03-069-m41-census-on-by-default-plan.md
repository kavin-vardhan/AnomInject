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
