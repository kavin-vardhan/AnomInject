# 2026-08-18 — 039 — S3a-2 fixed and re-gated in full

**All gates GREEN.** The fix landed on a fresh branch off `master`, re-applying the wiring from
`s3a-2-GATE-FAILED-do-not-merge` (which stays unmerged, as evidence).

---

## 1. FIX 1 — the destructive branch restructured so it cannot be stolen again

Not "move the inserted block back". `FinishRun`'s finish logic now contains **no `else` at all**:

```cpp
const bool bWroteSession = bRunBegun;

if (bWroteSession)  { ...drain, pacing, annotation, run_summary...  "FINISHED" }

if (!bWroteSession) { IFileManager::Get().DeleteDirectory(*RunDir, false, true);  "CANCELLED" }

if (Async.IsValid() && Async->SveCapturer.IsValid()) { Async->SveCapturer->SetActive(false); }

bRunBegun = false; bRunning = false; Phase = ECapturePhase::Idle; ...
```

Two independent `if` blocks over a captured `bWroteSession`. **A future append after either block
cannot inherit a destructive branch, because there is no `else` token to inherit.** A true early
`return` was rejected: `FinishRun` has a mandatory tail (lifecycle reset, fixed-timestep restore,
overlay restore, auto-injector resume) that an early return would skip.

The SVE teardown now sits **with the lifecycle reset**, which is where run-lifecycle state belongs —
the placement observation from journal 038, applied.

## 2. FIX 2 — **BLOCKED. Reported, not worked around.**

Clearing `RunDir` at the end of `FinishRun` **breaks a live consumer**:

`AnomalyControlServerSubsystem.cpp:625-643` — the `capture_stop` handler calls `Cap->StopRun()` and
**then** `Cap->GetStatus(bRun, Frames, RunDir, Seed)`, shipping `runDir` in the `capture_stopped`
reply to the dashboard. `capture_status` (`:658-676`) and the snapshot (`ControlSnapshot.cpp:226-238`)
read it the same way after a run ends.

Clearing it would send an **empty `runDir`** where the dashboard currently receives the finished
session's path. Per the standing instruction — *if clearing breaks anything that reads `RunDir` after
`FinishRun`, stop and report, do not work around it* — **FIX 2 is not implemented**, and no
substitute field was introduced.

**The latent hazard therefore remains exactly as characterised in journal 038:** unreachable today
(all four `FinishRun` callers are guarded), and one unguarded future call away from deleting a
*previous* session. FIX 1 does **not** remove it — a second call with `bRunBegun == false` would still
reach the delete. Chat-side call: change the contract (`GetStatus` returns a `LastRunDir`), or accept
and document.

## 3. FIX 3 — the wiring, re-applied unchanged

Cherry-picked from the failed branch, not rewritten: `IAI.Capture.SVE` (default 0, mid-run guarded,
GConfig `bSveCaptureDefault`), the StartRun/CaptureCurrentFrame/ProcessCompletedFrames/
DrainAsyncToCompletion/FinishRun wiring, **intermittent ForceMiss** (`0` off / `1` every key / `N>1`
every Nth), the `Corrupted` counter, the C3 resolution log, and the A48 effective-value echo.

---

## 4. THE RE-GATE — every step, on the artifact

Binaries: baseline legs on the A44-verified **m23** staged exe; fix legs on the hot-swapped **S3a**
exe (**A44 confirmed after the swap**: `IAI.Capture.SVE` ×8, `bSveCaptureDefault` ×2,
`IAI.Capture.SVE.ForceMiss` ×1). All legs identical config: `K=2 pre=4 positive=8 post=4 bursts=0`,
seed 777, cap 90, png, targeted `blinking` on `StaticMeshActor_49`, 1280×720, 30 fps, paced.

### G-S3a-1 (amended) — **PASS**

**STEP 1 — control pair, two m23 runs. The run-unique field set, measured not assumed: 54 fields.**

| surface | fields that vary run-to-run |
|---|---|
| `annotation.json` | `camera.rotation[0..1]`, `coverage_pct`, `coverage_ratio` (per event), `video.path` |
| `run_summary.json` | `speed_ratio`, `sustained_wall_fps` |
| `run.json` | `session_id`, `start_time_utc` |
| `labels.jsonl` | `t`, `t_wall`, `view.rot[0..1]`, `bbox_norm[*]`, `bbox_px[*]`, `bbox_valid`, `visible_positive`, `frame_index`, `session_index`, `image` |
| `Actual_Frames` | **all 90 of 90 images differ byte-wise** |

⚠ **This is A47 caught live, and it is the finding of the step.** Leg 1 recorded the camera
mid-settle (`rotation` ≈ 329.9 / 39.8); leg 2 was at rest (`0 / 0`). Everything downstream of camera
pose — coverage, bboxes, `visible_positive`, and every rendered pixel — moves with it.
**Consequence: byte identity of frames is NOT available as an identity check between two runs of the
same binary**, so C1's original "byte-identical" formulation was unsatisfiable in an even stronger
sense than first reported.

**STEP 2 — subset test, decided by rule.** m23 vs S3a-OFF difference set: **54 fields.**
**Fields differing in the test pair but NOT in the control pair: 0. → PASS.**

**Invariant core, asserted explicitly and all IDENTICAL:** event count (8); `frame_indices` for all
events; `manifested` for all events; `anomaly_type`/`anomaly_subtype`; the `video` block excluding
`path`; and `run_summary` `total_frames` / `positive_frames` / `bursts_done` / `zero_match_bursts` /
`end_frame` / `target_fps` / `stamped_fps` / `paced` / `delivery_mode` / `content_clock` /
`non_manifested_events`. *(`end_frame` identical across runs ⇒ engine startup is frame-deterministic
here.)*

**STEP 3 — frame identity.** 90 = 90 files, names equal, cadence `[[4,5,9,10] … [88,89]]` identical.
⚠ **Honest limit:** the marker was OFF on these legs, so the decoded-marker ↔ `frame_index` check was
**not** performed; and byte comparison is unusable per step 1. Count + names + cadence + the label
index series are what this step actually establishes.

**STEP 4 — A62, verified on disk after process exit.** Session directory present, **90 frames on
disk**, all five metadata files present, and **no `CANCELLED` line**. This is the step the previous
turn failed.

### G-S3a-3 — **PASS** (first cross-thread execution of B′, ever)

`grab point EFFECTIVE = SVE/scene-colour (UI-free) (sve=1, sveCapturer=1, sveExtension=1,
backbufferCapturer=0)`. 90 frames, 90 label rows, 8 events, cadence **byte-exact to the canonical
`[[4,5,9,10] … [88,89]]`**, `manifested` 8/8, `non_manifested_events` 0, ratio 1.0000006.

**Key ring: published 121 · consumed 121 · missed 0 · wrapped 57 · corrupted 0.** The game thread
published and the render thread recovered every key — **the property B′ exists for, executed for the
first time.**

**C3 — resolution delta, both numbers reported:** **SVE view rect 1280×720 vs backbuffer window rect
1280×720 → dW = 0, dH = 0.** No delta at this configuration. Not generalisable to windowed/DPI-scaled
or letterboxed configurations, where the view rect and the window rect can diverge.

### G-S3a-2 — three readings, gated on the artifact — **PASS**

| reading | ring counters | frames on disk | verdict |
|---|---|---|---|
| `ForceMiss 0` | pub 121 · missed **0** · corrupted 0 | **90** | guard **sleeps** |
| `ForceMiss 1` | pub 130 · consumed 0 · missed **130** · corrupted 130 | **0** | guard **fires**, total |
| `ForceMiss 4` | pub 2228 · consumed 1671 · missed **557** · corrupted **557** | **68** | guard **fires**, partial |

`ForceMiss 4` arithmetic is exact: 557/2228 = **25.0 %** corrupted, and **missed == corrupted**, so
every corrupted key missed and **no uncorrupted key did**.

**The discrimination half, on the artifact:** for the 68 survivors — 68 files ↔ 68 label rows, **1:1
in both directions**; `session_index` equals the image number on **every** row; `video.total_frames`
= 68; the annotation's `frame_indices` are the **byte-exact canonical cadence**; and **every claimed
index exists as a file on disk**. The guard drops only what it must and **does not poison the
survivors**.

`ForceMiss 1` writes a session with **0 frames, 0 label rows and 0 events** — total key loss means
total event loss, because events accumulate only as frames resolve. Nothing is labelled by guess,
which is the required behaviour.

---

## 5. Two observations that limit what the ForceMiss proof shows — reported, not smoothed over

1. **`ForceMiss N` is PERIODIC and the capture cadence is PERIODIC, so they can PHASE-LOCK.** In the
   `ForceMiss 4` leg the 22 dropped indices were
   `[1,7,11,13,19,23,25,31,35,37,43,47,49,55,59,61,67,71,73,79,83,85]` and the 30 claimed positives
   were `[4,5,9,10,16,17,…,88,89]` — **overlap: zero**. That is not luck (≈0.02 % by chance); the
   every-4th-key corruption never landed on a positive frame. **So reading 3 demonstrates that
   survivors are correctly labelled, but it did NOT exercise a dropped POSITIVE frame**, which is the
   more interesting discrimination case. A randomised or phase-offset corruption mode would be needed
   to reach it. Recorded as a limit of the instrument, not a pass.
2. **Under partial frame loss `video.total_frames` and the index range disagree:** the `ForceMiss 4`
   annotation reports `total_frames: 68` while `frame_indices` reference up to **89** and the files on
   disk run 0…89 with gaps. A consumer treating `total_frames` as an index bound would be wrong. It
   only arises when frames are lost — in production that means the loud-miss guard has already fired —
   but the artifact is self-inconsistent in that state.

**Also noted, cosmetic, not fixed:** the run-`STARTED` banner still prints `capture=async/backbuffer`
on an SVE run — it reports `bAsyncCapture`, not the grab point. The new `grab point EFFECTIVE` line is
authoritative; the banner should be reworded in a later slice.

**A near-miss on my own provenance check, recorded because A44 is only as good as the scan:** an
intermediate A44 scan reported **0** for every SVE symbol on the freshly staged binary. The cause was
mine — that scan checked only the **ASCII** decoding and these strings are stored as **UTF-16**. The
correct scan reads `ascii=0 utf16=8`. A single-encoding scan is a **false negative generator**, and it
would have read as "the change did not reach the package". Every A44 scan must decode **both**.

---

## 6. State

- `master` — S3a-2 merged, gates green. `CaptureBench` `8dad64e`, probe untouched.
- `s3a-2-GATE-FAILED-do-not-merge` (`087f4d9`) — **stays unmerged, keeps its name. It is evidence.**
- Staged package — carries the **S3a** binary (certified; switch defaults OFF and is
  subset-identical). The pre-S3 binary is preserved beside it as
  `StackOBot.exe.m23-baseline`.
- Bank — **29 dirs**; five new: `S3A2_BASE2` (control pair half 2), `S3A2_FIX_OFF`, `S3A2_FIX_ON`,
  `S3A2_FM1`, `S3A2_FM4`.
- **Next: S3a-3** — `run_summary` `capture_path` + ring counters, emitted **only when the switch is
  ON** (C1). Not started.
