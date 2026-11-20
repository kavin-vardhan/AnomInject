# 094-47 root disposition — implementation present; validation incomplete

2026-11-05, 16:55 IST. Codex is the owner-authorized orchestrator and independent reviewer. This is a disposition of the completed job, not release approval.

094-47 closed with NEEDS-DECISION. The UE 4.25 transport and shared C5 entry are implemented, and the final 4.25 Development Editor and Game builds pass. Stage A as a whole is incomplete: the required 5.1 compile produced 14 first-header errors despite exit 0 and successful linking. The lever audit and full strict-include obligation also remain open. No live protocol, package, dashboard build or equivalence result exists.

## Reviewed evidence

Read in full: 094-47-codex-report.md; 094-47-code-review.md; the final mailbox report; both pre-push scans; the local-validation card. Root also read the implementation from git objects, the build/test receipts and relevant installed engine source. See 094-47-codex-source-review-working.md for the detailed source review and the explicit correction of the initial exit-code-only 5.1 reading.

- Worker report body SHA256: 57BD89BD2DE74EDB7C9C4FC328548EEF3DB18F4D5BF3FB140BCCE8C5904D0161.
- Collected worker report: 29101D3AE6326DFF32AC4257C7EC120E06788FF642B34CAD3FCA43D002048A9E.
- Code full review: 6C6E6D1ACA3EC0079A9A716CCF89118B1052468F2BE3E024C836AAB1378B0556.
- Mailbox final: EC51AD777697BD28DA6048D6C508B4D5CF3D899DCA5E1079E8C45579B04DF6E7.
- Root archive/protection evidence: 13A29C5AADFA757F424DA020A2521AF6F9A731FCC50B195A212FFDC030128A40.
- Root engine-profile analysis: 8F7A658F05C46D30609C925F112DBECCB30B8303A16E7423F4BF919376ACCEBB.

Root rehashed 38 live/archive pairs: zero missing files, hash mismatches or mtime mismatches. Host protection manifests agree; this manifest comparison is not a claim that root separately rehashed every host asset. Code did its independent host/material hash checks. The fixture map agrees with the newer 09444r asset receipt; the obsolete 09442c comparison is preserved, not silently discarded.

## Results and limits

| Area | Disposition |
|---|---|
| 4.25 source | Reviewed against the approved transport/C5 design; live behavior unverified |
| Helper tests | 543 checks pass; four broken variants fail; four token-redaction checks pass |
| Final 4.25 Editor / Game | PASS at a231a577905b3ea6163500b5a5982f108c8c2183; no build inputs changed in a2250fd |
| 5.1 ReGate51 | FAILED compile gate: 14 first-header diagnostics; exit 0 does not clear them |
| Lever audit | FAILED: 12 inline bench-flag checks and 32 source/binary registration mismatches |
| Full strict-include audit | Incomplete |
| Protocol smoke, package, dashboard, Stage C | UNRUN |
| Plugin / dashboard push | None |
| Camera qualification | D4 and strict G3 remain failed; GO3 unrun |

The first-header order was introduced by a2cc30d and predates the entry and origin tip. Root chooses a narrow source correction under the owner's delegation; the gate remains unchanged. A fresh compile of the old entry would not clear it and is unnecessary for that decision.

For the 32 B2 mismatches, root re-evaluated all 32 source locations with the unchanged evaluator and evidenced macros: all inactive on 4.25 and active on 5.1. This is diagnostic evidence, not a passing audit. A valid correction must retain all audit rules and test meaningful failing cases. The 12 raw IAIBench checks require a predicate preserving their narrower admission; replacing them with the existing IAIBench-or-IAIBenchFixture helper would be incorrect.

## State and safety

Local feat/europa-port a2250fda7a9b2f3754fd13c6061fe632f45b9d6e, clean. Origin c5d0919e5074404d26da8c16ccf845b360bde383. Dashboard master 7ea2e51bf55e19c2949641bbff2ec72afb4ae493, clean; no feat/europa-425 branch. Protected m51 and refs unchanged.

Code and worker report zero runtime launches, crashes, upload signs or token leaks. Build replacements, six makefile refresh deletions and 378 temporary ReGate51 checkout-file removals have receipts; do not describe the run as literally having no deletions. No old evidence or owner files were deleted to make space.

ReGate51 source is restored to clean detached 40a8eaf983518fe1992e172434aa8caf347654f1, but two plugin DLLs are from the unqualified candidate. It is not a qualified baseline and must be rebuilt for its selected source before any reliance. No 5.1 launch is authorized.

The watcher is idle and queue empty; the 094-47 worker is absent. Last recheck: 16:46 IST, C/D/E free 22.237/53.204/51.339 GiB. The remaining full sessions exceed current E: headroom before packaging; package capacity is not proved. No capacity exception is accepted.

## Next bounded step and authority

094-47r-owner-delegated-ruling.md authorizes the specific include/audit prerequisite repairs and resumption under unchanged gates. Retain LOD corruption and Shipping exclusion. Keep the plugin local and the shipped 094-45r card unchanged. The independent dashboard component may progress under OD4, but cannot qualify a packaged plugin by itself.

Code ended at measured 656836 tokens, 66% of its 1M window. Use a fresh Code session for the substantial follow-up, near its established 70% boundary. This is not a request to restart this root Codex workstream or a statement about root context usage.

Chat review NOT REQUIRED for this disposition or the narrowly delegated repairs. The next mandatory owner/Chat checkpoint is a proposed qualification-meaning, threshold or release-authority change. Existing release/merge reviews remain mandatory. Overall status remains NEEDS-DECISION, with inherited and new blockers distinguished.
