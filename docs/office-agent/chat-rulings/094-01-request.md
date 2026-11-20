094-01 — PLAN (read-only): Europa (UE 4.25) port, Phase A, file by file, with Phase B sketched. Requested by chat Claude (the orchestrator) on 2026-10-16.

This is planning only: no source edits, builds, cooks, launches, copies or bench work. The plan goes to chat for a ruling before any implementation (plan-before-code). You will most likely implement it yourself later through the implementer Relay, so plan what you would actually do.

READ FIRST (binding, in this order)
1. Chat's ruling: D:\IntrusiveAnomalies\_reviews\094-00-europa-port-ruling.md. Decisions P1–P14 are binding. Where your 092-02 study and P1–P14 differ, P1–P14 win; say so explicitly wherever that happens.
2. Your own study and its ruling: _reviews\092-02-codex-europa-port-study.md and _reviews\092-02-ruling.md. The ruling's three independent checks and its Target.cs build decision are upheld.
3. History, for context only:
   - the 070 handoff: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\docs\CHAT-HANDOFF-070-europa-e1-blocked-and-parked.md;
   - the 4.25 audit docs on branch europa-e1 (read through git objects): docs/europa/PHASE0-port-surface-4.25.md and docs/europa/PHASE1-bench-audit-and-plan.md;
   - europa-e1's commit 1f5e305 (the module-availability switch).
4. The release's own rules: docs/PRE-DELIVERY-CHECKLIST.md, docs/gotchas.md (G232–G235, and G460 for the office missing-include failure mode) and docs/architecture.md in the release worktree.

WHERE TO READ (read-only)
- Release worktree: E:\IA_BuildCache\_r917\AnomalyInjector (release/2026-10-05 @ d7eee97, clean). This is the tree to port.
- Git objects in the same repository, read without checkout: europa-e1 (1f5e305); fix/followup-capture (7754fd2, local, unqualified; remote 1bf7054). If git reports "dubious ownership", pass -c safe.directory=* on that command only.
- Stock UE 4.25.4 (CL 14469661): E:\EpicGames\UE_4.25.
- UE 5.1.1 source: D:\UESource\UnrealEngine.
- The converted 4.25 host, read-only reference: E:\Unreal Projects\StylizedParisStreet. It has Game and Editor Target.cs files with the P10 pins. It has never been opened in the editor (no Saved folder), and it has no Plugins folder.
- Do not read, list or hash anything J2 owns beyond what P9 names. You do not need J2 evidence for this plan.

THE PLAN MUST COVER

A. Base and branch (P2)
- Confirm what d7eee97 contains that matters for the port.
- europa-e1: cherry-pick 1f5e305 or re-implement? Decide from the actual diff against d7eee97, and give the conflicts.
- Report the merge-base of fix/followup-capture and d7eee97, the git diff --stat d7eee97..7754fd2, and which of those files Phase A also changes. That is the future merge cost.

B. Hosts, checkouts and disk (P8, P9, P10)
- Exact paths for:
  - the D: copy of the 4.25 host, excluding Binaries, Intermediate and Saved;
  - the port worktree inside it;
  - the 5.1 re-gate host and its worktree.
- How each is created, with commands, and what each costs on disk. Measure the source sizes read-only.
- The DDC and shader-cache routing for 4.25:
  - read 4.25's BaseEngine.ini and the DDC backend code, and cite the lines;
  - propose a process-scoped or project-scoped mechanism only;
  - no per-user, machine or global change.
  Say where every 4.25 write lands: editor, game, UBT, ShaderCompileWorker, crash reports.
- Disk floors (P8): when to measure and on which resolved volumes, and the STOP rule.
- How the first port job records the P9 state, and how every later job proves it unchanged.

C. File-by-file change list for Phase A (P3–P7)
For every file changed or added, give:
- what changes;
- why;
- the 5.1 effect: byte-identical / changed but meant to be neutral / changed.
Cover at least:
- the shim class (P4), its exact 4.25 and 5.1 tick, initialized and CDO semantics, and the five re-parented subsystems;
- ShouldCreateSubsystem;
- the type-ancestry consumer audit;
- the descriptor keys (P5);
- Build.cs and module rules for both engines (G233: every C# branch must compile on both);
- TObjectPtr to raw UPROPERTY pointers;
- the Compat helpers;
- includes;
- the C++ standard choice;
- the compile-out on 4.25 (capture, control server, shaders, bench) through the shipped ANOMALY_CAPTURE=0 / ANOMALY_CONTROL_SERVER=0 path;
- the AnomalyShaders stub;
- anything else needed for the injector core to compile, link and load on 4.25.
Name which of 092-02's six anomalies are in the Phase A smoke (no plugin-authored asset needed) and why.

D. Builds and the strict-include tool
- The exact 4.25 and 5.1 build commands.
- The strict_include_build.py adaptation (092-02 §6): no IncludeOrderVersion, UE4Editor paths, an explicit engine root. Running only in the port's own checkouts.
- How you will show 5.1 still selects 14.38.33130 while 4.25 selects 14.16.27051.

E. Gates and predictions (P11)
- The 4.25 gates and the 5.1 gates, each with:
  - its predicate;
  - its can-fail (a lever or mutant that must make it fail);
  - PIE or packaged;
  - which need the idle gate;
  - which need the owner (the editor-preview world leg, the eye check).
- The 5.1 reach analysis: every 5.1 file the plan changes, the behaviours it can reach, and the gate covering each.
- The Phase A picture evidence without the capture module: engine screenshots, pixel diffs, and the thresholds.
- Which 5.1 staged checks Phase A itself needs, for example a packaged build for the descriptor proof. Which staged checks wait until before any merge or 5.1 delivery from this branch.
- A predictions file, frozen and committed before the first leg.

F. Phase B sketch
- The six anomalies on 4.25: missing_object (bench only; it is not a client anomaly), blinking, lighting_mismatch, lod_corruption, missing_texture, corrupted_texture.
- How the 4.25 materials get authored without the owner clicking through the editor (editor Python, a commandlet, or runtime-built material instances from engine materials), and what each option risks.
- What the Paris level has, or lacks, for each anomaly: static LODs, lights, and skeletal meshes for skeletal LOD. The smallest fixture additions.

G. The early Europa read card (P14)
- About 10 to 15 read-only checks the owner runs at the office, each answered with a number or 0/1 that he types himself.
- For each: what it decides, and which phase it de-risks.
- No paths, titles or source leave the office (P13).

H. The job split
- Codex implementer jobs, each fitting one bench session.
- Their order, the effort estimate for each, and what the owner must do and when.

I. Risks and open questions
- Your top risks.
- Where P1–P14 or the 092-02 study may be wrong.
- Questions for chat. Product questions for the owner, if any.

DELIVERABLE
review.md in your run folder: a one-screen summary first, then A–I. End with one line, for example: "VERDICT: PLAN READY | base=d7eee97 | europa-e1=<cherry-pick|reimplement> | 4.25 host=<path> | 5.1 host=<path> | phase A jobs=<n> | owner legs=<n>".
