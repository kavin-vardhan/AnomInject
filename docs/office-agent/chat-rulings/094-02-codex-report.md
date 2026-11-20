<!-- Collected by Claude Code (brief 094-02): run folder _relay\runs\2026-10-16-094-01-europa-phase-a-plan (resume of thread <uuid>) | model gpt-6-astra, effort max, sandbox danger-full-access | 2026-10-16 13:28:38-14:48:36 IST, duration 4,798 s | exit 0 | source report.md sha256 AC31230BF78ED0F82473D58085DD84695898A4CFADCB0A38537A041F642CB159, content unedited below -->
# 094-02 — Europa Phase A, A1/A2 execution report

**NEEDS-DECISION: A1 stopped at write-proof readiness; A2 was not executed. Chat review REQUIRED before resuming with a different observation mechanism or changing the proof.**

- The pause comparison passed: **23 inventory receipts (22 present, one expected absent), 139 freshly computed full key hashes checked against the pause receipts, protected refs and worktree state; zero differences**. The older receipts contain eight-character prefixes for most key hashes; the new receipts retain full SHA-256 values. This does not retroactively give the older receipts full-hash strength.
- Full P9 baseline: **PASS — 707,775 files / 582,901,644,507 bytes**. Exit protection: **PASS — matching inventories and 25,866 full binary/config hashes**.
- Windows denied kernel-file tracing and journal reads. A Windows exception-directory inventory had **51 access errors**. I cannot certify the requested exception totals, attribution or write budgets from those observations. I did not substitute a partial inventory or drive-free-space delta for that proof.
- Deliberate writes were new receipts and draft tooling on D:. **No host copies, worktrees, branch, commits, UBT/UHT invocation, C++ build, cook, editor/game launch, binary install, push or deletion.** J2's installed candidate was not restored or otherwise deliberately changed.
- Draft checks: **38/38 idle-gate synthetic cases passed**; the reflected-header forbidden-directive mutant was detected. These are draft-tool results, not completed A1/A2 qualification.

## P9 baseline and comparison

The new root was created at **2026-10-16 13:36:18 IST**, after resolving its parent to `D:\`, checking fixed-drive type and absence of a reparse point, verifying that the destination did not exist, and checking floors. Both date environment variables were absent. Receipt: `D:\IA_Europa\Receipts\P9\baseline\creation.json`.

The adapted pause checker used only local git reads with optional index locking disabled. It did not execute the original receipt script, query a remote, or suppress enumeration failures. It compared the explicitly recorded trees, refs/heads, tracked status, host/staging/config key hashes, archive-index hash and the J2 run-file key hash. `pause-comparison.json`, `pause-key-hashes.json` and `git-state.json` record the result. The partial E: archive remains absent as EA9 requires; neither installed host was treated as a release baseline.

The full baseline allowlist includes the protected worktrees and host trees, banks/archives, named evidence/run folders, and the 5.1 engine tree, including engine and engine-plugin binaries. Existing links are recorded and their protected subtrees resolved; unrelated siblings are not traversed. Common git object/worktree administration is excluded from the main checkout's file walk because authorized future port work must add objects and worktrees; protected refs, indexes and status are recorded separately.

The first full walker refused the existing `_r53_host` RoomGenerator content junction because its physical target was not separately listed. That was a harness scope error, not a detected state difference. One correction included the named host's mounted subtree, with its target recorded. The second run is the full-baseline attempt reported here; the first error receipt is retained.

The completed full baseline contains **707,775 files / 582,901,644,507 bytes (542.869 GiB)**. It finished at **2026-10-16 14:37:56 IST** after **2957.739 seconds**. `full-summary.json` reports PASS. The SHA-256 of `full-sha256.jsonl` is `db884f1e9c3debd7b9ed678db08036509959b1fc5a0d27799b10ecccb0625cce`. Enumeration and hashing errors were fatal, and each file's size/mtime was checked across hashing.

The exit check finished at **2026-10-16 14:45:27 IST**. All **707,775** protected paths/sizes/mtimes/file identities match. It re-hashed **25,866 binary/configuration files / 119,771,817,645 bytes**, including the required 5.1 engine binary trees: **zero differences**. The repeated pause comparison, protected refs/heads, indexes and full worktree status also match. Receipts: `D:\IA_Europa\Receipts\P9\exit\exit-summary.json`, `inventory-comparison.json`, `binary-config-sha256.jsonl`, and `git-state.json`. This is the EA8 intermediate-job exit proof; the final Phase A job still requires another full hash of every protected file.

## Hosts, worktrees and copy manifests

These requested destinations **were not created**:

| Role | Planned host | Planned plugin state |
|---|---|---|
| 4.25 | `D:\IA_Europa\StylizedParisStreet` | `Plugins\AnomalyInjector`, `feat/europa-port` from d7eee97 |
| 5.1 control | `D:\IA_Europa\Baseline51\StackOBot` | `Plugins\AnomalyInjector`, detached d7eee97 |
| 5.1 candidate | `D:\IA_Europa\ReGate51\StackOBot` | `Plugins\AnomalyInjector`, detached frozen port SHA |

Consequently there are no copy manifests or post-copy equality claims. Read-only source measurements found StackOBot Content **2,743 files / 2,167,654,828 bytes**, Config **5 / 19,182**, Source **7 / 11,756**. Paris Content is **1,453 / 2,960,507,507**, Config **18 / 36,168**, Source **5 / 1,603**. No nested exclusion-name directories or links were found in those three source subtrees. An external-actor hash containing `093` was merely a filename substring, not proof of a J2 fixture. A complete copied-content/J2-fixture assessment remains pending with the copies.

CaptureBench's selected future archive commit is **e64d6f9a43b7aa2e5eb041b7dab14e5285bb911b**. The release tree names `e64d6f9` in `docs/sessions/2026-09-30-089-02b-m53-rebench-prep.md:76`, describing the fixture correction and readback. Thus this selection has a source receipt; it is not an undocumented fallback. No archive or installed J2 copy was used.

## Routing, measured writes and the blocking condition

No host DDC graph or launch/build wrapper was installed. The accepted design remains three separate project DDC graphs, physical-D output roots, child-process TEMP/TMP, runtime `-UserDir`/`-ShaderWorkingDir`/`-abslog`, UBT `-Log=`, and UAT `uebp_*` folders. Global XML, registry, user/machine environment and engine settings were not edited.

The write-observation probes returned:

1. `logman start Europa09402TraceProbe -ets -o D:\IA_Europa\Receipts\trace-probe.etl -p Microsoft-Windows-Kernel-File 0x1eb0 4`: exit 1, **“Access is denied. Try running this command as an administrator.”** The command nevertheless allocated a session. I stopped this owned session successfully and a subsequent query reported **“Data Collector Set was not found.”** Its retained ETL is 8,192 bytes and is not used as a valid write trace.
2. The native journal prototype failed with **WinError 1, Incorrect function**. The system utility confirmed the access limitation: `fsutil usn readjournal D: csv startusn=0x00000003338d37d0` returned exit 1, **“Error 5: Access is denied.”** Journal-position queries worked; they do not identify the writes needed by the ledger. No journal was created or reconfigured.

No further observer attempt was made. A diagnostic Windows walk counted **197,129 readable files / 52,192,542,582 bytes**, but encountered **51 access errors**, including Prefetch and crash-related directories. That number is an **incomplete observation**, not the Windows exception-path total. Receipt: `D:\IA_Europa\Receipts\write-observation-block.json`.

ETW and USN were my chosen observation mechanisms; EA4 does not mandate those APIs. Their failure does not establish that every possible mechanism needs elevation. It establishes that the two attempted designs did not provide the required proof, and another design was not attempted under D2. The USN utility call was a diagnostic confirmation of the second design's access problem, not a successful ledger run.

Resolved-volume free space, GiB (physical drive roots):

| Volume | Entry | Final measurement | Floor | Warning |
|---|---:|---:|---:|---:|
| C: | 12.844 | 10.276 | 10 | 11 |
| D: | 159.448 | 158.959 | 50 | 55 |
| E: | 56.584 | 54.292 | 50 | 55 |

No sampled floor breach occurred. C: and E: finished below their warning margins. These are whole-volume observations, not port-attributed writes; in particular the C: free-space reduction is larger than the exception budget and its cause is unverified. A later job needs a fresh measurement before proceeding.

Retained owned artifacts at the final measurement (logical file lengths; **not** cumulative or per-operation write traffic):

| Artifact group | Bytes retained |
|---|---:|
| P9 baseline receipts, including full inventories/hashes | 462,045,942 |
| P9 exit receipts, including repeated pause comparison | 63,236,763 |
| Other D: scratch receipts and failed trace | 14,386 |
| **D:\IA_Europa total, 63 files** | **525,297,091** |
| Run-folder tools/drafts/results, 13 files | 114,168 |

`D:\IA_Europa\Receipts\final-artifacts.json` records full hashes and the measurement scope. It excludes itself, the final report written next, and Relay-owned log files. All deliberate output paths were on D:. **Per-operation D: traffic and complete C:/E: exception-path before/after totals are unavailable**, so these artifact sizes do not certify the requested write ledger.

Complete C:/E: exception before/after inventories and attribution were not obtained. **The 250 MB / 50 MB budgets are unverified.** No forbidden write or engine mutation is inferred merely from a volume delta, and no zero-write claim is inferred from read-only intent. This is the reason for the hold before copies, worktrees and UBT—not a new permission request for already-authorized source work. Permissions, security settings and global configuration were not changed to bypass the failure.

## Idle gate, predictions and commits

`europa_idle_gate.py` is a run-folder draft, not installed port tooling. Its **38 synthetic cases passed**, covering inclusive thresholds, below-threshold/unknown input, memory and GPU, foreign editors/game/UAT/cook/workers, owned-PID cases, each volume floor/resolution, three spaced polls, streak reset, mid-leg invalidation and the four-hour NEEDS-DECISION outcome without consuming D2. The real collector was not executed and no runtime leg was admitted. Evidence: `idle-self-tests09402.json`.

`predictions09402-draft.md` covers A0–A8/R0–R9, EA3's StackOBot/CaptureBench hosts, EA5's AA-off arbiter and paired TAA/TSR characterization, the per-world denominator, and EA10's sixteen-job split. **It is uncommitted.** There were **zero commits and zero pushes**, and no first build exists against which to claim prediction ordering. A resumed A1 must commit the reviewed predictions on the feature branch before UBT runs.

## A2 changes and proofs

**Applied engine/plugin source changes: none.** Existing 5.1 repository files remain unchanged; no runtime-neutrality claim is made for the proposed changes.

The run-folder `prepare_a2_09402.py` was exercised only in its in-memory dry-run mode against d7eee97. Its 22-file draft covers the five Build.cs files, descriptor, version/capability/stat/shared-pointer helpers, five subsystem headers, opaque control state and its implementation, shader/bench shells, and the two EA12 documentation lines. It is uncommitted and uncompiled, and must be reviewed before applying; it is not a completed implementation.

The actual files added in this run are report/receipt tooling and drafts: `preflight09402.py`, `p9_baseline09402.py`, `p9_exit09402.py`, `europa_idle_gate.py`, `europa_usn.py`, `check_europa_reflected_headers.py`, `prepare_a2_09402.py`, the predictions draft, `finalize09402.py`, and their result JSON/report files. Their 5.1 effect is **no engine/plugin modification**. The failed USN prototype is retained as diagnostic evidence, not qualified tooling.

EA11's checker self-test detects a synthetic reflected header containing the forbidden `#if ANOMALY_CONTROL_SERVER`. Its in-memory scan covered **six reflected headers**: **12 forbidden directives in the original release, zero in the proposed draft**. Evidence: `ea11-self-tests09402.json` and `ea11-draft-check09402.json`. This is not a worktree or UHT pass.

| Required proof | Result |
|---|---|
| 5.1 port versus d7eee97 JsonExport matrices | **Not run** |
| 4.25 JsonExport expected-set matrix | **Not run** |
| Removed-Shipping-blacklist matrix mutant | **Not run** |
| Both UBTs compile the shared C# branches | **Not run** |
| Selected 14.38.33130 / 14.16.27051 toolchains | **Not demonstrated** |
| Engine/plugin build actions | No UBT invocation or build occurred; this is not a matrix result |

Read-only inspection found both engines' existing rules assemblies and manifests; none of their listed source files was newer or missing. That is a preflight observation only, not proof that a subsequent UBT invocation will avoid every engine-tree write.

## Later backends and A3

No backend seam was installed. The proposed locations remain:

- **C, capture/labels/masks:** non-reflected capturer/render/readback implementations behind the existing capture subsystem's opaque async/preview state and backend factory. The 4.25 timing/render hook implementation must be proved independently; post-actor tick is not assumed equivalent to 5.1's capture anchor.
- **E, texture parity:** non-reflected material-readiness/draw and streaming/resource adapters behind the existing texture-corruption/stuck-mip facades and the shared capability profile. Strong reflected references are converted once in A3; backend replacement must preserve those declarations.
- **F, dashboard transport:** proposed `FAnomalyControlTransportState` in `Source/AnomalyControlServer/Private/Compat/AnomalyControlTransportState.h`, owned opaquely by the subsystem. The current 5.1 server and a later 4.25 adapter live behind that state; protocol and subsystem reflection need not be changed again.

The accepted A3 scope is unchanged: the literal shim and five parents; ShouldCreateSubsystem/world and ancestry audit; raw UPROPERTY pointers; Compat/use-site includes; inert/advanced-code dependency closure, capability admission and minimum host probe sources. **A3 must not start from this partial setup.** First resolve the A1 write proof, finish host/worktree/routing setup, commit predictions, apply/review A2, and run its required static and real-UBT matrix proofs. No full Editor/Game build belongs in that resumed A1/A2 work.

## STOP / decision requested

The blocker is an incomplete **EA4 / accepted-plan A0 write proof**, with observer attempts bounded under D2. It is not an automatic approval-review rejection. Windows access restrictions and the incomplete exception inventory are recorded above. I have neither relaxed the proof nor escalated permissions.

**Decision for Chat:** provide an approved observation mechanism available to the Relay, or explicitly amend the required proof. Until then, the proposed safe disposition is to retain the owned scratch/receipts, preserve J2 and every existing checkout, and resume at A1 only after a fresh protected-state and disk check. The owner has no editor/game action for this partial job; the independent numeric office read card remains the already-approved separate activity.

VERDICT: NEEDS-DECISION | A1 write proof blocked; no hosts, worktrees or commits; A2 not executed
