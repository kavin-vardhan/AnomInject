# REPORT for SRD-001-code-plan-only.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: SRD-001-code-plan-only.md | cwd: D:/IntrusiveAnomalies/SRDConcept/code | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-07 15:07:53 | finished: 2026-09-07 15:25:31 | duration: 18 min | exit code: 0
context at last turn: 105292 tokens (11% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\SRD-001-code-plan-only.live.txt

---- FINAL MESSAGE (Claude Code) ----
Plan written. I stopped at planning — no build scripts, no scene construction, no renders, no downloads, no subagents, nothing outside my workspace.

**Plan:** `D:\IntrusiveAnomalies\SRDConcept\code\PLAN.md`
**Discovery notes:** `D:\IntrusiveAnomalies\SRDConcept\code\notes\DISCOVERY.md` (+ three read-only probe scripts)

## Proposal in brief

26.0 s / 780 frames @ 30 fps — beat 1 establishing 6.5 s, beat 2 parallax 10.5 s (ending in a 2.0 s split), beat 3 pop-out 9.0 s. Two fully separated scenes, two passes per frame, MAIN always the active scene.

**I verified the core math rather than just deriving it.** I reconstructed Blender's own projection via `Object.calc_matrix_camera()` and compared it against an independent ray/plane derivation over four eye positions and 52 points: worst error 4.07e-07 UV. Corner pinning 4.2e-07; planar-anchor drift 7.3e-08; depth parallax matches `Δx·z/((d+z)·W)` to 2.5e-08; the in-front probe inverts sign as predicted. `shift_y = cy/W` (fit dimension), and the naive `cy/H` is wrong by exactly 16/9 → +0.2668 UV, 288 px at 1080p. Six orders of magnitude between right and wrong, so the KAT can't pass by luck.

Three findings that shaped the design:
- **`ViewLayer.depsgraph` doesn't exist in 5.2** — you can't evaluate a non-active scene. But `bpy.ops.render.render(scene=…)` does. So MAIN stays active permanently and GAME is rendered by name; the head locator is always in the live depsgraph.
- **The HERO seam is provably continuous.** A point in the panel plane projects to its own plane position for every eye, so the game half's boundary is drawn exactly where the MAIN half's boundary sits — no doubled car, no gap, no tear, from any camera. The honest cost is a C¹ kink (eye ray vs camera ray angle), which I log per frame instead of hiding. Crossing is contained by the eye-pyramid so the 0.833 m car doesn't emerge beside a 0.531 m bezel.
- **`Object.light_linking` is absent**, so the two halves get a deliberately view-independent car shader instead — matches by design and suits the flat art direction.

On viewer-vs-spectator: the panel shows one image for one eye, so beat 2 is locked-tripod side-on at 58° with a plane-flush start gate that stays pinned while the world sweeps up to 151% of panel width, then the labelled same-instant split. There's a `--panel-for-camera` flag that makes every shot look perfect — off by default and documented as a lie, because otherwise someone finds it and uses it quietly.

Full-sequence render is double-gated (`--allow-full` + `out/review/APPROVED`), and `--dry-run` prints every frame's eye/`d`/lens/shifts/seam/coverage checks in seconds before any pixel is spent.

Six things I need decided are listed in §17 — mainly the assumed dimensions (24″ panel, 45° tilt, 1.62 m eye, `GAME_WORLD_SCALE` 0.45), since those are my guesses and they set the whole look.

**VERDICT: NEEDS-DECISION** — owner plan approval is pending, not because the plan is failed. Chat review is not required for this concept-video asset; AnomalyInjector's next mandatory Chat checkpoint remains a coherent corrected design before implementation.
api-equivalent cost (not billed on Max): $3.06 | turns: 17 | running total across all briefs: $1,387.43
