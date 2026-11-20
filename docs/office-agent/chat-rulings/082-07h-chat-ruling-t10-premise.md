# 082-07h — Chat ruling on G-REASON T10: a gate-premise defect (G328), not a feature FAIL. Review the time axis, amend, then run the last 39 legs

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `f9d6710` · Staged `2FCDF059` / `20DA6F98` / `534C5863`

## What S1 has established (082-07b + 07e + 07h)
- **Identity is exact:**
  - G-ID max |d| is **0** on all 14 encoding rows, including MASKED on a tile that now really clips (PRE-CLIP PASS).
  - G-ID-M is 29/29, with max ≤ 1 against a limit of 2.
- **Every wrong copy is visible:** chanswap, sRGB-twice, alpha, normal, mipshift, mipgen and texelshift. The thinnest margin is texelshift at 34 vs 16.
- **Nothing is left behind:** G3 passes 43/43 at the same frame index; TRIP and G11 pass 154/154.
- **G-RD** passes 2/2, **G-BIND (3)** passes (ring 4, far 1, far-strip 1), and **G6** passes.
- **Refusals:** 14 of 15 G-REASON rows produced exactly their expected final reason.
- **P2-1 and P2-3** are confirmed over 153 legs (read-only).

## Rulings
1. **T10: GATE-PREMISE DEFECT. The feature is not at fault.**
   - At its decision frame (18), the leg read `resident=12 max=12` → `transformable` → APPLY. That is correct under the tree's rule (`TexCorruptTree.cpp:466`).
   - The row assumed that `T_TC_Stream2k` stays partly resident from the frame-1 census until apply. It does not: it is full within 17 frames.
   - This is G328, G320's shape on the time axis.
2. **T10's disposition in S1:**
   - The attempt and its halt row are superseded with their history kept. The row is recorded as FIXTURE-CANNOT-EXERCISE (premise), not PASS.
   - **In AMENDMENT 3, with a source citation:** confirm whether the G0 census and the apply path call the **same** V1 predicate.
     - **If they do:** S1's evidence for `not_fully_resident` is the predicate proven in-engine (three frame-1 censuses read `resident=7 max=12` → `not_fully_resident`), plus the refusal plumbing proven by the other 14 rows. Declare it as exactly that.
     - **If they don't:** say so; the evidence is then weaker, and S2 must carry it.
   - **A full T10 row is owed in S2** with a **deterministic** producer:
     - no timing race, so no "decide before the lead-in";
     - no lever the tree files under another sub-reason (`r.Streaming.MipBias` is T9S's `runtime_lod_bias`).

     First establish why the streamer loads the full chain (the cause read), then propose the producer with evidence.
3. **The time-axis review, before any resume: G328 applied to every unrun row, the same drill as 082-07g's ruling 4.**
   - For each of the 39 unrun legs and their rows, check any premise that rests on a state read earlier than the leg's decision frame: residency, streaming, async loads, level streaming, frame-1 census admissions.
   - For each row, give one line: **unaffected** (with the reason), or **affected**, with a declared encoding fix proven both ways, or a disposition.
   - Include the ADMIT pre-check: it reads a frame-1 census. Say whether any admission could flip by the decision frame.
4. **The MainWorld "zero yield" finding is downgraded to UNVERIFIED.** It came from a frame-1 G0 F-MW reading (floor `not_fully_resident`), and G328 shows frame-1 residency need not hold.
   - The rocks' `texture_not_parameter` is structural and stands.
   - AMENDMENT 3 adds a **read-only reading** to the resume: the MainWorld floor's decision-frame residency and final reason, taken from whichever unrun F-MW legs decide on it (`FMW_*`, the F-MW REASON legs).
   - If no unrun leg decides on the floor, say so, and it goes to the S3 yield work.
   - Chat will correct the owner-facing yield statement once this reading exists.

## Sequencing
- **082-07i (xhigh, continue the 082-07h session, bench-free):**
  - the time-axis review;
  - AMENDMENT 3: T10, the V1-predicate citation, the MainWorld reading, and any row the review changes;
  - the supersede record for T10;
  - all proof suites and a dry run;
  - the boundary re-issue (it is stale anyway) and the preflight replay;
  - the 082-07j invocation.
- **082-07j (high, fresh session):** the remaining 39 legs from `REASON_A5`, with P2-2, P2-5 and P2-6.
- **Then:** the Codex evidence review of all of S1 through the Relay.
