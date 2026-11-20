# REPORT for 078-01-m51-preflight-and-predictions.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-01-m51-preflight-and-predictions.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 14:50:33 | finished: 2026-09-06 15:09:26 | duration: 19 min | exit code: 0
context at last turn: 400038 tokens (40% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-01-m51-preflight-and-predictions.live.txt

---- FINAL MESSAGE (Claude Code) ----
## 1. Plain-language rundown

The source branch is now safely on origin — it was local-only, and 43 files of unreproducible work had no backup. I re-checked all sixteen review findings against current master rather than trusting the review's line numbers (it cited a different tree); **all sixteen reproduce**, and F1 is worse than reported — it silently discards roughly three-quarters of a session, reachable with two shipped default settings. Three things the brief told me to "fix as existing defects" turned out to be **regressions the branch would introduce**, so the work item inverts from "change master" to "don't import". The branch's own auditor would **fail** a correct m51 build. R8's tick-pin gate arm is **unrunnable** on both fixtures, and F2's briefed gate would fail a *correct* static build, so I replaced both designs. m51 needs **no cook** (measured: the branch touches no shader). Predictions are written as DRAFT and no leg may run until you and the reviewer release them.

## 2. Preflight

| item | value |
|---|---|
| `master` = `origin/master` | `ac13700d99fe116056f5aa11611f8931a563a3f2` (queried via `ls-remote`, not tracking ref) |
| `fix/capture-label-integrity` | `be16977c0802f57457ec68edcf8d0f96c12c13d9`, `merge-base` = `21c1e0db134f…` |
| **source backup push** | was **absent** from origin; pushed as new branch, remote verified `be16977…` — nothing overwritten |
| `m51` | created from `ac13700`, now `b532ddd97910c2357bc35e5bfe342ad21d58f11c`, **not pushed** |
| CaptureBench | `472a409dc70ae78d01a38d08fc3bddb15fe41c58` (+7 untracked tools) |
| Lyra worktree | detached `4a4bfcc2cdbade6bbeb99494e540952847ae9b63`, clean (0 entries) |
| staged exe | `B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8`, 241,400,320 B — **re-hashed after all work, unchanged** |
| container | `67EA1FE0…` / `2CEFB8F4…` / `E03C6610…` + `A16A18A8…` / `C70ECDAA…` — unchanged |
| A-side archive | `StackOBot.exe.m50-A159EDFD` (`A159EDFD`) + `m46-container-m50exe-A159EDFD\` holding **all five** container files **and** a second `A159EDFD` exe ⇒ P-C7 v3 A-side fully reconstructable |
| untracked | 10 `CHAT-HANDOFF-*.md` left untouched; `OPERATING-CONTRACT.md` now tracked per your authorization |

Two notes: `_binary_baselines\README.md` L454 still carries the **stale** 241,399,296 B for the gate exe (corrected at L495; disk agrees with L495). **`A9B10CC0` is neither staged nor archived** — correct under G201, recorded so nobody hunts for it.

## 3. Findings — verified at `ac13700`, with current line numbers

| # | verified at master | branch supplies | ruling |
|---|---|---|---|
| F1 | `:2965` effective flag; **`:4195` snapshot stores REQUESTED**; `:3750` drain runs only when effective; `:3801` waits on requested; `:3804-3811` **subsystem-wide** hold blocks whole batch | per-snapshot await + wait counter | R9 |
| F2 | `LabelWriter:106-108` live projection; `:5477-5482` live identity/provenance; **`:5498-5503` membership gated on a live projection** | `FTargetGeometry` frozen | R9 |
| F3 | `MaskMeasure:93` claim, `:71` run-scope clear only; `StencilTag.cpp:41` `IsFree`; pool `200..254` = **55** (`.h:27-29`); census headroom `Census.cpp:714` | retire on fire end | R9 + **R2 not flipped** |
| F4 | **`SVE.cpp:43-47` returns `IsActive()` — no world, no viewport test**; `:57-62` `Views[0]`, no stereo check; `:65` one consume/family; `:93-110` keys on `FrameNumber` alone | world/viewport/single-view filters | R9 + **R8** |
| F5 | `MaskSVE:80-90` `Reset()` **omits `InFlight`**; `:537-552` leaves it; **`Subsystem:2994` serial→0**, `:1159` mints from it ⇒ IDs repeat | `Generation` + monotonic serials | R9 |
| F6 | `:4948-4999` deletes whole event on the **separate** event-level record; **per-frame record never consulted** | veto deleted | **R1 + fork F-b** |
| F7 | `LabelWriter:205-206` **and** `:536-537` append result **discarded** (image write *is* checked at `:194-200`/`:528-530`); `:3998` accumulate before write outcome | commit receipts | R6 |
| F8 | `AsyncWriter:114-122` warns and returns with jobs pending (called 5.0 `:4089`, 2.0 `:601`); `:133-137` counter reset; writer created `:3719` | per-session writer, join | R6 |
| F9 | `ServiceTargetMask` `:1168` — no pass-validity/contamination checks | shared validity | R9 |
| F10 | `:4379-4387` membership before deferred sampling; `:4657` null-actor shortcut | joint sampling | R9 |
| F11 | `Viewport:172-174` **`bConstrainAspectRatio = false`**, aspect from viewport; `:342` discards behind-camera corners | real matrices + clipping | R9 |
| F12 | **both** `Anomaly_MissingTexture.h:30` and `Anomaly_CorruptedTexture.h:30` `TWeakObjectPtr` | `TStrongObjectPtr` | R9 |
| F13 | `PreviewTee:52` arm++, `:67` decrement only on completion, `:34-36` busy gate | guard + cancel | R9 |
| F14 | `LabelWriter:448` no globals; **`:4277` sync uses `IsHidden()`** vs async `IsLogicallyHidden` (`:4629`,`:4657`) | parity | R9 |
| F15 | `ControlServer:410/445/457` — no capture gate | WS refusal + console no-op | **R4 (console half rejected)** |
| F16 | `LabelWriter:222-243` four cases + black `default:` | layout gate | R9 |

**Three items sharpened beyond the review.** *F1 discards, not merely stalls:* the drain settles at ~1 frame per 4–5 ticks (the review's 29-of-120 model is arithmetically consistent with the code), `DrainAsyncToCompletion` gets **8** iterations, then `:4082-4086` **empties `PendingSnapshots`** — a 90-frame session ships ~a quarter of its frames, triggered by `OutputHeight != 0` (m28) on top of the mask's compiled default ON (m43). *F6 is a live contradiction inside the shipped binary*, not just a finding: m26 deletes events without consulting the per-frame record m49 added, while m49 rules zero-observable events stay. *F14 violates a locked CLAUDE.md invariant* — m45 stopped hide-class anomalies setting `bHidden`, so the sync path reports **every hide as non-manifested**.

**Refuted premises (measured):**
1. **The three "cleanup" items are branch-introduced, not master defects.** `SessionGlobalPositive/NegativeFrames` are incremented on master (`:4252/:4256/:4383/:4387`) and **nowhere** on the branch; `TargetMaskHoldTicks` is **live** on master; the silent `Published` hard-reset is the branch's (`MaskSVE:113` clears every live batch) and **master has no such map** — its key ring already does FIFO eviction with a `Wrapped` counter. Work item inverts to *do not orphan / do not import*.
2. **The branch auditor would fail a correct build** — asserts `anomaly_present == observable` (L151, reversed by R1), `shaders_pending` per **row** not per fire (L116-117, reversed by R5), `affected_frames ==` observable set (L168, breaks R1's fallback).
3. **R8's tick-pin arm is unrunnable** — `tickpin_compiled = false` on **both** fixtures (banked `PB1G_BL_NAT`, `LYRA_SMOKE_01`).
4. **Handoff §1's hide-class predicate is superseded** by `1c3d62c`/G258 — recorded, not retro-edited.

## 4. Architecture and gates

**Decisions:** cancellation is a terminal outcome with **immediate** capacity release (the R8 defect; 8→32 only delays it), and an obsolete completion is discarded by `Generation` so it can neither resurrect an entry nor double-decrement. A receipt carries the **final** disposition: a mask-write failure downgrades **both** row and event observation while the colour frame still commits; an image/label failure means no committed frame. **`capture_complete:false` means a promised artifact is missing** — write/readback/unrendered/mask drops — while an *intentionally unavailable* measurement (Nanite, mask off, `OutputHeight≠0`, shaders pending) leaves the session **complete and full of honest `-1`s`**. Phase B survives as behaviour: PB-5 both directions, and truth-table row I (hide-class, `DrawnCount > 0`, still `observable: true`) is the G258 ruling that keeps a true positive label on a real host. Console mutations stay open and counted (bench levers depend on them), with attempts *and* outcomes logged so a no-op can't masquerade as a lever (G245).

**Gate matrix:** 16 new legs + throughput + auditor, each with lever, expected reading, independent observation, stop condition; full regression set unchanged; one binary **per fixture** (the two games cannot share an exe). Three departures from what was briefed:
- **F2's gate replaced.** "frozen bbox == drawn bbox" fails a *correct* static build — a projected AABB is not a silhouette, and m30 measured that gap at ~4× (G149). Replaced with a **determinism** test: `bbox_px` invariant to readback latency, A/B on one seed, pre-fix side diverging with target speed. No synthetic truth needed.
- **R8 runs on `IAI.Capture.SVE.ForceMiss`** (same `PendingSnapshots` residue as an unrendered tick), with its limitation stated — it proves the recovery path, not decoupled-host behaviour. `SynthTickOrder` does **not** substitute, and `ticks_per_captured_frame > 1` is **not** evidence of missed renders (108/90 and 122/90 banked are lead-in and settle).
- **Expected auditor exit declared per leg**, so "failure legs must fail" and "natural legs must pass" cannot contradict each other.

**Measured, and it de-risks the whole milestone: `git diff 21c1e0d..be16977 -- Shaders` is EMPTY** ⇒ no cook, and Phase B's RT1 + reduce stride survive by construction. **Trap stated:** `master..be16977` shows Phase B's additions as **deletions**, so merging the branch as a unit deletes Phase B; six files overlap and the port is hunk-by-hunk.

## 5. Commit and state

**Commit `b532ddd97910c2357bc35e5bfe342ad21d58f11c` on `m51`** — `docs(m51): 078-01 preflight …`, **896 insertions, 1 deletion, documentation only** (verified: no staged path outside `docs/` and `CLAUDE.md`):

- `docs/sessions/2026-09-06-078-m51-preflight.md` (new, 305)
- `docs/predictions/2026-09-06-m51-release-hardening.md` (new, 445) — SHA-256 **`4A676AD2B08F859C3A5F147CD75C42FA29DAA69DF83086F6217C4F656BC2BDED`**
- `docs/OPERATING-CONTRACT.md` (added to VCS, body preserved verbatim, §9 appended incl. the DrawnCount correction)
- `CLAUDE.md` (+84/−1 — proportionate to a block insert, no whole-file re-encode; G115 check passed)

Checkout is on `m51`. **`master` and `fix/capture-label-integrity` are unmoved as commits.** The only remote write was the authorized source backup. **No implementation, build, cook, capture, leg, test run, binary staging, worktree change, merge or tag occurred**; bench exe/container and the Lyra worktree re-hashed identical afterwards; nothing written to `_mailbox`; no office data touched.

## 6. NEEDS-DECISION

**Blocking:** written adversarial review of `docs/predictions/2026-09-06-m51-release-hardening.md` before the first leg, per the operating contract §1 and §5.

Then three technical forks, with my preferred resolutions:

- **F-a — does `masks_dropped > 0` alone set `capture_complete:false`?** *Proposed **YES***: a promised artifact failed. Cost: the R6 mask-failure leg then fails `--integrity-only`, which is correct for a failure leg but must be declared per leg.
- **F-b — the m26 veto: re-point or delete?** The branch deletes it. *Proposed **re-point its input***: veto iff ≥1 frame measured **and** every measured frame reads `target_pixels == 0`. Deleting retires `vetoed_events` — a `run_summary` key the client already holds — to a permanent 0 (G96's shape) and re-admits a class of events the client's dataset has never contained. Re-pointing satisfies the review's actual requirement (derive from committed per-frame evidence; no observation from another time overrules it) because the veto stops being an independent measurement.
- **F-c — accept that F4's second-view lever may be unobtainable on both fixtures**, leaving that gate reported UNRUNNABLE with correspondingly weaker evidence rather than converted into a passing unit test?

VERDICT: NEEDS-DECISION
api-equivalent cost (not billed on Max): $12.19 | turns: 57 | running total across all briefs: $1,094.35
