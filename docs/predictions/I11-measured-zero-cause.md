BRANCH TABLE — cause of a MEASURED_ZERO
Pre-declared before the instrument exists. Restate this file VERBATIM before reading
any result.

SUBJECT
Every event the m26 veto deletes in a fresh auto-pool run in real gameplay, plus, if
they recur, the four from the owner's play-gate smoke: BP_Stomper_C x2,
RoomBuilderSquare_C x2.

WHY IT MATTERS
RoomBuilderSquare_C is on record as a LEGITIMATE, DRAWING target — journal Part Eleven's
cure table put it in the GOOD group (whole-frame delta 0.04413, peak-IN/OUT
0.2030/0.0027, a 75:1 in/out ratio), and Part Twelve named it as the cost of the
rejected blacklist rule. It measured zero in that view and was deleted. Either that is
correct under view-dependence, or the cure is deleting good events.

READINGS TAKEN, per vetoed event, per measured frame where applicable
R1  mask state, and the frame counts behind it (attempted / noPass / measured)
R2  the target's projected bbox and its on-screen extent — fully off-rect /
    partially on / fully on
R3  an independent read of whether the target was rendered in the main pass that frame
R4  the claimed area the label asserts — CONTEXT ONLY. Not a threshold. Not an input to
    any rule. It is recorded so the picture is visible, and for no other purpose.
R5  target identity: name, class, asset, and whether Nanite

BRANCHES — every vetoed event lands in exactly one
Z-1  NEVER ON SCREEN.
     R2 reads fully off-rect on every measured frame.
Z-2  ON SCREEN AND RENDERED, DREW ZERO.
     R2 on-screen on >=1 measured frame, R1 says that frame was measured not noPass,
     R3 says rendered on that frame, drawn count zero. Occlusion-shaped.
Z-3  ON SCREEN, NOT RENDERED.
     R2 on-screen on >=1 measured frame, R3 says not rendered on those frames.
Z-4  ANOMALOUS.
     R2 on-screen, R3 rendered, R4 non-trivial, AND a frame capture of that window
     shows the object plainly visible. This is the SM_Ramp2 false-accusation shape.
     STOP THE LINE. Report and halt. No same-turn fix. No rule change without a chat
     ruling.
Z-5  UNCLASSIFIABLE.
     Readings absent, contradictory, or the event fits none of Z-1..Z-4. Report as Z-5.
     Do NOT force it into a branch. Do NOT substitute a weaker reading for a missing
     one (A60).

WHAT EACH OUTCOME MEANS — pre-registered so the verdict is not argued afterwards
Majority Z-1 or Z-2 -> the veto is broader than "H5" and is behaving correctly. The
                       scope correction stands. No rule change. m26 stays as tagged.
Majority Z-3        -> same conclusion, plus the first sighting adjacent to H5 class (i).
                       Record it. Still no rule change.
Any Z-4             -> the cure is deleting good events. Delivery-affecting. Stop,
                       report, chat ruling before anything else proceeds.
Any Z-5             -> the instrument is under-specified. Redesign chat-side before any
                       verdict is written.

CONDITIONS AND LIMITS, DECLARED IN ADVANCE
- PIE is acceptable for this question because it is a mechanism question. G76 still
  holds: a PIE result licenses NO shipping claim and no statement about delivered
  captures. A packaged confirmation is a separate, later decision.
- Incidence: report the vetoed fraction for each run WITH its conditions attached
  (PIE or packaged, level, camera path, event count). Do NOT aggregate across
  dissimilar runs. The project has never claimed incidence and this does not start.
- Bank every run attempt before reading any gate. Match by session ID, never by
  directory name.
- The veto rule is ZERO-ONLY and is LOCKED. Nothing in this investigation changes it.
  A Z-4 does not change it either — a Z-4 stops the line and comes to chat.
