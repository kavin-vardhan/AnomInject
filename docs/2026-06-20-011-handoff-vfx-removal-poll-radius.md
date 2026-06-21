# Handoff — Session 011: VFX removal + changeable poll-radius

**Date:** 2026-06-20
**Scope:** two surgical fixes to the shared `AnomalyViewport` renderable-visible layer
**Status:** both shipped, compiled clean on 5.1, owner-validated in PIE, committed
**Not a numbered milestone — no tag, no version bump.**

> Save this to `docs/` alongside the session journal. It briefs a cold reader (fresh chat / Claude Code / collaborator) without access to the local repo. Operational line-level detail lives in the journal + gotchas; this doc carries the *why* and the *state*.

---

## TL;DR

- **FIX 1** — VFX/particles removed from the renderable-visible set. Reverses the old VFX-inclusion ruling (R1 / G29).
- **FIX 2** — new adjustable poll-radius distance cull around the player pawn. OFF by default.
- Both live entirely in `AnomalyViewport`; the injector / anomalies / selector / auto cores are untouched and simply inherit the refined set.
- Two atomic commits: `9bbd398` (fix) then `ae57b69` (feat). HEAD = `ae57b69`.

---

## 1. Current state — what's built & validated

- Plugin `AnomalyInjector`, UE **5.1**, own git repo at `Plugins/AnomalyInjector/` (`master`). VersionName still **0.7.0** (no bump — not a milestone).
- **HEAD = `ae57b69`** `feat(viewport): add changeable poll-radius distance cull`, preceded by **`9bbd398`** `fix(viewport): remove VFX from renderable-visible set (reverses G29)`.
- **Build proof:** clean Development-Editor compile on 5.1 (exit 0) before *each* commit. The `AnomalyControlServer` module recompiled against the changed header and linked clean both times — the A4 consumer still builds.
- **Owner-validated in PIE (Kavin confirmed working):** VFX no longer selectable anywhere; radius cull works and matches the debug sphere; `IAI.SetPollRadius 0` == prior behavior.
- **Staging was path-scoped** to `AnomalyViewport.{h,cpp}` + touched `docs/**` per commit. The unrelated **control-server WIP in the tree is untouched** (`ControlProtocol`, `ControlSnapshot`, `AnomalyPreviewCapture`, `WebClient/`, modified `AnomalyControlServer.Build.cs` / `AnomalyControlServerSubsystem.{h,cpp}`).

### What FIX 1 changed
- Dropped the `UFXSystemComponent` clause from `IsRenderableComponent` → renderable set is now **StaticMesh ∥ SkinnedMesh** only. Removed the now-dead `"FX"` branch in `ClassifyRenderableComponent` and the unused `ParticleSystemComponent.h` include.
- Propagates to **all** consumers of the set: M5 selector, M6 auto-injector, the control-server A4 read-back (`GetVisibleRenderableActorInfos` / `FRenderableActorInfo`), and `IAI.DumpVisible`.
- The `=name` console escape hatch (`IAI.Apply … =<VfxName>`) **still reaches VFX** — it routes through `AnomalyTargeting::Find(Visible)ActorsMatching`, which bypasses `IsRenderableComponent`. Verified by code path.

### What FIX 2 added
- Shared `GPollRadius` (file-static, cm) + accessors + `IAI.SetPollRadius <value>` console command, **all co-located in `AnomalyViewport.cpp`** (world-independent global state ⇒ a plain `FAutoConsoleCommand`, no world needed — this is what keeps "touch only AnomalyViewport" true).
- Cull lives in the shared chokepoint `IsComponentRenderableVisibleInternal`. Evaluation order: **renderable type-test → distance cull → frustum → occlusion** (cheapest-first; order is pure short-circuit perf — the set is an AND, so zero behavioral effect).
- **Origin = player pawn location** (`ResolvePollOrigin` → first PC's pawn, else camera fallback). **Metric = sphere-approx bounds:** `Dist(origin, B.Origin) − B.SphereRadius > R` culls. Reads the already-cached `Component->Bounds` (no double-compute).
- **Sentinel:** `R ≤ 0` ⇒ no cull ⇒ byte-identical to pre-session behavior (the default). Positive R enables.
- Applied **identically at both live entry points** (`GetVisibleRenderableActors` + `GetVisibleRenderableActorInfos`) so `IAI.DumpVisible`'s set-identity assertion still holds. Explicit-view / synthetic functions pass `0.f` → byte-identical synthetic surface preserved.
- Console command has a **defensive no-arg guard** (logs current radius + ON/OFF instead of crashing). **Dev debug sphere re-resolves the pawn per frame** and registers/unregisters on the OFF↔ON boundary (G25 hygiene). Accepted residual: a module unload while radius>0 leaks the draw handle (dev-only viz; a teardown hook would mean touching the module `.cpp`, which is out of scope).

---

## 2. Decisions made this session (rationale — the *why*)

1. **VFX hard-removed, not toggled.** Re-add is one line; a dead default-OFF toggle would just clutter `AnomalyViewport` right as the labeling milestone starts consuming it. Reversibility-without-recompile wasn't wanted.
2. **VFX removal is deliberately forward-looking, not just cleanup.** It removes particle systems' fuzzy/unstable bounds from the set *ahead of the bbox-labeling milestone*, where those bounds would produce unreliable bounding boxes.
3. **Single source of truth held — dashboard inherits both fixes, no fork.** The control-server A4 set correctly stops listing particles and correctly narrows with the radius. A dashboard listing actors you can't actually target would be misleading. Forking the set to keep VFX on the dashboard was rejected (violates the locked single-source design).
4. **Radius origin = pawn, not camera.** Matches "around the character." In third-person the camera/pawn gap is small next to any useful radius. **Documented consequence (NOT reconciled):** the A4 `FRenderableActorInfo::Distance` stays **camera-relative** (unchanged contract), while the *cull* origin is the **pawn** — so a dashboard distance number can read slightly above R for an actor that's still in-set. Expected; what stays exactly consistent is *which actors are in the set*.
5. **Cull placed in the chokepoint + radius threaded as a param (not read from the global inside).** Threading lets the two *live* whole-scene entry points opt in while the *synthetic-view* functions opt out (pass 0) — preserving their byte-identical synthetic-gate guarantee even when a radius is set.
6. **Console command lives in `AnomalyViewport.cpp`, not beside `IAI.SetViewportScoping`.** The radius is world-independent global state, so a no-world `FAutoConsoleCommand` co-locates state+accessor+command and honors "touch only AnomalyViewport."
7. **Two robustness additions over the original plan:** (a) defensive no-arg guard on the console command; (b) per-frame pawn resolution for the debug sphere (pawn moves — don't freeze it at registration).
8. **History not rewritten.** The 009 journal is left intact (correct record of what was asserted *then*); only the *living* docs (runbook §7, architecture/handoff current-state) flip the m5 gate assertion. Same discipline as the G29 `SUPERSEDED` annotation.
9. **BOM on commit subjects accepted.** The UTF-8 BOM prefix is a PowerShell `Out-File` artifact that matches existing commits (`2d50452`, `41ba104`); left consistent rather than divergent. Not worth a history rewrite.

---

## 3. Corrections / changed facts (drop stale understanding)

- **VFX is NO LONGER in the renderable-visible set.** Any prior note that "particles are selectable / `missing_object` & `blinking` target them via the set" is now **wrong** for the *set-driven* paths (selector, auto, dashboard A4). Particles remain reachable **only** via the `=name` console escape hatch. G29's VFX-inclusion bullet is annotated **`SUPERSEDED by G33`** (the rest of G29 stands).
- **Pure-VFX zero-match gate scenarios moved.** "Select a pure-VFX actor → LOD anomaly → 0 matched" can no longer be produced through the selector/auto (the actor isn't in the set). The `0 matched` plumbing is unchanged; the *trigger* is re-pointed to `IAI.Apply lod_corruption =<VfxName>` (console). Runbook rows updated.
- **Git is well past the m6 snapshot.** Before this session, HEAD was not m6 (`41ba104`) — the control-server track had already landed **`2645236`** (transport spike / Slice 0) and **`4c05344`** (core read-back / Slice 1 stage 1) into *this same repo*, with further uncommitted WIP. This session added `9bbd398` + `ae57b69` on top.
- **There is a THIRD live consumer of the viewport set:** the control-server dashboard, via `GetVisibleRenderableActorInfos` → `FRenderableActorInfo` → `ControlSnapshot.cpp`. `IAI.DumpVisible` enforces byte-identical set+order between the two live entry points — which is *why* FIX 2 had to apply identically to both.
- **No double-computation of bounds.** `Component->Bounds` is a cached O(1) member; the "compute once and reuse" instruction was satisfied trivially. `IsInFrustum`/`IsUnoccluded` were not churned.

---

## 4. Forward plan / sequencing (unchanged by this session, now better-prepared)

**ACTIVE NEXT — the LABELING + FRAME-CAPTURE milestone** (resequenced ahead of the new bug types, Kavin's call):
- **Goal:** capture N frames, temporal-label which frame(s) contain an anomaly, put a 2D bbox on the affected actor.
- **Starting direction (NOT locked):** labels come from the injector's OWN ground-truth (the M6 auto-injector already tracks which anomaly fired on which actor and when) ⇒ **v1 = a LIVE single labeled-capture stream**; bbox = project the affected actor's bounds via AnomalyViewport's M4 reversed-Z VP machinery.
- **Deferred/separate:** the record-replay-twice clean-vs-injected harness + pixel masks (custom-depth/stencil). Not needed to label anomalies we injected deliberately.
- **Headline open question for the scoping turn:** the frame-grab mechanism on 5.1 — **backbuffer readback vs SceneCapture2D vs movie-render-queue** — weighed against game-agnostic + ships-as-a-build + observer-effect on framerate.
- This session's VFX removal directly **de-risks** the bbox work (no fuzzy particle bounds in the set).
- Run as **Code-first scoping, plan-before-code, `xhigh`** for the first turn.

**THEN (after labeling/capture):** new visual bug types, built viewport-aware AND auto-injectable from birth — corrupted textures + object clipping (easy tier) → screen tearing + framerate (render tier) → animation bugs → Tier-2 runtime in-build control server (production form of the dashboard).

---

## 5. Open questions vs locked decisions

**Locked this session:** VFX hard-removed; pawn origin; sphere-bounds metric; `R≤0` sentinel / OFF-default; command in `AnomalyViewport.cpp`; chokepoint placement + param threading; no version tag.

**Locked, carried, still in force:** `IAnomaly` interface LOCKED; ships-as-a-build lens; one-anomaly-per-actor guard (M6 — collision-free by construction); occlusion = multi-sample camera-to-bounds line-trace; every anomaly only affects viewport-visible (frustum AND occlusion) objects.

**Open (next milestone):** frame-grab mechanism (above); whether the cull origin should ever switch to camera for the *capture* framing — the **only** reason to revisit pawn-vs-camera. Flagged, not needed yet.

---

## 6. Pointers — what a fresh chat should have Code read

- `docs/sessions/2026-06-20-011-viewport-vfx-removal-poll-radius.md` — this session's full journal (created this session).
- `docs/gotchas.md` — **G33** (VFX removal + G29 supersede annotation) and **G34** (poll-radius cull specifics).
- `docs/sessions/2026-06-18-008-viewport-visibility-layer.md` + `…-009-selector-inject-ui.md` + `…-010-auto-injection.md` — the viewport / selector / auto lineage these fixes sit on.
- `docs/post-m6-auto-injection-handoff.md` — M6 locked decisions (coordinator not needed, etc.).
- `architecture.md` + `setup-runbook.md` (§7 gate rows + command reference) — current-state docs, both flipped this session.
- `CLAUDE.md` — current-status refreshed this session.
- Control-server track context: its own WIP (`ControlProtocol`, `ControlSnapshot`, `AnomalyPreviewCapture`) is uncommitted and lives in a *separate chat's* scope; relevant here only as the **third consumer** of the viewport set.

---

## 7. Gates (all owner-validated this session)

- VFX **excluded**: a particle actor no longer appears in `IAI.DumpVisible` / the selector list / auto targets; `IAI.Apply missing_object =<VfxName>` still hits it (escape hatch).
- Radius set ⇒ actors beyond R culled in selector **and** auto **and** dashboard; `IAI.DumpVisible` still reports MATCH (set identity preserved).
- `IAI.SetPollRadius 0` ⇒ byte-identical to today (regression); OFF-regression for everything unrelated.
- Debug sphere draws only when R>0 and is centered on the live pawn at radius R.
- Owner real-Play eyeball: both fixes behave; VFX unselectable; sphere matches R.
