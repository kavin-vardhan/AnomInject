# REPORT for 081-19-m55-stage3a-verifier-oracle.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-19-m55-stage3a-verifier-oracle.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-24 02:43:58 | finished: 2026-09-24 03:04:25 | duration: 20 min | exit code: 0
context at last turn: 571075 tokens (57% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-19-m55-stage3a-verifier-oracle.live.txt

---- FINAL MESSAGE (Claude Code) ----
**VERDICT: GREEN**

## 1. Verdict
GREEN. Steps A–D are all done. Every existing test suite still passes with the same counts as before, and the new tests pass. The oracle recomputed every measured row in every banked m55 session, and all of them match the sidecar. No mismatch and no unavailable row.

## 2. Plain summary for the owner
- There is now a tool that re-reads the saved images and checks that the numbers m55 writes about them are correct.
- I ran it over every m55 capture saved so far: 228 sessions, 1,234 measured rows. Every row matched, on every build we have made.
- The tool has a self-check. It catches deliberately planted errors, and it fails if the tool itself is broken.
- It checks the arithmetic only. It says nothing about whether an anomaly was visible.
- No Unreal, no build and no bench were used, so the other project's editors didn't matter.

## 3. What changed
Branch `feat/m55-change-evidence`, pushed (`5ab5e61..c3f5753`). No merge, no tag.

| Commit | Files | Lines |
|---|---|---|
| `ea9946a` `feat(verify)` | `tools/verify_capture.py` | +806 (additions only) |
| | new `tools/test_verify_capture_change_oracle.py` | +163 |
| `79dc9ed` `docs(m55)` | journal with the predictions (committed and pushed before the banked run) | +89 |
| `c3f5753` `docs(m55)` | journal results | +161 |
| | `docs/change-evidence-fields.md`, new "Oracle" section | +54 |
| | `docs/gotchas.md`, G283 | +10 |
| | `CLAUDE.md` status block | +26 |

- **How to run it:** `verify_capture.py --change-oracle <session>` (or `--dir`, plus `--quiet` and `--oracle-json`).
- **What it compares:** for each measured row it decodes the current and previous colour PNGs and the current mask PNG. Counts, sums and the 8-bin histograms must match exactly. Means may differ by at most 0.00005, declared in advance, because the producer rounds to 4 decimals.
- **Reference comparisons** are recomputed against the reference frame the row names.
- **JPEG, resampled, backbuffer or missing deliveries** are reported as UNAVAILABLE with the reason, never guessed.
- **Every output ends with the §6.2 sentence, word for word.**
- **Independence:** it uses Pillow only and does not port the C++. The producer's numbers are read only for the comparison after recomputing.

## 4. Selftest and contracts

| Suite | Before | After |
|---|---|---|
| m47 `--selftest` | 2 OK | 2 OK |
| `--label-pixel-gate --selftest` | 98 OK | 98 OK, output byte-identical |
| `test_verify_capture_consistency` | 25 OK | 25 OK |
| 🆕 `--change-oracle --selftest` | — | **22 OK** |
| 🆕 `test_verify_capture_change_oracle` | — | **9 OK** |

- **G-GRAD(b), the `0,3,6,9,12` walk:** the four adjacent pairs read `[0,0,0,0]`. The reference comparisons read `[0,0,48,48]`, so the last one is the full count.
- **G-TIES:**
  - a difference of 8 is not counted, a difference of 9 is;
  - a 10-bit `A2B10G10R10` step from 0 to 35 becomes bytes 0 to 8, which is not counted.
- **G-DENOM:**
  - a row claimed as measured on a whole-frame mask, or on a tag that isn't in the mask, is flagged;
  - an `empty_region` refusal on a whole-frame mask is confirmed.
- **Must-fail mutations:** each fired on the mutated row only, and every other row still matched.
  - Count off by one: flagged `chg_gt8`.
  - Swapped predecessor: flagged `chg_gt8`, `chg_sum` and `chg_hist`.
  - Wrong mask value: flagged `chg_n`.
  - Pair id shifted by one: flagged `chg_gt8`.
  - Control region including one target pixel: flagged `ctl_n`.
- **Breaking the oracle on purpose makes its selftest fail:**
  - threshold raised by one: 2 cases fail;
  - control region widened to include the target: 14 fail;
  - every mean shifted by 0.0001: 13 fail.
- **Exit-code contract:**
  - The six existing CLI modes were run as real processes, old tool against new, with and without a mismatching sidecar. The codes are identical.
  - This was done on two setups: one where every mode returns 0, and one built to fail, where they return 0, 1, 2, 2, 0, 0. The first alone could not have shown a change; that lesson is gotcha G283.

## 5. Banked sessions
228 unique sessions ran in 116 s. The full table is in `_reviews/081-19-evidence/oracle-table.md` and the per-row detail in `oracle-ledger.json`.

| Group | Binary | Sessions | Rows claimed / compared / matched | Mismatched | Unavailable | References matched |
|---|---|---:|---|---:|---:|---|
| 081-17R Stack 1080p twins ×4 | 8E64FA39 | 4 | 80 / 80 / 80 | 0 | 0 | 80/80 |
| 081-17R Lyra G270 A2 | Lyra 48b9e048 (build 2) | 1 | 20 / 20 / 20 | 0 | 0 | 20/20 |
| R15 twins and count/wall can-fail legs | E8E2E43F | 6 | 132 / 132 / 132 | 0 | 0 | 132/132 |
| R11 V2, R12 (incl. moving camera and MainWorld/CB auto-pool), R15 A | A699E9EE | 27 | 671 / 671 / 671 | 0 | 0 | 669/669 |
| R14 Lyra twins | Lyra 6ef55ade | 3 | 25 / 25 / 25 | 0 | 0 | 25/25 |
| R11 twins | D0867240 | 4 | 75 / 75 / 75 | 0 | 0 | 75/75 |
| 09-21 Stage 2 | 5EADBCBD / 2C21EDE5 (*inferred*) | 12 | 200 / 200 / 200 | 0 | 0 | 200/200 |
| No measured rows (Stage 1 identity rows, EMPTY, NOHOLD, R14 G270 and similar) | various | 169 | 0 | 0 | 0 | — |

**Totals:** 1,234 / 1,234 / 1,234 rows, 1,232 / 1,232 reference comparisons, exit 0 on all 228. As an extra check, numpy reproduces the build-2 solid onset row exactly.

## 6. Mismatches
None.

## 7. Deviations
1. **Exit code of the new mode:** 0 whenever it ran and 3 when it can't run. §6.2 does not say what the new mode should return. A mismatch is printed, not signalled.
2. **Extra checks beyond §6.2, reported separately:**
   - the sums are compared exactly;
   - identity checks: threshold 8, consecutive pair ids, frame file name, both denominators positive;
   - a cross-check of `empty_region` refusals against the mask image. On the bank it could only return "unverifiable" (25 rows), because no mask image is written for an empty mask.
3. **Contract tests went in a new file,** so the old suite's count stays 25 by construction.
4. **Binary labels for 12 Stage 2 sessions and all Stage 1 sessions are inferred** from archive times and marked as inferred.
5. **The stripper was run on my two Python files only** (no comments found). Codex's Stage 1–2 C++ still carries comments and still needs stripping before merge.

## 8. NEEDS-DECISION items
None blocking. For chat to rule when convenient:
- Should a mismatch make `--change-oracle` exit non-zero?
- The oracle trusts the pair ids written in each row. A producer that wrote wrong ids and numbers matching those wrong frames would pass. Only a check on how the pairs were chosen can catch that.

## 9. Wall-clock (IST)
- Reading the brief, spec and code: 02:44:38 → ≈02:47 (≈3 min).
- Oracle, selftest, contracts, sabotage checks, tool commit: → 02:59:22 (≈12 min).
- Predictions pinned and pushed: → 02:59:42.
- Banked run: 02:59:50 → 03:01:47 (116 s).
- Docs, commit, push, worktree removal: → ≈03:06.
- **Total ≈ 22 min.**

**Boundary:** the scratch worktree is removed. The live checkout is on `m51` `53bf725`, with the same 12 untracked owner files as at the start. `master` = `origin/master` = `031a103`. The feature branch is at `c3f5753`, same as origin. No tags created, no processes left running, and only new files under `_reviews/081-19-evidence/`.
api-equivalent cost (not billed on Max): $15.44 | turns: 107 | running total across all briefs: $2,055.93
