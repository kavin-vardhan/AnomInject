# m55 — measured change evidence (v2, rewritten 2026-09-21)

**PLAN ONLY. No source, shader, tool, fixture or schema change was made. No build, no cook, no capture,
no leg, no tag, no merge.** Branch `feat/m55-change-evidence` off `master` `031a103`; the v1 plan is
`287ad3e`. `m51` `53bf725` untouched.

> 🚨 **`m55` v1 IS MEASUREMENTS ONLY.** The `m49` A1 `observable` predicate is unchanged **byte for
> byte**. No `present`/`absent` event verdict is produced, stored or printed anywhere in v1's output —
> the words do not appear as a field value in any artifact this unit writes. v1 publishes raw per-pair
> numbers, their coverage, their refusal reasons, and one durable final record per event. **A calibrated
> event verdict, and any change to what `observable` means, are a LATER unit** gated on an owner/client
> decision (`docs/OPERATING-CONTRACT.md` §3) which this plan does not pre-empt and does not assume.

**This document replaces the v1 plan in full.** v1 is preserved verbatim at the end of this file under a
`SUPERSEDED` fold; **nothing inside that fold may be cited.** The rewrite is driven by
`_reviews/081-02-codex-m55-design-review.md` (findings `F1`–`F9`, §4 lifetime table, §5 cost, §6
acceptance list) and its chat disposition `_reviews/081-02-chat-disposition.md` (**all nine findings
ACCEPTED**). Rulings are carried as `R1`–`R9` and each section names the ones it discharges.

Every source line cited below was re-read on `feat/m55-change-evidence` `287ad3e` during this session.
`287ad3e`'s diff against `031a103` contains **only documentation**, so these are `master`'s lines.

---

## §0 What this unit is, and the three contracts

`m55` v1 measures, per captured frame pair and per tagged target, **how many of the target's own
delivered mask pixels differ from the previous captured frame's delivered pixels, and by how much**,
together with the same two statistics over the rest of the same frame as a paired control. It publishes
that and nothing else.

Codex's §6 asks for three contracts before implementation. They are §1 (**numeric**), §2 (**identity and
lifetime**) and §3 (**temporal**). Each is stated so that an implementer can build it and a reviewer can
falsify it without reading the other two.

⛔ **What `m55` still cannot do, stated first so it is not claimed later.** `G263`'s correction is not
repealed by moving the measurement in-engine: a mask identifies pixels, not causes. Lighting, occlusion,
animation and camera motion move the same pixels. `m55` v1 reports *that the target's pixels differed
between two identified delivered images*. It does **not** report that the anomaly caused the difference,
that the effect persists after the measured pair, or that a person could perceive it.

### 0.1 The headline consequence of the rewrite: v1 needs NO COOK

v1's chosen mechanism was a GPU reduce pass, which forces a full cook (`G129`) and retires a container
quintet byte-unchanged since `m49` Phase B. **Codex `Q4` ruled CPU first and chat accepted (`R9`).**

With the GPU route parked, v1 adds **no global shader, no shader parameter struct, and no new `Content`
asset** — the one material it needs (`M_CorruptedTexture_Pink`, §7) is already a CDO hard-ref on
`UAnomalyInjectorSubsystem` (`AnomalyInjectorSubsystem.cpp:52-54`, accessor `:62-64`) and is already in
the cooked container. ⇒ **`m55` v1 is a code-only hot-swap (`G103`).** The container quintet
(`67EA1FE0`/`2CEFB8F4`/`E03C6610` + `A16A18A8`/`C70ECDAA`) survives, and every binary↔container pairing
in `_binary_baselines\README.md` survives with it. The whole of v1 §4.1 — the cook, `G92`, `G119`,
`G121`'s moving halves — **does not apply to this unit.**

---

## §1 The NUMERIC contract — `R2`, discharging `F6` and `F7`

### 1.1 Canonical pixels: the bytes the encoder receives, taken from the one function that produces them

The delivered PNG is produced by exactly this sequence (`AnomalyLabelWriter.cpp`, `EncodeAndWriteFrame`):

```
TArray<FColor> Pixels;
ConvertTightToBGRA(SrcFormat, BytesPerPixel, RawBytes, Width, Height, Pixels);   // :296-306
ResampleAndEncodeBGRA(OutFormat, Pixels, Width, Height, OutWidth, OutHeight, ImageBytes, bOutResampled);
```

**`Pixels` is the canonical buffer.** `m55` measures `Pixels`, obtained from the *same call* rather than
from a second conversion (§2.4), so agreement with the delivered bytes is **by construction, not by
comparison**. `ConvertTightToBGRA` calls `DecodeTightPixel` (`:266-294`) per pixel:

| source format | conversion | consequence `m55` inherits |
|---|---|---|
| `PF_B8G8R8A8` | channel reorder | exact |
| `PF_R8G8B8A8` | channel reorder | exact |
| `PF_A2B10G10R10` | `(V >> shift) & 0x3FF) >> 2` — **truncation** | Codex's `F7` example resolves here |
| `PF_FloatRGBA` | `Clamp(RoundToInt(h * 255), 0, 255)` | round-and-clamp, not a float mean |
| **anything else** | `default:` returns `FColor(0,0,0,255)` — **BLACK** | ⚠ see below |

🎯 **Codex's `F7` counterexample, resolved rather than argued.** A 10-bit red going `0 → 35` is
`8.724/255` normalised directly, but the writer emits bytes `0 → 8`, a difference of **8**, which the
`> 8` threshold does **not** count. Because `m55` reads the writer's own output, the delivered picture
and the measurement agree that this is a change of 8. **The delivered bytes are the definition; any other
normalisation is a different quantity and is not measured.**

🚨 **The `default:` branch is a false-zero generator and is refused, not measured.** A source format
outside those four converts to black, so a pair of such frames differs by exactly zero everywhere — an
instrument that reads *quiet* when it is in fact *blind* (`G96`'s shape, and `G159`'s: a control that
cannot fire is not a negative). ⇒ a frame whose `Frame.Format` is outside the four handled cases is
refused with `unsupported_delivery`, and the counter is printed.

### 1.2 Supported delivery scope

`chg_measured` is `false` with reason `unsupported_delivery` unless **all** of:

1. **PNG** (`Job.OutFormat == PNG`). JPEG is lossy, so the delivered bytes are not `Pixels`.
2. **No resample**: `OutWidth == Width && OutHeight == Height`. `AnomalyLabel::DeriveOutputSize`
   (`AnomalyCaptureSubsystem.cpp:4053`) derives these from `IAI.Capture.OutputHeight`; when they differ,
   `ResampleAndEncodeBGRA` resamples and the PNG is no longer `Pixels`. *(`m43` separately refuses the
   target mask entirely when `OutputHeight != 0`, so in practice this case already has no region — the
   check is kept because the two guards are independent and one may move.)*
3. **Single view**: the receipt's view index is `0` on both frames of the pair (§2.3).
4. **Source format** in the four handled cases of §1.1.
5. **SVE grab point** (`bSveCapture`). The backbuffer capturer produces no view rect, no family frame and
   no view index, so it cannot carry the §2 receipt. ⛔ **The backbuffer path is declared unsupported in
   v1** and reports `unsupported_delivery` rather than being measured with a weaker receipt.

### 1.3 Region and control

For captured frame `N` carrying a target mask, let `M` be the **delivered mask image for frame `N`** —
that is, the `Gray` buffer at `AnomalyCaptureSubsystem.cpp:1295`, after non-event tags are zeroed at
`:1300-1308` and before it is handed to `EnqueueTargetMaskPng` at `:1363`. `M` is byte-for-byte the
`target_mask/frame_%05d.png` the client receives, which is what lets §6's oracle reproduce the region
exactly from disk.

- **Region for tag `t`** = `{ p : M(p) == t }` — the **current** frame's mask only.
- **Control** = `{ p : M(p) == 0 }` — the complement of the union of **all** tag masks in that frame, so
  a second live target's own pixels and its spill never enter the control (`R2`). Census-tagged actors
  are zeroed at `:1300-1304` and therefore *are* in the control, which is correct: the census tags for
  measurement and does not change appearance.
- ⚠ When `IAI.Bench.MaskPairingProbe` is on, the probe's tag `255` is a live tag (`:1181`) and is
  therefore excluded from the control. Bench-only, stated so a probe leg's control is read correctly.

### 1.4 The per-pixel quantity and the per-pair statistics

Per pixel, over the canonical BGRA8 buffers `cur` and `prev`:

```
d(p) = max( |cur.R - prev.R|, |cur.G - prev.G|, |cur.B - prev.B| )      // bytes, 0..255
```

Alpha is ignored — `DecodeTightPixel` sets `A = 255` unconditionally, so it is constant by construction
and carries no information.

Per target `t`, per pair:

| field | definition |
|---|---|
| `chg_n` | `count{ p : M(p) == t }` |
| `chg_gt8` | `count{ p : M(p) == t and d(p) > 8 }` — strict `>`, byte units |
| `chg_hist` | 8 ints, bins over `d`: `[0] [1-2] [3-4] [5-8] [9-16] [17-32] [33-64] [65-255]` |
| `chg_sum` | `sum{ d(p) : M(p) == t }`, integer |
| `chg_mean` | `chg_sum / chg_n / 255`, 4 decimal places |

and the control twins `ctl_n`, `ctl_gt8`, `ctl_hist`, `ctl_sum`, `ctl_mean` over `{ p : M(p) == 0 }`.

**Three free internal asserts, each counted and printed, none of them a gate on the scene:**

1. `chg_n == target_pixels(t)` — the same `Count[t]` the row already publishes. `MASK-TIE`
   (`:1345-1356`) already proves `tableCount == pngCount` per tag, so a mismatch here is an instrument
   fault, not a finding. Counter `chg_denominator_mismatch`.
2. `chg_gt8 == chg_hist[4] + chg_hist[5] + chg_hist[6] + chg_hist[7]` (the `>8` bins).
3. `chg_n == sum(chg_hist)`.

🎯 **Why an 8-bin histogram and not two thresholds.** Codex `F5`: *"Two stored threshold counts and a sum
also cannot reconstruct counts for arbitrary new pixel thresholds; those sweeps require the saved images
or an adequate histogram."* The histogram makes every later threshold in `{1,3,5,9,17,33,65}` recoverable
**offline from banked legs, without the images**, which is what lets the deferred calibration unit (§8)
sweep without a re-capture. It costs one extra increment per pixel.

**Why `max` over channels and not luminance.** `corrupted_texture` swaps a surface to solid magenta; a
green→magenta swap can be near-neutral in luminance. A luma metric would under-read the catalogue's most
visible change. It also matches the verifier's existing statistic (`verify_capture.py:552-555` takes
`ImageChops.lighter` over the three difference channels), which is what makes §6 a comparison of like
with like.

### 1.5 Both denominators are guarded — `R2`, `F9`

`chg_measured` is `false` with reason `empty_region` when **either** `chg_n == 0` **or** `ctl_n == 0`.
Codex `F9` is explicit that `Count[t] > 0` does not prevent `W*H - Count[t] == 0`, and this project has
already met a target whose *claimed extent was the entire frame* (`m27`'s `InstancedFoliageActor`).
`chg_n == 0` is also exactly the already-occluded-at-onset hide case (§5.1).

### 1.6 The control is raw data, never a subtrahend and never a threshold in v1 — `R5`

- **No subtraction.** `G260` (079-02/03) built `target - control`, gated it and had it refuted in both
  directions: parallax is content-weighted so it does not cancel camera motion, and on a pinned bench leg
  the onset read `d_region 0.02286` against `d_ring 0.02872`, so the subtracted quantity went **negative
  at the exact frame the anomaly started**. Its premise is false for these anomalies — `m26`'s `A35`/`A-4`
  measured hiding `SM_Ramp2` changing **more outside its own bbox than inside** (peak-OUT `0.2955` vs
  peak-IN `0.1785`). None of that is a property of *where* the measurement is taken.
- **No control threshold in v1.** There is nothing to calibrate a cap against until §8's unit runs, and a
  cap invented now would be the uncalibrated floor `F5` refuses. The raw control ships on **every valid
  pair**, refused or not.
- **Two separate booleans, and they are not the same question.** `chg_measured` = *the pair arithmetic is
  valid*. `chg_eligible` = *this pair may be used as evidence for an inference*. **In v1
  `chg_eligible == chg_measured` always**; the field exists so that the later policy has a place to land
  without moving `chg_measured`'s meaning, and so that a consumer written now does not have to change.
- 📌 **Client-facing line, fixed here:** *a low control value is not a causal certificate.* Local occlusion
  or parallax can be large inside a target and tiny across the rest of the picture, and a real anomaly's
  spill can do the reverse.

### 1.7 The prior that argues hardest for §8's deferral

Codex's read-only probe of the three banked `G270` Lyra events, using each **current** target mask and
the existing `> 8` byte threshold:

| banked event | pair | target `>8` fraction | outside-target `>8` fraction | target mean /255 |
|---|---:|---:|---:|---:|
| 0, Cube5 | 22 → 23 | 3.2161 % | 2.1789 % | 0.005918 |
| 1, Cube4 | 63 → 64 | 0.1774 % | **0.2687 %** | 0.003237 |
| 3, Cube4 | 141 → 142 | 11.3463 % | 10.8893 % | 0.018934 |

🚨 **On all three, the control is the same order as the target, and on event 1 the control is HIGHER.**
Event 3 reaches **12.9160 %** target change within its first four labelled pairs. Whatever else these
numbers mean, they say that on a moving-camera host a naive target-fraction threshold would not separate
these events from their own background — which is precisely why §8's calibration is a unit of its own
with a held-out set and a *no-separation ⇒ no gating* stop, and why v1 publishes and refuses to judge.
⛔ These are **artifact arithmetic checks** on banked data, not certification of pairing, effect or cause.

---

## §2 The IDENTITY and LIFETIME contract — `R3`, discharging `F2` and Codex §4

### 2.1 What the source actually guarantees, and what it does not

Seven facts, each re-read this session. Together they are why v1's *"pin both request ids through
`PendingSnapshots` and require `SessionIndex(prev) == SessionIndex(cur) - 1`"* cannot work.

1. 🚨 **The predecessor's snapshot is gone.** `Async->PendingSnapshots.Remove(Frame.RequestId)` runs
   immediately after the writer job is enqueued (`AnomalyCaptureSubsystem.cpp:4153`). By the time frame
   `N` is processed, `N-1`'s entry has already been removed. The v1 lookup resolves nothing.
2. 🚨 **Arrival order is not capture order, and the inversion is exact, not hypothetical.**
   `Drain_RenderThread` scans in-flight items **backwards** (`AnomalySveCapturer.cpp:287`) and appends
   ready ones to `Completed` (`:359`); `PopCompleted` returns `Completed[0]` FIFO (`:371-379`).
   `InFlight` is appended in submit order, so **two frames that become ready in the same drain pass are
   popped newest-first**. `ProcessCompletedFrames` then iterates that batch in order (`:3902`).
3. 🚨 **Held batches re-order it again.** A frame whose mask outcome has not resolved is pushed into
   `Async->TargetMaskHeldFrames` and re-prepended to the *next* tick's batch (`:3892-3893`, `:3937-3948`),
   bounded by `GTargetMaskMaxHoldTicks = 4` (`:169`).
4. 🚨 **Colour and mask do not consume arms the same way.** Colour takes **one** wanted request per
   publish, FIFO (`AnomalySveCapturer.cpp:45-73`). The mask takes **all** pending arms into the next
   eligible pass — `ServedIds = MoveTemp(PendingArms); RequestId = ServedIds[0]`
   (`AnomalyMaskSceneViewExtension.cpp:122-131`) — and then **clones one render's result to every served
   id, with only one `PixelOwner` receiving `MaskPixels`** (`:512-529`). Assigning those clones an
   intended session index does not make them that session index's render.
5. **The clone-without-pixels case is already refused today.** `ServiceTargetMask` reads
   `MaskPixels.Num() < W*H` and lands `Unmeasured` (`:1279-1290`), so such a frame carries
   `target_pixels == -1` and `m55` never sees a region. ✅ That closes half of Codex's *"pending mask arms
   coalesced"* row. ⛔ **It does not close the other half:** a clone that *did* receive pixels still may
   not be the render its session index names. Only a receipt match closes that.
6. 🚨 **A key-ring lookup does not consume.** `LookupKey` returns the entry and leaves it in the ring
   (`AnomalySveKeyRing.cpp:132-148`). A second view in the same family resolves the **same**
   `Entry.RequestId` with `bWanted` still set, and `AfterPass_RenderThread` would submit a second
   in-flight readback under that id (`AnomalySceneViewExtension.cpp:96-160`).
7. **Nothing that travels today can identify a render.** `FAnomalyCapturedFrame`
   (`AnomalyFrameCapturer.h`) carries `RequestId, Width, Height, Format, BytesPerPixel, RawBytes` — no
   rect origin, no family frame, no view index, no time. `FAnomalyMaskResult` carries `ViewRectSize` and
   no render identity at all.

⛔ **`PendingSnapshots` is NOT retained longer as a workaround** (`R3`, explicit). The fix is a receipt.

### 2.2 The receipt

Every retained buffer carries an immutable `FAnomalyChangeReceipt`, filled once and never edited:

| field | type | source |
|---|---|---|
| `run_epoch` | `uint32` | §2.5 |
| `request_id` | `uint64` | `Entry.RequestId` |
| `session_index` | `int32` | `Snap->SessionIndex` |
| `family_frame` | `uint32` | `View.Family->FrameNumber` in the colour callback |
| `view_index` | `int32` | index of `&View` in `View.Family->Views` |
| `rect_min` / `rect_size` | `FIntPoint` ×2 | `SceneColor.ViewRect` (`AnomalySceneViewExtension.cpp:113`) |
| `source_extent` | `FIntPoint` | `Texture->Desc.Extent` (`:121`) |
| `format` / `bytes_per_pixel` | `EPixelFormat` / `int32` | `Texture->Desc.Format` |
| `cut_counter` | `uint32` | §2.5 |
| `cam_hash` | `uint64` | §2.6 |
| `capture_time` | `double` | `FPlatformTime::Seconds()` at submit and at drain, both stored |
| `delivered` | `bool` | **stamped by the writer** — §2.4 |

`family_frame`, `view_index`, `rect_min` and the submit time are new fields on `FAnomalyCapturedFrame`,
filled by `SubmitInFlight_RenderThread` (which already receives `Rect`, `SourceExtent` and `Format`). The
rest are stamped in the drain, where `Snap` and the run epoch are in scope.

### 2.3 Pair validity

A pair `(prev, cur)` is valid — `chg_measured: true` — only when **all** hold:

```
prev.run_epoch      == cur.run_epoch
prev.cut_counter    == cur.cut_counter
prev.view_index     == cur.view_index == 0
prev.session_index  == cur.session_index - 1
prev.rect_min       == cur.rect_min  and  prev.rect_size == cur.rect_size
prev.source_extent  == cur.source_extent
prev.format         == cur.format
prev.delivered      and  cur.delivered
cur's mask payload present, and its receipt matches cur's colour receipt
chg_n > 0  and  ctl_n > 0
delivery scope of §1.2 satisfied on both
```

🔑 **The mask join is by receipt, never by intended session index** (`R3`, `F2`). The mask arm for frame
`N` is bound at the same tick as the colour arm (`TargetMaskArmedTick = GFrameCounter`, consumed under
`TargetMaskArmedTick == GFrameCounter` at `:891`), but §2.1 fact 4 shows the *render* that serves it may
serve several arms. The mask SVE therefore records the serving pass's `family_frame` and `view_index`
alongside its result, and `m55` requires them to equal the colour frame's. A mismatch is
`mask_payload_missing` and the pair is unmeasured. ⚠ **This is the one item that requires touching the
mask extension.** It adds two fields to `FAnomalyMaskResult` and changes **no** mask shader, no reduce, no
table stride, no readback and no existing field — so `MASK-TIE`, `TARGET-PIXELS TIE`, `target_pixels`,
`target_drawn_pixels`, `bbox_drawn_px`, ONSET and pairing are untouched by construction.

### 2.4 Ordering, ownership and the `MoveTemp`s — stated exactly, as `R3` requires

**The change stage is a single serial worker**, not a per-frame task: one task in flight at a time,
re-queued while work remains. It admits receipts **ordered by `session_index`**, with a cursor and a
bounded wait (§2.7). It runs on a worker thread and never on the game thread (`R9`).

Two `MoveTemp`s decide where the shared refs are taken:

| existing line | what moves | where `m55` takes its ref |
|---|---|---|
| `AnomalyCaptureSubsystem.cpp:4140` `Job.RawBytes = MoveTemp(Frame.RawBytes)` | source-format bytes → writer job | **Downstream.** `m55` never contends for `Frame.RawBytes`. |
| `AnomalyCaptureSubsystem.cpp:1295` `TArray<uint8> Gray = MoveTemp(Result.MaskPixels)` | mask pixels → local, destroyed at scope end | **At that line.** `Gray` becomes `TSharedRef<const TArray<uint8>>`; the two existing consumers (`FoldExposureExclusion` `:1359`, `EnqueueTargetMaskPng` `:1363`) already take `const TArray<uint8>&` and are re-pointed at `*MaskRef` — **no copy, no second owner, no call-site semantics change.** |

**The canonical colour ref is taken inside `EncodeAndWriteFrame`, between `ConvertTightToBGRA` and
`ResampleAndEncodeBGRA`.** `EncodeAndWriteFrame` gains an optional out-parameter; the writer job then
publishes `{ receipt, TSharedRef<const TArray<FColor>>, delivered }` to the change stage after
`SaveArrayToFile` has returned. Consequences, all deliberate:

- **One conversion, two consumers.** The measurement reads the exact buffer the encoder read. No second
  `ConvertTightToBGRA` pass is added, so the per-frame conversion cost is unchanged.
- **`delivered` is exact, not inferred.** It is the writer's own success, known in the same job — which is
  Codex's *"declare the measurement stage and success receipts"* and *"never silently bridge a failed
  predecessor."* An undelivered predecessor yields `predecessor_undelivered`.
- **Retention does not scale with queue depth.** `FAnomalyAsyncWriter::Enqueue` dispatches with
  `Async(EAsyncExecution::ThreadPool, ...)` (`AnomalyAsyncWriter.cpp:22`) — unordered and concurrent, which
  is `G162`'s recorded cause. The canonical buffer is allocated **inside the running job**, so the bytes
  in flight are bounded by concurrent jobs, not by `Pending`.

🚨 **And the ordering fact settles where the per-pair numbers can live.** The label row for frame `N` is
serialised in the drain (`BuildLabelRecordForSnapshot`, `:4132`) and handed to the writer at `:4151`,
**before** `N`'s canonical buffer exists. A pair `(N-1, N)` therefore cannot be measured in time to
appear in `N`'s `labels.jsonl` row without delaying every row by a frame. ⇒ **v1 writes no per-row
change field into `labels.jsonl` at all** (§4). That is not a compromise: it also removes the delivery
hole `F3` names, because `Job.bWriteLabels = !bDeliveryMode || bLabelsInDelivery` (`:4150`) can switch
`labels.jsonl` off entirely.

### 2.5 Epoch, cut counter, and what resets together

`run_epoch` increments at `StartRun` and at `FinishRun`. `cut_counter` increments on world/level change
and on an explicit bench lever. On either increment, **one function clears all of it together**: the
retained predecessor canonical, every phase reference, every retained mask, the receipt cursor, the
change stage's queue and every pending phase record. Results whose receipt carries a stale
`run_epoch`/`cut_counter` are **rejected and counted** (`epoch_reset`).

⚠ **A camera cut is NOT detected, and that is declared rather than papered over.** Codex's §4 pairs
*"same-size camera cut"* with level/world transition; the latter is covered by the epoch, the former is
not, because nothing in the producer knows a teleport from a fast pan. A same-size cut passes every check
in §2.3 and is reported only through `cam_moved` (§2.6), which is a caveat field, not a refusal. The gate
table (§9) carries this as **UNEXERCISED, with the reason**.

### 2.6 `cam_moved` — a caveat, never a validity rule

`cam_hash` is a 64-bit hash of the view quantised to **1 cm** of location, **0.01°** of rotation and
**0.01°** of FOV, taken from `Snap->View` — the same `FAnomalyViewInfo` the row already publishes as
`view`. `cam_moved = (prev.cam_hash != cur.cam_hash)`.

🔑 **Moving-camera capture stays supported.** Codex: *"retaining only a full pose equality rule would make
much of moving-camera use unsupported. The design must state which claim remains valid on that path."*
The claim that remains valid is the measured one — *these two identified delivered images differ by this
much over this region* — and `cam_moved` is what tells a consumer that motion is inside it. No raw pose
delta is added; `labels.jsonl` already carries `view` per frame for anyone who wants one.

### 2.7 Bounded wait, bounded bytes

| bound | value | on breach |
|---|---|---|
| out-of-order wait | **4 captured frames** — a deliberate echo of `GTargetMaskMaxHoldTicks = 4` (`:169`), **not** a derivation from it | `out_of_order_timeout`, advance the cursor, count |
| retained bytes | **64 MB**, `IAI.Capture.ChangeMaxBytes`, `A48` echo | `budget_exceeded`, drop oldest, count |
| concurrent phase references | **2** | `budget_exceeded` on the third |
| in-flight canonical buffers admitted | **2** | back-pressure, then `budget_exceeded` |

**The arithmetic behind 64 MB**, `FColor` = 4 B/px, mask = 1 B/px:

| | 1280×720 | 1920×1080 |
|---|---:|---:|
| predecessor canonical ×1 | 3.69 MB | 8.29 MB |
| phase references ×2 | 7.38 MB | 16.59 MB |
| retained masks ×5 (hold bound + 1) | 4.61 MB | 10.37 MB |
| canonical in flight ×2 | 7.38 MB | 16.59 MB |
| **worst case** | **23.06 MB** | **51.84 MB** |

⚠ **This is a new consumer of `m51` `R7`'s open writer-byte bound.** `m51`'s `F1` is **not** on `master`
(`git merge-base --is-ancestor 6713f51 master` exits 1, and neither `bAwaitingTargetMask` nor
`TargetMaskWaitTicks` appears in `master`'s `AnomalyCaptureSubsystem.cpp`), so there is no code
interaction — but the byte bound and `F1`'s rule (*wait only for an outcome that can exist*, which the
bounded wait above honours) both need reconciling at whatever merge brings the two together.

---

## §3 The TEMPORAL contract — `R4`, discharging `F1` and `F3`

### 3.1 Scope is the PHASE, not the event

A **phase** is one maximal run of consecutive captured frames on which a given fire is labelled — i.e.
`IsFireLabelledThisFrame(F)` (`AnomalyCaptureSubsystem.cpp:4845-4865`) is true, which is already sampled
into `Snap->FireLabelled`. `blinking` and `lod_popping` produce many phases per event; `missing_texture`
produces one. Phases are keyed by `Id@StartFrame` + phase ordinal — the same event key `TagEvent` uses at
`:1167`.

### 3.2 The onset pair, `pending`, and `indeterminate`

```
onset_pair(phase) = ( last unlabelled captured frame before the phase , first labelled captured frame )
                  = ( session_index f0 - 1 , f0 )
```

- The phase enters state **`pending`** when `f0` is seen.
- It becomes **`measured`** when the onset pair's measurement resolves with `chg_measured: true`.
- It becomes **`indeterminate`** when the onset pair cannot be measured, carrying the refusal reason.
  A phase that is still `pending` at finalisation becomes `indeterminate` with its terminal reason.

🚨 **`F1`'s core requirement, in one line: one measured quiet pair NEVER stands in for an unobserved
onset.** Later window pairs are published, but they cannot move a phase out of `indeterminate`. In v1
this costs nothing, because v1 issues no verdict; it is written into the state machine now so that the
later policy unit inherits it rather than having to rediscover it.

⚠ **`pending` / `measured` / `indeterminate` are COVERAGE states, not verdicts.** No value of any field
in v1 says the change was present or absent.

### 3.3 The window, and the pre-onset reference

The window is the phase's **first `K = 4` labelled frames**, giving up to 4 pairs, the first being the
onset pair. ⚠ **`K = 4` is an experimental resource budget and is NOT a completeness bound** — Codex `Q4`,
accepted verbatim. It is not derived from the veto's arm budget; it merely happens to equal it.

Each window row also carries a comparison against the phase's **reference frame** — the canonical buffer
at `f0 - 1`, retained for the life of the window:

```
ref_gt8(i)  = count{ p : M_i(p) == t and max-channel |cur_i(p) - ref(p)| > 8 }
ref_mean(i) = mean of that difference over the same pixels, /255, 4 dp
```

🎯 **This is `F1`'s gradual-change answer, and it is arithmetic rather than hope.** Codex's counterexample
— all target pixels walking `0, 3, 6, 9, 12` — gives four adjacent-pair `chg_gt8` values of **zero** and a
`ref_gt8` fraction of **1.0**. Adjacent differencing cannot see it; the reference can.

Two honesty clauses that travel with it:
- **`ref_*` is screen-space and is not motion-compensated.** Under camera or target motion it measures
  motion as well as content. `cam_moved` accompanies every row.
- ⛔ **`ref_*` is never used to make a negative call.** In v1 nothing makes any call; the clause is fixed
  now so the later unit cannot quietly adopt it as one.
- Free consistency check: on the onset row `ref == prev`, so `ref_gt8 == chg_gt8` and
  `ref_mean == chg_mean` exactly. Counter `ref_onset_mismatch`, expected 0.

### 3.4 Running aggregates are NAMED as running

`F3`: *"Name running aggregates as running, or publish final aggregates separately."* Both are done.
Per-pair rows carry `chg_gt8_max_sofar` and `chg_mean_max_sofar` — suffixed, so a reader cannot mistake a
partial maximum for the event's. Final aggregates appear **only** in the final record (§4.3).

### 3.5 Finalisation — enumerated, with what each writes

| event | what is written |
|---|---|
| **phase end** (first unlabelled frame after a labelled run) | the phase record is closed: state, coverage, reason histogram, final aggregates |
| **event end** (`BeginRevert` / fire removed) | every open phase of that event is closed |
| **drop** (target destroyed, fire lost, `OnTargetLost`) | open phases closed with their terminal reason |
| **teardown** (`Deinitialize`, world teardown) | everything closed; the change stage is stopped and its queue cleared |
| **run end** (`FinishRun`) | bounded flush of the change stage (same shape as `FlushPending`), then every remaining phase closed, then the sidecar written |

**Results arriving after their phase is finalised are rejected and counted** (`late_results`), and can
never mutate a closed record or leak into the next run — the epoch check of §2.5 catches the cross-run
case independently.

### 3.6 Coverage is published, not implied

Per phase: `pairs_required` = `min(K, labelled frames in the phase)`; `pairs_measured`; and a **reason
histogram** over the closed enum of §4.2. A phase with `pairs_measured == 0` is `indeterminate` and says
which reason dominated. ⚠ `pairs_required` is a *window* count, not a claim that four observations cover
a visual transition.

---

## §4 Fields, reasons, and the artifact — `R2`, `R4`

### 4.1 What v1 does and does not touch

| artifact | v1 |
|---|---|
| `annotation.json` | ⛔ **untouched.** Root key set stays as measured this session: `label_schema`, `session_id`, `video`, `anomalies` (4). Per-event key set unchanged. `P6` does not move. |
| `label_schema` | ⛔ **stays `2`.** `m51` owns the bump to 3; two units must not both move it. |
| `labels.jsonl` | ⛔ **row and anomaly key sets untouched** — §2.4's ordering fact makes a per-row change field impossible without delaying rows, and `:4150` can switch the file off entirely. |
| `run_summary.json` | **+ one declared block.** Root is **86 keys** today (counted this session over the `Root->Set*Field` calls in the run-summary builder). |
| 🆕 `change_evidence.jsonl` | **new per-run sidecar**, written once at `FinishRun`, sorted by session index. |
| `target_mask/*.png`, `mask_map.json`, `selection_provenance.json`, `run.json`, `census_mask/*` | untouched |

**The sidecar is written in BOTH delivery modes**, like `mask_map.json` (`AnomalyLabelWriter.cpp:624`)
and unlike `selection_provenance.json` (`:654`). That is `R4`'s durability requirement and `F3`'s delivery
hole closed in one step. It is written **once, at finish, from an in-memory accumulator, sorted** — not
appended per frame — because per-frame appends from the thread pool are exactly `G162`'s non-determinism.
Accumulator bound: 100,000 rows, overflow counted in `change_rows_dropped`.

### 4.2 The closed refusal-reason enum

Exactly twelve values. Nothing else may be emitted; an unmapped condition is a defect, not a new reason.

| reason | meaning |
|---|---|
| `first_frame` | no predecessor exists — the run's first captured frame, or the phase starts there |
| `predecessor_missing` | session indices skip: the predecessor was dropped (extent clamp `AnomalySceneViewExtension.cpp:122-134`, key-ring miss `:96-105`, `CAP-PAIR-DROP` `:3926-3935`) |
| `predecessor_undelivered` | the predecessor's writer job did not succeed; `delivered` is false |
| `out_of_order_timeout` | the ordered cursor waited its bound and the predecessor never arrived |
| `epoch_reset` | `run_epoch` or `cut_counter` differs across the pair, or a late result carries a stale one |
| `view_mismatch` | `view_index != 0` on either frame, or the two differ |
| `extent_mismatch` | rect origin, rect size or source extent differ across the pair |
| `mask_payload_missing` | no mask pixels for the current frame, or the mask receipt does not match the colour receipt |
| `unsupported_delivery` | §1.2: JPEG, resample, multi-view, unhandled source format, or the backbuffer grab point |
| `budget_exceeded` | a §2.7 bound was hit |
| `empty_region` | `chg_n == 0` or `ctl_n == 0` |
| `no_labelled_frames` | the event produced no labelled frame at all, so no phase ever opened (`R7`) |

### 4.3 The sidecar's two record kinds

`change_evidence.jsonl`, one JSON object per line, `"kind"` discriminating.

**`"kind": "pair"`** — one per (session index, tag) in a window:

| key | type |
|---|---|
| `kind` | `"pair"` |
| `session_index`, `prev_session_index` | int |
| `event`, `phase_ordinal`, `window_index` | string, int, int (`window_index` 0 = onset pair) |
| `mask_value` | int (the tag) |
| `chg_measured`, `chg_eligible` | bool, bool |
| `reason` | string, one of §4.2; `null` when measured |
| `chg_n`, `chg_gt8`, `chg_sum` | int ×3; `-1` when unmeasured |
| `chg_hist` | array of 8 ints; `null` when unmeasured |
| `chg_mean` | float 4 dp; `null` when unmeasured |
| `ctl_n`, `ctl_gt8`, `ctl_sum`, `ctl_hist`, `ctl_mean` | the control twins, same conventions |
| `ref_gt8`, `ref_mean` | int, float; `null` when no reference was retained |
| `chg_gt8_max_sofar`, `chg_mean_max_sofar` | int, float — **running**, per phase |
| `prev_target_pixels` | int — §5.1; structurally `-1` on the onset pair |
| `cam_moved` | bool |
| `tau_px` | int — the effective threshold, `8`, echoed per row so a sweep cannot be mis-joined |

**`"kind": "event"`** — one per event, written at finalisation:

| key | type |
|---|---|
| `kind` | `"event"` |
| `event`, `anomaly_type`, `target` | string ×3 |
| `phase_count` | int — the true number of phases |
| `phases` | array, **capped at 8 entries**; truncation is visible because `phase_count` is published beside it |
| `phases[].ordinal`, `.state` | int, `"measured"` / `"indeterminate"` |
| `phases[].first_labelled_index`, `.onset_prev_index` | int ×2 |
| `phases[].pairs_required`, `.pairs_measured` | int ×2 |
| `phases[].reasons` | object: reason → count |
| `phases[].chg_gt8_max`, `.chg_mean_max`, `.ref_gt8_max` | **final** aggregates, no `_sofar` suffix |
| `late_results` | int — results rejected after this event was finalised |

### 4.4 The `run_summary` block

`change_pairs_measured` · `change_pairs_refused` · `change_reason_<r>` for each of the twelve ·
`change_phases_measured` · `change_phases_indeterminate` · `change_events_with_phases` ·
`change_rows_dropped` · `change_late_results` · `change_denominator_mismatch` ·
`ref_onset_mismatch` · `change_tau_px` · `change_window_k` · `change_max_bytes` ·
`change_bytes_high_water` · `change_worker_ms_total` · `change_grab_point` · and the `A48` provenance
string for every knob — **the EFFECTIVE value and where it came from**, never the value requested.

---

## §5 Per-class rules — `R6`

### 5.1 Hide class (`blinking`, `missing_object`) — `F8`

`m45` drops the main and depth passes while keeping `bRenderCustomDepth`, so a hidden target still has an
RT0 silhouette and therefore a denominator. The onset pair is (last visible) → (first hidden) over exactly
those pixels.

- ✅ **`target_drawn_pixels == 0` is SUPPORTING evidence and is never a necessary condition.** `G258`'s
  asymmetry is unchanged. The `drawn == count` case on a correctly hidden object is an **accepted case**,
  and the source says why in its own words (`AnomalyCaptureSubsystem.cpp:4023-4039`): `m45` silences the
  main pass and the depth pass through separate flags, and the measured bench frame reading `drawn == count`
  had in-bbox mean luminance **191.5** against a hidden band of 191.5–192.1 and a visible band of 167–168 —
  the object was **absent from the picture**. ⛔ `m55` does not touch `observable` and must not be read as
  reinstating the veto the 077 ruling removed. `change_*` is **never** an input to the `m26` veto.
- **Already occluded at onset** ⇒ the silhouette is empty ⇒ `chg_n == 0` ⇒ `empty_region` ⇒ the phase is
  `indeterminate`. An empty mask is not a zero-change measurement (`F8`, verbatim).
- 🚨 **A structural limitation, found from source and declared rather than discovered later: the producer
  cannot supply a pre-onset silhouette.** Target-mask arming is gated on `IsFireLabelledThisFrame`
  (`AnomalyCaptureSubsystem.cpp:1156`), so the frame **before** a phase begins is by definition unlabelled
  and is **never armed**. `prev_target_pixels` is therefore `-1` on every onset pair, for every class. It is
  meaningful on window pairs 1..3, whose predecessor is labelled. ⇒ **`m55` v1 does not establish pre-onset
  visibility**, and a disappearance claim that needs it cannot be made from this unit's output. The only
  thing that would supply it is arming the mask a frame before a fire starts, which requires a lookahead the
  producer does not have. Published as a limitation, not as a plan.

### 5.2 Texture class (`missing_texture`, `corrupted_texture`, `stuck_low_mip`)

The silhouette persists and is fully drawn, so the denominator is large and stable and the change is a
content change inside it. This is the class `G270` is about. `stuck_low_mip`'s `080-06` window already
starts at the first measurably-held frame, so the onset pair straddles unheld→held by construction.
⚠ **The streamer's mip arrival can be gradual**, which is exactly the shape §3.3's reference comparison
exists for; the gate is `G-GRAD` (§9), with its declared partial coverage.

### 5.3 LOD class

`lod_popping` is the only catalogue member with repeated in-window transitions, so it is the natural
multi-phase case and the best natural exercise of §3.1's phase scope. ⚠ At the compiled
`LodMaxDistance` default of 200 cm this bench produces **zero** `lod_popping` events (`m30`; `080-06`'s
`G-B` had to pass `IAI.Anomaly.LodMaxDistance 50000` to fire at all) — so any `lod_popping` leg **declares
that lever in this file before the leg** or it is vacuous (`G146`).

### 5.4 Light and global classes — unchanged, and structurally so

`lighting_mismatch` matches `ULightComponent` by substring (`Anomaly_LightingMismatch.cpp:48`); the
renderable-visible set is static-or-skinned meshes only (`G33`) and the stencil tag is written onto
`UPrimitiveComponent`s. A light actor has none, so `ArmTargetMaskOwn` finds no taggable actor and the
frame lands `Unmeasured`. ⇒ it already ships `target_pixels: -1` and `observable: null`, and `m55` changes
nothing for it. `camera_clipping` and `time_dilation` are global: no target, no tag, no mask.

⇒ **all three produce no `change_evidence.jsonl` rows.** 📌 A future `m53` adding a flicker/lighting
anomaly **must bring its own region source**; `m55` deliberately does not invent one.

---

## §6 The verifier oracle — `R2`, discharging `F6`

### 6.1 Why the existing statistic cannot be the comparison

Three concrete mismatches, all re-read this session:

1. The verifier's `d` is a **fraction of pixels** whose max-channel difference exceeds a byte threshold
   (`verify_capture.py:552-555` binarises with `255 if v > t else 0`; `_region_frac` at `:561` divides by
   `npix`) — not a mean magnitude. On 100 pixels each changing by 10/255, its `d` is **1.0** and a mean is
   **0.039216**.
2. Its region is the **union of the two frames' masks** (`union = ImageChops.lighter(a, b)` at `:767`),
   while `m55`'s region is the **current** frame's mask alone.
3. It reports its own **selected transition pair**, which need not be the producer's onset pair.

### 6.2 `tools/verify_capture.py --change-oracle`

A new, independent read that recomputes from the decoded PNGs on disk:

- **Same pair ids** as the producer: it reads `change_evidence.jsonl` and, for each `"kind":"pair"` row
  with `chg_measured: true`, decodes `Actual_Frames/frame_%05d.png` for `prev_session_index` and
  `session_index` and `target_mask/frame_%05d.png` for `session_index`.
- **Same region**: `mask == mask_value` for the target, `mask == 0` for the control. This reproduces §1.3
  exactly, because `Gray` *is* the delivered mask PNG.
- **Same threshold**: strict `> 8` on the max over R,G,B, read from the row's own `tau_px`.
- **Reports**: `chg_gt8` vs recomputed, `ctl_gt8` vs recomputed, `chg_mean` vs recomputed, `ctl_mean` vs
  recomputed — count against count and mean against mean — plus **coverage**: rows compared, rows claimed,
  rows unavailable and why.
- ⛔ **Exit codes unchanged on all six CLI cases. No new verdict. The 079 outcome vocabulary
  (`CONSISTENT` / `OFFSET-NOTE` / `NO-TRACE` / `PARTIAL` / `UNASSESSABLE` / `READING`) and the peak
  selection machinery are untouched and reported separately.**
- ⛔ **The producer's numbers are never an input to the tool's own measurement.** If the tool ever used
  `change_*` to choose a transition the cross-check would be circular.

📌 **Its output says what it validates, in its own printed words:** *agreement validates arithmetic and
transport only — that the numbers in the sidecar are the numbers the delivered images contain. It does not
establish renderer pairing, visible effect, or cause.* That sentence is a contract test, not a comment.

---

## §7 Bench cases — `R7`, discharging `F4`

### 7.1 Why the existing lever cannot be the null

`IAI.Bench.StuckMipNoHold 1` is kept, and kept honest. Its own implementation says what it does
(`Anomaly_StuckLowMip.cpp:515-520`): *"the streaming bias is DELIBERATELY NOT WRITTEN … held reads false
on every frame, observable reads false, and the event carries no labelled frame."* With no labelled frame
there is no mask arm (`AnomalyCaptureSubsystem.cpp:1156`), so no region, so no pair. **Predeclared reading:
zero `"kind":"pair"` rows, and one `"kind":"event"` record with `phase_count: 0` and reason
`no_labelled_frames`.** It tests failure to engage, which is a different thing from engaged-but-invisible.

### 7.2 The matched twin: `null_effect` and `solid_swap`

Two new **bench-only** anomaly ids, implemented as **one class with a constructor flag** —
`FAnomaly_ChangeCase(FName Id, bool bSwapMaterials)` — registered twice:

```
Register(MakeUnique<FAnomaly_ChangeCase>(FName("null_effect"), /*bSwap=*/ false));
Register(MakeUnique<FAnomaly_ChangeCase>(FName("solid_swap"),  /*bSwap=*/ true ));
```

🔑 **One class is the point.** Codex `F4` asks for a null and a positive *through the same path*. Sharing
the class makes the picker, the fire lifecycle, the labelling source and the arming **identical by
construction**; the two arms differ in exactly one boolean. Two independent classes would differ in more
than one variable, and the comparison would be worth less.

| | `null_effect` | `solid_swap` |
|---|---|---|
| picks | a visible static-mesh target via the shared renderable-visible classifier, as `corrupted_texture` does | same |
| enters held | yes — `EAnomalyActiveSource::FireWindow`, so `IsFireLabelledThisFrame` is true for the window | same |
| arms a mask | yes — labelled ⇒ `ArmTargetMaskOwn` tags it | same |
| touches the scene | **nothing at all** — no component state, no material, no flag | `SetMaterial` per slot to `GetCorruptedTextureMaterial()`, exact per-slot revert on end, `m17`'s contract |
| args | `delay=<frames>` (default 0) — hold labelled-but-unchanged for N frames before applying | same |

**Declared scope of the new instrumentation**, per `F4`'s *"declare any new bench-only label/arm
instrumentation and its scope before implementation"*:

- Both ids are added to `ResolveAnomalyActiveSource`'s map (`AnomalyCaptureSubsystem.cpp:281-292`) as
  `FireWindow`, so neither rides the unknown-id default at `:295`.
- ⛔ **Neither is in `GAutoPool`** (7, pinned by `static_assert` to `NumPoolKeys`), **nor in
  `GAutoPoolDefaultEnabled`** (4), **nor in the selector's `GAnomalyChoices`** (5). They cannot reach a
  client dataset by the auto path or by the selector.
- `Apply` **refuses by name and loudly** unless `IAI.Bench.ChangeEvidenceCases 1` — the
  `IAI.Bench.SpawnTranslucentProbe` precedent: it refuses, names itself, and applies nothing.
- 🚨 **The registered catalogue moves 10 → 12**, so `IAI.ListAnomalies` and the dashboard's authored-spec
  list will show them. That is a declared, visible consequence, not a silent one. `GetAuthoredSpec`
  (`AnomalyInjectorSubsystem.cpp:99`) gains two `Object`-scope entries, and `docs/architecture.md`'s
  catalogue table plus `PRE-DELIVERY-CHECKLIST`'s **categorical** catalogue box are updated to read
  *"10 shipping + 2 bench-only, refused unless `IAI.Bench.ChangeEvidenceCases 1`."*

⚠ **One flagged deviation from the brief's wording, with its reason, for the reviewer to overrule.** The
brief says `solid_swap` swaps to *"a flat unlit colour."* This plan uses the **already-shipped Lit**
`M_CorruptedTexture_Pink` instead, for two measured reasons: (1) `G50`/`m29` — an unlit-emissive magenta
**lit the Lumen scene and glowed onto neighbours**, which on a Lumen host inflates exactly the control
this pair exists to read; (2) a new material asset is new plugin `Content`, which forces the full cook
that §0.1 otherwise avoids. The existing material is reached through the public accessor
`GetCorruptedTextureMaterial()` (`AnomalyInjectorSubsystem.h:64`), so nothing new is referenced.

### 7.3 Predeclared readings for the twin — an ORDERING, not a threshold

Each arm runs on **both fixtures** (StackOBot `CB_GateLevel`, settled camera; Lyra, moving camera — the
camera-static and camera-moving cases `R7` asks for) and in **both tick orders**.

| | predeclared reading |
|---|---|
| `null_effect` | on every window pair, `chg_gt8 / chg_n` and `ctl_gt8 / ctl_n` are of the **same order**; both are printed; **no threshold is applied and no pass/fail is computed from a ratio** |
| `solid_swap` | on the onset pair, `chg_gt8 / chg_n` is **strictly greater** than the same leg's `ctl_gt8 / ctl_n`, and strictly greater than `null_effect`'s `chg_gt8 / chg_n` at the same fixture and pose |
| `solid_swap delay=3` | window pairs 0..2 read like `null_effect`; pair 3 reads like `solid_swap`; `ref_gt8` on pair 3 is large — the retained reference and the ordered stage are both exercised end to end |

🔑 **The gate condition is an ordering between two arms of one class, not a number.** That is what `G96`
requires (prove it can fire, both ways) and it is all that is available before §8's calibration exists.
⛔ **If the ordering does not hold, that is a finding to report loudly — the metric is measuring the
renderer, not the anomaly — and it is not a threshold to tune.**

---

## §8 Calibration — DEFERRED TO ITS OWN UNIT — `R8`, discharging `F5` and `Q7`

⛔ **No floor, no `tau` beyond the fixed `> 8` of §1.4, and no class threshold is chosen in `m55` v1, and
none may be chosen from a leg's outcome.** `tau_px = 8` is not a calibrated floor: it is the *existing*
verifier threshold, adopted so the two instruments measure the same quantity, and it is published per row
so the 8-bin histogram can be re-swept against any other value offline.

The calibration unit, when it is briefed, must carry all of the following — Codex's list, not a summary of
it:

1. **Independent positive and negative examples**, not verifier outcomes used as class labels. The 079
   record explicitly retains wrong picks inside `CONSISTENT` runs and has no certified moving-camera
   ground-truth session, so `NO-TRACE` / `CONSISTENT` are corroboration and never truth.
2. **The two bench anomalies of §7** as the controlled null/positive twins, plus **owner-eyeballed bench
   sessions** as independent visibility judgements.
3. **A calibration set and a held-out validation set**, declared before any threshold is fitted.
4. **Sample coverage and false-negative acceptance criteria**, stated in advance.
5. **The control rule**, stated in advance, including what a small or empty control means.
6. 🚨 **`no separation ⇒ no gating`**, as a stop, not a preference.
7. **A valid threshold range enforced.** The maximum per-pixel difference on an anomaly-free moving scene
   can be 255; a threshold above it has no usable range and an order-higher one is impossible. A
   derivation that cannot produce a valid threshold must say so rather than emit one.
8. **Provenance recorded** for every effective value (`A48`), and the derivation written down so another
   title can repeat it even where the number does not travel (`m48`'s standing caveat).

⛔ **And the semantics decision is separate again.** Changing what `observable` means is an owner/client
decision under `docs/OPERATING-CONTRACT.md` §3. Keeping `label_schema` at 2 and the key counts unchanged
does **not** waive it (`F3`, verbatim).

---

## §9 Gates

Every gate runs in **both tick orders** (native and `IAI.Bench.SynthTickOrder`) unless the row says
otherwise, and every pixel-identity reading is taken at the AA-off arbiter (`G228`/`G230`).

### 9.1 Build, schema and inertness

| # | gate | passes when |
|---|---|---|
| `G-0` | both build targets, **including the modular editor target** — the only one that catches a missing `MODULE_API` (`G221`) | exit 0, **zero warnings**, twice |
| `G-SCHEMA` | a leg with change rows and a leg without | `annotation.json` root and per-event key **sets** identical to the A-side by set difference (**added ∅, removed ∅**); `label_schema` `2`; `labels.jsonl` row and anomaly key sets **identical**; `run_summary` gains **exactly** the §4.4 block and nothing else |
| `G-INERT` | `IAI.Capture.ChangeEvidence 0` (the master switch, default **on**; off is the A-side) | no `change_evidence.jsonl` is written, no `run_summary` block is emitted, and the leg is otherwise byte-comparable to the A-side |
| `G-AIDENT` | A-side identity, cross-binary difference set vs a same-binary control pair (`080-06` `G-A`/`G-B` method) | `EXTRAS 0` |
| `G-A44` | `A44` on the staged artifact, both encodings | new symbols present, **pre-existing controls non-zero** (sound, not blind) and invented symbols absent (discriminating) |
| `G-NOCOOK` | container quintet hashed before and after staging | **byte-unchanged**; code-only hot-swap confirmed (`G103`) |

⚠ **`G-SCHEMA` deliberately carries no literal key counts for the per-event set.** This project has
shipped a stale literal count in a delivery gate **three times**, each time one that *"would have FAILED
A CORRECT BUILD"*. The set-difference form is both stronger and immune. The counts measured this session
— `annotation.json` root **4**, `run_summary` root **86** — are recorded here as *readings*, not as gate
constants.

### 9.2 The instrument, both directions

| # | gate | passes when |
|---|---|---|
| `G-NULL` | **CAN-FAIL, load-bearing.** `null_effect`, both fixtures, both orders | the arm produces measured pairs; `chg_gt8/chg_n` and `ctl_gt8/ctl_n` are the same order on every window pair; both printed |
| `G-POS` | `solid_swap`, same fixtures, same poses, same orders | the §7.3 **ordering** holds against both its own control and `G-NULL`'s target fraction |
| `G-NOLABEL` | `IAI.Bench.StuckMipNoHold 1` | zero `"kind":"pair"` rows; one `"kind":"event"` record, `phase_count: 0`, reason `no_labelled_frames` |
| `G-ORACLE` | `--change-oracle` on every leg above | count-vs-count and mean-vs-mean agree on every compared row; coverage printed; **exit codes unchanged on all six CLI cases; selftest and contracts still green; no new verdict** |

`G-NULL` is the gate that decides whether anything else is worth reading. Without it, a quiet reading is
indistinguishable from an instrument that cannot fire (`G96`).

### 9.3 Every row of Codex §4 and §6.6 — a gate, or an explicit UNEXERCISED line

| case | gate | how |
|---|---|---|
| first run frame / insufficient pre-roll | `G-FIRST` | the run's first captured frame reports `first_frame`; no phase is measured from it |
| predecessor already drained | `G-RECEIPT` | the whole receipt design; a leg where every window pair measures proves `PendingSnapshots`' removal at `:4153` is no longer load-bearing |
| out-of-order readbacks / held batches | `G-ORDER` | `IAI.Bench.ChangeForceOutOfOrder 1` reverses the change stage's admission order; every pair must still be measured against its true `N-1`, and `out_of_order_timeout` must be 0 |
| capture skipped before retention | `G-SKIP` | `IAI.Bench.ChangeForceStale 2` retains an index-2-older frame; **every** row reads `chg_measured:false` with `predecessor_missing`, and **no row is silently measured against the wrong frame** |
| readback/encode/write failure after retention | `G-UNDELIVERED` | `IAI.Bench.ChangeForceDeliveryFail 5` fails the writer on every 5th frame; the following pair reads `predecessor_undelivered`, never bridges |
| mask coalescing / lost pixel owner | `G-COALESCE` | a leg with ≥2 simultaneous live fires so the mask pass serves several arms (`:122-131`, `:512-529`); every row either matches by receipt or reads `mask_payload_missing` |
| repeated callback / another view | `G-VIEW` | a leg asserting `view_index == 0` on 100 % of receipts; a synthetic receipt with `view_index 1` must read `view_mismatch` |
| level/world transition, reset | `G-EPOCH` | `IAI.Bench.ChangeForceEpochReset <N>` bumps the epoch at captured frame N; the pair straddling it reads `epoch_reset` and every holder is cleared together |
| **same-size camera cut** | ⛔ **UNEXERCISED** | nothing in the producer distinguishes a teleport from a fast pan, and no lever synthesises one. Reported only via `cam_moved`, which is a caveat and not a refusal (§2.5). Naming what would exercise it: a bench lever that teleports the *camera* — `IAI.Bench.TeleportTargetOffscreenAt` moves the target, not the view. |
| letterbox / rect or projection change | `G-EXTENT` | `IAI.Bench.ChangeForceExtentMismatch 1` perturbs the recorded rect on one frame ⇒ `extent_mismatch`. ⚠ **Synthetic**: `IAI.Bench.Letterbox` **refuses on the packaged bench pawn** (`SpectatorPawn`, no `UCameraComponent`), so the natural route is unavailable at the bench (`G193`) |
| **first capture after a hitch** | ⛔ **UNEXERCISED** | no lever induces a hitch. Both capture times are in the receipt and both are published, so a consumer can see the gap; the producer does not refuse on it. Naming what would exercise it: a stall lever of the `m31`-era render/game-thread shape, which does not exist on `master` |
| missing/late result, final drain | `G-LATE` | a leg force-stopped mid-window; `late_results` non-zero, no finalised record mutates, nothing leaks into the next run |
| **missing onset + later quiet frames** | `G-ONSET-GAP` | `IAI.Bench.ChangeForceStale 1` **for the onset frame only**; the phase must read `indeterminate`, and the later quiet window pairs must **not** move it. **This is `F1`'s gate and it is load-bearing.** |
| **gradual sub-threshold change** | `G-GRAD` (**partial, declared**) | two halves. **(a)** `solid_swap delay=3` proves the retained reference, the window and the ordered stage end to end, with `ref_gt8` large on the delayed pair while adjacent pairs read quiet. **(b)** Codex's exact counterexample — all target pixels walking `0,3,6,9,12` — is proven as a **synthetic fixture in `--selftest`**: four adjacent `chg_gt8` of 0 against `ref_gt8` at the full count. ⛔ **UNEXERCISED in the producer:** no runtime-authorable sub-threshold colour ramp exists without a material parameter, and adding one is a `Content` change that forces the cook §0.1 avoids. |
| short events | `G-SHORT` | a fire shorter than `K` frames; `pairs_required == labelled frames`, the record finalises at phase end, no phase is left `pending` |
| threshold / format ties | `G-TIES` | `--selftest` fixtures at `d == 8` (not counted) and `d == 9` (counted), and a `PF_A2B10G10R10` `0 → 35` fixture reading a byte difference of exactly 8 |
| full-target / empty-control frames | `G-DENOM` | a synthetic mask covering the whole frame ⇒ `ctl_n == 0` ⇒ `empty_region`; and `chg_n == 0` ⇒ the same |
| already-occluded hides | `G-OCCLUDED` | a hide fired on a fully occluded target ⇒ `chg_n == 0` ⇒ `empty_region` ⇒ phase `indeterminate`; **not** a zero-change reading |
| the accepted hide-depth case | `G-DEPTH` | `IAI.Bench.HideOmitDepthPassSilencing`; `drawn == count` on a correctly hidden frame changes **nothing** in `m55`'s output and `observable` is unmoved |
| both tick orders | every gate above | native and `IAI.Bench.SynthTickOrder` |

### 9.4 The `G270` observation gate — `R8`

| # | gate | passes when |
|---|---|---|
| `G-G270` | **OBSERVATION, no expected value.** `080-04`'s banked Lyra `CO8` recipe, `MaxCoAffected 8` | the three ratio-26–31 events produce measured onset pairs, and their `chg_gt8/chg_n`, `ctl_gt8/ctl_n`, `chg_mean` and `chg_hist` are **recorded**. ⛔ **No threshold is applied and no event is forced into any outcome.** §1.7's numbers are the prior; a reading that disagrees with them is a finding to report, not a failure |

### 9.5 Cost — `R9`

| # | gate | passes when |
|---|---|---|
| `G-COST` | **pacing OFF**, `A,B,B,A` after a declared discard (`G186`), at **1280×720 and 1920×1080** | **reported both ways:** game-thread ms per captured frame and per engine frame · change-stage worker ms · end-to-end writer throughput (frames/s, `GetFramesWritten`, `GetDropped`) · queue high-water (`GetPending`) · **byte high-water** (`change_bytes_high_water`) · drops · tails. **Budget: ≤ +1 ms game thread AND writer throughput unchanged within measurement uncertainty.** 🚨 **If the measurement uncertainty itself exceeds the budget the gate reports UNRESOLVED — not pass** (Codex §5, and `G169`: a difference inside the within-build spread is *below the resolution of this instrument*, never *no cost*) |
| `G-PIXELS` | AA-off image identity, retained from v1 | a leg with `IAI.Capture.ChangeEvidence` on and one with it off produce **identical delivered frames** at the AA-off arbiter. `m55` reads pixels and writes none; this is the gate that proves it |

📌 **Why `G-PIXELS` survives the rewrite.** Codex §6.7: *"existing reduce source unchanged is a useful
constraint, but does not prove output or timing unchanged."* Correct — so the constraint is kept **and**
the timing is measured **and** the delivered pixels are compared. Three separate claims, three separate
checks.

### 9.6 The GPU route — parked

⛔ **No RDG design appears in this plan.** A GPU implementation remains available later as a **profiled
optimisation with identical numeric semantics**: it would have to reproduce `DecodeTightPixel`'s
conversion exactly, including the 10-bit truncation and the `> 8` ties, or report the format unsupported.
It is considered only if `G-COST` identifies a worthwhile bottleneck, and it would carry the full cook
(`G129`) that §0.1 currently avoids. Codex's scale arithmetic for reference, **not a benchmark**: at
1080p one pair is ~18.66 MB of input (2 × 8.29 MB colour + 2.07 MB mask), ~0.56 GB/s at 30 captured fps.

---

## §10 File-by-file

**New**

| file | ~lines | what |
|---|---:|---|
| `Source/AnomalyCapture/Private/AnomalyChangeTypes.h` | 70 | `FAnomalyChangeReceipt`, `FAnomalyChangePairResult`, `FAnomalyChangePhaseRecord`, `EAnomalyChangeReason` (the twelve of §4.2) |
| `Source/AnomalyCapture/Private/AnomalyChangeStage.{h,cpp}` | 260 | the single serial ordered worker: admission by `session_index`, the §2.7 bounds, the §1.4 arithmetic incl. the histogram, the §3 phase state machine, refusal accounting |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_ChangeCase.{h,cpp}` | 200 | the §7.2 twin — one class, two registered ids, `delay=<frames>`, `m17`-shape revert on the swap arm |

**Changed**

| file | ~lines | what |
|---|---:|---|
| `AnomalySceneViewExtension.cpp` | +10 | carry `family_frame`, `view_index`, `rect_min` and the submit time into `SubmitInFlight_RenderThread` |
| `AnomalySveCapturer.{h,cpp}` | +25 | widen `FInFlight` and pass the render-side receipt fields through the drain |
| `AnomalyFrameCapturer.h` | +12 | the render-side receipt fields on `FAnomalyCapturedFrame` |
| `AnomalyMaskSceneViewExtension.{h,cpp}` | +14 | record the serving pass's `family_frame` and `view_index` on `FAnomalyMaskResult`. ⛔ **No mask PS, no reduce CS, no table stride, no readback, no existing field** |
| `AnomalyMaskTypes.h` | +4 | those two fields |
| `AnomalyCaptureSubsystem.{h,cpp}` | +180 | run epoch + cut counter + the single reset; the mask shared ref at `:1295` and the two re-pointed call sites; receipt assembly in the drain; hand-off to the change stage; result intake; phase open/close against `Snap->FireLabelled`; finalisation at all five events; counters; cvars with mid-run guards; the `A48` echo; two `ResolveAnomalyActiveSource` entries |
| `AnomalyAsyncWriter.{h,cpp}` | +25 | carry the receipt on `FJob`; publish `{receipt, canonical, delivered}` after the write returns |
| `AnomalyLabelWriter.{h,cpp}` | +90 | `EncodeAndWriteFrame` canonical out-parameter; `WriteChangeEvidence` (the §4.3 sidecar, sorted, once, both delivery modes); the §4.4 `run_summary` block |
| `AnomalyInjectorSubsystem.cpp` | +14 | register the two bench ids; two `GetAuthoredSpec` entries |
| `tools/verify_capture.py` | +180 | `--change-oracle` (§6.2) plus selftest fixtures for `G-GRAD(b)`, `G-TIES` and `G-DENOM`, and a contract pinning that it **cannot** change an exit code |
| `docs/client-readme.md` §8, `docs/client-delivery.md`, `docs/PRE-DELIVERY-CHECKLIST.md`, `docs/architecture.md`, `docs/gotchas.md` | — | the sidecar's field table in the client's own words; the §1.6 *"a low control value is not a causal certificate"* line; the categorical catalogue box at 10 + 2 |

**Untouched by construction:** `IAnomaly.h` · every existing anomaly · the selector, auto-injector and
census · `AnomalyMaskReduce.usf` · `AnomalyVisibleMask.usf` · `AnomalyMaskMeasure.*` (the `m26` veto) ·
`annotation.json`'s shape · `labels.jsonl`'s shape · `label_schema` · the `m49` A1 `observable` predicate
at `AnomalyCaptureSubsystem.cpp:4016`.

**Estimated size:** ~1,000 lines of C++ and ~180 of Python. **No shader. No shader parameter struct. No
new `Content`. No cook.**

---

## §11 Risks

1. 🚨 **The instrument measures the renderer, not the anomaly.** TAA, dither and auto-exposure move pixels
   with no anomaly present, and §1.7's Lyra prior shows a control of the same order as the target on all
   three events. If `G-NULL` and `G-POS` do not separate, the unit's premise is in question and that is
   the finding — not a threshold to tune. This is the risk that makes the whole unit vacuous and it is
   `G-NULL`'s entire job.
2. **Identity is the deep risk and it is now a design, not a check.** §2.1's seven facts are each a way a
   pair could be silently wrong. The answer is receipts, an ordered stage and a closed refusal enum; the
   failure mode if it is wrong is a *confident wrong number*, which is worse than a refusal.
3. **Pre-onset visibility is structurally unavailable** (§5.1). Any later reading that needs it must not
   be built on `m55` v1's output.
4. **Camera cuts and hitches are UNEXERCISED** (§9.3), and both are named rather than assumed away.
5. **Moving targets under-read.** The region is the current frame's mask, so pixels the target vacated
   between `N-1` and `N` are outside it. `cam_moved` and the raw control expose it; nothing corrects it.
6. ⛔ **A new evidence field invites a new veto. It must not become one.** The 077 ruling removed a veto
   built on `target_drawn_pixels` for exactly this reason and `G258` is the standing warning. `change_*`
   is a measurement; it is **never** an input to the `m26` veto and in v1 it is not an input to
   `observable` either.
7. **Two instruments, one number.** §6's cross-check is worth having because the verifier is independent.
   Any future change that lets the producer's numbers steer the tool destroys it silently.
8. **`m51` merge surface** (§2.7): the byte bound and `F1`'s wait-only-for-an-outcome-that-can-exist rule.
9. **The catalogue moves 10 → 12** (§7.2) and two bench ids become visible in `IAI.ListAnomalies` and the
   dashboard. Declared, contained by three list exclusions and a refusing `Apply`, but not invisible.

---

## §12 What this plan does NOT decide

- ⛔ It does not change `observable`, and does not propose when to.
- ⛔ It does not choose a floor, a control cap or a class threshold (§8).
- ⛔ It does not flip `GAutoPoolDefaultEnabled`, raise `MaxCoAffected`, tune the `m52` ratio, or build S2′.
- ⛔ It does not move `annotation.json`, `labels.jsonl` or `label_schema`.
- ⛔ It does not settle `stuck_low_mip`'s default-on question. `G-G270` produces a reading; Codex `Q6` is
  explicit that that reading alone would not settle default-on visibility or readiness.
- ⛔ It does not claim causation, persistence or perceptibility (§0).

**Next checkpoint, as Codex proposed and chat accepted: review of the v1 implementation's identity,
arithmetic, coverage and cost evidence.** Changing `observable` remains a separate owner/client decision
and a separate review checkpoint.

---
---

<details>
<summary><b>⛔ SUPERSEDED 2026-09-21 — the v1 plan at <code>287ad3e</code>, kept verbatim. See
<code>_reviews/081-02-codex-m55-design-review.md</code> and
<code>_reviews/081-02-chat-disposition.md</code>. NOTHING INSIDE THIS FOLD MAY BE CITED: every numbered
section, field name, rule, floor-derivation step, mechanism and gate below is replaced by §§0–12 above.
In particular its §1.3 metric, §3.3 <code>observable</code> rule, §4.3 identity pin, §5 verifier
cross-check, §7.1 GPU mechanism, §8 calibration and §9 gate table are ALL withdrawn.</b></summary>

# m55 — measured change evidence: make `observable` mean measured visible change

**PLAN ONLY. No source, shader, tool, fixture or schema change was made. No build, no cook, no capture,
no leg. Branch `feat/m55-change-evidence` off `master` `031a103`; `m51` `53bf725` untouched and the main
checkout was never moved (this unit was authored in a worktree outside the StackOBot tree, shared-tree
rule 2).**

**This document is written to be reviewed BEFORE implementation and it is not self-approving.** Section
§13 carries the questions a reviewer must answer first; §2.6 carries a correction to the brief's own
control design that a reviewer should rule on explicitly, because building the brief as literally worded
would repeat a refutation this project already has on the record (`G260`).

---

## §0 Why this unit exists

Three independent findings converge on one gap:

- **`G263` (079-07, with its 079-09 dated correction).** The verifier can say *that* pixels changed and,
  with masks, *which* pixels — it cannot say *what caused* the change. It deliberately issues no
  label-correctness verdict. Attribution has to come from the side that knows what it injected.
- **`G270` (m52, 080-04).** The `stuck_low_mip` perceptibility ratio is a *size* test. On Lyra, events at
  ratio **26.76 / 26.76 / 31.59** — three to four times the 8.0 gate — read `NO-TRACE`, because a
  low-frequency paint or normal map is imperceptible at any achievable mip. No predicate computed from
  bounds and mip sizes can see what a texture *contains*.
- **`m49` A1.** For texture-class anomalies `observable` is `labelled && held && target_pixels >= N`
  (`AnomalyCaptureSubsystem.cpp:4016`). Every term is about **presence**: the fire is labelled, the
  anomaly's condition is held, and the target drew front-most pixels. Nothing in it asks whether the
  picture *changed*.

The client's F2 complaint — *labelled, nothing visible* — is only closed when the producer measures, per
frame, how much the target's own pixels changed. `m55` is that measurement.

⛔ **What `m55` cannot do, stated first so it is not claimed later.** `G263`'s correction is not repealed
by moving the measurement in-engine. A mask identifies pixels, not causes; lighting, occlusion and
animation move the same pixels. `m55` makes `observable` mean *the target's own pixels measurably
changed when the anomaly engaged*. It does **not** make `observable` mean *the anomaly caused the
change*, and no wording in the schema, the client docs or a commit message may say that it does.

---

## §1 D1 — what to measure, and out of which buffer

### 1.1 Which colour buffer the capture actually reads (the brief asks this explicitly)

| | hook | `EPostProcessingPass` index | what it sees |
|---|---|---|---|
| mask pass (`m26`/`m34`/`m49b`) | `Tonemap` | 2 | post-tonemap, **pre-FXAA** scene colour |
| colour capture (the delivered PNG) | `VisualizeDepthOfField` | 4 | post-tonemap, **post-FXAA** scene colour |

- `AnomalyMaskSceneViewExtension.cpp:97-101` subscribes the mask to `EPostProcessingPass::Tonemap`.
- `AnomalySceneViewExtension.cpp:72` subscribes the colour capture to
  `EPostProcessingPass::VisualizeDepthOfField`.
- The enum order is `SSRInput, MotionBlur, Tonemap, FXAA, VisualizeDepthOfField, MAX`
  (`Engine/Source/Runtime/Engine/Public/SceneViewExtension.h`).

⇒ **the mask is produced two subscribable passes before the pixels the client receives.** That is not a
defect today — the mask never reads colour — but it decides where `m55` must run (§7).

### 1.2 What m49 reads (the brief asks this too): no colour at all

`target_pixels` and `target_drawn_pixels` are derived entirely from **custom stencil + custom depth +
scene depth** (`AnomalyVisibleMask.usf:31, 39-40, 44, 51`), reduced per tag by
`AnomalyMaskReduce.usf:37-52`. There is no colour sample anywhere in that chain.

⇒ **`m55` would be the first colour-derived label evidence in the producer.** The one existing
colour-derived quantity is `m48`'s exposure dip, and it is a *CPU*, *whole-frame*, *subsampled* mean
luminance taken from the already-read-back delivered bytes
(`AnomalyCaptureSubsystem.cpp:4061-4063`) — neither per-target nor per-pixel. Nothing existing is being
extended; this is a new kind of evidence and it needs its own can-fail gates.

### 1.3 The quantity

For each captured frame `N` carrying a target mask, and for each reserved tag `t` present in that
frame's mask:

```
d(p)          = max over colour channels of | cur(p) - prev(p) |, normalised to [0,1]
change_px(t)  = count of p where mask_N(p) == t and d(p) > tau_px
change_sum(t) = sum over p where mask_N(p) == t of d(p)
change_frac_target(t) = change_px(t) / target_pixels(t)
change_mean_target(t) = change_sum(t) / target_pixels(t)
```

`prev` is the **previous captured frame's delivered colour** — session index `N-1`, pinned by identity,
never by adjacency (§5.3).

The denominator is the **current** frame's `Count[t]` from RT0 — i.e. the same `target_pixels` the row
already publishes, from the same render. It is not recomputed and cannot disagree.

**Why `max abs channel` and not luminance.** `corrupted_texture` swaps a surface to solid magenta; a
green→magenta swap can be near-neutral in luminance while being the most visible change the catalogue
produces. A luma-only metric would under-read exactly the anomaly it most needs to see. Chroma-aware is
the conservative choice and costs one `max` per pixel.

**Why a count *and* a sum.** `change_px` answers *how much of the target changed*; `change_mean` answers
*by how much*. A mip-blur that shifts every pixel by a little and a decal that flips a few pixels a lot
are different events, and `G270`'s Lyra class is precisely the "many pixels, tiny amplitude" corner. One
number could not separate them. `change_mean` is also **the statistic the verifier already prints as
`d=`**, which is what makes §6's cross-check a comparison of like with like rather than of two different
measurements.

**Two thresholds, accumulated in the same pass.** `change_px` is accumulated at **two** predeclared
per-pixel thresholds, `tau_lo` and `tau_hi`, at no extra dispatch. This is deliberate: it lets the class
floors of §9 be derived **offline from banked legs** rather than by re-running the campaign once per
candidate threshold, and it makes the metric's sensitivity visible in the artifact instead of buried in a
compiled constant.

### 1.4 The control, and the whole-frame confound

The same dispatch accumulates, at a reserved slot and over **every** pixel including untagged ones:
`changed_all_lo`, `changed_all_hi`, `sum_all`. With `W*H` known on the CPU:

```
change_frac_control = (changed_all_lo - change_px(t)) / (W*H - target_pixels(t))
```

— the frame outside the target, on the same frame pair, from the same render.

### 1.5 The whole-frame control is a REFUSAL input, not a subtrahend — a correction to the brief

The brief asks for `change_frac_target - change_frac_control` to be *"what the label carries"*.
**That specific step should not be built, and the reason is on this project's own record.**

`G260` (079-02/03) built exactly that subtraction, gated it, and had it **refuted by its own gates in
both directions at once**:

- **It does not cancel camera motion.** Motion is parallax- and content-weighted: a region of near,
  detailed geometry changes far more under a given camera move than flat distant wall. The difference
  carries a large content-dependent variance instead of cancelling.
- **It destroys real signal on a still camera, which is worse.** On a pinned bench leg the onset read
  `d_region = 0.02286` against `d_ring = 0.02872`, so the subtracted quantity went **negative at the
  exact frame the anomaly started**.
- **Its premise is false for these anomalies.** The effect leaves the region: `m26`'s `A35`/ruling `A-4`
  measured hiding `SM_Ramp2` changing **more outside its own bbox than inside** (peak-OUT `0.2955`
  against peak-IN `0.1785`). `G258` is the same family.

Neither failure is a property of *where* the measurement is taken. Both are properties of the **scene**.
Moving the measurement in-engine fixes the attribution of *which pixels* (which is real and valuable) and
fixes nothing about parallax weighting or about bounce light landing outside the silhouette. The brief's
justification — *same frame pair, same render, no attribution claim* — removes a pairing confound that
`G260` was never about.

⇒ **`m55` publishes `change_frac_target` and `change_frac_control` as two separate measured fields and
subtracts neither.** `G260`'s own stated rule is followed instead: *where the surroundings cannot be a
control, MEASURE the confound and REFUSE above a threshold.* The control becomes a refusal input —
above a predeclared `change_frac_control` cap the frame's change evidence is `unmeasured`, which
**admits** (§4).

📌 **This costs the reviewer nothing.** Both fields ship, so a subtractive rule remains exactly one
arithmetic step away in any downstream consumer if chat rules for it — without a code change, without a
re-capture, and with the banked evidence to test it on. What is refused here is *baking the subtraction
into the label*, which is the irreversible half.

⛔ A **spatial ring** (dilated bbox minus bbox) was also considered and is rejected for the same reason
plus one more: the reduce learns the tag's bounds *in the same pass* that would need them, so a ring
needs either a second dispatch or a one-frame-stale bbox. It buys nothing `G260` has not already refuted.

---

## §2 D2 — what change evidence means per anomaly class

The catalogue is **10** (`GAutoPool` is 7, default-enabled 4 —
`AnomalyAutoInjectorSubsystem.cpp:22-30`). The active-source taxonomy the capture already uses is at
`AnomalyCaptureSubsystem.cpp:281-292`.

| anomaly | active source | in default pool | has a target mask? | change evidence | `observable` under m55 |
|---|---|---|---|---|---|
| `blinking` | ActorHidden | yes | yes (silhouette survives; `m45` keeps custom depth) | onset transition **and** `target_drawn_pixels == 0` thereafter | m49 rule ∧ onset evidence |
| `missing_object` | ActorHidden | no (in pool) | yes | same | m49 rule ∧ onset evidence |
| `missing_texture` | FireWindow | yes | yes | onset transition over the silhouette | m49 rule ∧ onset evidence |
| `corrupted_texture` | FireWindow | yes | yes | onset transition over the silhouette | m49 rule ∧ onset evidence |
| `stuck_low_mip` | AnomalyState (deferred onset) | no | yes | unheld→held transition at the `080-06` window start | m49 rule ∧ onset evidence |
| `lod_popping` | AnomalyState | yes | yes | **repeated** transitions, one per toggle | m49 rule ∧ onset evidence |
| `lod_corruption` | FireWindow | no (not in pool) | yes | one-shot forced LOD ⇒ onset transition only | m49 rule ∧ onset evidence |
| `lighting_mismatch` | FireWindow | no (not in pool) | **no** — see §2.3 | whole-frame reading only | **`null`, unchanged** |
| `camera_clipping` | AnomalyState | yes | **no** — global, no target | whole-frame reading only | **`null`, unchanged** |
| `time_dilation` | FireWindow | no (hidden) | **no** — global | whole-frame reading only | **`null`, unchanged** |

### 2.1 Hide class (`blinking`, `missing_object`)

`m45` hides by dropping the main and depth passes **while keeping `bRenderCustomDepth`**, so a hidden
target still has an RT0 silhouette and a denominator. The onset frame pair is (last visible) → (first
hidden) and the change is large over exactly those pixels.

Within the hold there is nothing to compare — two consecutive hidden frames are identical. The brief's
own formulation is adopted: **onset-frame change, plus `target_drawn_pixels == 0` thereafter**, and the
second half already exists (`m49` Phase B). `G258` governs the asymmetry: `drawn == 0` is strong evidence
of absence; `drawn > 0` establishes nothing. `m55` does not change that and must not be read as
reinstating the veto the 077 ruling removed.

### 2.2 Texture class (`corrupted_texture`, `missing_texture`, `stuck_low_mip`, and a future UV-corruption `m54`)

The silhouette persists and is fully drawn, so the denominator is large and stable; the change is a
**content** change inside it. This is the class `G270` is about and the one `m55` exists to close: a
swap that changes nothing visible reads `change_frac_target ≈ change_frac_control ≈ 0` and is no longer
called observable on size alone.

`stuck_low_mip` is the one with a **deferred onset**, and `080-06` already put the labelled window at the
first measurably-held frame. That is a gift: the frame pair straddling the window start is exactly
unheld→held, so the transition is *in* the measured pair by construction rather than by luck.
⚠ **Risk, named: the streamer's mip arrival can be gradual**, spreading the transition over several
frames and leaving no single pair above a floor. §4.3's first-K latch is the mitigation and §10's `G-5`
is the gate that would expose it.

### 2.3 Light class (`lighting_mismatch`, and a future `m53`) — what the mask pass can and cannot give

**It can give nothing, and this is structural rather than a gap to be filled by tuning.**

`lighting_mismatch` matches `ULightComponent` by substring across the world
(`Anomaly_LightingMismatch.cpp:48`). The renderable-visible set the selector and the mask both work from
is static-or-skinned meshes only (`G33`), and the stencil tag is written onto
`UPrimitiveComponent`s. A light actor carries no primitive component, so `AnomalyStencilTag::TagActor`
tags nothing, `ArmTargetMaskOwn` finds no visible taggable actor
(`AnomalyCaptureSubsystem.cpp:1185-1197`) and the frame lands `Unmeasured`.

⇒ **`lighting_mismatch` already ships `target_pixels: -1` and `observable: null` today.** `m55` changes
nothing for it and must not pretend to. The brief's suggested regions — the attenuation sphere projected
to a screen bbox, or the census of lit primitives — are both real options, and both are a **new region
source**, not a new measurement: the first re-introduces a bounding box, which `G263` says carries no
verdict; the second needs a per-frame set of lit primitives that nothing computes today.

📌 **Filed, not designed, and deliberately out of `m55`'s scope.** It is also not urgent: `lighting_mismatch`
is not in `GAutoPool`, so it cannot reach a client dataset by the auto path. A future `m53` that adds a
flicker/lighting anomaly **must bring its own region source with it**, and that is the finding to carry
forward rather than a blocked item to re-discover.

### 2.4 Global class (`camera_clipping`, `time_dilation`)

No target, no tag, no mask; `bWholeFrameExtent` and `bbox_norm 0,0,1,1`. The whole-frame change fraction
is the only available statistic and **it has no control** — there is nothing outside the region. It ships
as a `run`-level reading. `observable` stays `null`, which is what it reads today, and the admit bias is
preserved by doing nothing.

### 2.5 LOD class

`lod_popping` is the only catalogue member with **repeated in-window transitions** (one per toggle), so
it is the one class where a per-frame change series is informative for its whole span rather than only at
its edges. It is therefore the best natural positive control for the per-frame field, and §10 uses it
that way. ⚠ At the compiled `LodMaxDistance` default of 200 cm this bench produces **zero**
`lod_popping` events (`m30`, and `080-06`'s `G-B` had to pass `IAI.Anomaly.LodMaxDistance 50000` to fire
at all) — so any `lod_popping` leg must declare that lever **in the predictions, before the leg**, or it
is vacuous (`G146`).

---

## §3 D3 — how `observable` changes

### 3.1 The finding that shapes the whole rule: a difference measures transitions, not states

`observable` is a **per-frame** field. A temporal difference between consecutive frames is a **transition**
measurement. For every steady-state class in the catalogue — which is all of them except `lod_popping` —
consecutive frames *inside* the hold are identical again once the anomaly has engaged, so
`change_frac_target` returns to ≈ 0 on every held frame after the first.

⇒ **`observable = current rule AND (this frame's change ≥ floor)` would mark the onset frame observable
and every subsequent held frame NOT observable.** It would empty `affected_frames` down to a single
frame for every texture- and hide-class event in the dataset. That is catastrophic and it is not what
the brief intends; the brief half-states it for hide class (*"after onset there is nothing to compare"*)
and the same is true for every other class.

**Therefore change evidence is an EVENT-level property, evaluated at the event's transitions, and
consumed per frame.** The per-frame `change_*` fields ship as measurements and gate nothing on their own.

### 3.2 No lookahead is available, and none is needed

`labels.jsonl` rows are emitted progressively from the drain
(`AnomalyCaptureSubsystem.cpp:3997-4048` computes `observable`, and the row is built and handed to the
async writer in the same pass). A row cannot be retro-edited, and `L3` already records that the veto does
not retro-edit it either. So a per-frame `observable` may not depend on a *later* frame's measurement.

It does not have to. **The onset is the first labelled frame, and the onset's change is measured on that
very frame** — the pair is (previous captured frame) → (this frame), and the previous frame is by
definition already past. So the evidence is available at the exact moment the first labelled row is
built. It is **latched onto the event's record** and every subsequent frame of that event reads the latch.

### 3.3 The proposed rule

```
per frame i of event e:

  change_measured(i)  = prev frame resolved && identity pinned && control below cap && denominator > 0
  change_evidence(e)  = PRESENT   once any of the first K labelled frames of e read
                                  change_frac_target >= floor(class)
                      = UNMEASURED if none of those frames was change_measured
                      = ABSENT     otherwise

  observable(i) = (m49 A1 rule)  AND  change_evidence(e) != ABSENT
```

with, unchanged from `m49` A1: `target_pixels == -1` ⇒ `observable` is `null` and the rule is not
reached at all.

Three properties, each deliberate:

1. **`UNMEASURED` admits.** Where the change could not be measured — no previous frame, a dropped frame,
   a resolution change, a control above the cap — the event keeps `m49`'s answer. This is `m26`'s admit
   bias and the 077 ruling in the same direction: *deleting a true positive is dataset loss*.
2. **The first `K` labelled frames are provisionally admitted.** Until the latch resolves, frames read
   the `m49` rule. `K` is a small predeclared constant (proposed **4**, matching the veto's per-event arm
   budget). This is what protects `stuck_low_mip`'s gradual hold and any class whose transition is
   spread. From frame `K+1` on, an event that never showed a change reads `observable: false`.
3. **`ABSENT` is the only state that removes anything**, and it removes an event's *observable frames*,
   not the event: an event with an empty observable set **stays in the file** (`m49` ruling, unchanged),
   ships `affected_frames` empty, and is visible to the client as such.

⚠ **The direction of the risk, stated rather than discovered.** `ABSENT` on a real anomaly is a true
positive losing its positive frames — the dangerous direction. Every default above is set so that the
only way to reach `ABSENT` is a *successful* measurement that found no change. That is why the floors of
§9 are derived conservatively (low) rather than tuned to reject the most events.

### 3.4 Schema

**Additive only. `annotation.json`'s root and per-event key sets DO NOT MOVE — `P6` is intact —
and `label_schema` stays `2`** (`m51` owns the bump to 3; two units must not both move it).

Per-row, inside an anomaly entry (the `stuck_mip.*` precedent: emitted only where meaningful):

| key | type | meaning |
|---|---|---|
| `change_measured` | bool | a sound previous-frame pair existed for this row |
| `change_px` | int | target-mask pixels differing by more than `tau_lo`; `-1` = unmeasured |
| `change_frac_target` | float \| null | `change_px / target_pixels` |
| `change_mean_target` | float \| null | mean per-pixel difference over the target's mask pixels |
| `change_frac_control` | float \| null | the same fraction over the frame outside the target |
| `change_px_hi` | int | the same count at `tau_hi`; `-1` = unmeasured |

Per-event (inside `labels.jsonl`'s event entry, **not** `annotation.json`):

| key | type | meaning |
|---|---|---|
| `change_evidence` | `"present"` \| `"absent"` \| `"unmeasured"` | the latched verdict |
| `change_frac_onset` | float \| null | the value at the first labelled frame |
| `change_frac_max` | float \| null | max over the event's labelled frames |
| `change_frac_median` | float \| null | median over the event's labelled frames |

`run_summary` (currently **86** keys) gains a small block: `change_frames_measured`,
`change_frames_unmeasured`, `change_events_present`, `change_events_absent`,
`change_events_unmeasured`, `change_refused_control`, `change_refused_identity`, `change_tau_lo`,
`change_tau_hi`, and the effective per-class floors with their provenance (`A48` — echo the **effective**
value and **where it came from**, never the value requested).

---

## §4 D4 — cost, and every way this can be wrong

### 4.1 A FULL COOK IS REQUIRED, and it is the largest single cost in this unit

Any GPU option here adds or changes a global shader parameter struct, and `G129` is unambiguous: a new
global shader cannot ride the code-only hot-swap, and a parameter-struct change is fatal against a stale
container (`m46`'s cook exists for exactly this). **This is true of the extend-the-existing-reduce option
as well as of the new-pass option — there is no GPU path that avoids it.**

Consequences to sequence deliberately, not discover:

- The container quintet (`67EA1FE0`/`2CEFB8F4`/`E03C6610` + `A16A18A8`/`C70ECDAA`) has been
  **byte-unchanged since `m49` Phase B**. A cook retires it, and with it every binary↔container pairing
  in `_binary_baselines\README.md`.
- `G121`: build identity is **exe hash + container identity**. Both halves move.
- `G92`: the archive step can wipe `Saved`. Bank first.
- `G119`/`G118`: read the cooked map set and the enforced config **back out of the artifact** afterwards.
- The cook must be its own sequenced operation, **never inside a measurement turn**.

⇒ **a reviewer may reasonably decide the cook is not affordable now.** §7.4's CPU fallback is costed for
exactly that case and is a real option, not a strawman.

### 4.2 GPU cost and memory

One additional compute dispatch per captured frame with a mask, `[numthreads(8,8,1)]` over the output
view rect — the same shape and domain as the existing `FAnomalyMaskReduceCS`, which the pacing gates have
never been able to resolve above noise. Per pixel: two texture loads, one `max`-of-channels, two
groupshared `InterlockedAdd`s on changed pixels plus one for the sum. Readback: **4 KB**
(256 tags × 4 uints), against the existing reduce's 6,144 B.

Retained memory: **one** frame of delivered colour at the view rect —
1280×720×4 B = **3.69 MB**, 1920×1080×4 B = **8.29 MB**. One texture, plugin-owned, released at
`FinishRun` and at teardown.

The declared budget is `<= +1 ms at 1080p`, measured as §10's `G-8` specifies — **pacing OFF**, because a
paced leg structurally cannot measure a sub-budget hook (`m35` finding, `G186`'s `A,B,B,A` order after a
declared discard). `G169` governs the reading: a difference not larger than the within-build spread is
**below the resolution of this instrument**, never *no cost*.

### 4.3 Identity: the previous colour must be the previous CAPTURED frame

This is the failure this project has met four times (`m21`, `m31`, `m44`, and `080-06`'s arm window) and
it is the one to design against first.

- The colour capture arms with `RequestId = ++CaptureRequestSerial` and stamps
  `Snap.SessionIndex = SessionFrameIndex` (`AnomalyCaptureSubsystem.cpp:4369-4372`), and the mask arm for
  the same frame is bound at the same tick by
  `TargetMaskArmedTick = GFrameCounter; TargetMaskArmedSessionIndex = Snap.SessionIndex` (`:4397-4398`),
  consumed at tick-end under `TargetMaskArmedTick == GFrameCounter` (`:891`). **Colour and mask for one
  session index are the same rendered frame**, and the mask result is already keyed to a session index
  through `TargetMaskPendingSessionIndex` (`:1249`).
- ⚠ **The mask pass does not run on every rendered frame** — the SVE is active only while arms are
  pending (`AnomalyMaskSceneViewExtension.cpp:49-53`). Retention must therefore be driven by the
  **captured-frame** cadence, not the arm cadence. The colour SVE's callback runs exactly on captured
  frames (`AnomalySceneViewExtension.cpp:107-110`, gated on `Entry.bWanted`), which is why §7 puts the
  retention there.
- **The rule: pin, do not tolerate.** The retained frame carries its `RequestId`; the subsystem resolves
  both request ids to session indices through `PendingSnapshots` and requires exactly
  `SessionIndex(prev) == SessionIndex(cur) - 1`. Anything else ⇒ `change_measured: false`. Frames *are*
  dropped in the wild — the extent clamp drops one (`AnomalySceneViewExtension.cpp:122-134`) and a
  key-ring miss drops one (`:96-105`) — and after a drop the indices skip and this check correctly
  refuses. **Refuse, never guess**, which is the key ring's own stated discipline.
- Resolution or letterbox change mid-run ⇒ retained extent differs ⇒ refuse and re-seed.
- `IAI.Capture.OutputHeight != 0` ⇒ `m43` already refuses the target mask entirely, so `m55` inherits the
  refusal and never has to reason about a resampled write.

### 4.4 The `m51` interaction, stated because it is a cross-branch fact

**`6713f51` is not an ancestor of `master`** (verified: `git merge-base --is-ancestor` exits 1), and
`bAwaitingTargetMask` / `TargetMaskWaitTicks` do not exist in `master`'s
`AnomalyCaptureSubsystem.cpp`. ⇒ **`m51`'s `F1` held-frame fix is NOT on `master`**, and `m55` built here
does not interact with it *as code*.

It does interact with it as a **design**, twice, and both need reconciling at whatever merge brings the
two together:

1. `m55` adds a **second readback per captured frame**. `m51`'s `R7` writer-byte-bound work is about
   exactly that budget. 4 KB per frame is small, but it is a new consumer of a bound that is still open
   on that branch.
2. `F1`'s subject is *waiting for a mask outcome that can exist*. `m55` adds a second outcome
   (the change table) that a frame can be waiting on. The same rule must hold for it: **wait only for an
   outcome that can exist**, and commit honestly unmeasured otherwise.

⛔ Neither is a blocker for `m55` on `master`. Both are recorded so the merge cannot silently orphan them.

### 4.5 Every route to `change_measured: false`

no previous captured frame (first frame of the run) · previous frame dropped or its session index is not
`N-1` · retained extent differs from this frame's · no target mask this frame (`target_pixels == -1`) ·
`Count[t] == 0` ⇒ no denominator · `change_frac_control` above its cap · the change readback was not
ready at the final bounded drain. **Every one of them admits** (§3.3 property 1).

### 4.6 Two measurement limitations, declared in advance

- **A moving target under-reads.** The change is accumulated over the **current** frame's mask, so pixels
  the target vacated between `N-1` and `N` are outside the region and are not counted. On the settled
  bench this is inert; on a moving-camera host it is real. `change_frac_control` and the existing
  whole-frame motion reading are what expose it, and the control cap is what refuses it.
- **The delivered picture is measured, so the delivered picture's own noise is measured too.** Temporal
  AA, dither and auto-exposure all move pixels with no anomaly present. That is precisely why the
  measurement ships with a paired control and why `tau_px` is derived rather than chosen (§9), and why
  §10's `G-1` (a null-effect anomaly must read at the control) is the load-bearing can-fail gate.

---

## §5 D5 — verifier interplay

The 079 tool stays **independent**: it differences delivered PNGs on disk over the delivered masks, and
`m55` changes nothing it reads. Its outcome vocabulary (`CONSISTENT` / `OFFSET-NOTE` / `NO-TRACE` /
`PARTIAL` / `UNASSESSABLE` / `READING`, `tools/verify_capture.py:479-483`) does not change and **no new
verdict is added**.

It gains **one reading line per event**:

```
CHANGE-EVIDENCE event=<id> producer onset=<change_frac_onset> mean=<change_mean> evidence=<present|absent|unmeasured>
                tool d=<measured d at the selected onset transition> tau=<tau>  AGREE | DIVERGE(<delta>)
```

Rules, each a consequence of something already ruled:

- Agreement **strengthens** a `CONSISTENT` line and is reported as such — it is two instruments, on two
  sides of the pipeline, measuring the same statistic (`change_mean_target` and the tool's `d` are the
  same quantity, §1.3) on the same buffers.
- Divergence is an **`OFFSET-NOTE`-class reading for a human**, never a failure and never a change of
  exit code. `NO-TRACE` remains the only failing outcome and its definition is untouched.
- ⛔ **The producer's numbers are never an input to the tool's own measurement.** 079 already states that
  *"M3 observable flags are producer evidence and are not independently verified by this tool"*; the same
  applies here. If the tool ever used `change_*` to choose a transition, the cross-check would become
  circular and the tool would stop being a second opinion.
- ⛔ And per `G263`'s correction, **agreement does not establish cause.** Two instruments agreeing that
  the target's pixels changed at frame `k` is still not evidence that the injection is what changed them.

📌 **The most valuable single use of this line is on the `G270` Lyra events.** Those read `NO-TRACE` from
the tool at ratio 26–31. If the producer independently reads `change_frac_onset ≈ 0` on the same events,
`m55` has closed the gap the ratio could not see — measured from the inside, on the exact class that
motivated the unit. That is §10's `G-3`.

---

## §6 Field list — one table

*(consolidated in §3.4; nothing here is in `annotation.json` and `P6` does not move.)*

---

## §7 The mechanism, and what was rejected

### 7.1 Chosen: a new change-reduce pass at the colour hook, consuming the mask published at the Tonemap hook

```
Tonemap  callback (mask SVE, existing):
    ... existing mask PS + reduce CS, byte-unchanged ...
    publish { ConvertToExternalTexture(MaskRT), View.Family->FrameNumber } into a shared holder

VisualizeDepthOfField callback (colour SVE, existing, runs only on captured frames):
    OwnTexture = existing W*H sub-rect copy at (0,0)          <- already built, AnomalySceneViewExtension.cpp:139-145
    if holder.FrameNumber == View.Family->FrameNumber
       and Prev.PooledRT valid and Prev.Extent == this extent:
        Mask = RegisterExternalTexture(holder.MaskRT)
        Prev = RegisterExternalTexture(Prev.PooledRT)
        dispatch FAnomalyChangeReduceCS(Mask, OwnTexture, Prev) -> 256x4 uint table
        AddEnqueueCopyPass(table readback)
    Prev = { ConvertToExternalTexture(OwnTexture), Entry.RequestId, extent }   <- retention, no extra copy
```

Why this shape:

- **It measures the delivered pixels.** `OwnTexture` *is* what becomes the PNG. Measuring post-tonemap
  pre-FXAA instead would measure a picture the client never receives, and would make §5's cross-check a
  comparison of two different images.
- **Retention is free.** `OwnTexture` already exists for the readback; extracting it costs one refcount,
  not a copy.
- **The cadence is already exactly right.** That callback runs on captured frames and no others
  (`Entry.bWanted`), so no SVE's activation pattern changes and nothing in the heavily-gated mask arm
  accounting is touched.
- **Cross-frame state is pooled-RT based** (`TRefCountPtr<IPooledRenderTarget>`, refcounted and
  frame-independent) — never a raw `FRDGTextureRef`, which is allocated from the graph's own allocator
  and would dangle across frames. The within-frame mask handoff is guarded by
  `View.Family->FrameNumber` equality — identity, not adjacency, the same discipline as the key ring.
- **The existing reduce is not touched at all**, so `MASK-TIE`, `TARGET-PIXELS TIE`, `target_pixels`,
  `target_drawn_pixels`, `bbox_drawn_px`, ONSET and pairing are unchanged **by construction** rather than
  by measurement. That is the `m49` Phase B lesson (*"indices 0..4 DID NOT MOVE"*) taken one step
  further.
- The new shader must **not** skip `V == 0` the way `AnomalyMaskReduce.usf:38` does — the whole-frame
  control needs the untagged pixels. That structural difference is a second reason not to fold it into
  the existing shader.

`ConvertToExternalTexture` (`RenderGraphBuilder.h:273`) is the correct call and returns the pooled target
immediately at construction time; `QueueTextureExtraction` (`:259`) completes only after execution and is
too late for same-frame use. `TryRegisterExternalTexture` (`RenderGraphUtils.h:263`) returns null rather
than asserting, which is what the first-frame case needs.

### 7.2 Rejected — extend the existing `FAnomalyMaskReduceCS` at the Tonemap hook

Brief's option (a). Rejected on three counts: it measures **pre-FXAA**, i.e. not the delivered picture;
retention at that hook requires the mask SVE to be **active on every captured frame**, which changes when
the post-process chain designates that callback its final writer (`overrideOutput`) and is therefore a
pixel-affecting change needing the `m45` identity arbiter to clear it; and it would have to abandon the
`V != 0` skip that the existing shader's structure depends on. It saves one dispatch and costs a
correctness margin. **It does not save the cook.**

### 7.3 Rejected — `FViewInfo::PrevViewInfo` / the TAA history as "the previous frame"

The TAA history is pre-tonemap, in history space, reprojected, and does not exist at all with AA off —
which is the arbiter configuration this project measures pixels in. It is not the previous delivered
frame under any configuration.

### 7.4 Costed fallback, not rejected — CPU differencing of the two delivered crops

`m48` already reads colour on the CPU from `Frame.RawBytes` in the drain
(`AnomalyCaptureSubsystem.cpp:4061-4063`), and the per-frame mask **is** on the CPU as well
(`Result.MaskPixels`). So a CPU implementation is genuinely available and it is the only option that
**needs no cook**.

Its costs, stated fairly: a ~1 MP scan per captured frame on the **game thread** inside the drain, which
is the shape of the render-thread CPU scan `m34` removed; retention of the previous frame's full
`RawBytes` in system memory, competing with the writer's byte budget (`m51` `R7`); and no access to
anything the GPU pass gets for free. Against that: no shader, no parameter struct, no container churn,
and every binary↔container pairing in `_binary_baselines` survives.

⇒ **If a reviewer rules the cook unaffordable this cycle, this is the build, with the same fields, the
same per-class rules, the same floors and the same gates.** The measurement's semantics do not depend on
where it runs. §13 Q2 asks for that ruling explicitly.

---

## §8 Floor derivation — the procedure, not the numbers

⛔ **No floor and no `tau` is chosen in this document, and none may be chosen from a leg's outcome.**

1. `tau_lo` / `tau_hi` come from the **noise floor of a NEGATIVE frame pair**: consecutive captured frames
   from a leg with **no anomaly firing**, on both fixtures, at the delivered configuration and at the
   AA-off arbiter. `tau_lo` sits above the highest per-pixel difference those pairs produce; `tau_hi` is
   set an order above it. Two-sided, the `m48` method: every quiet pair below it, every known-anomalous
   pair above it, with both margins reported.
2. The **class floors** come from banked legs whose visibility is already known from the outside:
   the six pinned bench legs and the three masked Lyra sessions the verifier scores, plus `080-04`'s
   Lyra `CO8` leg — the one carrying the `G270` events at ratio 26–31 that read `NO-TRACE`.
   A floor is admissible only if it separates *known-visible* from *known-invisible* on that set **with
   the band between them reported**, and it is set on the **conservative side** of the band, because
   `ABSENT` is the dataset-losing direction (§3.3).
3. **The known answer must be injected, not read from the artifact under test.** `G260`'s corollary: an
   onset read from the delivered masks is circular, because `m49` A1 *defines* `affected_frames` as the
   observable subset. The anchors here are the verifier's independent `NO-TRACE`/`CONSISTENT` readings
   and the client-side visibility judgements, not the producer's own labels.
4. The two-threshold accumulation of §1.3 means steps 1–2 are **offline sweeps over banked legs**, not a
   re-run per candidate value.
5. ⚠ **Derivation travels; the number may not** (`m48`'s standing caveat). The floors are compiled
   defaults with console overrides and an `A48` provenance echo, and the derivation is written down so
   another title can repeat it.

**All floors and `tau` values are predeclared in an amendment to this file BEFORE the implementation
gate, and are not touched afterwards on the strength of a reading.**

---

## §9 Gates

Every gate runs in **both tick orders** (native and `IAI.Bench.SynthTickOrder`) unless noted, and every
pixel-identity reading is taken at the AA-off arbiter (`G228`/`G230`).

| # | gate | passes when |
|---|---|---|
| `G-0` | both build targets, incl. the **modular editor** target (the only one that catches a missing `MODULE_API`) | exit 0, **zero warnings**, twice |
| `G-1` | **CAN-FAIL, load-bearing.** A control anomaly with a null visual effect — `IAI.Bench.StuckMipNoHold 1`, whose `held:false` on 51/51 is already proven — fires with the mask armed | `change_frac_target` reads **at the control**, not above it; `change_evidence: absent`; and the same leg with the lever off reads `present` |
| `G-2` | texture class on **both fixtures**, shipped configuration | `corrupted_texture` and `missing_texture` read `change_evidence: present` with `change_frac_onset` well clear of the floor; `observable` sets unchanged from the A-side |
| `G-3` | **the `G270` class.** `080-04`'s banked Lyra `CO8` leg recipe, `MaxCoAffected 8` | the three ratio-26–31 events read **low** `change_frac_onset` and `change_evidence: absent`, agreeing with the verifier's independent `NO-TRACE` on the same events |
| `G-4` | hide class | onset frame reads a large `change_frac_target`; every later held frame reads `target_drawn_pixels == 0`; `observable` unchanged from `m49` |
| `G-5` | `stuck_low_mip` deferred onset | the first held frame (the `080-06` window start) is `change_measured` and its pair straddles unheld→held; if the transition is spread, the first-`K` latch resolves `present` within `K` |
| `G-6` | `lod_popping` per-frame series (declare `IAI.Anomaly.LodMaxDistance 50000` **in advance**) | repeated in-window transitions appear in the per-frame `change_frac_target` series at the toggle cadence |
| `G-7` | **identity.** `IAI.Bench.ChangeForceStale 1` forces the retained frame to be one index older | `change_measured:false` on every row, `change_refused_identity` non-zero, **no row silently measured against the wrong frame** |
| `G-8` | **cost.** Pacing **OFF**, `A,B,B,A` after a declared discard, 1280×720 and 1920×1080 | ms per captured frame and per engine frame reported both ways; `<= +1 ms` at 1080p, or reported as **below the resolution of this instrument** (`G169`) if inside the within-build spread |
| `G-9` | **schema / `P6`.** A leg with no change-bearing event and one with | `annotation.json` root **4 → 4** and per-event **16 → 16**; `label_schema` **2**; `labels.jsonl` row keys unchanged; anomaly keys grow by exactly the declared `change_*` set and only inside an eligible entry; `run_summary` **86 → 86 + exactly the declared block** |
| `G-10` | **A-side identity.** Cross-binary difference set vs a same-binary control pair (`080-06`'s `G-A`/`G-B` method) | `EXTRAS 0` |
| `G-11` | **light and global classes are untouched** | `lighting_mismatch`, `camera_clipping`, `time_dilation` read `observable: null` exactly as on the A-side |
| `G-12` | verifier interplay | the `CHANGE-EVIDENCE` line prints on every event; exit codes unchanged on all six CLI cases; selftest and contracts still green; **no new verdict** |
| `G-13` | `A44` on the staged artifact, both encodings | new symbols present, **pre-existing controls non-zero** (sound, not blind) and invented symbols absent (discriminating) |
| `G-14` | post-cook | `verify_cooked_maps.ps1` passes; the cook log names `FAnomalyChangeReduceCS`; **and the build boots** (`G129`/`m46`: the boot is the evidence, not a string scan) |

`G-1` is the one that decides whether anything else is worth reading: without it, a `change_evidence:
absent` anywhere is indistinguishable from an instrument that cannot fire (`G96`).

---

## §10 File-by-file

**New**

- `Shaders/Private/AnomalyChangeReduce.usf` — the change-reduce CS. Loads mask + current + previous,
  accumulates per value (**including 0**) `{ChangeLo, ChangeHi, SumAbsDiff255}` into a groupshared table,
  one global atomic merge per group per present value. Overflow checked: 1920×1080 × 255 = 5.29e8,
  inside `uint32`.
- `Source/AnomalyShaders/{Public,Private}/AnomalyChangeReduceShader.{h,cpp}` — `FAnomalyChangeReduceCS`
  declaration and `IMPLEMENT_GLOBAL_SHADER`, alongside the two existing ones. ⚠ The module is
  `PostConfigInit` for the reason `G131` records; nothing about that changes.
- `Source/AnomalyCapture/Private/AnomalyChangeTypes.h` — `FAnomalyChangeTagResult`,
  `FAnomalyChangeResult` (mirroring `AnomalyMaskTypes.h`), and the `EAnomalyChangeEvidence` tri-state.

**Changed**

- `AnomalyMaskSceneViewExtension.{h,cpp}` — publish `{pooled MaskRT, family frame number}` into a shared
  holder at the end of the Tonemap callback. **No change to the mask PS, the reduce CS, the table
  stride, the readback, or `FAnomalyMaskResult`.**
- `AnomalySceneViewExtension.cpp` — the block of §7.1 inside the existing `AfterPass_RenderThread`, after
  `OwnTexture` is built and its readback enqueued.
- `AnomalySveCapturer.{h,cpp}` — carry the change-table readback beside the colour readback in `FInFlight`
  and surface the decoded table through the existing drain, keyed by the same `RequestId`. This is where
  the retained-frame holder lives and where it is released on `Reset()`.
- `AnomalyCaptureSubsystem.{h,cpp}` — join the change table to the frame's mask outcome at the session
  index; the identity check of §4.3; the per-event latch of §3.3; the extended `observable` rule at
  `:4016`; the counters; the `A48` provenance echo; `ObservableMinPixels`' sibling console knobs for
  `tau` and the floors, **all mid-run guarded**.
- `AnomalyLabelWriter.{h,cpp}` — emit the per-row and per-event keys of §3.4 and the `run_summary` block.
  ⛔ `annotation.json` untouched.
- `tools/verify_capture.py` — the single `CHANGE-EVIDENCE` reading line of §5, plus its selftest fixtures
  and a contract pinning that it **cannot** change an exit code.
- `docs/client-readme.md` §8, `docs/client-delivery.md`, `docs/PRE-DELIVERY-CHECKLIST.md`,
  `docs/architecture.md`, `docs/gotchas.md`.

**Untouched by construction:** `IAnomaly.h` · every anomaly · the selector, auto-injector and census ·
`AnomalyMaskReduce.usf` · `AnomalyVisibleMask.usf` · `AnomalyMaskMeasure.*` (the `m26` veto) ·
`annotation.json`'s shape.

Estimated size: ~450 lines of C++/HLSL plus the gate campaign and the cook.

---

## §11 Risks

1. **The cook.** §4.1. The single largest cost and the one a reviewer may refuse. Fallback at §7.4.
2. **`ABSENT` on a real anomaly** — a true positive losing its positive frames. Mitigated by the admit
   bias, the first-`K` latch, and conservative floor derivation; exposed by `G-2` and `G-4`.
3. **Delivered-picture noise** (TAA, dither, auto-exposure) inflating `change_frac_target` and making
   `absent` unreachable — the *opposite* failure, and the one that makes the whole unit vacuous. `G-1` is
   the gate that would catch it; a `change_evidence` that is `present` on a null-effect control means the
   metric is measuring the renderer, not the anomaly.
4. **Moving targets and moving cameras** under-read (§4.6). Both bench fixtures are settled; Lyra is not,
   which is why `G-3` runs there.
5. **Two instruments, one number.** §5's cross-check is valuable exactly because the tool is independent.
   Any future change that lets the producer's numbers steer the tool destroys it silently.
6. **`m51` merge surface** (§4.4).
7. **A new evidence field invites a new veto.** It must not become one: the 077 ruling removed a veto
   built on `target_drawn_pixels` for precisely this reason, and `G258` is the standing warning.
   `change_*` is a measurement and an `observable` input; it is **never** an input to the `m26` veto.

---

## §12 Questions for chat — none of these is decided here

- **Q1 — the subtraction (§1.5).** The brief asks for `change_frac_target - change_frac_control` to be
  what the label carries; `G260` built that, gated it and refuted it in both directions. This plan
  publishes both fields and subtracts neither, using the control as a refusal input per `G260`'s own
  stated rule. **Confirm, or overrule with the reason recorded.** Either way both fields ship, so the
  choice is reversible in a consumer without a re-capture.
- **Q2 — the cook (§4.1).** Every GPU route needs a full cook and retires a container that has been
  byte-unchanged since `m49` Phase B. Ruling needed: **GPU + cook now**, or **the CPU build of §7.4**
  with identical semantics and no cook.
- **Q3 — does `observable` actually change in `m55`?** §3.3 proposes it does, gated on an event-level
  latch. The conservative alternative is that `m55` ships the measurements and the per-event
  `change_evidence` verdict while `observable` keeps the `m49` A1 rule verbatim, and a follow-up flips it
  once the floors have been derived from real distributions. The second is slower and strictly safer.
- **Q4 — `K`, the provisional-admission window (§3.3).** Proposed 4, matching the veto's arm budget.
- **Q5 — the light class (§2.3).** `lighting_mismatch` has no mask and no region and this plan
  deliberately does not invent one. Confirm that a future `m53` is expected to bring its own region
  source rather than `m55` growing one.
- **Q6 — `stuck_low_mip`'s default-on decision.** `080-05` parked it *pending `m55`*. This plan does not
  flip `GAutoPoolDefaultEnabled` and does not assume the answer; `G-3` produces the reading that decides
  it.

</details>
