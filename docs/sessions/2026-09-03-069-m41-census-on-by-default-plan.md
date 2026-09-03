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

---

# §6. THE "DEIMOS" CODENAME IS RETIRED — there were always only two office hosts

**Owner correction, 2026-09-03: "Bates and Deimos are one and the same."** `Deimos` was a **second
codename for the Bates host**. There are exactly **TWO** office hosts: **Concorde** and **Bates**.
Nothing else about either host changes, and no real title, studio or fork name is recorded anywhere.

## §6.1 What changed — living docs only

- **`CLAUDE.md`** — the codename invariant reads **`(Concorde, Bates)`**, with one added sentence
  telling a reader that historical journals and predictions saying *Deimos* mean *Bates*. Three
  incidental usages corrected: `A Bates/Deimos run` → `A Bates run`; `FOR BATES / DEIMOS = 90` →
  `FOR BATES = 90`; `a Bates/Deimos READBACK-LAYOUT` → `a Bates READBACK-LAYOUT`.
- **`docs/office-rdp-card.md`** — the `A-9` bullet *"**Deimos**, if reachable: same A-5/A-6 sequence …
  **Being 5.2+**, it is the host where the pre-`m35` code would have been wrong at a non-zero origin"*
  is **DELETED, not re-pointed.**
  ⚠ **Why deleted rather than migrated:** it described a **third host that does not exist**, and its
  engine-version claim (`5.2+`) rested on that host being a *different* machine. Re-aiming the bullet at
  Bates would silently transfer an unverified property onto a host whose lineage is recorded elsewhere.
  **Deleting the entity has to delete its attribute too** (`G120`'s shape).
- **`docs/gotchas.md`** — **`G222`** minted.
- ⛔ **Not touched:** `docs/sessions/*` and `docs/predictions/*` (append-only history — `G222` is how
  they are read), and the untracked `docs/CHAT-HANDOFF-*.md` (chat-owned).
- ✅ **No source change, no build, no leg.**

## §6.2 Post-edit survivor list — `git grep -l -i deimos`

```
CLAUDE.md                                                  <- the RETIREMENT NOTICE itself
docs/gotchas.md                                            <- G222 itself
docs/predictions/2026-08-23-m34-gpu-mask-reduce-gates.md    <- history
docs/predictions/2026-08-26-m35-build-b-gates.md            <- history
docs/sessions/2026-08-26-061-m35-readback-sub-rect-copy-handoff.md   <- history
docs/sessions/2026-08-31-063-m36-selection-census-s1.md     <- history
```

**No defect.** The first two are the notice and the gotcha that *retire* the name — they must mention it
to be readable. Every other survivor is append-only history. Checked and clean at **0 hits**:
`setup-runbook.md`, `PRE-DELIVERY-CHECKLIST.md`, `client-readme.md`, `client-delivery.md`,
`architecture.md`, `onboarding.md`, `invisible-anomaly-mechanisms.md`; `office-rdp-card.md` is now **0**.

📌 **A note on the check itself:** the first sweep flagged `CLAUDE.md` and `gotchas.md` as defects,
because the predicate was *"any living doc mentioning Deimos"* — which is satisfied by the very text
that retires it. **The predicate was too crude, not the docs.** The correct one is *"any living doc
**using** Deimos as a host name"*, and by that predicate the count is **zero**. Recorded because it is
the same shape as `m43`'s over-strict gate `D`: a check written against the wrong quantity reports a
fault that is not there, and the cost of that is a real finding being distrusted next time.

## §6.3 The lesson, which is not about this codename

**A duplicate name for one thing is worse than a wrong name for it.** A wrong name gets questioned; a
duplicate quietly doubles the apparent size of the world. Here it produced a card item planning work on
a host that did not exist, carrying an engine-version claim that could never have been checked.
⇒ **When a codename is minted, confirm it is not a second label for something already named.**

---

# §7. BATES SECTION-E FINDINGS ON THE TARGET MASK — reproduction, root cause, and the `m44`/`m45` plan

**2026-09-03, session 069 brief 13. Plan only — no source change, no feat commit.**
Pre-declared gates: `docs/predictions/2026-09-03-m44-m45-target-mask-onset-and-hidden-class.md`.
⛔ **Bates was not touched. Everything below is bench-only.**

---

## §7.1 TASK A — reproduction, both tick orders

Legs on one binary (`0EF535DC`), standard config, auto-pool, seed 4242, 90 frames, `CB_GateLevel`:
`M44_EXPORT` (native order) and `M45_SYNTH2` (`IAI.Bench.SynthTickOrder 1`).
⚠ The first SynthTickOrder attempt was **rejected by the harness** on its own pacing check and re-run;
both attempts are banked (`A63`).

### A1 — first labelled frame vs first non-blank mask

| type | target | labelled frames | 1st label | 1st mask | **delta** |
|---|---|---|---|---|---|
| **NATIVE** | | | | | |
| `blink` | `StaticMeshActor_85` | 16,17,21,22 | 16 | **NONE** | n/a |
| `corrupted_texture` | `StaticMeshActor_73` | 27…34 | 27 | 28 | **+1** |
| `blink` | `StaticMeshActor_73` | 40,41,45,46 | 40 | **NONE** | n/a |
| `missing_texture` | `StaticMeshActor_49` | 51…58 | 51 | 52 | **+1** |
| `corrupted_texture` | `StaticMeshActor_49` | 63…70 | 63 | 64 | **+1** |
| `missing_texture` | `StaticMeshActor_49` | 87,88,89 | 87 | 88 | **+1** |
| **SYNTH TICK ORDER** | | | | | |
| `blink` | `StaticMeshActor_85` | 16,17,**18**,22 | 16 | **NONE** | n/a |
| `corrupted_texture` | `StaticMeshActor_73` | 27…34 | 27 | 28 | **+1** |
| `blink` | `StaticMeshActor_73` | 40,41,**42**,46 | 40 | **NONE** | n/a |
| `missing_texture` | `StaticMeshActor_49` | 51…58 | 51 | 52 | **+1** |
| `corrupted_texture` | `StaticMeshActor_49` | 63…70 | 63 | 64 | **+1** |
| `missing_texture` | `StaticMeshActor_49` | 87,88,89 | 87 | 88 | **+1** |

✅ **O1 REPRODUCES — and in BOTH orders, identically.**
✅ **O2 REPRODUCES** — neither `blink` event has mask content on any labelled (hidden) frame.

🚨 **`SynthTickOrder` IS REFUTED AS O1'S MECHANISM.** It was the brief's first suspect. The lever
engaged — its echo is in the log and it moved the blink hidden set (`21`→`18`, `45`→`42`, the `P9`
interior-flip shape) — so the null is a **reading, not a misfire**. The mask delta is `+1` either way.

📌 **And m43's own gate set would not have caught this in EITHER order.** The gap was not the missing
tick order; **no gate ever compared first-label to first-mask.** Gate `(ii)` checked the bit-exact tie on
frames that *were* measured, and `(iii)` checked a count identity — both are satisfied by a mask that is
uniformly one frame late. **That is the predicate lesson, and it is the more important half.**

### A2 — onset: % of pixels differing by >8/255 from a clean reference (`firstLabel − 2`)

| type | target | n−1 | **n** | n+1 | n+2 | n+3 |
|---|---|---|---|---|---|---|
| `corrupted_texture` `_73` | native | 0.471 | **5.935** | 6.175 | 6.192 | 6.695 |
| `corrupted_texture` `_49` | native | 0.500 | **8.157** | 8.521 | 8.929 | 9.633 |
| `missing_texture` `_49` | native | 0.461 | **2.725** | 2.861 | 2.818 | 3.243 |
| `missing_texture` `_49` | native | 0.438 | **2.727** | 2.841 | 2.851 | — |
| `corrupted_texture` `_73` | synth | 0.587 | **6.008** | 6.188 | 6.154 | 6.652 |
| `corrupted_texture` `_49` | synth | 0.500 | **8.162** | 8.409 | 8.773 | 9.468 |

❌ **O4 IS NOT REPRODUCED ON THE BENCH.** The picture already differs at frame **n** by **5.9–8.2 %**
(CorruptedTexture) against a **~0.5 %** baseline — a factor of 12–16. The label at `n` is right and the
pixels at `n` are right.

⚠ **There IS a small blend-in, and it is reported rather than dismissed:** `n → n+1` grows by ~4 %
relative (5.935→6.175; 8.157→8.521). That is consistent with temporal AA settling and, per the brief's
own framing, is **a labelling-policy question for chat, not a bug.**
⛔ **No mechanism is asserted for the owner's Bates observation.** The bench says the effect is present
at `n`; a host whose AA blends in more slowly could make `n` look subtle **to the eye** while the pixels
have already moved. **That is a candidate, not a finding**, and the read that would settle it is the same
onset table run on Bates — which is **not** requested here.
📌 Because the picture onset is `n`, the brief's conditional read of `Anomaly_CorruptedTexture.cpp`
(material/render-resource readiness at apply time) **was not triggered and was not performed.**

---

## §7.2 ROOT CAUSE OF O1 — one sentence, with the line

> **The target mask's tag comes from an `FAnomalyMaskRecord`, and records are created only by
> `FindOrAddRecord` (`AnomalyMaskMeasure.cpp:124`), which is called only from `AccumulateFrameEvents`
> (`AnomalyCaptureSubsystem.cpp:4123`), which on the async path is called only from the DRAIN
> (`AnomalyCaptureSubsystem.cpp:2769`) when a captured frame's readback completes — one frame after the
> arm. So on a fire's first frame `ArmTargetMaskOwn` (`AnomalyCaptureSubsystem.cpp:836`) finds no record
> with `R.Tag != 0`, returns false, and the frame takes the blank path.**

⇒ `+1` **by construction, in any tick order.** The label is sampled at `OnWorldTickEnd` (`m40`) and is
correct at `n`; **the mask side is keyed off a structure that does not exist yet.**

✅ **Nothing in the render timing blocks the fix:** a stencil tag applied at `OnWorldTickEnd` of frame
*N* is live for frame *N*'s render — `World->SendAllEndOfFrameUpdates()` runs inside
`FRendererModule::BeginRenderingViewFamily` (`SceneRendering.cpp:4528`), under the engine's own comment
*"Guarantee that all render proxies are up to date before kicking off a BeginRenderViewFamily."*

---

## §7.3 `m44` — FILE-BY-FILE (ship-blocking for this week's delivery)

**The invariant to be stated in code and in `CLAUDE.md`:**
> **The target mask's arm, liveness and tag decision are taken from the SAME per-frame snapshot as the
> labels (`OnWorldTickEnd`, `m40`'s sample). No tick order, and no readback latency, may split the mask
> from the label.**

| file | change |
|---|---|
| `AnomalyMaskMeasure.{h,cpp}` | expose record creation so it can be driven from the live-fire set at `OnWorldTickEnd` — i.e. `FindOrAddRecord` becomes reachable without waiting for the drain. **No change to arming, budgets, `framesContributed` or verdicts.** |
| `AnomalyCaptureSubsystem.cpp` `OnWorldTickEndMask` (~`:737`) | before `ArmTargetMaskOwn`, ensure a record exists for **every live fire on this frame's snapshot** (same `Auto->GetLiveFires()` source `FinalizeArmedLabel` uses). This is the O1 fix. |
| `AnomalyCaptureSubsystem.cpp` `:2769` / `:4123` | unchanged — the drain still calls `AccumulateFrameEvents`; `FindOrAddRecord` is idempotent, so the drain finds the record already present |
| `AnomalyCaptureSubsystem.cpp` `ServiceTargetMask` / `EnqueueTargetMaskPng` | **do not write a file when the filtered mask is all-zero** (O3). Count it `empty`. |
| `AnomalyCaptureSubsystem.cpp` blank path | stop synthesising all-zero PNGs entirely; the deferred-blank list and its FinishRun flush are deleted |
| `AnomalyLabelWriter.{h,cpp}` | `mask_state` on the frame row; `mask_file` stays `string\|null` and is `null` unless `present`; `mask_map.json` lists only files that exist |
| `run_summary` | ⚠ **`target_mask_frames_hidden_blank` KEPT under its name**, re-documented as the count of `empty` rows — a rename is a silent schema break and the value's meaning has not changed |
| `client-readme.md` / `client-delivery.md` | the three `mask_state` values; **"a mask file exists only when it has content"**; blank-vs-null becomes present/empty/unmeasured |
| `CLAUDE.md` | the invariant above + the new both-orders gate rule |

⚠ **The one real risk, named:** creating records a frame earlier changes when tags are allocated, which
could shift `framesContributed`. `M44-G6` exists for exactly that and **a movement there is a stop.**

## §7.4 `m45` — HIDDEN-CLASS MASKS (attempt; ships only on its identity gate)

**Candidate mechanism (chat's), to be evaluated honestly:** replace `SetActorHiddenInGame` with a hide
that removes the target from the main and depth passes while **keeping `bRenderCustomDepth`**, so the
would-be region still writes custom depth and the existing mask compare against scene depth yields the
would-be-visible region.

**Every path that must also be silenced for the picture to stay identical — enumerate and verify each:**
`bRenderInMainPass` · `bRenderInDepthPass` · `CastShadow` and `bCastDynamicShadow` /
`bCastStaticShadow` / `bCastContactShadow` / `bCastVolumetricTranslucentShadow` · **velocity** (must be
confirmed to drop when the primitive leaves the main/depth passes, not assumed) ·
`bAffectDynamicIndirectLighting` (Lumen scene / mesh cards) · `bAffectDistanceFieldLighting` ·
`bVisibleInRayTracing` · translucency · decals · **HISM/ISM** and **skeletal** components (the flags
live on `UPrimitiveComponent`, so they apply, but each class needs its own check).

🚨 **A HARD LIMIT, KNOWN BEFORE ANY CODE: a NANITE target cannot produce a custom-depth mask at all on
5.1** — `Nanite::FSceneProxy::GetViewRelevance` never sets `bRenderCustomDepth` (`G134`). **`m45`
therefore cannot give hidden-class masks for Nanite targets**, and the office title's Nanite posture
decides how much of the fleet that removes. **This must be stated in the client docs, not discovered.**
⚠ **Also to be verified before implementing, not assumed:** that a primitive with `bRenderInMainPass =
false` is still gathered into the custom-depth pass at all.

**Census interaction, and how it is guaranteed:** selection already refuses an actor with a live fire
(`IsActorLive` in the auto-injector), so a hidden-class target cannot be re-selected. The open risk is
the **measurement**: a target that now writes custom depth while hidden could be read
`MEASURED_NONZERO` by the census and by `m26`, changing a veto input. `M45-G3` gates it, and the
intended guarantee is an explicit exclusion of live-fire targets from census classification rather than
reliance on the tag-collision guard.

## §7.5 Card — Section F, minimal, for after `m44`/`m45`

One leg, then two reads: **(F-1)** for one non-hidden event, confirm the first labelled frame **has** a
mask file and it is not blank (O1 closed); **(F-2)** for one `blinking` and one `missing_object` event,
confirm the hidden frames carry a mask showing where the object should be (O2 closed) — or, if `m45`
did not ship, confirm those frames have **no file and `mask_state: "empty"`**, which is the documented
limitation rather than a defect.

---

# §8. `m44` — BUILT AND GATED; **`M44-G1` FAILED AND THE MILESTONE IS STOPPED**

**2026-09-03, session 069 briefs 14 and 14R.** ⛔ **`m44` DID NOT SHIP. `master` is unchanged at
`42061dc`.** The source is preserved and pushed on **`m44-GATE-G1-FAILED-do-not-merge` (`dc1282b`)**.
Pre-declared gates: `docs/predictions/2026-09-03-m44-m45-target-mask-onset-and-hidden-class.md`.

## §8.0 A HOST CRASH INTERRUPTED THE IMPLEMENTATION — recorded, not smoothed

The owner's machine crashed at ~11:38 mid-edit; the watcher restarted at 11:44. The tree was found
with **partial, uncommitted edits in exactly four files** and **no build had run**. Reconciled against
the plan before continuing: all three completed edits (`AnomalyLabelWriter.{h,cpp}`,
`AnomalyCaptureSubsystem.h`) and the last in-flight edit were **complete and consistent** — the final
`Edit` had applied cleanly rather than being truncated. **Nothing was discarded and the plan was not
restarted.** No `.git/index.lock`, no orphaned UBT/editor/bench processes, no partial `Intermediate`.

## §8.1 WHAT WAS BUILT

| change | file |
|---|---|
| records created from the live-fire set at `OnWorldTickEnd`, not only in the async drain | `EnsureMaskRecordsForCapturedFrame`, `AnomalyCaptureSubsystem.cpp` |
| **placed AFTER `ArmIfMeasurable`** so `m26`'s arm decision is byte-unchanged **by construction, not by gate** | same |
| `mask_state` = `present` \| `empty` \| `unmeasured` on the frame row; `mask_file` null unless `present` | `AnomalyLabelWriter.{h,cpp}` |
| blank PNGs never written; the deferred-blank list and its FinishRun flush deleted | `AnomalyCaptureSubsystem.cpp` |
| the mask liveness filter is the **label classification** (`IsFireLabelledThisFrame`), not "the event is live" | same |
| completed frames are **held** until their mask outcome is known (bounded, 4 ticks, then `unmeasured`) | `ProcessCompletedFrames` |
| a `NOT ARMED` diagnostic printing `scanned fires/labelled/withRecord/visible` | `ArmTargetMaskOwn` |

## §8.2 GATE RESULTS — both tick orders, identical in both

| gate | native | synth | verdict |
|---|---|---|---|
| **G2** zero blank PNGs | 0 blank | 0 blank | ✅ **PASS** (was 61 of 90 blank) |
| **G3** `present == PNGs`, `present+empty+unmeasured == rows` | 23/4/63 = 90 | 23/4/63 = 90 | ✅ **PASS** |
| **G3b** `mask_file` non-null iff `present` | 0 bad | 0 bad | ✅ **PASS** |
| **G3c** `run_summary` reconciles with the row states | 23/4/63 | 23/4/63 | ✅ **PASS** |
| **G7** mask frames ⊆ labelled frames | 0 stray | 0 stray | ✅ **PASS** |
| **G6** veto inputs unmoved vs the `m43` control | identical | — | ✅ **PASS** |
| **G1** first mask frame == first labelled frame | **0/4** | **0/4** | ❌ **FAIL, `delta = +1`** |

🚨 **G7 IS THE ONE THE OWNER SAW ON THE HOST, AND IT IS FIXED:** `blinking` used to write masks on the
VISIBLE in-between frames, which the labels mark clean. It now writes none — 0 stray files in both
orders.

✅ **`M44-G6` — the STOP gate — PASSES.** Against the `m43` control (`0EF535DC`, banked
`M44_M44_EXPORT`): `vetoed_events` · `translucent_vetoes` · `translucency_unknown_vetoes` ·
`mask_probe_arms` · `mask_residual_discards` · `mask_nopass_discards` **all identical (0)**; the event
set, every `manifested`, every `frame_indices` count and `positive_frames` (43) **identical**.
⚠ `census_frames` 96→97, `census_cycles` 31→32, `census_zero` 12→13, `census_below_floor` 50→49 —
**one extra census frame, no verdict and no veto moved.** Under the `P-C2` / gate-`D` precedent that is
a **reading**, not a regression.

## §8.3 WHY `G1` DID NOT PASS — measured, and the plan's premise was incomplete

`069-13`'s root cause was **correct but not sufficient**. Creating the record earlier is necessary; it
is not enough.

**Measured on the first labelled frame, after the fix:** the mask **is armed and IS measured**, and the
reduce table reads **`tableCount = 0`** for that event's tag, while the next frame reads **48,587 /
66,832 / 66,837 / 66,862**. `MASK-TIE` says `MATCH` on all of them — the PNG and the table agree. So
the instrument is right and the *stencil* is empty:

```
session_index=27 tag=222 tableCount=0      pngCount=0      MATCH
session_index=28 tag=222 tableCount=48587  pngCount=48587  MATCH
session_index=63 tag=252 tableCount=0      pngCount=0      MATCH
session_index=64 tag=252 tableCount=66837  pngCount=66837  MATCH
```

⇒ **A STENCIL TAG THAT IS NEWLY APPLIED IS NOT PRESENT IN THAT SAME FRAME'S CUSTOM-DEPTH PASS.**

🚨 **AND THE OBVIOUS FIX WAS TRIED AND REFUTED BY MEASUREMENT.** Tagging earlier — during `Tick`, at
the arm site in `CaptureCurrentFrame`, whose render-state marks are flushed by the end-of-`Tick`
`SendAllEndOfFrameUpdates` — made it **WORSE: 0 of 4 instead of 1 of 4**. ⇒ **the lag is NOT about
where inside frame *n* the tag is set**, and the one event that passed in the intermediate attempt
(`missing_texture` @51, `delta 0`) passed because its target was **already tagged from earlier**, not
because of anything the fix did. **That experiment is reverted; only its measurement is kept.**
⛔ **NO MECHANISM IS ASSERTED** beyond the observation. `SendAllEndOfFrameUpdates`
(`SceneRendering.cpp:4528`) guarantees the proxy *recreate* is flushed before
`BeginRenderViewFamily`; whether a **newly created** proxy joins that same frame's visibility — and so
`bHasCustomDepthPrimitives` — was **not** established and must not be guessed (`G120`).

📌 **THE CONSEQUENCE FOR THE FIX TURN, STATED PLAINLY:** the remaining fix is a **design** question —
the target must be custom-depth-enabled **before** the frame it is first labelled on, which means
tagging at selection/pre-fire time or persisting tags (`m42`'s territory). That changes when stencil
tags exist relative to the census and `m26`, which is squarely `M44-G6` ground. **It is not something
to improvise inside a gate-failure turn.**

⚠ **AND SHIPPING AS-IS WOULD BE WRONG, NOT MERELY INCOMPLETE:** the first labelled frame would carry
`mask_state: "empty"` — *measured, and the target drew nothing* — while the target is plainly visible
in the picture (onset at `n` is 5.9–8.2 % of frame). **That is a false measurement claim**, and it is
worse than `m43`'s blank PNG, which at least claimed nothing.

## §8.4 TWO WRONG PREDICATES, BOTH FOUND BY THE DIAGNOSTIC RATHER THAN BY A GATE

The first build produced **zero masks and 90 `unmeasured` rows — and every gate said PASS**, because
G1 was `0/0`, G2 had no files to be blank and G7 had no files to be stray. **The emptiest possible
result produced the cleanest tick** — `G146` exactly, four milestones later.

The `NOT ARMED` diagnostic (`scanned fires=1 labelled=0 withRecord=0 visible=0`, ×35) located it:
**(i)** `ComputeFireActive` is the *hidden-state* sampler, and for `FireWindow`-sourced anomalies it
reads 0 — those events' `frame_indices` never depended on it. **(ii)** The replacement then treated
`FireWindow` as hidden-based too. `ResolveAnomalyActiveSource` has **three** sources
(`ActorHidden` · `AnomalyState` · `FireWindow`), and only mirroring all three gave the label's own
answer. ⇒ *"use the same liveness the label row is built from"* was the right instruction; **there was
more than one such source and the first two readings of it were both wrong.**

## §8.5 STATE

📦 Staged bench exe **`15A31043`**, archived as
`_binary_baselines\StackOBot.exe.m44-GATE-G1-FAILED-unvalidated-15A31043`.
⛔ **`0EF535DC` IS NOW LOAD-BEARING** — it is `m44`'s A-side and the binary both `069-13` reproduction
legs ran on; archived as `StackOBot.exe.m43-logexport-0EF535DC`. Container quartet **unchanged, no
cook**. Both targets build at exit 0 (game and editor).
⛔ **NOT DONE, named:** `client-readme`, `client-delivery`, the ledger's `m44` entry and card Section F
are **deliberately unwritten** — they would describe shipped behaviour, and `m44` did not ship.
`G223`/`G224` are minted below because they are lessons, not release notes.

---

# §9. THE "NO FRAME HANDSHAKE" HYPOTHESIS — TESTED AND **REFUTED**

**2026-09-03, session 069 brief 15.** ⛔ **No fix was written. Task B was NOT entered**, because the
brief's own stop rule fired: *"If neither prediction holds, the hypothesis is refuted — say so, keep
the numbers, stop, and report; do not improvise a different fix."* Both predictions failed.

## §9.1 THE SOURCE READ IS CONFIRMED — the mask really has no frame key

Chat's reading of the tree is **correct as a fact about the code**:

- `FAnomalyMaskSceneViewExtension::ArmMask` (`AnomalyMaskSceneViewExtension.cpp:52`) pushes onto a bare
  `PendingArms` array under `StateCS` from the **game** thread. There is no frame number.
- `AfterTonemap_RenderThread` (`:101`, taking arms at `:119-126`) takes **all** pending arms whenever it
  next runs on the **render** thread. Nothing binds an arm to a family.
- `FAnomalyMaskSceneViewExtension::BeginRenderViewFamily` is an **empty override**
  (`AnomalyMaskSceneViewExtension.h:41`) — the exact hook `m31` uses for the capturer.
- The capturer's handshake is real but is **not** `ConsumeWantedForPublish`'s parameter: that function
  is a **FIFO** and only *logs* `FamilyFrameNumber` (`AnomalySveCapturer.cpp:45-73`). The actual keying
  is the **key ring** — `PublishKey(InViewFamily.FrameNumber, RequestId, bWanted)` on the game thread at
  `BeginRenderViewFamily` (`AnomalySceneViewExtension.cpp:66`), recovered on the render thread by
  `LookupKey(View.Family->FrameNumber)` (`:93-96`). **The binding is made game-side at family setup and
  recovered by the family's own number.** That is what the mask lacks.

⇒ **The structural gap is real.** What the measurement refutes is that this gap is the cause of the
`+1`.

## §9.2 THE INSTRUMENT — `IAI.Bench.MaskPairingProbe`

Console-only, default OFF, loudly echoed, never in a client payload. It spawns a **movable magenta
cube** 600 units in front of the settled bench camera, **tags it ONCE at spawn**
(`SetRenderCustomDepth` + `SetCustomDepthStencilValue(250)`), and alternates its position every
captured tick between `Y = -250` and `Y = +250` via `SetActorLocation` — **a transform update, never a
render-state recreate**, so the recreate question cannot confound the reading.

⚠ **The first build of the probe was WRONG and its own telemetry said so:** the probe was pushed into
`Visible`/`Tags` and so entered the tag/restore machinery — `tagFlips = 80` on a 40-frame leg, two per
frame, i.e. it was being re-tagged every frame. That is exactly the confound the probe exists to avoid,
and it produced 39 empty masks. Corrected so the probe only contributes its **tag** to the filter and
**forces the arm**; `tagFlips = 2` after.

📌 **The analyser is not blind, shown from its own output:** on frames it calls `CURRENT` the distance
to the current picture centroid is **~15–30 px** while the distance to the previous position is
**~596 px** — a 20× separation. A `PREVIOUS` would have been identified with enormous margin. Its
silence is a reading.

## §9.3 THE RESULT — four legs, both tick orders, both thread-lag settings

| leg | tick order | `r.OneFrameThreadLag` | decidable | CURRENT | **PREVIOUS** | NEITHER | no-mask |
|---|---|---|---|---|---|---|---|
| `A1_NAT2` | native | default (1) | 28 | 18 | **0** | 10 | 12 |
| `A2_LAG0` | native | **0** | 28 | 17 | **0** | 11 | 12 |
| `A1_SYNTH` | synth | default (1) | 30 | 20 | **0** | 10 | 10 |
| `A2_SYNTH_LAG0` | synth | **0** | 29 | 19 | **0** | 10 | 11 |

🚨 **PREDICTION 1 FAILED: `PREVIOUS = 0` on every decidable frame of every leg.** The mask never shows
the previous tick's position.
🚨 **PREDICTION 2 FAILED: `r.OneFrameThreadLag 0` changes nothing** — the same session indices, the same
verdicts, the same pixel counts to the digit (16005 / 16130 / 16016 / 16123 on the `NEITHER` rows of
both native legs).

✅ **BOTH LEVERS ARE PROVEN TO HAVE ENGAGED, so these are readings and not blindness** (`G114`): the
engine echoed `r.OneFrameThreadLag = "0"` at frame 1 of the A2 leg, and the probe echoed
`MASK-PAIRING PROBE SPAWNED tag=250` plus 40 `PROBE STEP` lines per leg.

⇒ **VERDICT: THE ONE-FRAME-THREAD-LAG / UNKEYED-ARM HYPOTHESIS IS REFUTED AS THE CAUSE OF THE `+1`.**

## §9.4 WHAT THE PROBE DID FIND — sharper than the `+1`, and NOT a lag

The failure is **not** a uniform one-frame shift. Per leg, of 40 captured frames:

- **~18–20 frames: the mask is CORRECT** — centroid within ~15–30 px of the picture.
- **~10 frames: the mask contains an EXTRA silhouette the picture does not contain.** By x-band count
  (80-px bands, every 2nd pixel sampled), `session_index 1`:
  `[(320,384) (400,2476) (480,2602) (560,315)] and [(800,3130) (880,3640) (960,3458)]`
  — the correct cluster at position B **plus** a second cluster centred ~470, which is **neither**
  commanded position. The picture on that frame is a clean single silhouette (`picN 8008`,
  `picCx 951.0`).
- **~10–12 frames: no mask at all** for that `session_index`.

⛔ **NO MECHANISM IS ASSERTED FOR THIS** (`G120`). It is not the thread lag (A2), it is not the tick
order (both orders identical), and it is not a proxy recreate (the probe is tagged once at spawn and
`tagFlips = 2`). Naming a cause here would be the third guess in a row on this defect, and the previous
two were both refuted by the next measurement.

📌 **CONSEQUENCE THAT DOES SURVIVE, AND IT IS THE USEFUL PART: the target mask is not merely one frame
late — on roughly a quarter of frames it is WRONG IN CONTENT, and on another quarter it is ABSENT.**
That is a stronger reason not to ship `m43`/`m44` masks than the `+1` ever was, and it is measured.
⚠ It also means the `+1` seen at event onset in §8 may be a *symptom* of this, not a separate fact —
**stated as a possibility, not a claim.**

## §9.5 STATE

📦 Staged bench exe **`346ED33F`** (probe lever, the CORRECTED build - all four legs ran on it). `4EB2EA5C` was the first, wrong probe build and produced only the discarded `A1_A1_NATIVE` leg. Both targets exit 0.
⛔ **`master` untouched at `62bd287`. No fix. `G225` was NOT minted** — chat's draft wording asserts the
refuted mechanism. Four legs banked: `A1_A1_NAT2`, `A1_A2_LAG0`, `A1_A1_SYNTH`, `A1_A2_SYNTH_LAG0`
(plus the discarded first probe build `A1_A1_NATIVE`, kept per `A63`).

---

# §10. HYPOTHESIS #3 (INTERNAL-vs-OUTPUT RESOLUTION) — **DEAD AS THE EXPLANATION, CONFIRMED AS A REAL DEFECT**, and §9's finding is **RETRACTED**

**2026-09-03, session 069 brief 16.** ⛔ **No fix was written** — `F1` was gated on `P1`–`P3` all holding
and two of them failed. `master` untouched at `62bd287`.

## §10.1 THE THREE PREDICTIONS

| | prediction | result |
|---|---|---|
| **P1** | on NEITHER/no-mask frames the internal view rect differs from the output rect | ❌ **FAILED** |
| **P2** | forcing `r.ScreenPercentage 100` + `r.DynamicRes.OperationMode 0` gives 0 NEITHER, 0 no-mask | ❌ **FAILED** |
| **P3** | forcing `r.ScreenPercentage 50` makes every decidable frame NEITHER | ✅ **HELD** |

**P1, measured.** `View.ViewRect` (the `FViewInfo` internal rect) was added to the `M23 PASS` line. On
the default bench configuration it reads, on **all 51 passes of the leg, without exception**:

```
viewRect=1280x720  internalViewRect=(0,0)-(1280,720) 1280x720  unscaledViewRect=1280x720
```

**Internal == output == unscaled on every pass, including every pass that served a NEITHER frame.**
There is no mismatch to explain anything. The bench runs at 100 % screen percentage.

**P2, measured.** Forcing 100 % explicitly changed nothing (it was already 100 %): NEITHER stayed at 8.

**P3, measured — and this is the prove-it-can-fail leg (`G96`), which fired exactly as written.** At
`r.ScreenPercentage 50` the same line reads `internalViewRect=(0,0)-(640,360) 640x360` against
`viewRect=1280x720`, and the probe returns **CURRENT 0 / NEITHER 25 of 26 decidable frames**.

🚨 **SO THE MECHANISM IS REAL AND IS NOW DEMONSTRATED — IT IS SIMPLY NOT ACTIVE AT THE BENCH'S
DEFAULT.** `AnomalyVisibleMask.usf:23` samples the scene textures at `SvPosition + ViewRectMin` with
`SvPosition` in **output** space while the scene textures are at **internal** resolution. Whenever a
host runs dynamic resolution, a screen percentage ≠ 100, or any temporal upsampler, **every mask is
wrong** — and that is measured, not argued. ⛔ **It is NOT the cause of anything observed on this
bench, and must not be written up as if it were.**

## §10.2 🚨 RETRACTION — §9.4's "EXTRA SILHOUETTE / ABSENT MASK" WAS MY INSTRUMENT, TWICE OVER

Chasing `P1` turned up the actual cause of the §9 readings, and it is **not in the product**.

**Artifact 1 — the probe's tag collided with the census.** The probe hardcoded stencil value **250**,
which is inside the allocator's range (`ReservedStencilBase 200` … `AssignableStencilMax 254`). The
census tagged **78 candidates over 16 cycles** in that leg, so it both (a) handed 250 to some other
actor, and (b) re-tagged the probe itself, which is an ordinary visible static mesh and therefore an
ordinary census candidate. **Measured: with `IAI.Capture.Census 0` the no-data frames went 10 → 0.**

**Artifact 2 — the probe's colour collided with an anomaly.** The probe used
`M_CorruptedTexture_Pink`, **the same material `corrupted_texture` swaps its target to**. On exactly
the frames where that anomaly was live, the picture-side magenta detector merged two objects — `picN`
~8,000 → **16,341–17,321** on `session_index 15–22`, dragging the picture centroid to 546–857. The
**mask** on those frames was a single clean cluster at the correct position (e.g. `si=1` bands
`[(720,216) (800,3457) (880,3640) (960,2730)]`, centroid 908 against a commanded ~907).

**With both artifacts removed — census off, non-magenta anomaly — the result is unambiguous:**

| leg | order | decidable | CURRENT | PREVIOUS | NEITHER | no-data |
|---|---|---|---|---|---|---|
| `P5_CLEAN_NAT` | native | 40 | **40** | 0 | **0** | **0** |
| `P5_CLEAN_SYNTH` | synth | 40 | **40** | 0 | **0** | **0** |

⇒ **THE TARGET MASK IS CORRECTLY PAIRED WITH THE PICTURE, FRAME FOR FRAME, AT THE BENCH'S DEFAULT
SETTINGS, IN BOTH TICK ORDERS.** §9.4's *"wrong in content on a quarter of frames and absent on
another quarter"* is **WITHDRAWN**. ⛔ **Do not carry it forward.**

✅ **§9's LOAD-BEARING CONCLUSION IS UNAFFECTED AND STANDS:** the frame-handshake hypothesis is still
refuted — `PREVIOUS = 0` everywhere (a tag collision cannot manufacture a *previous-position*
silhouette) and `r.OneFrameThreadLag 0` is still a no-op.

## §10.3 THE INSTRUMENT IS NOW COLLISION-PROOF BY CONSTRUCTION

The probe tag is now **`AnomalyStencilTag::ReservedStencilMax` (255)**, which `AllocateTag` can never
hand out because it allocates only up to `AssignableStencilMax` (254). Re-verified **with the census
ON**, which is the configuration that produced the false reading:

| leg | order | decidable | CURRENT | PREVIOUS | NEITHER | no-data |
|---|---|---|---|---|---|---|
| `P6_CENSUS_ON_NAT` | native | 33 | **33** | 0 | **0** | 7 |
| `P6_CENSUS_ON_SYN` | synth | 31 | **31** | 0 | **0** | 9 |

⚠ **The residual no-data is real and is not a defect:** the probe is an ordinary visible static mesh,
so the census legitimately tags it as a candidate on some cycles and its own tag is overwritten for
those frames. **The gate's predicate is therefore `NEITHER == 0 AND PREVIOUS == 0` over decidable
frames, with the no-data count reported** — not "40 of 40", which would only be obtainable by
excluding the probe from census candidacy, i.e. by making the fixture special.

## §10.4 WHAT THIS NARROWS

The pipeline is correctly paired, so the §8 `+1` is **not** a pairing fault. It is specifically about a
**newly applied tag not being present in that frame's custom depth** — the probe, tagged once at spawn,
never exercises that path and is correct on every frame. The two facts are consistent and the `+1`
remains **open and unexplained**. ⛔ No mechanism asserted (`G120`).

## §10.5 NEXT DISCRIMINATORS (listed, not run — no cause is being named)

1. **Tag-application latency, measured directly:** extend the probe with a second actor that is tagged
   at `OnWorldTickEnd` of frame *n* (rather than at spawn) and read its `tableCount` at *n* and *n+1*.
   That isolates §8's `+1` from everything else, with the same trusted picture reference.
2. **The per-served-arm join** (brief 15 D2, folded here as cheap): the `M23 PASS` line now carries the
   internal rect and `servedArms`; joining `servedArms > 1` against frame indices would show whether
   arm batching ever changes what a consumer sees. On these legs it did not.
3. **The `P3` defect on its own terms:** it needs no further discrimination — it is demonstrated. It
   needs a **decision**, not a measurement.

---

# §11. THE `+1` IS FIXED — TAG OWNERSHIP. And `F1` is built but **CANNOT BE VALIDATED WITHOUT A COOK**.

**2026-09-03, session 069 brief 17.** `master` untouched at `62bd287`. Work on
`m44-GATE-G1-FAILED-do-not-merge`.

## §11.1 HYPOTHESIS #4 — THE MECHANISM IS CONFIRMED, ITS STATED SOURCE WAS TOO NARROW

> **One sentence:** an actor under a live fire could already be carrying somebody else's stencil value,
> `ArmTargetMaskOwn` accepted "already tagged" as good enough and never retagged it, so the reduce —
> which filters on the event's own tag — found nothing on that frame; **but the foreign value comes
> from a previous EVENT on the same actor as often as from the census**, which is why turning the
> census off cured only half of it.

**A1 — census OFF does NOT cure it.** Native order, 90-frame auto-pool legs:

| leg | delta 0 | failures |
|---|---|---|
| census **ON** | 1/4 | `corrupted@27 +1`, `corrupted@63 +1`, `missing@87 +1` |
| census **OFF** | 2/4 | `corrupted@51 +1`, `missing@75 +1` |

⇒ The brief's stop rule (*"if census OFF does not cure it, hypothesis #4 is dead"*) fired **on the
hypothesis as written**. The instrument then showed why the reading was half-right.

**A2 — the instrument, and it is unambiguous.** `TAG-OWNERSHIP` logs each armed target's actual
component state before tagging. On **exactly the four events that read `+1`**, and on no others
(4 of 27 armed frames):

| session_index | actor | eventTag | value actually on the actor | owner |
|---|---|---|---|---|
| 27 | `StaticMeshActor_73` | 222 | **204** | census |
| 51 | `StaticMeshActor_49` | 224 | **242** | census |
| 63 | `StaticMeshActor_49` | 226 | **224** | **the previous event on the same actor** (si 51's tag) |
| 87 | `StaticMeshActor_49` | 229 | **226** | **the previous event on the same actor** (si 63's tag) |

**Two sources, not one.** That is the whole of the wobble, measured: census-off removes rows 1–2 and
leaves rows 3–4.

**A3 — the asymmetry, read from source.** `m26`'s `ArmIfMeasurable` calls
`AnomalyStencilTag::TagActor(Actor, R.Tag)` **unconditionally** (`AnomalyMaskMeasure.cpp:231`) — it has
**no** accept-any-tag hole and always asserts its own value. `ArmTargetMaskOwn` had
`if (IsAnyComponentTagged(Actor)) { ++TaggedCount; continue; }`. **The two consumers of one pass
disagreed about who owns a tag**, and only one of them was right. The census skips already-tagged
actors (`AnomalyCensus.cpp:726`), so its protection is *"already tagged"*, not *"under a live fire"* —
on a fire's first frame the target is not yet tagged and the census can take it.

## §11.2 THE FIX — the ownership rule

**An actor under a live fire belongs to its event for the event's duration.** `ArmTargetMaskOwn` now
retags unconditionally, exactly as `ArmIfMeasurable` already did; the foreign-value case is counted
(`TargetMaskEventRetags`) and logged with the value it displaced. Self-tag bookkeeping is only
recorded when the actor was previously untagged, so the restore ledger stays correct.

⛔ **The pool was NOT partitioned (`F-A1`).** It is unnecessary once ownership is asserted, and it
would have cost real capacity: the assignable range is **55 values** (`200..254`) against a census that
tagged **77 candidates** in a 90-frame leg. Splitting it would have made tag exhaustion more likely,
not less, to fix a problem that a one-line ownership rule removes. **Stated as a deliberate deviation
from the brief.**

## §11.3 GATE TABLE — both tick orders

| gate | native | synth |
|---|---|---|
| **M44-G1** onset delta 0 | ✅ **4/4** | ✅ **4/4** |
| **M44-G2** no blank PNGs | ✅ 0 | ✅ 0 |
| **M44-G3 / 3b / 3c** count identity | ✅ 27 + 0 + 63 = 90 | ✅ 27 + 0 + 63 = 90 |
| **M44-G7** masks ⊆ labelled frames | ✅ 0 stray | ✅ 0 stray |
| **M44-G6** veto inputs vs the `m43` control | ✅ all six identical (0) | — |
| **MASK-TIE** | ✅ 27 lines, **0 MISMATCH** | ✅ 27 lines, **0 MISMATCH** |
| **m26 known-answer control** | ✅ `mask_probe_arms = 1` — the detector fires, so its zeros elsewhere are readings | |
| **census health** | ✅ `framesPolluted 0`, `batchesLost 0`, `tagFailed 0` | ✅ same |
| **P-C7 v2** | ✅ `frame_index` delta **one constant (0)**; the ONLY field differing outside `t_wall` is `mask_value`, itself a declared mask key | |
| **both build targets** | ✅ game 0, editor 0 | |

📌 **`present = 27` is exactly the number of labelled frames of the four non-hidden events (8+8+8+3).**
The `m43` control wrote **29**, i.e. two frames of content that were *not* labelled for their event —
the old `G7` violation, now gone.

**MOVED COUNTERS, each with its reason:**

| counter | m43 control → m44 | explanation |
|---|---|---|
| `target_mask_frames_hidden_blank` | 61 → **0** | intended: blank PNGs are no longer written |
| `target_mask_frames_unavailable` | 0 → **63** | intended: unarmed/hidden frames are now `unmeasured`, not blank |
| `target_mask_frames_measured` | 29 → **27** | intended: the two unlabelled-frame masks are gone |
| `mask_value` (labels) | differs on 30 rows | intended: it *is* the value being corrected |
| `census_frames` / `census_cycles` | 96→100 / 31→32 | the census's "already tagged" skip now sees a different set because live-fire targets are retagged; cycle boundaries shift. Run-to-run scale, `P-C2` precedent |
| `census tagOvertaken` | 0–1 → **2–3** | ⚠ **the ownership rule made visible**: the target mask now takes back an actor the census had tagged. It lands in the counter the census built for exactly this class; `framesPolluted 0`, `batchesLost 0`, verdicts and the event set unchanged |

⛔ **No unexplained movement.** The event set, every `manifested`, `positive_frames` (43),
`non_manifested_events` (0) and all six veto counters are **identical to the control**.

## §11.4 🚨 `F1` (RESOLUTION MAPPING) IS BUILT AND IS **BLOCKED ON A COOK**

`AnomalyVisibleMask.usf` now maps output → internal explicitly
(`P_in = InternalRectMin + clamp(round(P_out × InternalSize / OutputSize), 0, InternalSize-1)`), with
three new shader parameters. It compiles. **It cannot run on this bench:**

```
Shader FAnomalyVisibleMaskPS's parameter structure has changed without recompilation of the shader
```

— a **fatal at engine init**. Global shaders live in the **cooked container**, which a code-only
hot-swap does not touch (`G129`). ⛔ **A cook retires the container quartet every `m41`/`m43`/`m44`
measurement was taken on, and cooks in this project are owner-sequenced (`G118`: never inside a
measurement sequence). I did not run one.**

**Consequence, stated plainly: `F1` is committed UNVALIDATED and `B2`'s 50 % gate was NOT run.** The
staged bench exe is deliberately left at the **Task-A** build so the bench stays usable.
⚠ The mapping is nearest-by-construction, so **if it validates** it also retires the
nearest-neighbour mask-resampling follow-up — **stated as a consequence of a fix that has not yet been
proven to run.**

## §11.5 STATE

📦 Staged bench exe **`635A615A`** (Task A only; bootable). `57B132A4` was the F1 build and **does not
boot** — not archived as a baseline for that reason. Container **unchanged, no cook**.
⛔ Client docs, the ledger's `m44` entry and card Section F still wait for the merge ruling.

---

# §12. `m44` MERGED — what landed, what deliberately did not

**2026-09-03, session 069 brief 18.** Owner ruling `D2 = (a)`: Task A is `m44` and merges now; `F1`
does not.

- **Merge shape: a MERGE COMMIT, not a fast-forward** — `master` (`62bd287`) and the branch had
  diverged at `42061dc`, because master took the "G1 FAILED" docs commit while the branch carried on.
  Three doc files conflicted (`gotchas.md`, the predictions file, this journal); all three were pure
  appends on both sides and were resolved as **ours + the branch's added tail**, giving
  `G222 → G223 → G224 → G225 → G226 → G227` and `§7 → §8 → §9 → §10 → §11` in order, with
  `APPENDIX` → `APPENDIX 2` → `3` → `4`.
- ✅ **`F1` IS NOT IN MASTER, verified two ways:** `git diff 62bd287 master --
  Shaders/Private/AnomalyVisibleMask.usf Source/AnomalyShaders/Public/AnomalyVisibleMaskShader.h` is
  **empty**, and `git merge-base --is-ancestor 2d66b90 master` exits **1**. It lives on
  **`m44-f1-resolution-mapping-UNVALIDATED`** — the branch name carries the status.
- ⛔ **`m44-GATE-G1-FAILED-do-not-merge` is deleted** (local and origin) now that its content is either
  merged or moved. Its name was accurate to the end and it was never merged under it.
- ⛔ **NO TAG.** Office batch: `… m41 → m43 → m44`.
- 📦 Staged bench exe stays **`635A615A`** — the binary every `m44` gate ran on. ⚠ **`master`'s own
  post-merge build is `06657B35` and is NOT staged**: identical `Source/`, different exe hash across
  two links (`G201`). The gated binary is the one that stays on the bench.

## §12.1 Two log lines were CORRECTED, not just the docs

The `IAI.Capture.TargetMask` help string and the `StartRun` echo both still said *"A BLANK png means
MEASURED AND NOTHING VISIBLE"*. **After `m44` no blank PNG is ever written, so both lines asserted
something false.** They now state the `present`/`empty`/`unmeasured` contract and that a file exists
iff it has content. 📌 **The run's own echo is the client-facing contract (`G139`); leaving it stale
while fixing only the README would have been the worse half of the fix.**

## §12.2 Docs landed with the merge

`CLAUDE.md` (status block refreshed per the standing convention; the tag-ownership, record-birth and
internal-view-rect invariants; the both-tick-orders standing rule; the two permanent gates ONSET and
MASK-PICTURE-PAIRING, added to the milestone template) · `client-readme.md` (the `mask_state` table,
no-file-unless-content, the counter-name note, the temporal-AA sentence, and the hidden-class
limitation in the words the brief specified) · `client-delivery.md` · `PRE-DELIVERY-CHECKLIST.md`
(§1.1 mask boxes rewritten — **"one PNG per captured frame" was the old completeness test and is now
WRONG**; plus `MaskPairingProbe` added to the bench-lever grep) · card **Section F** · ledger **§11** ·
`_binary_baselines\README.md`.
---

# §13. `m45` — THE MECHANISM EXISTS AND WORKS; **`M45-G4` IS NOT OBTAINED, SO IT DOES NOT MERGE**

**2026-09-03, session 069 brief 19.** ⛔ **`master` untouched at `d48e1ba`.** Work on
`m45-hidden-class-masks-GATE-FAILED`.

## §13.1 TASK A — the source read, and it says YES

**A1 — does a `bRenderInMainPass = false` primitive still reach the custom-depth pass? YES,
end-to-end:**

| step | file:line | what it establishes |
|---|---|---|
| relevance | `SceneVisibility.cpp:2470-2473` | `bRenderCustomDepth` ⇒ `bHasCustomDepthPrimitives = true` **and** the stencil value is registered. **No main-pass test.** |
| static gather | `:2634` | `(bUseForMaterial \|\| bUseAsOccluder) && (bRenderInMainPass \|\| bRenderCustomDepth \|\| bRenderInDepthPass)` — an **OR** |
| custom-depth add | `:2727-2730` | `if (ViewRelevance.bRenderCustomDepth)` → `AddCommandsForMesh(..., EMeshPass::CustomDepth)` |
| dynamic gather | `:3161`, `:3166` | the same OR |
| the pass runs | `CustomDepthRendering.cpp:148` | `View.bHasCustomDepthPrimitives` |
| the processor accepts | `CustomDepthRendering.cpp:263-265` | gates **only** on `PrimitiveSceneProxy->ShouldRenderCustomDepth()` |
| the base pass REFUSES | `BasePassRendering.cpp:1831` | `&& (!PrimitiveSceneProxy \|\| PrimitiveSceneProxy->ShouldRenderInMainPass())` |
| the depth pass refuses | `PrimitiveSceneProxy.h:613` + `DepthRendering.cpp:883,887` | `ShouldRenderInDepthPass() = bRenderInMainPass \|\| bRenderInDepthPass` — **both false ⇒ off** |

⚠ **A trap worth naming:** `:2708` adds the mesh to `EMeshPass::BasePass` *even when main-pass is off*,
because the enclosing condition is an OR. **The command slot is created; the base-pass PROCESSOR then
refuses it** at `BasePassRendering.cpp:1831`. Reading only the gather would have said "the object still
draws".

**A2 — the other paths and what silences each:** shadows `CastShadow` +
`bCastContactShadow` (`FPrimitiveSceneProxy::IsShadowCast`, `PrimitiveSceneProxy.cpp:1280-1283`
returns false when neither static nor dynamic shadow is cast — note the **old** hide relied instead on
`!DrawInGame` at `:1288`) · Lumen / mesh cards `bAffectDynamicIndirectLighting` (`:1161`) · distance
fields `bAffectDistanceFieldLighting` (`:1166`) · ray tracing `bVisibleInRayTracing` (`:1118`) ·
decals `bReceivesDecals` (`:1083`) · velocity — falls out with the main pass
(`SceneVisibility.cpp:2639` gates velocity on `bRenderInMainPass`). 🚨 **Nanite targets remain
impossible (`G134`)** — a Nanite proxy never sets `bRenderCustomDepth`, so hidden-class masks cannot
exist for them and that limit is unchanged.

**A3 — occlusion semantics: correct by construction.** `AnomalyVisibleMask.usf:37` outputs the tag iff
`CustomDeviceZ >= SceneDeviceZ - DepthBias`. With the target absent from scene depth, `SceneDeviceZ` is
whatever is actually behind it; reversed-Z makes the (nearer) target's `CustomDeviceZ` larger, so the
test passes exactly where the target *would* have been the front-most surface, and fails where
something real occludes it. **That is the would-be-visible region, occlusion-aware, with no new code.**

**A4 — the census could have taken a main-pass-off target** (it is no longer `IsHidden`). Hardened:
`AnomalyCensus.cpp:88` and `:722` now also test the logical hidden state.

## §13.2 THE BUILD, and the one design point that matters

`AnomalyHiddenClass` (new, `AnomalyInjector`) owns the hide: a per-component ledger of the eight flags
it touches, `Hide`/`Show`, and a **logical hidden registry**.

🚨 **THE NON-OBVIOUS PART: the labels' notion of "hidden" was `AActor::IsHidden()`.** Stop calling
`SetActorHiddenInGame` and `blinking`'s entire hidden set silently empties — the labels would break
while the pixels stayed right. Every consumer of that test now asks
`AnomalyHiddenClass::IsLogicallyHidden`: the two label paths
(`AnomalyCaptureSubsystem.cpp:3619,3647`), **`m26`'s `LOCK-1` guard and its three siblings**
(`AnomalyMaskMeasure.cpp:226,272,315,506` — without this `m26` would start arming on hidden ticks and
`framesContributed` would move), and the census (`AnomalyCensus.cpp:88,722`).

Bench levers, console-only, never in a client payload: **`IAI.Bench.HideMode 0|1`** (default 1) and
**`IAI.Bench.HideOmitShadowSilencing`** (default off) — the deliberate mis-application for the
prove-it-can-fail leg.

## §13.3 🚨 THE IDENTITY GATE — and why the FIRST instrument was blind

**At the delivered configuration the comparator cannot answer the question.** Two runs of the **same
configuration** differ by **9.1612 %** of pixels (>8/255), worst frame 15.40 %:

| pair | mean % differing | worst % |
|---|---|---|
| **CONTROL** old-vs-old2 (identical config) | **9.1612** | 15.3992 |
| TEST old-vs-NEW | 8.5619 | 11.8074 |
| **CAN-FAIL** old-vs-NOSHADOW (shadows deliberately left on) | **9.5381** | 16.4272 |

⇒ **the deliberate violation sits INSIDE the control's own band, and the correct fix reads LOWER than
the control.** Pose, frame alignment, event sets and blink hidden sets are **identical** across all
three legs, so this is not `A47` and not misalignment — it is genuine per-run rendering
nondeterminism, and it dwarfs the effect. **`G169`: below the resolution of this instrument, never
"no difference".**

**Removing the temporal confound makes the comparator exact.** With
`r.AntiAliasingMethod 0, r.Lumen.DiffuseIndirect.Allow 0, r.DynamicGlobalIlluminationMethod 0,
r.ReflectionMethod 0`:

| pair | frames differing |
|---|---|
| **CONTROL** old-vs-old2 | **0 of 60** — the floor is genuinely ZERO, so the sensitivity is one pixel |
| **TEST** old-vs-NEW | **0 of 60** ✅ |
| **CAN-FAIL** old-vs-NOSHADOW | **0 of 60** ⚠ |

✅ **`M45-G1` PASSES on a zero-floor control**: the new hide changes not one pixel across 60 frames.
⚠ **`M45-G4` (prove-it-can-fail) IS NOT OBTAINED.** Both levers are **proven engaged** — the engine log
echoes `IAI.Bench.HideMode -> 1` and `IAI.Bench.HideOmitShadowSilencing -> ON` at frame 1 — so the
omission really happened and the picture still did not move. ⇒ **this target casts no shadow that
reaches the frame in `CB_GateLevel`**, so the fixture cannot exhibit the class the gate exists to
catch. **`G135`'s shape: a fixture that structurally cannot show the defect produces a clean pass.**

✅ **The mechanism does work:** the same legs produce **20 mask files under the new hide and 0 under the
old one.**

## §13.4 WHY IT DOES NOT MERGE

The brief makes `M45-G4` a required gate, and this project has fired `G96` four times on exactly this
shape: **a guard that has never been shown to fire is not a guard.** The identity claim is also
narrower than it looks — it is proven **at AA/GI/reflections OFF**, which is *not* the delivered
configuration; at the delivered configuration the instrument is blind. ⛔ **So `m45` branches and does
not merge, and I did not run the remaining gates (G2 detail, G3 IoU, G6, MASK-TIE, ONSET,
MASK-PICTURE-PAIRING) — a milestone that cannot ship does not need them, and running them would
manufacture a green table around an unproven safety argument.**

## §13.5 WHAT WOULD DECIDE IT

1. **A fixture that casts a visible shadow from the target** — then the can-fail leg becomes decisive
   at AA-off and `M45-G4` is obtainable. This is the cheapest route and needs no product change.
2. **Identity at the DELIVERED configuration** needs a within-frame comparator (`G-M9`'s shape, m35),
   because cross-run temporal accumulation is 9 % here. Alternatively, declare that the identity
   arbiter runs at AA-off **by design** and say so in the gate.
3. The fallback if identity ever fails: coarse projected-bbox masks flagged `"coarse": true`, which
   needs `m39`'s honest bbox first.

---

# §14. `m45` — EVERY GATE PASSES; THE SYNTH-ORDER IDENTITY ARBITER IS **UNOBTAINABLE**, SO THE MERGE IS CHAT'S CALL

**2026-09-03, session 069 brief 20.** `master` untouched at `d48e1ba`. Branch
`m45-hidden-class-masks-GATE-FAILED`.

## §14.1 THE CAN-FAIL LEVER WORKS — `M45-G4` IS OBTAINED

Ruling `D1 = (b)`: the lever is **`IAI.Bench.HideOmitDepthPassSilencing`** (console-only, default off,
echoed). It leaves `bRenderInDepthPass` true while the main pass is off, so the target still writes the
depth prepass and **occludes what is behind it while drawing nothing itself** — deterministic wrong
pixels wherever it overlaps background, in any fixture.

**Native order, at the AA-off arbiter** (`r.AntiAliasingMethod 0` + Lumen/GI/reflections off):

| leg | frames differing | mean % >8/255 | worst % |
|---|---|---|---|
| **CONTROL** old-vs-old | **0 of 60** | 0.0000 | 0.0000 |
| **TEST** old-vs-NEW | **0 of 60** | 0.0000 | 0.0000 |
| **CAN-FAIL** old-vs-depth-omitted | **20 of 60** | 1.5608 | 4.6875 @ si 46 |

✅ **`M45-G1` PASSES on a zero-floor control** — the new hide changes not one pixel.
✅ **`M45-G4` IS OBTAINED** — the gate demonstrably catches a mis-applied hide, with the lever proven
engaged in the engine log. **The previous lever (shadow omission) failed to fire because this fixture
casts no shadow into frame; this one does not depend on the fixture at all.**

## §14.2 ⚠ THE SYNTH-ORDER ARBITER IS UNOBTAINABLE — and the control proves it is not `m45`'s fault

| leg | frames differing | mean % |
|---|---|---|
| **CONTROL** old-vs-old, `SynthTickOrder` | **60 of 60** | **5.9526** |
| TEST ctrl-vs-NEW | 56 of 60 | 7.6437 |
| CAN-FAIL ctrl-vs-depth-omitted | 20 of 60 | 1.5609 |

🚨 **The CONTROL — the OLD hide against itself — differs on every frame.** Poses, origins and
`frame_index` are identical across all four synth legs, so this is not `A47`. **`IAI.Bench.SynthTickOrder`
relocates the injector's dispatch and makes the run nondeterministic even at AA-off**, which puts a
5.95 % floor under any cross-run comparison there. ⇒ **the identity question cannot be decided in that
order — RECORDED AS UNOBTAINABLE, never as passed** (the `m41` precedent for `B-G1`/`C-G1b`).
📌 **It is a property of the bench lever, not of `m45`: the old hide is equally nondeterministic there,
and the lever never ships.** ⛔ **But it is one of the two orders the standing rule requires, so I am
not calling this a full pass.**

## §14.3 EVERY OTHER GATE — BOTH ORDERS, DELIVERED CONFIGURATION

| gate | native | synth |
|---|---|---|
| **M45-G2** masks on hidden frames | ✅ **both `blink` events now have masks**, delta 0 | ✅ same |
| **G7 becomes EQUALITY for hidden-class** | ✅ `files=35 labelled=35 stray=0` | ✅ `35 / 35 / 0` |
| **M44-G1** onset (now all six events) | ✅ **6/6 delta 0** | ✅ **6/6** |
| **M44-G2** no blank PNGs | ✅ 0 | ✅ 0 |
| **M44-G3** counts | ✅ 35 + 0 + 55 = 90 | ✅ 35 + 0 + 55 = 90 |
| **MASK-TIE** | ✅ 35 lines, **0 MISMATCH** | ✅ 35 lines, **0 MISMATCH** |
| **G6** veto inputs vs the `m44` control | ✅ all six identical; event set, every `manifested`, `positive_frames` 43 and `non_manifested_events` 0 unchanged | — |
| **census health** | ✅ `framesPolluted 0`, `batchesLost 0`, `tagFailed 0`, `hidden=2` (the new live-fire skip, counted) | ✅ same |
| **both build targets** | ✅ | |

**MOVED COUNTERS, each explained:**

| counter | m44 → m45 | why |
|---|---|---|
| `target_mask_frames_measured` | 27 → **35** | **+8 = exactly the eight `blink` hidden frames (4+4) that now carry a mask.** This is the milestone |
| `target_mask_frames_unavailable` | 63 → **55** | the same eight, moved out of `unmeasured` |
| `census_frames` / `_zero` / `_below_floor` | 100→97 / 12→13 / 50→49 | run-to-run scale; verdicts and the histogram unchanged |

⛔ **`M45-G3` (IoU of first-hidden vs last-visible mask) was NOT run** — it is a quality measure on a
milestone whose merge is not settled, and running it would add a number to a table that cannot yet
close. Named, not skipped silently.

## §14.4 THE POSITION

`m45` is **functionally complete and green everywhere it can be measured**. The single open item is
that one of the two required orders cannot host the identity arbiter, **for a reason proven to be
independent of `m45`** (the old hide is equally nondeterministic there). ⛔ **I did not merge**: the
standing rule says both orders, and inventing an exemption for my own change is exactly the shape this
project stops for. **The merge is chat's ruling.**

---

# §15. `m45` MERGED — the ruling, `M45-G3`, and what shipped

**2026-09-03, session 069 brief 21.** Owner ruling `(a)`.

## §15.1 THE RULING, RECORDED AS A SCOPE RULE

> **Identity/pixel arbiters run native-order at the AA-off configuration; alignment gates run both
> orders.**

`IAI.Bench.SynthTickOrder` perturbs injector dispatch and is nondeterministic by its **own old-hide
control** (60 of 60 frames differ, mean 5.95 %, at AA-off where the native control is 0 of 60), so it
cannot host a pixel arbiter — and it never ships. ⛔ **This is the rule's scope, not an exemption for
`m45`**; `G230` carries it, and the both-orders requirement is untouched for the alignment gates
(labels, masks, onset, pairing), all of which passed in both orders.

## §15.2 `M45-G3` — the would-be silhouette, measured

Hidden-frame mask against **the same actor's mask while visible**, same settled camera:

| event | hidden frame | visible reference | pixels | **IoU** | camera delta |
|---|---|---|---|---|---|
| `blinking` `StaticMeshActor_73` | si 40 | si 27 (`corrupted_texture`) | 48,590 vs 48,591 | **0.9987** | `(0,0,0) / (0,0,0)` |
| `missing_object` `StaticMeshActor_73` | si 3 | si 27 (`corrupted_texture`) | 48,568 vs 48,591 | **0.9969** | `(0,0,0) / (0,0,0)` |

**Both tick orders, identical to four decimals.** Pass bar was 0.9.

📌 **The reference had to be chosen, and the choice is the point:** there is no mask on a blink's
visible in-between frames **by design** (`G7` forbids it), so "the last visible frame's mask" does not
exist within a hidden-class event. The sound reference is **the same actor's silhouette from a
non-hidden event in the same run at the same camera** — which `StaticMeshActor_73` provides, having
both a `corrupted_texture` and a `blinking` event.

⚠ **One measurement was thrown away before it was read.** The first `missing_object` legs ran on the
**m44** binary — I had restored the bench to the shipped exe at the end of the previous brief and did
not re-stage. They produced `visible=0` on all 40 frames and no masks, which is exactly what the m44
hide should produce. **Diagnosed from the leg's own `NOT ARMED` counters, not guessed**, and re-run on
`8A895272`.

## §15.3 THE MERGE

Merge commit on `master`; the branch was renamed `m45-hidden-class-masks` first so the merge does not
carry `GATE-FAILED`, and the old name is deleted from `origin`.
✅ **`F1` verified still out of `master` two ways:** `git diff 62bd287 master --
Shaders/Private/AnomalyVisibleMask.usf …` is **empty**, and the F1 commit is not an ancestor. It waits
on the cook, on `m44-f1-resolution-mapping-UNVALIDATED`.
⛔ **NO TAG.** Office batch: `… m41 → m43 → m44 → m45`.

## §15.4 DOCS

`client-readme` — the hidden-class limitation paragraph is **removed** and replaced by the mask
semantics (would-be silhouette, occlusion-aware, none on a blink's visible frames, Nanite excluded,
the measured IoU range). `client-delivery` — the identity arbiter and its can-fail leg as cook-time
gates, `G7`-equality, ONSET 6/6, and the delivered-configuration limitation stated in as many words.
`PRE-DELIVERY-CHECKLIST` §1.1 — three `m45` boxes; the bench-lever grep now covers `HideMode`,
`HideOmitShadowSilencing` and `HideOmitDepthPassSilencing`. Card **Section F read (g)**. `CLAUDE.md` —
the hidden-class hide, the logical-hidden registry, the permanent arbiter and lever, the scope rule,
and **`git add <directory>` banned alongside `git add -A`** after it swept the owner's untracked
handoffs once. Gotchas **`G228`–`G230`**. Ledger **§12**.
---

# §16. TWO COOKS, AND `m46` — F1 IS VALIDATED AND MERGED

**2026-09-03, session 069 brief 22.** Both cooks run as their own sequenced operations (`G118`).

## §16.1 The first cook — re-baselining `master` at `m45`

Followed runbook §8.6 verbatim. **STEP 0** disk 188 GB free (floor: ≥15 GB GO). **Step 1 re-bank
(`G92`): 16 unbanked sessions found BY SESSION ID** in the package tree — including three
`M43G_LBOX` attempts, `M45_SYNTH_try1` and a 2026-08-23 `AnomalyCaptures` session — copied to
`B22_PRECOOK_RESCUE`, 16/16 with per-file counts matching. **Step 2** the quartet archived and
**hash-verified at the new location** (`A62`). **Step 3 map set declared in writing BEFORE the cook:**
`CB_GateLevel` + `MainMenu` + `MainWorld` (+ `Entry`, engine default). **Step 3.5** editor target
rebuilt and A44-scanned — `HideOmitDepthPassSilencing` ×4, `IAI.Bench.HideMode` ×4, control `blinking`
×9, so the scan is sound and not blind. ⚠ **The runbook's example control `IsHideTypeAnomaly` reads 0
— it is STALE (renamed at session 053) and `blinking` was used instead.**

`BUILD SUCCESSFUL`, exit 0, 1 m 6 s. **Map gate PASS exit 0**, all four maps read **out of the
container index** (`G119`). **Token read-back: 64 characters, from `DefaultGame.ini`, not the
placeholder** (`G118`). No `Missing global shader`.

| | exe | `.utoc` | `.ucas` | `.pak` |
|---|---|---|---|---|
| pre-cook | `8A895272` | `2A66CA57` | `A7EF9B12` | `D8009AD7` |
| **`m45` post-cook** | `38A55169` | `4621F571` | `513F4D35` | `2163A13A` |

## §16.2 `A4` — the cook changed NO behaviour (`G103`)

Re-baselined on the new container, both tick orders: **ONSET 6/6 · G7 equality 35 = 35 stray 0 · G2 0
blanks · G3 35+0+55 = 90 · MASK-TIE 35 lines 0 MISMATCH · MASK-PICTURE-PAIRING NEITHER 0 / PREVIOUS 0
(33/33 native, 35/35 synth) · `m26` known-answer control fires (`mask_probe_arms 1`) · census
`candidates 77 · zero 13 · nonzero 62 · belowFloor 49 · tagFailed 0 · hidden 2 · framesPolluted 0 ·
batchesLost 0 · tagOvertaken 3`** — **every one identical to the pre-cook container.** Only
`census_frames`/`cycles` move at run-to-run scale. **These are `master`'s new `P-C7` controls.**

## §16.3 The second cook and `m46`

F1 cherry-picked onto `m46-f1-resolution-mapping`; the `A4` quartet archived first; editor rebuilt;
second cook `BUILD SUCCESSFUL` exit 0, 2 m 25 s, map gate PASS.
**`m46` quartet: exe `60AE8C61` · `.utoc` `EF8EB23C` · `.ucas` `A8BFFF88` · `.pak` `3C026A8D`.**

**THE DECISIVE GATE, against the banked prove-it-can-fail leg:**

| `r.ScreenPercentage 50` (internal 640×360 vs output 1280×720) | pairing |
|---|---|
| BEFORE (`069-16`) | **CURRENT 0 of 26, NEITHER 25** |
| AFTER | **CURRENT 35 of 35, NEITHER 0, PREVIOUS 0** |

100 % unchanged (33 of 33). ONSET 6/6, G7 equality, MASK-TIE 0, `m26` probe fires, census clean, every
veto counter and `positive_frames` identical — all both orders.

🚨 **`P-C7 v2` LOOKED LIKE A FINDING AND WAS NOT, AND THE CONTROL IS WHY.** 5 of 35 mask silhouettes
differ against the pre-`m46` control, one by **23,198 pixels** at `session_index 52`. **A same-build
control reproduces it EXACTLY** — the same 5 frames, the same frame, the same 23,198-pixel symmetric
difference, with the two pixel counts merely swapped (66,635 ↔ 89,833). ⇒ **census run-to-run variance,
not `m46`** (`G169`). ⛔ **Without that control this would have been reported as a defect.**

## §16.4 State

`master` carries `m45` + `m46`. ⛔ **NO TAG** — office batch `… m43 → m44 → m45 → m46`.
`m44-f1-resolution-mapping-UNVALIDATED` is deleted; its content is on `master`.
📦 Three container quartets archived: `m45-precook-container-d204598`,
`m45-postcook-container-A4`, and the live `m46` one.
✅ **The nearest-neighbour mask-resampling follow-up is RETIRED** — the mapping is nearest by
construction.
## §17 `m47` — SHADER READINESS: THE MECHANISM IS REAL, THE SYMPTOM DID NOT FOLLOW

**Brief 069-25** (chat thread A), after 069-24 halted at its repo-state gate. Chat ruled the checkout:
`europa-e1 1f5e305` → `master 489c29d` at the start, back at the end. Both directions stated.

### §17.1 What the owner reported, and what was actually asked

Bates AND Concorde, **editor target only, never packaged**: (1) in ~5–10 % of `corrupted_texture` /
`missing_texture` events the swapped material is not visible on the first frame(s); (2) in some of
those the target renders BLACK; (3) occasionally the WHOLE picture is black for a burst and recovers.
Chat's hypothesis: **on-demand shader compilation in the editor** — the first use of an anomaly
material per mesh usage compiles its shader map and the object draws the fallback until it lands.

### §17.2 Step 0 (ND-4) — the usage flags were ALREADY correct on disk

`tools/set_material_usage_flags.py`, headless: **14 of 14 flags (7 × 2 materials) read `True` BEFORE`,
`True` AFTER, "already correct - no change made", EXITCODE=0.** ⇒ the `m30`/Concorde
"missing `bUsedWith…` ⇒ forcing default material" route is **EXCLUDED on this bench's assets**, and it
is excluded by a read rather than by assumption (`G159` closes the string-scan route, so the editor
was the only instrument).
⚠ The script saves unconditionally, so the two `.uasset` files came back byte-different with
**identical flag values**. That is re-serialisation, not a change. **The churn was REVERTED** so the
tree still matches the bytes the `m46` cook was made from; committing it would have put a binary diff
with no semantic content into `m47` and raised a re-cook question for nothing.

### §17.3 The mechanism IS real — and its trigger is PER VERTEX FACTORY

`FMaterialRenderProxy::GetMaterialWithFallback` (`MaterialShared.cpp:4207-4229`) walks to a complete
fallback and calls `SubmitCompileJobs_RenderThread` — on-demand compilation at draw time, confirmed
from source (069-24). The chain terminates at the engine default material, i.e. the **grey grid**.

**But `IsGameThreadShaderMapComplete()` is a WHOLE-shader-map predicate and a draw needs ONE vertex
factory.** With 7 usage flags a material must compile for ~7 VF families; the one a plain
`StaticMeshActor` uses (`FLocalVertexFactory`) is compiled first. That is the reason for §17.4.

### §17.5 A1 — measured on the EDITOR target (`run_leg_editor.ps1`, new)

| leg | condition | pending | incomplete | frame_lum min | target_lum on labelled frames | black | dark 1st |
|---|---|---|---|---|---|---|---|
| E1 (1st run after build) | warm-ish, prewarm OFF | 157 → 73 | (not yet instrumented) | 60.111 | 106.894 – 127.000 | 0 | 0 |
| E1 (2nd run) | warm DDC, prewarm OFF | **0** throughout | **0** | 100.787 | — | 0 | 0 |
| E2 | `IAI.Bench.ForceAnomalyShaderRecompile` | 0 | **0 — LEVER DID NOT FIRE** | — | — | 0 | 0 |
| **E2b** | **`-nomaterialshaderddc` (cold material DDC)** | **3760 → 3672** | **2 on 90/90 frames** | **59.992** | **106.883 – 124.727** | **0** | **0** |

🚨 **§17.4 THE RESULT: THE CONDITION WAS FORCED TO ITS MAXIMUM AND THE SYMPTOM DID NOT APPEAR.** On
`E2b` both anomaly materials were incomplete on **every one of 90 armed frames** with **3760 shader
jobs outstanding**, and the `corrupted_texture` target still rendered correct magenta on every frame
**including every event's first frame** — target luminance 106.9–124.7 against the warm control's
106.9–127.0. **No black frames. No dark first frames. No dark target regions anywhere.**
⇒ **The hypothesis is SUPPORTED AS A MECHANISM and NOT REPRODUCED AS A SYMPTOM on this fixture.**
That is stronger than "we could not make it happen": we made it happen at full strength.

⚠ **`IAI.Bench.ForceAnomalyShaderRecompile` COULD NOT FIRE** — `ForceRecompileForRendering()` clears
the in-memory map and the warm DDC refills it before the next query (`incomplete BEFORE=0 AFTER=0`).
The lever that worked is an ENGINE switch, `-nomaterialshaderddc`, which simulates a cold DDC on
first encounter — i.e. **exactly the owner's "first use per combination, cached afterwards" shape.**
The plugin-side lever is kept because it names the refusal in a packaged build, which is its own
measurement; **its inability to fire on a warm cache is recorded, not hidden** (`m45`'s shadow-lever
shape recurring — a can-fail lever that a fixture defeats).

### §17.6 What symptom (2) and (3) land on — HONEST ANSWER: UNKNOWN, with the next discriminator named

- **(1) "swap not visible on first frame(s)"** — CONSISTENT with the mechanism, NOT observed here.
- **(2) "target renders BLACK"** — **NOT EXPLAINED.** The mechanism's fallback is the engine GREY
  grid, not black, and no dark target region occurred on any leg. The competing candidate (both
  materials are `MSM_DEFAULT_LIT` with no emissive, so zero light renders black) is **NOT TESTED** —
  it could not be, because no black target ever appeared to attribute.
- **(3) "whole picture black for a burst"** — **NOT EXPLAINED and NOT REPRODUCED.** A per-material
  fallback cannot blacken a whole frame. Named but untested candidates: auto-exposure response (the
  bench runs `r.DefaultFeature.AutoExposure 0`, the owner's editor does not) and Lumen surface-cache
  invalidation on a large re-materialised surface. ⛔ **NO MECHANISM ASSERTED for either.**
- 🚨 **NEXT DISCRIMINATOR, and it follows from §17.3: fire at a SKELETAL or Nanite target under a
  cold material DDC.** Those VFs compile later than `FLocalVertexFactory`, so they are where the
  fallback window is wide — and the owner's Concorde symptom was on a **held weapon (skeletal)**,
  which is the same axis `m30`'s usage-flag defect sat on. ⚠ **ATTEMPTED TWICE AND NOT RUN:**
  `MainWorld` + target `Bot` returned **`-> 0 matched`** (no actor name or class contains it), and
  `MainMenu` **failed to load** under `-game` (`Failed to enter /Game/StackOBot/Maps/MainMenu`).
  Recorded as UNTESTED, not as a null.

### §17.7 TWO GATE-DESIGN DEFECTS FOUND BEFORE THEY SHIPPED

**(a) The briefed counter is GLOBAL.** `GShaderCompilingManager->GetNumRemainingJobs()` is
process-wide. On the first editor run after a build it read **157 falling to 73 across all 90 frames**
while our materials were perfectly complete and every pixel was right. A `render_state` keyed on it
would have marked **90 of 90 frames** of that leg "shaders_pending" — true, useless, and it would
train the reader to ignore the key. **So the shipped gate keys on `CountIncompleteAnomalyMaterials()`
— THIS PLUGIN'S two swap materials — and the global count ships beside it as a reading.**

**(b) Even the specific predicate over-fires**, and that is stated rather than papered over:
`IsComplete()` was `false` on all 90 `E2b` frames while the drawing VF was complete. It means
"something about this material is still compiling", not "this draw will fall back". It errs toward
marking good frames suspect — the safe direction — and the log line says so in as many words.

⇒ **ND-2 ACCEPTED AND NOW MEASURED, not merely argued:** `frames_shaders_pending == 0` in a packaged
build is a **READING** (`EnsureIsComplete`'s body is `WITH_EDITOR` only, and the prewarm costs
**0.0017 ms** there — the number that proves it is a no-op). The packaged claim rests on **B3's pixel
gate**.

### §17.8 A2 — packaged legs, and the gate table

| leg | prewarm | prewarm_ms | incomplete | frames_shaders_pending | frame_lum min | black | dark 1st | ONSET | G7 |
|---|---|---|---|---|---|---|---|---|---|
| P1 | OFF | −1 (skipped) | 0 / 90 | 0 | 99.467 | 0 | 0 | 8/8 Δ0 | stray 0 |
| P2 | ON | **0.0017** | 0 / 90 | 0 | 99.611 | 0 | 0 | 8/8 Δ0 | stray 0 |
| P3 | ON + `SynthTickOrder` | 0.0019 | 0 / 90 | 0 | 99.312 | 0 | 0 | 8/8 Δ0 | stray 0 |

All three: 90 frames, 59 positive, `vetoed_events` 0, `non_manifested_events` 0, `speed_ratio` 1.0000,
`target_mask_frames_measured` 59, B1 pose gate **PASS** (`modal_rot (0,0,0)`, `distinct=1`,
`modal=100.0 %`), A63 accepted on attempt 1.
**Both tick orders run, per the standing rule.**

### §17.9 `P-C7 v2` and `P6`

`run_summary` **55 → 58 keys, added exactly `shader_prewarm_ms` / `shader_prewarm_incomplete` /
`frames_shaders_pending`, removed 0.** `labels.jsonl` field set **21 → 21, IDENTICAL** on a packaged
leg — the two m47 frame keys are emitted only when a swap material is incomplete, so a healthy run
carries no new key at all. ⛔ **`annotation.json` 48 keys, IDENTICAL — `P6` DID NOT MOVE.**

### §17.10 B3 — the black-frame gate, and its threshold

`tools/verify_capture.py --black-frame-gate`, plus `--selftest`. **Threshold 6.0 on the 0..255
whole-frame mean, DERIVED not chosen:** the darkest legitimate frame this fixture produces is
**59.992** (E2b) and the gate fires an order of magnitude below it, so a frame would have to lose 90 %
of its brightness to trip. The derivation rule travels to a darker title even though the number does
not; `--black-threshold` exists for that.
✅ **PROVEN BOTH WAYS (`G96`):** `--selftest` builds a synthetic mid-grey session and an all-black one
and asserts **PASS then FAIL**. It reports `SELFTEST: BROKEN - the gate PASSED an all-black frame` if
the black case ever passes, because a gate that cannot fire is blindness.
The second reading, **DARK FIRST FRAMES**, scores each event's first labelled frame against **that
event's own mean** rather than an absolute, and is REPORTED — a gate only where it must be zero.

### §17.11 Build identity

📦 Staged bench exe **`F309D836`**; predecessor **`60AE8C61`** (the m46 quartet's exe) archived FIRST
as `StackOBot.exe.m46-resolution-60AE8C61` and **hash-verified at the archive**. **Container quartet
UNCHANGED** (`EF8EB23C` / `A8BFFF88` / `3C026A8D`) — code-only hot-swap, **NO COOK** (`G103`); m47
adds no shader and no shader parameter.
⚠ **A44 CAUGHT A REAL STALENESS.** The first staged exe (`2C167ADE`) was linked BEFORE the second
round of edits: `IAI.Bench.ForceAnomalyShaderRecompile` and `anomaly_materials_incomplete` read **0**
while `IAI.Capture.ShaderPrewarm` read 7 and the pre-existing `IAI.Bench.MaskPairingProbe` read 5 —
**so the scan was sound, not blind**, and it named the missing half. Rebuilt and re-staged.
✅ **BOTH BUILD TARGETS exit 0** (`G221`): packaged `StackOBot` and editor `StackOBotEditor`.

---

## §18 `m47b` — AUTO-EXPOSURE: MEASURED, LARGE, NOT EDITOR-ONLY, AND NOT THE SYMPTOM

**Brief 069-26** (chat thread A), the `ND-B (iii)` option from `m47`'s close. ⛔ **NO SOURCE CHANGE —
docs and harness only.** Pre-declaration `docs/predictions/2026-09-03-m47b-auto-exposure.md`, written
and committed **before any leg ran**; nothing in it was edited afterwards.

### §18.0 The checkout (shared-tree rule clause 4)

| | branch | SHA | state |
|---|---|---|---|
| **IN** | `europa-e1` → `master` | `1f5e305` → `93c2b65` | `europa-e1 == origin/europa-e1`, tracked tree clean |
| **OUT** | `master` → `europa-e1` | `93c2b65` → `1f5e305` | left exactly as found |

`master == origin/master == 93c2b65`. **Staged exe verified BEFORE any leg: `F309D836`**, matching
`_binary_baselines/README`'s LIVE entry; container quartet `EF8EB23C` / `A8BFFF88` / `3C026A8D`
unchanged. Editor DLLs on disk were master's (`m47` build, 17:44-17:53) so **no rebuild was needed**.

### §18.1 The premise was checked before it was tested — and it held

`m47` named auto-exposure as a candidate on the grounds that *"every bench leg forces it off; the
owner's editor runs the game's defaults"*. Both halves were verified by reading first:

- The AE-off pair is issued by `run_leg.ps1:187`, `run_leg_editor.ps1:78`, `verify_lastrundir.ps1:61`.
- The plugin never touches exposure — one log string, no code.
- **StackOBot's `DefaultEngine.ini` sets no exposure key, and neither does `BaseEngine.ini`** ⇒ the
  game's defaults ARE the engine defaults: `r.DefaultFeature.AutoExposure = 1`, Histogram, Bias 1.0,
  legacy range, `Min 0.03 < Max 8.0` — adaptive, not fake-manual.
- **It reaches the pixels**: the capture SVE subscribes AFTER `Tonemap`
  (`AnomalySceneViewExtension.cpp:72`), and eye adaptation lives in the tonemapper.

An **A48 echo was added to both runners on BOTH sides of the switch**, so a leg's AE regime is read off
its own log rather than inferred from the flag passed. It confirmed `AutoExposure = "1" LastSetBy:
Constructor` / `EyeAdaptationQuality = "2" LastSetBy: Scalability` **in the packaged build too.**

### §18.2 A1/A2 — the measurement, and the `AE-LIVE` control that licenses it

Full eight-leg table: **ledger §11.2**. Matched packaged pair, same binary/seed/target/config:

| | AE OFF | AE ON |
|---|---|---|
| whole-frame mean | 102.488 | **77.907** (−24 %) |
| spread over 90 frames | 7.100 | **32.020** (4.5×) |
| max drop vs previous 8 frames | 2.04 % | **9.01 %** |
| the same anomaly's target luminance | 123.7 – 128.1 pinned | **117.8 → 90.5** in one session |

**Four AE-OFF legs: ZERO frames dropping >3 % below their own recent mean. Four AE-ON legs: 13-21
each.** No overlap on any statistic, across two anomaly types and both build targets.
⇒ `AE-LIVE` (`G96`) **PASSES**, so the dip readings are evidence rather than blindness.

🔑 **The dominant effect is a session-start convergence transient (103.9 → ~74 over ~30-40 frames), not
an event-locked dip; the steady-state sawtooth is 1.4-2.1 %.** The long-window leg bounds recovery at
**>39 frames and still rising**, matching `SpeedDown = 1.0` (τ = 1 s).

### §18.3 A3 — 🚨 THE DIP IS **NOT** EDITOR-ONLY

The packaged AE-ON leg **reproduces the editor leg almost exactly** — spread 32.020 vs 31.258, max dip
9.01 % vs 8.61 %, target 90.5-117.8 vs 90.6-117.5. **The client would see this.** That is the answer
that decides the recommendation, and it contradicts the working assumption that these symptoms were an
editor-side artefact.

### §18.4 A4 — the black-frame gate does NOT fire, and that is correct

`--selftest` passes a grey session and **fails** an all-black one, so the gate can fire. On the deepest
AE-ON leg: **min 72.419 against threshold 6.0 — 0 black frames, 0 dark first frames, VERDICT PASS**, a
12× margin. **A legitimate exposure dip is the game's own rendering and must not be gated away.**
⚠ The `m47` threshold was derived from a **pinned-exposure** darkest frame (59.992). It survives the
regime change, **but by luck of direction** — the regime was not considered when it was derived.

### §18.5 The verdicts

- **(3) whole picture black** — **`AE-PARTIAL`.** Real, large, previously unmeasured, in both builds;
  ⛔ **but nothing approaches black at this stimulus (darkest 72.4/255).** NOT reproduced.
- **(2) target renders black** — 🚨 **AE REFUTED as a standalone explanation, STRUCTURALLY**: eye
  adaptation is global and cannot darken one object alone. Remaining named candidate (lit material,
  no emissive) is still **UNTESTED** — no dark target has ever occurred to attribute.
- ⚠ **`G135` GUARD, pre-declared in §4 of the prediction file:** the target is **7.2-7.8 % of frame**.
  ⛔ **This null does NOT exclude auto-exposure on the owner's content**, where coverage and the
  brightness gap may be far larger. Do not upgrade it.

### §18.6 Two corrections, both recorded rather than smoothed

**(a) The prediction was right for the wrong reason.** §1 of the prediction file argued the target's
first frame would be BRIGHTER (contradicting the brief's clause, which said darker) on the premise that
the swap material is brighter than the scene. The direction was **correct** — but a pinned-exposure
measurement then showed the swap is **DARKER** (`corrupted_texture` **−17.7 %**, `missing_texture`
**−3.5 %**). The real driver is **session position under a converging exposure**, not per-event
response. ⛔ **No mechanism is asserted for the sign of the per-event swing** (`G120`).

**(b) A checker defect, caught before its verdict was read (`G142`).** `bbox_px` is
**`[x, y, WIDTH, HEIGHT]`** (`AnomalyLabelWriter.cpp:95` emits `{X0, Y0, X1-X0, Y1-Y0}`). Read as
`(x0,y0,x1,y1)` it degenerates on this fixture — `(0, 485, 306, 235)` gives `y1 < y0` — to a 1-pixel
strip, and it returned a **confident wrong number** (region mean 240 instead of 131). Found by checking
the source when the value disagreed with the mask-derived one. **`m47_lum_table.py` carried the same
bug**; it is corrected, and it was **LATENT ONLY** — every `m47` leg resolved `region='mask'`, so the
fallback never ran and **no `m47` number came through it.**

### §18.7 `m48` RECOMMENDATION — PROPOSED, NOT BUILT (Task B)

Because **A3 shows the dip in packaged**, the brief's contingency selects **(i) + (ii)**:

- **(i)** a per-frame labels-side reading **`exposure_dip: true`** (emitted only when true, so a healthy
  run gains no key — the `m47` precedent that kept `labels.jsonl` at 21 fields), derived from
  `100 × (mean(lum[i−8..i−1]) − lum[i]) / mean(...)`, plus `run_summary.frames_exposure_dip`.
  🔢 **THRESHOLD, DERIVED AND TWO-SIDED — 4.0 %.** The highest drop any exposure-pinned leg produced
  across four legs and two anomaly types is **2.39 %**; the AE-driven legs reach **7.27-9.01 %**. Any
  value in `(2.39, 7.27)` separates them perfectly; **4.0 % sits 1.67× above the control ceiling and
  1.82× below the lowest AE-ON maximum.** Expected yield ≈ **9-12 frames of 90**, concentrated in the
  session-start convergence. *(Stronger than `m47`'s black-frame 6.0, which is bracketed on one side
  only.)*
- **(ii)** one paragraph in `client-readme.md`: the game's auto-exposure re-adapts for roughly a second
  after a large texture anomaly and at session start; those frames are marked, not removed.
- **(iii)** ⛔ **the plugin must NOT force exposure.** The dataset should look like the game.

⛔ **NONE OF THIS IS IMPLEMENTED.** Chat decides. **NEXT DISCRIMINATOR if (2)/(3) stay open: Lumen
surface-cache invalidation**, the other candidate `m47` named — and, for (2) specifically, a fixture
that actually produces a dark target, since without one the lit-no-emissive candidate has nothing to
discriminate.
