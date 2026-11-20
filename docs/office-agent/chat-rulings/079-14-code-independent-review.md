# 079-14 — Claude Code's independent review of `fix/verifier-field-validity` (2026-09-20)

**Role:** Codex implemented briefs 079-09..13; this is the independent review. I did not edit
the branch, the tool, the docs, CaptureBench or the bank. Everything below was produced from a
detached read-only worktree (`git worktree add --detach C:\ClaudeTemp\079-14\wt c1945b6`, removed
at the end with `git worktree remove --force`) and scratch fixtures under `C:\ClaudeTemp\079-14\`,
now cleared.

**Everything cited here is preserved, outside the repo, beside this file:**

| Artifact | Path |
|---|---|
| Fixture generators (rounds 1–3, 35 fixtures) | `_reviews/079-14-fixtures.py`, `-fixtures2.py`, `-fixtures3.py` |
| Predeclarations, written before any run | `_reviews/079-14-predeclared-round1.md`, `-round2.txt`, `-round3.txt` |
| Mutation probe | `_reviews/079-14-mutation-probe.py` |
| Raw per-pair `d` probe (F5's numbers) | `_reviews/079-14-probe-raw-d.py` |
| All gate output, sweep, scorer ledger, mutation table | `_reviews/079-14-evidence/` (55 files) |

**Repo state on exit, verified:** live checkout on `m51` `53bf725` with **zero tracked
modifications**; `fix/verifier-field-validity` == `origin/...` == `c1945b6`; `master` `ac13700`;
the pre-existing Lyra worktree untouched; no commit, push, merge, tag, build, cook or capture.

**Candidate HEAD:** `c1945b6` *"fix(verifier): separate motion caveats and revise peak selection"*.
`git rev-parse fix/verifier-field-validity` == `git rev-parse origin/fix/verifier-field-validity`
== `c1945b64060ef5c10ed5a379443340d7574db8d6`. The live checkout was **not** switched: it was on
`m51` `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67` at the start and at the end, and `master` is
`ac13700d99fe116056f5aa11611f8931a563a3f2` == `origin/master`.

---

## 0. Verdict

**CHANGES REQUIRED** — two documentation defects, both small and doc-only. **No P1 (blocking)
finding: B1, B2, B3, B4, B5 and B6 all held.**

**I could not produce a false NO-TRACE.** 35 purpose-built fixtures, every expectation
predeclared, including constructions aimed squarely at B1 — an above-`tau` change hidden outside
both edge windows, a moving target with exact per-frame masks, a change reachable only through an
extrapolated mask, a plateau whose interior is not a peak, a fabricated run sitting beside a real
one, and five separate ways of breaking mask or RGB coverage mid-span. Every one was caught and
reported `UNASSESSABLE` or `OFFSET-NOTE`; `NO-TRACE` fired only where the pixels genuinely
contained no change. A 143-session bank sweep produced zero NO-TRACE. A 16-point mutation probe
confirms the whole-span guard is not decorative: deleting it turns the selftest red.

The two CHANGES REQUIRED items are documentation, not behaviour:

- **F1** — the single most decision-relevant measured number on this branch (24 of 314 scored
  cells read `CONSISTENT` while carrying a known one-frame label shift) is published only in a
  file outside the repo, not in the characterisation doc the tool points readers at.
- **F2** — an internal review-finding id (`079-08 F2`) and an internal hold statement
  (`Section G remains held…`) now ship inside the client README, and the former is **printed by
  the tool on every run**.

Both are a few lines of text. The semantics, the coverage rules, the peak/assignment walk and the
NO-TRACE guard are sound as implemented.

I also recommend adding one fixture (§4, M15): the rule that a NO-TRACE run can never be hidden
by a healthy sibling run in the same event passes all 94 fixtures and all 22 contracts when
deliberately broken, and its failure mode is a fabricated label passing silently.

---

## 1. Reproduction of Codex's readings (brief §4.1)

| Reading | Codex | Mine | Match |
|---|---|---|---|
| `--label-pixel-gate --selftest` | 94 cases OK, exit 0 | `SELFTEST: OK - 94 cases`, exit 0, 32.9 s | ✅ |
| `test_verify_capture_consistency.py` | 22 contracts | `Ran 22 tests … OK`, exit 0, 1.9 s | ✅ |
| Pinned bench ×6 edges | 64/64 observed, all zero-delta | **64 edges, 64 `delta=+0`**, 0 `NO-TRANSITION`, 0 `UNASSESSABLE` | ✅ |
| Pinned bench CONSISTENT events | 24 (4 per leg) | 4+4+4+4+4+4 = **24**, all `(4 with caveat)` | ✅ |
| Pinned bench `tau` | `0.0040` | `tau=0.0040` on every pinned edge, all six legs | ✅ |
| Pinned bench exits | 0 | 0 on all six | ✅ |
| `A2L_LEGA` / `LYRA_SMOKE_01` / `A1L_LEGA` / `M50L_LG9` | see `079-13-evidence/bank-validation.txt` | **verdict strings identical line for line** (see §1.1) | ✅ |
| `M50L_LG9` | 6 READING + `UNREAD-BBOX-ONLY` | 6 READING + `UNREAD-BBOX-ONLY` | ✅ |
| Sweep ledger | 572 planned / 314 scored / 258 unscored | `planned=572 cells=314 unscored=258` | ✅ |
| Sweep classes | 218 recovered / 55 wrong / 2 no-transition / 39 unassessable | `{'recovered': 218, 'wrong': 55, 'unassessable': 39, 'no-transition': 2}` | ✅ |
| 55 wrong, 24 inside CONSISTENT runs | report L74 | `wrong by run_outcome: {'CONSISTENT': 24, 'PARTIAL': 8, 'OFFSET-NOTE': 23}` | ✅ |
| Batch/CLI exits | 2 / 0 / 3 | see §1.2 | ✅ |

**No mismatch was found in any reproduced reading.**

### 1.0 The sweep was re-run, not just re-read

I did not take the published ledger on trust. I re-ran the whole 572-key sweep myself with
`IAI_VERIFIER_TOOLS` pointed at the **branch-tip working file** (`615b6c8f…`, CRLF), against the
same three masked sessions:

```
CODEX planned=572 scored=314 unscored=258  {'recovered': 218, 'wrong': 55, 'unassessable': 39, 'no-transition': 2}
       wrong=55  by run_outcome={'CONSISTENT': 24, 'PARTIAL': 8, 'OFFSET-NOTE': 23}   verifier_sha=badd0e3e…
MINE  planned=572 scored=314 unscored=258  {'recovered': 218, 'wrong': 55, 'unassessable': 39, 'no-transition': 2}
       wrong=55  by run_outcome={'CONSISTENT': 24, 'PARTIAL': 8, 'OFFSET-NOTE': 23}   verifier_sha=615b6c8f…

cell-key sets identical: True
cells whose classification differs: 0
```

and the regenerated `verifier-characterisation.md` is **identical to the shipped one except for a
single line** — the verifier SHA (172 lines each, one differing line; see F4). The published
characterisation tables are therefore confirmed to come from the code on this branch, by a second
reviewer running an independently line-ended copy.

### 1.1 Bank verdicts, mine vs Codex's `bank-validation.txt`

```
M49_GEDGE_BL_NAT  NO FAILURE FOUND (4 consistent (4 with caveat), 0 offset-notes …, 0 partial, 0 unassessable, 0 readings)
M49_GEDGE_CT_NAT  NO FAILURE FOUND (4 consistent (4 with caveat), 0 …, 0 partial, 0 unassessable, 0 readings)
M49_GEDGE_MO_NAT  NO FAILURE FOUND (4 consistent (4 with caveat), 0 …, 0 partial, 0 unassessable, 0 readings)
M49_GEDGE_MT_NAT  NO FAILURE FOUND (4 consistent (4 with caveat), 0 …, 0 partial, 0 unassessable, 0 readings)
M49_GEDGE_BL_SYN  NO FAILURE FOUND (4 consistent (4 with caveat), 0 …, 0 partial, 0 unassessable, 0 readings)
M49_GEDGE_MT_SYN  NO FAILURE FOUND (4 consistent (4 with caveat), 0 …, 0 partial, 0 unassessable, 0 readings)
A2L_LEGA          NO FAILURE FOUND (3 consistent (3 with caveat), 0 …, 0 partial, 4 unassessable, 0 readings)
LYRA_SMOKE_01     NO FAILURE FOUND (3 consistent (3 with caveat), 0 …, 2 partial, 1 unassessable, 0 readings)
A1L_LEGA          NO FAILURE FOUND (4 consistent (2 with caveat), 1 …, 0 partial, 1 unassessable, 0 readings)
M50L_LG9          NO FAILURE FOUND (0 consistent (0 with caveat), 0 …, 0 partial, 0 unassessable, 6 readings)  + UNREAD-BBOX-ONLY
```

### 1.2 Exit contracts, measured

| Case | Expected | Measured |
|---|---|---|
| Single session, NO-TRACE | 2 | 2 |
| Single session, NO-TRACE, `--report-only` | 0 | 0 |
| Single session, no `labels.jsonl` | 3 | 3 (`LABEL-PIXEL: CANNOT RUN`) |
| Single session, cannot-run, `--report-only` | 3 (preserved) | 3 |
| `--all` over 24 sessions incl. 3 NO-TRACE | 2 | 2 |
| `--all --report-only` | 0 | 0 |
| `--all --report-only` over a cannot-run root | 3 | 3 |
| `--all` self-test gate | runs first, refuses on break | `BATCH: proving the instrument before using it on 24 session(s)...` |
| Batch identifiers | unique `<flattened>__<sha1[:8]>` | 24 unique, no collision refusal triggered |

---

## 2. My own fixtures (brief §4.2)

32 fixtures, written from the §1 semantics, not derived from Codex's generators
(`C:\ClaudeTemp\079-14\fixtures.py` and `fixtures2.py`). **Every expectation was predeclared in
`C:\ClaudeTemp\079-14\PREDECLARED.md` / `PREDECLARED2.txt` before any fixture was run.**

Geometry: 160×120, target 40×30 = 1200 px, mask value 200, constant background, so clean pairs
are byte-identical (`d == 0`) and `tau` collapses to the `0.0040` floor. That makes every reading
arithmetic rather than luck: `d` is a fraction with a 1200-pixel denominator I control exactly.

### 2.1 Round 1 — 24 fixtures

| # | Predeclared | Measured | |
|---|---|---|---|
| F01 interior-only change outside both windows | UNASSESSABLE (whole-span change at 18); **not** NO-TRACE | `UNASSESSABLE - whole-span change at frame 18 d=1.0000 > tau=0.0040`, exit 0 | ✅ |
| F02 change only in the end window's far edge | OFFSET-NOTE, end `delta=+4` | `OFFSET-NOTE(no-transition,+4)`, `end TRANSITION 30 … delta=+4` | ✅ |
| F03 moving target, exact per-frame masks | CONSISTENT, both `delta=+0` | `CONSISTENT`, `tau=0.0952`, both `delta=+0` | ✅ |
| F04 masks in-span only; only change is a pre-span pair | OFFSET-NOTE `-2`, extrapolation tagged | `OFFSET-NOTE(-2,no-transition)`, `onset TRANSITION 8 … delta=-2 … (mask extrapolated from frame 10 …)` | ✅ |
| F05 nothing ever changes, masks everywhere, `blink` | **NO-TRACE, exit 2** | `NO-TRACE … exit 2` | ✅ |
| F06 interior mask absent | UNASSESSABLE | `UNASSESSABLE(mask missing at frame 13)` | ✅ |
| F07 interior mask carries id 201 not 200 | UNASSESSABLE | `UNASSESSABLE(mask id 200 not present at frame 13)` | ✅ |
| F08 interior mask PNG truncated | UNASSESSABLE | `UNASSESSABLE(mask unreadable at frame 13)` | ✅ |
| F09 interior mask all-zero | UNASSESSABLE | `UNASSESSABLE(mask missing at frame 13 (empty))` | ✅ |
| F10 five one-frame runs | 5 × CONSISTENT | 5 × CONSISTENT, 10 edges `delta=+0`, assignments printed | ✅ |
| F11 two targets, overlapping windows | both CONSISTENT, no cross-target stealing | both CONSISTENT | ✅ |
| F12 span past the last frame | PARTIAL, end truncated | `PARTIAL`, `UNASSESSABLE(edge truncated by the session boundary)` | ✅ |
| F13 one-frame run beside a two-frame run | *I predicted CONSISTENT ×2* | **OFFSET-NOTE ×2** — **my prediction was wrong, the tool is right** (see F8 below) | ⚠ mine |
| F14 occluder crosses the target mid-span | CONSISTENT | CONSISTENT | ✅ |
| F15 lighting step on the onset | *I predicted CONSISTENT + caveat* | **PARTIAL** — **my fixture was miswritten** (the target never returned to ON, so there is genuinely no end transition). The reading is correct and is exactly 079-08 F3's rule. | ⚠ mine |
| F16 2 % of target changes (24/1200) | CONSISTENT | CONSISTENT, `d=0.0200` **exactly** | ✅ |
| F17 target leaves frame mid-span | UNASSESSABLE | `UNASSESSABLE(mask missing at frame 13 (empty))` | ✅ |
| F18 nothing changes, class `lighting_mismatch` | UNASSESSABLE | `UNASSESSABLE - NO-TRACE unavailable for class lighting_mismatch` | ✅ |
| F19 no masks at all, nothing changes | READING | `READING (bbox-only)` + `UNREAD-BBOX-ONLY` | ✅ |
| F20 plateau: change on every frame 11–14 | OFFSET-NOTE `+1`, peaks {11,14} | `OFFSET-NOTE(+1,no-transition)`, `other peaks in window: 14` | ✅ |
| F21 fabricated run beside a real run, one event | event NO-TRACE, exit 2 | `NO-TRACE runs{CONSISTENT=1 … NO-TRACE=1}`, exit 2 | ✅ |
| F22 fabricated event beside a real event | NO-TRACE 1 + CONSISTENT 1, FAIL | `CONSISTENT 1 … NO-TRACE 1`, `VERDICT FAIL`, exit 2 | ✅ |
| F23 RGB frame deleted mid-span | UNASSESSABLE | `UNASSESSABLE(RGB missing for pair 12..13)` | ✅ |
| F24 RGB frame with wrong dimensions mid-span | UNASSESSABLE | `UNASSESSABLE(RGB unreadable for pair 12..13)` | ✅ |

### 2.2 Round 2 — 8 further probes

| # | Predeclared | Measured | |
|---|---|---|---|
| F25 lighting step on both labelled edges | CONSISTENT + lighting caveat | `CONSISTENT (caveat: edge coincides with a whole-region change …)`, `ring d=1.0000` both edges, `CONSISTENT 1 (1 with caveat)` | ✅ |
| F26 NO-TRACE needing extrapolated edge masks | NO-TRACE + disclosure | `NO-TRACE … edge pairs use extrapolated masks; masks: in-span actual, edges extrapolated <=20`, exit 2 | ✅ |
| F27 `mask_value: 0`, sole value, nothing changes | UNASSESSABLE, never NO-TRACE | `UNASSESSABLE - NO-TRACE unavailable: mask id unresolved - sole value used` | ✅ |
| F28 60 % of the target churns every pair | CONSISTENT + motion caveat, never a refusal | `CONSISTENT (caveat: high regional change m=0.6000 …)`, `tau=0.6000`, both `delta=+0` | ✅ |
| F29 exactly 300/1200 target px change | `d=0.2500` exactly | `d=0.2500` both edges | ✅ |
| F30 change below the RGB-channel threshold (Δ5 < 8) | NO-TRACE (documented thresholded absence, R3) | NO-TRACE, exit 2 | ✅ |
| F31 `manifested: false` event | event UNASSESSABLE, never NO-TRACE | `UNASSESSABLE(non-manifested/empty event or no explicit frame_indices)` | ✅ |
| F32 span starting at frame 0 | PARTIAL, onset truncated | `PARTIAL`, `UNASSESSABLE(edge truncated by the session boundary)`, `end TRANSITION 4 … delta=+0` | ✅ |

### 2.3 Round 3 — the two coverage rules the README states explicitly

| # | Predeclared | Measured | |
|---|---|---|---|
| F33 in-span mask PNG with wrong dimensions, nothing ever changes | UNASSESSABLE; **no** extrapolation fallback; never NO-TRACE | `UNASSESSABLE(mask unreadable at frame 13 (dimensions))` | ✅ |
| F34 out-of-span masks carry only a foreign id; nothing changes | bounded boundary fallback ⇒ NO-TRACE with disclosure | `NO-TRACE … edge pairs use extrapolated masks; masks: in-span actual, edges extrapolated <=20`, exit 2 | ✅ |
| F35 same masks, real change on the labelled edges | CONSISTENT, both `delta=+0` | `CONSISTENT`, both `delta=+0`, extrapolation disclosed | ✅ |

These pin the README's two explicit claims — *"Corrupt masks and invalid dimensions are errors,
not permission to extrapolate"* and *"An explicit absent ID is an error inside the span; outside
it, a valid PNG containing only other targets may trigger the bounded fallback"* — and both hold.

**33 of 35 matched my predeclaration exactly. The two that did not were my errors, not the
tool's** — F13 because I had not applied R9's one-sided prominence rule to a 3-frame plateau, and
F15 because I wrote the fixture wrong. Both are recorded above rather than quietly re-predicted.

### 2.3 The B1 result, stated plainly

**No false NO-TRACE was produced.** The only route to `R_NO_TRACE` in the whole file is
`verify_capture.py:983`, and it is reached only after four guards in `_run_outcome` — span-pair
errors (`:968`), unresolved mask id (`:970`), producer class (`:972`) and the whole-span
above-`tau` scan (`:975`). The scanned range is
`lo = min(run.start, every edge lo)` … `hi = max(run.end, every edge hi)` (`:964-965`), which is a
superset of the span plus both edge windows, and the threshold used is `min` over the edges'
`tau` (`:974`) — the most sensitive of the two. `run["outcome"]` is assigned at exactly one place
(`:1092`) and exit 2 is gated solely on `counts[R_NO_TRACE]` (`:1175`). **There is no code path to
NO-TRACE that skips the whole-span check** (brief §4.3 answered).

F01 is the constructed adversarial case: a full-target change (`d=1.0000`) placed deliberately
outside both edge windows, so both edges legitimately read `NO-TRANSITION`. The span check caught
it and printed the frame and the number.

---

## 3. Wider bank sweep (my own extension, beyond the brief)

Codex validated 10 bank sessions. A false NO-TRACE on real banked data is the P1 that matters, so
I widened it: of the 1,399 session-shaped directories in `_bench_sessions_bank`, **535 carry a
`target_mask/` folder** and can therefore reach NO-TRACE at all. I swept one mask-mode session per
distinct bank family (`_try<N>` suffixes collapsed), in deterministic path order, with
`--label-pixel-gate --report-only`.

```
sessions swept : 143
runs reading NO-TRACE : 0
sessions with VERDICT FAIL : 0
aggregate events : 365 consistent · 3 offset-note · 72 partial · 184 unassessable · 21 reading
```

Families covered run from `A1F_*` through `M49_GEDGE_*` (alphabetical by family). **I stopped the
sweep at 143 rather than exhausting all 535** — the run was ~13 s/session and the marginal value
was falling; the number reached is reported rather than rounded up. Within it, `NO-TRACE` never
fired on real banked data, which is the conservative direction and matches Codex's 10-session
result.

⚠ This is evidence that the gate does not manufacture NO-TRACE on real sessions. It is **not**
evidence that those labels are correct — by the branch's own semantics nothing here certifies a
label.

---

## 4. Does the 94-case suite actually pin the rules? (my own extension)

A suite that cannot fail is not a suite (`G96`). I copied the tool into scratch and applied 16
single-point mutations, each to a rule the branch claims to implement, then ran the 94-case
selftest **and** the 22 contracts against each mutant (`C:\ClaudeTemp\079-14\mutate.py`).
**The branch itself was never touched.**

```
mutation                                 applied            selftest   contracts
----------------------------------------------------------------------------------
M0_control                               control            0          0
M1_peak_ratio_1.0                        mutated            0          1
M2_peak_ratio_9.0                        mutated            0          1
M3_marker_0.99                           mutated            0          1
M4_min_baseline_1                        mutated            0          0   <-- SURVIVES BOTH
M5_region_frac_0.999                     mutated            0          0   <-- SURVIVES BOTH
M6_DELETE_whole_span_change_check        mutated            2          0
M7_span_tau_max                          mutated            0          0   <-- SURVIVES BOTH
M8_DELETE_unresolved_guard               mutated            2          0
M9_DELETE_span_error_guard               mutated            1          0
M10_DELETE_class_allowlist               mutated            2          0
M11_span_lo_hi_narrowed                  mutated            0          0   <-- SURVIVES BOTH
M12_edge_window_bad_ignored              mutated            1          1
M13_identity_check_removed               mutated            1          1
M14_partial_promoted_to_consistent       mutated            2          0
M15_event_token_ignores_no_trace         mutated            0          0   <-- SURVIVES BOTH
M16_report_only_swallows_exit3           mutated            0          0   <-- SURVIVES BOTH
```

**The load-bearing guards are pinned.** Deleting the whole-span change check (M6) — the single
thing standing between the tool and a false NO-TRACE — turns the selftest red. So do deleting the
unresolved-id guard (M8), the span-error guard (M9), the class allowlist (M10) and promoting
PARTIAL to CONSISTENT (M14). Loosening or tightening the peak ratio (M1/M2) and moving the 0.42
marker (M3) are caught by the **contracts**, not the selftest — the two suites are complementary
and both are needed.

**Five mutations survive both suites.** One of them I can convert straight into a demonstration.

### M15 — the event-aggregation precedence is unpinned, and the failure mode is a silent pass

`_event_token` (`verify_capture.py:988`) checks `R_NO_TRACE` first so that a NO-TRACE run can
never be hidden by a healthy sibling run in the same event. Reordering that tuple so NO-TRACE is
checked last passes all 94 fixtures and all 22 contracts. Measured against my F21 (one event,
run `[10..15]` fabricated, run `[24..24]` real):

```
control  F21  exit=2  tokens=[NO-TRACE]
M15      F21  exit=0  tokens=[CONSISTENT]
```

**A fabricated label silently passes**, and nothing in the suite notices. F22 (two separate
events) does not discriminate — the NO-TRACE event has no sibling run — which is why the existing
fixtures miss it: none of them puts a NO-TRACE run and a non-NO-TRACE run inside one event.

*Suggested fix:* add F21 (`C:\ClaudeTemp\079-14\fixtures.py`, `F21_fabricated_run_beside_a_real_run`)
as a permanent selftest case with known answer `NO-TRACE`, exit 2.

### M7 / M11 — the NO-TRACE guard's *conservatism* is unpinned

- **M7**: `tau = min(e["tau"] …)` → `max` (`:974`). The whole-span scan then uses the *less*
  sensitive of the two edge thresholds, making NO-TRACE easier to reach. No fixture has two edges
  with materially different taus and a mid-span change between them, so nothing sees it.
- **M11**: `lo/hi` narrowed from `min(run.start, every edge lo) … max(run.end, every edge hi)` to
  just `run.start … run.end` (`:964-965`). That deletes the **edge-window half** of §1's NO-TRACE
  condition ("no change above tau across the span *and both edge windows*"). Nothing sees it.

I could not build a discriminator for either within this unit, and I want to be exact about why
rather than imply the code is at risk: for M11 you need an above-`tau` pair inside an edge window
that is *not* a peak while both edges still read NO-TRANSITION, and the one-sided prominence rule
makes the leftmost pair of any above-`tau` block a peak except at marginal amplitudes. So these
are **coverage gaps in the suite, not demonstrated defects in the code** — the code is correct as
written; the suite would not stop someone loosening it.

### M4 / M5 — weaker results, stated as such

- **M4** `MIN_BASELINE_FRAMES` 3 → 1: a one-pair baseline gives `MAD = 0` and `tau` equal to that
  single pair's value. Unpinned.
- **M5** `MAX_REGION_FRAC` 0.90 → 0.999: I record this as a **weak mutation** — the
  `moving_fullframe_region` fixture's region is the whole frame, whose area still exceeds
  `0.999·w·h`, so the refusal still fires and the mutation may change no reachable behaviour.
  Not evidence of a gap.

### M16 — the CLI's exit-3 preservation is unpinned by the suites (but is correct)

`main()`'s `sys.exit(0 if (args.report_only and code != 3) else code)` (`:2100`) is what keeps
`--report-only` from swallowing a cannot-run. Replacing it with `sys.exit(0 if args.report_only
else code)` passes both suites, because the selftest calls `label_pixel_gate` directly and the
contracts exercise `label_pixel_batch`, not `main()`. My §1.2 CLI checks do cover it, and the
shipped code is correct.

**Reading, stated plainly:** the suite is real — it kills every mutation that would produce a
false NO-TRACE or a promoted outcome. Its blind spots are the *aggregation precedence* (M15,
which is a silent-pass class and has a ready fixture) and the *conservatism* of guards that are
currently correct (M7, M11, M4). None of this changes the branch's behaviour today.

---

## 5. Findings

Severity uses the brief's own taxonomy: **P1** = one of B1–B6. Nothing below is P1.

### F1 — CHANGES REQUIRED — the branch's most decision-relevant measured number is published only outside the repo

`docs/verifier-characterisation.md` is the document the tool's own header points a reader at, in
two lines (`tools/verify_capture.py:1116-1117`), as the place where the known limitations live.
It lists all 55 wrong cells with their competing peaks. What it never states is the breakdown by
run outcome:

```
wrong by run_outcome: {'CONSISTENT': 24, 'PARTIAL': 8, 'OFFSET-NOTE': 23}
```

**24 of the 314 scored cells (7.6 %) had their label deliberately shifted by the perturbation
cohort and still read `CONSISTENT`.** That is the number a person needs in order to know how much
weight a `CONSISTENT` line carries, and it is the one number the doc's table cannot yield: its
columns are `Cell | Label | Selected | Expected | Observed | m_edge | Competing peaks`
(`docs/verifier-characterisation.md:20`) — there is no run-outcome column.

The figure exists in exactly one place: `_reviews/079-13-codex-delta-report.md:74`, which is
outside the repo, not shipped, and not linked from the tool, the README or the card.

*Reproduction:* `python -B m079_shift_recovery.py --json … --previous-cells …` with
`IAI_VERIFIER_TOOLS` pointed at the branch tools, then count
`Counter(c["run_outcome"] for c in cells if c["outcome"]=="wrong")`. My independent run
(§1, `C:\ClaudeTemp\079-14\out\char_repro.json`) gives the identical breakdown.

*Suggested fix (doc-only):* add a `Run outcome` column to the wrong-cell table, and one sentence
to the doc's opening paragraph and to README Step 6's `CONSISTENT` row — "in the measured
perturbation cohort, 24 of 314 scored cells read CONSISTENT while carrying a known one-frame
label shift."

### F2 — CHANGES REQUIRED — internal review identifiers and an internal hold now ship to the client

`docs/client-readme.md` is mapped into the client bundle as its `README.md` (CLAUDE.md, `m27`
entry: *"`PLUGINFILE` is a CROSS-REPO entry (`docs/client-readme.md` → the bundle's
`README.md`)"*). Step 6 adds two things the client cannot resolve and should not be reading:

- `docs/client-readme.md:216` — *"This is not 079-08 F2's boundary-mask reuse: there, real
  per-frame masks existed and were ignored; here, none exist."* `079-08 F2` is an internal review
  finding id.
- `docs/client-readme.md:270` — *"Section G remains held pending evidence and independent
  review."* Section G is the internal office-RDP procedure; to a client this reads as an
  unexplained hold on the tool they were just handed.

The same `079-08 F2` sentence is **printed at runtime on every client run**:
`tools/verify_capture.py:1113` emits it as a header line of the gate's output (I see it at the
top of all 35 of my fixture readings and all 10 bank readings).

*Reproduction:* run the gate on any session; the second header line is the 079-08 F2 sentence.

*Note:* milestone references (`m44`, etc.) already appear in that file and are an accepted
convention there; review-finding ids and internal gate holds are not.

*Suggested fix (doc-only):* keep the substantive distinction, drop the identifier — e.g. *"This
is not mask reuse: where real per-frame masks exist they are always used; extrapolation happens
only where the producer wrote none."* Delete the Section G sentence from the client file (it is
already stated, correctly, in `docs/office-rdp-card.md`).

### F3 — NOTE — encoding corruption re-introduced at `tools/verify_capture.py:55`

Master has `⚠ THE ASSET NAME COMES FROM …`. The branch has `Ã¢Å¡Â  THE ASSET NAME COMES FROM …`
— the `⚠` double-encoded, which is G115/G141's exact signature. It is the only non-ASCII
character in the file and it sits in the module docstring the client reads.

*Reproduction:*
`git show ac13700:tools/verify_capture.py | grep "THE ASSET NAME COMES"` → `⚠ …`;
the same grep at `c1945b6` → `Ã¢Å¡Â  …`.

### F4 — NOTE — the published verifier SHA256 names a file that exists nowhere in the repo

`docs/verifier-characterisation.md:170` reads *"Verifier SHA256:
`badd0e3e867814d1de702f4259d9a23f94d8ed3738b72f360c91740cc38ed8d0`"*. Measured:

| Artifact | SHA256 |
|---|---|
| `c1945b6:tools/verify_capture.py` (LF blob) | `b9b4eb954779ee6c…` |
| the same file in a checkout (CRLF, 120,953 B) | `615b6c8fbf39140d…` |
| `_reviews/079-13-sweep-tools/verify_capture.py` (mixed endings, 120,921 B) | `badd0e3e867814d1…` |

So the published hash is the frozen sweep copy, which lives outside the repo. A reader cannot
verify the provenance line against anything they have.

**This is a traceability defect only, and I have closed the substantive question behind it.**
The two files are content-identical — 2,252 lines each, zero unified-diff lines — and,
independently, my own sweep run using the branch-tip file (recorded as
`verifier_sha256: 615b6c8f…`) regenerated `docs/verifier-characterisation.md`
**byte-identically except for that one SHA line** (172 lines each, one differing line). The
published tables are from the code under review.

⚠ One claim in Codex's report is nonetheless false as written:
`_reviews/079-13-codex-delta-report.md:372` — *"Frozen tools used for the single sweep exactly
match final working sources byte-for-byte."* They differ by 32 bytes. `G121`'s shape: a hash is
not content identity across two copies.

*Suggested fix:* publish the LF-blob hash (`b9b4eb95…`) or the commit SHA, not a scratch copy's.

### F5 — NOTE — a `NO-TRANSITION` window can contain an above-`tau` pair, and the edge line does not say so

R9's one-sided prominence rule suppresses plateau interiors by design, and both the README
(*"Above-threshold changes can exist without a local peak"*, *"NO-TRANSITION means no qualifying
peak, not no above-tau change"*) and the tool header state the rule. What the per-edge line does
not do is say it **at the point of reading**.

Measured on my F13 (`C:\ClaudeTemp\079-14\fx\F13_one_frame_next_to_two_frame`), runs `[10..10]`
and `[12..13]`, raw `d` computed through the gate's own `_PairSignal`:

```
pair 10 d=1.000000   pair 11 d=1.000000   pair 12 d=1.000000   pair 13 d=0.000000   pair 14 d=1.000000
above tau=0.0040 : [10, 11, 12, 14]
one-sided peaks  : [10, 12, 14]
ABOVE TAU BUT SUPPRESSED: [11]
```

Pair 11 is run A's true end — a 100 %-of-target change — and it is not a candidate. Run A's end
therefore takes pair 12 and prints `TRANSITION 12 … label 11 delta=+1`; run B's onset then takes
11 and prints `delta=-1` with `note: assigned frames out of label order (12 > 11)`. Both runs
read OFFSET-NOTE, which is the honest outcome, and the inversion note is printed — but nothing
on the line tells the reader that a full-amplitude change at pair 11 was excluded from run A's
candidate set.

Per brief §3 this is a disclosed, documented limitation of frame differencing and is **not
blocking**. Recording it because the disclosure is one level away from where it is needed.

*Suggested fix (optional):* when a window contains above-`tau` pairs that failed the prominence
test, append `; above tau but not a peak: 11 d=1.0000` to the edge line.

### F6 — NOTE — systematic missing space before a numeral in reader-facing strings

`exit2`, `code2`, `return3`, `floor0.004`, `limit28`, `default28`, `All94`, `and22`, `only15`,
`peaks60/61`, `scored6 unscored10`. 16+ occurrences across `tools/verify_capture.py` (the header
the client reads), `docs/office-rdp-card.md` (the transcription the owner types from),
`CLAUDE.md` and `docs/gotchas.md`. Same editing-pipeline class as F3; cosmetic.

### F7 — NOTE — merge sequencing, not a branch defect

`git merge-base c1945b6 53bf725` is `ac13700`: the branch was cut from the `master` lineage, not
from `m51`. `git merge-tree --write-tree 53bf725 c1945b6` reports
`CONFLICT (content): Merge conflict in CLAUDE.md` — the branch prepends a 079-13 status block
where `m51` has 078-17 at the top. Chat needs to sequence the target and the resolution; nothing
to fix on the branch.

---

## 6. Scope and regression checks (B5)

| Check | Result |
|---|---|
| `git diff --name-status ac13700 c1945b6` | `CLAUDE.md`, 4 modified + 11 added `docs/*`, `tools/verify_capture.py` (M), `tools/test_verify_capture_consistency.py` (A). **Nothing else.** |
| `Source/` changed | **no** (`git diff --name-only … -- Source` empty) |
| `Shaders/` changed | **no** |
| `tools/measure_label_offset.py` changed | **no** |
| `Content/`, `WebClient/` changed | **no** |
| `docs/gotchas.md` append-only | **441 added, 0 removed** |
| Predictions file append-only | 89/113/102/95 added, **0 removed** on all four amendment commits |
| Predictions precede implementation | 11:16→12:01, 13:24→13:56, 15:55→16:23, 16:49→17:15 (author dates, same day) |
| CaptureBench | `472a409`, tracked-clean; `tools/m079_shift_recovery.py` untracked, SHA256 `159e6944…` matches the published scorer hash |
| Pinned bench ×6 | 64/64 zero-delta, 24 CONSISTENT, `tau=0.0040`, exit 0 — unchanged |
| Bank NO-TRACE | none, in Codex's 10 and in my wider sweep |

---

## 7. What I recommend to chat

1. **F1 and F2 are the two items I would fix before merge** — both doc-only, together under a
   dozen lines of text. Neither touches the tool.
2. **F3** (the `⚠` mojibake) is a one-character revert.
3. **Add F21 to the permanent selftest** (from §4, M15). It is the one coverage gap whose failure
   mode is a *silent pass on a fabricated label*, and the fixture already exists.
4. **F4**: publish the LF-blob hash or the commit SHA rather than a scratch copy's, and withdraw
   the "byte-for-byte" sentence in the 079-13 report.
5. **F5–F7** are recorded, not requested.
6. **Sequencing**: the branch is off `ac13700`, so merging into `m51` needs a `CLAUDE.md`
   conflict resolution (F7). Chat's call, not a branch change.

⛔ Nothing in this review releases Section G, the client hold, the `m51` campaign or the release
hold, and no output of this tool certifies a label. I did not merge, push, commit, build, cook or
capture, and I did not touch `m51`, `master`, the bank, CaptureBench or the plugin tree.

---

## 8. Cost and wall time

| Item | Value |
|---|---|
| Wall time | ~1 h 10 min end to end (first git command ~17:20, review written ~18:30, local time) |
| Model | Claude Opus 5, effort xhigh, single session, no sub-agents |
| Measured step costs | selftest 32.9 s · contracts 1.9 s · pinned bench leg 4.3–4.5 s each · Lyra bank session 12.7–16.1 s each |
| Scorer sweep (my re-run) | ~21 min, 572 keys, 3 sessions |
| Mutation probe | 17 builds × (selftest + contracts) ≈ 11 min |
| Bank sweep | 143 sessions ≈ 31 min (stopped deliberately; see §3) |
| Fixtures built | 35 across three rounds, all expectations predeclared first |
| Gate invocations | 35 fixtures + 10 bank + 24 batch + 8 mutation checks + 4 CLI contracts ≈ 81 |
| Scratch | `C:\ClaudeTemp\079-14\` (fixtures, worktree, mutation copies, all outputs) — removed after |

Costs in currency are not available to me; the numbers above are what I can measure.

---

VERDICT: CHANGES REQUIRED
