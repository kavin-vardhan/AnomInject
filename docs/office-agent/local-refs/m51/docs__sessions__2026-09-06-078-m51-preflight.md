# Session 078 - m51 preflight, integration design, and predictions for review

**Date:** 2026-09-06. **Brief:** 078-01, issued by Codex as incoming orchestrator (m51 pilot).
**Scope fence:** read-only inspection + the authorized source-branch backup push + documentation
commits. **NO implementation, no build, no cook, no leg, no staging, no merge, no tag.**
**Verdict at close:** NEEDS-DECISION - the predictions require written adversarial review before the
first leg.

---

## 1. State verification

Verified against disk and against the remote directly (`git ls-remote`), not against
remote-tracking refs.

| item | expected | found | ok |
|---|---|---|---|
| `master` | `ac13700` | `ac13700d99fe116056f5aa11611f8931a563a3f2` | yes |
| `origin/master` (queried) | same | same | yes |
| `fix/capture-label-integrity` | `be16977`, base `21c1e0d` | `be16977c0802f57457ec68edcf8d0f96c12c13d9`; `git merge-base` = `21c1e0db134f...` | yes |
| `m51` | absent | absent | yes - created this session |
| Lyra worktree | detached `4a4bfcc`, clean | `4a4bfcc2cdbade6bbeb99494e540952847ae9b63`, clean | yes |
| CaptureBench | sibling repo | `472a409dc70ae78d01a38d08fc3bddb15fe41c58`, 7 untracked tools | yes |
| working tree | clean but for untracked `CHAT-HANDOFF-*.md` | 10 handoffs + `OPERATING-CONTRACT.md` untracked | yes |

### 1.1 Source-branch backup (the one push this brief authorizes)

`fix/capture-label-integrity` was **local-only** - absent from `git ls-remote --heads origin` before
the push, exactly as the handoff recorded. Pushed as a new branch and verified:

```
git push origin refs/heads/fix/capture-label-integrity:refs/heads/fix/capture-label-integrity
git ls-remote --heads origin fix/capture-label-integrity
  be16977c0802f57457ec68edcf8d0f96c12c13d9   refs/heads/fix/capture-label-integrity
```

Nothing was overwritten; the remote had no such ref. **43 files / +1415 / -421 of unreproducible
work is now backed up.**

### 1.2 Artifact provenance (full hashes)

Staged bench binary, hashed from disk:

```
StackOBot.exe   B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8
                241,400,320 B   mtime 2026-09-06 10:15:43
```

Container quartet (Phase B), all five files:

```
StackOBot-Windows.utoc  67EA1FE0A15ACF2429516C461C5FD85989285B3C93B24C42855AE4959386257A     294,235 B
StackOBot-Windows.ucas  2CEFB8F43D048E2534471BC1D9532B77A870FAF46F14C9C2AFAE813B99B6C4D3 364,596,784 B
StackOBot-Windows.pak   E03C661084D8BC2BB6EF0F51F2DECB162853B2188B733B1DBD22B96CA974CDB7  10,115,711 B
global.ucas             A16A18A8FFB5151D2E850BB52841162F6844A0361818F8D1F158E8377DFFBA9E   1,833,008 B
global.utoc             C70ECDAA232DB8CF77BEEAB47B7226C48FCC91FF705526D507500CD14CF21C51         539 B
```

The A-side archive is intact and complete: `_binary_baselines\StackOBot.exe.m50-A159EDFD` (`A159EDFD`,
241,378,304 B) plus `m46-container-m50exe-A159EDFD\` holding all five container files **and** a
second copy of the `A159EDFD` exe - so the P-C7 v3 A-side is fully reconstructable. The gate exe is
also archived as `StackOBot.exe.m49b-gate-B8AC65D8`, hash-identical to the staged copy.

Two notes worth carrying:

- The README's line 454 still records the gate exe as **241,399,296 B**; the correction to
  **241,400,320 B** is at line 495 and matches disk. The stale figure was left in place (append-only
  record), and disk is authoritative.
- **`A9B10CC0` - the fresh build of unchanged source - is neither staged nor archived.** That is
  correct under `G201`; recorded here so nobody looks for it.

### 1.3 Untracked inventory (unchanged, nothing staged)

Ten `docs/CHAT-HANDOFF-*.md` files plus `docs/OPERATING-CONTRACT.md`. The handoffs stay untracked
and unmodified. The operating contract is added to version control by this brief's explicit
authorization; its body is preserved verbatim and a dated section 9 line is appended.

---

## 2. The sixteen findings, verified against master

The external review's line references are against a **different tree** (`m49-phase-b` tip `5e45339`
with uncommitted Phase B hunks). Every finding below was re-located in **`ac13700`** and the current
line numbers are given. **All sixteen reproduce.** Nothing was accepted on the strength of the
review's own citation.

| # | P | finding | verified at `ac13700` | what `be16977` supplies | ruling applied |
|---|---|---|---|---|---|
| F1 | P1 | disabled masks stall and discard | `:2965` `bTargetMaskEffective = bTargetMask && bMaskMeasure && bAsyncCapture && (EffectiveOutputHeight == 0)`; `:4195` `Snap.bTargetMask = bTargetMask` stores the **requested** flag; `:3750` the mask drain runs only when *effective*; `:3801` the wait gates on the **requested** flag; `:3804-3811` a **subsystem-wide** `TargetMaskHoldTicks` holds the whole remaining batch | per-snapshot `bAwaitingTargetMask` from the **effective** flag + per-snapshot `MaskWaitTicks` | R9 keep |
| F2 | P1 | late bbox / identity | `AnomalyLabelWriter.cpp:106-108` projects `F.TargetActor.Get()` live; `AnomalyCaptureSubsystem.cpp:5477-5482` reads live identity/provenance; `:5498-5503` gates `AffectedFrames` membership on a **live** projection | `FTargetGeometry` frozen at `SampleDeferredActiveState` | R9 keep |
| F3 | P1 | tag pool exhausts | `AnomalyMaskMeasure.cpp:93` adds to `EventClaimed`, `:71` clears only at run scope - **no per-event release**; `AnomalyStencilTag.cpp:41` `IsFree` excludes it; pool `200..254` = **55** (`AnomalyStencilTag.h:27-29`); census stops at `NumFree() <= EventTagHeadroom` (`AnomalyCensus.cpp:714`) | `RetireInactiveRecords` releases on fire end, history kept | R9 keep; **R2 Nanite rule NOT flipped** |
| F4 | P1 | no view binding | `AnomalySceneViewExtension.cpp:43-47` `IsActiveThisFrame_Internal` returns `Cap->IsActive()` with **no world, no viewport test**; `:57-62` takes `Views[0]`, no stereo/single-view check; `:65` consumes one pending request per eligible family; `:93-110` keys on `Family.FrameNumber` alone | world + viewport + single-view + no-stereo filters; `RenderFrame` on both paths | R9 keep + **R8 cancellation** |
| F5 | P1 | cross-session results | `AnomalyMaskSceneViewExtension.cpp:80-90` `Reset()` clears `PendingArms`/`AssignedTags`/`Results` but **not `InFlight`**; `:537-552` final drain logs `LOST-IN-FLIGHT` and leaves them; `AnomalyCaptureSubsystem.cpp:2994` `TargetMaskOwnSerial = 0` and `:1159` mints from it, so **request IDs repeat across runs** | `Generation` + discard of old generations; monotonic serials; `BlockUntilGPUIdle` | R9 keep |
| F6 | P1 | event veto erases frame positives | `AnomalyCaptureSubsystem.cpp:4948-4999`: for each manifested event, look up the **separate** event-level `FAnomalyMaskRecord`; if `MeasuredZero`, `SessionEvents.RemoveAt(i)`. **The per-frame record is never consulted.** | veto deleted | **R1 + FORK F-b - see section 4** |
| F7 | P1 | image write != labeled frame | `AnomalyLabelWriter.cpp:205-206` and `:536-537` `SaveStringToFile(..., FILEWRITE_Append)` **return value discarded** in both async and sync paths, while the image write **is** checked (`:194-200`, `:528-530`); `AnomalyCaptureSubsystem.cpp:3998` accumulates events **before** the write outcome is known | commit receipts, `CommittedFrames`, mask PNG before row, checked append | R6 |
| F8 | P1 | flush timeout != finished | `AnomalyAsyncWriter.cpp:114-122` `FlushPending` warns and returns with jobs pending; called `5.0` at `:4089` and `2.0` at `:601`; `:133-137` `ResetCounters`; writer created once at `:3719`, reset at `:602`, counters reset at `:2922` | per-session writer, `FlushPending(-1)` join, atomic JSON | R6 (bounded pool, byte cap, one-frame wait, counted drop) |
| F9 | P1 | per-frame mask bypasses validity | `ServiceTargetMask` at `:1168`; the pass-validity, contamination and unassigned-reserved-tag checks used by the event and census consumers are **not applied** before a count is read as evidence | one shared validity assessment | R9 keep |
| F10 | P1 | globals depend on tick order | `:4379-4387` decides global membership **before** deferred end-of-tick sampling; `:4642` `ComputeFireActive`; `:4657` returns active for null-actor entries | globals + fires + geometry sampled together | R9 keep, **but see section 3.1** |
| F11 | P2 | reconstructed projection | `AnomalyViewport.cpp:172-174` sets `VI.AspectRatio` from the viewport and `VI.bConstrainAspectRatio = false` explicitly; `:342` `ProjectBoxToNormalizedRect` discards behind-camera corners | actual `ProjectionMatrix` from the rendered view + near-plane clipping | R9 keep |
| F12 | P1 | original material lost to GC | `Anomaly_MissingTexture.h:30` and `Anomaly_CorruptedTexture.h:30` both `TWeakObjectPtr<UMaterialInterface> OriginalMaterial` | `TStrongObjectPtr` in both | R9 keep |
| F13 | P2 | one dropped preview freezes it | `AnomalyPreviewTee.cpp:52` `++Outstanding` on arm, `:67` decrement **only** on completion, `:34-36` `IsBusy` true while `Outstanding > 0` | unknown-request guard + 2 s cancel | R9 keep |
| F14 | P2 | sync path labels differently | `AnomalyLabelWriter.cpp:448` `Fires = Auto->GetLiveFires()` - **no session globals**; `AnomalyCaptureSubsystem.cpp:4277` sync sampling uses `FActor->IsHidden()` while the async path uses `AnomalyHiddenClass::IsLogicallyHidden` (`:4629`, `:4657`) | sync uses `IsLogicallyHidden`, includes globals | R9 keep |
| F15 | P2 | no mutation ownership | `AnomalyControlServerSubsystem.cpp:410`, `:445`, `:457` handle `inject`/`revert`/`revert_all` with **no capture gate** | WebSocket refusal + console no-op | **R4 - console half rejected, see below** |
| F16 | P2 | unsupported formats become black | `AnomalyLabelWriter.cpp:222-243` four cases plus a `default:` that yields black, then encodes and counts it | `IsSupportedPixelLayout` in decode, luma and encode | R9 keep |

### 2.1 F1 is worse than "a stall" - it discards most of the session

Traced end to end rather than taken from the review's summary. With masks **requested** but not
**effective** (the ordinary trigger is `IAI.Capture.OutputHeight != 0`, m28's shipped feature, on
top of m43's compiled-default-ON mask):

1. `:3750` never runs, so no outcome is ever produced for any frame.
2. `:3801` waits on the requested flag. `TargetMaskHoldTicks` is subsystem-wide, so the **first**
   unresolved frame holds the entire remaining batch (`:3807-3811`).
3. It takes 5 `ProcessCompletedFrames` calls to release one frame (4 to reach the cap, 1 to fall
   through), and `:3813` resets the counter to 0 so the **next** frame starts a fresh wait.
   Steady-state drain is **~1 frame per 4-5 ticks** against an arm rate of ~1 per tick. The review's
   in-memory model (29 of 120 after 120 drains) is arithmetically consistent with this code.
4. `DrainAsyncToCompletion` (`:4040`) gives only **8 iterations**, releasing ~2 more frames.
5. `:4082-4086` then logs *"N frame(s) did not resolve by run end (dropped)"* and **empties
   `PendingSnapshots`**.

So a 90-frame session in that configuration ships roughly a quarter of its frames and silently drops
the rest. This is release-blocking, and it is reachable with two shipped, documented,
compiled-default settings.

### 2.2 F2's destroyed-actor case is structural, not incidental

`:5498` is `else if (const AActor* FActor = F.TargetActor.Get())`. If the actor is gone by readback
time the branch is skipped entirely, so `Ev->AffectedFrames.Add(SessionIndex)` never runs - the frame
**leaves `affected_frames` although its pixels were captured**. The same gate applies when the actor
has merely moved out of frustum, because membership is conditioned on projection success at `:5501`.

### 2.3 F14 violates a locked project invariant

CLAUDE.md's invariants state that the logical-hidden registry is *the single source of "hidden" for
the label paths*, and warn: **"never re-point any of those consumers at `IsHidden()`."** The sync
path at `:4277` does exactly that. Because m45 deliberately stopped hide-class anomalies calling
`SetActorHiddenInGame`, `IsHidden()` is **false** for a live `blinking` / `missing_object` target -
so a synchronous capture reports every hide as non-manifested. The invariant was written; the sync
path was not brought with it.

---

## 3. Refuted premises

Reported before any plan is built on them.

### 3.1 The three "cleanup" items are branch-introduced, not master defects

The brief (section 3.2) and the chat review (section 1, F10 row; section 2.3) treat stale global
counters, a vestigial hold counter and a silent `Published` reset as existing defects to fix.
**Measured, they are regressions the branch would introduce:**

| item | master `ac13700` | branch `be16977` |
|---|---|---|
| `SessionGlobalPositive/NegativeFrames` | **incremented** at `:4252`, `:4256`, `:4383`, `:4387` | zeroed at `:4286-4287` and logged at `:4406`, **incremented nowhere** |
| `TargetMaskHoldTicks` | **live** at `:3804-3813` | replaced by per-snapshot `MaskWaitTicks` (`:3772-3774`); the member survives, zeroed at `:2913`/`:3778` |
| `Published` map | **does not exist**; the key ring does FIFO eviction with a `Wrapped` counter (`AnomalySveKeyRing.cpp:120-123`) | `AnomalyMaskSceneViewExtension.cpp:113` `if (Published.Num() >= 64) { Published.Reset(); }` - clears every live batch, no counter |

The m51 work item is therefore **"do not orphan, do not import"**, not "fix existing staleness". The
`Published` case is the sharpest: master already solved the same problem correctly one file away.

### 3.2 The branch's auditor would fail a correct m51 build

`tools/verify_integrity.py` on `be16977` encodes the **superseded** schema:

- `L151` `check(row.get("anomaly_present") == positive, ...)` - R1 keeps `anomaly_present` meaning
  "an injection was present", so this fails on a correct build.
- `L116-117` applies `render_state == "shaders_pending"` **per row**; R5 requires per fire.
- `L168` asserts `affected_frames` equals the observable set, which is false under R1's
  zero-measured fallback.
- `L38-39` requires `label_schema == 3` and `capture_complete is True`, with no provision for a
  deliberately incomplete failure-lever session or for a banked pre-m51 session.

It is rewritten, not adopted.

### 3.3 R8's tick-pin arm cannot run on either fixture

`tickpin_compiled` reads **false** on both - banked `PB1G_BL_NAT` (StackOBot) and `LYRA_SMOKE_01`
(Lyra) - because both are stock UE 5.1 and the pin compiles only behind `ANOMINJECT_FW_TICKPIN`.
The "with the pin applied" half of R8's gate is structurally unrunnable here and is declared so.
`SynthTickOrder` does not substitute: it relocates dispatch within a tick, it does not produce a
tick with no render. The runnable substitute is `IAI.Capture.SVE.ForceMiss`, which leaves the same
`PendingSnapshots` residue an unrendered tick leaves - details in the predictions file section 6.4,
including the limitation that it proves the recovery path and not decoupled-host behaviour.

### 3.4 The handoff's own hide-class sentence is superseded

`CHAT-HANDOFF-077...` section 1 still reads *"Hide-class `observable` = labelled && DrawnCount == 0
&& silhouette in view"*. That predicate was **removed** in `1c3d62c` and the same handoff's section 2
records the refutation (`G258`). Recorded here per the brief; the handoff is history and is not
retro-edited.

---

## 4. Architecture decisions

The full truth tables, the lifetime/ownership table and the writer arithmetic are in
`docs/predictions/2026-09-06-m51-release-hardening.md` sections 2, 3, 4. The decisions that a
reviewer should attack first:

1. **Cancellation is a terminal outcome with immediate capacity release.** Every accepted request
   ends in exactly one of Committed / Cancelled / Dropped / Failed. A cancelled arm retires its
   `PendingSnapshots` entry and its mask and view bookkeeping in the same tick; an obsolete
   completion arriving afterwards is discarded by `Generation` and can neither resurrect the entry
   nor decrement capacity twice. This is the R8 defect Codex identified, and raising the cap from 8
   to 32 only delays it.
2. **A completion receipt carries the FINAL disposition.** A mask that failed to encode downgrades
   **both** the committed row (`mask_state:"unmeasured"`, `mask_file:null`) **and** the event's
   observation at that index (`target_pixels -1`, `observable null`), so annotation and row can
   never disagree - while the colour frame still commits. An image or label failure means there is
   no committed frame at all.
3. **`capture_complete:false` means an artifact that should exist does not** - write drop, readback
   drop, unrendered tick, mask drop. An **intentionally unavailable measurement** (Nanite, mask off,
   `OutputHeight != 0`, shaders pending) is **not** incompleteness; those sessions are complete and
   full of honest `-1`s. Conflating the two would make every Nanite-heavy host session read as
   broken.
4. **Phase B survives as behaviour, not as key names.** The injection-free exposure baseline must
   not render the target-excluded detector inert - `exposure_dip_scope` present but ineffective is a
   failure - and PB-5 stays in the set **in both directions**. `target_drawn_pixels` remains evidence
   of absence only (`G258`); row I of the truth table (hide-class, `DrawnCount > 0`, still
   `observable: true`) is the ruling that keeps a true positive label on a real host.
5. **The console mutation path stays open and counted.** R4's WebSocket refusal is kept; the
   branch's console no-op is rejected because the bench's own can-fail levers inject during capture.
   Attempts *and* outcomes are logged with the session index so a no-op cannot masquerade as a
   successful lever (`G245`'s lesson: a lever that silently did nothing produced an artifact
   identical to the control). Auto-injector operations do not count as external mutations.
6. **Historical tag records outlive live leases.** F3 separates the two: `EventClaimed` is released
   when the fire ends *and* all of its rendering and readback work has terminated, while the record
   is kept for history. Reuse safety comes from `Generation` plus the existing owner-violation
   detection, not from a blanket quarantine - the 075 quarantine design was refuted by its own
   priced cost (it shrinks the pool that was already exhausting, `G250`/`G253`) and is not revived.
   Census headroom (`EventTagHeadroom = 8`) and host reservations are unchanged.

---

## 5. Gate matrix summary

Sixteen new legs (one per finding) plus a throughput leg and an auditor leg, each with a named
can-fail lever, an expected reading, an independent observation and an explicit stop condition -
predictions file section 6. The full existing regression set runs unchanged, both orders, both
fixtures, on one binary per fixture.

Three entries deserve attention here because they change what the review proposed:

- **F2's gate is a determinism test, not a bbox equality test.** The review's *"frozen bbox ==
  drawn bbox within tolerance"* is rejected: a projected AABB does not equal a drawn silhouette, and
  m30 measured that gap at ~4x on real content (`G149` amendment). Requiring equality would fail a
  correct static build. Instead, `bbox_px` must be **invariant to readback latency** - an A/B on the
  same seed at two latencies, where the pre-fix side diverges and the divergence grows with target
  speed. No synthetic truth is needed.
- **F4's lever may not exist on either fixture.** Producing a second eligible view family
  concurrently with capture needs a second PIE world or a second eligible viewport; if neither can
  be produced, the gate is reported UNRUNNABLE, never converted into a passing unit test.
- **R8 runs on `ForceMiss`, and its limitation is stated** - section 3.3.

**Binary provenance:** the one-binary rule is applied **per fixture**, since StackOBot and Lyra
cannot share a host executable. One frozen StackOBot exe + container and one frozen LyraEditor
build, both from the same plugin source revision, both hashes recorded, no substitution mid-campaign,
every discard banked and classified.

---

## 6. Cost and scope

Proposed, not established.

| tranche | content | estimate |
|---|---|---|
| implementation | commits 1-13 (predictions section 5.2), heaviest in `AnomalyCaptureSubsystem.cpp` | 2 Code sessions |
| new levers and leg recipes | eight new levers (predictions section 6.5) | inside the above, but the F4 lever may be unobtainable |
| gate campaign | 16 new legs x 2 orders x 2 fixtures + full regression + throughput + auditor | 1-2 Code sessions |
| docs | client README section 8 additive schema-3 + v2->v3 changelog, `capture-label-integrity.md`, checklist, gotchas, journal | 0.5 session |

**3.5-4.5 Code sessions**, consistent with the handoff's "expect 3-4". No cook is required
(predictions section 1, measured). Office actions and the M3 client contract are out of local scope.

---

## 7. Forks for the orchestrator

- **F-a** - does `masks_dropped > 0` alone set `capture_complete:false`? **Proposed YES** (a promised
  artifact failed). Cost: the R6 mask-failure leg then fails `--integrity-only`, which is correct for
  a failure leg but must be declared per leg.
- **F-b** - the m26 event veto. The branch **deletes** it. **Proposed instead: re-point its input** -
  veto iff the event has >= 1 measured frame and every measured frame reads `target_pixels == 0`.
  Deleting it retires `vetoed_events` (a `run_summary` key the client already has) to a permanent
  zero, which is the `G96` shape, and re-admits a class of events the client's dataset has never
  contained. Re-pointing satisfies the review's actual requirement - *"derive event summaries from
  the committed per-frame evidence; an observation from another time must not overrule that
  evidence"* - because the veto stops being an independent overruling measurement and becomes a
  consequence of the per-frame record.
- **F-c** - accept that F4's gate may be UNRUNNABLE on both fixtures, with its evidence
  correspondingly weaker?

---

## 8. What this session did NOT do

No source file was edited. No build, cook, capture, leg, test run, binary staging, worktree change,
merge or tag. `master` and `fix/capture-label-integrity` are untouched as commits; the only remote
write was the authorized backup push of the source branch. The staged bench binary and container and
the Lyra worktree were hashed and left byte-identical. Nothing was written into `_mailbox`. No office
data was read or moved.

---

## 9. CORRECTIONS — appended 2026-09-06 by brief 078-02 (append-only; nothing above is rewritten)

Two written reviews returned **CHANGES REQUIRED**: `_reviews\078-01-codex-preflight-review.md`
(eleven objections) and `_reviews\078-01-chat-predictions-review.md` (agrees with all eleven, adds
four blockers). The corrected active specification is **AMENDMENT 1** of
`docs/predictions/2026-09-06-m51-release-hardening.md`; the full session record is
`docs/sessions/2026-09-06-078-02-m51-predictions-revision.md`. Four claims made above are corrected
here, and the corrected statement governs.

1. **§2's "all sixteen reproduce" is a STATIC confirmation, not a runtime reading.** Every finding
   was re-located at `ac13700` and the cited paths exist as described. **No capture ran.**
2. **§2.1's frame-loss fraction is ARITHMETIC from the drain structure, not a measurement** — and its
   trigger claim is wrong. `client-delivery.md:529` and `client-readme.md:715` only *document* that a
   non-zero `OutputHeight` refuses the mask; the host `Config/DefaultGame.ini` `[AnomalyCapture]`
   block sets **no `OutputHeight` key**, so the compiled default `0` (native) holds. **F1 is a
   configuration-dependent P1**, not one reachable by shipped defaults. Severity on firing is
   unchanged.
3. **§5's "F2's briefed gate is REJECTED and REPLACED" mis-credits the session.** Brief 078-01 §3
   item 9 *already* demanded an independent same-frame projection reference and explicitly rejected
   the equality. The equality proposal came from the earlier chat review, not the brief. Nothing in
   the brief was refuted.
4. **§3.1's "three premises refuted" overstates the framing.** Brief 078-01 §3 item 2 listed those
   items inside the implementation sequence without asserting they were master defects. The
   *measurement* — which ref each item lives on — stands and is useful; the word "refuted" does not.

Additionally, **§5.1's conflict-trap sentence is corrected**: a three-way merge uses the merge base
and retains non-conflicting changes from both sides, so it does **not** necessarily delete Phase B.
The dangerous operations are applying the endpoint diff or replacing whole files. The hunk-by-hunk
port remains the strategy. And the empty shader diff supports **no cook for the port**, not no cook
for the milestone.
