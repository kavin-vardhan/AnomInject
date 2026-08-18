# Chat handoff — S3a: B′ landed, cross-thread, certified. S3b not started.

**Session date:** 2026-08-18
**Plugin repo:** `AnomalyInjector` — HEAD `f922ba8`, clean, pushed. **No tag** — S3 is not a milestone
until S3b certifies it.
**Bench repo:** `CaptureBench` — `8dad64e`, local-only. **Probe untouched all session** (the one commit
is a tools guard, not a probe edit).
**Audience:** a cold reader — fresh chat, fresh Claude Code session, or a collaborator.

**Read the three S2 handoffs first if you have not** (`CHAT-HANDOFF-s2-keying-design.md`,
`CHAT-HANDOFF-s2-gate-env-and-i10-setup.md`, `CHAT-HANDOFF-s2-i10-and-m23-p3-fix.md`). This doc is the
fourth and carries **S3a, A59–A63, G97–G105**, and the P6/H4 outcomes.

---

## 1. The one-line status

**B′ is landed in production behind a default-OFF switch, it has run cross-thread, and it is green on
every gate S3a defined. Ratio-independence is STILL UNDISCHARGED — every S3a leg ran at ratio
≈1.0000006, the easy regime, and the one that has historically masked bugs in this project. S3a proves
the mechanism works. It proves nothing about the regimes S3 exists to certify. And S3 going green will
not close P1.**

---

## 2. What is in the code now

| | |
|---|---|
| `IAI.Capture.SVE <0\|1>` | **default 0**, mid-run guarded, packaged default via GConfig `[AnomalyCapture] bSveCaptureDefault` |
| `IAI.Capture.SVE.ForceMiss <N>` | 0 off · 1 every key · N>1 every Nth key |
| `IAI.Capture.SVE.ForceMissPhase <P>` | corruption fires when `((serial + P) mod N) == 0` |
| `IAI.Capture.SVE.RingTest [n]` | headless ring exercise |
| `run_summary.json` | `capture_path` + `key_ring_{published,consumed,missed,wrapped,corrupted}` — **emitted only when the switch is ON** |

**New files** (`Source/AnomalyCapture/Private/`): `AnomalySveKeyRing.{h,cpp}`,
`AnomalySceneViewExtension.{h,cpp}`, `AnomalySveCapturer.{h,cpp}`.
**Build:** `AnomalyCapture.Build.cs` gained `Renderer` + the **Renderer PRIVATE include path**,
non-Shipping only (**G100**).

### The seam — why S3a was small

The async path already keys frame↔state by **`RequestId = GFrameCounter`**. B′ publishes
`(ViewFamily.FrameNumber → GFrameCounter)` at `BeginRenderViewFamily` and the render pass recovers it
by lookup — **that recovered value *is* the key `PendingSnapshots` already uses**. So S3a swapped the
**producer** of `FAnomalyCapturedFrame::RequestId` and touched **no consumer**: the label record,
accumulator, writer, `labels.jsonl` and `annotation.json` are untouched by design.

---

## 3. What is proven, and what is not

**PROVEN**

- **Cross-thread keying works.** SVE leg: published **121** / consumed **121** / **missed 0**.
  First execution of the property B′ exists for.
- **Switch-OFF is subset-identical** (see A63 below for the gate that establishes it).
- **The loud-miss guard fires and discriminates.** ForceMiss 0/1/4 → 90 / 0 / 68 frames on disk;
  `missed == corrupted` exactly 25 % at N=4; survivors 1:1 with their label rows.
- **P3b does not arrive by the SVE route.** With 7 positives deliberately dropped, every one vanished
  from disk **and** from the claims; **claimed indices with no file behind them: empty**.
- **C3 resolution delta measured:** SVE view rect 1280×720 vs backbuffer window rect 1280×720, **dW=dH=0**
  at this config. Not generalisable to DPI-scaled or letterboxed setups.
- **UI-free by construction** — the grab is post-tonemap, pre-Slate.

**NOT PROVEN — do not let these be cited as done**

- **Ratio-independence.** Every leg ≈1.0. The matrix is S3b.
- **Behaviour under stall.** The ring has never run starved.
- **Frame identity by marker.** Legs ran **marker-OFF**, so the decoded-marker ↔ `frame_index` check was
  never performed; frame identity rests on count + names + cadence + the label index series. Byte
  comparison is unusable — see A47 below.
- **`LastRunDir`.** Implemented, **runtime-unverified** (§6).

---

## 4. New standing rules

| # | Ruling |
|---|---|
| **A59** | **MCP-bridge provenance.** No bridge measurement is attributed to this project until `Paths.project_dir()` **and** the engine version are read back and stated. → **G97** |
| **A60** | A quantity absent from the artifact is **operator-supplied**, or the claim is **UNDECIDABLE**. Never reconstructed, never defaulted, and **never replaced by a weaker test reported as the original** |
| **A61** | A newly discovered **shape** earns a **diagnostic tag**, never a new verdict bucket |
| **A62** | **For any gate whose subject is written output, THE ARTIFACT ON DISK IS THE GATE.** A log line saying `FINISHED` is not evidence a file exists |
| **A63** | **Focus-match is a leg validity condition.** Legs in a **cross-binary comparison** are COMPARABLE only if `start_frame` matches (neither rode the 30 s focus timeout). Mismatch ⇒ not comparable, the comparison **does not run**, the leg is **INVALID** and re-run |

**A63's load-bearing sentence — quote this whenever A63 is cited:**

> **A leg is discarded for HOW IT RAN, never for WHAT IT SHOWED.**

Invalidity is declared by a **pre-fixed rule**, on a condition **independent of the outcome**, decided
**before** the comparison is read. That is what stops "invalid, re-run" becoming a laundering route.
⛔ Pinning with `IAI.Capture.FocusGate 0` was **rejected** — it changes the system under test (**G93**).

**A47 AMENDED:** the bifurcation is in camera **ROTATION**, not position (eye position invariant at
`(-1500,0,260)` on 369/369 gate samples). New clause: **inter-actor occlusion is invariant across it** —
but **do not generalise**, it holds only because gate targets are STATIC and the player start is fixed.

---

## 5. Gotchas G97–G105

**G97** bridge attaches to whichever editor is listening · **G98** `AffectedFrames` is a
projection-filtered set, not a range · **G99** the level-authoring script is destructive by default ·
**G100** Renderer-private include path breaks far from its cause · **G101** `IAI.Capture.Start`'s
`outDir` is **CWD-relative**, not `Saved`-relative · **G102** appending after `}` can **steal an
`else`**, and it compiles clean · **G103** staging a code-only change is an **exe hot-swap** (no cook;
G92's archive-wipe not involved — but the hot-swap **is** the stage step, so A44-scan the **staged**
artifact) · **G104** the focus-drift environmental fact · **G105** the ambiguous-metric fourth instance.

---

## 6. DEBTS — carried, not dropped

1. **`LastRunDir` runtime verification.** Ships **uncertified**. `capture_stop` / `capture_status` /
   `ControlSnapshot` read the path through `GetStatus` after a run ends; confirming the dashboard still
   receives the finished session's path needs a **WS client against the control server post-run** —
   `-ExecCmds` is startup-only. **Fold into S3b**, where a live check is cheap alongside the matrix legs.
2. **Marker-OFF limit on frame identity** (§3).
3. **`ForceMiss` phase-lock coarseness** — 3 of 4 phases reproduce the same drop pattern; only phase 2
   broke the lock. A randomised mode would be stronger.
4. **`video.total_frames` vs index range under partial loss** — `total_frames` 68 while indices run to
   89. Reachable only after the guard has fired. **Noted, deliberately not fixed** — do not change the
   schema on a post-guard path.
5. Pre-existing: P6's `camera.path` naming; P6's `coverage_pct`-vs-`coverage_ratio` (predicted from
   source, never observed); the `node.bounds` render-relevant-bounds milestone candidate; H4.

---

## 7. Environment the next session inherits

- **Staged package:** `Builds\BenchGate\Windows\StackOBot\Binaries\Win64\StackOBot.exe` carries the
  **S3a-3 build** (240,539,648 bytes), A44-verified in **both encodings**. The **pre-S3 m23 baseline is
  preserved beside it** as `StackOBot.exe.m23-baseline` (240,502,272 bytes) — restore it if you need a
  pre-S3 leg.
  ⚠ `Builds\BenchGate\Windows\StackOBot.exe` (217 KB) is the **launcher stub** — scanning it reports 0
  for everything (**G90**).
- **Bank:** `D:\IntrusiveAnomalies\_bench_sessions_bank`, **34 dirs**. The S3a set:
  `S3A2_BASE` + `S3A2_BASE2` (the m23 **control pair**), `S3A2_FIX_OFF`, `S3A2_FIX_ON`, `S3A2_FM1`,
  `S3A2_FM4`, `S3A3_OFF*`, `S3A3_P1..P3`, `S3A3_OFF_T1`.
  **`S3A3_P2` is the only banked leg that exercises a dropped positive frame.**
- **Branches:** `master`; `s3a-2-GATE-FAILED-do-not-merge` (`087f4d9`) **unmerged on purpose — it is
  evidence of the re-parented-`else` defect**; `feature/stencil-capture` (do not rebase).
- The **packaged-leg recipe** is in `docs/setup-runbook.md` §8 — launch line, the CWD output trap, and
  the focus-forcing step. Use it rather than reconstructing one.

---

## 8. Three self-corrections in three turns — the pattern that matters

This project's historical failure mode is **plausible-untested mechanisms surviving into the record**
(P2's stale-present story, the counter-catch story, the A54-v1 oracle). Three consecutive turns caught
one **from the inside, after acceptance**:

1. **The re-parented `else`** — diagnosed to a one-token cause, with both offered leads *eliminated*
   rather than ranked, and the "FinishRun ran twice" hypothesis killed.
2. **The ambiguous overlap metric** — `overlap = missing ∩ claimed = 0` is simultaneously "never
   tested" and "tested and handled perfectly", so it could not judge the region it was introduced for.
   Sound discriminator: **did the claimed set shrink against the clean run.**
3. **The publish-count mechanism** — an accepted journal said "dropped frames stalled the phase
   machine"; the real cause was the **focus-gate wait** (the SVE activates before the focus branch).
   The correction was followed immediately by checking whether any verdict depended on the wrong
   mechanism. **None did.**

**The check that makes a correction safe is the second half: when the mechanism was wrong, ask what
rested on it.**

---

## 9. Forward

**S3b — the matrix.** Not planned, not started; scope is being decided chat-side. It is where
**ratio-independence is discharged**, and where the `LastRunDir` debt is cleared.

**Unchanged and not drifting:**

- **S3 IS NOT A MILESTONE UNTIL S3b CERTIFIES IT. No tag.**
- **S3 GOING GREEN DOES NOT CLOSE P1.** P1 has never been reproduced, and you cannot demonstrate a fix
  for something you cannot summon. A clean matrix proves the new path does not carry **the old race**.
  It is **not** evidence it cures her defect. P1 stays **OPEN** — leads: **H1** (GPU load, no lever
  exists) and the **delivery-mode gap**.
- **If the S3b matrix goes red, that is a DESIGN FAILURE of B′, not a bug — redesign, not patch.**
- **No client-facing action item exists anywhere in the plan** (struck 2026-08-18; there is no client
  channel in either direction).
