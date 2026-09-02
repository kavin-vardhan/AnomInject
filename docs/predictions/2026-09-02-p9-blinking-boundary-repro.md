# `P9` — blinking hide-boundary reproduction: pre-declared predictions, instrument and gates

**Written 2026-09-02, session 067, BEFORE ANY LEG RUNS.** Committed as its own `docs:` commit before
the first leg, per the standing rule.

> 🔢 **THE NUMBER IS `P9`, NOT `P8`.** The instruction that commissioned this file said *"P8"*. `P8`
> has been taken since 2026-08-18 (TAU is not camera-pose invariant) and phenomenon numbers are
> never reused; `P9` was minted in journal 066 §3 and is already in `CLAUDE.md`'s ledger. The
> filename carries `p9` for the same reason. **The deviation is deliberate and is stated, not
> silent.**

> ⛔ **PREDICTIONS FILES ARE NEVER AMENDED ONCE A MEASUREMENT AGAINST THEM EXISTS.** A defective
> wording is annotated in the session journal, beside its own measurement — never edited here. That
> rule is why this file may be tightened up to the moment the first leg runs and not after.

> ⛔ **NO MECHANISM IS PROPOSED ANYWHERE IN THIS FILE.** Not in the config, not in the
> discriminators, not in §5 or §6. Measure, then design.

**What `P9` is** (full entry: `docs/invisible-anomaly-mechanisms.md` §8): on Bates, owner-observed
×3 across both m36 Section B legs, a `blinking` event's `annotation.json` `frame_indices` claim
hidden `{42,43,47,48}` while the eye saw hidden `{42,43,44,48}` — **one frame missing from the claim
and one extra in it, inside the same window.** Not a constant shift.

---

## 0. 🚨 THE INSTRUMENT PROBLEM, AND IT IS THE REASON THIS FILE IS LONG

**The A54 oracle CANNOT SEE `P9`, and it fails toward "clean".** This is a source fact, not a
worry:

```python
# CaptureBench/tools/a54_oracle.py:135
SHIFTS = (-1, 0, 1)
```

A54's entire hypothesis space is **uniform displacement of the whole claimed set**. It answers *"is
this set shifted, and by how much?"* `P9`'s signature is a **set difference in two directions at
once**, which no shift can express. Worked against the transcribed instance — claimed
`{42,43,47,48}`, actually hidden `{42,43,44,48}` — three of the four claimed frames really are
hidden, so `score(0)` stays far above `TAU` while `score(±1)` collapses, and A54 returns
**`ALIGNED`**.

🚨 **SO THE CERTIFICATION ORACLE WOULD BLESS A `P9` EVENT.** That is `G192`'s shape on a new axis:
the instrument is being asked a question at a point where its two candidate answers degenerate.
Running A54 alone and reporting "12/12 ALIGNED, `P9` not reproduced" would be a **false negative
dressed as a certification**, and this project has published that shape before (`G106`, `G142`,
`G172`).

⇒ **`P9` needs a NEW instrument that reports the OBSERVED HIDDEN SET ITSELF**, not a displacement.
A54 still runs, but in a different role — see §3.

---

## 1. Configuration — Bates-shaped, on StackOBot

Every value below is **reported by read-back from the `StartRun` echo (`A48`), never by the value
issued.** A leg whose read-back disagrees with this table **does not run** (§7).

| axis | value | read-back source |
|---|---|---|
| exe | **`D2BB25A5`** (master post-m36, staged) | `(Get-FileHash …).Hash.Substring(0,8)` before launch |
| container | m34 quartet — `utoc 2A66CA57` · `ucas A7EF9B12` · `pak D8009AD7` | hashed with the exe (`G121`: an exe is half an artifact) |
| map | **`MainWorld`**, packaged | launch arg + log |
| letterbox lever | **ON**, non-zero `Rect.Min.Y` | `LETTERBOX APPLIED on …` + `READBACK-LAYOUT rect=` |
| census | **ON** | `Capture(census): EFFECTIVE FOR THIS RUN - census ON` |
| floor | **0.5** | `floor=0.50%` **and its bracketed source = console** |
| mask | **ON** | `Capture(mask): M23 CVAR beginRun` |
| `excludeTranslucent` | **1** | census echo |
| `reservation` | **1** | `M36 STENCIL RESERVATION ON - reserved=N […]` |
| `maxVerdictAgeTicks` | **12** | census echo |
| blinking half-period | **3** (compiled default, `F-BLINK`) | `blinking: matched N actor(s) … at half-period 3 frame(s)` |
| burst schedule | `IAI.Capture.Config 2 4 8 4 0`, cap **90** | `IAI.Capture.Config: K=2 pre=4 positive=8 post=4 bursts=0` |
| VideoFps | **30** | `IAI.Capture.Fps` echo |
| content clock | **wall** | `IAI.Capture.ContentClock` echo |
| delivery mode | **OFF** on every oracle leg | `IAI.Capture.Delivery` echo |

### 1.1 ⚠ THE LETTERBOX MUST BE EXERCISED, AND ITS OWN LOG SAYS SO

`AnomalyCaptureLetterbox.cpp:73-81` prints the predicted rect and states the trap outright: *a
`minY` of 0 means the lever is a NO-OP and any clean result from it is an artifact of insulation,
not evidence.* **Read `minY > 0` from that line before the leg counts.** The lever refuses on a
`SpectatorPawn` view target, which is why the map is `MainWorld` and not `CB_GateLevel` (`G193`).

### 1.2 ⚠ A CORRECTION TO THE BRIEF'S PREMISE ABOUT DELIVERY MODE, FROM SOURCE

The commissioning brief said delivery mode *"suppresses the `labels.jsonl` bbox the I10 oracle
needs."* **Source says otherwise.** `bWriteLabelsInDeliveryDefault` has a **compiled default of ON**
(`AnomalyCaptureSubsystem.cpp:3591`, and `IAI.Capture.DeliveryLabels` overrides it between runs), so
`labels.jsonl` **is** written in delivery mode unless someone turns it off. What delivery mode does
suppress is `run.json` — which is why `run_leg.ps1:198-207` measures focus acquisition live on
delivery legs instead of reading `start_frame`.

⛔ **THE OLD CLAIM IS RETIRED. Do not repeat "delivery suppresses the `labels.jsonl` bbox".**

**WHY DELIVERY IS OFF ON A / A′ / B / B′ — the actual reason: COMPARABILITY.** The `A53` controls and
all four reader controls are **banked delivery-OFF data**. Reading `P9` legs on the same setting
keeps the legs and the instrument's calibration on one side of a known axis. *(Secondary, real but
not the reason: `run.json` exists, so `A63` is decided by the artifact rather than by the live focus
proxy — `run_leg.ps1:294-297`.)*

**SOURCE-VERIFIED, ONE LINE, AS REQUIRED BEFORE LEG C RUNS: every field the reader reads is PRESENT
in a delivery-ON `labels.jsonl` row.** `BuildFrameLabelRecord` (`AnomalyLabelWriter.cpp:38-109`)
takes **no delivery argument** and is called identically on both paths; delivery gates only whether
the finished record is appended at all — `Job.bWriteLabels = !bDeliveryMode || bLabelsInDelivery`
(`AnomalyCaptureSubsystem.cpp:1937`, and `:2141`) reaching `if (bWriteLabels)`
(`AnomalyLabelWriter.cpp:403`). Field by field, all unconditional:

| field the reader needs | written at | conditional? |
|---|---|---|
| `session_index` | `AnomalyLabelWriter.cpp:46` | no |
| `image` | `:49` | no |
| `anomalies[].id` | `:58` | no |
| `anomalies[].target_name` | `:59` | no |
| `anomalies[].bbox_valid` | `:76` | no |
| `anomalies[].bbox_px` | `:84` | no |
| `view.rot` | `:98` | no |

✅ ⇒ **LEG C IS READ BY THE FULL PIXEL READER. No annotation-shape-only fallback is needed**, and
that is settled here, **before** leg C runs, as required.

📌 What delivery **does** suppress, for completeness: `run.json` (`WriteRunManifest`,
`AnomalyCaptureSubsystem.cpp:1501-1504`) and `selection_provenance.json` (`:3341`). Neither is a
reader input.

### 1.3 ⛔ BATES' OWN DELIVERY SETTING IS NOT RECORDED, AND IT IS NOT ASSUMED

Neither journal 066 nor the owner's write-up records `delivery_mode` for Section B. The card's B-1
payload never issues `IAI.Capture.Delivery`, so Bates ran on **whatever its ini or compiled default
is** — compiled default is OFF (`AnomalyCaptureSubsystem.cpp:333`), but an ini on that host can set
it and no one has read it back.

**Therefore delivery is a BRACKETED axis, not an assumed one.** Leg family **C** (§2) repeats the
primary configuration with **delivery ON** and `DeliveryLabels` left at its compiled ON, so
`labels.jsonl` still exists for the reader. If C and A agree, delivery is excluded as a factor and
Bates' unknown setting stops mattering. If they disagree, that disagreement is the finding.
🔴 **This bracket is a PROPOSAL and is decided at plan approval, not assumed.**

---

## 2. The legs

Same seed, same map, same exe, same container across all of them. **Exe `D2BB25A5`** — it is the
staged master post-m36 build and the one Bates' code is merged into; no reason to use another.

| family | targeting | pacing | delivery | why it exists |
|---|---|---|---|---|
| **A** (primary) | **targeted** blinking on one named actor | **paced 30** | OFF | the oracle-clean read: one target ⇒ one stable modal bbox ⇒ `A56` certifiable |
| **A′** | same target | **unpaced** | OFF | the brief's paced/unpaced pair |
| **B** | **auto-pool**, blinking in the pool | paced 30 | OFF | **Bates fidelity** — Section B ran auto-pool, and the census only participates in selection when selection happens |
| **B′** | auto-pool | unpaced | OFF | pair for B |
| **C** | as A | paced 30 | **ON** | the §1.3 delivery bracket (approval-gated) |

### 2.1 ⚠ THE TARGET FOR FAMILY A IS CHOSEN BY MEASUREMENT, AND THE CHOICE IS RECORDED FIRST

`MainWorld` has no calibration target — `B1`/`CALIB_BBOX` is scoped to `CB_GateLevel`'s
`StaticMeshActor_49` (`G117`), and `run_leg.ps1`'s `-RequireModalRotZero` is **`CB_GateLevel`-only**
(MainWorld settles at `(0,-40,0)`; applying it there would be `G117`'s error on a new axis —
`run_leg.ps1:65-80`).

So: run **one census-ON dry leg** at floor 0.5 and take the target from its `DRAWN-COVERAGE`
histogram — **the highest-drawn candidate that is not scenery-scale**, i.e. not the landscape-class
row. **Record the name and its drawn % in the journal BEFORE any `P9` leg runs.** A target picked
after seeing a `P9` result would be a ruler built to fit the object.

⛔ **Family A must NOT target a Nanite actor** (e.g. `SM_Ramp2`) — Nanite geometry cannot write
custom depth on 5.1 (`G134`), so the census reads it `NOT_MEASURABLE` and the mask reads nothing.
That would confound the leg with a known blind spot.

### 2.1a EVENT COUNTS AND SEEDS — pre-declared, fixed before running

**Seeds, declared now:** primary **4242** (the seed Bates' Section B ran), fallback **777** (the
bench harness default, with the most banked precedent).

| family | events required for the leg to COUNT | on a shortfall |
|---|---|---|
| **B / B′** (auto-pool) | **≥ 3** counted `blinking` events | run **once more on the fallback seed only**. ⛔ **Never a third run.** A second shortfall is reported as a shortfall, not chased |
| **A / A′ / C** (targeted) | **N = 5** counted `blinking` events | see below |

**Why `N = 5` for the targeted legs, derived from the schedule and fixed before running:**
`Config 2 4 8 4 0` writes `pre 4 + positive 8 + post 4 = 16` frames per burst, and the cap is 90, so
90 frames span **5 complete bursts and a partial sixth**. A targeted leg fires the same
target+anomaly every burst ⇒ **5 complete events plus a 6th clipped by the cap**, which is
**TRUNCATED** and excluded from counts by `A50`'s addendum — exactly the shape the `R30` control
shows (12 counted + 1 TRUNCATED).

⚠ **`N` is a PREDICTION, not a filter. A departure from 5 is itself reportable** and is reported
rather than corrected. ⛔ **A targeted leg is NOT re-run on the fallback seed for a count shortfall**
— its count does not depend on the seed, so a shortfall there means something else, and re-running
would hide it.

### 2.2 What a targeted leg does and does not cost

A targeted run **bypasses selection entirely**. That is correct here — `P9` is a *labelling versus
manifestation* question, not a selection question — and it buys the stable bbox the pixel oracle
needs. **The B family exists precisely because A gives that up**, so nothing about Bates' shape goes
unmeasured.

---

## 3. Instruments

### 3.1 A54 — RE-VERIFIED FIRST, AND ITS ROLE IS NARROWED

**`A53` gate: RUN AND PASSED, 2026-09-02, before this file was finished.** Both controls, on this
box, on today's tree:

| control | leg | result |
|---|---|---|
| known-**ALIGNED** | `_bench_sessions_bank\M23\R30_regress\session_20260816-163706` | **12/12 ALIGNED, 12/12 decidable**, median \|margin\| **0.10478**, verdict `ALL-ALIGNED`, exit 0 |
| known-**ABSENT** | `_bench_sessions_bank\I10HF\HF1_nat120\session_20260816-145719` | **12/12 ABSENT, 0/12 decidable**, median \|margin\| **0.000133**, verdict `NON-ALIGNED-PRESENT`, exit 1 |
| positive control | `R30_regress --shift 1` | **12/12 `SHIFTED(-1)`**, every margin collapsing **0.105 → 0.050** — it recovers an injected displacement decisively |

Both legs read `A56` **CERTIFIABLE** (modal 100.0 % of 99 bbox rows, 1 distinct) and `B1` **pose-match
YES**. **`TAU` IS ALREADY FROZEN AT `0.04684` IN THE FILE AND IS NOT RE-DERIVED** — the brief's
"freeze TAU from the controls" is satisfied *a fortiori*: the constant predates the controls and the
controls confirm it still discriminates. ⛔ **Do not tune it** (the file says so, and tuning after
seeing results is building the ruler to fit the object).

⚠ **ONE DISCREPANCY, REPORTED NOT CHASED:** `a54_oracle.py`'s header claims it reproduces *"the
published R30 median (0.10737)"*. Today's run gives **0.10478** on `R30_regress` and **0.109008** on
`M23B\R30_recert`. **Neither is 0.10737.** The `A53` gate itself is unaffected — it turns on
ALIGNED-vs-ABSENT, and that separation is total (0.105 vs 0.0001). **CAUSE NOT ESTABLISHED**;
candidates include the header quoting a third leg or an earlier margin definition. Recorded here so
it is not re-discovered as new, and so nobody cites 0.10737 as reproduced.

**A54's ROLE ON `P9` LEGS IS TO EXCLUDE `P1`, NOTHING MORE.** It answers *"is this a constant
shift?"* Per §0 it cannot answer *"is this a `P9` set difference?"* — and an `ALIGNED` from A54 on a
`P9` leg **must never be reported as "`P9` not reproduced."**

### 3.2 🆕 THE `P9` HIDDEN-SET READER — the instrument this phenomenon actually needs

**New tool, `CaptureBench/tools/p9_hidden_set.py`, committed to CaptureBench before it grades
anything** (`G106`: an analysis instrument that grades a certified result is a committed artifact,
not a scratch script).

**It deliberately does NOT reuse `TAU`.** `TAU` is an absolute luminance difference and is known not
to be pose-invariant (**that is `P8`**, and it fails toward `ABSENT`). Every constant below is
**relative and within-event**, so the reader inherits neither that failure nor its calibration.

#### (i) The window, the anchor, and the split

Per labelled event, with claimed set `C` from `annotation.json`:

- **Window** `W = [min(C) − 2 … max(C) + 2]` — every frame in it is classified, not just the claimed
  ones, because an **unclaimed frame observed hidden** is half of `P9`'s signature.
- **The VISIBLE cluster is ANCHORED by the two flank frames on each side** — `min(C)−1`, `min(C)−2`,
  `max(C)+1`, `max(C)+2`. **The hidden cluster is the other one.** Anchoring removes the polarity
  guess: the reader never has to decide which cluster means "hidden".
- **Mean luminance inside the event's bbox only** (`A35` — hiding an object removes its cast shadow
  *outside* the box, so whole-frame scoring is wrong here).
- **Split:** 1-D two-means over `W`, seeded at the anchor mean (visible) and at the window value
  farthest from it (hidden), iterated to convergence.

**Frozen constants, declared here BEFORE any control or leg runs, and NOT tuned afterwards:**

| constant | value | meaning |
|---|---|---|
| `FLANK` | **2** | flank frames per side forming the visible anchor |
| `SEP_RATIO` | **10.0** | the two centroids must be at least this many times the larger within-cluster spread apart |
| `MARGIN_FLOOR` | **0.5** | a frame is decidable when its distance to the midpoint is at least this fraction of half the separation |
| `KMAX` | **6** | the shift search runs `−6 … +6` |

🚨 **ANCHOR CHECK, and it is a real guard rather than a formality:** all four flank frames must land
in the **same** cluster after the split. If they do not, the anchor is unreliable — the window's
edges are not all visible — and the event is **UNDECIDABLE (anchor unreliable)**. ⛔ **It is never
guessed.** This matters because a `blinking` hidden phase adjacent to the window's edge would
otherwise invert the polarity and turn a clean read into a confident wrong answer.

#### (ii) Shift search, then classification IN THIS ORDER

The reader computes the **best integer shift `k` over `−6 … +6`**: the `k` minimising the size of the
**residual**, the symmetric difference between `C + k` and the observed hidden set `H`; ties broken
toward the smaller `|k|`. Then:

| # | condition | verdict |
|---|---|---|
| 1 | `k = 0` **and** residual empty | **ALIGNED** |
| 2 | `k ≠ 0` **and** residual empty | **SHIFTED(k)** — ⛔ **`P1` class, NOT `P9`** |
| 3 | residual **non-empty at every `k`** | classify **at `k = 0`**: differences in **both** directions → **`P9`-SHAPE**; in **one** direction → **ONE-DIRECTIONAL** (its own class, ⛔ **no `P`-number without chat**) |
| 4 | split below `SEP_RATIO`, anchor check failed, `A56` failed, or margins below `MARGIN_FLOOR` | **UNDECIDABLE** |

**The best-`k` residual is REPORTED AS DATA alongside every verdict**, including the ALIGNED ones.

📌 **ORDERING NOTE, stated so it is not a silent reordering:** row 4's tests are **preconditions** —
an event with no valid split has no sets to compare, so they are evaluated first in code. **Rows 1–3
govern the classification of every event that HAS a valid split**, exactly as written. ⛔ **An
UNDECIDABLE is never reported as a `P9` absence.**

#### (iii) The bbox is PER EVENT, and `A56` is evaluated per event

- The bbox comes from the `labels.jsonl` rows belonging to **that event** — joined by
  `anomalies[].id == anomaly_type` **and** `anomalies[].target_name == nodes[primary].name`, over
  rows whose `session_index` falls in `W`. The **modal** `bbox_px` among rows with
  `bbox_valid == true` is the event's box.
- **`A56` is evaluated PER EVENT** — modal-crop coverage **≥ 90 %** of that event's valid rows and
  **≤ 3 distinct** boxes. 🚨 **An event failing `A56` is UNDECIDABLE, never silently skipped.**
- This is uniform across leg families: on **targeted** legs (A/A′/C) the per-event box equals the
  leg-modal box, and on **auto-pool** legs (B/B′) — where the target differs per event — it is the
  only box that means anything.
- ⛔ **`B1` / `CALIB_BBOX` IS NOT APPLIED AND IS NOT AVAILABLE.** `CALIB_BBOX` is scoped to
  `CB_GateLevel`'s `StaticMeshActor_49` (`G117`) and these legs run on `MainWorld`. The reader
  **declares it NOT APPLICABLE**; it never reports "passed".

#### The reader's own gate — FOUR controls, all from banked data, ALL BEFORE ANY LEG

| # | control | required behaviour |
|---|---|---|
| **(a)** | `M23\R30_regress` (known-ALIGNED) | observed **==** claimed on **12/12** counted events, **both differences empty**, `k = 0` |
| **(b)** | `I10HF\HF1_nat120` (known-ABSENT) | **no clean split — UNDECIDABLE 12/12.** Never a confident set |
| **(c)** | 🆕 **synthetic `P9`** — one claimed frame moved to a neighbour in `R30`'s annotation | **exactly that one-in / one-out difference**, and the residual **non-empty at every `k`** |
| **(d)** | 🆕 **synthetic constant shift** — `+1` applied to `R30`'s **whole** annotation | **`k = 1`, residual EMPTY, reported `SHIFTED(1)`** — ⛔ **and NEVER as `P9`-SHAPE** |

🚨 **(c) AND (d) ARE THE LOAD-BEARING PAIR AND NEITHER ALONE IS ENOUGH.** **(c) proves the reader
SEES the shape; (d) proves it does not MISTAKE `P1` FOR IT.** A reader with only (a), (b) and (c)
could pass by calling every disagreement `P9` — which is precisely the confusion this whole
phenomenon has to be protected from. Both are built from banked data at zero leg cost.

⛔ **ANY of the four failing is a HARD STOP: no dry leg, no `P9` leg, report and stop.** `G189` is
what happens when a check can only pass.

### 3.3 Reporting, per event

Every event on every leg reports, in one row: the **A50 verdict** (`ALIGNED` / `SHIFTED(N)` /
`ABSENT` / `TRUNCATED`) with its **A55 margin**, **plus the raw observed hidden set and the two set
differences**. The raw sets are printed whatever the verdicts say, so the `P9` shape is visible
directly rather than inferred from a classification.

---

## 4. Pre-declared discriminators

**Declared before any leg. May be TIGHTENED before the first measurement; never loosened; never
edited after.**

### 4.1 `P9` REPRODUCED

**≥ 1 blinking event, on any leg, whose observed hidden set differs from its `annotation.json`
`frame_indices` in BOTH directions within the event window** — at least one **claimed** frame
observed **visible** AND at least one **unclaimed** frame observed **hidden** — **and the mismatch is
not expressible as a constant shift of the whole set.**

Report the **gap between the two differing frames as data** (on Bates: **3**). ⛔ It is a number to
record, not a threshold, not a period, and not a mechanism.

### 4.2 NOT `P9` — a constant shift

A **uniform per-event displacement of every claimed frame** → that is `SHIFTED(N)`, it belongs to
**`P1`**, and it is filed there. ⛔ **Do not conflate.** `P1` is open and unreproduced; quietly
folding a shift into `P9` would corrupt both.

### 4.3 ONE-DIRECTIONAL mismatch

A difference in **only one** direction — a claimed frame observed visible **without** an unclaimed
frame observed hidden, or the reverse — is **its own observation class**. Report it as such, with
its sets. ⛔ **No new `P`-number without chat.**

### 4.4 NOT REPRODUCED

**Every** blinking event on **both** primary legs `ALIGNED` under A54 **with margin ≥ TAU**, **and**
the hidden-set reader returning **empty differences in both directions on every event**. ⛔ **Both
conditions. An A54 `ALIGNED` alone does not qualify** — §0 says why. **Only then does §6 go live.**

### 4.5 UNDECIDABLE

Margins below `TAU`, or the reader's split below its separation floor → **report as UNDECIDABLE**.
⛔ **No re-thresholding after the fact**, and an `ABSENT` is not read as a defect until the leg's
camera pose has been checked against the calibration pose (`P8`, and the oracle prints the warning
itself).

---

## 5. `ticks_per_captured_frame` — what the number IS, from source

**It is not a mystery quantity. It is a quotient of two counters the same file already writes:**

```cpp
// AnomalyLabelWriter.cpp:546-548
Root->SetNumberField(TEXT("capture_game_ticks"), TickPin->GameTicks);
Root->SetNumberField(TEXT("ticks_per_captured_frame"),
    TotalFrames > 0 ? ((double)TickPin->GameTicks / (double)TotalFrames) : 0.0);
```

`GameTicks` is `CaptureGameTicks`, reset at `StartRun` (`AnomalyCaptureSubsystem.cpp:1516`) and
incremented **once per capture-subsystem tick while the run is live**
(`AnomalyCaptureSubsystem.cpp:544`). `TotalFrames` is frames **written**.

**Bates: 122 / 90 = 1.35555… = `1.3556`. The reported ratio is arithmetically consistent with the
two counters Bates also reported.** ⇒ **What is unexplained is not the ratio — it is the 32 surplus
game ticks over 90 written frames.**

**Ticks that advance the counter but write no frame, enumerated from source, NOT attributed:**

- **settle phases** — `SettleAfterFire` and `SettleAfterRevert`, `PhaseFramesLeft = SettleFrames`
  (`:2049-2050`, `:2064-2065`, `:2074-2075`); with `Config 2 4 8 4 0` that is `K=2` at **two**
  points per burst;
- **`DrainTail`** — `PhaseFramesLeft = FMath::Max(10, ViewLagFrames + 4)` (`:634-635`), at least
  **10** ticks that tick the subsystem and write nothing;
- ticks before the first arm, and any armed frame dropped before write.

With 16 written frames per burst (`pre 4 + positive 8 + post 4`), 90 frames is 5 full bursts plus a
partial, giving ~24 settle ticks, and `DrainTail` adds ≥10. **~34 against an observed 32 — the same
order, from the schedule alone.** ⛔ **That is an ARITHMETIC ACCOUNT OFFERED AS A HYPOTHESIS TO BE
MEASURED, not a finding.**

**What each leg does:** compute its own `ticks_per_captured_frame` and report it beside Bates'.

- **If the bench's value equals 1.3556** under the same schedule ⇒ **say plainly that it is NOT a
  discriminator** — a number two hosts share cannot separate them.
- **If it differs** ⇒ state **what would have to be true of Bates to yield 1.3556** — a different
  burst schedule, a different frame cap, extra dropped frames, a different `DrainTail` length — and
  file that as a **measurement gap**, ⛔ **not as a mechanism for `P9`.**

⚠ `tickpin_compiled` is **false** on both Bates and this bench, so the tick-pin is **not** in play
on either side. The coincidence noted in journal 066 §4 (equal to this bench's session-051 unpinned
baseline) stays a coincidence to check.

---

## 6. Source verification: does the census touch the FIRED target?

⛔ **A SOURCE READ. It is not a claim about `P9`'s cause, and it establishes nothing about `P9`.**
It exists so that if a leg does reproduce `P9`, one candidate is already characterised rather than
speculated about.

### 6.1 What the guards are — four of them, each cited

1. **Hidden actors are never queued.** `ClassifyCandidate` returns `Hidden` when `Actor->IsHidden()`
   → `AnomalyCensus.cpp:86-89`, and that verdict path (`:403-405`) never reaches `CycleQueue`.
2. **An actor the event mask has tagged is never queued.** `IsAnyComponentTagged(Actor)` →
   `HeldElsewhere` (`AnomalyCensus.cpp:90-93`), which also skips the queue (`:406-407`). The event
   mask's `TagActor` registers the component in `GTaggedComponents`
   (`AnomalyStencilTag.cpp:102-108`) and it stays registered until `RestoreActor`/`RestoreAll`,
   which the mask only calls at `EndRun` (`AnomalyMaskMeasure.cpp:58`, `:74-77`). ⇒ **once the event
   mask has armed on the fired target even once, the census excludes it for the remainder of the
   run.**
3. **Both tests are RE-CHECKED at arm time, immediately before tagging** —
   `AnomalyCensus.cpp:627-631` (hidden → verdict `NotMeasurableHidden`, `continue`, **no tag**) and
   `:632-635` (already tagged → `continue`, **no tag**). `TagActor` is at `:638`, after both.
4. **A tag overtaken mid-flight takes no credit.** Crediting requires
   `VerifyActorStillTagged` (`AnomalyCensus.cpp:552-563`); failing it goes to the **TAG-OVERTAKEN**
   path (`:470-478`), which explicitly names *"the event mask tagging a fresh fire"* as the expected
   case and takes **no credit and performs no restore**.

⇒ **On this reading the fired target is removed from the census set and left untagged before its
first hide frame, and a census verdict cannot be credited to a target the event mask has claimed.**

### 6.2 The part that is NOT decidable from these files, stated as a boundary

`TagActor` calls `SetCustomDepthStencilValue` + `SetRenderCustomDepth(true)`
(`AnomalyStencilTag.cpp:115-116`), and `RestoreActor` flips them back (`:177-178`). The census's own
cost line says what that means: *"`SetRenderCustomDepth` only QUEUES the proxy recreate; the
destroy/create runs later inside `SendAllEndOfFrameUpdates`, OUTSIDE this timed block"*
(`AnomalyCensus.cpp:734-739`) — and `S3` measured **~1,200 recreates per run**, 95 % of the census's
cost.

By §6.1 those recreates are of **other** actors, never the fired target. **Whether a deferred proxy
recreate of an unrelated primitive can affect what a given frame draws is NOT ANSWERABLE FROM THE
PLUGIN SOURCE** — it is engine behaviour inside `SendAllEndOfFrameUpdates`.

**Surface searched (`G136`), stated so the blind spot is visible rather than absent:** both plugin
repositories' `Source/` trees in full — `AnomalyInjector` (all four modules) and `CaptureBench` —
by full reads of `AnomalyCensus.{h,cpp}`, `AnomalyMaskMeasure.cpp`, `AnomalyStencilTag.cpp`,
`Anomaly_Blinking.{h,cpp}` and the census/mask hook sites in `AnomalyCaptureSubsystem.cpp`, plus
tree-wide greps for `MarkRenderStateDirty|RecreateRenderState|DeferredProxy` (**zero plugin hits** —
the mechanism is entirely engine-side). ⛔ **UE 5.1 engine source was NOT read this session.** If a
leg reproduces `P9`, that read is the next step — **and not before.**

### 6.3 One ordering fact, recorded without inference

The census ticks **after** the event mask arms, in the same `OnWorldTickEndMask`
(`AnomalyCaptureSubsystem.cpp:678-693`): verify → drain → collect → `ArmIfMeasurable` (`:681`) →
census `Tick` (`:693`). Blinking toggles its target's hidden flag in the anomaly's own tick
(`Anomaly_Blinking.cpp:80-97`), and the mask skips hidden targets at arm (`AnomalyMaskMeasure.cpp:225-229`,
`++SkippedHidden`). ⛔ **Recorded as ordering, not as a mechanism.**

---

## 7. STOP conditions — any one of these ends the leg or the read

| condition | consequence |
|---|---|
| The A54 oracle fails **either** `A53` control | ⛔ **STOP. Do not read any `P9` leg.** |
| The hidden-set reader fails **any** of its **four** controls — **both synthetic fixtures included** | ⛔ **HARD STOP: no dry leg, no `P9` leg.** A reader that cannot fail is not a reader (`G189`); one that cannot tell `P1` from `P9` is worse. |
| Any leg's config read-back disagrees with §1 | ⛔ **STOP — the leg does not run.** Fix the payload, re-echo (`A48`). |
| `LETTERBOX` line reports `minY = 0` | ⛔ **The leg is a NO-OP for the letterbox axis. VOID for that axis** (`G192`, and the lever's own log says it). |
| Key ring not clean, or the marker series not strictly increasing (`A45`) | ⛔ **That leg is VOID. Say so** — a lone decode is never evidence a marker was drawn. |
| `census_fires_fallback_all` non-zero | ⛔ **Report it. Do not re-run to a green.** |
| Zero-byte frames survive the flush wait | ⛔ **VOID for pixel work** (`run_leg.ps1:262-265`). |
| `B1` / `A47` gates | **NOT APPLICABLE on `MainWorld`** and declared so, never "passed" (`G117`). |

---

## 8. What is deliberately NOT in this file

- ⛔ **No fix, no mechanism, no "likely cause".**
- ⛔ **No ceiling knob and no floor default change.** The floor/ceiling DECIDE sits with the owner
  and nothing is built until chat relays it. Floor **0.5** appears here only as a **leg parameter**,
  which is what it has always been.
- ⛔ **No new `P`-number.** §4.3's one-directional case is an observation class until chat says
  otherwise.
- 📌 **The one-event typed ask for Bates is DRAFTED IN THE SESSION JOURNAL, NOT SENT, and is NOT on
  the RDP card.** It is used only if the bench cannot reproduce `P9` — asking the owner for a
  hand-transcription the bench could have produced is the wrong order of work.
