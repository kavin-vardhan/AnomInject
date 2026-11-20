# REPORT for 094-43a-europa-depthonly-byname-proof.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-43a-europa-depthonly-byname-proof.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-03 23:31:38 | finished: 2026-11-04 01:48:05 | duration: 136 min | exit code: 0
context at last turn: 736389 tokens (74% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-43a-europa-depthonly-byname-proof.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:**
- The cheap fix almost works. It builds, causes no shader recompile and never touches the game's custom depth.
- But it marks an object one frame before the picture actually shows it, on 19 of 2,352 frames (the masked and wind-moving test objects). The run stopped there as the rules require, so the outline-on test was never run. On this run's data, a small rule change fixes it, but that is untested.
- Earlier 5.1 data: the only 5.1 game where we ever recorded an outline-reading material is our own Lyra test bench, which was never delivered. For office and client data, no scanner reading ever reached home, so we can't say either way.

**The route** (spike `c1193bd`, local only):
- The shader types are found by name through RenderCore's public registry, then taken from the material's own shader map for the mesh's vertex factory. It falls back to the default material for plain opaque meshes, as the engine does.
- Drawing goes through the exported mesh-pass machinery into plugin-owned depth and ID targets.
- Every include resolves to a 4.25 Public folder. It names no unexported Renderer symbol and calls no Renderer-private member. Nothing sets custom depth, stencil, a write mask or `r.CustomDepth`.
- Shortcuts that will matter on Europa: plain static-mesh components only, a search of every scene primitive per object, a new depth texture every frame, and a blocking wait every captured frame.

**Gates (checked myself):**
- **P1 — pass.** Final Editor and Game builds both have 0 link errors. The Editor's three-build budget is spent.
- **P2 — pass.** The second warm launch logged 0 `Missing cached shader map` lines. The private-pass run itself also logged 0.
- **P3 — fail.** I reproduced Codex's numbers exactly with my own reader.
  - Opaque, forced-LOD and lighting-group objects match (minimum IoU 0.99992). All 132 blink-hidden frames are non-empty with IoU 1.0, and all 731 event masks match at 1.0.
  - The masked object fails on 7 frames and the wind-moving (WPO) object on 12, each at IoU 0.
  - On every failing pixel the scene depth is empty, and the next frame's reference mask covers all of them. Example: `_reviews\094-43a-example-frame708-private-early.png` (the object is absent at frame 708 and present at 709).
  - Codex's explanation (the engine's occlusion culling lags a frame) fits, but it was not instrumented, so the cause is not established.
- **P4 — not run,** because the stop rule (GC4) halted the job at P3.
- **P5 — pass.**
  - 2,352 of 2,352 rows have 0 custom-depth components; the before and after states are identical.
  - `r.CustomDepth` reads "1", set by the engine default, at all three checks.
  - The pass's own "no stencil writes" field is a declared value, not a measurement.
- **Cost per captured frame:**

  | Measure | Mean | p95 |
  |---|---:|---:|
  | GPU | 0.486 ms | 0.628 ms |
  | Render-thread submit | 0.213 ms | 0.333 ms |
  | Submit plus blocking wait | 2.24 ms | 2.56 ms |

  Game-thread cost was not measured.

**GC6** (`576975d`):
- The kit now saves a stop's intent (time, PID, reason) before stopping, and counts it only for the same PID while the game is still running.
- 7 of 7 self-tests pass on my own copy.
- On the real runs it classifies correctly: one deliberate stop (exit 125) as an owned stop, the one real crash (exit 3) as a crash.

**FZ9** (`_reviews\094-43a-fz9-findings.md`):
- Lyra recorded 1 reader every time (`MaterialInstanceDynamic(depth+stencil)`, its outline material): 56 logs on its two shooter maps, Sep 4–25.
- StackOBot recorded 0 on every map (612 logs).
- Neither office host has any reading at home. Their pre-m41 builds had no scanner, and later readings were never typed home. Tagging was active in those runs.

**Safety:**
- One crash: the first warm-up hit a D3D11 shader-compile error (a 4-channel UAV read). It was fixed in the shader only, and the native binaries were unchanged.
- No upload signs. All 6 fixture launches used `-nowrite`.
- The Paris host's Content, Config, binaries, descriptor and `.uproject`, plus the fixture's seven assets and preserved `Engine.ini`, are all hash-equal. Paris was never launched or built.
- `m51` is unchanged. Binaries changed only during builds.
- Nothing was deleted by command. Three files from the old spike left the fixture's working folder when it switched branch; they are still in the old spike's commit `740220c`.
- No push or tags. The new spike is not merged. `feat/europa-port` is at `75ca580` locally; origin is still `d1cdc7b`.

**Decisions for chat:**
1. On this run's data, a rule passes P3 on every object. It counts a shown object only where its private depth equals the scene depth (a bit the pass already computes), and keeps today's test for hidden and missing targets. Do we spend one more night re-testing the spike with that rule (then P3 and P4), or take GC3's recompile fallback? This rests on one run on one fixture; it is not a proof.
2. The Editor's three-build budget is spent, so any re-test needs a fresh FM1 build budget.

My review is in `_reviews\094-43a-code-review.md`. Codex's report is copied byte-identical to `_reviews\094-43a-codex-report.md`, and my receipts are in `code-receipts-094-43a\`.
api-equivalent cost (not billed on Max): $26.15 | turns: 194 | running total across all briefs: $5,579.89
