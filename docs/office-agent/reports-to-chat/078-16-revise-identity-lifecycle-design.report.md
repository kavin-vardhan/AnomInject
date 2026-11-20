# REPORT for 078-16-revise-identity-lifecycle-design.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-16-revise-identity-lifecycle-design.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-07 08:35:15 | finished: 2026-09-07 09:08:47 | duration: 34 min | exit code: 0
context at last turn: 554509 tokens (55% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-16-revise-identity-lifecycle-design.live.txt

---- FINAL MESSAGE (Claude Code) ----
Bounded documentation unit complete. Nothing ran; nothing is in flight.

## Deliverable

**`docs/predictions/2026-09-07-m51-identity-lifecycle-geometry.md`** — revised **in place**
- SHA-256 `B4BD570349D192E0C040D3E9450C287960E8A0104E591D45F129C19E356E5F8C`, **1,078 lines**, 88,919 bytes, LF, no BOM
- Supersedes the reviewed `a031b46` snapshot (`021B26E9…F672B3F`, 664 lines), which is retained in history

**Commit `560e28ac2b88f6c65f1683970ebb5ab6c69bb969`** on `m51`, five named paths, `5 files changed, 1225 insertions(+), 554 deletions(-)`:

| file | numstat | shape |
|---|---|---|
| `docs/predictions/…-identity-lifecycle-geometry.md` | 887/462 | revised in place, dated 078-16 header + R1–R7 disposition table |
| `CLAUDE.md` | 50/90 | **52-line block replaces the 92-line 078-15 block**, hunks confined to lines 14–104 |
| `docs/OPERATING-CONTRACT.md` | **84/0** | §9 pointer at the 078-15 heading + new dated `078-16` entry |
| `docs/sessions/…-078-15-…predictions.md` | **82/0** | top pointer + appended §9 correction; historical bytes preserved |
| `docs/sessions/…-078-16-…revision.md` | 131/0 | new journal |

`git diff --check` clean. CLAUDE.md verified uniformly CRLF (6130/6130, zero lone LF, zero lone CR, no BOM); the other four LF, no BOM.

## Review input actually used

`_reviews/078-15-codex-draft-review-and-request-to-chat.md` and `_reviews/078-16-codex-takeover-and-review-clarification.md`. ⚠ **`_reviews/078-15-chat-identity-lifecycle-draft-review.md` was checked and is ABSENT.** No Chat response existed, its absence was **not** treated as approval, and no Chat input was used. Stated in the design header, both journals and the contract entry.

## R1–R7 disposition, with the source that decided each

**R1 — ACCEPTED** (§A.1, §A.2, §A.4, §A.5, §D.1). Domain table completed with scope/lifetime/reset/wrap. Confirmed independently: `FSceneViewFamily::FrameNumber` is **scene-local** (`SceneRendering.cpp:4552-4563` increments only on `bIsFirstViewInMultipleViewFamily` and assigns **one value to every family in the loop**), the scene-less fallback writes `GFrameNumber` into the **same uint32 field** (`:4564-4571`), and `LookupKey` does not consume (`AnomalySveKeyRing.cpp:132-148`). Colour eligibility is narrower than the draft claimed — `Views[0]` and two flags only (`AnomalySceneViewExtension.cpp:57-58`). **Resolved interfaces named with source:** `FSceneViewExtensionContext` already carries `Viewport`, `Scene` and `GetWorld()` (`SceneViewExtensionContext.h`), `FSceneViewFamily::Scene`/`FrameNumber`/`bIsMultipleViewFamily` are readable on both threads (`SceneView.h:1766`, `:1784`, `:1807`), and both extensions already own `BeginRenderViewFamily`. Topology binding moved to step **2a′, before** the behaviour change.

**R2 — ACCEPTED** (§B.1–§B.6). Six orthogonal owners; publication separated from submission, with the reconciliation term that **already exists and has never been read**: `Handshake.Matches` − `Submits`. Per-domain equation carries an `outstanding` term with a **quiescent-only** zero; `Measure`/`Census` terminate `Resolved` (no image, no row). **`Cancelled(payload)` deleted**, replaced by `Failed(payload)` reachable only after submission. Bounded terminal ring (4 × `ReadbackPendingCap`, key-only) + per-`(Generation, Domain)` watermark. `A1.6`'s tag rule (fire-end **and** `ArmsIssued == ArmsResolved`, release-hardening:701-705) preserved verbatim.

**R3 — PARTLY ACCEPTED, PARTLY BLOCKED** (§C.1, §C.3–§C.5). Matrix/jitter/rect provenance specified: `GetProjectionNoAAMatrix()` vs the jittered matrix whose TAA offset shears the same `M[2][0]/M[2][1]` as `OffCenterProjectionOffset` (`SceneView.h:340/345/443-449`; `CameraStackTypes.cpp:124-132`); host SVEs can rewrite matrix **and** rect (`LocalPlayer.cpp:1151-1154`). 🆕 **Destruction silently removes frames**, not merely stales a box (`AnomalyCaptureSubsystem.cpp:5525-5536`). ⛔ **The GT-bounds-to-rendered-proxy equivalence is BLOCKED, not solved.**

**R4 — ACCEPTED, claim RETRACTED** (§A.3). *"Today the frame that would be mis-bound is exactly the frame that reports unmeasured"* is withdrawn, with a counter-trace: colour is FIFO one arm per family (`AnomalySveCapturer.cpp:56-57`), the mask drains **all** arms into one render, `PixelOwner` is the **first** wanting index (`AnomalyMaskSceneViewExtension.cpp:512-520`) — so the arm that gets pixels and reports a **number** can be the stale one. The request-ID join and `PixelOwner` mechanics survive.

**R5 — ACCEPTED, claim RETRACTED** (§C.2). Algebra finished: `CalculateProjectionMatrix` uses the 4-arg `TReversedZPerspectiveMatrix` (`PerspectiveMatrix.h:140-147`) where `MinZ` sits at `M[3][2]` and reaches **`Clip.Z` only**, while `ProjectBoxToNormalizedRect` (`AnomalyViewport.cpp:342-375`) reads **only `Clip.X/W`, `Clip.Y/W`**. Matrix-depth provenance stands; the bbox exposure does not; the real defect is **absent near-plane clipping**. `M-GEO2` **withdrawn**.

**R6 — PARTLY ACCEPTED, PARTLY HELD** (§D.4–§D.6). Denominators, floors, outcome classes, healthy+injected controls, schedules and restore added. `N-ID2` gains a **≥50 % measured floor** so all-unmeasured rows cannot pass. `WithholdWorldRender` traced: `CaptureCurrentFrame` runs only in `LeadIn`/`Positives`/`PostGap` (`:690`, `:700`, `:710`), **not** the settle phases — **a burst in a settle phase arms zero.** `N-LC2` is **OPEN on its can-fail device**; the encoded probe is on explicit hold and the categorical "in-pixel validity impossible" claim is withdrawn in favour of a four-class codebook.

**R7 — ACCEPTED, with a correction to R7 itself** (§D.5, §D.7). Real names `IAI.Capture.SVE.ForceMiss` / `…ForceMissPhase` (`AnomalySveKeyRing.cpp:17`, `:24`); no `IAI.Bench` alias exists. 🔻 **R7's "throughput validity rule" characterisation is WITHDRAWN** against the bytes — frozen-pairing `A2.1`:567 adopts no throughput precondition, `A2.3`:617-628 calls same-row FPS/`speed_ratio` *"a legibility requirement, not a gate"*. **`A1.10`/`A2.2` and recipe `T` still require `speed_ratio <= 1.05`, 1080p native, 600 requests (first 60 warm-up for timing only), both fixtures, both orders, zero drops, zero holes.** No gate policy changed.

**Manifest contradictions carried visibly** (file byte-unchanged, `D33AF5D4…6025F9`): `existing_levers_used` **and** `M-R8b` carry the wrong cvar names ⇒ **CORRECTION REQUIRED**, plus wanted-request accounting (`ShouldCorrupt` keys on `PublishSerial`, which counts **all** publishes) and an archive-provenance claim resting on the wrong name. **`M-R8c` contradicts itself twice**: a bare-constant `unrendered_ticks == 3` the FSM refutes, and a falsifier that *removes* the identity rejection when `A2.1` and `planned_levers` require `NoStaleArmSweep` to **retain** it.

## Superseding the 078-15 final report

The historical 078-15 report was **not edited**. This report supersedes these of its claims: *"The mis-bound frame reports unmeasured today only by **ordinal luck**"* and *"the label bbox uses the live near plane"* as a bbox-exposure claim — **both retracted above**. Its statement that the crosswalk proposes **three** new rows is superseded: `M-GEO2` is withdrawn, leaving **two** (`M-ID1`, `M-ID2`).

## Unresolved implementation blockers (named, unschedulable)

1. **Mask extension activation predicate** — `IsActiveThisFrame_Internal` returns `PendingArms.Num() > 0` (`AnomalyMaskSceneViewExtension.h:47`), decided at `GatherActiveExtensions` (`GameViewportClient.cpp:1292`) **before** `BeginRenderViewFamily`; that is the same state a publication design must move.
2. **GT-bounds-to-rendered-proxy equivalence** — `SendAllEndOfFrameUpdates` (`SceneRendering.cpp:4527-4528`) ordering alone is not proof, and reading an RT proxy later on GT is not proposed.
3. **Backbuffer identity bridge — OPEN.** No family, no `FrameNumber`, no context at `OnBackBufferReadyToPresent`. ⛔ **NOT disabled, NOT certified**; `M-F1b` still UNRUN.
4. **`N-LC2`'s can-fail device does not exist** (cancellation *after* submission).

**One hypothesis formed and refuted mid-session, recorded so it is not re-formed:** planar-reflection arm theft — `SetupViewFamilyForSceneCapture` sets `bIsSceneCapture` unconditionally (`SceneCaptureRendering.cpp:625`), so the existing guard catches them.

## Deviations, stated

- **The design grew 664 → 1,078 lines** — a direct consequence of R1–R3 asking for completed interfaces where the draft listed intentions. Flagged so the reviewer can judge reviewability.
- **The CLAUDE block is 52 lines against the brief's `<= 40` aim** (down from 92). Three tightening passes were made; the retraction text is load-bearing for a cold reader who reads only CLAUDE.md.
- **No binary string scan was run**, so whether `IAI.Capture.SVE.ForceMiss` is present in `B8AC65D8` is **UNVERIFIED**; the design says so rather than inheriting the manifest's claim.

## Scope verification

HEAD `560e28a`; `master` == `origin/master` == `ac13700`; `6713f51..HEAD -- Source Shaders` **empty**; ten owner handoffs preserved untracked; CaptureBench `472a409` tracked-clean + seven prior untracked tools; Lyra detached-clean `4a4bfcc`; staged exe and candidate archive both `D50DDE78…4796`; A archive `B8AC65D8…8EEAD8`; manifest `D33AF5D4…6025F9` and frozen-pairing `8D7951F5…D2E9B5` unchanged. Scratch was outside every repository and mailbox. No capture, checker run, build, cook, staging, swap, source/shader/tool/fixture/schema edit, owner-app action, merge, push, tag, replay, interleave, agent or worktree. **No runtime validation is claimed. Nothing is in flight.**

**Held, unchanged:** no valid candidate pairing baseline (four attempts INVALID, unretried, no cause established in either direction, no verdict of any kind) · 078-05/06 valid failures remain failures · B1 untouched, no order-insensitivity claim, no `F1` clearance · interleave not adopted · writer byte bound OPEN · publication/orphan/auditor HELD · all sixteen findings, every `open_arms` entry and the full regression set required · exhausted campaign never auto-replayed.

VERDICT: NEEDS-DECISION
api-equivalent cost (not billed on Max): $28.89 | turns: 102 | running total across all briefs: $1,341.29
