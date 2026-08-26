# 2026-08-26 — session 061 — m35: the plugin-owned sub-rect copy (COLD-START HANDOFF)

> **THIS FILE IS SELF-CONTAINED.** If you are a fresh session picking up m35, read this file, then
> `docs/predictions/2026-08-26-m35-build-b-gates.md` (the pre-declared gates + every result), then
> `docs/predictions/2026-08-26-m35-readback-layout-build-a.md` (Build A). You do not need the chat
> transcript. Everything below is either measured or explicitly labelled as not.

---

## 1. YOU ARE HERE, in one paragraph

**m35 is a hotfix for a capture-readback crash observed on a SECOND HOST** — "Bates", a Callisto-fork
UE 5.1 game, letterboxed picture, `Rect.Min.Y = 104`, view rect 1389×581, `fmt = PF_A2B10G10R10`: an
access violation reading past a mapped readback buffer at a non-zero view-rect origin. **The fix is
BUILT, STAGED, and GREEN on every gate that has run — and it is still UNCOMMITTED** (8 files in the
working tree of branch `feature/mask-gpu-reduce`). **No leg has failed.** The remaining work is
gate-building and gate-running, not fixing. **Master is untouched and STILL CARRIES THE CRASH.**

⛔ **NO TAG.** m35 joins the office-pass tag sequence after m34. Highest tag remains **m30**.

---

## 2. THE DEFECT, AND WHY THE FIRST DESIGN DIED

The original brief was: measure the engine's readback staging layout, then remove a `Rect.Min.Y`
offset from the drain indexing. **It had an explicit STOP CONDITION, and the stop condition FIRED.**

Measured from engine source AND from a Build A leg on this box:

| | sourceExtent | rect | picture | **bufferHeight** | rowPitch | fmt |
|---|---|---|---|---|---|---|
| letterboxed | 821×869 | (0,263)-(821,607) | 821×344 | **869** | 832 | 18 |
| un-letterboxed | 821×869 | (0,0)-(821,869) | 821×869 | **869** | 832 | 18 |

`bufferHeight 869 == sourceExtent.y 869` on **both** legs ⇒ **stock UE 5.1 allocates FULL-SOURCE-SIZE
staging and copies the sub-rect to its own position.** The layout changed at **UE 5.2** (rect-sized
staging, dest 0,0), and 5.3 keeps a `FResolveRect` compat overload so call sites compile unchanged on
5.1/5.2/5.3. ⇒ **the unconditional offset removal is refuted by measurement AND by source**, and
Build A's pre-m35 indexing is CORRECT here — which is exactly what makes its frames a valid reference.

⛔ **A `bufferHeight`/pitch SNIFF was also REJECTED, and the rejection is MEASURED, not argued:**
`rowPitch 832` vs `width 821` = **11 px of padding** (832×4 = 3328 = 13×256, D3D12 256-byte
alignment). At a pillarbox narrower than the padding the two engine layouts become **numerically
indistinguishable**, so the sniff has a physical blind spot and **fails silently inside it**.

### THE DESIGN THAT SHIPPED (owner-ruled): make the layout IRRELEVANT BY CONSTRUCTION

1. Copy the view sub-rect into a **plugin-owned W×H texture at (0,0)**.
2. Enqueue a **whole-texture** readback — **no rect argument at all**.
3. Drain indexes **sub-rect-locally**: `SrcRow = Base + (int64)y * RowPitchInPixels * BPP`.

⇒ We own the texture, so `bufferHeight == picture height` on **every engine**, and no engine's staging
choice can reach us. ⚠ **This REMOVES the discriminator that a Bates/Deimos `READBACK-LAYOUT` photo
used to provide** — see the Build B gate file §4 for what the photo decides now.

---

## 3. THE UNCOMMITTED WORK — WHAT IT IS AND HOW TO RECOVER IT

Branch **`feature/mask-gpu-reduce`** @ **`aefa971`** (pushed). Master tip **`9f52cab`**, untouched.

```
 Source/AnomalyCapture/Private/AnomalyFrameCapturer.cpp          | 146 ++++++++--
 Source/AnomalyCapture/Private/AnomalyFrameCapturer.h            |  11 +
 Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp  |   4 +-
 Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.h    |   3 +
 Source/AnomalyCapture/Private/AnomalySceneViewExtension.cpp      |  31 ++-
 Source/AnomalyCapture/Private/AnomalySceneViewExtension.h        |   1 +
 Source/AnomalyCapture/Private/AnomalySveCapturer.cpp             |  10 +-
 Source/AnomalyCapture/Private/AnomalySveCapturer.h               |   2 +
 8 files changed, 192 insertions(+), 16 deletions(-)
```

**INSURANCE COPY (regenerated 2026-08-26 at handoff time):**
`D:\IntrusiveAnomalies\_binary_baselines\m35-buildb-uncommitted-2026-08-26.diff`
size **18,756 B**, `sha256 7A0CC269AD34CDDC733F2D0B61A785EC1B22CD4E417558CFF46206A8C774A426`
⚠ An earlier hash (`8479FFE7…`, 18,374 B) was recorded for the same 8-file change; the diffstat is
identical at 192/16, so the CODE is the same one — the byte delta is a redirection-encoding
difference and **that cause is not established.** Trust the hash above.

**Recovery if the working tree is ever lost:** `git -C <plugin> apply <that .diff>`.
📌 The files are plain on-disk sources; the diff is insurance, not the primary copy.

### What each file does now (so you can review without re-deriving)

- **`AnomalyFrameCapturer.h`** — shared types for BOTH paths: `FAnomalyReadbackLayout` (`bValid`,
  `SourceExtent`, `Rect`, `W`, `H`, `BufferHeight`, `RowPitchInPixels`, `Format`) and namespace
  `AnomalyReadback` with `NoteLayoutOnce(...)` + `CheckDrainBounds(...)`. `FAnomalyFrameCapturer`
  gains `GuardDrops`, `ClampDrops`, `FTextureRHIRef OwnSubRect`, `OwnSize`, `OwnFormat`,
  `LayoutPathName`.
- **`AnomalyFrameCapturer.cpp`** — the bench cvar `IAI.Bench.ReadbackGuardInflate` (guard
  proof-by-breaking), the shared bounds guard, and the **backbuffer owned-copy path**: a loud clamp
  (drop if the granted rect ≠ requested), a persistent owned texture recreated on size/format change
  with `.SetFlags(ETextureCreateFlags::ShaderResource).SetInitialState(ERHIAccess::CopyDest)`, an
  explicit `CopyTexture` with `SourcePosition = Rect.Min` / `DestPosition = 0`, transitions
  CopyDest→CopySrc→CopyDest, then `Readback->EnqueueCopy(RHICmdList, OwnSubRect.GetReference())`
  **with no rect**.
- **`AnomalySceneViewExtension.cpp`** — the SVE path. Extent-clamp early-out (drops the frame, never
  captures a clamped region), then an RDG texture created
  `TexCreate_ShaderResource | TexCreate_RenderTargetable`, `AddCopyTexturePass(..., Rect.Min, 0, {W,H})`,
  `AddEnqueueCopyPass(GraphBuilder, Readback.Get(), OwnTexture)` — **no rect** — and
  `SubmitInFlight_RenderThread`. 🚨 **`FinalizeSveAfterPassOutput` is preserved on EVERY return path,
  including both new early-outs** — that function came from `b05066f` (the stale-present fix), NOT from
  m34, and `G-M4` exists to prove m35 has not disturbed it.
- **`AnomalyMaskSceneViewExtension.*`** — the same bounds guard **and nothing else**; a guard failure
  there lands in the existing `NOT_MEASURED` ⇒ **ADMIT** direction, so the m26 safety property is
  unmoved by construction. Header now includes `AnomalyFrameCapturer.h`.
- **`AnomalySveCapturer.*`** — carries `SourceExtent` / `Format` through the in-flight record.

**ALSO on the branch, already committed:** `9aec10f` = Build A (readback-layout telemetry +
`IAI.Bench.Letterbox` lever), `aefa971` = the Build B gate file. `AnomalyCaptureLetterbox.cpp` is the
lever; it refuses out-of-range aspects and **warns that `minY == 0` means the lever is a no-op**.

---

## 4. ENVIRONMENT / BUILD IDENTITY (`G121` — identity is exe hash **plus** container)

| | value |
|---|---|
| staged exe | **`733FE83C`** = Build B, archived as `_binary_baselines\StackOBot.exe.m35-buildb-733FE83C` |
| predecessor exe | **`7F37A4AC`** = `_binary_baselines\StackOBot.exe.m34-fix-candidate-7F37A4AC` (hash-verified BEFORE the swap) — **this is `G-M6`'s A-side** |
| container | **UNCHANGED** m34 quartet: utoc `2A66CA57` · ucas `A7EF9B12` · pak `D8009AD7` |
| cook | **NONE this session** — code-only hot-swap (`G103`) |
| A44 | green on the **staged** artifact, both encodings, including m34's own tokens as a positive control |

⛔ Do NOT cook. m35 rides the m34 container; the four-item cook is the office/Concorde pass.

---

## 5. WHAT PASSED, WITH NUMBERS (full ledger in the gate file §12)

- **`G-M1` letterboxed:** exactly ONE field moved — `buffer_height: 869 → 344`. That is **the fix
  working** (we own the texture, so the buffer is picture-sized by construction). Frames 821×344.
- **`G-M2` un-letterboxed:** **NOTHING moved.** This is the leg that proves the shipped path did not
  move.
- **`G-M3` guard proven BOTH ways (`G96`):** inflate 1 ⇒ 90 `READBACK-GUARD FIRED`, 0 frames on disk,
  `total_frames 0`, **no crash and the run completed and wrote its artifacts**; inflate 0 ⇒ silent.
- **`G-M4` the display fix reproduces:** `A-I1` **29/29 `overrideOutput=1`, zero at 0**; `G-F2`
  **29 IDENTICAL / 0 FIRST-DIFF**; all 8 events inside the known band with **both endpoints (66,843
  and 66,878) hit exactly**. ⇒ m35 has NOT undone `b05066f`.
- **`G-M5` (3 of 5 legs):** `SM_Ramp2` NOT_MEASURED ×8 / MEASURED_ZERO **0** / `vetoed 0`;
  `BP_SplineSpawn_C` MEASURED_ZERO ×8 / 8 `VETOED-OBJECT` / `vetoed_events 8` / annotation anomalies
  **0**; both plus CTRL49 gave m34's `COMPARE` **29 IDENTICAL / 0 FIRST-DIFF** across **two maps**.
- **`P6` unmoved:** `annotation.json` key set 48, measured against a banked baseline.
  `run_summary` gains `readback_layout` and nothing else.
- Guard 0 / clamp 0 on every non-guard leg.

**Answered, not owed:** `G-M6`'s instrument question (§10 of the gate file), the backbuffer format
question (§11), and whether the lever bites packaged (§13 — **it does not**).

---

## 6. WHAT IS OUTSTANDING — IN THE OWNER'S STATED ORDER

The owner fixed this sequence; **do not reorder it**:

1. **`G-M5`'s remaining two legs** — `StaticMeshActor_73` and the `IAI.Capture.MaskProbe` leg.
2. **`G-M6` fine prior via the archived-exe swap.** ⛔ **Do NOT rebuild and do NOT ask Kavin.** Swap
   back to `7F37A4AC`, run **pacing OFF**; swap forward to `733FE83C`, run **pacing OFF**; same
   map/seed/config, **order-matched**; **re-verify hashes at every swap**. Report **per-captured-frame
   milliseconds AND per-megapixel**; any Concorde (3200×2000) figure is an **EXTRAPOLATION** and must
   be labelled one. **No threshold.**
3. **Build and self-prove `G-M9`** (gate file §8). Premise (a) is already verified; (b) prove it can
   fail, (c) prove cvar-OFF reproduces Build B.
4. **The column checker for `G-M8`**, validated against the known-answer datum FIRST.
5. **`G-M7` / `G-M8` / `G-M9` both origins** — **PIE on MainWorld** (packaged refuses, §13).
6. **Fix commit + push, NO TAG.** Then: journal, CLAUDE.md status refresh, the revised office-pass
   list, and the gotcha list.

**Docs still owed at close:** CLAUDE.md's Current-status block (standing convention: refreshed at
every milestone close, as its own `docs:` commit) and `docs/architecture.md` if the shipped shape
changes.

---

## 7. STANDING CONSTRAINTS — OWNER RULINGS, VERBATIM WHERE THEY WERE GIVEN

- **"Do NOT checkout master. The checkout lift I authorised last turn is WITHDRAWN"** — never
  exercised. m35 develops, tests, commits and lands **on `feature/mask-gpu-reduce`**.
- **"ONE ROUTE ONLY: m35 must not ALSO be cherry-picked onto master in parallel."** Master reaches the
  fix by the branch **MERGE**. (Fallback cost if `G-R7(ii)` fails: m34 gate file §A2.4.)
- **"Stop rule unchanged: any leg fails, report and stop, do not fix in the same turn."**
- **"EVERY CHECKER IS PROVEN AGAINST A KNOWN ANSWER BEFORE ITS VERDICT IS READ."**
- **"chat never states what a gate or artifact measures without Code quoting the gate file first."**
- No ratio, no threshold, anywhere (journal §209). `P6` does not move.
  `feature/stencil-capture` untouched. No force-push. Source carries **no comments** — run
  `python _strip_comments.py <repo-root>` before every commit.
- Role ruling: **this box is the only canonical author**; the office instance is eyes/builder/runner
  and commits nothing.

---

## 8. LESSONS THIS SESSION ESTABLISHED, WITH RECEIPTS

*(The canonical numbered gotcha list is maintained chat-side; these are the ones Code established
with evidence, so they survive without it.)*

1. **A crash is not a gate failure of a design** — Build B's first leg died on
   `State != D3D12_RESOURCE_STATE_COMMON` because the D3D12 **transient** allocator derives an initial
   state only from RT/DS/UAV flags. One flag, not a redesign.
2. **The guard leg writes ZERO frames and that is the PASS** — 0 frames *with* 90 drops is a pass;
   0 frames *without* drops is a fail. Read the drops, not the emptiness.
3. **`bufferHeight` reading 344 instead of 869 is the fix WORKING, not a different engine.** Written
   into the gate file *before* the leg, because it is exactly the misreading that would fire.
4. **Strict cross-run frame byte-identity is unobtainable in every environment measured** — even a
   same-binary control pair differs (mean |Δ| 0.00117, 0.116 % of pixels). `NOT OBTAINED`, never
   `FAILED`. The within-frame comparator (`G-M9`) is the sound replacement.
5. **The pacer masks hook cost.** Median `t_wall` delta **0.03334 on all five paced legs** — a paced
   leg cannot measure a sub-budget hook. Pacing OFF or no measurement.
6. **A refusal path is worth more than a lever that silently no-ops** — the packaged letterbox leg
   named `SpectatorPawn` instead of quietly producing an un-letterboxed frame that would have read as
   a letterboxed PASS.
7. **A row-only checker is blind to a horizontal defect.** Hence `G-M8`'s column checker, and hence
   validating it against a known answer first.
8. **My pose checker was VACUOUS** — it read `annotation["camera"]`, which lives under `anomalies[]`,
   got `None == None`, and printed "MATCHED". Corrected to `labels.jsonl view.rot/origin`. This is the
   origin of the standing "every checker proven against a known answer" rule.
9. **`max_frames` does not exist; the key is `frame_cap`.** My first probe read a non-existent key and
   reported empty. **The checker was wrong, not the build** — the same shape as `G161`/`G142`.
10. **A bare class-name target matches nothing.** `SM_Ramp2` gave `zero_match_bursts=8`, 0 events ⇒
    leg **INVALID**, not failed. The replayable UAID tokens exist **only inside banked `run.json`
    files** and are now recorded in the gate file §12.
11. **Attribution must be per-file.** I reported "m34 touched four files"; per-file `git log` showed
    `AnomalySceneViewExtension.cpp` was touched by **`b05066f`**, the stale-present fix. m34 never
    touched it. Recorded as A2.1 in the m34 gate file.
12. **"Per armed frame" ≠ "per captured frame."** The mask arms ~29× per leg; the colour readback arms
    **90** — once per captured frame. Corrected in both gate files (§6 and A2.5).
13. **An RHI check that exists can still be absent.** `ValidateCopyTexture`'s format `checkf` is real
    and is compiled out by default (`ENABLE_RHI_VALIDATION`), so the backbuffer path has no assert
    *and* no graceful failure. Structural guarantees are the whole protection there.
14. **RDG does not reorder passes** — verified before building `G-M9`, because the comparator's
    soundness rests on it. A premise checked before the instrument is built, not after it disagrees.

---

## 9. HOW TO RUN A LEG (do not reconstruct these)

- **Packaged-leg recipe:** `docs/setup-runbook.md` **§8.6** (STEP 0 disk floor and STEP 3.5 "rebuild
  the EDITOR target" are **NOT optional** — `G47`/`G131`).
- **A63 harness:** `CaptureBench/tools/run_leg.ps1` — banks every attempt, including discards.
- **Pose gate:** `CaptureBench/tools/check_pose.py` — reporting only; it prints the discriminator and
  never attributes a cause.
- **Bench cvars reachable packaged:** `IAI.Capture.SVE`, `IAI.Bench.ReadbackGuardInflate`,
  `IAI.Capture.MaskReduce`. **NOT reachable packaged:** `IAI.Bench.Letterbox` (needs a camera-bearing
  view target ⇒ PIE/MainWorld).
- Legs are banked under `D:\IntrusiveAnomalies\_bench_sessions_bank\M35_*`.

---

## 10. THINGS THAT WILL BITE A FRESH SESSION HERE

- 🚨 **`git` via the Bash tool hung in this session** (the tool's cwd file went missing;
  `D:\IntrusiveAnomalies\StackOBot` is not itself a repo). **Use PowerShell with `git -C <plugin>
  --no-pager`.**
- 🚨 **Do NOT edit tracked docs through PowerShell redirection** (`G115`/`G141`) — it re-encodes every
  non-ASCII line and adds a BOM while the text still *reads* correctly. Use the editor tool, and read
  `git diff --stat` before every commit: a diffstat far larger than the intended change **halts** the
  commit.
- **`P6` does not move**, and it is *measured* against a banked baseline, never asserted.
- **`NOT_MEASURED` ⇒ ADMIT; `MEASURED_ZERO` ⇒ veto.** No ratio, no threshold. The mask guard can only
  push toward `NOT_MEASURED`, which is the safe direction by construction.
- **Not-crashing is not a pass condition anywhere in the gate file.**
- The bench pawn under `-unattended` is a **SpectatorPawn**; the PIE MainWorld view target is
  `BP_Bot_C_0 / FollowCamera`.
