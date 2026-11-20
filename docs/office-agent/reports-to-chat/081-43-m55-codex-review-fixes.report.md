# REPORT for 081-43-m55-codex-review-fixes.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-43-m55-codex-review-fixes.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-26 11:17:52 | finished: 2026-09-26 13:13:13 | duration: 115 min | exit code: 0
context at last turn: 912344 tokens (91% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-43-m55-codex-review-fixes.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Summary for the owner**
- All nine of Codex's findings are fixed exactly as chat ruled: six in the checking tools and client docs, and three smaller notes. No game code changed. Nothing was built or captured.
- The tools now catch Codex's three demonstrated gaps: a hidden measurement, a single altered label count, and corrupt input files.
- I re-ran the old and new tools over the whole saved capture bank. No real result changed: 476 of 476 session folders, all 9 legacy comparisons, and all 70 requalification evaluations came out the same.
- The client-facing docs now describe cost and memory as a dated result on one test scene and one machine, not a guarantee. They also say correctly what the "control" region includes, and what a refused row still carries.
- The work is committed and pushed on the feature branch (`fd175dc`). It is not merged and not tagged; the next step is chat's review of these replays.

**Fix table** (repo files at `fd175dc`; the harness library `_reviews/081-23-lib.py` lives outside the repo)

| Finding | Change | Proof: expected / actual | Evidence |
|---|---|---|---|
| F1 | Oracle: a refusal whose mask shows both target and surroundings now counts as a disagreement (`empty_region_disagrees`) and makes the exit 1 (`verify_capture.py:2416`, `:2481–2690`). A missing or all-zero mask PNG stays "unverifiable". In the library, `oracle_disagreement_error` (`:106`) is read by G270 (new FAIL branch, `:974`), the `empty` stack case, the Lyra twins, NoHold and the legacy evidence-on legs. | Codex's G270 mutation, old / new: PASS / **FAIL**; oracle exit 0 / **1** (target 184,933 / control 1,888,667). Same mutation with no mask PNG: 0 / 0, unverifiable. With the oracle stubbed to exit 0, each of the 4 gates accepts under old and rejects under new. | `negatives.json` |
| F2 | Nine per-leg invariants run on the raw data before any noise excusal (`leg_invariants` `:645`, called at `:748`). A self-excused singleton on a candidate leg outside the frozen run-unique and timing-sampled sets, not resolved by an invariant, is NEEDS-DECISION (`:728`). Verdict order: invariant FAIL > earlier-ruled NEEDS-DECISION > FAIL > candidate-singleton NEEDS-DECISION > PASS (`:885`). | Codex's MAINON_S2 `target_pixels` 20,949 → 20,950: PASS / **FAIL (INV-2)**. Each of INV-1…9 fires on its own mutation. An uncovered candidate singleton: PASS / **NEEDS-DECISION**. The same singleton on a pre-m55 leg or a timing-sampled path: PASS / PASS. A count difference whose mask noise is shared with the twin leg: PASS / PASS (resolved by the invariant); the same difference on one leg only: PASS / **NEEDS-DECISION**. | `negatives.json` |
| F3 | The sidecar is read at an input boundary (`:2434`). Malformed lines, non-object lines, and measured rows with wrong-type ids, `mask_value`, `tau_px`, `receipt` or `rect` exit 3 with line numbers (`:2276`). Precedence is 1 > 3 > 0 (`:2681`), and the tool never exits through a traceback (`main()`, `:3180`). | Old / new: `{bad json` only 0 / **3**; valid row + malformed 0 / **3**; mismatch + malformed 1 / **1**; `receipt=[1]` traceback / **3**; `receipt=[]` 0 / **3**. Selftest 35/35, unit tests 19 + 25. | `negatives.json`, tests |
| Scope note | A measured row with a refusal reason, `chg_eligible`/`pair_valid` not true, a wrong expected predecessor, or no hist/sum is a mismatch (`:2293`). The oracle's scope is stated in its help and both docs. | Codex's contradictory row: 0 / **1**; hist/sum omitted: 0 / **1**; reference moved to an identical image: 0 / 0 (a documented limitation). | `negatives.json` |
| F4 | The control excludes the target's own pixels but can include its light, shadow and reflections (readme `:1035`, fields `:60`). | Grep 0 | `doc-grep.json` |
| F5 | Refused rows: the target and reference statistics are placeholders (-1/null); the control values, reference index, running maxima and bookkeeping can still be real (readme §9.1, fields `:18`, `:174`). | Checked against real refused rows in the bank | — |
| F6 | Cost is written as a dated result: −0.0718 / +0.1100 ms upper bound on mean game-thread CPU time, measured at about 25.07 captures/s, 120/120 pairs, 0 drops, no detected writer loss. Stress results, "no full-yield guarantee" and the 134.8 MB second-game peak are kept (readme §9.4–9.5, fields `:274`). | Numbers match the 081-40 report and the bank | — |
| N1 | Delay test split: adjacent windows 0–2 at most 63 changed pixels; window 3 all 66,837; the reference had already reached 156–819 by windows 1–2. | Measured in both delay banks | — |
| N2 | `change_teardown_flush` is always present (`fields:242`). | 106 build-3 run summaries | — |

**Oracle bank replay:** 476 session folders (326 unique sessions, alias and `_tryN` copies both). Exit code 0 → 0 on all 476, no changes. Both oracles compared and matched the same 4,981 measured rows. New counts: 0 contradictory, 0 uninterpretable, `empty_region` 0 agree / 0 disagree / 83 unverifiable. (`replay-oracle-bank.json`)

**Legacy and G270 replay** (rebuilt from the raw bank):
- **Legacy, 9 groups:** 081-12 MAIN and CB, 081-22 D MAIN-N and MAIN-S, c1 CB-N, CB-S, MAIN-N and MAIN-S all read PASS / PASS. 081-22 C reads FAIL / FAIL; it is not a legacy-compare group, and that FAIL was already there in 081-35. 0 invariant violations and 0 candidate singletons.
- **Evaluations, 70:** every G270 evaluation (c3 G270, L1, L4, LYRA_G3_N_A2), 62 stack legs, 2 Lyra twins and 2 NoHold. Verdict unchanged and oracle exit unchanged on all 70.

**Invariants added under F2** (each proven able to fire on a real leg):
- **INV-1:** a mask file named in a label exists, is 8-bit grayscale, and is not all-zero.
- **INV-2:** `target_pixels` equals the decoded count of that tag in that frame's mask.
- **INV-3:** rows without a mask image are unmeasured → −1, or empty → 0 or −1.
- **INV-4:** `bbox_drawn_px` equals the tag's bounding box in the mask.
- **INV-5:** `observable` is null exactly when the count is −1, and true only at or above the minimum pixel count.
- **INV-6:** the drawn count is never more than the pixel count.
- **INV-7:** summary counters are at least the rows they count.
- **INV-8:** each measured evidence row's `chg_n` equals the label's `target_pixels`.
- **INV-9:** each counted entry has exactly one MASK-TIE log line, equal to the label, and the comparator's cached copy equals the log.

I checked them over the whole bank before gating on them (382 sessions, 59,923 rows; 34,210 MASK-TIE entries): 0 violations, except INV-1 on the G5 fault gate. That gate deliberately fails a mask write, so the label names a file that doesn't exist; no legacy leg has one.

**Deviations**
- **Three AMENDMENT 6 proof cases changed.** They asserted that a single synthetic candidate-only difference PASSes with a reading, which F2.2 revokes; they now read NEEDS-DECISION. The recurrence tripwire gives identical output in every case. My first version turned ten other AMENDMENT 6 negatives from FAIL into NEEDS-DECISION; I fixed that by putting the new decision below proven failures.
- **The other AMENDMENT 1–6 proofs match the pre-081-43 library case by case.**
  - resume-proof and pp-proof read the live c1 ledger, which has moved on since they were written. Run on the ledger as it stood at the time, they give 7/7 and 42/42, the same as their originals.
  - am5-proof reads 31/36 under both libraries; those five cases were already superseded in 081-33.
- **Added beyond the ruled minimum:**
  - INV-9 (so a count difference isn't raised twice).
  - An expected-predecessor check among the contradictory-field checks.
  - More input classes: non-integer `mask_value`/`tau_px`, an absent `receipt`, and a `tau_px` outside 0–255 is now a mismatch.
  - N2 extended: `pending_colour/mask_cancelled` are also always present.
  - N1: the old "9–63" range was wrong; it is now "at most 63".
- **Where things live now:**
  - The library is `1c4ae649`; the pre-fix version `f5876b05` is kept in the evidence folder.
  - The new oracle is frozen at `_reviews/081-43-frozen` (`b399e884`). Both bench boundaries still pin the old library, so any future bench window needs a re-issue first.
  - Legacy readings cached before this change now fail on the missing disagreement count (on purpose); a re-evaluation must rebuild them from the bank.
- **Mutated copies:** the bank is a junction to `E:`, so the mutated copies were built in an `E:` scratch folder and mirrored into `081-43-evidence/neg/mutated-copies/`; the scratch folder is deleted.

**Commits pushed:** `fd175dc` on `feat/m55-change-evidence`, fast-forward from `eafa654`; local matches origin. The main checkout is still on `m51` `53bf725`; `master` (`031a103`), tags, containers, CaptureBench and `ToCodex\` are untouched.

**NEEDS-DECISION:** none. Chat may want to confirm the three AMENDMENT 6 proof expectations that F2.2 supersedes.
api-equivalent cost (not billed on Max): $41.87 | turns: 299 | running total across all briefs: $2,385.47
