094-02 — IMPLEMENT Europa Phase A jobs A1 and A2, under chat's ruling 094-01. Stop at the end of A2. Requested by chat Claude (the orchestrator) on 2026-10-16.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING, in this order
1. _reviews\094-01-ruling.md (EA1–EA15). It amends your plan where they differ.
2. Your plan: _reviews\094-01-codex-europa-phase-a-plan.md, as amended by the ruling.
3. _reviews\094-00-europa-port-ruling.md (P1–P14).
Claude Code's _reviews\094-01-code-plan-review.md is background. Its findings bind you only where EA1–EA15 adopt them.

THE OWNER'S TARGET
Full parity (EA2): every delivered anomaly, capture, labels and masks, and the dashboard through a 4.25 control transport. Phase A's obligations are unchanged. Build the Compat seams so that phases C, E and F can add 4.25 backends without changing reflected declarations again. Say in your report where each later backend plugs in.

A1 — BASELINE AND SCOPED SETUP
1. Pre-flight:
   - free space on the resolved volumes (EA4 floors and warning margins);
   - P9 baseline under D:\IA_Europa\Receipts\P9\baseline.
   The P9 baseline (EA8):
   - full SHA-256 of every protected file;
   - the allowlist comes from 094-00 P9, _reviews\094-01-j2-pause-receipts\ and EA8's 5.1 engine tree;
   - no network, and error-fatal enumeration.
   Compare it with the 11:00 pause receipts. The post-pause state is the baseline: J2's installed candidate stays as it is, and the partial E: archive is gone (EA9). Any other difference is a STOP.
2. Create D:\IA_Europa after proving that the resolved path is on physical D:. Make three hosts there:
   - the 4.25 Paris copy, per plan B1;
   - the two StackOBot copies, D:\IA_Europa\Baseline51\StackOBot and D:\IA_Europa\ReGate51\StackOBot, per EA3. Copy only the .uproject, Config, Content and Source. Use /XJ, and check nested folders for the /XD names first.
   Take a manifest of each source before and after copying. The E: Paris original and the D: StackOBot original must be unchanged.
3. Branch and worktrees, addressing the common repository directly (EA14):
   - feat/europa-port from d7eee97 at D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector;
   - Baseline51's plugin detached at d7eee97;
   - ReGate51's plugin detached at feat/europa-port.
   CaptureBench goes into both 5.1 hosts by git archive, at the commit the release qualification used. If no receipt names one, use e64d6f9 and record it. Never use J2's installed copies. Disable any other plugin in the copies' .uproject unless a gate level provably needs it. No plugin may need an engine-tree build (EA8).
4. Routing (EA4):
   - project-scoped DDC graphs, one for 4.25 and one for each 5.1 copy;
   - the process-local launch and build wrappers (TEMP/TMP, -UserDir, -ShaderWorkingDir, -abslog, -Log=, uebp_*);
   - a write-ledger tool that inventories the EA4 exception paths before and after every operation and enforces the 250 MB (C:) and 50 MB (E:) budgets.
5. The port idle gate (EA6), in port tooling, with synthetic can-fail self-tests for every condition. It is not used in A1–A2: there are no runtime legs.
6. Commit docs/predictions/094-europa-phase-a.md on feat/europa-port before the first build. It covers every A and R gate, as amended:
   - EA5: the AA-off arbiter, paired TAA/TSR characterization, and the per-world tick denominator;
   - EA3: the StackOBot/CaptureBench fixtures for 5.1;
   - EA10: the job split.
   Later revisions are allowed only before the legs they affect (plan E8).

A2 — AVAILABILITY, BUILD RULES AND DESCRIPTOR
1. Exact-version C# rules for both engines. Every branch must compile in both UBTs (G233 / 070).
2. The shared version and capability header.
3. Legacy descriptor keys, and the two doc lines (EA12).
4. Minimal shader, bench and module stubs. The 4.25 inert shells (EA7).
5. The C++ standard policy: C++17 on 4.25 only.
6. The EA11 work:
   - the UHT-safe replacement of #if __has_include in all five reflected headers;
   - moving the control header's #if out of the reflected region;
   - the static check, with its mutant can-fail.
7. The descriptor matrix proof, level 1 (EA13): UBT -Mode=JsonExport on both engines through the approved wrappers.
   - 5.1: port against d7eee97, all valid target and configuration pairs.
   - 4.25: port against the expected set.
   - Plus the removed-blacklist mutant, which must change the matrix.
8. Do not start A3's shared C++ closure, beyond what A2 strictly needs. No full Editor or Game builds yet. Those are A4a and A4b.

STOP RULES (EA15)
Each of these is a STOP:
- a P9 difference;
- a floor breach;
- a write outside D: other than the EA4 exceptions, or one over budget;
- any engine-tree write (the 5.1 source engine, or the 4.25 install other than Engine\Intermediate\Build\BuildRules);
- any change to J2's state, m51, master, release/2026-10-05 or tags;
- any global or per-user setting change.
Also:
- D2: at most 2 attempts per harness problem.
- No editor or game launch.
- No push.
- Commits go on feat/europa-port only, as Conventional Commits, with GIT_AUTHOR_DATE and GIT_COMMITTER_DATE absent.
- Never kill a process you did not start, and never by name.
- Delete nothing outside your own new D:\IA_Europa scratch, and only by receipt.
- Codenames only: "Europa" and "Europa's fork".

DELIVERABLE
A report in your run folder. Put a one-screen summary first, then:
- the P9 baseline result and its comparison with the pause receipts;
- the host and worktree layout, with the copy manifests;
- the CaptureBench commit used and its source;
- the routing design, with the measured writes per operation (D: totals; C: and E: exception paths before and after);
- the idle-gate self-tests;
- the commits, with times, showing the predictions came before the first build;
- the A2 changes, file by file, each with its 5.1 effect class;
- the EA11 static-check result and its can-fail;
- the JsonExport matrices (5.1 port against release; 4.25), and the mutant result;
- where each later backend (C, E, F) plugs in;
- the plan for A3, and anything that changes it;
- any STOP or NEEDS-DECISION item.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
