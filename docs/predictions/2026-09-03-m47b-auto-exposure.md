# PRE-DECLARATION — `m47b` / session 069 brief 26: AUTO-EXPOSURE as the candidate for symptoms (2) and (3)

**Written BEFORE any leg ran.** Nothing below was edited after a measurement existed; corrections ride
the journal (`P-C2` route), never this file.

Brief 069-26, chat thread A. Follows `069-25` (`m47`), whose result was a **strong negative**: the
shader-readiness mechanism is real and did not reproduce the symptom at maximum forcing (journal 069
§17). Symptoms **(2) "target renders BLACK"** and **(3) "whole picture black for a burst, recovers"**
were left **UNEXPLAINED**, with auto-exposure and Lumen surface-cache invalidation named as the two
untested candidates. `ND-B` chose **auto-exposure first** — the cheapest, and the only one that can
reach symptom (3), because a per-material fallback cannot blacken a whole frame.

---

## §0 WHY THIS IS THE PRIME CANDIDATE — established by reading, before any leg

| fact | source | status |
|---|---|---|
| every packaged bench leg forces `r.DefaultFeature.AutoExposure 0, r.EyeAdaptationQuality 0` | `run_leg.ps1:187` | READ |
| the editor runner does the same unless `-AutoExposure 1` | `run_leg_editor.ps1:78` | READ |
| ditto the `LastRunDir` verifier | `verify_lastrundir.ps1:61` | READ |
| the plugin never touches exposure | `grep -i exposure Source/` → **one log string, no code** | READ |
| StackOBot's `Config/DefaultEngine.ini` sets **no** exposure key at all | direct read of all 5 project inis | READ |
| `Engine/Config/BaseEngine.ini` sets **no** exposure key either | direct read | READ |
| ⇒ **the game's own defaults ARE the engine defaults** | the two above | DERIVED |

**The engine defaults, from UE 5.1 source (not from memory):**

| cvar / field | default | source |
|---|---|---|
| `r.DefaultFeature.AutoExposure` | **1 — ON** | `SceneView.cpp:165-170` |
| `r.DefaultFeature.AutoExposure.Method` | 0 = **Histogram** | `SceneView.cpp:172-177` |
| `r.DefaultFeature.AutoExposure.Bias` | 1.0 | `SceneView.cpp:179-182` |
| `r.DefaultFeature.AutoExposure.ExtendDefaultLuminanceRange` | 0 = **legacy UE4 range** | `SceneView.cpp:184-191` |
| `AutoExposureMinBrightness` / `MaxBrightness` | **0.03 / 8.0** | `Scene.cpp:468-469` |
| `HistogramLogMin` / `LogMax` | −8 / +4 | `Scene.cpp:470-471` |
| `AutoExposureLowPercent` / `HighPercent` | 10 / 90 | `Scene.cpp:453-454` |
| **`AutoExposureSpeedUp` / `SpeedDown`** | **3.0 / 1.0** | `Scene.cpp:478-479` |

🔑 **`Min 0.03 < Max 8.0`, so auto-exposure is genuinely ADAPTIVE here — not the
`Min == Max` "fake manual" degenerate case** (`Scene.cpp:1055-1059`). ⇒ the owner's editor, running
the game's defaults, has **live eye adaptation**; every bench leg ever run has had it **forced off**.
**That asymmetry is real and has existed for the whole project.**

🔑 **AND IT REACHES THE CAPTURED PIXELS.** Eye adaptation is applied inside the tonemapper, and the
capture SVE subscribes at **`EPostProcessingPass::VisualizeDepthOfField`**
(`AnomalySceneViewExtension.cpp:72`), which is **after `Tonemap`** in the enum. ⇒ exposure is baked
into the written PNGs, so this is measurable from the artifact and needs no new instrument.

⚠ **SCOPE, STATED FIRST: this establishes the defaults for THIS BENCH (StackOBot).** Bates and
Concorde are different projects with their own inis and their own post-process volumes. Nothing here
is a claim about what *their* exposure settings are. What it does establish is that **AE-ON is the
engine default**, so a title has to opt OUT deliberately.

---

## §1 THE MODEL, AND THE DIRECTION IT PREDICTS — derived before measuring

Adaptation is exponential toward a target with time constant `τ = 1 / Speed`. At the pinned 30 fps
capture rate with `IAI.Capture.Config 2 4 8 4 0`:

| edge | speed | τ | frames to 63 % | frames to 95 % |
|---|---|---|---|---|
| scene gets BRIGHTER (exposure adapts down) | `SpeedUp` 3.0 | 0.33 s | **10** | 30 |
| scene gets DARKER (exposure adapts up) | `SpeedDown` 1.0 | 1.0 s | **30** | 90 |

**A burst is 18 frames (2 pre + 4 settle + 8 positive + 4 post) of which 8 are positive.** So:

- Across an 8-frame positive window the exposure travels `1 − e^(−8/30 × 3)` = **55 %** of the way.
  Substantial — not negligible.
- Across the 4 post frames it recovers `1 − e^(−4/30 × 1)` = **12 %**. Almost nothing.

🚨 **THEREFORE THE PREDICTED SIGNATURE IS ASYMMETRIC, AND THE DEEPEST DIP IS *NOT* ON THE LABELLED
FRAMES.** On this fixture the swap material is **BRIGHTER** than the scene it replaces — `m47`
measured target luminance **106.9 – 127.0** against a whole-frame mean of **≈ 99 – 100**. So:

1. **Fire.** Bright magenta appears while exposure is still adapted to the darker scene ⇒ the target
   renders **BRIGHT on its first frames**, and the whole frame then **darkens** over the window as AE
   pulls exposure down (τ = 10 frames).
2. **Revert.** The magenta vanishes but exposure is still LOW ⇒ **the whole picture is DARKER THAN
   NORMAL and recovers slowly** (τ = 30 frames, against a 12-frame gap to the next fire).
3. ⇒ **the trough lands on the post/pre/settle frames — frames the labels call CLEAN — and the trace
   is a SAWTOOTH that never fully recovers inside a burst cycle.**

📌 **THIS CONTRADICTS ONE CLAUSE OF THE BRIEF, AND THAT IS DECLARED HERE RATHER THAN DISCOVERED
LATER.** The brief predicts *"the target's first-frame luminance is lower than its event mean"*. That
direction requires the anomaly material to be **DARKER** than what it replaces. On this fixture it is
**BRIGHTER** (measured, `m47`), so the physically expected direction here is the **opposite**:
first frame **brighter**, then darkening. **Both directions are recorded as READINGS**; whichever
occurs, the report states the sign and the measured target-vs-scene luminance that explains it. The
brief's clause is not "wrong" — it is **conditional on a sign this fixture does not have.**

---

## §2 THE POSITIVE CONTROL — RUN AND READ FIRST, BEFORE ANY DIP NUMBER (`G96`)

**`AE-LIVE`: the AE-ON and AE-OFF legs must differ in steady-state whole-frame luminance.**
`r.DefaultFeature.AutoExposure 0` pins `Min = Max = 1` (fake manual) and `r.EyeAdaptationQuality 0`
disables the pass, so an AE-ON leg that produces the *same* luminance level as its AE-OFF control has
**not got live eye adaptation**, and every subsequent "no dip" reading from it would be **blindness,
not a null.**

- **`AE-LIVE` PASS** ⇒ AE is demonstrably active; dip readings are evidence.
- **`AE-LIVE` FAIL** ⇒ ⛔ **NO dip verdict is offered at all.** Report the failure, do not report a null.

---

## §3 PRE-DECLARED BRANCHES

Let `dip_pct(i) = 100 × (mean(frame_lum[i−8 .. i−1]) − frame_lum[i]) / mean(frame_lum[i−8 .. i−1])`,
computed on the whole-frame mean luminance of the written PNG, over the frames of one session.

- **BRANCH `AE-DIP`** — the AE-ON leg shows whole-frame excursions phase-locked to the fire/revert
  edges that the AE-OFF control **does not**. ⇒ auto-exposure is **SUPPORTED as the mechanism for
  symptom (3)**, with depth / duration / recovery / phase reported per event and per anomaly type.
  ⛔ Still a MECHANISM claim on this fixture, never an incidence claim about the owner's sessions.
- **BRANCH `AE-NULL`** — AE-ON and AE-OFF traces are equivalent within the AE-OFF leg's own
  frame-to-frame spread. ⇒ auto-exposure **did not reproduce** symptom (3) here.
  🚨 **`AE-NULL` IS ONLY REPORTABLE IF `AE-LIVE` PASSED AND THE STIMULUS WAS ADEQUATE** — see §4.
- **BRANCH `AE-PARTIAL`** — an excursion exists but is far too small to read as "black". Report the
  number and say plainly that it does not account for the symptom. **This is the branch a 7 %-coverage
  fixture is most likely to produce, and saying so in advance is the point of writing it down.**

---

## §4 THE `G135` GUARD — WHAT A NULL WOULD AND WOULD NOT MEAN

`CB_GateLevel`'s target `StaticMeshActor_49` covers **≈ 7.2 – 7.8 %** of frame. A 7 % area change may
simply not move the **10th–90th percentile** log-luminance histogram that the Histogram method reads.

⇒ **A null on this fixture does NOT refute auto-exposure as the owner's mechanism.** `G135` in its
exact shape: a fixture built from a restricted set cannot exhibit what that set cannot produce, and
the blindness presents as a clean pass. **If `AE-NULL` or `AE-PARTIAL` fires, the report must state
the stimulus size that produced it** — target coverage, and the measured target-vs-scene luminance
gap — **and must not upgrade the null into "auto-exposure is excluded".**

---

## §5 SYMPTOM (2) — WHAT WOULD AND WOULD NOT COUNT

Symptom (2) is *"the target renders BLACK"*. Auto-exposure can only explain it if the whole frame
darkens with it — AE is a **global** operator and cannot darken one object alone.
⇒ **if a dark TARGET is ever observed without a corresponding whole-frame dip, auto-exposure is
REFUTED for symptom (2)** and the remaining named candidate (both swap materials are
`MSM_DEFAULT_LIT` with no emissive, so an unlit region renders black) becomes testable — it is
**UNTESTED today because no dark target has ever occurred to attribute** (journal 069 §17.6).
⛔ **No mechanism is asserted for (2) in this brief unless a dark target actually appears.**

---

## §6 `A4` — THE BLACK-FRAME GATE AGAINST THE DIP

`tools/verify_capture.py --black-frame-gate`, threshold **6.0** on the 0..255 whole-frame mean, was
DERIVED at `m47` from this fixture's darkest legitimate frame (**59.992**) — an order of magnitude of
headroom (journal 069 §17.10).

- **Expected: the gate does NOT fire on a legitimate exposure dip.** A dip is the game's own
  rendering, correctly captured; a gate that deleted it would be deleting truth.
- **If the dip goes below 6.0, the THRESHOLD is wrong, not the dip** — and this file says so **before**
  the number exists, so the finding cannot be rationalised after the fact.
- The minimum whole-frame luminance during the dip is reported against 6.0 either way.

---

## §7 WHAT THIS BRIEF DOES NOT DO

⛔ **NO SOURCE CHANGE.** Docs + harness only. The `-AutoExposure` switch added to `run_leg.ps1` is a
**bench** parameter (it only decides whether two cvars are issued); it is never in a client payload,
and its default is **0**, so every existing leg recipe is byte-unchanged.
⛔ **No product handling of the dip is implemented** — Task B is ruling PREP, and the `exposure_dip`
key / `frames_exposure_dip` counter are **proposals with a threshold to be derived from A1**, not
built. Chat decides.
⛔ **No claim about Bates or Concorde exposure settings** — this bench is StackOBot.
⛔ **No incidence claim.** At most a mechanism, on one fixture, at one coverage.
