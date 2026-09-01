# 2026-08-21 — session 050 — m31 fix: the SVE wanted-handshake re-keyed off the engine clock

## §1 Goal

Option B ruled: design and implement the mechanism-independent fix for the SVE zero-frames
defect on Concorde, in one turn, with its own permanent gate instrumentation, validated on
StackOBot (the only locally testable host) on BOTH capture paths, pushed for office-side V-3/V-4.

## §2 STATED PLAINLY, FOR THE RECORD

**FIRST DEFECT EVER FOUND BY A SECOND HOST, AND THE SHIPPING DEFAULT CAPTURE PATH WAS THE BROKEN
ONE.** The SVE path — default since m25, certified across ten configs, every ratio regime, both
delivery modes, four milestones — wrote ZERO frames on Concorde (forked engine loop: FIXED game sim +
VARIABLE render, engaged by the owner's ordinary networked-client workflow). The backbuffer path
wrote 120/120 across three sessions on the same build. **StackOBot could never have caught this:
every certification leg ran on one host with a lockstep engine loop. Fourth instance in five days
of certification depth on one axis proving nothing about a second axis** (m27's settled camera,
the m30 near-vacuous gate, m31's single-host certification — and now this fix's validation, which
is why V-3/V-4 are Concorde legs and the StackOBot legs claim only NO-REGRESSION).

## §3 The mechanism is DELIBERATELY UNSETTLED — and the retraction chain, recorded honestly

What is ESTABLISHED (plugin-side verified from source here; engine-fork claims OFFICE-CITED and
unverifiable on this box): the pipeline died at the single silent post-lookup exit
(`!Entry.bWanted`); publish is `bWanted`'s only writer; the ring was PERFECT on the broken host
(`published==consumed`, 133/133 and 332/332, `missed=0`) — both sides agree on the KEY
(`FSceneViewFamily::FrameNumber`); ONLY THE PAYLOAD was wrong. OFFICE-CITED: the fork increments
`GFrameCounter` AFTER `GameSimTickFunc` (`FWFixedVariableLoop.h:620-628`); 89 sim ticks against
133 publishes over one window; a clean 0-of-60 under consecutive arming means the two sequences
were DISJOINT, not offset.

**The retraction chain:** an earlier office mechanism ("GFrameCounter temporarily overwritten
with GFrameNumber") was published, then SELF-RETRACTED — the swap is undone before the publish
site runs. Chat's multiple-view-families candidate was REFUTED (publishes match the render-frame
span). **Chat CANCELLED the S1 instrumentation on the strength of the retracted mechanism, then
reinstated and finally superseded it with this fix.** THE LESSON, verbatim into the record: **a
mechanism assembled from source reading alone is a hypothesis however many citations it carries;
only a measurement that reads BOTH SIDES settles a handshake.** The fix is mechanism-independent
BY DESIGN — it is immune to every candidate mechanism, which is what made settling the mechanism
optional; the diagnostic instrumentation was traded for a SELF-DIAGNOSING fix.

## §4 THE INVARIANT, WIDENED (owner ruling, dated — recorded in architecture.md §Game-agnostic)

Not just "never reference host TYPES": **never let correctness depend on anything a host can
redefine.** m31 was that invariant violated through an ENGINE GLOBAL — the plugin used
`GFrameCounter` as a cross-thread handshake token, comparing two independent reads whose
agreement is a property of the stock loop's increment placement, which a fork may lawfully
change. The fix REMOVES the dependency; it does not compensate (no engine-mode sniffing, no
branching — a fork can redefine anything we might sniff; the next fork will redefine something
else). **No tolerance windows anywhere: office-cited rate evidence says the clocks can run at
DIFFERENT RATES, and no fixed tolerance bridges a diverging gap.**

## §5 THE DESIGN — both shapes evaluated, the choice and its reasons

**Chosen: Shape 1's FIFO pairing semantics carrying Shape 2's plugin-owned token as the
identity.** The pure forms each fail a binding constraint:

- Pure Shape 1 (wanted-COUNT only) leaves no identity for `PendingSnapshots` — pairing snapshots
  to completions would rest on order-of-completion assumptions, reintroducing the constraint-2
  silent-drop trap one stage downstream.
- Pure Shape 2 ("token matched at publish") is unreachable as stated: no view family exists at
  arm time, so publish can only learn which token by ordered handoff — which IS a FIFO.

The synthesis is the backbuffer pattern transplanted. **Why the backbuffer path is
fixed/variable-safe BY DESIGN, established from source (this is what makes Shape 1 inheritable
rather than hopeful):** `ArmForCapture` (`AnomalyFrameCapturer.cpp:49-67`) mints identity ONCE,
game-side, and ships it BY VALUE inside the arm struct through the FIFO-ordered render-command
stream (the m21 fix); `OnBackBufferReadyToPresent_RenderThread` (`:81-86`) consumes
`PendingArms[0]` by WINDOW MATCH + ORDER — **no frame number is read, re-derived or compared
anywhere on the render side**; completion (`:160`) returns the minted value verbatim, and the
game thread pairs it against a map keyed from the SAME single read site. Its only cross-thread
agreement is ORDERING, which no clock redefinition can break. **One latent flaw even there:**
`GFrameCounter`-at-arm as the token depends on that global being UNIQUE per arm — a fork ticking
sim faster than render mints duplicates. So the token becomes a plugin-owned monotonic serial
(`++CaptureRequestSerial`, minted once in `CaptureCurrentFrame`), applied to BOTH paths — the
widened invariant applied consistently, not only where it already hurt.

Decision criteria as ruled: (a) least annotation-contract distortion — the token is PAIRING-ONLY;
every artifact value keeps its source (`Snap.FrameCounter` stays arm-time `GFrameCounter`, so
`labels.jsonl` `frame_index` and the bench marker oracle are untouched), and on any lockstep host
the FIFO degenerates to the identical pairing (measured: see §8); (b) no cross-clock comparison
anywhere — the sole one (`IsWanted(publish-read)` vs mark-read) is DELETED, grep-provable;
(c) simplicity — mirrors the proven path; the wanted-set becomes a FIFO.

**The pairing semantics on decoupled hosts** ("the captured frame is the next eligible family
after the arm" — the backbuffer's m21/m22-characterised semantics) are documented in
capture-fps.md §Arm→frame pairing semantics. Field set unchanged; P6 does not move.

## §6 WHAT SHIPPED — files and consumers (constraint 2's enumeration)

- `AnomalyCaptureSubsystem.{h,cpp}` — `CaptureRequestSerial` minted once per armed frame at the
  single arm site; keys `PendingSnapshots` (Add), both capturers' arms
  (`ArmForCapture(RequestId,…)` / `ArmWanted(RequestId)`), `ArmedLabelRequestId` (m18
  finalize), `DeferredHiddenRequestId` (m20 deferred-hidden) — every consumer of the old
  `GFrameCounter` key, re-keyed from ONE mint. Artifact stamps (`Snap.FrameCounter`, run.json
  `start_frame`, run_summary `end_frame`, sync/manual-shot paths) keep `GFrameCounter` as a
  VALUE — none is a pairing key.
- `AnomalySveCapturer.{h,cpp}` — `MarkWanted`/`IsWanted`/wanted-set REPLACED by a pending-wanted
  FIFO: `ArmWanted` (push + arm trace), `ConsumeWantedForPublish` (pop-front for the next
  eligible family + publish trace), handshake stats (arms, matches, submits, max/now FIFO depth,
  ineligible families), bounded trace (first 64 each side, compiled constant).
- `AnomalySveKeyRing.{h,cpp}` — payload renamed `RequestId`; the KEY is untouched:
  `FSceneViewFamily::FrameNumber`, the one value MEASURED-CORRECT on the broken host
  (`published==consumed`, `missed=0` in the exact failing configuration). Counter renamed
  `WantedMatches` (Amendment 3: name = semantics — it counts publishes that consumed a pending
  arm).
- `AnomalySceneViewExtension.cpp` — FAMILY-ELIGIBILITY GUARD: scene-capture / reflection-capture
  families must not consume an arm; their views' flags are set in the init options
  (`SceneCaptureRendering.cpp:625`) before construction, so they are readable at
  `BeginRenderViewFamily` (`SceneRendering.cpp:4590`). Ineligible families are COUNTED
  (Amendment 2) so a mis-guard on a future host localises itself from the gate line. Then:
  consume → publish. `AfterPass` submits `Entry.RequestId` — rename only.
- Pair-drop PROMOTED Verbose → Warning with its own token (G154): the silence that made a broken
  pairing indistinguishable from a path that never submitted.
- `AnomalyLabelWriter.{h,cpp}` — run_summary field `wanted_matches` (renamed from v1's
  never-consumed `wanted_published`; counts wanted-matches made; semantics line in
  capture-fps.md).
- outDir hardening: `IAI.Capture.Start` strips one wrapping quote pair; a surviving quote char
  or an uncreatable run directory REFUSES LOUDLY at StartRun (log Error + own token, run never
  starts, auto-injector resumed) — closing G153 (addendum appended there).
- STALE BRIEF ITEM, refused: the `IAI.Capture.Mask` help string was ALREADY FIXED at m29 (its
  text now reads "SLICES 1+2+3 — MEASURE, REPORT AND VETO" with ini provenance) — verified in
  source, no work done. The mask-measure path was also verified value-carried (its `RequestId`
  travels by value; no `GFrameCounter` in the mask SVE) — already immune, out of scope.

**FIFO overrun behaviour, stated (approval item):** when arms outpace eligible publishes, the
FIFO grows — bounded by the run's arm count (one entry per captured frame; no cap needed) — and
unconsumed arms reach run end LOUDLY twice: their snapshots hit the existing
"N frame(s) did not resolve by run end (dropped)" WARNING (`DrainAsyncToCompletion`), and the
gate line prints `pendingWantedAtEnd`. `SetActive(false)` at FinishRun stops publishes, and
`Reset()` at the next StartRun clears leftovers — no cross-run leakage.

**Tokens** (VETOED-OBJECT discipline; named once, here): per-event trace `SVE-WANT-TRACE`
(2 emit sites: arm + publish), gate line `SVE-WANT-SUMMARY` (1), pair-drop `CAP-PAIR-DROP` (1),
RunDir refusal `CAP-RUNDIR-REFUSED` (2 sites, one failure class). All verified unique repo-wide;
no referring line repeats a literal. A44 on the STAGED exe, both encodings: all five present at
exactly emit-site multiplicity, and the retired `wanted_published` ABSENT — sound both ways.

## §7 Pre-registration

`docs/predictions/2026-08-21-m31-fix-validation.md`, committed with the fix: V-1…V-4 (both-path
variants per Amendment 1) and V-3's failure branch — a still-zero Concorde run MUST localise to
arm-side / publish-side / eligibility-skip / pairing from the gate line alone, and an
unlocalisable zero is a gate failure OF THE INSTRUMENTATION. Restated verbatim before any leg
was read.

## §8 VALIDATION RUN HERE — V-2 (packaged, both paths, true A/B) — ALL PASS

Environment: pre-fix legs on the CERTIFIED m30 staged exe `99AE7526` (archived to
`_binary_baselines\StackOBot.exe.m30-prefix-m31-99AE7526`, hash-verified at the new location);
post-fix exe `DC55CB9B` built==staged verified (game target built `-NoDebugInfo` — the monolithic
PDB blew the §8.6 disk floor; a build-config choice, no code effect). Harness:
`run_leg.ps1`, CB_GateLevel, TARGETED blinking on `StaticMeshActor_49`, seed 777, 90 frames —
targeted per G150 so the legs are comparable across the pool changes since m27.

Four legs, all A63-valid, B1 pose gate PASSED on three (modal_rot (0,0,0), CALIB bbox exact;
the pre-fix BB leg's attempt 1 was a genuine A47 pose discard, banked, re-run valid):

- **SVE pre (`99AE7526`) vs SVE post (`DC55CB9B`):** 90/90 frames, 0 zero-byte, 8 events, EVERY
  event identical (type, span, indices count, manifested — canonical gapped cadence [4..10]…),
  `annotation.json` KEYSET IDENTICAL (P6 unmoved, MEASURED), key ring identical
  (121/121/0/57/0), positives 59 both. run_summary differs by EXACTLY the one pre-declared
  field: `+wanted_matches = 90`.
- **BB pre vs BB post:** structurally identical everywhere; run_summary keysets identical.
- **The gate line, post-fix SVE leg:** `marksIssued=90 publishesSeen=121 wantedMatches=90
  submitsIssued=90 framesWritten=90 pendingWantedAtEnd=0 maxPendingDepth=1` — the handshake
  CONNECTED end to end, and `maxPendingDepth=1` is the lockstep degeneration measured: on this
  host the FIFO never holds more than one intent, i.e. the pre-m31 pairing exactly. 31 of 121
  publishes were unarmed lead-in/settle frames — bWanted=false, correct. 128 trace lines
  interleave arm→publish with depth 1→0 every cycle.
- **Refusal guard POSITIVE test (G96):** `IAI.Capture.Start Q:\m31refuse …` on the staged exe →
  `Error: … could not create run directory … REFUSING TO START` with the token, no run started,
  nothing written. (Branch exercised: MakeDirectory failure; the quote-strip branch shares the
  same refusal surface.)

**V-1 (StackOBot PIE): NOT RUN IN THIS SESSION — stated plainly, not silently skipped.** No
editor was running and no bridge was reachable; PIE smokes in this project are owner-run gates.
The packaged pair is the stronger instrument on this host (G76), and the owner's office
procedure (§10) includes the in-editor capture that covers V-1's instrument where it matters —
on Concorde.

## §9 NOT DONE, named

- V-3 / V-4 (Concorde in-editor + packaged) — office-side, after this push. **m31 does NOT tag
  until V-4 passes.**
- The mask measure system on a fixed/variable host: its pairing is value-carried (immune), but
  its MEASUREMENT cadence under a decoupled loop has never been observed; its failure direction
  is NOT_MEASURED ⇒ ADMIT (safe). Named, not chased.
- `labels.jsonl` `frame_index` on decoupled hosts identifies the ARM TICK, not the rendered
  frame (capture-fps.md). Lockstep hosts unchanged. No consumer is affected today; delivery
  mode never writes it.

## §10 THE OWNER'S OFFICE PROCEDURE (V-3) — written for reading

1. Pull. Rebuild the EDITOR target (G47/G131).
2. Play in-editor, normal workflow (fixed/variable engages itself). Run ONE SVE capture —
   defaults, no cvar needed.
3. Read off the screen: PNGs in `Actual_Frames/`, `total_frames` in run_summary, and the
   run-end summary line — its token is named in §6; healthy = the four numbers agree.
4. Run ONE backbuffer sanity capture: `IAI.Capture.SVE 0`, capture, confirm PNGs — thirty
   seconds, guards the only previously-working path (the serial mint touched it too).
5. If the SVE capture is still zero: grep the two trace/summary tokens (§6) and read the gate
   line against the pre-registered failure branch in
   `docs/predictions/2026-08-21-m31-fix-validation.md`. It will name arm-side, publish-side,
   eligibility-skip, or pairing. Report the line verbatim to chat. NO same-turn fix.
