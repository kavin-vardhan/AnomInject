# m51 - frame identity, lifecycle and geometry - PREDICTIONS (DRAFT)

**REVISION 078-16, 2026-09-07. DOCUMENTATION ONLY. Nothing in this file is released for execution.**

> ## Revision header - what this file is, and what it replaces
>
> This revision **SUPERSEDES the reviewed draft committed at `a031b469315daa7815efd5a3fc732e1eca556525`**
> (SHA-256 `021B26E9C341B4C603F4D7E849E649C1FADB2003C42BD358AD25DBC81F672B3F`, 664 lines,
> 46,971 bytes). That snapshot is retained in git history and is the artifact
> `_reviews/078-15-codex-draft-review-and-request-to-chat.md` reviewed. **Where this revision and the
> `a031b46` draft disagree, THIS FILE governs**; where this revision and any *other* earlier record
> disagree, **the earlier record still governs** until a reviewed brief says otherwise.
>
> **Reviews applied:** `_reviews/078-15-codex-draft-review-and-request-to-chat.md` (findings `R1`-`R7`)
> and `_reviews/078-16-codex-takeover-and-review-clarification.md` (the exact-source correction to
> `R7`'s throughput wording). ⚠ **No Chat response existed at the time of this revision** -
> `_reviews/078-15-chat-identity-lifecycle-draft-review.md` was checked and is **ABSENT**. Its absence
> is **not** treated as approval, and a later response must be read in full.
>
> ⛔ **Still DOCUMENTATION ONLY.** No implementation, capture, checker run, build, cook, staging,
> binary swap, source / shader / tool / fixture / schema edit, owner-app action, merge, push or tag was
> performed to produce it, and none is authorised by it. Every command, lever, tool and gate named
> below is **PROPOSED / NOT IMPLEMENTED** unless explicitly marked otherwise.
>
> **`docs/predictions/2026-09-06-m51-gate-manifest.json` remains the single execution inventory and is
> BYTE-UNCHANGED.** §D.7 is a **proposed crosswalk/delta** and is **visibly NOT executable**.
>
> Governing precedence: `docs/predictions/2026-09-06-m51-release-hardening.md` **AMENDMENT 2**, then
> **AMENDMENT 1**, then §§0-8; then the `078-04`/`078-05` F1 disposition; then the `078-13`/`078-14`
> campaign record as corrected.

---

## R1-R7 disposition - what changed, and where

| id | finding | disposition in this revision | where |
|---|---|---|---|
| **R1** | the request stamp is not a complete render binding | **ACCEPTED.** Domain table completed with scope/lifetime/wrap; a common `FAnomalyPublication` is specified with its GT mint point, its GT->RT transfer, its per-consumer one-time claim and its refusal rules; world/viewport/topology binding moved **before** the behaviour change; two named subunits marked **NOT IMPLEMENTATION-READY** | §A.1, §A.4, §A.5, §D.1 |
| **R2** | logical terminal state and physical ownership conflated | **ACCEPTED.** Six orthogonal owners; publication separated from submission; per-domain conservation with an explicit `outstanding` term and a quiescent-only zero; `Cancelled(payload)` **removed**; bounded terminal retention + watermark rejection specified | §B.1-§B.6 |
| **R3** | rendered geometry has no concrete collection/binding design | **PARTLY ACCEPTED, PARTLY BLOCKED.** Matrix/rect/jitter provenance specified with source; identity/destruction survival specified; the GT-bounds-to-rendered-proxy binding is **NOT** claimed and the exact subunit is marked blocked | §C.1, §C.3, §C.4, §C.5 |
| **R4** | the ordinal payload observation does not identify the mis-bound frame | **ACCEPTED - claim RETRACTED.** *"Today the frame that would be mis-bound is exactly the frame that reports unmeasured"* is withdrawn everywhere. A counter-trace from source is given | §A.3 |
| **R5** | the near-plane argument stops before the projector | **ACCEPTED - claim RETRACTED.** The algebra is completed: the near value cannot move this projector's 2D box. Matrix-depth provenance and absent near-plane clipping are separated; `N-GEO2` is **withdrawn as a falsifier** and held | §C.2, §D.4 |
| **R6** | gate proposals are not discriminating executable specifications | **PARTLY ACCEPTED, PARTLY HELD.** Denominators, outcome classes, controls, exact schedules and restore added; `WithholdWorldRender` traced through the capture FSM; the encoded probe is put on an **explicit implementation hold** | §D.4, §D.5, §D.6 |
| **R7** | manifest/source and validity contradictions not carried into the crosswalk | **ACCEPTED, with a correction to R7 itself.** Real cvar names carried; `M-R8b` marked **correction required**; `M-R8c`'s two internal contradictions carried; **R7's characterisation of frozen-pairing A2.3 as a "validity rule" is WITHDRAWN** against the exact bytes, while release-hardening `A1.10` / manifest `T` are preserved intact | §D.5, §D.7 |

---

## 0. Evidence classes used throughout

Every claim below carries exactly one of these tags. A claim with no tag is a definition.

| tag | meaning |
|---|---|
| **SOURCE-READ** | read directly from plugin or local UE 5.1 source **this session**, with file:line. Establishes what the code does. Establishes **nothing** about how often it happens. |
| **BANKED OBSERVATION** | measured in a banked artifact or a prior session's evidence, cited to its bank. |
| **INFERENCE (conditional)** | 🆕 a consequence that follows **only under stated assumptions**. The assumptions are written out. It is **not** a source finding and **not** a measurement. |
| **PROPOSED** | a design this draft puts forward for review. Not built, not approved. |
| **OPEN** | a question this draft cannot answer and does not pretend to. |
| **NOT IMPLEMENTATION-READY** | 🆕 a subunit whose interface this draft could **not** resolve from source. Named, with the exact missing fact. It may not be scheduled. |

⛔ **A SOURCE-READ path is not a runtime finding.** Nothing in §§2-4 claims a defect occurred on any
particular leg, except where a BANKED OBSERVATION is cited beside it.

⛔ **An INFERENCE (conditional) may never be restated as a SOURCE-READ or a BANKED OBSERVATION.**
The `a031b46` draft's §A.3 headline is exactly that error, and §A.3 below retracts it.

---

## 1. Unresolved preconditions carried in - none waived

These are inputs to the design, not obstacles it removes.

1. **There is no valid candidate pairing baseline.** `078-13`/`078-14`: the `F1R_B_NAT` cell exhausted
   four B1-invalid attempts and produced **no pairing verdict of any kind**; `F1R_B_SYN`,
   `F1R_B_NAT_SP50`, `F1R_B_SYN_SP50` and all four `F1R_A_CTL_*` controls are **UNRUN**. Nothing here
   isolates or excludes an `F1` contribution. **Those four attempts remain predeclared-INVALID and are
   not retried, re-read for a verdict, or reclassified by anything in this file.** ⛔ **No causal
   binary-versus-time dichotomy is adopted** (`078-15` offline findings §3).
2. **The `078-05`/`078-06` valid pairing failures remain failures**, `PREVIOUS` 9/12/10/17 on the
   candidate in both orders at 100/50, with a concurrent A-side reading `PREVIOUS 10`. **`078-13` was
   NOT the F1 candidate's first effective-mask run.** Root cause **OPEN**, not chased (`G120`).
3. **`M-F1b` (backbuffer x target-mask) is UNRUN**, not working and not disproved. `bSveCapture` stays
   out of the effective-mask predicate (`078-12`).
4. **The writer byte bound is OPEN.** `IImageWrapper::GetCompressed` returns a whole `TArray64<uint8>`
   with no size hint; `deflateBound + 256` does not account for PNG chunk framing, and **no
   process/GPU/allocator/scratch byte cap is claimed** (AMENDMENT 2 `A2.2`).
5. **Every `open_arms` entry in the gate manifest stands**: the `R8` applied-tick-pin arm, real
   decoupled-host behaviour, mixed-Nanite actors, `M-F4`'s editor-viewport route, and the **MISSING**
   defect injectors for `M-F11` and `M-F10`. A gate set containing an OPEN arm may not be summarised as
   "all required gates pass."
6. **The two-position pairing probe cannot detect an even-frame slip.** Any probe proposed here is
   **beside** the existing pairing gate, never instead of it.
7. 🆕 **Offline sorting settled nothing about the candidate.** Sorting each attempt's `labels.jsonl` by
   `session_index` **flipped no candidate verdict**; only the **A reference's** settle start moved
   (1 -> 0). Nonmonotonic append order exists in the **pre-F1 A reference** too. ⇒ **no general
   order-insensitivity claim, no gate exoneration, no `F1` clearance, no gate edit and no known-answer
   repair test is triggered.** **48** equal-full-view anomaly comparisons had **zero** `bbox_px` /
   `bbox_valid` differences, and there is **no equal-view anomaly coverage for tries 2 and 4**.

---

## 2. SECTION A - identity, publication and consumption

### A.1 The domain table - completed

**SOURCE-READ.** Every identifier that participates, with its **scope**, **lifetime**, **reset** and
**wrap**. Three request domains share one `TMap` keyspace; two *different* frame-number counters share
one `uint32` keyspace.

| # | domain | assigned at | value | scope | reset / wrap | compared against |
|---|---|---|---|---|---|---|
| 1 | **colour capture** | `AnomalyCaptureSubsystem.cpp:4197` `++CaptureRequestSerial` | monotonic `uint64`, **no namespace bit** | subsystem instance | declared `= 0` at `AnomalyCaptureSubsystem.h:283`, **never reset in the implementation**; `uint64` wrap unreachable | equality only, `PendingSnapshots.Find(Frame.RequestId)` (`:3795`) |
| 2 | **normal mask measure (m26)** | `AnomalyCaptureSubsystem.cpp:833` - the argument **is `GFrameCounter` itself** | raw engine game-frame counter | process | never reset; `uint64` | equality only |
| 3 | **target mask** | `:1159` `GTargetMaskRequestBit \| (++TargetMaskOwnSerial)`, bit at `:903` = `1ull << 61` | `(1<<61) \| serial` | run | **`= 0` at `:3003` (StartRun) ⇒ REPEATS EVERY RUN** | equality only |
| 4 | **census** | `AnomalyCensus.cpp:705` `GCensusRequestBit \| (++CensusIdSerial)`, bit at `:40` = `1ull << 62` | `(1<<62) \| serial` | run | **`= 0` at `AnomalyCensus.cpp:116` ⇒ REPEATS EVERY RUN** | equality only |
| F1 | **scene family frame number** | **engine**, `SceneRendering.cpp:4555-4562` | `FSceneViewFamily::FrameNumber` (`uint32`, `SceneView.h:1784`) = `Scene->GetFrameNumber()` | 🆕 **PER SCENE** (`FScene::SceneFrameNumber`, `ScenePrivate.h:3662`, `++` at `:3430-3432`) | never reset per run; **wraps at 2^32** | **equality, `LookupKey` (`AnomalySveKeyRing.cpp:138`)** |
| F1b | 🆕 **scene-less fallback** | **engine**, `SceneRendering.cpp:4569` | `GFrameNumber` written into the **same** `FrameNumber` field | process | - | the same `LookupKey` comparison |
| F2 | **arm-time game frame** | `AnomalyCaptureSubsystem.cpp:4199` `Snap.FrameCounter = GFrameCounter` | `uint64` | process | - | **never compared to anything** |
| P | 🆕 **publish serial** | `AnomalySveKeyRing.cpp:110` `++PublishSerial` | `uint64` | process, **reset by `Reset()` (`:78`)** | - | **only** by `ShouldCorrupt` (`:61-72`) - it is a *test* input, not an identity |

🚨 **Three hazards the `a031b46` draft did not carry, all SOURCE-READ:**

- **`FSceneViewFamily::FrameNumber` is SCENE-LOCAL, not global.** `SceneRendering.cpp:4552-4563`
  increments `Scene->GetFrameNumber()` **only when `ViewFamilies[0]->bIsFirstViewInMultipleViewFamily`
  is true** and then assigns **the same value to every family in the loop**. ⇒ two families rendered in
  one `BeginRenderingViewFamilies` call **carry the same `FrameNumber`**, and two scenes advance
  independently.
- **A scene-less family writes `GFrameNumber` into the same field** (`:4564-4571`). ⇒ the `uint32`
  key ring can be handed values from **two different counters** with no way to tell them apart.
- **`AnomalySveKeyRing::LookupKey` does not consume.** It reverse-scans for
  `bValid && FamilyFrameNumber == key`, returns the entry and increments `Consumed`
  (`AnomalySveKeyRing.cpp:132-148`) - **it never clears `bValid`**. A repeated lookup on the same
  number returns the same entry again. The ring is a **namespace-scope `static`**
  (`:30-32`) ⇒ **process-global**, carrying no world, viewport or scene identity.

**Verified against the banked request IDs** (BANKED OBSERVATION,
`_bench_sessions_bank/M51_MASK_BATCH_DIAG_EVIDENCE/payload_outcomes.json`):
`2305843009213693954 = (1<<61) | 2` and `4611686018427387911 = (1<<62) | 7`; `PB1F_F_PAIR_NAT` log line
151 serves `[9, 2305843009213693959, 4611686018427387911]` = one domain-2 id, target serial 7, census
serial 7.

🚨 **Domains 2, 3 and 4 all key the single `TMap<uint64, FAnomalyMaskResult> Results`**
(`AnomalyMaskSceneViewExtension.h`). Domain 2 is a **time** value; 3 and 4 are **counts**. Numerical
collision is not the hazard (bits 61/62 separate them); **the hazard is that an equality match across
the union carries no shared meaning.** And `Results.Add` **overwrites an existing key silently**
(`AnomalyMaskSceneViewExtension.cpp:508`, `:528`), so a repeated serial from domain 3 or 4 (which reset
every run) **replaces** a live entry rather than being refused.

⛔ **An observed offset between two of these numbers is not evidence of a mismatch**, and two of them
agreeing is not evidence of a binding. Domain-2 ids and `Snap.FrameCounter` are numerically equal on a
tick where both are taken - **coincidence of source, not a contract.**

### A.2 What is bound today - and the step nobody accounts for

**SOURCE-READ.**

**Colour path - arm consumed on the GAME thread at publish; readback submitted on the RENDER thread,
per VIEW:**

- `FAnomalySveCapturer::ArmWanted` appends the id to `TArray<uint64> PendingWanted`
  (`AnomalySveCapturer.cpp:22-43`). **No frame stamp is stored** - `GFrameCounter` reaches only the
  trace log at `:41`.
- `ConsumeWantedForPublish(uint32 FamilyFrameNumber, uint64& OutRequestId)` (`:45-73`) pops
  `PendingWanted[0]` and **never compares `FamilyFrameNumber`** - the parameter reaches only the trace
  log at `:70`. ⇒ **arm -> family is bound by QUEUE POSITION.**
- `FAnomalySceneViewExtension::BeginRenderViewFamily` (`AnomalySceneViewExtension.cpp:49-67`) is the
  **game-thread** publish: eligibility, then `ConsumeWantedForPublish`, then
  `AnomalySveKeyRing::PublishKey`.
- `AfterPass_RenderThread` (`:82-163`) runs **per VIEW**, recovers by
  `LookupKey(View.Family->FrameNumber)` (`:93-96`) and submits at `:159`. ⇒ **family -> readback IS
  bound by identity**, and a miss is loud and drops the frame (`:96-105`).

🆕 **The eligibility test is narrower than the `a031b46` draft claimed.** `AnomalySceneViewExtension.cpp:57-58`
tests **only** `Views.Num() == 0 || !Views[0] || Views[0]->bIsSceneCapture || Views[0]->bIsReflectionCapture`
- **on `Views[0]` alone**, with **no planar-reflection test and no view-count test**. The
per-view guard at `:88` repeats the same two flags. ⚠ **Planar reflection is nonetheless excluded in
practice, and this was checked rather than assumed:** `SetupViewFamilyForSceneCapture` sets
`ViewInitOptions.bIsSceneCapture = true` **unconditionally** (`SceneCaptureRendering.cpp:625`) and the
planar path calls it with `bIsPlanarReflection = true` (`PlanarReflectionRendering.cpp:632-636`), so
such views carry **both** flags. ⛔ **A hypothesis that planar reflections steal colour arms was formed
and is REFUTED by that read; it is recorded here so it is not re-formed.**

🚨 **CONSUMPTION AT PUBLISH IS NOT SUBMISSION AT PASS, AND NOTHING RECONCILES THE DIFFERENCE.**
An arm is removed from `PendingWanted` on the game thread at `:65`. The readback is enqueued on the
render thread at `:159`. **Every path between them can drop the frame with the arm already consumed:**

| drop point | source | counted today? |
|---|---|---|
| `LookupKey` miss | `AnomalySceneViewExtension.cpp:96-105` | ring `Missed` only |
| `!Entry.bWanted` | `:107-110` | no (correct - not a capture) |
| null texture / empty rect | `:114-119` | **no** |
| extent clamp | `:122-134` | `ExtentClampDrops`, a **per-extension member**, log-only |
| `AfterPass` never invoked for the family at all | subscription is `EPostProcessingPass::VisualizeDepthOfField` (`:72`) | **no** |

⇒ the request stays in `Async->PendingSnapshots` and is destroyed at run end by a **count-only**
warning that prints **no request ids** (`AnomalyCaptureSubsystem.cpp:4108-4113`).
🆕 **The reconciliation term already exists in the counters and has never been read:**
`Handshake.Matches` (arms consumed, `AnomalySveCapturer.cpp:59`) minus `Submits` (readbacks enqueued,
surfaced at `:123`) **is exactly the count of consumed-but-never-submitted arms.**

**Mask path - N arms to 1 render, entirely on the RENDER thread, with no publication at all:**

- `ArmMask` appends to `TArray<uint64> PendingArms` with a parallel `TArray<uint8>
  PendingArmWantsPixels`. **No frame stamp on either.**
- 🚨 **`FAnomalyMaskSceneViewExtension::BeginRenderViewFamily` is an EMPTY OVERRIDE**
  (`AnomalyMaskSceneViewExtension.h`: `virtual void BeginRenderViewFamily(FSceneViewFamily&) override {}`).
  **The mask path has no game-thread publication point whatsoever.**
- `AfterTonemap_RenderThread` (`AnomalyMaskSceneViewExtension.cpp:104-138`) takes **all** pending arms
  in one move under `StateCS` (`:120-138`), sets `RequestId = ServedIds[0]` (`:131`), records
  `Item.RequestIds = ServedIds` (`:198`), and at drain **clones the whole reduction to every served
  id**: `FAnomalyMaskResult Copy = Result; ... Results.Add(Item.RequestIds[k], MoveTemp(Copy));`
  (`:521-529`).
- Only **one** consumer receives pixels: `PixelOwner` is the **first index whose `WantsPixels` flag is
  set** (`:512-520`), and `Copy.MaskPixels = MoveTemp(TightPixels)` runs only for that index
  (`:524-527`).
- Its eligibility test at `:109` **does** include `bIsPlanarReflection`, and it serves the **first view
  only** - a second view finds `PendingArms.Num() == 0` and returns silently (`:122-125`).
- 🆕 **Its activation predicate is `PendingArms.Num() > 0`** (`IsActiveThisFrame_Internal`), evaluated
  by the engine at `GatherActiveExtensions` (`GameViewportClient.cpp:1292`), which runs **before**
  `BeginRenderViewFamily` (`SceneRendering.cpp:4590`). **This is the same state any publication design
  must move, and §A.5 treats it as a blocker rather than a detail.**

⚠ **`ServedWantsPixels.SetNumZeroed(ServedIds.Num())` at `:130`** is a silent repair over two parallel
arrays: a short array is zero-filled (losing a wants-pixels flag) and a long one is truncated. Neither
is refused or counted.

🆕 **Neither extension inspects its context.** `FAnomalySceneViewExtension::IsActiveThisFrame_Internal`
returns `Cap->IsActive()` (`AnomalySceneViewExtension.cpp:43-47`) and the mask SVE returns
`PendingArms.Num() > 0` - **neither reads `FSceneViewExtensionContext`**, which carries
`FViewport* Viewport`, `FSceneInterface* Scene` and `UWorld* GetWorld()`
(`Engine/Public/SceneViewExtensionContext.h`). ⇒ **there is no world, viewport or scene binding on
either path today**, and the API to add one already exists.

### A.3 The ordinal observation - what it establishes, and the claim now RETRACTED

**BANKED OBSERVATION (078-07, request-ID joined, not inferred).** Three callbacks in each of the three
failing legs served **two distinct target-capture requests** - `[2,3]`, `[17,18]`, `[25,26]` on OLD and
NEW, `[1,2]`, `[17,18]`, `[25,26]` on the control - and in each case the **second** serial is one of
exactly three `TARGET MASK UNAVAILABLE` results with `pixels=0`, at session indices `2,17,25` (OLD/NEW)
and `1,17,25` (control). The four passing legs show **zero** two-target callbacks. `servedArms` of 2 or
3 is **healthy** where the several consumers are the *same* frame's normal/target/census requests.

**SOURCE-READ.** This joins exactly to `PixelOwner` (`AnomalyMaskSceneViewExtension.cpp:512-527`) and
to the short-array report in `ServiceTargetMask` (`AnomalyCaptureSubsystem.cpp:1192-1203`).

🔻 **RETRACTED - the `a031b46` draft's headline at its lines 149-152:**
> *"Today the frame that would be mis-bound is exactly the frame that reports unmeasured."*

**It is withdrawn here and everywhere it was copied** (`078-15` journal, the `078-15` CLAUDE block, the
`078-15` contract append and the `078-15` final report). It is not a SOURCE-READ and not a BANKED
OBSERVATION; it is an **INFERENCE (conditional)** that additionally **fails under an ordering the
source permits**:

- **Assumptions it needs:** that both arms want pixels; that no other pixel-wanting consumer (the
  census **can** want pixels, `AnomalyCensus.cpp:797-802`) sits ahead of them; that each arm's colour
  request was consumed by the family whose ordinal position matches; and that colour was delivered at
  all.
- 🚨 **A counter-trace the source permits.** Suppose arm A is armed at tick `N`, no family publishes at
  tick `N`, and family `F` publishes and renders at tick `N+1` with arm B armed the same tick. Colour is
  FIFO one-per-family (`AnomalySveCapturer.cpp:56-57`), so `F` consumes **arm A's** colour: **arm A's
  picture comes from a render one game frame LATER than its own frozen snapshot** - it is the stale
  one. The mask drains **both** arms into `F`, and `PixelOwner` is the **first** wanting index - **arm
  A**. ⇒ **arm A is mis-bound and reports a NUMBER; arm B reports unmeasured.** The claimed
  coincidence is exactly inverted.
- **And the m26 consumer has no pixel check at all.** `FAnomalyMaskMeasure::CollectResults`
  (`AnomalyMaskMeasure.cpp:404-433`) takes the result by id, immediately does `++R.ArmsResolved`
  (`:432`) and reads `Mask.ViewRectSize` (`:433`); its four discard gates - probe, no-pass, residual,
  polluted - are **state** tests, **none a render-identity test**. A cloned table contributes with no
  check whatever.

✅ **What survives, unchanged:** the request-ID-joined second-target payload loss (three per failing
run) and the `PixelOwner` mechanics. ⛔ **What does not:** any statement that assigns stale-or-correct
render identity **by ordinal**, or that implies all other frames are protected.
⛔ **Healthy same-render sharing between one frame's normal / target / census requests remains valid.**
⛔ **One rendered mask is never cloned into records for different capture frames.**

📌 **Framing, so the history is not misread.** `m43`'s *"one render serves every pending arm"*
(ledger §10.1) **fixed a real starvation defect** and its justification - *"the RT's content depends
only on which actors are tagged at render time, and each consumer filters by its own tag set"* - is
**correct for a tag-set question**. It is **not sufficient for a per-captured-frame question**: a
`blinking` target's tag set is identical on a hidden frame and a visible one, and one render cannot
answer for both. ⛔ **Do not re-propose the one-slot FIFO** (`G217`); the fix is identity, not rationing.

### A.4 PROPOSED - one authoritative immutable publication, shared by both extensions

**PROPOSED.** Two immutable records. The first is minted at the arm site; the second is minted **once
per rendered family on the game thread** and is the *only* thing the render thread is allowed to serve
from.

```
struct FAnomalyRequestKey                 // immutable; minted once at the arm site
{
    uint64  Generation;                   // ++ at StartRun; never reset within a process
    uint64  ArmGameFrame;                 // GFrameCounter at OnWorldTickEnd of the arming tick
    uint64  RequestId;                    // existing per-domain value, unchanged
    int32   SessionIndex;                 // -1 where no captured frame exists (census, probe)
    uint8   Domain;                       // Colour | Measure | TargetMask | Census
    uint8   Flags;                        // bWantsPixels, ...
};

struct FAnomalyPublication                // immutable membership; minted on the GAME thread
{
    uint64      PublicationSerial;        // ++ under one lock; monotone within a process
    uint64      Generation;
    uint64      PublishGameFrame;         // GFrameCounter at BeginRenderViewFamily
    const void* SceneKey;                 // FSceneViewFamily::Scene, opaque identity ONLY
    uint32      FamilyFrameNumber;        // FSceneViewFamily::FrameNumber (scene-local)
    const void* WorldKey;                 // UWorld*, bound at activation
    const void* ViewportKey;              // FViewport*, bound at activation
    int32       NumViews;
    uint8       bMultipleViewFamily : 1;  // FSceneViewFamily::bIsMultipleViewFamily
    uint8       bRefused : 1;             // topology / eligibility refusal
    TArray<FAnomalyRequestKey> Members;   // FROZEN at publish; render thread never adds
    // one int32 claim slot per member, mutated ONLY by InterlockedCompareExchange
};
```

**Source basis for every field and hook, read this session:**

| element | source basis |
|---|---|
| the GT mint point exists on **both** extensions | `ISceneViewExtension::BeginRenderViewFamily` is *"Called on game thread when view family is about to be rendered"* (`SceneViewExtension.h:141`), invoked at `SceneRendering.cpp:4590`; the colour SVE already overrides it (`AnomalySceneViewExtension.cpp:49`) and **the mask SVE overrides it as an empty body**, so no new virtual is required |
| it runs **after** the frame number is assigned | `SceneRendering.cpp:4552-4571` precedes `:4590` |
| `SceneKey` / `FamilyFrameNumber` are readable on **both** threads | `FSceneViewFamily::Scene` (`SceneView.h:1766`) and `::FrameNumber` (`:1784`); the render thread reads them as `View.Family->Scene` / `->FrameNumber` (already done at `AnomalySceneViewExtension.cpp:93`) |
| `WorldKey` / `ViewportKey` are obtainable | `FSceneViewExtensionContext` carries `FViewport* Viewport`, `FSceneInterface* Scene` and `UWorld* GetWorld()` (`SceneViewExtensionContext.h`), and is passed to `IsActiveThisFrame_Internal` on both extensions |
| topology is detectable at the mint point | `FSceneViewFamily::bIsMultipleViewFamily` (`SceneView.h:1807`) and `Views.Num()` |
| `ArmGameFrame` is well defined and identical across domains within a tick | all four arm sites run in the same `OnWorldTickEnd` block: `ArmIfMeasurable(..., GFrameCounter, ...)` at `AnomalyCaptureSubsystem.cpp:833`, `ArmTargetMaskOwn` at `:839`, census `Tick` at `:858`, colour `ArmWanted` at `:4234` reached from the capture `Tick` |

**Rules.**

1. **Minting is idempotent and order-independent.** `FindOrAddPublication(SceneKey, FamilyFrameNumber,
   GFrameCounter)` under one lock: whichever extension's `BeginRenderViewFamily` runs first mints, the
   second attaches and closes **only its own domain's** membership. ⚠ **This is required, not
   stylistic:** the colour SVE has **no `GetPriority()` override** (default `0`,
   `SceneViewExtension.h:191`) while the mask SVE returns `-1`, the list is sorted by priority
   (`:307`, `:314`), and **the sort direction was not established this session** - so the design must
   not depend on which runs first.
2. **A member is admitted only if `ArmGameFrame == PublishGameFrame`.** This is AMENDMENT 2 `A2.1`
   rule 2, and it is sound for `A2.1`'s reason: publish is game-thread and `GFrameCounter++` happens
   after `GEngine->Tick()`. **A non-matching arm is not admitted, not cloned, and not served**; it is
   retired terminal-`Cancelled(unrendered)` with capacity released **exactly once**, and its consumer
   records **unmeasured**, never a number.
3. 🆕 **The rule reaches the mask path only by giving the mask path a publication.** `A2.1` is written
   against `ConsumeWantedForPublish`; the mask SVE consumes on the **render thread**, where
   `GFrameCounter` is not the arming tick's value. ⇒ **`A2.1` rule 2 cannot be transplanted as written**
   - the membership close must move to the mask SVE's `BeginRenderViewFamily`, and
   `AfterTonemap_RenderThread` must then take the **publication for `(SceneKey, FamilyFrameNumber)`**
   instead of draining `PendingArms` directly.
4. **Topology refusal, before any behaviour change.** If `bIsMultipleViewFamily` is true, or
   `Views.Num() != 1`, or the context's `Viewport != ViewportKey`, or `GetWorld() != WorldKey`, the
   publication is minted **`bRefused`**, admits nothing, and every eligible arm of that tick is retired
   terminal-`Cancelled(topology)`. This **implements** AMENDMENT 1 `A1.6`'s already-adopted disposition
   (`release-hardening:717-721`: *"the extension refuses to admit requests when the family reports
   stereo or more than one view"*, mid-run change ⇒ `capture_complete:false`, reason
   `view_topology_changed`). ⛔ **It is not a new ruling and does not reopen one.**
5. **One-time claim per member.** The render thread claims a member with a single
   `InterlockedCompareExchange` on its claim slot. A second claim for the same member **fails and is
   counted** - which is what makes `LookupKey`'s non-consuming scan safe, and what turns a second view
   in one family from a **duplicate submission** into a **counted refusal**.
6. **At most one member per domain per publication.** A second same-domain admission is refused and
   counted. This is exactly `N-ID1`'s predicate (§D.4), and it is the rule that forbids two capture
   frames sharing one render.
7. **Payload ownership is by domain and identity, never by ordinal.** Where two same-publication
   consumers both want pixels the payload is **shared by reference**; where two **different**
   publications want pixels the second never entered this publication at all. ⛔ **The second requester
   is never declared stale because it is second.**
8. **The reduction table follows the same rule as the pixels.** A cloned `FAnomalyMaskResult` may reach
   **only** members of the publication that produced it.
9. **Locks and threads.** Membership closes on the **game thread** under the existing `StateCS`; the
   render thread reads a `TSharedRef<const FAnomalyPublication, ESPMode::ThreadSafe>` and mutates
   nothing but claim slots. This is where the code already is - `AfterTonemap_RenderThread` takes
   `StateCS` only to move `PendingArms` (`:120-138`).
10. **`(SceneKey, FamilyFrameNumber)` must be unique among live publications.** It is **not** unique in
    general (`SceneRendering.cpp:4559-4562`), which is precisely why rule 4 refuses the multi-family
    case rather than keying through it. A repeat while a publication is still live is a **refusal**,
    never an overwrite - the opposite of today's `Results.Add` (`:508`, `:528`).

**Cases the rule must answer, and its answer:**

| case | proposed outcome |
|---|---|
| natural and synth tick order | identical; `SynthTickOrder` relocates dispatch **within** a tick and does not change `GFrameCounter`, so it is **not** a binding bypass (the manifest's `NoViewBinding` note says the same) |
| several arms before any eligible family | rule 2: only the current game frame's arms are admitted; older ones are retired `Cancelled(unrendered)` |
| an ineligible family (scene / reflection / planar capture) | refused before admission; counted; **never consumes an arm**. ⚠ the colour SVE's family test inspects `Views[0]` only (`AnomalySceneViewExtension.cpp:57-58`) and must be widened to every view |
| a **second view** in one family | **REFUSED by rule 4** (`Views.Num() != 1`), and defended in depth by rule 5's one-time claim. 🔻 The `a031b46` draft called this OPEN; `A1.6` had already ruled it |
| two families in one `BeginRenderingViewFamilies` call | **REFUSED by rule 4** (`bIsMultipleViewFamily`); they share one `FrameNumber` and cannot be told apart |
| render thread consuming a batch after newer game-thread arms | frozen membership: the newer arms are simply not in this publication and wait for their own |
| view topology change mid-run | `A1.6`'s adopted disposition: accepted work is joined and published first, then the run terminates `capture_complete:false`, reason `view_topology_changed` |
| a `LookupKey` hit for a family that never reaches `AfterPass` | rule 5 leaves the member unclaimed; the **publication retirement sweep** (§B.4) retires it `Failed(payload)` with a reason - **today this is the silent `PendingSnapshots` drop** |

### A.5 🆕 What is NOT IMPLEMENTATION-READY, and the exact missing fact

⛔ **These subunits may not be scheduled. Naming them is the point.**

1. **The mask extension's activation predicate.** `IsActiveThisFrame_Internal` returns
   `PendingArms.Num() > 0`, and the engine decides activation at `GatherActiveExtensions`
   (`GameViewportClient.cpp:1292`) **before** `BeginRenderViewFamily`. Under this design the
   membership close consumes `PendingArms`, so the predicate must be re-based - onto "a capture run is
   active for this world/viewport", or onto the publication store. **The missing fact:** whether every
   family that reaches `BeginRenderViewFamily` was gathered from a context whose `Viewport`/`Scene`
   match the capture's, and how `GatherActiveExtensions` composes with the scene-capture,
   reflection-capture and planar-reflection call sites (`SceneCaptureRendering.cpp:759`,
   `ReflectionEnvironmentCapture.cpp:1283`, `PlanarReflectionRendering.cpp:630`, which passes a
   **Scene** context with `Viewport == nullptr`). Until that is traced, **the predicate change is not
   designed.**
2. **The backbuffer path's render-identity bridge.** `FAnomalyFrameCapturer::ArmForCapture`
   (`AnomalyFrameCapturer.cpp:196`) hooks `OnBackBufferReadyToPresent`; there is **no view family, no
   `FrameNumber` and no `FSceneViewExtensionContext`** there. `FAnomalyRequestKey` and the whole of
   §B apply to it unchanged, **but `FAnomalyPublication` has nothing to key on.** ⇒ its identity
   binding is **OPEN**. ⛔ **The path is NOT disabled, `bSveCapture` is NOT added to the effective-mask
   predicate, and no claim is made that backbuffer x target-mask works or fails** - that is `M-F1b`,
   and it is a gate, not an assumption. ⛔ **It must never be silently certified by a colour-path gate.**
3. **`GetPriority()` sort direction.** Not established this session. Handled by making the mint
   order-independent (rule 1) rather than by assuming an order - but a design that *needed* the order
   would not be ready.

---

## 3. SECTION B - lifecycle, ownership and conservation

### B.1 Six orthogonal owners

**PROPOSED.** Each row is an independent lifetime. A transition in one **never** implies a transition
in another, and each is released **exactly once at its own end of ownership**.

| # | owner | what it holds | released when |
|---|---|---|---|
| **1** | **logical disposition** | the request's single state | at the terminal transition |
| **2** | **publication membership** | one claim slot in one `FAnomalyPublication` | at claim, refusal, or publication retirement |
| **3** | **submission / physical readback** | the `FRHIGPUTextureReadback` or `FRHIGPUBufferReadback` | on completion, or at the final `BlockUntilGPUIdle` |
| **4** | **admission credit** | a `ReadbackPendingCap` slot (`A2.2`, range `[1,256]`) | when the slot's request reaches a terminal state **and** owner 3 has released |
| **5** | **CPU payload bytes** | `RawBytes` + `Pixels` + `Scaled` + `Record`, charged **including jobs in service** (`A2.2`) | when the writer job ends, success or failure |
| **6** | **tag lease** | `EventClaimed` (`AnomalyMaskMeasure.cpp:93`, cleared at `:71`) | **only when the fire has ended AND `ArmsIssued == ArmsResolved`** (`A1.6`, release-hardening:701-705) |

🚨 **Publication (owner 2, game thread) is NOT submission (owner 3, render thread).** The
`a031b46` draft's `Published` row called the state *"GPU submitted"*; it is not. The gap between them
is a real, currently unaccounted drop surface (§A.2) and it gets its own counted reasons here.
🚨 **A logical cancellation never retires GPU work.** Marking a request cancelled does not stop an
already-submitted readback from occupying memory or a tag from being in flight; owner 6's rule is
`A1.6`'s and survives this refinement **verbatim**.
🚨 **RDG pass scheduling is not ownership.** A pass added to an `FRDGBuilder` is executed later; the
physical owner is the readback object the pass writes into, which the plugin holds in `FMaskInFlight` /
`FInFlight`. Owner 3 begins at `AddEnqueueCopyPass`, not at pass construction.

### B.2 States, per-domain terminals, and the reasons

**PROPOSED.** One request has exactly one state; the terminal set is disjoint.

| state | terminal | note |
|---|---|---|
| `Armed` | no | owner 1 + 4 (+ 6 for tag-bearing domains) |
| `Admitted` | no | a member of exactly one publication (owner 2) |
| `Submitted` | no | a readback exists (owner 3) |
| `Completed` | no | payload on the CPU (owner 5) |
| `Committed` | **yes** | **Colour / TargetMask only**: image (and row, or mask PNG) written |
| `Resolved` | **yes** | 🆕 **Measure / Census / TargetMask-unmeasured**: the reduction reached its consumer and was accounted. **No image and no row is required** |
| `Cancelled(unrendered)` | **yes** | rule 2 rejection or the game-thread sweep |
| `Cancelled(topology)` | **yes** | 🆕 rule 4 refusal |
| `Cancelled(generation)` | **yes** | a completion for a prior `Generation` |
| `Cancelled(evicted)` | **yes** | 🆕 publication or ring eviction, **explicit per consumer** (`A1.6`:711-715) |
| `Failed(payload)` | **yes** | 🆕 **published and rendered, payload unrecoverable**: key miss, empty rect, extent clamp, short pixel array, `AfterPass` never invoked |
| `Failed(write)` | **yes** | writer failure |
| `Failed(oversize)` | **yes** | `A2.2`'s `writer_oversize_failures`; the index is a hole; `capture_complete` false |
| `Rejected(late)` | **yes** | 🆕 a completion arriving after reclamation (§B.5) |

🔻 **`Cancelled(payload)` is DELETED.** The `a031b46` draft's §B.2 row 6 invented it, it was absent
from its own state table and its own equation, and its worked example gave a completion to an
*unrendered* request. **A genuinely unpublished request cannot produce a real late GPU completion**
absent an explicitly faulty injected path; the honest state is `Failed(payload)`, which is reached
**only after submission**.

### B.3 Conservation - per domain, with an outstanding term

**PROPOSED.** For each `(Generation, Domain)`:

```
attempted   = refused_admission + admitted
admitted    = outstanding
            + committed + resolved
            + cancelled_unrendered + cancelled_topology + cancelled_generation + cancelled_evicted
            + failed_payload + failed_write + failed_oversize
            + rejected_late
```

- **`outstanding` is a live term** and the equation balances at **every** boundary, not only at the end.
- **`outstanding == 0` is required at exactly one declared quiescent boundary**: after `FinishRun`'s
  final drain, `FlushPending` and `BlockUntilGPUIdle`. Nowhere else.
- **Domain-specific success:** `Colour` terminates `Committed`; `TargetMask` terminates `Committed` or
  `Resolved`; **`Measure` and `Census` terminate `Resolved` and never write an image or a row.**
- Every join is **by `(Generation, Domain, RequestId)`**, never by array position, never by
  `session_index` alone, and never by a frame-number offset.

**Independent physical reconciliations, each with its own equation:**

```
arms_consumed_at_publish - submits_issued == publish_without_submit   // 🆕 today = Handshake.Matches - Submits
readbacks_enqueued       == readbacks_completed + readbacks_retired_at_shutdown
payload_bytes_charged    == payload_bytes_released                     // at quiescence (A2.2 scope)
leases_taken             == leases_released                            // at quiescence (A1.6 rule)
admission_credits_taken  == admission_credits_released                 // exactly one per admitted request
```

🚨 **The hole this closes is measured.** `DrainAsyncToCompletion` ends with
`Async->PendingSnapshots.Empty()` behind a **count-only** warning that prints **no request ids**
(`AnomalyCaptureSubsystem.cpp:4108-4113`) - **SOURCE-READ**. **BANKED OBSERVATION (078-05/078-06):**
the A-side leg wrote a contiguous prefix `0..32` and lost **57 requested indices**; because the prefix
is contiguous the loss is **invisible in the artifact's own index series** and appears only in that log
line. ⛔ **Missing requested indices must stay visible; the injection ledger is never intersected with
the written images to hide loss.**

### B.4 Worked traces

**PROPOSED.** Each row is one request; `->` is a state transition. Owners in brackets.

| # | scenario | trace | owner notes |
|---|---|---|---|
| 1 | healthy colour | `Armed -> Admitted -> Submitted -> Completed -> Committed` | 4 released after 3; 5 released at write end |
| 2 | healthy measure / census | `Armed -> Admitted -> Submitted -> Completed -> Resolved` | no owner 5 at all |
| 3 | unrendered, unadmitted | `Armed -> Cancelled(unrendered)` at rule 2 or the GT sweep | owner 3 never began; 4 released once; **6 NOT released** unless the record's `ArmsIssued == ArmsResolved` |
| 4 | admitted, never submitted | `Armed -> Admitted -> Failed(payload)` at publication retirement | 🆕 the case with **no counter today** (§A.2) |
| 5 | published, delayed completion | as (1); the delay is telemetry, not a fault | |
| 6 | **cancellation BEFORE submit** | `Armed -> Admitted -> Cancelled(*)`; owner 3 never began | safe; capacity returns immediately |
| 7 | **cancellation AFTER submit** | `Armed -> Admitted -> Submitted -> Cancelled(*)`; **owner 3 keeps the readback and owner 4 does NOT release until it completes or is retired at shutdown** | 🚨 the case `A1.6` already guards for tags; extended to admission credit |
| 8 | wrong generation | a completion arrives for a prior run: **discarded and logged**, no ledger of the new run touched | `Cancelled(generation)` in the *old* generation's ledger |
| 9 | **same-generation terminal late completion** | request already terminal; the completion is discarded and logged | ⚠ a generation check cannot catch this - the **terminal flag** does (`A2.1` rule 5). Reachable **only via (7)** |
| 10 | key miss (rendered, unreadable) | `... Submitted -> Failed(payload)` | **`ForceMiss` is this class**; distinct from (3) |
| 11 | admission cap rejection | refused **before** `Armed`; no key minted | counted refusal; **not a hole** |
| 12 | publication / ring eviction | every affected member gets an **explicit** `Cancelled(evicted)`, capacity released once, eviction counted | `A1.6`:711-715; `Wrapped` alone is not sufficient today |
| 13 | duplicate claim (second view) | the second claim's CAS fails; counted; **no second submission** | rule 5 |
| 14 | stop / start | every non-terminal request retired at `FinishRun`; `Generation`++ at `StartRun` | no cross-session carry |
| 15 | shutdown | as (14), plus writer quiescence, then `BlockUntilGPUIdle` retires owner 3 | §B.9 |

### B.5 🆕 Bounded retention and safe late rejection

**PROPOSED, because an ever-growing terminal map would satisfy §B.3 while violating the brief.**

- Live requests are bounded by `ReadbackPendingCap` (`A2.2`, `[1,256]`).
- Terminal records are kept in a **fixed-size ring** of `TerminalRetention = 4 x ReadbackPendingCap`
  entries holding **only** `(Generation, Domain, RequestId, TerminalReason)` - **no payload, no
  snapshot, no matrix.** Eviction from this ring is not a loss: it moves the request behind a
  watermark.
- **Watermark rejection.** Each `(Generation, Domain)` keeps a monotone `RetiredWatermark`. A
  completion whose `RequestId` is at or below it and is **not** in the ring is `Rejected(late)`,
  counted, never applied.
- 🚨 **The watermark is sound only within a `Generation`, and that is exactly why `Generation` is in
  the key**: target-mask and census serials **reset every run** (§B.7), so without it a new run's
  serial 3 is indistinguishable from the previous run's.
- **Wrap.** Colour and publication serials are `uint64` and monotone - wrap unreachable. Target/census
  serials are bounded by a run's frame count. `FamilyFrameNumber` is `uint32` and scene-local
  (`ScenePrivate.h:3662`); it is **never used as a lifetime key**, only to find a live publication, and
  a repeat while one is live is a refusal (rule 10).
- **Reconciliation output survives the ring**: per-index reconciliation is written from the run's
  ledger counts, not from retained per-request records.

### B.6 Stale expiry by identity - and the correction to its reach

**Adopted unchanged from AMENDMENT 2 `A2.1`:** `StaleArmMaxTicks` stays DELETED; expiry is by render
identity; the game-thread sweep runs **before admission and before the cap check**; **published GPU
work is never expired by the sweep**; the same-session terminal guard stands; capacity retires exactly
once.

🔻 **CORRECTED against the `a031b46` draft.** That draft said it *"extends the rule to the MASK path"*
as though the extension were mechanical. **It is not.** `A2.1` rule 2 compares `ArmGameFrame` to
`GFrameCounter` **at publish, on the game thread**; the mask SVE consumes on the **render thread**
(`AnomalyMaskSceneViewExtension.cpp:120-138`) where `GFrameCounter` is not the arming tick's value, and
its `BeginRenderViewFamily` is an **empty override**. ⇒ **the rule reaches the mask path only after the
mask path acquires a game-thread publication (§A.4 rule 3), and that is a behaviour change with its own
blocking review**, not a line-for-line port.

### B.7 The serial-reset asymmetry

**SOURCE-READ, and it is a concrete cross-session leak surface:**

- `CaptureRequestSerial` declared `= 0` (`AnomalyCaptureSubsystem.h:283`) and **never reset** ⇒ colour
  ids never repeat for the subsystem's lifetime.
- `TargetMaskOwnSerial = 0` at `AnomalyCaptureSubsystem.cpp:3003` (StartRun) ⇒ target-mask ids
  **repeat every run**.
- `CensusIdSerial = 0` at `AnomalyCensus.cpp:116` ⇒ census ids **repeat every run**.
- `Results.Add` **overwrites** silently on a repeated key (`AnomalyMaskSceneViewExtension.cpp:528`).

⇒ a late render-thread completion carrying a previous run's target-mask id can land in the new run's
`Results` and be taken by `ServiceTargetMask` for a same-numbered `session_index`.
**PROPOSED:** `Generation` in the key makes this a refusal on all four domains. ⛔ **No banked instance
of this leak is claimed** - it is a path, and `M-F5` is the gate that would exercise it.

### B.8 Second capture context

**SOURCE-READ:** `StartRun`'s only guard is `if (bRunning)` (`AnomalyCaptureSubsystem.cpp:2762-2766`),
which is **per world-subsystem instance**, while `AnomalySveKeyRing`'s ring
(`AnomalySveKeyRing.cpp:30-32`) and `AnomalyStencilTag`'s ledger are **process-global**. ⇒ a second
context in another world is **not** refused today. **PROPOSED (unchanged from AMENDMENT 2):** an
explicit process-wide refusal at `StartRun`, with a named reason, **before any pixel is captured and
before any shared singleton state is reset.** 🆕 The `WorldKey`/`ViewportKey` of §A.4 is what makes the
refusal checkable rather than advisory.

### B.9 Deliberately NOT released here

The writer and tag-lease implementations are **prerequisites named, not implemented** by this unit -
but their **boundary must be honest**:

- **Writer is unbounded today - SOURCE-READ.** `FAnomalyAsyncWriter::Enqueue` fires one
  `Async(EAsyncExecution::ThreadPool, ...)` per job with **no admission control and no byte
  accounting** (`AnomalyAsyncWriter.cpp:17-27`), each job carrying a full frame's `RawBytes`; and
  `FlushPending` **breaks on timeout leaving jobs live** (`:114-127`) - called with a **5.0 s** budget
  at `AnomalyCaptureSubsystem.cpp:4115` - so run-final JSON can be written while workers may still
  append.
- **The label append result is discarded - SOURCE-READ.** `EncodeAndWriteFrame` checks
  `SaveArrayToFile` for the image (`AnomalyLabelWriter.cpp:528-531`) then calls `SaveStringToFile` for
  the `labels.jsonl` row **without reading its result**, returning `true` (`:533-540`). ⇒ a successful
  RGB write cannot establish committed image+label integrity. Corroborates `078-12` at the same lines.
- ⛔ **No isolated label-append patch is released**, and **no complete encoder/plugin-buffer byte bound
  is claimed** (§1.4): `deflateBound + 256` omits PNG chunk framing, and **no process/GPU/scratch byte
  cap is asserted**. Receipts, orphan policy, publication and the auditor must agree under one reviewed
  design (commit 6). ⛔ **Publication / row-orphan / auditor semantics remain HELD pending F7/F8.**

---

## 4. SECTION C - geometry and immutable membership

### C.1 What is frozen and what is live, today

**SOURCE-READ.** The label bbox mixes a **frozen view** with **live world state**.

| input | frozen at arm? | where |
|---|---|---|
| view origin / rotation / horizontal FOV / aspect | **YES** - `Snap.View = ProjView` | `AnomalyCaptureSubsystem.cpp:4187`, `:4203` |
| membership (`FireActive`, `FireLabelled`, `ConditionHeld`, `MaskValues`) | **YES** - at `OnWorldTickEnd` | `SampleDeferredActiveState` |
| `Snap.NearClip` | recorded at `:4202` | **but see C.2: the projector never reads it** |
| **the target actor's identity** | **NO - resolved LIVE at drain** | `F.TargetActor.Get()` at `:5504`, `ResolveNodeIdentity` / `EvaluateSelectionProvenance` at `:5507-5509` |
| **the target actor's bounds** | **NO - read LIVE at drain** | `ProjectActorBoundsToScreenRect` at `:5528` -> `GetActorRenderableBounds` unions `Prim->Bounds.GetBox()` at call time (`AnomalyViewport.cpp:730-735`) |
| the near plane used by the projection | **NO - live global** | see C.2 |
| the render's actual view-projection matrix | **NEVER CAPTURED** - reconstructed from four scalars | `AnomalyViewport.cpp:166-189` |
| the render's view rect | **NEVER CAPTURED** - `FAnomalyViewInfo` has exactly five members | `AnomalyViewport.h:11-22` |

**And `AccumulateFrameEvents` runs in the drain** (`AnomalyCaptureSubsystem.cpp:4024`, inside
`ProcessCompletedFrames`), which is what makes the frozen/live split observable at all. The sync path
calls it with the *current* view (`:4309`) and is a different case.

🆕 **Destruction does not merely produce a stale box - it silently removes frames.** If
`F.TargetActor.Get()` returns null at drain, the whole `else if` at `:5525-5536` is skipped: **no
`AffectedFrames.Add`, no coverage contribution**, and the anchor identity fields at `:5506-5509` are
never filled. ⇒ **a target destroyed between arm and drain loses those frames from the event's affected
set with no counter.** ⛔ SOURCE-READ; no leg is cited.

⛔ **These are paths, not incidence claims.** On the current bench the target is a static mesh at a
settled pose, which is exactly why every banked comparison is silent about them.

**BANKED OBSERVATION, and it must not be over-read (078-15 offline checks):** across all ten pairings
of the five physical attempts, **48 matched anomaly-record comparisons had exactly identical full saved
view data and 0 of them differed in `bbox_px`/`bbox_valid`**; every observed bbox difference
accompanied a **different saved rotation**. ⚠ That is **repeated coverage, not 48 independent
samples**; there is **no equal-view anomaly coverage for tries 2 and 4**; and **equal saved view fields
do not establish that all projection/render inputs were equal.** It neither demonstrates nor excludes a
live-projection side effect.

### C.2 🔻 The near plane - the algebra is finished, and the claim is RETRACTED

**SOURCE-READ, plugin and engine. The matrix-provenance half stands; the bbox half does not.**

**What stands:**

- `AnomalyViewport::BuildProjectionMatrix` fills an `FMinimalViewInfo` and calls
  `VI.CalculateProjectionMatrix()` (`AnomalyViewport.cpp:166-176`). It **never sets
  `PerspectiveNearClipPlane`**.
- `FMinimalViewInfo::PerspectiveNearClipPlane` defaults to **`-1.0f`** (`CameraTypes.h:125`), and
  `GetFinalPerspectiveNearClipPlane()` returns *"`PerspectiveNearClipPlane` if positive, and
  `GNearClippingPlane` otherwise"* (`:155-157`).
- `CalculateProjectionMatrix` passes that value as `MinZ` into `FReversedZPerspectiveMatrix`
  (`CameraStackTypes.cpp:115-121`).
- ⇒ **the reconstructed matrix's depth column is built from `GNearClippingPlane` as it stands when
  `AccumulateFrameEvents` runs**, not from `Snap.NearClip`. **That provenance defect is real and
  remains a design input.**

🔻 **What is WITHDRAWN:** *"the exposure is to any other event's bbox computed while a near-plane change
is in force"*, and `N-GEO2` as a falsifier. **The algebra refutes it on this path:**

- The 4-argument `TReversedZPerspectiveMatrix(HalfFOV, Width, Height, MinZ)` used by
  `CalculateProjectionMatrix` is (`PerspectiveMatrix.h:140-147`):
  row0 `[1/tan(HalfFOV), 0, 0, 0]` · row1 `[0, W/tan(HalfFOV)/H, 0, 0]` · row2 `[0, 0, 0, 1]` ·
  row3 `[0, 0, MinZ, 0]`.
  ⇒ for a view-space point `(x, y, z, 1)`: `Clip.X = x/tan(HalfFOV)`, `Clip.Y = y * W/tan(HalfFOV)/H`,
  **`Clip.Z = MinZ`**, `Clip.W = z`. **`MinZ` appears in `Clip.Z` and nowhere else.**
- `AnomalyViewport::ProjectBoxToNormalizedRect` (`:342-375`) transforms eight corners, **discards only
  `Clip.W <= SMALL_NUMBER`** (`:355`), and uses **only `Clip.X/W` and `Clip.Y/W`** (`:360-361`).
  **It never reads `Clip.Z` and never clips an edge to the near plane.**
- ⇒ **changing `GNearClippingPlane` cannot move this projector's 2D box for a fixed point and view.**

⇒ **two separate facts, now kept apart:**
**(a)** the matrix's depth provenance is a live global read - **real, and a reason to freeze the render's
own matrix**; **(b)** near-plane **clipping is absent** from the projector - a corner behind the near
plane is either dropped (`W <= SMALL_NUMBER`) or projected unclipped, so the box **under- or
over-covers** at the boundary. **(b) is the geometry defect; it is not caused by (a).**

⛔ **Firing `camera_clipping` beside another event is NOT a demonstrated falsifier** and is withdrawn as
one. A can-fail near-plane test needs **known geometry that crosses the clipping boundary**, an
**explicit intended bbox definition** (clipped-polygon extent vs surviving-corner extent) and an
**independently computed expected result**. ⛔ **This correction clears nothing else** - not general
live bounds, not host matrix overrides, not frustum/visibility behaviour elsewhere, not render identity.

📌 `camera_clipping` itself takes the whole-frame branch (`AnomalyCaptureSubsystem.cpp:5519-5524`) and
does not reach the projector for its own event.

### C.3 Matrices, rects, jitter and projection provenance

**SOURCE-READ.** The reconstruction is missing more than the near plane.

| what the render has | what `FAnomalyViewInfo` has | consequence |
|---|---|---|
| `FViewMatrices::GetProjectionMatrix()` - **TAA-jittered**; `HackAddTemporalAAProjectionJitter` adds the jitter into `M[2][0]`/`M[2][1]` (`SceneView.h:443-449`) | nothing | 🆕 **the jitter shears exactly the coefficients that feed `Clip.X`/`Clip.Y`**, so jittered and unjittered projections give **different** boxes. **The design must name which is authoritative.** `GetProjectionNoAAMatrix()` (`:345`) is the unjittered one and `TemporalAAProjectionJitter` (`:316`) is the offset |
| `FMinimalViewInfo::OffCenterProjectionOffset` writes the **same** `M[2][0]`/`M[2][1]` (`CameraStackTypes.cpp:124-132`) | nothing | a host camera with an off-centre projection renders a box the reconstruction cannot express |
| `ProjectionMode` may be `Orthographic` (`CameraStackTypes.cpp:94-112`) | hardcoded `Perspective` (`AnomalyViewport.cpp:173`) | **a different projection from the one that rendered** |
| the **constrained** rect and `AspectRatioAxisConstraint`, produced by `CalculateProjectionMatrixGivenView` (`LocalPlayer.cpp:1149`) | nothing | 🔻 **CORRECTED:** the plugin sets `bConstrainAspectRatio = false` (`:174`), but `CalculateProjectionMatrix` **ignores that flag by design** (`CameraTypes.h:148`: *"regardless of `bConstrainAspectRatio`"*). ⇒ setting it is a **no-op**; what actually diverges is the **view RECT**, computed by the *other* function |
| a host SVE may rewrite the projection **and** the rect: `ViewExt->SetupViewProjectionMatrix(ProjectionData)` for every active extension (`LocalPlayer.cpp:1151-1154`) | nothing | **no reconstruction from saved scalars can see this** |
| stereo replaces the matrix and sets its own rect (`LocalPlayer.cpp:1156-1163`) | nothing | refused by §A.4 rule 4 |
| **three** rects are in play: `FViewInfo::ViewRect` (internal), `SceneColor.ViewRect` (output), `FSceneView::UnscaledViewRect` / `UnconstrainedViewRect` (`SceneView.h:1007`, `:1010`) | nothing | the mask pass already needs all of them (`AnomalyMaskSceneViewExtension.cpp:156-159`); the colour SVE copies `SceneColor.ViewRect` into a `W x H` texture at `(0,0)` (`AnomalySceneViewExtension.cpp:113`, `:144-145`); `IAI.Capture.OutputHeight` resamples again at write time |

**PROPOSED - the geometry record, frozen with its frame:**

```
struct FAnomalyRenderGeometry              // immutable; frozen with the publication
{
    FAnomalyRequestKey Key;                // (Generation, ArmGameFrame, RequestId, SessionIndex, Domain)
    FMatrix   ViewProjectionNoAA;          // the render's OWN matrix, unjittered - AUTHORITATIVE
    FVector2D TemporalAAJitter;            // recorded, never folded in
    FIntRect  InternalViewRect;            // FViewInfo::ViewRect
    FIntRect  OutputViewRect;              // SceneColor.ViewRect
    FIntRect  UnconstrainedViewRect;
    FIntPoint CaptureSubRectSize;          // the W x H the colour SVE copies
    FIntPoint WrittenImageSize;            // after IAI.Capture.OutputHeight resampling
    uint8     ProjectionMode;              // Perspective | Orthographic - recorded, not assumed
    // per target, one entry:
    //   stable actor+component identity, primitive membership, and the bounds used
};
```

**Composition rule (PROPOSED):** a normalized box is mapped
`internal -> output -> capture sub-rect -> written image` by **recorded rects only**, never by a
re-derivation, and `bbox_px` is stated in **written-image** pixels. Any stage whose rect is absent
makes `bbox_valid` **false** - never a guess.

### C.4 🆕 What C.3 does NOT establish, and the blocked subunit

⛔ **NOT IMPLEMENTATION-READY: the GT-bounds-to-rendered-proxy binding.**

- `GetActorRenderableBounds` unions `UPrimitiveComponent::Bounds` (`AnomalyViewport.cpp:730-735`) -
  the **game-thread cached** bounds, not the render proxy's.
- `World->SendAllEndOfFrameUpdates()` runs immediately before family setup
  (`SceneRendering.cpp:4527-4528`), so **at that instant** GT bounds and proxy state agree.
- ⛔ **That ordering alone is not proof that an arbitrary GT bounds snapshot equals the rendered proxy
  state**, and `AccumulateFrameEvents` reads them **many ticks later**, in the drain.
- ⛔ **Reading an RT proxy later on the game thread is not a safe substitute** and is not proposed.

⇒ **the design freezes the bounds at a GT point whose equivalence to the render is asserted only for
the `SendAllEndOfFrameUpdates` instant.** Freezing at `OnWorldTickEnd` is *earlier* than that instant.
**The missing fact is whether any component's bounds can change between `OnWorldTickEnd` and
`SendAllEndOfFrameUpdates` in the same tick** (deferred movement, animation evaluation, physics
fetch-results ordering). Until that is traced, **the "bounds equal the render's" claim is not made**,
the subunit is **blocked**, and the honest fallbacks are stated:

- freeze at the **latest game-thread point that is still before the render**, record **which** point it
  was in `FAnomalyRenderGeometry`, and let the oracle assert only what that point supports; or
- record the bounds **and** a change-detection stamp, and mark `bbox_valid` false when they disagree.

⛔ **No client-facing meaning changes and no shipped target class is silently excluded by either
fallback.**

### C.5 The independent oracle - what "independent" has to mean

**PROPOSED**, tightening `m51_bbox_oracle.py`'s manifest contract.

1. It reads a `BENCH_GEOM` sidecar only. It **must not import the implementation's projection helper**
   and **must not read the frozen label snapshot**. (Manifest `planned_tools` already says this.)
2. 🆕 It must also **not consume a value the implementation derived**. If the sidecar carries the
   render's matrix, the oracle recomputes the box from **bounds + matrix + rects**; if it carries only
   scalars, the oracle is testing the same reconstruction twice and its agreement means nothing.
   **State which, per gate.**
3. 🆕 **Its collection point, schema and join must be independent of the label buffers.** `BENCH_GEOM`
   is emitted at the **publication**, keyed by `(Generation, Domain, RequestId, SessionIndex)` - the
   same key §B.3 reconciles on - and **not** by reading `labels.jsonl`, `Snap`, or any structure the
   label writer consumes. A sidecar produced by the label path would make the oracle a second reader of
   one source.
4. **Known-answer both ways, before any verdict** (`A53`/`G96`): `--perturb-px N` feeds a deliberately
   displaced copy and **must fail**; a deliberately **invalid** geometry row (absent rect, absent
   matrix) must be **refused**, not silently passed.
5. **Declared ceiling:** the oracle validates *projection*, not *render*. It cannot detect a
   correct-looking box computed against the wrong render. That is Section A's job, and the two gates
   are not substitutes.
6. **Cross-run bbox equality is conditional**: comparable **only** when both legs' independently
   recorded `BENCH_GEOM` **actor series and view series are equal row for row**. **A shared seed
   establishes no such thing** (unchanged from `M-F2a`).

### C.6 Membership and meanings - preserved, not renegotiated

**Unchanged and carried forward verbatim in effect:**

- **`IsFireLabelledThisFrame`** (`AnomalyCaptureSubsystem.cpp:4647-4667`) remains the membership
  sampler, **sampled and frozen per fire per armed frame at `OnWorldTickEnd`**. "An injection exists"
  is **not** the condition.
- **The observable predicate is master's own**: `labelled && held && target_pixels >=
  ObservableMinPixels`, **after** the unmeasured early-out (`:3898-3906`). The configurable threshold
  is never replaced by a hardcoded `> 0`.
- **`target_drawn_pixels` and `frames_drawn_unexpected` stay DIAGNOSTIC** (`G258`): `drawn == 0` is
  evidence of ABSENCE; `drawn > 0` establishes nothing. The one-directional rule and Phase B behaviour
  survive, not merely their keys.
- **Schema 2 / schema 3 boundaries stand** (contract §3): schema 3 is additive only; `anomaly_present`
  keeps meaning "injection present"; `visible_positive` is observable-only; injection membership and
  observable/fallback membership stay separable via `observability_basis`; **the explicit empty-window
  `-1` correction stands.**
- **`capture_complete` is prepublication readiness**, never a claim about its own write; an absent or
  failed `run_summary.json` is a FAILED session to the auditor.
- ⛔ **Publication / row-orphan / auditor semantics remain HELD** pending the F7/F8 receipt design.

### C.7 The identity/geometry interaction, stated once

A frozen box computed against the **wrong render** is still wrong, and a correct render binding with
**live** geometry is still wrong. ⇒ **Section A and Section C must land together or the pair is not
certifiable.** Neither gate detects the other's failure.

---

## 5. SECTION D - sequence, instruments and falsifiers

### D.1 PROPOSED subunit sequence, with prerequisites

Numbering **follows the existing commit sequence** (`A1.15`, as corrected by AMENDMENT 2). This draft
does not renumber it. 🔻 **Corrected against `a031b46`:** the world/viewport/topology binding moves
**before** the behaviour change, not after it.

| step | content | prerequisite | review checkpoint |
|---|---|---|---|
| **2a** | `FAnomalyRequestKey` minted at all four arm sites; `Generation`; `ArmGameFrame` on **both** `PendingWanted` and `PendingArms`; `PendingArms`/`PendingArmWantsPixels` collapsed into one struct array (removing the `SetNumZeroed` silent repair) | none | reviewer sees the key and the four arm sites **before** any behaviour changes |
| **2a′** | 🆕 **world / viewport / single-view binding and topology refusal FIRST**: bind `WorldKey`/`ViewportKey` from `FSceneViewExtensionContext`; refuse `bIsMultipleViewFamily`, `Views.Num() != 1` and foreign contexts; second-context refusal at `StartRun`; per-request terminal state | 2a | **blocking review**; manifest `M-F4`, `M-F5`, `M-F5b` |
| **2b** | `FAnomalyPublication` on **both** SVEs; publication-frozen membership; rule-2 rejection; one-time claim; reduction and pixels confined to the frozen set; explicit per-consumer cancellation on eviction | 2a, **2a′** | **blocking review** - this is the behaviour change |
| **2b-blocked** | ⛔ **the mask extension's activation predicate** (§A.5 item 1) | **NOT IMPLEMENTATION-READY** | must not be scheduled |
| **2c** | freeze the render's matrix/jitter/rects and target identity into `FAnomalyRenderGeometry`; `BENCH_GEOM` sidecar at the publication | 2a, 2b | reviewer sees the sidecar schema and its **independent** join |
| **2c-blocked** | ⛔ **the GT-bounds-to-rendered-proxy equivalence** (§C.4) | **NOT IMPLEMENTATION-READY** | must not be scheduled |
| **4** | game-thread stale sweep + capacity retired exactly once on both paths; `unrendered_ticks`; `key_misses`; `publish_without_submit`; the six-owner ledgers | 2b, 2a′ | manifest `M-R8`, `M-R8b`, `M-R8c` |
| **6** | committed-write receipts, bounded writer, reconciliation fields | 4 | **separate unit; not designed here** |

⛔ **Nothing above may be implemented under this brief.** Steps 2a, 2a′ and the *unblocked* parts of
2b/2c are the only ones this draft claims are **designable** now.

### D.2 What is blocked, and by what

| item | blocked by |
|---|---|
| any leg of `M-F2a` / `M-F2b` / `M-F11` | `BENCH_GEOM` sidecar, `m51_bbox_oracle.py`, `IAI.Bench.ReadbackDelayTicks`, `IAI.Bench.NoGeometryFreeze`, a spawned moving bench actor - **all PROPOSED / NOT IMPLEMENTED** (manifest `planned_tools` / `planned_levers`) |
| `M-F11` orthographic arm | additionally **§C.3**: the projector hardcodes `Perspective` |
| `M-F11` constrained arm | 🔻 additionally **§C.3 corrected**: not `bConstrainAspectRatio` (a no-op at that call) but the **absent constrained view rect** |
| the near-plane geometry question | **§C.2**: no discriminating case exists yet |
| any candidate certification leg | **§1.1** - no valid candidate pairing baseline exists |
| `M-R8` family | `IAI.Bench.WithholdWorldRender`, `IAI.Bench.NoStaleArmSweep`, `m51_reconcile.py` |
| the whole writer tranche | **§1.4** - no obtainable a-priori encoded-output bound |
| `M-F4`'s A-side | no editor-target archive exists and a packaged exe cannot host PIE (manifest `open_arms`) |
| `M-F11` / `M-F10` can-fail | their **defect injectors are MISSING** (manifest `open_arms`) |
| both Lyra arms of every new gate | the Lyra module **SET plus manifest**, not a source SHA - **OPEN prerequisite, not waived** |

### D.3 The missing valid candidate baseline - fixture validity, prospectively

**The problem, stated without a cause.** The candidate cell produced 4 of 4 B1-invalid attempts against
the reference's 1 of 1 valid, **111 s** apart on the same box, container, fixture and recipe. **No cause
is established in either direction**: the four attempts are **not attributed to the candidate binary
and the candidate binary is not cleared.** The failing predicates are recorded (`078-14` correction 3)
and the mechanism is **UNRESOLVED** (`G206`, `G120`).

**Offline results that constrain, but do not resolve, it (BANKED OBSERVATION, 078-15):** see §1.7.

⚠ **The nonmonotonic order is the pre-existing async-writer completion order** (`G162`); `m44`'s pairing
checker already sorts by `session_index` and `check_pose.py` reads append order deliberately. That
difference is an input to the writer/identity review, **not** a new campaign gate.

**PROPOSED, and none of it weakens B1:**

1. **B1 is unchanged.** No threshold, tolerance, attempt budget or calibration bbox moves. The four
   discarded attempts stay INVALID and receive no retrospective verdict.
2. **Fixture validity is established BEFORE the measurement it gates, prospectively.** A future
   campaign declares in advance a **validity-only** preamble whose outcome is recorded and whose
   *measurement* is never read: the pairing checker is not invoked on it and no control is released by
   it.
3. **Any such probe must be predeclared with exact predictions, explicit outcome definitions and a
   stated uncertainty.** ⛔ **The A/B/A/B/A/B interleave is NOT adopted**: *"candidate failure rate much
   greater"* is not a defined criterion, and three attempts per binary cannot support a
   binary-versus-time dichotomy.
4. ⛔ **The exhausted frozen campaign is not automatically replayed**, and nothing in this file releases
   a leg.

### D.4 Gate table - PROPOSED, none runnable

Each row states its discriminating expectation, its **opportunity denominator and floor**, its outcome
classes, its **healthy and injected controls**, its **exact schedule** and its **restore**. Every row is
**both tick orders**; the fixture column names the OPEN prerequisite where one exists.

| id | proposed gate | discriminating expectation | opportunity denominator / floor | outcome classes | healthy control · injected control | schedule · restore |
|---|---|---|---|---|---|---|
| **N-ID1** | one render never serves two capture-frame identities | for every publication, **at most one admitted member per domain**; a second same-domain admission is a **counted refusal**. Two same-domain capture serials served by one render = **FAIL** | **denominator:** publications minted in the leg. **Floor:** `>= 20` publications **and** `>= 3` publications with an observed accumulation opportunity, both **measured and printed**, never assumed | PASS / FAIL / REFUSED (no publications logged) / INVALID (`< 3` counted events, or floor unmet) / **NOT EXERCISED** (no accumulation occurred - *said*, never scored as a pass) | healthy: a leg with no lever, expecting 0 refusals · injected: `IAI.Bench.NoFrameIdentityBinding 1` (**PROPOSED**; ⚠ reconcile with the manifest's already-planned `IAI.Bench.NoViewBinding`, which must **not** be duplicated) | on at `session_index` S, off at S+K, both **printed**; prior state restored at teardown so an intended failure never hangs `FinishRun` |
| **N-ID2** | colour and mask for one `session_index` come from the same render | per row, the recorded publication identity of the picture equals that of its mask, **or the mask is honestly unmeasured** | **denominator:** rows with an **effective** mask. 🆕 **Floor: `>= 50 %` of effective-mask rows must be MEASURED**, printed as a count | as N-ID1, plus **INVALID when the measured floor is unmet** | as N-ID1 | as N-ID1 |
| **N-LC1** | conservation | §B.3 balances **exactly**, per domain, at every printed boundary; `outstanding == 0` **only** at the declared quiescent boundary; each credit released once | **denominator:** all requests of all four domains | a non-zero residual is a **FAIL**, never rounded away | healthy: no lever · injected: `IAI.Bench.NoStaleArmSweep 1` **retaining rule 2's identity guard** | interval printed by `session_index`; **normal cleanup restored at teardown** |
| **N-LC2** | terminal late completion | a completion for a **terminal same-session** request is discarded, logged and counted `Rejected(late)`; no ledger moves | **denominator:** delayed completions **actually produced** - reported, never assumed | **NOT EXERCISED** if none occurred | healthy: no lever · injected: see the note below | as N-LC1 |
| **N-GEO1** | latency invariance | `bbox_px` identical row-for-row at `ReadbackDelayTicks` 0 vs 6 **and** within 1 px of the oracle | **denominator:** rows with a valid box, printed | oracle **REFUSES** on a missing/invalid sidecar (exit != 0) - **not a pass** | healthy: delay 0 · injected: `IAI.Bench.NoGeometryFreeze 1` (in-candidate) | delay applied for the whole leg; restored at teardown; **Lyra OPEN** |
| ~~N-GEO2~~ | 🔻 **WITHDRAWN** | the near-plane falsifier is refuted by §C.2's algebra | - | - | - | **HELD** until a boundary-crossing case with an explicit intended bbox definition exists |

🆕 **N-LC2's opportunity is the honest problem, and it is now stated rather than assumed.** A
same-generation terminal late completion is reachable **only** via trace (7) - **cancellation AFTER
submission**. `ReadbackDelayTicks` defers *game-thread consumption* (manifest `planned_levers`), which
delays a completion but does not by itself cancel a submitted request. ⇒ the gate needs a device that
**cancels an already-submitted request** while its readback is still in flight, and **no such device is
proposed or exists**. ⛔ **N-LC2 is therefore OPEN on its can-fail device**, and a leg that reports
`NOT EXERCISED` is the honest outcome, not a pass. ⛔ **No example is offered in which a genuinely
unpublished, unsubmitted request produces a real GPU completion** - the `a031b46` draft's row 5 did
exactly that and it is withdrawn (§B.2).

⛔ **`decidable == 0` is UNDECIDABLE and INVALID, not a pass** - the four-class stop taxonomy (REFUSED /
UNDECIDABLE / coverage / event count) applies unchanged to every row above.
⛔ **The existing two-position pairing gate is PRESERVED beside all of these**, with its declared
even-frame blind spot.

### D.5 Device corrections carried forward

- 🔻 **`ForceMiss`'s real names, from source.** The registrations are
  **`IAI.Capture.SVE.ForceMiss`** (`AnomalySveKeyRing.cpp:17`) and
  **`IAI.Capture.SVE.ForceMissPhase`** (`:24`), plus `IAI.Capture.SVE.RingTest` (`:210`).
  **There is no `IAI.Bench.ForceMiss` alias anywhere in `Source/`.** ⇒ any recipe typing the `IAI.Bench`
  spelling issues an **unrecognised command**, corrupts nothing, and yields a leg that exercises the
  device **not at all**.
- 🆕 **`ForceMiss`'s wanted-request accounting.** `ShouldCorrupt` keys on `Entry.Serial = ++PublishSerial`
  (`:110-113`), which counts **every** publication - **wanted or not**. ⇒ *"every Nth key"* is **not**
  *"every Nth wanted capture request"*, and a recipe that assumes the latter mis-states its own
  opportunity count. The interval must be expressed in **wanted requests** and the achieved corruption
  count read back from `Corrupted` / `WantedMatches`.
- **`ForceMiss` stays a different failure** - rendered but unreadable (`Failed(payload)` / `key_misses`),
  never an unrendered tick.
- 🆕 **`WithholdWorldRender`, traced through the capture FSM.** It sets
  `UGameViewportClient::bDisableWorldRendering`, which gates `BeginRenderingViewFamily`
  (`GameViewportClient.cpp:1707`). ⚠ **Withheld ticks are NOT achieved arms.** `CaptureCurrentFrame()`
  is called from **exactly three** phase branches - `LeadIn` (`AnomalyCaptureSubsystem.cpp:690`),
  `Positives` (`:700`), `PostGap` (`:710`) - and **NOT** from `SettleAfterFire` (`:694-697`),
  `SettleAfterRevert` (`:704-707`) or `DrainTail`; each branch also decrements a finite
  `PhaseFramesLeft` per tick. **A withhold burst that lands in a settle phase produces ZERO arms.**
  Additionally the target-mask arm requires `bCapturedThisTick` = `bTargetMaskEffective &&
  TargetMaskArmedTick == GFrameCounter && TargetMaskArmedSessionIndex >= 0` (`:830-831`), set only
  inside `CaptureCurrentFrame` (`:4226`). ⇒ **the achieved arm count must be measured and printed; no
  fixed opportunity count may be promised in advance.** Positive coverage must be **observed**, not
  inferred from the withhold length.
- **`NoStaleArmSweep`** must bypass **all** stale capacity-retirement routes for the test interval
  **while retaining rule 2's identity guard**, and restore normal teardown. Sweep-only is not a valid
  falsifier because publish-time retirement would recover for the wrong reason (`A2.1`, and the
  manifest's own `planned_levers` note).
- 🔻 **Throughput, scoped exactly - and R7's own wording corrected.** Frozen-pairing **`A2.1` line 567**
  says *"It is NOT an FPS or pacing band. No throughput precondition is adopted (see A2.3)"*, and
  **`A2.3` lines 617-628** require same-row FPS/`speed_ratio` reporting while stating it is *"a
  legibility requirement, not a gate"*, with *"no pacing band is adopted, none is enforced, and no
  historical leg is retroactively invalidated by throughput."* ⇒ **`R7`'s description of that rule as a
  "throughput validity rule" is WITHDRAWN as inaccurate against the exact bytes** (SHA-256
  `8D7951F5…D2E9B5`), and the `a031b46` draft's D.5 sentence was **correct as a restatement of it**.
  🚨 **But it is scoped to the pairing campaign and waives nothing else.** **Release-hardening `A1.10`
  and `A2.2`, and manifest recipe `T`, separately REQUIRE `speed_ratio <= 1.05` at 1920x1080 native,
  `VideoFps 30`, 600 requested frames (first 60 warm-up **for timing metrics only**, still counting for
  completeness/holes/drops), mask + census ON, **both fixtures and both orders**, zero natural
  `writer_drops`, zero natural `readback_drops`, zero requested-frame holes, `capture_complete true`**,
  with `T`'s own `bar_rule`: *"The band is fixed BEFORE the leg. It is never lowered after seeing a
  result."* ⛔ **Generic "no band" prose must never be read across to waive `T`.**
  ⛔ **No historical leg is invalidated, no quiet machine is required, and no owner-app closure is
  required.**
- ⛔ **No lever is sent to the frozen A-side `B8AC65D8` that does not exist in it.** An in-candidate-only
  regression is falsified in-candidate against a healthy control on the same binary. ⚠ **Whether the
  real `IAI.Capture.SVE.ForceMiss` string is present in the `B8AC65D8` archive was NOT verified this
  session** (no binary scan was run); the manifest's `M-R8b` A-side claim rests on the **wrong** name
  and must be re-established before that arm is scheduled.

### D.6 An encoded frame-identity probe - EXPLICIT IMPLEMENTATION HOLD

**PROPOSED as a candidate only, and placed on hold. The existing two-position pairing probe is
PRESERVED beside it and is not replaced.**

🔻 **The categorical claim is corrected.** The `a031b46` draft said *"a validity marker that is itself
encoded in the same pixels does not achieve this."* **That is too strong and is withdrawn.** What is
required is a **codebook** in which *valid zero*, *absent*, *occluded* and *invalid* are **distinct
codewords**; whether the marker shares the pixels is not what decides it.

Requirements a proposal must satisfy before it can be scheduled:

1. **Range and wrap.** Stated in bits, with the wrap period compared against the **longest leg intended
   (600 requests, manifest `T`)**. ⛔ A six-cube modulo-64 scheme repeats after 64 captures; **8 bits
   wrap at 256 and are still insufficient for 600**. Either the width covers the longest leg
   (`>= 10 bits`) or the wrap is **explicitly reconciled against a monotonic companion**.
2. **A codebook with four distinguishable classes** - valid-zero, absent, occluded, invalid - each with
   its own codeword, and the mapping stated.
3. **Independent joins.** The probe must join **RGB identity** and **mask identity** through
   **independent readers**, so a picture/mask cross-binding (§A.3) is **visible** rather than
   cancelling out.
4. **Known-answer cases both ways** - a deliberately **shifted** sequence and a deliberately **invalid**
   frame must both be detected before any verdict is read (`G96`).
5. **Cost and blast radius** stated: it changes rendered pixels on the bench fixture and must **never**
   reach a client payload.

⛔ **Until all five are answered in a reviewed brief, no probe change is proposed for implementation,
and no gate depends on one.**

### D.7 Proposed crosswalk to the gate manifest - a DELTA, not an inventory

**The manifest is BYTE-UNCHANGED by this file** (SHA-256 `D33AF5D4…6025F9`). The table says what a
future brief would have to amend, and why. **Nothing here is runnable, and no recipe is renumbered.**

| manifest record | proposed delta | reason |
|---|---|---|
| `existing_levers_used` | 🔻 **CORRECTION REQUIRED**: it lists `"IAI.Bench.ForceMiss"` and `"IAI.Bench.ForceMissPhase"`; the real registrations are **`IAI.Capture.SVE.ForceMiss`** / **`IAI.Capture.SVE.ForceMissPhase`** (`AnomalySveKeyRing.cpp:17`, `:24`) | the wrong spelling is an unrecognised command that exercises nothing |
| `M-R8b` | 🔻 **CORRECTION REQUIRED** (was "unchanged"): (a) both cvar names in `command_stackobot`; (b) **wanted-request accounting** - `ShouldCorrupt` keys on `PublishSerial`, which counts all publishes, so *"every 4th key"* != *"every 4th wanted request"*; (c) **archive provenance** - `a_side_note` asserts *"ForceMiss EXISTS in `B8AC65D8`"* on the **wrong** name, and the correct-name presence was **NOT verified this session** | as written the recipe corrupts nothing and its A-side arm rests on an unchecked string |
| `M-R8c` | 🔻 **TWO INTERNAL CONTRADICTIONS carried, not resolved here.** (i) `expected` promises a **bare constant** `unrendered_ticks == 3` while `M-R8`'s own text forbids a bare constant and the capture FSM makes the achieved count phase-dependent (§D.5) - a 3-tick withhold inside a settle phase arms **zero**. (ii) its `falsifier` says *"removes both the identity rejection and the sweep, restoring bare FIFO consumption"* and requires a mis-paired row, which **contradicts** `A2.1`'s and the manifest's own `planned_levers` requirement that `NoStaleArmSweep` **RETAIN** the identity guard that forbids labelling a later render with an older snapshot | `M-R8` and `M-R8c` disagree with each other on the same lever |
| `M-R8`, `M-R8c` | extend `A2.1`'s identity rule to the **mask** SVE - but **only via §A.4 rule 3**, since the mask path has **no game-thread publish point** (`BeginRenderViewFamily` is an empty override) | `A2.1` reaches the colour path only, and the extension is not mechanical |
| `M-F2a` | the oracle must consume the **render's own matrix and rects**, not a scalar reconstruction; state **which** the sidecar carries; add the **jitter convention** (`GetProjectionNoAAMatrix` vs the jittered matrix) and the **independent collection point/join** (§C.5.3) | otherwise the oracle re-checks the same reconstruction, or joins through the label path |
| `M-F2b` | add the **terminal-late-completion** case (§B.4 row 9) beside target destruction, **and** record that its can-fail device does not exist (§D.4) ; add 🆕 **destruction silently removes frames** (`AnomalyCaptureSubsystem.cpp:5525-5536`), not merely staling a box | a generation check cannot catch a same-session terminal; the destruction hole has no counter |
| `M-F11` | mark the **orthographic** arm blocked on the hardcoded `Perspective` (`AnomalyViewport.cpp:173`) and the **constrained** arm blocked on the **absent constrained view rect** - 🔻 **not** on `bConstrainAspectRatio`, which `CalculateProjectionMatrix` ignores by design (`CameraTypes.h:148`) | a leg cannot certify a projection the projector cannot express |
| `M-F4` | add the **second-view-in-one-family** case and the **two-families-share-one-`FrameNumber`** case (`SceneRendering.cpp:4559-4562`); note that `LookupKey` does not invalidate its entry; record that `A1.6`:717-721 **already ruled** the refusal, so this is implementation, not a new ruling | currently unreachable on both fixtures and therefore untested |
| `M-F5` | add the **serial-reset asymmetry** (§B.7) as the concrete leak surface, **and** that `Results.Add` overwrites silently on a repeated key | target/census ids repeat per run; colour ids do not |
| `M-F5b` | unchanged; record that today's only guard is per-instance `bRunning` (§B.8) and that `WorldKey`/`ViewportKey` make the refusal checkable | states what the refusal must replace |
| `M-W1`, `M-W4`, `M-W5` | unchanged and **still blocked** by §1.4 | the byte bound is not obtainable through `IImageWrapper` |
| `T` | **unchanged and still REQUIRED in full** (§D.5) | generic "no band" prose does not waive it |
| `planned_levers` | reconcile the **PROPOSED** `IAI.Bench.NoFrameIdentityBinding` against the already-planned `IAI.Bench.NoViewBinding`; do not create two levers for one binding | duplicate devices make a failed gate ambiguous |
| **new** `M-ID1`, `M-ID2` | the two identity rows of §D.4 | no existing recipe asserts one-render-one-frame identity |
| ~~**new** `M-GEO2`~~ | 🔻 **WITHDRAWN** - the near-plane provenance row is refuted as a falsifier (§C.2) | no discriminating case exists yet |
| regression set `R-*` | **unchanged and still required in full** | ONSET, PAIRING 100/50, G7, MASK-TIE, TARGET-PIXELS TIE, G-EDGE x8, OBS-1..4, m50 G1/G2/G6, PB-1..PB-8, black-frame, A44, P-C7 v3, Lyra LG |

⚠ **All sixteen review findings remain required**, and this unit closes none of them. **Every
`open_arms` entry stands**: the R8 applied-tick-pin arm, the real decoupled host, mixed-Nanite actors,
`M-F4`'s editor route, and the **MISSING** `M-F11` / `M-F10` injectors. **Shader readiness, the
backbuffer path, the Lyra module SET + manifest, and the writer / readback / auditor gates remain
required.** Any change touching `Shaders/`, a cooked fixture or the label schema is a **future explicit
release boundary** (one cook, quartet recorded, re-baseline) and is not approached here.

---

## 6. Self-review - against the ORIGINAL acceptance scope and against R1-R7

**Against `_reviews/078-15-identity-lifecycle-predictions.brief.md` (the acceptance scope):**

- §4.A asked for a file/function-level trace and domain table with assignment, increment, transfer and
  comparison, and warned against comparing unrelated counters. §A.1 now adds **scope, lifetime, reset
  and wrap** per identifier and names **two** frame-number counters sharing one `uint32` field.
- §4.A asked for one authoritative immutable binding across all four domains, with shared-payload and
  rejection rules, both tick orders, multiple arms, wrong/second family, topology change and an RT
  batch after newer GT arms. §A.4 answers each in its case table, and **§A.5 names what it could not
  resolve** rather than implying it did.
- §4.B asked for orthogonal lifetimes with exactly-once release and no unbounded retention. §B.1-§B.5
  give six owners, a per-domain equation **with an outstanding term**, both cancellation orders, and a
  **bounded** terminal ring with watermark rejection.
- §4.C asked for the rendered view matrices/rects/output transform and an oracle independent of the
  label snapshot **and** the implementation helper. §C.3/§C.5 answer; **§C.4 declares the bounds
  binding blocked** instead of asserting same-frame evidence.
- §4.D asked for denominators, outcome classes, can-fail devices, exact timing/restore, real CLI names,
  the `WithholdWorldRender` trace and no invented throughput waiver. §D.4/§D.5 answer, and the
  encoded probe is put on an explicit hold rather than listed as requirements.
- **Deviation from the original scope, stated:** the crosswalk still proposes **two** new rows
  (`M-ID1`, `M-ID2`) beyond the records the brief named. `M-GEO2` has been **withdrawn**. The manifest
  file remains unchanged.

**Against R1-R7:** the disposition table at the head of this file is the summary; each row names its
section. Two findings are **partly** accepted and say so: **R3** (the bounds binding is blocked, not
solved) and **R6** (`N-LC2`'s can-fail device does not exist, and the mask activation predicate is not
designed).

**Two things this revision could have claimed and did not:**

1. That the identity design is implementation-ready end to end. **It is not** - §A.5 and §C.4 name three
   blocked subunits, and the backbuffer path's identity bridge stays **OPEN**.
2. That any near-plane or ordinal claim survived review. **Both were retracted** (§C.2, §A.3), including
   from the `078-15` journal, CLAUDE block, contract append and final report.

**One hypothesis formed and refuted during this revision, recorded so it is not re-formed:** that
planar-reflection families steal colour arms because the colour SVE's guard omits `bIsPlanarReflection`.
`SceneCaptureRendering.cpp:625` sets `bIsSceneCapture` unconditionally on that path, so the existing
guard catches them (§A.2).

*End of draft. It is a DRAFT. No leg may run and no implementation may start until the reviewer
approves this exact snapshot in writing and a follow-up brief releases execution.*
