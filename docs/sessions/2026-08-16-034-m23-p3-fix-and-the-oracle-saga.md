# 2026-08-16 — 034 — m23: the P3 fix, and three ways an oracle can be blind

Commit **`2f74799`** on `master`, pushed. **NOT TAGGED** — the `m23` tag waits on the owner's
play-gate smoke. Bench: `CaptureBench` `163dd12`, **zero probe edits — ninth consecutive turn**
(every instrument here is Python/harness-side).

**Production changed this turn, by design.** The "production byte-unchanged" invariant that held
through all of S2 retires here; from m23 on the invariant is *production changes only via approved
milestone plans*.

---

## 1. What shipped

8 files, +103 / −34.

- **P3a — the blink clock.** `FAnomaly_Blinking` accumulated forwarded tick dt against a half-period
  in **seconds**. Under capture that dt **is** `1/VideoFps`, so above ~90 fps a window never
  accumulated a half-period, the toggle never fired, and the actor was **never hidden**. The
  half-period is now defined in **FRAMES** (`int32`, default **3**, clamped 1..600).
- **P3b — the label guard, the actual poisoning amplifier.** The annotation writer inferred hide-type
  from the sampling **outcome** and silently substituted `AffectedFrames` when nothing sampled hidden.
  Hide-type identity now comes from the **anomaly ID** (`IsHideTypeAnomaly`, sited beside the existing
  id→client table). Zero sampled hidden ⇒ **zero positives**, `manifested: false`, a loud per-event
  warning, and a session counter. An **unregistered id** falls back to old behaviour *with a loud
  warning* — a forgotten registration is noisy rather than silently clever.
- **Targeted-fire args.** The guard was untestable without a way to force non-manifestation, and no
  knob existed. Tokens after the target on `IAI.Capture.Start` are now forwarded verbatim to the
  anomaly's own parser. No tokens ⇒ the previous call byte-for-byte. **Auto-pool (`TryFireOnce`)
  untouched. `IAnomaly` untouched. `labels.jsonl` untouched.**

Schema additions, both additive: annotation event **`manifested`**, run_summary
**`non_manifested_events`**.

---

## 2. Certification, and exactly what it covers

| leg | disposition |
|---|---|
| **30 fps** | **CERTIFIED, floor-robust** — 12 decidable ALIGNED / 0 non-ALIGNED under **both** candidate floors; claimed sets byte-exact to the historical `[4,5,9,10]` |
| 60 fps | **FLOOR-BLOCKED** — deferred, *not failed*. 11 ALIGNED, median margin 0.02325 (~4.4× the clean-noise median 0.00523), one sub-noise coin flip (ev@88, margin 0.001) |
| ≥90 fps | **P5-BLOCKED** — single-frame alignment undecidable by the physics of the signal |

**The guard is proven in BOTH directions, and that pair is the certification:**

- **Negative test** (forced non-manifestation, `half_period_frames 40` > window), read from
  `annotation.json` rows and not the log: **8 events, `non_manifested_events=8`, `manifested:false`
  on all, `frame_indices` lengths all 0**, 8 loud warnings each naming the prior behaviour
  (*"previously this emitted 8 on-screen frames as positives"*). The arg demonstrably reached
  `Apply` — `blinking: matched 1 actor(s) … at half-period 40 frame(s)`.
- **Paired control** (30 fps, default args): guard **silent**, zero warnings,
  `non_manifested_events=0`, `manifested:true` throughout.

⚠ **`positive_frames = 59` in the negative test is CORRECT AND EXPECTED.** That is the **fire-active**
per-frame counter — a fire genuinely was live — and it is deliberately unchanged.
**Fire-active ≠ manifested.** Manifestation truth lives at event level. And the client artifact
carries *neither* per-frame flag: `anomaly_present` exists only in `labels.jsonl`
(`AnomalyLabelWriter.cpp:51`), gated by `bWriteLabels` (`:121`, `:247`), which delivery mode sets
false. Event-level `manifested` is therefore the only channel to the client.

**Cross-build bridge:** the byte-exact 30 fps re-cert on the rebuilt binary satisfies the bridge
condition, so banked m23 evidence carries — P3b shapes at all fps, P3a state sampling, and HF2-d1.

---

## 3. The A55 saga — a floor that cannot be derived, and why that is a result

Two defensible constructions were built. **They disagree diametrically about the legs in question,
so choosing one would have been choosing the verdict.**

- **Construction A** (pseudo-events = real event shapes translated into clean territory on the two
  ALIGNED controls): n=17, median 0.04602, **p99 = 0.06371**. **CONTAMINATED** — and the median being
  ≈TAU is the tell. A pseudo-event can sit entirely on clean frames while its **±1 shifted variants
  land on real hidden frames**, inflating `score(±1)`. Unrepairable by placement: a shape-preserving
  all-clean window (event ±1 ≈ 9 frames) **does not exist at 30 fps**.
- **Construction B** (the genuine no-signal leg, pre-fix HF1): n=12, median 0.00010,
  **p99 = 0.00026**. Uncontaminated but measures the **wrong quantity** — with zero signal anywhere it
  is the statistic's numerical **precision**, not its **discrimination** noise under a real spread signal.

| leg | fps | median\|margin\| | under A (0.06371) | under B (0.00026) |
|---|---|---|---|---|
| R30 | 30 | 0.10737 | 12 / 0 | 12 / 0 |
| DA60 | 60 | 0.02325 | 2 / 0 | 11 / 1 |
| HF1 | 120 | 0.00481 | 0 / 0 | 1 / 11 |
| HF3 | 120 | 0.00432 | 0 / 0 | 1 / 11 |

Under A, DA60 is under-powered; under B, DA60 "fails" on a 0.001 coin flip and 120 fps becomes
*decidably SHIFTED* on 0.0005 margins — asserting a defect against the measured ~1:1 energy split.
**Only R30 is invariant across both, which is why only R30 is certified.**

### A57 — floor-robustness / bracket-vs-contain

> When defensible calibration constructions disagree, only verdicts **invariant across all of them**
> are certifiable. A regime-specific decidability floor requires a **known-answer control IN that
> regime**; a calibration set that **brackets** the regime without **containing** it cannot yield one.

Both constructions are retained here as the demonstration pair. The **blend-ladder** (adjacent-frame
blends of the certified 30 fps leg at 100:0 / 75:25 / 60:40 / 50:50, known ground-truth offset) is
assigned to **P5 as its founding instrument** and is the eventual source of a contained-regime floor
for DA60's deferred certification. Not built this turn.

---

## 4. Two chat-side errors, on the record — and A58

1. The plumbing micro-plan said *"both call sites (:237, :303) honour the contract"* **and**
   *"AUTO-POOL PATH UNTOUCHED"*. **`:237` IS the auto-pool path** (`TryFireOnce`); the brief
   contradicted itself. Resolution taken: follow the conservative half, change only `:303`, verify
   `:237` byte-identical, and flag it.
2. The diff-confinement rule (*plumbing confined to `AnomalyAutoInjectorSubsystem`*) was
   **unsatisfiable under the contract it accompanied**: args enter at `IAI.Capture.Start`, which lives
   in `AnomalyCaptureSubsystem`, so they cannot reach `TryFireSpecific` without passing through it.
   Not a violation — a bad rule. Amended to: the plumbing may touch the targeted-fire **input path**;
   the four label/blink files stay **byte-identical** (they are — verified per-file).

> **A58 — diff-isolation rules are stated as INVARIANTS TO PRESERVE (which files must NOT change),
> never as CONFINEMENT PREDICTIONS (which files MAY change).** The rule's author does not always know
> the call graph, and an unsatisfiable rule turns a correct stop into a wasted turn.
> **Corollary: when a brief contradicts itself, take the conservative branch and flag it — never pick
> silently.**

---

## 5. Supporting evidence banked this turn

- **Window-shrink probe** (`IAI.Capture.Config 0 2 2 2 0`, i.e. a positive window shorter than the
  3-frame half-period): 15 events, **all `manifested:true`, `frame_indices` length 1** each
  (`[2]`, `[6]`, `[10]`). The object is **already hidden by the first positive frame** — the toggle
  lands during Apply/settle, before the window opens. Recorded as **P3a-phase evidence**; no
  arithmetic adopted, **P3a threshold bookkeeping stays OPEN and non-blocking**. Incidentally proves
  length-1 positive sets emit correctly.
- **HF2-d1**, camera-independent annotation-shape evidence at **240 fps natural**: pre-fix
  **13/13 fully-consecutive** (the P3b fallback fingerprint) → post-fix **12/13 gapped**
  (`[4,5,9,10]` …), the one consecutive being the truncated tail event; `manifested` all true;
  counter 0. **P3b fixed and P3a state-real at 240 fps without needing a valid camera.**
- Oracle-blindness instances (i)–(iii) → **G96**.

---

## 6. Standing

**P1** not reproduced · **P2** signature absent · **P3** fixed (this milestone) · **P4** permanently
retired, number never reused · **P5** queued with its founding instrument assigned.
**H1** and the **delivery-mode gap** unchanged and untested. **A47** stands. Client-facing wording
untouched — comms are owner-lane, gated on the owner's fps audit.

**Tag `m23` is held for the owner's play-gate smoke.**
