<!-- Collected by Claude Code (mailbox brief 094-23) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md; sha256 A4B330E9A44AA2C656D4DA0C40E676BA450CBEDE0E183B50E34EE656492401CD, 15868 bytes; thread 01a14334, resume 2026-10-28 10:47:59 - 15:55:14 IST, 18435 s over 2 tries (try 1 ended on a model-capacity error), exit 0; body below is byte-identical -->
094-23 is complete: the EX1–EX5 fixture-authoring kit is re-armed for EA_Core and EA_Travel. Both final dry runs passed on throwaway slots, and the real collector excluded the complete launcher tree correctly. All 15 real 09423 paths and all 15 superseded 09422 paths remain absent; the old arming refuses. Zero Unreal launches or builds occurred. Bound binaries, manifests, Paris Content, probe sources and the spent 09417 kit are unchanged.

Final feature commit: `97de9d425743c5a3c971642a767fd5165e26bd43`. The kit binds tooling commit `8c4da784fb4d9857bae18a207dbf78343b842f8f`; the final commit changes only CLAUDE.md and the session journal. No push, tags, new binary archives or persistent setting changes. Real authoring remains for the later authorized job.

EA8 entry and exit both pass against P9 v2: 472,553 inventory files, 1,023 admitted metadata entries, and 23,033 selected binary/config hashes covering 104,794,989,456 bytes. Protected git state and original source manifests match. The v2 file remains `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`. ET2 marker `EditorRuns/31124` remains unchanged. Evidence: [entry](D:/IA_Europa/Receipts/09423/P9/entry/summary.json), [exit](D:/IA_Europa/Receipts/09423/P9/exit/summary.json), [bound-file check](D:/IA_Europa/Receipts/09423/bound-exit-check.json).

| Free space | Entry GiB | Exit GiB | Start / warning / floor GiB |
|---|---:|---:|---|
| C: | 20.35 | 25.23 | 13 / 11 / 10 |
| D: | 70.62 | 48.26 | 40 / 30 / 25 |
| E: | 33.42 | 22.36 | 25 / 20 / 15 |

ED3 toolchains are unchanged: VS 2022 17.12.35707.178 and VS 2017 Build Tools 15.9.34031.82; MSVC 14.16.27023, 14.36.32532, 14.38.33130 and 14.42.34433; SDK 10.0.17763.0 and 10.0.22621.0. m51 remains `53bf725`, with its original index and 15 baseline untracked entries. The other 71 refs and five prunable registrations are unchanged. Baseline51 remains `d7eee97`, ReGate51 `40a8eaf`; all three worktrees are clean. [Toolchains](D:/IA_Europa/Receipts/09423/toolchains.json); [git/W4 proof](D:/IA_Europa/Receipts/09423/final-state.json).

EX1 recomputes launcher ancestry at each poll, records every exclusion and rejects parent-PID reuse using creation times when available. Unreal/game processes outside that tree still block the measured start gate. EX2 retains that full start gate, then acts only on disk floors, free commit below 6 GiB and the independent 1,800-second session deadline. Physical memory, GPU, human input and foreign processes become observations. Acting limits stop only the retained owned tree, including descendants; there is no mid-session pause or automatic retry. [EX1–EX2](D:/IntrusiveAnomalies/_reviews/094-22-ruling.md:43).

EX3 verifies existing files and the exact declared-output preflight before quarantining partial outputs. It records source identity/hash and destination before each move, rechecks them, and verifies the moved bytes. Only declared new packages can move into that attempt’s quarantine; pre-existing files, aliases, links, changed identities and reused destinations refuse. The launch lock stays spent. EX5 changes the job-template wording to “armed for EW2” and retains inherited EUROPA_* filtering. No probe or production-source edit was needed. [EX3–EX5](D:/IntrusiveAnomalies/_reviews/094-22-ruling.md:67); [implementation note](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09423-authoring-amendments.md).

| Validation | Result |
|---|---:|
| EX1 ancestry, outsiders, missing launcher and PID reuse | 13/13 PASS |
| EX2 acting limits, observations, phase handoff and owned-tree stops | 29/29 PASS |
| EX3 declared-output quarantine, preservation and refusal cases | 10/10 PASS |
| Arming, amendments and wording | 5/5 PASS |
| Retained kit boundary/slot/alias/environment regressions | 89/89 PASS |
| Retained measured-gate cases, including bare and dotnet UBT | 48/48 PASS |
| Retained static check and mutants | 33/33 PASS |
| Command-audit tests and can-fails | 33/33 PASS |

The EX tooling passed on D2 candidate 1. Stop tests used harmless Python children and a sibling control; quarantine tests used synthetic packages in owned scratch. No real authoring output was created. [Combined tests](D:/IA_Europa/Receipts/09423/self-tests/attempt-1/combined.json), [EX cases](D:/IA_Europa/Receipts/09423/self-tests/attempt-1/EX/result.json), [retained kit cases](D:/IA_Europa/Receipts/09423/self-tests/attempt-1/regression/result.json).

The arming cites `094-21-ruling.md#EW2` and `#EW3`, amended by `094-22-ruling.md#EX1` through `#EX5`. `authorized_by_chat` and `runtime_authorized` cover exactly two ordinary 4.25 Editor authoring sessions: one per recipe, 1,800 seconds each, declared new packages only. No PIE, cook, commandlet, packaging, build/query, production edit or config-file edit is authorized. `-unattended` stays off; EU4’s false upload override remains mandatory. [Arming receipt](D:/IA_Europa/Receipts/09423/arming/receipt.json); [exact argv for all six slots](D:/IA_Europa/Receipts/09423/arming/exact-argv.json).

The real slot paths are:

```text
D:/IA_Europa/Receipts/09423/fixture-attempts/EA_Core/01
D:/IA_Europa/Receipts/09423/fixture-attempts/EA_Core/02
D:/IA_Europa/Receipts/09423/fixture-attempts/EA_Core/03
D:/IA_Europa/Receipts/09423/fixture-attempts/EA_Travel/01
D:/IA_Europa/Receipts/09423/fixture-attempts/EA_Travel/02
D:/IA_Europa/Receipts/09423/fixture-attempts/EA_Travel/03
```

The six TEMP paths mirror these under `D:/IA_Europa/Temp/09423/fixture-attempts`. State paths are `D:/IA_Europa/Receipts/09423/fixture-state/global`, `/EA_Core` and `/EA_Travel`. All 15 are absent. Three slots permit prelaunch refusals; the first process creation permanently spends that recipe. The 09422 config is preserved as evidence and is refused by current policy; its 15 paths remain absent. The 09417 launched lock stays spent.

Declared outputs remain absent:

| Recipe | Exact new package |
|---|---|
| EA_Core | `D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA/SM_EA_TwoLod.uasset` |
| EA_Core | `D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA/EA_Core.umap` |
| EA_Travel | `D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA/EA_Travel.umap` |

| Bound item | SHA-256 |
|---|---|
| Europa425-FixtureAuthoring.bat | `4c5ad14eeb8fd22f0ebcd6ec2349ad28a874c21826cfda36df03c99b45982c41` |
| Europa425-FixtureAuthoring.json | `b1daf567382eb0c81f12eb6d6b2dc7b36b52c693eefae5b48ff9499a8f414f69` |
| 4.25 manifest, 27 artifacts | `c501895c9856638dd76e1c2091be8726f6797014bcb0714e306fd9d6ebd98df6` |
| Baseline51 manifest, 39 artifacts | `b05e6f8c9ce12fa1c5b4dbd142a59383f737bb8a3b7885b7219d1bcdb6c1fc37` |
| ReGate51 manifest, 44 artifacts | `3865fe0c6ed039d5bad2bc54d1a6a89f11c403e98237b4c6caf74ca661ea12e2` |

The [arming receipt](D:/IA_Europa/Receipts/09423/arming/receipt.json) records all 61 tooling hashes, 199 host bindings and seven engine executable/plugin bindings. Every individual EXE/DLL and artifact hash remains in the unchanged EW1 [4.25](D:/IA_Europa/Receipts/09421/qualification/final-425-binaries.json), [Baseline51](D:/IA_Europa/Receipts/09421/qualification/final-baseline51-binaries.json) and [ReGate51](D:/IA_Europa/Receipts/09421/qualification/final-regate51-binaries.json) manifests. All 110 artifacts, 1,458 existing Content files, fifteen probe files and six old makefiles re-hashed equal.

The actual .bat passed both recipes under `Receipts/09423/dry-run-final` and `Temp/09423/dry-run-final`. Protected-state, Content, config, tooling/binary bindings and routing checks were real; the waiting-gate samples were explicitly synthetic. Zero Unreal launches. The initial aggregate test deadline of 1,800 seconds expired during Travel preflight after Core passed. That attempt remains retained. The unchanged armed kit passed on fresh roots with a 3,600-second aggregate harness deadline; the real per-session limit remains 1,800 seconds. [Final dry-run receipt](D:/IA_Europa/Receipts/09423/dry-run-result.json); [first harness receipt](D:/IA_Europa/Receipts/09423/disk/04-dry-live.json).

The live observe-only reading ran after both dry runs, at 07:18:34–07:18:45 UTC (12:48 IST). It excluded root PID 26116 and descendants 4420, 35088, 39596, 40516, 43312, 43852, 44568 and 46296, including all five overlay processes. There were zero misread launcher blockers and zero outside blockers. Input idle was 6.985 seconds, free physical memory 7.33 GiB, GPU 1%, and reading errors were empty. Its only reason was `input_idle`; no clearance or launch was attempted. [Live collector receipt](D:/IA_Europa/Receipts/09423/live-gate.json).

At the live reading, no restart had occurred. Windows boot time remained 2026-10-22 17:12:18 IST; launcher PID 26116 still dated from 2026-10-27 10:38:17 IST. [Restart record](D:/IA_Europa/Receipts/09423/restart-state.json). The live console line was exactly:

```text
Waiting: mouse and keyboard idle for 300 seconds.
```

The complete [verbatim console catalog](D:/IA_Europa/Receipts/09423/arming/console-catalog.json) records all 191 authored print/refusal expressions with source lines. Dynamic paths, PIDs, timings and OS exception text are substituted at runtime; raw editor output is redirected to its log. The ordinary visible lines/templates are:

```text
Europa fixture authoring: checking the EW2 arming.
{recipe}: attempt {n} of 3; checking protected files, Content, binaries and routing.
{recipe}: protected files, Content, binaries and routing passed.
Waiting: three passing polls, 25 seconds apart.
Waiting: {each unmet condition joined by ; }.
Measured gate passed: three quiet polls; the launcher is already running.
Stopped: no measured idle window within four hours. No editor was started.
{recipe}: armed dry run passed. No editor was started; real slots remain unused.
{recipe}: starting the one authorized authoring session; limit 1800 seconds. Output is recorded in {session_path}.
{recipe}: clean exit and output checks passed; this recipe is now spent.
{recipe}: previously completed; its launch authorization remains spent.
{recipe}: {n} declared partial outputs quarantined by receipt. A retry requires chat re-arm.
Stopped: {recipe} did not pass. No later recipe or automatic retry will start.
Stopped: {reason} No further session will be started.
Both recipe dry runs finished. No editor was started.
Both recipe sessions finished. The two launch authorizations are spent.
```

Waiting names every unmet start condition: human idle, physical memory, GPU, process-reading availability, launcher presence, foreign Unreal PID or disk floor. Missing readings are named. Every refusal preserves its specific reason. In-session observations and acting-limit reasons are recorded separately.

Log and receipt paths under `D:/IA_Europa/Receipts/09423`:

```text
fixture-runs/<uuid>/wrapper.json
fixture-runs/<uuid>/<recipe>-disk.json
fixture-runs/<uuid>/<recipe>-entry-result.json
fixture-attempts/<recipe>/<slot>/claim.json
fixture-attempts/<recipe>/<slot>/prepared.json
fixture-attempts/<recipe>/<slot>/preflight-console.log
fixture-attempts/<recipe>/<slot>/P9/entry-fixture
fixture-attempts/<recipe>/<slot>/idle.jsonl
fixture-attempts/<recipe>/<slot>/runtime-ledger-console.log
fixture-attempts/<recipe>/<slot>/ledger/fixture-runtime/child-001.log
fixture-attempts/<recipe>/<slot>/ledger/fixture-runtime/child-001.json
fixture-attempts/<recipe>/<slot>/session/Runtime.log
fixture-attempts/<recipe>/<slot>/session/CrashClient.log
fixture-attempts/<recipe>/<slot>/session/probe/probe.jsonl
fixture-attempts/<recipe>/<slot>/authoring.json
fixture-attempts/<recipe>/<slot>/attempt-result.json
fixture-state/<recipe>/launch-reserved.json
fixture-state/<recipe>/launched.json
fixture-state/<recipe>/success.json
fixture-attempts/<recipe>/<slot>/session-gate.jsonl
fixture-attempts/<recipe>/<slot>/quarantine-receipt.json
fixture-attempts/<recipe>/<slot>/quarantine/<filename>
fixture-state/<recipe>/authoring-started.json
fixture-attempts/<recipe>/<slot>/creation-failed-marker-removal.json
```

EU2 preparation waits also emit JSON containing state, elapsed wait, affected volumes and free GiB; ledger/P9 progress emits its scope, inventory and hash counts. Exact source expressions and refusal messages are in the catalog above.

The full EK1 scan covered 45,426 files and 27,185,847,143 bytes: zero read errors, zero oversized skips and zero secret values after classification. Nine changed name contexts were reviewed; 309 other name-hit files match the prelaunch hashes. The two old locked temp files were read unchanged with the accepted Read|Write|Delete sharing mode. Accepted binary-extension and old synthetic-fixture exclusions remain explicit. [EK1 qualification](D:/IA_Europa/Receipts/09423/secret-scan/qualified.json).

The fresh canary was present and hash-verified before filtering. The launch-second all-extension scan found zero matches in 465 new/changed files, 1,968,891,909 bytes; it is repeated after report creation. The first full EK1 pass was interrupted by a recorded model-capacity failure. All four old owned PIDs were confirmed absent, its incomplete evidence was retained, and its ledger closeout passed before the fresh full scan. No completion or exit code was invented. [Canary scan](D:/IA_Europa/Receipts/09423/canary-exit.json); [interruption record](D:/IA_Europa/Receipts/09423/scan-interruption.json); [post-report canary scan](D:/IA_Europa/Receipts/09423/canary-final.json).

EU1 audit at report time: 118 filtered commands and one direct read-only bootstrap deviation; zero direct write-capable commands, unresolved commands or lifecycle violations. Both job turns are included, with distinct item identities. A failed turn alone cannot clear an active supervisor; the interrupted scan is closed in the audit only after the recorded process-absence evidence. The audit’s first self-tests caught inner/outer PowerShell normalization; the corrected reader passed all 33 tests, including write/stop can-fails. Failed receipts remain visible. [Report-time audit](D:/IA_Europa/Receipts/09423/command-audit-report.json); [final audit](D:/IA_Europa/Receipts/09423/command-audit-final.json).

EU2 pauses: none. No edits occurred within a pause window. Charged C:, E: and engine51 growth is zero. The ledger retains 232 known OS/tooling access denials; protected enumeration and full EK1 have zero errors. No foreign process was stopped. The [ledger summary](D:/IA_Europa/Receipts/09423/ledger-summary.json) retains the failed dry-run harness, interrupted scan and failed audit self-test receipts with their resolutions. Work finished within the original nine-hour limit, without resetting the clock after the model interruption.

EN5: no new interpretive exception was applied. The carried interpretation permits repeated saves of the same newly created declared package within one authoring session; [EX3](D:/IntrusiveAnomalies/_reviews/094-22-ruling.md:67) now expressly ratifies it. Its standing source is [EN5](D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:54) and the unchanged [authoring design](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09422-authoring-design.md). Nothing permits overwriting existing Content or retrying a spent session. No other EN5 item was used.

Local commits on `feat/europa-port`:

- `8c4da784fb4d9857bae18a207dbf78343b842f8f` — fix(europa): rearm fixture authoring under EX session rules.
- `97de9d425743c5a3c971642a767fd5165e26bd43` — docs(europa): record EX kit dry runs and live gate proof.

None was pushed. W4 date overrides were absent. No tags, binary archives or persistent bench settings changed. Synthetic controls and harmless child processes were confined to the tests; the real slots remain unused.

VERDICT: GREEN | EX1–EX5 kit re-armed and dry-run; live launcher gate and closeout pass; zero Unreal launches
