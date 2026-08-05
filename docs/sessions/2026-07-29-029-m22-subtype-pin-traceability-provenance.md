# 2026-07-29 — 029 — m22: blink subtype pin + G81 + annotation traceability + selection provenance

Base: plugin `b210a52` (docs) on top of `ed2b851`. Three commits: `af8d937`, `03a51d5`, `28bc6f1`.
**NOT tagged** (awaiting owner Play-gate smoke) and **NOT pushed** (owner owns remote pushes).

## Scope note — Part 1 was DESCOPED mid-brief

The first brief asked for a blink **single-toggle semantics** change (make one blink event produce exactly one
contiguous hidden run) with an investigation-first turn. The owner then **descoped it**: *blink's behaviour is
correct and does not change*. Multi-toggle events within a capture window are **intended**. So no investigation
was run, no anomaly behaviour was touched, and the only label change is the subtype pin. Registration order,
seeding and selection are all untouched.

## What shipped

### 1 — subtype pin (`af8d937`)
`MapAnomalyToClient` no longer takes a `Transitions` argument and no longer counts visibility transitions.
`blink` emits `anomaly_subtype = "disappear_reappear"` unconditionally. `"flicker"` **leaves the blink family**
and is reserved for the future separate **`flickering`** class (unbuilt; scene-region / light toggling; name
stays reserved). The `anomaly_subtype` **field is retained** in the schema so that class has a slot and the
client's file shape does not churn twice. Non-blink anomalies unchanged (they still mirror their id). The dead
transition counter + `PrevHidden` bookkeeping were removed; the hidden-index collection is otherwise identical,
so m20's sorted-key derivation of the hidden set is preserved.

### 2 — G81 fixed (`af8d937`)
`affected_frames.frame_count` was `end - start + 1`, i.e. a **span**. On a gapped blink event it reported **7**
for **4** real indices — in the file the client actively reads. It is now `len(frame_indices)`. The span stays
recoverable from `start_frame`/`end_frame`.
**Scope guard held:** `frame_count` exists only in `WriteSessionAnnotation` (`AnomalyLabelWriter.cpp`). The
m18-validated labels / range-builder path (`BuildFrameLabelRecord`) was **not** touched, so the stop-and-report
condition never triggered. The parked shared-range-builder refactor stays parked.

### 3 — B1 traceability (`03a51d5`)
`affected_objects.nodes[]` gains `asset_name`, `component_class`, `bounds{origin,extent}` via a new
`ResolveNodeIdentity()` (first visible `UMeshComponent`; static → `GetStaticMesh()->GetName()`, skinned →
`GetSkinnedAsset()->GetName()`; bounds from `GetComponentsBoundingBox(true)`).
⚠ **ANCHOR-FRAME SEMANTICS:** sampled **once**, at the event's anchor frame, like `global_position` and the
camera block. **Not per-frame truth** — for a moving actor, `bounds` is its anchor-frame pose. Documented in
`docs/architecture.md` so a future reader cannot misread it. Additive only; no existing field changes value.

### 4 — B2 selection provenance (`28bc6f1`)
New `AnomalyViewport::EvaluateSelectionProvenance` → `FSelectionProvenance{CoveragePct,
OcclusionSamplesPassed/Total, PollDistance, bValid}`, called once per event at the anchor frame.
**Split placement as directed:** `coverage_pct` → **annotation.json**, client-visible, **both modes**;
occlusion samples + poll distance → **internal, non-delivery only**.

**OBSERVATIONAL ONLY — enforced structurally, not by discipline.** The selection path has **zero edits**:
both `AnomalyViewport.cpp` diff hunks are pure insertions (`-317,0 +318,45` and `-494,0 +540,41`, 0 lines
removed), and `AnomalyAutoInjectorSubsystem` / the selector are not in the changed-file set at all.
`ClassifyRenderableVisibleLive`, `IsUnoccluded`, `FirstRenderableVisibleComponent` and
`GetVisibleRenderableActors` are byte-unchanged. The early-out boolean still decides selection; the 9-sample
count lives in a separate function no selection code calls.

## Two deviations from the approved plan — both ACCEPTED by the owner

1. **Standalone evaluator instead of an opt-in out-param threaded through `ClassifyRenderableVisibleLive`.**
   Rationale: (a) "observational only" then holds **by construction** rather than by careful null-defaulting;
   (b) cost is 9 traces per **fired event** (~10/session) instead of per-candidate-per-poll whenever the opt-in
   is on; (c) no new parameter on a hot function for a future caller to misuse.
   **Consequence:** there is **no runtime ON/OFF toggle**, so the "provenance ON vs OFF ⇒ identical selection"
   gate is discharged **structurally** (zero selection-path edits) plus an empirical same-seed determinism run,
   rather than by an A/B.
2. **Finish-time `selection_provenance.json` sidecar instead of `run.json`.** `run.json` is written by
   `WriteRunManifest` at **StartRun, before any event exists**, so per-event data cannot go there without
   changing its write timing. The sidecar is suppressed in delivery mode, which is the stated intent.

### ⚠ GATE SUBSTITUTION — RECORDED SO NO COLD READER THINKS A GATE WAS SKIPPED

The originally-specified gate **"provenance ON vs OFF ⇒ identical selection sequence" is FORMALLY WITHDRAWN**,
not skipped. It became unrunnable *by design*: deviation D1 removed the runtime toggle, because a threaded
null-default opt-in made "observational only" true **by discipline** (a future caller could mis-default it)
whereas the standalone evaluator makes it true **by construction**.

It is replaced by two stronger checks, both green:
1. **Structural diff proof of zero selection-path edits** — `AnomalyViewport.cpp`'s two hunks are pure
   insertions (`-317,0 +318,45`, `-494,0 +540,41`, **0 lines removed**); `ClassifyRenderableVisibleLive`,
   `IsUnoccluded`, `FirstRenderableVisibleComponent`, `GetVisibleRenderableActors` byte-unchanged;
   `AnomalyAutoInjectorSubsystem` and the selector absent from the changed-file set entirely.
2. **Same-seed selection-identity determinism** — seed 4242, 8 events, two runs byte-identical.

Owner ruling: the structural proof is **stronger evidence than the A/B**, which could have passed by
coincidence. **The runtime toggle must NOT be added back** — reintroducing it would restore exactly the misuse
risk the redesign removed.

## Gates — all green in a LOCAL PACKAGE (game-target build + exe hot-swap, G76)

| Gate | Result |
|---|---|
| blink subtype pinned | `runs=2` events (`[4,5,9,10]`, `[40,41,45,46]`) → `disappear_reappear` (previously `flicker`) |
| `frame_count` true count | **10/10 events** `frame_count == len(frame_indices)` (blink 4==4, was 7; missing_texture 8==8) |
| non-blink unchanged | `missing_texture` subtype still `missing_texture` |
| m20 trailing reappear | last hidden frames 10 and 46; `frame_00011.png` and `frame_00047.png` present |
| B1 fields | `StaticMeshActor_0` → `asset_name` `SM_SlopeWarpLandscape`, `component_class` `StaticMeshComponent`, bounds populated |
| **seeded selection identity** | seed 4242, 8 events, **two runs byte-identical** (ids, targets, start_frames) |
| structural | `git diff` — no existing selection function modified |
| provenance informative | `coverage_pct 100`, occlusion **4/9 passed**, poll_distance recorded |
| delivery suppression | delivery run wrote only `annotation.json` + `run_summary.json`; sidecar **absent**; `coverage_pct` **still present** |

*Note:* in the MainMenu test map `poll_distance` is negative because the pawn sits inside the huge landscape
mesh's bounds sphere — the metric behaving as defined (distance to bounds sphere), not a defect.

## Client-facing changes (flag in client notes)

Two values change in files the client already receives — both to the expected value:
- `affected_frames.frame_count` — was a span, now a true count.
- blink `anomaly_subtype` — was `"flicker"` on multi-toggle events, now always `"disappear_reappear"`.

## Status — SVE migration

**APPROVED but NOT IMPLEMENTED.** S1 (evaluation) complete → journal 028. **S2** (render-thread frame↔state
keying model), **S3** (depth), **S4** (production integration) are **NOT started**. Production still captures via
the **backbuffer**, and **the client's 1.2-band −1 lag remains unfixed until S4.** S2 opens in a **fresh session
after m22 tags**. Standing framing: the SVE removes the arm→present race architecturally, but the
ratio-independence requirement is only **discharged** when the new keying model is designed and gated **across
ratio regimes on the paced path** — S2 is where that is met or missed. The CaptureBench free-run limitation
(journal 028) stands: it cannot reproduce or refute the 1.2-band lag, and its 10 green rows must never be cited
as evidence about it.

## Observation for later — NOT acted on this milestone

The provenance gate reported **`coverage_pct` 100 alongside occlusion 4/9 samples passed**. On a menu-map
landscape mesh that is unremarkable in itself — but it is **the first look at the numbers behind the client's
"annotation present, nothing visibly changed" complaint, and it is the SHAPE that complaint would take**: a
full coverage score with a majority of occlusion samples blocked. **Hypothesis to test once provenance data
from REAL GAMEPLAY LEVELS exists** (this ran in MainMenu — the m19/G80 menu-map artifact). Deliberately not
acted on now; recorded so the pattern is recognised when real data arrives.

## Rule change recorded this milestone — CODE OWNS PUSHES

The old **"owner owns remote pushes"** rule is **RETIRED** (it added a round trip and nothing else). From now
on: when work is committed and gated, **push it — including tags — without waiting.** **Kept:** report
`git status` + `git log origin/master..master` before pushing as a **LOG, not an approval request** (two tracks
share this repo and entangled once — G43 — so the record matters, the gate does not); **never force-push on a
rejection**, stop and flag; auth stays with the owner. ⚠ **This does not dissolve owner-owned QUALITY gates** —
an owner Play-gate/eyeball smoke still precedes a tag. For m22 the hold was the **smoke gate**, not push
ownership. Mirrored into `CLAUDE.md` beside the commit convention.

## Close-out

Owner **Play-gate smoke PASSED**. Tagged **`m22`** and pushed (4 feature/docs commits + the tag).

## Hand-off

1. **S2 (render-thread frame↔state keying model) opens in a FRESH session** — a genuine milestone boundary,
   and the design work is the riskiest part of the migration.
2. Production still captures via the **backbuffer**; the client's **1.2-band −1 lag remains unfixed until S4**.
3. Client notes owe two changed values: `frame_count` (span → true count) and blink `anomaly_subtype`
   (`"flicker"` → `"disappear_reappear"`). Owner is handling client communication.
