# REPORT for 094-48d-europa-push-final.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-48d-europa-push-final.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-06 17:38:14 | finished: 2026-11-06 17:46:38 | duration: 8 min | exit code: 0
context at last turn: 400467 tokens (40% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-48d-europa-push-final.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**For the owner (3 lines):**
1. Pull `feat/europa-port` at **`2828f6ab85406cacc6b5e4c06840a3a1b4cebbc0`**; it is now pushed. The dashboard stays at `b8ffaca` (unchanged).
2. The card is `D:\IntrusiveAnomalies\_reviews\094-48b-office-card-europa-dashboard.md` and names only that commit. Use `-skipcook` if the 094-47 cook output is still there; otherwise use the full BuildCookRun it gives.
3. Dashboard captures are normal 5.1 sessions in the Library: `annotation.json`, `labels.jsonl`, `run_summary.json`, `mask_map.json`, `Actual_Frames\`, `target_mask\`, and nothing else.

**What I did:** 094-48c's Part C was already complete (Codex `2828f6a`, exit 0, with E1, E2 and E3 passing), so I didn't resume Codex. I wrote GR6b verbatim to `_reviews\094-48d-chat-ruling-GR6b.md`, applied rule 3 to every free-play run, re-ran the pre-push scan, pushed, and rewrote the card.

**The push**
- `ls-remote` was `bd767ca8ac7cd1ca18abe34d9cc66764fee100a6` before and is `2828f6ab85406cacc6b5e4c06840a3a1b4cebbc0` after. It was a fast-forward of 3 commits (`6b171b5`, `1667d05`, `2828f6a`), with no force and no tags at the tip.
- The re-run scan of `bd767ca..2828f6a` came out byte-identical to 094-48c's on all 11 outputs plus the tip check, and it is clean: every flagged tip line is already present in `bd767ca`. There are no binaries, no uasset files and no Content changes.

**E1 file set** (`e1_check-E1.txt`): the root holds `annotation.json`, `labels.jsonl`, `run_summary.json` and `mask_map.json`. There are 300 frame PNGs and 140 mask PNGs. Extras 0, `.u32`/`.zlib` 0, blank masks 0. `target_mask_frames_measured` is 140, matching the mask PNG count and the `present` rows. The Library gate passes. The only other files are the dashboard's own `.dashboard\state.json` and `thumb.jpg`.

**GR6b rule 3** (`code-receipts-094-48d\gr6b_rule3_48d.json`)

| Run | Texture frames | Changed px outside mask | Clean neighbour frames / changed px | Labelled frames with no change (min changed px) | Mask px outside boxes |
|---|---:|---:|---|---|---|
| F1 | 182 | 0 | 28 / 0 | 0 (7,826) | not measurable (see below) |
| F3 | 88 | 0 | 14 / 0 | 0 (7,826) | not measurable (see below) |
| F5 | 260 | 0 | 40 / 0 | 0 (7,826) | not measurable (see below) |
| S4 | 182 | 0 | 28 / 0 | 0 (7,826) | 0 of 13,231,036 |
| E2 | 88 | 0 | 14 / 0 | 0 (7,826) | 0 of 5,845,424 |

- **All runs above, plus F2 ×2 and S4-F2 ×2:** dropped 0, polluted 0, tagged 0, annotation PASS, mask check true, clean-frame mask pixels 0. The four F2 runs have no texture events; their masks are all inside their boxes (S4-F2: 164,680 and 6,023,732 px).
- **F1/F3/F5 boxes:** these runs use the older per-epoch folder layout, whose rows carry no `bbox_px`, so there is no box to test against. S4 is the flat-layout re-run of F1, with identical texture numbers.
- **E1** has no reader output, because the dashboard file set carries no verification data. From its own files: dropped 0, polluted 0, host stencil writes 0, mask pixels outside boxes 0 of 5,845,424. E2 is its verification-on twin: all 300 rows have the same mask state and file, and all 140 mask silhouettes are equal; only the stencil tag values differ.

**Coverage (information only, IoU):**

| Anomaly | Runs | Frames | Min | Mean | Max |
|---|---|---:|---:|---:|---:|
| corrupted_texture | F1, S4 | 65 | 0.9206 | 0.9740 | 0.9878 |
| corrupted_texture | F3, E2 | 23 | 0.9878 | 0.9878 | 0.9878 |
| corrupted_texture | F5 | 91 | 0.9206 | 0.9772 | 0.9878 |
| missing_texture | F1, S4 | 117 | 0.2898 | 0.477 | 0.9740 |
| missing_texture | F3, E2 | 65 | 0.2898 | 0.5848 | 0.9747 |
| missing_texture | F5 | 169 | 0.2898 | 0.4519 | 0.9740 |

**Safety:** none. `m51` (`53bf725`), `master`, `release/2026-10-05`, tags, the dashboard `master` and the protected projects were not touched. Nothing was deleted or killed. The held card is kept in receipts as `card-094-48b-before-48d.md`.

**Decisions chat must make:** none. One note: the brief says anomalies start at about frame 3, but every free-play run measured the first labelled frame at 26 (under a second). The card states 26.

Receipts are in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-48d\`.
api-equivalent cost (not billed on Max): $6.45 | turns: 41 | running total across all briefs: $5,895.05
