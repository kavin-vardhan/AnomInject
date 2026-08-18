# Chat handoff — S3: the SVE capture migration, certified and tagged (m24)

**Session date:** 2026-08-18
**Plugin repo:** `AnomalyInjector` — HEAD `d429f3b`, clean, pushed. **Tag `m24` → `8373b76` → `d429f3b`, pushed.**
**Bench repo:** `CaptureBench` — `0f89212`, clean, local-only. **Probe untouched the entire arc.**
**Audience:** a cold reader — fresh chat, Claude Code, or a collaborator.

**Read the S2 set first if you have not:** `CHAT-HANDOFF-s2-keying-design.md` (B′, MainMenu, A8–A33) →
`CHAT-HANDOFF-s2-gate-env-and-i10-setup.md` (gate env, A34–A43) →
`CHAT-HANDOFF-s2-i10-and-m23-p3-fix.md` (I10, P3/m23, A44–A58, P1–P6).
**This doc is the fourth in the set and supersedes their status lines.** It carries **A59–A64**, **G97–G109**,
**P7 and P8**, the **P1 narrowing**, and one **outstanding brief** (§11) that must be verdicted before S4.

**One-line status:** the SVE capture path is **landed, switchable, and certified** across every ratio regime and
in delivery mode. **Depth is parked. S4 is the backbuffer demotion. The client's original defect (P1) is still
unreproduced and unfixed, and now has exactly one named lead left.**

---

## 1. What this session did, in one paragraph

S3 was built and certified end to end. **S3a** landed B′ in `AnomalyCapture` behind a default-OFF switch across
three gated slices (one of which failed its own gate and was correctly halted, diagnosed, and rebuilt). **S3b**
ran the ratio × config matrix: nine packaged runs across nominal / client-band ×2 / deep / pacing-off, plus three
delivery-mode pairs. **33 events, 33 ALIGNED, 33/33 decidable, zero extras on every delivery pair.**
Ratio-independence — undischarged since the stage was renumbered — is discharged. Along the way the certification
oracle was discovered **not to exist as a tool**, was rebuilt from prose, and was found to have two silent defects
and one non-pose-invariant threshold; each was caught by a known-answer control before it graded anything.
Tagged **m24**. Two new phenomena (**P7**, **P8**) opened. **The delivery-mode gap — one of P1's two live leads —
is eliminated by measurement.**

---

## 2. Current state

| | |
|---|---|
| `AnomalyInjector` HEAD | `d429f3b` — clean, pushed, 0 unpushed |
| **Milestone tag** | **`m24` → `8373b76` → `d429f3b`, pushed, remote confirmed** |
| Production code | **UNCHANGED since S3a-3.** The whole S3b arc was validation and docs |
| `CaptureBench` | `0f89212`, clean, local-only. Probe untouched all session |
| Session bank | `D:\IntrusiveAnomalies\_bench_sessions_bank` — **58 dirs** |
| Staged package | S3a-3 binary, SHA-256 `3BA854FB…`; pre-S3 preserved as `StackOBot.exe.m23-baseline` |
| Branches kept unmerged | `s3a-2-GATE-FAILED-do-not-merge` (`087f4d9`) — **evidence, do not merge** |

**New CaptureBench tooling** (local-only, committed): `a54_oracle.py` (the A54 oracle + the ±1 positive-control
shifter + the A56 checker, one file so the definition lives in one place), `check_pose.py`, `run_leg.ps1`,
`eval_leg.py`.

### What m24 certifies — and its four limits

The tag is **annotated and carries its own scope statement**, deliberately, so a reader who never finds the journal
still gets the truth. Reproduced here:

**CERTIFIED:** ratio-independence across nominal / client ×2 / deep / pacing-off (33 events, 33 ALIGNED, 33/33
decidable, positive control decisive in both directions on every leg); delivery-mode orthogonality (three pairs,
zero extras, invariant core identical including all five `key_ring_*`); A10 by marker at every regime; ring under
stall; `LastRunDir` post-run.

- **LIMIT 1 — MODAL CAMERA POSE ONLY.** A defect manifesting only in a bifurcated pose would systematically not be
  seen by this design.
- **LIMIT 2 — A52.** VideoFps 30 pinned throughout; clean at 30 licenses **nothing** at any other fps, in either
  direction.
- **LIMIT 3 —** Stage 3 is **annotation-shape evidence only**; the pixel oracle cannot run in delivery mode.
- **LIMIT 4 —** the oracle is certified at 30 fps; its **margins** are not reproduced above it (**P7**).
- **AND: S3 going green does not close P1.**

### The matrix

| leg | stall | pace | ratio | band | ev | verdict | med \|margin\| | dec |
|---|---|---|---|---|---|---|---|---|
| L1 nominal | 0 | on | 0.999914 | nominal | 7 | ALL-ALIGNED | 0.107745 | 7/7 |
| L4 deep | 99 | on | 3.030798 | deep ≥2.80 | 5 | ALL-ALIGNED | 0.110588 | 5/5 |
| L2 client | 40 | on | 1.254839 | client | 7 | ALL-ALIGNED | 0.110870 | 7/7 |
| L3 client | 39 | on | 1.239722 | client | 7 | ALL-ALIGNED | 0.108453 | 7/7 |
| L5 pacing-off | 0 | **OFF** | 0.332742 | own cat | 7 | ALL-ALIGNED | 0.105930 | 7/7 |

**33 counted · 33 ALIGNED · 0 SHIFTED · 0 ABSENT · 33/33 DECIDABLE.** In-leg positive control run on every leg,
both directions: no ALIGNED verdict survived any synthetic ±1 shift anywhere.

**Delivery pairs (D1/D2/D3 ↔ L1/L2/L3):** difference set == the regime's run-unique set + `delivery_mode` + the
files delivery legitimately suppresses. **Zero extras, all three.** Invariant core asserted **positively**, not
inferred from the differ's silence: event count, `frame_indices`, `affected_frames` start/end/count, `manifested`,
type/subtype, node bounds/name/path/asset, `coverage_ratio`/`coverage_pct`, the camera block, and 15 `run_summary`
invariants **including all five ring counters**. B′ behaves *identically* in delivery mode, not merely acceptably.

**Run-unique sets, measured per regime, not argued from source:**
`nominal = {session_id, video/path, speed_ratio, sustained_wall_fps}`;
`client band` adds `{video/fps, stamped_fps}` — m11's honest-fps stamp activates above ratio 1.02.

---

## 3. Decisions made this session, with rationale

### 3.1 The client channel is CLOSED, in both directions — and it re-shaped the plan

**Owner ruling, mid-session:** nothing can be asked of the client and nothing can be requested from her.

Consequences, all already applied:
- The **delivered-session fabrication auditor was CANCELLED** (not paused). It had been designed, planned, and
  schema-verified; the plan is banked in journal 035 as reference, not as pending work.
- The **office-machine `target_fps` audit** and the precautionary **"cap VideoFps at 30" client note** are
  **STRUCK from the standing plan** — removed, not deferred. If a cold reader finds either in an older doc, this
  supersedes it.
- **Every remaining lead on the client's #1 complaint (labelled-but-invisible anomalies) must now be derivable
  from our own source and our own bench.** That is what raised **H4** (§8) from a curiosity to the best-placed
  open item.

### 3.2 The auditor was cancelled *after* it had already produced its most valuable finding

Before cancellation, Code established **G98**: `AffectedFrames` is **not a contiguous frame range**. It is
accumulated one index at a time and only on frames passing `ProjectActorBoundsToScreenRect`, so a target that
leaves frame mid-window leaves a **gap**. That killed the entire "gapped ⇒ genuine" discriminator the audit rested
on — in *both* the window-blind and windowed paths, and **in the dangerous direction** (a fabricated event would
have been blessed).

Keep the finding even though the tool is dead. It is why no shape-based fabrication test can be trusted.

### 3.3 The A54 oracle did not exist — and that is a bigger finding than a missing tool

Rebuilding it for S3b revealed that **TAU (0.04684) and the A54 canonical definition existed only in prose**
(gotchas, the S2 handoff, journal 034). The scripts that graded **I10 and m23** were written in-session and never
committed.

> **Every certified result this project holds was graded by scripts that no longer exist. Those results are not
> wrong. They are not currently reproducible**, which is a different and quieter problem.

**→ G106:** analysis instruments that grade a certified result are **committed artifacts**. A verdict whose grader
no longer exists is not reproducible, however well it was documented in prose.

### 3.4 The rebuild had correct verdicts and wrong confidence — the worst failure shape available

Two load-bearing points were under-specified in the prose, and the attractive reading was wrong on both:

- **(a) The event flanks MOVE with the shift hypothesis.** A wrong shift must drag a genuinely-hidden frame into
  its own reference so its score collapses toward zero; **that collapse is the discriminating power.** Fixed
  flanks leave a wrong shift scoring exactly half the right one.
- **(b) The canonical shift space is `{-1, 0, +1}`.** Admit ±2 and ±2 becomes the runner-up, halving every margin
  again.

**Either error alone takes R30 from 12/12 decidable to 0/12 while leaving "12 ALIGNED / 0 non-ALIGNED" intact.**
An instrument that agrees on every verdict and disagrees on every confidence **passes a casual re-check** and
silently corrupts every A55 decidability annotation downstream.

**Both were caught only because the gate demanded published *numbers*, not published *conclusions*.** That clause
was written as belt-and-braces; it was the entire gate.

### 3.5 P8 — TAU is not camera-pose invariant, and it fails toward "defect"

The oracle returned **ABSENT** on the banked client-band leg L3. Code **read the raw in-bbox series before
reporting** and found the hide plainly present: perfect separation (claimed 0.8153–0.8180 vs non-claimed
0.8531–0.8559, **zero overlap**), perfect alignment. The oracle said ABSENT for exactly one reason:
`0.0383 < 0.04684`.

**Cause:** TAU is an **absolute luminance difference**. The A47 rest pose sets both the bbox *and* the background
behind it, so it sets the **scale** of the very quantity TAU thresholds. TAU was frozen from legs that all settled
in the modal pose.

**Why it matters more than a missed certification:** A50 treats **ABSENT as reproduction of the defect**. A false
ABSENT does not read as inconclusive — it reads as **evidence of a defect that is not there**, on the client band.

**Corroboration that could not have been fitted:** the bifurcated pose makes the hidden side *darker*; journal 031
independently recorded L3's hidden side as "low" against "high" everywhere else, **written months before this
instrument existed**.

**A56 cannot catch this and that is not a bug in A56.** A56 asks whether a leg is *self-consistent* (L3 passes at
97.6%). It never asks **whether the leg's pose is the pose TAU was calibrated on**.

**Adopted: B1** — a **pose-match precondition** on A56. TAU and the A54 definition untouched. The property that
makes it right: **it does not make L3 pass; it makes L3 honestly NOT-CERTIFIABLE instead of falsely ABSENT.**
It fired on its 2nd and 3rd instances within one turn of being ruled — L4 and L3 both settled non-modal on first
run, and **L4 would have produced a false ABSENT on the deep leg.**

**Filed, not built: B2** — replace the absolute difference with a **scale-free separability statistic**
(separation / within-class spread). Correct, expensive: a definition change needing its own full-control gate, and
it may reopen m23's DA60 floor, a certified milestone. **It is now gateable** — L3_client39 is banked as a
**known-ALIGNED, bifurcated-pose control**, so B2's gate is eight controls, not seven. A57's bracket-vs-contain
gap for the *pose* axis is closed.

### 3.6 The settle window is mechanical, and journal 031's windows were not

Frozen **before** application: `SETTLE_TOL_DEG = 0.5`, `SETTLE_K = 5`. Rule: drop leading frames until `view.rot`
is stable for K consecutive frames. No per-leg override exists in the code.

**Gate vs journal 031's published windows: 4 of 6.** Both misses diagnostic, neither closable by tuning K:
- **L1 is structurally unreachable by any camera rule** — its camera has one distinct rotation across all 90
  frames. Its published `16` is the **luminance ramp** (570 ms / 33.3 ms = 17.1).
- **L3 is off by one** (29 vs 30) — a boundary convention.

Code then measured the **competing hypothesis**: a luminance-settle rule matches the published windows on **zero**
legs. That is what makes this a *finding* rather than a failure:

> **Journal 031's windows are neither camera nor luminance. They are a per-leg hand-chosen mixture, selected after
> seeing each leg. No single mechanical rule can reproduce them, because they are not the output of one.**

**Consequence, and it is live — see §11.**

### 3.7 The stolen `else` — and why the fix is structural

S3a-2's first attempt **passed its own logs and destroyed its own output.** `FinishRun` was
`if (bRunBegun) {write} else {delete dir}`; the new block was appended after the `if` body's closing brace, and the
next token was `else`. **The `else` changed owner** — the delete now fired whenever the SVE capturer was absent,
i.e. **always, with the switch OFF.** It compiles clean; neither branch is dead; nothing automated could catch it.

**Blast radius answered from source, caller by caller:** all four `FinishRun` callers are guarded and `FinishRun`
closes its own preconditions on exit, so it executes **at most once per run** on m23. **No client build can reach
the delete after a successful write.** Confined to the failed branch.

**Fix is structural, not a moved line:** two independent `if`s over a captured bool. **No `else` token exists for a
future append to inherit.** An early return was correctly *rejected* — `FinishRun` has a mandatory tail (lifecycle
reset, fixed-timestep restore, overlay restore, auto-injector resume) that an early return would skip.

### 3.8 C1 was unsatisfiable — and the replacement is stricter

Chat-Claude ruled "switch-OFF output must be byte-identical, no exclusions." **Impossible by construction**:
`session_id` is a timestamp, `end_frame` is an absolute `GFrameCounter`, and — as the control pair later showed —
**all 90 images differ byte-wise** because A47 caught one leg mid-settle.

**Replacement (adopted):** a **control pair** of two same-binary runs establishes the run-unique field set
**empirically**; the test pair's difference set must be a **subset** of it. Decided by a rule fixed in advance,
never by judgement after seeing a diff. Extras came back **0**.

### 3.9 Focus is a validity condition, not an instruction to the operator

The game window losing focus makes a run ride a 30 s timeout, shifting every absolute frame/time field. This
**flipped a gate result** and had never been controlled — S3a-2's earlier pass was partly luck (all four of its
legs happened to get prompt focus).

**A63 adopted**, with Code's wording as canonical:

> **A leg is discarded for HOW IT RAN, never for WHAT IT SHOWED.**

That is what stops "invalid, re-run" from becoming a laundering route: invalidity is declared by a pre-fixed rule,
on a condition independent of the outcome, decided **before** the comparison is read. Because of that, **automating
the retry changes nothing about its soundness** — the harness now discards and re-runs on both focus timeout and
bifurcated pose, up to 3 attempts, **banking and reporting every attempt including discards.**

**Rejected: pinning via `FocusGate 0`** — G93 records focus-gate × fixed-timestep corrupting the camera. Not
disabling a safety mechanism with a known interaction with the thing being measured.
**Rejected: virtual-desktop isolation** — it *removes* foreground focus, which is exactly what the gate waits for.
It would **guarantee** the timeout it was meant to prevent. Do not re-propose it.

### 3.10 Depth is parked; S4 is the backbuffer demotion

Owner ruling. Depth (`SceneDepthTexture` in `PrePostProcessPass_RenderThread`, FP32, plus the typed FP16/FP32
path) is **PARKED and UNNUMBERED** — revivable if the ML side wants it, or if H4/stencil needs a cheap instrument,
and it revives onto a keying model that is already green.

**S4 (was S5) = backbuffer demoted to the UI-on option, defaults flipped, client config.** The number moved down;
**there is no hole at S4** (the m22 renumber hazard).

---

## 4. Amendment index — A59 to A64

| # | Ruling |
|---|---|
| **A59** | **MCP bridge provenance.** No measurement over the bridge is attributed to this project until `Paths.project_dir` **and** engine version are read back and stated. The bridge attaches to whichever editor listens on :12029 — and a second UE project runs on this box permanently (G97) |
| **A60** | **A quantity absent from the artifact under test is supplied explicitly by the operator, OR the analysis reports UNDECIDABLE for every claim depending on it.** Never reconstructed, never defaulted, **and never replaced by a weaker test that is then reported as if it were the original one.** First real use in Stage 3, where it produced a *better* method than the proxy it replaced |
| **A61** | **A newly discovered shape does not earn a new verdict bucket.** It gets a diagnostic tag on an existing verdict, or an existing amendment excludes it. Taxonomy inflation is goalpost-moving with better manners |
| **A62** | **For any gate whose subject is written output, the artifact ON DISK is the gate.** Both S3a-2 legs' logs read identically and perfectly while one deleted its session. A log line saying FINISHED is not evidence a file exists. (m19's lesson recurring) |
| **A63** | **Focus-match is a leg validity condition**, joining A31's four. Two legs are comparable only if their focus condition matches. **A leg is discarded for HOW IT RAN, never for WHAT IT SHOWED** — which is why auto-retry is sound |
| **A64** | **A delivery-pair comparison requires a pose-match precondition on the PAIR**, not merely per-leg B1 admissibility. Two legs can each pass B1 and sit in *different* admissible poses — 0.35° apart moved `coverage_ratio` ~1.9% and read as a divergence. `coverage_ratio` is the indicator: **a discriminator for the comparison, never a gate** |

**Also ruled, not numbered:** ≥3 counted events per leg is a **validity condition** (the settle window scales with
frame time, so a deep enough stall can drop a leg below the bar while still reporting ALL-ALIGNED); A40 bands are
compared at their own stated precision.

---

## 5. Gotchas landed: G97–G109

The ones this doc depends on:

**G97** MCP bridge attaches to whichever editor listens on :12029 — HeistCrewUE / 5.7.4 silently captured it.
**Permanent environmental fact of this box, not an incident** (the owner runs a second UE project).
**G98** `AffectedFrames` is a **projection-filtered set, not a frame range** — gaps are possible.
**G99** `make_gate_level.py` is **destructive by default** and the asset it destroys is the frozen gate instrument.
Now refuses unless `--allow-overwrite-frozen` is passed. *General form: a tool whose normal mode deletes a
load-bearing artifact should require the DESTRUCTIVE intent to be stated, not the safe one.*
**G100** the Renderer **private** include path is engine-layout-specific, non-Shipping only; the break surfaces as
a compile error inside our module, far from the `Build.cs` line that caused it. **The odd-looking
`class FViewInfo;` forward declaration must NOT be tidied away.**
**G101** `IAI.Capture.Start`'s `outDir` is **CWD-relative**, not `Saved`-relative — output lands beside the exe and
"the run did not write" is the wrong conclusion.
**G102** **the stolen `else`** — read the next token after a `}` before appending; prefer anchoring to the START of
the following construct; a block whose `else` is destructive deserves restructuring, because a silent re-parent is
unreviewable at the diff level.
**G103** staging a code-only change is an **exe hot-swap** (~85 s build + one copy, no cook, G92's archive-wipe not
in play). Two things it does not excuse: **the hot-swap IS the stage step**, so A44-scan the *staged* artifact; and
back up the previous exe when it is a baseline you still need.
**G106** analysis instruments that grade a certified result are **committed artifacts** (§3.3).
**G107** A57 recurring on the **pose** axis — the calibration set bracketed the phenomenon without containing it,
so no pose-invariant floor was derivable and none was noticed missing.
**G108** **a stalled process fails foreground activation.** A thread busy-waiting 99 ms/tick reads as unresponsive
and Windows refuses `AppActivate`. *General form: an "environmental" halt that only fires on the slowest
configuration is not environmental.*
**G109** **a frame-count threshold cannot generalise** where frame time varies 3× (the 30 s gate expires at ~900
frames nominal, ~299 at stall 99). **Time is the invariant; frames are not.**

*(G104/G105 landed but are not load-bearing for anything in this doc — read `docs/gotchas.md`.)*

---

## 6. The phenomena ledger — P7 and P8 added

Existing: **P1** (client's −1, unreproduced) · **P2** (signature absent) · **P3** (fixed by m23) ·
**P4** (permanently retired, number never reused) · **P5** (spread/decay at high fps, open) ·
**P6** (annotation.json field contracts, open).

| # | Name | Status |
|---|---|---|
| **P7** | **A54 margin scale is regime-dependent (fps).** ×0.98 where the signal is sharp (30 fps), ×2.04–2.05 where it is spread (60/120 fps) | **OPEN.** Verdicts unaffected; **decidability annotations affected.** Leading hypothesis **named and NOT adopted**: mean-of-claimed vs flank-mean, versus a per-frame formulation that would penalise a mixed claimed set harder. Untested — testing it is a definition change needing its own gate |
| **P8** | **TAU is not camera-pose invariant** (§3.5) | **OPEN**, mitigated by B1. B2 filed and now gateable against eight controls |

**P7 ↔ P5 adjacency, recorded and NOT merged:** both are about **spread-signal behaviour at high fps** — P5 in the
pixels, P7 in the statistic that measures them. **They may be the same phenomenon from two sides.** Whichever is
investigated first must check the other. **The blend-ladder now serves both**, which raises its priority.
**P8 is NOT on that list** — its axis is *pose*, and a blend-ladder manufactures spread, not pose. Two items, not
three.

**P6 widened** to *"annotation.json field-contract defects"*, no new number (A61 applied to phenomena): `node.bounds`
**settled**; `camera.path` **open** (naming); `coverage_pct` vs `coverage_ratio` **open, predicted from source and
NEVER MEASURED**, and only manifests under H4's exact condition.

### The P1 narrowing — this outranks the green

> **P1 had two live leads. One is now eliminated by measurement.** Delivery mode changes nothing in the annotation
> contract, so whatever produced the client's −1, **it was not delivery mode**.
>
> **H1 (GPU-load starvation shape) is P1's only remaining named lead. H1 has no lever in existence.**
>
> **If H1 also comes back clean, P1 has no named leads.** Stated here and in `CLAUDE.md`, deliberately, while a
> queue still sits in front of it.

Lever design for H1 remains **chat-side first, and never same-turn as its first measurement.**

---

## 7. Errors on the record — both sides

### Chat-side
1. **C1's unsatisfiable byte-identity rule** (§3.8) — over-corrected to close an argument, into a rule nothing can
   satisfy.
2. **A stale calibration figure carried forward** — briefed stall 34 as the client band from the superseded gate-env
   handoff; journal 031 had re-measured it at 1.0558 months earlier. *Generalisation adopted: cite the **journal
   that last measured** a figure, not the handoff that summarises it. Summaries go stale; the measurement record
   does not.*
3. **The S3a-1 cvar contradiction** — required both console vars in the slice *and* required inert-by-construction.
   Mutually exclusive. Code took the conservative branch and flagged it (A58's corollary).
4. **"A named CB_ target"** — unsatisfiable in a package; actor labels are editor-only (G91).
5. **Bifurcation rate stated as ~1-in-12** — measured at **2-in-5**, both instances on **stalled** legs.
6. **Prioritisation churn** — the delivered-session audit was argued ahead of P5/S3 on client urgency that then
   evaporated; the occlusion recon was justified as possibly changing S3's requirements, which it does not.

### Code-side, all self-corrected before causing damage
- The **overlap = 0 metric was ambiguous** — it reads identically for "never tested" and "tested and handled
  perfectly", because a correctly dropped positive vanishes from files *and* claims. Caught **inside an already
  accepted, already committed result**, then the whole N=4 phase space was exhausted rather than stopping at the
  first hit. Sound discriminator: **did the claimed set shrink against the clean run.**
- **The publish-count explanation** ("dropped frames stalling the phase machine") was wrong — it was the focus-gate
  wait. Corrected, *and* immediately checked whether any verdict depended on it. None did.
- **The 1010 cube** was diagnosed as a spring-arm/camera union; the real cause is `UDrawFrustumComponent`'s
  hardcoded `10 + 1000`, and it is **editor-only** (`WITH_EDITORONLY_DATA`).
- **"The deep leg could not acquire focus" was called environmental** — it was the harness (G108).
- **A compliance failure reported unprompted:** two bifurcated legs were overwritten by their re-runs despite an
  explicit instruction to bank them. Measurements survive; **pixels are lost.** Bounded, because the pose control
  B2 needs (L3_client39) is already banked. Harness now banks every attempt before any gate.

**Both sides putting their own errors on the record is why the corrections sections are where a cold reader should
look first.**

---

## 8. H4 — filed, designed, unrun

**The hypothesis:** `ProjectActorBoundsToScreenRect` runs **no occlusion trace**, while selection
(`IsComponentRenderableVisibleInternal`) is occlusion-aware. **So a target on-screen but fully occluded projects
successfully and is labelled positive while contributing no pixels** — a labelled-but-invisible sample, from a
mechanism entirely separate from P3b, in current shipping code.

**Status: NAMED, NOT ADOPTED.** Read from source; never observed producing an instance.
**Routed to `feature/stencil-capture`** — that branch's premise (report actual pixel contribution before hiding) is
its cure, so H4 **strengthens a locked ruling** rather than opening a lane.

**Recon findings (read-only, no mutation):**
- **7–8 already-occluded, already-on-screen targets exist in `CB_GateLevel` as it stands and as already cooked.**
  No re-authoring, no re-cook, no G92 exposure.
- **A47 is a *rotation* bifurcation, not a position one** — eye position invariant on 369/369 samples at
  `CB_PlayerStart`. So **inter-actor occlusion is invariant across the bifurcation** *(does not generalise — holds
  because gate targets are STATIC and the player start is fixed; fails in any level with motion)*.
- Targeted fire reaches an occluded actor; **auto-pool does not** (it routes through `IsUnoccluded`).

**Pre-declared test design:**
- **Target:** any of the ≥94 % on-screen occluded set (`_11, _22, _24, _33, _100, _139`). **Not `StaticMeshActor_5`**
  — 89.2 % sits under A56's 90 % line.
- **Config:** `IAI.SetViewportScoping` **OFF**, effective value echoed (A48). Scoping ON selects nothing and reads
  as a clean null for the wrong reason.
- **Two signatures, and the pairing is the point:** A54 → **ABSENT** (*symptom*, shared with P3, alone discriminates
  nothing) **and** `selection_provenance.json` → **`valid:false`, 0/0 samples** (*cause*, unique to H4 — a P3-style
  non-manifestation has a *visible* target, so provenance reads 9/9 passed, `valid:true`).
- **Control, same run:** a known-visible target must return A54 **ALIGNED** and provenance **9/9 `valid:true`**.
- **Prediction:** label **is** emitted; A54 ABSENT; provenance `valid:false` 0/0; `coverage_pct` 0 with
  `coverage_ratio` > 0.
- **Refutation branch, pre-declared as counting:** no label emitted ⇒ **H4 REFUTED for path (b)**, provided the
  scoping echo confirms the gate was not the cause.

**Licensing ruling — this is the line:**
- **(b) targeted fire on an already-occluded actor LICENSES a mechanism claim** — the projector reads the current
  view and the actor's bounds and has no memory of how the target got occluded.
- **(b) DOES NOT LICENSE an incidence claim** — that H4 causes the client's complaint. Incidence is entirely a
  question about **(a)** (selected while visible, becomes occluded during the window), which **this level cannot
  produce**: everything is `STATIC` and the camera position is invariant.
- **Asymmetry that makes (b) worth running:** a **negative (b) refutes H4 in one packaged run**, and (a) never needs
  building.

---

## 9. Open vs locked

### Locked — do not relitigate
- **B′** as the keying design; key minted at `BeginRenderViewFamily` only; A8 latch rule; A3 never-assert.
- **Stage numbering: S3 = done (m24). S4 = backbuffer demotion. Depth = PARKED, UNNUMBERED. No hole at S4.**
- **The backbuffer path is KEPT, not retired** — UI on/off is a grab-point choice.
- **A59–A64**; A63's canonical wording; the ≥3-counted-events validity condition.
- **Ratio-independence is load-bearing.** No client is ever told their machine is too slow. The internal ship rule
  is telemetry only.
- **Content-clock default = wall.** Tested. Do not flip.
- **The A54 oracle is certified at 30 fps only** — the scope block is in `a54_oracle.py`'s own header, in the file,
  because the original tool was lost precisely by living in prose.
- **CaptureBench local-only, permanently.** `feature/stencil-capture`: **do not rebase** — mine Stage 3a on current
  master.
- `IAnomaly` **LOCKED** since M1.
- **No force-push, fix-forward.** A commit message carries a stray BOM in the log; it stays. An unamendable blemish
  beats a clean log with rewritten history.
- **Virtual-desktop focus isolation is rejected** and the reason is recorded (§3.9).

### Open
- **P1** — unreproduced, unfixed. **H1 is the only named lead, and it has no lever.**
- **P5 / P7** — open, adjacent, not merged. Blend-ladder assigned, not built, now serving both.
- **P8** — mitigated by B1; **B2 filed, gateable against eight controls.**
- **P6** — `camera.path` naming; `coverage_pct` vs `coverage_ratio` (predicted, never measured).
- **H4** — filed, designed, unrun.
- **A11** — **STAYS OPEN.** No natural ring miss occurred at deep or pacing-off. Recorded precisely: *the design
  prevented the condition A11 wanted to observe.* That is a satisfying reason to leave a debt open and is **not**
  written up as a discharge. `wrapped > 0` does not close it; `ForceMiss` counters are synthetic and never will.
- **A4 Condition 1 (VP equality)** — **UNSATISFIED.** ViewRing / `ViewLagFrames` deletion contingent; keep a bisect
  switch when it lands.
- **A17/A19** retroactive audit — paper only, still pending.
- **The two lost bifurcated legs** — recorded as lost, not silently dropped. **Do not hunt a replacement**; the
  harness now banks every attempt, so the next one arrives free.
- **`CHAT-HANDOFF-m10-m21.md`** — never written; a separate standing debt. Do not attempt to reconstruct it.
- Client-facing: **channel closed.** Reply unsent and unsendable; invisible-anomaly fix unscoped; resolution /
  JPEG / defaults profile unbuilt.

---

## 10. Corrections — discard this stale understanding

- **"S4 = depth"** → **no.** S4 is the backbuffer demotion; depth is **PARKED and UNNUMBERED**.
- **"The office-machine `target_fps` audit / the cap-at-30 client note are pending"** → **STRUCK.** No client-facing
  action item survives anywhere in the plan.
- **"The delivered-session auditor is planned"** → **CANCELLED**, closed, not outstanding.
- **"The delivery-mode gap is a live P1 lead"** → **ELIMINATED by measurement.**
- **"Gaps in a claimed set prove it is genuine"** → **false.** G98: `AffectedFrames` is projection-filtered and can
  gap.
- **"The A54 oracle exists as a tool"** → it did not. Rebuilt and committed this session.
- **"Journal 031's analysis windows are camera-settle windows"** → **neither camera nor luminance.** A hand-chosen
  per-leg mixture.
- **"I10's null rests on a mechanical analysis"** → **no.** See §11. **WEAKENED, not overturned.**
- **"`overlap = 0` proves a dropped positive was never exercised"** → **ambiguous metric**; it is also the pass
  condition.
- **"The 1010 cube is a spring-arm/camera union"** → **no.** `UDrawFrustumComponent`'s hardcoded `10 + 1000`,
  admitted via `bNonColliding`, centred on the camera **by construction**.
- **"P6's 1010 cube is client-impacting and it ships"** → **downgraded, leaning false** — the frustum component is
  `WITH_EDITORONLY_DATA` and compiled out of packaged builds. **Unconfirmed** (the corpus is confounded); one
  packaged run would settle it. *What survives in both configs: the union still admits the collision capsule, so
  `node.bounds` is still not the mesh bounds — by a capsule, not by 1010.*
- **"Bifurcation is ~1-in-12"** → **2-in-5 measured**, both on stalled legs (**association only** — no mechanism
  adopted without measurement).
- **"S3a-2's re-gate green was robust"** → it *was* (verified retroactively), but it had **never been robust to a
  variable nobody had identified**. A63 now covers it.
- **"m24 means the capture migration is finished"** → **no.** S4 (defaults flip) is still outstanding, and it is a
  **client-visible change**.
- **"S3 going green closes P1"** → **no**, and it never will. You cannot demonstrate a fix for something you cannot
  summon.

### P6 contract ruling — LOCKED, not yet implemented

> **`node.bounds` must be render-relevant bounds** — the union over components that contribute drawn pixels — **not**
> a whole-actor union admitting collision capsules and visualisation primitives. **Reuse the existing renderable
> definition** (`IsRenderableComponent`, StaticMesh-or-SkinnedMesh, G33) so label geometry and selection geometry
> agree on what "the object" is. A box larger than the visible object is a diluted label, and dilution is the same
> family of harm as a labelled-but-invisible sample.

Parked as a **milestone candidate**. Do not relitigate; do not implement without a plan.

---

## 11. ⚠ OUTSTANDING BRIEF — the I10 window re-check

**This brief is with Claude Code and has not been verdicted.** Its pre-declared predictions live only in chat, so
they are reproduced here **verbatim**. A fresh session must restate them before reading any result.

**Why it exists:** I10's headline is *"CPU starvation is REFUTED as a cause of P1"* — a **load-bearing null**. It
was graded using **hand-chosen per-leg analysis windows** (§3.6), which is an unaccounted degree of freedom, and
the direction of risk for a null is that a window chosen *after* seeing a leg could exclude the frames where a
defect would show.

**Method:** re-run the six banked I10 legs through the **mechanical** settle rule (`0.5` / `K=5`, frozen) and the
**certified** oracle, unchanged. All six are 30 fps, so the oracle's certified range covers them and P7 does not
apply. **Paper only, banked data, no runs, no code.** Constraints: do not re-tune the settle constants; apply B1
and A64; in-leg positive control both directions on every certifying leg, reported before its verdict; ≥3 counted
events or the leg is INVALID.

> **I10's NULL SURVIVES.** The mechanical window returns ALL-ALIGNED on every certifiable leg, and CPU starvation
> remains refuted as a cause of P1. Rationale: the hand-chosen windows were chosen to exclude camera settle, not to
> exclude defect evidence, and no mechanism has ever been proposed by which a settle window would hide a one-frame
> shift.
>
> **IF A LEG RETURNS SHIFTED OR ABSENT UNDER THE MECHANICAL WINDOW, THAT IS A MAJOR RESULT:** I10's refutation
> would be overturned or narrowed, CPU starvation would return as a live P1 lead, and P1 would go from ONE named
> lead to TWO. Reported as a finding, not as a failed re-check.
>
> **IF LEGS LAND NOT-CERTIFIABLE**, the null is NARROWED to the legs that certify, and the rest are honestly
> unjudgeable — not evidence in either direction.

**This re-check cannot disappoint.** Null survives ⇒ I10 rests on a mechanical rule and the weakening is
discharged. Null breaks ⇒ we recover a P1 lead that twelve legs had closed off, **and we recover it before building
an H1 lever on the assumption that CPU starvation was settled.**

`CLAUDE.md` and journal 042 flag this with the explicit words **"do not let this tag bury it."**

---

## 12. Forward plan

**Owner ruling: S4 next, after the I10 re-check is verdicted.**

1. **Verdict the I10 re-check** (§11). It is already briefed; do not re-brief it.
2. **S4 — backbuffer demoted to the UI-on option, defaults flipped, client config.**
   **⚠ This is a CLIENT-VISIBLE CHANGE, not a silent default flip.** The pre-Slate SVE grab is **UI-free by
   construction**, so flipping the default **changes delivered image content**. It happens to satisfy a stated
   client ask (UI excluded) — but plan it as a visible change. *Note: nothing reaches the client without a
   deliberate send, and the channel is closed, so this blocks nothing.*
   Also folded in at S4 time: measure and record the **actual** SVE-view-rect vs backbuffer-window-rect resolution
   delta on a non-trivial config (S3a measured `dW=0 dH=0` at 1280×720 windowed — **not generalisable** to
   DPI-scaled or letterboxed configs).
3. **H4** — one packaged run, per the design in §8. Cheap, aimed at the client's #1 complaint, needs nothing from
   her.
4. **P5 / P7 — the blend-ladder.** Serves two open items. Discriminators pre-declared chat-side **before** it is
   built.
5. **P1's H1** — GPU-load lever. **Design chat-side first, never same-turn as its first measurement.**
6. Later / unordered: **B2** (scale-free separability, eight-control gate, may reopen m23's DA60 floor);
   **P6 bounds implementation**; A17/A19 audit; resolution selection / JPEG / defaults profile.

---

## 13. Working agreements — carry these verbatim

> ⚠ **SCOPE — READ THIS BEFORE APPLYING ANYTHING IN THIS SECTION.** This section describes
> **chat-Claude's output conventions to the owner**. **Claude Code does not emit the 🔴 marker and does
> not route decisions to the owner; unresolved calls go to chat-Claude** — flagged in plain text, as
> loudly as the call deserves (e.g. `CHAT-DECISION REQUIRED: …`). Plain-language summaries are
> mandatory for chat-Claude and **optional** for Code. *(Added 2026-08-19: a fresh Code session read
> this section as its own contract and duly emitted 🔴 markers and owner-addressed decisions in a plan.
> The section was correct; it just never named its addressee. → **G111**.)*

**Roles.** Kavin is tech lead and project owner. He does **not** write code and does **not** make technical
decisions — both are fully delegated. **Chat-Claude makes all design and technical calls** and writes the exact
paste-back blocks. **Claude Code implements** against approved plans. Kavin ferries messages, runs smoke tests, and
applies executive judgement **only** on genuine product/scope tradeoffs.

**🔴 THE RED-CIRCLE RULE (owner-requested, mandatory).**
Kavin treats this project as obligatory work and **skims rather than reads**; his attention is on a separate
personal project and he trusts chat-Claude to engineer the solution. So:

> **Mark with 🔴 every item he MUST read, and nothing else.**

Only three things earn the marker:
- **ACT** — a smoke test, command, or machine-side task only he can do.
- **DECIDE** — a genuine product/scope tradeoff that is not chat-Claude's to make. **Always a closed choice with a
  recommendation attached. Never an open-ended "what do you think?"**
- **HEADS-UP** — something that changes what ships to the client, or a risk chat-Claude has accepted on his behalf
  that he would be annoyed to discover later.

**Marked items go at the TOP of the reply**, before the summaries, so he can glance at the first two lines and know
whether he is needed. **A reply with no 🔴 means he can ferry the paste-back block and read nothing else.**
Unmarked content stays in full regardless — the summaries are for the record and for cold readers, and paste-back
blocks stay as verbose as Code needs. **The marker never becomes a route to hand him implementation choices.**

**Plain-language summaries are mandatory in BOTH directions, every exchange, however routine.** When Kavin pastes a
Code response: lead with a short scannable jargon-free rundown of what Code did, **then** the formal verdict,
**then** a clearly-marked paste-back block. When the reply contains a brief for Code, **end** with a short summary
of what was proposed plus the **Claude Code effort setting**.

**Effort settings:** `xhigh` planning/design/debugging · `high` mechanical implementation · `max` rarely ·
`ultracode` large multi-stage batch jobs only.

**Discipline.** Plan-before-code (file-by-file plan, wait for approval). Stage gates with concrete thresholds.
**Stop-on-failure — any validation miss halts the turn for a verdict, no same-turn fixes.** Predictions
pre-declared before the instrument exists and restated **verbatim** before results, including what a clean result
would mean. Numbers never reused. **Measure then design** — chat-Claude must not write a confident mechanism into a
brief without Code's measurement. Conventional Commits; one milestone one commit; Code commits **and** pushes
including tags, reporting what it is about to push for the record, not for approval. **Never force-push.**

**Operator contract for packaged runs.** Click freely and use the other editor — A63's harness detects focus
steals, discards and re-runs automatically. **One caveat:** for the first ~10–35 s of each leg the harness fights
for foreground and wins, **partly by synthesising an ALT keypress** — so do not type anything you care about during
leg startup. Once capturing, it leaves focus alone.

---

## 14. Pointers

**Plugin repo:** `CLAUDE.md` (current-status block, refreshed at every milestone close) · `docs/gotchas.md`
(**G97–G109** new; G43, G76, G86–G96 relevant) · `docs/sessions/` **journals 035–042** (035 = P6 settled + auditor
premise halt; 036 = auditor cancelled + H4 recon; 037 = S3a plan + rulings; 038 = the stolen-else diagnosis;
039 = S3a-2 re-gate; 040 = the focus confound; 041 = A63 + certification; 042 = **the S3b matrix, the debts list**)
· `docs/capture-fps.md` · `docs/client-delivery.md` · `docs/architecture.md` · and the three S2 companion handoffs.

**CaptureBench (local-only):** `tools/a54_oracle.py` (**read its header block first — it states its own certified
range**), `check_pose.py`, `run_leg.ps1`, `eval_leg.py`, `make_gate_level.py` (**destructive by default, now
guarded — G99**), the marker decoder.

**Banked evidence:** `D:\IntrusiveAnomalies\_bench_sessions_bank` — 58 dirs. Notable:
`S3B_L{1..5}_*` (the certified matrix) · `S3B_S1_MARKER` · the three A63 focus-timeout discards ·
`I10\L1_nominal…L6_client40` + `I10HF\HF1_nat120` (the oracle's known-answer controls) ·
**`I10\L3_client39` — the known-ALIGNED bifurcated-pose control that makes B2 gateable** ·
`NEG2\session_20260816-183524` (the post-m23 guard control, rescued from the wipe path).
**Re-bank before any staging step (G92).**

---

**Standing lessons, with this session's receipts:**
**Only known-answer controls expose instrument blindness** — five instances now, and the fifth was the sharpest: an
oracle that agreed on every verdict and was wrong about every confidence.
**Eyes, then number** — reading the raw series before reporting caught a false ABSENT on the client band.
**The artifact on disk is the gate** — two identical, perfect logs; one deleted its session.
**A guard that has never fired is not a guard** — and neither is a gate that has never run.
**Hunt before you build** — and **reproduce published numbers, never published conclusions.**
