# Current Codex orchestrator handoff — 2026-09-06, 19:39 IST

This is the entry point for the NEXT Codex session, prepared at the owner's request for a safe session transition. It supersedes the *current-state description* in the original ToCodex/00–09 seed bundle, which is historical. It does not replace the repository's operating contract, dated rulings, or raw evidence. Recheck live state before acting.

## Where we stopped

the owner said: "Chat's responded. Before we proceed, should I start a new session? This one has been going on for too long. Will the new session have appropriate context?"

The outgoing Codex session read Chat's latest response in FULL and prepared this handoff. It did NOT dispatch a new brief, run captures, change source, change gate rules, or commit anything during the transition. The next campaign is approved in principle by Chat; exact predictions and remaining dispositions still need work. The old session should remain inactive once the new one takes ownership.

## Role and authorization

- The owner explicitly appointed Codex the orchestrator. Codex designs, reviews, rules, and writes bounded briefs. Claude Code / Opus 5 implements, runs legs and performs repository git writes through the existing local mailbox. Chat Claude is the adversarial reviewer and responds under `_reviews`; it does not dispatch briefs.
- Continue in the SAME local workspace `D:\IntrusiveAnomalies` and existing m51 checkout. Do not make a new worktree, reset branches, start a second watcher, or run two orchestrators against this mailbox. The bench checkout and staged executable are shared resources.
- Autonomous technical decisions, mailbox dispatch and normal local implementation are already authorized. Do not repeatedly ask the owner to approve routine steps. Predictions must be reviewed before legs, and evidence before merge. Product/schema meaning changes, office actions, owner app changes, and delivery commitments have separate owner boundaries.
- Office data stays at the office. Only owner-typed readings return to `_bates_reads`. Client codenames: Concorde, Bates, Europa. <other-home-project> is the owner's unrelated PERSONAL project, not a client game. StackOBot is the packaged bench fixture; Lyra is the second required fixture.

## Read order (full files; do not infer an unread report from its verdict)

1. `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\docs\OPERATING-CONTRACT.md` — authoritative rules, including ALL dated corrections. The ToCodex copy is stale.
2. `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\CLAUDE.md` — current status and repository conventions.
3. `D:\IntrusiveAnomalies\_reviews\078-09-chat-paired-evidence-review.md` — latest Chat response, accepted findings and proposed next scope. File timestamp 19:31:38; its heading's approximate 19:45 time is not the filesystem time.
4. `D:\IntrusiveAnomalies\_reviews\078-09-codex-paired-evidence-review-and-request-to-chat.md` — independently checked editor-pair evidence and proposed campaign.
5. `D:\IntrusiveAnomalies\_reviews\078-07-codex-offline-findings-and-disposition.md` — namespace-aware mask batching and request-ID evidence; essential to the next identity design. Its then-current next action and 77% old Code-session note are historical.
6. `D:\IntrusiveAnomalies\_mailbox\to_chat\078-09-editor-closed-capture.report.md` — FULL latest Code report. Outgoing Codex already read this and 078-08 in full and reviewed the evidence.
7. `D:\IntrusiveAnomalies\_mailbox\README.md`, live `config.json`, `watcher_status.txt`, `last_verdict.txt`, and queue inventory. README is current; ToCodex copy is stale.
8. Before the next brief, read the relevant prediction files in the repo: `docs/predictions/2026-09-06-m51-release-hardening.md` (amendments supersede draft sections), `2026-09-06-m51-gate-manifest.json`, `2026-09-06-m51-pairing-instrument-diagnosis.md`, and `2026-09-06-m51-editor-comparison.md`. Use `rg --files` to locate the F1 smoke predictions/amendments and existing briefs rather than inventing command options.

For broader background, the original ToCodex review/diff and repo session journals retain the sixteen findings and R1–R8. Retrieve them as needed; do not restart the entire historical analysis.

## Verified state at transition

| Item | State |
|---|---|
| Plugin repo | `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector` |
| Branch / HEAD | `m51`, `d572b3dbcace8202f18de33ce9c697ab89f96037` |
| master / origin/master | `ac13700d99fe116056f5aa11611f8931a563a3f2`, unchanged |
| Only production change on m51 | F1, `6713f510166a0d9458387caaae711779b07fad64`; subsequent changes are docs only |
| Working tree | Ten pre-existing untracked `docs/CHAT-HANDOFF-*.md` files, no other dirty paths; preserve them |
| Original source branch | `fix/capture-label-integrity` at `be16977c0802f57457ec68edcf8d0f96c12c13d9`, backed up to origin |
| CaptureBench | Sibling plugin repo, `472a409`, last checked tracked-clean |
| Lyra worktree | `E:\UEProjects\LyraStarterGame\Plugins\AnomalyInjector`, last known detached `4a4bfcc`, clean; no new Lyra work |
| Mailbox | Idle, queue empty, watcher PID 16056; heartbeat checked at 19:38:37 |
| Last verdict | Declared NEEDS-DECISION for 078-09, rc 0, finished 19:24:56 |
| Code session | `<uuid>`; reported 392,974 tokens / 39%; reusable |
| Merge / push / tag | None for m51; release remains blocked |

Recheck these pins, queue and any new review before dispatch. A changed pin is evidence to reconcile, not permission to restore the old state.

## Runtime pins

- A archive: `D:\IntrusiveAnomalies\_binary_baselines\StackOBot.exe.m49b-gate-B8AC65D8`; SHA256 `B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8`.
- Candidate archive: `D:\IntrusiveAnomalies\_binary_baselines\StackOBot.exe.m51-f1-candidate-D50DDE78`; SHA256 `D50DDE789A94933182DC6B3FA74811F8BB57DC1F8332B4AC88ACE47337BD4796`.
- Staged runtime: `D:\IntrusiveAnomalies\StackOBot\Builds\BenchGate\Windows\StackOBot\Binaries\Win64\StackOBot.exe`, restored to candidate and independently hashed after 078-09. Preserve the frozen five-file container; exact hashes are in the paired evidence receipts.
- 078-04's interrupted-link executable was quarantined and NEVER run; do not select it as a baseline or candidate. The successful F1 build was recovered in 078-05. No rebuild/cook is needed for the approved frozen-binary campaign.

## What is established, and what remains open

**F1:** requested but ineffective target masks now commit unmeasured frames instead of timing out and losing captures. Native `TargetMask 1 / Mask 0` changed 33/90 images and 57 missing requested indices to 90/90, zero missing, in both orders; all rows unmeasured, schema meaning unchanged. OutputHeight360 readings also improved but their B1 pose gate did not certify. Candidate native cap90 ONSET/G1/G2/G3/G3b/G3c/G7 smokes passed both orders. Effective-mask pairing then failed on candidate AND concurrent A-side. A failure without F1 does not rule out an F1 contribution. Full release and target-game correctness are NOT certified.

**Pairing evidence:** candidate PREVIOUS counts 9/12/10/17 (natural/synth,100/50); four historical A banks had zero. Current A controls also failed (PREVIOUS9/10). Replaying Marker1 versus Marker0 did not restore pairing; source mtimes/clean checkout are not complete historical hash receipts. Low throughput correlates with failure but is not an established cause.

**Concrete mask defect:** healthy callbacks can serve normal / target / census consumers for ONE render (normal game-frame namespace; target bit61; census bit62). Chat's "healthy servedArms always1" prediction was falsified. Each of three failing runs instead has exactly three callbacks serving TWO distinct target-capture serials; none in the four healthy runs. The first pixel requester takes `TightPixels`, the second receives an empty payload. Actual request-ID joins link this exactly to three unavailable masks per failing run. It does NOT explain all PREVIOUS or NO-DATA results. Do not blindly copy one render into records for different frames.

Evidence: `D:\IntrusiveAnomalies\_bench_sessions_bank\M51_MASK_BATCH_DIAG_EVIDENCE`, especially `summary.json` and `payload_outcomes.json`, source log hashes and line-numbered receipts. Summary SHA256 `401057FA05BC3BCFB8CB1D3783F8A04DF9902DD2AE0CFFA78CB50F4C22FDC1EB`. Relevant source at F1: `AnomalyMaskSceneViewExtension.cpp:511–528`, `AnomalyCaptureSubsystem.cpp:1191–1204`.

**Editor pair completed:** owner reopened then closed PERSONAL <other-home-project>; Code ran the packaged StackOBot bench. RESIDENT A session `20260906-190743`: CURRENT35/PREVIOUS0/NEITHER0/NO-DATA5, 40 images/rows, ~29.84 FPS. CLOSED A session `20260906-191936`:37/0/0/3, 40 images/rows, ~29.99 FPS. Both accepted first pose attempt. Same frozen A/container, natural, native1280x720, SVE1, Marker1, cap40, seed4242, blinking StaticMeshActor_49, probe enabled and observed live. Predictions `39afe83` preceded both legs.

The editor had restarted, so original failing workload was NOT reproduced. PASS/PASS cannot establish whether closing that workload cures failure. Comparing CPU averages over76seconds versus hours says nothing about the ~1.7-second capture workload. Cause of afternoon low throughput remains OPEN; do not chase it further in this campaign. No more owner coordination is needed for the completed pair. Owner was told they could reopen their personal project; current app state may have changed. No general editor ban (it would also exclude Lyra).

Pair evidence: `D:\IntrusiveAnomalies\_bench_sessions_bank\M51_EDITOR_DIAG_EVIDENCE\078-09-PAIR-CONSOLIDATED-EVIDENCE.txt`, SHA256 `BAF9DF22EF20B6C7142A3248B82F6C26DD13BA60A77BADDFAA1E35A67154697D`. Final Code report SHA256 `F7A1C82A942E7DD111956159FA37A6F8DFA0EA05F8B30D625A04E33593B50972`.

## Chat's new response and the next bounded step

Latest Chat response accepts all five 078-07 corrections, accepts the mask payload-loss defect and editor pair as scoped, and approves the outgoing Codex campaign VERBATIM:

1. Write/review exact predictions and full commands (including Marker and live probe checks) before any leg.
2. One fresh A-side natural pairing reference. If it fails, STOP.
3. Existing candidate pairing smoke in BOTH tick orders at100% and50%, archived executables/frozen container.
4. A valid candidate failure permits EXACTLY ONE predeclared A-side control on the same recipe, then STOP. No retry-until-pass or unannounced exploratory legs. Keep pose-invalid retries distinct from valid checker failures; define them before execution.
5. Review evidence before moving to identity/lifecycle predictions. A passing scoped smoke is not the full release gate suite and never erases earlier failures.

The campaign is NOT queued. Suggested next numbering is 078-10, but check files/ran.txt first. The new orchestrator should write a concise disposition of Chat's response, preserve scope, then prepare the next brief under existing authorization. Do not begin identity implementation in the frozen-binary campaign.

**Pending pacing-rule disposition:** Chat §2 newly recommends declaring legs INVALID outside a predeclared pacing band. This is a REVIEWER PROPOSAL, not an adopted contract change. No numeric band, rationale, harness implementation or falsifier has been agreed. It must NOT silently enter this campaign or retrospectively invalidate the saved slow failures. Separate any future pacing-conformance gate from image/label integrity: reduced throughput cannot make a wrong-frame label correct. Preserve the §3 agreement of no new performance waiver. Do not treat Chat's overall approval as blanket adoption of every recommendation.

**Later identity unit:** frame-bound publication at BeginRenderViewFamily, RT consumption of that published batch, stale/unrendered cancellation with honest unmeasured outcomes, same-render consumer payload ownership, and lifecycle conservation. Determine *which* request is stale using actual identities; "second requester" order alone does not prove staleness. Log family frame number, arm game-frame and publish binding together. Raw family-frame vs GFrameCounter equality is not valid without domain proof (observed offset2 in healthy and failing colour traces). RT draining later GT arms into an earlier family's tonemap is a hypothesis to discriminate, not yet the full proven cause.

Chat suggests8-bit+constant-validity-cube probe (256-frame range). It remains a design candidate: establish range, all-zero/occlusion/partial-bit validity and a validity test before reliance. Preserve the old two-position pairing gate, which cannot distinguish even-frame slips because it repeats every2. `WithholdWorldRender` is NOT a proven arm-accumulation lever; measure whether it permits arming, or use a properly measured alternative. Predictions need review before any implementation leg.

## Other constraints that must survive the transition

- Phase B DrawnCount / frames_drawn_unexpected / exposure_dip_scope behaviour must survive. DrawnCount is diagnostic, neither a visibility-positive input nor a veto. Observable predicate remains labelled && held && target_pixels >= configured threshold, after the unmeasured early-out.
- Preserve R1 meanings. Schema3 is mostly additive; anomaly_present retains injection meaning, visible_positive is separately observable-only. Zero-measured fallback remains affected_frames == injected_frames with observability_measured:false. Do not intersect injection history with successful image receipts to hide missing images. Existing per-fire label sampler stays; final capture_complete/auditor reconcile incomplete sessions. Read the explicit empty-window value-change ruling.
- F1 effective mask predicate excludes bSveCapture deliberately; the independent extension does not justify an SVE-only restriction. Backbuffer x target-mask remains UNRUN, not proven working.
- Deterministic writer bound remains an implementation prerequisite. Proposed deflateBound+256 does not include PNG IDAT chunk overhead: libpng8192-byte chunks add12bytes each; at1080pRGBA, roughly1013chunks add12,156bytes. Bound the whole enabled encoder format/settings and simultaneously owned buffers; do not claim this also bounds codec scratch, process/GPU memory or JPEG.
- Full gates in BOTH orders/BOTH fixtures, one binary per fixture, byte-identity A anchors. Lyra baseline is a module set+manifest, not a source SHA. No summary "all required gates pass" while any required arm remains open. Throughput1080p, backbuffer, shader readiness, writer/readback saturation, auditor/schema, lifecycle and remaining findings are unfinished; use the manifest for the complete inventory.

## Dispatch mechanics

- One brief in flight. Write a source copy under `_reviews`, then the actual numbered `.md` under `_mailbox\to_code`. No temporary files in mailbox or mailbox artifacts in repo.
- Resume current Code session by omitting SESSION. Required next headers: `# NAME: ...`, `# EFFORT: xhigh`, `# MODEL: claude-opus-5`, `# ACK-VERDICT: <explicit scoped resolution of 078-09 NEEDS-DECISION>`. Config default effort is high, so the override matters. Roll Code at70%, not merely because Codex's own chat was rolled.
- Read each final report in FULL before the next brief. Exit0 is not acceptance; require the literal final `VERDICT: GREEN|NEEDS-DECISION|RED`. The fail-closed watcher change has only been source-inspected here, not runtime-tested.
- Use PowerShell directly: Code's Bash path repeatedly hangs. Explicit UTF-8 for docs. Use `git commit -F <absolute message-file>` instead of inline multiline shell quoting; avoid unnecessary temp-message deletion. Do not leave background builds/captures running at report time.
- Do not start another watcher; inspect current PID/process first if heartbeat fails. Do not rerun completed brief numbers. No scheduled automation was created by this handoff.

## Suggested owner kickoff in the new Codex task

Read D:\IntrusiveAnomalies\ToCodex\10-CURRENT-CODEX-HANDOFF-2026-09-06.md and follow its read order. Continue as the sole AnomalyInjector orchestrator in the existing local workspace. Recheck live state, review Chat's latest response, and proceed with the next bounded campaign through the existing Code mailbox. Preserve the pending pacing-rule distinction and all existing gate requirements.
