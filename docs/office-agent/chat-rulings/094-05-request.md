094-05 — RESUME Europa Phase A job A3 from 668c075, under chat's ruling 094-04 (EE1–EE7). Stop at the end of job A3. Requested by chat Claude (the orchestrator) on 2026-10-22.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING, in this order
1. _reviews\094-04-ruling.md (EE1–EE7). It is new.
2. _reviews\094-04-request.md (ED1–ED7) and its ORDER, STOP RULES and DELIVERABLE, except where EE1–EE7 change them.
3. _reviews\094-03-ruling.md (EC1–EC6), _reviews\094-02-ruling.md (EB1–EB5), _reviews\094-01-ruling.md (EA1–EA15).
4. Your plan, sections C1–C5, plan §D2 and plan §H, as amended. Then _reviews\094-00-europa-port-ruling.md.
Background only: _reviews\094-04-code-review.md.

WHAT CHANGED SINCE YOUR LAST TURN
- Your STOP was correct. Your resume point in 094-04 §"Exact resume point" is accepted, as amended below.
- EE1: the 4.25 UHT settings writes under %LOCALAPPDATA%\UnrealHeaderTool\Saved\ are now an EA4 C: exception: Config\Windows\*.ini, Config\CrashReportClient\UE4CC-Windows-*\ (including the folder replacement), and Logs\. They are held by per-operation evidence inside the 250 MB C: budget. Anything else under %LOCALAPPDATA%\UnrealHeaderTool\ is still a STOP. Drop the UHT -UserDir attempt through UE-CmdLineArgs for build-mode children.
- EE2: the host Target.cs probe rooting is ratified. The plan's "4.25 targets stay byte-identical" now reads "byte-identical apart from the probe-rooting block". Baseline51 and ReGate51 stay identical.
- EE7: before launching you, Claude Code cleared E:\IA_BuildCache on the owner's instruction (J2 is shelved) and wrote P9 v2. Code's added context below says exactly what was removed and kept, and where v2 and its removal manifest are. If Code could not delete, v1 still applies; the added context says which.

ORDER
1. Entry
   - Re-derive P9 v2 from v1 (full-sha256.jsonl db884f1e…) and Code's removal manifest. They must match byte for byte. Any mismatch is a STOP.
   - Then the EA8 per-job entry proof against v2. The deleted roots are no longer protected because they no longer exist. Everything else is unchanged: m51, master, release/2026-10-05, tags, the remaining worktrees, both engines, the Paris original, J2's Lyra host and D:\IA_BankOverflow.
   - ED1 free space, and the ED3 toolchain recheck (14.38.33130; 14.16.27051 with SDK 10.0.17763.0).
2. Tooling first (EE6):
   - the environment receipt with case-insensitive keys;
   - the diagnostics module parser;
   - the revived A1/A2 budget can-fail suite;
   - removing the UHT -UserDir attempt.
   Each fix gets its self-test. Commit.
3. 4.25 PCH policy (EE3), in the 4.25 C# branch only. Then re-run the full JsonExport set: 5.1 port against release (8 pairs), 4.25 (5 pairs), and all three Shipping mutants. 5.1 module objects must equal release.
4. 5.1 pass-through (EE5):
   - On 5.1, Register, the registry, the constructor's asset finders, and the responses to unknown IDs behave exactly as in d7eee97.
   - The capability check admits whatever the registry registers. The explicit allowlist exists on 4.25 only.
   - Show the 5.1 tokens of these paths against d7eee97, at diff level.
   - The empty protected: blocks may go.
5. The compile fixes, inside C1–C5. Your 094-04 table is the starting list: disabled Capture helper guards and every common caller, the TFunction reset adapter, C4099, 4.25 LOD and skeletal access, the frustum construction, constants and optional-feature access.
6. Working compiles (ED2), each commit-bound:
   - 4.25 StylizedParisStreetEditor Win64 Development, to 0 errors. Apply EE4 only if C5043 survives EE3, and record its count before suppression.
   - Then 5.1 StackOBotEditor Win64 Development in ReGate51 (move its detached HEAD to the compiled SHA), to 0 errors, with -NoEngineChanges and 0 engine and engine-plugin actions. This closes EA8 for 5.1, and EC5 item 3.
   - You may run one early 5.1 compile if it helps surface 5.1-only errors sooner. The final 4.25 and 5.1 compiles must be on the same final SHA.
   - Fix every new port warning, or list it with its cause.
7. Exit
   - If C#, a descriptor or a .uproject changed after step 3, re-run step 3's set on the final source.
   - EA11 on the final source.
   - The EA8 exit proof against v2, and the ledger summary.
   - Commit, with W4.

STOP RULES
094-04's, with EE1's exception added. Also:
- a v2 re-derivation mismatch;
- any write under %LOCALAPPDATA%\UnrealHeaderTool\ outside EE1's classes;
- a fix that would need an engine edit, a reflected-declaration change outside your ledger, a new renderer dependency, or a 5.1 behaviour change beyond EE5.
The same compile error surviving two fix attempts stops the job. The 9-hour work limit, with a clean committed resume point and NEEDS-DECISION "A3 continues", still applies. No editor, game, commandlet, cook or UAT launch. No Game or Shipping build. No Baseline51 build. No push. No tags.

DELIVERABLE
As in 094-04-request.md, plus:
- the v2 re-derivation;
- each EE fix and its proof;
- the final compile pair (SHA, toolset lines, action counts, 0 errors, warnings with causes, the C5043 count before and after);
- the 5.1 engine and engine-plugin action counts;
- what A4a and A4b must know.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
