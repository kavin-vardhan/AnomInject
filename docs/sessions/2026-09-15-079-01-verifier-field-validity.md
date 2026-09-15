# 2026-09-15 — 079-01 — label-pixel verifier: the field misfire, reproduced and diagnosed

**Unit:** verifier field validity. **Branch:** `fix/verifier-field-validity` off `master` (`ac13700`).
**Scope:** tool + docs only. ⛔ **NO FIX WAS WRITTEN.** `tools/verify_capture.py` and
`tools/measure_label_offset.py` are **BYTE-UNCHANGED** by this session. `Source/`, `Shaders/`,
fixtures, schema and every gate are untouched. No build, no cook, no capture, no leg, no merge, no tag.
`m51` is untouched at `1f61196` and stays HELD; nothing in 078-17 is released by this work.

---

## 0. Orchestration change (recorded)

From **2026-09-15** chat Claude is the orchestrator of the GDP mailbox again. The Codex pilot
(`078-01` → `078-17`) is **over**. Codex is now used as an **independent reviewer at crucial
checkpoints only**, named by chat. **One orchestrator per mailbox** — unchanged; the contended
resource is the bench checkout.

`_mailbox/README.md`'s **Orchestrator** line is updated to say exactly this, with the 2026-09-06 →
2026-09-15 Codex-pilot arrangement marked superseded rather than deleted.

🚨 **THE OTHER HALF OF §0 COULD NOT BE EXECUTED AS WRITTEN, AND IT IS A REAL COLLISION, NOT AN
OVERSIGHT ON MY PART.** The brief says *append a dated entry to `docs/OPERATING-CONTRACT.md`* and
also says *branch off `master` (`ac13700`)*. **`docs/OPERATING-CONTRACT.md` DOES NOT EXIST ON
`master`.** Measured:

```
git ls-tree -r master --name-only | grep OPERATING-CONTRACT   ->  (empty)
git ls-tree -r m51    --name-only | grep OPERATING-CONTRACT   ->  docs/OPERATING-CONTRACT.md
```

The contract is **m51-resident** — it was created during the Codex pilot and lives on the line that
078-17 leaves **HELD and NOT ACCEPTED**. Three ways to force it, all refused with reasons:

- **Create a second contract on this branch** — two files claiming to be the single source of the
  operating contract. That is the defect this project names `G88`/`G139` in a new place, and it is
  worse than the gap.
- **Cherry-pick the contract from `m51`** — imports held, unaccepted m51 content into a branch cut
  from `master`. It entangles the very line the brief's own `ACK-VERDICT` says stays held.
- **Switch to `m51` and append there** — the brief put this unit on `master` deliberately, and
  editing the held line to record an orchestration change is not what the freeze contemplates.

⇒ **The orchestration change is recorded HERE (this §0) and in `_mailbox/README.md`, and the
`OPERATING-CONTRACT.md` entry is OWED on whichever line that file ends up on.** It is named as owed
rather than improvised. **Chat decides** whether the contract entry lands on `m51` now, or waits for
the contract to reach `master`.

---

## 1. §3.1 — what the gate actually computes (source read, line-cited, no paraphrase)

Everything below is `tools/verify_capture.py` unless marked `mlo:` (= `tools/measure_label_offset.py`).

### 1.1 The metric

`_frac_above(cache, k, thresh, region)` — **:516-540**:

```
d(k) = |{ p in Region : |gray_k(p) - gray_{k-1}(p)| > thresh }| / |Region|
```

- `thresh` default **8**/255 (`DIFF_THRESH_DEFAULT`, **:408**).
- `Region` is the mask silhouette when a mask exists, else the clamped label bbox
  (`_build_region`, **:496-513**; floor `mlo.MIN_REGION_PX = 64`, `mlo:327`).
- `|Region|` is `region["npix"]` — the mask pixel count, or the bbox **area**.

🔑 **`d` IS A FRACTION AND IS THEREFORE BOUNDED: `d ∈ [0, 1]`, ALWAYS.** The numerator counts pixels
inside the region; the denominator is the size of that region. **This bound is the whole defect.**

### 1.2 The threshold

`_threshold_from(vals, mlo)` — **:623-630**:

```
tau = max( median(base_vals) + K_SIGMA * MAD(base_vals),  SIGNAL_FLOOR )
      K_SIGMA      = 6.0      (mlo:311)
      SIGNAL_FLOOR = 0.0040   (mlo:312)
      MAD          = median(|v - median|)
```

`base_vals = { d(k) : k in base_idx }`, where `base_idx` is every frame **not** inside any annotated
window ± a guard, with the guard **relaxed 2 → 1 → 0** until at least 3 survive (**:703-714**,
printed at **:915-919**).

⛔ **`tau` HAS NO UPPER BOUND. `d` DOES.** Nothing anywhere clamps `tau` to the range its own
statistic can occupy.

### 1.3 "Changed", and the edge

`_dominant_edge` — **:543-572**: over `k` in the search window, skipping frames belonging to other
events, take `argmax d(k)` **subject to `d(k) > tau`** (**:567**, strict). Returns `(None, None)`
when nothing clears `tau`. Window `±EDGE_WINDOW_DEFAULT = 4` (**:409**).

### 1.4 Every path that yields NOT-VISIBLE — there are exactly TWO

| # | line | condition |
|---|---|---|
| 1 | **:763-767** | mask pixel count `< min_visible_px` on some labelled frame. **Requires masks.** |
| 2 | **:840-842** | `onset_k is None AND end_k is None` — i.e. **`d(k) <= tau` for every `k` in the window**, at both edges. |

🔑 **On masks-off data path 1 CANNOT FIRE.** The field captures carry no masks ⇒ **every field
NOT-VISIBLE is path 2**, and path 2 is exactly "nothing beat `tau`".

### 1.5 Every NOT-MEASURABLE reason

| line | reason |
|---|---|
| :737 | `manifested` false, or empty `frame_indices` |
| :741 | `no-frame_indices-in-annotation` — `mlo.event_indices` set `derived=True` (`mlo:966-975`: `affected_frames` carried `start_frame`/`end_frame` but no `frame_indices`) |
| :747 | no `labels.jsonl` row carries this event |
| :771 | `no-region` — no usable mask and no usable bbox |
| :782 | `baseline: only N clean frame(s), need 3` |
| :845 | shift beyond the **measurable range** `±ceiling`, `ceiling = min_clean_gap // 2` (`_measurable_ceiling`, **:633-656**) |
| :856 | onset not decidable |
| :858 | end truncated by the session's last frame |
| :861 | end not decidable |

### 1.6 The confidence annotation

`contaminated = sum(1 for v in base_vals if v > tau)` (**:787**); `LOW` if contaminated, else `MED`
if bbox-only, else `HIGH` (**:882-886**).

🚨 **THIS IS ANTI-CORRELATED WITH THE FAILURE, AND THAT IS A FINDING IN ITS OWN RIGHT.** When `tau`
saturates above *every* baseline value, `contaminated` is **0** and the run prints no contamination
warning at all. **The one signal that would tell a reader the baseline is junk goes silent precisely
when it is junk.** Measured below: the fully-saturated session reports `CONTAMINATED` on **zero** of
six events, while the mildly-affected sessions report 1–7.

---

## 2. The prediction, derived from §1 BEFORE any session was run

Stated as an arithmetic consequence of §1.1 + §1.2, not as a guess:

> Under camera motion every pixel in the region changes every frame, so `d(k) → 1` on **clean**
> frames too. Then `median(base_vals) → 1` and `MAD` grows with the frame-to-frame variability of
> the motion. Since `tau ≥ median` and `d ≤ 1`, the headroom `1 - tau` collapses. And because
> `tau = median + 6·MAD` has **no ceiling**, `tau` exceeds **1.0** as soon as
> `MAD > (1 - median)/6` — at which point **`d(k) > tau` is mathematically unsatisfiable** and the
> gate returns NOT-VISIBLE **unconditionally, for every event, regardless of the pixels**.

**Predicted readings:** pinned bench ⇒ `tau` pinned at `SIGNAL_FLOOR = 0.0040` and PASS; camera
motion ⇒ `tau` rises with motion; enough motion ⇒ `tau ≥ 1.0` and 100 % NOT-VISIBLE.

---

## 3. §3.2 — REPRODUCED. And it needed no new capture, no lever, and no source change.

🎯 **THE REPRODUCTION IS ALREADY IN THE BANK.** The brief anticipated a new home capture with a
camera-motion lever that would have had to be invented — which collides with §4's *"`Source/`
untouched anywhere"*. **That collision dissolved:** `_bench_sessions_bank` already holds
moving-camera sessions on a real game, and one of them reproduces the field reading exactly.

Instrument: **`tools/verify_capture.py --label-pixel-gate --report-only`, UNMODIFIED** — the same
command the owner ran on the office box. Motion proxy `M_med` = median over consecutive frame pairs
of the **whole-frame** fraction of pixels differing by > 8/255 (throwaway script, `%TEMP%`, read-only,
not committed — it computes the same quantity `_frac_above` does, over the whole frame instead of the
region).

| session | camera | masks | `M_med` | `tau` observed | verdict |
|---|---|---|---|---|---|
| `M49_GEDGE_BL_NAT` | pinned | yes | — | **0.0040** | PASS 4/4 |
| `M49_GEDGE_CT_NAT` | pinned | yes | — | **0.0040** | PASS 4/4 |
| `M49_GEDGE_MO_NAT` | pinned | yes | — | **0.0040** | PASS 4/4 |
| `M49_GEDGE_MT_NAT` | pinned | yes | **0.0005** | **0.0040** | PASS 4/4 |
| `M49_GEDGE_BL_SYN` | pinned | yes | — | **0.0040** | PASS 4/4 |
| `M49_GEDGE_MT_SYN` | pinned | yes | — | **0.0040** | PASS 4/4 |
| `A2L_LEGA` (Lyra) | light | yes | 0.0033 | 0.0040 – 0.0343 | PASS 6, NOT-MEAS 1 |
| `LYRA_SMOKE_01` | moderate | yes | 0.0239 | 0.0248 – 0.3957 | PASS 5, NOT-MEAS 1 |
| `A1L_LEGA` (Lyra) | strong | yes | 0.0884 | 0.0075 – **0.8541** | PASS 4, NOT-MEAS 2 |
| **`M50L_LG9`** (Lyra) | **heavy** | **no (bbox-only)** | **0.2649** | 0.5262 – **1.1938** | 🚨 **NOT-VISIBLE 6/6 — VERDICT FAIL** |

🚨 **`M50L_LG9` IS THE FIELD MISFIRE, REPRODUCED AT HOME, ON A SESSION WHOSE LABELS ARE
INDEPENDENTLY ESTABLISHED AS CORRECT.** It is the `m50` Lyra `L_Convolution_Blockout` leg — journal
076's **headline success**, the leg where the `m50` fix took `vetoed_events` 6 → 0 and turned
`anomalies: []` into **6 events delivered**. Those six events are `m50`'s own gate `G9`. **This gate
calls all six NOT-VISIBLE and the session FAIL.**

**It matches the field conditions on every axis that matters:** `masks present: no (bbox-only mode)`,
regions taken from `labels_bbox_px`, real game content, moving camera.

### 3.1 Two of the six are in the unsatisfiable regime

`tau = 1.0950` (idx=0) and `tau = 1.1938` (idx=5), against a statistic bounded at **1.0**. For those
two events **no pixel change of any magnitude whatsoever could have passed**. The other four sit at
0.53–0.99, i.e. demanding that 53–99 % of the region's pixels change between consecutive frames.

### 3.2 The gate prints a sentence that is false

idx=4, verbatim:

```
run[63..70]  d(onset=63)=0.7400  d(clear=71)=0.8007  tau=0.8392  region=labels_bbox_px/902400px
NOT-VISIBLE  run[63..70] no pixel change above tau anywhere in +/-4 of the claim
```

**74 % and 80 % of the region's pixels changed** at the two edges. The reading printed to the reader
is *"no pixel change"*. ⚠ The wording is technically scoped by *"above tau"*, but a human reading a
client-facing report reads it as **the anomaly is not in the picture** — which is the exact false
conclusion the owner's eye-check overturned in the field.

---

## 4. Ranking the hypotheses — from the numbers, not from argument

**H1 — moving camera. CONFIRMED, AND IT IS THE MECHANISM.** `M_med` and `tau` move together
monotonically across five sessions spanning a 500× range of motion
(0.0005 → 0.0033 → 0.0239 → 0.0884 → 0.2649), and the verdict degrades in lockstep
(PASS 4/4 → PASS 6/7 → PASS 5/6 → PASS 4/6 → **FAIL 0/6**). The predicted saturation is not merely
approached, it is **crossed** — `tau > 1.0` on two events. ⛔ The `6·MAD` term is what carries `tau`
past the achievable ceiling: `median(d)` can never exceed 1, but `median + 6·MAD` can, and does.

**H2 — dense auto-injection. CONFIRMED AS A REAL SECOND-ORDER CONTRIBUTOR, NOT THE CAUSE.** The
measurable ceiling collapses with schedule density: `±7` (min clean gap 14) on the bench vs **`±1`**
(gap 3) on `A1L_LEGA` and `±2` (gap 4) on `M50L_LG9`. Baseline frames fall 37 → 30 → 24 → **15**.
That converts readings into NOT-MEASURABLE and shrinks the baseline `tau` is learned from — but it
does **not** produce NOT-VISIBLE on its own, and the pinned bench legs with 4 events apiece never
misfire.

**H3 — oversized v1 bboxes. CONFIRMED AS AN AMPLIFIER.** `M50L_LG9`'s regions include
`labels_bbox_px/**2073600**px` — that is **1920 × 1080, the ENTIRE FRAME** — plus 1,776,000 /
1,651,200 / 902,400 / 656,829 / 631,200 px. 🔑 **When the region IS the frame, `d(region)` is
*identically* the global motion**, there is no local signal left to find, and H1's saturation is
guaranteed rather than merely likely. This is the `G124` class (bounds-derived, spline/instanced
targets) meeting H1.

⇒ **One cause (H1), two amplifiers (H2, H3). A fix that addresses only H2 or H3 leaves the gate
broken; a fix that addresses H1 must still report H2/H3 honestly rather than silently.**

---

## 5. Why nothing ever caught this — the missing half of `G96`

🚨 **`tau = 0.0040` — EXACTLY `SIGNAL_FLOOR` — ON ALL SIX HISTORICAL BENCH LEGS, EVERY EVENT, BOTH
TICK ORDERS, ALL FOUR ANOMALY TYPES.** The learned term `median + 6·MAD` **has never once determined
`tau` in any test this project has ever run.**

And it cannot, in either existing test regime:

- **The selftest is structurally static.** `_synth_session` (**:935-994**) paints
  `Image.new("RGB",(w,h),(90,90,90))` with one fixed dark rectangle every frame (**:948-952**); only
  the target box changes. Clean-frame `d(k)` is **exactly 0** ⇒ `median = MAD = 0` ⇒
  `tau = SIGNAL_FLOOR` in all seven synthetic cases (**:1051-1059**).
- **The bench is pose-pinned** by the `B1` gate, which is *why* `CB_GateLevel` exists.

⇒ **`K_SIGMA = 6.0` is dead code in every test, and the first thing that ever exercised it was field
data — where it saturates.** The selftest proves the gate **CAN** fail (`blank_region` → NOT-VISIBLE);
**nothing ever proved it PASSES a known-good REAL session.** That is `G96`'s missing half, and this is
its cost. Worse: the `blank_region` case's *meaning* also inverts under motion — a blank region
**does** change when the camera moves, so the one case that anchors NOT-VISIBLE is anchored only in
the pinned regime.

---

## 6. Prior art — this codebase already solved motion rejection, and the m49 gate dropped it

`measure_label_offset.py` carries an **ambient ring**: `ring_mean` (`mlo:483-491`), `RING_DILATE_PX =
48`, `AMBIENT_FACTOR = 3.0` (`mlo:313-316`), `net = raw - ambient` (`mlo:1268-1269`). Its own
docstring, **verbatim** (`mlo:92-93`):

> *"Subtracting ambient is what stops camera motion, a passing mover or a lighting change from faking
> a manifestation: a whole-frame change lifts the ring as much as the region and cancels."*

and (`mlo:126`):

> *"A frame is UNMEASURABLE when ambient(f) > AMBIENT_FACTOR * T — ambient change is too large to
> separate signal from it."*

🔑 **That second sentence is ALREADY the brief's acceptance rule (a)** — when motion makes the read
invalid, the older module declares **UNMEASURABLE**, never a failure.

🚨 **`label_pixel_gate` imports that module and takes its THRESHOLD CONSTANTS while implementing its
own metric WITHOUT the ring.** Its docstring says so explicitly (**:421-425**): *"adds one metric the
module does not carry … and takes everything else … from the module."* **But `K_SIGMA` and
`SIGNAL_FLOOR` were calibrated for `net` — a ring-SUBTRACTED, motion-COMPENSATED quantity — and are
applied here to a raw, motion-UNCOMPENSATED one.** On a pinned camera the two coincide
(`ambient ≈ 0`), which is why every bench leg and every selftest case passes. On a moving camera they
diverge without bound.

⇒ **The fix direction is not an invention. It is restoring a mechanism this repo already has, already
documented as the motion cure, to the one consumer that omitted it.** → proposal
`docs/predictions/2026-09-15-verifier-field-validity.md`.

### 6.1 `G156` — linked, and corrected

`G156` (gotchas.md:4161) is the prior-art note the brief asked me to find: *"THE CLIENT'S SHIPPED
CONFIG CANNOT SELF-VERIFY"*, and it already names the ring — *"loses the ambient ring that rejects
camera motion at the same time"*.

⚠ **But `G156`'s stated MECHANISM is now factually stale, and repeating it would misdirect the fix.**
It says *"Delivery mode does not write `labels.jsonl`"* — true when it was written (2026-08-21,
session 051), **superseded the next day**: session 052 shipped `labels.jsonl` **in delivery mode,
default ON** (`IAI.Capture.DeliveryLabels`). The field captures **have** `labels.jsonl` and **have**
per-frame bboxes; they did not degrade to FULLFRAME for `G156`'s reason.

🔑 **So the m49 gate did not inherit `G156`'s envelope. It inherited a DIFFERENT unstated one — the
PINNED CAMERA — and `G156`'s lesson ("the shipped config cannot self-verify") turns out to be right
for a reason `G156` did not name.** `G156` is annotated, not rewritten (append-only).

---

## 7. What was NOT done, and why

- ⛔ **No fix.** `verify_capture.py` and `measure_label_offset.py` are **byte-unchanged**
  (`git status` clean on both).
- ⛔ **No `--debug-events` instrumentation (§3.3).** It is a change to a tracked tool and is gated on
  approval. ⚠ **And the banked reproduction largely obviated it for the RANKING** — `tau`, `base`,
  the per-edge `d` values and the region source/size are **already printed** by the existing
  non-`--quiet` output (§3.2 above is entirely from it). The instrumentation is still worth building
  for the *fix* (it needs the per-frame series and the ring/view-delta numbers), and it is specified
  in the proposal.
- ⛔ **No new capture, and no camera-motion lever invented.** The brief allowed inventing one; it was
  not needed, so §4's *"`Source/` untouched anywhere"* is honoured **in full**. **This also removes
  the §3.2-vs-§4 contradiction rather than resolving it by judgement call.**
- ⛔ **No office data touched.** Every session read is StackOBot or Lyra, from this box's bank.
- ⚠ **The owner's transcribed field readings have still not arrived.** Nothing here depends on them.
  When they do, the single number to look for is **`tau`**: the prediction is that the field sessions
  read `tau` at or above **~0.5**, with the worst at or above **1.0**.

## 8. Consequences already ruled (recorded, acted on in no other way)

- The verifier **does not go to the client** for the M2 re-review until fixed and proven on a
  known-good real session.
- **Card Section G readings on field sessions are void** until then.
- This unit goes **ahead of** the identity work. `m51` stays HELD.

## 9. Commands (exactly as run, all read-only)

```powershell
# the gate, unmodified, on banked sessions
python tools\verify_capture.py --dir <SESSION> --label-pixel-gate --report-only --quiet   # summary
python tools\verify_capture.py --dir <SESSION> --label-pixel-gate --report-only           # + detail lines
# sessions used (each is _bench_sessions_bank\<NAME>\session_*):
#   M49_GEDGE_{BL,CT,MO,MT}_NAT, M49_GEDGE_{BL,MT}_SYN, A2L_LEGA, LYRA_SMOKE_01, A1L_LEGA, M50L_LG9
```

⚠ The detail run exits **255** under PowerShell only because `Select-Object -First N` closes the pipe
early (broken-pipe on stdout). The `--quiet` runs exit **0**. Not a tool failure.
