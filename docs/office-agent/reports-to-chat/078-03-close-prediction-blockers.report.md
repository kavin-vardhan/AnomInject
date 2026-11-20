# REPORT for 078-03-close-prediction-blockers.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-03-close-prediction-blockers.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 15:45:07 | finished: 2026-09-06 16:17:36 | duration: 32 min | exit code: 0
context at last turn: 513116 tokens (51% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-03-close-prediction-blockers.live.txt

---- FINAL MESSAGE (Claude Code) ----
Committed and verified. Everything in the scope fence held.

## 078-03 report

**Commit `760ad947` on `m51`** (parent `adb0d4d`). `master` unchanged at `ac13700`. 5 files, +1782/−8.

| path | state |
|---|---|
| `docs/predictions/2026-09-06-m51-release-hardening.md` | AMENDMENT 2 appended (314 lines) |
| `docs/predictions/2026-09-06-m51-gate-manifest.json` | **new** — 40 recipes, 147 expansions |
| `docs/sessions/2026-09-06-078-03-m51-bounded-corrections.md` | **new** |
| `docs/OPERATING-CONTRACT.md` | §9 dated entry |
| `CLAUDE.md` | status block |

**Opening state matched the brief exactly** — branch, HEAD, master, `be16977`, prediction SHA256 `725CD30F…`, ten untracked handoffs. No discrepancy to preserve. All eight A1.13 StackOBot hashes **recomputed and reproduced exactly**; staged exe == archive byte-identical.

### Dispositions

**C1 — done, and the premise was stronger than stated.** `StaleArmMaxTicks` deleted; expiry is by render identity (arm's `GFrameCounter` vs publish), GT sweep before admission *and* the cap check, published GPU work never swept, capacity retired exactly once, same-session terminal guard. The correction that matters: **there is no publish-time check today at all** — `ConsumeWantedForPublish` (`AnomalySveCapturer.cpp:45+`) pops FIFO with no frame comparison, so A1.6 spoke of *keeping* something absent. `NoStaleArmSweep` now bypasses **both** retirement routes while retaining the identity guard. `WithholdWorldRender`'s mechanism confirmed (the plugin's `BeginRenderViewFamily` is downstream of the `bDisableWorldRendering` gate).

**C2 — done; one item honestly left OPEN.** Soft reservation rejected; N=2 / 256 MiB including in-service; deterministic per-buffer reservation with arithmetic (15.88 MiB per 1080p 8-bit colour job ⇒ N binds, cap is headroom; first oversize single job at >33.5 Mpx). No `EncoderReserveFactor`, no oversize exception. **The exact source reason you asked for:** `IImageWrapper::GetCompressed` (`IImageWrapper.h:164`) returns a whole `TArray64<uint8>` with no size hint, maximum, streaming variant or allocator hook ⇒ **an a-priori bound on encoded output is not obtainable through that interface**. Codec scratch is bounded structurally by N and input size; a true byte bound is an **OPEN implementation prerequisite**, not solved design. Saturation is now a runtime worker delay with numeric predictions; the `WriterQueueBytes` arm survives only as validation recipe `M-W4v`.

**C3 — done, and A1.5 was wrong in a way the brief anticipated only half of.** `AccumulateFrameEvents` runs in the **drain** (`:3998`), so an armed-but-never-drained frame is in **neither** ledger — `injected_frames` is not a faithful requested ledger today. Separately, the writer is fire-and-forget *after* accumulation, so a drained-but-write-failed frame is in `injected_frames` with an observability verdict and no image. Both feed the R1 fallback. `mask_write_required`, prepublication-vs-publication split, `observability_basis`, the auditor rule that must **not** demand observable-only on a fallback, `IsFireLabelledThisFrame` named, second-context refusal — all specified.

**C4 — done, with three real corrections.** Manifest carries per-recipe executable commands (16 harnesses, CLIs read from the scripts), `planned, not implemented` for `m51_bbox_oracle.py`, `m51_reconcile.py` and the rewritten auditor, per-leg allowlists including the omitted row fields, and full 64-char provenance with Lyra resolved to actual DLL names. **Three A1.9 falsifiers were wrong, not missing** — `NoTagRetire` *prevents* the reuse M-F3b needs; forcing both fires pending cannot discriminate; synth order is not a view-binding bypass.

### Remaining gaps, stated not papered over

- **A bounded encoded-output guarantee is OPEN** (C2 item b) — needs a streaming encoder `IImageWrapper` does not offer.
- **M-F11 and M-F10 have no defect injector**, recorded `MISSING`.
- **M-F4's A-side is OPEN** — no editor-target archive exists for either fixture and a packaged exe cannot host PIE.
- AMENDMENT 1's four open arms stand unchanged.
- **AMENDMENT 2 ran 314 lines against your ~250 guidance.** I cut it from 353 and stopped; every remaining paragraph carries a mandated element, and I judged going under the line by dropping required content — or by moving design rationale into the manifest — worse than reporting the overrun.

### No-implementation confirmation

`git diff ac13700 HEAD -- Source Shaders tools` is **EMPTY**. No build, cook, leg, capture, staging, push, merge, tag, worktree change, office action or mailbox touch. All other refs unmoved; staged bench exe still `B8AC65D8`; Lyra worktree detached `4a4bfcc`, clean. The ten owner handoffs remain untracked. The commit-message temp file was written outside the repo and deleted.

VERDICT: NEEDS-DECISION
api-equivalent cost (not billed on Max): $22.68 | turns: 84 | running total across all briefs: $1,132.15
