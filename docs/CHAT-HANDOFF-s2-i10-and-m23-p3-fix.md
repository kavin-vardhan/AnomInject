# Chat handoff — S2: I10 completed, P3 discovered/diagnosed/fixed (m23)

**Session date:** 2026-08-17
**Plugin repo:** `AnomalyInjector` — HEAD `ee36281`, clean, pushed. **Tag `m23` → `2f74799`, pushed.**
**Bench repo:** `CaptureBench` — `163dd12`, clean, local-only, **zero probe edits for ten consecutive turns.**
**Audience:** a cold reader — fresh chat, Claude Code, or a collaborator.

**Read these two first, in order:** `CHAT-HANDOFF-s2-keying-design.md` (B′, MainMenu discovery,
A8–A33) and `CHAT-HANDOFF-s2-gate-env-and-i10-setup.md` (gate environment, banked calibration,
A34–A43). This doc is the third in the set and carries **A44–A58**, the **P1–P6 phenomena
ledger**, and everything that changed since.

**The one-line status:** m23 fixed a real, client-facing label-fabrication bug on the **current
backbuffer path**. **S3 has not started. The SVE migration has not started. Production still
captures via the backbuffer.** The client's original defect (P1) is still unreproduced and
unfixed.

---

## 1. What this session did, in one paragraph

I10 ran to completion on both levers and came back **clean** — the client's defect does not
reproduce under CPU starvation of either thread, at any ratio from 0.33 to 14.8. The hunt moved
to high VideoFps (the shape the historical residual was actually seen in) and **reproduced a
defect there — but a different one**. Labeled hide windows never appeared in the pixels at all:
49/49 events, both fps, levered and unlevered. That is **P3**, and it is dataset poisoning.
It was diagnosed to two compounding causes, fixed as **m23**, validated through three correct
halts, committed, tagged, and confirmed by owner smoke in real gameplay content. Two further
phenomena (**P5**, **P6**) were discovered along the way and are open.

---

## 2. Current state

### Repo

| | |
|---|---|
| `AnomalyInjector` HEAD | `ee36281` — clean, pushed, 0 unpushed |
| **Milestone tag** | **`m23` → `2f74799`, pushed, remote confirmed** |
| Production code | **CHANGED** — the "byte-unchanged" invariant retired at m23, by design |
| `CaptureBench` | `163dd12`, clean, local-only, zero probe edits (10 turns) |
| Session bank | `D:\IntrusiveAnomalies\_bench_sessions_bank` — 21 dirs / 5,614 MB |

Commits this session: `5d43055`, `d8870f9`, `8c9d254`, `777135f`, `2e0b7a8`, `33b143a`,
**`2f74799` (m23)**, `56b3f07`, `ee36281`. Journals 031–034.

### What m23 changed

- **F-BLINK (P3a):** blink half-period is now **frames** (`int32`, default **3**, min 1 max 600).
  Default 3 reproduces the old 30 fps cadence *exactly*. Tick counts ticks; Revert resets the
  counter. The `[hz]` console argument is **retired** — the second argument is now
  `half_period_frames`.
- **F-LABEL (P3b):** hide-type identity comes from the **anomaly ID** via
  `IsHideTypeAnomaly(FName)` (explicit set `{blinking, missing_object}`), never from the
  sampling outcome. Zero sampled-hidden frames now emit **zero positives** + `manifested:false`
  + a loud per-event warning + a `non_manifested_events` session counter. Unregistered IDs fall
  back to AffectedFrames **loudly**.
- **Args plumbing:** tokens after the target on `IAI.Capture.Start` forward verbatim to the
  anomaly's parser. No tokens ⇒ byte-identical to before. **Auto-pool path untouched.**
- **Untouched:** `IAnomaly`, `labels.jsonl`, auto-pool, client defaults.

### Certification scope of m23 — say this precisely, it matters

| Claim | Status |
|---|---|
| P3b guard works at **all** fps (30/60/90/120/240) | **CERTIFIED** — no fallback shape anywhere, `non_manifested_events=0` on every real-blink leg |
| P3a state-level fix at **all** fps | **CERTIFIED** — real sampled hides at 120/240 where pre-fix there were none |
| 30 fps frame-level pixel alignment | **CERTIFIED, floor-robust** (12 decidable ALIGNED / 0 non-ALIGNED under *both* candidate floors); claimed sets byte-exact to historical `[4,5,9,10]` |
| 60 fps frame-level alignment | **FLOOR-BLOCKED** — deferred, **not failed**. 11 ALIGNED, median margin ~4.4× clean noise, one sub-noise coin flip |
| ≥90 fps frame-level alignment | **P5-BLOCKED** — undecidable by physics of the signal, not by fix defect |
| The `manifested:false` branch | **PROVEN BOTH WAYS** — fires 8/8 on forced non-manifestation, silent on the 30 fps control |

---

## 3. The phenomena ledger — P1 to P6

This replaces all earlier informal talk of "the defect." **Numbers are never reused.**

| # | Name | Status |
|---|---|---|
| **P1** | Client's one-frame shift (`−1`) at ~1.2 ratio, 30 fps | **NOT REPRODUCED** across 12 legs, both levers, ratios 0.33–14.8. Still unfixed. Mechanism unknown |
| **P2** | Stale / duplicate present | **SIGNATURE ABSENT** everywhere — 0/149 adjacent duplicates on every leg |
| **P3** | Labeled hide never manifests in pixels | **REPRODUCED → DIAGNOSED → FIXED (m23)** |
| **P4** | *(withdrawn)* | **PERMANENTLY RETIRED.** Was an oracle artifact, not a defect. Number never reused |
| **P5** | Temporal spread + decay of hide manifestation at high fps | **OPEN.** Instrument assigned, not built |
| **P6** | Bounds/camera-block contract in `annotation.json` | **OPEN.** Evidence gathered, no mechanism adopted |

### P3 — the adopted mechanism, two compounding causes

- **P3a (timing).** `Anomaly_Blinking` accumulated forwarded tick dt against a **wall-time**
  half-period (`0.5/Hz`, default 0.100 s). Under capture, dt = `1/VideoFps`
  (`SetFixedDeltaTime`). At 120 fps the whole 8-frame positive window is ~0.067 s — under one
  half-period — so **the toggle never fired and the object never hid**. Scene-level. Confirmed
  by D-B; `missing_object` (hides in `Apply`, no Tick) is immune, confirmed by D-C.
- **P3b (labeling).** `AnomalyCaptureSubsystem.cpp:1466-1467` inferred hide-type from *sampling
  outcome* and silently substituted `AffectedFrames` when nothing sampled hidden — converting
  non-manifestation into a **full window of fabricated positives**. Anomaly-agnostic.
  **This is the dataset-poisoning amplifier.** Fingerprint on record: 30 fps claimed sets are
  **gapped** (`[4,5,9,10]`, sampled shape); 120/240 pre-fix were **8-consecutive** (fallback shape).
- **Threshold bookkeeping is OPEN and non-blocking.** The naive arithmetic predicted the cliff
  between 60 and 90 fps; measurement put it between **90 and 120**, and the 60 fps tail shape
  did not match either. Banked structure instead: the hidden run always **terminates at the
  window's last frame** and lengthens monotonically as fps falls — 120→0, 90→2, 60→5, 30→genuine
  two-block toggling. The fix removes the seconds-vs-frames race entirely, so the arithmetic
  never needed to close.

### P5 — the finding that outlived the fix

Post-m23 the labels are **state-true**, but at high fps the hide is **faint and smeared**:
local-contrast energy decays with fps *and* redistributes across adjacent frames — offset0:offset+1
is ~**2:1 at 30 fps** (0.213/0.095) and ~**1:1 at 120 fps** (0.036/0.039). Consequence: asking
"which single frame is the hide on" has **no answer** at ≥90 fps; it is not a measurement failure.

- **NOT an m23 regression** — pre-fix those frames were never hidden at all; m23 strictly improved them.
- Off-trend datapoint retained as discriminating evidence: HF4 (240 levered, ~25.5 ms wall/frame)
  reads 0.0968, so decay is **not monotonic once wall-clock frame time re-enters**. Any future
  mechanism must explain that.
- **No mechanism adopted.** TAA smearing is *named and untested* — do not write it in.
- **Founding instrument assigned (not built):** a **blend-ladder** — blend adjacent frames of the
  certified 30 fps leg (100:0 / 75:25 / 60:40 / 50:50) to manufacture a known-ground-truth spread
  control. That is also the only route to the contained-regime floor that would unblock 60 fps.
- P5 sits **adjacent to the client's #1 complaint** (labeled-but-invisible anomalies) and to the
  `feature/stencil-capture` perceptibility lane.

### P6 — bounds/camera contract, client-shipping

The `camera` block is **not** mis-sourced — `camera.global_position` is a genuine
`GetPlayerViewPoint` origin, constant across all 90 frames of `labels.jsonl`, matching to the
digit. The equality with the node's `bounds.origin` **comes from the other side**:
`GetComponentsBoundingBox(true)` unions **every** component including non-rendering ones
(spring arm, camera, collision), returning a perfect **1010 cube** for `BP_Bot_C_0` whose centre
lands on the camera.

- **So: for a blueprint pawn, `node.bounds` is not the mesh bounds.**
- `camera.path` is the **view-target actor** path, not a camera path — a naming/contract question,
  not a wiring error. It coincides with `node.path` here because the anomaly fired on the pawn.
- **Client-impacting and it ships:** there is **no delivery gating** on the camera block or node
  bounds (`bDeliveryMode` is referenced only at `AnomalyLabelWriter.cpp:322/338`, the run_summary field).
- **Pre-existing, not an m23 regression.** Cheapest next evidence: inspect the Bot's component
  list in the editor — about a minute, settles the bounds side.

---

## 4. The big measured results — what is now closed

### CPU starvation is REFUTED as a cause of P1, not merely unobserved

Across both I10 sets: **87 hide events, 974 oracle frames, zero misalignments; 1052/1054
identity-check frames at diff 0** (both exceptions are known markerless warm-ups).

- **Game-lever legs:** 6/6 valid — nominal 1.0000, mild 1.0558, client 1.2145 / 1.2342,
  deep 3.0027, pacing-off 0.3312. All clean.
- **Render-lever legs:** 6/6 in band on the **first attempt**, zero retries — nominal 1.0000,
  mild 1.0815, client 1.2145 / 1.3071, deep 3.4840, pacing-off 1.4317. All clean.
- **Pacing-off is two distinct regimes** and both are clean: L5 free-runs *fast* (0.33),
  R5 is render-limited and runs *slow* (1.43).
- Positive control live on every leg: a ±1 shift costs 19–30 mismatches. The null is real.

### The render lever — dead twice, then alive, with zero probe edits

Root cause was **compiled-but-never-staged** (G92): the fix existed in
`<Project>\Binaries\Win64\StackOBot.exe` 36 seconds before the failing legs ran, but the
stage/archive step never ran, so `Builds\BenchGate` kept serving a binary from ten days earlier.
**You hold a green build the whole time** — which is worse than forgetting to build.

**A44 was amended because of it:** the **symbol/string scan is the load-bearing half**;
timestamps are **advisory only and mislead in both directions** (a stale binary with a green
build; a freshly staged exe that *inherits the compile time*, so it still reads old).

### One model covers both levers

```
frame_time ≈ max( 1/VideoFps , natural_frame_time , stall + residual )
    residual 1.3 ms  GAME    → knee 32.0 ms
    residual 6.9 ms  RENDER  → knee 26.4 ms
```

- **Cannot-attribute corollary (now in `docs/capture-fps.md`):** a `speed_ratio` reading
  identifies **frame time**, and **never** the starved thread. The client's 1.2 says her frames
  took ~40 ms and says nothing about which thread starved.
- **High fps: the dial becomes a thermometer.** Natural capture cost on the dev box is
  12.7–17.8 ms (±20 % wander) at 1280×720 with PNG. At VideoFps 120 the budget is 8.33 ms and at
  240 it is 4.17 ms — both **below** natural — so the pacer never has headroom and ratio becomes
  a *readout* of natural starvation. Natural floors: **1.52–2.13 at 120 fps**, **3.18–4.06 at
  240 fps (already in the deep band, unlevered)**. The client band is a **measured impossibility**
  at those fps on this box — recorded as a property, not a coverage failure.

### D-B — the result that justifies the whole detour

A **true dual capture** (production backbuffer + CaptureBench SVE, same run, marker-matched,
one oracle) showed both grab points agreeing **to four decimal places**: the object is visible in
both during every labeled window.

> **The SVE migration would NOT have cured P3.**

Had S3 been built first per the original plan, we would have shipped the migration, kept
poisoning datasets, and concluded the new mechanism was broken. **This is the receipt for
hunt-before-build.**

---

## 5. Decisions made this session, with rationale

### 5.1 Coverage requirements are scoped to the question they were minted for

A40's required coverage (nominal/client/deep/pacing-off) belonged to the **30 fps I10 question**
and is **discharged**. It does **not** extend to high-fps work, where the client band is
physically unreachable. High-fps legs carried pre-declared **regime windows** (A49) instead.
Generalising a coverage rule past its question is how goalposts move without anyone noticing.

### 5.2 Prediction discipline held four times, and failed usefully twice

Predictions were pre-declared **before any sweep existed** and restated verbatim before results.

- I10 predicted the defect would appear in the client band. **It did not** — recorded as a
  MAJOR RESULT per the pre-declared rule, not a partial failure.
- The high-fps prediction (**reproduces**) was written before the calibration sweep existed,
  with the **ABSENT branch pre-declared as counting** — which is exactly the branch that fired.
  Broadening what could satisfy a prediction *before* running it is what made the result strong.
- Render legs were **UNPREDICTED as a class, permanently**. Left that way.
- Code's own survey-derived expectation (cliff between 60 and 90 fps) was **stated before the
  run and falsified**. Reported as falsified.

### 5.3 The oracle saga — three instruments, only the third trustworthy

The certification instrument failed twice, and **only known-answer controls exposed it**:

1. **Fixed-K robust sigma** went blind under A47 camera drift (MAD inflated 0.0102 → 0.0481).
   It reported **false ABSENT on our own fix**. Caught in-turn by raw series → eyes → drift-immune
   statistic, in that order.
2. **Local contrast on ±2 neighbours (A54 v1)** was **unevaluable on contiguous claimed sets** —
   i.e. blind in exactly the P3b fallback shape it had to judge. It failed its own killer control
   (pre-fix HF1 returned ALIGNED=2). Amended to **event-flank neighbours**; TAU (0.04684) and the
   classification rules unchanged.
3. **A54 canonical** passed all three A53 controls: blesses known-ALIGNED, condemns known-ABSENT
   (pre-fix HF1 → ALIGNED=0), and degrades decisively under a deliberate ±1 shift.

**Standing consequence (A53):** any oracle/analysis change re-verifies against one known-ALIGNED
**and** one known-ABSENT control before its results are used. "It found what I expected" is not
verification.

### 5.4 A55's floor is not derivable — and that is a finding, not a blocker

Two defensible constructions were built and they **disagree diametrically about the very legs in
question**:

- **Construction A** (pseudo-events from real event shapes on clean territory): **contaminated** —
  a pseudo-event can sit on clean frames while its ±1 variants land on **real hidden frames**, so
  the margins are signal-inflated. Median ≈ TAU is the tell. No all-clean 9-frame window exists at
  30 fps, so placement cannot repair it.
- **Construction B** (the genuine no-signal leg, pre-fix HF1): **uncontaminated but measures the
  wrong quantity** — with zero signal anywhere it captures the statistic's numerical *precision*,
  not its *discrimination* under a real-but-spread signal. Under B, 120 fps would read "decidably
  SHIFTED" on margins of 0.0005 against a measured ~1:1 energy split.

**The structural cause:** the calibration set **brackets** the regime (sharp signal at 30 fps,
no signal pre-fix) but does not **contain** it — nothing banked exhibits the *spread* regime.
→ **A57.** Choosing a construction here would have been **choosing the verdict**. Only
floor-invariant verdicts certify; **30 fps certifies under both**, which is why it is the one
that landed.

### 5.5 The guard had to fire before it could ship

The `manifested:false` branch — the entire point of the fix — **had never executed**. Two no-code
routes to trigger it were tried and refuted with evidence (the targeted fire was hard-coded to
`{ Token }`; shrinking the positive window can't help because the toggle lands during Apply/settle).

**Ruling: a guard that has never fired is not a guard.** The args plumbing was approved as an
m23 scope amendment purely to make it testable. Its correctness proof **is** the negative test —
which is also why it was not split into its own commit (it would have been justified only by the
next commit).

The certification is the **pair**: guard **fires** 8/8 on forced non-manifestation (verified in
`annotation.json` rows, not the log), and stays **silent** on the 30 fps control. Fires-when-it-should
plus sleeps-when-it-should. Either alone proves nothing.

### 5.6 Sequencing ruled: P3-fix before S3

Client-facing dataset poisoning outranks internal migration, and D-B proved the SVE is orthogonal
to P3 so no technical ordering forced otherwise. S3's matrix now validates on a path that isn't
lying about its labels.

### 5.7 Three halts, all correct

Stop-on-failure was invoked three times during m23 validation and held every time — no same-turn
fixes, no re-thresholding after seeing results, no proceeding on an uncertified base. The refusal
to grade with a ruler built *after* seeing the results is the single most important behaviour of
the session.

---

## 6. Errors on the record — both sides

### Chat-side (design/verdict errors)

1. **The "existing routing knowledge" false premise.** m23's brief required hide-type identity to
   come from routing knowledge that **did not exist** — `bHasDeferredHidden` is set unconditionally
   and sampling is type-blind. Intent survived; the source had to be invented.
2. **The micro-plan contradiction.** It named `:237` and `:303` as "both call sites" *and* ruled
   auto-pool untouched — but `:237` **is** the auto-pool path. Code took the conservative branch
   and flagged it. **Corollary earned: when a brief contradicts itself, take the conservative
   branch and flag it — never pick silently.**
3. **The unsatisfiable diff-confinement rule** → **A58**: diff-isolation rules are stated as
   *invariants to preserve* (which files must **not** change), never as *confinement predictions*
   (which files **may** change). The rule's author does not always know the call graph.
4. **A20 item 4 carried as owed for ten days** after it had been discharged in `fbf8ad1` — the
   stale carry propagated through the handoff, journal 030, Code's drafts, and chat verdicts.
5. **The counter-catch story** — chat credited the A41 execution counter with catching the dead
   render lever. The counter was never in that binary; ratio arithmetic made the catch.
   *A counter that never printed is not a counter that printed 0.*
6. **The camera-block suspicion** (this turn) — the equality came from the bounds side, not the
   camera side. The 1010 cube was the real tell.

### Code-side (self-corrected, all before they caused damage)

- The counter-catch story, corrected against its own report.
- **False ABSENT on our own fix** — caught in-turn, overturned in the required order.
- **"Weak but aligned"** — superseded; positivity is not alignment.
- **P4-candidate** flagged, then killed with evidence and withdrawn.
- The 60–90 fps expectation, pre-stated and falsified.

**Both sides putting their own errors on the record is why the corrections sections are where a
cold reader should look first.**

---

## 7. Amendment index — A44 to A58

| # | Ruling |
|---|---|
| **A44** | **Binary provenance = symbol/string scan of the artifact under test (or embedded version stamp). Timestamps ADVISORY ONLY — they mislead in both directions.** "The harness ran" is never "the change ran" |
| **A45** | Marker validity = a **strictly increasing** decoded series over the analysis window. A lone successful decode is never evidence a marker was drawn (the decoder confidently misreads markerless frames) |
| **A46** | Harness process hygiene: kill by process **NAME**, assert a zero-instance idle box before every launch (the 217 KB launcher trap already produced one wrong ratio) |
| **A47** | Per-leg settled bbox + settle window. Camera rest position **bifurcates run-to-run**; no design may assume a fixed bbox across legs. G89's "deterministic" is content-deterministic, not camera-deterministic |
| **A48** | **Config echo** — report the *effective* value via multiple independent read-backs, never the value you set. A silently clamped setting must not be able to fake a result |
| **A49** | Pre-declared **regime windows** replace A40 bands where A40's bands are unreachable (the HF set) |
| **A50** | Per-event outcome taxonomy: **ALIGNED / SHIFTED(N) / ABSENT**. *Addendum:* events clipped by the frame cap are **TRUNCATED** and excluded from counts |
| **A51** | **Signature kit** — any manifestation/alignment claim requires all three: marker↔label identity, adjacent-duplicate scan, in-bbox deviation vs clean baseline in robust sigmas. One alone is not a classification |
| **A52** | **fps scoping** — manifestation results are VideoFps-scoped. Clean at one fps licenses nothing at another, in either direction |
| **A53** | Any oracle/analysis change re-verifies against one **known-ALIGNED** and one **known-ABSENT** control before its results are used |
| **A54** | Certification oracle = **local contrast** vs the two non-claimed frames flanking the **event** (not per-frame ±2 — that is unevaluable on contiguous claimed sets). TAU frozen from controls before any leg is read. K=6 robust sigma demoted to reported diagnostic |
| **A55** | **Decidability** — every verdict carries its deciding margin; verdicts are *annotated* decidable/undecidable, never reclassified. **The floor itself is currently NOT DERIVABLE — see A57** |
| **A56** | **Camera-certifiability** — a leg is A54-certifiable only under calibration-like camera conditions (modal-crop coverage ≥90 % of label rows, ≤3 distinct bboxes). Gates the **pixel** oracle only; annotation-shape evidence is camera-independent |
| **A57** | **Floor-robustness / bracket-vs-contain** — where defensible calibration constructions disagree, only verdicts **invariant across all of them** certify. A regime-specific floor requires a known-answer control **in** that regime; bracketing does not count |
| **A58** | Diff-isolation rules are stated as **invariants to preserve**, never as **confinement predictions**. *Corollary:* when a brief contradicts itself, take the conservative branch and flag it |

### Gotchas landed: G90–G96

**G90** launcher-process trap · **G91** `TryFireSpecific` `=` prepend + editor-only actor labels ·
**G92** compiled-but-never-staged (with the scan one-liner, the bank path, and the
archive-wipes-`Saved` hazard) · **G93** focus-gate × fixed-timestep camera corruption
(**extended:** intermittent at 240 fps *even with the gate ON* — the 2×2 was incomplete) ·
**G94** the `:1467` fallback landmine · **G95** dual-capture SVE write load starves production's
writer · **G96** one principle, three blindness instances (fixed-K under drift / neighbour-window
LC on contiguous sets / bracket-not-contain floor non-derivability).

---

## 8. Forward plan

**Nothing is queued.** The next call is open. Sequenced by my recommendation:

1. **P6 bounds side — ~1 minute, editor already open.** Inspect `BP_Bot_C_0`'s component list to
   confirm what the 1010 cube is. Cheapest evidence available on a client-shipping field.
2. **P5 founding instrument — the blend-ladder.** Build the known-ground-truth spread control from
   the certified 30 fps leg. It is the only route to (a) a contained-regime decidability floor,
   which (b) unblocks **60 fps certification**, and (c) gives P5 its first real measurement.
   Discriminators pre-declared chat-side **before** it is built.
3. **S3** — B′ into `AnomalyCapture` behind a default-OFF switch, colour only, full ratio × config
   matrix on the real paced path. **This is where ratio-independence is discharged.** Now built on
   a label path that no longer fabricates.
4. **P1** — still unreproduced. Remaining untested hypotheses, in order: **H1** (GPU-load
   starvation — no lever exists; lever design is chat-side first, never same-turn as its first
   measurement), the **delivery-mode gap** (both I10 sets and every HF leg ran delivery **OFF**,
   because the oracle needs the `labels.jsonl` bbox that delivery suppresses — while the client
   captures **in** delivery mode), and the **client-config/content audit** (owner lane).
5. **S4** depth · **S5** backbuffer demoted to the UI-on option.

**Owner-lane, gates client comms:** the **office-machine `target_fps` audit** of delivered client
sessions. If any session captured above 30, a precautionary "cap VideoFps at 30" note goes out
immediately (P5 means high-fps captures are degraded even with true labels). If all are 30, m23
folds into the feature reply with a fix timeline instead.

---

## 9. Open vs locked

### Locked (do not relitigate)

- **B′** as the keying design; key minted at `BeginRenderViewFamily` only; A8 latch rule; A3 never-assert.
- Stage numbering: **S3** = B′ behind default-OFF switch + gates on the real paced path;
  **S4** = depth; **S5** = backbuffer demotion. **P3-fix before S3** (done).
- **m23's mechanism and scope** — P3a + P3b as stated in §3; certification scope as stated in §2.
- **A44–A58**; A50's TRUNCATED addendum; A54 canonical (event-flank).
- **Ratio-independence is load-bearing.** No client is ever told their machine is too slow. The
  internal ship rule is telemetry only.
- **Content-clock default = wall.** Tested. Do not flip.
- **P4 permanently retired**; numbers are never reused (the m22 renumber hazard).
- CaptureBench **local-only, permanently**. `feature/stencil-capture`: **do not rebase** — mine
  Stage 3a on current master.
- `IAnomaly` **LOCKED** since M1.

### Open

- **P1** — unreproduced, unfixed, mechanism unknown. H1 and the delivery-mode gap are the live leads.
- **P5** — open; instrument assigned, not built. **No mechanism adopted; TAA is named and untested.**
- **P6** — open; bounds side one editor check from settled; camera-path naming is a contract question.
- **60 fps certification** — floor-blocked, deferred.
- **P3a threshold bookkeeping** — open, non-blocking (the fix removed the race).
- **A4 Condition 1 (VP equality)** — **UNSATISFIED**. ViewRing / `ViewLagFrames` deletion is
  contingent on it; keep a bisect switch when it lands.
- **A11** one clean non-zero ring-counter observation; **I2** re-measure; **A17/A19** audit
  (paper, both axes — new input: the banked calibration legs ran marker-OFF).
- **Deep starvation** — still open, does **not** drift toward "probably fine."
- Client-facing: reply unsent; invisible-anomaly fix unscoped (`feature/stencil-capture` is the
  principled cure); resolution selection / JPEG / defaults profile unbuilt.
- `CHAT-HANDOFF-m10-m21.md` still absent from the repo.

---

## 10. Corrections — discard this stale understanding

- **"The execution counter caught the dead render lever"** → **no.** The counter was never in that
  binary. Ratio arithmetic made the catch.
- **"The render lever was never built"** → **no.** It was built and *never staged*. You held a
  green build (G92).
- **"speed_ratio is blind to render-side starvation"** → **REFUTED by measurement.** It responds
  with the same functional form as game-side, with a larger residual.
- **"speed_ratio tells us which thread starved"** → **never.** Frame time only (cannot-attribute).
- **"A20 item 4 is owed"** → **discharged** in `fbf8ad1`, ten days before anyone noticed.
- **"The high-fps defect is the m21 stale-present residual"** → **no.** Identity perfect, zero
  duplicate frames. It is P3, a different phenomenon.
- **"The SVE migration will fix the labeled-but-invisible problem"** → **not P3, measured.** D-B:
  both grab points agree to four decimals.
- **"m23 makes high-fps captures good"** → **no.** It makes their labels **true**. P5 means the
  anomalies there are still faint and spread. High-fps capture quality is an open problem.
- **"Production is byte-unchanged"** → **retired at m23, by design.** The invariant is now
  "production changes only via approved milestone plans."
- **"The camera block is mis-sourced"** → **no.** It is a genuine viewpoint; the bounds are the
  problem (P6).
- **"A PIE smoke is validation"** → **no.** G76 stands. The owner smoke is a sanity gate; m23's
  certification evidence remains the packaged BenchGate legs.

---

## 11. Pointers

**Plugin repo:** `CLAUDE.md` (Current-status), `docs/gotchas.md` (**G90–G96** new; G43, G76, G86–G89
relevant), `docs/sessions/` **journals 031–034** (031 = I10 both levers + render provenance;
032 = P3 reproduced at high fps; 033 = mechanism adopted; 034 = the m23 fix, the A55
non-derivability saga, the smoke addendum, the P6 evidence pack), `docs/capture-fps.md`
(cannot-attribute · dial-becomes-readout · P3 dataset-poisoning warning · 60 fps deferred),
`docs/client-delivery.md`, `docs/architecture.md`, and the two companion handoffs
(`CHAT-HANDOFF-s2-keying-design.md`, `CHAT-HANDOFF-s2-gate-env-and-i10-setup.md`).

**CaptureBench (local-only):** `tools/make_gate_level.py`, the SVE probe sources, the marker
implementation and its Python decoder. Frozen-by-preference discipline continues.

**Banked evidence:** `D:\IntrusiveAnomalies\_bench_sessions_bank` — 21 dirs / 5,614 MB, including
every I10, HF, sweep and discriminator session. **The archive step wipes the `Saved` tree —
re-bank before staging (G92).**

---

**Standing lessons, with this session's receipts:**
**Measure then design** — two chat-Claude models and one Code expectation died to measurement here.
**Eyes, then number, then benchmark** — the owner's `annotation.json` paste surfaced P6, which
nobody was hunting.
**A test that never summoned its subject proves nothing** — and a **guard that has never fired is
not a guard**.
**Only known-answer controls expose instrument blindness** — three times, in three different ways (G96).
**Hunt before you build** — D-B is the receipt: the migration would not have cured the bug we
were about to build it to fix.
