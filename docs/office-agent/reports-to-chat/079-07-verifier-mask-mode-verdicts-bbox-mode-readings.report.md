# REPORT for 079-07-verifier-mask-mode-verdicts-bbox-mode-readings.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 079-07-verifier-mask-mode-verdicts-bbox-mode-readings.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-15 15:49:51 | finished: 2026-09-15 16:47:03 | duration: 57 min | exit code: 0
context at last turn: 639868 tokens (64% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\079-07-verifier-mask-mode-verdicts-bbox-mode-readings.live.txt

---- FINAL MESSAGE (Claude Code) ----
All work complete and verified. Final report:

---

# 079-07 — Code's report

**Branch `fix/verifier-field-validity`, four commits on top of `bff1b00`, pushed to `1cbc07e`. ⛔ NOT MERGED.** Checkout restored to `m51` `53bf725`, tracked-clean with the same ten untracked handoffs. `master` == `origin/master` == `ac13700`. CaptureBench tracked-clean at `472a409`. `Source/`, `Shaders/` and `measure_label_offset.py` **byte-identical to `master`**, verified by `git diff --quiet`. No build, capture, cook or tag.

| commit | what |
|---|---|
| `0e7d96c` | **AMENDMENT 5**, committed **before a line of the correction existed** |
| `d7f4e40` | the fix — `tools/verify_capture.py`, +782/−160 |
| `0a87825` | docs: client-readme Step 6, Section G, `G263`, two appended prediction corrections |
| `1cbc07e` | the session journal |

## Per-P

**P1 — `SELFTEST: OK`, 46 cases.** All seventeen 079-06 cases keep their tokens (they gain masks — a bbox-only fixture now yields readings, and a suite of readings cannot prove the gate can still FAIL). Five 079-05 Codex fixtures become `READING` with **two new masked twins** carrying their negative-control role. Eighteen new 079-07 cases. Plus the real-session half on a banked leg: `real+1` → `ONSET-SHIFT(-1)` **4 of 4**, `real-1` → `ONSET-SHIFT(+1)` **4 of 4**. Every expected-string change is tabled with its reason in A5.2.

**P2 — the pinned bench is unmoved.** All six legs `PASS 4 / SHIFT 0 / NOT-VISIBLE 0 / PARTIAL 0 / NOT-MEASURABLE 0 / READINGS 0`, `VERDICT PASS (4 of 4 events fully checked)`, `tau = 0.0040` on **all 64 edges**, every one mask-attributed at `m_edge = 0.000000`, exit 0.

**P3 — zero `SHIFT` and zero `NOT-VISIBLE` on all four.** `A2L_LEGA` `PASS-PARTIAL (2 fully, 0 partly, 5 unread of 7)` · `LYRA_SMOKE_01` and `A1L_LEGA` `UNREAD (0 of 6)` · **`M50L_LG9` `UNREAD-BBOX-ONLY (6 events; readings printed …)` with six `READING` lines, exit 0.** No mask-mode edge was fully read and dominant and disagreeing, so there is no finding to print.

**P4 — scorer v2.1, mask-mode cohort, 572 planned = 572 scored.** Guard OFF: recovered 368 / **wrong 17** / refused 175 / no-signal 12 / unsupported 0. Guard ON: 52 / **0** / 520 / 0 / 0. **The cap re-derives to `0.00014000` — unchanged — still pinned by the exact cell Codex named** (`A1L_LEGA` ev 0 run[4..5] onset, expected −1, read +1, `m_edge = 0.00014459`). Retained positive support **4 original edges of one session**, which the scorer now prints itself. The 079-06 780-cell tables re-published split: **385 refusals = 255 real + 130 no-signal**, guard-on 728 with 0 — and **118 of the 130 are `M50L_LG9`**, which F1's boundary removes by construction.

**P5 — batch.** Codex's collision retains **two** reports with opposite verdicts and per-line paths; missing-label under `--report-only` → 3; healthy → 0; failing without `--report-only` → 2.

## Two things to look at

**1. A deviation I chose, declared in AMENDMENT 5 before implementing — this is the line to argue with.** The brief gave F2's bound as `max(previous labelled, previous DETECTED)` and, in its own parenthetical, as *"detected clear if found, else its labelled end"*. Those differ, and the `max` form is unsound in the case the gate exists for: on a **one-frame run whose labels are LATE by one** the true clear sits at the labelled onset frame itself, so a labelled floor of `rs+1` excludes the correct answer even when the onset was detected correctly. I implemented **detected-else-labelled**. Both of Codex's `−2` readings are gone and the control fixture's genuine `+1` survives.

**2. 🔻 One of my predeclared expectations was defective and the build was not.** I declared `c07_previous_end_control_masked` as *"never a SHIFT"*; it reads `END-SHIFT(+1)`, which is **the fixture's own known answer** and exactly Codex's prediction (*"first end +1, second onset 0"*). Corrected by appending (A5.4), never by editing the table. **The repair is a strengthening**: the event token returns the *first* run's shift, so it would have printed identically whether the second run was right or wrong — `G262`'s shape inside the correction for it — so the five F1/F2 cases are now judged on **per-edge offsets**. A second latent inconsistency (the token counted attributed *edges* while mask mode is a property of the *run*) was found and fixed before any result was read, with its fixture predeclared at A5.5.

## Filed, not acted on

The comment stripper would change `tools/verify_capture.py` (−472 bytes of `#` comments; docstrings survive), so it has not been run on this file across the 079 series. Running it now would delete explanatory comments 079-03..06 added and that Codex's reviews cite by line. My own new code uses docstrings only. Chat's call. Also: `D:\IntrusiveAnomalies\m079_shift_egnkcjfk` is a 079-06 scorer tempdir left behind before this session; I did not touch it (it may hold junctions into the bank).

## Closure

`_reviews/079-07-code-response-to-codex.md` answers F1–F8 finding by finding with the reading that proves each. Journal `docs/sessions/2026-09-15-079-07-mask-mode-verdict-boundary.md`. Gotcha **`G263`** — *a difference gate has no verdict without target attribution: masks or nothing*. Evidence banked at `_bench_sessions_bank\M079_VERIFIER_EVIDENCE\079-07\` (17 files). ⛔ No client, Section G, campaign or `m51` hold is released.

VERDICT: GREEN
api-equivalent cost (not billed on Max): $54.26 | turns: 173 | running total across all briefs: $1,611.39
