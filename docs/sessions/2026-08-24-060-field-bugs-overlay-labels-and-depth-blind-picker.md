# 2026-08-24 — 060 — Field bugs: overlay asset labels, dashboard names, and the depth-blind picker

**Two tester-reported field bugs, both closed on their SELECTION half. Zero engine code changed. No
cook. No tag. The m34 branch `feature/mask-gpu-reduce` was NOT touched — it stays at `d3b9f08`,
unmerged.**

Numbering note: journals **058** and **059** live on the m34 branch, not on master. This is **060**
to avoid a collision when that branch merges.

---

## What shipped

| Repo | Commit | What |
|---|---|---|
| AnomDash | `68bf3c0` | targeted-mode target dropdown shows the asset name |
| AnomDash | `c3ce8ce` | Active + now-firing rows show the asset name |
| AnomDash | `5d35cbe` | click-to-select ranks by distance, not projected area |
| AnomInject | `962dd29` | overlay boxes label the ASSET name, actor name dimmed |
| AnomInject | *(this commit)* | docs — this journal + the pre-delivery boxes |

Gates: `tsc --noEmit` exit 0 · **79/79** vitest (10 new in `src/lib/geom.test.ts`) · comment
stripper 0-changed in both repos · every diffstat proportional to its change (**G115** pre-commit
read, clean each time).

---

## BUG 2 — overlay boxes named the actor, not the asset

Boxes read `blinking BP_MovingPlatform_C_UAID_B42E9936F542EBDB00_1649270448`, which identifies
nothing to a human looking at a frame. They now read
`blinking SM_Modules_Platform (BP_MovingPlatform_C_UAID_...)` — asset primary in the box colour,
actor name dimmed grey, matching the session-053 dashboard convention. The actor name stays because
it is the join key against `annotation.json` and `labels.jsonl`.

**Gate — banked `CM_CM_PLAT/session_20260819-194623`, before/after counts IDENTICAL:** 59 frames
had boxes, 59 written of 90, 30 RED SHIPPED / 29 AMBER OUTSIDE-SUBSET. Only the label text moved.
Vacuity guard proven by blanking `asset_name` on every node → 59 boxes fall back to actor-only,
counts unchanged, and the new fallback note fires with the right count. Third path proven: no
`annotation.json` at all → unchanged NOTE path, all RED, actor-only.

⚠ **A LIMIT THAT LANDS ON BUG 1's OWN OVERLAYS.** `asset_name` exists only in `annotation.json`'s
`affected_objects.nodes[]` (m22). **`labels.jsonl` carries only `target_name`.** So a box whose
event is NOT in `annotation.json` — **VETOED** and **UNMATCHED** — has no asset name to show and
stays actor-only. That is exactly the Bug 1 session. The tool now prints how many boxes took the
fallback and why. Closing it properly means adding `asset_name` to the `labels.jsonl` anomaly
record, which is a **label-contract change (`P6` territory)** — **NOT done, flagged for ruling.**

Bundle mapping needed no change: `PLUGINFILE tools/verify_capture.py` ships this exact file and
there is no second copy to drift.

---

## BUG 1 — resolution summary

**The chain, and every link is established:**

> depth-blind pick → decal target → opaque swap renders visibly → veto correctly zeroes →
> empty annotation.

1. `pickActorAt` kept the **smallest projected rect** containing the click and read **no depth at
   all** — `VisibleActor.dist` was populated engine-side (`AnomalyViewport.cpp:902`) and never
   consulted, and any rect ≥ 0.8 of frame was skipped outright.
2. So a click aimed at a wall selected the small decal mesh lying on that wall's face —
   `Wall_Grime_F_CR_Decal_INST776`, `StaticMeshComponent`, **TRANSLUCENT**.
3. The opaque material swap then rendered **visibly, on the wall, where the tester was looking** —
   which is why the tester reported the effect on the front object while the box sat on something
   smaller. There was never a front/back contradiction to resolve: the decal is ON the front face.
4. The mask measured the decal at **zero**, the veto removed all 10 events, and the session shipped
   `annotation.json` with **no anomalies at all**. `vetoed_events=10`, `translucent_vetoes=10`.

**The veto is exonerated and stays ZERO-ONLY.** It measured a real zero and did the right thing
with it. `translucent_vetoes` counting a **property of the measured target** — m27's hedge — is
what let the counter be read as evidence about **WHICH object was measured**, and it paid for
itself here.

### What the source read established, and it is worth keeping

**Fire-onward there is no divergence, and none was invented to explain the report.** All three
resolution sites read the SAME `AActor*` stored on the LiveFire at fire time
(`AnomalyAutoInjectorSubsystem.cpp:343-344`):

- fire target — resolved once by `=` **exact** match, `AnomalyAutoInjectorSubsystem.cpp:314-323`
  (the `=` is prepended **engine-side**, so substring matching is excluded by construction)
- label node — `ResolveNodeIdentity(F.TargetActor.Get())`, `AnomalyCaptureSubsystem.cpp:2629`
- label bbox — `F.TargetActor.Get()`, `AnomalyLabelWriter.cpp:71-73`
- mask tag — `FindOrAddRecord(..., F.TargetActor.Get())` `:2638` → `TagActor`,
  `AnomalyMaskMeasure.cpp:169-180`

**No name re-resolution happens anywhere after the fire.** The dashboard-side string path is
likewise single-source: `selectedActor` is one field with exactly three writers
(`PreviewCanvas.tsx:151`, `TargetsPanel.tsx:30`, `CapturePanel.tsx:86`); `setSnapshot`
(`store.ts:146-161`) never touches it — no reconciliation, no expiry, no re-resolution.

### The fix that shipped

`AnomDash 5d35cbe`. Containing candidates ranked **nearest-first, smaller area as tiebreak**; the
≥ 0.8 near-fullscreen rule changed from **exclude** to **deprioritise** (a backdrop still loses to
anything else under the cursor, but a large foreground object is no longer unclickable — before,
such a click selected whatever small thing was under it, or nothing).

⚠ **Stated weakness, not smoothed over:** `dist` is bounds-origin to view-origin, **not depth at
the clicked pixel**, so a long object whose origin is far but whose near face is close can still
lose. It is a better proxy than area, not a correct one.

---

## PARKED — recorded, deliberately NOT worked

- **Route (e) — the measurement half.** Why an opaque swap on a translucent-slotted target measures
  zero. Self-vs-shared blast-radius read + corruption-material blend-mode check +
  dataset-consequence paragraph. **QUEUED for post-delivery.** Evidence banked: owner photo set,
  the kept log, and the `VETOED-OBJECT` line —
  `Wall_Grime_F_CR_Decal_INST776 / StaticMeshComponent / TRANSLUCENT`. **This is the first FIELD
  receipt of route (e); it was previously "UNMEASURED on any delivered title".**
- **Engine-side line-trace picker** — the correct depth fix (trace from the clicked pixel rather
  than ranking bounds). **Filed candidate.** Needs a new WS command and a cook.
- **General translucent-target selection exclusion** — a broader rule than the two patterns below.
  **GATED design question, owner sign-off required.** Not designed, not proposed.
- **PMCore `"Rewindable tick requires fixed f..."` handled-ensure sighting** — ledger entry,
  **office-pass item**.
- **`asset_name` in `labels.jsonl`** (from Bug 2's limit above) — would let vetoed/unmatched boxes
  carry an asset name. `P6` contract change, **needs a ruling**.

## RECORDED DECISIONS

- **Canonical Concorde ini block gains two lines, landing at the NEXT cook — the shipped bundle is
  NOT re-cooked for this:**
  ```ini
  +ExcludedTargetNamePatterns=Decal
  +ExcludedTargetNamePatterns=_CR_
  ```
  Both boxes are now in `PRE-DELIVERY-CHECKLIST.md` §1 so they survive a session that has forgotten
  this journal. ⚠ The canonical pre-cook block itself lives in
  `docs/CHAT-HANDOFF-crisis-weekend-delivery.md` §3, which is **untracked in git** — the two lines
  must be added there by hand before the cook.
- **Next-delivery client note:** decals / overlay meshes are excluded from targeting.
- **Severity / client comms:** chat's call. None drafted here.

## NOT DONE, named rather than implied

No engine code changed · no cook · no tag · `P6` did not move · `feature/stencil-capture`
untouched · `feature/mask-gpu-reduce` untouched at `d3b9f08` · no force-push · no ratio, no
threshold proposed anywhere · route (e) still **documented, not fixed**.
