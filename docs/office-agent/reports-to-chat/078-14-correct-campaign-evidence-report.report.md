# REPORT for 078-14-correct-campaign-evidence-report.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-14-correct-campaign-evidence-report.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 21:02:22 | finished: 2026-09-06 21:14:58 | duration: 13 min | exit code: 0
context at last turn: 801421 tokens (80% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-14-correct-campaign-evidence-report.live.txt

---- FINAL MESSAGE (Claude Code) ----
All five findings are implemented and committed. Documentation only — nothing ran.

# REPORT — 078-14, evidence-explanation corrections

## Docs commit

**`46139fd3ef20fab90d0768492a987a9952cceb62`** — `docs(m51): 078-14 five evidentiary corrections to the 078-13 campaign record` — **3 files, 296 insertions / 68 deletions**, path-scoped adds, `commit --file` with an absolute message file outside the repo and mailbox. `m51` HEAD `c73fc2d` → `46139fd`.

| path | change | bytes | sha256 |
|---|---|---|---|
| `CLAUDE.md` | +58 / −17 | 566,898 | `A401B48CCD735C4EA92619DF3A69AAE8D5E933F1D89F2C758F4FC43AC2EF3A5D` |
| `docs/OPERATING-CONTRACT.md` | **+91 / −0** | 54,001 | `A544CEBE449C4820BF17F0A7519059FA13D2D66F1FA35F8DFED37D5A91FA62F9` |
| `docs/sessions/2026-09-06-078-13-m51-frozen-pairing-campaign.md` | +147 / −51 | 27,960 | `A22276F5AE6D631947E6C70742942C13B959E12D2E6C57C64857DE0E732A5BA1` |

**Diff locality verified:** every `CLAUDE.md` hunk falls between lines 13 and 78 — entirely inside the 078-13 status block; no historical section touched. Contract hunks are the pointer insert at the 078-13 header and a pure append. `git diff --check` clean. **Prediction bytes unmodified: `8D7951F542D61658738D8ED4716D98FCCF563C10ABDCCF2EFBDFF05197D2E9B5`**, and it is not in the diff.

## Correction disposition — all five implemented, each re-verified before rewording

| # | finding | verified against | disposition |
|---|---|---|---|
| 1 | `NO-DATA` ≠ "legitimate mask outcome" | prediction `A1.3` scope | **Done.** Replaced with the scoped fact: undecidable frames are permitted when `decidable >= 20` and the artifact conditions hold (both did); ⛔ it does **not** certify mask production, and the **34** existing `mask_file` references cannot certify the seven readings that produced none. All counts/indices preserved. |
| 2 | one unavailable effective-mask frame ≠ "the very class `F1` addresses" | `Capture(m43): TARGET MASK ON FOR THIS RUN` in the banked log | **Done.** Equivalence removed — `F1` concerns requested-but-**ineffective** masks; this leg ran masks **effective and ON**. Broader outcome, **cause not established**. The observation is preserved verbatim. |
| 3 | `B1` description + failure attribution | `check_pose.py` + `a54_oracle.a56_check` read in full; view/bbox histograms **recomputed from the banked `labels.jsonl`** | **Done.** `a56_check` needs `coverage >= 0.90` **AND** `distinct <= 3` **AND** `pose_match` — so it reads the `view.rot` settle history **and** the anomaly-`bbox_px` history, not camera data alone. **try1 = 24×(0,0,0) + 16×(0,359.83,0); try3 = 26×(0,0,0) + 14×(0,0.17,0)** — pose is *not* constant within those runs. **`distinct=2` PASSES the `<=3` test**, so tries 1/3 fail on **64.00 % modal coverage alone**; try2 fails **`pose_match` only** (coverage 100 %); try4 fails **coverage 62.50 % AND `pose_match`**. "Genuine A47 rotation bifurcation" and "a different cause" **withdrawn**; ⛔ **no cause established in either direction** — the gate's field-dependence neither isolates nor excludes an `F1` contribution. Attempts stay predeclared-INVALID, not retried. |
| 4 | `P8`/`TAU` was the wrong reason | `m44_pairing_probe.py:215-230` | **Done.** The checker compares **magenta/tag-255 centroids** (40 px current/previous, 50 px same-position) and reads no `TAU`; `TAU`/`P8` govern the A54 luminance oracle. Governing reasons restated: **`A63`/`B1` precondition + attempt-exhausted stop**, and the released recipe targeting the **accepted bank, which does not exist**. ⚠ The tool is **not** inherently unable to read a `_tryN` bank; any future offline diagnostic must preserve the INVALID status. |
| 5 | seven → **eight** receipts | receipts dir enumerated (8 files) | **Done.** Cause named (BEFORE/AFTER collapsed onto one row, then rows counted). Journal table expanded to one row per file; all inventory statements corrected in journal, `CLAUDE.md`, contract and the evidence `INDEX.md`. |

**Banned phrasing removed:** "the failure is pose alone" → *no configuration or artifact-completeness defect explains the stop*, with the failing predicate named per attempt · "NOTHING is established about the `F1` candidate" → **"there is NO VALID CANDIDATE PAIRING RESULT"** · "the pose gate reads the camera only" → withdrawn · "genuine A47 bifurcation" → withdrawn. **Artifact-completeness and effective-configuration observations are kept and explicitly scoped as real readings that identify no cause.**

## Where the corrections live

- **Journal** — corrected **in place**, each marked 🔻 with the original wording quoted so nothing is silently rewritten, plus a dated **078-14 correction marker** at the top linking to the contract.
- **`CLAUDE.md`** — dated **`CORRECTION 078-14`** block inserted above the 078-13 status block (with the authoritative link), plus the nine specific corrected sentences inside it. **CRLF 6037 → 6078, zero bare LF, no BOM.**
- **`OPERATING-CONTRACT.md` §9** — new authoritative **`### 078-14`** entry (append-only), and a pointer block inside the 078-13 entry: *"THIS ENTRY IS CORRECTED ON FIVE POINTS BY `078-14` BELOW … where they disagree, `078-14` governs."*
- **Evidence `INDEX.md`** — dated correction **appended**; original **3,106 bytes preserved byte-for-byte as prefix** (byte-compared, not assumed): `C333CD74…4BC1` → `C45F4D64…B6A3`.

## Immutability and final state

**14 banked evidence files re-hashed against the 078-13 report — 0 mismatches** (all 8 receipts, both command records, both runner transcripts, the checker transcript, the facts readout). Attempts tree unchanged (5 dirs / 418 files); 6 `M51R_*` banks; **no banked report, log, JSON, image, command/output record or receipt modified.**

Staged exe **`D50DDE78…4796`** (candidate) · container quintet unchanged (`67EA1FE0` / `2CEFB8F4` / `E03C6610` / `A16A18A8` / `C70ECDAA`) · both archives unchanged · `run_leg.ps1` `9E8D669F…B945` and `m44_pairing_probe.py` `918EB4B5…7AF1F` unchanged · **source freeze `6713f51..HEAD -- Source Shaders` EMPTY** · plugin repo **0 tracked modifications**, 10 handoffs untracked · CaptureBench `472a409`, 0 tracked modifications, 7 untracked tools preserved · Lyra detached `4a4bfcc`, clean · `master` == `origin/master` == `ac13700` · nothing pushed (`origin/m51` absent) · 30 tags, unchanged · **0 `StackOBot*` processes, 0 background jobs.**

Session-local scratch added under the evidence `tools/` directory (outside every repo and the mailbox): `contract_entry_07814.txt`, `index_correction_07814.txt`, `commit_msg_07814.txt`, `fix_claude_07814.py`.

## Scope held

No capture, staging, binary swap, build, cook, new checker execution, hypothesis test, retry, owner-app action, merge, push or tag. The B1-input recomputation was a **read-only recalculation from banked `labels.jsonl`**, not a harness or checker run.

**VERDICT: GREEN — for the DOCUMENTATION CORRECTIONS ONLY.** All five findings are implemented, verified against source and banked readings, and committed. ⛔ **The campaign itself remains NEEDS-DECISION and INCOMPLETE** — one released main cell passed, one halted without a verdict, three main cells and all four conditional controls are UNRUN, and full release stays HELD. Stopping here for Codex then Chat review.
api-equivalent cost (not billed on Max): $22.06 | turns: 48 | running total across all briefs: $1,286.09
