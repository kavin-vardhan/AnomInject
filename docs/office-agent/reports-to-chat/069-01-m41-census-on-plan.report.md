# REPORT for 069-01-m41-census-on-plan.md
brief: 069-01-m41-census-on-plan.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: high
started: 2026-09-02 22:33:13 | finished: 2026-09-02 22:53:33 | duration: 20 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-01-m41-census-on-plan.live.txt

---- FINAL MESSAGE (Claude Code) ----
**Plan committed and pushed: `47acfe6` (docs-only). No source, no build, no leg.**

---

# SESSION 069 — `m41` "CENSUS ON BY DEFAULT" — PLAN + PREDICTIONS

## State summary

1. `git rev-parse HEAD` == `origin/master` == **`0bd1a6d`** at bootstrap; now **`47acfe6`** after this turn's docs commit (pushed).
2. Only untracked files are the owner's five `docs/CHAT-HANDOFF-*.md` — never staged.
3. Full plan is durable at **`docs/sessions/2026-09-03-069-m41-census-on-by-default-plan.md` §1**; this report is its summary.
4. Census compiled default **OFF**; mask compiled default **OFF**; **`bAsyncCapture` compiled default `true`**.
5. 🔑 **The client ini has already set `bMaskMeasureDefault=True` since `m27`** — the delivered cook runs the mask now.
6. `bCensusEffective = census && mask && async` (`AnomalyCaptureSubsystem.cpp:1480`). Provider registration is guarded by it (`:1834`), cleared at `:3322` ⇒ census-OFF is **structurally** the pre-m36 picker.
7. 📌 **Record correction:** the `census_*` block is **12** keys, not 11 (`m37` added `census_above_ceiling`). `CLAUDE.md` says "+11" twice. `m41` takes it to **15**.
8. ✅ **C is implementable in a cooked build** — `UsedSceneTextures` is a serialized `LAYOUT_FIELD`, not editor-only data. No fallback needed.
9. ⛔ **F → `m42`.** Its cost premise is confirmed from 5.1 source; its *correctness* premise (TSR shimmer) is **not supported** by the same source.
10. B's loophole is confirmed exactly as briefed — and the census and the veto are **not independent** on that actor, which is why the fix belongs at the census layer.
11. E is **not a predicate bug** — the two visible-sets are structurally nested; unseen candidates are **temporal**.
12. **5 NEEDS-DECISION items.** Nothing implemented; stopping here.

---

## A — THE DEFAULT FLIP

### File-by-file

| file | change |
|---|---|
| `AnomalyCaptureSubsystem.h` | `bCensus` `false→true`; `bMaskMeasure` `false→true`; `+ bool bCensusFromConsole` |
| `…cpp:917-935` (`SetCensus`) | set `bCensusFromConsole` |
| `…cpp:1046-1053` `DescribeCensusSource` | 🚨 **must-fix.** Returns `"COMPILED DEFAULT (off) or IAI.Capture.Census"` — **false after the flip**. Three exact branches: console / ini / `COMPILED DEFAULT (on)` |
| `…cpp:457-463`, `:1456-1466` | same provenance correction for the mask echoes |
| `…cpp:1500-1507` | keep the inactive-census warning; it now genuinely means "the mask was turned off" |
| `client-delivery.md`, `PRE-DELIVERY-CHECKLIST.md`, `CLAUDE.md` | ini block, `G-R7(ii)` box, status + the 11→12→15 correction |

**Client ini** gains `bSelectionCensusDefault=True`, `CensusMinDrawnCoveragePctDefault=0.5`, `CensusMaxDrawnCoveragePctDefault=25.0`, `CensusMaxVerdictAgeTicksDefault=12`, `bCensusExcludeTranslucentDefault=True`. ⚠ Now **redundant-but-explicit** — keep them so the provenance echo reads `ini`, and *say so in the doc* or someone deletes the "redundant" key and re-opens `G88`.

**Key-set delta, `run_summary.json`, census-effective only: 12 → 15.** Existing 12 + `census_host_pp_customdepth_readers` (C) + `census_fires_partial_fallback` (D) + `census_fires_unseen_candidates` (E). ⛔ **`annotation.json` does not move** — `P6` unmoved, measured.

**Comparability:** `P-C7` stays gated, re-anchored on the `m41` build. Census-ON is the new baseline from the `m41` SHA; the `IAI.Capture.Census 0` bisect still reaches every banked census-OFF leg.

### Gates & predictions

| gate | prediction |
|---|---|
| **A-G1** inert-when-OFF (`P-C7`) | `frame_indices` identical, `labels.jsonl` 0 row diffs, **no `census_*` key**, only the run-unique trio differs |
| **A-G2** provenance, **both directions** (`G96`) | (a) no ini/console → census **ON** from **`COMPILED DEFAULT (on)`**, mask same; (b) `Census 0` → **off** from **`IAI.Capture.Census (console)`**. ⚠ **a disjunction in either branch is the defect** |
| **A-G3** subset | delta = **exactly the 15 `census_*` keys** |
| **A-G4** `P6` | 48/48, added 0 removed 0 |

⚠ **The bench cannot certify A** — max drawn coverage here is ≈6 %, so the ceiling never bites and floor 0.5 admits nearly everything; selection may be near-unchanged. **Bates is the instrument (G).**

---

## THE MASK-DEFAULT AND `G-R7(ii)` STATEMENT — VERBATIM

> **`bCensusEffective = census && mask && async`, so shipping the census ON requires the mask ON.** The mask's compiled default is `false` today; the client ini has set `bMaskMeasureDefault=True` since `m27`, so the delivered cook already runs it. `m41` flips the compiled default to `true` as well, so that a lost ini key downgrades provenance instead of silently restoring `m25` labelling. **The cost of the census above an already-ON mask is S3's measured +1.5352 ms per engine frame (1080p, unpaced, dev box); at the shipped paced 30 fps the dev box absorbed it entirely — that is headroom, not free.** The mask's own cost has never been isolated on any host. **`G-R7(ii)` — the physical-only hitch + throughput gate on master's own cook — is therefore a HARD DELIVERY PRECONDITION for the next client cook, not a merge precondition and not optional.** Nothing ships to the client off master until it passes there.

⛔ **Do not quote S3's +1.5352 as the mask's cost** — both S3 sides ran mask-ON. `G-M6` (the hook-cost prior) was never run, so the mask's own cost is unmeasured on every host.

---

## B — THE TRANSLUCENT LOOPHOLE

**Confirmed from source, exactly as briefed.** `ComponentSlotsAllTranslucentWithoutOptIn` (`AnomalyCensus.cpp:40-64`) returns *"not translucent-only"* the moment any slot answers `IsTranslucencyWritingCustomDepth()`. The fog card is then Measurable, tagged, and the mask counts its **geometric silhouette** → `MEASURED_NONZERO` at a healthy `drawnPct` → selected → hidden → nothing visible changes.

🚨 **And the veto does not catch it either, because the census and the veto read the SAME custom-depth silhouette. On this actor class they are not independent checks.** That is the argument for fixing it at the census layer.

| file | change |
|---|---|
| `AnomalyCensus.h` | `FAnomalyCensusParams += bIncludeTranslucentCustomDepthWriters = false` |
| `AnomalyCensus.cpp:40-64` | → `ComponentSlotsAllTranslucent(Prim, bAllowCustomDepthOptIn)`; the opt-in early-return becomes conditional |
| `…cpp:84-138`, `:158-169` | `ClassifyCandidate` forwards it; `Begin` banner echoes it |
| `AnomalyCaptureSubsystem.{h,cpp}` | member + ini `bCensusIncludeTranslucentCustomDepthWritersDefault` + console `IAI.Capture.CensusTranslucentWriters` (mid-run guarded, out-of-range refused, provenance echoed) + threaded at `:1836-1842` |
| `…cpp:1481-1499` | the **existing** effective-for-this-run line gains `includeTranslucentWriters=%d(%s)` |
| `…cpp:993-998`, `:4270-4280` | help text states the two knobs' relationship |

⚠ Naming deviation: ini key verbatim from the brief; console shortened to `…CensusTranslucentWriters` to match the family.

**Veto question — I agree with chat's lean: census layer only.** The veto deletes only on a **measured zero**, no ratio, no class rule; a class rule in the veto is a new deletion criterion that fails in the data-destroying direction if wrong. The census **refuses to select**, which is the safe direction. ⚠ **Named residual, recorded not queued:** a translucent-only target can still reach an event via the fallback path or targeted fire, and the veto will not catch it. That is a limitation of `m41`, not a hole to plug by widening the veto.

**Gate B-G1, both directions (`G96`):** knob OFF → `EXCLUDED(translucent)` in the `NOT-MEASURED` listing, `census_excluded_translucent ≥ 1`, never `Eligible`. Knob ON → `MEASURED_NONZERO` with a plausible `drawnPct`, counter one lower. **Failure branch:** `EXCLUDED` with the knob ON means the fixture is wrong (likely a *Masked* material) — report `GetBlendMode()` and the opt-in flag; **do not adjust the fixture until the cause is named.**

⚠ No such actor exists here and **`CB_GateLevel` is FROZEN (`G99`)** → **NEEDS-DECISION 2.**

---

## C — HOST POST-PROCESS PREFLIGHT

### The Shipping-build answer (and a correction to the question)

**`ANOMALY_CAPTURE = 0` in Shipping — the census does not exist there at all. Shipping is moot.** The real constraint is a **cooked build with `WITH_EDITORONLY_DATA = 0`**, which is what a client capture build is. In that build:

- ⛔ Expression walking (`UMaterial::Expressions` → `UMaterialExpressionSceneTexture`) is **stripped**.
- ✅ **A runtime read exists.** `FMaterialCompilationOutput::UsedSceneTextures` is a **`LAYOUT_FIELD(uint32,…)`** (`MaterialShared.h:735`), declared **outside** the `#if WITH_EDITOR` block ⇒ **serialized into the frozen shader map and present in a cook.** Exposed as **`FMaterialShaderMap::UsesSceneTexture(uint32)`** (`MaterialShared.h:1382`, public). `PPI_CustomDepth = 13`, `PPI_CustomStencil = 25` (`MaterialSceneTextureId.h:45, 69`).
- Path, game thread: `UMaterialInterface::GetMaterialResource(FeatureLevel)` → `FMaterial*` → `GetGameThreadShaderMap()` (`:2097`, public) → `UsesSceneTexture(…)`. MIDs resolve to the parent's shader map, so they are covered for free.

⇒ **No honest-fallback branch is taken; C ships as a real detector.** The owner's card glance is retained as a **second instrument**, not a fallback.

**Sources at StartRun:** `TActorIterator<APostProcessVolume>` → `Settings.WeightedBlendables`; and `PlayerCameraManager->GetCachedPostProcessBlends()` (`PlayerCameraManager.h:381`, public — the accumulated per-frame list; one authoritative source rather than hand-walking camera components).

**The line** (unconditional when the census is effective; `=0` Log, `=N` **Warning**):
```
Capture(census): HOST-PP CUSTOM-DEPTH READERS = 0 (scanned V volume(s), C camera blend(s), M material(s))
Capture(census): HOST-PP CUSTOM-DEPTH READERS = N [<Name>(depth), <Name>(stencil), …] (scanned V/C/M) - WARNING
```
🔑 **The `scanned` counts are load-bearing:** a `=0` with `scanned 0/0/0` is **blindness**, not a clean read. `G96` applied to a preflight. `run_summary += census_host_pp_customdepth_readers`.

**Limits, in the line's own text:** (1) it detects *sampling*, not visible effect — `N>0` is a "look at this", not a defect; (2) ⛔ **`N=0` does NOT mean nothing on the host reads custom depth** — a host SVE/custom pass, Niagara, UMG or a decal is invisible to it; (3) StartRun snapshot — a mid-run blendable is missed; (4) weight-0/disabled volumes count as present (safe direction); (5) feature-level scoped.

**Gate, split so a missing fixture is not a blind spot:**
- **C-G1a (always runnable):** the query correctly reports `PPI_PostProcessInput0` on a stock PP material ⇒ enumeration + shader-map path proven alive with nothing authored.
- **C-G1b (both directions):** an authored CustomDepth-sampling PP material via a volume → `=1` named at Warning; removed → `=0` at Log with non-zero scanned counts. ⚠ **Needs a cooked asset and therefore a cook** (every census gate so far was a code-only hot-swap) → **NEEDS-DECISION 3.**

---

## D — CYCLE-RELATIVE VERDICT EXPIRY

**Today:** `Age > MaxVerdictAgeTicks (12)` → expired (`AnomalyCensus.cpp:298-305`). `CycleTicks` is *already computed and logged* at `:748`. A host whose cycle exceeds 12 ticks expires its earliest-measured candidates **before the cycle closes** ⇒ recency bias, and the only loud signal fires on **ALL**-fallback, so partial bias is silent.

**New:** `Window = max(MaxVerdictAgeTicks, LastCompletedCycleTicks + LostAfterTicks(8))`. `Margin` **reuses an existing constant** — `LostAfterTicks` already means "how long a batch may legitimately be in flight". Zero-until-first-close ⇒ unchanged behaviour, and WaitCensus already defers the first fire. The knob keeps its name and becomes the **floor**. ⚠ `G139` applies: echo the **effective window**, not the knob.

**Per-fire line, Log verbosity, shared with E:**
```
Auto.Fire: census consulted=N eligible=E excluded=X fallback=F expired=Y unseen=U (window=W ticks) -> <id> on <target>
```
⚠ `Reason` is a `const TCHAR*` literal — cross-TU pooling is not guaranteed, so ⛔ **never compare it by pointer.** Instead **`FAnomalyCensusOpinion` gains `bExpired` / `bUnseen`** (additive struct fields, no enum change). The fire-report callback widens to `void(int32 Consulted, int32 Fallback, int32 Unseen)`; `NoteFireAllFallback` → `NoteFire(...)`.

| file | change |
|---|---|
| `AnomalyCensus.h` | `+LastCompletedCycleTicks`, `+FiresPartialFallback`, `+FiresUnseenCandidates`, `+HostPpCustomDepthReaders`, `NoteFire(...)`, `GetFreshnessWindowTicks()` |
| `AnomalyCensus.cpp:298-305 / :695-760 / :189-198` | window expression + `bExpired`; store cycle ticks, `CYCLE n DONE` gains `window=`; SUMMARY gains the counters |
| `AnomalyCensusProvider.h` | `+bExpired`, `+bUnseen`; widened `FAnomalyCensusFireReportFn` |
| `AnomalyAutoInjectorSubsystem.cpp:274-331` | count, emit the Log line, call the widened report |
| `AnomalyLabelWriter.{h,cpp}` | 3 new `run_summary` fields |
| `AnomalyCaptureSubsystem.cpp:958-979`, `:4240-4260` | help: **floor of the window**, not a fixed age |

**Gates.** **D-G1:** bench-only `IAI.Bench.CensusBatchCap <n>` stretches a cycle to ≈30 ticks. 🚨 **The A-side needs the old semantics, and the lever is `m41` code — that is `m40`'s L2 problem, which cost an unarchived intermediate binary. Do not repeat it:** ship a paired bench-only `IAI.Bench.CensusFixedExpiry <0|1>` so **A and B run on ONE binary.** *Prediction:* A-side `expired>0` with the pool skewed late; B-side `expired=0`, `window ≈ cycleTicks+8`; the `consulted=` line on **every** fire in both. **D-G2:** standard leg, no lever → *prediction* `window=12`, `expired=0`, otherwise identical to A-G1's control — **byte-inert where the cycle is short**, which is every host measured so far.

---

## E — PREFILTER ⊇ FIRE-VISIBLE-SET

**Source finding: the predicates are not mismatched.** Prefilter = renderable ∧ poll ∧ frustum (`AnomalyViewport.cpp:864-909`); fire adds ∧ `IsUnoccluded` and then the union's screen coverage (`:450-467`, `:255-268`). Same `IsRenderableComponent` `G33` chokepoint, same `ResolvePollOrigin`, same `GPollRadius`, and `BuildFrustum:183-188` is byte-identical to the inline construction at `:845-847`. ⇒ **at the same tick and view, fire-visible ⊆ prefilter, structurally.**

🔑 **So "unseen" is TEMPORAL, not a bug.** `Entries` is rebuilt from a prefilter snapshot at `StartCycle` (`:385`); an actor entering the frustum after cycle start has **no entry**, and `QueryActor`'s `!Found` branch (`:257-262`) says `"not_yet_measured"` — **today indistinguishable from "entry exists, measurement pending".** That indistinguishability is the defect. Two other routes named: `Prefiltered.Num()==0` (no view) makes **everything** unseen; `HeldElsewhere` **creates** an entry and correctly reads `not_yet_measured` — which is exactly why the reasons must be separate.

**Change:** `!Found` sets `Reason = "unseen"`, `bUnseen = true`. **The decision does not change** — still `FallbackBounds`; only its explanation.

⛔ **Do NOT "align the two functions" by adding occlusion/coverage to the prefilter.** That shrinks the census's measured set to exactly what bounds-based selection already trusts — defeating its purpose and blinding it to an actor that becomes unoccluded between cycles. **The prefilter being wider is correct by design.** Stated because "align the two functions" is the brief's phrasing and the aligned direction is the wrong one.

**Gates.** **E-G1:** two standard bench legs → *prediction* **`census_fires_unseen_candidates = 0`** (settled camera, static level). A non-zero is a **finding** — report verbatim, do not adjust. **E-G2 (required, `G96`):** bench-only `IAI.Bench.CensusDropEntry <n>` omits every *n*th actor from the prefilter → *prediction* at `n=2`, `unseen ≈ half the consulted set`. ⛔ **"0 on the bench, detector unproven" is not a pass** — this counter's whole job is to be believed when it says zero on a client host.

---

## F — VERDICT: **`m42`, not `m41`**

**Premise CONFIRMED from 5.1 source:** `SetRenderCustomDepth` → `MarkRenderStateDirty()` and **only when the value changes** (`PrimitiveComponent.cpp:4075-4082`) = full recreate; `SetCustomDepthStencilValue` → `SceneProxy->SetCustomDepthStencilValue_GameThread` (`:4084-4097`) → one scalar written on the render thread (`PrimitiveSceneProxy.cpp:964-984`), **no recreate.** Rotating values is genuinely cheap.

🚨 **But the stated CORRECTNESS motivation does not survive the same read.** Previous-transform state lives in **`FSceneVelocityData`**, keyed by **`FPrimitiveComponentId`** (stable across a recreate), whose own class comment reads *"Tracks primitive transforms so they will be persistent across rendering state recreates."* (`ScenePrivate.h:2410-2413`). `UpdateTransform` is `check(Proxy->HasDynamicTransform())` — most candidates are **static** and have no velocity to lose. And `m26`'s banked `F-1` established the deferred recreate is flushed **inside the same frame's** `BeginRenderingViewFamilies`, so the primitive is not missing from the frame.
⇒ **The named mechanism is NOT SUPPORTED.** ⛔ Not refuted as a *symptom* — GPU-Scene / static-draw-list churn is real and its per-frame pixel effect has **never been measured here.** Per `G120`: the concern stands as **unmeasured**; the mechanism does **not** stand as stated.

**Why `m42`, in order of weight:**
1. **Its justification is now unmeasured** — building a tag-lifetime redesign on it is the `ReservedStencilMax` mistake again. **`m42`'s first task is a MEASUREMENT** (census-ON vs census-ON-with-flips-suppressed, one binary, matched pose, grid/luma read under `G125` marker discipline).
2. **It reverses a closed ruling** (m36 tag-lifetime). Persistent tags make `IsAnyComponentTagged` — the `HeldElsewhere` guard at `:90` and the arm-time skip at `:648` — always-true, and the event mask always collides with a live census tag. That is an allocator redesign.
3. **It makes C's hazard permanent instead of intermittent** — ~60 actors writing custom depth every frame. Parking at 0 hides them from our mask (`Stencil < ReservedBase → 0`, `AnomalyVisibleMask.usf:26-30`) but **not from the custom-depth buffer**, which is what a host PP reads. ⇒ **C must land and read `=0` on the target host before F is safe to consider** — an ordering *reason*, not a preference.
4. `m41` already moves the client default plus three behaviours.

**`m41` carries from F: nothing in code, one docs correction** so the next reader does not build on a refuted mechanism.

---

## G — BATES, CARD **SECTION E**

**E-0 pass conditions, pre-declared:** (a) echo reads census **ON** from **`COMPILED DEFAULT (on)`** + mask **ON**; (b) the `HOST-PP` line present **with non-zero scanned counts**; (c) `aboveCeiling ≥ 1` naming the landscape actor; (d) the fog card in `NOT-MEASURED` as **`EXCLUDED(translucent)`**; (e) a `census consulted=` line on **every** fire; (f) eye list ≥ m36-leg-2's ~90 % visible, no 2–3-target repetition, no pitch-black frames.

⚠ **(c) and (d) can come back different and still be results.** `aboveCeiling=0` ⇒ the landscape's coverage moved (different letterbox) — report the histogram verbatim. Fog card `MEASURED_NONZERO` ⇒ its material is not translucent-blend (likely Masked) — **a finding that changes B's scope, not a failed gate.**

**E-1** pull + rebuild the **EDITOR** target (§8.6 STEP 3.5 not optional). **E-2** `blinking` **TICKED**; **`IAI.Capture.RunLog 1` FIRST** (Bates runs delivery mode ⇒ `m38`'s log is auto-OFF, `G210`) so the read is in `anomaly_log.txt`, not scrollback; `Config 2 4 8 4 0`; `Start "" png 4242 90`. **E-3** the two effective-for-this-run lines · the `HOST-PP` line · one settled cycle's `DONE`+histogram+`ABOVE-CEILING`+`NOT-MEASURED` · every `census consulted=` line · the 15 `census_*` keys · **the eye list (target name + visible yes/no), written during the run.** **E-4** ⛔ **do not type `IAI.Capture.Census` or `IAI.Capture.Mask`** — the leg's entire purpose is that the *compiled defaults* do it. **E-5** D-5's ceiling read is absorbed into E-3, D-5 marked superseded.

**Predictions:** eligible set between m36-leg-2's ~8 and that minus the fog card and the landscape actor; `vetoed_events` **0**; **`census_fires_unseen_candidates > 0` on Bates** (the PIE camera moves) — the counter's first real reading, **expected, not a fault**; `census_fires_partial_fallback > 0` likewise.

---

## Order & commit shape

**A → B → C → D → E**, F deferred, G written with A+B and updated after C–E.
1. `docs(m41)` predictions file + journal (**journal already landed as `47acfe6`**; the predictions file lands with the implementation brief, before any source).
2. `feat(census): m41 - census on by default (+ translucent rule, host-PP preflight, cycle-relative expiry, coverage assertion)` — ⚠ the parenthetical **drops "tag persistence"**.
3. `docs` — CLAUDE.md · client-delivery.md · ledger · card Section E · gotchas.
**No tags.**

Likely new gotchas, named now: (i) an ini key duplicating the compiled default becomes a *provenance readout*, not a correctness dependency — say so or it gets "cleaned up"; (ii) `UsedSceneTextures` is a serialized `LAYOUT_FIELD` ⇒ scene-texture usage **is** readable in a cook; (iii) a "requested but inactive" warning becomes noise the moment the requested state is the compiled default.

---

## NEEDS-DECISION

1. **Mask compiled default — flip it too (recommended) or census only?** Census-ON beside mask-OFF makes the `:1502` warning fire on **every run of the shipped default**. Flipping also removes the `G139` failure structurally: today a lost ini key silently restores `m25` labelling. Fallback if refused: flip census only + gate that warning to ini/console-requested cases.
2. **B's fixture** — bench runtime spawn `IAI.Bench.SpawnTranslucentProbe` (recommended; `CB_GateLevel` is frozen, `G99`) vs a sibling level vs Bates-only.
3. **C-G1b's fixture** — host-project bench content (**needs a cook**; recommended) vs deferring the positive direction to Bates. C-G1a covers the mechanism either way.
4. 🚨 **D's "absolute cap" has two readings and one undoes the fix.** As an *upper bound* the window clamps back to 12 on the very host it exists for. **Recommend `Window = max(knob, lastCycleTicks + 8)` (knob = floor), with the existing `[0,600]` clamp as a runaway ceiling.**
5. **`census_fires_unseen_candidates`** — fire count (recommended: consistent `census_fires_*` trio, candidate count in the log line) or candidate count.

**STOP.** Implementation is the next brief after approval.
api-equivalent cost (not billed on Max): $11.10 | turns: 54 | running total across all briefs: $103.11
