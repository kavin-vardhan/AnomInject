# 094-07 — Claude Code review of Codex's Europa A3 finish (W3: written after Codex exited)

**Date:** 2026-10-24, 15:30–16:15 IST · **Reviewer:** Claude Code (mailbox brief 094-07-europa-a3-finish.md)
**Job:** resume of thread `<uuid>` in `_relay\runs\2026-10-16-094-01-europa-phase-a-plan\`,
gpt-6-astra / max / danger-full-access, 13:46:29–15:29:32 IST, 6,183 s, exit 0, one try. Codex's verdict:
`GREEN | A3 shared closure complete; final 4.25/5.1 working pair and Baseline51 clean-engine query pass; P9 v2 checks pass; A4 gates pending; no push`.
Codex's report: `_reviews\094-07-codex-report.md` (report.md sha256 `239D83BF…`). My receipts:
`…\code-receipts-094-07\` (review\ subfolder) and `D:\IA_Europa\Receipts\CodeReview09407\`.

## Summary
- **A3 is complete, and I reproduce the evidence.** The final SHA is `204724a`. Both working compiles ran at `286f2ee`, and
  `286f2ee..204724a` changes only docs and tooling (Source, Content, Shaders, `.cs` and descriptors byte-identical). The
  final runs on `204724a` are up-to-date confirmations with 0 actions.
  - **5.1** `StackOBotEditor Win64 Development` (ReGate51): 29 actions, 9 project / 20 plugin / 0 engine / 0 engine-plugin,
    exit 0, 0 errors. The only warnings are two pre-existing host C4996s.
  - **4.25** `StylizedParisStreetEditor Win64 Development` (Paris host): 4 incremental actions (Injector recompile and link,
    plus the target), exit 0, 0 errors, 0 C++ warnings.
- **EG1 holds.** Every 5.1 engine-tree write in the job window is in EG1's class: 1,022 files, 1,913,446 B, written
  15:05:28–15:05:34 by the Baseline51 query. Nothing else in either engine changed. The Baseline51 query shows **0 engine
  and 0 engine-plugin outdated actions** (29 outdated, all inside Baseline51).
- **P9 v2 is unchanged and Codex re-derived it byte for byte.** My re-hash is 18,264/18,264 OK. The protected state at
  start and after exit is equal. The owner has not deleted yet.
- **Counts: 25 OK · 0 GAP · 4 OPEN · 0 STOP · 0 CONFLICT.** I have two non-blocking notes: (a) the 4.25 re-run was
  incremental, so most 4.25 DLLs are 10-22 outputs; (b) the relay script reused the ledger id `…-resume5`.

## 1. Rulings
| Ruling | Evidence | Status |
|---|---|---|
| EG1 bookkeeping class | My window scan (`review\engine-window-51.json`) covers 339,410 engine files, 13:46–15:30, and checks each hit against v2. 1,022 hits, 1,913,446 B; non-EG1 hits: 0. By class: `Engine\Intermediate\Build` has 414 LiveCodingInfo.json (all byte-equal to v2), 207 UHT Timestamp (82 with new content) and ActionHistory.bin (new content). `Engine\Plugins\**\Intermediate\Build` has 239 LiveCodingInfo.json (all equal to v2) and 161 Timestamp (72 new content). 0 binaries, generated C++, Source, Content, Config, Shaders or Programs. Engine files in v2: 339,409, 0 missing, 0 resized. The only engine file not in v2 is `UbtRuns\42980_aaa272f4…` (accepted in place). UbtRuns has 17 markers before and after, so every UBT run removed its own marker. Codex's per-op EG1 charges are 1,167,215 + 550,738 + 1,913,446 = 3,631,399 B, under 100 MB; its last op equals my final-state bytes exactly. | OK |
| EG1 clean-engine query | `builds\baseline-clean-engine`: Baseline51 at `d7eee97`, `StackOBotEditor Win64 Development -NoEngineChanges -WriteOutdatedActions` plus the usual flags, query_only. I read `outdated-actions.json` myself: 29 actions, and every ProducedItem is under `D:\IA_Europa\Baseline51\StackOBot` (engine 0, engine-plugin 0). Its other writes: 89 new files in Baseline51 Intermediate (UHT outputs, makefile, response files; no .obj or .dll) and the EG1 bookkeeping above. | OK |
| EG2 receipts | Every build `result.json` has `binary_hashes` (425-01: 2 produced; 51-01: 16), a hashed `host_binary_inventory` (14 / 16), and `cpp_standard_responses` (3 / 28). The final runs hold `retained_compile_responses` (9 / 12). I hashed all 30 final binaries in Codex's table and they are equal. Flags, read from the response files myself: 4.25 has 9 plugin compile responses, all `/std:c++17`, with no `/Yc`, `/Yu` or `/Fp`; 5.1 has 6 plugin compile responses, all `/std:c++17`, with `/Yu`+`/Fp` on SharedPCH Engine (4) and UnrealEd (2). | OK |
| EG2 4.25 re-run on the final SHA | Ran: `07-425-working` at `286f2ee` (4 actions) and `10-425-final` at `204724a` (up to date). **Note:** it was incremental. Only `UE4Editor-AnomalyInjector.dll/.pdb` are from this job (14:46:29). The Bench, Capture, ControlServer and Shaders DLL/PDB pairs date from 10-22 16:57–16:58 (the run with no binary hashes), and the two project DLL pairs from 10-22 14:11. All are now hashed, and UBT judged them current. A clean rebuild needed a cleaning authorization the job did not have (ED2: A4 decides; EF2's cleaning is ReGate51-only). | OK (note for A4) |
| EG3 `tooling_self_tests.py` | `--output` is required and must be under the owned D: root. My bare run exits 2 and the `Receipts`/`Temp` folder trees are unchanged (`CodeReview09407\eg3-bare-run.txt`). With `--output`: 31/31 PASS. | OK |
| EG4 empty folders | Done by me before launch: 12/12 verified empty and removed non-recursively, deepest first (`code-receipts-094-07\eg4.txt`). | OK |
| EG5 P9 v2 | See §2. | OK |
| EG5 protected state | See §3: equal at start and after exit. | OK |
| EG6 order | Codex's ledger op order: tooling and commit (`c0dc592`), EF2 cleaning plus include fixes (`286f2ee`), 5.1 working, 4.25 working, final source (`59fbf11`, `204724a`), 5.1 final, 4.25 final, Baseline51 query, exit. The first launch harness failed before UBT (a renamed import) and was fixed on the second attempt (disclosed). | OK |
| EF1 memory gate | Memory logs for all 9 UBT invocations (`ledger\*\child-*-memory.jsonl`). Each was admitted only after 3 consecutive passing 20-s polls (lowest admission 7.03 / 33.20 GiB). The 5.1 wait took 24 polls while the owner's Blender ran; the 4.25 wait took 18 polls, with free physical down to 0.56 GiB while waiting. In-run sampling had a 5.0–5.1 s median interval; the 5.1 compile's minimum was 2.06 GiB physical / 24.49 GiB commit, above the 1 / 4 GiB stop line. 0 stops. `MaxParallelActions=1` on every command. Self-tests re-run by me: `finish_self_tests.py` 82/82 PASS, including one-byte-below thresholds, single-low no-stop, second-low stop for physical and commit, and the wait limit returning "no memory window", all on a synthetic sampler (no real starvation or kill). | OK |
| EF2 recovery checks | Codex: EA8 entry against v2 PASS, `git fsck --connectivity-only` exit 0, interrupted ledger `integrity_check` ok. The 4.25 fallback is in the EG2 row. | OK |
| EF2 cleaning | `recovery\EF2-cleaning-result.json`: 100 files, 66,986,117 B, all under `D:\IA_Europa\ReGate51\StackOBot` (host, AnomalyInjector and CaptureBench Intermediate, plus host BuildRules). Each was hashed before deletion. Mtimes are 10-22 16:08–17:10. 0 Source, Config or Content. Basis: the saved 09405 action plan (sha256 `4ded42fd…`). | OK |
| EF3 / EE7 owner delete | Not done yet: my inventories at 13:41 and 15:31 equal 094-05's before-inventory. Under EG5 it no longer blocks anything. | OPEN (owner, any time) |
| ED1 disk | Lowest seen: C 18.27, D 136.83, E 53.43 GiB; never near a warning. | OK |
| ED2 working compiles | See the summary. Every UBT command line (19 found, 7 distinct) carries `-NoEngineChanges -NoBuildUHT -NoHotReload -NoXGE -MaxParallelActions=1`. The 5 action lists hold 0 engine and 0 engine-plugin actions (my classification by ProducedItems). | OK |
| ED3 toolchain | Unchanged: VS 2022 17.12.35707.178; MSVC 14.36 / 14.38 / 14.42 and 14.16; SDK 17763 + 22621; no 14.44. Logs: 4.25 "VS2017 14.16.27051 … 10.0.17763.0"; 5.1 "VS2022 14.38.33144 (…14.38.33130) and 10.0.22621.0". | OK |
| ED4 / EG1 4.25 install | My full 4.25 listing (179,503 lines incl. BuildRules) is equal before and after. The Paris original (hashed, 1,512 lines) is equal. | OK |
| EE1 UHT C: classes | `%LOCALAPPDATA%\UnrealHeaderTool` and `UnrealBuildTool` are equal before and after (mine). | OK |
| EE3 4.25 PCH policy | NoPCHs in effect (no PCH flags in the 4.25 responses); 0 C2855 / C5043. | OK |
| EE5 5.1 pass-through | Source diff `5bd368b..204724a` is only the 8 include moves (§5). 5.1 runtime parity is still A9's. | OK (diff level) |
| EB1 routing | C: only `XmlConfigCache-E+EpicGames+UE_4.25.bin` changed in my snapshot (190 B, same size). Codex charges it, plus Windows/WER files conservatively under EA4: C 53,248 B, E 0. Equal: ProgramData\Epic, UE TEMP entries, D: root, staging/host binaries (Lyra), StackOBot uproject/Config/Source/Content. Root mtimes moved only for `C:\ClaudeTemp` and the owner's `E:\<personal-folder>` (foreign). | OK |
| EA8 | Codex: entry and exit each 23,033 protected binary/config hashes; inventories 472,553 files = v2's 472,552 + the 42980 marker (confirmed by me); 1,023 accepted EG1 differences. Codex's first entry scan missed 8 RoomGenerator assets that v1 reached through the `_r53_host` junction; the retry added the D: target. I confirmed those 8 lines are in v2 and hash-equal now. | OK |
| EA11 headers | Mine at `204724a`: 7 reflected / 0 violations in both the Paris and ReGate51 roots; the mutant FAILs (line 13 `#if ANOMALY_CAPTURE`). | OK |
| Ancestry | Mine: 749 text files, 57 `TickableWorldSubsystem` name hits, 0 ancestry lines on subsystem types; 6,368 assets, 0 hits. | OK |
| EA14 commits / W4 | 4 local commits, `c0dc592`, `286f2ee`, `59fbf11`, `204724a`; author time = committer time on each; `feat/europa-port` absent from origin; refs 72 → 72 with only `feat/europa-port` moved. | OK |
| JsonExport (conditional) | No `.cs`, `.uplugin` or `.uproject` changed in `5bd368b..204724a`, so the full re-run and mutants were not triggered and I did not re-run them. Codex re-ran the two Editor Development exports. | OK (not triggered) |
| Ledger row | The relay script appended `094-01-europa-phase-a-plan-resume5` (exit 0, GREEN). That id **repeats 094-06's row**: the script counts `exit-code-attemptN.txt`, and 094-05's crash left none. The script's quirk, not Codex's. | OPEN (relay tooling, cosmetic) |
| Strict tool | `tools/strict_include_build.py`: EE1 env override removed, both UBT calls under EF1, synthetic wiring only. Its EF1 stop exits instead of clean-and-retry (Codex says so). | OPEN (A4) |
| Runtime parity / A5–A9 | Not in A3's scope. | OPEN (A4/A5/A9) |

## 2. P9 v2
- **Mine** (13:42, `code-receipts-094-07\p9v2\make_p9_v2.py`):
  - v2 has 472,552 lines, `8e3ed095…2c74`; removed has 235,223 lines, `dd970a07…d063`; the receipt is `47e1b31d…`.
  - All six checks are true: lines, file bytes and size fields add up; sorted merge equals v1; the in-order interleave
    reproduces v1; the release baseline's 8 lines are kept.
  - Roots are the physical E: paths from the owner's list. v1 has 0 lines under any D: alias or step-1 link path.
- **Codex's re-derivation:** identical lines, bytes and SHA-256s for all three files.
- **My re-hash against v2** (seed 9407): 18,244 protected exe/dll/pak/ucas/utoc plus 20 random v2 files = 18,264 OK,
  0 mismatches, 17.1 GB, 61 s.
- **Worktree heads and index hashes:** equal before and after, except Paris and ReGate51, which moved to `204724a`
  (expected). Both are clean. Baseline51 is at `d7eee97`, clean. `m51` is at `53bf725` with 0 tracked changes.
- **Not covered by P9:** `E:\IA_BuildCache\StackOBot\Binaries` (keep list) has 0 lines in v1 or v2; it was never a P9
  root. My own SHA-256 listing (27 files) is equal before and after.

## 3. Protected state (EG5)
- **At start (13:40):**
  - release baseline 8/8 size- and hash-equal;
  - all six D: junctions resolve;
  - `D:\IntrusiveAnomalies\StackOBot\Content` byte-identical to its 094-05 snapshot (2,744 lines);
  - `D:\IA_BankOverflow`: 0 missing and 0 changed against v1 (89,583 files). The one extra file,
    `09102-final-binaries.zip` (2026-10-03), is pre-v1 and outside v1's roots.
- **After exit (15:30–15:33):** the same results on every item.
- **Cache:** inventories at 13:41 and 15:31 are equal to each other and to 094-05's (432,470 files, 41 groups, 3
  reparse points).
- **No RED condition.**

## 4. Engine trees
- **5.1:**
  - EG1 row above;
  - my snapshot pairs show `Engine\Binaries\Win64` (1,608), every engine plugin's Binaries (2,469), BuildRules,
    Programs\UnrealBuildTool and Engine\Saved all equal;
  - `Engine\Intermediate\Build`: 622 changed, 0 added, 0 gone;
  - plugin `Intermediate\Build`: 400 changed of 44,313;
  - these are the same paths as the inherited set.
- **4.25:** 0 changes, BuildRules included.
- **UBT logs:** `-NoEngineChanges` in every command; 0 engine and 0 engine-plugin actions in every action list.

## 5. The closure (`git diff --stat 5bd368b..feat/europa-port`: 21 files, +801/−126)
- **Source: 8 files, each a one-line move.** The own header moves to line 1, ahead of `#include
  "Compat/AnomalyEngineVersion.h"` and the outer `#if ANOMALY_ENGINE_51`, which stays. This is EG6 item 3 exactly.
- **C3:**
  - Three own headers guard themselves on `ANOMALY_ENGINE_51`: CameraClipping, StuckLowMip and TexCorrupt.
  - Five are unguarded and are now parsed in the 4.25 TU: ChangeCase, CorruptedTexture, LodPopping, MissingTexture and
    TimeDilation. They include only `CoreMinimal.h` / `IAnomaly.h`, plus ChangeCase's `Anomaly_CorruptedTexture.h`.
  - The 4.25 unity compile of `Module.AnomalyInjector.cpp` after the move has 0 errors, so they are portable today.
- **Everything else:**
  - docs: CLAUDE.md status block, closure, reach, predictions addendum, session journal;
  - tooling: `build_memory.py`, `finish_self_tests.py`, `recovery_cleanup.py`, `working_build.py`, `write_ledger.py`,
    `tooling_self_tests.py`, `strict_include_build.py`.
- **No comment lines were added** in any changed `.py` or C++ file (repo invariant).
- **Codename check:** the CLAUDE.md block uses "Europa" only.

## 6. Working compiles
- See §1 ED2 / EG2.
- **Binary hashes:** 30/30 equal to Codex's table (mine).
- **Warnings:**
  - 4.25: only the pre-existing user `BuildConfiguration.xml(16)`/`(22)` schema lines.
  - 5.1: C4996 `SetMasterPoseComponent` at `MidReproActor.cpp:110/153`. That file is byte-identical in ReGate51,
    Baseline51 and the original StackOBot (sha256 `8FD56169…`), so it is host code, not port code.
  - New port warnings: 0.

## 7. Self-tests (mine, all with `--output` under `D:\IA_Europa\Receipts\CodeReview09407\`)
- `tooling_self_tests.py`: 31 PASS.
- `finish_self_tests.py` (EF1/EG1/EG2): 82 PASS.
- `ledger_delta_self_tests.py`: 9 PASS. `ledger_budget_self_tests.py`: 5 PASS.
- `resume_self_tests.py`: 16 PASS. `idle_gate.py --self-test`: 45 PASS.
- I ran all of them with `python -B`, and the port worktree stayed at 0 status lines afterwards.

## 8. My own actions in 094-07 (all inside the brief's allowed list)
- **Deletions:** EG4's 12 empty folders, and nothing else.
- **Writes:**
  - `code-receipts-094-07\`;
  - `D:\IA_Europa\Receipts\P9\baseline-v2\` (3 files);
  - `resume-prompt-094-07.txt` (17,676 B, sha256 `357B21B2…`, no BOM, request verbatim prefix);
  - `_reviews\094-07-codex-report.md`;
  - this file;
  - `D:\IA_Europa\Receipts\CodeReview09407\`.
- **Launch:** detached, W4 applied (the child saw neither date variable). I did not steer Codex and killed nothing.

## 9. Top 5 risks for A4a and A4b
1. **Memory.** The owner's Blender took free physical to 0.56–0.60 GiB during this job's waits, and the 5.1 Editor
   compile itself fell to 2.06 GiB. Monolithic Game/Shipping links and the strict pass are heavier and longer, so expect
   EF1 waits and possibly in-run stops. A 4-hour wait limit could end a job with "no memory window" if renders run.
2. **EG1's 100 MB cap against invocation count.** Every 5.1 UBT invocation, a query included, rewrites ~1–1.9 MB of
   engine bookkeeping (1,022 files). A4b's baseline + candidate Editor/Game/Shipping builds plus a strict pass that calls
   UBT per header could reach 50+ invocations, so the cumulative charge needs a budget plan up front.
3. **4.25 provenance.** Most final 4.25 binaries are incremental reuses of 10-22 outputs (§1, EG2 row). A4a's final
   4.25 Editor/Game/Shipping builds should be clean builds, with cleaning authorized explicitly, so every product is
   produced and hashed in one receipted run.
4. **The strict tool has never run for real.** Its EF1 stop exits rather than clean-and-retry. Its restoration,
   per-action receipts and header-TU proof are unexercised, and UE 5.1 IWYU/first-header diagnostics are what blocked
   A3 for two jobs.
5. **C3 and EE5 fragility.** Five own headers are now unguarded on 4.25, so any future UE5-only type added to them
   breaks the 4.25 TU. The 5.1 bodies still sit behind function-like macros that evaluate to 0 silently if an include is
   missed. A9 must assert the registry, catalogue and asset finders at runtime on 5.1. Also, once the owner deletes
   `_r53_host`, the RoomGenerator D: target must stay an explicit P9 root (Codex's retry added it).

VERDICT: REVIEW DONE — 25 OK / 0 GAP / 4 OPEN (owner delete; relay ledger id repeats resume5; strict tool for A4; runtime parity A4–A9) / 0 STOP / 0 CONFLICT; A3 complete at 204724a (5.1 29 actions and 4.25 4 incremental actions at 286f2ee, 0 errors, 0 engine actions; both up to date at 204724a); EG1 1,022 files / 1.91 MB only; Baseline51 query 0 engine actions; P9 v2 unchanged (Codex byte-equal; mine 18,264/18,264); protected state equal at start and end
