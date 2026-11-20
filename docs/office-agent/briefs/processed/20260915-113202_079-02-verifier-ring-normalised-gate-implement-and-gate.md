# NAME: 079 verifier field validity
# MODEL: claude-opus-5
# EFFORT: xhigh

# 079-02 — implement the ring-normalised label-pixel gate, prove it on the banked sessions. Resume session f7ac5221 (39%).

## 0. Rulings on 079-01's open items (chat, 2026-09-15)
1. **Contract entry:** `docs/OPERATING-CONTRACT.md` is m51-resident, so the orchestration-change entry lands on **m51 as a docs-only commit** (exactly the kind 078-15..17 made). That releases nothing on m51. Do it at the end of this brief, one commit, contract file only. When m51 eventually merges, the contract reaches master with it.
2. **No home moving-camera capture, no camera lever.** The banked sessions ARE the field surrogate: `M50L_LG9` (heavy motion, bbox-only — the known-good-called-NOT-VISIBLE case), `A1L_LEGA` (strong), `LYRA_SMOKE_01` (moderate), `A2L_LEGA` (light), `M49_GEDGE_*` ×6 (pinned). `Source/`/`Shaders/` stay untouched everywhere.
3. **Blast radius confirmed:** `tools/verify_capture.py` only. `measure_label_offset.py` byte-unchanged (its `ring_mean` is shared with Section G and stays as is).
4. 079-01's diagnosis is ACCEPTED as the mechanism: `d ∈ [0,1]` against an unbounded `tau = median + 6·MAD` ⇒ under motion `tau > 1` ⇒ `d > tau` unsatisfiable ⇒ NOT-VISIBLE unconditionally. G259 stands. The deviation from the plan gate was correct — read-only reproduction on banked data beat inventing a lever.

## 1. The fix (design fixed by chat; implement exactly this, on `fix/verifier-field-validity` from `c7d1703`)
**F-A Ring-normalised signal.** For every frame and every event region, `net = d(region) − d(ring)` where `ring` is the ambient band around the bbox that `measure_label_offset.py` already defines (`ring_mean`, mlo:92-93 — reuse it, do not re-implement). All onset/end/visibility tests run on `net`, never on raw `d`. `tau` is learned from the clean-frame `net` series (median + K·MAD, floored at `SIGNAL_FLOOR`) — same estimator, new statistic.
**F-B Unsatisfiable threshold is an instrument fault, never a verdict.** After learning `tau`, compute the attainable maximum of `net` for that region (`1 − median(d_ring_clean)`, or simply 1.0 if you can justify it in the header). If `tau ≥ attainable`, the event is `NOT-MEASURABLE(threshold unsatisfiable: tau=… attainable=… motion=…)`. Assert, never clamp — 079-01's reasoning holds: a clamped impossible test still FAILs silently.
**F-C No ring ⇒ no read.** If the bbox covers ≥ 90 % of the frame, or the ring has fewer than `MIN_RING_PX` pixels (pick a value, state it in the header), the event is `NOT-MEASURABLE(no ambient ring: region=…px of …px)`. This is the H3 case (2,073,600-px region).
**F-D Baseline floor.** Per-event baseline from the nearest clean frames of THAT region; if fewer than `MIN_BASELINE_FRAMES` (pick, state), `NOT-MEASURABLE(baseline n<min)`. This is H2.
**F-E Bbox-only tagging.** In bbox-only mode (no masks), every NOT-VISIBLE line carries `(bbox-only)` after the verdict so a reader knows it was not mask-measured. Verdict semantics otherwise unchanged; exit codes unchanged; `--report-only` unchanged; per-frame table columns may gain `net`/`ring`.
**F-F Header prints the envelope:** `motion M_med=…` (median clean-frame `d(ring)` or global fraction), `mode masks|bbox-only`, and the K/floor/min constants in use. The tool docstring states the validity envelope in plain words (camera motion handled by ring normalisation; regions covering the frame and dense schedules read NOT-MEASURABLE).
**F-G Batch mode.** `--all <root>`: treat every sub-folder with `labels.jsonl` as a session, run the gate on each, print one line per session `session | exit | PASS n SHIFT n NOT-VISIBLE n NOT-MEASURABLE n | VERDICT`, and write the full per-session output beside a `SUMMARY.txt` under `--out` (default `<root>\verifier-batch-<timestamp>`). Selftest is run once first and the batch refuses to start if it is not OK. Sessions without `labels.jsonl` are listed as `CANNOT RUN`, never crash the batch. (This replaces the owner's ad-hoc `verify_all_sessions.ps1`; the client gets one tool.)
**F-H Selftest gains the must-PASS half.** Add synthetic cases with a MOVING background (scroll a textured background N px/frame, N ∈ {2, 8}): aligned anomaly ⇒ PASS; label late/early by 1 ⇒ `ONSET-SHIFT(∓1)` with the right sign; a labelled region with NO anomaly under motion ⇒ NOT-MEASURABLE or PASS, NEVER NOT-VISIBLE… wait — a region with no anomaly under a correct label cannot exist; the case is: a labelled region where nothing changes locally ⇒ `NOT-VISIBLE (bbox-only)` still fires (the gate must still be able to fail); a whole-frame region ⇒ `NOT-MEASURABLE(no ambient ring)`; a forced unsatisfiable threshold ⇒ `NOT-MEASURABLE(threshold unsatisfiable)`. Existing 7 cases unchanged and still passing. Print the table as today.

## 2. Predeclared readings — write them in `docs/predictions/2026-09-15-verifier-field-validity.md` (amend 079-01's file; AMENDMENT 1, dated) BEFORE running anything
P1 Selftest: 7/7 old + all new cases ⇒ `SELFTEST: OK`.
P2 `M49_GEDGE_*` ×6 (pinned): `tau` prints `0.0040` on every event (fix inert on a pinned camera), PASS 4/4 each — identical verdict lines to 079-01's run except any new columns.
P3 `M50L_LG9` (the known-good session): **zero NOT-VISIBLE, zero SHIFT**; every event PASS or NOT-MEASURABLE with a printed reason. Before running, produce the eye-check evidence: for each of its 6 events, overlay frames at `start−1`, `start`, `start+1` with the bbox drawn (existing overlay tool) into the evidence dir, view them, and record per event whether the anomaly is visibly present inside the box at `start` and absent at `start−1`. That is the ground truth P3 is judged against — an event whose eye-check contradicts its label is a FINDING about m50's labels, reported as such, not hidden.
P4 `A1L_LEGA`, `LYRA_SMOKE_01`, `A2L_LEGA`: PASS count ≥ 079-01's count on each; no new NOT-VISIBLE; any verdict that changes is listed with before/after and the numbers.
P5 Batch mode over the bank folder holding those sessions: one line each, counts identical to the individual runs.
P6 Office (owner errand, after this brief is GREEN): re-run at the office with the new tool on the same M2 sessions; predeclare: mostly PASS, remaining NOT-MEASURABLE with printed reasons, zero NOT-VISIBLE on the events the owner verified by eye.
If P2 or P3 fails, STOP and report NEEDS-DECISION with the numbers; do not tune constants to pass.

## 3. Execution order
1. AMENDMENT 1 with §2 (commit, docs only).
2. Implement F-A..F-H. Keep `measure_label_offset.py` byte-identical (verify with `git diff --stat`).
3. Run P1 → P2 → P3 (eye-check first) → P4 → P5. Bank every output under the evidence dir; transcribe the summary + VERDICT lines and every non-PASS event line into the journal.
4. Docs: tool docstring/header (envelope + constants); `docs/client-readme.md` verifier section (what NOT-MEASURABLE reasons mean; bbox-only tagging; `--all`); gotchas: G259 already minted — add the "half-proven gate" rule if not already there; journal `docs/sessions/2026-09-15-079-02-verifier-ring-normalised-gate.md`.
5. Commits on `fix/verifier-field-validity`, Conventional Commits (`fix(verifier): …`, `test(verifier): …`, `docs(verifier): …`), pushed. NO merge to master in this brief — 079-03 merges after chat reads the evidence.
6. Then the one docs-only commit on `m51`: the OPERATING-CONTRACT entry (§0.1). Restore checkout to where you found it.

## 4. Rules
Predictions before results. No `Source/`, `Shaders/`, fixture, schema or harness change. No capture, build, cook. Existing evidence untouched. Report: per-P reading with the exact lines, any deviation stated, commit SHAs, and `VERDICT: GREEN` only if P1–P5 all read as predeclared; otherwise `NEEDS-DECISION`.
