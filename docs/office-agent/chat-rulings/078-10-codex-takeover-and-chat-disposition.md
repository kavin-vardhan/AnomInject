# 078-10 - Codex takeover and disposition of Chat's 078-09 review

2026-09-06, approximately 19:50 IST. Sole orchestrator: this Codex task. The existing Code mailbox and watcher remain authoritative; no second orchestrator, watcher, worktree or agent was started.

## Verified state and read order

Read `ToCodex/10-CURRENT-CODEX-HANDOFF-2026-09-06.md` and followed its read order: operating contract, complete CLAUDE conventions/history, Chat's 078-09 response, Codex's paired review, 078-07 disposition, complete 078-09 report, mailbox state, and the full release-hardening predictions, gate manifest, instrument-diagnosis and editor-comparison records, including the F1 amendments. Later dispositions govern superseded historical claims.

Live read-only checks reproduce:

- AnomalyInjector branch `m51`, HEAD `d572b3dbcace8202f18de33ce9c697ab89f96037`; master and origin/master `ac13700d99fe116056f5aa11611f8931a563a3f2`; `fix/capture-label-integrity` and `origin/fix/capture-label-integrity` both `be16977c0802f57457ec68edcf8d0f96c12c13d9`, verified again at dispatch. Production diff `6713f51..HEAD -- Source Shaders` empty. Ten untracked owner handoffs preserved; no tracked WIP.
- CaptureBench `472a409dc70ae78d01a38d08fc3bddb15fe41c58`, tracked clean; seven existing untracked tools preserved. Lyra detached `4a4bfcc2cdbade6bbeb99494e540952847ae9b63`, clean.
- A archive SHA256 `B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8`; candidate archive AND staged runtime SHA256 `D50DDE789A94933182DC6B3FA74811F8BB57DC1F8332B4AC88ACE47337BD4796`.
- Staged Phase B container hashes: StackOBot-Windows.utoc `67EA1FE0A15ACF2429516C461C5FD85989285B3C93B24C42855AE4959386257A`; .ucas `2CEFB8F43D048E2534471BC1D9532B77A870FAF46F14C9C2AFAE813B99B6C4D3`; .pak `E03C661084D8BC2BB6EF0F51F2DECB162853B2188B733B1DBD22B96CA974CDB7`; global.ucas `A16A18A8FFB5151D2E850BB52841162F6844A0361818F8D1F158E8377DFFBA9E`; global.utoc `C70ECDAA232DB8CF77BEEAB47B7226C48FCC91FF705526D507500CD14CF21C51`.
- Immutable evidence hashes: M51_MASK_BATCH_DIAG_EVIDENCE/summary.json `401057FA05BC3BCFB8CB1D3783F8A04DF9902DD2AE0CFFA78CB50F4C22FDC1EB`; M51_EDITOR_DIAG_EVIDENCE/078-09-PAIR-CONSOLIDATED-EVIDENCE.txt `BAF9DF22EF20B6C7142A3248B82F6C26DD13BA60A77BADDFAA1E35A67154697D`.
- Watcher PID16056 alive and idle, latest heartbeat 19:48:28; queue empty; last declared verdict NEEDS-DECISION for 078-09. Existing Code session remains reusable below the 70% rollover boundary. No watcher/config changes.

## Disposition

Accept Chat's acceptance of all five 078-07 corrections and the 078-09 PASS/PASS pair within its stated scope. The restarted editor did not reproduce the original workload. The cause of the earlier slow condition remains OPEN. Stop chasing performance/environment for this frozen campaign; no owner-app action is required.

The multi-target payload-loss finding remains separate from the unexplained remainder of PREVIOUS/NO-DATA. Healthy callbacks can serve distinct namespaces for one render. Two target serials do not alone establish which arm is stale. Narrow Chat's phrase "the second target arm": cancellation must follow proven render/request identity, never queue position alone. A raw ArmGameFrame/FamilyFrame comparison requires a demonstrated counter-domain relationship; an observed offset is not itself proof of mismatch. The render-thread-ahead-arm explanation remains a hypothesis; the color key path does not certify mask pairing.

Chat's proposed pacing-validity band is NOT ADOPTED. No numeric band, independent rationale, enforcement change or can-fail has been approved. speed_ratio/FPS remain telemetry for this pairing campaign. No slow historical leg is retroactively invalidated; a valid pairing failure remains a failure at any speed. The separately declared full-milestone throughput gate remains held and unchanged.

Chat section 3 approves the bounded campaign in principle: fresh A natural reference; if it passes, candidate pairing at both tick orders and both 100/50 percentages; one predeclared same-recipe A control after the first valid candidate failure, then stop. A reference failure stops without candidate legs. Pose-invalid retries must have their own finite predeclared budget; no valid-failure retry and no extra exploratory leg. Probe must be live; Marker must be explicit. Keep the existing pairing predicate and all original failure banks.

The exact campaign record is not yet reviewed. **078-10 releases documentation and preflight ONLY.** Code will create and commit the concrete predictions and full commands, then return NEEDS-DECISION. Codex will review the complete report and changes and prepare the exact snapshot for Chat's adversarial review. No capture, staging, build, cook or implementation is released in 078-10. After reviewed runtime evidence, the identity/lifecycle unit's predictions may be prepared for review; implementation remains held. No full F1/m51 release, Lyra pass or all-gates claim follows from this smoke.
