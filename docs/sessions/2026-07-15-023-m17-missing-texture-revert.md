# 2026-07-15 — 023 — m17: harden the missing_texture revert for runtime / modular-character materials

Base: plugin `84dfa52` (tag `m16`, tree clean at start), dashboard `f978f1b` (untouched this milestone).
Design → build → validate in one turn (the diagnosis + local repro landed in the previous two sessions;
journal 022 is m16). **No commit this turn** — owner review first.

## Goal

Fix the confirmed Concorde bug: `missing_texture` does not revert on runtime/modular characters (body-only;
props and StackOBot's plain content revert fine; `IAI.RevertAll` fails too). The anomaly saved the slot's
original material behind a `TWeakObjectPtr` and restored through a saved component pointer; on characters whose
own runtime logic re-creates the component and/or its materials after Apply, both pointers are dead by Revert,
the stale-skip silently skipped, and the corruption persisted on the live component. Severity: post-revert
frames are LABELED CLEAN while the pixels stay corrupted → dataset contamination on client titles.

## What was done — the locked fix (D1–D5)

Only the anomaly's own two files changed. `IAnomaly`, the injector, the other anomalies, the leaf helpers and
the capture loop are byte-unchanged; no new module dependency; catalog stays 8.

### What Apply now saves (D5)

`FCapturedSlot` (`Anomaly_MissingTexture.h`) gains two fields alongside the existing
`Mesh`/`SlotIndex`/`OriginalMaterial`/`bWasExplicitOverride`:
- `TWeakObjectPtr<AActor> Owner` — the component's owning actor, the anchor for re-finding.
- `FName ComponentName` — identity that survives the component pointer.

The anomaly also stores `TWeakObjectPtr<UMaterialInterface> AppliedChecker` (the material it actually applied),
because `IAnomaly::Revert()` takes no arguments and the D3 guard must identify our own material without world
access. **Apply's visible behavior is unchanged** — same resolution, same per-slot `SetMaterial(i, checker)`,
same logs.

### How Revert now restores (D1–D3)

1. **Re-find, don't trust (D1).** Per captured slot: use the saved component if still alive, else re-find a
   live component with the same name on the saved owner (`FindLiveComponentByName`). Counted as `re-found`.
2. **Guard first (D3, load-bearing).** Only touch a slot whose CURRENT material still *is* our corruption —
   `IsCheckerDerived()`: identical to the applied checker, or a material instance whose parent chain reaches it
   (catches a game MID layered on top of our checker; depth-capped at 8). If the game already re-took the slot,
   leave it and log it (`left-to-game`). This is what stops us fighting the character system.
3. **Restore (D2).** Saved original still alive and it was an explicit override → restore that exact object.
   Otherwise → `SetMaterial(slot, nullptr)`, which clears the override so the mesh's built-in default renders
   and **the game re-takes ownership on its next material re-assertion** (`default-reset`). A dead runtime
   original logs a Warning.
4. **Sweep (D1, the coverage net).** For every touched owner actor, walk all live `UMeshComponent`s and all
   slots; anything still holding our checker (i.e. corruption that rode `OverrideMaterials` onto a *successor*
   component we never captured — name-matching cannot catch those) is reset to the mesh default and warned
   (`swept`). The guard makes the sweep idempotent, so restored slots are never double-touched.
5. **No more silence.** Every revert logs `restored=/default-reset=/left-to-game=/unresolved=/swept=/re-found=`,
   with a Warning per unresolved/swept/dead-original. Previously a failed revert produced no trace at all.

The one conservative refusal: if `AppliedChecker` cannot be resolved (a can't-happen path — the checker is a
CDO hard-ref on the injector subsystem), Revert cannot verify slot ownership, so it logs an Error and touches
nothing rather than risk stomping game materials.

### D4 — modular-character targeting (answered empirically, not by inspection)

Repro Mode 3 models the writeup's `FWMasterSkeletalMeshComponent` shape: a master component plus a
master-posed sub-part, both carrying runtime MIDs, where the sub-part is re-created while the master
re-asserts its own MID. **Finding: the correct restore target is neither "the master" nor "the sub-parts" as a
category — it is whatever components are LIVE ON THE ACTOR at revert time**, which the re-find + per-owner
sweep delivers. Both are reached because both are components of the same actor (`GetComponents<UMeshComponent>`
at revert time, not the saved set). A single revert handled them with *different* dispositions in one pass:
the master's re-asserted MID was left to the game (guard), the rebuilt sub-part's checker was swept off the
successor. The restore does **not** need to survive the character system's next re-assertion — surviving would
mean *fighting* the owner system; resetting to the mesh default is precisely what lets the system re-take the
slot cleanly (observed: the harness re-asserts normally on its next tick).

**Stated limitations** (honest scope of the guarantee):
- We reach mesh components **on the matched actor**. Sub-parts parented to a *different actor* (child-actor
  equipment) are out of reach of a per-actor revert — targeting would have to match that actor too.
- For a successor component we never captured we have no saved original, so the sweep can only reset to the
  mesh default (the game re-takes it) — it cannot restore a game MID's accumulated parameter state.
- A fully-custom merged proxy that rebuilds itself *from a pre-revert snapshot* after our revert could re-apply
  corruption we already cleared; nothing short of a post-revert re-check would catch that. Not observed in the
  repro, flagged as theoretical.
- Concorde itself remains the final confirmation (owner-run, office box) — local repro is a model, not the title.

## Files touched

Plugin (the m17 commit): `Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.h`,
`Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.cpp`.
Docs: this journal, `architecture.md` (save-state shape + revert contract), `gotchas.md` (G74–G76),
`CLAUDE.md` status.
**NOT part of the commit — validation asset only:** `D:\IntrusiveAnomalies\StackOBot\Source\StackOBot\
MidReproActor.{h,cpp}` lives in the **StackOBot project's game module**, not in this repo (the plugin repo
shows only the two anomaly files as modified). Console: `SOB.MidRepro.Spawn <1|2|3> [period] [nameSuffix]`,
`SOB.MidRepro.State`, `SOB.MidRepro.DumpMaterials <substr>`, `SOB.MidRepro.Watch <substr>`.

## Gates — all run in a LOCAL PACKAGE (`Builds\MidRepro\Windows`), not PIE

Driven headless over the control server's own WS surface (`-ExecCmds` + a raw-socket probe); `=` exact
targeting throughout.

- **G1a — immediate revert, no churn: PASS.** `restored=1 default-reset=1 left-to-game=0 unresolved=0 swept=0`;
  slot 0 came back as the *same* MID object it was before Apply.
- **G1b — revert after component churn (the m16 failure): PASS.** Saved component destroyed → `unresolved=2`
  (now warned, not silent) → sweep found the checker on the successor → `swept=2` → `CHECKER_STUCK=no`, slot 0
  back to the mesh default `M_BotBase`, and it stays clean across further rebuilds. Under m16 this same
  sequence left `CHECKER_STUCK=YES` indefinitely.
- **G2 — `revert_all` clears it: PASS.** Identical counters and end state via the WS `revert_all` path.
- **G3 — the guard holds: PASS.** With the harness re-asserting its own MID during the hold, revert logged
  `revert left '…' slot 0 untouched — the game replaced it with 'MaterialInstanceDynamic_2147481914' after
  apply` (`left-to-game=1`) and the game's MID survived; slot 1 (still ours) was default-reset in the same
  revert. m16 restored a dead pointer as null here and stomped the live game material.
- **G4 — regression on plain content: PASS (byte-identical behavior).**
  Static prop `StaticMeshActor_11` (3 slots, no overrides): `default-reset=3`, everything else 0; slots read
  back exactly `MI_Metal` / `MI_Carbon` / `MI_Plastic`, override None. Plain skeletal `SkeletalMeshActor_3`
  (which carries a *real* game-created MID on slot 1): `restored=1 default-reset=1`, zero skips/sweeps, and the
  MID restored as the same object (`MaterialInstanceDynamic_2147482464`). The new machinery is inert on normal
  content — the counters show it never engages.
- **G5 — capture/labeling correctness: the fix PASSES; a separate pre-existing defect was found (below).**
  Targeted capture on the fast-churning repro (100 frames, ~9 bursts): every burst's revert logged clean
  counters (mix of `restored=1` on unchurned holds, `swept=2` on churned ones), **zero silent skips**, and the
  interleaved timeline shows the checker cleared at every single hold→revert boundary — it never survives into
  the next phase. Targeted capture on the visible Bot (100 frames) additionally verified at the **pixel** level
  (dependency-free PNG decode, mean abs diff inside the labelled `bbox_norm`): post-revert frames return to the
  pre-fire baseline (diff ~3 vs ~24 while corrupted).

## Found while validating — PRE-EXISTING, NOT m17, NOT fixed here

The G5 pixel analysis surfaced a **1-frame label/pixel misalignment at both burst boundaries**, perfectly
periodic (stride 12 = one burst cycle), 17/100 frames in the default config:
- the **last lead-in / post-gap frame** of every burst is labeled clean but its pixels already show the anomaly
  (idx 3, 15, 27, … — a **contaminated negative**);
- the **last positive frame** of every burst has clean pixels but is labeled positive (idx 11, 23, 35, … — a
  **false positive**).

Cause (`AnomalyCaptureSubsystem.cpp` Tick, m7-era ordering, untouched by m17): each phase does
`CaptureCurrentFrame(); --PhaseFramesLeft;` and then, in the **same tick**, `BeginFire()` / `BeginRevert()` when
the phase empties. The label snapshot is taken at arm time, but the state change lands before that frame is
rendered — so the frame's pixels carry the new state while its label carries the old one. (The async log line
`armed frame id=N submitted (rtframe=N+1)` confirms the captured backbuffer is the end-of-tick-N render.)
**Not attributable to m17:** the fire-side mismatch involves no revert code at all, and m17 changed neither
Apply's behavior nor the tick ordering. It affects **every anomaly**, not just `missing_texture`, and every
delivered dataset. Fix shape for a future milestone (needs its own gates): perform the phase transition at the
TOP of the tick, before `CaptureCurrentFrame()`, so the armed frame and its label agree. Recommend prioritising
it alongside the m18 preview re-plumb — it is the same class of silent dataset contamination this milestone
exists to eliminate.

## CLOSED — Concorde confirmation RESOLVED (owner, office box, 2026-07-15, on pulled + rebuilt `m17`)

**The D4 open question is answered: the slot reset HOLDS on the real merged/master-pose proxy.** The owner ran
`missing_texture` apply → revert on Concorde's **actual character body (`FWMasterSkeletalMeshComponent`)**:
- **immediate apply → revert** → body reverts clean (the **re-find** path: the component is still the one we
  captured);
- **the CHURN case — apply → ~30 s of play (the character system re-creates the body materials/component
  mid-hold) → revert** → body reverts clean (the **staleness path**: what the re-find + **sweep** + dead-original
  default-reset exist for). *This is the scenario that previously left the hand stuck.*

**Both revert paths are therefore validated on the real proxy, not only on the model.**

The character system does **not** re-assert the checker back and does **not** leave the reset stuck, so **G77
outcomes 2 and 3 below are dead** and the modular-proxy follow-up is **not needed**. (The owner's report is
behavioral — "it reverts clean". Which internal branch fired on the real component, `restored` vs `swept`, is
readable from the new revert log counters if anyone ever needs that detail; both were exercised in repro.)

**Validation status, stated precisely — m17 is now confirmed on the real title, not just the model:**
- **VALIDATED on Concorde's real `FWMasterSkeletalMeshComponent`** (office box): immediate revert **and**
  revert-after-churn (mid-hold component re-creation) both clear the body.
- **VALIDATED on the local StackOBot repro, in a package:** the stuck revert clears — immediate and
  after-churn; `revert_all` likewise; regression-clean on normal props (`StaticMeshActor_11`) and plain skeletal
  content (`SkeletalMeshActor_3`, incl. a real game-created MID restored as the same object); the
  our-material-only guard holds (a game-re-asserted MID is left untouched).

**The mechanism note is kept below** — it is why the fix works, and it is where to look **if a future title ever
regresses this way** (the outcomes are retained as diagnostic history, no longer as open items).

**The D4 question this answered:** does our slot reset **STICK** on the merged/master-pose proxy, or does the
character system **re-assert over it** on its next update? Three outcomes were possible:
1. *Reset sticks, or the system re-asserts its OWN material over it* → m17 confirmed on Concorde.
   (The system re-asserting its own material is the intended outcome, not a failure — see G75.)
   **← THIS IS WHAT HAPPENED. Closed 2026-07-15.**
2. ~~*The system re-asserts the CHECKER back*~~ → would have meant the proxy rebuilt itself from a state snapshot
   taken **before** our revert. **Did not occur.**
3. ~~*The hand stays corrupted with no revert log activity on those components*~~ → would have meant the corrupted
   slots are unreachable from the matched actor (driven sub-parts on a *different* actor). **Did not occur** — the
   per-actor enumeration reaches the real proxy's slots.

**Retained diagnostic map (for a future regression, not an open item):**
- **Outcome 2 or 3 → `Anomaly_MissingTexture.cpp`, `Revert()`, the per-owner sweep loop** — the
  `Owner->GetComponents<UMeshComponent>(Components)` enumeration is the single place that decides *which live
  components we are allowed to touch*. It resolves the actor's components **at revert time**, which is what makes
  the master + live sub-part case work in repro Mode 3. If Concorde's driven sub-components are not in that set,
  this loop is where the targeting must be extended (walk attached/child actors, or resolve the proxy's driven
  sub-components explicitly) — **not** the captured-slot list, which by construction only knows the components that
  existed at Apply.
- **Outcome 3 also implicates targeting upstream:** `AnomalyLod::ResolveLodComponents` →
  `AnomalyTargeting::FindComponentsMatching<T>` is per-actor (`Actor->GetComponents<T>`), so a sub-part on a
  different actor is never captured in the first place.
- **Outcome 2 specifically:** the fix is a *timing* one (re-check/sweep after the system's next assertion, or reset
  on the driven sub-component rather than the merged proxy), not a targeting one. Note that m17 deliberately does
  NOT try to make a restore survive re-assertion (G75) — for outcome 2 the goal is to make the *checker* not
  survive, which is the opposite problem.
- **Diagnostic to run first on Concorde:** the new revert log line
  `restored=/default-reset=/left-to-game=/unresolved=/swept=/re-found=` now says which branch fired. `swept>0` =
  the sweep reached the corrupted successor. `unresolved>0` with the hand still corrupted = outcome 3.
  A revert that logs nothing on the hand's components = they were never captured (outcome 3).

## State — m17 CLOSED (confirmed on the real title)

Code + docs written; G1–G5 all green in a local package (`Builds\MidRepro\Windows`). Comment stripper run before
commit: 0 changed / 59 no-change (source already comment-free). Shipped as one `fix(missing-texture)` commit
`e2c6dd2` + tag `m17`, pushed (master and `refs/tags/m17` both confirmed at `e2c6dd2` on the remote) — pushed
**before** the Concorde test on purpose, since the fix had to reach the office box before the real component could
be exercised. The repro actor is **not** in the commit — it lives in the StackOBot project
(`Source/StackOBot/MidReproActor.{h,cpp}`) and the plugin repo tracks zero test files.

**The owner then pulled + rebuilt on the office box and confirmed the fix on Concorde's real
`FWMasterSkeletalMeshComponent` — immediate revert and the ~30 s churn case both clear the body (see the CLOSED
section above). m17 needs no follow-up; the modular-proxy contingency is dead.** This status flip is docs-only and
was deliberately **not** given its own commit — it rides along with the next commit that touches these files
(m18 or whenever), per the owner.

Still open, and **not** m17's doing:
- **Bug 1 — packaged black preview** → backbuffer tee. Deferred to **m18**, untouched here. Delivery-gating.
- **Burst-boundary label/pixel misalignment** (found while validating m17, pre-existing, affects **every**
  anomaly and every delivered dataset — see the section above). Recommend prioritising with m18.
