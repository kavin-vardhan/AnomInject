# 2026-08-21 — session 051 — the label-offset instrument, the material usage flags, and the tick-mode pin

## §1 Goal and shape

Four briefs in one session, all under the Monday client-delivery clock. **NO MILESTONE WAS
OPENED AND NOTHING WAS TAGGED — `m31` is still untagged and still awaiting Concorde V-3/V-4.**
Six commits on the plugin, two on AnomDash, one cook. This journal is self-contained.

The through-line: the owner reported an annotation-timing offset visible on BOTH capture paths on
Concorde, so the disease was to be treated as living in the SHARED annotation timeline. The first
brief built the MEASURING INSTRUMENT for that, deliberately with no fix designed. The later briefs
closed two ship-visible defects found along the way and added the levers needed to act on Saturday's
measurement without a re-cook.

## §2 WHAT SHIPPED

| commit | what |
|---|---|
| `ea99d6b` | `tools/measure_label_offset.py` moved here from AnomDash |
| `d3f3152` | material usage flags on both shipped materials + the headless setter |
| `98d04d4` | measurement-ceiling banner and `--require-gap` |
| `f999d7a` | capture-time engine tick-mode pin |
| `b4e07e0` | tick-mode pin — console override |
| `a3aa1e6` | ini + console defaults for auto-pool anomaly parameters |

AnomDash: `f79a40c` (script, later moved away) then `7922457` (removal + README revert).

⚖ **STANDING RULE ESTABLISHED: INTERNAL diagnostics ride the PLUGIN repo's `tools/`, beside
`verify_capture.py`. CLIENT-SHIPPED host tooling stays in AnomDash `host-tools/`** (the
`encode_watcher.py` precedent). The office box pulls the plugin repo; requiring a second clone on a
sealed box under a deadline is a failure mode we are not buying. A future client-facing label
refiner would go to AnomDash, not here.

## §3 THE MEASUREMENT INSTRUMENT — `tools/measure_label_offset.py`

Read-only. Per annotated event it measures WHERE THE ANOMALY MANIFESTS IN PIXELS against WHERE
`annotation.json` CLAIMS IT IS, and prints a per-type offset table plus a per-event CSV. Python 3 +
Pillow only. It never writes into a session directory. Full method, every constant and every limit
are in the script's own module docstring — that is the reference, not this section.

Two conventions it prints in its own output header, so a screen photo carries them:
**the PNG filename index IS the session index and it is 0-BASED**, and
**offset = MANIFESTED − ANNOTATED, positive = pixels LAG the label.**

🚨 **THE FINDING THAT MATTERS MOST IS A CEILING, NOT A NUMBER.** The instrument derives its baseline
from frames the ANNOTATION calls clean. If the annotation is offset by more than the clean gap
between bursts, some of those reference frames are anomalous in the pixels, and THE FIRST VERSION
REPORTED A CONFIDENT, WRONG, UNDER-READ OFFSET — several events read "+0" when the truth was ±3.
The instrument now detects it: any reference frame whose own net signal exceeds the threshold is
counted, the row drops to LOW confidence, and the type line prints
`*** BASELINE CONTAMINATED ***`. The session header additionally prints
`CEILING *** MEASURABLE RANGE +/-N frames (min clean gap G) — offsets beyond this are UNDER-READ,
not absent ***` with `N = G // 2`, and `--require-gap N` exits nonzero so a badly-configured office
run announces itself BEFORE anyone reads numbers off a screen.

**N = G//2 is structural, not tuned:** the tool attributes each frame to the nearer event using the
MIDPOINT between adjacent windows, so an offset past half the gap is credited to the neighbour.

### §3.1 Gate results

- **G-C classifier sanity — PASS 10/10**, three checker cell sizes and four negatives. Shipped as
  `--selftest` so it is re-runnable on the office box, where a different Pillow build is the one
  thing that could silently change the detectors.
- **G-B trusted sessions, unmodified** (CB_GateLevel, camera rotation (0,0,0), `speed_ratio`
  1.00000): blink 7/7 shift **+0**; missing_texture 8/8 startΔ/endΔ **+0/+0**; missing_object 7/7
  shift **+0**. Median |offset| = 0 on every type.
- **G-A linearity, synthetic controlled session** (sparse duty cycle, 18 clean frames between
  windows), four types × three shift variants, **EXACT ON ALL TWELVE**: baseline +0; annotations +3
  reads −3; annotations −3 reads +3; pixels +3 reads +3.
- **G-A linearity on real banked bench sessions at ±1** — the largest shift the 8-of-12 duty cycle
  admits without contaminating the baseline: **nine fixtures, every one exactly ±1**, including the
  hide-type set-alignment path.

⚠ **±3 ON A REAL BENCH SESSION IS NOT ACHIEVABLE AND NO PASS WAS MANUFACTURED FOR IT.** On the
standard `2 4 8 4 0` config the clean gap is 4–5, so a ±3 shift necessarily contaminates the
baseline. The banner fires; the numbers are reported as they printed.

### §3.2 Two detectors that passed synthetic and FAILED on real pixels

Recorded because the failure shape is the lesson, not the fix.

- **MAGENTA.** An absolute-brightness test (`min(R,B) > 128`) sailed through a saturated synthetic
  patch and MISSED real magenta frames whose lit pixels sat at 0.203 and 0.046 of the region. The
  shipped test is RELATIVE (`G < 0.60·min(R,B)`, `|R−B| < 0.40·max(R,B)`) and reads the same real
  frames at 0.373 / 0.198 while reading 0.000 on checkered ones.
- **CHECKER.** The absolute route (bimodal + edge + achromatic) FAILED on a real case for an
  instructive reason: a bbox holding a DARK OBJECT ON A BRIGHT BACKGROUND is strongly bimodal
  whether or not a checker is present, so the anomalous and clean frames were nearly identical
  (sep 0.739 vs 0.720, conc 0.914 vs 0.943). The only quantity that moved was edge energy,
  0.0081 → 0.0108. A DIFFERENTIAL route against the baseline frame's own edge energy was added and
  is what catches it.

Both were calibrated against pixels that were looked at, not against the synthetic. → **G155**.

### §3.3 Delivery mode costs the instrument its region — MEASURED

Built delivery-mode fixtures (annotation + frames, `labels.jsonl` deleted) from sessions whose true
offset is KNOWN, so this is a calibration rather than an impression. With delivery ON there is no
bbox: the region degrades to FULLFRAME and the ambient ring is gone, so the one mechanism that
rejects camera motion is unavailable and a small object's change is diluted across the whole frame.
Contrast collapses about fifteenfold (peak/T 33.9 → 2.3).

**The damage is loss of CORRECTNESS, not merely of confidence:**

| fixture | true offset | delivery-mode reading |
|---|---|---|
| blink | +0 | median +0 but per-event range +0..+7 |
| missing_texture | +0 | **median startΔ +6 — WRONG**, half the events unmeasurable |
| missing_texture | +1 | **ALL EIGHT EVENTS UNMEASURABLE** |
| missing_object | +1 | median +1, but only 2 of 8 events survive |

⇒ **AN OFFSET OF 1–6 FRAMES IS NOT RELIABLY DETECTABLE IN DELIVERY MODE. THE CLIENT'S SHIPPED
CONFIG CANNOT SELF-VERIFY; VERIFICATION IS ALWAYS A DELIVERY-OFF EXERCISE.** → **G156**.

## §4 MATERIAL USAGE FLAGS — a ship-visible defect, confirmed on Concorde

Preserved Concorde logs carry, for both shipped materials, the engine warning that
`bUsedWithStaticLighting` is missing and the default material will be used. The owner's control run
showed the consequence: with only `corrupted_texture` in the auto pool, the held weapon rendered
MAGENTA while other objects rendered the engine default grid. **ONE ANOMALY TYPE, TWO APPEARANCES.**

Engine path, `StaticMeshRender.cpp:2225` — a static mesh section with `bHasSurfaceStaticLighting`
whose material fails `CheckMaterialUsage_Concurrent(MATUSAGE_StaticLighting)` draws the DEFAULT
MATERIAL. The held weapon is SKELETAL and `MATUSAGE_SkeletalMesh` was already set, so it drew our
magenta. Same material, two component types, two outcomes.

**Measured on our own assets, not inferred:** both materials read
`used_with_static_lighting = False` before the fix (and `used_with_clothing = False`).

🚨 **WHY THIS NEVER FIRED AT HOME, so nobody re-derives it: StackOBot sets
`r.AllowStaticLighting=False` in `DefaultEngine.ini`.** No primitive here carries a lightmap,
`bHasSurfaceStaticLighting` is false everywhere, and the usage is never queried. Eleven packaged home
logs contain ZERO `LogMaterial` lines while a dozen other categories log freely and no suppression is
configured — the absence is NEVER-FIRED, not FILTERED. ⇒ **THIS BOX STRUCTURALLY CANNOT REPRODUCE OR
VERIFY THIS DEFECT.** → **G157**.

### §4.1 The seven flags, and why exactly these

The reachable component universe was read from our own source, not guessed.
`AnomalyLod::ResolveLodComponents` — the only route by which either anomaly's `Apply` reaches a
component — collects exactly `UStaticMeshComponent` and `USkinnedMeshComponent`, and
`AnomalyViewport::IsRenderableComponent` ends in `IsA<>` of those two. Derivations verified against
engine headers: ISM/HISM and SplineMesh from the first; Skeletal and Poseable from the second.

**INCLUDED (7):** StaticLighting (measured missing) · SkeletalMesh · InstancedStaticMeshes (ISM/HISM
pass the filter; only foliage ACTORS are excluded, m27) · SplineMeshes · MorphTargets · Nanite (a
HOST setting must not decide our correctness) · **Clothing (NEW** — a skeletal target may have cloth
sections; simply not hit in the runs that produced the warnings).

**EXCLUDED, each UNREACHABLE rather than merely unlikely:** every particle and Niagara usage (not
static/skinned, and G33 removed `UFXSystemComponent` from the renderable-visible set);
GeometryCollections and GeometryCache (both `: UMeshComponent`, verified, so `ResolveLodComponents`
never returns them); Water, HairStrands, LidarPointCloud, VirtualHeightfieldMesh; EditorCompositing.
A usage is only ever queried by the component DRAWING the material, so a flag for an unreachable type
can never be consulted.

⚠ **IF `IsRenderableComponent` OR `ResolveLodComponents` IS EVER WIDENED, THIS LIST MUST WIDEN IN THE
SAME CHANGE.** That coupling is written into the script's docstring and the commit message.

⚠ **The log only warns for usages actually EXERCISED at runtime**, so the absence of other warnings
means those paths were not hit — NOT that those flags were set. The fix was deliberately not narrowed
to the one observed flag.

**Permutation cost, stated:** ~40 % more vertex-factory permutations (7 vs 5) on two near-minimal
materials. Paid at cook time and in package size, never at runtime.

**Scripted and reproducible** — the owner does not open the editor.
`tools/set_material_usage_flags.py` runs headless under `-run=pythonscript`, prints BEFORE→AFTER per
flag per asset, re-reads from disk to prove serialisation, writes
`Saved/AnomalyMaterialUsageFlags.txt`, and exits nonzero on a mandatory-flag failure. Verified
IDEMPOTENT. `tools/create_anomaly_materials.py` now draws its usage list from the same constant so
re-authoring cannot silently reintroduce the bug.

### §4.2 THE ESCALATION — prior deliveries are affected

`missing_texture` applies `M_MissingTexture_Checker`: a UV-space grey/white checker at 8×8 tiling,
Lit, matte, two-sided, opaque. **Its CHECKER reading at home is the INTENDED APPEARANCE, not the
fallback** — three independent lines, the engine gate first: static lighting is disallowed here so
the usage is never queried; `used_with_skeletal_mesh` was already true and m29's G-4S was
owner-confirmed "pink confirmed", which the default material cannot produce; and zero `LogMaterial`
lines exist in eleven packaged logs.

🚨 **DIRECTLY: CONCORDE-CAPTURED DELIVERIES ARE AFFECTED.** The client captures on Concorde, whose
logs carry the warning, so there any STATICALLY-LIT STATIC-MESH target drew the engine default while
the event was labelled manifested with full coverage. **Precise characterisation, neither softened
nor overstated: these are NOT "labelled but invisible" — the default material IS a visible change, so
the anomaly is present. They are WRONG-APPEARANCE samples.** A grey default grid still reads roughly
as "missing texture"; grey is NOT magenta corruption, so a model trained on those frames learned the
wrong appearance for that class. Skeletal targets are unaffected — which is exactly why the owner saw
a magenta weapon beside grid-rendered props. **No remediation of past datasets was attempted; owner
ruling, recorded not chased.**

## §5 THE CAPTURE-TIME ENGINE TICK-MODE PIN

Productises the lever the owner validated on the office machine: forcing the fork's
fixed-sim/variable-render mode OFF for the duration of a capture produced ZERO label/pixel desync on
owner inspection, in BOTH PIE and PACKAGED in-round runs, multiple rounds, mode confirmed engaged
each time. That validation ran from a LOCAL UNCOMMITTED EDIT on a sealed box; this is the same shape
re-expressed from spec so the deliverable cooks from pushed canon.

**Fork detection is at BUILD time.** `AnomalyCapture.Build.cs` probes the engine tree for
`FWNetSubsystem.cpp` — six plausible `Runtime/` paths, then a bounded, exception-guarded recursive
scan of `Source/Runtime` and `Plugins` — and PrivateDefines `ANOMINJECT_FW_TICKPIN` to 1 or 0,
**logging the probe result either way**. Every fork-touching line is under
`#if ANOMINJECT_FW_TICKPIN`. Core module deps unchanged; the core module never learns the fork exists.

**Behaviour:** save the current value, force false, RE-APPLY every capture tick (a SET, never a
toggle — the host re-asserts, so idempotent enforcement is the point, and each re-assertion is
counted), restore at finish. If the process dies mid-capture the host self-heals on next round entry;
that is in the restore log line itself.

**DEVIATION:** the save/force/echo live in `BeginActualRun`, not `StartRun`. `StartRun` only ARMS
when the focus gate is on; `BeginActualRun` is where the run begins and already hosts the analogous
`FApp` lever (`SetUseFixedTimeStep`) and m30's session globals, after `StartRun`'s clean slate. And
`saved=` cannot be truthful before the save happens.

**Echo — exactly one unconditional greppable line per run**, naming its provenance:
`TICKPIN active saved=<0|1>` / `TICKPIN disabled-by-ini` /
`TICKPIN not-compiled (no decoupled-tick fork detected)`; plus at finish
`TICKPIN restored=<n> reasserts=<n> gameTicks=<n>`.

**`run_summary` gains six fields; `annotation.json`'s field set DOES NOT MOVE** (m27 precedent):
`tickpin_compiled` · `tickpin_applied` · `tickpin_saved` · `tickpin_reasserts` ·
`capture_game_ticks` · `ticks_per_captured_frame`. The last two are the CADENCE EVIDENCE, computable
from artifacts alone.

**📊 UNPINNED HOME BASELINE, MEASURED: `ticks_per_captured_frame` = 1.3556** (122 ticks / 90 frames).
It exceeds 1 even on a lockstep host because lead-in and settle ticks produce no frame. This is the
reference the office comparison needs.

### §5.1 The guard proven by breaking it (m27 rule)

Forcing `ANOMINJECT_FW_TICKPIN=1` by hand on this stock engine FAILS the build loudly and names the
fork symbol at both access sites:
```
AnomalyCaptureSubsystem.cpp(66): error C2039:
    'sUseFixedGameTickWithVariableRenderTick_Net': is not a member of 'FApp'
AnomalyCaptureSubsystem.cpp(66): error C2065: undeclared identifier
AnomalyCaptureSubsystem.cpp(71): the same pair at the write site
```
Build exit 6. Reverted; both targets rebuilt clean. **A guard that cannot fail is not a guard.**

### §5.2 The console override, and why an ini-only lever was not enough

`bTickModePinDefault` was ini-only, and **G88 says a loose ini beside a package is a SILENT NO-OP —
the cooked config wins.** So on a packaged Concorde build the only route to the unpinned control leg
was a SECOND COOK, which kills the pinned-vs-unpinned A/B.

`IAI.Capture.TickPin <0|1>` closes that, in the `IAI.Capture.SVE 0` tradition. **Precedence: console
> ini > compiled**, named inside the existing TICKPIN line. Settable both ways. **On a build where
the pin compiled out the command STILL EXISTS and says so** — a silently missing command on the host
that matters is the failure mode we refuse. **NAMED BISECT: `IAI.Capture.TickPin 0`.**

### §5.3 Determinism pair — reported as it printed

Two legs on the PRE-change binary establish the run-unique field set; the POST leg is tested against
it with `subset_gate.py`. A63 precondition satisfied (all `start_frame=1`).

- control pair differing fields **7**; test pair **13**
- extras (differ in TEST but not CONTROL): **6, and they are EXACTLY the six declared
  `run_summary` fields**. Field-set delta is +6 with nothing removed.
- **INVARIANT CORE: ALL IDENTICAL** — event count 8, every `frame_indices` set, every `manifested`
  flag, type/subtype, the video block, and eleven `run_summary` fields.

⚠ **The gate EXITS 1 and that is reported, not relabelled.** It flags any field the control pair does
not exhibit, and six were deliberately added — the m25 S4-3 shape, where the backbuffer field set
added exactly `['capture_path']`.

⚠ **One difference investigated rather than waved away.** The first POST leg had two of seven events
at agreement 0.9375 instead of 1.000 (one extra boundary frame), dropping them HIGH→MED. A SECOND
POST leg reads 1.000 on all seven, identical to both PRE legs. Corroborating: **90 of 90 frames
differ between two runs of the SAME binary** here — A47 re-measured — so pixel-level run-to-run
variation demonstrably exists and a single boundary frame is its smallest expression. It moved no
offset, no coverage and nothing in the invariant core. → **G158**.

## §6 AUTO-POOL ANOMALY PARAMETER DEFAULTS

`blinking`'s half-period and `lod_popping`'s were reachable ONLY as TARGETED-fire arguments, and the
client ships an AUTO-POOL config — so if the pre-registered cadence recalibration fires, the lever to
act on it did not exist.

**Section choice: `[AnomalyInjector]`, not `[AnomalyCapture]`.** These govern ANOMALY behaviour and
the auto-injector fires outside capture too. First `GConfig` read in that module; `GConfig` is Core,
so no new dependency and the game-agnostic invariant is untouched.

```
[AnomalyInjector]
BlinkingHalfPeriodFramesDefault=<frames>     compiled default 3
LodPoppingHalfPeriodFramesDefault=<frames>   compiled default 8
```
Range `[1..600]`. **An out-of-range value is REFUSED, NOT CLAMPED** — a clamp turns a typo into a
different cadence that still looks deliberate (G144's shape). **ABSENT KEY ⇒ COMPILED DEFAULT**, and
the artifact says so in its own words.

⚠ **CONSOLE OVERRIDES ADDED BEYOND THE BRIEF, flagged not slipped in**: `G88` means an INI-ONLY lever
still needs the very cook this item exists to avoid, so the same shape was applied —
`IAI.Anomaly.BlinkHalfPeriod <frames|default>` and `IAI.Anomaly.LodHalfPeriod <frames|default>`.

**Full precedence: targeted-fire argument > console override > ini > compiled default.** The existing
targeted path does not regress.

**Echo on the EXISTING run-config line, no new unconditional line:**
`... | blinkHalf=5(console) lodHalf=8(compiled)`.

**ONE DEFINITION SITE, ENFORCED AT COMPILE TIME.** The capture module's echo needs the compiled
defaults and the anomalies own them, so a naive version duplicates `3 / 8 / [1..600]` across two
modules — and the echo would print one number while the anomaly used another. Canonical values live
in `AnomalyDefaults`; each anomaly's `Apply` carries a `static_assert` tying its constant and clamp to
them. Drift breaks the build, not the artifact.

**NO VALUE WAS CHANGED. The pre-registration stands:** recalibration only from a MEASURED tick ratio,
and only if coverage or agreement actually degrades.

## §7 THE COOK

Container quartet preserved and hash-verified at the new location first (A62) to
`_binary_baselines\m31-v2-container\`, 5/5.

`BUILD SUCCESSFUL`, exit 0, **1 m 34 s**. **NEW QUARTET (G121): exe `DD76385F` · utoc `E4FE9B35` ·
ucas `D9929F6F` · pak `BFB95333`.** Map gate **PASS at exit 0** — CB_GateLevel, MainMenu, MainWorld,
Entry all present, and clean without `CB_LodCalib`, so the gate's expected set was not silenced.

### §7.1 The cooked read-back that could NOT be done, and the control that caught it

I tried to string-scan the cooked `.ucas` for `bUsedWithStaticLighting` **and ran the control first.
The control killed the instrument:** `bUsedWithSkeletalMesh` — which we KNOW is true pre-fix, because
m29's G-4S proved skeletal pink renders out of the cooked artifact — is EQUALLY ABSENT from the
pre-fix container, as are all `bUsedWith*` names, while both material NAMES are present. Cooked
packages do not store these flags as searchable strings, so a scan returning "absent" would have been
BLINDNESS, NOT A NEGATIVE. **The instrument is rejected; its result is not reported.** → **G159**.

Measured corroboration only, and named as such: `StackOBot-Windows.ucas` 364,557,872 → 364,594,736
bytes (+36,864) while the `.pak` is BYTE-IDENTICAL and `global.*` unchanged, consistent with extra
permutations from two new usage flags on two materials. **A size delta is not a flag read-back.**

⇒ **A DIRECT READ-BACK OF THE FLAG FROM INSIDE THE COOKED CONTAINER WAS NOT ACHIEVABLE HERE.
Concorde's re-cook plus the warning grep remains the only verification that exists.**

### §7.2 Appearances unchanged — pre-fix cook vs post-fix cook, same config, same seed

| type | pre-fix cook | post-fix cook |
|---|---|---|
| `corrupted_texture` | MAG 8 / CHK 0 / OTH 0 | **MAG 8 / CHK 0 / OTH 0** |
| `missing_texture` | MAG 0 / CHK 16 / OTH 0 | **MAG 0 / CHK 16 / OTH 0** |

All offsets on the post-fix cook read **+0** (startΔ and endΔ, every type). **`missing_texture` still
reads CHECKER, so §4.2's reading is CONFIRMED, not refuted** — the pre-registered branch "CHECKER
pre-fix and MAGENTA post-fix ⇒ fallback" did NOT fire.

## §8 🚨 A CORRECTION TO THIS SESSION'S OWN EARLIER ANSWER — the spacing knob

An earlier report in this session recommended raising **`preFrames`** to widen the clean gap between
annotated windows. **THAT IS WRONG AND MUST NOT BE CARRIED FORWARD.**

Measured on an auto-pool leg with `IAI.Capture.Config 2 14 8 4 0`:
windows `13-20, 27-31, 39-43, 49-56, 61-68, 85-92, 97-104, 109-116` → **MIN CLEAN GAP = 4,
unchanged**, burst period still 12 rather than 26.

**`preFrames` is a ONE-TIME LEAD-IN — it runs once per RUN, not per burst.** The inter-window gap is
governed by **`postFrames`**. Re-measured with `2 4 8 14 0`: windows `3-10, 26-32, 48-54, 69-76,
91-98` → **MIN CLEAN GAP = 14**, ceiling ±7, `--require-gap 12` passes.

⚠ **`IAI.Capture.Config` DOES govern spacing on the AUTO-POOL path** — the burst FSM is
targeting-agnostic and `bTargetedMode` only selects which fire route `BeginFire()` takes. The knob was
simply misidentified. **It was corrected because the tool PRINTED the achieved gap, not because it was
re-derived** — which is the argument for `--require-gap` over arithmetic. → **G160**.

## §9 NOT DONE, NAMED

- **`m31` IS STILL UNTAGGED and still awaiting Concorde V-3/V-4.** Nothing here changes that.
- **The material fix is UNVERIFIABLE ON THIS BOX** (§4). Concorde's re-cook is the only verification.
- **A direct cooked-container read-back of the usage flags** (§7.1) — instrument rejected, not faked.
- **No remediation of previously delivered datasets** (§4.2) — recorded, owner's call.
- **No cadence compensation, no F-BLINK rescaling, no constant invented.** The lever now exists; the
  pre-registration governs whether it is ever used.
- **`P6` did not move.** `feature/stencil-capture` untouched at `76cac74`. No force-push. No ratio,
  no threshold proposed anywhere.

## §10 HAND-OFF — the office read-off pass, in order

1. **Pull the PLUGIN repo only** (AnomDash is not needed — the script lives here now).
2. Rebuild the **EDITOR** target first (G47/G131), then the **GAME** target. Read one build line:
   `AnomalyCapture: TICKPIN probe FOUND … ANOMINJECT_FW_TICKPIN=1`.
   ⛔ **If it reads `=0` on Concorde, STOP** — the probe missed the fork and every leg below is
   meaningless.
3. **PIE sanity:** one SVE capture. Read `TICKPIN active saved=1` and
   `TICKPIN restored=1 reasserts=<n> gameTicks=<n>`. `reasserts > 0` is EXPECTED and healthy.
4. **Cook** (setup-runbook §8.6 step 4).
5. **Grep the cook and run logs for `missing bUsedWith` — EXPECT ZERO HITS. THIS IS THE ONLY
   VERIFICATION OF THE MATERIAL FIX ANYWHERE.** Also confirm by eye that skeletal magenta is magenta.
6. **Packaged PINNED leg:**
   `IAI.Capture.Delivery 0` · `IAI.Capture.TickPin 1` · `IAI.Capture.Config 2 4 8 14 0` ·
   `IAI.Capture.Start "<OUT>\PINNED" png 777 120`
   Read the TICKPIN lines and the six `run_summary` fields; compare
   `ticks_per_captured_frame` against the unpinned home baseline **1.3556**.
7. **Packaged UNPINNED leg:** identical but `IAI.Capture.TickPin 0`.
8. **Script over both:**
   `python -m pip install --upgrade Pillow` ·
   `python Plugins\AnomalyInjector\tools\measure_label_offset.py --selftest` ·
   `python Plugins\AnomalyInjector\tools\measure_label_offset.py "<PINNED session>" "<UNPINNED session>" --label pinned --label unpinned --require-gap 12 --log "<UE .log>"`
   Read the **CEILING banner first**, then the per-type table, then the arm→publish gap line. Home
   reference for that line: gap uniformly **1 publish**, n=48, `hist[1:48]`, with pixel offsets +0.
   **If step 8 exits 4 the capture was configured too tightly — fix the `Config` line and re-run
   before reading any number.**

## §11 PRE-REGISTERED READINGS, restated verbatim and unchanged

- Older backbuffer sessions show the SAME offset distribution as packaged SVE → shared
  annotation-timeline disease; m31 FIFO not implicated.
- Backbuffer differs materially from SVE → the FIFO owns part of it; m31's design partially reopens.
- Editor SVE ≈ 0 while packaged is large → the lever is runtime-mode-specific.
- Offsets differ strongly BY TYPE → per-type render-side latency is real; any single global
  correction constant is the wrong shape.
- Low coverage with manifestation clustered at the window END → labels lead pixels, consistent with
  the sim-clock-labels hypothesis; clustered at the START → hypothesis weakened.
- Arm→publish gaps ≈ uniform ~1 publish while pixel offsets run 1–6 variable → the pairing gap
  CANNOT own the offset; render-side interpolation latency becomes the lead suspect.
- Per-event offsets correlate with per-arm consumption gaps → the pairing gap owns it.
- **Tick pin:** if measured ticks-per-captured-frame under the pin is ≫ 1 AND pinned-run blink
  coverage or agreement degrades versus the unpinned baseline, then F-BLINK's half-period is
  under-sampled and must be recalibrated as (current half-period × measured tick ratio) —
  calibration FROM the measurement, never an invented constant. If coverage holds, no change is made.
