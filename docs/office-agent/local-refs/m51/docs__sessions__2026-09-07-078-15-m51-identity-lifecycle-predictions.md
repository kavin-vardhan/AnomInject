# Session 078-15 - 2026-09-07 - m51 identity / lifecycle / geometry: prediction-only design

**Brief 078-15. DOCUMENTATION ONLY.** No capture, no checker execution, no build, no cook, no
staging, no binary swap, no source / shader / tool / fixture / schema edit, no owner-app action, no
merge, no push, no tag. No agents, no worktrees. Nothing ran.

**Deliverable:** `docs/predictions/2026-09-07-m51-identity-lifecycle-geometry.md` - a reviewable
DRAFT for the next identity/lifecycle/geometry unit. **It releases nothing.**

> 🔻 **CORRECTED BY 078-16 AND 078-17 - READ §9 AND §10 AT THE END OF THIS FILE BEFORE TRUSTING §4.2 OR §4.3.**
> Two claims made below are **RETRACTED** (the ordinal "mis-bound == unmeasured" identity claim and the
> near-plane bbox-exposure claim), and the deliverable named above has been **revised in place**. The
> reviewed snapshot of that deliverable is `a031b46`; the current bytes are the 078-16 revision. The
> body of this journal is **preserved as the historical record and is not retro-edited.**

---

## 1. Cold start - what a fresh reader needs

Read in this order:

1. `docs/OPERATING-CONTRACT.md`, including the dated `078-13` and `078-14` entries.
2. The new draft `docs/predictions/2026-09-07-m51-identity-lifecycle-geometry.md`.
3. `docs/predictions/2026-09-06-m51-release-hardening.md` - **AMENDMENT 2 first**, then AMENDMENT 1,
   then the superseded §§0-8, then the 078-04 F1 disposition.
4. `docs/predictions/2026-09-06-m51-gate-manifest.json` - **still the single execution inventory**.
5. `_reviews/078-14-chat-campaign-evidence-review.md`,
   `_reviews/078-15-offline-check-scope.md`,
   `_reviews/078-15-codex-offline-findings-and-request-to-chat.md`,
   `_reviews/078-12-codex-f1-source-review.md`,
   `_reviews/078-07-codex-offline-findings-and-disposition.md`.

**Authorisation for this unit:** Chat's `078-14` review §3 approved prediction-only drafting after
its two offline checks; Codex completed both in `078-15` and released the drafting unit. The
campaign itself remains **INCOMPLETE / NEEDS-DECISION**.

---

## 2. State verified at the start of the session, and again before the commit

| item | value |
|---|---|
| `m51` HEAD | `46139fd3ef20fab90d0768492a987a9952cceb62` (before this session's docs commit) |
| `master` == `origin/master` | `ac13700d99fe116056f5aa11611f8931a563a3f2` |
| `git diff 6713f51..HEAD -- Source Shaders` | **EMPTY** - production frozen |
| untracked owner handoffs | **10**, all preserved |
| CaptureBench | `472a409dc70ae78d01a38d08fc3bddb15fe41c58`, tracked-clean, 7 untracked tools |
| staged executable | candidate `D50DDE78...` - **not touched, not run** |

⛔ Binary and container hashes were **read as provenance only**. Nothing was staged, swapped or
launched. No candidate exists for the proposed identity unit and none is invented.

---

## 3. What this session actually did

Read the governance and review chain in full, then performed a **direct source trace** of every
claimed thread boundary, identity domain, arming/admission gate and projection primitive in the
plugin, plus the relevant local UE 5.1 engine source. Earlier evidence was used as **leads**, not as
a substitute for the current read. The `be16977` endpoint diff was **not** imported.

The draft's substance is in the predictions file. This journal records the **findings that were new
to the record**, so they are not lost if the draft is revised.

---

## 4. Findings new to the record - all SOURCE-READ

### 4.1 Four request-identifier domains, three sharing one keyspace

- colour: `++CaptureRequestSerial` (`AnomalyCaptureSubsystem.cpp:4197`), plain serial, **no bit**
- normal mask measurement: **`GFrameCounter` itself** is the request id (`:833`)
- target mask: `(1ull<<61) | ++TargetMaskOwnSerial` (`:903`, `:1159`)
- census: `(1ull<<62) | ++CensusIdSerial` (`AnomalyCensus.cpp:40`, `:705`)

The last three all key the single `TMap<uint64, FAnomalyMaskResult> Results`
(`AnomalyMaskSceneViewExtension.h:87`). One is a **time** value; two are **counts**. Arithmetic on
the banked request IDs confirms the bit layout:
`2305843009213693954 = (1<<61)|2`, `4611686018427387911 = (1<<62)|7`.

### 4.2 The colour path binds 1:1; the mask path binds N:1 - and clones

`ConsumeWantedForPublish(uint32 FamilyFrameNumber, ...)` pops `PendingWanted[0]` FIFO and **never
compares `FamilyFrameNumber`** (`AnomalySveCapturer.cpp:45-73`; the parameter reaches only the trace
log at `:69`). The subsequent family -> readback step **is** identity-bound, through the key ring
(`AnomalySceneViewExtension.cpp:65-66`, `:93-105`).

The mask SVE takes **all** pending arms into one render
(`AnomalyMaskSceneViewExtension.cpp:126-131`) and at drain **clones the whole reduction to every
served id** (`:521-529`), giving the pixel payload to the **first** arm whose wants-pixels flag is
set (`:512-520`).

⇒ under arm accumulation, colour arm *j* lands on family F+j-1 while mask arm *j* lands on family F.
**The picture and the mask for one `session_index` can come from different renders**, and the join
between them is by `session_index` (`TargetMaskOutcome`, `:1162`, `:1195`, `:3830`), which validates
no render identity on either side.

🔑 **Today the mis-bound frame is the one that reports unmeasured**, because `ServiceTargetMask`
`continue`s on a short pixel array (`:1192-1203`) before it reads the cloned table. **That is
ordinal luck, not a rule**, and three things erode it: the census can also want pixels
(`AnomalyCensus.cpp:797-802`); the m26 consumer has **no pixel check at all**
(`AnomalyMaskMeasure.cpp:404-432`, whose four discard gates are state tests, not identity tests);
and making pixels available to every consumer - the obvious "fix" - would make every mis-bound frame
report a **correct-looking** mask.

📌 **Framing:** `m43`'s "one render serves every pending arm" (ledger §10.1) fixed a real starvation
defect and is **correct for a tag-set question**. It is not sufficient for a **per-captured-frame**
question - a `blinking` target's tag set is identical on hidden and visible frames. ⛔ Do not
re-propose the one-slot FIFO (`G217`).

### 4.3 The label projection uses the LIVE near plane, and hardcodes perspective

`AccumulateFrameEvents` runs **in the drain** (`AnomalyCaptureSubsystem.cpp:4024`) and projects the
**live** actor (`:5504`, `:5525`, `:5528`) against the **frozen** four-scalar view. Its projection
comes from `AnomalyViewport::BuildProjectionMatrix` (`AnomalyViewport.cpp:166-176`), which

- hardcodes `ProjectionMode = Perspective` and `bConstrainAspectRatio = false` (`:173-174`), and
- never sets `PerspectiveNearClipPlane`, which defaults to `-1.0f`, so
  `FMinimalViewInfo::GetFinalPerspectiveNearClipPlane()` returns **`GNearClippingPlane`**
  (`Engine/Classes/Camera/CameraTypes.h`; used at
  `Engine/Private/Camera/CameraStackTypes.cpp:115-121`).

⇒ the box is computed with the **live global near plane at drain time**, not `Snap.NearClip`
(`:4202`) - and `camera_clipping` is a shipped session-scoped anomaly whose mechanism is mutating
that global. ⛔ No banked leg is cited as an instance; `camera_clipping`'s own event takes the
whole-frame branch (`:5519-5524`), so the exposure is to **other** events' boxes.

Also engine-side: `ULocalPlayer::GetProjectionData` calls
`ViewExt->SetupViewProjectionMatrix(ProjectionData)` (`Engine/Private/LocalPlayer.cpp:1151-1154`), so
a host extension can alter the real projection - which **no reconstruction from four scalars can
see**. `FAnomalyViewInfo` has exactly five members and carries no matrix and no view rect
(`AnomalyViewport.h:11-22`).

### 4.4 The serial-reset asymmetry

`CaptureRequestSerial` is never reset (`AnomalyCaptureSubsystem.h:283`; no assignment in the .cpp),
so colour ids never repeat. `TargetMaskOwnSerial = 0` at `:3003` and `CensusIdSerial = 0` at
`AnomalyCensus.cpp:116`, so **both mask-side keyspaces repeat every run**. That is a concrete
cross-session leak surface for `M-F5` and is what `Generation` would close.

### 4.5 Smaller items, each with its line

- **`LookupKey` never invalidates its entry** (`AnomalySveKeyRing.cpp:132-148`), so two views sharing
  one family `FrameNumber` would both submit for one `RequestId`. Unreachable on both current
  single-view fixtures ⇒ **untested**, and an `M-F4` input.
- **Unresolved snapshots are dropped by count** - `PendingSnapshots.Empty()` behind a count-only
  warning (`:4108-4113`). This is where 078-05/06's **57 missing requested indices** went; the
  written prefix was contiguous, so the loss is invisible in the artifact's index series.
- **F1's hold moves the entire remaining batch** into `TargetMaskHeldFrames` when the head waits
  (`:3834-3838`), which is *why* its counter measures **drain visits at the waiting head**, not
  elapsed game frames - head-of-line blocking, confirming `078-12`'s reading with the mechanism.
- **The label append result is discarded** - `SaveStringToFile` unchecked, `return true`
  (`AnomalyLabelWriter.cpp:533-540`), corroborating `078-12` at the same lines.
- **The writer is unbounded** - one `Async(EAsyncExecution::ThreadPool, ...)` per job, no admission
  control, no byte accounting (`AnomalyAsyncWriter.cpp:17-27`); `FlushPending` **breaks on timeout
  leaving jobs live** (`:114-127`).
- **`StartRun`'s only guard is per-instance `bRunning`** (`:2762-2766`) while the key ring and the
  tag ledger are process-global ⇒ a second capture context is **not refused today**.
- **`ServedWantsPixels.SetNumZeroed(...)` at `AnomalyMaskSceneViewExtension.cpp:130`** silently
  repairs a desync between two parallel arrays rather than refusing it.
- **The observable predicate** was re-read and matches the contract exactly:
  `labelled && held && target_pixels >= ObservableMinPixels`, after the unmeasured early-out
  (`:3898-3906`). **`IsFireLabelledThisFrame`** (`:4647-4667`) is sampled and frozen at
  `OnWorldTickEnd` (`:4607-4612`). Both are preserved unchanged by the draft.

---

## 5. What the draft proposes, in one paragraph

An immutable `FAnomalyRequestKey` - `(Generation, ArmGameFrame, RequestId, SessionIndex, Domain)` -
minted once at all four arm sites; **publication-frozen membership**, so a render family may serve
only requests whose `ArmGameFrame` matches at publish, and the reduction and the pixels are confined
to that frozen set; **the same rule applied to the mask SVE**, where AMENDMENT 2's identity rule does
not currently reach; a terminal-state lifecycle with a stated conservation equation and capacity
retired exactly once; and geometry frozen with its frame, including the **render's own matrix and
rect** rather than a four-scalar reconstruction. ⛔ **The second requester is never declared stale by
queue position** - only by a mismatched `ArmGameFrame`. ⛔ **One rendered mask is never cloned into
records for different capture frames.**

---

## 6. Held, and named rather than implied

- **No valid candidate pairing baseline exists**; the four discarded attempts stay INVALID, are not
  retried and receive no retrospective verdict. **No cause is established in either direction** -
  the candidate binary is neither implicated nor cleared.
- The **078-05/078-06 valid pairing failures remain failures**; root cause OPEN, not chased.
- **B1 is untouched.** No threshold, tolerance, budget or calibration bbox moves. **No candidate
  verdict flipped under sorting** (078-15 offline), so no gate edit and no known-answer repair test
  is triggered. The reference's settle start did move (1 -> 0), so **no general order-insensitivity
  claim is made**.
- **The A/B/A/B/A/B interleave is NOT adopted** - "much greater failure rate" is undefined and three
  attempts per binary cannot support the offered dichotomy.
- **`M-F1b` UNRUN**; Lyra not waived; every `open_arms` entry stands; all sixteen findings and the
  full regression set remain required.
- **The writer byte bound is OPEN** and **no isolated label-append patch is released.**
- Publication / row-orphan / auditor semantics remain **HELD** pending the F7/F8 receipt design.

---

## 7. Deviations from the brief

**One, stated rather than absorbed.** The brief asked for a crosswalk to "M-F2a/b, M-F11, M-F4,
M-F5/b, M-R8/b/c and relevant pairing/ownership gates". The crosswalk (draft §D.7) covers all of
those and additionally proposes **three new rows** (`M-ID1`, `M-ID2`, `M-GEO2`), because no existing
recipe asserts one-render-one-frame-identity or near-plane provenance. They are marked **proposed,
not runnable, and the manifest file is unchanged.**

---

## 8. Next

Codex, then Chat, review the exact bytes of the draft. ⛔ Do not implement, do not run a leg, do not
replay the exhausted campaign, do not change B1, and do not start any other unit unprompted.

**VERDICT: NEEDS-DECISION** - the draft awaits review; document completion is not design, execution
or release approval.

---

## 9. 🔻 CORRECTION 078-16, 2026-09-07 - what this journal claimed that the evidence does not support

**Appended, not rewritten.** Everything above is preserved as the historical record. The corrections
below are authoritative where they and the body disagree. Sources:
`_reviews/078-15-codex-draft-review-and-request-to-chat.md` (findings `R1`-`R7`) and
`_reviews/078-16-codex-takeover-and-review-clarification.md`. ⚠ **No Chat response existed** -
`_reviews/078-15-chat-identity-lifecycle-draft-review.md` was checked and is **ABSENT**; its absence
was **not** treated as approval. ⛔ **Documentation only: nothing ran, no gate moved, no leg was
re-run, no negative result was erased, and the manifest is byte-unchanged.**

**Revised design:** `docs/predictions/2026-09-07-m51-identity-lifecycle-geometry.md`, revised **in
place** under brief 078-16. The reviewed snapshot is the `a031b46` version (SHA-256 `021B26E9…F672B3F`,
664 lines); the current bytes supersede it and carry a revision header and an `R1`-`R7` disposition
table.

**(1) 🔻 RETRACTED - §4.2's headline.** *"Today the frame that would be mis-bound is exactly the frame
that reports unmeasured"* is **withdrawn**. It is neither SOURCE-READ nor BANKED OBSERVATION but a
**conditional inference**, and the source permits the opposite ordering: colour is consumed FIFO one
arm per family (`AnomalySveCapturer.cpp:56-57`) while the mask drains **all** pending arms into one
render, and `PixelOwner` is the **first** wanting index (`AnomalyMaskSceneViewExtension.cpp:512-520`)
- so the arm that receives pixels and reports a **number** can be the stale one. **What survives:** the
request-ID-joined second-target payload loss (three per failing run) and the `PixelOwner` mechanics.
⛔ **No stale/correct render identity may be assigned by ordinal**, and healthy same-render
normal/target/census sharing remains valid.

**(2) 🔻 RETRACTED - §4.3's bbox exposure.** The **matrix-provenance** half stands: the reconstruction
never sets `PerspectiveNearClipPlane` (`AnomalyViewport.cpp:166-176`), the default is `-1.0f` and
`GetFinalPerspectiveNearClipPlane()` falls back to the **live** `GNearClippingPlane`
(`CameraTypes.h:125`, `:155-157`). The **bbox** half does not. `CalculateProjectionMatrix` uses the
4-argument `TReversedZPerspectiveMatrix` (`PerspectiveMatrix.h:140-147`), where `MinZ` occupies
`M[3][2]` **only** - it reaches `Clip.Z` and nothing else - while
`AnomalyViewport::ProjectBoxToNormalizedRect` (`:342-375`) reads **only `Clip.X/W` and `Clip.Y/W`**,
discards on `Clip.W <= SMALL_NUMBER`, and never clips to the near plane. ⇒ **changing the near value
cannot move this projector's 2D box.** The real geometry defect is the **absent near-plane clipping**,
which is a different fact. ⛔ **`camera_clipping` fired beside another event is NOT a demonstrated
falsifier**, and the proposed `M-GEO2` row is **withdrawn**.

**(3) The mask path's identity extension is not mechanical.** §5's paragraph describes extending
AMENDMENT 2 `A2.1`'s rule to the mask SVE. `A2.1` rule 2 compares `ArmGameFrame` to `GFrameCounter`
**at publish, on the game thread**; the mask SVE consumes on the **render thread** and its
`BeginRenderViewFamily` is an **empty override**. ⇒ the rule reaches it only after the mask path
acquires a game-thread publication - a behaviour change with its own blocking review.

**(4) Three subunits are NOT IMPLEMENTATION-READY**, and the body implied otherwise: the mask
extension's activation predicate (`IsActiveThisFrame_Internal` returns `PendingArms.Num() > 0`, decided
at `GatherActiveExtensions`, `GameViewportClient.cpp:1292`, before `BeginRenderViewFamily`); the
GT-bounds-to-rendered-proxy equivalence; and the **backbuffer path's render-identity bridge**, which
stays **OPEN** - the path is **not** disabled and **not** certified.

**(5) The second-view case was already ruled, not open.** §4.5 and the draft called it 🆕 OPEN;
AMENDMENT 1 `A1.6` (release-hardening:717-721) had **already adopted** the refusal and the
`view_topology_changed` termination. The unit owes implementation and a detection point, not a ruling.

**(6) Manifest lever names.** The manifest's `existing_levers_used` and `M-R8b` spell
`IAI.Bench.ForceMiss` / `IAI.Bench.ForceMissPhase`; the real registrations are
**`IAI.Capture.SVE.ForceMiss`** and **`IAI.Capture.SVE.ForceMissPhase`** (`AnomalySveKeyRing.cpp:17`,
`:24`). `M-R8b` is marked **correction required**, not unchanged. **The manifest file is unchanged.**

**(7) Throughput - a correction to the review's own wording, carried for Chat.** Frozen-pairing `A2.1`
line 567 and `A2.3` lines 617-628 state that **no throughput precondition is adopted** and that same-row
FPS/`speed_ratio` reporting is *"a legibility requirement, not a gate"*. ⇒ describing it as a
**validity rule** was inaccurate and is withdrawn. 🚨 **Separately, release-hardening `A1.10`/`A2.2` and
manifest recipe `T` still REQUIRE `speed_ratio <= 1.05` at 1080p native, 600 requests, both fixtures
and both orders, zero natural drops and zero holes.** Generic "no band" prose waives none of it. **No
gate policy changed; no historical leg was invalidated.**

⛔ **Unchanged by this correction:** the outcome, the stop, every count, the four INVALID candidate
attempts, the absent candidate pairing baseline, the 078-05/06 failures, B1, the coverage floor, the
attempt budget, the stop taxonomy, all sixteen findings, every `open_arms` entry and the full
regression set.

**VERDICT: NEEDS-DECISION** - the revised draft awaits exact-design review; the campaign remains
INCOMPLETE and release remains held.

---

## 10. 🔻 CORRECTION 078-17, 2026-09-07 - the §9 hold list was too narrow

**Appended, not rewritten. This corrects §9 only where §9 itself now understates the state.** Source:
`_reviews/078-16-codex-independent-review-and-request-to-chat.md`. ⛔ **Documentation only; nothing ran,
no gate moved, the manifest is byte-unchanged.**

- 🔻 **§9(4) - "Three subunits are NOT IMPLEMENTATION-READY" is superseded.** The mask activation
  predicate, the GT-bounds-to-rendered-proxy equivalence and the backbuffer identity bridge are
  **retained holds, and they are not the only ones.** Also OPEN: the publication's **final seal and
  expected consumer set**, a **safe game-thread→render-thread carrier** (the engine copies the family
  and its views, so no raw pointer transfers), the unique-vs-repeated family distinction, world/viewport
  **lifetime / address-reuse tokens**, **per-index reconciliation**, and the geometry **return join,
  transform/clipping algorithm and jitter convention**.
- 🔻 **§9(2)'s geometry correction is EXTENDED.** It withdrew the near-plane **bbox** claim. **078-17
  additionally withdraws in full** the narrow *"`SendAllEndOfFrameUpdates` makes GT bounds and proxy
  state agree at that instant"* claim carried in the design: the engine path runs deferred updates,
  tells the scene **only if** `ShouldRender()` (or the shadow/indirect/ray-tracing exceptions) holds,
  and then **enqueues** the proxy update.
- ✅ **§9(1), (3), (5), (6) and (7) stand unchanged** - the ordinal retraction, the non-mechanical mask
  extension, the already-ruled second-view case, the real lever names, and the throughput scoping with
  `A1.10`/`T` intact.

**VERDICT: NEEDS-DECISION** - unchanged; the campaign remains INCOMPLETE and release remains held.
