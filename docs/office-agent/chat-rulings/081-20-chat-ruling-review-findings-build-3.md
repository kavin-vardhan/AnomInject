# 081-20 — chat ruling: review of Codex's Stages 1–2 — no P1; fix F1/F2/F3 (+ small items) as build 3 before requalification (chat Claude, orchestrator, 2026-09-24 03:55 IST)

## Disposition — review accepted
Independent review by a fresh Code session (Opus 5.5, xhigh, 30 min, $15.28): full read of `031a103..5649a6d` Source, replay of 365 banked sidecars (1,450 measured rows, 0 invariant violations, retained 0 at every close), timeout-vs-late scan (no contradicted timeout outside the can-fail gates), Python port of the arithmetic (0 mismatches; ties and the `0,3,6,9,12` walk as specified), 1,200-run model of the stage clock (no hang, closure ≤ 5 s, bytes back to 0, never a wrong predecessor). **No P1 in Codex's code.** Two P2, eleven P3. Scorecard: Codex escaped defects = 0 P1 / 2 P2 / 11 P3.

## Rulings per finding
| id | ruling | why |
|---|---|---|
| **F1** P2 cancel-before-focus re-creates the deleted session folder with an empty sidecar | **Fix in build 3.** A stage belonging to a run that wrote no session never persists and never creates a directory; stale stages are discarded, not flushed, at the next `StartRun` / `Deinitialize`. | m55 must not break m16's "cancel writes nothing". |
| **F2** P2 multi-view / non-viewport family rejected before the legacy arm is consumed; zero legacy PNGs; pending arms grow per frame; mask pass skipped for families without m55 data | **Fix in build 3.** Whenever m55 cannot or does not attach data to a family (evidence disabled, multi-view, non-viewport/other-world family), **legacy colour and mask behaviour must be exactly pre-m55** and m55 records the frame as `unsupported_delivery` (evidence only). No per-frame growth of pending arms. The pre-m55 multi-view unsoundness itself (one readback per view under one RequestId) is **not** fixed in m55 — record it as a counter (`multi_view_families` in the run summary) and queue it for the m51 pairing work. | The prime invariant of a default-ON feature: it may refuse evidence, never cost legacy output. Split-screen reach on client hosts is unknown; the fail-safe is required either way. SIE/PIE viewport cases are covered by the same rule. |
| **F3** P3 legacy frames dropped when the stage exists but carries no m55 data (double fault) | **Fix in build 3** (same rule as F2): legacy delivery never depends on the stage's open/closed state or on sidecar persistence. | Same invariant; cheap once F2 is done. |
| F4 P3 closure can hold the game thread up to 5 s | **Document** (bounded; field reference + gotcha). | Rare (owner change / run-end drop); a fix would touch identity paths for little gain. |
| F5 P3 refusal names differ from docs | **Docs follow code** — the field reference states the exact mapping (timed-out predecessor → `predecessor_undelivered`; budget-refused predecessor → `predecessor_missing`; wrong-family mask → `view_mismatch`). | All are honest; no output change. |
| F6 P3 event `late_results` always 0 | **Remove the dead event-level field** (v1 unreleased); the run-level counter stays. | A field that cannot be non-zero misleads. |
| F7 P3 `duplicate_completion` counts unsupported frames | **Fix:** unsupported frames get their own counter; `duplicate_completion` counts true duplicates only. | Fault-sounding counter on healthy JPEG/resampled runs. |
| F8 P3 66 comment lines | **Strip before merge** (already on the checklist). | Convention. |
| F9 P3 bench surfaces partly gated | **Fix:** every `IAI.Bench.ChangeGate` value refused without `-IAIBenchFixture`. **M3 packaging checklist:** the `AnomalyBench` module is excluded from client packages. | A client typing a bench command must not lose frames; bench fixture names/coordinates must not ship. |
| F10 P3 row cap is a count (≈ 0.25 GB + a copy at persist) | **Document** the envelope (≈ 55 min at 30 fps before rows drop; memory at cap). **FUTURE v1.1:** stream rows to disk as they finalize. | Not a correctness issue; long-session scaling is real and queued. |
| F11 P3 single serial scan worker ≈ 45 ms/pair at 1080p; 1 ms sleep loop during run closure on a pool thread | **Document**; FUTURE (GPU/SIMD route, v2). Revisit only if the 081-18 long-run readings implicate it. | Ordinary recipes stay under the ceiling. |
| F12 P3 mean rounding undocumented | **Docs:** half-up at 4 dp. | |
| F13 P3 `ResetEpoch` keeps a deferred end | **Document** as unreachable today; no code change. | An untestable change in the identity path is worse than the note. |

Open questions: (1) SIE — covered by the F2 rule; SIE capture is not a product scenario. (2) Split-screen scope — unknown; F2's fail-safe makes it not matter for legacy output. (3) Orphan folder — removed by F1. (4) 8-completion margin — kept; the requalification's can-fail legs and the 081-18 readings watch it.

## Also in build 3
The 081-19 oracle exit code (1 on any mismatch, 0 all match, 3 cannot run) and one oracle-docs line naming the trusted-pair-id limit.

## Order from here
1. **081-21 (Code, xhigh): build 3** = F1, F2, F3, F6, F7, F9 code fixes + oracle exit code + docs (F4, F5, F10, F11, F12, F13); verification that needs no quiet PC (build, offline tests); bench legs **only if the PC is quiet**.
2. **081-18 rerun on build 3** once the owner schedules a quiet PC; then the requalification inventory on build 3 (oracle on every leg), then Stage 3's remainder.
3. After Stage 3: Codex reviews every Code-authored change (build 2, build 3, oracle, Stage 3).
