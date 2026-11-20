# REPORT for 080-01-m52-stuck-low-mip-source-read-and-plan.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 080-01-m52-stuck-low-mip-source-read-and-plan.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-20 18:44:08 | finished: 2026-09-20 19:17:37 | duration: 33 min | exit code: 0
context at last turn: 522913 tokens (52% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\080-01-m52-stuck-low-mip-source-read-and-plan.live.txt

---- FINAL MESSAGE (Claude Code) ----
Done. Everything committed and pushed; checkout restored.

---

# 080-01 — m52 `stuck_low_mip` plan — REPORT

**Branch `feat/m52-stuck-mip` = `origin/feat/m52-stuck-mip` = `eed4071`** (2 commits off `master` `3ff88db`, pushed).
Checkout was on `m51` `53bf725` and was **returned to `m51` `53bf725`**, tracked-clean. `master` == `origin/master` == `3ff88db`, unchanged. CaptureBench untouched at `472a409`, tracked-clean — deliberately, so `m51`'s frozen campaign keeps its anchor.

⛔ **No plugin source, no build, no cook, no leg, no binary touched, no tag, no agents.** Dedupe ran first (no branch, commit, file or `git grep` hit for m52 anywhere) — this brief had not arrived before.

## Deliverables
- `docs/predictions/2026-09-20-m52-stuck-low-mip.md` — 10 sections incl. the file-by-file plan (§9)
- `docs/sessions/2026-09-20-080-01-m52-plan.md`
- `tools/m52_texture_sharing_scan.py` — the only executable artifact: read-only, offline, opens no engine

## The lever decision — three of your four are refused, and the chosen one is a fifth

**🚨 `L3` is worse than "expected to reject" — it is a trap.** `UTexture::LODBias` is a runtime `BlueprintReadWrite` UPROPERTY and `UpdateCachedLODBias()` is exported and *not* editor-gated, so it compiles and runs in a packaged build — and **`CalculateLODBias` skips the `LODBias` term entirely when `FPlatformProperties::RequiresCookedData()`** (`TextureLODSettings.cpp:176-186`). It works in PIE and is a **silent no-op in the client's configuration**: G119 and G114 in one place. I did not "measure it once" — the source settles it, and measuring would have cost a build plus a leg. **Every m52 gate is pre-declared packaged-only as a result.**

**`L1` fails twice.** The streamer doesn't merely re-stream after you: it **cancels your request** — `AsyncTextureStreaming.cpp:803-808` queues a cancellation whenever `RequestedMips < min(ResidentMips, WantedMips)`, which a manual `StreamOut` on a visible texture satisfies by construction. **`L2` inherits that**: unlink-first stalls (nothing ticks an unlinked asset's pending update; `TickStreaming` is not `ENGINE_API`), StreamOut-first is the race. Retained only as an optional lock, default OFF.

**✅ `L4′` CHOSEN — `NumCinematicMipLevels` + `UpdateCachedLODBias()`.** Public runtime UPROPERTY; its term sits **outside** the cooked gate (`TextureLODSettings.cpp:171-175`); it reduces `MaxAllowedMips` every streamer update and **every** wanted-mips path is clamped to it, including under force-fully-load. So `RequestedMips == WantedMips`, the cancel predicate is false, and **the streamer stops being the adversary and becomes the enforcer** — no resource recreation, no blocking wait, no global cvar. Five failure modes named, each detected rather than assumed absent.

## The sharing numbers (measured, both fixtures, against the banked target set — not an invented one)

| fixture | targets | all-exclusive | ≥1 exclusive | least-shared ≤1 |
|---|---|---|---|---|
| StackOBot | 18 | **0 (0 %)** | 3 (17 %) | 7 (39 %) |
| Lyra | 5 | 0 | 1 (20 %) | **4 (80 %)** |

Six StackOBot targets sit on **one identical 9-texture modular family** shared with 17–84 meshes; the heavy sharers are utility textures (`BaseFlattenNormalMap` 84, `127grey` 74). Lyra's weapons have exactly **one** other mesh per texture.

⚠ **This is level-wide, not view-wide** — your example phrasing implies a visible-set join, which needs a runtime probe, which needs plugin code this brief forbids. It bounds the hazard from **above**. The visible-set number is gate `G7` and is named as the first number the implementation session must produce.

**All three of your options are refused by the measurement.** S1/S3 are per-*target* filters (admit ≤3 of 18); S2 is schema-compatible but not machinery-compatible (one stencil tag per event since m50, one-target ownership since m44). **Recommended S4: gate per TEXTURE on the visible set** — on `SM_rock` that holds the rock's own three textures and leaves `T_black` (39 sharers) alone.

**🚨 `CB_GateLevel` is out as an m52 fixture, and not for the reason I first wrote.** Its targets carry `BasicShapeMaterial`, which references **no texture at all** (`make_gate_level.py:60,68-69`) — the anomaly refuses 100 % of it *before* sharing is consulted. I caught this verifying a citation, corrected both documents before committing, and added it as declared weakness #6 of the scanner (an asset's default material slot is not what a level assigns).

## Evidence
Registered **`AnomalyState`** with `IsCurrentlyAnomalous()` = *the mip is measurably below baseline*, so onset-latency and streamer-wins frames are **not labelled** — which makes m44's ONSET gate satisfiable by construction instead of by hoping latency is zero. `observable` needs no change (`bLabelled && bHeld && Px >= min`, `AnomalyCaptureSubsystem.cpp:3906`). The perceptibility rule is a **pick-time filter only** and never decides `observable`; its UV-tiling weakness is stated, and the engine's better metric (`WantedMips − ResidentMips`) is unreachable from a plugin — filed, not wished for twice.

## Open questions for chat (predictions §8)
1. Pool: `GAutoPool` yes, `GAutoPoolDefaultEnabled` **no** until `G7` has a yield number? (recommended — `lod_popping`'s m29/m30 precedent)
2. `CB_GateLevel` substitute: (a) MainWorld + Lyra only, or (b) a sibling `CB_MipLevel` (costs a cook)?
3. `label_schema` 2 → 3, or leave at 2? (recommended: leave — keys appear only inside an anomaly type no v2 consumer has seen)
4. `MaxCoAffected` default `0` — keep, or relax before `G7` measures the yield? (recommended: keep)
5. S2 multi-node disclosure — file as Tier-2 escalation only?
6. Hold all eligible textures or only the largest? (recommended: all)

**Deviations flagged:** console names follow the shipped `IAI.Anomaly.LodMaxDistance` convention, not `IAI.Anomaly.StuckMip.*`; the chosen lever is not one of L1–L4; the sharing policy is a fourth option; the sharing numbers are a bound, not the view-wide count. Gates `G1`–`G11` are pre-declarations, not results — `G6` (hitch) and `G7` (yield) cannot be run in a plan-only session and predict no value.

VERDICT: GREEN
api-equivalent cost (not billed on Max): $29.45 | turns: 105 | running total across all briefs: $1,742.04
