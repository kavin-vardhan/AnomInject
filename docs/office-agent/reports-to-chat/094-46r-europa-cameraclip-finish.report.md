# REPORT for 094-46r-europa-cameraclip-finish.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-46r-europa-cameraclip-finish.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-05 09:32:24 | finished: 2026-11-05 11:36:19 | duration: 124 min | exit code: 0
context at last turn: 578061 tokens (58% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-46r-europa-cameraclip-finish.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:** Camera clipping now passes the new "near-plane" test on D3D11 but misses it on D3D12, by 15 pixels on 6 frames, all within the same 2-brightness-level flicker. Nothing was pushed, so keep using the same office card, `_reviews\094-45r-office-card-europa.md`.

**GO2: the floors and D4 on each RHI.** I recomputed every number below from the raw frames, and each matches what Codex sealed. Each floor was sealed before the comparison it judges.

| | D3D11 1280×720 | D3D12 1280×720 |
|---|---|---|
| Session floor (max levels / pixels at that level) | 1 / 560,155 (from 094-46) | 1 / 559,838 |
| Near-plane floor | 2 / 255 | 2 / 240 |
| Unlabelled frames within the larger floor | **2,496 of 2,496: passes** (worst frame 254 px) | **2,490 of 2,496: fails** (6 frames at 255 px) |
| Any pixel above 2 levels | 0 | 0 |
| Labelled frames | 144, every pixel changed (921,600) | the same |
| Spans | 3, exactly as scheduled | the same 3 |
| Baseline controls label | 0 | 0 |
| Near plane restored to 10 at Stop | yes | yes |

- **Both far-only floor sessions are clean:** they label 0 camera frames and never visit the near-wall position.
- **D3D11 reused 094-46's sessions,** which ran on the older build. I checked that the rebuild doesn't matter: the new and old near-100 sessions differ by at most 1 level.
- **The six failing D3D12 frames are one render state repeating** (frames 1156, 1163, 1996, 2003, 2087, 2094). On average the positive has fewer 2-level pixels than the floor pair: median 157 against 172, p99 212 against 227.
- **Extra check, run after the stop (not a gate):** pairing the same far-only session with the other control (B) gives 254 px on both RHIs, so one pair's maximum shifts by about 15 px depending on which control is used.

**The D3D12 legs.**
- The five new sessions (D3D11 near-floor; D3D12 control A, control B, near-floor and positive) all pass H4, strict texture G1–G3 and G5 by my readers:
  - 780 masks, IoU 1.000, 0 spill;
  - 0 of 140 edge frames changed, 0 of 35 reverts left residue;
  - `unavailable` 0, `r.CustomDepth` 1;
  - GN4's counter reads 0.
- **Not run,** because Codex stopped at the D3D12 failure: the 1279×719 combined leg, GO3's default-only run, and the card's read commands.
- As information only, D3D12 control A compared against `c5d0919`'s D3D12 output removes no key and changes no type.

**D5:** the auto pool and default routes are unchanged. The only source change is the guarded `-IAICameraFarOnly` switch (non-Shipping, `-IAIBench`, EA_Core only).

**Push and card:** not pushed and no pre-push scan written, because the push condition wasn't met. Origin is still `c5d0919` (local `886e1a0`), and the card stays 094-45r. The full review is `_reviews\094-46r-code-review.md` and Codex's report is `_reviews\094-46r-codex-report.md`.

**Safety: none.**
- No crash and no upload sign.
- The Paris host's own Content, Config, descriptor and `.uproject` are unchanged, and both 4.25 and both 5.1 material `.uasset`s are unchanged.
- Binaries changed only during the two builds; m51 is still `53bf725` / `526aba7f` with its 15 untracked files.
- Nothing was deleted and there are no reparse points. C: had no per-session drop and ended at 23.2 GiB.
- One new practice from Codex: it compressed 13 receipt files it had created, with `compact`, to keep the kit's 3 GiB disk reserve. I rehashed all 13 and none changed.

**Decisions chat must make:**
1. **The D3D12 count bound** (15 px over on 6 frames, no pixel above 2 levels). Options:
   - **(a)** keep GO2 as written, so camera_clipping fails on D3D12;
   - **(b)** seal the near-plane floor over several pairs; it would read 254 here, so D3D12 still fails by 1 px;
   - **(c)** where the two floors differ in level, bound only the level for unlabelled frames; both RHIs have 0 pixels above 2;
   - **(d)** compare the distributions instead of the maxima.
2. **Then:** D3D12's D4 under that rule, the 1279×719 combined leg, GO3's default-only run, and my scan, push and card update.
api-equivalent cost (not billed on Max): $17.44 | turns: 137 | running total across all briefs: $5,803.34
