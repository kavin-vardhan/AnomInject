# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 081 m55 build 2 (Code, Opus 5.5)

# 081-17 — m55 build 2: 256 MiB payload cap + ownership census, then the fail-fast 1080p/Lyra legs

You are Claude Code, running **headless** from the GDP mailbox. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator), who rules on it.

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long — put status notes in the same message as your next tool call and keep going. End the run only when (a) every step below is done, (b) a stop condition below fires, or (c) something you need is deliberately protected from you or needs the owner. Every final message follows the structure at the end and starts with a `VERDICT:` line. Builds and legs outlive the tool timeout: run them in the background and poll; **never end the run while a background task is still running** (the harness then records the run as killed/RED). If the Bash tool stalls on this box (known), switch to the PowerShell tool. This does not override the rules on destructive actions below.

## Context (read the sources; this is only orientation)
m55 "measured change evidence" is on branch `feat/m55-change-evidence` (head `8f0f831`, docs only on top of built source `5649a6d`, Game exe **E8E2E43F**). It measures, per captured frame and tagged target, how many of the target's own mask pixels changed vs the previous frame, with a whole-frame control, into the sidecar `change_evidence.jsonl` — **measurements only, no verdict, `observable` unchanged**. Codex implemented Stages 1–2 and the completion clock (081-04 … 081-16). **From this round you implement** (owner's call). After Stage 3, Codex independently reviews everything you author, and a fresh Code session (not you) reviews Codex's work.

Where it stands: at 1080p the old 64 MiB payload budget saturates and refuses pairs (081-15). Chat's replacement "bound" was disproved by Codex before any build (081-16). The ruling now: **hard payload cap 256 MiB, gate = zero `budget_exceeded` + full required-pair yield, high-water is an explained reading (ownership census), never a gate.**

## Read first, in this order
1. `D:\IntrusiveAnomalies\_reviews\081-16-chat-ruling-memory-criterion-and-handover.md` — **the governing ruling**, full.
2. `_reviews\081-16-codex-budget-bound-prebuild-review.md` full, and `081-16-evidence\source-bound-audit.json`, `postflight.json`.
3. `_reviews\081-15-codex-completion-clock-requalification.md` full; `_reviews\081-15-chat-ruling-budget-bound-build-2.md` (its Ruling 1b bound, WARN and high-water gate are **superseded** by 081-16).
4. `_reviews\081-14-chat-ruling-closure-clock-g270.md` (§3.4 = the Lyra G270 branches) and `081-13-chat-ruling-nohold-requalify-g2.md` (pose-reference policy).
5. On the feature branch (read with `git show feat/m55-change-evidence:<path>` — the live checkout is `m51`): `CLAUDE.md` status block; `docs/predictions/2026-09-20-m55-change-evidence.md` (amendments at top); `docs/change-evidence-fields.md`; journals `docs/sessions/2026-09-23-081-1*`; gotchas G272–G282.
6. Codex's harness, which you reuse: `_reviews\081-15-suite.py` (cases `null1080` / `solid1080`, orders 0/1), `081-15-measure.ps1`, `081-15-stage.py`, `081-15-build.ps1`, `081-15-prepare.py`, `081-15-finish.py`, `081-15-b1*.py` (incl. the declared 1080p viewport normalisation), `081-14-lyra-run.ps1` / `081-14-lyra*.py`, `081-08-audit.py`; and `081-15-evidence\{final-ledger,final-readings,binary-v1,preflight,postflight}.json`.
7. For context only (do not start it): `_reviews\081-03-codex-implement-m55-v1.md` §Stage 3.

## Hard rules
- **One bench.** Verify at start: main checkout `m51` @ `53bf725` with E8E2E43F binaries staged; Lyra detached @ `caa68c6` with its original nine modules; no `UnrealEditor` / `StackOBot` / `LyraGame` / `UnrealBuildTool` process running. If any such process is running and is not yours, **stop and report — never kill it**. Never kill anything by process name; stop only PIDs you started.
- Restore the boundary at the end exactly as Codex does (checkout back to `m51`, Lyra source and modules restored), and prove it in `_reviews\081-17-evidence\postflight.json`.
- Never touch `master`, `m51`, tags, cooked containers, CaptureBench, `ToCodex\`, or the owner's untracked files. Commit and push **only** `feat/m55-change-evidence`. No merge, no tag, no cook.
- **One build set** (StackOBot Game, StackOBot Editor, LyraEditor). A compile error you fix before any leg runs is the same build; a second build after any leg has run is not allowed — stop instead.
- Predictions are written and committed **before** the build and before any result. No post-hoc waivers, no retry of a fixture-valid result, no threshold or budget change after seeing results.
- A harness self-fault before sampling (e.g. a checker that cannot run on this build) is HARNESS-INVALID, declared, not counted; at most two fix iterations per step. Any harness change is listed in the report.
- Codename discipline: projects and hosts by codename only (Concorde, Bates, Europa); never a real game title anywhere.

## The work
**A. Bootstrap check.** Confirm the state above and the hashes in `081-16-evidence\postflight.json`. Any mismatch → stop.

**B. Predictions amendment (dated, committed first).** In the m55 prediction doc: the memory criterion per 081-16 (hard 256 MiB payload cap; gate = zero `budget_exceeded` anywhere in the run + 20/20 required pairs per 1080p twin; high-water a reading with peak census; no bound, no WARN); every budget/high-water predicate re-worded; your expected high-water range for the 1080p twins labelled as a prediction, not a gate; the G270 branches restated. Commit before step C.

**C. Implementation — this and nothing else.**
1. Compiled default of `IAI.Capture.ChangeMaxBytes`: 64 MiB → **256 MiB (268,435,456)**. Admission stays byte-only; clock and refusal semantics exactly as 5649a6d.
2. Diagnostic (i): at every `budget_exceeded` — one log line plus a sidecar field on the refused row: bytes held; colours held split pending / previous / phase references (a shared owner counted once); masks held and their bytes; cursor; LatestIndex; open phases with first/last indices; what the head waits on (colour / mask / observed / sealed).
3. Diagnostic (ii): the same census at each new high-water (log without flooding — e.g. only when it rises by at least one colour's size, plus once at run end); the run summary carries the **peak census**.
4. Field reference updated for the new fields. No `change_bytes_closure_bound`, no formula-based warning.
5. Build the set, hash, archive and stage it the way 081-15 did (`binary-v2.json`, Lyra archive named `m55-stage2-r17-lyra-<hash8>`). Verify built = staged = archive before every leg.

**D. Stack 1080p twins, both orders (4 legs)** — exactly 081-15's `null1080` and `solid1080` recipes, orders 0 and 1; only the binary differs. Up to 3 attempts per leg for fixture-invalid captures only; the first fixture-valid capture is the reading. **Pass per leg:** 20/20 required pairs measured, **zero `budget_exceeded` anywhere in the run**, all invariant counters 0, five events. Report per leg: high-water with peak census, latency p50/p95/max, refusal reasons over all indices, onset readings vs control (unsubtracted). **Stop on the first fixture-valid failure** (NEEDS-DECISION) — do not run step E.

**E. Lyra G270, 300 frames** — A2's exact CLI from `081-14-evidence\command-LYRA_G3_N_A2.json` with 300 frames, the new Lyra modules, the 081-14 fixture sequence (input lock, settle, one L2 placement, independent L1 within 150 cm / 10°, respawn audit, foreground throughout). Fixture allowance 3; the first fixture-valid capture is the reading, never retried. Branches (081-14 §3.4, with zero `budget_exceeded` in required windows as part of "measured"):
- ≥ 3 events and every labelled phase's required pairs measured, invariants 0 → **PASS**;
- fixture-valid, every phase measured, but < 3 events → **OBSERVED-BELOW-EVENT-FLOOR** (does not hold Stage 3);
- 3 fixture-invalid → G270 **dropped for m55** as an UNRESOLVED observation;
- fixture-valid and any required pair refused by `closure_timeout` or `budget_exceeded` → **stop, NEEDS-DECISION**.

**F. Close.** Restore the boundary; write the journal `docs/sessions/2026-09-2x-081-17-code-m55-build2.md` and the CLAUDE.md status block on the feature branch; add G282 the line "a wait threshold is not by itself an admission/retention bound"; commit, push the feature branch; write `_reviews\081-17-evidence\` (ledger, readings, manifests, postflight) and your full report as `_reviews\081-17-code-m55-build2-report.md`.

## Do NOT start
The remaining requalification inventory (Stack 720 measurement/finalization, identity G1–G15, `ChangeMaxBytes 1` controls, NoHold, legacy comparisons, Lyra 720 twins, count/wall can-fails on build 2) — that is the next brief. Not Stage 3. Not any review of Codex's code.

## Final message structure (the mailbox reads this)
1. `VERDICT: GREEN | NEEDS-DECISION | RED` — GREEN only if steps A–F all completed and D and E are PASS, OBSERVED-BELOW-EVENT-FLOOR, or G270-dropped.
2. **Plain summary for the owner** — 4–6 short lines, no jargon.
3. What changed — commits (hashes), files, the exact diff scope.
4. Build identity — Game / Editor / Lyra hashes, archive names.
5. Leg table — leg, attempt, fixture result, measured/required, `budget_exceeded` count, high-water + peak census, latency, disposition.
6. Readings — onset vs control per leg; G270 branch reached and why.
7. Deviations and harness faults — every one, with how it was handled.
8. Boundary proof — postflight verdict line.
9. NEEDS-DECISION items — or "none".
10. Your wall-clock per step (A–F), for the owner's implementer comparison.
