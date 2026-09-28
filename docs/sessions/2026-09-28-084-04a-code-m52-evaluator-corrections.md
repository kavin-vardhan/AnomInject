# 084-04a — m52 part B harness: the evaluator gaps corrected before tonight's bench (ruling 084-02b decision 3)

Date: 2026-09-28 (IST) · Claude Code (Opus 5.5, xhigh), headless, fresh session · Branch `fix/m52-label-timing`
Spec: `_reviews/084-02b-chat-ruling-bench-tonight.md` **decision 3**, against the gate findings in
`_reviews/084-02b-codex-m52-minidelta.md` (§ "Two-sided gate — P1" and the purity paragraph).

**Python only.** No source change, no build, no staging, no swap, no game or editor launch, nothing that takes focus. The
main checkout stayed on `m51` `53bf725`; `master` untouched. Docs were committed from the branch worktree on
`E:\IA_BuildCache\_r84_host`. The harness lives outside the repo in `D:\IntrusiveAnomalies\_reviews`.

## 1. The corrections (evaluator id `084-04a`, stamped into `report.json` and every gate summary)

| | correction | where (`_reviews/084-03-lib.py` unless noted) |
|---|---|---|
| **(a) gap censoring** | A **break** between si and si+1 is a missing `session_index` or an engine-frame delta ≠ 1 (`brk`). The **onset** is CENSORED when a break sits between first_vis−1 and first_vis, or inside its 2-frame confirmation. The **offset** is CENSORED when a break sits between last_vis and last_vis+1 or anywhere inside the 3-frame clean suffix (which must be 3 **contiguous** clean frames), or when the span ends first. A break **between two visible frames** inside the window censors nothing, because settle gaps are routine in capture (measured: a jump at every burst boundary). **Per event, FAIL > CENSORED > PASS:** a censored event still FAILS on a definite failure. Definite failures are any unlabelled visible frame, a label before the first render-held frame, an over-label on a measured side, or an over-label past the confirmed clean suffix. Over-labels that depend on a censored edge are listed as `labelled_outside_window_ambiguous`. Leg verdicts: `FAIL`, `PASS`, `PASS-WITH-CENSORED` or `NO-JUDGEABLE-EVENT`. Residuals use only events whose edge on that side is measured. | `measure_event`, `gate_sync`, `summarize` |
| **(b) visible outside the window** | Every pixel-visible frame outside [first_vis..last_vis] **FAILS**, unless the paired null shows a visible drop at the event-aligned frame. Alignment: same target, same event ordinal, same offset from the event's first row. Test: the leg's relative threshold against the null's own pre-level. Reported as `visible_outside_window_failing`, `…_null_explained` and `recurrence_runs`. **Withdraws G345's "isolated runs are reported, not required".** ⚠ Where the threshold was calibrated from that null (thr ≥ its worst in-span drop), the null cannot excuse inside its spans. A **labelled** later run still fails as an over-label: the gate is single-window. | `null_explained`, `gate_sync` |
| **(c) missing masks** | Every labelled frame needs a **measured** mask. It FAILS on: no `mask_value`; `target_pixels` −1/null; `target_pixels > 0` with no PNG (or a PNG without the value); an empty mask on a pixel-visible frame. Reasons are counted. Gate (ii) now runs on **every** event with a record, not only pixel-measurable static ones. | `mask_misses`, `gate_window` |
| **(d) contamination** | Each capture-side `HOLD CONTAMINATED event=… from si=N` is joined **by reason** to an anomaly-side `HOLD CONTAMINATED` line. That line must say `REVERTING THE HOLD NOW`. The event needs a `RESTORE TRAIL CLOSED` at or after si N (`RESTORE UNRESOLVED` fails it). Every member frame from si N must carry `stuck_mip.contaminated = 1`, with no flag before si N or outside membership. Otherwise (iii) = `FAIL-CONTAMINATED`. Flags with no line fail too. **Every contamination is listed** (report section "contaminated events"). `apply_census_verdict` is kept separately, and `lifetime_note` states that a new user the monitor misses (Codex F4 routes) is invisible to this gate. | `contamination_check`, `gate_purity`; `084-03-window.py` passes the session |
| **(e) readings** | `edge_paths` per leg and `edge_paths_fix_totals` cover the six paths the ruling names: reopens, ORDER HOLD, gap-unknowns, carried trails, contaminated events, and unresolved/timeout, including the 64-tick `observeForced` from the RENDER-TRUTH SUMMARY. Each comes from log lines, run_summary counters and row flags. `monitor_cost` per FIX leg: **measured = false** (§3). | `edge_paths`, `monitor_cost`; report sections in `write_md` |

## 2. Proofs, both ways

**Selftest `_reviews/084-03-gateselftest.py`: 60 checks, 0 failures.**

| section | checks |
|---|---|
| sync (the 084-02b cases) | 11 |
| (a) gap censoring | 11 |
| (b) visible outside the window | 7 |
| (c) masks | 6 |
| (d) contamination | 15 (incl. the 5 purity checks) |
| (e) edge paths and monitor cost | 4 |
| `[before]` | 6 |

- The 084-02b can-fail cases still FAIL: late over-label (Codex), early over-label, onset +2, offset −4, nothing labelled.
  The exact window still PASSES.
- The old "isolated frame is REPORTED" check is inverted: it now FAILS (G345 withdrawal).
- `[before]` runs the **frozen 084-02b evaluator** (`_reviews/084-04a-evidence/before-084-02b-lib.py`, `431268AD`, byte
  copy) on the same inputs. It **PASSES all six**: Codex's four, a single spike, and census PASS + HOLD CONTAMINATED. So
  every correction closes a gap that really existed.

**Real banked pixels (`_reviews/084-04a-evidence/realpixel_proof.py` → `realpixel_proof.json` / `.md`):**

- **All 15 variant checks are proven, 0 contradicted.** They run on M52F_B_GC_GATED_NAT event 577 (window 507–521,
  confirmed suffix to 524, paired with M52D_G2_CANFAIL).
  - PASS: repaired; a jump between two visible frames (508|509).
  - FAIL: onset +2; offset −5; early −2; late +4 (Codex).
  - (i) PASS but (ii) FAIL: masks missing; mask PNG absent.
  - CENSORED: a dropped si 522; a jump in the suffix (522|523); a jump before the onset (506|507).
  - FAIL: a 2-frame recurrence at 525–526, built from the event's own blur PNGs 516/515; a single spike at 525.
  - PASS: the same recurrence shown by the null.
  - FAIL: the **unedited real spike at 497** (`original_spike_unedited`).
- ⚠ **Two declared edits.**
  - **(1) Base edit:** 577's only defect is the G345 spike at si 497 (single, 10 frames before the onset), so every
    variant except `original_spike_unedited` re-points 497 at its clean neighbour 496. The window pixels are not edited.
  - **(2) Null carrier:** the paired null has no event at 577's ordinal, so a copy of the session carries the null role for
    `recurrence_shown_by_null`. That exercises the excusal path on real values; it is not a real null.
- **Why only one event:** under the corrected rules, **no unedited banked event can pass.** Across the 16 banked m52 legs
  (StackOBot, eras M52 to M52F) there are 23 measurable static events. 20 are censored: most because the span ends before
  a 3-frame clean suffix (those diagnosis configs had short post gaps), some on an engine-frame jump next to the offset
  (si 232|233, 533|534). The 3 uncensored ones are event 577 in GC, GD and GE. → G347 rule 3.
- ⚠ **OBSERVATION, reproducible, NO MECHANISM (G120).** Event 577's first row is si 492. In all three M52F legs (same
  schedule), a pixel-visible excursion starts at **si 497**, 5 frames after that row and 10 before the main blur (507–521).
  - GC: the single frame 497, the G345 case.
  - GD/GE: 497, 498 and 501. 497/498 form a 2-frame pair, so it becomes the pixel window [497..501] ("first pair wins").
    The real blur 507–521 then counts as outside the window, and the event FAILS (it failed under 084-02b too).
  - The paired diagnosis null has no event at this ordinal, so scene vs anomaly is **not established**. Tonight's B5 null
    runs the same schedule with no hold, and its aligned frame decides it: if the null shows it, (b) excuses it; if not,
    it is an unlabelled visible transient.
- **Bank re-evaluation, 084-02b → 084-04a** (the same 23 events): **CENSORED → FAIL 20, FAIL → FAIL 3, no PASS in
  either.** Legs M52C_G3B_SETTLE8, M52C_G7 and M52E_G7 move from NO-JUDGEABLE-EVENT to FAIL. Every one of the 20 has
  unlabelled pixel-visible frames inside its window: the baseline under-label that 084-01 measured, previously hidden by
  "censored ⇒ excluded".

**Dry run (stubbed launch, banked stand-ins):**

- `084-03-window.py dry` exits 0, preflight **0 problems**.
- Gates: B3_BASE **FAIL** (4 judged, 0 censored); B0_BASE NO-JUDGEABLE-EVENT (both events camera-moved); null PASS.
- The doctored proof is **proven** on B3_BASE:577 with the same declared base edit.
- No FIX sessions exist, so the edge-path and monitor sections show baseline legs only.

## 3. The monitor-cost reading cannot be produced by this build (NEEDS-DECISION)

FD621B27's `ScanHoldForNewUsers` has **no timer**. It increments `HoldMonitorScans`, which is emitted neither to
run_summary nor to the RENDER-TRUTH SUMMARY. Every leg is paced at 30 fps, and the pacer sleeps inside the game tick, so a
sub-budget cost is absorbed (G-M6). **Tonight's run cannot report the per-tick game-thread cost the ruling asks for.** It
reports `measured=false` with labelled proxies:

- components walked per tick (`HOLD MONITOR ON - N existing component(s)`);
- the Apply census ms (`PURITY ENUMERATION … in X ms`, a one-shot walk that also enumerates materials: a scale, not the
  per-tick cost);
- any future run_summary `*monitor*` key.

A real reading needs a source timer (084-04). That is chat's call.

## 4. Invocation — unchanged

```
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-03-window.py live
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-03-window.py live --with-optional
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-03-window.py gates
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-03-window.py dry
```

Harness bytes after this session:

| file | hash |
|---|---|
| `084-03-lib.py` | `05FEAE6E` |
| `084-03-window.py` | `51E7A2F5` |
| `084-03-gateselftest.py` | `79FE18EB` |
| `084-03-README.md` | `9457CA6D` |
| `084-03-common.py` | `532ED8ED` (unchanged) |
| `084-03-run.ps1` | `E175E6DD` (unchanged) |

The 084-02b bytes are frozen under `_reviews/084-04a-evidence/before-084-02b-*`.

## 5. What tonight should expect from the corrected evaluator

- More `CENSORED` offsets where a revert's settle gap sits next to the last visible frame. The 084-03 config has a 30-frame
  post gap, so span-end censoring should be rarer than on the diagnosis bank.
- A single noisy frame outside the window now FAILS the event (b). The report lists the frame, so chat can see it.
- A labelled later run (an F2 reopen that labels a real recurrence) FAILS the single-window rule.
- If the si-497-type early excursion recurs on the rock, "first pair wins" can make an early 2-frame transient the pixel
  window. The event then FAILS with its main blur listed outside the window, and its residual is not meaningful. Read
  `first_visible` against the label start before reading any residual for such an event.
