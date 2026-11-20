# REPORT for 084-01-m52-diagnosis-source.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-01-m52-diagnosis-source.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 12:32:01 | finished: 2026-09-28 14:01:15 | duration: 89 min | exit code: 0
context at last turn: 671061 tokens (67% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-01-m52-diagnosis-source.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

Part A is complete and both root causes are established from source and bank, with Codex agreeing independently. Nothing was launched, compiled or staged, and no source changed. Chat has to rule on the fix contract before part B or any code.

## 1. Summary for the owner
- **Labels start late:** the plugin waits for a game-side "mips dropped" counter. The engine updates that counter only after it has already drawn the blurred frame. On our bench the blur shows 1 frame before the label every time, sometimes with a faint change 2–4 frames earlier. Your 2 frames is the same mechanism at a different timing.
- **Labels end early:** the label switches off the moment the anomaly is told to stop, but the sharp texture takes several frames to stream back in. On the bench every complete event has at least 5 clean-labelled frames that are still blurred, which matches your ~5.
- **Why some of your events look right is not established.** The bench has no event where the offset is correct.
- Masks share the label's switch, so they are wrong together. That is why they agree with each other.

## 2. Bank measurements
30 unique sessions (20 StackOBot, 10 Lyra), every frame saved. Sharpness was measured inside each event's own target-mask region.
- Only 60 of 138 StackOBot events (all on one rock target) and 10 of 198 Lyra events show any measurable blur. The rest are grids, holograms and flat maps: the G270 class.

| StackOBot, still camera | Result |
|---|---|
| Onset: main blur step before the label | **1 frame on 43 of 43** events; first faint change 1–4 frames before |
| Offset: frames after the label still at least half-blurred | **5–14** on all 42 complete events (mostly 5–6) |
| Revert → first recovering frame (engine frames) | +6 (24 events), +9 where +6…+8 weren't captured (18) |
| Revert → all textures verified restored (log) | **+7 on 42 of 42** |
| Labelled frames per event at `positive=8` | 7 on 56 of 60: the revert-tick frame is always labelled clean |

- The bench can't show a varied timing: its bursts keep step with the engine's texture streamer.
- Lyra's camera moves in all but one measurable event, so Lyra gives no edge statistics. Its logged restore times vary from 1 frame to over 34.

## 3. Root causes
**Confirmed:**
- **Onset:** the label reads the game-side mirror `GetNumResidentMips()` (`Anomaly_StuckLowMip.cpp:892-907` → `Texture2D.cpp:481-483`). The engine updates it only after the render-thread swap (`StreamableRenderAsset.cpp:172-179`, `StreamableTextureResource.cpp:214-237`), and its streamer runs after our sample in each frame (`GameEngine.cpp:1775/1891/1901`).
- **Offset:** `BeginRevert` drops the fire in the same tick (`AnomalyCaptureSubsystem.cpp:756-759, 806, 4666`), and `Revert()` sets the anomaly inactive at once (`:764`), while the restore is still in flight.
- **Masks and `observable`:** both use the same switch (`:1187`, `:4053-4071`, `IAnomaly.h:32`).
- **The original gates were blind:** m52's onset gate passed by construction, and its edges were never checked against pixels.

**Open:**
- What separates your correct offsets from the late ones.
- Whether the timing race Codex found (`Revert` skips a texture that looks restored while a stream-out is still pending) actually happens.
- What causes the slow final part of the recovery.

**Withdrawn after the Codex consult:**
- My explanation of the slow recovery as a mip fade: that engine code affects no rendering.
- "The label is always ≥1 frame late": the source allows a rare race.
- "All textures always switch together": true on 65 of 70 records only.

## 4. Part B bench plan
- **Build:** the archived m55-merge exe `E0BE6F0A` with the pre-m53 container `67EA1FE0`. Hash the staged m53 set (`2FCDF059`/`20DA6F98`/`534C5863`) before swapping, and restore and re-verify it afterwards.
- **Minimum legs (≥20 measurable events across ≥3 targets, plus nulls):**
  - B0: targeted rock, 1,200 frames, long gap after each burst.
  - B3: auto-pool across at least three targets.
  - B5: the no-hold null.
- **Then:** B7 (the synthetic tick order), B9 (anti-aliasing off, to test whether the slow recovery is temporal history).
- **Optional:** B1/B2 (streamer cycle changes), B6 (only if the log proves a stream-out was pending), B8 (Lyra).
- **Per-leg checks:** focus, total camera movement, streamer settings read from the log, run log on.
- A few frames around each phase change are never captured. An edge that falls in one is reported as unmeasured, not passed.

## 5. Draft fix (not implemented)
- **One source of truth:** for `stuck_low_mip`, a render-thread record of which mips were resident when that exact captured frame was drawn. It is read in the capture pass (`FStreamableTextureResource::GetState()`) and drives the label, the mask, `stuck_mip.held` and `observable`.
- **The event outlives the revert** until that record shows every texture back.
- **The next burst waits** for the restore, while capture continues.
- **A timeout** becomes an explicit "unresolved" outcome, never silently clean.
- **Masks are armed ahead** from the anomaly's start through the restore.
- **Two separate gates:** one for the residency window (controlled single texture, anti-aliasing off) and one for the visible edge (thresholds set on independent null runs, plus three stable clean frames). Both must fail on doctored labels (onset +2, offset −5) and on a lever that restores today's timing.
- **Scope:** only `stuck_low_mip` has this pattern. `lod_popping` is a possible risk, named but untested.

## 6. Codex
- **084-01:** ROOT-CAUSE-ESTABLISHED, independently. Its exact frame match against a no-hold run shows blur one frame before the label. It saw five blurred, unlabelled frames after the label ends, and first flagged the timing race.
- **Where we agreed:** onset signal, offset signal, masks, onset measurement, the open separator, the timing race.
- **Consult 084-01b:** AGREED-WITH-CORRECTIONS.

| Row | Point | Ruling |
|---|---|---|
| 5 | Last visibly blurred frame | Half-blur point agreed (6 frames on the event checked); true "last visible" frame can't be decided from the bank |
| 6 | Slow recovery was a mip fade | My claim withdrawn |
| 7 | Label always ≥1 frame late | My claim withdrawn |
| 8 | Streamer timing as the cause of the lag | Narrowed |
| 11 | Hold the next burst until restore | Agreed, with corrections |
| 12 | Where to read the render record | Agreed, with corrections |
| 13 | My proposed gates | Insufficient; Codex's cases added |
| — | Errors in my camera and restore census | Fixed and re-measured |

## 7. Pushed
- Commit `6405efb` on the new branch `fix/m52-label-timing`, off `master` `b5f15a3`: the journal `docs/sessions/2026-09-28-084-01-code-m52-label-timing-diagnosis.md` and gotcha **G343**. G343 skips ahead because G309–G342 already exist on the m53 branch.
- No tag, no merge. `master`, `m51` and the m53 files are untouched.

Files are in `D:\IntrusiveAnomalies\_reviews\`:
- `084-01-codex-m52-label-timing-diagnosis.md`
- `084-01b-codex-m52-consult.md`
- `084-01b-codex-m52-consult-request.md`
- `084-01-code-m52-measure\`

Both Relay runs have ledger rows.

## 8. Decisions needed
1. **Fix contract:** a residency window with visibility reported separately, or a visible-pixel guarantee? The latter needs a stronger render-side witness.
2. **Restore handling:** gate the next burst on the restore (my proposal, which Codex accepts for this scope), or allow overlapping events with per-event records.
3. **Timeout:** stop the run or refuse further fires with an explicit "unresolved" outcome, instead of labelling still-blurred frames clean.
4. **Part B:** authorise the bench window, which means temporarily staging the m55-merge build while m53 S2 is paused.
5. **CLAUDE.md status block** was not updated; the brief listed only the journal and gotcha.

Separately, several MCP connectors (GitHub, Slack, Notion and others) need authorising via `claude mcp` or claude.ai connector settings, and the PDF viewer failed to connect. Nothing here depended on them.
api-equivalent cost (not billed on Max): $20.00 | turns: 151 | running total across all briefs: $3,009.57
