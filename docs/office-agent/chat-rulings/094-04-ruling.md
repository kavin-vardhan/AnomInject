# 094-04 — Chat ruling: the STOP was right; one narrow C: exception for 4.25's header tool; A3 resumes with the compile fixes; the owner's build cache is cleared first

**Date:** 2026-10-22, about 16:00 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-04-codex-report.md` (thread `01a14334`, Astra max, full access, 12:31–14:58 IST, 8,795 s, exit 0). Its verdict: `NEEDS-DECISION | A3 incomplete; 4.25 UHT passed but C++ failed; non-exempt UHT C: config writes triggered STOP; P9 and both engines unchanged; no 5.1 compile or push`.
- **Claude Code's review:** `_reviews\094-04-code-review.md`. 23 OK · 2 GAP (ED5, ED7) · 3 OPEN (ED2, EC5, EA8 for 5.1) · 2 STOP (one event) · 0 CONFLICT.
- **Mailbox report 094-04:** NEEDS-DECISION, 187 min, $21.03 API-equivalent.
- **The owner (about 15:35 IST):** "I have freed up 62GB more from my end. But there's again more than 350gb in the build cache folder. We need to clear that up. The only focus now is porting."

## Verdict
**The STOP was correct, and nothing protected changed.**
- P9 shows 0 differences: Codex's entry and exit proofs, plus Code's independent re-hash of 19,290 of 19,290 files.
- Both engine trees and the Paris original show 0 writes.
- The only port writes outside D: were the 12 UHT config deltas (zero net bytes) and the allowed 4.25 XmlConfigCache rewrite.

**What A3 delivered (source written, not yet compiling):**
- the shim and the five re-parented subsystems, matching C1 and the 5.1 engine class;
- the nine raw pointers;
- the Compat adapters, with each 5.1 branch the original expression verbatim;
- the 4.25 disabled units and facades;
- the capability policy;
- the C and E seams;
- EC1, ED5 and ED7, and the descriptor set with three mutants (reproduced by Code on all 21 exports).

**The first 4.25 Editor compile:**
- UHT passed. C++ failed: 9 × C2855 plus about 125 others, and 20 × C5043 warnings.
- 35 actions, 0 of them in the engine.
- No fix was attempted after the STOP, as the rules require.

`feat/europa-port` = `668c075` (compiled source `9fdc8f0`), local, no push.

## Rulings

### EE1. 4.25's header tool writes its own settings on C:: a narrow exception
**Why:**
- Stock 4.25 UHT, in an installed engine, writes its program settings under the per-user folder `%LOCALAPPDATA%\UnrealHeaderTool\Saved\`.
- UBT fixes UHT's command line, and the CrashReportClient config is written in the first second, before any argument merge. Routing it elsewhere would need an engine edit or a per-user setting, and both are forbidden.
- The ten ini files have existed since 2023-09-14, and the owner's own 4.25 build rewrote them on 10-05. This is the same kind as EA4's "UBT and UHT logs that cannot be redirected".

**The exception, added to EA4's C: list:**
- `%LOCALAPPDATA%\UnrealHeaderTool\Saved\Config\Windows\*.ini`, the ten files named in `stop\unauthorized-writes.json`;
- `%LOCALAPPDATA%\UnrealHeaderTool\Saved\Config\CrashReportClient\UE4CC-Windows-*\`, including UHT's replacement of the previous folder;
- `%LOCALAPPDATA%\UnrealHeaderTool\Saved\Logs\`, if UHT writes there.

**How it is held:**
- Per-operation path, size, mtime and hash evidence, inside the existing 250 MB C: budget.
- Anything else under `%LOCALAPPDATA%\UnrealHeaderTool\` is still a STOP.

**Drop the UHT `-UserDir` attempt** through `UE-CmdLineArgs` for build-mode children. It does not work, and a mechanism that does nothing must not appear in receipts as a redirect.

### EE2. The host Target.cs probe rooting is ratified (ED7 GAP)
**Why:** UBT builds a project module only when a target roots it. The export proved that a `.uproject` declaration alone does not. Rooting it through UBT's own `ModuleDescriptor.IsCompiledInConfiguration` keeps the `.uproject` Shipping exclusion in charge, and the mutants proved that.

**Conditions:**
- It is host-only, and never in the plugin or any delivered file.
- Baseline51 and ReGate51 carry identical edits.
- Apart from that block, each Target.cs stays byte-identical to its source. That keeps the 4.25 toolchain pins and the 5.1 `IncludeOrderVersion` exactly as they were.
- **The plan's "4.25 targets stay byte-identical" now reads:** "byte-identical apart from the probe-rooting block".

### EE3. C++17 against the 4.25 shared PCHs (C2855)
- **On 4.25 only,** give the five plugin modules a PCH policy that matches their C++17 standard: `NoSharedPCHs` with a private PCH per module, or no PCH. Pick one, and show it ends C2855.
- **5.1 is untouched.** The C# change sits in the 4.25 branch, and the full JsonExport set with all three mutants re-runs. 5.1 module objects must still equal release.
- **C++17 stays on 4.25** (EA1).

### EE4. The 20 C5043 warnings
**Why:**
- They come from 4.25's operator new/delete replacement macro, which IMPLEMENT_MODULE expands under C++17. They do not come from our logic.
- Europa's office build may treat warnings as errors, and a known engine-macro warning must not fail D-lite.

**The ruling:**
- **First apply EE3.** If C5043 persists, suppress warning 5043 only, only around the IMPLEMENT_MODULE line of each of our five module files, and only on 4.25 with MSVC (push, disable, pop).
- **Record the count before suppression** in the warnings record, with this cause.
- **No other warning is suppressed** without a ruling. Fix every other new port warning, including C4099, which will also fire on 5.1.

### EE5. On 5.1 the capability layer passes everything through (Code's risk 4)
**Why:**
- A hard-coded 5.1 list that gates `Register` would silently drop any future 5.1 anomaly missing from it.
- The new 5.1-visible differences (new messages for unknown IDs, and a `!UE_BUILD_SHIPPING` condition on the asset finders) are reach the port does not need.

**On 5.1:**
- `Register`, the registry and the Injector constructor's asset finders behave exactly as in `d7eee97`, with no new condition.
- Responses to unknown IDs are the release's (same path and same messages).
- The capability check admits whatever the registry registers.

**On 4.25:** the explicit allowlist stays (blinking, lighting_mismatch, lod_corruption, plus missing_object only with `-IAIBench`). Unknown or unported IDs fail closed with `unsupported_engine_phase_a`.

**Proof:** at diff level, the 5.1 tokens of these paths equal `d7eee97`, apart from the shim and raw-pointer mechanics. A9 will assert that the registry and catalogue equal release.

The empty `protected:` blocks left by the removed overrides may go.

### EE6. Tooling fixes before the next compile counts
1. **The environment receipt** records exactly what the child receives, using a case-insensitive key match on Windows.
2. **The diagnostics parser** takes the module from the `Source\<Module>\` path component.
3. **The A1/A2 budget can-fail suite runs again,** so its cumulative C: case is exercised (ED5 GAP).
4. **EE1's** removal of the UHT `-UserDir` attempt.

Each fix gets its self-test.

### EE7. The owner's build cache is cleared (owner's scope call), and P9 becomes v2
**The owner's call:** porting is the only focus, and `E:\IA_BuildCache` (369.76 GiB, 432,470 files, nothing written since 10-16) is cleared.

**What that means:**
- **J2 and the natural-cause research are shelved.** Their code stays in git (`fix/followup-capture`, `feat/m52-proxy-blur`, `feat/m53-uv-normal-corruption`, `release/2026-10-05` and every other ref), but their builds, banks and measurements are gone.
- **J2's Lyra host** (`E:\UEProjects\LyraStarterGame`) and **its D: leftovers** (`D:\IA_BankOverflow`, the J2 run folder) are not part of this request, and stay.

**What stays in `E:\IA_BuildCache`:**
- `_binary_baselines\release-2026-10-05-23098fa-218BA4C7`, the delivered release's binaries, kept for client support.
- Any folder in `StackOBot\Builds` named for the release (`release-2026-10-05`, `23098fa` or `d7eee97`). Keep it and report it.
- Every folder that a D: junction targets, emptied but kept, so no junction dangles: `StackOBot\Builds`, `StackOBot\Intermediate`, `StackOBot\Saved`, `_binary_baselines`, `_bench_sessions_bank`, and any other one Code finds.

**Everything else goes:**
- `_bench_sessions_bank`'s contents;
- the rest of `_binary_baselines`;
- `_r52p_src`, `_r53_host`, `_r53_selftest`, `_r53_src`, `_r917`, `_r9311`, `_r9312`, `_r9313`, `_r9314`, `_r933`, `_r936`, `_r937`, `_r938`, `_r939`;
- the contents of the main StackOBot's `Builds`, `Intermediate` and `Saved`.

**Who and how:** Claude Code does it, by receipt, before Codex starts.

1. **Inventory first:** every top-level child's size, file count and newest time; every reparse point and its target; and every git worktree inside. Those worktrees are `AnomalyInjector1` (`_r53_host`), `2` (`_r53_src`), `3` (`_r933`), `4` (`_r52p_src`) and `7` (`_r917`).
2. **Save any work before deleting:**
   - For each worktree, save `git status` and `git diff --binary` to the cleanup receipts if anything tracked is modified.
   - Zip untracked non-build source and docs if their total is under 50 MB; otherwise list them.
   - Check that every detached HEAD there is reachable from a ref. If it isn't, keep that worktree and report it.
   - Do not run `git worktree prune`. The registrations stay, and that keeps every commit reachable.
3. **Nothing may be running from the delete set** (process images and command lines). Never kill anything.
4. **Delete:**
   - remove reparse points as links first (`rmdir` without `/s`), checking their targets are untouched;
   - then delete real folders with `rd /s /q`;
   - never use PowerShell `Remove-Item -Recurse`, which can follow junctions.
5. **After:** an inventory of what is left, and free space.
6. **If Code's own safety controls refuse the delete,** Code does not work around them. It writes the exact list for the owner, and the job continues on P9 v1.

**P9 v2:**
- `D:\IA_Europa\Receipts\P9\baseline-v2\full-sha256.jsonl` = v1 minus every entry under a deleted root, with no re-hash.
- A `removed-manifest.jsonl` holds the removed lines verbatim, and a receipt gives counts, bytes, SHA-256s and the root list.
- The kept release baseline stays protected.
- Codex re-derives v2 from v1 and the manifest at entry, and runs its entry and exit proofs against v2.

## Next
- **094-05 is queued.** Code clears the cache and builds P9 v2. Then Codex resumes A3 from `668c075`:
  - EE3–EE6 first;
  - then the compile fixes inside C1–C5;
  - the 4.25 Editor Development compile to 0 errors;
  - then the 5.1 ReGate51 Editor Development compile to 0 errors, which closes ED2, EA8 for 5.1, and EC5 item 3.
  - Code reviews after Codex exits.
- **Owner:**
  - don't install the Visual Studio 17.14 update that is waiting (ED3);
  - don't delete anything in `E:\IA_BuildCache` yourself while a job runs.
