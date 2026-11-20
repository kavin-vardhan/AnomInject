# m55 Stage 3 — consolidated report (081-41)

Build 3 `85c99b3` (Game exe `002805CF`), branch `feat/m55-change-evidence`. Stage 3 = oracle (081-19,
exit codes 081-21), cost (081-37 → 081-40), docs (081-41). The requalification of build 3 (081-23 →
081-36) is summarised here because Stage 3's cost gate ran on it. Sources: rulings 081-19, 081-24,
081-37, 081-38, 081-39, 081-41; reports `_reviews/081-40-evidence/stage3-cost-report.md` (supersedes
081-38's), journal 081-36 (the 92-leg ledger), predictions `docs/predictions/2026-09-26-m55-stage3-cost.md`
(AMENDMENT 1 governing) and `docs/predictions/2026-09-24-m55-081-23-requalification.md` (AMENDMENTS 1–6).

**Status: Stage 3 complete. NOT MERGED, NOT TAGGED. Next: Codex reviews every Code-authored change
(081-42), then the comment stripper, then the merge by Code (no-ff onto `master`, no tag unless the
owner asks).**

## 1. Oracle — `tools/verify_capture.py --change-oracle` (081-19; exit codes 081-21)

- Pillow-only, recomputes every measured pair and reference comparison from the delivered PNGs and
  target-mask PNGs; never reads a producer number as an input. Its output ends with the contract
  sentence: *agreement validates arithmetic and transport only — that the numbers in the sidecar are the
  numbers the delivered images contain. It does not establish renderer pairing, visible effect, or
  cause.* A producer that published consistent wrong pair ids with the numbers those ids name would
  match; pair selection is the identity contract (Stage 1 identity gates, re-run in c1/c2).
- Selftest 24/24 (22 at 081-19; 081-21 added the reference-only mismatch → exit 1 and the
  nothing-compared → exit 0 cases). `G-GRAD(b)` `0,3,6,9,12`: adjacent `chg_gt8` `[0,0,0,0]` against
  reference `[0,0,48,48]`. Five must-fail mutations fire, each on its row only; three deliberate oracle
  sabotages each broke the selftest. The six existing modes' exit codes are unchanged (G283).
- Exit codes (081-21): **0** every compared comparison matched (coverage 0 is printed and is a gate's
  failure, not the tool's); **1** any mismatch; **3** cannot run.
- Banked run (081-19): **228 sessions, 1,234 / 1,234 measured rows and 1,232 / 1,232 reference
  comparisons match** across every m55 binary, 0 unavailable.
- Since then, on build 3: exit 0 on every c0 ON session (179/179, 168/168, 220/220 compared), on every
  c1–c3 leg that measures (Lyra G270 28/28), and on all 13 cost B legs (081-38 and 081-40).

## 2. Cost

### 2.1 Headline — block P, paced 30 fps, run log OFF, gate 0 (AMENDMENT 1, 081-39/081-40): **PASS at 720p and 1080p**

A/B = build 3 evidence OFF/ON, same binary. Two ABBA blocks per resolution after a declared discard,
600-frame legs, steady window si 60–599. Statistic: Δ = mean(B) − mean(A), pooled within-condition SD,
one-sided 95 % bound (t = 1.943, 6 df). 16/16 counted legs VALID on attempt 1.

| clause | 720p Δ / upper | 1080p Δ / upper | rule | reading |
|---|---|---|---|---|
| game-thread ms / engine frame | −0.115 / **−0.072** | −0.037 / **+0.110** | upper ≤ +1.000 ms | PASS / PASS |
| writer PNG/s | +0.035 / +0.078 | −0.014 / +0.029 | upper ≥ 0, half-width ≤ 5 % | PASS / PASS |
| drops | 0 on all 8 | 0 on all 8 | all 0 | PASS / PASS |
| pacing held (arm fps) | −0.0003 / +0.0013 | −0.0023 / +0.0033 | upper ≥ 0 | PASS / PASS |
| B yield (vacuity ≥ 0.9) | 120/120 × 4 | 120/120 × 4 | ≥ 0.9 | valid |

Readings (not gates): writer latency p50 162.8–163.2 ms (720p) / 318.4–334.5 ms (1080p), no growth over
the leg; backlog high-water 6–7 / 11–15 frames; m55 high-water 19.35–20.28 MB / 56.02–68.47 MB (7.2–7.6 %
/ 20.9–25.5 % of the 256 MiB cap); worker 2.46–2.52 / 7.22–7.42 ms per captured frame (12.3–12.6 /
36.1–37.1 ms per measured pair), off the game thread; completion latency p50 164 / 324–336 ms;
`budget_exceeded` 0, `closure_timeout` 0, late 0, rows dropped 0; `census_cycles` 132–135 / 119–123 per
600 frames. The pacer's busy-wait (≈ 0.2 ms/frame) is on both sides of every Δ (G303).

### 2.2 Stress — pacing OFF (081-38; labelled, not the headline, not re-run)

| res | label | game-thread Δ / upper | writer Δ / upper | verdict | cause |
|---|---|---|---|---|---|
| 720p | pacing-off stress, measured-pair yield 0.17–0.20 | −0.083 / −0.002 | +1.23 / +2.51 PNG/s | **PASS** (as declared) | — |
| 1080p | B1 yield 8/60 = 0.13, LEG-FAILURE | — | — | **UNRESOLVED** | writer saturation (≈ 32.5 PNG/s vs ≈ 57 fps armed, backlog 141) → retained payload 267.6 MB (99.7 % of the cap) → honest `budget_exceeded` (30 of 60 windows) |

The 720p yield collapse is `view_mismatch` 60–68 and `unsupported_delivery` 22–28 per leg: the mask is
served from the view family one before the colour frame (camera Δ 0). m55 refuses those pairs — the
identity contract working; the mispairing is the m51 defect (G302).

### 2.3 Diagnostic pair (outside every gate) and the FPS-21.5 prior

`S3_DIAG_A/B` (720p, pacing OFF, run log ON): no stall anywhere in either leg; the prior's single 1.56 s
start-of-run stall **did not reproduce (n = 2)** ⇒ the prior is **closed as PARTIAL** (081-37), its
mechanism not established. The declared cycles prediction (±0.3 ms of the run-log-OFF legs) **MISSED**:
the run log ON costs ≈ **+0.99 ms** of game-thread CPU per engine frame, identically with evidence OFF
and ON (G304, mechanism not established). DIAG B − A game thread −0.081 ms: no visible m55 cost with the
run log on either.

### 2.4 Instrument limitation and prediction misses

- The per-process GPU-engine counter reads **0.0 %** for the game on every leg: the game's GPU load is
  not measured by this instrument (cause not established). INVALID-LOAD reads the foreign GPU only.
- Misses, reported and not refitted: the paced **arm-rate band** (declared 24.6–25.0 / 24.5–25.0, read
  25.062–25.071; the band came from whole-session priors, the gate reads the steady window; no verdict
  effect) and the **diagnostic cycles** prediction (§2.3). Every other A1.8 prediction hit.

## 3. Requalification of build 3 (081-23 → 081-36): complete, 92 legs

| window | legs | result |
|---|---|---|
| c0 | 6 long runs, 1800 frames (Lyra ABBA ON/OFF/OFF/ON, StackOBot ON/OFF) | VALID READINGS; plateau on both hosts (081-24) |
| c1 | 19 stack legs + 4 legacy-identity groups (20 legacy legs) | 19 PASS (C1_G8_N PASS-REEVALUATED; C1_G2_S A1–A3 ENV-VOIDED, A4 PASS); CB-N and CB-S PASS-REEVALUATED under AMENDMENTS 5–6; MAIN-N and MAIN-S PASS |
| c2 | 42 stack legs (twins, delay 3, hide, hide-depth, occluded, blink, short, drop, run-end, teardown, moving, identity devices G1–G13, both orders) | 42 PASS, all attempt 1 |
| c3 | NoHold × 2, Lyra G270 + 720p twins | 5 PASS (NOHOLD_N A2; Lyra G270 on the PASS branch: 7 measured events, 28 measured + 4 `empty_region`, oracle 28/28) |

First runtime proofs in this campaign: the refusal census (C1_LOW, `ChangeMaxBytes 1`, all 90 refused)
and the G10 forced colour refusal. Nothing dropped, nothing below a floor, no FAIL outstanding.

**AMENDMENTS 1–6** (each dated, declared before use, proven both ways, checked for no verdict flips; none
loosened a stack predicate, threshold or m55 measurement):

| # | brief | what |
|---|---|---|
| 1 | 081-26 | the G8 "no receipt" clause re-encoded to match the ruled class; prediction unchanged (post-hoc, dated) |
| 2 | 081-27 | pre-run: PQ-A, PQ-B and the G270 `empty_region` clause |
| 3 | 081-28 | pre-run: the G270 vacuity guard |
| 4 | 081-30 | the person-present gate and ENV-VOIDED — scheduling only, no prediction changed |
| 5 | 081-32/33 | legacy identity: the coalesced-arm unmeasured frame (G295) under signature + cap, tags modulo one bijection, the historical run-unique set in the cross-pair clause; A.6 (the excused frame's colour file present, same size/format) |
| 6 | 081-35 | legacy identity: m47 shader-readiness keys excused under their own PREWARM signature (G298) with a cap; G297 self-excused singletons made a reported reading with a recurrence tripwire |

## 4. Priors carried into Stage 3

| prior | source | Stage 3 reading |
|---|---|---|
| FPS 21.5 vs 30 on evidence-OFF MAIN4 (081-05) | one Stage-1 leg, run log ON | PARTIAL: one 1.56 s start-of-run stall through the per-line run-log flush (G300); did not reproduce in n = 2; closed |
| high-water 17.5 / 19.4 / 39.4 / 49.8 / 66.4 MB | paced bank legs (64 MiB era and build 2) | kept beside: paced block P 19.35–20.28 (720p) and 56.02–68.47 MB (1080p); pacing-off 36.86–46.08 (720p) and 267.64 MB (1080p, at the cap) |
| build-2 twins at 1080p 51.9 / 64.3 / 66.4 / 70.5 MB; Lyra G270 197.0 MB | 081-17 | not re-run in Stage 3; the 256 MiB cap held every paced leg below 26 % |
| paced required-pair yield 20/20 | c2 twins | 120/120 on all 8 paced B legs |
| c0: evidence ON ≈ 0.12 logical cores, ON leg 0.55 % slower (declared risk 1) | 081-24 | vanished with the run log off: arm Δ −0.001 % (720p), −0.009 % (1080p) |
| scan ceiling ≈ 13.5 ms/pair (720p), ≈ 45 ms/pair (1080p), upper bounds | 081-20 F11 | 12.3–12.6 and 36.1–37.1 ms per measured pair |
| writer drop 1–3 % at 1080p pacing off (predicted, no tolerance added) | 081-37/38 | writer saturated; 1080p stress UNRESOLVED as recorded |

## 5. The c0 long-run reading (081-24)

Six 1800-frame legs, 1800/1800 PNGs and 0 drops each; oracle exit 0 on every ON session. **Plateau on
both hosts**: writer latency flat per decile on every ON leg; stage high-water reached in the first half
and never exceeded (Lyra 64.3 / 134.8 MB, StackOBot 66.4 MB); 0 `budget_exceeded`, 0 `closure_timeout`.
**Evidence ON vs OFF: no measurable effect** on sustained fps, PNG rate, writer latency, CPU or drops;
stage ≈ **0.12 logical cores**. No build 4. Block P reproduces the picture at 600 frames with full yield
(bg_worker cores B 2.679–2.691 vs A 2.608–2.621 at 720p; B 6.828–7.181 vs A 6.603–6.922 at 1080p).

## 6. UNEXERCISED — verbatim

Every line marked UNEXERCISED in `docs/predictions/2026-09-20-m55-change-evidence.md` at `ab634f5`
(the file is byte-identical to `805f317` and `2125159`; the block is identical to 081-40's).

```
90:   pair `unsupported_delivery`, no measured row. Otherwise **UNEXERCISED**, said so.
104:   a fixture-only fault). The RT duplicate-claim `check` no longer runs at SI 8 (UNEXERCISED on build 3).
192: - **(i) is expected NOT to fire in D or E.** Its runtime proof is therefore **UNEXERCISED this
346:    writer failures; a path requiring engine changes may be explicitly UNEXERCISED with
735: table (§9) carries this as **UNEXERCISED, with the reason**.
1184: ### 9.3 Every row of Codex §4 and §6.6 — a gate, or an explicit UNEXERCISED line
1196: | **same-size camera cut** | ⛔ **UNEXERCISED** | nothing in the producer distinguishes a teleport from a fast pan, and no lever synthesises one. Reported only via `cam_moved`, which is a caveat and not a refusal (§2.5). Naming what would exercise it: a bench lever that teleports the *camera* — `IAI.Bench.TeleportTargetOffscreenAt` moves the target, not the view. |
1198: | **first capture after a hitch** | ⛔ **UNEXERCISED** | no lever induces a hitch. Both capture times are in the receipt and both are published, so a consumer can see the gap; the producer does not refuse on it. Naming what would exercise it: a stall lever of the `m31`-era render/game-thread shape, which does not exist on `master` |
1201: | **gradual sub-threshold change** | `G-GRAD` (**partial, declared**) | two halves. **(a)** `solid_swap delay=3` proves the retained reference, the window and the ordered stage end to end, with `ref_gt8` large on the delayed pair while adjacent pairs read quiet. **(b)** Codex's exact counterexample — all target pixels walking `0,3,6,9,12` — is proven as a **synthetic fixture in `--selftest`**: four adjacent `chg_gt8` of 0 against `ref_gt8` at the full count. ⛔ **UNEXERCISED in the producer:** no runtime-authorable sub-threshold colour ramp exists without a material parameter, and adding one is a `Content` change that forces the cook §0.1 avoids. |
1286: 4. **Camera cuts and hitches are UNEXERCISED** (§9.3), and both are named rather than assumed away.
```

Also unexercised, stated in the docs rather than the predictions file: **1440p and 4K** (the memory
envelope's 195 MB and 440 MB are planning estimates, not measurements).

## 7. Product findings for m51 / M3 (pre-existing, not m55; FUTURE, filed not built)

| id | finding | cost | proposed fix |
|---|---|---|---|
| **G295** | a target-mask render that slips a frame serves the next frame's arm; that frame comes out unmeasured (`mask_state: unmeasured`, `target_pixels -1`) | yield, not correctness | a per-arm pixel readback, or re-arm the coalesced arm on the next render |
| **G298** | m47's shader-readiness mark fired on a packaged build and marked 90 good frames `shaders_pending` (`shader_prewarm_incomplete 1`); run-wide, not per anomaly; the "structurally zero" wording is refuted | a false-positive client quality flag | tie the marker to the materials in use; log which material is incomplete; correct the wording |
| **G302** | with pacing OFF the mask is served from the view family before the colour frame; m55 refuses those pairs (`view_mismatch`/`unsupported_delivery`) | pacing-off yield collapses (0.17–0.20) | m51 mask/colour pairing |
| **G304** | the m38 run log costs ≈ +1 ms game-thread CPU per engine frame (pacing OFF, run log ON); mechanism not established | debug-only; off in delivery mode and in the gated legs | clients told to leave it off; the mechanism is unchased |
| **`coverage_pct`** | timing-sampled: computed from the live world/view when the drain processes the anchor frame; G297 singletons on it appear on both binaries | legacy-identity noise; slight run-to-run difference | compute it from the frame's own mask (a legacy-output change, outside m55) |

## 8. Docs (081-41)

- `docs/change-evidence-fields.md` — the field reference rewritten against the source: what it is (no
  verdict in v1, `observable` unchanged), where/when, the measurement (adjacent pair, control,
  pre-onset reference, histogram), the camera delta, every pair-row, receipt, event-row and phase
  field, the 14 refusal reasons with meanings, every `run_summary` key, the memory envelope, cost and
  oracle usage. The earlier development text is kept below an "Internal development notes" heading,
  the reference above it authoritative.
  **Cross-check against the writer source: 178 emitted keys, 0 undocumented, 0 documented-but-not-emitted,
  census fields all documented** (`field-crosscheck.py`, output `field-crosscheck.json`), with both
  directions proven able to fire on a mutated copy.
- `docs/client-readme.md` — new section 9 (what it is, reading a row, the 14 reasons, late/gradual
  change, memory, cost, oracle, `positive_frames`, the two test entries, known limits); `change_evidence.jsonl`
  added to §8.0's file list; two pointers where the shader-readiness text is contradicted by G298.
  **Identifier grep over the 179 added lines: 0 hits** (brief numbers, G-numbers, chat/Codex/Code,
  `_reviews`, AMENDMENT, ruling, requalification windows; positive control 4/4).
- `docs/architecture.md` — catalogue note for the bench twins.
- `docs/gotchas.md` — **G305**: a brief's field names and formulas are spec text; no `cam_delta` field exists (three quantised fields),
  and the closure-bound formula withdrawn as a bound in 081-16 ships as a labelled planning estimate.
- Journal `docs/sessions/2026-09-26-081-41-code-m55-stage3-docs.md`; CLAUDE.md status block on the branch.
- Not changed, reported for chat: readme Step 6 runs `python tools\verify_capture.py`; the bundle ships it as `host-tools\verify_capture.py`.
- Merge-prep inventory: `merge-prep-inventory.md` (this directory).
