# 2026-08-26 — session 061 — m35: the plugin-owned sub-rect copy (COLD-START HANDOFF)

> **THIS FILE IS SELF-CONTAINED.** Written for a reader with zero context. Companion files:
> `docs/predictions/2026-08-26-m35-build-b-gates.md` (pre-declared gates **and** the results ledger)
> and `docs/predictions/2026-08-26-m35-readback-layout-build-a.md` (Build A). You do not need the chat
> transcript for anything below. Everything is either measured or explicitly labelled as not.

---

## 0. ⛔ DO NOT DO THIS — read before you run a single command

1. 🆕 **THE `WIP` COMMIT IS PUSHED. THE DO-NOT-PUSH RULE IS LIFTED (owner ruling, 2026-08-26).** The
   branch is **fully published — `origin` == local, nothing withheld** — so the office can pull and
   **START REBUILD-AND-COOK IMMEDIATELY**. That was the only thing this rule was holding up, and it is
   the longest wall-clock item.
   ⛔ **COOKABLE IS NOT MERGEABLE AND NOT TAGGABLE.** `master` is **not** merged (still `9f52cab`,
   still carrying the crash) and **nothing is tagged** (highest is still `m30`). The merge remains
   gated on `G-R7(ii)`; the tag order is still `m31` → `m33` → `m34` → `m35`.
   🚨 **THE WIP CAN NO LONGER BE AMENDED.** If a remaining gate forces a change it lands as a
   **FOLLOW-UP COMMIT** ⇒ **m35 is two commits**, deliberately overriding one-milestone-one-commit,
   and **the tag goes on the FINAL commit**. ⛔ No re-parenting. No force-push. Ever.
   📌 Because the amend path is closed, **this commit's SHA is now stable and may be quoted.** The
   de-SHA rule elsewhere in these docs applies to *amendable* commits and still stands for any future
   one (`G185`); it has simply lapsed for this one.
   📎 `wip/session-061-backup` stays on origin until the fix lands. **The remote is the backup now** —
   see §3: the insurance diff is no longer protection.
2. **DO NOT `stash`, `clean` or `checkout` WITHOUT READING THE `WIP` COMMIT FIRST.** That commit *is*
   the fix. It was made specifically because a cold session's reflexive `git stash` would have
   destroyed 8 files of uncommitted work.
3. **DO NOT DELETE `_binary_baselines\StackOBot.exe.m34-fix-candidate-7F37A4AC`.** It is `G-M6`'s
   **A-side** and cannot be rebuilt from the current tree.
4. **DO NOT CUT A DELIVERY BUILD FROM `master`.** Master is untouched at `9f52cab` and **still
   carries the crash**. m35 reaches master by the **MERGE — one route only**, never also by
   cherry-pick.
5. **DO NOT RE-DERIVE THE ENGINE LAYOUT QUESTION. IT IS SETTLED AND MEASURED** (§2). Two designs are
   dead: the unconditional `Rect.Min.Y` offset removal (its stop condition fired) and the
   `bufferHeight`/pitch sniff (measured blind spot).
6. **DO NOT USE `MainWorld` FOR ANY FRAME COMPARISON.** Its cross-run noise floor is mean |Δ| **4.42**
   over **78 %** of pixels (§8). Use `CB_GateLevel`, and even there byte-identity is unobtainable.
7. **DO NOT READ A BARE-NAME TARGET LEG AS A PASS.** `zero_match_bursts = 8` with 0 events is
   **INVALID, not a pass and not a failure** (§6, §7).
8. **DO NOT TRUST ANY CHECKER BEFORE PROVING IT AGAINST A KNOWN ANSWER.** Standing owner rule. It was
   minted because a pose checker of mine printed **"MATCHED"** while comparing `None` to `None`.
9. **DO NOT `git add -A` IN THIS REPO** — it sweeps the owner's two untracked
   `docs/CHAT-HANDOFF-*.md`. Path-scope every add.
10. **NOT-CRASHING IS NOT A PASS CONDITION** anywhere in the gate file.

---

## 1. YOU ARE HERE, in one paragraph

**m35 is a hotfix for a capture-readback crash observed on a SECOND HOST** — "Bates", a Bates-lineage
UE 5.1 game: an access violation reading past a mapped readback buffer at a **non-zero view-rect
origin**. Letterboxed picture, `Rect.Min.Y = 104`, view rect 1389×581, `fmt = PF_A2B10G10R10`.
**The fix is BUILT, STAGED, GREEN on every gate that has run, and is committed LOCALLY as a `WIP`
commit that must not be pushed.** **No leg has failed.** The remaining work is gate-*building* and
gate-*running*, not fixing. ⛔ **NO TAG** — m35 joins the office-pass tag sequence after m34; highest
tag remains **m30**.

---

## 2. THE DEFECT, AND THE TWO DEAD DESIGNS

The original brief was: measure the engine's readback staging layout, then remove a `Rect.Min.Y`
offset from the drain indexing. **It carried an explicit STOP CONDITION, and the stop condition
FIRED.** Measured from engine source AND from a Build A leg on this box:

| | sourceExtent | rect | picture | **bufferHeight** | rowPitch | fmt |
|---|---|---|---|---|---|---|
| letterboxed | 821×869 | (0,263)-(821,607) | 821×344 | **869** | 832 | 18 |
| un-letterboxed | 821×869 | (0,0)-(821,869) | 821×869 | **869** | 832 | 18 |

`bufferHeight 869 == sourceExtent.y 869` on **both** legs ⇒ **stock UE 5.1 allocates FULL-SOURCE-SIZE
staging and copies the sub-rect to its own position.** The layout changed at **UE 5.2** (rect-sized
staging, dest 0,0), and **5.3 keeps a `FResolveRect` compat overload, so call sites compile unchanged
on 5.1/5.2/5.3** — compiling on all three is therefore **not** evidence the semantics match.
⇒ the unconditional offset removal is refuted by measurement **and** by source, and Build A's pre-m35
indexing is CORRECT here — which is exactly what makes its frames a valid reference.

**DEAD DESIGN 2 — the `bufferHeight`/pitch sniff, rejected by MEASUREMENT not argument.**
`rowPitch 832` vs `width 821` = **11 px of padding** (832 × 4 = 3328 = 13 × 256, D3D12 256-byte
alignment). 🚨 **The padding can be ZERO** — at any width whose byte stride is already 256-aligned,
`rowPitch == width`. So at a pillarbox narrower than the padding, and at every aligned width, the two
engine layouts become **numerically indistinguishable** and the sniff **fails silently inside its own
blind spot**. ⚠ A literal `rowPitch == 0` has never been observed here; the finding is that the
*padding* can be zero, which is what kills the sniff.

### THE DESIGN THAT SHIPPED: make the layout IRRELEVANT BY CONSTRUCTION

1. Copy the view sub-rect into a **plugin-owned W×H texture at (0,0)**.
2. Enqueue a **whole-texture** readback — **no rect argument at all**.
3. Drain indexes **sub-rect-locally**: `SrcRow = Base + (int64)y * RowPitchInPixels * BPP`.

⇒ We own the texture, so `bufferHeight == picture height` on **every** engine, and no engine's staging
choice can reach us.

---

## 3. THE `WIP` COMMIT — WHAT IT CONTAINS AND HOW TO RECOVER IT

Branch **`feature/mask-gpu-reduce`**, bottom to top: **`aefa971`** (the Build B gate file) →
**`5f9cbbc`** (the first m35 docs commit) → **the session-061 close-out docs commit** → **the
session-062 corrections docs commit** → **the `WIP` fix commit, which is the TIP**.
🆕 **`origin/feature/mask-gpu-reduce` == local. THE WHOLE BRANCH IS PUSHED, INCLUDING THE WIP**
(owner ruling, 2026-08-26 — see §0 item 1). Master **`9f52cab`**, untouched and still carrying the
crash.

🚨 **THE ORDER IS DELIBERATE AND WAS CORRECTED IN SESSION 062.** The close-out docs were first
committed **on top of** the WIP, which made every doc in them unpublishable without also publishing a
commit that must never be pushed. **Unpushable work must never sit beneath publishable work.** The WIP
goes on top: the docs publish freely, and the WIP stays a trivially amendable tip that needs no
force-push. → `G185`.

```
 Source/AnomalyCapture/Private/AnomalyFrameCapturer.cpp           | 146 ++++++++--
 Source/AnomalyCapture/Private/AnomalyFrameCapturer.h             |  11 +
 Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp  |   4 +-
 Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.h    |   3 +
 Source/AnomalyCapture/Private/AnomalySceneViewExtension.cpp      |  31 ++-
 Source/AnomalyCapture/Private/AnomalySceneViewExtension.h        |   1 +
 Source/AnomalyCapture/Private/AnomalySveCapturer.cpp             |  10 +-
 Source/AnomalyCapture/Private/AnomalySveCapturer.h               |   2 +
 8 files changed, 192 insertions(+), 16 deletions(-)
```

Comment stripper run before the commit: **0 changed / 87 no-change** (repo invariant: source carries
no comments).

**INSURANCE COPY — REGENERATED 2026-08-26 (session 062) BECAUSE THE OLD ONE DID NOT APPLY:**
`D:\IntrusiveAnomalies\_binary_baselines\m35-buildb-uncommitted-2026-08-26.diff`
size **18,375 B**, `sha256 1069B1905A79C7C3BB8FE80FD3C519792B93229B916678EA6C8DAC8119A3760C`,
**LF, no BOM, trailing newline.** Written by **git itself** — `git diff --output=<file> <docs-commit>
<wip-commit>` — never by a shell redirect. **Verified by `git apply --check` against the published
pre-fix tree, from its final location: exit 0.**

🚨 **BOTH PREVIOUS INSURANCE COPIES WERE UNUSABLE, AND NOBODY HAD ASKED THE QUESTION THAT WOULD SHOW
IT.** The recorded check was "the diffstat matches at 192/16" — true, and irrelevant. Insurance has to
**apply**. Measured against the pre-fix tree:

| copy | bytes | form | `git apply --check` |
|---|---|---|---|
| `7A0CC269…` (the one on disk) | 18,756 | **BOM + CRLF** | ❌ `patch does not apply`, **all 8 files** |
| `8479FFE7…` (the earlier one, reproduced exactly) | 18,374 | LF, **no trailing newline** | ❌ **`corrupt patch at line 378`** |
| regenerated by `git diff --output=` | **18,375** | LF + trailing NL | ✅ **applies** |

⇒ **CRLF is the sole killer** (BOM + LF applies fine — the BOM is inert to `git apply`, the opposite
of the expected culprit), and the earlier copy died on a **missing final newline**. **The 382-byte
delta is now fully counted, not reasoned:** 3 (BOM) + 378 (one CR per line; the file has exactly 378
lines) + 1 (trailing newline) = **382** — confirmed by reconstruction, since truncating git's
canonical output by its final byte reproduces `sha256 8479FFE7…` exactly. → `G181`.
📌 The broken copy is kept beside it as `…-2026-08-26.diff.BROKEN-crlf-bom` — it is `G181`'s receipt.

🚨 **AND AS OF 2026-08-26 THIS FILE IS NO LONGER LOAD-BEARING AT ALL. DO NOT TREAT IT AS PROTECTION
AGAIN.** The WIP commit is **pushed** (§0 item 1), so **THE REMOTE IS THE BACKUP** — the fix exists on
`origin/feature/mask-gpu-reduce` and on `wip/session-061-backup`, as whole commits, on a machine that
is not this one. A patch file on one disk was always the weakest of the three, and this one spent its
entire working life unusable. It is kept **only** as `G181`'s receipt.
📌 The filename still says *uncommitted*; that is **historical** and is left alone because published
docs point at that path.

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
- **`AnomalySceneViewExtension.cpp`** — the SVE path. An extent-clamp early-out (drops the frame; it
  never captures a clamped region, because that would silently deliver a different picture than the
  label describes), then an RDG texture created
  `TexCreate_ShaderResource | TexCreate_RenderTargetable`,
  `AddCopyTexturePass(..., Rect.Min, 0, {W,H})`,
  `AddEnqueueCopyPass(GraphBuilder, Readback.Get(), OwnTexture)` — **no rect** — and
  `SubmitInFlight_RenderThread`. 🚨 **`FinalizeSveAfterPassOutput` is preserved on EVERY return path,
  including both new early-outs** — that function came from `b05066f` (the stale-present fix), **not**
  from m34, and `G-M4` exists to prove m35 has not disturbed it.
- **`AnomalyMaskSceneViewExtension.*`** — the same bounds guard **and nothing else**. A guard failure
  there lands in the existing `NOT_MEASURED` ⇒ **ADMIT** direction, so the m26 safety property is
  unmoved *by construction*, not by gate.
- **`AnomalySveCapturer.*`** — carries `SourceExtent` / `Format` through the in-flight record.

**Already committed on the branch:** `9aec10f` = Build A (readback-layout telemetry +
`IAI.Bench.Letterbox` lever, incl. `AnomalyCaptureLetterbox.cpp`, which refuses out-of-range aspects
and **warns that `minY == 0` means the lever is a no-op**); `aefa971` = the gate file; `5f9cbbc`
created these docs, and the session-061 close-out commit (now published beneath the WIP) rewrote them.

---

## 4. ENVIRONMENT / BUILD IDENTITY (`G121` — identity is exe hash **plus** container)

| | value |
|---|---|
| staged exe | **`733FE83C`** = m35 Build B, archived `_binary_baselines\StackOBot.exe.m35-buildb-733FE83C` |
| predecessor exe | **`7F37A4AC`**, archived `_binary_baselines\StackOBot.exe.m34-fix-candidate-7F37A4AC` — hash-verified BEFORE the swap. **THIS IS `G-M6`'s A-SIDE. DO NOT DELETE IT.** |
| container | **UNCHANGED** m34 quartet: utoc `2A66CA57` · ucas `A7EF9B12` · pak `D8009AD7` |
| cook | **NONE this session** — code-only hot-swap (`G103`) |
| A44 | green on the **staged** artifact, both encodings, incl. m34's own tokens as a positive control |

⛔ Do not cook. m35 rides the m34 container; the four-item cook is the office/Concorde pass (§12).

📐 **THE IDENTITY INSTRUMENT, STATED ONCE SO NOBODY GUESSES IT.** Every 8-hex build identity in this
project — exe, `.utoc`, `.ucas`, `.pak` — is the **FIRST 8 HEX CHARACTERS OF SHA-256**:

```powershell
(Get-FileHash <path> -Algorithm SHA256).Hash.Substring(0,8)
```

Verified this session against the archived predecessor (`…m34-fix-candidate-7F37A4AC` returns
`7F37A4AC`; MD5 and SHA-1 do not). It was stated only in `_binary_baselines\README.md` — a file
**outside version control** (`G112`) — and **never in the runbook**; it is now in
`setup-runbook.md` §8.1 as well.

⛔ **`_binary_baselines\StackOBot.exe.m34-candidate-17DEAA74` IS LOAD-BEARING AND IS NAMED IN NO STOP
BLOCK.** Established from the gate record: journal 058 identifies `17DEAA74` as the **staged exe every
m34 home gate leg ran on** (`G-R1`..`G-R6` and `G-R7`'s StackOBot half — the 145/145 `COMPARE`
IDENTICAL set), and journal 059 puts it at the head of the exe chain `17DEAA74` → `64568A5D` →
`7F37A4AC`. **It is the A-side of any future m34-attribution question**, exactly as `7F37A4AC` is
`G-M6`'s. 📌 Journal 059 also records that `64568A5D` (the `A-I1` instrument build) was **NOT archived
before being overwritten** — an instance of this rule not being followed, kept on the record.
⚖ **STANDING RULE (owner, session 062): NO ARCHIVED BASELINE IS DELETED UNTIL THE DOCS SAY WHAT GATE
DEPENDS ON IT.** "It looks old" is not an answer; the answer is a named gate or an explicit
"nothing depends on it".

---

## 5. THE GATE LEDGER — status per gate

| gate | what it tests | status |
|---|---|---|
| **G-M1a/b** | letterboxed equivalence, pose-independent half | ✅ **PASS** — frames 821×344; **exactly one field moved, `buffer_height 869 → 344`**, which IS the fix working; guard and clamp silent; drops 0 |
| **G-M1c** | letterboxed pixel bytes | ⚪ **NOT OBTAINED** (not failed) — see §8 |
| **G-M1d** | eyes on one frame | ✅ **PASS** (owner) |
| **G-M2** | un-letterboxed equivalence — *the leg that proves the shipped path did not move* | ✅ **PASS** — **NOTHING moved** |
| **G-M3** | guard proof-by-breaking, both ways (`G96`) | ✅ **PASS** — inflate 1 ⇒ 90 `READBACK-GUARD FIRED`, 0 frames on disk, `total_frames 0`, **no crash, run completed and wrote artifacts**; inflate 0 ⇒ silent again |
| **G-M4** | `b05066f`'s display fix must reproduce | ✅ **PASS** — `A-I1` **29/29 `overrideOutput=1`, zero at 0**; `G-F2` **29 IDENTICAL / 0 FIRST-DIFF**; all 8 events in band, **both endpoints 66,843 and 66,878 exact** |
| **G-M5** | m34's bench gates on the SVE path | 🟡 **3 of 5 PASS** — CTRL49, `SM_Ramp2`, spline all green. **OWED: `StaticMeshActor_73` and the `MaskProbe` leg — run them from §7.1's banked-derived commands, NOT from §7's payloads, which are a docs defect** |
| **G-M6** | hook-cost prior (attribution instrument, not a number) | 🟡 **METHOD SETTLED, PRIOR NOT TAKEN** — §9, order **`A,B,B,A`** after a declared discard leg |
| **G-M7** | the backbuffer path on Build B | 🔴 **NOT RUN** — needs PIE (§10) |
| **G-M8** | pillarbox / non-zero `Rect.Min.X` | 🔴 **NOT RUN** — needs PIE + a column checker (§11) |
| **G-M9** | within-frame dual-path comparator | 🔴 **NOT BUILT** — premise (a) verified; (b) and (c) unrun because the code does not exist (§7) |
| **P6** | `annotation.json` key set must not move | ✅ **MEASURED unmoved, 48 keys**, against a banked baseline. `run_summary` gains `readback_layout` and nothing else |

`READBACK-GUARD FIRED = 0` and `EXTENT-CLAMP FIRED = 0` on every non-guard leg.

---

## 6. BANKED ARTEFACTS — every path, and what each one is FOR

**Build A reference legs** (`D:\IntrusiveAnomalies\StackOBot\Saved\AnomalyCaptures\`):
- `session_20260826-152855_letterboxed` — **the letterboxed reference.** Build A's indexing is correct
  on this engine, which is what makes it a valid reference rather than a second suspect.
- `session_20260826-152922_noletterboxed` — the un-letterboxed reference.

**Build B legs** (same folder):
- `session_20260826-160750` — letterboxed (`G-M1`).
- `session_20260826-160834` — un-letterboxed (`G-M2`).
- `session_20260826-160905` — guard, inflate 1 (`G-M3`, the firing half).
- `session_20260826-160926` — guard restored, inflate 0 (`G-M3`, the silent half).
- `session_20260826-155749_DEAD_PARTIAL_BUILDB_CRASH_0_FRAMES` — **the crash leg. KEPT DELIBERATELY**
  as the before-picture of §14's crash branch.

**Noise-floor calibration** (`D:\IntrusiveAnomalies\_bench_sessions_bank\`):
- `M33_CTRL_A\session_20260823-221502` and `M33_CTRL_B` — **two runs of the SAME binary.** This pair
  is what establishes that cross-run frame byte-identity is unobtainable (§8). It is the control that
  turns "the frames differ" from a finding into a noise reading. **Do not delete it** — every future
  frame-comparison claim needs it.

**Today's five packaged legs** (`D:\IntrusiveAnomalies\_bench_sessions_bank\`, `_try1` attempts kept
alongside per `A63`):
- `M35_M35_GM4_CTRL49` — `G-M4`, the decisive display-fix leg. **PASS.**
- `M35_M35_GR3_RAMP2` — **KEPT AS INVALID, NOT AS A FAILURE.** Ran with a bare `SM_Ramp2` token, which
  matched nothing (`zero_match_bursts = 8`, `positive_frames = 0`). Below the ≥3-event validity floor
  ⇒ **INVALID**. It is kept because `A63` requires banking discarded attempts, and because it is the
  evidence for lesson 10 — an unreplayable target reads as a clean empty run.
- `M35_M35_GR3_RAMP2B` — the re-run with the full UAID token. **PASS.**
- `M35_M35_GR3_SPLINE` — the zero-only veto leg. **PASS.**
- `M35_M35_PKG_LB_PROBE` — **KEPT AS VOID, NOT AS A PASS.** The packaged letterbox lever refused
  (§10). Its `READBACK-LAYOUT` and bbox are identical to an un-letterboxed leg, so had the lever
  silently no-op'd this leg would have read GREEN. It is kept as the receipt that the refusal path
  earned its keep.

---

## 7. THE KNOWN-ANSWER LEG TARGETS, AND THE OUTSTANDING COMMANDS

🚨 **THESE TOKENS EXIST IN REPLAYABLE FORM ONLY INSIDE BANKED `run.json` FILES.** That is a discovery
problem for every future session, so they are recorded here and in the gate file §12 and the runbook.

```
SM_Ramp2_UAID_B42E9936F5429ADA00_2086822137
BP_SplineSpawn_C_UAID_A85E45CFE40412DE00_1511100424
```

📌 **WHY ONLY SOME TARGETS NEED A UAID:** `CB_GateLevel`'s actors were script-spawned with only
`set_actor_label()`, so their `GetName()` really is `StaticMeshActor_<n>` and a bare name matches.
MainWorld's editor-placed and Blueprint actors carry a runtime `_UAID_…` suffix, so a bare class name
matches nothing there. **The failure is silent and looks like an empty run.**

### The two outstanding `G-M5` legs

`ExecCmds` payloads. Wrap them with the A63 harness (`CaptureBench/tools/run_leg.ps1`) per
`docs/setup-runbook.md` §8.6 — **do not reconstruct a launch line.**

```
IAI.Capture.Delivery 0, IAI.Capture.Config 2 4 8 4 0, IAI.Capture.Mask 1, IAI.Capture.Start "" png 4242 90 missing_texture =StaticMeshActor_73
```
Known answer (`M34_R3_CYL73`): `StaticMeshActor_73` is the **Cylinder, non-Nanite** control —
**8/8 `MEASURED_NONZERO` at ~5.27 % of frame** against its own claimed 6.87 %, every discard bucket
clean. m34's `MASK-REDUCE COMPARE` must read IDENTICAL with 0 FIRST-DIFF.

```
IAI.Capture.Delivery 0, IAI.Capture.Config 2 4 8 4 0, IAI.Capture.Mask 1, IAI.Capture.MaskProbe 1, IAI.Capture.Start "" png 4242 90 missing_texture =StaticMeshActor_73
```
Known answer: the probe fires **ONE deliberate known-hidden arm**, the 255 detector + confirmation +
frame-scoped discard all report, `run_summary.mask_probe_arms = 1`, and the probe frame is disposed of
by the admit bias. **This is `F-6` item 5 — without it, items 1–4 can pass on an instrument that has
stopped looking.**

### §7.1 — ⛔ THE TWO PAYLOADS ABOVE ARE A DOCS DEFECT. DO NOT RUN THEM. (appended, session 062)

They are left standing above rather than silently edited, because the divergence is the lesson.
**Measured against the banked known-answer leg they cite** — `M34_R3_CYL73`, its own
`_leg_geometry.json` + `run.json`:

| axis | payload as written in §7 | `M34_R3_CYL73` as it actually ran | consequence of running the payload |
|---|---|---|---|
| anomaly | `missing_texture` | **`blinking`** | different anomaly ⇒ the 8/8 `MEASURED_NONZERO` / ~5.27 % datum does not apply |
| seed | `4242` | **`777`** | different actor selection / burst placement; not comparable |
| target form | `=StaticMeshActor_73` | **`StaticMeshActor_73`** (bare) | both match on `CB_GateLevel` (`G174`), but only the bare form is the banked one |
| mask reduce | *(absent)* | **`IAI.Capture.MaskReduce both`** | 🚨 **DISQUALIFYING — with `both` absent NO `MASK-REDUCE COMPARE` LINE IS EMITTED AT ALL**, so the leg cannot be graded against the datum it cites |

🚨 **THE FOURTH ROW IS WHY THIS IS WORSE THAN WRONG.** The payload does not fail; it **completes,
writes artifacts, and produces a leg with no verdict available on it** — the `G174` shape, arriving
through a missing *command* instead of a missing *target*. `IAI.Capture.Mask 1` alone measures the
mask; only `MaskReduce both` runs the CPU and GPU reductions side by side and emits the comparison
`G-M5` is reading.

**THE AUTHORITATIVE FORM — derived from the banked config, not transcribed.** Every unspecified
parameter is `run_leg.ps1`'s default and already matches the banked leg (`Pace 1`, `Delivery 0`,
`Sve 1`, `Seed 777`, `MaxFrames 90`, `1280x720` windowed, `Map /Game/CaptureBenchGate/CB_GateLevel`,
`Anomaly blinking`):

```powershell
# G-M5 leg 4 — the StaticMeshActor_73 / CYL73 known-answer leg
& "D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\run_leg.ps1" `
    -Label M35_GM5_CYL73 -BankPrefix "M35_" -Target StaticMeshActor_73 `
    -ExtraExecCmds "IAI.Capture.Mask 1, IAI.Capture.MaskReduce both"

# G-M5 leg 5 — the MaskProbe leg
& "D:\IntrusiveAnomalies\StackOBot\Plugins\CaptureBench\tools\run_leg.ps1" `
    -Label M35_GM5_PROBE49 -BankPrefix "M35_" -Target StaticMeshActor_49 `
    -ExtraExecCmds "IAI.Capture.Mask 1, IAI.Capture.MaskReduce both, IAI.Capture.MaskProbe 1"
```

📌 **THE PROBE LEG TARGETS `StaticMeshActor_49`, NOT `_73`** (§7 said `_73`). Every banked probe leg
on this box — `M34_R3_PROBE49`, `M34_F2_PROBE49`, `P26_FIX2_PROBE49`, `P27_EXT_PROBE49` — used `_49`,
its comparators exist, and `_49` is **the only target for which the `B1` pose gate is in scope at all**
(`b1_pose_gate_applies` reads `true` on those legs and `false` on every `_73` leg, `G117`). §7's known
answer for the probe leg is target-agnostic, so it does not contradict this.

⚠ **THE BANKED RECORD HAS A BLIND SPOT, NAMED SO NOBODY ASSUMES OTHERWISE.** The two files are
**complementary and both are required** — `_leg_geometry.json` carries anomaly/target/map/geometry
(19 fields) and `run.json` carries `seed` / `frame_cap` / `paced` / `start_frame` (its own
`target_anomaly` and `target_actor` are **empty**). **`CaptureBench.Marker` is recorded in NEITHER.**
It cannot affect `G-M5`, whose verdict is read from log lines rather than frame bytes — but any future
leg graded against a banked datum **by pixels** has an un-diffable axis there, and `G125` says the
marker changes every frame by construction. **Run marker OFF for any frame comparison and say so in
the label.**

### ⚖ STANDING RULE (owner, session 062) — `G184`

**A re-run leg's payload is DERIVED FROM THE BANKED LEG'S OWN RECORDED CONFIG, never transcribed by
hand into a doc.** A hand-copied payload is a defect surface with no known-answer control on it —
`G142`'s shape applied to launch lines. ⇒ **Before running ANY leg graded against a banked datum, diff
the intended payload against that datum's recorded config on EVERY axis and report the diff, even when
it is empty.**

### `G-M9` — full design, and its three verifications

The cvar is **`IAI.Bench.DualPathReadback <0|1>`, DEFAULT OFF** (named by owner ruling, session 062).
🚨 **IT MUST ECHO ITS EFFECTIVE STATE AT `StartRun`, the way the mask key does (`A48`)** — a
diagnostic that can be silently off is a clean null waiting to be misread, which is `G114`/`G170`'s
shape pointed at an instrument instead of at a lever.

When on, the SVE path enqueues a **second, independent** readback in
the **OLD form** — whole source texture, engine-chosen staging, sub-rect indexed with the `Rect.Min.Y`
offset — alongside the new owned-copy readback, **on the same frame**, and byte-compares the two
drained pictures.

- **VERIFICATION (a) — THE PREMISE, ALREADY DONE.** RDG executes passes in handle order and contains
  no sort or reorder; both passes are added consecutively and are read-only with respect to the
  source ⇒ they are guaranteed to see the same contents. **This is what makes a within-frame compare
  sound where a cross-run frame compare is not.** Verified from source *before* building the
  instrument, deliberately.
- **VERIFICATION (b) — PROVE IT CAN FAIL.** Provoke a non-zero diff (`G96`). A comparator that has
  only ever reported zero is not a comparator.
- **VERIFICATION (c) — PROVE cvar-OFF REPRODUCES BUILD B** exactly.
- 🚨 **ITS OUTPUT LINE MUST NAME BOTH CAUSES OF A NON-ZERO DIFF, AND SAY WHICH TO CHECK FIRST:**
  (i) the owned copy is not reproducing the sub-rect, or (ii) the added pass has broken the adjacency
  the premise rests on. **CHECK ADJACENCY FIRST** — a broken premise makes the comparison
  *meaningless* rather than failing, and reading it as a failure would send someone hunting a defect
  in working code. **The comparator is its own guard.**

---

## 8. WHY THERE IS NO CROSS-RUN FRAME-BYTE GATE (measured, not conceded)

- **MainWorld:** mean |Δ| **4.42**, max **225**, **78 %** of pixels differ. Unusable.
- **CB_GateLevel, same binary** (`M33_CTRL_A` vs `M33_CTRL_B`): mean |Δ| **0.00117**, max **3**,
  **0.116 %** of pixels — concentrated in the **lower half**, **top four grid rows exactly 0.000**,
  and **no corner box**, so it is **NOT** `G125`'s CaptureBench frame marker.

⇒ `G-M1c` / `G-M2c` are reported **NOT OBTAINED, never FAILED**, and `G-M9` is the replacement
instrument. **A pose-matched pair is necessary and not sufficient for byte identity.**

---

## 9. `G-M6` — THE METHOD, AND WHY A REBUILD CANNOT SUPPLY IT

**There is no hook-cost field in any artifact.** The available proxy is the median consecutive
`t_wall` delta from `labels.jsonl`, and it read **0.03334 on all five legs with `paced=True`** — the
**m11 pacer pins the tick to `1/VideoFps`**, so it absorbs any sub-budget hook cost and **a paced leg
structurally cannot measure one.** A rebuild changes nothing about that.

**THE METHOD, as ruled — no rebuild, no owner leg. ⚠ THE ORDER WAS REVISED IN SESSION 062; the
five-step form that stood here before is superseded, and the reason is below.**

**ORDER: one DISCARD leg, then `A, B, B, A`.** Hashes re-verified at **every** swap with
`(Get-FileHash <exe> -Algorithm SHA256).Hash.Substring(0,8)`. Pacing **OFF** (`-Pace 0`) on all four
measured legs. Same map, same seed, same config throughout.

| # | exe staged | leg | banked as |
|---|---|---|---|
| 0 | `7F37A4AC` | **DISCARD — declared a discard BEFORE it runs, never banked as a measurement** | flattens the steepest part of the warm-up gradient |
| 1 | `7F37A4AC` | **A**₁ | measured |
| 2 | `733FE83C` | **B**₁ | measured |
| 3 | `733FE83C` | **B**₂ | measured |
| 4 | `7F37A4AC` | **A**₂ | measured |

🚨 **WHY `A,B,B,A` AND NOT `A,B,A,B`.** Warm-up (`G66`) makes **earlier** legs slower. In `A,B,A,B`
the A-side occupies positions {1,3} (mean 2.0) and B {2,4} (mean 3.0), so **A sits earlier on average
and a monotonic gradient inflates A while flattering B — it would systematically hide the very cost
this prior exists to size.** In `A,B,B,A` both builds sit at mean position 2.5 and a monotonic order
effect **cancels**. The receipt for the gradient is Build A's own two legs: `speed_ratio` **1.1677**
on the leg that ran FIRST vs **1.0123** second — a **15 % spread with the *smaller* copy on the
*slower* leg**. → `G186`.

**REPORTING, ruled:**
1. **All four measured legs INDIVIDUALLY**, and the A/B means.
2. **Per-captured-frame milliseconds AND per-megapixel** (`G176` — per *captured* frame, 1:1 with
   output PNGs, not the mask's per-burst rate).
3. Any Concorde (3200×2000) figure is an **EXTRAPOLATION** and must be labelled one.
4. 🚨 **THE WITHIN-BUILD SPREAD ACROSS POSITIONS, REPORTED BESIDE THE BETWEEN-BUILD DIFFERENCE.**
   **If the A/B difference is not larger than the within-build spread, the honest statement is
   "BELOW THE RESOLUTION OF THIS INSTRUMENT" — never "no cost".** A null reported as zero here is the
   paced-leg mistake in a different costume (`G169`).

⛔ **NUMBERS ONLY. NO THRESHOLD, NO GATE, IS PROPOSED OR IMPLIED.**

📌 **ORDERING vs `G-M9` — THE CONSTRAINT IS SOFT, AND HERE IS THE MEASUREMENT.** Building `G-M9`
re-stages the exe, so `G-M6` is taken **first**; but that is **ordering hygiene, not a one-way door.**
Both sides are copied in from `_binary_baselines`, and the B-side archive **verifies**:
`StackOBot.exe.m35-buildb-733FE83C` = 240,890,368 B, `sha256` first-8 **`733FE83C`** — byte-complete
and hash-identical to the staged exe. So the prior stays recoverable after a `G-M9` build.
⚠ **CONDITIONAL ON ONE THING: `G-M9` MUST STAY CODE-ONLY.** An exe is half an artifact (`G121`), and
the archive is exe-only — it reconstructs a build only because the **container is shared and
unchanged**. `G-M9` adds a bench cvar, a second `FRHIGPUTextureReadback` and a CPU byte-compare, and
**no `.usf`** (the plugin's only global shaders are `AnomalyMaskReduce.usf` and
`AnomalyVisibleMask.usf`, both untouched) ⇒ code-only hot-swap, `G103`, container stays.
⛔ **If `G-M9` ever grows a new global shader it needs a cook (`G129`), the container moves, and the
constraint becomes HARD** — at that point the exe-only archive no longer reconstructs the B-side.

📌 **WHAT THE A-SIDE ACTUALLY IS, since §2's "Build A vs Build B" wording overstates it.** `7F37A4AC`
is the **m34 + display-fix predecessor — PRE-m35 entirely**, earlier than Build A (`9aec10f`). **The
Build A exe was never archived**; it was overwritten by Build B, and a rebuild is ruled out. ⇒ this
prior measures **m35 in its entirety** (Build A's telemetry and lever included), not Build B's copy in
isolation. That is the strictly larger and more useful quantity for `G-R7(ii)`, but it is not what
§2's phrasing promises, so it is stated rather than left to be inferred.

📎 Coarse reading already in hand, and it is **not** a cost measurement: both Build B un-letterboxed
legs read exactly **1.0000 / 30.000** against Build A's **1.0123 / 29.635** — consistent with the
pacer absorbing the cost.

---

## 10. THE BENCH COMMAND INVENTORY — packaged vs PIE-only

**Reachable in a packaged `-unattended` leg:**
`IAI.Capture.SVE <0|1>` · `IAI.Capture.Mask <0|1>` · `IAI.Capture.MaskProbe <0|1>` ·
`IAI.Capture.MaskReduce <gpu|cpu|both>` · `IAI.Capture.Delivery <0|1>` ·
`IAI.Capture.Config <pre> <settle> <pos> <post> <n>` · `IAI.Capture.Start [outDir] [fmt] [seed] [maxFrames] [anomaly] [target]` ·
`IAI.Bench.ReadbackGuardInflate <rows>` · and the future `G-M9` cvar.

**NOT reachable packaged — `IAI.Bench.Letterbox <aspect|off>`.** Measured, not assumed:

```
Capture(bench): LETTERBOX REFUSED - view target 'SpectatorPawn_2147482483' has no UCameraComponent.
```

🚨 **THE PACKAGED BENCH PAWN UNDER `-unattended` IS A `SpectatorPawn`, AND IT HAS NO
`UCameraComponent`.** The lever sets `bConstrainAspectRatio`/`AspectRatio` on the view target's camera
component, so it has nothing to set. **The command itself EXECUTED** — it failed on a semantic
precondition, not on being unknown, which is why other bench cvars are unaffected.

🎯 **THE REFUSAL PATH EARNED ITS KEEP.** That leg's `READBACK-LAYOUT` read `rect=(0,0)-(1280,720)`
with a bbox identical to the un-letterboxed leg, so a silent no-op would have read as a letterboxed
**PASS**. The leg is **VOID by the pre-declared rule**, not a pass.

⇒ **`G-M7`, `G-M8`, and `G-M9`'s both-origins half must run in PIE on MainWorld** (view target
`BP_Bot_C_0 / FollowCamera`). `G-M9`'s zero-origin half CAN be self-proven packaged.
📌 UNTESTED AND CHEAP: a packaged **MainWorld** leg might supply a camera-bearing pawn and move
`G-M7`/`G-M8` back to unattended. Not attempted.

---

## 11. `G-M8`'s COLUMN CHECKER — THE KNOWN-ANSWER DATUM IT IS PROVEN AGAINST FIRST

`Rect.Min.X` has been **ZERO in every leg ever run**, so the X half of the sub-rect origin is
completely untested. A pillarboxed leg (aspect NARROWER than the window) must give `rect.min.x > 0`
with a correct picture, guard and clamp silent.

⛔ **A ROW-ONLY CHECKER IS BLIND TO A HORIZONTAL DEFECT.** A column checker is required, and **it is
validated against this known answer BEFORE its verdict is read on the pillarboxed frame** —
owner-supplied, from the un-letterboxed frame:

- **0 near-black columns**
- **left-edge column mean well above zero (~50, min 5.0)**
- **right-edge ~87**
- **no collapsed band at either edge**

A checker that cannot reproduce that on the known-good frame does not get its verdict read on the
unknown one.

---

## 12. THE OFFICE PASS, AS REVISED AND RULED

**Order is fixed. The hand-resolved-conflict step is DELETED — it existed only for the withdrawn
cherry-pick route.**

1. **REBUILD, THEN COOK THE BRANCH.** A full cook of `feature/mask-gpu-reduce` carrying all four
   items (m34 · `b05066f` · `5495aa6` · m35). m34 needs a full cook (`G129` — a new global shader
   cannot hot-swap). **Rebuild the EDITOR target first** — `G47`/`G131`, and runbook §8.6 STEP 3.5 is
   not optional. **Verify artifact SIZE and hash after the build** (`G164` — a killed build left a
   2 MiB exe and the next `Build.bat` said "up to date" at exit 0).
2. **GATE IT:** `G-R7(ii)` **as amended by AMENDMENT 2** — (display) hitch A/B by eye/OBS judging
   **`b05066f`** and only that; (throughput) read **EXCLUSIVELY** from the m33 wall instruments
   (`t_wall` span vs frames/VideoFps, `speed_ratio` beside `game_clock_speed_ratio`), **against the
   `G-M6` home prior** — without the prior a throughput number is unattributable between m34 and m35.
   Plus **m35 frame-identity** (frames are the picture's size, no band, guard silent, drops 0) and
   **a photo of the `READBACK-LAYOUT` line**.
3. **MERGE the branch to master.** One route only.
   🔗 **MEASURED AHEAD OF TIME (session 062) — THE MERGE IS CLEAN, AND HERE IS WHY, so nobody
   rediscovers it at the console.** `master` and the branch carry **four cherry-pick TWIN PAIRS**,
   verified by identical `git patch-id --stable`:

   | on `master` | on the branch | patch-id |
   |---|---|---|
   | `9f52cab` | `5495aa6` | `7ebd45c2…` |
   | `e9bf96d` | `3363d5f` | `68386d8e…` |
   | `20c6a4e` | `3be67fc` | `264c8c0e…` |
   | `962dd29` | `f5e3f0f` | `c9ab8bc0…` |

   ⇒ **nothing exists on `master` IN CONTENT that the branch does not already carry**, and those four
   collapse at the merge. Merge-base **`1a3b1eb`**.
   `git -C <plugin> merge-tree --write-tree --name-only master feature/mask-gpu-reduce` returned
   **exit 0 with a tree OID and NO conflict list** — a clean forecast **with the m35 fix in it**.
   ⚠ Re-run that one command before the merge; it is cheap, and it is the only thing that would notice
   if either side moved after this was written.
   📌 `master`'s `2b6c93f` (`docs/client-readme.md` + the m33 gate file) is **not** on the branch and
   has no twin, but the branch never touched either file, so the merge keeps master's copy.
4. **TAG IN SEQUENCE: `m31` → `m33` → `m34` → `m35`.**

### Bates and Deimos — eye gates, and what the photo now decides

Both hosts report by **camera photograph off a screen we cannot reach**; nothing leaves those
machines. The `READBACK-LAYOUT` line is self-describing by construction (it names both known engine
layouts and the `bufferHeight` each predicts), which is what makes a photo sufficient evidence.

| reading | conclusion |
|---|---|
| `rect` inside `sourceExtent`, **no `EXTENT-CLAMP` line**, picture correct | the host's view rect and scene-colour texture agree; **D-2 REFUTED**; the fix is working |
| an **`EXTENT-CLAMP FIRED`** line instead | the view rect is OUTSIDE the source texture ⇒ **D-2 CONFIRMED** — the coordinate spaces disagree on that host, and the frame was **dropped rather than silently mis-captured** |
| **`rect.min.y > 0` with a correct picture** | the host letterboxes AND the sub-rect origin is being applied correctly — **the Bates crash condition, now handled** |

✅ **FAILURE IS FAST — READ THIS BEFORE YOU SIT THROUGH A RUN.** The one observed crash of this class
fired **22 ms after capture start**, on the **first armed frame's EXECUTION**, 0 frames written
(§14.1). ⇒ **A run that survives its first armed frame's execution has CLEARED this failure mode.**
So the first second of a Bates or Deimos capture is genuinely informative, and nobody should wait out
90 frames to learn what it already told them. ⚠ **But early survival is NOT a pass** — it clears one
failure mode, not the gate. Declaring success still needs the floor below.

⛔ **MINIMUM ARMED-FRAME COUNT — RULED, session 062: 90.** The full standard leg
(`frame_cap = 90`), evidenced by **90 files on disk** and `total_frames = 90`, with
`READBACK-GUARD FIRED = 0` and `EXTENT-CLAMP FIRED = 0`. **Below that, m35 is not reported working on
that host** — a short run is not a weaker pass, it is not a result. ⛔ Not an `IAI.Capture.Shot`.
⚠ **Declared as a FLOOR, not derived** — no mechanism gives a "safe after N" boundary for the
drain-fault class. Full justification and the log evidence behind it are in **§14.2**, and the reason
"survived one frame" is worth nothing is in **§14.1**: the one observed crash killed the run on the
**first** armed frame's execution, 22 ms after a perfectly clean `submitted` line.
📌 **This number belongs in the office-pass list beside the three-row table above.**

📌 **WHAT THE PHOTO NO LONGER DECIDES, stated rather than left to be discovered:** under Build B,
**D-1 (does the fork allocate rect-sized staging?) is NO LONGER DISCRIMINABLE — and no longer
matters**, because we own the texture. That is the design working, but it is a **real change** to what
the photo can answer versus what the original ruling assigned it.
📌 **Deimos (5.3) is still worth the photo:** it should report a correct picture with
`bufferHeight == picture height`, and being 5.2+ it is the host where the PRE-m35 code would have been
wrong at a non-zero origin. **It confirms the fix, not the layout.**

---

## 13. FOUR FINDINGS THAT LIVE ONLY HERE

1. **THE PACER IS BLIND TO HOOK COST.** Median `t_wall` delta **0.03334** on all five paced legs. Any
   perf question about a capture hook must be asked with **pacing OFF**, or it is asked of the pacer.
2. **ROW PADDING CAN BE ZERO** (§2). This is the whole reason the layout sniff is dead. A literal
   `rowPitch == 0` was never observed; the *padding* going to zero at an aligned width is the killer.
3. **THE BACKBUFFER PATH HAS NO FORMAT ASSERT AND NO GRACEFUL FAILURE BY DEFAULT.**
   `FValidationRHIUtils::ValidateCopyTexture` (`RHIValidationUtils.h:10-45`) DOES carry
   `checkf(bValidCopyFormats, ...)` plus source/dest bounds checks — **behind
   `#if ENABLE_RHI_VALIDATION` (line 5), OFF in a default Development build.** D3D12's own
   `RHICopyTexture` (`D3D12Texture.cpp:2868+`) has an `ensure()` for block alignment and **no format
   check.** ⇒ a format mismatch there is **undefined behaviour**. The structural guarantee — read the
   format from `BackBuffer->GetFormat()` every frame, recreate before the copy in the same
   straight-line block, `Item.Format = SrcFormat` for the drain's BPP — is the **only** protection,
   and it is verified safe for the first frame after a change. Contrast the SVE path, whose
   `AddCopyTexturePass` `checkf` is unconditional. 📌 This matters for the office-pass HDR
   preview-format item, which is exactly the scenario where the format changes under us.
4. **THE COPY IS PER CAPTURED FRAME, NOT PER ARMED FRAME.** `CaptureCurrentFrame()`
   (`AnomalyCaptureSubsystem.cpp:1736`, called at `:569`/`:579`/`:589`) mints one RequestId (`:1752`),
   arms one readback (`:1769` SVE / `:1773` backbuffer) and increments `SessionFrameIndex` (`:1777`),
   which **names the PNG** at `:1790` and is what `FrameCap` is tested against at `:551`. ⇒ **one arm
   == one captured frame == one PNG, 1:1**, so the copy runs 30× per second at 30 fps. The slip came
   from the **mask**, which really does arm a few times per burst — this session's receipts separate
   the rates: **90** guard fires for a 90-frame cap vs **29** `M23 PASS` mask arms.

---

## 14. THE CRASH BRANCH, AND A NAMED GAP

**Build B's first leg crashed at capture start:** `State != D3D12_RESOURCE_STATE_COMMON`.
**ROOT CAUSE:** the RDG texture was created with `TexCreate_ShaderResource` only, and the D3D12
**transient** allocator derives an initial state solely from RenderTarget / DepthStencil / UAV flags.
**FIX:** add `TexCreate_RenderTargetable` to `OwnDesc`.

⚠ **ASYMMETRY, DELIBERATE:** the flag is on the **RDG** texture only. The backbuffer texture is
persistent and declares `SetInitialState(ERHIAccess::CopyDest)` directly, so it needs no render-target
flag. **The asymmetry is named in that path's own log line** so a reader does not "tidy" the two into
agreement.

⛔ **A crash is not a gate failure of the design** — it was an allocator-contract error in the
implementation. But it IS why `G-M1`/`G-M2` are re-run after any change to `OwnDesc`.

### 14.1 — THE GAP IS CLOSED, FROM THE LOG (session 062). ⚠ IT CORRECTED THE ACCOUNT.

The crash run's log **survives** at `Saved\Logs\StackOBot-backup-2026.08.26-10.27.55.log`. It is
matched to the banked crash session by UTC start (`run.json` `start_time_utc` `10:27:49.239Z` vs the
first capture line at `10.27.49:265`), viewport `821x869` and seed `4242`. The sequence, in order:

```
[135] 10.27.49:265  Capture(sve): keyed frame id=1 submitted (rtframe=1136, fmt=18, rect=821x344).
[136] 10.27.49:287  SVE-WANT-TRACE arm 2/64 requestId=2 gameFrame=1135
[136] 10.27.49:287  Error: appError called: Assertion failed: State != D3D12_RESOURCE_STATE_COMMON
[137] 10.27.55:784  Capture(sve): keyed frame id=2 submitted (...)     <- 6.5 s later, in crash handling
      Callstack: GetInitialResourceState() <- HandleTransientAliasing() <- RHIEndTransitions()
      LogThreadingWindows: Error: Runnable thread RHIThread crashed.
```

✅ **THE DEFENSIBLE LESSON, and it is the strong one: SUBMISSION SUCCEEDING PROVES NOTHING ABOUT
EXECUTION.** `keyed frame id=1 submitted` printed cleanly, with the sub-rect **correct** (`821x344`,
the clamp passed) and the format correct (`fmt=18`), and the process died **22 ms later on the RHI
THREAD**, executing that same graph, inside the D3D12 **transient** allocator. Submit/enqueue lines
are game- and render-thread statements; the contract that broke is enforced one hop further on.
⇒ **the evidence a path works is the artifact on disk, never a submit line.** Here: `run.json` and
**zero frames**. → `G187`.

🚨 **AND IT CORRECTED THE ACCOUNT THAT REACHED THE NEXT SESSION.** That account said the assert fired
**on the second armed frame**, and drew from it *"a smoke test capped at one armed frame would have
passed."* **The log refutes both.** The assert fired after arm 1 was submitted and **before arm 2 was
ever submitted** — arm 2's `submitted` line is 6.5 s later, inside crash handling with the modal
dialog already up. **A one-armed-frame smoke test would have CRASHED.** The quoted log line in that
account was exact to the character; the frame index and the inference built on it were not, and the
inference was the half that would have shaped a gate.
📌 Consequence worth stating: **"why did frame 1 survive?" was never a real question**, so the
transient-allocator-reuse candidate offered to answer it explains nothing. One grep replaced a
mechanism nobody needed (`G120`).

📛 **LOGGED AS CHAT ERROR #6** (continuing the numbering from `#5`, the fabricated client
observation). **RETRACTED BY THE OWNER on the receipt**, verbatim: *"I gave inference dressed as
testimony — the quoted submit line was accurate, the 'second armed frame' was not, and one grep beat
it."* ⚠ **Scope of the damage, measured rather than assumed:** the false lesson was published
**exactly once** — in `a57af4e`'s version of this section — and it was **explicitly labelled *"a
hypothesis, not a finding"***, which is precisely why it never hardened into a gate. It is corrected
here by APPENDING, with the retracted claim shown beside the log, rather than by a silent edit.
⛔ **Any wording of the form "a defect that first fires on frame 2 is invisible to a smoke test" is
FALSE and must not be reintroduced anywhere.**

✅ **THE OPERATIONAL CONSEQUENCE, AND IT IS GOOD NEWS: THIS CRASH CLASS ANNOUNCES ITSELF WITHIN THE
FIRST SECOND OF CAPTURE, NOT AFTER 90 FRAMES.** The assert fired **22 ms** after capture start, on
the first armed frame's execution. ⇒ **a run that survives its first armed frame's EXECUTION has
cleared this failure mode.** **The 90-frame floor governs DECLARING SUCCESS (§14.2); FAILURE IS
FAST.** Nobody should sit through a run to learn something the first second already told them — and
"it is still running after a few seconds" is real, early, positive information, just not sufficient
information. This is repeated beside the three-row table in §12, where whoever takes the photo will
actually be standing.

### 14.2 — MINIMUM ARMED-FRAME COUNT FOR A BATES / DEIMOS RUN

⛔ **90 ARMED FRAMES — the full standard leg (`frame_cap = 90`), evidenced by 90 FILES ON DISK and
`total_frames = 90`, with `READBACK-GUARD FIRED = 0` and `EXTENT-CLAMP FIRED = 0`.** Below that, m35
is **not reported working on that host**; a short run is not a weaker pass, it is not a result.

⚠ **THIS IS A DECLARED FLOOR, NOT A DERIVATION, and the distinction is deliberate.** No mechanism
establishes a "safe after N" boundary for the drain-fault class, so nothing here is derived. What
supports 90:
1. **It is the only count with a control behind it.** Every home leg that passed, passed at 90 — and
   the guard leg confirmed exactly **90 arms for a 90-frame cap**, so the arm count is known to track
   the cap 1:1 (`G176`). A different size is a **new regime with no prior** (`G109`).
2. **Every smaller number is a guess.** The one observed failure killed the run on the **first** armed
   frame's execution, so "it survived one frame" is worth nothing — and 14.1 is what proves that.
3. **A drain fault needs frames to drain.** Arm and drain are separated by the readback fence, so a
   run must be long enough for arm→drain→write to complete repeatedly, not once.
4. **It costs 3 seconds of capture at 30 fps** on a host we cannot reach, cannot debug and cannot
   cheaply re-run. There is no budget argument for less.
⇒ **This number goes in the office-pass list beside §12's three-row table.** ⛔ Do NOT substitute
`IAI.Capture.Shot`. If a host cannot reach 90, **that is the report** — not a reduced pass.

---

## 15. HOW TO RUN A LEG (do not reconstruct these)

- **Packaged-leg recipe:** `docs/setup-runbook.md` **§8.6**. STEP 0 (disk floor: ≥15 GB GO, <10 GB
  NO-GO) and STEP 3.5 (**rebuild the EDITOR target** — `G47`/`G131`) are **NOT optional**.
- **A63 harness:** `CaptureBench/tools/run_leg.ps1` — banks every attempt, including discards, and
  takes `-Map` / `-Sve` / `-Marker`. ⚠ **Run marker OFF for any frame comparison** (`G125` — the
  CaptureBench frame marker changes every frame by construction and contaminates frame diffs).
- **Pose gate:** `CaptureBench/tools/check_pose.py` — **reporting only.** It prints the per-component
  ratio and the discriminator and never attributes a cause.
- **Validity floors:** ≥3 counted events per leg, and a pose mismatch makes a pixel half **INVALID,
  not FAILED** (`A63`/`A64`). Bank the discarded attempt either way.

---

## 16. STANDING CONSTRAINTS — OWNER RULINGS, VERBATIM WHERE GIVEN

- **"Do NOT checkout master. The checkout lift I authorised last turn is WITHDRAWN"** — never
  exercised. m35 develops, tests, commits and lands **on `feature/mask-gpu-reduce`**.
- **"ONE ROUTE ONLY: m35 must not ALSO be cherry-picked onto master in parallel."**
- **"Stop rule unchanged: any leg fails, report and stop, do not fix in the same turn."**
- **"EVERY CHECKER IS PROVEN AGAINST A KNOWN ANSWER BEFORE ITS VERDICT IS READ."**
- **"chat never states what a gate or artifact measures without Code quoting the gate file first."**
- No ratio, no threshold, anywhere (journal §209). **`P6` does not move.**
  `feature/stencil-capture` untouched. No force-push. Source carries **no comments** — run
  `python _strip_comments.py <repo-root>` before every commit.
- **Role ruling:** this box is the **only canonical author**; the office instance is
  eyes/builder/runner and commits nothing; nothing leaves that machine.
- **NEW (session 062) — A RE-RUN LEG'S PAYLOAD IS DERIVED FROM THE BANKED LEG'S OWN RECORDED CONFIG**
  (`_leg_geometry.json` + `run.json`), **never hand-transcribed into a doc.** Diff the intended payload
  against the datum's recorded config on every axis and **report the diff even when it is empty**
  (§7.1, `G184`).
- **NEW (session 062) — UNPUSHABLE WORK NEVER SITS BENEATH PUBLISHABLE WORK.** When a `WIP` commit
  that must not be pushed coexists with docs, **the WIP goes on top** (`G185`). And a commit that will
  be **amended** is identified by **subject and position, never by SHA** — its hash changes by
  construction.
- **NEW (session 062) — NO ARCHIVED BASELINE IS DELETED UNTIL THE DOCS SAY WHAT GATE DEPENDS ON IT**
  (§4).
- 🕳 **NAMED GAP — there is NO session journal for 2026-08-25.** `master`'s tip `9f52cab` landed that
  day, between session 060 and session 061. It is recorded in the m34 gate file's **A2.1** table and
  in `CLAUDE.md`'s four-item staging line, under its branch twin **`5495aa6`**. **The gap is named,
  not filled** — a reconstructed journal would be a fabrication (`G120`). The de-facto session index
  is `CLAUDE.md`'s status block; there is no separate index file.
- **`m26` veto semantics are untouched by m35:** `NOT_MEASURED` ⇒ **ADMIT**; `MEASURED_ZERO` ⇒ veto.
  The mask guard can only push toward `NOT_MEASURED`, which is the safe direction by construction.

---

## 17. THINGS THAT WILL BITE YOU IN THIS ENVIRONMENT

- 🚨 **`git` via the Bash tool HUNG this session** (the tool's cwd file vanished, and
  `D:\IntrusiveAnomalies\StackOBot` is not itself a repo). **Use PowerShell:
  `git -C <plugin-path> --no-pager …`.**
- 🚨 **Never edit a tracked doc through PowerShell redirection** (`G115`/`G141`) — it re-encodes every
  non-ASCII line and adds a BOM **while the text still reads correctly**. Use the editor tool, and
  **read `git diff --stat` before every commit**: a diffstat far larger than the intended change
  **halts** the commit.
- 🚨 **PowerShell 5.1: a here-string passed to `git commit -F -` becomes a PATHSPEC, not stdin** —
  measured this session (exit 1, `pathspec … did not match any file(s)`). **Write the message to a
  file and use `git commit -F <file>`.** Same family as the known embedded-double-quote trap.
- The bench cooked ini has **`bMaskMeasureDefault=True`** — a leg with no mask command runs mask
  **ON**; true inert needs an explicit `IAI.Capture.Mask 0`.
- Full lesson set with receipts: `docs/gotchas.md`, session-061 block.
