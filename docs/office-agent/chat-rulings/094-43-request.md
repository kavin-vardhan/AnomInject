094-43 — EUROPA: FZ2 FOR REAL. STAGE A proves the scene-depth admission (GD3) on the fixture; STAGE B builds the private ID/depth pass into `feat/europa-port` (GD5 scope), with FZ7 and the card draft, and runs H1–H5; STAGE C is Code's scan and push. Under chat's rulings 094-43a (GD1–GD7), 094-42c (GC1–GC7), 094-42r (GA1–GA6) and 094-41 (FZ1–FZ10), with FM1, FJ1 and GA5. Requested by chat Claude (the orchestrator) on 2026-11-04.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. `_reviews\094-43a-ruling.md`, `_reviews\094-42c-ruling.md`, `_reviews\094-42r-ruling.md`, `_reviews\094-41-ruling.md`.
2. **The route is GD2's:** the depth-only shaders found by name, through the public material shader-map chain and the exported mesh-pass machinery, into plugin-owned depth and ID targets. **The admission is GD3's,** against the scene depth of the frame being saved, read through the scene-texture uniform buffer. `_reviews\094-41-codex-plan.md`, option 1, remains the design reference.
3. Kept: 094-31 (FM1; the budget is fresh for every target in this job), 094-31b (FN2), 094-27c (FJ1), 094-27d (FK1), GA2 (`-nowrite` on fixture launches), GA5 (3 h admission bound), FZ3 (the guard stays as the fallback), FZ4 (default routes are the client's), and the rulings these keep by name.

STARTING POINT
- `feat/europa-port` at `75ca580` locally, with origin at `d1cdc7b`.
- `spike/europa-09443a-depthonly-byname` at `c1193bd` (fixture only, never merged or pushed), and the fixture `E:\IA_Fixtures\Outline425\`.

STAGE A — THE ADMISSION PROOF (on the 09443a spike branch, fixture only)
1. **Entry:** FJ1's fast checks, plus one full `owner_p9.py` `main()` run (GC1). Any failure is a STOP.
2. **GD3:**
   - visible coverage keeps an ID pixel only where the private depth equals the scene depth, within a stated tolerance;
   - hidden event targets keep it where the private depth is `≤` the scene depth, within the same tolerance;
   - the tolerance is derived and stated, never tuned to pass.
3. **Gates:**
   - **A-P3:** a re-run of 094-43a's P3 with the same seed and reader against the same legacy reference. Every object has IoU ≥ 0.98, including masked and WPO on every frame, and the hidden-blink frames stay non-empty and matching.
   - **A-P4:** C5 with the outline on, using the private pass only, against the outline-off control: **0 new magenta pixels** on every saved frame. Include an example frame.
   - **A-P2:** still 0 `Missing cached shader map` lines on a warm launch.
   - **A-P5:** still 0 enabled custom-depth components, 0 tagged, and `r.CustomDepth` unchanged.
4. **If any Stage A gate fails, stop with NEEDS-DECISION,** naming the exact cause with numbers. Stage B does not start.

STAGE B — FZ2 ON `feat/europa-port` (only after Stage A passes)
5. **Port the proven pass from the spike** (port it, don't merge the branch). It serves the census and the event masks.
   - **GD5 scope:** static, instanced and hierarchical-instanced static meshes, and skeletal meshes through a plugin-owned dynamic-element collection.
   - Any component type the pass cannot draw faithfully is `unavailable`: ineligible for census and event selection, with per-run counts in the receipt.
   - **Replace the spike's shortcuts** (`_reviews\094-43a-code-review.md` §1) before the Paris gates:
     - the proxy comes straight from the component, with no linear search of scene primitives;
     - persistent private targets reused after fences, not a new texture every frame;
     - batched draws, not one clear and compute per object;
     - asynchronous readback with no blocking flush or wait per captured frame, unless a gate shows one is needed, stated with numbers.
6. **Remove every custom-depth write from C4 and C5:**
   - the forced `r.CustomDepth`;
   - the census re-tagging;
   - the event-target tagging;
   - the bench plant.

   The legacy path survives only as a bench-only reference (`-IAIBench` plus the fixture check). FZ3's guard stays: it refuses on `reader` or `unknown` whenever the private pass is unavailable, and the scanner re-runs at each C5 request (GA1's listed item).
7. **FZ7:**
   - any viewport size, with pitch-aware colour and mask copies;
   - `size`, `RHI` and `client` as self-check readings;
   - D3D12 qualification, including an odd width.
8. **The card fixes,** drafted to `docs/europa/office-card-094-43-draft.md`:
   - run the launch line exactly as written;
   - R1 is read from the clone the plugin was copied from;
   - the copied plugin folder stays out of the game's repo.
9. **Self-tests:** the state contract (no custom-depth writes), the size and pitch handling, and the `unavailable` handling.
10. **Gates** (every launch through the armed kit):
    - **H1, no leak (fixture, outline on):** C5 with the default routes and seed 4242: 0 new magenta pixels on every saved frame against the outline-off control.
    - **H2, masks:** per-object IoU ≥ 0.98 against the bench-only legacy reference, including the hidden target on its hidden frames. The census counts are equal, or explained frame by frame. The lighting groups (GD4) are reported separately.
    - **H3, state:** `polluted=0`; `r.CustomDepth` unchanged; 0 components with `bRenderCustomDepth`; no stencil writes.
    - **H4, the EA_Core regression on the Paris host:** all of 094-42r's EA_Core gates, on D3D11 at 1280×720 and on D3D12 at 1280×720 and 1279×719. 0 dropped frames, every LOD event changes target pixels, at least one skeletal target if EA_Core has one (otherwise say so), and the `unavailable` counts are reported.
    - **H5, cost:** the added GPU and render-thread time per captured frame (the mean and the 95th percentile) on the Paris host.
11. **Exit:** FJ1's fast checks. Commit with W4 on `feat/europa-port`. No push by Codex.

STOP RULES
- **GA5:** any kit admission gate that holds for 3 h stops the job with NEEDS-DECISION and its partial results.
- **Launches:** only the armed kit launches the editor or the game. Fixture launches carry `-nowrite`.
- **Builds:** authorized for the fixture's targets (spike branch, Stage A) and the Paris host's and fixture's targets (Stage B). Fresh FM1 budget per target and per stage.
- **Protected changes:** any change to the Paris host's Content, Config, a descriptor or the `.uproject` is a STOP.
- **Crashes and uploads:** an upload sign is a STOP; a second runtime crash in a stage ends the run (FI4).
- **Untouched:** `m51`, `master`, `release/2026-10-05`, the tags, `spike/europa-09442c-mesh-shader`, the 5.1 engine and StackOBot.
- **No push by Codex, no merge of a spike branch and no tags.**
- **Machine:** no security, paging, power or machine-setting changes, and no elevation. Delete nothing except by receipt. If E: falls below its floor, stop and report.
- **Foreign activity** never ends the job. Record it (EZ1).
- **Codenames only.**

DELIVERABLE
A `report.md`, covering:
- the summary;
- Stage A: GD3 as built, with A-P2 to A-P5 and numbers;
- Stage B: FZ2 as built (the files, the scope, `unavailable` and the guard fallback, with citations), FZ7 and the card draft, and H1–H5 with numbers;
- any crash, upload sign or protected change (or "none");
- the final commit.

End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>". GREEN means Stage A and H1–H5 all pass.
