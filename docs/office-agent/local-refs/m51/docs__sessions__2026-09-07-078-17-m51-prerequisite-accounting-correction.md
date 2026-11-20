# Session 078-17 - 2026-09-07 - m51 identity / lifecycle / geometry: prerequisite and accounting correction

**Brief 078-17. DOCUMENTATION ONLY.** No implementation, capture, checker run, build, cook, staging,
binary swap, source / shader / tool / fixture / schema edit, owner-app or office action, merge, push,
tag, replay or interleave. No agents, no worktrees. Nothing ran. **The design is NOT accepted and the
campaign remains INCOMPLETE.**

⛔ **This journal records the correction. It is not a second copy of the design**, which is
`docs/predictions/2026-09-07-m51-identity-lifecycle-geometry.md`.

---

## 1. Goal

Replace the unexecuted draft's remaining contradictory assertions with a source-backed
prerequisite/accounting contract and **explicitly named unresolved interfaces**, and carry those
corrections into every current summary and historical pointer that repeats them. ⛔ **Not in scope, and
deliberately not attempted: a large new "complete" render bridge, or any claim that a subunit is
implementation-ready while its dependencies are open.**

**Governing input:** `_reviews/078-16-codex-independent-review-and-request-to-chat.md` - the independent
review of the exact bytes 078-16 committed (`560e28a`). It **governs over the 078-16 report's own
`R1`/`R2` `ACCEPTED` labels**, which were Code's dispositions of review input, not approval.
Findings `R1`-`R7` originate in `_reviews/078-15-codex-draft-review-and-request-to-chat.md` and are
**stable IDs, not renumbered.**

⚠ **Latest actual Chat response is still `_reviews/078-14-chat-campaign-evidence-review.md`** (read in
full). `_reviews/078-15-chat-identity-lifecycle-draft-review.md` was re-checked and is **ABSENT**; its
absence was **not** treated as approval and no Chat input was used. **Chat review is NOT REQUIRED for
this documentation correction** under the cadence recorded at 078-16; **the next mandatory Chat
checkpoint is the coherent corrected design BEFORE implementation.**

## 2. Method

Read in the brief's order, then **re-verified every changed technical claim directly against source**
rather than citing a prior report.

🚨 **Engine reference confirmed, and it is not the one the brief named.** The brief said *"UE source is
at `E:/EpicGames/UE_5.7/Engine` (confirm path)"*. That install exists, **but this plugin is compiled
against the project's UE 5.1 source build at `D:\UESource\UnrealEngine\Engine`**
(`Engine/Build/Build.version` = `++UE5+Release-5.1`, 5.1.1; the `.uproject`'s `EngineAssociation` GUID
resolves there). **Every engine citation below and in the design is from the 5.1 tree**, and each of the
independent review's cited line numbers was checked to land on the claimed construct there. ⚠ Also
noted: the brief's `AnomalyWorldSubsystem.cpp` is `AnomalyCaptureSubsystem.cpp`; the line numbers match.

**Plugin source re-read:** `AnomalyCaptureSubsystem.cpp`, `AnomalySveCapturer.cpp`,
`AnomalySveKeyRing.cpp`, `AnomalySceneViewExtension.cpp`, `AnomalyMaskSceneViewExtension.{h,cpp}`,
`AnomalyCensus.cpp`, `AnomalyViewport.cpp`.
**Engine source re-read (5.1):** `SceneRendering.cpp`, `SceneViewExtension.cpp`, `GameViewportClient.cpp`,
`RenderGraphUtils.cpp`, `LevelTick.cpp`, `Components/PrimitiveComponent.cpp`, `RendererScene.cpp`.
**Frozen records re-read, unedited:** release-hardening `A1.6`:701-721, `A2.1`:1219-1245, `A2.2`;
frozen-pairing `A2.1`:567 / `A2.3`:617-628; the gate manifest.

## 3. R1-R7 disposition - corrected assertion versus OPEN design interface

**The distinction is the point of this unit.** Removing a contradiction settles an *assertion*; it does
not resolve an *interface*. Full detail is the design's `R1`-`R7` table; this is the summary.

| id | corrected assertion (settled) | design interface (still OPEN) |
|---|---|---|
| **R1** | the four arm sites do **not** share one boundary - colour arms in `CaptureCurrentFrame` (`AnomalyCaptureSubsystem.cpp:4181`, call `:4234`), the other three in `OnWorldTickEndMask` (`:809`: `:833`/`:839`/`:858`) ⇒ **a common freshness boundary is a PREREQUISITE, not a fact** · the mask activation predicate's **body** is `AnomalyMaskSceneViewExtension.cpp:49-53` (the header only declares it at `:47`) · `GetPriority` sort is **descending, higher first** (`SceneViewExtension.cpp:118-126`) ⇒ colour (`0`) precedes mask (`-1`) · the engine **copies** family and views (`SceneRendering.cpp:2402-2406`, `:2415-2417`, `:819-826`, `:2453`) ⇒ **no raw GT pointer transfers** | publication **final seal + expected domain/consumer set** (mint-then-mutate is not immutable) · a **safe GT→RT carrier** (**not invented here**) · unique-vs-repeated `(SceneKey, FamilyFrameNumber)` · insertion / idempotent attachment · retention & reclamation · world/viewport **lifetime & address-reuse tokens** · **membership must outlive the claim** · **4a foreign-family refusal** (non-destructive) vs **4b owning-context topology change** (destructive) - 4a needs the lifetime token |
| **R2** | logical credit releases **once at the FIRST terminal** (`A2.1` rule 6, release-hardening:1234), physical/tag/payload accounts separate, `A1.6` intact · `Failed(payload)` is **reachable pre-submit** - lookup miss returns at `AnomalySceneViewExtension.cpp:96-105`, **before** `:147`/`:149`/`:159` · `AddEnqueueCopyPass` **is** pass construction (`RenderGraphUtils.cpp:842-855`; lambda `:853` captures the raw readback) · late/old-generation delivery is a **diagnostic**, never a second terminal or an overwrite · one admission boundary (**logical admission**, separate from publication membership) · payload consumers include measure/normal/census with **no writer job** (census pixels: `AnomalyCensus.cpp:797-802`) · `Matches − Submits` is a **lead**, not an exact count (`LookupKey` does not invalidate, `AnomalySveKeyRing.cpp:132-148`) | **bounded or streamed per-index requested-frame record** - the 4×-cap ring + monotone watermark is **WITHDRAWN as unsound** (out-of-order retirement can reject a **live** request; aggregate counts cannot name missing indices; written files may not define completeness) · reclamation by **zero references across all owners** · the cancellation-vs-scheduling lifetime rule · mid-run per-index refusal accounting |
| **R3** | *"GT bounds and proxy state agree at that instant"* is **withdrawn in full**: `LevelTick.cpp:1046-1200` runs deferred updates (`:1165-1168`), `PrimitiveComponent.cpp:620-633` tells the scene **only if** `bDetailModeAllowsRendering && (ShouldRender() \|\| …)` (`:625-626`), `RendererScene.cpp:2195-2202` captures params and `:2248-2261` **ENQUEUES** the proxy update · `UnscaledViewRect` was named but absent from the record, and is added | GT-bounds-to-rendered-proxy equivalence · registration lifetime · primitive membership · supported proxy/bounds semantics · exact rendered-state observation · survival after destruction · the **return join** (the fields arrive at ≥3 points on 2 threads; a GT publication does not contain final RT facts) · the **transform/clipping algorithm** · the **jitter convention** · `BENCH_GEOM` collection path / schema / primitive join / discriminating known answers |
| **R4** | **RETRACTED and preserved retracted**: three request-ID-joined second-target payload losses per failing run survive; `PixelOwner` (`AnomalyMaskSceneViewExtension.cpp:512-529`) assigns **no** render identity; healthy same-render normal/target/census sharing stays valid; **never a one-slot FIFO** | - |
| **R5** | **RETRACTED and preserved retracted**: `MinZ` reaches `Clip.Z` only; the projector reads `X/W`, `Y/W` (`AnomalyViewport.cpp:342-375`). Matrix-depth provenance real; **absent near-plane clipping** is the actual defect. `N-GEO2` withdrawn | a boundary/clipping definition and a known answer, if ever gated |
| **R6** | `WithholdWorldRender` capture-vs-settle trace, `NoStaleArmSweep` **retaining** the identity guard, `N-ID2`'s measured floor (all-unmeasured is NOT a pass), `N-LC2`'s missing device, the encoded-probe hold, and proposed-vs-adopted floor distinctions **all stand** | **every schedule is UNSCHEDULABLE** - `S`, `K` and the `session_index` interval have no independent clock/event (and `session_index` stalls under withholding or cap refusal), no start/stop bounds, no restoration contract on normal/stop/finish/deinit · `N-ID1` opportunity generation and `>= 3` accumulation opportunities **in both controls** · `N-GEO1` positive floor + known moving series/coordinate convention · `N-LC1` bound and exercise floor |
| **R7** | real names `IAI.Capture.SVE.ForceMiss` / `…ForceMissPhase` (`AnomalySveKeyRing.cpp:16-28`) · `ShouldCorrupt` keys on `PublishSerial` (`:110-112`), counting **all** publishes, while a `WantedMatches` counter already exists (`:126-129`) · `M-R8b` **CORRECTION REQUIRED** incl. archive provenance · `M-R8c`'s two contradictions · **frozen-pairing `A2.3` is same-row throughput TELEMETRY, expressly not a pacing-validity band** (corrects Codex's own wording), while release-hardening `A1.10`/`A2.2` and manifest `T` **still require** `speed_ratio <= 1.05`, 1080p native, 600 requests, 30 fps, both fixtures, both orders, warm-up 60 for timing only, zero natural drops, zero holes | the manifest is **not edited here**; all rows stay **PROPOSED CORRECTIONS** for a separate reviewed gate-amendment brief |

## 4. Deviations, stated

- **The design grew 1,078 → 1,427 lines.** Sections were **removed or demoted** as instructed (§B.5's
  ring and watermark, §A.4's "authoritative immutable" heading, §D.1's designable-now claim, §D.4's
  "exact schedule"), but each correction carries the **withdrawn text plus the source line that
  refutes it**, which is net additive. Flagged so the reviewer can judge reviewability.
- **The CLAUDE.md block is 45 lines against the brief's `<= 35` aim** (down from the 078-16 block's 50).
  Two compression passes were made; the five withdrawals and the six OPEN interfaces are load-bearing
  for a cold reader who opens only `CLAUDE.md` and would otherwise act on a superseded claim.
- **The contract's dated `078-17` entry is 87 lines against the `<= 45` aim**, plus an 11-line
  supersession pointer at the `078-16` heading (99 insertions, zero deletions). It carries the
  supersession, five withdrawals, the accounting rules and the preserved holds; nothing was omitted to
  hit the number. ⚠ **Both overruns are stated at their measured size** - the 078-16 report shipped a
  stale self-metric and that is precisely the failure this unit exists to correct.
- **`E:\EpicGames\UE_5.7` was NOT used**, and the brief's engine path is corrected in §2 rather than
  followed.
- **No binary string scan was run**, so whether `IAI.Capture.SVE.ForceMiss` exists in the frozen A-side
  archive `B8AC65D8` remains **UNVERIFIED** - carried forward from 078-16, not newly claimed.

## 5. State at the end

`m51` HEAD moves only by this session's single path-scoped documentation commit; parent `560e28a`.
Production frozen `6713f51`; `master` == `origin/master` == `ac13700`; `6713f51..HEAD -- Source Shaders`
**empty**; ten owner handoffs preserved untracked; CaptureBench `472a409` tracked-clean with its seven
prior untracked tools; Lyra detached-clean `4a4bfcc`; staged exe and candidate archive both
`D50DDE78…4796`; A-side archive `B8AC65D8…8EEAD8`; all five container files unchanged; **gate manifest
byte-unchanged at `D33AF5D4…6025F9`** and frozen-pairing at `8D7951F5…D2E9B5`; all 33 older prediction
files rehashed against the preflight with zero mismatches. **No runtime validation is claimed and
nothing is in flight.**

**Held, unchanged:** no valid candidate pairing baseline (four attempts INVALID, unretried, **no cause
established in either direction**, no verdict of any kind) · the 078-05/06 valid pairing failures remain
failures, root cause OPEN · B1, the pairing predicate, the coverage floor, the attempt budget and the
four-class stop taxonomy untouched · **no order-insensitivity claim and no `F1` clearance** · the
A/B/A/B/A/B interleave **not adopted** · `M-F1b` UNRUN · the writer byte bound OPEN · publication /
row-orphan / auditor semantics HELD for F7/F8 · all sixteen findings, every `open_arms` entry and the
full regression set still required · Lyra needs its module SET plus manifest · **the exhausted campaign
is never auto-replayed.**

## 6. Next

Codex reviews the exact revised bytes. **Chat review is NOT REQUIRED for this documentation
correction's disposition; the next mandatory Chat checkpoint is the coherent corrected design BEFORE
implementation**, then final predictions before the first physical leg, then evidence/source before
merge. ⛔ **Nothing past design step `2a` is designable.** Do not implement, run a leg, change B1, edit
the manifest, or start any other unit unprompted.

**VERDICT: NEEDS-DECISION** - the design is not accepted; `R1`, `R2`, `R3` and `R6` retain OPEN
interfaces. Document completion is not design, execution or release approval; the campaign remains
INCOMPLETE and release remains HELD.
