094-04 — IMPLEMENT Europa Phase A job A3, the shared C++ closure, under chat's rulings 094-03 (EC1–EC6) and the new rulings ED1–ED7 below. Stop at the end of job A3. Requested by chat Claude (the orchestrator) on 2026-10-22.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING, in this order
1. This request, including ED1–ED7.
2. _reviews\094-03-ruling.md (EC1–EC6).
3. _reviews\094-02-ruling.md (EB1–EB5).
4. _reviews\094-01-ruling.md (EA1–EA15).
5. Your plan, _reviews\094-01-codex-europa-phase-a-plan.md: sections C1–C5, plan §D2 (the strict-include tool) and plan §H (the A3 row), as amended by the above. ("D2" alone, below, means the two-attempt harness rule.)
6. _reviews\094-00-europa-port-ruling.md (P1–P14).
Background only: _reviews\094-03-code-review.md. Its §7 risks bind you only where EC5 or ED1–ED7 adopt them.

NAMING
"Job A3" is this job. The prediction file's gates (A0–A8, R0–R9) are written here as "gate A1 (Build/toolchain)", "gate A2 (Strict)" and so on.

WHERE THINGS STAND (from 094-03; verify, don't assume)
- feat/europa-port = 4c6c1d2, local and unpushed. The tested A2 source is 2ee5cac. ReGate51's plugin worktree is detached at 2ee5cac, Baseline51's at d7eee97, and the Paris host's worktree is on feat/europa-port.
- No C++, UHT, editor, game or cook has run in the port yet.
- Development was paused from 10-16 to 10-22. The PC froze and was restarted on 10-18.
- Free space at 2026-10-22, about 12:00 IST: C: about 24–27 GiB, D: about 130 GiB, E: about 34 GiB. E: was about 102 GiB at your 094-03 exit. The port did not cause this; the owner's own projects live on E:.
- On 10-16 a Visual Studio update that includes MSVC 14.44 was downloading. Whether it installed, and whether 14.38.33130 survived, is unknown (ED3).

NEW RULINGS (chat, 2026-10-22)

ED1. Disk limits, re-based for E:
Why: the port's E: charge was 0 B over all 18 A1/A2 operations. Its only E: exception is the 4.25 BuildRules folder (about 4.3 MB). E: free space swings by tens of GiB with the owner's own projects, which the port cannot control, so a 50 GiB E: floor no longer protects anything the port could consume. A floor still matters: E: holds Codex's thread store (E:\CodexData\home, through the .codex junction) and the frozen J2 build cache (E:\IA_BuildCache).
- E: start at 25 GiB or more; warning at 20 GiB; floor 15 GiB. The port's E: budget stays 50 MB per job.
- D: start at 70 GiB or more (new, because A3 adds compile outputs and ledgers); warning at 55 GiB; floor 50 GiB.
- C: unchanged. Start at 13 GiB or more; warning at 11 GiB; floor 10 GiB; budget 250 MB per job.
- Below a warning: finish the current operation, measure, and continue only while every drive stays at or above its floor.
- A floor breach, for any reason, is still a STOP.
- All readings are on the resolved volumes.

ED2. Job A3 includes working compiles, Editor Development only
Why: "the closure compiles" can only be shown by the compiler. Your plan's A3 row is a "source/build session" that "may stop with compile blocker", and EA10 starts A4b's Game builds "as soon as A3's closure compiles".
- Allowed targets, and nothing else:
  - 4.25: StylizedParisStreetEditor Win64 Development, in D:\IA_Europa\StylizedParisStreet;
  - 5.1: StackOBotEditor Win64 Development, in D:\IA_Europa\ReGate51\StackOBot only, and only after ED3 passes and EC1 is proven.
- Run them through your wrapper, with a new build mode restricted to exactly these two pairs, as many incremental runs as the closure needs.
- Every run passes -NoEngineChanges, as your A2 JsonExport runs did, and shows from its action list that no action writes inside either engine tree.
- Every run records:
  - the commit SHA it compiled (commit first; for 5.1, move ReGate51's detached HEAD to that SHA);
  - the toolset, SDK and C++ standard lines;
  - action counts split into project, plugin and engine (engine must be 0);
  - errors, and warnings by module and file:line, with new port warnings marked;
  - its duration and its ledger operation.
- These are working results, not gate evidence. Gates A1 (Build/toolchain), A2 (Strict) and R1 still come from A4a/A4b's builds of the final source.
- Leave the outputs in the hosts' own Intermediate and Binaries on D:. A4 decides on cleaning.
- Not in A3:
  - Game or Shipping targets;
  - any Baseline51 build;
  - the strict-include run;
  - -Clean or -Rebuild;
  - Live Coding or hot reload;
  - any editor, game, commandlet, cook or UAT process.

ED3. Toolchain preflight, before any compile (EC5 item 4)
- 5.1: show that MSVC 14.38.33130 (cl.exe 14.38.33144) still exists where the A2 logs recorded it, and that a fresh 5.1 JsonExport still logs that selection. Record every installed MSVC directory and the Visual Studio version (vswhere, read-only).
- 4.25: show that 14.16.27051 (directory 14.16.27023) and SDK 10.0.17763.0 still exist, and that a fresh 4.25 JsonExport still selects them.
- Step 3's JsonExports may serve as the selection evidence.
- If the 5.1 selection is missing or different:
  - run no 5.1 compile at all, and use no fallback toolset. A different compiler would make UBT treat the engine as out of date;
  - finish the rest of A3, including the 4.25 compile;
  - end NEEDS-DECISION "5.1 toolset".
- Never install, repair or change a toolset, a Visual Studio component, global XML or the environment.

ED4. Engine trees in build mode
- 5.1 (D:\UESource\UnrealEngine):
  - A run that passes -NoEngineChanges, succeeds, and executes 0 engine compile or link actions may update UBT's own metadata caches under Engine\Intermediate\Build\: dependency and source-file caches, makefiles, action history, and UHT manifests and timestamps.
  - Record each changed path with its size and mtime. Up to 100 MB in total per job is allowed, and it is not a STOP.
  - Any other write in the 5.1 engine tree is still a STOP: Binaries, any plugin's Binaries or compiled outputs, Source, Content, Config, Shaders or Programs.
- 4.25 (E:\EpicGames\UE_4.25):
  - Unchanged: only Engine\Intermediate\Build\BuildRules\ may change.
  - Before the first 4.25 compile, find in the 4.25 UBT and UHT sources where an installed-engine compile writes its logs and caches, and show it with paths.
  - If any of them is inside the 4.25 install other than BuildRules, STOP before compiling.
- On C:, compile children (UBT, UHT, cl, link, mspdbsrv, vctip) are port children. Their writes in the EB1-scanned C: locations must fall under the EA4 exceptions and count against the C: budget. Any new exception class needs chat.

ED5. The ledger: drive-root folders (EC3), compile children, and size
- EC3: the C:, D: and E: drive-root scopes record top-level folders as well as files.
  - Can-fail: a synthetic inventory pair with one new root folder must STOP.
  - Never create a folder on C: or E: to test it.
- Compile children: replace the A1/A2 "no renderer or editor child" attribution rule for build-mode operations (ED4). Runtime attribution stays A5's work.
- Size: keep this job's ledger evidence on D: at 2 GiB or less. 094-03 used 6.4 GiB for 18 operations, and about 12 jobs remain in Phase A.
  - Keep the EB1 proof standard: one full inventory per scope per job, then per-operation deltas, each with its snapshot hash.
  - Extend the budget self-tests so that a delta-stored operation still catches a one-byte overrun, a new file in a scanned location, and a new root folder.
  - Leave earlier jobs' ledgers untouched.

ED6. The "Anomaly profile" build line (Code's 094-03 risk 3)
- Print it at most once per target per UBT run, not once per module, keeping its content (profile and toolchain).
- While in those C# files, match each file's existing indentation.
- Proof: the 5.1 JsonExport still equals release.

ED7. The host probe module: a skeleton now, behaviour in A5
- Add a minimal EuropaPhaseAProbe module to all three hosts: its module class and Build.cs, plus only the empty classes it needs to compile.
  - No behaviour and no shim-audit access.
  - No include of feat/europa-port's Compat headers, so it also compiles against d7eee97.
  - The two 5.1 hosts get identical sources.
- Declare it in each host's .uproject with the legacy key that excludes Shipping.
- Can-fail: on each engine, a mutant .uproject without the probe's Shipping exclusion must put the probe into the Shipping export. Restore it and re-export.

ORDER
1. Entry
   - The EA8 per-job entry proof against your full baseline: git state; protected inventories (path, size, mtime); full hashes of protected binaries and configs. Do not re-run the full hash.
   - Free space against ED1.
   - Port-owned items (feat/europa-port, the three port worktrees, D:\IA_Europa) are expected to change. Nothing else is.
2. The ED3 toolchain preflight.
3. The host set and descriptor proof, all before any compile:
   - EC1: each 5.1 copy's effective plugin set equals the original StackOBot's, except RoomGenerator and UnrealMCPython, which stay disabled and uncopied. Show the plugin-list diff against the original, and that every enabled engine plugin's binaries already exist in the engine tree.
   - ED7: the probe skeleton, and its declaration in all three hosts.
   - If a frozen prediction becomes stale (module totals, pair counts, reach), commit a dated addendum before the run that tests it.
   - Re-run JsonExport:
     - 5.1 port against release (8 pairs);
     - 4.25 (5 pairs);
     - the plugin Shipping-blacklist mutant on 5.1;
     - ED7's probe mutant on each engine.
4. The closure, on feat/europa-port (plan C1–C5, as amended by EA1, EA7 and EA11):
   a. The shim (C1) and the five re-parented subsystems.
      - ShouldCreateSubsystem calls Super first and returns Game or PIE only. The five DoesSupportWorldType overrides go.
      - 4.25: IsTickable returns !IsTemplate() && bInitialized. 5.1: IsAllowedToTick is kept exactly as the engine has it, and final.
      - One registration. No Always tick type on 4.25.
      - The tick audit as EA7 allows it.
   b. Raw UPROPERTY pointers for all nine properties (C2): null-initialized, names and specifiers kept, no weak-pointer substitution.
   c. Compat adapters and includes (C2): skinned-asset identity, static RenderData, camera POV and near clip, contact shadows, and the container helper only if needed.
   d. The 4.25 inert closure:
      - Capture's 18 implementation files (C2);
      - complete disabled types for every TUniquePtr destruction path, including the FVTableHelper constructors;
      - the control subsystem's disabled behaviour;
      - the C3 Injector files, compiled out on 4.25;
      - the AnomalyUnsupportedTexture facade, which never reports applied, ready or measured.
   e. Capability admission: one policy (AnomalyCapabilities) for the registry, the auto-injector and the selector.
      - 4.25 supports blinking, lighting_mismatch and lod_corruption, plus missing_object as bench-only (EA1, EA7).
      - On 4.25 the Injector's constructor loads no 5.1-authored plugin asset. Its asset finders sit behind the capability boundary, and the fields stay null.
      - 5.1 keeps its catalogue, its nine pool keys, its arrays, its draw order and its defaults unchanged.
      - An unsupported ID never consumes a successful draw. Unknown or unported requests fail closed.
   f. The 4.25 stock-no-Nanite component probe in the Injector module (C2). Do not set AllowNanite globally.
   g. EC4, the C and E seams:
      - List every reflected member or method of UAnomalyCaptureSubsystem, and of the texture facades' reflected users, whose type is engine-version-specific. Move any such type behind an opaque, non-reflected holder now.
      - Then declare a non-reflected capture-backend interface or factory (C) and a texture-adapter interface (E). Declarations only, with no 5.1 rewiring.
      - Each gets a one-line note naming the file where its 4.25 implementation will land in phase C or E.
   h. The ancestry audit (C5):
      - a source scan of all tracked source and tools, and of all three hosts: Cast, IsA, IsChildOf, StaticClass, GetDerivedClasses, GetSubsystemArray and class-path strings;
      - a byte-string scan of the .uasset and .umap files in the plugin Content and the hosts' Content for "TickableWorldSubsystem";
      - no asset loads. The asset-registry check waits for the first editor launch.
   i. EA11 on every reflected header, including the new shim header, plus its mutant.
   j. tools/strict_include_build.py adapted: explicit roots, UE4Editor, no 4.25 IncludeOrderVersion. It is not run until A4.
   k. EC2: move the spliced lines in docs/PRE-DELIVERY-CHECKLIST.md:291–292 and docs/architecture.md:26–27 after their sentences. And ED6.
5. Working compiles (ED2): 4.25 first, then 5.1 if ED3 passed.
   - Iterate to 0 errors.
   - Fix new port warnings where you can, and list the rest.
6. Exit
   - If any C#, descriptor or .uproject changed after step 3, re-run step 3's JsonExport set on the final source.
   - EA11 on the final source.
   - The EA8 exit proof, and the ledger summary.
   - Commit the results, with W4.

STOP RULES
- EA15, as amended by EB1, ED1 and ED4.
- A compile blocker: an error you cannot fix inside the C1–C5 design as amended. Examples: one that needs an engine edit, a reflected-declaration change outside your ledger, or a new renderer dependency. STOP, and report the exact error with the design change you propose.
- If the same compile error survives two fix attempts, stop and report it.
- New production-file reach that changes the design or a ruled behaviour goes to chat. Ordinary include fixes are recorded in the reach manifest.
- D2 for harness problems: at most 2 attempts each. A compiler out-of-memory failure (C1060, C1076, C3859) may be retried once at lower parallelism; that is attempt 1.
- Time: if A3 is not complete after about 9 hours of work, stop at a clean, committed point. Write the report with an exact resume point, and end NEEDS-DECISION "A3 continues".
- No editor, game, commandlet, cook or UAT launch. No Game or Shipping build. No push. No tags.
- Commits go on feat/europa-port only, as Conventional Commits, with GIT_AUTHOR_DATE and GIT_COMMITTER_DATE absent (W4).
- Never kill a process you did not start, and never by name.
- Delete nothing outside your own D:\IA_Europa scratch, and only by receipt.
- No security, global or per-user setting change. No ETW, no USN, no elevation.
- Codenames only: "Europa" and "Europa's fork".

DELIVERABLE
A report in your run folder (report.md). Put a one-screen summary first, then:
- the entry and exit proofs, and free space at each operation against ED1;
- the ED3 toolchain findings;
- EC1: the plugin-set diff, the engine-binary existence check, and the JsonExport matrices with all three mutants;
- the closure, file by file:
  - the actual change and reach manifest, joined to git diff --name-only d7eee97..HEAD;
  - each file with its 5.1 effect class (N, C or B) and its reason;
  - files the plan named that needed no edit, marked B;
- the shim contract table (C1), mapped to the code that implements each row;
- the nine raw pointers;
- the 4.25 disabled translation units and their guards, and the unsupported-texture facade;
- the capability policy on each engine;
- EC4: the reflected-surface list, the seams, and where each 4.25 backend lands;
- the ancestry audit results;
- the EA11 results and the mutant;
- every working compile (the ED2 fields), and the warnings list;
- the ED4 engine-tree and C: findings, per compile;
- the ledger changes (ED5) and their self-tests, and the per-operation EB1 results (charges, foreign writes, tooling overhead);
- the commits, with times;
- anything A4a or A4b must know, including the ED7 probe status and the strict tool's readiness;
- any STOP or NEEDS-DECISION item.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
