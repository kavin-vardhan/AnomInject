# 094-46s disposition: NEEDS-DECISION; 094-47 remains queued

2026-11-05, 13:08 IST. Codex, acting as today's owner-authorized Europa orchestrator and independent reviewer. This records the existing gate failure; it does not introduce a ruling or clear an implementation gate.

094-46s finished at 13:03 IST with NEEDS-DECISION. Camera clipping is not complete. The D3D12 1279x719 combined leg fails D4 and strict texture G3. GO3, the pre-push scan, push and new office card are unfinished. Origin remains c5d0919. Keep using the 094-45r office card.

**Chat review NOT REQUIRED** for this evidence review and preservation of the existing hold. **Chat review REQUIRED** at the next checkpoint: disposition of the uncovered odd-size failure and compression scope, or an explicit owner ruling within the handoff's authority, before dependent dispatch. No gate is relaxed.

## Read order and authority

1. [Owner-approved handoff](CODEX-HANDOFF-094-46s-47.md).
2. [094-46r ruling](094-46r-ruling.md), GP1-GP4; [094-46 ruling](094-46-ruling.md), GO1-GO4; [094-45r ruling](094-45r-ruling.md), GN1-GN5.
3. [Full Code review](094-46s-code-review.md), [full implementer report](094-46s-codex-report.md), and [mailbox report](../_mailbox/to_chat/094-46s-europa-cameraclip-finish.report.md).
4. The receipt and raw-frame evidence below.
5. [Approved 094-47 plan](094-47-codex-controlserver-plan.md), [GQ1-GQ5 ruling](094-47-plan-ruling.md), and [queued brief](094-47-europa-dashboard-packaged.QUEUED.md).

The latest owner instruction authorizes orchestration until about 20:30 IST, with uncovered decisions held for Chat/owner. The new instruction to implement does not waive that condition. The earlier Plan Mode restriction is no longer the blocker.

## Findings

| Gate | Evidence | Disposition |
|---|---|---|
| D4, D3D11 and D3D12 at 1280x720 | Code's complete raw-frame recomputations report all 2,496 unlabelled frames within the measured 2-level limit for each RHI, and 144 labelled frames changing every pixel. | Existing accepted coverage retained. Root did not rerun these full raw-frame checks. |
| D4, D3D12 at 1279x719 | 131 of 2,496 unlabelled frames exceed 2 levels; maximum 117, with 63,341 pixel-frame exceedances. All 131 indices agree between Code's per-frame results and the implementer's stop receipt. | FAIL under GP2. Changing the count from a gate to information does not permit levels above 2. |
| Strict texture G3 at 1279x719 | Positive run has outside-mask changes. Root independently reproduced one outside-mask pixel at y=375, x=239: delta 3 in frame 767 and delta 4 in frame 769. Each sampled frame has IoU 0.9999953764922371. | FAIL under the strict IoU 1.000 / zero-spill requirement. The implementer's 0.98 pass does not satisfy this round's gate. |
| GO3 default-only compatibility | No qualifying default-only D3D12 1279x719 session, key comparison or card reads ran. The combined baseline comparison is information only. | UNRUN. |
| Compression scope | Code reports 57 files compressed, including 24 archived EXE/DLL/PDB copies, with unchanged hashes and modification times. GP1 explicitly allows compression on own receipts only. | Scope disposition remains open. Root did not decompress or otherwise alter them. |

The failing D4 pixels correlate with EA_Floor texture fires. This is evidence of correlation, not a proven cause. No new floor, mask exclusion, size exemption, retry or capture was authorized or performed by root.

Root's independent checks were bounded to determining whether dispatch is permitted: complete aggregation of Code's 2,640 per-frame records and matching all failed indices to stop.json; six raw positive/control frame pairs; and two same-session texture/mask comparisons. This is not a full rerun or new acceptance of H4 or the other gates.

Raw positive/control samples, each compared without resizing, cropping or filtering:

| Frame | Camera-clipping label | Maximum RGB difference | Pixels above 2 |
|---:|---|---:|---:|
| 760 | absent | 2 | 0 |
| 767 | absent | 104 | 349 |
| 769 | absent | 104 | 347 |
| 780 | absent | 2 | 0 |
| 2352 | present | 174 | 919601 |
| 2400 | absent | 2 | 0 |

Evidence: `D:/IA_Europa/Temp/CodeReview09446s/d4gp2-d3d12-1279.json`, `tex1px-1279-positive.json`, `D:/IA_Europa/Receipts/09446s/stop.json`, and raw files under `E:/IA_BuildCache/09446s-evidence/D3D12-1279-{positive,control-a}/01/plugin-frames/epoch01/`. Texture comparisons use positive 767 against 760 and 769 against 762, with the corresponding positive masks. Inline Python used read-only file access and wrote no helper or result files.

## Why 094-47 was not staged

The handoff allows an unpushed starting tip only if the report says the camera changes stand alone and compile. The full reports say that 094-46s changed tools/docs only and reused existing binaries; they do not explicitly clear the camera commits as standalone and compiled for this exception. Source equality to entry is not a substitute for that required disposition.

The queued brief also requires local = origin at the completed 46s tip and reads a 46s office card. Actual local and remote differ, and that card does not exist. The watcher holds the next brief after NEEDS-DECISION unless an ACK states how it was resolved. These failures are unresolved, so root cannot truthfully write that ACK or substitute a pushed 46s tip.

The queued brief is preserved unchanged, with both `<46s TIP>` placeholders. Its SHA256 remains `2F52B1A46DEDCD13F8F8F097D51B35DEAF22D438DEF09BDFD910FF5E0A6D622A`. No `.part` or `.md` was placed in to_code.

The approved 094-47 scope continues to show LOD corruption. Its GQ rulings still govern: omitted seed defaults to 4242; Shipping remains excluded; 5.1 gets a compile check only in this round, with runtime regression required before any 5.1 release merge. No dashboard or plugin implementation was changed by root.

## Exact state and next decision

- Local feat/europa-port: `f3fbba7f93e5943843200a2b64a7d0c4c7f25e9b`.
- Remote feat/europa-port, independently checked with ls-remote: `c5d0919e5074404d26da8c16ccf845b360bde383`.
- Dashboard: master at `7ea2e51bf55e19c2949641bbff2ec72afb4ae493`; root did not modify it.
- Sole watcher PID 29140 alive and idle. Queue empty. Exact relay PIDs 28188 and 29100 absent; all ten PIDs in the completed job's owned-process receipt absent on recheck. No capture/build was abandoned or interrupted.
- No pre-push scan or office card for 094-46s exists. No root push, build, launch, source/index edit, deletion, compression, owner-app action or office-data transfer occurred.
- Root writes this disposition and prepends the current boundary to CURRENT.md, preserving its prior bytes. The existing handoff, approved plan, rulings and queued brief remain unchanged.

Chat/owner must disposition the uncovered odd-size failure before another physical leg or corrective brief: for example, explicitly authorize a bounded diagnostic/correction while retaining the current D4 and strict G3 requirements. Code's suggested far-only 1279x719 diagnostic is a proposal only; it has not become a new floor or authorization. Compression of archived binaries also needs an explicit scope decision. If the owner elects to decouple 094-47, its starting-tip/compile evidence and queued preconditions must be explicitly resolved; camera clipping must remain unqualified. No such election is inferred from the general implementation instruction.

After resolution, recheck live ownership, refs, queue and the ruling before dispatch through the existing watcher. Read the next complete reports and evidence before acceptance. Root remains today's orchestrator; no competing brief or background helper was started. A fresh Codex session is not needed for this continuing workstream.

BLOCKED: 094-46s has unresolved D4 and strict G3 failures; GO3 is unrun and the handoff's 094-47 dispatch conditions are unmet.
