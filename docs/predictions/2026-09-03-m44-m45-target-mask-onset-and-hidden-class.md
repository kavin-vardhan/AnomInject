# `m44` / `m45` — TARGET-MASK ONSET AND HIDDEN-CLASS MASKS: PRE-DECLARED GATES

**Written BEFORE any `m44`/`m45` source change.** Reproduction and root cause: journal 069 §7.
⛔ **Plan only — nothing implemented.** ⛔ Never amended once a measurement exists (`P-C2` route).

---

## 0. WHAT THE REPRODUCTION ESTABLISHED — read this before the gates

| observation | status on the bench |
|---|---|
| **O1** first mask frame is `n+1`, label says `n` | ✅ **REPRODUCED — in BOTH tick orders**, `delta = +1` on all four non-hidden events |
| **O2** hidden-class frames carry no mask | ✅ **REPRODUCED** — both `blink` events have **no** mask content on any labelled (hidden) frame |
| **O4** CorruptedTexture's effect starts at `n+1` | ❌ **NOT REPRODUCED** — the picture already differs by **5.9–8.2 %** at `n` against a 0.5 % baseline |

🚨 **`IAI.Bench.SynthTickOrder` is REFUTED as O1's mechanism.** It was the stated first suspect. Both
orders give the identical `+1`. The lever *did* engage (its echo is in the log, and it moved the blink
hidden set from `16,17,21,22` to `16,17,18,22` — the `P9` shape), so the null is a reading, not a
misfire.

**Root cause, and it is tick-order independent by construction:** `ArmTargetMaskOwn`
(`AnomalyCaptureSubsystem.cpp:836`) needs an `FAnomalyMaskRecord` with `R.Tag != 0`. Records are created
**only** by `FindOrAddRecord` (`AnomalyMaskMeasure.cpp:124`), called **only** from
`AccumulateFrameEvents` (`AnomalyCaptureSubsystem.cpp:4123`), which on the async path is called **only
from the drain** (`AnomalyCaptureSubsystem.cpp:2769`) — when a captured frame's **readback completes**,
one frame after the arm. On a fire's first frame there is no record, no tag, and the frame takes the
blank path.

✅ **No render-timing obstacle to fixing it:** a stencil tag applied at `OnWorldTickEnd` of frame *N* is
live for frame *N*'s render — `World->SendAllEndOfFrameUpdates()` runs inside
`FRendererModule::BeginRenderingViewFamily` (`SceneRendering.cpp:4528`), under the engine's own comment
*"Guarantee that all render proxies are up to date before kicking off a BeginRenderViewFamily."*

---

## 1. `m44` — ONSET ALIGNMENT AND NO BLANK FILES (ship-blocking)

### `M44-G1` — ONSET, THE PERMANENT GATE, BOTH TICK ORDERS
For **every** event, in **native and `SynthTickOrder`**:
**first-mask-frame == first-label-frame == first-differing-picture-frame.**

**PREDICTED after the fix:** `delta = 0` on every non-hidden event in both orders (measured today: `+1`
on 4 of 4, both orders). The picture-onset column is already `n` and must not move.
**FAILURE:** any non-zero delta, or a picture onset that moves — the second would mean the fix changed
what renders, which it must not.

### `M44-G2` — NO BLANK PNGs EXIST
**PREDICTED:** `Get-ChildItem target_mask -Filter *.png` contains **zero** all-zero images; a file
exists **iff** it has content.
Today: **61 of 90 files are blank** on a standard leg. ⇒ file count should fall from 90 to ≈29.

### `M44-G3` — THE COUNT IDENTITY
**PREDICTED:** `count(mask_state == "present") == number of PNGs on disk`, and
`present + empty + unmeasured == captured frames`.

### `M44-G4` — SCHEMA, ADDITIVE ONLY
**PREDICTED:** `labels.jsonl` gains exactly **`mask_state`** (`"present" | "empty" | "unmeasured"`);
`mask_file` keeps its `string|null` type and is `null` whenever `mask_state != "present"`.
`annotation.json` **unmoved**. `run_summary`: `target_mask_frames_measured` unchanged in meaning;
**`target_mask_frames_hidden_blank` is KEPT under its existing name and re-documented as the count of
`mask_state == "empty"` rows** — ⚠ **kept, not renamed**: a renamed key is a silent schema break for any
consumer already reading it, and the value's meaning is unchanged (it always counted exactly these
frames; only the file stopped being written).

### `M44-G5` — `P-C7 v2` WITH MASKS AS INTENDED CONTENT
**PREDICTED:** against the `m43` control, the ONLY differences are (a) blank PNGs absent, (b) the new
`mask_state` key, (c) `mask_file` becoming `null` where it was a blank file. Everything else — pictures,
`annotation.json`, every other label field — byte-identical bar `t_wall`.

### `M44-G6` — `m26` AND THE VETO UNTOUCHED
**PREDICTED:** `vetoed_events`, `translucent_vetoes`, `translucency_unknown_vetoes`, `mask_probe_arms`,
`mask_residual_discards`, `mask_nopass_discards`, per-event `framesContributed` and every
`MEASURED_*` verdict **identical** to the `m43` control in both orders.
⚠ The fix creates mask records **earlier**, which is exactly the kind of change that can move
`framesContributed`. **If it moves, that is a finding, not a rounding — report and stop.**

---

## 2. `m45` — HIDDEN-CLASS MASKS (ships only if its identity gate passes)

### `M45-G1` — THE ARBITER IS BYTE-IDENTITY OF THE PICTURE
A `blinking` leg and a `missing_object` leg under the new hide must produce pictures **byte-identical**
(`P-C7 v2`) to the same leg under `SetActorHiddenInGame`, **in both tick orders**.
🚨 **This is the gate that decides whether `m45` ships at all.** ⛔ **Any pixel difference on any frame
means the hide is no longer a hide, and `m45` does NOT ship** — it becomes a documented limitation with
the fallback below.

### `M45-G2` — THE MASK ON HIDDEN FRAMES
**PREDICTED:** on every hidden frame the mask is **non-empty** and equals the would-be-visible region;
the bit-exact tie (`MASK-TIE`, 0 mismatches) still holds.

### `M45-G3` — THE CENSUS DOES NOT SEE THE HIDDEN TARGET AS VISIBLE
**PREDICTED:** a hidden-class target under anomaly is **never** a selection candidate and **never**
counted `MEASURED_NONZERO` by the census while its fire is live; `census_*` counters and the verdict
histogram unchanged vs the `m43`/`m44` control.

### `M45-G4` — PROVE-IT-CAN-FAIL
**PREDICTED:** with the new hide deliberately mis-applied (one silencing flag omitted), `M45-G1`
**fails** and names the frame. ⛔ A `G1` that has only ever passed is not evidence.

**FALLBACK, pre-declared so it is not invented later:** if byte-identity cannot be reached, hidden-class
frames get a **projected-bbox** mask from `m39`'s bounds, marked **`"coarse": true`** in
`mask_map.json` and documented as such. ⚠ **A coarse mask must never be presented as a silhouette.**

---

## 3. THE NEW STANDING RULE (goes in the milestone template)

**Every capture-side gate that asserts a per-frame alignment — labels, masks, onset, bbox — runs in BOTH
tick orders (native and `IAI.Bench.SynthTickOrder`), and the onset instrument is permanent.**
📌 `m43` was gated in the bench's native order only; it shipped a systematic `+1` that **both** orders
would have shown. ⚠ **The gap was not the missing order — it was that no gate compared first-label to
first-mask at all.** Both halves of that lesson belong in the rule.

---

## APPENDIX - RESULTS, 2026-09-03 (appended; nothing above was edited)

`m44` was built and gated in BOTH tick orders. **`M44-G1` FAILED and the milestone STOPPED.**

| gate | result |
|---|---|
| `M44-G1` onset | FAIL **0/4, `delta = +1` in both orders** |
| `M44-G2` no blank PNGs | PASS (0; was 61 of 90) |
| `M44-G3` count identity | PASS (23 present / 4 empty / 63 unmeasured = 90) |
| `M44-G4` schema additive | PASS - `mask_state` added; `mask_file` still `string|null` |
| `M44-G6` veto inputs unmoved | **PASS** - all six counters, the event set, every `manifested` and `positive_frames` identical to the `m43` control |
| `M44-G7` masks subset of labelled frames | PASS (0 stray; this is the defect the owner saw on the host) |

**`M44-G5` (`P-C7 v2`) WAS NOT RUN** - it certifies a shipping build, and `m44` does not ship.

**THE PREDICTION IN SECTION 1 WAS INCOMPLETE AND IS CORRECTED BY MEASUREMENT, NOT REWRITTEN:**
"creating the record earlier fixes the delta" is **false**. It is necessary and not sufficient. On the
first labelled frame the mask is armed and measured and the reduce table reads **`tableCount = 0`** for
that tag (`MASK-TIE MATCH`), rising to 48,587-66,862 on the next frame, so **a newly applied stencil
tag is not in that same frame custom-depth pass.** Tagging during `Tick` instead was tried and made it
**worse** (0/4 vs 1/4), so the lag is not about placement inside frame n. No mechanism asserted.
Journal 069 section 8.3.

---

## APPENDIX 2 - THE FRAME-HANDSHAKE HYPOTHESIS, TESTED AND REFUTED (2026-09-03, brief 15)

Pre-declared in brief 15: (1) the mask centroid matches the PREVIOUS frame's probe position on nearly
every frame; (2) `r.OneFrameThreadLag 0` makes the masks align.

| leg | order | lag | decidable | CURRENT | PREVIOUS | NEITHER | no-mask |
|---|---|---|---|---|---|---|---|
| A1_NAT2 | native | default | 28 | 18 | **0** | 10 | 12 |
| A2_LAG0 | native | **0** | 28 | 17 | **0** | 11 | 12 |
| A1_SYNTH | synth | default | 30 | 20 | **0** | 10 | 10 |
| A2_SYNTH_LAG0 | synth | **0** | 29 | 19 | **0** | 10 | 11 |

**BOTH PREDICTIONS FAILED.** PREVIOUS is 0 everywhere; the lag cvar changes nothing (identical
verdicts and identical pixel counts). Both levers are proven engaged, so this is a reading and not
blindness. **HYPOTHESIS REFUTED.** No fix was written.

What the probe DID show: the mask is correct on ~18-20 of 40 frames, carries an EXTRA silhouette the
picture does not contain on ~10, and is absent on ~10-12. No mechanism asserted. Journal 069 section 9.
---

## APPENDIX 3 - HYPOTHESIS #3 (INTERNAL-vs-OUTPUT RESOLUTION), 2026-09-03 brief 16

| | prediction | result |
|---|---|---|
| P1 | internal view rect differs from output rect on NEITHER/no-mask frames | **FAILED** - internal == output == unscaled == 1280x720 on ALL 51 passes |
| P2 | forcing ScreenPercentage 100 + DynamicRes off gives 0 NEITHER / 0 no-mask | **FAILED** - already 100%, NEITHER unchanged |
| P3 | forcing ScreenPercentage 50 makes every decidable frame NEITHER | **HELD** - internal 640x360 vs output 1280x720, CURRENT 0 / NEITHER 25 of 26 |

**HYPOTHESIS DEAD AS THE EXPLANATION** (all three were required). **CONFIRMED AS A REAL, SEPARATE
DEFECT**: whenever internal resolution differs from output - dynamic resolution, screen percentage
other than 100, any temporal upsampler - every mask is wrong. Demonstrated, not argued. It is simply
not active at the bench default.

**RETRACTION.** Appendix 2's "extra silhouette / absent mask" was TWO artifacts in the probe itself:
the probe's tag 250 sat inside the allocator range and the census (78 candidates / 16 cycles) both
took it and re-tagged the probe; and the probe used the same magenta material corrupted_texture swaps
to, so the picture-side detector merged two objects. With both removed the mask is CURRENT on 40 of 40
frames in BOTH tick orders. Appendix 2's REFUTATION of the frame-handshake hypothesis is unaffected
and stands. Journal 069 section 10.
---

## APPENDIX 4 - TAG OWNERSHIP: THE +1 IS FIXED (2026-09-03, brief 17)

Hypothesis #4 as stated (census vs event tags share one pool) is HALF RIGHT. The mechanism is real -
an actor under a live fire could carry a foreign stencil value and ArmTargetMaskOwn accepted it
without retagging - but the foreign value comes from a PREVIOUS EVENT on the same actor as often as
from the census. Census OFF cured 2 of 4; the fix cures 4 of 4.

A2 instrument, on exactly the four +1 events and no others (4 of 27 armed frames):
  si=27 eventTag=222 actor carried 204 (census)
  si=51 eventTag=224 actor carried 242 (census)
  si=63 eventTag=226 actor carried 224 (the previous event on that actor)
  si=87 eventTag=229 actor carried 226 (the previous event on that actor)

FIX: an actor under a live fire belongs to its event - ArmTargetMaskOwn retags unconditionally, as
m26's ArmIfMeasurable already did. The pool was deliberately NOT partitioned: 55 assignable values
against 77 census candidates per leg, so a split would make exhaustion more likely, not less.

GATES both orders: G1 4/4 delta 0, G2 0 blanks, G3 27+0+63=90, G7 0 stray, G6 all six veto counters
identical to the m43 control, MASK-TIE 27 lines 0 MISMATCH, m26 probe fires (mask_probe_arms=1),
P-C7 v2 frame_index delta one constant with mask_value the only other differing field, both build
targets exit 0.

F1 (resolution mapping) is BUILT AND UNVALIDATED: changing the shader's parameter struct is fatal
against the cooked container ("parameter structure has changed without recompilation"), so it needs a
COOK, which is owner-sequenced. B2's 50% gate was NOT run.

---

## APPENDIX 5 - m45 HIDDEN-CLASS MASKS: MECHANISM CONFIRMED, M45-G4 NOT OBTAINED (brief 19)

A1 = YES: a bRenderInMainPass=false primitive still reaches the custom-depth pass. The custom-depth
processor gates only on ShouldRenderCustomDepth (CustomDepthRendering.cpp:265); the base pass refuses
on ShouldRenderInMainPass (BasePassRendering.cpp:1831); the depth pass refuses because
ShouldRenderInDepthPass = bRenderInMainPass || bRenderInDepthPass (PrimitiveSceneProxy.h:613).

M45-G1 IDENTITY - the delivered configuration CANNOT answer it. Two runs of the SAME config differ by
9.1612% of pixels; the deliberate violation reads 9.5381% and the correct fix 8.5619% - all one band.
Pose, alignment and event sets identical, so this is per-run rendering nondeterminism, not A47.
With AA/Lumen/GI/reflections OFF the floor collapses to ZERO and:
  CONTROL old-vs-old2   0 of 60 differ  (floor is zero; sensitivity one pixel)
  TEST    old-vs-NEW    0 of 60 differ  -> M45-G1 PASSES
  CAN-FAIL old-vs-NOSH  0 of 60 differ  -> M45-G4 NOT OBTAINED

Both levers are PROVEN ENGAGED (engine log echoes both at frame 1), so the can-fail leg really omitted
shadow silencing and the picture still did not move: this target casts no shadow reaching the frame in
CB_GateLevel. The FIXTURE cannot exhibit the class (G135). A guard never shown to fire is not a guard
(G96), so m45 does NOT merge. The mechanism works - 20 mask files under the new hide against 0 under
the old. Journal 069 section 13.

---

## APPENDIX 6 - THE GATE'S RECIPE AND ITS INSTRUMENT ARE PART OF THE GATE (2026-09-03, brief 28)

Appended; nothing above was edited.

### What happened

At brief 27 MASK-PICTURE-PAIRING read `NEITHER 54` of 79 decidable against a recorded baseline of
`NEITHER 0`. An A-side on the previous binary read `NEITHER 54` too, so it was reported as "moved
nothing". **Both cells were already off-baseline.** Bisected at brief 28:

|  | recipe `blinking`/4242/40 | recipe `corrupted_texture`/777/90 |
|---|---|---|
| m46 `60AE8C61` | `A4_PAIR_NAT` **33/33 N0 P0** | `B28_B1` **25/79 N54 P0** |
| m48 `DE65F84A` | `B28_B4` **33/33 N0 P0** | `M48_P4` **26/80 N54 P0** |

The reading tracks the **RECIPE**, not the binary. `m44_pairing_probe.py` was byte-unchanged since
`eea1a31` and reproduces every banked baseline exactly. Cause: the probe wears
`M_CorruptedTexture_Pink` (`AnomalyCaptureSubsystem.cpp:1408-1409`) and `picture_centroid()` finds it
by that magenta - which is the same asset `corrupted_texture` swaps its target to. G226 on the colour
axis; 069-16 fixed the TAG half structurally and the COLOUR half only by fixture choice, which lived
nowhere but in the typed command.

### THE RULE THIS ADDS TO SECTION 3

**A gate's INSTRUMENT and its FIXTURE RECIPE are versioned WITH the gate, and the gate reads its
fixture back out of the leg's own artifacts rather than trusting the command.**

Concretely, for MASK-PICTURE-PAIRING:

- **The recipe is part of the predicate.** The baseline recipe is
  `-Anomaly blinking -Target StaticMeshActor_49 -Seed 4242 -MaxFrames 40`, in BOTH tick orders. A
  leg on any other anomaly is a different instrument until shown otherwise.
- **The instrument REFUSES rather than reports** when it cannot see its subject:
  exit `2` no `run.json` (fixture unknowable - delivery legs can never be read by this gate),
  exit `3` colliding fixture, exit `4` no probe in the leg. A refusal is **not** a pass, a fail, or a
  `NEITHER` count.
- **Proven both ways (G96):** fires on the banked `corrupted_texture` leg; silent on all four passing
  baselines, whose verdicts return byte-identical to the pre-fix analyser.
- **A name list is not the only guard.** The magenta pixel count is always printed split by
  `anomaly_present`: coincident bands on a clean fixture (8,109 vs 8,040), separated on a colliding
  one (8,493 vs 11,497). An unlisted magenta anomaly is visible even though it would not refuse.
- **The tolerance was NOT widened.** 40 px stands.

### CARRIED LIMIT, MEASURED HERE FOR THE FIRST TIME

The **synth-order** clause is marginal at **si=8**: Delta reads 33.6 / 38.9 on m46 and 38.2 / 40.7 on
m48 against a 40 px tolerance, so it can read `NEITHER 1` by ordinary run-to-run variance **on either
binary**. Branch (a) of a rule fixed before the deciding legs ran (`CaptureBench/tools/
b28_si8_predeclared.md`): the bands overlap, so it is NOT binary-attributable. `PREVIOUS == 0` on all
four legs. NO MECHANISM ASSERTED (G120). A future `NEITHER 1` at si=8 is this, not a new finding.

### FILED, NOT BUILT

Give the probe a colour no anomaly uses - the true analogue of 069-16's tag fix. It is a binary
change and would retire the exe every other `m48` gate ran on, so it was not started unprompted.