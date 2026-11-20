# REPORT for 095-02-plans-a1-continuity-c-terrain.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-02-plans-a1-continuity-c-terrain.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-13 12:24:53 | finished: 2026-11-13 12:51:01 | duration: 26 min | exit code: 0
context at last turn: 712944 tokens (71% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-02-plans-a1-continuity-c-terrain.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

Both plan files are written and complete. Nothing was built, run or changed in any repo; the checkout is still `master` at `ebc34ed`, with only the 15 known untracked files.

**1. For the owner**
- Today two frames go missing right after every anomaly appears and right after it disappears. Plan A-1 makes every frame present, and the extra frames carry a flag the client already knows to drop. A drop count will report any frame that is ever lost.
- Plan C labels hidden ground as `terrain_disappear_reappear`. Today's code can only label ground built from ordinary meshes correctly. Unreal's built-in landscape terrain can't be labelled properly yet, so the plan stops those events from being made at all.
- The next steps, once E: has room: rebuild, one cook, then roughly 8 bench hours for A-1 and 1–1.5 for C.

**2. Plan A-1 in ten lines**
- **Cause, checked:** the two settle phases count down without capturing. A cycle is 16 ticks but only 12 frames, and the video plays about 25 % short. A banked bench session shows it directly: `frame_index` jumps by 3 at every onset and offset, and `ticks_per_captured_frame` is 1.344.
- **Change:** about 6 lines in the state machine make both settle phases capture every tick. Those frames get a new reason, `settle` (bit 9); onset settle frames keep their positive label. A row-level flag covers settle frames that have no anomaly entry. The settle setting K is kept and now means "frames flagged per edge"; the default stays 2.
- **New keys:**
  - each row gains `tick_index` and `t_wall_run`;
  - `run_summary` gains `frames_dropped`, `frames_dropped_ticks`, `frames_dropped_by_cause`, `frames_armed`, `capture_span_ticks`, `settle_frames`, `label_transition_settle_entries` and `label_rows_failed`;
  - `ticks_per_captured_frame` is redefined so it reads exactly 1.0 on a healthy run;
  - `label_schema_minor` goes to 2.
- **Fps stamp:** it now follows frames written, so nothing changes on a continuous run.
- **Writer:** run end waits for every job, failures are counted per frame, and the `labels.jsonl` append result is checked. A small in-flight cap blocks rather than letting the queue grow.
- **`6713f51`:** cherry-picked as its own commit. One hunk has to be moved past the m52 order-hold block; with reduced context the whole patch applies.
- **Readers that break loudly:** `verify_capture.py:3260` rejects schema minor 2; the office check kit (`label_sync_check.py`) then refuses every new session and doesn't know `settle`.
- **Readers that would be silently wrong:** ffmpeg in the encode step stops at the first missing frame number, the dashboard encoder repeats frames to fill gaps, and `measure_label_offset.py` and `a54_oracle.py` can let settle frames skew their readings. No reader matches reason names by substring, so the name `settle` is safe.
- **Gates (about 58 legs, ~6.2 GB of sessions):**
  - continuity, 8 legs plus one MainWorld load leg;
  - pixel edges at AA-off, 24 legs (8 anomaly types × 3);
  - masks, read from those same legs;
  - can-fails: the release binary, two new bench drop levers, mutated sessions, and the `6713f51` legs including the never-run backbuffer leg;
  - a comparison against the release build showing non-settle frames unchanged.

  PIE legs need no cook. Staged legs need one cook, because no staged package exists any more.
- **Estimate:** 5 briefs, about 8 bench hours after disk is freed.

**3. Plan C in six lines**
- The subtype is decided in `MapAnomalyToClient` (`AnomalyCaptureSubsystem.cpp:652`) when the annotation is written. Terrain will be decided earlier, in `FreezeEventAnchor` while the target still exists, and stored with the event. Both places that set the subtype will use it.
- **Detection:** look up the landscape classes by name at run start, with no new module dependency; if the lookup fails it says so in the run banner. Plus an ini pattern list (default empty) with a console override.
- **Item 4 answer: no.** Engine landscape can't be labelled to the standard today:
  - the auto-pool and census can never select it;
  - non-Nanite runtime grass reports 0 instances, so it fails the empty-instance check;
  - a targeted fire does hide the terrain tile, but ships with no mask and a box that is wrong or missing.
- Ground built from meshes (likely what the client has) is labelled correctly today and gets the new subtype through a pattern.
- **Gates:** a unit test with mutants; class detection on a MainWorld landscape tile; the pattern rule on `CB_GateLevel` (clearing the pattern must fail it); non-terrain unchanged; no change to which targets are picked.
- **Estimate:** about 1–1.5 bench hours, sharing A-1's rebuild and cook.

**4. Questions for chat (my recommendation in each)**
- **A1:** add to the office read the `frame_index` and `t_wall` of rows 674–677 in the reviewed session. Frame 675 = 56×12 + 3, which is exactly an onset frame followed by a settle gap at default settings, if numbering starts at 0. A step of 3 confirms the gap; a step of 1 points to a timing hitch instead.
- **A2:** redefine `ticks_per_captured_frame` and add `capture_span_ticks`.
- **A3:** put the row-level settle flag on every settle frame.
- **A4:** remove frames that failed to write from `annotation.json` and count them.
- **A5:** put the writer cap in A-1, not A-2.
- **A6:** the encode step and the dashboard should report gaps, never truncate silently or repeat frames (a later dashboard brief).
- **A7:** don't advise the client to use settle 0 in the meantime.
- **A8:** C does not bump the schema minor; A-1 bumps it to 2.
- **C1:** option (b): ship the subtype plus a refusal of landscape targets.
- **C2:** office yes/no read: is the reviewed session's terrain a landscape or a mesh?
- **C3:** keep the default pattern list empty.
- **C4:** keep `missing_object` out of m58.
- **C5:** an actor counts as terrain if the actor or any of its components matches.
- **C6:** no schema bump for C.

**5. Safety**
None. Three read-only sub-reviewers were used; I re-checked every claim a plan step depends on, and the plans mark checked claims [V] and the rest [R].

Files are in `D:\IntrusiveAnomalies\_reviews`:
- 095-02-plan-A1-capture-continuity.md
- 095-02-plan-C-terrain-blink-subtype.md

Receipts are in `_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-02\`.
api-equivalent cost (not billed on Max): $47.00 | turns: 94 | running total across all briefs: $5,965.47
