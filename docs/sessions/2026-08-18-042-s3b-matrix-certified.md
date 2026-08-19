# 2026-08-18 — 042 — S3b: the ratio × config matrix. S3 CERTIFIED, tagged `m24`.

**Plugin:** `AnomalyInjector` — docs-only this arc; production code unchanged since S3a-3.
**Bench:** `CaptureBench` — four tool commits, local-only repo. **Probe untouched throughout.**
**Bank:** 58 dirs.

**S3 is certified. Ratio-independence is discharged. Delivery mode is orthogonal. `P1` is still open.**

---

## 1. What certified

| stage | result |
|---|---|
| **Stage 1** | marker survives an SVE production capture — 90/90 decoded, strictly increasing, ~5× the decoder floor |
| **Stage 2a** | the A54 oracle **rebuilt from prose** and certified against **eight** banked controls |
| **Stage 2** | five legs — **33 counted events, 33 ALIGNED, 0 SHIFTED, 0 ABSENT, 33/33 decidable** |
| **Stage 3** | three delivery pairs — **zero extras**, invariant core **identical** |

**Both pre-declared predictions HELD.** They were written before any leg existed and are restated
verbatim in every report of this arc:

> **RATIO-INDEPENDENCE HOLDS** … B′ keys by IDENTITY, not by order, so the arm→present race that
> produced the −1 has no positional step left to fail on.
>
> **DELIVERY MODE IS ORTHOGONAL** … delivery gates DISK WRITES and not compute.

### The matrix

| leg | stall | pace | ratio | band | ev | verdict | med \|margin\| | dec |
|---|---|---|---|---|---|---|---|---|
| L1 | 0 | on | 0.999914 | nominal | 7 | ALL-ALIGNED | 0.107745 | 7/7 |
| L4 | 99 | on | 3.030798 | deep ≥2.80 | 5 | ALL-ALIGNED | 0.110588 | 5/5 |
| L2 | 40 | on | 1.254839 | client | 7 | ALL-ALIGNED | 0.110870 | 7/7 |
| L3 | 39 | on | 1.239722 | client | 7 | ALL-ALIGNED | 0.108453 | 7/7 |
| L5 | 0 | **off** | 0.332742 | pacing-off | 7 | ALL-ALIGNED | 0.105930 | 7/7 |

**In-leg positive control on every certified leg, both directions, reported BEFORE each verdict:**
`+1` → every event flips to `SHIFTED(-1)`; `−1` → every event flips to `SHIFTED(+1)`. **No ALIGNED
verdict survived any synthetic shift on any leg.** The oracle was not blind on this material.

### Delivery pairs

| pair | OFF leg | ON leg | difference set | extras |
|---|---|---|---|---|
| D1 | `L1_nominal` | `D1_nominal` | run-unique + `delivery_mode` | **0** |
| D2 | `L2P_client40_a1` | `D2_client40` | run-unique + `delivery_mode` | **0** |
| D3 | `L3P_client39_a1` | `D3_client39` | run-unique + `delivery_mode` | **0** |

Run-unique sets were established **empirically, per regime**, from control pairs of two OFF legs at
identical config — not argued from source. `video/fps` and `stamped_fps` join the set only in the client
band, because m11's honest-fps stamp activates above ratio 1.02.

**The invariant core was asserted POSITIVELY, not inferred from the differ's silence.** *"We found no
differences"* and *"we checked these things and they are identical"* are different claims and only the
second is evidence. Identical on all three pairs: event count · `frame_indices` ·
`affected_frames` start/end/count · `manifested` · type/subtype · node bounds · node name/path/asset ·
`coverage_ratio`/`coverage_pct` · the camera block · 15 `run_summary` invariants.

> **B′ behaves IDENTICALLY in delivery mode, not merely acceptably** — all five `key_ring_*` counters
> and `capture_path` are identical across every pair.

> ⛔ **CORRECTION TO m24 (surfaced by S4-4's control pair, 2026-08-19): of the five `key_ring_*`
> counters, only `missed` and `corrupted` are invariant across runs. `published` / `consumed` /
> `wrapped` are RUN-UNIQUE — they count view families rendered before capture begins. m24's verdicts
> are UNDISTURBED; they rest on `missed == corrupted == 0` and on the file set, not on the publish
> count. The claim as written did not generalise.**
>
> It was true of *these* pairs. It became a general invariant only in the prose. **This is the second
> time this project has found a certified claim resting on prose rather than on a measurement** — the
> first was **G106**, where the grading instrument itself existed only in prose. Here, S4-4's control
> pair had already shown the three counters varying *before* the test diff was read, and the first
> invariant list written against it repeated m24's wording anyway. **The control pair is authoritative
> over the prose.** → journal 044 §6.5.

---

## 2. Stage 2a — the oracle did not exist, and rebuilding it was the risk

**Every frame-alignment certification this project holds — I10 and m23 — was graded by scripts that were
never committed.** TAU and the canonical definition lived only in prose. The results are not wrong; they
were **not reproducible**. → **G106.**

Rebuilding from the written definition left **two load-bearing points under-specified, and the
attractive reading was wrong on both:**

| point | attractive reading | what reproduces |
|---|---|---|
| do event flanks move with the shift hypothesis? | **fixed** — "they are the clean baseline" | **they move.** A wrong shift must drag a hidden frame into its own reference so its score collapses |
| which shifts are scored? | −2…+2 | **−1, 0, +1 only.** With ±2 admitted, ±2 becomes the runner-up |

Either error alone takes R30's median margin from the published **0.10737** to **0.0548 / 0.0505**, and
its decidability from **12/12** to **0/12** — *while leaving every per-event verdict ALIGNED and the
headline "12 ALIGNED / 0 non-ALIGNED" intact.*

**Both were caught only by reproducing published NUMBERS, not published CONCLUSIONS.** The gate clause
demanding quantities rather than verdicts was written as belt-and-braces; it was the entire gate.

**This is G96's principle a fifth time and the sharpest.** The previous four were instruments *blind* in
a region — they eventually surface as unevaluable results. This one is not blind; it is **confidently
wrong about how much it knows**, and it surfaces as **nothing at all**.

### The settle window — 4 of 6, and the misses are the finding

Constants **frozen before the rule was run against anything**: `SETTLE_K = 5`, `SETTLE_TOL_DEG = 0.5`.
*(Disclosure recorded in the tool: L3's rotation series had already been observed, so a start near 30 was
foreseeable. The constants are insensitive to that — any K in 3..10 gives the same answer.)*

Reproduces journal 031's per-leg windows on **L2/L4/L5/L6**. Two misses:

- **L1 is structurally unreachable by any camera rule** — its camera has **one distinct rotation across
  all 90 frames**. Its published `16` is the ~570 ms **luminance ramp** (570/33.3 ≈ 17).
- **L3 is off by one** (29 vs 30) — a boundary convention.

**A competing hypothesis was tested rather than assumed:** a luminance-settle rule matches the published
windows on **zero** legs. ⇒ **journal 031's windows are neither camera nor luminance but a per-leg
hand-chosen mixture, selected after seeing each leg.** K was **not** tuned to close the gap.

### P8 — and the ABSENT was FALSE

L3 re-gated: A56 passed (97.6 %, 2 distinct) and the oracle returned **ABSENT on all 4 counted events**.
**Reading the raw series before reporting** showed the hide is real:

    claimed 0.81534–0.81755 · non-claimed 0.85335–0.85525 · ZERO overlap
    every claimed frame on the low side · d = −0.03828 · TAU = 0.04684

Perfectly separated, perfectly aligned, and called ABSENT purely because `0.0383 < 0.04684`. Same set,
same binary, same target, same seed, **modal** pose (`L6`): **+0.11260**.

**Cause:** TAU is an **absolute** luminance difference, so it silently inherits the A47 **camera-pose**
dependency of that quantity — the rest pose sets both the bbox and the background behind the target.
**A50 treats ABSENT as reproduction of the defect, so this fails in the DANGEROUS direction.** → **G107.**

**The sign flip is independently corroborated:** journal 031 recorded L3's *"hidden side: low"* against
*"high"* everywhere else, months before this instrument existed, by someone not looking for it.

**B1 adopted** — A56 gains a **pose-match precondition** (`CALIB_BBOX`, `POSE_TOL_PX = 8.0`, both frozen
before application). It **does not make any leg pass**; it converts a false ABSENT into an honest
NOT-CERTIFIABLE. Re-gate: all seven modal controls **unchanged**, L3 flips as intended.

**Sign handling verified, not assumed:** the oracle thresholds on **magnitude** — L3's raw signed
difference is −0.03828 and the reported score is +0.038280.

---

## 3. Stage 2 — and P8's guard firing twice more within one turn of being ruled

On their first run **L4 and L3 both landed in bifurcated poses**. L4 would have produced a **false ABSENT
on the deep leg**. **Ruling B1 one turn earlier is the difference between this report and a false red on
the headline.** L3's camera drifted so far the target left frame — **zero bbox rows** after the settle
window.

Re-run under the pose gate, **both landed modal on the first attempt** and certified.

> **BIFURCATION IS 2-IN-5, NOT 1-IN-12.** The prior was wrong by a wide margin, and **both instances were
> on STALLED legs.** The stalled-leg association is recorded as an **association only** — nobody adopts a
> mechanism for it without measurement.

### The harness, and two corrections it forced

**G108 — a stalled process fails foreground activation.** The deep leg rode the 30 s gate **three times**
(`start_frame` 298/299/300 = exactly 30 s at ~100 ms/frame) and the harness declared an **environmental
halt**. It was not environmental — it was the harness. A thread busy-waiting 99 ms/tick reads as
unresponsive and Windows' foreground lock refuses `AppActivate`. ALT tap + direct `SetForegroundWindow`
fixed it: **frame 1, 0.1 s, first attempt.** The focus gate itself is untouched (**G93** stands).

> **An "environmental" halt that only fires on the slowest configuration is not environmental.**

⛔ **Virtual-desktop isolation investigated and REJECTED** — it *removes* foreground focus, which is
exactly what the gate waits for; it would **guarantee the timeout it was meant to prevent.**

**G109 — a frame-count threshold cannot generalise.** The 30 s gate expires at ~900 frames nominal,
~726 at stall 40, **~299 at stall 99**. `start_frame > 100` was simultaneously too loose for one regime
and only accidentally right for another. **Time is the invariant; frames are not.**

### Debts touched

- **A10 DISCHARGED AT EVERY REGIME, by marker, not inference** — `frame_index − decoded_marker = {0}`,
  one distinct value, all five legs, 90/90 decoded and strictly increasing. Strictly stronger than S3a's.
- **Ring under stall DISCHARGED** — `published == consumed`, `missed = 0` on every leg including deep
  3.03 and pacing-off.
- ⛔ **A11 STAYS OPEN.** No natural miss occurred. Recorded precisely: **the design prevented the
  condition A11 wanted to observe.** That is a satisfying reason to leave a debt open and it is **not**
  written up as a discharge. `wrapped > 0` does not close it; `ForceMiss` never will.

### Compliance failure — the two lost bifurcated legs

The instruction was *"bank every bifurcated leg."* **The instruction was in force and the code was not** —
bank-every-attempt landed *after* those two runs. Both were overwritten.

**Survives:** the full A56 measurement of both (modal rotation, bbox, coverage, distinct count, L3's
zero-bbox-rows finding). **Lost:** the pixels, and with them the ability to re-grade those legs under a
future instrument — which **B2 would have wanted**. Bounded, because `L3_client39` already serves as the
pose-regime control and the regime is cheap to resample at 2-in-5. **Not fished for a replacement.**

---

## 4. Stage 3 — the apparent divergence, and the control that was already in hand

D2/D3's first comparison showed **35 and 27 differing annotation fields** — `camera/rotation` 359.65→0,
`coverage_pct`, `coverage_ratio`, `engine/ticks_msec`. On the client band. That is exactly the shape that
panics people into declaring a divergence or re-running until it disappears.

**Neither was needed. The discriminator already existed:** D1's pair happened to land **pose-matched**
and showed **zero extras**. If delivery mode moved coverage or the camera block, **D1 would have shown it
too.** A control **recognised rather than manufactured** — no new runs, no new instrument.

Pose-matched OFF partners were then run; both landed at rotation 0 on the first attempt and the
difference sets collapsed to run-unique + `delivery_mode`.

### What delivery mode breaks, and how each was handled

**`run.json` is suppressed**, so `start_frame` — the quantity A63 has always been checked on — **does not
exist** on a delivery leg. Reconstruction (`end_frame` minus a schedule-derived span) was available and
**forbidden by A60**. Focus acquisition is instead measured **live and externally** — A60's
**operator-supplied** branch, in its **first real use** since it was written during the cancelled auditor.
It turned out **strictly better than the artifact proxy it replaced**: it works identically in both modes.
Measured: D1 1.5 s · D2 1.6 s · D3 1.6 s.

**`labels.jsonl` is suppressed**, so `view.rot` and `bbox_px` do not exist and **the camera pose is
unknowable from a delivery artifact.** The B1 gate cannot run — consistent with A56's scope, since a
delivery leg has no pixel oracle to constrain — but it is why the pose confound was invisible until the
diff was read.

**Scope limit honoured:** Stage 3 is **annotation-shape evidence only**. The pixel oracle did not run on
any delivery leg, and the bbox series was **not** transferred from the paired OFF leg — §4 is the concrete
demonstration of why A47 forbids it: the two legs were not even in the same pose.

### LastRunDir — DISCHARGED

Verified **post-run**, against a capture already finished on disk (A62): `running: false`,
`runDir: LRD_probe/session_20260818-215041`, non-empty, and naming the session that exists.

Small as scoped — reuses the existing `_ws-gate.ps1` precedent (`System.Net.WebSockets.ClientWebSocket`)
rather than hand-rolling a raw-socket handshake, because Python stdlib has no WebSocket client.

⚠ **Trap recorded:** the control-server handshake message is **`hello`, not `auth`**
(`AnomalyControlServerSubsystem.cpp:343`). An `auth` message is silently dropped and the socket aborts —
which reads exactly like a bad token and is not.

---

## 5. New standing rule — A64

| # | Ruling |
|---|---|
| **A64** | **A delivery-pair comparison requires a POSE-MATCH PRECONDITION ON THE PAIR**, not merely per-leg B1 admissibility. Two legs can each pass B1 and still sit in **different admissible poses** — 0.35° apart, inside tolerance, enough to move `coverage_ratio` ~1.9 % and read as a divergence. **B1 constrains each leg against CALIBRATION; nothing constrained the pair against EACH OTHER.** Use `coverage_ratio` as the observed pose indicator — **a discriminator for the comparison, never a gate** (a delivery artifact cannot report pose directly and no reconstruction is permitted). Same family as A47's non-transferability between legs |

**Also ruled this arc:**

- **≥ 3 COUNTED EVENTS PER LEG is a validity condition**, joining A31's. It was I10's minimum and remains
  it. A leg below 3 is **INVALID — not evidence, whatever its verdicts say.** The settle window scales
  with frame time (at 100 ms/frame it dropped 24 leading frames and 2 events, leaving L4 at 5), so there
  is a stall depth at which a leg silently drops below the bar while still reporting ALL-ALIGNED. Report
  counted events per leg alongside TRUNCATED and PRE-SETTLE counts, always.
- **A40 band reading rule** — bands are compared at their **own stated precision (2 d.p.)**. The earlier
  "4 s.f." wording does not generalise: it gives 1.0000 for 0.99999935 but 0.9999 for 0.99991396, putting
  a textbook nominal leg out of band on 0.009 % of pacing noise.
- **The BOM in `a871c81`'s subject is fix-forward, NOT amended.** An unamendable blemish in the log is
  worth more than a clean log with rewritten history. **The standing no-force-push rule is confirmed, not
  excepted.**

---

## 6. `P1` — NARROWED, NOT SOLVED. This outranks the green.

**S3 GOING GREEN DOES NOT CLOSE P1.** P1 has never been reproduced, and you cannot demonstrate a fix for
something you cannot summon. A clean matrix proves the new path does not carry the **OLD** race. It is
**not** evidence it cures her defect.

**P1 had two live leads. One is now eliminated by measurement:** the **delivery-mode gap is CLOSED as a
divergence hypothesis** — delivery mode changes nothing in the annotation contract, so whatever produced
the client's −1, **it was not delivery mode.**

> ⚠ **H1 (GPU-load starvation shape) is P1's ONLY REMAINING NAMED LEAD, and H1 HAS NO LEVER IN EXISTENCE.**
> **If H1 also comes back clean, P1 HAS NO NAMED LEADS.** Worth knowing now, while there is still a queue
> in front of it, rather than discovering it when H1 finishes.
> Lever design for H1 stays **chat-side first, and never same-turn as its first measurement.**

---

## 7. Scope limits of `m24` — these travel with the tag

- **MODAL CAMERA POSE ONLY.** A defect manifesting only in a bifurcated pose would systematically not be
  seen by this design. Nothing reachable was lost — such a leg cannot be graded either way — but the limit
  is real.
- **A52 — `VideoFps` 30 pinned throughout.** Clean at 30 licenses **nothing** at other fps, in either
  direction.
- **Stage 3 is ANNOTATION-SHAPE EVIDENCE ONLY.** The pixel oracle cannot run in delivery mode.
- **The oracle's own scope: certified at 30 fps; MARGINS are not reproduced above it** (**P7**).
- **S3 GOING GREEN DOES NOT CLOSE P1.**

---

## 8. Debts — complete and current

| debt | status |
|---|---|
| **A11** | **OPEN** — the design prevented the condition it wanted to observe. `wrapped > 0` does not close it; `ForceMiss` never will |
| ~~I10 hand-chosen-window re-check~~ | ✅ **DISCHARGED — §10.** Null SURVIVES on five legs covering every band I10 claimed; the hand-chosen windows were excluding **startup marker noise, not defect evidence**, and the mechanical window is **stricter** on L1 with no verdict change. ⚠ **Client band narrowed from two legs to one** — see §10 and the row below |
| **Client-band thinness (new, from §10)** | I10's refutation of CPU starvation **at ~1.2 — the band `P1` actually lives in — now rests on ONE certified leg (`L6_client40`) plus one honestly unjudgeable leg (`L3_client39`).** Overturns nothing and is not a defect; it is the **thinness of the evidence, stated now** rather than discovered later if H1 also comes back clean |
| **Journal 031 arithmetic corrections (new, §10.3)** | `532/534` → the corpus is **540**; the "only two exceptions … in L3" is **eight**, in **L1 (5) and L3 (3)**. Reconciled against journal 031's own per-leg table, which reproduces this re-check exactly. **Both sites are now annotated IN PLACE in journal 031** (§4.5 corrected, §9.2 flagged) so a cold reader hits the note where the figure is, not only here. ⛔ **`1052/1054`: the denominator carries the same arithmetic slip as `532/534`. True corpus 1080. The RENDER HALF HAS NOT BEEN RE-MEASURED, so NO CORRECTED FIGURE EXISTS. Do not cite `1052/1054` without this note.** This is a **FLAG, not a correction** — re-measuring the render half stays OPEN and out of scope |
| **P7 / P5 blend-ladder** | serves **two** open items; priority raised. It manufactures **spread**, not pose |
| **P8 / B2** | scale-free separability statistic — **FILED, NOT SCHEDULED.** Now **gateable against eight controls** including `L3_client39` (known-ALIGNED, bifurcated pose). 🆕 **B2's payoff now includes RECOVERING `L3_client39` and restoring the client band to two legs** — a concrete gain it did not have when filed. Still a definition change needing its own eight-control gate, and it may reopen m23's DA60 floor, so it does not ride inside another milestone |
| **The two lost bifurcated legs** | **recorded as lost**, not silently dropped |
| **H4** | occlusion-blind labelling — filed, test designed, **unrun** |
| **P6** | `camera.path` naming; `coverage_pct` vs `coverage_ratio` — **predicted from source, never measured** |
| `ForceMiss` phase-lock coarseness · `video.total_frames` vs index range under partial loss | unchanged; the latter deliberately not fixed |

---

## 9. State

- `AnomalyInjector` — docs commits only; **production code unchanged since S3a-3**. Tagged **`m24`**.
- `CaptureBench` — `a54_oracle.py`, `run_leg.ps1`, `eval_leg.py`, `check_pose.py`,
  `verify_lastrundir.ps1`. Local-only. **Probe untouched all arc.**
- **Bank 58 dirs** — five certified Stage-2 legs, three Stage-3 delivery legs, three pose-matched OFF
  partners, the Stage-1 marker leg, three banked A63 focus-timeout discards, the P8 control record.
- Staged exe unchanged, SHA-256 `3BA854FB…`.

**Next: S4 — the backbuffer demotion. NOT planned this turn.** ⚠ Per **C2** it is a **client-visible
change, not a silent default flip**: the pre-Slate SVE grab is **UI-free by construction**, so flipping
the default **changes delivered image content**. **Depth remains PARKED and UNNUMBERED.**

---

## 10. The I10 window re-check — **the null SURVIVES, narrowed by one leg**

The six banked I10 legs re-run through the **mechanical** settle rule (K=5, tol=0.5°, unchanged) and the
**certified** oracle. All six are 30 fps, so the oracle's certified range covers them and **P7 does not
apply**. Paper only — banked data, no runs.

| leg | ratio | band | window | pose | ev | verdict | med \|margin\| | dec |
|---|---|---|---|---|---|---|---|---|
| L1_nominal | 1.000006 | nominal | 0..89 | YES | 7 | ALL-ALIGNED | 0.108140 | 7/7 |
| L2_client34 | 1.055830 | mild | 0..89 | YES | 7 | ALL-ALIGNED | 0.110411 | 7/7 |
| **L3_client39** | 1.214760 | client | 29..89 | **NO** | — | **NOT-A54-CERTIFIABLE** | — | — |
| L4_deep99 | 3.002709 | deep | 0..89 | YES | 7 | ALL-ALIGNED | 0.108624 | 7/7 |
| L5_paceoff | 0.331247 | pacing-off | 0..89 | YES | 7 | ALL-ALIGNED | 0.110169 | 7/7 |
| L6_client40 | 1.234177 | client | 0..89 | YES | 7 | ALL-ALIGNED | 0.111510 | 7/7 |

In-leg positive control on every certifying leg, both directions, before its verdict: `+1` → ALIGNED 0 /
SHIFTED 7; `−1` → ALIGNED 0 / SHIFTED 8. **No ALIGNED verdict survived any synthetic shift.** Settle
constants were **not** re-tuned. Every certifying leg has 7 counted events, clearing the new ≥3 floor.

⇒ **CPU STARVATION REMAINS REFUTED as a cause of P1. The weakening is DISCHARGED** — I10's null no
longer rests on hand-chosen windows.

### 10.1 The hand-chosen windows were excluding startup noise, not defect evidence

A45/A10 failed on L1 and L3, so it was **quantified rather than flagged**:

| corpus | bad rows | rate | where |
|---|---|---|---|
| **I10 (backbuffer, pre-S3)** | **8 / 540** | 1.48 % | L1 idx 0,1,2,3,4 (+1,+2,+3,+4,+7) · L3 idx 0,12,16 (+1,−32,−96) |
| **S3b (SVE)** | **0 / 450** | 0.00 % | all five legs clean |

L1's five bad rows sit at indices **0–4**, which journal 031's hand-chosen window (16..89) excluded and
the **mechanical window (0..89) INCLUDES** — and **L1 still returns ALL-ALIGNED, 7/7 decidable.**

> **The mechanical window is STRICTER on L1 than the hand-choice was, and the verdict does not move.**
> The feared direction was a window excluding *defect evidence*. What it actually excluded was
> **startup marker noise**. This **strengthens** the null rather than qualifying it.

⚠ **RECORDED AS A LEAD, NOT AN ATTRIBUTION — do not let a later reader promote this:** the SVE set shows
**zero** such rows in 450 frames. That is *consistent with* B′ removing an arm→present startup race, but
**the two sets differ in BINARY as well as capture path**, so it is not a claim about B′.

### 10.2 ✅ B1 PASSED A KNOWN-ANSWER CASE — recorded as a control that passed

`L3_client39` is the banked **known-ALIGNED, bifurcated-pose** control (its hide is real, perfectly
aligned and perfectly separated — see the P8 record). Under B1 it returned **NOT-A54-CERTIFIABLE, not a
false ABSENT.** That is **precisely the property B1 was adopted for, exercised on the case it was
adopted against.** It is a control that **passed**, not merely a leg that dropped out.

### 10.3 Journal 031's `532 / 534` — **BRANCH R: it reconciles, and my proposed explanation was WRONG**

Journal 031 §4.5 published, verbatim:

```
label.frame_index - marker_gfc  ==  0   on  532 / 534 decoded frames
per leg: L1 85, L2 90, L3 87, L4 90, L5 90, L6 90.
The only two exceptions are pre-window warm-up frames in L3 with no marker drawn
```

**The arithmetic:** `85 + 90 + 87 + 90 + 90 + 90 = 532` matching. Those per-leg counts are over **full
90-frame legs**, not over its analysis windows — decisively, **L1's 85 and L3's 87 both EXCEED their own
published window sizes** (74 and 60 frames). The corpus is therefore `6 × 90 = 540`.

**Implied non-matching, leg by leg: L1 5 · L2 0 · L3 3 · L4 0 · L5 0 · L6 0 — IDENTICAL to this
re-check's independent measurement.** The measurement reproduces exactly.

⇒ **Two errors in journal 031, both internal to it and both now corrected:**

1. **The denominator `534` is an arithmetic slip.** It is `532 + 2`, written to match the
   "two exceptions" narrative. **The corpus is 540**, and its own per-leg table says so.
2. **"The only two exceptions … in L3" is wrong.** There are **eight**, and they are in **L1 (five) and
   L3 (three)** — L1 is not mentioned at all.

⛔ **MY OWN PROPOSED EXPLANATION IS WITHDRAWN.** I proposed that the 534 arose because journal 031's
windows filtered its denominator. **Its own per-leg data refutes that** — the counts are full-corpus.
The gap was arithmetic, not methodological. *Recorded because the wrong explanation was plausible,
fitted the narrative, and would have entered the record unchallenged had the reconciliation not been
demanded.*

⚠ **The same slip propagates to §9.2's combined `1052 / 1054`.** The combined corpus is `1080`
(two sets × 6 legs × 90). **The render half is NOT re-measured here** — out of scope for this turn — so
the combined figure is flagged, not corrected.

**None of this disturbs journal 031's conclusions.** The matching count 532 is right, the per-leg
distribution is right, and the identity claim holds in every regime.

### 10.4 The positive-control count asymmetry — a frame-cap boundary effect

`+1` counts 7 events, `−1` counts 8. The cause is the tail event `[88,89]` alone:

- **shift 0** → `[88,89]`, trailing flank 90 — **absent** ⇒ TRUNCATED, 7 counted.
- **shift +1** → `[89,90]`, trailing flank 91 — **absent** ⇒ still TRUNCATED, 7 counted.
- **shift −1** → `[87,88]`, flanks 86 and 89 — **both exist** ⇒ evaluable, **8 counted**.

Shifting the claimed set *earlier* pulls the tail event's trailing flank back inside the 90-frame cap and
makes an otherwise-TRUNCATED event scoreable. It touches no other event and no verdict.
