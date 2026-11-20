<!-- Collected by Claude Code (mailbox brief 094-10) on 2026-10-25 from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md (sha256 B5191954FDA6B7CD539698E8C0D3B41AD98110B5FA248970A15983C3084261D7, 13,185 B); thread 01a14334, gpt-6-astra / max, 2026-10-25T08:43:54 to 2026-10-25T09:00:43 IST, 1,008 s, exit 0, 1 try. Content below is Codex's report, unchanged. -->
094-10 — Europa Phase A A5 — STOP at environment hygiene

**A5 is incomplete. Two stop conditions occurred: the effective-environment reader exhausted D2's two harness attempts, and the subsequent closing ledger copied credential-like session identifiers into a new receipt.** The new identifier copies have been redacted in memory with before/after file hashes. No messaging-token match was found. The affected ledger is invalidated and must not be used as completed write-proof evidence.

- P9 v2 entry and exit PASS; protected refs, engines and original hosts unchanged.
- Existing qualified binaries remain hash-equal: 4.25 27/27, Baseline51 39/39, ReGate51 44/44.
- Feature HEAD remains `d30cbe03da39dabf69c8bda09b3dcdaec8314ae6`. All three plugin worktrees are clean. No source/host edits, commits, pushes, tags or pruning.
- No UBT, compiler, editor, game, commandlet, cook, UAT or ShaderCompileWorker launch.
- Historical EJ4 cleanup, runtime readiness, probe, readers, fixtures, A6 analysis and rebuilds remain pending.
- Evidence: `D:/IA_Europa/Receipts/09410`. Report: 2026-10-25T08:59:32+05:30.

**Entry and exit proofs**

V2 was re-derived byte for byte from v1 and the removal roots, at entry and exit. V2 remains `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`, 472,552 baseline lines. Each proof inventoried 472,553 retained files, accepted only the same 1,023 previously receipted EG1 bookkeeping differences, and fully hashed 23,033 selected binaries/configs (104,794,989,456 bytes). No new protected difference occurred. The full-file baseline hash was not repeated.

Protected refs, checkout heads/index hashes/status and original source manifests matched. `git fsck --connectivity-only` exited 0; dangling objects were reported without connectivity failure. The old interrupted ledger's read-only SQLite integrity check returned `ok`. The five stale worktree registrations were left intact. V2 handled the previously authorized cache removals; this job deleted nothing there.

Owned checkout heads remain Paris/feature `d30cbe0`, ReGate51 detached `1c4cce7`, Baseline51 detached `d7eee97`. Main m51 and other protected refs are unchanged. See `P9/entry/` and `P9/exit/` for inventories, selected-file hashes, git state, source manifests and summaries.

Resolved-volume free space in GiB:

| Reading | C: | D: | E: |
|---|---:|---:|---:|
| Initial direct reading | 18.13 | 85.05 | 259.91 |
| Entry completed | 18.13 | 84.80 | 259.91 |
| Stopped ledger before | 18.13 | 84.75 | 259.91 |
| Exit proof completed | 18.13 | 84.67 | 259.91 |
| Report | 18.13 | 84.67 | 259.91 |

Every reading passed ED1: start C/D/E 13/70/25 GiB; warning 11/55/20; floor 10/50/15. No warning or floor breach occurred.

ED3 verified VS 2022 17.12.35707.178, MSVC directory 14.38.33130, compiler product 14.38.33144 (file 19.38.33144.0), SDK 10.0.22621.0; and VS 2017 Build Tools 15.9.34031.82, directory 14.16.27023, product 14.16.27051, SDK 10.0.17763.0. The installed MSVC directory list is in `toolchains/installed.json`. No fresh UBT selection was obtained because builds were not reached. Existing A4a/A4b logs remain bound.

**EJ4: draft, failed reader and historical cleanup**

`environment09410.py` is a run-folder-only draft. It has a case-insensitive child allowlist, credential-name denial, masked receipts and an attempted makefile/action-export effective-environment reader. It was not installed in port tooling. Its allowlist was used only for read-only `vswhere` and the exit-protection child; `stop/exit-child.json` records names and masked values. That child's TEMP/TMP resolve to owned D: scratch.

Denied name classes: `*TOKEN*`, `*KEY*`, `*SECRET*`, `*PASSWORD*`, `*PASSWD*`, `*AUTH*`, `CLAUDE_*`, `ANTHROPIC_*`, `OPENAI_*`, `CODEX_*`; unknown names are dropped. W4 date variables were absent. No credential value is printed in this report or chat.

| Test | Result |
|---|---|
| Synthetic allowlist/masking/list-decoder cases | 14/14 PASS |
| Real decoder attempt 1: chain candidate length-prefixed pairs | FAIL: `UBT makefile environment list was not decoded` |
| Real decoder attempt 2: sequential strings within candidate counted lists | Same FAIL |
| Third decoder attempt | Not run: D2 stop |

Both real attempts read `D:/IA_Europa/Baseline51/StackOBot/Intermediate/Build/Win64/StackOBotEditor/Development/Makefile.bin` without changing it. Synthetic PASS does not qualify the actual reader. See `stop/harness-attempts.json` and `environment-selftest.json`.

The 11 historical affected files reported by Code remain untouched: eight regenerable makefiles and three action-export receipts. Those counts are Code's prescan observations, not a completed independent historical scan by this job. Historical receipts redacted: **0**. Makefiles deleted: **0**. Their hashes were not silently invalidated.

Prescan variable names: `CLAUDE_CODE_CHILD_SESSION`, `CLAUDE_CODE_ENTRYPOINT`, `CLAUDE_CODE_GIT_BASH_PATH`, `CLAUDE_CODE_MESSAGING_SOCKET`, `CLAUDE_CODE_MESSAGING_TOKEN`, `CLAUDE_CODE_SESSION_ATTENDED`, `CLAUDE_CODE_SESSION_ID`, `CLAUDE_PID`, `CODEX_CI`, `CODEX_MANAGED_BY_NPM`, `CODEX_MANAGED_PACKAGE_ROOT`, `CODEX_SESSION_ID`, `CODEX_THREAD_ID`, `CODEX_VERSION`. Values are omitted.

Source evidence: 5.1 `BuildMode.cs:768` saves the environment and `:788` restores cached variables; `ActionGraph.cs:937` exports the effective delta from the initial environment; `TargetMakefile.cs:474` serializes the list. `EpicGames.Core/BinaryArchiveWriter.cs:239` and `BinaryArchiveReader.cs:181` define the UTF-8 length-prefixed strings. The draft's mismatch remains unresolved. No engine defect is alleged.

**New-file credential-like identifier STOP and redaction**

After the D2 stop, I started a read-only closing inventory with the existing ledger. That was an error: EJ4 hygiene had not yet been integrated into its tooling-path inventories. A names-only scan found live values corresponding to `CLAUDE_CODE_SESSION_ID`, `CODEX_SESSION_ID` and `CODEX_THREAD_ID` in the new `ledger/current.sqlite`, through the inventoried tooling paths. These names are explicitly covered by EJ4 even though they are identifiers. No match was found for `CLAUDE_CODE_MESSAGING_TOKEN`. No value was printed.

I stopped the ledger, did not run its after-snapshot, and redacted both `current.sqlite` and its compressed initial copy. Each database was reconstructed in memory from sanitized rows, so no original-value backup or SQLite freelist residue was written. No protected file or old ledger was changed. The databases retain 824,958 inventory rows; affected values are replaced by named redaction markers.

| Redacted artifact | Before SHA-256 | After SHA-256 |
|---|---|---|
| current.sqlite | `d4367a7925eacda59c355cd63999d635ccff0310077b2dfa56939d168f1e63df` | `a32956f4fc42d2105ee883a9c5b0cc817f4c1eb04efc82be661e0d273f510cd2` |
| initial.sqlite.gz | `6a664952dd313c77ffd38234baf0f0f5d6ba89434c87160a8f7bb962d06f2304` | `bc7dbf555ea1ee5e346bce892cece09f7f1de74933544e23b25899618081568a` |

The full path/hash/name/count log is `stop/redaction-log.json`; the original names-only detection is `stop/credential-stop-scan.json`. `ledger/01-stop-verification/ABANDONED.json` invalidates the old snapshot digest and records the missing after-proof. Sanitized snapshots are retained for review, not qualification. This redaction does not erase the fact that the new-file STOP occurred.

The final check, `stop/new-file-value-scan.json`, scans all files this job created for current credential-like values of length at least 12, including binary evidence, without recording values or value hashes. Shorter values are not treated as arbitrary byte-substring credentials. The in-memory database redaction separately verifies that none of the checked values remains in the serialized databases. Historical cleanup is still pending.

**Runtime readiness, probe, readers and fixtures**

| A5 obligation | State |
|---|---|
| Runtime attribution classes and descendant-aware idle gate | Not implemented |
| Real observe-only collector for at least 15 minutes | Not run; no idle-clearance claim |
| Lifecycle observer, independent per-world denominator, positive preview sentinel | Not implemented; prior probe skeletons retained |
| Apply/revert/pause/travel/GC and acknowledged screenshot driver | Not implemented; no screenshots requested |
| Synthetic readers/can-fails for A3–A8 | Not run |
| Synthetic readers/can-fails for R3–R9 | Not run |
| Paris EA_Core/EA_Travel and paired CaptureBench recipes/manifests | Not authored; assets untouched |
| A6 package-header/source prediction table and isolation proposal | Not completed; no content-admission conclusion |
| Draft A6 owner card | Not produced; A6 is not ready |
| Dated prediction revision | Not made; frozen predictions unchanged |

One read-only memory sample found approximately 10.10 GiB free physical and 33.54 GiB free commit. It was neither EI2's three-poll admission nor the required runtime collector. No foreign process was stopped.

**Rebuilds, binary hashes and binding**

Build count **0**. No new compile commands, actions, warning results or binaries. Source, C#, descriptors and .uprojects were unchanged, so conditional matrices/mutants were not triggered. The clean-engine query and final EA11 were not reached; no new reflected header exists and no new qualification claim is made.

All 110 qualified files were re-hashed successfully. `stop/qualified-binaries-unchanged.json` contains every binary path, size, full SHA-256 and retained source binding. Existing manifest hashes:

| Manifest | Files matched | Manifest SHA-256 |
|---|---:|---|
| final-425-binaries.json | 27 | `a831dfa0fe63cea4af3823b6c88008f989dfc1f1e33c09d0bbaf2848eb9d5d65` |
| final-baseline51-binaries.json | 39 | `3d7a8827ac6f5dd4818081a1f08af7998dfe9b92e37cfcd133c5d66cd548e532` |
| final-regate51-binaries.json | 44 | `797bf35a8f9b25f3caa708fdacc1e8eeac0f5f42433a8f360b61c57c2fc533bb` |

No re-binding was performed. A6–A8 retain A4a's 27-file manifest; A9–A11 retain A4b's Baseline51 39 and ReGate51 44 until touched-target rebuilds explicitly replace them.

**Ledger limits and scope of evidence**

The only ledger operation was the abandoned read-only closing inventory. There is **no completed per-operation budget/write-proof result** for this job and no claimed zero C:/E: charge from an incomplete ledger. No new EG1 bookkeeping is admitted; P9 exit found none beyond the inherited set. Entry/exit P9 and the binary re-hash independently passed.

The recorded before tooling sizes are below. After sizes were deliberately not inventoried into another unsafe ledger after the STOP.

| Scope | Before bytes | Resolved location |
|---|---:|---|
| tool_claude | 10,861,889,417 | <USERPROFILE>\.claude |
| tool_codex | 28,041,055,401 | E:\CodexData\home |
| tool_temp | 5,084,463,054 | C:\ClaudeTemp |

Sanitized ledger size: 159,510,217 bytes, below 2 GiB. Nonfatal Windows/WER/tooling access denials remain documented in `before.json`; strict protected scopes had no enumeration error. Free space is only a floor monitor.

Writes were limited to new `Receipts/09410`, `Temp/09410/stop-verification` and run-folder scripts/report. No repository or host source changed. No commits, pushes, tags, binary archives, bench levers or global/per-user settings changes occurred. All inherited qualification manifests remain unchanged. The failed draft is not installed tooling.

**Exact resume point and A6 needs**

Chat must rule on the two failed parser attempts and the new identifier-copy STOP. Resume at A5 order item 2 after a fresh entry proof. Finish allowlisted child environments and the effective-environment proof, make every new receipt—including tooling inventory paths—safe before writing, redact historical receipts with before/after hashes, and delete only receipted regenerable makefiles. A fresh ledger is needed; the abandoned snapshot chain cannot be used as passing evidence.

One alternative for chat to assess is the controlled initial environment combined with native ActionGraph's effective environment delta, whose source semantics are known; no third decoder attempt was made here. Tooling inventory paths also need redaction before persistence, with a stable sanitized snapshot-hash policy that retains the required size/mtime evidence. This is proposed work, not an accepted ruling.

Then complete A5 items 3–10: runtime tooling/collector, paired probe and screenshot driver, reader can-fails, fixture recipes, content admission/owner card, touched-target builds and manifests, clean-engine query, predictions, conditional matrices, EA11 and exit proof. A6 still requires the ruled idle window, owner first-editor/preview leg, and a content-isolation ruling if analysis predicts startup load, blocking dialog or crash. No owner action or launch is requested now.

VERDICT: RED | A5 incomplete: D2 environment-reader limit and EJ4 session-identifier receipt STOP; new identifiers redacted, ledger invalidated, P9 v2 and qualified binaries unchanged
