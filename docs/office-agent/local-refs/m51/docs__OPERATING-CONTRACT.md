# OPERATING CONTRACT — AnomalyInjector (GDP) — seeded 2026-09-06 by chat Claude, owned by the orchestrator from here on
One authoritative page. When this file and older docs disagree, THIS file wins; fix the older doc. `CLAUDE.md`'s status block remains the "you are here"; this is the "how we work". Change it only by a dated line under §9.

## 1. Roles
- **Owner (the owner):** product/scope rulings, playtests, physical bench actions, anything that touches the office PC. Does not write code, does not make technical calls.
- **Orchestrator (chat Claude, from 2026-09-15; Codex pilot 2026-09-06 → 2026-09-15):** designs, writes briefs, reads every report in full, rules on technical questions, owns the brief queue and the checkout. ONE orchestrator per mailbox at a time — the contended resource is the bench checkout.
- **Implementer (Claude Code via the mailbox):** implements against an approved plan; measures; may refuse a faulty premise with a measurement and STOP with NEEDS-DECISION. Runs all git on Windows. Never pushes tags.
- **Independent reviewer (Codex, from 2026-09-15):** invoked at CRUCIAL CHECKPOINTS ONLY, named by chat — not on every brief. Reviews in writing under `D:\IntrusiveAnomalies\_reviews\`. Issues no briefs. *(2026-09-06 → 2026-09-15 this role was chat Claude's, as the adversarial reviewer during the Codex pilot; the two roles swapped, they did not disappear.)*

## 2. Hard rules (no exceptions, no rewording)
- ⛔ **Office data never leaves the office PC.** Capture sessions, frames, logs, screenshots, client-provided data stay there. Only the owner's typed transcriptions of readings come home, into `_bates_reads\`. Anything that must run on office data runs on the office box and the owner reports numbers.
- ⛔ **Codenames only:** Concorde, Bates, Europa. Never a real title, studio, fork name, the office's own codenames, or any mapping — in briefs, docs, journals, memory, reviews.
- ⛔ **Nothing from `_mailbox` temp files into the repo; nothing temp into `_mailbox`.**
- ⛔ **No git write from a Linux mount** (CRLF repo). Code does git on Windows.
- ⛔ **Tags batch at the office.** Code never tags. Pushes to origin are allowed and expected after a green merge.

## 3. Schema and label meanings (client-facing — changing a meaning is an owner + client decision)
- `label_schema` 2 is what the client has (Bates M2, Concorde re-issue pending). `affected_frames` = observable subset; `injected_frames` = all frames the injection existed; `observability_measured` = Measured > 0; fallback at ZERO measured frames: `affected_frames = injected_frames`, `observability_measured:false`; zero-observable events stay in the file.
- **Schema 3 (m51) = schema 2 + ADDITIVE keys only.** No key changes meaning. New: `event_id`, `injection_id`, `observability_basis`, `observability_complete`, `frame_observations`, `capture_complete`, `requested_frames`, `written_frame_indices`, per-row `schema_version`, `condition_requested`; empty windows start/end −1. `anomaly_present` keeps meaning "injection present"; `visible_positive` = observable-only. A fallback window is injection-labelled data, not verified visible ground truth — the README says so.
- Evidence vocabulary: `target_pixels` = front-most drawn pixels of the target (silhouette); `target_drawn_pixels` = target itself drawn (evidence of ABSENCE only); `observable` true/false/null; `-1`/null = NOT MEASURED, never clean, never positive; `mask_state` present/empty/unmeasured; `render_state: shaders_pending` ⇒ no positive on that fire.
- Renegotiating any meaning = the **M3 contract**, agreed with the client, with a migration note.

## 4. Rulings in force for m51 (R1–R8, amended 2026-09-06 — full text in `_reviews\2026-09-06-review-of-fix-capture-label-integrity.md` §3 + §5)
R1 additive schema 3, fallback only at zero measured, `observability_complete` new · R2 Nanite rule `NaniteOnly == Renderable` · R3 exact targeting name-OR-class, plus path, log ambiguity · R4 WebSocket mutations refused during capture, console mutations allowed + counted · R5 shader-pending per FIRE, capture and auditor · R6 bounded writer pool, ≤ 1-frame wait then counted drop, mask failure downgrades row AND event, frame still commits · R7 readback cap knob (32), counted drops, zero-drop gate · R8 stale arms rejected with immediate snapshot cancellation + `unrendered_ticks`, can-fail = recovery after > cap missed ticks. Phase B behaviour survives, not just its keys.

## 5. Gate bar (a change to capture is not done until all of this is true)
- Predictions file written BEFORE the first leg; amended only with dated AMENDMENT blocks.
- Every gate in BOTH tick orders (natural, synth), on BOTH fixtures (StackOBot bench, Lyra worktree), on ONE binary whose hash is recorded; A-side archived and byte-identical.
- A can-fail lever per gate, fired in both orders; a certifying leg with < 3 counted events is INVALID.
- Regression set: ONSET, PAIRING 100/50, G7, MASK-TIE, TARGET-PIXELS TIE, G-EDGE ×8, OBS-1..4, m50 G1/G2/G6, PB-1..PB-8, black-frame, A44, P-C7 v3 (declared keys only, `frame_indices` identical), Lyra LG set + verifier report-only. m51 adds one leg per review finding, throughput at 1080p native, auditor `--integrity-only` on banked schema-3 sessions.
- Shader change ⇒ ONE cook, quartet recorded, re-baseline proving nothing outside declared keys moved. Same source rebuilt ⇒ NOT re-staged (G201).
- Journal per session; CLAUDE.md status block; gotchas append-only and G-numbered; Conventional Commits, one milestone = one merge.

## 6. Mailbox protocol
- One brief in flight. Header: `# SESSION: new|new-default`, `# NAME:`, `# EFFORT: xhigh` (Opus 5 default; never medium/low; Fable 5.1 crucial-reasoning only; ultracode = large batches only), `# MODEL:`, `# ACK-VERDICT: <resolution>` when `last_verdict.txt` is NEEDS-DECISION/RED.
- Report line 2 = `verdict:`; Code ends its final message with a literal `VERDICT: GREEN|NEEDS-DECISION|RED`. **A NEEDS-DECISION or RED report is not acceptance whatever the exit code.** Read every report in full before the next brief.
- Roll the Code session at ~70 % context. Journal + CLAUDE.md before every session end.
- Local orchestrator waits on the report file / `watcher_status.txt`; cloud sessions use the signal relay (E:\MailBox\README.md).

## 7. Escalation boundaries
- **Ask the owner:** any client-facing meaning change; any delivery/timeline commitment; anything requiring the office PC or a physical action; Europa/Bench scheduling; spend outside the normal pattern (a Fable 5.1 brief, ultracode).
- **Orchestrator decides alone:** design, sequencing, technical rulings, gate design, brief content, session rolls, when to STOP a thread.
- **Reviewer must see before merge:** any change under `Source/AnomalyCapture/`, shaders, the label schema, the verifier/auditor.
- **Never silently:** reverse a measured ruling (§3–§4), skip a gate, widen a brief's scope fence, or edit a prediction after its leg.

## 8. Standing facts
Repo `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector` (master, origin exists) · CaptureBench sibling repo · Lyra worktree (never a junction) · bench binaries `_binary_baselines\` · legs `_bench_sessions_bank\` · reviews `_reviews\` · office reads `_bates_reads\` · dashboard `E:\MailBox` (tray app, toasts) · engine UE 5.1 source build; Europa = UE 4.25 (parked).

## 9. Change log
- 2026-09-06 seeded by chat Claude at the Codex-pilot handoff (m49 complete `ac13700`; m51 next).
- 2026-09-06 **added to version control** and adopted by the orchestrator (brief 078-01). Body unchanged.
- 2026-09-06 **brief 078-01 (m51 preflight)** — branch `m51` cut from `ac13700`; source branch
  `fix/capture-label-integrity` (`be16977`) backed up to origin; all sixteen review findings
  re-verified at `ac13700`; predictions written as DRAFT pending the reviewer. Journal
  `docs/sessions/2026-09-06-078-m51-preflight.md`, predictions
  `docs/predictions/2026-09-06-m51-release-hardening.md`.
- 2026-09-06 **brief 078-02 (predictions revision).** Two written reviews returned CHANGES REQUIRED;
  **AMENDMENT 1** of `docs/predictions/2026-09-06-m51-release-hardening.md` is the active
  specification and §§0–8 of that file are the superseded reviewed snapshot. Journal
  `docs/sessions/2026-09-06-078-02-m51-predictions-revision.md`. Rulings now in force:
  - **R1/F6 — the m26 event veto is DELETED.** Zero-observable and measured-zero events STAY in the
    file (contract §3). `vetoed_events` is retained as a compatibility key reading **0** with a
    changelog line; NEW `zero_observable_events` = retained events with >=1 committed injected-frame
    measurement and no observable injected frame — **a fully unmeasured event is excluded**, never
    counted as a measured negative. Empty-window `start/end = -1` is a **declared value change**, not
    an additive key.
  - **The observable predicate is master's own** (`AnomalyCaptureSubsystem.cpp:3880`):
    `labelled && held && target_pixels >= ObservableMinPixels`, after the unmeasured early-out.
    **`target_drawn_pixels` neither establishes nor vetoes it** (`G258`), and the configurable
    threshold is never replaced by a hardcoded `> 0`.
  - **`capture_complete` is a reconciliation**, not a conjunction of drop counters: requested indices
    vs committed receipts, every required artifact of every receipt, all drop counters zero, every
    accepted request terminal, and all three final JSON writes successful. `run_summary.json` is
    written **last** via tmp+rename so the field never depends on its own write; if it is absent the
    auditor reports FAILED.
  - **An intentionally unavailable measurement is NOT incompleteness** (Nanite, mask off, backbuffer,
    `OutputHeight != 0`, shaders pending). A **promised artifact that failed** is.
  - **A console lever absent from an archived executable is not a device.** A regression that exists
    only in the candidate is falsified **in-candidate**, against a healthy control on the same binary.
  - **Auditor exits:** 0 valid complete schema-3 · 1 schema-3 integrity failure · 2 malformed/missing
    · **3 NOT APPLICABLE (valid older schema)**. 2 and 3 are distinct. `verify_capture.py` remains the
    gate for v1/v2 and for the pixel/visual check; the auditor does not replace it.
  - **A gate set with an OPEN arm may not be summarised as "all required gates pass."** Currently
    OPEN: the R8 applied-tick-pin arm (`tickpin_compiled == false` on both fixtures), real
    decoupled-host behaviour, mixed-Nanite actors, and M-F4 if the editor-viewport route fails.
  - **Provenance:** m51's runtime A-side is **`B8AC65D8` + the Phase B container**; **`A159EDFD` + the
    m46 container** stays the load-bearing historical anchor. **A Lyra baseline is a module SET plus
    its `UnrealEditor.modules` manifest, never a plugin checkout SHA.** No candidate hash is recorded
    before the binary exists; the one-binary rule applies **per fixture**.
- 2026-09-06 **brief 078-03 (bounded prediction corrections).** **AMENDMENT 2** of
  `docs/predictions/2026-09-06-m51-release-hardening.md` supersedes A1.5's completeness conjunction,
  A1.6's stale expiry, **A1.7 entirely**, A1.8.1's falsifier scope, A1.9's saturation device and three
  of its falsifiers, A1.9's "no separate manifest", A1.11's leg classes and A1.13's deferred Lyra
  hashes. New file `docs/predictions/2026-09-06-m51-gate-manifest.json` is the **single execution
  inventory**. Journal `docs/sessions/2026-09-06-078-03-m51-bounded-corrections.md`. Still a DRAFT —
  no leg, no implementation. Rulings added:
  - **Stale arms expire by RENDER IDENTITY, not by readback capacity.** `StaleArmMaxTicks` is
    DELETED. Each arm carries its `GFrameCounter`; publish rejects a mismatch and a GT sweep runs
    before admission and before the cap check. **Published GPU work is never expired by the sweep**,
    and capacity is retired **exactly once**. Measured: `ConsumeWantedForPublish`
    (`AnomalySveCapturer.cpp:45+`) pops FIFO with **no frame comparison at all** today, so A1.6 spoke
    of keeping a check that does not exist.
  - **The writer bound is deterministic and the soft reservation is REJECTED.** `N=2`, 256 MiB
    including in-service jobs, reserved from source format and dimensions. **No `EncoderReserveFactor`
    and no oversize-admission exception.** Invalid configuration is refused at `StartRun`; an accepted
    request that cannot fit is a **counted failure**, never an over-budget admission.
    ⚠ **The byte cap bounds plugin-owned buffers only** — `IImageWrapper::GetCompressed`
    (`IImageWrapper.h:164`) returns a whole `TArray64<uint8>` with no size hint, maximum, streaming
    variant or allocator hook, so **an a-priori bound on encoded output is not obtainable through that
    interface**; codec scratch is bounded structurally by `N` and input size, and a true byte bound is
    an **OPEN implementation prerequisite**. No cap on process/GPU/allocator memory is claimed.
  - **`mask_write_required` comes from the EFFECTIVE artifact policy, before the write** — not from
    "armed". An empty or unmeasured mask promises no PNG and its absence is **not** an artifact
    failure; a failed *required* write does **not** clear its obligation by downgrading the row.
  - **`capture_complete` is prepublication readiness, not publication success.** It never claims its
    own write; an absent or failed `run_summary.json` is a FAILED session to the auditor.
  - 🚨 **A1.5's membership claim is corrected by measurement.** `AccumulateFrameEvents` runs in the
    **drain**, so an armed-but-never-drained frame is in **neither** ledger; and the R1 zero-measured
    fallback leaves `affected_frames == injected_frames`, so on an incomplete session it **can**
    contain indices with no image. `observability_basis` separates observable from fallback
    membership; the auditor fails such a session on the **reconciliation** and **must not** demand the
    observable-only rule on a fallback, which would manufacture an R1 contradiction.
  - **The membership sampler is `IsFireLabelledThisFrame`** (`AnomalyCaptureSubsystem.cpp:4620`),
    frozen per fire per armed frame at `OnWorldTickEnd`. It is preserved unchanged; "an injection
    exists" is not the condition.
  - **A second simultaneous capture context is REFUSED at `StartRun`** — the key ring and the tag
    state are process-global singletons and cannot be isolated.
  - **Three A1.9 falsifiers were WRONG, not missing:** `NoTagRetire` cannot manufacture
    cross-attribution (it prevents reuse); forcing both fires pending cannot discriminate per-fire
    from frame-wide; synth order is not a view-binding bypass. **M-F11 and M-F10 have no defect
    injector and are recorded MISSING.**
- 2026-09-06 **CORRECTION to §3 / the 077 handoff, recorded not retro-edited.** The handoff's §1
  still reads *"Hide-class `observable` = labelled && DrawnCount == 0 && silhouette in view"*. That
  predicate was **REMOVED in `1c3d62c`** and the same handoff's §2 records the refutation.
  **`target_drawn_pixels` and `frames_drawn_unexpected` are DIAGNOSTICS and MUST NEVER become label
  inputs** (`G258`): `drawn == 0` is evidence of ABSENCE; `drawn > 0` establishes nothing. Measured
  in both directions on two fixtures — StackOBot read `drawn == count` on a frame whose pixels
  proved the object absent; Lyra read `drawn > 0` on 7 rows whose pixels proved the hides took.

- 2026-09-06 **078-04 + 078-05 — the `F1` candidate campaign. BUILT, GATED, MEASURED; NOT merged,
  NOT pushed, NOT tagged. `master` unchanged at `ac13700`.** Journal
  `docs/sessions/2026-09-06-078-05-m51-f1-candidate-campaign.md`; evidence
  `_bench_sessions_bank\M51_F1_EVIDENCE\`. Candidate exe **`D50DDE78`** (A-side `B8AC65D8`),
  container byte-unchanged. Rulings and corrections recorded:
  - **The certifying M-F1 arm passes in both tick orders.** Native `TargetMask 1` + `Mask 0`, B1
    live and accepted on attempt 1: **33 of 90 frames written and 57 dropped becomes 90 of 90 and
    0**, `NEVER RESOLVED` 33 → 0, events 3 → 8, exactly one one-shot F1 warning, and the schema
    shape is unchanged (13 row keys / 12 anomaly keys on both sides) with the honest unmeasured
    representation on every row.
  - 🚨 **MASK-PICTURE-PAIRING FAILS its declared predicate on the candidate — and the PRE-FIX binary
    reproduces the failure exactly, concurrently, on a byte-identical payload.** `PREVIOUS` reads
    9/12/10/17 on the candidate against a banked-baseline 0, but an interleaved control on
    `B8AC65D8` reads `PREVIOUS 10`. Confirmed on a second recipe: cap-90 mask yield is 35 banked,
    **23 on the same binary today**, 25 on the candidate. ⇒ **the split is by TIME/ENVIRONMENT, not
    by binary; the failure is NOT attributable to F1.** `G235` in full: today's A-side is not
    on-baseline, so this compares two failing cells and *"it moved nothing vs the previous binary"*
    is **not** a gate result. **Cause NOT established and NOT chased** (`G120`); the resident owner
    editor (`G97`) is a **named lead, not a finding**. This is why the campaign is NEEDS-DECISION.
  - **A released gate recipe that never enabled its own instrument is not a gate.** The M-F1 smoke
    passed no `IAI.Bench.MaskPairingProbe`, and `m44_pairing_probe.py` **refuses such a leg with
    exit 4** — a refusal to judge, not a pass. Amended and committed **before** any smoke leg
    (`0cb7644`), together with the natural-only SP50 arm (the banked baseline runs SP50 in **both**
    orders) and the pairing frame cap (40, not 90 — `G184`/`G235`). Probe enablement is now read
    back from the run log, never from the typed command.
  - **A killed build leaves an artifact that can be mistaken for up-to-date, and its log may not
    survive.** 078-04's interrupted link left a **398 MB** exe where the whole artifact is ~241 MB,
    with a task output of **10 bytes reading only `[killed]`**. `G164` with the sign flipped: the
    partial was *larger*, and only the size distinguished it. Quarantined in place, never executed;
    removing it from the target path is what forced the Link action.
  - **A session declares its own verdict.** 078-04 ended while its build was still linking and its
    report carried a *derived* GREEN that its own final message contradicted. Nothing was falsified;
    the header was simply not the session's statement.
  - **Two corrections to the 078-04 record:** the A-side has **eight** unique attempts, not ten —
    the accepted copies share a session id with their attempt, verified by session id and not by
    directory name; and the 18–30 written-frame band was **MISSED** at 33 (mechanism exact, rate
    arithmetic pessimistic). Both preserved, neither retro-fitted.
  - **`M-F1b` (backbuffer) stays OPEN and UNRUN.** `bSveCapture` is deliberately NOT in the
    effective predicate — the target mask is a separate scene view extension with its own request
    id and never consults the colour grab point, so requiring it would stop arming masks on the
    backbuffer path where they work. Backbuffer × target-mask has never run on any banked leg.

- 2026-09-06 **078-06 — the MASK-PICTURE-PAIRING failure is NOT recipe drift, and three
  overstatements from 078-05 are corrected.** Predeclared
  `docs/predictions/2026-09-06-m51-pairing-instrument-diagnosis.md` (`265403a`) before any leg;
  evidence `_bench_sessions_bank\M51_PAIR_DIAG_EVIDENCE\`. Production stayed frozen at `6713f51`;
  no build, cook, push, merge, tag, Lyra or office action; H-editor and H-load remain HELD and no
  owner action was requested.
  - **Tool drift is EXCLUDED, and that was established before any leg.** `run_leg.ps1` and
    `m44_pairing_probe.py` are byte-unchanged since before the morning run (CaptureBench `472a409`,
    tracked files clean), and the current checker re-run offline on the immutable banks reproduces
    **both** eras exactly — morning `PREVIOUS 0` ×4, afternoon `PREVIOUS 9/10/9`.
  - 🚨 **A RECORDED-AXIS DIFF IS NOT A RECIPE DIFF.** `_leg_geometry.json` records no `marker`
    field while `run_leg.ps1:214` issues `CaptureBench.Marker`, so the morning legs ran at the
    runner's default `Marker 1` and the 078-05 legs at `Marker 0` — and 078-05's `G184` payload diff
    read "identical on every axis" because it could only compare the axes the file records. The
    difference was then confirmed **from the pixels** (an 8×8 consecutive-frame grid agreeing to ~3
    decimals everywhere except the top strip, 45.797/39.998 vs 0.286/0.668 — `G125`'s signature).
    **Generalises: a payload diff is only as strong as the set of inputs its record captures; state
    what the record does NOT cover, or the diff's silence is mistaken for equivalence.**
  - ➡ **AND THE DIFFERENCE WAS TESTED, NOT ARGUED.** Replaying the recovered morning recipe on the
    same archived A-side bytes today reads `PREVIOUS 9` — **identical to the `Marker 0` recipe**
    (4/9/0 vs 4/9/0, decidable 13, no-data 27, masks 19 on both). ⇒ **the recipe-drift explanation
    is UNSUPPORTED.** ⛔ This does **not** exonerate the instrument in general and does **not**
    establish an environmental cause. **ROOT CAUSE REMAINS OPEN.**
  - **The 078-05 causal claim is WITHDRAWN.** *"The split is by time / environment, not by binary"*
    asserted a cause the evidence does not carry. Corrected everywhere to: the concurrent A-side
    reads `PREVIOUS 10` and the candidate `PREVIOUS 9`, so **the failure OCCURS WITHOUT `F1`** —
    and **neither an environmental cause nor the absence of an `F1` contribution is established.**
  - **Denominators corrected:** 33 of 90 = **36.7 %**, never "about a quarter"; and the A-side's
    **57 are missing REQUESTED indices**, not gaps — the written prefix `0..32` is contiguous, so the
    loss is invisible in the artifact's own index series and appears only in the finalize log line.
  - **`M-F1b` (backbuffer × target-mask) is UNRUN, not "works".** Wording implying otherwise is
    corrected in the status block.
  - ⚠ **Scope deviation recorded rather than absorbed:** 078-05's three post-failure control legs
    exceeded its fence and are **exploratory evidence, not certification**. They were the right
    instinct — they prevented a false attribution to `F1` — so the remedy is a rule: **a future brief
    must PRE-AUTHORISE any post-failure control explicitly, naming its identity, recipe and stop
    condition. It is not a standing right to expand scope.**
  - 📊 **The throughput association is recorded AS an association:** across 9 pairing legs on both
    binaries and both recipes, every leg at `speed_ratio` ≈ 1.00 passes and every leg at ≥ 1.59 fails;
    sustained fps 29.9 → 8.2–10.0, mask yield 39 → 19 in lockstep, SP50 1.59–1.67 against 3.00 at
    full. **Confounded on time, marker and throughput together — not a cause, not chased** (`G120`).
    ⛔ **Editor residency alone is already insufficient, measured:** PID 25324 started 11:33:30, after
    the 10:18 morning legs, yet the 17:00–17:04 `F1A` legs ran with it resident at ratio 1.0002. Its
    accumulated CPU is 65,263.7 s over ~6.9 wall-hours (~2.6 cores sustained), which makes it a heavy
    and plausible load source whose per-interval activity was never recorded — **a named candidate,
    not a finding.**
  - **Chat's `run_brief.ps1` fail-closed change** (undeclared verdict ⇒ `UNDECLARED` holds the next
    brief; background-task termination or non-zero rc ⇒ RED) is recorded as **externally made and
    source-inspected only** — no runtime test of it was performed here and no harness file was
    modified.

### 078-08, 2026-09-06 — the H-editor RESIDENT half ran; the pair lost its discriminating power

Journal: `docs/sessions/2026-09-06-078-08-m51-editor-resident-half.md`. Predeclared before the leg
at `docs/predictions/2026-09-06-m51-editor-comparison.md` (`39afe83`). Evidence
`_bench_sessions_bank\M51_EDITOR_DIAG_EVIDENCE\` (11 files); bank `M51D_HE_RESIDENT_NAT`.

- **RESIDENT PASSES**: `decidable 35 · CURRENT 35 · PREVIOUS 0 · NEITHER 0`, at `speed_ratio`
  **1.0053** and **29.84 fps**, on the archived A-side `B8AC65D8` over the frozen container,
  accepted on attempt 1. The predeclared **primary** expectation was a FAIL; it is recorded as
  **wrong**, and the pass branch the same section held open is the one that fired.
- 🚨 **AN EDITOR IS RESIDENT ON ALL THREE ROWS AND THE VERDICT STILL MOVES** — the two 18:25/18:26
  legs failed at ratio 3.64/3.43 and 8.2/8.8 fps. ⇒ **editor residency is NOT sufficient to produce
  the failure**, measured twice now, counting 078-06's `F1A` at ratio 1.0002. What strengthens is
  the **throughput** association (five legs at ≈ 1.00 pass, five at ≥ 1.59 fail). ⛔ Still an
  association, confounded on time and throughput; **no mechanism, no threshold, no editor policy**
  (`G120`).
- ⚠ **The pass is PRICED, not banked:** the resident workload is a **restarted** editor
  (PID 20300, 18:59:43) running at **1.32 cores sustained across the leg window** against PID
  25324's **2.63**. It does not reproduce the original resident condition, so it is weak evidence
  about that condition — a limitation predeclared, then measured.
- ⛔ **CLOSED IS UNRUN AND SHOULD BE RECONSIDERED, NOT MERELY SCHEDULED.** The pair assumed RESIDENT
  would reproduce the failure so CLOSED could test recovery. It did not, so CLOSED could now only
  return `(pass, pass)` — nothing about the failure, at the cost of an owner save/close cycle.
- Candidate `D50DDE78` restored and hash-verified; container byte-unchanged before and after; the
  owner's editor was never touched beyond process metadata and is still open. Runner cleanup kills
  `StackOBot*` only and is structurally unable to reach `UnrealEditor.exe`.
- **VERDICT: NEEDS-DECISION**, fixed in advance, because CLOSED is deliberately unrun.

### 078-09, 2026-09-06 — the H-editor pair COMPLETES; both halves pass, and it decides nothing

Journal (paired section appended): `docs/sessions/2026-09-06-078-08-m51-editor-resident-half.md`.
Evidence `_bench_sessions_bank\M51_EDITOR_DIAG_EVIDENCE\` (22 files), banks
`M51D_HE_RESIDENT_NAT` and `M51D_HE_CLOSED_NAT`. Both halves were predeclared at `39afe83`
before either leg; neither prediction was rewritten.

- **CLOSED PASSES** (`decidable 37 · CURRENT 37 · PREVIOUS 0 · NEITHER 0`, `speed_ratio` 1.00023,
  29.99 fps), accepted on attempt 1 at the identical pose, with closure **independently verified**
  (no `UnrealEditor.exe` of any name, PID 20300 gone). ⇒ predeclared category **both pass — the
  current failure is NOT REPRODUCED and the pair discriminates nothing.** The predeclared
  **inversion did not occur**; it was retained and tested, not assumed away.
- 🚨 **THE PAIR NEVER CONTAINED THE FAILING CONDITION.** Both legs sit at `speed_ratio` ≈ 1.00
  against the 18:25/18:26 failures at 3.64/3.43 and 8.2/8.8 fps on the same binary, container,
  recipe and box. Closing the editor moved the readings marginally (1.0053 → 1.0002). ⇒ the
  **throughput** association survives; the **editor-presence** one does not. ⛔ **Whether closing
  an editor restores a FAILING run is UNTESTED**, and no root cause is asserted (`G120`).
- 🔻 **TWO 078-08 OVERSTATEMENTS CORRECTED IN PLACE.** *"CLOSED could now only return
  `(pass, pass)`"* — wrong; the inversion was live. *"1.32 cores vs 2.63"* — does **not** establish
  the tested workload was half as heavy: the **true capture span is 1.709 s / 1.700 s** (from each
  leg's own `t_wall`), while 1.32 averaged the **76 s between receipts** (~45× the capture, mostly
  launch and shutdown) and 2.63 averaged **hours**. **Workload equivalence remains UNMEASURED.**
- ⚠ Scope held: **failure without `F1` does not exclude an `F1` contribution**; script mtimes are
  not historical hash receipts. The banked 078-07 payload-loss finding stands, unrepeated here.
- Candidate `D50DDE78` restored and hash-verified after **each** leg; container byte-unchanged
  after both; no build, cook, push, merge or tag. **VERDICT: NEEDS-DECISION** — pairing release
  certification and every `m51` implementation item remain **BLOCKED** regardless of this result.

### 078-10, 2026-09-06 — a concrete frozen-pairing campaign exists as a REVIEWABLE DRAFT; nothing ran

New record: `docs/predictions/2026-09-06-m51-frozen-pairing-recheck.md`. Brief 078-10 released
**documentation and read-only preflight ONLY** — no capture, staging, build, cook, lever or
implementation. Preflight reproduced: HEAD `d572b3d`, `master` == `origin/master` `ac13700`,
`6713f51..HEAD -- Source Shaders` **EMPTY**, 10 handoffs, CaptureBench `472a409` tracked-clean,
Lyra detached `4a4bfcc` clean, A `B8AC65D8` / candidate `D50DDE78` / frozen container quintet
all matching.

- **Nine full commands, no ellipses and no inherited "same as" placeholders**, each mechanically
  validated: every parameter exists in the unchanged `run_leg.ps1` and every integer parses.
  Five campaign legs in fixed order (A natural reference → candidate NAT/SYN/NAT_SP50/SYN_SP50)
  plus **four conditional A-controls of which AT MOST ONE may ever run**, selected by the first
  valid candidate pairing failure, **then STOP regardless of its result**.
- 🔻 **078-05's identical-recipe claim is CORRECTED, not carried.** That smoke subrecord pinned
  `-Marker 0` while asserting it reproduced `PB1F_F_PAIR_*` "exactly (G184)"; the banked Phase B
  legs pass **no** `-Marker` and therefore ran at the runner default **`Marker 1`**. The campaign
  issues `-Marker 1` explicitly. ⚠ Two limits stated with it: `Write-LegGeometry` records
  **neither `Marker` nor `AutoExposure` nor `MaxAttempts`**, so the Phase B value rests on the
  default and **not on a receipt**; and 078-06 measured the marker axis to move the verdict by
  **nothing**, so this is provenance hygiene, not an expected number change.
- **Predicate UNCHANGED** (`PREVIOUS == 0 AND NEITHER == 0` over decidable frames). **Exit 0
  means READ, never PASS**; exits 2/3/4 are refusals; **`decidable == 0` exits 0 with
  `UNDECIDABLE` and is classified INVALID, not a pass.** Exit 3 is structurally impossible here
  (`COLLIDING_ANOMALIES = {"corrupted_texture"}`; every leg fires `blinking`).
- **`MaxAttempts 4` is a pose-invalidity budget only**, never four tries at a checker pass; an
  accepted pose-valid failure is **never** retried; physical attempts are counted separately
  from accepted bank copies; `< 3` counted events **stops for review** rather than being re-run.
- ⚠ **Ceiling declared before use: the two-position probe cannot detect an even-frame slip.**
  ⚠ **FPS/`speed_ratio` remain telemetry** — Chat's pacing-validity band is **NOT ADOPTED**, no
  band is enforced, and **no historical leg is retroactively invalidated**.
- Bank prefix `M51R_` verified free (45 bank paths + 36 attempt dirs, zero clashes); no existing
  evidence may be overwritten, renamed or deleted. Editor absence is **not** a prerequisite for
  any leg (a blanket `UnrealEditor.exe` rule would kill the Lyra fixture), and the original
  ~2.6-core workload is **not reproducible from the receipts and is not chased.**
- ⛔ **Everything else HELD**: production source/shaders, fixtures, harness/checker source,
  schema, observability meanings, identity/lifecycle/geometry/tag/writer/control changes, all
  levers and probes. "Second target arm stale" stays **narrowed to proven render/request
  identity, never queue position**; cross-domain frame-number equality is **unproved**; the
  8-bit+validity probe and `WithholdWorldRender` arming remain design work. `M-F1b` UNRUN, Lyra
  arm not waived, no push/merge/tag.
- **VERDICT: NEEDS-DECISION** — the exact draft awaits Codex and Chat review before any release.

### 078-11, 2026-09-06 — five bounded corrections to the 078-10 draft; still no leg

`docs/predictions/2026-09-06-m51-frozen-pairing-recheck.md` gains **AMENDMENT 1** (append-only;
the `e0d63f1` draft stays intact, and where they disagree the amendment governs). Journal
`docs/sessions/2026-09-06-078-10-11-m51-frozen-pairing-draft.md`. No capture, staging, build,
cook, source, harness or tool change; nothing pushed, merged or tagged.

- 🔻 **CORRECTS THE 078-10 ENTRY ABOVE.** That entry says the Phase B `Marker` value *"rests on
  the default and **not on a receipt**"*. **That is wrong, and a banked runtime receipt exists.**
  `_leg_geometry.json` is blind to the Marker axis — that half stands — but each `_tryN` attempt
  bank carries `_leg_game.log`, and **line 1533 of all four
  `PB1F_F_PAIR_{NAT,SYN,NAT_SP50,SYN_SP50}_try1` logs reads `CaptureBench.Marker = "1"`**, with
  the **full launch command at lines 17 and 207** containing `CaptureBench.Marker 1`. Hashes are
  in AMENDMENT 1 §A1.2. ⇒ **`Marker 1` on the banked baseline is PROVEN BY RECEIPT.** The
  correction of 078-05's `-Marker 0` claim is unchanged; only its evidentiary basis is upgraded.
  ⚠ `MaxAttempts` remains **invocation-script evidence, not a runtime field**; these logs do
  **not** establish historical tool hashes; and the **accepted alias bank carries no
  `_leg_game.log`** (measured) — cite the matching `_tryN` bank.
- 🔻 **Attribution corrected:** if a candidate leg and its A control BOTH fail, **the failure
  occurs without `F1`; that neither isolates nor excludes an `F1` contribution.** If the control
  passes, a candidate contribution is **consistent with** the observation while time and machine
  state stay confounded. **Neither branch is causal proof.**
- 🔻 **Runner limits stated honestly:** `IAI.Capture.Fps 30` / `IAI.Capture.Config 2 4 8 4 0` are
  runner-issued at `:219-220` and `-ExtraExecCmds` is appended **after** at `:224`, so a CLI
  override **is possible** — these nine commands simply do not do it. And **not every physical
  attempt gets a bank**: the `no run_summary` (`:322-324`) and `rode focus timeout` (`:371-373`)
  paths `continue` **before** the `_tryN` copy at `:408-418`, so they leave **no bank and no
  `_leg_game.log`**, and the packaged log rotates on the next launch. Raw
  `<Label>_try<N>` directories persist on unique paths and are preserved after the runner; **any
  missing receipt is enumerated by name.** ⛔ No harness change is released.
- 🔻 **Three stop classes stay distinct** — **REFUSED** (exit 2/3/4), **INVALID/UNDECIDABLE**
  (`decidable == 0`), **INVALID (< 3 counted events)** — none of which releases a control.
  **Exit 3 is UNEXPECTED, not impossible**; an actual refusal is always honoured.
- 🔻 **Screen-percentage read-back identified:** the `Capture(mask): M23 PASS` line's
  `internalViewRect` / `unscaledViewRect` (SP50 banked at 640x360 internal vs 1280x720 unscaled,
  with `r.ScreenPercentage = "50"`). The **100 % arm is read from rendered geometry**, since **no
  explicit `r.ScreenPercentage = "100"` echo exists** and demanding one would be impossible.
- 🔻 **False relative time removed:** the two A-side passes are timestamped **19:07:43** and
  **19:19:36**, not *"three hours earlier"*.
- **VERDICT: NEEDS-DECISION** — corrected exact snapshot awaits Codex and Chat review; all
  captures and every `m51` implementation item remain **HELD**.

### 078-12, 2026-09-06 — the observation-coverage floor is committed; still no leg

`docs/predictions/2026-09-06-m51-frozen-pairing-recheck.md` gains **AMENDMENT 2** (append-only;
`AMENDMENT 1` and §§0–10 intact, and where they disagree the amendment governs). Journal
`docs/sessions/2026-09-06-078-10-11-m51-frozen-pairing-draft.md`. Implements Chat's conditional
approval (`_reviews/078-11-chat-prediction-review.md`) as dispositioned by Codex
(`_reviews/078-12-codex-disposition-of-chat-approval.md`). ⛔ **DOCUMENTATION ONLY — no capture, no
staging, no build, no cook, no source or tool change; nothing was run.** Execution awaits a separate
Codex release brief.

- 🆕 **THE `decidable >= 1` NON-VACUITY TEST IS REPLACED BY A COVERAGE FLOOR OF `decidable >= 20`
  of 40.** A leg with two decidable frames and `PREVIOUS 0` would have PASSED — the `G146` shape, a
  vacuous zero. **The basis is HALF THE REQUEST** (40 captured frames ⇒ 40 probe opportunities), fixed
  prospectively from the request size. ⛔ **NOT statistical sufficiency · NOT a tolerance or any
  loosening of the predicate · NOT an FPS band · AND EXPRESSLY NOT DERIVED FROM THE FAILING BANK** —
  picking a floor above the failing legs and then citing them as its justification would be fitting
  the rule to the results it judges, and the amendment says so.
- **Attainability is cited only from receipts actually read:** `M51D_HE_RESIDENT_NAT` **decidable 35**
  and `M51D_HE_CLOSED_NAT` **decidable 37**, both measured in-session with the unchanged checker.
  ⚠ **No other decidable count is repeated**, because no other was re-verified here.
- 🚨 **AN UNDER-COVERED LEG STILL REPORTS ITS COUNTS AND FAILING INDICES IN FULL.** Insufficient
  coverage prevents **certification**; it does not make an observed mismatch disappear. It stops the
  campaign, releases **no** A control and is **not** re-run ad hoc. ⛔ **No retrospective
  reclassification** — historical legs keep their original classifications, including the earlier
  failures recorded at 13 decidable frames. The floor applies to the reference, all four candidate
  cells and the conditional control.
- **The stop taxonomy is now FOUR classes**, none releasing a control and none retried: **REFUSED**
  (exit 2/3/4) · **INVALID (UNDECIDABLE)** (`decidable == 0`) · 🆕 **INVALID (coverage)**
  (`decidable` 1–19) · **INVALID (event count)** (fewer than 3 events).
- **Each leg's row now carries verdict + `decidable`/`CURRENT`/`PREVIOUS`/`NEITHER`/`same-pos`/
  `no-data` + sustained FPS + `speed_ratio` ON ONE LINE**; a genuinely absent reading prints
  `UNKNOWN` and is never invented. ⚠ **FPS/pacing remain telemetry — no band adopted, no retroactive
  invalidation.** This is legibility, not a gate.
- 🔻 **CHAT'S PARAPHRASE OF THE ATTRIBUTION RULE IS EXPLICITLY REJECTED.** The review asked to keep
  *"control fails ⇒ not attributable to the candidate"* verbatim; that is the exact overclaim
  `A1.1` was written to correct, and adopting it would silently undo the correction. **Governing,
  unchanged: both fail ⇒ the failure occurs WITHOUT `F1`, which neither isolates nor excludes an
  `F1` contribution; control passes ⇒ consistent with, time and state confounded. Neither is causal
  proof.**
- 🔻 **"Container quartet" is corrected to the FIVE-FILE QUINTET** — and **§1 of the prediction was
  already correct**; the slip was in the paraphrase, not the record.
- **Swap/restoration receipts are now explicit policy** for the execution brief: staged exe hashed
  against the declaring archive **before every leg**; all five container files hashed **before and
  after every swap and leg**, both banked; candidate `D50DDE78` restored and re-hashed **on every
  exit path**; no rebuild, cook or third binary.
- **A PASS would be scoped pairing smoke on these bytes and recipes**, through a probe with a
  declared **even-frame blind spot** — ⛔ **not full `F1`/`m51` release certification**, and it does
  not erase the failure bank. Broader *"a PASS certifies F1"* phrasing is not adopted.
- Unchanged: nine commands and their order · `MaxAttempts 4` pose budget · no retry of a valid
  failure · at-most-ONE control then STOP · predicate and tolerances · at-least-3 events · every hold.
- **VERDICT: NEEDS-DECISION** — execution awaits the separate Codex release brief.

### 078-13, 2026-09-06 — the frozen pairing campaign RAN and STOPPED at leg 2; the reference passes, the candidate has no verdict

> 🔻 **THIS ENTRY IS CORRECTED ON FIVE POINTS BY `078-14` BELOW — READ THAT ENTRY WITH THIS ONE.**
> The outcome, the stop and every count are unchanged; five **evidentiary** claims in this entry
> overstate what the artifacts show and are superseded there. Where they disagree, **`078-14`
> governs.**

First execution released in this line. Brief `_reviews/078-13-run-frozen-pairing-campaign.brief.md`,
released by `_reviews/078-13-codex-amendment-acceptance-and-execution-release.md`. Journal
`docs/sessions/2026-09-06-078-13-m51-frozen-pairing-campaign.md`. Recipe
`docs/predictions/2026-09-06-m51-frozen-pairing-recheck.md`, verified at execution time as
**SHA-256 `8D7951F5…D2E9B5`, 691 lines**, governed by `AMENDMENT 2`, then `AMENDMENT 1`, then §§0–10.
⛔ **No source, shader, fixture, harness, checker or schema change; no build, no cook, no third
binary, no push, merge or tag.** `m51` HEAD `d727aec` at every leg; production frozen `6713f51`;
`master` == `origin/master` == `ac13700`.

- ✅ **LEG 1 — `F1R_A_REF_NAT` (A `B8AC65D8`, natural, 100 %) — VALID and PASSES, accepted on
  attempt 1.** `decidable 33 · CURRENT 33 · PREVIOUS 0 · NEITHER 0 · same-pos 0 · no-data 7` ·
  **29.9237 fps · `speed_ratio` 1.002550** · checker exit 0 (read, not assumed). No failing index
  exists to print. Every validity condition met individually: **coverage 33 ≥ 20** (`A2.1`, the new
  floor's first exercise — it did not bind) · **4 counted events ≥ 3** · 40 rows `session_index`
  0..39 and 40 PNGs `frame_00000..039` at **1280×720**, no duplicate, missing, orphan or zero-byte ·
  live probe `tag=255` with **40 `PROBE STEP`** lines · every referenced `mask_file` present on disk.
  Effective config read back from the leg's own log (A48): SVE grab point, mask/target-mask/census ON
  at compiled defaults, output height 0 (native), `Marker "1"`, AE `"0"`, **`m40 SYNTH TICK ORDER =
  off`**, and the 100 % arm proven from **rendered geometry** — `internalViewRect` /
  `unscaledViewRect` both **1280×720**, with **no `r.ScreenPercentage` echo**, exactly as `A1.3`
  predicted and forbade requiring.
- ⛔ **LEG 2 — `F1R_B_NAT` (candidate `D50DDE78`, natural, 100 %) — HALTED, NO VERDICT OFFERED.**
  Runner exit **2** after **four consecutive B1 pose-gate failures**; `MaxAttempts 4` is a
  pose-invalidity budget and it is exhausted. **This is the attempt-exhausted stop class (`§6`,
  `A1.4`): it is NOT a valid pairing failure, it releases NO A control, and it was NOT re-run.**
- 🚨 **The four failures are TWO different shapes under one label, and the label hides that.**
  try2/try4 are genuine **A47 rotation bifurcation** (`modal_rot` (338.54, 358.63, 0.0) and
  (358.05, 349.65, 0.0), bbox off calibration, non-uniform ratios). **try1/try3 are NOT pose
  displacement at all** — `modal_rot` exactly `(0,0,0)`, bbox byte-for-byte `CALIB_BBOX`,
  `pose_match=True` — they fail only the `a56` **self-consistency** conjuncts (`distinct=2`,
  `modal 64.0 %`). That is the **third cause** behind `POSE GATE FAILED (B1)`, the `m26` P24 shape.
  ⛔ **Cause not established for either shape and none attributed** (`G206`, `G120`).
- 🔑 **The discard is `A63`'s shape and not a laundered failure:** the condition is the camera pose,
  fixed at run start, read by `check_pose.py` under a rule written days earlier, **before any pairing
  result existed — and the pairing outcome was never computed at all.**
- ⛔ **No candidate pairing number of any kind is reported, and that is STRUCTURAL.** The runner
  halts before its final bank copy (`:481-489` precedes `:491-494`), so `M51R_F1R_B_NAT` does not
  exist (measured); the predeclared checker invocation resolves *exactly one* session in that bank and
  **throws when it cannot**, so the released command is not runnable on this leg. Reading a verdict
  off a pose-invalid leg is what **`P8`** forbids — `TAU` is not pose-invariant and fails in the
  dangerous direction.
- **The leg was correctly CONFIGURED; the failure is pose alone.** All four attempts: 40 frames / 40
  rows / 40 PNGs, 0 zero-byte, 4 events, `label_schema 2`, probe spawned with 40 `PROBE STEP`,
  `SYNTH TICK ORDER = off`, `Marker "1"`, viewrect 1280×720/1280×720, `speed_ratio` 1.000006–1.002026
  at 29.94–30.00 fps.
- ⚠ **Observation, `n = 1`, NO MECHANISM:** the A-side reference accepted on attempt 1 with
  `distinct=1 modal=100 %`; the candidate leg ~100 s later on the same box, container, fixture and
  recipe gave **4-of-4** pose-invalid attempts against `A47`'s recorded ~2-in-5 band. ⛔ **Not
  attributed to the candidate binary** — the pose gate reads the camera only, and naming a cause here
  would be `G120`'s error.
- **Receipts.** Seven banked receipts bracket **every swap and every leg**, each covering the staged
  executable **and all five container files** (`A2.5`/`A2.6`): `00_PRECAMPAIGN` ·
  `01_LEG1_A_BEFORE/AFTER_SWAP` · `02_LEG1_POST` · `03_LEG2_CAND_BEFORE/AFTER_SWAP` · `04_LEG2_POST` ·
  `99_FINAL_RESTORE`. **The quintet is byte-identical in all seven.** Only the two pinned archives
  were copied, each hash-verified first. **Staged executable restored to candidate `D50DDE78` and
  re-verified by hash; zero `StackOBot*` processes at every receipt.** The resident `UnrealEditor`
  does not match the killed pattern and **no owner application was touched**.
- **Retention: five physical attempts, five `_tryN` banks, five `_leg_game.log` files — no missing
  receipt.** `A1.4`'s two receipt-less early-exit paths did not fire. Raw attempt directories copied
  into `_bench_sessions_bank\M51_FROZEN_PAIRING_RECHECK_EVIDENCE\attempts\`; nothing overwritten; all
  81 candidate `M51R_`/attempt paths were verified free at preflight.
- ⛔ **UNRUN and named:** `F1R_B_SYN` · `F1R_B_NAT_SP50` · `F1R_B_SYN_SP50`, and **all four**
  `F1R_A_CTL_*` controls (the stop class that fired releases none).
- **What it establishes:** a third consecutive A-side pass on this binary and container (35, 37, now
  33 decidable). ⛔ **NOTHING about the `F1` candidate** — its first cell produced no valid leg, so
  there is no candidate reading, pass or fail. ⛔ **The 078-05/078-06 failure bank is untouched** —
  not reproduced, not refuted, not reclassified; root cause OPEN and not chased. ⚠ Leg 1's PASS is
  **scoped pairing smoke** with the declared **even-frame blind spot**, not `F1`/`m51` release
  certification. Everything in `§10` stays HELD; `M-F1b` UNRUN; Lyra not waived.
- **VERDICT: NEEDS-DECISION** — one of five released main cells passed, one halted without a verdict,
  three UNRUN, four controls UNRUN. Codex then Chat evidence review; no implementation unit started.

### 078-14, 2026-09-06 — FIVE EVIDENTIARY CORRECTIONS to the 078-13 record; documentation only, nothing ran

**Authoritative correction to the `078-13` entry above and to the `078-13` session journal.** Sources:
`_reviews/078-14-codex-report-correction-findings.md` and `_reviews/078-13-codex-independent-artifact-review.md`.
⛔ **DOCUMENTATION ONLY — no capture, no staging, no binary swap, no build, no cook, no source,
shader, fixture, harness, checker or schema change, no new checker execution, no retry, no push,
merge or tag.** `m51` HEAD was `c73fc2d` at the start of this brief; production frozen `6713f51`;
`master` == `origin/master` == `ac13700`; staged exe unchanged at candidate `D50DDE78`; the five
container files, both archives and every banked artifact are **untouched**.

🔑 **WHAT DID NOT CHANGE, stated first so the corrections cannot be misread as a reversal:** the
campaign outcome, the attempt-exhausted STOP, every count and index, the reference PASS, the fact
that **no valid candidate pairing result exists**, and the release hold. **No gate was loosened, no
leg re-run, no attempt budget extended, and no negative result erased.** What changed is **what the
record CLAIMS the evidence shows.**

- 🔻 **(1) `NO-DATA` IS MISSING PROBE EVIDENCE, NOT "A LEGITIMATE MASK OUTCOME".** The reference's
  seven `no-data` frames (`si` 15, 24, 25, 26, 32, 34, 35) were described as *"a legitimate mask
  outcome"*. **Withdrawn.** Supported statement: the prediction permits undecidable frames **provided**
  `decidable >= 20` and the separate artifact-integrity conditions hold — both did — and `A1.3` says
  such a frame does not violate those conditions. ⛔ **It does not certify correct mask production on
  those frames**, and the existence of all **34** promised `mask_file` references **cannot certify the
  seven readings that produced none.** All counts and indices preserved.
- 🔻 **(2) THE REFERENCE'S ONE UNAVAILABLE EFFECTIVE-MASK FRAME IS NOT "THE VERY CLASS `F1`
  ADDRESSES".** `F1`'s tested change concerns **requested-but-INEFFECTIVE** target masks waiting on an
  outcome that cannot exist and losing RGB/label rows. **This reference ran with the target mask
  EFFECTIVE and ON.** *Mask unavailable / unmeasured* is a **broader outcome**, and ⛔ **its cause is
  not established by this run.** The observation — `target_mask_frames_unavailable = 1`, one
  `mask_state: unmeasured` row — is preserved exactly; the equivalence is removed.
- 🔻 **(3) THE `B1` DESCRIPTION AND ITS FAILURE ATTRIBUTION ARE BOTH CORRECTED.**
  ⛔ **`B1` does not read camera data alone** and **pose is not "fixed at run start"**: `check_pose.py`
  derives the settle window from the **`view.rot` history** and then `a54_oracle.a56_check` decides on
  the **rounded anomaly `bbox_px` history**, requiring **`coverage >= 0.90` AND `distinct <= 3` AND
  `pose_match`**. **Recomputed from the banked rows this session:** try1 view rows are **24 × (0,0,0) +
  16 × (0, 359.83, 0)**; try3 are **26 × (0,0,0) + 14 × (0, 0.17, 0)** — so **the rotation series is
  not constant within those runs**, and `(0,0,0)` is only the **modal** value. Both have the
  calibration modal bbox but only **16 of 25** boxes match it: **64.00 %**, below the 90 % floor.
  🚨 **`distinct = 2` PASSES the `<= 3` criterion**, so listing it as a failing conjunct was wrong —
  **on tries 1 and 3 the single failing predicate is the 64 % modal coverage.** try2 fails
  **`pose_match` only** (coverage 100 %, distinct 1, modal rot `(338.54, 358.63, 0)`, modal bbox
  `(9.9, 240.4, 342.9, 230.4)`); try4 fails **both** coverage (**62.50 %**) **and `pose_match`**
  (modal rot `(358.05, 349.65, 0)`, modal bbox `(173.5, 449.0, 271.1, 238.9)`).
  ⛔ **"Genuine A47 rotation bifurcation" and "a different cause" are WITHDRAWN** — the first is the
  wording of `check_pose.py`'s own **REPORTING-ONLY** block, which establishes nothing. **Observed
  inputs and failing predicates are recorded; the mechanism is UNRESOLVED** (`G206`, `G120`).
  🚨 **And the exclusion argument is withdrawn too: which fields a validity gate reads NEITHER
  ISOLATES NOR EXCLUDES an `F1` contribution.** The honest position is **no cause established in
  either direction** — the four invalid attempts are not attributed to the candidate binary, and the
  candidate binary is not cleared. ⛔ **These four remain predeclared-INVALID and are not retried.**
- 🔻 **(4) `P8` / `TAU` IS NOT WHY NO PAIRING NUMBER EXISTS.** `m44_pairing_probe.py` compares
  **magenta picture centroids against tag-255 mask centroids** (**40 px** current/previous threshold,
  **50 px** same-position test) and **reads no `TAU` and no luminance threshold**; `TAU`/`P8` govern
  the **A54 luminance oracle**, a different instrument that was not involved. ✅ **The governing
  reasons are (i) the campaign's `A63`/`B1` precondition and its attempt-exhausted stop, and (ii) the
  released checker recipe targets the ACCEPTED bank, which for this leg does not exist.**
  ⚠ **The tool is not inherently incapable of reading a `_tryN` bank** — that is simply not what the
  released recipe points at, and it was not pointed elsewhere. ⛔ **No discarded attempt received a
  valid pairing verdict, and any future offline diagnostic must preserve their INVALID status: it
  could release no control and could not become a valid campaign result.**
- 🔻 **(5) THERE ARE EIGHT RECEIPT FILES, NOT SEVEN.** `00_PRECAMPAIGN` · `01_LEG1_A_BEFORE_SWAP` ·
  `01_LEG1_A_AFTER_SWAP` · `02_LEG1_POST` · `03_LEG2_CAND_BEFORE_SWAP` · `03_LEG2_CAND_AFTER_SWAP` ·
  `04_LEG2_POST` · `99_FINAL_RESTORE`. The 078-13 table collapsed each swap's two receipts onto one
  row and then counted rows. **Both inventory statements are corrected**; the evidence directory's
  `INDEX.md` carries the same correction **appended after its original bytes**, which are preserved.
  **The quintet is byte-identical in all eight**, and every original receipt file is unmodified.

⚠ **ALSO SCOPED, not withdrawn:** the artifact-completeness and effective-configuration readings on
all four discarded attempts (40 frames / 40 rows / 40 PNGs, 0 zero-byte, 4 events, `label_schema 2`,
probe spawned with 40 `PROBE STEP` lines, `SYNTH TICK ORDER = off`, `Marker "1"`, viewrect
1280×720/1280×720) **are real observations and stand exactly as measured.** ⛔ **They identify no
cause**, which is why *"the failure is pose alone"* is replaced by the named failing predicates.
Likewise *"NOTHING is established about the `F1` candidate"* is replaced by the precise claim:
**there is NO VALID CANDIDATE PAIRING RESULT**, and nothing here isolates or excludes an `F1`
contribution.

📌 **Codex's independent review** (`078-13-codex-independent-artifact-review.md`) decoded every RGB
and mask PNG, reconciled index sets and mask references, read all five effective-config logs, compared
full relative-path/SHA-256 maps across each bank, raw session and preserved copy, re-read all eight
receipts, and rehashed both archives, the staged exe, the five container files, `run_leg.ps1` and
`m44_pairing_probe.py` — **all match the approved pins.** ⛔ **None of that certifies the invalid
candidate pairing cell.**

- **VERDICT: NEEDS-DECISION for the CAMPAIGN, which remains INCOMPLETE** — one released main cell
  passed, one halted without a verdict, three main cells and all four conditional controls are UNRUN,
  and full release stays HELD. The documentation corrections themselves are complete.

### 078-15, 2026-09-07 — the identity / lifecycle / geometry unit exists as a REVIEWABLE DRAFT; nothing ran

> 🔻 **PARTLY SUPERSEDED BY `078-16` BELOW — read that entry before relying on this one.** Two of the
> assertions recorded here are **RETRACTED** (the ordinal "mis-bound == unmeasured" identity claim and
> the near-plane bbox-exposure claim), and the draft named below has been **revised in place**; the
> reviewed snapshot is `a031b46`. This entry is preserved as the historical record and is not
> retro-edited.

New record: `docs/predictions/2026-09-07-m51-identity-lifecycle-geometry.md`. Journal
`docs/sessions/2026-09-07-078-15-m51-identity-lifecycle-predictions.md`. Released under Chat's
`078-14` review §3 (prediction-only drafting) after Codex completed both approved offline checks
(`_reviews/078-15-offline-check-scope.md`, `_reviews/078-15-codex-offline-findings-and-request-to-chat.md`).
⛔ **DOCUMENTATION ONLY — no capture, checker run, build, cook, staging, binary swap, source /
shader / tool / fixture / schema edit, owner-app action, merge, push or tag; no agents, no
worktrees.** `m51` HEAD was `46139fd` at the start; production frozen `6713f51`; `master` ==
`origin/master` == `ac13700`; ten owner handoffs preserved; CaptureBench `472a409` tracked-clean;
staged executable left untouched at candidate `D50DDE78`.

- 🔑 **THE DRAFT IS A PROPOSAL AND CHANGES NO RULING.** Where it and an earlier record disagree, the
  **earlier record still governs** until a reviewed brief says otherwise. Nothing in it is adopted
  here as a settled mechanism. **`docs/predictions/2026-09-06-m51-gate-manifest.json` remains the
  single execution inventory and is UNCHANGED**; the draft's §D.7 is a *proposed delta* against it,
  marked not runnable, and it renumbers no recipe.
- **Method:** a direct source trace of every claimed thread boundary, identity domain, arming
  /admission gate and projection primitive, in the plugin and in the local UE 5.1 engine. Earlier
  evidence was used as leads, not as a substitute for the current read; the `be16977` endpoint diff
  was not imported. Every claim in the draft is tagged **SOURCE-READ / BANKED OBSERVATION /
  PROPOSED / OPEN**, and no source path is described as a runtime finding.
- 🆕 **FOUR REQUEST-IDENTIFIER DOMAINS, THREE SHARING ONE KEYSPACE (SOURCE-READ).** Colour is
  `++CaptureRequestSerial` with no namespace bit (`AnomalyCaptureSubsystem.cpp:4197`); the normal
  mask measurement's request id **is `GFrameCounter` itself** (`:833`); the target mask is
  `(1ull<<61) | serial` (`:903`, `:1159`) and the census `(1ull<<62) | serial`
  (`AnomalyCensus.cpp:40`, `:705`). The last three key the **single**
  `TMap<uint64, FAnomalyMaskResult> Results` (`AnomalyMaskSceneViewExtension.h:87`) — one a **time**
  value, two **counts**. The bit layout is confirmed arithmetically against the banked request IDs.
  ⛔ **An observed offset between two of these numbers is not evidence of a mismatch, and agreement
  is not evidence of a binding.**
- 🆕 **THE COLOUR PATH BINDS 1:1; THE MASK PATH BINDS N:1 AND CLONES (SOURCE-READ).**
  `ConsumeWantedForPublish(uint32 FamilyFrameNumber, ...)` pops FIFO and **never compares
  `FamilyFrameNumber`** (`AnomalySveCapturer.cpp:45-73`; it reaches only the trace log at `:69`),
  while the later family→readback step **is** identity-bound through the key ring. The mask SVE takes
  **all** pending arms into one render (`AnomalyMaskSceneViewExtension.cpp:126-131`) and clones the
  whole reduction to every served id (`:521-529`), giving the pixel payload to the **first** arm that
  asked for pixels (`:512-520`). ⇒ under arm accumulation the picture and the mask for one
  `session_index` can come from **different renders**, and their join is by `session_index`
  (`:1162`, `:1195`, `:3830`), which validates no render identity on either side.
  **BANKED OBSERVATION (078-07, request-ID joined):** three callbacks per failing leg served two
  distinct target serials, and in each case the **second** is one of exactly three
  `TARGET MASK UNAVAILABLE` results with `pixels=0`; the four passing legs show none.
  🔑 **Today the mis-bound frame is the one that reports unmeasured**, because `ServiceTargetMask`
  `continue`s on a short pixel array (`:1192-1203`) before reading the cloned table — **ordinal luck,
  not a rule**: the census can also want pixels (`AnomalyCensus.cpp:797-802`), the m26 consumer has
  **no pixel check at all** (`AnomalyMaskMeasure.cpp:404-432`), and making pixels available to every
  consumer would make every mis-bound frame report a **correct-looking** mask.
  📌 `m43`'s "one render serves every pending arm" (ledger §10.1) **fixed a real starvation defect**
  and is correct for a **tag-set** question; it is not sufficient for a **per-captured-frame**
  question. ⛔ **Do not re-propose the one-slot FIFO** (`G217`). ⛔ **No banked instance of m26
  double-contribution is claimed** — that half is a source path only.
- 🆕 **THE LABEL PROJECTION USES THE LIVE NEAR PLANE AND HARDCODES PERSPECTIVE (SOURCE-READ, plugin
  and engine).** `AccumulateFrameEvents` runs **in the drain** (`:4024`) and projects the **live**
  actor (`:5504`, `:5525`, `:5528`) against the **frozen** four-scalar view.
  `AnomalyViewport::BuildProjectionMatrix` hardcodes `Perspective` and `bConstrainAspectRatio=false`
  (`AnomalyViewport.cpp:173-174`) and never sets `PerspectiveNearClipPlane`, which defaults to
  `-1.0f`, so `GetFinalPerspectiveNearClipPlane()` returns **`GNearClippingPlane`**
  (`Engine/Classes/Camera/CameraTypes.h`; consumed at
  `Engine/Private/Camera/CameraStackTypes.cpp:115-121`) ⇒ the box uses the **live global at drain
  time**, not `Snap.NearClip` (`:4202`) — and `camera_clipping` is a shipped session-scoped anomaly
  that mutates exactly that global. ⛔ **No banked leg is cited as an instance**, and
  `camera_clipping`'s own event takes the whole-frame branch (`:5519-5524`), so the exposure is to
  **other** events' boxes. Engine-side, `ULocalPlayer::GetProjectionData` calls
  `ViewExt->SetupViewProjectionMatrix` (`Engine/Private/LocalPlayer.cpp:1151-1154`), so a host
  extension can alter the real projection — which a reconstruction from four scalars cannot see
  (`FAnomalyViewInfo` has five members and carries no matrix and no view rect).
- 🆕 **THE SERIAL-RESET ASYMMETRY (SOURCE-READ).** `CaptureRequestSerial` is never reset
  (`AnomalyCaptureSubsystem.h:283`), so colour ids never repeat; `TargetMaskOwnSerial = 0` (`:3003`)
  and `CensusIdSerial = 0` (`AnomalyCensus.cpp:116`) ⇒ **both mask-side keyspaces repeat every run** —
  a concrete cross-session leak surface for `M-F5`, which `Generation` would close.
- 🆕 **Smaller, each with its line:** `LookupKey` never invalidates its entry
  (`AnomalySveKeyRing.cpp:132-148`), so two views sharing one family number would both submit for one
  id — unreachable on both single-view fixtures and therefore **untested** · unresolved snapshots are
  dropped by count (`PendingSnapshots.Empty()` behind a count-only warning, `:4108-4113`), which is
  where 078-05/06's **57 missing requested indices** went, invisible in a contiguous index prefix ·
  **F1's hold moves the ENTIRE remaining batch** into held frames when the head waits (`:3834-3838`),
  which is *why* its counter measures **drain visits at the waiting head** and not elapsed game
  frames — the mechanism behind `078-12`'s reading · the `labels.jsonl` append result is discarded
  (`AnomalyLabelWriter.cpp:533-540`) · the writer is **unbounded**, one thread-pool job per frame with
  no admission control or byte accounting, and `FlushPending` **breaks on timeout leaving jobs live**
  (`AnomalyAsyncWriter.cpp:17-27`, `:114-127`) · `StartRun`'s only guard is per-instance `bRunning`
  (`:2762-2766`) while the key ring and tag ledger are process-global, so a second capture context is
  **not refused today**.
- **Preserved unchanged and re-read this session:** the observable predicate
  `labelled && held && target_pixels >= ObservableMinPixels` after the unmeasured early-out
  (`:3898-3906`) · `IsFireLabelledThisFrame` (`:4647-4667`) sampled and frozen at `OnWorldTickEnd`
  (`:4607-4612`) · `G258`'s one-directional `target_drawn_pixels` rule · schema 2/3 boundaries ·
  `capture_complete` as prepublication readiness.
- **The proposal, in one line:** an immutable `FAnomalyRequestKey`
  `(Generation, ArmGameFrame, RequestId, SessionIndex, Domain)` minted once at all four arm sites,
  **publication-frozen membership** so a family serves only requests whose `ArmGameFrame` matches at
  publish, **the same rule applied to the mask SVE where AMENDMENT 2's identity rule does not
  currently reach**, terminal-state lifecycle with a stated conservation equation and capacity
  retired exactly once, and geometry frozen with its frame including the render's own matrix and
  rect. ⛔ **The second requester is never declared stale by queue position** — only by a mismatched
  `ArmGameFrame`. ⛔ **One rendered mask is never cloned into records for different capture frames.**
- ⛔ **UNRESOLVED PRECONDITIONS CARRIED IN, NONE WAIVED:** there is **no valid candidate pairing
  baseline**, and the four discarded attempts stay INVALID, are not retried and receive no
  retrospective verdict — **no cause is established in either direction** · the 078-05/078-06 valid
  pairing failures remain failures, root cause OPEN and not chased · **B1 is untouched**, and because
  **no candidate verdict flipped under sorting** (078-15 offline) **no gate edit and no known-answer
  repair test is triggered**; the reference's settle start did move (1 → 0), so **no general
  order-insensitivity claim is made**, and nonmonotonic append order exists in the **pre-F1 A
  reference** too · **the A/B/A/B/A/B interleave is NOT adopted** ("much greater failure rate" is
  undefined and three attempts per binary cannot support the offered dichotomy) · `M-F1b` UNRUN and
  `bSveCapture` stays out of the effective-mask predicate · the writer byte bound is **OPEN** through
  `IImageWrapper` and **no isolated label-append patch is released** · publication / row-orphan /
  auditor semantics stay **HELD** pending F7/F8 · every `open_arms` entry, all sixteen findings and
  the full regression set remain required · **the exhausted campaign is not automatically replayed.**
- ⚠ **Scope deviation recorded rather than absorbed:** the crosswalk additionally proposes **three
  new rows** (`M-ID1`, `M-ID2`, `M-GEO2`), because no existing recipe asserts one-render-one-frame
  identity or near-plane provenance. They are marked proposed and not runnable, and **the manifest
  file is unchanged.**
- **VERDICT: NEEDS-DECISION** — the exact draft awaits Codex then Chat review. Document completion is
  not design, execution or release approval, and the campaign remains INCOMPLETE.

### 078-16, 2026-09-07 — the identity / lifecycle / geometry draft is CORRECTED against review; still a draft, still nothing ran

> 🔻 **PARTLY SUPERSEDED BY `078-17` BELOW — read that entry before relying on this one.** This entry
> records the revision as *"specifies a common game-thread publication object … one-time claims, six
> orthogonal ownership lifetimes … and **bounded** terminal retention"* and names **three** subunits as
> the holds. **The independent review of the exact 078-16 bytes**
> (`_reviews/078-16-codex-independent-review-and-request-to-chat.md`) **found `R1`, `R2`, `R3` and `R6`
> still carrying contradictions or incomplete interfaces**, and the design's own `R1`/`R2` `ACCEPTED`
> labels were **Code's dispositions of review input, not approval**. ⇒ **the ownership and publication
> interfaces are NOT resolved, the holds are NOT limited to three subunits, and the bounded-retention
> scheme is WITHDRAWN as unsound.** This entry is preserved as the historical record and is not
> retro-edited.

Revised in place: `docs/predictions/2026-09-07-m51-identity-lifecycle-geometry.md` (reviewed snapshot
`a031b46`, SHA-256 `021B26E9…F672B3F`, 664 lines). Journal
`docs/sessions/2026-09-07-078-16-m51-identity-lifecycle-revision.md`; the 078-15 journal gains a §9
correction. Sources: `_reviews/078-15-codex-draft-review-and-request-to-chat.md` (findings `R1`–`R7`)
and `_reviews/078-16-codex-takeover-and-review-clarification.md`.
⛔ **DOCUMENTATION ONLY — no capture, checker run (including old-bank diagnostics), build, cook,
staging, binary swap, source / shader / tool / fixture / schema edit, owner-app or office action,
merge, push, tag, replay or interleave; no agents, no worktrees.** Production frozen `6713f51`;
`master` == `origin/master` == `ac13700`; ten owner handoffs preserved; CaptureBench `472a409`
tracked-clean with seven prior untracked tools; Lyra detached-clean `4a4bfcc`; staged exe untouched at
candidate `D50DDE78`. **`docs/predictions/2026-09-06-m51-gate-manifest.json` is BYTE-UNCHANGED
(`D33AF5D4…6025F9`) and remains the single execution inventory.**

⚠ **No Chat response existed.** `_reviews/078-15-chat-identity-lifecycle-draft-review.md` was checked
and is **ABSENT**; its absence was **not** treated as approval and no Chat input was used. A later
response must be read in full.

**Corrections adopted (full detail in the revised design's `R1`–`R7` disposition table):**

- 🔻 **RETRACTED (`R4`):** *"Today the frame that would be mis-bound is exactly the frame that reports
  unmeasured."* It was a **conditional inference stated as fact**, and the source permits the opposite
  ordering. **What survives:** the request-ID-joined second-target payload loss (three per failing run)
  and the `PixelOwner` mechanics. ⛔ **No render identity may be assigned by ordinal**; healthy
  same-render normal/target/census sharing remains valid; **one rendered mask is never cloned into
  records for different capture frames.**
- 🔻 **RETRACTED (`R5`):** the near-plane **bbox** exposure, and `M-GEO2` as a falsifier. `MinZ` occupies
  the projection's depth column only and reaches `Clip.Z`; the projector reads only `Clip.X/W` and
  `Clip.Y/W`. **The matrix-depth provenance defect is real and remains a design input**; the actual
  geometry defect is the **absent near-plane clipping**, a separate fact. ⛔ **`camera_clipping` fired
  beside another event is NOT a demonstrated falsifier.**
- **`R1`/`R2`:** the mask path has **no game-thread publication point** (`BeginRenderViewFamily` is an
  empty override), so `A2.1`'s publish-time identity rule is **not** a line-for-line port. The design
  now specifies a common game-thread publication object, world/viewport binding, topology refusal
  **before** the behaviour change, one-time claims, six orthogonal ownership lifetimes, a per-domain
  conservation equation with an `outstanding` term and a quiescent-only zero, and **bounded** terminal
  retention. `Cancelled(payload)` is deleted.
- **`R3`/`R6`:** matrix / jitter / rect provenance is specified; **three subunits are named NOT
  IMPLEMENTATION-READY** — the mask activation predicate, the GT-bounds-to-rendered-proxy equivalence,
  and the **backbuffer identity bridge, which stays OPEN (the path is NOT disabled and NOT
  certified)**. `N-LC2`'s can-fail device does not exist and is OPEN. The encoded probe is on an
  explicit implementation hold.
- 🔻 **`R7`, including a correction to the review's own wording:** frozen-pairing `A2.1`:567 and
  `A2.3`:617-628 adopt **no throughput precondition** and describe same-row FPS/`speed_ratio` as *"a
  legibility requirement, not a gate"* ⇒ **calling it a validity rule was inaccurate and is
  withdrawn.** 🚨 **Release-hardening `A1.10`/`A2.2` and manifest recipe `T` separately REQUIRE
  `speed_ratio <= 1.05` at 1920x1080 native, 600 requested frames, both fixtures and both orders, zero
  natural drops and zero holes; generic "no band" prose waives none of it.** ⛔ **No gate policy
  changed, no historical leg invalidated, no quiet machine or owner-app closure required.**
- **Manifest deltas PROPOSED, file unchanged:** the real levers are `IAI.Capture.SVE.ForceMiss` /
  `IAI.Capture.SVE.ForceMissPhase` (no `IAI.Bench` alias exists), so **`M-R8b` is CORRECTION REQUIRED**
  — cvar names, wanted-request accounting, and an archive-provenance claim resting on the wrong name;
  and **`M-R8c` carries two internal contradictions** — a bare-constant arm count the capture FSM
  refutes, and a falsifier that disables the identity guard `A2.1` requires it to retain. Two new rows
  (`M-ID1`, `M-ID2`) stay proposed and not runnable; `M-GEO2` is withdrawn.

**Review cadence recorded, per the owner's delegation:** Codex may drive bounded source-backed
documentation corrections **without** a Chat round after each one. **Chat reviews the coherent
corrected design before implementation, the final predictions before the first physical leg, and
evidence/source before merge**, and is re-engaged for material correctness/gate changes or
contradictory evidence. Owner-only meaning, physical and delivery boundaries are unchanged.

⛔ **Unchanged and still held:** no valid candidate pairing baseline (four attempts INVALID,
unretried, **no cause established in either direction**, no valid pairing verdict of any kind) · the
078-05/078-06 valid pairing failures remain failures, root cause OPEN · B1, the pairing predicate, the
coverage floor, the attempt budget and the four-class stop taxonomy untouched · offline sorting flipped
no candidate verdict but moved the A reference's settle start (1 -> 0) ⇒ **no order-insensitivity claim
and no `F1` clearance** · the A/B/A/B/A/B interleave NOT adopted · `M-F1b` UNRUN · the writer byte
bound OPEN (`deflateBound + 256` omits PNG framing; no process/GPU/scratch cap) · publication /
row-orphan / auditor semantics HELD for F7/F8 · every `open_arms` entry, all sixteen findings and the
full regression set required · Lyra needs its module SET plus manifest · **the exhausted campaign is
never automatically replayed.**

- **VERDICT: NEEDS-DECISION** — the revised design awaits exact-design review before implementation.
  Document completion is not design, execution or release approval; the campaign remains INCOMPLETE
  and release remains HELD.

### 078-17, 2026-09-07 — the prerequisite / accounting correction; unsupported assertions REMOVED, interfaces named OPEN

Revised in place: `docs/predictions/2026-09-07-m51-identity-lifecycle-geometry.md` (superseding the
078-16 bytes `B4BD5703…E5F8C`, 1,078 lines). Journal
`docs/sessions/2026-09-07-078-17-m51-prerequisite-accounting-correction.md`; the 078-15 and 078-16
journals gain dated correction pointers. Source: the independent review of the exact 078-16 bytes,
`_reviews/078-16-codex-independent-review-and-request-to-chat.md`, which **governs over the 078-16
report's own `ACCEPTED` labels**. ⛔ **DOCUMENTATION ONLY — no capture, checker run, build, cook,
staging, swap, source / shader / tool / fixture / schema edit, owner-app or office action, merge, push,
tag, replay or interleave; no agents, no worktrees.** Production frozen `6713f51`; `master` ==
`origin/master` == `ac13700`; ten owner handoffs preserved; CaptureBench `472a409` tracked-clean; Lyra
detached-clean `4a4bfcc`; staged exe untouched at candidate `D50DDE78`; **the gate manifest is
BYTE-UNCHANGED at `D33AF5D4…6025F9` and remains the single execution inventory.**

⚠ **Latest actual Chat response is still `_reviews/078-14-chat-campaign-evidence-review.md`.**
`_reviews/078-15-chat-identity-lifecycle-draft-review.md` was re-checked and is **ABSENT**; its absence
was **not** treated as approval. **Chat review was NOT REQUIRED for this documentation correction**
under the cadence recorded at `078-16`; **the next mandatory Chat checkpoint is the coherent corrected
design BEFORE implementation.** ⛔ **This changes no review authority and no release gate.**

- 🚨 **SUPERSEDES the `078-16` entry's resolved-interface claim.** The ownership and publication
  interfaces are **not** resolved, and the holds are **not** three named subunits. **Additionally
  OPEN:** the publication's **final seal and expected domain/consumer set** (a first mint followed by
  second-extension mutation is not an immutable sealed publication) · a **safe GT→RT carrier** — the
  engine **copies** the family and its views (`SceneRendering.cpp:2402-2406`, `:2415-2417`, `:819-826`,
  and `:2453` re-points the view at the renderer's copy), so **no raw game-thread pointer transfers**
  and **no bridge may be invented to close this** · **unique-vs-repeated family** distinction ·
  insertion / idempotent attachment and retention / reclamation · **world/viewport lifetime and
  address-reuse tokens** (raw opaque pointers today) · **membership may not be discarded at claim**
  while it is still needed for duplicate rejection or payload association · **per-index reconciliation**
  · the geometry **return join, transform/clipping algorithm and jitter convention**.
- 🔻 **Assertions WITHDRAWN, each against the project's own UE 5.1 tree.** **(a)** the four arm sites do
  **not** share one boundary — colour arms in `CaptureCurrentFrame` (`AnomalyCaptureSubsystem.cpp:4181`,
  call `:4234`), the other three in `OnWorldTickEndMask` (`:809`) ⇒ **a common freshness boundary is a
  PREREQUISITE, not an implemented fact**; `A2.1` rule 1's *"at `OnWorldTickEnd`"* is the intended
  design point. **(b)** `Failed(payload)` is **not** post-submit-only — the lookup miss returns at
  `AnomalySceneViewExtension.cpp:96-105`, **before** `:147`/`:149`/`:159`, so **published-but-no-readback
  is a real outcome and `ForceMiss` is that class**; `Matches − Submits` is a lead, not an exact count
  (`LookupKey` does not invalidate). **(c)** `AddEnqueueCopyPass` **is** pass construction
  (`RenderGraphUtils.cpp:842-855`; `EnqueueCopy` runs in the lambda at `:853`, which captures the raw
  readback) ⇒ **cancellation may not free a readback a scheduled pass still references.** **(d)** the
  fixed 4×-cap tombstone ring and monotone watermark are **unsound** — out-of-order retirement lets the
  watermark reject a **live** request, and aggregate counts cannot name missing indices. **(e)** the
  narrow *"GT bounds and proxy agree at that instant"* claim is **withdrawn in full**:
  `LevelTick.cpp:1046-1200` runs deferred updates, `PrimitiveComponent.cpp:620-633` tells the scene
  **only if** `ShouldRender()` (or the shadow/indirect/ray-tracing exceptions) holds, and
  `RendererScene.cpp:2248-2261` **enqueues** the proxy update.
- ✅ **Accounting rules now in force in the design, none of them a policy change.** Logical admission
  credit releases **exactly once at the FIRST terminal disposition** — `A2.1` rule 6
  (release-hardening:1234) preserved verbatim, with physical readback, scheduled pass, shared CPU
  payload and tag lease carrying **separate bounded accounts** and `A1.6`'s fire-ended-**and**-physically-
  resolved tag rule intact. `attempted = refused + admitted`; `admitted = outstanding + disjoint first
  terminal outcomes`. **Late and old-generation deliveries are diagnostics, never a second terminal and
  never an overwrite.** A **mid-run** requested-index refusal needs explicit per-index failure
  accounting; a **pre-`StartRun`** configuration refusal has no session request. Shared payload
  consumers include measure/normal/census with **no writer job** (census can request pixels,
  `AnomalyCensus.cpp:797-802`).
- ⛔ **Every gate schedule is UNSCHEDULABLE** until an independent clock/event, start/stop bounds and
  prior-state restoration on normal/stop/finish/deinit are specified — `session_index` stalls under
  withholding or cap refusal. `N-ID1` owes concrete opportunity generation and `>= 3` accumulation
  opportunities **in both controls**; `N-GEO1` a positive floor and a known moving series with a
  coordinate convention; `N-LC1` a bound and exercise floor. `N-LC2`'s post-submit cancellation device
  still **does not exist**; the encoded probe stays on implementation hold. **No gate became executable
  or passed.**
- **Preserved exactly and not inflated:** `R4`'s three request-ID-joined second-target payload losses
  per failing run, with **no** render identity assigned by ordinal and healthy same-render
  normal/target/census sharing still valid (never a one-slot FIFO) · `R5`'s withdrawal of direct
  near-induced 2D bbox motion, `N-GEO2` held · `R7`'s real lever names `IAI.Capture.SVE.ForceMiss` /
  `…ForceMissPhase`, the wanted-request accounting, the archive-provenance problem and `M-R8c`'s two
  contradictions — **all PROPOSED CORRECTIONS, not manifest edits.** `R7`'s clarification corrects
  **Codex's own prior wording**: frozen-pairing `A2.3` is same-row throughput **telemetry**, expressly
  not a pacing-validity band, while release-hardening `A1.10`/`A2.2` and manifest `T` separately still
  require `speed_ratio <= 1.05` at 1080p native, 600 requested frames, 30 fps, both fixtures, both
  orders, warm-up 60 for timing only, zero natural drops and zero holes. **No gate policy changed and no
  historical classification moved.**
- ⛔ **Unchanged and still held:** no valid candidate pairing baseline (four attempts INVALID, unretried,
  **no cause established in either direction**) · the 078-05/078-06 valid pairing failures remain
  failures, root cause OPEN · B1, the pairing predicate, the coverage floor, the attempt budget and the
  four-class stop taxonomy untouched · no order-insensitivity claim and no `F1` clearance · the
  A/B/A/B/A/B interleave NOT adopted · `M-F1b` UNRUN · the writer byte bound OPEN · publication /
  row-orphan / auditor semantics HELD for F7/F8 · all sixteen findings, every `open_arms` entry and the
  full regression set required · Lyra needs its module SET plus manifest · **the exhausted campaign is
  never automatically replayed.**

- **VERDICT: NEEDS-DECISION** — the design is **not accepted** and **no implementation subunit past
  step `2a` is designable**. Document completion is not design, execution or release approval; the
  campaign remains INCOMPLETE and release remains HELD.

### 079, 2026-09-15 — ORCHESTRATION RETURNS TO CHAT CLAUDE; the Codex pilot is over

- 🔑 **From 2026-09-15 the GDP mailbox is orchestrated by CHAT CLAUDE. The Codex pilot
  (`078-01` → `078-17`) is OVER. Codex is now an INDEPENDENT REVIEWER AT CRUCIAL CHECKPOINTS ONLY,
  named by chat — not on every brief.** §1's role lines are updated to match, and
  `_mailbox/README.md`'s **Orchestrator** line says the same. **ONE orchestrator per mailbox** is
  unchanged; the contended resource is the bench checkout. The pilot arrangement is marked
  superseded, **not deleted**.
- ⚠ **This entry is written on `m51` as a DOCS-ONLY commit and RELEASES NOTHING.** `m51` stays HELD
  exactly as `078-17` left it: the identity / lifecycle / geometry design is **NOT ACCEPTED**, no
  implementation subunit past step `2a` is designable, the campaign remains INCOMPLETE and release
  remains HELD. **The 079 unit is a SEPARATE unit** (verifier field validity, branch
  `fix/verifier-field-validity` off `master`) and touches no `m51` source and no gate.
- 📌 **Why it lands here rather than on the 079 branch:** this file exists only on `m51`. 079-01 found
  that and refused the three ways of forcing it — a second contract on `master` (two files claiming to
  be the single source), cherry-picking held `m51` content onto a `master` branch, or editing the
  frozen line for an orchestration note. Chat ruled the docs-only `m51` commit; **when `m51`
  eventually merges, the contract reaches `master` with it.**
- 🚨 **079 so far, recorded because it changes what may be shipped:** the `m49` label-pixel verifier
  **misfires on field data** — it marked almost every event NOT-VISIBLE / NOT-MEASURABLE on M2
  Concorde captures whose labels the owner verified BY EYE as correct. Reproduced at home on BANKED
  data with the unmodified tool. Cause: the gate thresholds a statistic bounded in `[0,1]` against
  `tau = median + 6·MAD`, which is unbounded; under camera motion `tau` exceeds 1.0 (measured
  **1.0950**, **1.1938**) and `d > tau` becomes unsatisfiable ⇒ NOT-VISIBLE unconditionally
  (**`G259`**). `tau` read `0.0040` — the floor — on **all six** historical bench legs, so the learned
  term had **never once** determined the threshold in any test ever run.
- ⛔ **CONSEQUENCES, IN FORCE UNTIL THE VERIFIER IS FIXED AND PROVEN ON A KNOWN-GOOD REAL SESSION:**
  the verifier **does not go to the client** for the M2 re-review · **card Section G readings on field
  sessions are VOID** · this unit goes **ahead of** the identity work.
- 🔻 **079-02 REFUTED ITS OWN PROPOSED FIX and is NEEDS-DECISION.** Ring subtraction
  (`net = d(region) − d(ring)`) failed three predeclared gates: it **destroys a real small signal on a
  PINNED camera** (the anomaly's GI bounce lands in the ring — `m26`'s `A35` had already measured that
  these anomalies change pixels outside their own bbox), and it **does not cancel camera motion**,
  which is parallax- and content-weighted rather than a global scalar. **Nothing is merged.**
- 🔻 **AND A CORRECTION TO 079-01's OWN RECORD:** it called the banked `M50L_LG9` Lyra leg *"a session
  whose labels are independently established as correct"*. **Withdrawn** — all six of its events carry
  `observability_measured: false` and `bbox_source: "projected"`, i.e. **the producer itself declares
  it has no pixel evidence.** A known-good REAL session for this gate **does not yet exist**, and
  finding or making one is a precondition for closing the unit.
