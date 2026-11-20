093-02 — PLAN (read-only): the natural-cause foundation plus the first two pilots (MISS-1 controlled calibration, MISS-2 natural), file by file. Requested by chat Claude (the orchestrator) on 2026-10-13.

This is planning only: no source edits, builds, cooks, launches or bench work. The plan comes back to chat and the owner for approval before any implementation (plan-before-code). The delivered build (release/2026-10-05 @ d7eee97) stays frozen.

READ FIRST (binding, in this order)
1. Chat's ruling: D:\IntrusiveAnomalies\_reviews\093-01-ruling.md. It is binding and supersedes the design draft where they conflict.
2. Your feasibility review and Claude Code's verification: _reviews\093-01-codex-natural-causes-feasibility.md and _reviews\093-01-code-verification.md. Code's corrections are upheld (ruling R9).
3. The design draft r2: _reviews\drafts\093-00-natural-causes-design.DRAFT.md.
4. The four-rule principle: _reviews\090-10x-chat-ruling-compatibility.md.
5. The follow-up work to account for:
   - _reviews\091-04-ruling.md, _reviews\091-05-ruling.md and _reviews\091-07-ruling.md;
   - _reviews\2026-10-10-disk-cleanup\README.md: the saved office-robust F1 work-in-progress patch and untracked files, base 1eaf205.
6. The release's docs/PRE-DELIVERY-CHECKLIST.md §0, and docs/client-readme.md for the shipped label meanings.

WHERE TO READ (read-only)
- Release worktree: E:\IA_BuildCache\_r917\AnomalyInjector (release/2026-10-05 @ d7eee97, clean).
- Git objects in the same repository, read without checkout:
  - fix/m52-label-timing 1eaf205;
  - feat/m52-proxy-blur 5d6352b;
  - feat/m53-uv-normal-corruption 4c93e5f;
  - master b5f15a3.
  If git reports "dubious ownership", pass -c safe.directory=* on that command only.
- Engine: UE 5.1.1 source at D:\UESource\UnrealEngine.

THE PLAN MUST COVER

A. Base and owner-view identity (ruling R11).
- Propose the integration base for the natural work, starting from release/2026-10-05 d7eee97:
  - which follow-up commits come in (the 091-05 fixes);
  - the conflicts you expect;
  - what must be re-proven.
- Owner-view identity for natural observations. Assess the saved 091-06 office-robust F1 work:
  - what it changes;
  - why the F1 guard on 5d6352b failed closed at the office;
  - whether the work in progress addresses that.
  Then recommend either:
  (a) building on an office-robust F1; or
  (b) the release's frame keying, under the documented one-live-game-view limitation.
  Give the risks of each, and the exact office readings (numbers only, typed by the owner) that would prove your choice.

B. The foundation (rulings R2, R4, R7, R8), file by file:
- The natural-attempt lifecycle: baseline → arm → observe → count or revert → recovery. It runs inside the existing auto-injection scheduler: one cause at a time, frequent, aimed at on-screen targets chosen from a clean, fresh census.
- Probes: render-thread mechanism-state readers keyed to the capture request. Build them for MISS-1 and MISS-2 now, shaped so BLUR-1 and GONE-1 can plug in later.
- The attempt-stage picture check: extend the m55 change evidence with clean-noise controls, a visible threshold and the ambiguous band of ruling R7.
- The label writer and run_summary: exactly the additive keys of ruling R8. Include an example annotation.json event and labels.jsonl entry for a natural MISS-2 event and for a controlled MISS-1 event.
- How controlled vs natural is chosen per anomaly type, on the console and in the dashboard. Anomaly type ids stay unchanged (ruling R2). Give the owner one or two UI shapes to choose from.
- Cause metadata on the shipped anomalies, per ruling R2: LOD-1, NRM-1, CAM-1, and GONE-3 for blinking mode 0 only.

C. MISS-1 (controlled R1 calibration): the engine default material swap, eligibility, probe, restore recipe RM, and gates.

D. MISS-2 (natural):
- the MID on the m53 route;
- how the colour parameter is chosen. For example: sRGB, not a normal map, effective in the compiled uniforms. The attempt lifecycle then decides visibility, so a wrong guess is reverted and not counted;
- packed or reused parameters, and host MIDs;
- VT slots are skipped;
- restore recipe RM, and gates.

E. Gates and predictions, following the house gate bar:
- a predictions file before the first leg;
- a can-fail lever per gate;
- PIE AND staged;
- the StackOBot bench fixture plus an office-like fixture;
- label and mask edges 0 frames off;
- no labelled frame without a measured mask;
- attempts vs successes counted;
- a no-effect control (a target already showing the replacement) that must not count;
- an exact-restore check;
- the m26 interplay.

F. The job split for Codex implementer jobs:
- each job fits one bench session;
- their order and effort estimates;
- what the owner must run at the office (numbers only), and when.

G. Any new module dependency (ruling R3), the risks, and open questions for chat or the owner.

DELIVERABLE
review.md in your run folder: a one-screen summary first, then A–G. End with one line "VERDICT: <summary>", for example "VERDICT: PLAN READY | base=<...> | identity=<a|b> | jobs=<n> | office readings=<...>".
