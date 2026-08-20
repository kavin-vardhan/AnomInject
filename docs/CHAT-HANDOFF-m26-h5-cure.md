# Chat handoff — m26: the H5 cure (zero-only veto), tagged

**Session date:** 2026-08-20
**Plugin repo:** `AnomalyInjector` — HEAD `d6bee7a`, clean, pushed. **Tag `m26` at HEAD, pushed.**
**Bench repo:** `CaptureBench` — `f51ed51`, clean, local-only.
**Audience:** a cold reader — fresh chat, Claude Code, or a collaborator.

**Read the earlier set if you have not:** `CHAT-HANDOFF-s2-*` (three docs) → `CHAT-HANDOFF-s3-m24-capture-migration.md`.
**This doc is the fifth in the set and supersedes their status lines.** It covers the arc from m25 to m26 — H4, H5, the MainWorld cook, and the cure.

**One-line status:** the client's #1 complaint now has a **named mechanism (H5), a working pixel measurement, and a shipped partial cure**. m26 deletes labels whose target drew **zero** pixels. It deliberately does **not** delete over-claiming labels, and it is **inert on Nanite geometry**. P1 is still unreproduced. H4 is still uncured.

---

## 1. What this arc did, in one paragraph

The session opened intending to test **H4** (occluded targets labelled positive). H4 was **confirmed as a mechanism** in a lab condition, then the owner inspected the client's real output and named two culprit classes that were **not occlusion** — instanced meshes and a volumetric fog volume. That opened **H5**: *the selector admits objects that cannot manifest a visible hide*. H5 class (ii) (aggregate/oversized bounds) was reproduced in MainWorld at shipping defaults. To reach MainWorld at all the project had to **re-cook** (MainWorld had never been packaged — the "active redirect" story in G87 was wrong). The cure was then designed from measurement rather than intuition: five candidate instruments costed, four rejected, and the surviving one (a stencil mask counting the target's actually-drawn pixels) built across three gated slices. **m26 is tagged.**

---

## 2. Current state

| | |
|---|---|
| `AnomalyInjector` HEAD | `d6bee7a` — clean, pushed, 0 unpushed |
| **Milestone tag** | **`m26` at HEAD, pushed** |
| `CaptureBench` | `f51ed51`, clean, local-only |
| Session bank | `D:\IntrusiveAnomalies\_bench_sessions_bank` — ~160 dirs |
| Staged bench build | exe `5EA6AB92` + container `utoc 9334496D / ucas 62EB0072 / pak 78C977A5`, **4 maps** |
| Preserved quartets | `_binary_baselines\m25-h4h5m1-measurement-build\` (complete, 6 files) and `m26-slice1-measurement-build\` |
| `feature/stencil-capture` | `76cac74` — **UNTOUCHED, never checked out. Mined read-only, not resumed.** |

**Build identity is a QUARTET (G121):** exe + utoc + ucas + pak. An exe hash alone identifies half a build — the m26 cook produced an *identical exe hash* with a different container. `.m25-baseline` is exe-only and does **not** reconstruct that build; its pak is gone. The loss is bounded (the debt sweep was empty).

**Staged bench exe `5EA6AB92` is the tagged source MINUS one log-wording commit (`49d1c7a`).** Deliberate: `5EA6AB92` is the exact binary all nine slice-3 gate legs ran on, and swapping it for an ungated one to fix a log string is the worse trade.

**Disk:** `StackOBot\Intermediate` and `Saved` were moved to `E:\IA_BuildCache\` and **junctioned** back. Every hardcoded `D:` path still works; zero edits were needed. The engine's own `Intermediate` (~60 GB) is still on D: and is the remaining swing. **The full E: migration is SUSPENDED, not cancelled** — the junction solved the pressure.

---

## 3. What m26 is, precisely

**The mechanism (H5):** `IsRenderableComponent` checks two things — `IsVisible()` (plus instance count for ISM) and `IsA<UStaticMeshComponent>() || IsA<USkinnedMeshComponent>()`. That is **renderable TYPE, flagged visible**, which is strictly weaker than **draws pixels**. Absent: the owner actor's `bHidden`, `bRenderInMainPass`, a null `StaticMesh`, section/triangle count, material presence, `WasRecentlyRendered()`, and **any distinction between ISM/HISM/Foliage and a plain SMC** — all of which inherit from `UStaticMeshComponent` and pass trivially. Every companion predicate (frustum, poll radius, `IsUnoccluded`, coverage) is computed on **bounds**. **Not one reads a pixel, a material or a draw call.**

**The cure:** a stencil mask tags the target, the renderer resolves occlusion/visibility naturally, and the surviving tagged pixels are counted. If an event's target is **measured** and the count is **zero**, the event is removed from `annotation.json` before it is written.

**The three slices:**
1. **Measure** (log only) — mask, tagging, reduction, LOCK-1 timing.
2. **Report** — `mask.provided` carries the tri-state's bool. A *value* change in a slot that already shipped.
3. **Veto** — deferred invalidation in `FinishRun`, between `DrainAsyncToCompletion` and `WriteSessionAnnotationFile`.

**The safety property, and it is structural not careful:** `MaskStateProvidesMeasurement` and `MaskStateVetoes` switch on the **state enum alone**. `MaxCount` is never read by the veto and never emitted. There is no code path on which a magnitude can move an event between the two zeros.

```
NOT_MEASURED      -> mask.provided false -> NEVER vetoed. MUST ADMIT.
MEASURED_ZERO     -> mask.provided true  -> vetoed (if manifested).
MEASURED_NONZERO  -> mask.provided true  -> never vetoed, at ANY magnitude.
```

**`run_summary` is +4 since m25:** `mask_probe_arms`, `mask_residual_discards`, `mask_nopass_discards`, `vetoed_events`. **`annotation.json`'s field SET is unchanged** — measured on every leg. **P6 did not move.**

---

## 4. Decisions made this session, with rationale

### 4.1 The veto rule is ZERO-ONLY. The ratio was refused. (chat ruling)

The four measured targets separate cleanly on drawn ÷ claimed:

| target | drew | claimed | ratio |
|---|---|---|---|
| Cube `StaticMeshActor_49` (good) | ~66,800 px | 71,864 px | **0.93** |
| Cylinder `StaticMeshActor_73` (good) | ~48,590 px | 63,296 px | **0.77** |
| `InstancedFoliageActor_0_0_0` (bad) | 5,689–13,342 px | 921,600 px | **0.014** |
| `BP_SplineSpawn_C` (bad) | **0 px** | 35,535 → 210,921 px | **0.000** |

A 55× gap. **Refused anyway.** Both GOOD targets are **convex primitives viewed head-on** (`/Engine/BasicShapes` Cube and Cylinder) — which is *why* they score high. A legitimate target with a **complex silhouette** (fence, railing, ladder, grate, sparse foliage) can draw a small fraction of its bounding rect while being fully visible and fully valid. **No such target exists in the measured set.** Calibrating a threshold on four points, all simple convex shapes, is **G135's exact failure**: a calibration set that cannot exhibit the case that would break the rule, with the blindness presenting as a clean pass.

A count of **zero** needs no calibration. A target contributing not one pixel cannot be the visible anomaly its label claims, whatever its silhouette.

**The accepted cost, stated not buried:** the foliage — 1.4% of a whole-frame claim — **ships as a valid label**. m26 is a **partial** cure for H5.

### 4.2 The shader module: Option B (a dedicated `PostConfigInit` module)

A new global shader must register before engine init completes, and `AnomalyCapture` loads at `Default` and depends on `AnomalyInjector`. Rather than move the whole plugin early (option A — 25 milestones of behaviour sit downstream of that order), a minimal `AnomalyShaders` module carries **only** the shader declaration and directory mapping, at `PostConfigInit`, with deps `Core/Engine/RenderCore/RHI` and **no Renderer**. `AnomalyCapture` and `AnomalyInjector` keep `Default` and their order is untouched.

### 4.3 `RQT_Occlusion` was disqualified on CORRECTNESS, not cost

The engine's hardware pixel counter rasterises the primitive's **bounding box**, not its mesh. For an over-claiming target it returns a large, confident, wrong number — it would **actively confirm the false claim**, not merely fail to catch it. This is settled and does not get reopened for convenience.

### 4.4 `SM_Ramp2` retired as a control, repurposed as the KNOWN-NANITE control

It failed F-6 item 2 by producing a clean `MEASURED_ZERO` on a target we had independently measured as drawing. Cause: Nanite (§4.5). An instrument cannot be validated against a target it cannot see. It now must read `NOT_MEASURED` **every time** — a *positive* test that the instrument recognises what it cannot see, and the place a future engine bump adding Nanite custom-depth support would show up first.

### 4.5 The Nanite limit, and the entanglement finding

`Nanite::FSceneProxy::GetViewRelevance` **never** sets `bRenderCustomDepth` (both branches read); `bHasCustomDepthPrimitives` rises only from that flag; the 5.1 custom-depth pass has **zero** Nanite references. A Nanite target is **selectable, taggable, verifiable — and permanently unmeasurable by this cure on UE 5.1.**

**The entanglement is the harder half, and it was measured on StackOBot rather than projected:** authored structural geometry — walls, floors, platforms, pillars, pipes, fences, crates, doors, ramps — is overwhelmingly Nanite, while foliage and simple planes are not. **The two H5 instances are reachable because of what they happen to be made of, not because H5 favours measurable geometry.** On a Nanite-heavy host title the cure is inert for most authored geometry. Those targets are always **admitted**, never vetoed — safe, and also the cure not working there.

**Scene depth IS Nanite-inclusive on 5.1** (answered from source: `Nanite::EmitDepthTargets`). A path exists where the mask has none. **But the parked depth work (C-2) still addresses H5 class (ii) not at all**, because its reference depth comes from the same bounds H5 calls untrustworthy. Recorded as scoping, not a licence.

### 4.6 G-9 closed by pose-matching (route a), not by widening the baseline (route b)

Two routes existed. Route (b) — widen the run-unique set so the untested axis is covered — was refused with the sharper argument: **it means re-running legs until the baseline widens enough to excuse the difference being cleared. That is the laundering shape, even when every individual step is legitimate.** Route (a) makes the difference *vanish* rather than *excused*. The run-unique set went 26 → 4 with **no pose field in it at all**, so no pose difference could have been excused even if one had occurred. **The gate got harder when it closed.**

### 4.7 The A35 ruling

`BP_SplineSpawn_C`'s banked hide showed a small in-bbox luma change (0.0175) while the mask reads exactly zero. A zero-silhouette target can still have indirect visual effect (shadow, GI). **m26 vetoes it anyway**, because the label points at the object and not at its shadow. That is a ruling with its reason, not an oversight.

### 4.8 The probe: a guard that fires on demand

`IAI.Capture.MaskProbe` deliberately arms **one** measurement on a known-hidden tick. It proves the 255 detector, the enforcing confirmation and the frame-scoped discard are **all live**, in one frame, with the admit bias disposing of it safely. Default OFF, **inert in delivery mode by a guard at the fire site**, `PROBE` in every log line, artifact-attributable via `mask_probe_arms`, and on the pre-delivery checklist.

### 4.9 Staging: measure → report → veto

Deliberate ordering. **The cure had to be shown to measure correctly before it was given the power to act on the measurement.** Slice 1 alone took eleven parts and exposed five faults; had the veto shipped alongside it, the false `MEASURED_ZERO` on `SM_Ramp2` would have deleted good events.

---

## 5. The forward plan

### 5.1 🚨 FIRST: the `RoomBuilderSquare_C` observation — it may reframe what m26 does

In the owner's play-gate smoke, the auto-pool run vetoed **4 of 8 events**, and the targets were:

- `BP_Stomper_C` ×2 (a **moving** platform)
- `RoomBuilderSquare_C` ×2

**`RoomBuilderSquare_C` is on record as a LEGITIMATE, DRAWING target** — Part Eleven's cure table put it in the GOOD group (whole-frame Δ 0.04413, peak-IN/OUT 0.2030/0.0027, a 75:1 in/out ratio), and Part Twelve explicitly named it as *the cost* of the rejected blacklist rule.

It measured **zero** in that view and was deleted.

**This is most likely correct behaviour under view-dependence** (§6.3) — if it drew zero pixels in that window, the label pointed at nothing visible. But it means **the cure's real scope is broader than "H5"**: it deletes *any* event where the target drew nothing in that view, which includes occluded targets (H4-shaped), off-screen targets, and moving targets that left frame. **That reframing has not been written into the tag or the docs.**

It also touches **incidence**, which the project has never claimed: in one auto-pool run in real gameplay, **half the labels pointed at targets drawing zero pixels**. n=1, PIE not packaged, one level, one camera path — but it is the first data on how often this happens in something resembling a real capture.

**Recommended first task for a fresh chat:** characterise this. Was `RoomBuilderSquare_C` off-screen, occluded, or genuinely non-drawing at that moment? A repeat auto-pool run with per-event `framesNoPass` / provenance / bbox reported would settle it cheaply. **Do not change the veto rule on the strength of it.**

### 5.2 Then, in order

1. **The over-claim rule (m27 candidate).** Requires a calibration campaign **including complex-silhouette legitimate targets**, which do not exist in the current measured set. Named, not guessed. This is what closes the other half of H5.
2. **`labels.jsonl` vs `annotation.json` (L3).** See §6.1. Accepted as a limit; a fix is its own change with its own gates.
3. **Does the m26 mask also catch H4?** The shared-cure question is now *testable* rather than speculative — point the finished mask at a known-occluded target. If it catches it, H4 closes as a side effect. **Still a hypothesis, deliberately not claimed.**
4. **H5 class (i)** — the non-drawing mesh component. **ENUMERATED, NOT OBSERVED.** Two candidate rounds in StackOBot returned "selected but manifests". StackOBot is a polished sample and may not contain the pattern; that is a property of *this project*, not evidence against class (i) in the client's game.
5. **P1's H1 lever** (GPU-load starvation shape) — P1's **only** remaining named lead, and it has no lever. Design chat-side first, never same-turn as its first measurement.
6. **P5 / P7 blend-ladder** — open, adjacent, not merged. Discriminators pre-declared chat-side before it is built.
7. **B2** (scale-free separability), **B1-NDC** (resolution-relative `CALIB_BBOX`), **P6 implementation**, A17/A19 audit, resolution selection / JPEG / defaults profile.

---

## 6. Corrections — discard this stale understanding

### 6.1 `labels.jsonl` and `annotation.json` now disagree, inside one folder

L3 was recorded as *"delivery OFF and ON will disagree"*. The live form is sharper: **in a single delivery-OFF session folder, `annotation.json` and `labels.jsonl` disagree on event content.** A fully-vetoed session ships an empty `anomalies` array beside **59 label rows asserting `anomaly_present` and `visible_positive`**. The veto edits the in-memory accumulator; `labels.jsonl` is prebuilt per frame and cannot be corrected.

**No client impact** — delivery mode does not write `labels.jsonl`. **Owner-side tooling IS affected:** `overlay_watcher.py` → `tools/verify_capture.py` reads `labels.jsonl` and draws the boxes; three copies of `overlay_watcher.py` exist. The **dashboard is not affected** (verified by absence, it does not read `labels.jsonl`).

### 6.2 Other superseded understandings

- **"MainWorld is actively redirected away"** → **false.** MainWorld was **never cooked** into any build; the engine simply falls back to `GameDefaultMap` when a map is absent. G87's headline rule ("check the level NAME, not the picture") survives; its *explanation* was invented and sat in the record for two weeks.
- **"The exe hash identifies the build"** → **no.** G121: build identity is a quartet. The m26 cook produced an identical exe hash with a different container, a different secret and one more map.
- **"Partial occlusion is the right target"** → **withdrawn.** The owner's inspection of client output showed the anomalies were not partially hidden — they could not be found at all.
- **"`framesNoPass` is a Nanite counter"** → **no.** It counts frames where the custom-depth pass did not produce for this target. Nanite is one cause; **frustum culling reaches the same mechanism**. In all cases the frame is discarded and the event tends toward `NOT_MEASURED`, which **admits**.
- **"`asset_name` / `component_class` degrade in packaged builds"** → **no.** 15/15 populated across 109 banked legs, 1,267 node entries, zero empty; no `WITH_EDITOR` branch anywhere. What differs is `node.name` = `AActor::GetName()`, the internal object name, **not** the editor label — a script-spawned actor reads `StaticMeshActor_<n>` in both editor and build. **Not a field-population defect.**
- **"The tag/arm separation fixes the stale read"** → **withdrawn**, refuted from source: `MarkForNeededEndOfFrameRecreate` is flushed inside `BeginRenderingViewFamilies` in the **same** frame. Zero ticks are needed, and it is a guarantee.
- **"Moving `ReservedStencilMax` fixes the 255s"** → **refuted, and the repair would have been worse**: every read would still return 255, but 255 would then fall outside the range, the detector would go **silent**, and every event would return a clean-looking `MEASURED_ZERO` — vetoing everything.
- **"Journal 031's `532/534`"** → arithmetic slip. True corpus **540**, eight exceptions not two. The propagated `1052/1054` is **flagged, not corrected** (its render half was never re-measured).
- **"`node.bounds` is roughly the object"** → no. It is the **whole actor**: 42 m × 26 m × 17 m centred ~2.2 km away for `BP_MovingPlatform`; 252 m × 217 m × 67 m for the foliage. Labels are unaffected (`bbox_px` comes from the projector), but any consumer using `node.bounds` or `global_position` for geometry can be wrong by kilometres. **P6's second and third observations.**

### 6.3 View-dependence — the important reframing

`BP_SplineSpawn_C` measured **54,779 px** in the smoke's auto-pool run and **exactly zero** on the bench. That is the mask being view-dependent, which is correct: it reports drawn pixels **in that view**, not a fixed property of the asset.

> **"H5-shaped target" is a property of a TARGET IN A VIEW, not of a target.**

Nobody should read the two results as contradictory, and no target should be described as "an H5 target" without its view.

---

## 7. Open vs locked

### Locked — do not relitigate
- **The veto rule is ZERO-ONLY.** No ratio, no threshold, no constant that could become one.
- **`NOT_MEASURED` is never vetoed.** The admit bias is the safety argument.
- **`feature/stencil-capture` stays untouched at `76cac74`** — mined read-only. Its foliage blacklist, its `USkeletalMeshComponent` narrowing, `StencilViz` and the `bbox_norm` re-sourcing **do not survive** into the cure.
- **A class blacklist is NOT a fix.** The mechanism is *oversized bounds*; `BP_SpawnPad_C` is a plain `StaticMeshComponent` with a negative poll distance, and a blacklist would miss it while looking closed.
- **`RQT_Occlusion` is disqualified on correctness.**
- **P6 does not move.** `mask.provided` false→true is a VALUE change; sub-fields under `mask` are a SHAPE change and are out. `run_summary` is not P6.
- **`CB_GateLevel` is frozen** (G99). Its BasicShapes-only composition is why it is a stable instrument **and** why it could not have surfaced the Nanite blindness (G135).
- **Stencil range stays 200/255.**
- Ratio-independence; content-clock default = wall; `IAnomaly` locked since M1; no force-push.

### Open
- **P1** — unreproduced, unfixed. **H1 is the only named lead and it has no lever.**
- **H4** — SUPPORTED as a mechanism (path b) in a lab condition. **Path (a) PARKED, NOT REFUTED** — a priority decision, not a scope one. Re-opening needs a decision, not an argument.
- **H5 class (i)** — enumerated, not observed.
- **The over-claim half of H5 class (ii)** — the foliage still ships.
- **The Nanite blindness** — no cure on 5.1.
- **P5 / P7 / P6 / B2 / B1-NDC / A11 / A17-A19** — all as before.
- **G118 residue** — none; the token rotation shipped with the cook and is read back from the running build.

---

## 8. Working agreements — carry these verbatim

**Roles.** Kavin is tech lead and project owner. He does **not** write code and does **not** make technical decisions — both are fully delegated. **Chat-Claude makes all design and technical calls** and writes the exact paste-back blocks. **Claude Code implements** against approved plans. Kavin ferries messages, runs smoke tests, and applies executive judgement **only** on genuine product/scope tradeoffs.

**🔴 THE RED-CIRCLE RULE.** Mark with 🔴 every item he MUST read, and nothing else. Only three things earn it: **ACT**, **DECIDE** (a closed choice with a recommendation attached, never open-ended), **HEADS-UP** (something that changes what ships, or a risk accepted on his behalf). Marked items go at the **TOP** of the reply.

**📊 STATUS LINE (added this session).** Every reply opens with one line: milestone, rough % complete, whether things are going well, and turns remaining.

**Plain-language summaries are mandatory in BOTH directions, every exchange.** Lead with a scannable jargon-free rundown, then the verdict, then the paste-back block. End with what was proposed plus the **Claude Code effort setting**.

*Note: effort settings (`xhigh` / `high` / `max` / `ultracode`) are named for the Opus slider. If a different model is selected, check whether that line still maps to anything.*

**Discipline that has repeatedly paid.** Plan-before-code. Stage gates with concrete thresholds. **Stop-on-failure — no same-turn fixes to a validity instrument.** Predictions **pre-declared as a committed file** before the instrument exists, restated **verbatim** before results. **Measure then design.** Numbers never reused. Conventional Commits; one milestone one commit; Code commits **and** pushes including tags, reporting for the record not for approval. **Never force-push.**

---

## 9. Standing lessons, with this arc's receipts

- **Only known-answer controls expose instrument blindness.** The negative control (`SM_Ramp2`) caught a clean `MEASURED_ZERO` on a target we had measured as drawing — the false accusation, at full strength, through every guard built that turn.
- **A guard that has never fired is not a guard.** Hence the probe: a deliberate, flag-gated wrong arm that proves three mechanisms live in one frame.
- **The obvious repair was wrong twice, and source refuted both before they were built** — the stencil range, and the tag/arm separation. Establishing the mechanism first is not overhead; it is the method.
- **A missing check must never read as a passed check.** Whitelist polarity on the confirmation; the extent precondition as *positive evidence the pass ran*, never the absence of a warning. G133 is what happens otherwise.
- **A false FORECLOSURE is never re-run, by definition** (G120). An observation and its explanation are **separate claims**.
- **Build identity is a quartet** (G121). **A cook runs on editor binaries** (G131) — verify the artifact a cook consumes *before* the cook, not from its exit code.
- **A calibration environment built from a restricted asset set cannot exhibit defect classes outside that set, and the blindness presents as a clean pass** (G135).
- **Widening a baseline to excuse a difference is laundering, even when every step is legitimate.**

---

## 10. Pointers

**Plugin repo:** `CLAUDE.md` (current-status block, refreshed at m26) · `docs/invisible-anomaly-mechanisms.md` (**the ledger — read this second, after CLAUDE.md**) · `docs/gotchas.md` (**G116–G135 new**; G43, G47, G76, G86–G115 relevant) · `docs/sessions/2026-08-19-045-h4-cook-and-h5-mainworld-arc.md` (**32 parts, PART INDEX at the top — use it, do not read linearly**) · `docs/client-delivery.md` (the KNOWN LIMITATION section) · `docs/PRE-DELIVERY-CHECKLIST.md` · `docs/setup-runbook.md` §8.6 (the full-cook recipe) · `docs/architecture.md` · `docs/capture-fps.md`.

**CaptureBench (local-only):** `tools/a54_oracle.py` (**read its header — it states its own certified range**), `check_pose.py`, `run_leg.ps1`, `eval_leg.py`, `resolution_delta.py`, `verify_cooked_maps.ps1`, `prune_verify.ps1`, `p24_join.py`, `p29_g9_check.py`, `p31_veto_check.py`, `nanite_signature_scan.py`, `mainworld_instance_join.md`.

**Banked evidence:** `D:\IntrusiveAnomalies\_bench_sessions_bank` — the m26 gate legs are `P26_FIX2_*`, `P27_EXT_*`, `P28_*`, `P29_S2_*`, `P30_G9_*`, `P31_S3_*`. **Re-bank before any staging step (G92); match BY SESSION ID, never by directory name** — a name match once destroyed the only copy of a session while reporting a clean duplicate.
