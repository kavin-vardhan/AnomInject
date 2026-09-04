# Session 071 — Concorde M2 feedback: triage, mechanism reads, and the `m49` plan (READ-ONLY)

**Date:** 2026-09-04 · **Brief:** 071-01 · **Effort:** xhigh · **Scope:** source reads and a plan.
**No source edit, no build, no cook, no checkout switch.** The main checkout stayed on `europa-e1`
(`1f5e305`); every read below was taken in two `git worktree`s: **master** at
`D:\IntrusiveAnomalies\_worktrees_071\master` = `914f14b` (== local `origin/master`; no fetch was run,
the first shell attempt at `git fetch` hung and was abandoned) and **bundle** at
`D:\IntrusiveAnomalies\_worktrees_071\bundle-2b6c93f` = `2b6c93f` (`docs: m33 AMENDMENT 2 +
client-readme launcher note`, committed 2026-08-23 — the plugin tip of the 2026-08-24 Concorde bundle).
Line numbers are `Get-Content` 1-based lines of the file **in the worktree named**, and are stated as
`file:line` with the worktree in the section heading.

Codename discipline: Concorde / Bates / Europa only.

---

## §0. What this session is, in one paragraph

The client reviewed one Concorde session (`session_20260901-165056`, 1080p, ~1,200 frames) by eye and
sent eight findings and three schema recommendations. We do not have the artifacts yet. This session
reads the code the client's build actually ran (`2b6c93f`) and the current `master`, and sorts the
findings into three classes: **timing** (F1, F3, F6-onset), **observability** (F4, F5, F7) and the
**fog blueprint** (F8), with F2 being a schema-semantics question. The single most important source
result is in §3: **at `2b6c93f` the labels of `missing_texture` and `missing_object` are derived from
state the capture subsystem changes inside its own tick, so NO subsystem tick order can move them** —
the mechanism that produced P9 on Bates (blinking, injector-driven toggles) does not reach F1 or F3.
Whatever moved F1 and F3 acts on the picture↔index pairing or on how the frames were read, not on the
sample point, and it is decidable from the client's own PNGs by the verifier specified in §7. Every
finding then gets a mechanism status (MEASURED / CONSISTENT-WITH / CANDIDATE), and §8 carries the `m49`
plan: per-frame observability derived from the frame's own render.

---

## §1. The findings, restated as observations (client's eye; frame numbers taken as `session_index`)

| # | idx | type | target | label | eye | class |
|---|---|---|---|---|---|---|
| F1 | 2 | missing_texture | `InstancedFoliageActor_0` | 27–34 | 26–33 | timing (−1 at both edges) |
| F2 | 0 | blinking | `StaticMeshActor_64` | window 4–10, indices [4,5,9,10] | "two runs 4–5, 9–10" (a question, not an alignment observation) | schema |
| F3 | 4 | missing_object | `StaticMeshActor_47` | 51–58 | 50–58 | timing (−1 at onset, end matched) |
| F4 | 5 | blinking | `StaticMeshActor_2261` | window 64–70, [64,65,69,70] | object leaves frame; 69–70 not observable | observability |
| F5 | 9 | missing_texture | `StaticMeshActor_33` | 111–118 | nothing visible | observability |
| F6 | 41 | blinking | `StaticMeshActor_35` | window 496–502, [496,497,501,502] | 495–497 and 499–502; 498 clean | timing (−1 at onset) + interior |
| F7 | 44 | missing_texture | `StaticMeshActor_261` | 531–538 | cannot be seen | observability |
| F8 | 98 | missing_texture | `WLD_EasyFog_BP_C_3` | 1179–1186 | 1179–1184; 1185–1186 clean | fog BP / material retaken |

Two facts about the delivered configuration that the reads below depend on: the client ini set
`bMaskMeasureDefault=True`, so the `m26`/`m27` veto (measured-zero events deleted) **was live** in
that build; and `labels.jsonl` is written in delivery mode by default since `f491514` (2026-08-22,
inside the bundle), so the client's session **has per-frame `bbox_px` rows** — the verifier in §7 can
run on it.

---

## §2. T4 — schema check at `2b6c93f` (bundle worktree)

`AnomalyLabelWriter.cpp:541-583` (`WriteSessionAnnotation`):

```
566: int32 Start = 0, End = 0;
567: const int32 Count = E.FrameIndices.Num();
570: Start = E.FrameIndices[0];
571: End = E.FrameIndices.Last();
573-575: start_frame = Start, end_frame = End, frame_count = Count
578: frame_indices = E.FrameIndices
```

So **`frame_count == frame_indices.Num()`**, `start_frame`/`end_frame` are the first/last index.
Where `FrameIndices` comes from (`AnomalyCaptureSubsystem.cpp:2719-2769`): for every id whose active
source is NOT `FireWindow` — `blinking` and `missing_object` are `ActorHidden` (`:251-252`) — the list
is the **sorted keys of `ActiveByIndex` whose value is 1** (`:2721-2729`, `:2747-2753`), i.e. the gapped
ACTIVE SUBSET; for `FireWindow` ids (`missing_texture`, `corrupted_texture`, `lighting_mismatch`,
`lod_corruption`, `time_dilation`, `:254-259`) it is `Ev.AffectedFrames` verbatim (`:2766`), and
`manifested` is unconditionally true for them (`:2240-2246`).

**Identical on master** (`AnomalyLabelWriter.cpp:718-734` at `914f14b`) — the block is byte-for-byte
the same logic. ⇒ For F2, `frame_count` is **4**, not 7: the client's reading "frame_count describes
the span" is refuted for their data. The client's real question — *why one record for two runs* —
is answered by construction: one **fire** = one event; `blinking` toggles inside its fire window and
`annotation.json` carries the active subset (the `TOGGLING-SUBSET` log line `:2772-2777` says so).
The span (7) is recoverable as `end_frame − start_frame + 1`, and §8 adds it as `span_frame_count`.
⚠ The delivered README defines none of these fields (§3.9): the misreading is a **documentation gap,
MEASURED**, and the v2 README must carry the field table.

---

## §3. T1 — the timing class: transition arithmetic at `2b6c93f`

### §3.1 The exact sample points (bundle worktree, measured from code)

- **Arm.** `CaptureCurrentFrame()` (`AnomalyCaptureSubsystem.cpp:1663-1707`) is called from the phase
  switch inside `Tick` (`:509-568`). It mints `RequestId`, stamps `Snap.FrameCounter = GFrameCounter`
  (`:1681`) and `Snap.SessionIndex` (`:1682`), parks the snapshot in `PendingSnapshots` (`:1693`) and
  arms the SVE (`:1696`). The snapshot carries **no fire state yet**.
- **Event membership.** `FinalizeArmedLabel()` is the **last statement of `Tick`** (`:570`) and sets
  `Snap->Fires = Auto->GetLiveFires()` (`:1867`). Because it runs after the phase switch, a
  `BeginFire()` (`:1637-1651` → `TryFireOnce`/`TryFireSpecific` → `ApplyAnomaly`, all synchronous:
  `AnomalyAutoInjectorSubsystem.cpp:196-287`, `AnomalyInjectorSubsystem.cpp:445-470`) or a
  `BeginRevert()` (`:1653-1661` → `RevertAllLiveFires`, `AnomalyAutoInjectorSubsystem.cpp:579-592`)
  that ran in the **same tick, after the arm**, is already reflected in that frame's `Fires`.
- **Active state.** `SampleDeferredActiveState()` runs at the **top of the NEXT `Tick`** (`:461`,
  body `:1991-2033`) and fills `Snap->FireActive` for the frame armed on the previous tick:
  `ActorHidden` ids read `FActor->IsHidden()` (`:2029`); `AnomalyState` ids read
  `IsAnomalyCurrentlyAnomalous` (`:2025`); `FireWindow` ids fall into the same else branch (`:2027-2030`)
  but the value is never consulted for them (§2).
- **Accumulation.** The async drain `ProcessCompletedFrames` (`:491`, body `:1442`) runs **after** the
  deferred sample (`:461`), so a completed frame's snapshot already carries `FireActive` when
  `AccumulateFrameEvents` (`:2590-2664`) runs: `AffectedFrames.Add` is gated on
  `ProjectActorBoundsToScreenRect` (`:2650-2652`, G98); `ActiveByIndex.Add(SessionIndex, Active)`
  (`:2662`).
- **Config.** `SettleFrames=2, PreFrames=4, PositiveFrames=8, PostFrames=4`
  (`AnomalyCaptureSubsystem.h:250-254`); `BeginFire` sets `PhaseFramesLeft = SettleFrames` (`:1650`),
  `BeginRevert` likewise (`:1660`).
- **The three anomalies.** `Blinking::Apply` never hides (`Anomaly_Blinking.cpp:10-71`, `bHiddenPhase=
  false`, `FramesSinceToggle=0` at `:64-65`); `Blinking::Tick` (`:73-98`) does `++FramesSinceToggle`
  (`:80`) and toggles `SetActorHiddenInGame` when it reaches `HalfPeriodFrames` (`:81-91`, default 3);
  it is ticked from `UAnomalyInjectorSubsystem::Tick` (`AnomalyInjectorSubsystem.cpp:184-199`).
  `MissingObject::Apply` hides at `Anomaly_MissingObject.cpp:35`, has **no `Tick`**, and `Revert`
  un-hides at `:53`. `MissingTexture::Apply` swaps **every slot of every matched mesh component**
  (`Anomaly_MissingTexture.cpp:112-124`), has no `Tick`, and `Revert` restores or default-resets
  (`:136-253`).
- **Render.** The picture filed under the index armed on tick T is the render of the world at the end
  of tick T: render-state dirtying is flushed inside `BeginRenderingViewFamilies` in the same frame
  (F-1, `SceneRendering.cpp:4528`, journal 045 §157-§161), and on a lockstep loop the SVE pairs the arm
  with that tick's view family (`maxPendingDepth=1`, m31).

### §3.2 The tick model, both orders

Let C(T) be the capture subsystem's `Tick` on engine tick T and I(T) the injector's. Inside C(T) the
order is fixed: (1) deferred sample for the previous frame; (2) phase switch — arm, then possibly
`BeginFire`/`BeginRevert`; (3) `FinalizeArmedLabel`. Two host orders exist:

- **CI** — C(T) before I(T). The bench (measured: the injector is constructed first and ticks second,
  CLAUDE.md "the order is not declared anywhere"). `apply → first toggle` reads **+2**.
- **IC** — I(T) before C(T). Bates, inferred from the **+3** reading (ledger §8.6a).

⚠ The brief's phrase *"a host where the capture subsystem ticks BEFORE the injector"* names **CI**,
which is the bench's own order. Both are derived below, so the result does not depend on which one
Concorde has.

**Session index ↔ tick.** Within a burst whose fire tick is T_f, the frame armed on T_f has index f
(the last lead-in / post-gap frame; `BeginFire` follows its arm at `:512-513`); settle consumes
T_f+1, T_f+2 with no arm; the eight positives are armed on T_f+3 … T_f+10 = indices f+1 … f+8;
`BeginRevert` runs inside C(T_f+10) **after** the arm of f+8 (`:522-523`). With the default config
the first burst has f = 3, which is why the banked annotation windows read `[3..10]` for texture
types and `{4,5,9,10}` for `blinking`.

### §3.3 `missing_texture` (`FireWindow`) — order-independent by construction

Label(index armed on T) ∈ event ⇔ the fire is in `LiveFires` at step (3) of C(T).

- **Onset.** The fire is created at step (2) of C(T_f) → index f is IN. The material swap happens in
  the same C(T_f) → the render at the end of T_f carries the checker → **offset 0**.
- **Revert.** `RevertAllLiveFires` at step (2) of C(T_f+10), after the arm of f+8 → step (3) sees no
  fire → f+8 is OUT (and the fire record is gone, so f+8 has no entry at all). The materials are
  restored in the same tick → the render at the end of T_f+10 is clean → **offset 0**.
- Label = pixels = {f, f+1 … f+7}: eight frames. Nothing the injector's tick does touches either
  side. **The same in CI and IC.**

Check against F1: label 27–34 = f = 27, derived pixels = 27–34, eye = 26–33.

### §3.4 `missing_object` (`ActorHidden`, hide-in-Apply) — order-independent

The hidden flag changes only at step (2) of C(T_f) (hide) and step (2) of C(T_f+10) (un-hide);
`Blinking`-style ticking does not exist for it.

- **Onset.** Index f: hidden in C(T_f); its `IsHidden()` is read at step (1) of C(T_f+1) → 1; the
  render at the end of T_f is hidden → **offset 0**.
- **Revert.** Index f+7 (armed on T_f+9) is read at step (1) of C(T_f+10), **before** the un-hide at
  step (2) → 1; its render (end of T_f+9) is hidden. Index f+8 is armed on T_f+10, un-hidden in the
  same tick, and dropped from `Fires` at step (3) → no entry; its render is clean → **offset 0**.
- Label = pixels = {f … f+7}. In IC, I(T) precedes C(T) but changes nothing for this id. **Same
  in both orders.**

Check against F3: label 51–58 = f = 51, derived pixels = 51–58, eye = 50–58 (onset −1, end equal).

### §3.5 `blinking` — the P9 shape falls out of IC exactly; F6 is not that shape

Apply at step (2) of C(T_f): `FramesSinceToggle = 0`, visible. Half-period 3.

**CI** (the apply tick IS counted: I(T_f) runs after C(T_f)): toggles at I(T_f+2) HIDDEN,
I(T_f+5) VISIBLE, I(T_f+8) HIDDEN. The revert in C(T_f+10) precedes I(T_f+10), so the render at
T_f+10 is visible. Hidden renders: T_f+2 (settle, uncaptured), T_f+3, T_f+4, T_f+8, T_f+9 =
indices **{f+1, f+2, f+6, f+7}**. Label: `IsHidden()` at step (1) of C(T+1) is read after I(T) and
before I(T+1), i.e. it is the state the render of T showed → label = pixels = {f+1,f+2,f+6,f+7}.
With f = 3: **{4,5,9,10}** — the bench's measured m20 `G2` set.

**IC** (the apply tick is NOT counted): toggles at I(T_f+3) H, I(T_f+6) V, I(T_f+9) H. The revert
in C(T_f+10) follows I(T_f+10) (which only counts, `FramesSinceToggle` 0→1) → T_f+10 visible.
Hidden renders: T_f+3, T_f+4, T_f+5, T_f+9 = pixels **{f+1, f+2, f+3, f+7}**. Label(T) is read at
step (1) of C(T+1), which is now AFTER I(T+1) → it is the state of the render of T+1, one tick early:
label(f+1)=1 (after I(T_f+4), still hidden), label(f+2)=1, label(f+3)=0 (I(T_f+6) toggled VISIBLE
before the read), label(f+6)=1 (I(T_f+9) toggled HIDDEN before the read, although T_f+8 rendered
visible), label(f+7)=1 → label = **{f+1, f+2, f+6, f+7}**. With f = 3: claimed {4,5,9,10} against
observed {4,5,6,10} — **exactly the ledger's P9 signature** (claimed {n,n+1,n+5,n+6}, observed
{n,n+1,n+2,n+6}, n = 4), hidden-frame count conserved. This reproduces ledger §8.6a with no free
parameter.

**F6** (label [496,497,501,502] ⇒ n = 496, f = 495). CI predicts pixels = label; IC predicts pixels
**{496,497,498,502}**. The eye reports {495,496,497,499,500,501,502} with **498 clean**. IC is
contradicted by "498 clean"; neither order produces 495, 499 or 500. **F6 is not the P9 shape and not
a one-tick shift of the 2-3-2 cadence.**

### §3.6 Answer to T1(a): no ordering produces the three shapes at once

Neither CI nor IC moves a `missing_texture` or `missing_object` label at `2b6c93f`, and neither
produces F6's 495. The premise *"one ordering produces F1, F3 and F6-onset"* is **refuted by
derivation**. Whatever moved the onsets acts on the **picture↔index pairing** or on the **reading**,
not on the sample point. Candidates, each with the one artifact reading that decides it:

| candidate | direction it produces | discriminating reading |
|---|---|---|
| (a) **Reading offset** between the client's viewer and `session_index` (the m20 "Bug A" precedent). A 1-based player makes the eye read **later** than the label (+1), the opposite of F1. The eye reading **earlier** needs the viewed sequence to be one frame AHEAD of the PNG numbering — e.g. an MP4 whose frame k is PNG k+1, or counting from the second frame. ⚠ **Weakened by §3.9:** the encoder writes `-start_number 0`, so MP4 frame k IS PNG k; (a) survives only as an unusual reading (a viewer that drops frame 0, or a count started at the second frame). | −1 at both edges (F1 ✓), −1 at F3/F6 onset ✓; F3's end needs (c) | Open `Actual_Frames/frame_00026.png` vs `frame_00027.png`: the first checkered PNG IS the onset. §7's verifier does this mechanically. One question to the client: MP4 or PNGs, and which viewer. |
| (b) **Picture↔index pairing slip on the host**: the image filed under index i is the render of the tick after its arm. Impossible on a lockstep loop (m31, `maxPendingDepth=1`); Concorde's decoupled fork is forced lockstep by the tick pin during capture, and the owner's 2026-08-22 Concorde measurement read **+0** on this code lineage (§3.9). | same as (a) | `run_summary.tickpin_applied`, `tickpin_saved`, `wanted_matches == total_frames`, `key_ring_missed == 0`; the log's `SVE-WANT-SUMMARY … maxPendingDepth=` (1 = lockstep; >1 = live). |
| (c) **Temporal accumulation at the revert** (the ledger §8 re-appearance ladder): a reappearing object or a swapped-back material fades in over 1–3 frames under TAA/motion blur, so the first clean frame reads "still missing" by eye. | +1 at the END only | Under (a)/(b) F3's end (eye 58 = PNG 59, the clean frame) needs exactly this; F1's end does not (the eye saw 34 = PNG 35 clean). The verifier's END reading with an AA-off host smoke settles it. |
| (d) `AffectedFrames` projection filter (G98) dropping the fire frame → label starts late | +1 at onset only | **Refuted** for F1/F3: their windows are 8 frames; a dropped fire frame leaves 7. |
| (e) The swap not reaching the fire frame's render | pixels LATE, not early | Refuted by F-1 for the lockstep case and by direction. |

Status: F1 / F3-onset / F6-onset = **CANDIDATE (a) or (b)**; F1-end matched under (a)/(b);
F3-end = **CONSISTENT-WITH (c)** given (a)/(b), otherwise an eye reading. **Nothing is claimed.**
One independent data point for (b) being absent: the owner's in-round Concorde measurement on the
pinned m32 build read every offset **+0 at start and end** (session 052, quoted in §3.9).

### §3.7 T1(b) — what master certifies, and the leg that would close it

Surveyed: `docs/predictions/2026-09-02-m40-…`, `…2026-09-03-m43-target-mask*.md`,
`…2026-09-03-m44-m45-…`, journal 069 §7–§15, `PRE-DELIVERY-CHECKLIST.md`, and the CaptureBench
instruments at `D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\`.

| gate | types it ran on | edge | orders |
|---|---|---|---|
| m40 `L1`–`L4` + the m20 re-run (hidden-set equality) | `blinking` only | the whole hidden set | native + lever |
| `M44-G1` ONSET (`m44_gates.py`) — `delta == 0` between `frame_indices[0]` and the first mask PNG with content | `corrupted_texture` ×2, `missing_texture` ×2; after m45 `blinking` ×2 (6/6) | ONSET only | native + `IAI.Bench.SynthTickOrder` |
| `M44-G7` / `M45-G2` mask ⊆ labelled frames, equality for hidden-class | blinking, corrupted_texture, missing_texture | per-frame set membership | both |
| MASK-PICTURE-PAIRING (`m44_pairing_probe.py`) | blinking recipe | pairing, not onset | both |
| m43 `(i)`–`(x)` | all pooled types | counts / bit-exact tie | native |

⚠ The "first differing PICTURE frame" clause of ONSET is **printed, not asserted**: `m44_gates.py`
prints `ONSET % pixels >8/255 vs clean ref (firstLabel-2): n-1 / n / n+1` (whole-frame RGB
difference, `THRESH = 8`, reference frame `first − 2`) and appends nothing from it to `fails`. The O4
wording it reproduces (069 §7.1 A2): *"onset: % of pixels differing by >8/255 from a clean reference
(`firstLabel − 2`)"* — *"the picture already differs at frame n by 5.9–8.2 % (CorruptedTexture)
against a ~0.5 % baseline"*.

**Answer: NO.** Three independent gaps: (a) `missing_object` is an event in **no** onset leg (069
§7.1 A1 and §14.3 list only blink / corrupted_texture / missing_texture); (b) **no gate on master is
end-side** for any type — nothing asserts that the frame after `end_frame` is the first clean frame;
(c) the picture clause of ONSET is a reading, not a verdict.

**The leg that closes it — `m49-G-EDGE` (specified, NOT run):** packaged bench binary,
`CB_GateLevel`, targeted mode, one leg per type ∈ {`missing_texture`, `corrupted_texture`,
`missing_object`, `blinking`} × order ∈ {native, `IAI.Bench.SynthTickOrder 1`} = 8 legs; delivery
OFF; `IAI.Capture.Config 2 4 8 14 0` (clean gap 14 ⇒ ceiling ±7, G160); marker OFF (G125); the
AA-off configuration (`r.AntiAliasingMethod 0`, motion blur off — the identity arbiter's
configuration) so a one-frame edge is a step and not a ramp; 90 frames; `-RequireModalRotZero`.
Reading per event, region = the event's `bbox_px` (the mask when present):
ONSET := d(start) > τ AND d(start−1) ≤ τ; END := d(end+1) > τ AND d(end) ≤ τ, where d(k) is the
fraction of region pixels differing by > 8/255 from frame k−1 and τ = 3 × the clean-frame baseline
with an absolute floor of 0.5 %; for `blinking` every contiguous run of `frame_indices` is an edge
pair. **Pre-declared pass:** every event 0 / 0 on all eight legs; any non-zero is a STOP with the
delta printed, never re-run for green. Instrument: the §7 verifier in gate mode — one algorithm,
two homes — proven on a synthetic ±1 first (G142). The same eight legs at the DELIVERED
configuration are REPORT-ONLY: the ~9 % cross-run floor (G228) forbids a pixel claim there.

### §3.8 T1(c) — F6's 499–500

Candidates for the two VISIBLE in-between frames of the blink reading as affected, with the single
artifact reading that discriminates each. **NOT CLAIMED.**

1. **Re-appearance opacity ladder** (ledger §8): the object fades back in across 498–500 under
   temporal accumulation, so 499–500 look "partly there". Against it: the eye called 498 (the first
   visible frame, where the ladder is steepest) **clean**. Reading: per-frame in-bbox luma series
   from the PNGs (`d(k)` in §7) — a ladder shows a monotone ramp 498→500; a clean 498 with a dip at
   499–500 does not.
2. **A second visual change on the same target** — the tag/untag of the m26 mask arm on this actor
   (`ArmIfMeasurable`, `AnomalyMaskMeasure.cpp:154-202`, ≤4 arms per event) queues a render-state
   recreate on each tag flip; `tagFlips` was measured 0 on every bench leg (m43) but is UNMEASURED on
   Concorde. Reading: the log's `M23 ARM` lines for idx 41 (which frames were armed) against 499–500.
3. **Camera motion / motion blur on a moving target** (the client is playing): the object's
   appearance at 499–500 differs from its neighbours for reasons unrelated to the hide. Reading:
   `labels.jsonl` `view.origin`/`rotation` deltas across 495–503 and the ambient (outside-bbox)
   difference series — `measure_label_offset.py`'s AMBIENT test reads this directly.
4. **Eye error on a fast 30 fps blink** — the discriminator is the same PNG series as (1).

### §3.9 Numbers taken from other records (survey, cited not re-derived)

- **The owner's Concorde tick-pin validation** (`docs/sessions/2026-08-22-052-…md` §0, lines 25-29,
  dated 2026-08-22): *"Concorde validation, packaged and in-round: `TICKPIN active saved=1`, 300
  frames, 7 events, 5 measurable, EVERY offset `+0` at start and end, 4 of 5 at HIGH confidence,
  measurable range ±7."* No build SHA and no `IAI.Capture.Config` tuple are recorded for it; a ±7
  ceiling implies a clean gap of 14 (the `2 4 8 14 0` diagnostic schedule, G160), not the shipped
  `2 4 8 4 0`. It is the only in-round measurement on Concorde's hardware of this label lineage and
  it read aligned at both edges — n = 5 events, one session, AA state unrecorded.
- **The encoder numbers the MP4 exactly as the PNGs.** `D:\IntrusiveAnomalies\host-tools\
  encode_watcher.py:117-131` builds `ffmpeg -framerate <fps> -start_number 0 -i frame_%05d.<ext> …`
  with no `-r` and no vsync flag, so **MP4 frame k is `frame_000kk.png`**. The delivered README says
  the same (bundle `docs/client-readme.md:270`: *"frame N in the folder is frame N in the video and
  frame N in the annotation's frame indices"*), and `measure_label_offset.py:16-24` states the viewer
  rule: *"A media player or image viewer that numbers the first frame '1' will appear to disagree
  with this tool by exactly +1. It is the viewer that is offset, not the data."*
- **The bundle's README carries no field-level schema** for `annotation.json` — `affected_frames`,
  `frame_count`, `frame_indices`, `manifested` and `mask.provided` do not appear in it (only §8's
  one-paragraph description at `:267-270`); master's README adds the m43 mask block but still no
  `frame_count` definition. The client's F2 reading was made against an undocumented field.
- **The delivered ini block** in the bundle's `client-delivery.md:299-303` is
  `[AnomalyCapture] bMaskMeasureDefault=True / bDeliveryModeDefault=True`; the `[AnomalyInjector]`
  patterns for Concorde live in session 052 (`:133-138`) with an unresolved placeholder in the bundled
  copy. The `Decal` / `_CR_` patterns were added to the checklist 2026-08-24, after the bundle.
- **The dashboard's capture-mode default is auto-pool** (`anomaly-dashboard/src/store.ts:120`
  `captureMode: 'auto'`, re-asserted on reset at `:210`); a targeted capture requires the client to
  have switched the toggle.
- **The bundle manifest** (`anomaly-dashboard/host-tools/bundle_manifest.txt:48-71`) ships
  `tools/verify_capture.py` as a `PLUGINFILE` into `host-tools/`, plus `overlay_watcher.py` and
  `encode_watcher.py`; **`tools/measure_label_offset.py` does not ship.** `PLUGINFILE` entries are
  opt-in (`--plugin-repo`), and without it the README and the verifier are omitted with exit 0.
- **Client-facing TAA statement:** master's README has one, onset-only (*"The first labelled frame of
  a texture anomaly can look subtle … temporal anti-aliasing settles over the following frames"*,
  `:357-362`); nothing about the revert edge, and the bundle's README has no TAA statement at all.

---

## §4. T2 — the observability class

### §4.1 What the mask pass already gives per frame (master worktree)

- Per armed frame the pass renders `AnomalyVisibleMask` (`AnomalyMaskSceneViewExtension.cpp:144-161`;
  shader `Shaders/Private/AnomalyVisibleMask.usf:22-44`: output = the reserved stencil tag where the
  tagged surface is front-most, `CustomDeviceZ >= SceneDeviceZ − DepthBias`, mapped through the
  internal view rect — m46) and reduces it on the GPU to a **256-entry {Count, MinX, MinY, MaxX,
  MaxY} table** (`AnomalyMaskReduce.usf`, integer atomics, bit-exact to the CPU scan; consumed at
  `:357-378`). The result per request is `FAnomalyMaskResult::TagResults` (`AnomalyMaskTypes.h:5-25`),
  plus `MaskPixels` when asked.
- One render serves every pending arm (m43, `:117-135`), so the target mask, `m26` and the census read
  the same frame.
- `ArmTargetMaskOwn` (`AnomalyCaptureSubsystem.cpp:866-988`) tags every live fire's actor with the
  EVENT's tag on the captured tick and arms with `bWantPixels`; `ServiceTargetMask` (`:990-1100`)
  joins the result to the **session index**, already reads **`TableCount = TagResults[tag].Count`**
  per event tag (`:1059-1062`, the MASK-TIE line), writes the PNG only if any pixel survived, and
  stamps `Present / Empty / Unmeasured` (`:1076-1092`), which the drain joins into the snapshot
  (`:3178-3206`) and the row emits as `mask_state`/`mask_file` (`AnomalyLabelWriter.cpp:107-119`).
- The per-fire tag reaches the row as `mask_value` (`:3835-3852`, `AnomalyLabelWriter.cpp:65-70`).
- Hidden-class frames: `m45` hides by dropping the main and depth passes while **keeping custom depth**
  (`AnomalyHiddenClass.cpp:73-133`), so the would-be silhouette is in the same table; the labels' notion
  of hidden is `IsLogicallyHidden` (`:174-185`), used by `IsFireLabelledThisFrame` and
  `ComputeFireActive` (`AnomalyCaptureSubsystem.cpp:3855-3893`).

⇒ **The per-frame, per-fire count that T2 needs is computed today and thrown away after the tie
check.** Threading `TableCount` into the snapshot beside `MaskValues` is the whole of the
`target_pixels` mechanism; no new shader, no new pass.

### §4.2 F5 / F7 — max drawn count > 0 yet nothing an eye can see

First a correction to the brief's premise. At `2b6c93f` the veto deletes an event **only** when its
state is `MEASURED_ZERO` (`AnomalyMaskMeasure.cpp:437`; the veto reads the enum state alone,
`AnomalyCaptureSubsystem.cpp` `MaskStateVetoes`). `NOT_MEASURED` also survives — no pass
(`framesNoPass`, `:370-389`), residual/unconfirmed/polluted discards, tag failure, or all four arms
skipped as hidden. So *"survived ⇒ drew ≥1 pixel"* holds only if the event's state was
`MEASURED_NONZERO`. **The reading that decides it is in the client's artifact:
`annotation.json anomalies[9|44].mask.provided`** (`true` = measured, `false` = never measured;
`AnomalyLabelWriter.cpp:633` at the bundle) together with `run_summary.mask_nopass_discards` and, if
the client kept the game log, the `M26S1 EVENT … state=… maxCount=` lines. Concorde has Nanite
disabled, so no-pass would be frustum culling or an off-screen arm, not G134.

Candidates, given `MEASURED_NONZERO`:

| # | mechanism | what `target_pixels` alone says | what else discriminates |
|---|---|---|---|
| (i) | **A sliver** — the target is on screen but tiny (a distant or edge-on mesh); tens of pixels at 1080p. | **Settles it**: the count is small. | — |
| (ii) | **Mip collapse of the checker** — at small on-screen size the checker averages to flat mid-grey and is indistinguishable from an untextured surface. `coverage_pct` (bounds coverage at pick time, ≥ the 6 % floor) says the BOUNDS were large; bounds over-read drawn extent by ~4× (G149), so the drawn area can still be ~1 % of frame with a large `coverage_pct`. | **Does not settle it**: the count can be large. | A contrast reading inside the mask/bbox region — the CHECKER classifier already in `measure_label_offset.py` (`CHECKER_MODE_SEP`, `CHECKER_SAT_MAX`): bimodal luma = checker present, unimodal = collapsed. §7 reports it. |
| (iii) | **Partial occlusion** — most of the target is behind something; front-most count small. | Partly: the count is small, but (i) and (iii) look alike on the count. | The reduce table's per-tag **bounds** (MinX..MaxY) vs `bbox_px`: an occluded target's mask bounds are a sub-rect or fragmented; a distant one's mask ≈ its bbox. This is m39's honest-bbox input, which is one reason to fold m39 in (§8.2). |

So `target_pixels` alone settles (i), half-settles (iii) and does not settle (ii); the knob
`ObservableMinPixels` therefore catches slivers and occlusion, and the verifier's contrast reading is
what catches a collapsed checker. Recorded as a limit of the count-based `observable`.

### §4.3 F1 — how an `InstancedFoliageActor` was selected at `2b6c93f`

MEASURED from code (bundle worktree). The auto pool (`TryFireOnce`,
`AnomalyAutoInjectorSubsystem.cpp:196-287`) draws candidates from
`AnomalyViewport::GetVisibleRenderableActors` (`:230`; `AnomalyViewport.cpp:835-862`) →
`ClassifyRenderableVisibleLive` (`:450-467`) → `IsComponentRenderableVisibleInternal` (`:252-267`) →
`IsRenderableComponent` (`:580-609`), which returns **false for every component whose owner
`IsA<AInstancedFoliageActor>()`** (`:587-593`, the m27 exclusion, in the bundle). **The auto pool
cannot pick an engine foliage actor at `2b6c93f`.**

The **targeted** path can. `TryFireSpecific` (`:289-350`) resolves the name with
`AnomalyTargeting::FindActorsMatching(World, "=" + ActorName)` (`:314`) and applies **with no viewport
predicate at all** — it is the m10 design ("visibility-independent", G61). It is reached from the
dashboard's `capture_start {anomaly, target}` (`AnomalyControlServerSubsystem.cpp:605-644`) and from
`IAI.Capture.Start … <anomaly> <target>`. `MissingTexture::Apply` then swaps every slot on every mesh
component the name resolves to (`Anomaly_MissingTexture.cpp:84-127`), foliage ISMs included.

Candidates, ranked:
1. **The client ran a targeted capture naming `InstancedFoliageActor_0`** (MEASURED: the only code
   path at `2b6c93f` that admits the actor; the dashboard's default is auto-pool, §3.9, so this needs
   the toggle to have been switched). Reading: the game log's
   `=== Capture run STARTED … mode=targeted[missing_texture on InstancedFoliageActor_0]` line
   (`AnomalyCaptureSubsystem.cpp:1207-1210`); `run.json` would say `mode` but is suppressed in
   delivery, and the m38 run log post-dates the bundle.
2. **The actor is not an `AInstancedFoliageActor`** — a host class merely named like one; then the
   auto pool admits it. Reading: `annotation.json affected_objects.nodes[0].component_class` —
   `FoliageInstancedStaticMeshComponent` means engine foliage (and therefore candidate 1); anything
   else means candidate 2.

The seeded auto pool changes with this actor's presence in the candidate set (G140), so the answer
also decides whether the session's other events are comparable to any auto-pool run.

### §4.4 The design: `target_pixels`, `observable`, knob, schema v2

**`labels.jsonl`, per anomaly entry (additive):**
- `target_pixels` (int): the front-most drawn pixel count of the target on THIS frame, from the
  frame's own reduce table (`TagResults[mask_value].Count`); for hidden-class frames it is the
  would-be silhouette count (m45); **`-1` = unmeasured** (no mask this frame: `mask_state:
  "unmeasured"`, mask off, Nanite/translucent target, sync fallback).
- `observable` (bool): event active on this frame (the existing per-frame active rule) AND the
  anomaly's visual condition held at draw time (§5) AND `target_pixels >= ObservableMinPixels`.
  Unmeasured (`-1`) reads **false** under the brief's rule — see the fork in §9 Q3.

**Knob:** `IAI.Capture.ObservableMinPixels <n>` / ini `[AnomalyCapture] ObservableMinPixels`,
compiled default **1**, three-branch provenance echo at `StartRun` (G139), refused mid-run like the
other capture knobs. Absolute pixels vs per-mille of the picture: a per-mille threshold scales with
resolution (1‰ of 1080p = 2,074 px, of 720p = 922 px), which is what an "eye-visible" criterion wants,
but it hides the unit the client actually looks at and makes the number differ between the 720p bench
and a 1080p host, so every gate reading would need converting. **Recommendation: absolute pixels as
the stored field and the knob, with the echo line printing the equivalent per-mille for the session's
written size**; chat sets the calibrated number from the client's session (G135: `1` is not a
tolerance, it is "any pixel", the only uncalibrated value that is not a guess).

**`annotation.json`, per event — a SEMANTIC CHANGE of existing keys, recorded as such, not additive:**
- `affected_frames.{frame_indices, start_frame, end_frame, frame_count}` become the **OBSERVABLE
  subset** (client R1: min/max visually affected frame). Same key names, tighter meaning; this is
  schema v2 and the client docs must say so in those words.
- `affected_frames.span_frame_count` = `end_frame − start_frame + 1` (client R2).
- new sibling `injected_frames.{start_frame, end_frame, frame_count, frame_indices}` = today's
  active subset, unfiltered (client R3's `injected_in_scene`).
- `observable_frame_count` (= `affected_frames.frame_count` by construction; kept explicit so a reader
  who only sees `frame_count` cannot mistake which subset it counts); `manifested`, `coverage_ratio`,
  `coverage_pct` unchanged.
- a root `label_schema: 2` (a new root key — P6 moves, deliberately; chat to confirm the key name).

**Veto semantics this milestone: unchanged.** An event with drawn pixels on some measured frame but
zero observable frames (e.g. every count below the knob, or the condition lost on every frame) **stays
in the file** with an empty observable set and a full `injected_frames`. Should it follow the veto
instead? Recommendation: **no, not this milestone** — the veto's rule is "measured zero on every clean
frame", a property of the target; an empty observable set is a property of the threshold and the
per-frame condition, and deleting on it would re-introduce a threshold into the veto, which the m26
ruling forbids. The client sees `observable_frame_count: 0` beside `injected_frames`, which is the
honest reading. Chat decides.

---

## §5. T2b — the structural fix: the label from the frame's own render

Every desync so far was host-specific and invisible on the bench (P9 on Bates; F1/F3/F6 on Concorde),
and the client checks labels against pixels. So the per-frame positive must rest on evidence that
comes out of the SAME render as the picture, not on CPU state sampled somewhere in the tick.

**What is already same-frame.** The mask pass runs inside the frame's own render (after tonemap,
reading that frame's custom stencil/depth), and the SVE grabs scene colour from the same view family;
`m44`'s MASK-PICTURE-PAIRING gate proves the mask and the picture are the same frame. So
`target_pixels` (§4.4) is same-frame evidence of "the target's surface is front-most at N pixels" —
occlusion-aware, resolution-correct (m46), and independent of any subsystem tick order.

**What is not yet same-frame, per type:**

- **Hide types** (`blinking`, `missing_object`): the silhouette count says the target *would be*
  visible; it does not by itself say the main pass did not draw it. Cheapest GPU reading that does:
  the mask shader distinguishes **drawn** (`|CustomZ − SceneZ| ≤ bias`: the front-most surface IS
  the target, i.e. the main pass drew it) from **hidden-but-would-be-visible** (`CustomZ > SceneZ +
  bias`: the target's depth is nearer than what the main pass actually put there). Emit it as a
  second output (RT1 = tag where drawn, else 0; RT0 unchanged so MASK-TIE, the PNG and every existing
  gate stay bit-exact) and reduce both → per tag `Count` and `DrawnCount`. Then for hide types
  `condition_held(frame) := DrawnCount == 0 && Count > 0` — the GPU's own statement that the target was
  absent from the picture where it should have been. Cost: one extra R8 target and a second reduce
  (≈ 1 ms at 1080p, measured shape of the existing reduce); **a global-shader change ⇒ needs a full
  cook (G129)**.
- **Texture types** (`missing_texture`, `corrupted_texture`): the stencil is per primitive, not per
  material, so the GPU cannot say which material was drawn, and a colour-signature test on the GPU
  would be a heuristic under lighting. What CAN be read cheaply: at `FWorldDelegates::OnWorldTickEnd`
  (`LevelTick.cpp:1814`, after every tickable, before the render kick — the same point m40 and m44
  rest on) walk the anomaly's `Captured` slots and test `IsCheckerDerived(Mesh->GetMaterial(slot),
  AppliedChecker)` (`Anomaly_MissingTexture.cpp:16-36`, already the revert's test). By F-1 that is the
  material the render of this frame will use. `condition_held := every captured slot still ours`
  (and `DrawnCount > 0` once phase B exists: the surface was drawn at all).

**Minimum that closes the loop:** (1) `target_pixels` from the frame's own table (no shader change);
(2) the `OnWorldTickEnd` condition audit for texture types and `IsLogicallyHidden` for hide types;
(3) `observable` derived from (1)+(2)+knob. That removes tick order from the positive by making the
active-state sample and the pixel evidence both same-frame. (4) The `DrawnCount` output is the GPU
corroboration of (2) for hide types and is the only part that needs a cook — sequenced as m49 phase B.

**Residual risk, named:** the texture-type condition is CPU-side. It misses a material swapped on the
render thread by a host proxy, or on the game thread after `OnWorldTickEnd` and before
`BeginRenderingViewFamilies` (no such mutator is known: Slate ticks after the render kick, so a
widget-driven swap lands next frame, where the audit catches it). `IsCheckerDerived` walks ≤ 8 parents
(`GMaxParentChainDepth`). Nanite (G134) and translucent-only targets have no mask ⇒ `target_pixels =
-1` ⇒ never observable under the strict rule (§9 Q3). Hide types rest on the m45 identity arbiter's
scope (AA-off identity) until phase B lands.

---

## §6. T3 — F8, the fog blueprint

**(a) Can a translucent-only actor be a fire target?**
- At `2b6c93f` (bundle): **yes, for every type.** `IsRenderableComponent` (`AnomalyViewport.cpp:580-
  615`) tests `IsVisible`, foliage, the name patterns, ISM instance count and static/skinned type —
  nothing about blend mode. Translucency appears only after the fact, in the veto's
  `translucent_vetoes` counter (`AnomalyCaptureSubsystem.cpp:2229-2237`), and only for events already
  `MEASURED_ZERO`.
- On master: the picker has **no translucency rule either**. Exclusion exists solely as a census
  opinion — `ComponentSlotsAllTranslucent` (`AnomalyCensus.cpp:41-65`, with the m41 rule that a
  translucent material writing custom depth does NOT exempt it when `bAllowCustomDepthOptIn` is false)
  classifies an actor `Translucent` when every renderable component is all-translucent
  (`:85-140`, `:131-134`), and `TryFireOnce` skips `ExcludedTranslucent` opinions
  (`AnomalyAutoInjectorSubsystem.cpp:289-308`). That path is live only when
  `bCensusEffective = census && mask && async`, and never on the targeted path.

**(b) Any per-frame apply-state audit?** None. `IAnomaly::IsCurrentlyAnomalous()` exists (default
`IsActive()`, `IAnomaly.h`) and is overridden only by `camera_clipping` and `lod_popping`;
`missing_texture` learns that "the game replaced it with '%s' after apply" only in `Revert`
(`Anomaly_MissingTexture.cpp:193-200`, the `left-to-game` counter) — after the whole window has been
labelled positive. `master`'s `Anomaly_MissingTexture.cpp` is byte-identical to the bundle's (no
diff between `2b6c93f` and `914f14b`).

**The F8 hypothesis (chat's, unmeasured):** the fog BP re-applies its material (a dynamic instance)
some ticks after our apply. The revert-time counter is exactly the reading that would show it, and it
is post hoc: if the client kept the log, `missing_texture: revert … left-to-game=N` for idx 98 with
N > 0 is the observation. (A BP that calls `CreateDynamicMaterialInstance(slot)` with no source
material would instead parent the MID to OUR checker and the checker would persist; the eye says it
vanished, which is consistent with an explicit re-assignment — CANDIDATE.)

**Proposals, costed:**
- **(i) Picker-level exclusion of translucent-only targets.** Move the census's slot test to the
  viewport layer (shared by census and picker) and refuse such actors in `IsRenderableComponent`'s
  caller for **all** types, not only texture swaps — argument: under §4.4 a translucent-only target
  cannot write custom depth, so `target_pixels = -1` on every frame and the event can never carry an
  observable positive; selecting it buys a burst of nothing. Cost: one blend-mode read per renderable
  component per candidate at pick time (the census already pays it); a G140 boundary (seeded selection
  changes where the census was not effective); loses hide-type events on fog/glass (a real anomaly
  class, but one the label layer cannot vouch for). Knob-able, compiled default ON.
- **(ii) Per-frame "material is still ours" audit at `OnWorldTickEnd`** feeding `observable = false`
  from the frame the slot stops holding our material, plus a `frames_condition_lost` counter and one
  log line per event naming the first such frame. Cost: N captured slots × a pointer compare and a
  ≤ 8-deep parent walk per captured frame — negligible. It is also §5's texture-type condition, so it
  is not extra work.

**Recommendation: `m49` carries BOTH — (ii) is mandatory (it is the T2b condition and covers any host
that swaps materials at runtime, Concorde's modular characters included, m17), (i) as a
default-ON picker rule for all types with a knob.** If chat wants fog/glass hide events kept, (i)
becomes texture-types-only and the hide-type events on such targets ship with
`observable_frame_count: 0` — say so in the client docs.

---

## §7. T6 — the label-vs-pixel verifier the client can run

**Home:** `tools/verify_capture.py --label-pixel-gate` (the tool ships in the bundle as
`host-tools/verify_capture.py`, §3.9). The pixel machinery is not new: `tools/measure_label_offset.py`
(1,960 lines, m32) already measures per-event manifest-vs-claim offsets with a self-calibrated
threshold, a baseline-contamination detector and the MEASURABLE-RANGE ceiling (G160). The verifier
reuses that module — imported from the same directory, which means a second `PLUGINFILE` line in
`bundle_manifest.txt` so it ships beside the verifier (it does not today) — never a second
implementation of the region/baseline/threshold code — and adds the mask readings and the per-event
verdict line.

**Inputs:** `annotation.json` (events, `frame_indices`, `manifested`), `labels.jsonl` (per row:
`session_index` — the join key, never `frame_index`, G161 — `bbox_px`, `mask_file`, `mask_value`),
`Actual_Frames/*.png`; optional `target_mask/*.png` + `mask_map.json`.

**Per event:**
- Region R(k): the event's mask pixels on frame k when a mask exists (`mask_value` from that row),
  else `bbox_px` from that frame's row (fallback: the nearest row inside the window with a valid
  bbox; none ⇒ `NOT-MEASURABLE(no-region)`).
- Signal d(k) = mean over R(k) of |L_k − L_{k−1}| (luma, 0..1), for k ∈ [start−W, end+W], W = 4,
  clipped to the neighbouring windows.
- Baseline τ from the same region on frames outside every window ±2: median + K·MAD (K = 6,
  `K_SIGMA`), with the absolute floor `SIGNAL_FLOOR = 0.004` (≈ 0.5 % of full scale — the O4-style
  baseline). Any baseline frame whose own d exceeds τ counts as CONTAMINATED and demotes the row to
  LOW confidence with the banner (the measure_label_offset lesson: an offset larger than the clean gap
  poisons the baseline and yields a confident wrong number).
- ONSET: the first k ∈ [start−W, start+W] with d(k) > τ; n = k − start. `PASS` if n = 0, else
  `ONSET-SHIFT(±n)` (negative = the pixels changed BEFORE the label; F1 would read `ONSET-SHIFT(−1)`).
  The brief's two-sided form is asserted explicitly: d(start) > τ AND d(start−1) ≤ τ.
- END: the first k ∈ [end+1−W, end+1+W] with d(k) > τ after the plateau; n = k − (end+1);
  `END-SHIFT(±n)`.
- Toggling types (`blinking`): each contiguous run of `frame_indices` is an onset/end pair.
- NOT-VISIBLE: with masks, any positive frame whose mask count < `--min-visible-px` (default 1);
  without masks, an event whose max d over [start−W, end+W] ≤ τ (no pixel change anywhere near the
  claim — the F5/F7 shape).
- REPORTED, not gated: the CHECKER/MAGENTA classifier verdict on the region for texture types (the
  §4.2 (ii) discriminator), and the ambient (outside-region) d series so camera motion is visible.
- Ceiling: `MEASURABLE RANGE ±G//2` for the session's smallest clean gap G (G160); an offset beyond it
  is UNMEASURABLE, never a number.

**Output:** one line per event —
`idx=<i> <type> <target>  PASS | ONSET-SHIFT(±n) | END-SHIFT(±n) | NOT-VISIBLE | NOT-MEASURABLE(<why>)
[confidence] [checker=present|collapsed|n/a]` — then the session ceiling, the contamination banner
if any, and a summary count. Exit code: non-zero on any SHIFT or NOT-VISIBLE among `manifested`
events (`--gate`), 0 in `--report-only`.

**On the M2 bundle's artifacts (no masks):** it CAN conclude onset/end shifts (bbox region) and
"no pixel change inside the claimed box" (NOT-VISIBLE by pixels). It CANNOT separate a sliver from an
occluded target from an empty box, cannot judge per-frame observability inside a window (F4's
"leaves the frame at 69" reads only as a collapse of d(k)), and under camera motion τ rises with the
motion, so a fast pan reads NOT-MEASURABLE rather than PASS — reported, never a silent pass.

**Self-proof (G96/G142):** `--selftest` builds a synthetic session with a known onset, then asserts a
copy with `start_frame` moved ±1 reads `ONSET-SHIFT(∓1)`, and a copy with the target blanked reads
`NOT-VISIBLE`; the bench gate additionally runs it on a banked known-answer leg.

**Delivery gate:** a `PRE-DELIVERY-CHECKLIST` box — run it on the HOST's own smoke session (labels and
masks exist in delivery mode since m32/m43), every `manifested` event must read PASS, and the changelog
names the tool and the command.

---

## §8. T5 — the `m49` plan

### §8.1 Files (all under `Source/` unless stated; the two phases are separately gated)

**Phase A — code-only (exe hot-swap, no cook):**
- `AnomalyCapture/Private/AnomalyLabelWriter.h/.cpp` — `FCaptureSnapshot` += `TargetPixels`,
  `ConditionHeld`, `Observable` (parallel to `Fires`/`MaskValues`); row keys `target_pixels`,
  `observable`; `FSessionEvent` += `InjectedFrameIndices`, `ObservableFrameCount`; `WriteSessionAnnotation`
  emits v2 (`affected_frames` = observable subset + `span_frame_count`; `injected_frames`;
  `observable_frame_count`; root `label_schema`).
- `AnomalyCapture/Public/AnomalyCaptureSubsystem.h` + `Private/AnomalyCaptureSubsystem.cpp` — the knob
  and ini key with the G139 echo; `ServiceTargetMask` records per-session-index per-tag `Count` (extend
  `TargetMaskOutcome` from a state byte to {state, counts}); the drain join (`:3178-3206`) fills
  `TargetPixels[i]` by `MaskValues[i]`; `OnWorldTickEndSample` computes `ConditionHeld[i]` (§5);
  `AccumulateFrameEvents` gains `ObservableByIndex`; `WriteSessionAnnotationFile` builds both subsets;
  `run_summary` += `observable_frames`, `frames_condition_lost`, `observable_min_pixels`.
- `AnomalyInjector/Public/IAnomaly.h` — `virtual bool IsVisualConditionHeld() const { return
  IsCurrentlyAnomalous(); }` (precedent: `IsCurrentlyAnomalous` itself, added for m29/m30).
  `Anomaly_MissingTexture.h/.cpp`, `Anomaly_CorruptedTexture.h/.cpp` — override: every `Captured` slot
  still `IsCheckerDerived`; `AnomalyInjectorSubsystem.h/.cpp` — `IsAnomalyVisualConditionHeld(Id)`.
- T3(i): `AnomalyInjector/Private/AnomalyViewport.cpp` (+ `Public/AnomalyViewport.h`) — shared
  `IsTranslucentOnlyComponent`; picker rule + knob; `AnomalyCapture/Private/AnomalyCensus.cpp` calls the
  shared test instead of its private copy.
- m39 fold-in (§8.2): the same drain join also carries `TagResults[tag]` bounds → `bbox_drawn_px` per
  row (additive) and the event's `bbox` source; `P-C13` conjunct 2 rides it.
- `tools/verify_capture.py` (+ import of `tools/measure_label_offset.py`) — `--label-pixel-gate`,
  `--selftest` extension.
- CaptureBench (separate tree): `tools/m49_gates.py` (both orders × four types; the can-fail levers),
  `run_leg.ps1` payloads; two bench-only console levers in `AnomalyInjectorSubsystem.cpp`:
  `IAI.Bench.TeleportTargetOffscreenAt <si>` (F4 shape) and `IAI.Bench.RetakeMaterialAfter <n>`
  (F8 shape) — console-only, default off, loudly echoed, never in a client payload.
- Docs: `docs/client-readme.md` §8 (schema v2 in the client's words: `affected_frames` now = the
  observable subset; `injected_frames`; `target_pixels`/`observable`; the knob; what `-1` means),
  the mask section, `docs/client-delivery.md` (ini key, changelog, the verifier command),
  `docs/architecture.md`, `PRE-DELIVERY-CHECKLIST.md` (the verifier box), `docs/predictions/…m49…md`
  (pre-declared before any leg), AnomDash `bundle_manifest.txt` (ship `measure_label_offset.py`
  beside `verify_capture.py`).

**Phase B — the GPU drawn-count (global-shader change ⇒ full cook, G129):**
`Shaders/Private/AnomalyVisibleMask.usf` (second output), `AnomalyShaders/Public/AnomalyVisibleMaskShader.h`
(RT1 binding), `Shaders/Private/AnomalyMaskReduce.usf` + `AnomalyMaskReduceShader.h` (second table or
stride 10), `AnomalyCapture/Private/AnomalyMaskSceneViewExtension.h/.cpp`, `AnomalyMaskTypes.h`
(`DrawnCount`), the capture subsystem's condition for hide types. RT0 and the existing table are
untouched, so MASK-TIE, the PNG path, ONSET, MASK-PICTURE-PAIRING and G7 are unchanged by
construction and re-run as regression.

### §8.2 Sequencing against `m39` (honest bbox) — recommendation: FOLD IN

Both consume the same per-frame per-tag reduce table entry; m49 phase A already threads that entry
into the snapshot keyed by session index, and the honest bbox is the same join with four more ints.
Reasons to fold rather than ship m39 first: (1) `labels.jsonl` rows and `annotation.json` change
shape ONCE for the client, not twice in consecutive deliveries; (2) the verifier's region should be the
drawn box, or background dilutes d(k); (3) §4.2 (iii)'s discriminator IS the drawn bounds; (4)
`P-C13` conjunct 2's instrument (the uniform PIE pillarbox leg) runs in the same campaign. Risk: m49
grows — mitigated by slice gates (A1 observability, A2 honest bbox, B drawn-count). m39-first is
rejected on (1) alone.

### §8.3 Gates (pre-declared in the predictions file before any leg; every per-frame alignment gate
in BOTH tick orders — native and `IAI.Bench.SynthTickOrder`; packaged bench binary; both build
targets exit 0, G221)

- **Unchanged and re-run:** ONSET (m44 instrument), MASK-PICTURE-PAIRING (probe tag 255), G7
  hidden-class equality (35 = 35, stray 0), MASK-TIE 0 MISMATCH, both build targets.
- **OBS-1 bench inertness:** knob = 1, settled bench, four types × two orders:
  `injected_frames == affected_frames` on every event (delta 0), every positive row `observable=true`
  with `target_pixels > 0`; `frames_condition_lost = 0`.
- **OBS-2 can-fail, the F4 shape:** `IAI.Bench.TeleportTargetOffscreenAt <si>` mid-window — those rows
  read `observable=false`, `affected_frames` shrinks by exactly those indices, `injected_frames` does
  not; both orders.
- **OBS-3 can-fail, the F8 shape:** `IAI.Bench.RetakeMaterialAfter n` — `observable=false` from
  frame n, `frames_condition_lost > 0`, the revert's `left-to-game` counter non-zero on that event.
- **OBS-4 knob sweep:** 1 / 50 / 10,000 — only `observable`/`affected_frames` move; `injected_frames`
  and every other field byte-identical.
- **OBS-5 P-C7 v3 (re-anchored):** on a pose-matched pair (A64) against the pre-m49 binary,
  `labels.jsonl` byte-identical except the declared new keys per anomaly entry, `annotation.json`
  identical except the declared v2 keys, and **`affected_frames.frame_indices` identical value-for-
  value** (the settled bench has no frame the filter should touch). Not byte-inert, by declaration.
- **OBS-6 the verifier:** `--selftest` both directions; PASS on every event of every OBS-1 leg, both
  orders; a synthetic +1 in a copied `annotation.json` reads `ONSET-SHIFT(−1)`.
- **OBS-7 T3(i):** a `IAI.Bench.SpawnTranslucentProbe` fixture (refused on this container — the
  gate rides the host cook, as `B-G1` does) is refused by the picker with a counted log line.
- **Phase B:** `DrawnCount == Count` on every texture-type positive frame; `DrawnCount == 0` with
  `Count > 0` on every hidden-class labelled frame (the G7 shape); MASK-TIE unchanged.

**P-C7 and the knob default:** with default 1 the delivered artifacts change wherever a frame has
`target_pixels == 0` (occluded / off-screen mid-window) or the condition was lost — by design — and
everywhere by the schema keys. On a host with the mask off, Nanite targets or translucent targets
every frame is unmeasured, so under the strict rule `affected_frames` empties (§9 Q3 is the fork).

### §8.4 Client-facing doc deltas
`client-readme.md` §8: the v2 table (semantic change of `affected_frames` called out in bold; the new
keys; `target_pixels`/`observable` on rows; what `-1` and `false` mean; the knob; the verifier's
command and how to read its lines). The mask section: `target_pixels` is the count of the delivered
mask's pixels for that value, so the PNG and the number agree by construction (the MASK-TIE guarantee,
now client-visible). `client-delivery.md`: the ini key, the delivery gate, the changelog entry.

### §8.5 Effort
Phase A implementation + gates ≈ 2 sessions; verifier + selftest + known-answer ≈ 1; T3 (i)+(ii)
inside A ≈ 0.5; m39 fold-in ≈ 0.5; phase B shader + cook (≈ 40 min) + gates ≈ 1; docs ≈ 0.5.
**≈ 5–6 sessions and one cook**, the cook shared with the m46 shader that any post-`2b6c93f` delivery
already needs.

### §8.6 What in `m49` also changes Bates' upcoming delivery
Bates' next build already carries `m40`–`m48` (the P9 fix, target masks, hidden-class masks, m46
mapping, shader readiness, exposure dip) and needs the m46 cook. `m49` would add: (1) **schema v2**
(client-facing: `affected_frames` tightened, `injected_frames`, row keys) — if Bates ships before m49
the two hosts' datasets carry different schemas; (2) the verifier as a delivery gate on Bates' own
smoke session; (3) T3(i)'s selection change (a G140 boundary on Bates too); (4) phase B's shader in the
same cook. **Chat ruling to follow**; the plan's recommendation is that both hosts receive v2 in the
same delivery window so no client ever holds two schemas, with the verifier shipped in both.

---

## §9. Open questions for chat (numbered) and the one fork

1. **F1's selection route:** targeted or auto-pool? Decided by the log's `Capture run STARTED …
   mode=` line or `nodes[0].component_class` (§4.3). It also decides whether the session's events are
   comparable to any auto-pool run (G140).
2. **The frame-numbering question (§3.6 (a)):** which artifact did the client scrub — the MP4 or the
   PNGs — and is their viewer 0- or 1-based? One question to the client; the verifier answers it
   from the PNGs regardless.
3. **The unmeasured fork:** the brief defines `observable = false` when `target_pixels = -1`. On a
   mask-blind host (mask off, Nanite, translucent) that empties `affected_frames` for every event.
   Alternative: `observable: true | false | null`, with `affected_frames` = `true` ∪ (`null` AND the
   CPU condition held) and `unmeasured_frame_count` beside `observable_frame_count`, so a mask-blind
   host keeps today's labels and says how much of them is measured. Recommendation: the alternative;
   the strict rule is the honest one only where the mask is guaranteed. **Chat decides.**
4. **Veto vs empty observable set** (§4.4): keep the event with an empty observable subset
   (recommended) or delete it.
5. **T3(i) scope:** all types (recommended) or texture-swap types only.
6. **Root schema key** name (`label_schema: 2`) and whether the bundle's README gets a v1→v2 diff.
7. **Bates**: same delivery window as Concorde for v2, or v1 to Bates now (§8.6).
8. **F5/F7's `mask.provided`** and `run_summary.mask_nopass_discards` from the client's artifacts —
   the reading that says whether the count-based mechanism even applies to those two events.

**NEEDS-DECISION (product/scope only):** none that blocks the plan. Q3 is the one design fork that
changes what a client with a mask-blind host receives; the plan can be implemented either way and is
written for the recommended alternative with the strict rule as a one-line switch.

---

## §10. Bookkeeping

- Worktrees created: `D:\IntrusiveAnomalies\_worktrees_071\master` (branch `master`, `914f14b`) and
  `D:\IntrusiveAnomalies\_worktrees_071\bundle-2b6c93f` (detached, `2b6c93f`). The journal was committed
  from the master worktree with a path-scoped add. Both worktrees are removed at the end of the
  session (`git worktree remove`), and the removal is stated in the final report.
- No other file was touched. The main checkout stayed on `europa-e1` throughout.
- The eight untracked `docs/CHAT-HANDOFF-*.md` files in the main checkout were never staged.
