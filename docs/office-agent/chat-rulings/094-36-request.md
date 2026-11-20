094-36 — C5 RE-RUN WITH 4.25 CENSUS PARITY AND A WELL-POSED PRE-PASS. In short:
- hold census batches until readback and keep event targets tagged for the event (5.1);
- never touch the game's own custom-depth actors;
- prove the census changes no pixels (census on vs off);
- remove the grain for scoring;
- score stable pixels only;
- fix the annotation values.
Under chat's ruling 094-35 (FT1–FT6), with FM1 and the lean profile (FJ1). Requested by chat Claude (the orchestrator) on 2026-11-01.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-35-ruling.md (FT1–FT6). It is new. In short:

   FT2 — CENSUS PARITY AND NO INTERFERENCE
   - Parity:
     - hold each census batch until its own readback resolves;
     - keep event targets tagged for the whole event;
     - both as 5.1 does (AnomalyCensus.cpp:629-699);
     - no per-frame RestoreAll and re-tag.
   - The game's own custom-depth actors:
     - never change the custom-depth state of an actor whose custom depth the game already has on;
     - such actors are not targets;
     - this deviates from 5.1 on purpose: log it in gotchas and as a 5.1 follow-up.
   - Interference gate, per pose:
     - a census-on clean hold must match the census-off reference, with no frame active by the C2 rule on stable pixels;
     - report each pose's per-frame maximum of changed stable pixels.
   - If parity alone fails the gate:
     - 4.25-only reductions of re-tagging are allowed if 5.1's floor and ceiling still hold (e.g. not re-measuring actors whose coverage is known for an unchanged view);
     - tagging every candidate for the whole session is not allowed.
   - Last resort:
     - a residual region counts as z-fighting only with evidence that two coplanar surfaces trade places there (e.g. two actors alternately winning custom depth at equal depth);
     - those pixels join the pose's unstable map before the scored pass, are counted in the report, and go in gotchas;
     - any other residual fails the gate.

   FT3 — THE PRE-PASS (replaces FS2's 5 % rule; frozen now)
   - Grain:
     - off for scoring through a runtime-only setting cited from 4.25 source, never a saved asset;
     - if 4.25 has none, phase-match the references to the grain's period.
   - Per pose, in order:
     1. a census-off clean hold, at least 240 frames → the reference and the unstable map;
     2. a census-on clean hold, at least 240 frames → the interference gate.
   - Unstable:
     - a pixel that changes by more than 1 level during the census-off hold, dilated 2 px;
     - excluded from every C2 and C3 count.
   - Self-check: a map built from the first half of the census-off hold must leave the second half with no active frame by the C2 rule. A pose that fails is dropped.
   - Poses:
     - a pose is usable if at least 50 % of its pixels are stable;
     - poses may pitch down to keep the sky small;
     - at least 6 poses, all fixed before the run.
   - Events:
     - scored if both edges are in scored hold frames and at least 50 % of the target's visible pixels are stable;
     - others are counted, not scored.
   - The rest of FS2 stands:
     - C2 on the affected label;
     - C3 by FQ2, FQ3 and FR2;
     - both negatives;
     - at least 3 scored events per anomaly;
     - a scored late-entering target (outside the view at the scored pass's first frame);
     - the census, custom-depth, view and annotation gates.

   FT4 — annotation.json values carry 5.1's meaning (cite 5.1 for each):
   - coverage_ratio as 5.1's screen share;
   - bbox_source as 5.1 sets it;
   - the camera far plane from the view.

   FT5 — FR3's reader checks the freeze against the first scored frame's write time.

   FT6 — this job:
   - one street-map session, plus one retry slot;
   - the window for real sessions ends at 07:00 IST on 11-02.

2. Kept rulings:
   - _reviews\094-34-ruling.md (FS2's changes and gates, as amended by FT3);
   - _reviews\094-33c-ruling.md (FR2, FR3);
   - _reviews\094-33b-ruling.md (FQ2, FQ3);
   - _reviews\094-32-ruling.md (FO2);
   - _reviews\094-31-ruling.md (FM1);
   - _reviews\094-31b-ruling.md (FN2);
   - _reviews\094-27c-ruling.md (FJ1);
   - _reviews\094-27d-ruling.md (FK1);
   - the rulings these keep by name.

3. 094-35's results:
   - _reviews\094-35-codex-report.md;
   - _reviews\094-35-code-review.md (its §2 has the grain, motion and strip analysis).

STARTING POINT
- feat/europa-port at 2143215. C5's census, ledger restore, player view and schema work; nothing was scored.
- FAnomalyMasks425::Advance runs on every captured frame:
  1. FlushRenderingCommands;
  2. RestoreAll;
  3. re-tag the live-fire targets plus a batch of up to 27.
- The census re-tags the reserved-203 actor for one frame per cycle.
- At pose 3 (the wall), rows 686–719 flip every frame with the custom-depth winner (the street ProvaStrada5 vs the wall SM_MuroCasa5_LP2_73).

ORDER
1. Entry: FJ1's fast checks.
2. The changes:
   - FT2, FT4 and FT5, citing 5.1;
   - the FT3 protocol, with a bench-only census switch;
   - test only the changed code (FJ1);
   - write FR3's rule before any session.
3. Rebuild under FM1. Re-bind the manifest.
4. One street-map session under FJ1's gate:
   1. per pose, the census-off then the census-on clean holds;
   2. freeze the maps;
   3. the scored pass.
   Score every gate. If a gate fails and a quick, testable fix exists, use the retry slot.
5. Exit: FJ1's fast checks. Commit with W4. No push.

STOP RULES
- Only the armed kit launches the editor or the game. Builds are authorized in step 3, and for a retry's rebuild.
- Any change to the Paris host's Content, Config, a descriptor or the .uproject is a STOP. The street map is loaded, never saved. Binaries change only in the builds.
- An upload sign is a STOP. A second runtime crash ends the run (FI4).
- m51, master, release/2026-10-05, the tags, the 5.1 engine and StackOBot stay untouched. No push. No tags.
- No security, paging, power or machine-setting changes. No elevation. Delete nothing except by receipt.
- Foreign activity never ends the job; record it (EZ1).

DELIVERABLE
A short report.md, covering:
- the summary;
- the changes, with the 5.1 citations;
- the build;
- the grain setting, with its citation;
- per pose: the stable share, the self-check, the interference result and per-frame maximum, kept or dropped;
- any z-fighting region, with its evidence;
- the scored pass, per anomaly: fired and scored events, C2 mismatches, IoU, precision, spill and completeness;
- the census: peak drawn actors, the scored late-entering target, any ledger exhaustion;
- custom depth: the before, mid and after samples, the reserved value, and the game's own custom-depth actors left untouched;
- the view;
- the annotation schema and values;
- the negatives;
- any crash, upload sign or protected change (or "none");
- the commit.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>". GREEN means every gate passes.
