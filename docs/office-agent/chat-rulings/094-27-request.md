094-27 — Phase A runtime, in one job: BUILD the 4.25 runtime kit for A6's remaining legs and A7 (the four smoke anomalies, with pictures), REHEARSE it with stand-ins, ARM it, then RUN it for real when the PC is idle. Under the rulings FB1–FB7 in this request, and 094-26c (FF1–FF5), 094-26b (FE1–FE5), 094-28 (FD1–FD6), 094-26 (FC1–FC9), 094-25 (FA1–FA8), 094-24 (EZ1–EZ6) and 094-23 (EY1–EY8). Requested by chat Claude (the orchestrator) on 2026-10-29. Pre-staged: it starts only after 094-26d ends GREEN.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. This request, including FB1–FB7. They are new.
2. _reviews\094-26c-ruling.md (FF1–FF5): FF2's Saved\ class applies to every editor and game leg; FF5: one full preflight per run, not per leg. _reviews\094-26b-ruling.md (FE1–FE5): FE1's anti-aliasing override at capture time and FE3's guard gaps are in this kit's scope; FE2's canary class applies. _reviews\094-28-ruling.md (FD1–FD6): FD2's UBT rule and FD3's memory lines carry into this kit. _reviews\094-26-ruling.md (FC1–FC9): FC2–FC5 carry into this kit (FB7). _reviews\094-25-ruling.md (FA1–FA8): FA5's idle-start rule, FA6's lean checks, FA7's chaining.
3. _reviews\094-24-ruling.md (EZ1–EZ6); _reviews\094-23-ruling.md (EY1–EY8; EY5: the port is 4.25 only); _reviews\094-22-ruling.md (EX1–EX6) as amended by EY1–EY4 and EZ1–EZ2; _reviews\094-21-ruling.md (EW1–EW4); and the earlier rulings they bind, down to _reviews\094-12-request.md (STOP RULES, DELIVERABLE).
4. Your Phase A plan, _reviews\094-01-codex-europa-phase-a-plan.md: §E2 gates A3, A5, A6, A7 and A8 as amended by FB1; §E3 (pictures); §H's A6 and A7 rows. Your A5 probe, readers and fixture manifests, as committed. The 09425 kit's machinery.
5. 094-26d's results: _reviews\094-26d-codex-report.md and _reviews\094-26d-code-review.md (094-26d completed the A6 authoring). If either shows a blocking GAP, or EA_Core or EA_Travel is missing, stop at entry with NEEDS-DECISION.

NEW RULINGS (chat, 2026-10-29)
FB1. Scope. 4.25 only (EY5), in the Paris port host on stock 4.25, using EA_Core and EA_Travel as authored in 094-26d.
  - A6's runtime legs:
    - gate A3: editor load and compile-out. The package-boot part is dropped with A8 (EY5).
    - gate A5: worlds and lifecycle. PIE with two worlds, pause and resume, travel, three PIE end/restart cycles and the preview sentinel; a standalone -game leg with pause, travel and exit.
    - gate A6: reflected GC.
  - A7: the four smoke anomalies in your plan (blinking, lighting_mismatch, lod_corruption, and bench-only missing_object):
    - at least 3 completed, visible apply/revert events each, with before, during and after engine screenshots per §E3;
    - the NoApply and SkipRestore controls;
    - the invalid-target and unsupported-ID refusals.
  - A8's non-package part: admission; seeded auto (4242, twice, the same sequence); the registry; no plugin 5.1 Content load; no residue after stop, travel and teardown.
  - EV6: glass normals are a host-content baseline. Mask glass in the pixel-restoration checks.
FB2. The runtime kit. Derive it from the 09425 kit's machinery: the gate, the launcher exemption by folder, locks and slots, the ledger and root attribution, the admission wait, quarantine, owned-tree stops, the EU4 switch on every argv, and routing. The differences:
  - PIE and a standalone -game launch are allowed, for these legs only. No cook, commandlet or packaging.
  - Nothing under Content may change. Any change to Content, config, a descriptor or the .uproject is a STOP.
  - The outputs are only receipts, probe rows and screenshots under the job's receipts root on D:.
  - Per-leg session limits: propose them in your design note. At its limit, a leg stops only its owned tree.
  - One session per leg, with no automatic retry. A failed leg's evidence is kept.
  - The kit binds only the 4.25 manifest (c501895c…), not the frozen 5.1 records (EY5).
FB3. In-session rules for measured legs (EX2's note).
  - The acting limits stay: free commit below 6 GiB, a drive floor, the leg's session limit, and an acting reading unreadable for 60 s.
  - Human input, foreign GPU load and physical memory are recorded.
  - An event whose window saw human input, or foreign GPU load above the gate's line, is marked invalid and does not count toward the 3. The leg may take further events within its limit to reach 3 valid ones.
FB4. Before the real run:
  - a design note with 4.25 source citations;
  - the kit's tests and can-fails;
  - a rehearsal with harmless stand-ins on owned scratch: a passing leg, a failing leg with its evidence kept, the deadline, and a Content-change STOP;
  - arming for job 09427;
  - a dry run of every leg on throwaway slots;
  - one live, observe-only gate reading.
  D2 applies. If any of these fails twice, stop with NEEDS-DECISION before any real launch. Create no reparse point under D:\IA_Europa or the run folder (FA1).
FB5. The real run.
  - Once FB4 passes, run the armed kit for real under FA5's idle rule: the PC idle for 10 minutes before each start; the kit's gate decides each leg.
  - The window: from the end of FB4 until 06:00 IST the next morning.
  - The order: A6's runtime legs first (load, worlds, GC), then A7, then A8.
  - A crash is a STOP: no further leg; keep everything; report.
FB7. Carry 094-26's lessons (FC2–FC6) into this kit:
  - every script that runs inside the editor or the game is tested under the engine's own Python 2.7.14 with a recording stub of `unreal` (FC2), before any real launch;
  - one real-editor smoke session before the first real leg (FC3), with the same argv; a failed smoke ends the run before any leg;
  - FC4's analytics opt-out switches on every argv, the `-game` leg included, proven for that leg too; no upload of any kind;
  - FC5's managed classes; FD2's UnrealBuildTool rule with FE3's gaps closed, and FD3's memory lines, for the editor and the game;
  - FE1: anti-aliasing off for every A7 picture through a command-line override proven from 4.25 source, with no config edit.
FB6. Evidence and pictures.
  - Each gate's reader verdict, with its receipts.
  - For A7: one contact sheet per anomaly (before, during and after for each counted event), as PNGs under the receipts root, plus one index file listing them, for the owner's eye check.
  - A lean exit (FA6; A7 is a milestone): the canary scan of new and changed files from the launch second, the command audit, the EA8 exit proof, and the ledger summary. No full EK1 scan.

STARTING POINT
- feat/europa-port at 094-26d's final commit. The 09426d kit is spent for its two recipes. EA_Core and EA_Travel exist, as 094-26d authored them.
- The 31124 marker stays in place (ET2).
- The owner wants speed (FA5). His standing OK covers unattended editor and game runs whenever the PC is free.
- A fresh canary, IA_CANARY_TOKEN, is in your launch environment. Its value is not given to you.

ORDER
1. Entry: the EA8 entry proof against v2; ED1 free space (EV3's D: lines, EY4's E: lines); the ED3 toolchains; 094-26d's results (BINDING 5).
2. FB2–FB4 and FB7: the design note; the kit; its tests, the Python 2.7 harness and can-fails; the rehearsal; arm 09427 (with one smoke session); the dry runs; the live reading.
3. FB5: the real run, when the PC is idle, inside the window: the smoke session first (FB7), then the legs.
4. FB6: the evidence, the reader verdicts and the contact sheets.
5. Exit (FA6, lean, milestone): the canary scan; the command audit; the EA8 exit proof; the ledger summary; commit, with W4.

STOP RULES
- As in 094-12-request.md, read with EN5, ET2, EU1, EU2, EV1, EV3, EY4, EZ1, EZ2 and FA6. EN5's other hard STOPs are unchanged.
- Any launch other than the armed kit's legs is a STOP. The kit's own editor, game, ShaderCompileWorker and crash-client processes belong to its owned tree.
- Any change to Content, config, a descriptor, the .uproject, a bound binary or a manifest is a STOP.
- A crash of our editor or game is a STOP for the remaining legs.
- Foreign activity never ends the job (FA6).
- The work limit is FB5's window. Report-only work after it is allowed.
- No push. No tags.

DELIVERABLE
A report in your run folder (report.md). Put a summary first, then:
- the entry and exit proofs;
- the design note's summary, with its citations;
- the kit's tests, the rehearsal and the dry-run receipts;
- the armed scope and slots;
- the live gate reading;
- the real run's timeline, per leg;
- each gate's reader verdict;
- the contact-sheet index;
- the command audit;
- every EN5 item applied, with its source citation.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
