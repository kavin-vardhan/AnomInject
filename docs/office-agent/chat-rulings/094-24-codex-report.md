<!-- Collected by Claude Code (mailbox brief 094-24) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md; sha256 65661C8595BC8CB0AB8B5D2F519C86062D48C757134AF5CFBE9C2E0D647B2C41, 12822 bytes; thread 01a14334, resume 2026-10-28 17:49:47 - 21:36:56 IST, 13629 s over 2 tries (try 1 ended ~20:52 on a model-capacity error during a remote compact task; the relay waited 10 min and resumed at 21:02:27), exit 0; body below is byte-identical -->
094-24 stopped before re-arming. The ledger detected a new top-level folder, E:\OBSVideos, and raised ED5 STOP. Its writer is not established. Implementation stopped; the 09424 kit is not armed, and the later real run is not ready.

The EY1-EY4 candidate passed its first tooling test set. Four real-branch rehearsal cases passed using six harmless Python recipe sessions. The missing-probe-stop case was first refused before launch by D:'s 40 GiB start line; its resume then stopped at ED5 before any task child. The deadline rehearsal, final supplemental checks, re-arm, armed dry runs and live collector reading remain pending. Zero Unreal launches, builds, commits, pushes or tags.

The stop receipt is [ledger/04-rehearsal-resume/between-operations.json](D:/IA_Europa/Receipts/09424/ledger/04-rehearsal-resume/between-operations.json). ED5 explicitly requires the new-root-folder can-fail to STOP: [ED5](D:/IntrusiveAnomalies/_reviews/094-04-request.md:79). No EN5 interpretation was used to waive it.

**Entry and exit proofs**

| Check | Result |
|---|---|
| EA8 entry against v2 | PASS: 472,553 inventory files; 1,023 admitted records; 23,033 full hashes covering 104,794,989,456 bytes. |
| Entry disk space | C: 24.97 GiB, D: 65.27 GiB, E: 17.73 GiB. Start/warn/floor GiB: C 13/11/10, D 40/30/25, E 15/12/10. |
| Report-time disk space | C: 24.31 GiB, D: 60.53 GiB, E: 55.48 GiB; all floors pass. |
| ED3 toolchains | Read-only proof passed. 4.25: MSVC 14.16.27023 / cl 19.16.27051, SDK 10.0.17763.0. Frozen 5.1 selection: MSVC 14.38.33130 / cl 19.38.33144, SDK 10.0.22621.0. |
| Full EA8 exit | REFUSED at the unchanged clean-owned-checkout requirement because 15 tooling files remain uncommitted after the STOP. No full exit PASS is claimed. |
| Separate protected Git proof | PASS: protected refs and worktrees match P9; m51 index/status match; both frozen 5.1 checkouts remain clean at their original heads. |
| Bound files | 110 artifacts, three manifests, 1,458 Content files, host/config routing, 15 probe files, six old makefiles, spent 09417 kit and 31124 marker unchanged. |
| EA11 | Accepted reflected-header check passed; no reflected probe header applies. |
| Canary | Non-vacuous launch-environment positive control; zero matches from launch second 2026-10-28T12:19:46Z. Initial closeout scan: 1,194 files / 1,780,536,256 bytes, all extensions. A final scan follows report creation. |
| Full EK1 | PASS: 47,425 files / 29,033,636,750 bytes; zero read errors, oversized skips or secret values. Changed name contexts were individually classified. |

Receipts: [entry](D:/IA_Europa/Receipts/09424/P9/entry/summary.json); [exit refusal](D:/IA_Europa/Receipts/09424/EA8-exit-attempt.json); [protected Git](D:/IA_Europa/Receipts/09424/stop-state/protected-git.json); [bound files and pending source hashes](D:/IA_Europa/Receipts/09424/stop-closeout.json); [full EK1 qualification](D:/IA_Europa/Receipts/09424/secret-scan/qualified.json); [EA11](D:/IA_Europa/Receipts/09424/ea11.json). The immutable v2 SHA-256 remains 8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74; its exit re-derivation passed.

**Tooling changes and self-tests**

The uncommitted candidate derives the authoring marker from the validated state path and removes EUROPA_AUTHORING_MARKER. It inventories named environment reads, narrows launcher exemptions to live install folders, refuses unreadable image paths and editor/game exemptions, adds UnrealPak/UnrealLightmass blockers, adds the 12 GiB commit Waiting condition, bounds unreadable acting readings at 60 seconds, and applies E: 15/12/10 GiB. Session acting reads stay in SessionGate after the marker; preparation retains EU2 supervision. Sources: [EY1](D:/IntrusiveAnomalies/_reviews/094-23-ruling.md:37), [EY2](D:/IntrusiveAnomalies/_reviews/094-23-ruling.md:62), [EY3](D:/IntrusiveAnomalies/_reviews/094-23-ruling.md:73), [EY4](D:/IntrusiveAnomalies/_reviews/094-23-ruling.md:81).

| Test set | Passed |
|---|---:|
| EY1-EY4 cases, including filtered-name and 59/60-second can-fails | 46 |
| Environment inventory at the tooling stage | 54 helpers; 16 named reads; 11 self-tests |
| Retained kit cases | 89 |
| Measured gate cases | 50 |
| Static probe check cases | 33 |
| Retained session/quarantine cases | 57 |

These are first-candidate results, not a completed qualification of the final kit. Final inventory/supplemental checks and the remaining rehearsals did not run after the stop. No gate was weakened or re-tuned. [Test dispatch receipt](D:/IA_Europa/Receipts/09424/self-tests-summary.json) and [EY cases](D:/IA_Europa/Receipts/09424/self-tests/ey-attempt1/result.json) preserve the individual results.

**Real-branch rehearsal**

The actual batch entry and non-dry main were exercised through reservation, derived marker, run_child, session monitor, qualification, spent lock and quarantine. Host, Content, TEMP, receipts and state were isolated under owned rehearsal scratch. Synthetic gate readings and scratch preflight were explicitly labelled; they are not an Unreal or EA8 runtime proof.

| Case | Harmless recipe sessions | Observed outcome |
|---|---:|---|
| pass-both | 2 | EA_Core AUTHORING_PASS; EA_Travel AUTHORING_PASS |
| partial-core | 1 | EA_Core STOP; EA_Travel NOT_STARTED |
| partial-travel | 2 | EA_Core AUTHORING_PASS; EA_Travel STOP |
| extra-core | 1 | EA_Core STOP; EA_Travel NOT_STARTED |
| missing-stop | 0 | Refused at D: 37.31 GiB, below the 40 GiB start line and above the 25 GiB floor. |
| missing-stop resume / deadline | 0 | ED5 stopped the resume before its task child; neither case completed. |

Each passing recipe wrote synthetic outputs, a skipped cleanup row and one probe_stop, then became spent. Failed Core runs prevented Travel. Failed sessions quarantined only declared synthetic outputs by receipt; the extra-file case refused its undeclared output. The deadline through the actual branch and supplemental retry/root-boundary checks remain pending.

Predictions and complete console/child/quarantine receipts are under [Receipts/09424/rehearsal](D:/IA_Europa/Receipts/09424/rehearsal); the four qualification files and hashes are indexed in [stop-closeout.json](D:/IA_Europa/Receipts/09424/stop-closeout.json). Real Paris Content and real slots were not used.

**Armed scope, slots and dry runs**

No 09424 arming was written. The unchanged 09423 config remains at D:\IA_Europa\Launch\Europa425-FixtureAuthoring.json, SHA-256 b1daf567382eb0c81f12eb6d6b2dc7b36b52c693eefae5b48ff9499a8f414f69. The unchanged batch entry is SHA-256 4c5ad14eeb8fd22f0ebcd6ec2349ad28a874c21826cfda36df03c99b45982c41.

The candidate refuses the old config with this exact reason:
> Fixture arming is not the 09424 EW2/EW3 arming amended by EX1-EX5 and EY1-EY4.

All 15 old real paths remain absent: global state, both recipe states, and receipt/TEMP slots 01-03 for each recipe under the 09423 fixture-state and fixture-attempts roots. Their exact absolute paths are in [stop-closeout.json](D:/IA_Europa/Receipts/09424/stop-closeout.json). The 09424 real fixture-state/fixture-attempts roots also remain absent. No new authorization scope or declared-output binding was issued. The required final armed dry runs and live observe-only collector reading were not reached; no current launcher-exemption or restart conclusion is substituted for that missing reading.

EW1 hashes remain:

| Record | Files | Manifest SHA-256 |
|---|---:|---|
| 425 | 27 | c501895c9856638dd76e1c2091be8726f6797014bcb0714e306fd9d6ebd98df6 |
| baseline51 | 39 | b05e6f8c9ce12fa1c5b4dbd142a59383f737bb8a3b7885b7219d1bcdb6c1fc37 |
| regate51 | 44 | 3865fe0c6ed039d5bad2bc54d1a6a89f11c403e98237b4c6caf74ca661ea12e2 |

Every unchanged artifact path, size and hash is in [unchanged-binary-inventory.json](D:/IA_Europa/Receipts/09424/unchanged-binary-inventory.json). No exe/DLL was built or copied and no binary archive was created. EY5's two 5.1 records remain frozen; no build, EO3 refresh, EI4 query or checkout change was made there. [EY5](D:/IntrusiveAnomalies/_reviews/094-23-ruling.md:90)

**Console and log evidence**

[The complete console/refusal expression catalog](D:/IA_Europa/Receipts/09424/candidate-console-catalog.json) records file/line citations for the uncommitted candidate. Each completed rehearsal qualification also preserves its actual console verbatim. Relevant exact lines include:

> Rehearsal: owned scratch and harmless stand-ins; no Unreal process will start.
>
> Europa fixture authoring: checking the EW2 arming.
>
> Measured gate passed: three quiet polls; the launcher is already running.
>
> EA_Core: clean exit and output checks passed; this recipe is now spent.
>
> EA_Travel: clean exit and output checks passed; this recipe is now spent.
>
> Stopped: EA_Core did not pass. No later recipe or automatic retry will start.
>
> Waiting: 12 GiB free commit.

These are rehearsal/source records, not a final armed owner card. Dynamic paths, slot numbers, refusal reasons and all ledger output remain in the catalog and child logs.

**Command audit, ledger and deviations**

The audit records 118 filtered commands, 1 direct read-only deviation(s), zero direct write-capable commands and zero pause-window violations. Its read-only DriveInfo/process inspection case has positive coverage and write/Stop-Process can-fails. [Final command audit](D:/IA_Europa/Receipts/09424/command-audit-final.json)

The initial direct read-only bootstrap inspection is retained under EU1. One concurrent wrapper receipt append was refused before the requested inspection child; its read was repeated serially. No canary value was written. All later commands used the filtered wrapper.

The first exit-check operation was interrupted by a model-capacity turn failure before its task child. Both recorded PIDs were independently absent (OpenProcess error 87). Its original receipt and partial snapshot were preserved, and only closeout was retried in a fresh operation. [Interruption and preservation receipt](D:/IA_Europa/Receipts/09424/scan-interruption.json)

The first completed audit flagged two process-inspection forms it did not recognize. One was a PowerShell parse failure before execution; the other only queried Win32_Process and selected image paths. After inspecting their exact commands and results, the report-only audit added those two literal forms, with appended-write, Stop-Process and mutating-CIM can-fails. The initial REVIEW_REQUIRED receipt remains at [command-audit.json](D:/IA_Europa/Receipts/09424/command-audit.json). This parser change did not alter the kit or waive a write.

There were zero EU2 floor pauses. D:'s start-line refusal is retained separately; D later recovered without a lowered gate. The writer behind that disk movement is not established. ED5's new E:\OBSVideos folder remains a STOP with unknown writer; no deletion, attribution waiver or policy change was attempted.

The ledger retains that STOP even though later closeout operations pass. Charged C:, E: and engine51 growth is zero. Aggregate ledger evidence is 1.68 GiB. The 232 known OS/tooling enumeration denials are retained; protected enumeration and the separate full EK1 scan have no read errors. [Ledger summary](D:/IA_Europa/Receipts/09424/ledger-summary.json)

No EN5 exception was applied. In particular, the new-root STOP was not treated as benign engine behavior. Disk and session amendments came directly from EU2, EV3 and EY1-EY4. [EN5](D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:54)

The deviation from the requested completion is explicit: the stop prevented remaining rehearsals, final supplemental checks, re-arm, armed dry runs, live reading and commit. Feature branch feat/europa-port remains at 97de9d425743c5a3c971642a767fd5165e26bd43, with 11 modified and four new Python tooling files preserved for review. The tracked diff and all 15 hashes are in [pending diff](D:/IA_Europa/Receipts/09424/stop-state/tracked-tooling.diff) and [pending file hashes](D:/IA_Europa/Receipts/09424/stop-closeout.json). No commit or push was made; no tags changed. W4 date overrides were absent in the launch/wrapper proofs.

Short deadlines, synthetic readings and synthetic packages were confined to scratch tests. The real 1,800-second session limit and persistent machine/bench settings were not changed. Foreign processes and files, protected branches, the frozen hosts and the 31124 marker were left alone. No cleanup was performed outside receipted job scratch.

Chat must dispose of the ED5 root-folder stop before the remaining rehearsal and re-arm can proceed. The later real authoring run is not ready.

VERDICT: NEEDS-DECISION | ED5 STOP at E:\OBSVideos; 09424 not re-armed; zero Unreal launches.
