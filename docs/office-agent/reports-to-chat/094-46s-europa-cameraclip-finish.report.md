# REPORT for 094-46s-europa-cameraclip-finish.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-46s-europa-cameraclip-finish.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-05 11:41:40 | finished: 2026-11-05 13:03:10 | duration: 82 min | exit code: 0
context at last turn: 586256 tokens (59% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-46s-europa-cameraclip-finish.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:** Camera clipping passes at 1280×720 on both D3D11 and D3D12. It fails the odd-size test (D3D12 at 1279×719), so nothing was pushed. Keep using the office card `_reviews\094-45r-office-card-europa.md`.

**D4 under GP2** (I recomputed each from the raw frames with my own reader; Codex's numbers match):

| Leg | Unlabelled frames within 2 levels | Max \|Δ\| | Count at 2 levels (information only) | Labelled frames |
|---|---|---|---|---|
| D3D11 1280×720 (existing data) | 2,496 / 2,496: **pass** | 2 | max 254 | 144, every pixel changed |
| D3D12 1280×720 (existing data) | 2,496 / 2,496: **pass** | 2 | max 255 | 144, every pixel changed |
| D3D12 1279×719 (new control + positive) | 2,365 / 2,496: **fail** | **117** | n/a | 144, every pixel changed |

- On all three legs the spans are exact (2352–2399, 2448–2495, 2544–2591), the controls label 0 frames, the near plane is restored to 10 at Stop, and the shifted-label mutants are caught.
- The 1279×719 pair uses D3D12's 1280×720 floors, as GP3 allows. Codex sealed that comparison before the positive was launched.

**Where the 131 failing frames are** (a correlation; the cause isn't established):
- They are exactly the 131 EA_Floor checker/pink frames (91 checker, 40 pink), in the far section where nothing is clipped.
- The failing pixels sit in rows 373–375, which is the floor's farthest visible row. 99.8 % of them are inside the floor's texture mask, and that mask is identical in both sessions. One pixel just outside the mask, (375,239), reaches 4 levels.
- The plain blue floor stays within 2 levels, and neither 1280×720 leg shows any of this.

**The rest of the 1279×719 leg:**
- H4 passes on both sessions, and G5 is clean.
- Strict texture G3 (IoU 1.000, 0 spill) passes on the control but fails on the positive: the same pixel (375,239) changes on every EA_Floor texture frame (IoU 0.999995). Codex judged G3 against the 0.98 bar instead and called it a pass.

**GO3 did not run.** Codex stopped at the D4 failure, so the default-only run, its key diff and the card reads are all missing. For information only, the 1279×719 control compared with c5d0919's output removes no key and changes no type.

**Push, scan and card:** none.
- Local `feat/europa-port` is at `f3fbba7`: tools and docs only, with no Source, Content or build change.
- Origin is still `c5d0919`, confirmed with `ls-remote`.
- Details are in `_reviews\094-46s-code-review.md` and `_reviews\094-46s-codex-report.md`.

**Safety:** none. There were no crashes or upload signs. The Paris host's Content, Config, descriptor and `.uproject` are hash-equal before and after, and so are all four material files. `m51` matches P9. Nothing was deleted. C: is at 23.0 GiB.

**Decisions for chat:**
1. **D4 at 1279×719.** The options:
   - (a) keep the shared 1280×720 floors, and the leg fails;
   - (b) seal a 1279×719 near-plane floor; it would probably capture the same strip and push the level to about 117;
   - (c) judge D4 only outside other anomalies' masks; it still fails on the one 4-level pixel;
   - (d) record it as a size-dependent side effect of the held near plane.

   My suggestion is to first run a 1279×719 far-only near-100 session as information, not as a floor, to see whether the strip appears with no clipping at all.
2. **Strict texture G3 under a held near plane:** whether the positive's 1–2 px spill at 1279×719 fails it, given that GN1 says IoU 1.000 and Codex used 0.98.
3. **Compression scope:** Codex compressed 24 archived EXE/DLL/PDB copies, as well as receipts. All hashes and modification times are unchanged, but GP1 allowed compression on receipts only.
api-equivalent cost (not billed on Max): $16.33 | turns: 113 | running total across all briefs: $5,819.67
