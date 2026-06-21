# Chat Handoff — m7 (Frame-Capture + Labeling) Closed

**Date:** 2026-06-20
**Written for:** a fresh chat (chat-Claude / Claude Code / collaborator) cold-reading into this project.
**Status at write time:** m7 shipped, committed, tagged. Session wrapped. One executive action (push) pending with the owner.

> This is the chat-side continuity artifact. It does **not** duplicate the on-disk repo docs (journals, gotchas, architecture) — a fresh chat cannot read those until Code opens them. This doc orients; the repo docs carry the operational detail. **Pointers to the right repo reads are in the last section — use them.**

---

## 1. Current state — what's built and validated

**The plugin:** `AnomalyInjector` — a game-agnostic UE 5.1 plugin that injects labeled visual anomalies into a running game to generate synthetic ML training data. Public UE APIs only; tested on StackOBot; ships-as-a-build (no editor in the shipped product). **VersionName now 0.8.0.**

**Milestone history (all 5.1-validated, tagged):** m1 (IAnomaly interface + registry, LOCKED since) → m2 (breadth) → m3 (LOD breadth) → m4 (AnomalyViewport: frustum + occlusion visible-set) → m5 (in-game selector UI) → m6 (automatic injection) → **m7 (frame-capture + labeling) — NEW THIS SESSION.**

**m7 delivers:** a labeled image-sequence capture pipeline. During an auto-injection run it captures frames, **temporal-labels** which frames contain an anomaly, and **spatial-labels** each with a 2D bounding box on the affected object — written to disk as PNGs + a JSONL sidecar + a run manifest. Built by composing existing primitives (the m4 viewport projection, the m6 auto-injector's live-fire ground-truth, the control-server's capture primitive); the injector core and `IAnomaly` stayed LOCKED.

**Validated through three gates (all green, owner-verified in real Play):**
- Gate 1 — one correctly-labeled frame end-to-end (incl. the hidden-actor "where the hole is" case).
- Gate 2 — bounded multi-burst run: correct frame counts, both-boundary temporal transitions, boxes land, decision-level reproducibility.
- Gate 3 — boxes track the object **under camera motion** (the moving-capture case, which is the actual use case).

**Commits (Plan A two-commit close; NO push yet — owner's to make):**
- `ff1be3c` — `feat(control-server): Slice-1 dashboard capture primitive + WS snapshot/protocol` (the parallel dashboard track's Slice-1 WIP, promoted first; **untagged**). 10 files; `ControlProtocol`/`ControlSnapshot` created.
- `f5125ae` — `feat(capture): labeled frame-capture + 2D bbox labeling milestone (m7)`, **tagged `m7`**. 16 files (+1611). VersionName 0.7.0 → 0.8.0.
- Tag chain: `m1…m6 → m7`. Both commits build exit 0.

**Not yet done / pending:**
- **`git push && git push --tags`** — deliberately left to the owner. No git remote was configured historically; owner owns any remote push.
- **One stray uncommitted file:** `docs/2026-06-20-011-handoff-vfx-removal-poll-radius.md` (pre-existing untracked, deliberately left untouched this session). `git status` clean except this. Worth a standalone `docs:` commit eventually; not urgent, not part of m7.

---

## 2. Decisions made this session (rationale, not just outcome — the most important section)

**D1 — Labels come from the injector's own ground-truth, NOT a replay diff.** We inject deliberately, so we already know which anomaly fired, on which actor, during which frames (the m6 auto-injector tracks exactly this). So m7 v1 = a **live single labeled-capture stream** during an auto-injection run. The record-replay-twice clean-vs-injected harness (the separate decoupled plugin idea) stays SEPARATE and DEFERRED — it's for frame-aligned clean baselines, not needed to find anomalies we placed ourselves.

**D2 — Frame grab = reuse the existing game-viewport `FViewport::ReadPixels` path (synchronous, on the game thread).** Source-verified on 5.1: it captures the *actual displayed frame* (post-process/AA/UI), is `ENGINE_API`, works in a packaged build, no editor. Its synchronous nature was chosen as a **feature for v1**: it makes image↔label alignment exact (same game-thread tick, same `GFrameCounter`), eliminating cross-thread hazards. SceneCapture2D rejected (second render = wrong frame). MovieRenderQueue rejected (cinematic/editor-leaning).

**D3 — 2D bbox = project the affected actor's 3D bounds to screen, reusing m4's existing VP machinery.** A surprise during scoping: the projection (`ProjectBoundsToScreenRect` + reversed-Z VP) **already existed privately** in `AnomalyViewport.cpp` — so the work was *exposing* it (`ProjectActorBoundsToScreenRect`), not building it. Pixel-accurate masks (custom-depth/stencil) remain a documented FUTURE enhancement.

**D4 — Housing = inside the existing `AnomalyControlServer` module, not a new module.** The dep-quarantine we wanted already existed there (ImageWrapper isolated, the capture primitive + snapshot machinery already present). A labeled-capture stream is "the dashboard's snapshot+frame written to disk instead of streamed." The injector core stayed clean (only the sanctioned exposures). The standalone `AnomalyCapture` module remains the documented future-extraction path **if Shipping-config capture is ever required** — ruled NOT required, because dataset-gen runs on dev/research builds we control (packaged Development/Test, where the control-server compiles in), so L5 "ships-as-a-build / no editor" is satisfied. (YAGNI on the separate module.)

**D5 — Three sanctioned core-shell exposures only; `IAnomaly` + injector core stayed byte-clean.** (a) `ProjectActorBoundsToScreenRect` in AnomalyViewport (type-union of SM/SK bounds, NOT `IsVisible`-gated, so hidden actors still project — "where the hole is"); (b) `FAutoLiveFireInfo` widened with `TWeakObjectPtr<AActor> TargetActor` + `uint64 StartFrame` (the readback previously carried only names; bbox needs the actor for its bounds); (c) `RevertAllLiveFires()` exposed on the auto-injector (so capture reverts through the auto-injector, keeping `GetLiveFires()` accurate). All three are on the auto-injector **shell**, not the locked core. `IAnomaly` / injector core / anomalies / leaf helpers / the `=` exact-match / `GetVisibleRenderableActors` stayed LOCKED.

**D6 — Cadence = capture-driven deterministic bursts.** The capture subsystem drives the m6 core directly via `FireOnce`/`RevertAllLiveFires` (NOT `AdvanceTime`, which could fire a *new* interval-anomaly and collide with one-per-burst). Burst shape: `[pre-roll M clean] → FireOnce → [settle K] → [positives P] → revert → [settle K] → [post-roll M clean]`, looped (post-roll doubles as next pre-roll). Reproducible, clean positive/negative framing, bounded stall. "Capture every frame while Run is active" rejected for v1.

**D7 — The settle-K must be SYMMETRIC (both boundaries).** The render trails the game thread ≥1 frame (`r.OneFrameThreadLag`), so BOTH transitions have a lag window where game-thread ground-truth and on-screen pixels disagree. Skip K frames after BOTH `FireOnce` AND revert. The revert→negative side is the silent-corruption risk (a post-roll frame labeled clean while the anomaly is still on-screen). Default K=2.

**D8 — Image format = PNG-lossless default** (JPEG retained behind a flag). A bug-detection dataset must not bake in JPEG blocking artifacts the model could mislearn as corruption/tearing. Native resolution, no downscale in capture.

**D9 — `ViewLagFrames = 0` is the validated default for motion bbox — and it's NOT "zero lag."** This was the session's sharpest finding. Stage 3 *predicted* L=1 (project with the camera from 1 frame ago, to match the delayed captured pixels). Empirically L=0 was correct and L=1/L=2 pushed the box *off* the object. **Resolved by source-verification:** the view ring's origin convention means `ring[0]` at capture time is already *last* tick's view — so "L=0" already reaches exactly one physical render-frame back. The "0" is the ring-origin convention, not an absence of lag. This is **FPS-invariant** (it's a fixed frame-count relationship, not time-dependent), so no low-FPS re-test was needed. It coheres with the settle-K=2 (both account for the same ~1-frame render lag). **Critical:** the async capture path (see D11) will have its OWN lag characteristic — do NOT assume L=0 carries over; re-derive it there.

**D10 — `visible_positive` contract (the off-screen-during-hold finding).** Under camera motion, a fired actor can leave the viewport *during* its hold: the anomaly stays applied in game-state (`anomaly_present=true`) but has no projectable on-screen box (`bbox_valid=false`). These frames are **not visible positives** (no visible bug in the image) — they're legitimate hard-negatives for detection. Decision: keep `anomaly_present` as game-state truth (it drives the validated temporal transitions); **define `visible_positive = anomaly_present AND any bbox_valid`** and write it as a per-frame field so consumers filter on it. The verify script tallies the off-screen rate. (The in-frustum-but-*occluded* sub-case is NOT caught by bounds projection — that's the deferred `GetLastRenderTimeOnScreen` refinement.)

**D11 — The async backbuffer path is the next prerequisite, doubly motivated.** `OnBackBufferReadyToPresent` + `FRHIGPUTextureReadback` (staged GPU copy) is the superseding exact capture path. It is **required before** (a) framerate-bug anomalies enter the pool — the synchronous `ReadPixels` flush would corrupt the very framerate label being captured — and (b) pixel-exact view-matching under motion. Deferred for m7 (v1's 4 object-scoped anomalies don't trigger the framerate confound), but it now sits at the front of the queue.

**D12 — The Slice-1 entanglement, resolved via Plan A.** This session cold-booted and read the control-server's capture primitive as "existing" when it was actually **uncommitted working-tree WIP from the parallel dashboard chat**. m7 got built on top of it, entangling the milestone with another track's unsaved work in the same module. Ruling: **Plan A** — commit the dashboard's Slice-1 as its own correctly-attributed commit FIRST (`ff1be3c`, untagged, owner-approved file set), then m7 path-scoped on top (`f5125ae`, tagged). Rejected: bundling Slice-1 into the m7 tag (mis-attribution), and ripping m7 out / reverting the WS files (would endanger the parallel track's WIP, fork the shared primitive, and rewrite green code). Before committing, a read-only history check (`git log`/`reflog`/`status`) confirmed nothing was orphaned/lost. **Consequence (intended):** the shared capture-primitive generalization (raw-BGRA + PNG/JPEG + opaque-alpha) physically lives in `ff1be3c`, the dashboard commit — the m7 journal documents this. The split was validated by commit 1 having zero forward-references to m7 symbols.

---

## 3. Forward plan / sequencing

**Immediate (owner action):** `git push && git push --tags`. Optionally a standalone `docs:` commit for the stray 011 handoff file.

**Next milestone — the async backbuffer capture path (D11).** Front of the queue. `OnBackBufferReadyToPresent` + `FRHIGPUTextureReadback`. Re-derive the view-lag for that path from scratch (D9's L=0 does NOT carry over). This unblocks framerate-bug anomalies and gives pixel-exact motion bboxes.

**Then — the High-priority new visual bug types**, built viewport-aware AND auto-injectable AND capture-labelable from birth (the m4/m5/m6/m7 stack now means new bugs plug in with no retrofit):
- Easy tier: corrupted textures, object clipping.
- Render tier: screen tearing, framerate bugs (these specifically depend on the async capture path landing first — see D11).
- Then animation bugs.
- Then the Tier-2 runtime in-build control server (the production form of the dashboard).

**Deferred / separate tracks (unchanged):**
- The record-replay-twice clean-vs-injected harness (separate decoupled plugin) — for frame-aligned clean baselines; not needed for deliberate-injection labeling.
- Pixel-accurate masks (custom-depth/stencil) — future labeling enhancement beyond bbox.
- `GetLastRenderTimeOnScreen` occlusion refinement — for the in-frustum-but-occluded label-accuracy edge (D10).
- The parallel dashboard track (separate chat) continues its own Slice sequence.

---

## 4. Locked decisions vs open questions

**LOCKED (do not relitigate):**
- `IAnomaly` interface — LOCKED since m1; held through m7.
- The injector core / anomalies / leaf helpers / `=` exact-match / `GetVisibleRenderableActors` — byte-clean; only the three named auto-injector-shell exposures (D5) were added.
- Visibility = frustum + occlusion (not frustum-only); poll-radius distance cull; VFX/particles REMOVED from the renderable-visible set (StaticMesh/SkinnedMesh only) — particles reachable only via the `=name` console escape hatch.
- One-anomaly-per-actor scheduler guard + registry one-live-instance-per-id = collision-free by construction (no ref-count coordinator in v1).
- m7 design decisions D1–D12 above.
- Capture housing = `AnomalyControlServer` module (D4); PNG default (D8); symmetric settle-K (D7); `ViewLagFrames=0` default (D9); `visible_positive` contract (D10).

**OPEN / to-decide next:**
- The async-path view-lag value (re-derive — D9/D11).
- Exact API shape of the async backbuffer capture (the `OnBackBufferReadyToPresent` hook + GPU readback staging on 5.1) — a Code-first source-verification scoping turn when that milestone starts.
- Target-curation for loose labels (see backlog below).

---

## 5. Corrections / things that changed this session

- **`ViewLag=1` prediction was WRONG; `ViewLag=0` is correct** — and "0" means *one render-frame back* via the ring-origin convention, not zero lag (D9). Any doc or mental model saying "project with the current view" or "L=1 to match the delayed frame" is superseded.
- **The control-server module was further along than memory implied** — Slice-1 (capture primitive + WS snapshot/protocol) existed as working-tree WIP and is now committed at `ff1be3c` (D12). Memory previously implied only Slice-0 + early WIP.
- **The capture/labeling milestone was pulled AHEAD of the new visual-bug types** (owner's resequence, already reflected) — and is now DONE as m7.
- **The async capture path moved from "eventual" to "next prerequisite"** (D11) — it now gates the render-tier bug types.

---

## 6. Pointers — what a fresh chat should have Code read for operational detail

This handoff orients; the repo docs (on disk, invisible to a cold chat until Code opens them) carry the detail. Have Code read, in order:
- `CLAUDE.md` — current status / in-flight / milestone list (now reflects m7, VersionName 0.8.0).
- `docs/sessions/2026-06-20-012-frame-capture-labeling.md` — the full m7 session journal (Stages 1/2/3 arc, the L=0 finding + explanation, the Slice-1 entanglement + Plan A resolution).
- `docs/gotchas.md` — **G35–G43 are new this session:** unity-build anon-namespace; `GFrameCounter` same-tick alignment; symmetric settle-K both boundaries; hidden-actor bbox-from-persisted-bounds; PNG/opaque-alpha; `ReadPixels` synchronous-flush observer-effect + the async deferred trigger; the `ViewLag=0` ring-origin/tick-order explanation + FPS-invariance; `visible_positive`/off-screen + deferred occlusion; and the cold-boot committed-vs-WIP process gotcha (G43).
- `docs/post-m7-capture-labeling-handoff.md` — Code's own m7 close handoff (the repo-side companion to this doc).
- `docs/architecture.md` — the capture/labeling section + the three sanctioned exposures + the shared-primitive dependency on the control-server module.
- `docs/setup-runbook.md` §6c — the `IAI.Capture.*` console surface + the moving and still capture gates + the L=0 default.

**Carry-forward reminders (also for the owner):**
1. **Parallel dashboard track:** `ff1be3c` promoted that track's uncommitted Slice-1 verbatim. When that chat resumes, it must reconcile its own dashboard journal/handoff against SHA `ff1be3c` (the Slice-1 code itself is unchanged).
2. **Backlog (non-blocking):** `flicker` on `InstancedFoliageActor` projects a box over the foliage actor's full (huge) bounds — a loose, low-value label. A future target-curation pass should address label quality on instanced/foliage actors.

---

*Bootstrap protocol for a fresh chat: read this doc, then have Code cold-read the pointers in §6 and summarize current state back before proceeding. A new chat cannot see the local repo — this doc + project memory are the continuity.*
