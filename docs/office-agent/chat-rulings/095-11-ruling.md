# 095-11 — Chat ruling: the review of m57 and m59 is ACCEPTED; every finding and note is ruled

**Date:** 2026-11-14, about 11:45 IST · **Author:** chat Claude (orchestrator)
**Input:** mailbox report 095-11 (GREEN, 24 min, $39.26 API-equivalent; fresh side session, read-only) and the full review file `_reviews\095-11-independent-review.md`, read by chat.

## The review's verdicts
| Unit | Verdict | BLOCKER | SHOULD-FIX | NOTE |
|---|---|---|---|---|
| m57 `lod_popping` spatial trigger (`f77deed`) | READY AFTER FIXES | 3 | 6 | 14 |
| m59 forced-LOD refusals (`af1b08f`, in `integ/m56-m58` `876c719`) | READY FOR GATES | 0 | 3 | 1 |
| CaptureBench `m57-gates` (`9eb8ca6`) | NOT READY | 3 | 2 | 0 |

**The label path of m57 was found sound in both tick orders.** The blockers are about yield (as built, almost no test clip would contain an event), one loosened check, and bench scripts that could not have judged anything. B1 and B2 are worked out from code and engine source, not measured.

## BLOCKERS: all fixed
- **B1 view entry never arms on the bench, and the cause is partly in the product.** A candidate must sit within a fixed 20° of the view edge at the instant of a firing chance, and firing chances come once per 16-tick cycle. At normal turn speeds that window is shorter than a cycle.
  - **Product:** the view-entry candidate rule becomes time-based. A target qualifies when, at the current yaw rate, it is predicted to enter the view within the arm look-ahead, with a floor of 20° and a cap of 120°. The arming window must be longer than one burst cycle plus the motion-estimate lag at the minimum yaw rate; a pure test proves that.
  - **Bench:** the turn paths give each target an arming window of at least two cycles. The capture config is pinned in the predictions.
- **B2 distance fires time out before anything is visible.** Both of the reviewer's fixes: the distance arm looks ahead to the **later** of the contrast switch and the size at which coverage reaches the visibility threshold; and the arm timeout restarts at the first wrong frame. The bench copies and the second mesh pair move onto the path's end, with room for the release margin. **Deviation 11 (the copy placement) is rejected as built.**
- **B3 the label-rule reader loosens the by-id veto check on schema 2.3.** Fix: minor 2 or later. Add 2.3 copies of the veto cases to the selftest.
- **B4, B5, B6 bench scripts.** The null-pair check reads the box as `[x, y, w, h]`, judges every common frame of both legs, refuses unequal legs, and cannot pass with nothing labelled. The clip script builds a clip from the files the overlay tool really writes, proven by running the real tool in its selftest, and writes under its output folder.

## SHOULD-FIX: all fixed
- **S1 the render-side check is too weak.** It probes the swap frame and the window after it, compares the dither samples, and reads the first resident LOD on the render thread. A disagreement flags `lod_unconfirmed`. **Flag first** (the safe direction); waiting for dither agreement before a release is considered only after a bench reading.
- **S2 a wrong LOD behind a see-through blocker reads as a clean negative.** Fix: held, in the frustum, engine finer, but trace-blocked becomes transition-only with the existing `lod_subvisible` reason and a state of `occluded`. Never a plain negative. No new reason bit.
- **S3 the box, mask and visibility count cover the whole actor while one component is forced.** Both m57 and m59 set the event's component scope to the forced components and clear it at revert, as the texture anomalies do. This is a label fix: a box must cover what changed.
- **S4 a vetoed LOD event with flagged frames makes the reader refuse the session.** Fix as the reviewer says, on minor 3, by id.
- **S5 the client's check kit cannot judge a spatial LOD event and does not say so.** The kit prints "not judgeable (spatial lod)" with the reason, its version is bumped, and the readme and the office-check note say it. **Queued as its own unit:** a kit path that judges the recorded swap edge from pixels.
- **S6 docs:** schema 2.3, the two reasons, the new keys, the burst extension, and "except zero-pixel vetoed events" beside the kept-event rule.
- **S7, S8 bench:** the readout cannot pass with nothing checked and pairs each revert with its fire; the plugin logs a revert line for a destroyed mesh too. The fixture script is atomic (checks before saving), its two options work in separate runs, and it never discards an open map's unsaved changes.
- **S9 (m59) an explicit LOD index that is not coarser than the current LOD is refused.** Same class of defect as the rest of m59: a label with nothing changed.
- **S10 (m59) a mesh that MinLOD or stripped LODs reduce to one usable LOD is refused as `single_lod`,** static and skinned.
- **S11 (m59) the readme and help text describe the skeletal switch correctly.**
  - **Correction to chat's own H4 advice:** the first node's `component_class` is not a reliable filter for old datasets (it is the first visible mesh component, not the forced one, and games subclass the instanced component). The client note must not promise a filter.

## NOTES
| Note | Ruling |
|---|---|
| N1 commit the stored tick-end evaluation unconditionally | **Apply.** Simpler and order-proof. |
| N2 held LOD finer than the engine's after a retreat on a 3-LOD mesh | **Apply.** Release at the first tick the engine's LOD is at or past the held one; that release is invisible. |
| N3 candidate scan cost unmeasured | **Apply the readout** the plan promised (`lod_predict_ms` keys). Optimise only after a bench reading. |
| N4 shadow test is coarse | **Leave;** document. It over-refuses, which is the safe side. |
| N5 the candidate path bypasses the usual exclusions | **Apply** the label-safety ones: translucent-only targets and name-pattern exclusions. Document the rest. |
| N6 `lod_unconfirmed` frames keep a mask | **Document.** |
| N7 a hold-cap swap inferred on a frame never captured | **Apply.** No swap is recorded for a frame that does not exist; the kind is `no_swap`. |
| N8 an event whose labelled frames all failed to write is listed as untriggered | **Leave;** document. |
| N9 no render record on the backbuffer path | **Apply.** Spatial fires are refused there (`render_record_unavailable`); a label that cannot be confirmed is not written. |
| N10, N13, N14 cosmetic and prefix check | **Apply.** |
| N11 the positive rule has no contrast term | **Apply.** A positive needs the contrast test too; wrong but low-contrast frames are `lod_subvisible`. The m30 lesson: a faint pop is not a usable positive. |
| N12 the selector shows "0 matched" on a refusal | **Apply** if it is a few lines: show the refusal reason. |

**Also to check (the reviewer did not):** whether any shipped dashboard surface still sends a numeric second argument to `lod_popping`. It would now be refused as bench-only. Read-only look at the dashboard repo; report.

## The 13 deviations
Accepted as the reviewer judged them: 2, 4, 5, 8, 9, 12, 13 as built; 1 with S3; 3 with N1; 6 with N7; 7 with N6; 10 with S5. **11 rejected** (B2).

## Order of work (light)
1. **095-12 (xhigh):** m59 follow-ups (S3 for `lod_corruption`, S9, S10, S11, N14) on its branch, merged into `integ/m56-m58`. This goes first because the integration branch is what the heavy run gates and ships.
2. **095-13 (xhigh):** m57 fixes, readers and docs, on its branch (after merging the integration tip in), and the bench scripts and paths. Predictions amended before any leg.
3. **On the owner's word (PC free, about 85 GiB on E:): 095-07** (heavy), then the m57 clips for the owner.

---

## Addendum, 2026-11-14 about 12:10 IST — the owner's answers

- **Characters:** excluded from LOD anomalies. The owner confirmed ("Yes. Let's exclude characters"); the client says characters do not matter. H2's default stands.
- **Earlier datasets:** no note to the client. They are concerned with future datasets only.
- **The client's reference LOD clip:** the player stands and turns; an object that should be at full detail comes into frame at low detail and visibly becomes high detail after about a second. So **view entry is the primary case, and its hold default is about one second (30 frames at 30 fps), not 4.** Folded into 095-13 as Part 1b, with clip variants at 15 and 60 frames.
- **Movement must be covered as well,** so the distance trigger stays.
- **The down-pop on retreat:** to be confirmed with the client. Built, on by default for the clips.
- **The reviewed session** (dated 2026-09-09) was captured with the **M2 build on the owner's office PC.** Its own records may no longer exist, so the 675→676 reading may never be confirmed from data; the cause stays "most likely the settle gap".
- **The review document** has nothing beyond the three-topic table.
- **The owner's apps are closed** (Blender, ComfyUI, Resolve). The heavy run now waits only on E: space, which he will come back on.
- **A client-facing plan** was drafted as a living doc at his request (title: "Annotation quality review: our plan for the three priority topics").
