# Anomaly Injector — Forward Direction & Viewport-Layer Handoff

**Purpose:** This captures decisions made in planning discussion that are NOT yet reflected in the plugin's code or session journals. It is the bridge between "M3 complete + GDP rename done" and the next milestone (the viewport-visibility layer). Read this alongside `CLAUDE.md`, `architecture.md`, `gotchas.md`, and the latest session journal.

**State at time of writing:** 7 anomalies shipped and validated on UE 5.1; `IAI.*` console surface; plugin fully GDP-free (commit `351c7e8`, on tag `m3`). Bridge severed for 5.1 and working. No viewport-awareness or UI yet — control is console-only.

---

## 1. The strategic reframe
The project is moving from **"a catalog of independent anomalies triggered one at a time via console"** to **"a viewport-aware injection system with a debug UI, where anomalies only ever affect what is visible on screen, building toward automatic injection for ML dataset capture."**

The load-bearing new constraint: **every anomaly must only affect objects currently visible in the player's viewport.** This is not a per-anomaly feature — it is a shared capability nearly every anomaly depends on, and the foundation the automatic-injection vision sits on. The end goal is to capture these frames and feed them to an ML model, so an anomaly applied to an off-screen or occluded object is an unlabeled-but-invisible training sample (worse than useless).

---

## 2. Locked decisions

### 2.1 Visibility = frustum + occlusion (not frustum-only)
**Decided:** visibility means the object is inside the camera frustum AND not occluded. Rationale: the frames are ML training data; corrupting an object the player can't actually see produces a frame labeled "anomaly present" with no visible anomaly. Frustum-only is cheaper but insufficient for this goal. Occlusion adds a trace/query per candidate — accepted cost.

### 2.2 Sequencing (agreed, in order)
1. **Viewport-visibility layer first** — a shared helper (proposed `AnomalyViewport`) before/alongside the High-priority bugs, NOT after. Reason: building High bugs first without it just forces a retrofit later (same retrofit the existing 7 need). Stand up the layer, then retrofit the existing 7 AND build new bugs viewport-aware from birth, in one pass.
2. **Finish High-priority visual bugs** (built on the viewport layer).
3. **Debug UI** to toggle built anomalies on/off.
4. **Animation-related bugs.**
5. **Automatic injection** (depends on the viewport layer + the UI).

### 2.3 Blinking vs Flickering — naming correction (client taxonomy)
The client's taxonomy distinguishes these; our current names do not match their meaning. Correction required:
- **Blinking** = objects toggling hidden/visible (NOT lights). → This is what the current `flicker` anomaly actually does. **Rename `flicker` → `blinking`.**
- **Flickering** = a large portion of the scene, or certain lights, toggling on/off. → This is a NEW, broader anomaly (scene-region or light-scoped). **Build new `flickering`.**
This is a real correction, not cosmetic — do it before more depends on the `flicker` name.

### 2.4 Lighting anomaly — broaden beyond per-light property mutation
Current `lighting_mismatch` mutates matched lights' color/intensity/shadow. The client wants a more realistic, **region/area-scoped** anomaly: find all lights in the scene, kill some so a particular area is **darker than it should be**. This is spatial (which lights affect this on-screen region), so it couples to the viewport/spatial layer. Treat region-darkening as a NEW anomaly on the viewport/spatial layer; leave the existing per-light one as-is.

### 2.5 Debug UI
A UI to toggle the built anomalies on/off. This is the first time the plugin gains UI — it adds Slate/UMG + InputCore/Slate/SlateCore/UMG module deps, the first dependency-surface growth since M0 (until now: Core/CoreUObject/Engine only). Expected and fine; just no longer "engine-core-only," and it's a 5.1 surface to validate.

### 2.6 Automatic injection — locked spec
At gameplay start, a UI selects which anomaly types are enabled. Then, during play, those anomalies fire **randomly** on whatever objects are **on screen at that moment**. This is the real-time "live autonomous" injection flavor. NOTE: this is DISTINCT from the dataset-generation pipeline (replay/labeling/capture); both are valid, this is the one being prioritized for the gameplay-injection path.

---

## 3. Realistic lighting anomalies worth adding (brainstorm, not all locked)
Lighting bugs that actually occur in shipped games (cheap = actor/component/cvar; render-tier = harder):
- **Missing shadows** — force `CastShadow=false` on lights/primitives so objects float (very common real bug; cheap). *High-signal pick.*
- **Region darkening** — kill the lights affecting an on-screen area (§2.4; the user's explicit ask). *High-signal pick.*
- **Unbuilt / flat GI** — force preview lighting or kill a GI/Lumen contribution so indirect light looks wrong (very recognizable). *High-signal pick.*
- **Shadow acne / peter-panning** — perturb shadow-bias cvars (cheap-ish).
- **Light leaking / wrong intensity** — over-bright or near-black regions (overlaps region darkening).
- **Reflection/specular blowout** — break reflection capture or crank specular (more involved).
- **Wrong exposure / missing emissive** — auto-exposure stuck or emissive gone dark.

---

## 4. Scope to enrich anomalies already built (realistic variants)
Several built anomalies are narrower than the real-world bug they represent:
- **`lod_corruption`** forces a *worse* LOD; real LOD bugs also include *stuck-at-LOD0* (too detailed at distance) and **LOD mismatch across a seam** (the sheet's "LOD boundary"). Cheap extensions.
- **`camera_clipping`** does near-plane; real camera bugs also include the camera clipping *into geometry* (overlaps "object clipping").
- **`missing_object`** is permanent-hide; the realistic variant is *flickering in/out* (z-fight/streaming) — folds into blinking/flickering.
- **`time_dilation`** is uniform/global; real "high/low-speed" bugs are often *per-actor* (one animation too fast) — a per-component anim-rate variant.

---

## 5. The viewport layer — design notes for the scoping turn
This is the next milestone and should be run as a **scoping/architecture turn with Code first (xhigh)**, plan before implementation. Open design questions to resolve in that turn:
- **Occlusion query method** on 5.1: what's the right primitive (line traces to bounds, `GPU`/hardware occlusion query results, `WasRecentlyRendered`/last-render-time as a cheap proxy, render-bounds visibility, etc.) — verify against 5.1 source; each has accuracy/cost/timing tradeoffs.
- **Visible-set semantics per anomaly:** e.g. does `missing_object` hide something already on screen (player sees it pop out — jarring but maybe desired for detection), or only target objects *entering* the frustum? Decide per anomaly.
- **Retrofit pattern:** the existing targeting helpers (`AnomalyTargeting::FindActorsMatching` / `FindComponentsMatching<T>`) are centralized — the viewport filter should compose with them cleanly so retrofitting the existing 7 is one pass, not 7 edits scattered through anomaly code.
- **Which camera/view** defines "the viewport" (player camera in PIE/standalone; in Simulate there may be no player camera — handle gracefully).
- **Cost/perf:** occlusion per candidate per injection — bound it (only test matched candidates, cache per-frame, etc.).

---

## 6. Conventions to carry forward (unchanged)
- Game-agnostic invariant: deps stay minimal; plugin never includes host types; match by Name/Class, never `GetActorLabel`. (The UI milestone is the first sanctioned dep expansion — Slate/UMG — and only for the UI module surface.)
- Shared-helper pattern: free functions in `Public/` with the API macro, heavy includes in `.cpp`, type dispatch internal (`AnomalyTargeting`/`AnomalyArgs`/`AnomalyLod`; `AnomalyViewport` joins this family, and a `AnomalyAudio` likely later).
- Locked `IAnomaly` interface (Apply/Tick/Revert/IsActive/GetId/GetUsage) — no change expected; flag immediately if the viewport work seems to need one.
- Per-target state capture keyed to `TWeakObjectPtr`; Revert undoes exactly what Apply did; re-entrancy = revert-then-reapply in Apply; ticking anomalies use while-drain + phase-safe revert.
- Two-Claude workflow, plan-before-code, doc discipline, Conventional Commits + milestone tags, effort min-max (xhigh planning/debug, high mechanical, ultracode for large mechanical sweeps).
- Verification: state gates over the MCP bridge (5.1, `editor_play_simulate()` autonomous path); visual gates by the owner.

---

## 7. Open caveat carried from M3
Stock StackOBot has **no deterministic multi-LOD visual target** (props single-LOD, rocks instanced foliage, the Bot `SKM_Bot` is single-LOD) — LOD anomalies are state-validated per gotcha G15. A real visual LOD/anomaly demo (and richer dataset capture) will eventually want **purpose-built multi-LOD / controllable test content** — a future "test scene" task, not yet scheduled.
