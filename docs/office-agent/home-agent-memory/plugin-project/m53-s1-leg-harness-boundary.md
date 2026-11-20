---
name: m53-s1-leg-harness-boundary
description: m53 S1 leg harness (082-07-*, boundary 1c3a348f) is HISTORICAL since S1 closed (082-09); the S2 plan (082-10, f6b47d4) puts S2's harness in a NEW _reviews\082-10-* set with its own boundary - never re-issue or edit the 082-07 lock.
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-09-28T02:55:40.298Z
---

The m53 S1 step-6 leg harness lives in `D:\IntrusiveAnomalies\_reviews\082-07-*` (common, legs, lib, run.ps1, preflight,
postflight, window). It is locked by `_reviews/082-07-evidence/boundary.json`.

**Boundary history:**
- **082-07a** issued it at `7dc666e`; kept as `boundary-082-07a.json`.
- **082-07d** re-issued it at `202ae72` with AMENDMENT 1 and utoc `30FE0FDE`; kept as `boundary-082-07d.json`.
- **082-07g (2026-09-28)** re-issued it (`6520d84d…`). The lock pins:
  - feature head `18336a9`;
  - the predictions with AMENDMENTS 1 and 2 (`acfbbe53`);
  - lib `3fd05513`, common `91af53d8`, supersede `85fb48dd`;
  - the staged build: exe `2FCDF059` + utoc `20DA6F98` + ucas `534C5863`.

**082-07g changed:**
- `common.STAGED`, utoc and ucas only. `common.ARCHIVE` still names the `30FE0FDE` archive on purpose; it is not read at run time.
- The lib now has:
  - `PRE-CLIP MASKED`, which stops with code 12;
  - the halo ring `HALO_R = 42`: ring < 32, far field ≤ 2;
  - three flagged defaults: the G-BIND global-half ring, TC_Layer half-locating at < 16, and G-RD phase-matched drift.
- Supersede entry 3 covers `G0_FSYN_A2` and the three MASKED A1 attempts; the aliases are renamed `__SUPERSEDED_082-07g`.
- Proof scripts are in `082-07g-evidence/prep/`, including `boundary-issue-082-07g.py`, which refuses unless the 082-07d
  boundary is still present.

**082-07h run:** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07h 540`.
- It resumes with G0_FSYN_A3, then MASKED whole, then SHARED onward: 143 legs.
- It is held until chat rules the flagged defaults.
- Use a new tag per spent 45-minute wait budget.

**Why:** the preflight refuses on any change to the head, predictions, harness, supersede record or staged build. A docs commit
on the branch after the lock silently blocks the window. This is the same trap as [[m55-requal-prep-boundary-locks-feature-head]].

**How to apply:**
1. Commit and push the docs **before** issuing the boundary.
2. To re-issue: write a new `boundary-issue-<brief>.py` modelled on 082-07g's, which moves the current `boundary.json` aside
   itself.
3. Replay the preflight: `082-07-preflight.py <outdir>`.
4. Re-run the proof suites in the latest `…-evidence/prep/`. `e2e-window.py` runs the REAL window with stubbed launch (G315);
   the bank is a junction to `E:\IA_BuildCache\_bench_sessions_bank`, so scratch hardlinks go on E:.

**No-flip after a supersede act:** re-read each row against the inputs it recorded (G327).

**Pose rule on F-SYN:** there is no A47 input lock, so pose-only invalidity has a budget of 6. A within-leg frame comparison reads
the tonemapper dither (G318/G325).

**082-07h (2026-09-28):** the window ran 104 legs (all accepted) and stopped at G-REASON T10 (stop row 9): TC_Stream2k is 7/12 resident at the frame-1 census but 12/12 at the leg's decision frame 18, so it APPLIED (repo G328). 39 legs unrun from REASON_A5 (G4, G-COLL, F-MW, S8, A5, A6, V2). Docs commit f9d6710 moved the head and edited the predictions (noflip citation), so the boundary must be re-issued before any resume. Read-only readings: _reviews/082-07h-evidence/.

**082-07i (2026-09-28):** boundary `456f4146` (082-07g's kept as `boundary-082-07g.json`), head `570352e`, predictions
`e0cddf9a` (AMENDMENT 3), lib `b1e0cdcd`, legs `84a423a6`, supersede `ff7f884f` (entry 4: T10). T10 is `NOT_RUN` (no leg), its
row a class-D FIXTURE-CANNOT-EXERCISE; F-MW rows compare only events all legs applied (G329); MW-FLOOR is a read-only D row.
Proof suites live as copies in `_reviews/082-07i-evidence/prep/` (so earlier evidence is not overwritten); `proof-082-07g`
reads 32/33 because its P5a real-data scan sees 10 max-1 differences on ChainD grazing/transition tiles (G330).
Invocation: `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07j 540`.
Never write markdown through a PowerShell double-quoted here-string (G331).

**082-07j (2026-09-28):** 4 legs accepted (A5, A6, V2, G4 FIN-UV), then `G4_CAN_UV` went NOT-RUN: the runner's `Hold-Focus`
forces focus before frame 1, so cancel-before-focus is unreachable (G332). A recorded NOT-RUN stops every window (lib
`run_leg`/`run_sequence`, code 2) until superseded.

**082-07k (2026-09-28):** boundary `1c3a348f` (082-07i's kept as `boundary-082-07i.json`), head `7a5b67b`, predictions
`8acb75d4` (AMENDMENT 4), lib `96831d8f`, legs `c552c587`, supersede `e67f0ec9` (entry 5: the three CAN attempts + the
label-less NOT-RUN via the new `records` key), plus a NEW locked harness file `_reviews/082-07-mw-trajectory.json` (`1ce9d988`).
- CAN legs are kind `stop_before_fire` (runner kind `capture`); TRIP reads run_summary (a 0-frame session has no labels.jsonl).
- F-MW pixel legs must match the pinned MainWorld trajectory (G333); non-pixel F-MW / F-GATE legs have no pose clause.
- GC legs carry `Log LogGarbage Log` (G334); TWO's named frame-1 refusal reads FIXTURE-CANNOT-EXERCISE.
- Attempt numbers count labelled ledger rows only (the fresh CAN is A4).
- Proof copies in `_reviews/082-07k-evidence/prep/`; `proof-evaluators` reads 207/210 by design (3 superseded old-premise checks).
Invocation: `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07l 540`.
