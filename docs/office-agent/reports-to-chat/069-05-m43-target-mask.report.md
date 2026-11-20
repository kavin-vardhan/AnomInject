# REPORT for 069-05-m43-target-mask.md
brief: 069-05-m43-target-mask.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: high
started: 2026-09-03 00:50:48 | finished: 2026-09-03 00:56:48 | duration: 6 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-05-m43-target-mask.live.txt

---- FINAL MESSAGE (Claude Code) ----
Plan written and committed. **Stopped at the plan** — the spec has four source-refuted assumptions, two of them blocking.

---

# SESSION 069, BRIEF 5 — `m43` TARGET ID MASK: **PLAN ONLY, NEEDS-DECISION**

## 1. Plan vs spec: **NOT "no deviations"** — 4 conflicts, 2 blocking

Bootstrap verified: `HEAD == origin/master == db2f49b` before any work. Plan is journal 069 §3, committed and pushed as **`bcd9bb6`**. **No source touched, no build, no predictions file** — the gate set cannot be pre-declared while two of its own gates are unsatisfiable against the design.

**What the spec gets right, verified and not re-litigated:** S1's pixel source (`MaskRT` is `PF_R8_UINT` at `SceneColor.ViewRect.Size()`, `AnomalyMaskSceneViewExtension.cpp:124-133`) · S1's "no new shader" · **S2's filter is genuinely necessary** (the shader emits *any* stencil ≥ `ReservedBase`, so census tags do land in an event frame's RT) · S3's rect equality at native output height · S5's mask-ON requirement · gate (ii) as the right load-bearing gate.

### 🚨 D1 — BLOCKING. The mask arms per EVENT, not per frame: **~21 masks for 90 frames**

`MaxArmsPerEvent = 4` (`AnomalyMaskMeasure.h:44`), and `ArmIfMeasurable` arms **one record per tick**. The mask was never a per-frame instrument — `m26` sized it to answer *"did this target draw anything during its window"*.

**Measured on the banked `m41` leg (90 captured frames, 6 events):** `M23 PASS` render passes **99** − `Census: ARM` **78** ⇒ **≈ 21 event mask arms.**

⇒ `target_mask/` would hold **~21 PNGs for 90 frames (~23 %)**, with ~69 rows at `mask_file: null`.
🚨 **Gate (i) is unsatisfiable as written** — it demands `frames_unavailable == 0` on a healthy leg; that counter is **~69 by construction**. ⚠ And 23 % coverage with unexplained gaps is not what "an instance mask to train on" means to the client.

**Options:** (a) raise `m26`'s arm budget — ⛔ **changes the veto's inputs**, a behaviour change to a shipped labelling path, which I will not take unprompted; (b) accept 23 % and document; **(c) a separate arm budget for the target mask** — same shader and pass, own `RequestId` space, `m26`'s `framesContributed` untouched. **(c) recommended** if real coverage is wanted.

### 🚨 D6 — BLOCKING. A hidden target produces **no file**, not a blank one

`ArmIfMeasurable` refuses on a hidden target (`AnomalyMaskMeasure.cpp:225-229`) — that is **`LOCK-1`**, and it exists so `m26` can never manufacture a `MEASURED_ZERO` that the veto would act on. So a `blinking`-hidden or `missing_object` frame has **no arm → no readback → no PNG**.

**Gate (iii) expects a blank PNG. The design produces no file.** Those mean different things — blank = *"measured, target not visible"* (real ground truth); absent = *"not measured"*. 🔑 **This is `m26`'s `NOT_MEASURED` ≠ `MEASURED_ZERO` distinction recurring at the frame level.** ⚠ And hidden frames are exactly what a hide-type anomaly is *about*, so the client would get masks where the target is visible and nothing where it vanished — the inverse of the point. **Fix: emit an explicit all-zero PNG from the known game-side hidden state** (`m40` already samples it at `OnWorldTickEnd`). ⛔ **Do not arm on a hidden tick — `LOCK-1` forbids it.**

### D4 — latent correctness: `m28` output height

Mask RT is view-rect sized; `Actual_Frames` are **resampled on write** when `CaptureOutputHeightDefault != 0`. No such key in the client ini today, so S3 holds — but the moment one is set, mask and picture disagree in size. ⚠ **A label mask must never be filtered**; bilinear would invent stencil values. **Implement nearest-neighbour, or refuse loudly when `EffectiveOutputHeight != 0` — refusing recommended.**

### D2 / D3 / D7 — non-blocking, but must be written correctly

- **D2 (cost, not a blocker):** the surface readback is gated by `if (Mode != Gpu)` and the shipped default is `gpu`, which reads back a **5 KB table, not the surface** — precisely what `m34` removed. S1 acknowledges this, but state it plainly: **`m43` re-introduces a full W×H readback per armed frame** (~0.9 MB at 720p), and under D1 option (c) it is paid **every** frame. S7 is mandatory.
- **D3 (wording):** the mask's `RequestId` **is `GFrameCounter`**; the capture frame's is a plugin-owned serial (`m31`). Different id spaces. The exact join is `labels.jsonl.frame_index` — the same join `P9` proved 704/704. S1's wording would send an implementer to the wrong id.
- **D7:** "multiple live targets keep their distinct values" is **unverified** — one arm per tick, and tags are restored after collection, so one tag per armed frame is the common case. Keep it out of client docs until a leg shows two values in one PNG.

## 2. Implementation: **not started** (per the brief's stop rule)

## 3. StartRun echo: **not applicable** — no code was written

## 4. NEEDS-DECISION — four, first two blocking

1. 🚨 **Coverage (D1)** — (a) raise `m26`'s budget ⛔ *touches the veto*, (b) accept ~23 %, or **(c) separate arm budget (recommended)**. Gate (i) must be re-worded either way.
2. 🚨 **Hidden frames (D6)** — emit an explicit all-zero PNG, or re-word gate (iii) and accept that hide-type anomalies ship without masks on their hidden frames.
3. **`m28` interaction (D4)** — nearest-neighbour resample, or refuse loudly. Refusing recommended.
4. **Cost (D2)** — confirm the full-surface readback's return is accepted; it compounds with D1(c).

📌 §3.8 of the journal records the parts **not** in doubt — file-by-file shape, the two label keys, the two counters, the knob, `P-C7 v2` for OFF-inertness, and gate (ii) as load-bearing — so the next brief starts from there rather than re-deriving it. ⚠ One detail worth carrying: mask PNGs must be numbered by **`session_index`**, not `frame_index` (`G161`).

**STOP.**
api-equivalent cost (not billed on Max): $9.75 | turns: 12 | running total across all briefs: $174.41
