# REPORT for 081-41-m55-stage3-docs.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-41-m55-stage3-docs.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-26 06:43:52 | finished: 2026-09-26 07:01:06 | duration: 17 min | exit code: 0
context at last turn: 560425 tokens (56% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-41-m55-stage3-docs.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Summary for the owner**
- Stage 3 of m55 is finished. The field reference, the new client readme section, the catalogue note, the consolidated Stage 3 report and the merge-prep list are all written and pushed.
- Every field the capture code writes is described in the field reference. A script compared the two in both directions and found 0 gaps.
- The client readme contains no internal identifiers (0 hits).
- Nothing was built, run or launched; only docs changed.
- Next is chat's step: the Codex review, then the comment stripper, then the merge.

**Documents written or changed** (commit `eafa654`, 6 files, +615 / −9)
- `docs/change-evidence-fields.md` (+297 / −1): a new front section written from the source. It covers:
  - what m55 is: measurements only, no verdict in v1, `observable` unchanged;
  - the three comparisons (adjacent pair, whole-frame control, pre-onset `ref_*`) and the histogram;
  - the camera delta;
  - every pair, receipt, event and phase field, and the 14 refusal reasons with their meanings;
  - every `run_summary` `change_*` key, the memory envelope, cost, and how to run the oracle.

  The old text is kept word for word below an "Internal development notes" heading; the new section wins where they differ.
- `docs/client-readme.md` (+184 / −6): a new, self-contained section 9. The delivery bundle ships only the readme, not the field reference, so section 9 cannot rely on it. Section 9 also covers:
  - late or gradual change, using the delay=3 numbers read from the banked sidecars;
  - memory, cost ("use paced capture"), `positive_frames`, and the oracle (exit codes 0/1/3);
  - the two test entries in the anomaly list, and the known limits.

  Two small edits outside section 9: a file-list bullet in §8.0, and a pointer at each of the two places that say the shader-readiness marker cannot appear in a packaged build. The G298 measurement contradicts both statements.
- `docs/architecture.md` (+12): catalogue note on the two test anomalies, `null_effect` and `solid_swap`. They exist only outside Shipping builds, refuse to run outside the test fixture, and are in no capture pool.
- `docs/gotchas.md` (+25): G305, described under Deviations.
- `docs/sessions/2026-09-26-081-41-code-m55-stage3-docs.md` (83 lines): the journal. `CLAUDE.md` (+20): new top status entry.
- New files under `_reviews\081-41-evidence\`:
  - `stage3-report.md`: oracle, cost (paced headline, pacing-off labelled as stress), the 92-leg requalification with AMENDMENTS 1–6, the priors table, the c0 reading, the product findings, and the UNEXERCISED block regenerated from the predictions file (identical to 081-40's);
  - `merge-prep-inventory.md`, plus the scripts and their JSON outputs.

**Field reference vs source**
- 178 keys can be emitted. 0 are undocumented, 0 documented keys are never emitted, and 0 census fields are missing.
- I proved the check can fail in both directions: an added fake key was flagged, and a removed key was flagged. The first removal attempt did not fire because the key appears twice in the doc; once both copies were removed, it did.

**Client-facing identifier grep**
- 179 added readme lines were searched for brief numbers, G-numbers, chat/Codex/Code, `_reviews`, "AMENDMENT", "ruling" and similar: 0 hits, in both a case-sensitive and a case-insensitive pass. A test line containing those words was caught 4 times out of 4.
- The field reference's new front section is also clean.

**Merge-prep inventory**
- **Code-authored changes since build 2:**

| Commit | What | Lines (+/−) |
|---|---|---|
| `9c75abe` | build 2, `AnomalyChangeStage.cpp/.h` | 146 / 5 |
| `ea9946a` | the oracle, `verify_capture.py` and its new test | 969 / 0 |
| `85c99b3` | build 3, six Source files | 138 / 43 |
| `c27024d` | oracle exit codes | 30 / 14 |

  Nothing else in the range touches anything but docs. The harness files outside the repo (`081-23-lib.py` and the rest) are listed with line counts.
- **Comment stripper:** I ran the stripper's own code in memory without writing anything. It would change 65 lines in 14 files. Every one of those lines traces to a Codex-authored m55 commit; Code's commits contributed 0 and older code 0. `AnomalyCapture.Build.cs` would not change.
- **`AnomalyBench` packaging:** the descriptor allows it in Editor and Game targets and excludes Shipping, so a client Development package would carry it. The removal box is already in `PRE-DELIVERY-CHECKLIST.md` §1 (line 58). Removing `AnomalyBench` does not remove the two test anomalies, which register in `AnomalyInjector`.

**Deviations**
- **No field named `cam_delta` exists.** The code writes `cam_dpos_cm`, `cam_drot_deg` and `cam_dfov_deg` (the last two in tenths of a degree). The docs describe those three and say there is no `cam_delta` key.
- **The closure-bound formula is shipped as a planning estimate labelled "not a bound".** The ruling asks for "the closure-bound formula", but 081-16 withdrew it as a bound. The readme gives the formula and its 49 / 110 / 195 / 440 MB figures, alongside 081-16's statement that full yield is never guaranteed.
- The readme's existing Step 6 runs `python tools\verify_capture.py`, but the bundle ships the tool as `host-tools\verify_capture.py`. This predates m55 and I left it unchanged.

**Commits pushed**
- `eafa654` on `feat/m55-change-evidence` (`ab634f5..eafa654`, local == origin).
- The main checkout is still `m51` `53bf725`; `master` is still `031a103`. Tags, containers, CaptureBench and `ToCodex\` are untouched. The scratch worktree has been removed.

**NEEDS-DECISION**
- none
api-equivalent cost (not billed on Max): $15.69 | turns: 124 | running total across all briefs: $2,343.60
