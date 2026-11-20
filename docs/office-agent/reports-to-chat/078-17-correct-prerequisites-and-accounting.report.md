# REPORT for 078-17-correct-prerequisites-and-accounting.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-17-correct-prerequisites-and-accounting.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-07 09:40:40 | finished: 2026-09-07 10:17:25 | duration: 37 min | exit code: 0
context at last turn: 603035 tokens (60% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-17-correct-prerequisites-and-accounting.live.txt

---- FINAL MESSAGE (Claude Code) ----
Bounded documentation unit complete. Nothing ran; nothing is in flight.

## Commit

**`1f61196bfb2e3477eef50f65f1f564886d3a29d4`** on `m51`, parent exactly **`560e28ac2b88f6c65f1683970ebb5ab6c69bb969`**. One local commit; no old commit was amended (the two amends were of this session's own commit, to correct my own self-reported metrics before finalising — see Deviations).

`git diff --numstat 560e28a..HEAD` — **6 files, 857 insertions(+), 208 deletions(-)**, `git diff --check` exit 0:

| file | numstat | shape |
|---|---|---|
| `docs/predictions/…-identity-lifecycle-geometry.md` | 509/160 | revised **in place** |
| `CLAUDE.md` | 42/47 | 45-line block replaces the 50-line 078-16 block; **one hunk, `@@ -11,56 +11,51 @@`** |
| `docs/OPERATING-CONTRACT.md` | **99/0** | 11-line pointer at the 078-16 heading + 87-line dated `078-17` entry |
| `docs/sessions/…-078-15-…predictions.md` | 27/**1** | §10 append; the single deletion is the **top pointer line** (now names 078-16 *and* 078-17) |
| `docs/sessions/…-078-16-…revision.md` | **62/0** | top pointer + §8 correction |
| `docs/sessions/…-078-17-…prerequisite-accounting-correction.md` | 118/0 | new journal |

**Final design:** SHA-256 `4714B57F2908951F02DE07C7016F284E791DFA1CDE939ED36C9C8B363C44EA03`, **1,427 LF lines, 125,980 bytes, no BOM**, trailing newline. Other five: CLAUDE `39853BEF…` (uniformly CRLF, no BOM); contract `FB78035F…`; 078-15 `63C54118…`; 078-16 `8383BF41…`; 078-17 `69D817C6…` — all LF, no BOM. Metrics measured **after the last edit**.

## Engine reference — the brief's path was wrong and I did not follow it

The brief named `E:/EpicGames/UE_5.7/Engine`. That install exists, **but this plugin compiles against the project's UE 5.1 source build at `D:\UESource\UnrealEngine\Engine`** (`Build.version` = `++UE5+Release-5.1`, 5.1.1). Every engine citation below is from the **5.1** tree, and each of the independent review's cited lines was checked to land on the claimed construct there. (Also: the brief's `AnomalyWorldSubsystem.cpp` is `AnomalyCaptureSubsystem.cpp`; its line numbers match.)

## R1–R7 dispositions — corrected assertion vs OPEN interface

**R1.** *Corrected:* the four arm sites do **not** share a boundary — colour arms at `AnomalyCaptureSubsystem.cpp:4234` inside `CaptureCurrentFrame` (`:4181`, from `Tick` `:619`); normal/target/census inside `OnWorldTickEndMask` (`:809`; `:833`/`:839`/`:858`) ⇒ **a common freshness boundary is a PREREQUISITE, not a fact**. Mask predicate **body** is `AnomalyMaskSceneViewExtension.cpp:49-53` (header only declares at `:47`). `GetPriority` sort **is** descending (`SceneViewExtension.cpp:118-126` → `GameViewportClient.cpp:1290-1292` → `SceneRendering.cpp:4588-4591`). The engine **copies** family and views (`:2402-2406`, `:2415-2417`, `:819-826`, `:2453`) ⇒ **no raw GT pointer transfers; no carrier invented.** *OPEN:* final seal + expected consumer set (mint-then-mutate is not immutable) · carrier · unique-vs-repeated family · attachment · retention/reclamation · world/viewport **lifetime tokens** · membership must outlive the claim · **4a foreign-family refusal (non-destructive) vs 4b owning-context cancellation** · activation predicate and backbuffer bridge are **retained holds, not the only ones**.

**R2.** *Corrected:* logical credit releases **once at the FIRST terminal** (`A2.1` rule 6, release-hardening:1234), physical/tag/payload separate, `A1.6`:701-709 intact · `Failed(payload)` is **pre-submit reachable** — the miss `ForceMiss` produces returns at `AnomalySceneViewExtension.cpp:96-105`, **before** `:147`/`:149`/`:159` · `AddEnqueueCopyPass` **is** `GraphBuilder.AddPass` (`RenderGraphUtils.cpp:842-855`; lambda `:853` captures the raw readback) · late/old-generation delivery are **diagnostics, never a second terminal** · one admission boundary · census pixels (`AnomalyCensus.cpp:797-802`) ⇒ payload consumers without writer jobs · `Matches − Submits` is a **lead**, not exact (`LookupKey` doesn't invalidate). *OPEN:* **the 4×-cap ring + watermark is WITHDRAWN as unsound** (out-of-order retirement can reject a **live** request; counts can't name indices) — replaced by a stated **obligation**, no substitute invented.

**R3.** *Corrected:* the narrow **"agree at that instant" is withdrawn in full** — `LevelTick.cpp:1046-1200` → `PrimitiveComponent.cpp:620-633` (scene told **only if** `ShouldRender()`…) → `RendererScene.cpp:2248-2261` **ENQUEUES**. `UnscaledViewRect` added. *OPEN:* the five identity/lifetime questions · **return join** (fields arrive at ≥3 points on 2 threads) · transform/clipping algorithm · jitter convention · `BENCH_GEOM` path/schema/join/known answers.

**R4/R5/R7 preserved, uninflated.** Three request-ID-joined payload losses stand; no identity by ordinal; same-render sharing valid; never a one-slot FIFO. `MinZ`→`Clip.Z` only; `N-GEO2` held. Real names `IAI.Capture.SVE.ForceMiss`/`…ForceMissPhase` (`AnomalySveKeyRing.cpp:16-28`), `ShouldCorrupt` on `PublishSerial` (`:110-112`) while `WantedMatches` already exists (`:126-129`) — **all proposed corrections, NOT manifest edits**. `A2.3` = same-row **telemetry**, not a validity band; `A1.10`/`A2.2`/`T` still require `speed_ratio ≤ 1.05` etc.

**R6.** **Every schedule marked UNSCHEDULABLE** — `session_index` stalls under exactly the levers under test. New §D.4a lists what each row owes. Preserved: `WithholdWorldRender` capture-vs-settle, `NoStaleArmSweep` retaining the guard, `N-ID2`'s floor, `N-LC2`'s missing device, probe hold. **No gate became executable or passed.**

## Remaining blockers / unrun

Design **NOT ACCEPTED**: `R1`/`R2`/`R3`/`R6` retain OPEN interfaces; **nothing past step `2a` is designable** (`2a` itself needs the stamp point *decided*). `M-F1b` UNRUN; no valid candidate pairing baseline (four INVALID, no cause either direction); 078-05/06 failures stand; writer byte bound OPEN; publication/orphan/auditor HELD; all sixteen findings, every `open_arms` entry and the full regression set required; Lyra needs module SET + manifest.

## Deviations, stated

- **Design grew 1,078 → 1,427 lines.** Sections were removed/demoted as instructed (§B.5's ring, §A.4's "authoritative immutable" heading, §D.1's designable-now claim, §D.4's "exact schedule"), but each correction carries the withdrawn text **plus the source line refuting it**, which is net additive.
- **CLAUDE block 45 lines vs the ≤35 aim** (down from 50); **contract entry 87 lines vs ≤45**. Both stated at measured size.
- **Two amends of my own commit**, to fix my own stale self-metrics (contract append "~70" → 87) and a stale heading ("Six orthogonal owners" → seven after the 3/3b split). Parent stayed `560e28a`; still one commit. Flagged because shipping an inaccurate self-metric is the exact failure this unit exists to correct.
- **No binary string scan run** — `IAI.Capture.SVE.ForceMiss` presence in `B8AC65D8` stays **UNVERIFIED** (carried, not newly claimed).

## Scope verification

Working tree clean but for the ten preserved untracked handoffs. `6713f51..HEAD -- Source Shaders` **empty**. `master` == `origin/master` == `ac13700`. **All 53 pinned files re-hashed, zero mismatches** — 33 older predictions incl. manifest `D33AF5D4…6025F9`, both exe archives + staged candidate `D50DDE78`, five containers, watcher/`run_brief` local+canonical, the 078-16 report, five evidence labels, two checkers. CaptureBench `472a409` tracked-clean + seven prior untracked tools; Lyra detached-clean `4a4bfcc`. **Exactly one watcher, PID 22072** (a second "match" was my own query string echoing in a command line — verified). No capture, checker run, build, cook, staging, swap, source/shader/tool/fixture/schema edit, owner-app action, merge, push, tag, replay, interleave, agent or worktree. Scratch was `C:\ClaudeTemp\078-17-scratch` — outside every repo and the mailbox, left in place.

**These report labels are Code self-assessment pending independent Codex review.**

**Chat review is NOT REQUIRED for this correction's documentation disposition; the next mandatory Chat checkpoint is the coherent corrected design BEFORE implementation.**

VERDICT: NEEDS-DECISION
api-equivalent cost (not billed on Max): $43.08 | turns: 142 | running total across all briefs: $1,384.37
