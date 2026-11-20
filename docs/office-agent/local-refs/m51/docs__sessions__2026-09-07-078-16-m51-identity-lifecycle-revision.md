# Session 078-16 - 2026-09-07 - m51 identity / lifecycle / geometry: bounded documentation revision

**Brief 078-16. DOCUMENTATION ONLY.** No implementation, capture, checker run (including old-bank
diagnostics), build, cook, staging, binary swap, source / shader / tool / fixture / schema edit,
owner-app or office action, merge, push, tag, replay or interleave. No agents, no worktrees. Nothing
ran. **The design is not approved and the campaign remains INCOMPLETE.**

⛔ **This journal is a record of the correction, not a second copy of the design.** The design is
`docs/predictions/2026-09-07-m51-identity-lifecycle-geometry.md`.

> 🔻 **PARTLY SUPERSEDED BY 078-17, 2026-09-07 — READ §8 AT THE END OF THIS FILE BEFORE TRUSTING §3 OR
> §4.** The independent review of the exact bytes this session produced found `R1`, `R2`, `R3` and `R6`
> still contradictory or incomplete; **§3's claim that a resolvable interface was found, §3.5's "three
> subunits" framing, §3.6's bounded terminal ring, and §4's line/byte metrics are all corrected there.**
> The body below is **preserved as the historical record and is not retro-edited.**

---

## 1. Goal

Correct the unexecuted 078-15 identity/lifecycle/geometry draft against Codex findings `R1`-`R7`
(`_reviews/078-15-codex-draft-review-and-request-to-chat.md`) and the exact-source clarification
(`_reviews/078-16-codex-takeover-and-review-clarification.md`); produce a coherent reviewable revision
that **resolves interfaces where source supports them and names implementation blockers where it does
not.** Five writable paths only.

⚠ **`_reviews/078-15-chat-identity-lifecycle-draft-review.md` was checked and is ABSENT.** No Chat
response existed; **its absence was not treated as approval**, and no Chat input was used. A later
response must be read in full.

## 2. Work

Read in the brief's order: root `AGENTS.md`; the full operating contract; the current CLAUDE status;
both Codex reviews in full; the 078-15 final report, the exact 664-line draft and the 078-15 journal;
the original 078-15 acceptance brief; 078-14 Chat's evidence review; the 078-15 offline findings and
scope; 078-07's request-ID evidence; release-hardening `A1.6`/`A1.10`/`A2.1`/`A2.2`/`A2.3`, the
frozen-pairing `A2.1`/`A2.3` bytes, and the gate manifest's `M-R8`/`M-R8b`/`M-R8c`/`T` records,
`planned_levers`, `planned_tools` and `open_arms`.

Then **re-read actual plugin and local UE 5.1 source at every `R1`-`R7` lead** rather than citing the
prior report. Source consulted this session includes `AnomalySveKeyRing.cpp`,
`AnomalySceneViewExtension.{h,cpp}`, `AnomalyMaskSceneViewExtension.{h,cpp}`, `AnomalySveCapturer.cpp`,
`AnomalyCaptureSubsystem.cpp`, `AnomalyMaskMeasure.cpp`, `AnomalyViewport.{h,cpp}`, and engine
`SceneRendering.cpp`, `ScenePrivate.h`, `SceneView.h`, `SceneViewExtension.h`,
`SceneViewExtensionContext.h`, `LocalPlayer.cpp`, `CameraTypes.h`, `CameraStackTypes.cpp`,
`PerspectiveMatrix.h`, `SceneCaptureRendering.cpp`, `PlanarReflectionRendering.cpp`.

## 3. The corrections that changed what the record claims

Full detail is in the revised design's `R1`-`R7` disposition table and in §9 of the 078-15 journal.
The load-bearing ones:

1. **🔻 The ordinal identity claim is RETRACTED** (`R4`). *"Today the frame that would be mis-bound is
   exactly the frame that reports unmeasured"* was a **conditional inference** presented as fact, and
   the source permits the opposite ordering. The banked request-ID join and `PixelOwner` mechanics
   survive; the identity assignment does not.
2. **🔻 The near-plane bbox claim is RETRACTED** (`R5`), with the algebra finished:
   `TReversedZPerspectiveMatrix`'s `MinZ` occupies `M[3][2]` alone and reaches only `Clip.Z`, while
   the projector reads only `Clip.X/W` and `Clip.Y/W`. **Matrix-depth provenance** (real) and **absent
   near-plane clipping** (the actual geometry defect) are now separate facts, and `M-GEO2` is
   withdrawn as a falsifier.
3. **The mask path has no game-thread publication point at all** - `BeginRenderViewFamily` is an
   **empty override** - so `A2.1`'s publish-time identity rule cannot be transplanted line for line.
   This is the single largest correction to the design's premise (`R1`, `R2`).
4. **A resolvable interface was found where the draft had an aspiration** (`R1`):
   `FSceneViewExtensionContext` already carries `Viewport`, `Scene` and `GetWorld()`, and
   `FSceneViewFamily` exposes `Scene`, `FrameNumber` and `bIsMultipleViewFamily` on both threads. The
   revision specifies a common `FAnomalyPublication` minted idempotently on the game thread, with
   one-time claims and topology refusal, and moves that binding **before** the behaviour change.
5. **Three subunits are marked NOT IMPLEMENTATION-READY** rather than implied resolved: the mask
   extension's activation predicate, the GT-bounds-to-rendered-proxy equivalence, and the **backbuffer
   path's identity bridge** (which stays OPEN - not disabled, not certified).
6. **Accounting is separated** (`R2`): six orthogonal owners, publication distinguished from
   submission, a per-domain equation with an `outstanding` term and a quiescent-only zero,
   `Cancelled(payload)` deleted, and a **bounded** terminal ring plus watermark rejection.
7. **Manifest contradictions are carried visibly** (`R7`): the real cvar names
   (`IAI.Capture.SVE.ForceMiss` / `…ForceMissPhase`), `M-R8b` marked **correction required** with its
   wanted-request accounting and archive-provenance problem, and `M-R8c`'s two internal
   contradictions - a bare-constant arm count that the capture FSM refutes, and a falsifier that
   disables the identity guard `A2.1` requires it to retain.
8. **`R7`'s own throughput wording is corrected**: frozen-pairing `A2.3` is same-row
   **telemetry/legibility**, expressly not a gate. Release-hardening `A1.10` and manifest `T` still
   require `speed_ratio <= 1.05` at 1080p / 600 requests / both fixtures / both orders. **No gate
   policy changed.**

## 4. Deviations

- **The revised design grew** from 664 lines / 46,971 bytes to **1,074 lines / 88,592 bytes**. That is
  a direct consequence of `R1`-`R3`, which asked for completed domain/ownership/geometry interfaces
  where the draft listed intentions. It is stated so the reviewer can judge whether it is still
  concise enough to review adversarially.
- **The crosswalk now proposes TWO new rows, not three.** `M-ID1` and `M-ID2` remain proposed;
  **`M-GEO2` is withdrawn** by correction 2. The manifest file is unchanged.
- **One hypothesis was formed and refuted mid-session, and is recorded rather than dropped:** that
  planar-reflection families steal colour arms because the colour SVE's family guard omits
  `bIsPlanarReflection`. `SceneCaptureRendering.cpp:625` sets `bIsSceneCapture` unconditionally on that
  path, so the existing guard catches them. It is written into the design (§A.2) so it is not
  re-formed.
- **No binary string scan was run**, so whether the *correct* `IAI.Capture.SVE.ForceMiss` string is
  present in the frozen A-side archive `B8AC65D8` is **UNVERIFIED**; the design says so rather than
  inheriting the manifest's claim.

## 5. State at the end

`m51` HEAD moves only by this session's single path-scoped documentation commit. Production frozen
`6713f51`; `master` == `origin/master` == `ac13700`; ten owner handoffs preserved untracked;
CaptureBench `472a409` tracked-clean with its seven prior untracked tools; Lyra detached-clean
`4a4bfcc`; staged exe re-read as candidate `D50DDE78…4796` and untouched; gate manifest byte-unchanged
at `D33AF5D4…6025F9`; frozen-pairing predictions unchanged at `8D7951F5…D2E9B5`. **No runtime
validation is claimed and nothing is in flight.**

**Held, unchanged:** no valid candidate pairing baseline (the four attempts stay INVALID, **no cause
established in either direction**, no valid pairing verdict of any kind) · the 078-05/06 valid pairing
failures remain failures, root cause OPEN · B1, the pairing predicate, the coverage floor, the attempt
budget and the stop taxonomy untouched · **no global order-insensitivity claim and no `F1` clearance**
· the A/B/A/B/A/B interleave **not adopted** · the exhausted campaign **never auto-replayed** ·
`M-F1b` UNRUN · the writer byte bound OPEN · publication / row-orphan / auditor semantics HELD for
F7/F8 · all sixteen findings, every `open_arms` entry and the full regression set still required ·
Lyra needs its module SET plus manifest.

## 6. Review cadence recorded

Per the owner's delegation: **Codex may drive bounded, source-backed documentation corrections without a
Chat round after each one.** Chat reviews the **coherent corrected identity/lifecycle/geometry design
before implementation**, the **final predictions before the first physical leg**, and
**evidence/source before merge**, and is re-engaged for material correctness or gate changes or
contradictory evidence. Owner-only meaning, physical and delivery boundaries are unchanged. **This
unit releases no implementation and no physical leg.**

## 7. Next

Codex reviews the exact revised bytes; then Chat reviews the coherent corrected design **before any
implementation**. ⛔ Do not implement, do not run a leg, do not change B1, do not edit the manifest,
and do not start any other unit unprompted.

**VERDICT: NEEDS-DECISION** - the revised design awaits exact-design review; document completion is
not design, execution or release approval, and the campaign remains INCOMPLETE.

---

## 8. 🔻 CORRECTION 078-17, 2026-09-07 - what this session claimed that the review did not sustain

**Appended, not rewritten.** Everything above is preserved. Where these and the body disagree, **these
govern.** Source: `_reviews/078-16-codex-independent-review-and-request-to-chat.md`, the independent
review of the exact bytes committed by this session (`560e28a`). ⛔ **Documentation only: nothing ran,
no gate moved, no leg was re-run, no negative result was erased, and the manifest is byte-unchanged.**
The corrected design is the same file, revised in place under brief 078-17.

**(1) 🔻 The `R1`/`R2` "ACCEPTED" labels are withdrawn.** §3 and the final report recorded them as
dispositions; **they were this session's dispositions of review input, not approval**, and the
independent review found `R1`, `R2`, `R3` and `R6` still carrying contradictions or incomplete
interfaces. **`R1`-`R7` remain stable review IDs.**

**(2) 🔻 §3.4 - "a resolvable interface was found where the draft had an aspiration" is too strong.**
`FSceneViewExtensionContext` and `FSceneViewFamily` do expose the **values** named; that part stands.
**What does not:** a record minted by one extension and then **mutated** by the second is not an
immutable sealed publication (no final seal, no expected consumer set), and **the object cannot simply
travel to the render thread** - `FViewFamilyInfo` copies the family (`SceneRendering.cpp:2402-2406`),
`FSceneRenderer` copies it again (`:2415-2417`), `FViewInfo` copies each view (`:819-826`), and
`:2453` re-points the view at the renderer's copy. ⇒ **no raw game-thread pointer transfers, and no
carrier is designed.** World/viewport keys are **raw opaque pointers with no lifetime token**.

**(3) 🔻 §3.5's "three subunits" understates the holds.** Also OPEN: the publication protocol itself,
per-index reconciliation, the geometry return join / transform / clipping / jitter convention, and the
world/viewport lifetime token. **The activation predicate and the backbuffer bridge are retained holds,
not the only ones.**

**(4) 🔻 §3.6's bounded terminal ring is WITHDRAWN as unsound.** A monotone `RetiredWatermark` over
**out-of-order** retirement can reject a **live** request's genuine completion, and per-index
reconciliation cannot be reconstructed from aggregate counts. **The obligation replaces the mechanism;
no substitute is invented.**

**(5) 🔻 Two accounting statements this session shipped are corrected.** `Failed(payload)` was
described as *"reached only after submission"* while this session's own worked traces reached it with
no submission - the lookup miss returns at `AnomalySceneViewExtension.cpp:96-105`, **before**
`:147`/`:149`/`:159`. And *"Owner 3 begins at `AddEnqueueCopyPass`, not at pass construction"* is
self-contradictory: `AddEnqueueCopyPass` **is** `GraphBuilder.AddPass` (`RenderGraphUtils.cpp:842-855`).
⚠ **The 078-16 final report carries the same post-submit-only wording and its acceptance framing; it is
an immutable artifact and is NOT edited - it is superseded by this section.**

**(6) 🔻 §4's metrics are stale and are corrected here.** The final committed design was **1,078 lines /
88,919 bytes**, SHA-256 `B4BD5703…E5F8C` - **not** the 1,074 lines / 88,592 bytes recorded in §4, which
was measured before the last edit. The design file's actual commit numstat was **878 insertions / 464
deletions**; the final report's table said 887/462, which was inaccurate (its overall diffstat,
1,225/554 across five files, was correct).

**(7) ✅ Preserved and not inflated:** corrections 1, 2, 7 and 8 of §3 - the `R4` ordinal retraction and
its surviving request-ID join, the `R5` near-plane retraction, the real `ForceMiss` names with
`M-R8b`/`M-R8c` as **proposed corrections**, and the `R7` throughput scoping with `A1.10`/`T` intact.
⛔ **No gate policy changed and no historical classification moved.**

**VERDICT: NEEDS-DECISION** - the design is not accepted; the campaign remains INCOMPLETE and release
remains HELD.
