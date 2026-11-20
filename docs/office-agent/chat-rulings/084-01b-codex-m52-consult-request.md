# Request to Codex (via the GDP Relay): consult round on the m52 label-timing diagnosis — rule on the disagreements (Claude Code, 2026-09-28, brief 084-01)

**Role:** independent reviewer, read-only. Write the report to `review.md`; it will be copied to `_reviews/084-01b-codex-m52-consult.md`.
End with a `VERDICT:` line: AGREED / AGREED-WITH-CORRECTIONS / DISAGREE.

**Inputs**
- Your own report: `D:/IntrusiveAnomalies/_reviews/084-01-codex-m52-label-timing-diagnosis.md`.
- Claude Code's diagnosis (written in parallel, now shared): `D:/IntrusiveAnomalies/_wt_084_01_m52/docs/sessions/2026-09-28-084-01-code-m52-label-timing-diagnosis.md`
  (a worktree of the plugin repo on branch `fix/m52-label-timing` off `master` `b5f15a3`; source anchors are `master`).
- Claude Code's findings note written BEFORE it read your report: `D:/IntrusiveAnomalies/_reviews/084-01-code-m52-measure/my_findings_before_codex.md`.
- Claude Code's measurement scripts and per-event output: `D:/IntrusiveAnomalies/_reviews/084-01-code-m52-measure/` (`measure.py`, `measure2.py`,
  `summary.py`, `pertex.py`, `measure2_out.json`, `summary.txt`). Metric: mean |horizontal+vertical grey gradient| in the event's own
  target-mask region (bbox fallback), normalised to the median over the first ≤9 frames after Apply.
- Plugin source at `master` (`git show master:<path>`); engine source `D:/UESource/UnrealEngine/Engine/Source/`; bank
  `D:/IntrusiveAnomalies/_bench_sessions_bank/`.

**Reconciliation table (Claude Code's summary).** Rows 1–4, 9 and 10 agree and need no ruling unless you see an error.
Rows 5–8 and 11–13 are the points to rule on.

| # | Question | Claude Code | Codex (084-01) | Agree? |
|---|---|---|---|---|
| 1 | Onset signal | GT mirror `GetNumResidentMips()` (CachedSRRState), acknowledged after the RT swap | same | yes |
| 2 | Offset signal | fire removed at `BeginRevert` in the same tick; revert-tick frame labelled clean; picture restores later | same | yes |
| 3 | Masks | same predicate over `LiveFires`; move together; a tail cannot be labelled by a predicate change alone | same | yes |
| 4 | Bank onset | main step at label−1 on 57/59 StackOBot events; pre-steps at −2…−4 | D/E si 75 clearly changed at label−1, smaller departures si 72–74 | yes |
| 5 | Bank offset: last visibly changed frame | still ≥50 % blurred for 5–14 captured frames after label end (mode 5–6); `S` keeps creeping back for up to ~R+30 on some events | F-H: 166–169 blurred, 170 partial, **171 clearly detailed** | **partly** — where does "visible" end? |
| 6 | Slow recovery = `MipBiasFade` | Code first said yes; **Code now WITHDRAWS**: `CalcMipBias` is used only in `SetNewMipCount` and `HasPendingLODTransition` (RenderResource.cpp:1096, Texture.cpp:1128), never by the renderer | not established | Code conceded; confirm |
| 7 | Onset lower bound | ≥1 captured frame by construction in normal paths (sample at `OnWorldTickEnd` precedes the streamer tick; GT learns only in a later `TickStreaming`); synchronous flush path excepted | "no rule making the two observations belong to the same frame" | rule: is ≥1 a sound bound? |
| 8 | Why the lag varies; the bench's constancy | all held textures' GT counts flip on the SAME frame (final-stage in-flight poll, StreamingManagerTexture.cpp:1740–1744) while each texture's picture changes at its own slice; bench is phase-locked (19-frame pre-roll, restore +7 on 55/55, burst period 35 = 5 × 7-frame cycle) so it shows one phase | lag depends on stage/IO/queue; "subtract two" falsified | rule on the per-texture/phase-lock explanation |
| 9 | Separator for correct-looking offsets | OPEN; leading: restore latency vs uncaptured settle frames, imperceptible textures | OPEN; contribution, restore speed, occlusion, saved-frame gaps, `already-back` race | yes |
| 10 | `already-back` race (`Anomaly_StuckLowMip.cpp:719`, no `bBusy` test) | not assessed before; Code agrees it is source-supported, unmeasured | source-supported | yes |
| 11 | Fix: event retention vs next burst | trailing event + an FSM **restore-settle gate** so the next `BeginFire` waits (bounded by the restore timeout) | retain the event through restoration; FSM not discussed | rule: is the gate required, or is overlapping multi-event labelling acceptable? |
| 12 | Fix: RT receipt read point | read `GetState().NumResidentLODs` (first visible mip on partially-resident paths) for the request's textures **inside the captured family's own SVE render pass**, strong refs held on GT until the frame resolves | latch "at a draw-submission boundary" or engine transition hooks, with resource generations | rule: is the SVE-pass read a valid latch for the exact captured view? |
| 13 | Gates | pixel-edge gate per edge; can-fail levers that label from the GT mirror / close at revert must fail; B5 null | corrupted-metadata can-fails (onset +2, offset −5), delayed-restoration and delayed-GT-ack cases | rule: is Code's set sufficient, and which of yours must be added? |

**Questions**
1. Rule on rows 5, 6, 7, 8, 11, 12 and 13, each with evidence (file:line, bank path + frame numbers, or a measurement you run
   read-only). Say AGREE / DISAGREE / CANNOT-DECIDE per row.
2. Row 5 in particular: using Claude Code's archived per-event series (`measure2_out.json`, keys `S_series`, `gfc_series`) and the PNGs,
   is the post-171 creep a visible anomaly, TAA history, a second texture's later swap, or scene drift? What frame should an
   acceptance gate call "last visible", and by what predeclared criterion?
3. Anything in Claude Code's diagnosis, bench plan (§7) or fix design (§8) that is wrong or missing? Rank P1/P2/P3.

Keep it focused. Cite file:line.
