# REPORT for 081-37-m55-stage3-cost-design.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-37-m55-stage3-cost-design.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-26 04:31:30 | finished: 2026-09-26 05:17:44 | duration: 46 min | exit code: 0
context at last turn: 749948 tokens (75% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-37-m55-stage3-cost-design.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Plain summary for the owner**
- The "21.5 fps" leg from Stage 1 was one stall of about 1.5 s at the very start of an otherwise normal run, not a slow run. It wasn't caused by the new feature (it was switched off on that leg).
- The cost test is now fully designed and written down before anything runs. It will alternate feature-off and feature-on runs at 720p and 1080p and pass only if the feature costs at most +1 ms of game-thread time per frame, with a margin of safety, and doesn't slow image writing.
- The test harness was built and checked against known answers and fake runs; everything came out as expected, and it caught one bug of its own before it could matter.
- Nothing was launched on the bench. The next brief (081-38) runs it, in about 12–15 minutes of PC time (60-minute cap).

**1. The FPS 21.5 prior: PARTIAL**
- **Established from the bank.** The Stage-1 leg `M55S1V6_MAIN4_E0` (feature off, paced, 720p) reads 21.535 because that figure is a whole-session ratio: speed ratio 1.393, wall time 5.526 s against 3.967 s of game time.
  - All 1.558 s of excess sits between session_index 0 and 4 (frame gaps of 883, 111, 238 and 527 ms).
  - Frames 5–89 match the three sibling legs to the millisecond.
  - Among 340 banked m55 sessions, it is the only start-of-run stall. The other 21 stalls over 150 ms are deliberate bench delays, plus two ~170 ms blips in the 1800-frame S1 leg.
- **Excluded:** m55 (the feature was off) and shader compilation (`SHADERS pending=0 incomplete=0` on the first frames; prewarm 0.0018 ms).
- **Established from the log and source.** During the stall, each run-log line took 6–215 ms; the sibling legs emit the same lines within 2 ms of each other. The m38 run log forces a disk flush (`FlushFileBuffers`) on every line, on whichever thread logged it (G300).
- **Not established:** why the disk or the process was slow for those ~2 s. Stage-1 recorded no load context.
- **Handling in the cost run:**
  - the counted legs run with the run log off;
  - the game-thread measure counts CPU cycles, so time spent blocked or descheduled isn't charged;
  - the first 60 frames of each leg are excluded;
  - every leg carries a stall list and its load context;
  - a `HELD-FPS-PRIOR` rule stops the gate being read if any leg runs at under 0.75× its condition's median rate;
  - a declared diagnostic pair (`S3_DIAG_A`/`B`, 720p, run log ON) runs outside the gate.

**2. The campaign design** (`docs/predictions/2026-09-26-m55-stage3-cost.md`)
- **Recipe:** the accepted `C2_SOLID` / `C1_SOLID1080` / c0 recipe (`CB_GateLevel`, `StaticMeshActor_49`, `solid_swap`, config `2 4 16 4 0`, B1-gated fixture). It's the only accepted recipe run at both resolutions.
- **Settings:**
  - The only A/B difference is `IAI.Capture.ChangeEvidence 0` on A.
  - Both sides get pacing off, bench gate 0 (the base line's gate 14 is a bench device) and run log off.
- **Legs:** at each resolution, one declared discard leg (feature off), then A1 B1 B2 A2 and A3 B3 B4 A4.
  - 720p legs are 600 frames (30 events, under the 55-event tag-pool limit).
  - 1080p legs are 300 frames: pacing-off saturates the 1080p image writer (38–43 PNG/s in the bank), and the end-of-run writer flush waits at most 5 s.
  - Then the diagnostic pair. About 20 launches, 12–15 min expected, 60-min hard cap.
- **Metrics:**
  - **Game-thread ms per engine frame (gated):** CPU cycles of the thread UE names `GameThread`, sampled externally at 10 Hz and converted at the 2688 MHz timer rate. The engine's CSV profiler was rejected because it writes its header when capture ends and the runner force-kills the game. The OS thread-time counter was rejected because it misread a known load by 5–29 % (G299).
  - **Writer throughput (gated):** PNGs completed per second in the steady window.
  - **Readings:** m55 worker ms, CPU by thread type, m55 memory high-water against the 256 MiB cap and the writer backlog, drops, p95/p99 tails, required-pair yield with reasons, latency, `census_cycles`, and CPU/GPU/IO/disk load context for every leg.
- **Statistic:** Δ = mean(B) − mean(A). The pooled A/A and B/B spread is the noise floor, with a one-sided 95 % bound (t = 1.943, 6 degrees of freedom).
  - Game thread: PASS only if the upper bound is ≤ +1.000 ms.
  - Writer: UNRESOLVED if the upper bound of the change is below 0 (a drop beyond uncertainty), or if the uncertainty exceeds 5 % of the A mean (too coarse to see a drop).
- **Validity:**
  - Any person evidence, foreign load (over 2.0 cores average, 3.5 cores in any second, 5 % GPU or 20 MB/s I/O), or a foreign editor → the leg is invalid and re-run, never averaged in.
  - Fixture failures: 3 attempts, then NOT-RUN. Environment failures: 5 attempts, then exit 4.
  - Mode-check failures stop with exit 3; integrity or no-work failures stop with exit 1, at the first failure.
  - An interrupted ABBA block is voided and re-run whole.
  - Today's idle baseline was 1.16 foreign cores, mostly Windows Widgets.

**3. Proof table** (evidence in `_reviews\081-37-evidence\`)

| proof | expected | actual | evidence |
|---|---|---|---|
| +0.4 ms, tight spread | PASS | PASS (bound 0.422). End-to-end full-size run recovered +0.4 at both resolutions, PASS | `dryrun-post-lock\dryrun-results.json` |
| +1.4 ms | UNRESOLVED | UNRESOLVED (bound 1.422), also end-to-end | same |
| +0.6 ms, wide spread | UNRESOLVED | UNRESOLVED (bound 1.161; the tight-spread control gives 0.622, PASS) | same |
| writer drop beyond uncertainty | UNRESOLVED | UNRESOLVED (40→38 PNG/s, bound −1.88; end-to-end 40→36); 40→39.95 → PASS | same |
| invalid leg excluded and re-run | INVALID, re-run as T2, absent from the statistic | foreign load 2.6 cores → INVALID-LOAD; person evidence → INVALID-PERSON; foreign editor → INVALID-FOREIGN; each re-run as T2 and counted only there | same (S2) |
| game-thread sampler on a known load | ≈4.0 and ≈1.0 ms per frame | 3.745 and 0.970; background worker 0.93 cores | `proof-sampler.json` (4/4) |
| cycle counter runs at timer rate | within 5 % of 2688 MHz | 1.00–1.04× | `proof-cyclecounter.json` |
| analysis on real banked c0 legs | 081-24's published numbers | latency 331.5/330.5 ms (vs 333/331), throughput 24.74/24.88 (vs 24.71/24.84), high-water 66.39 MB; the mode check rejects both legs by name | `proof-readings.json` (2/2) |

Also in the walk:
- resume, the halting classes (writer flush timeout, feature not active, no measured pairs, wrong resolution, fixture exhausted), the environment cap, the bench budget, and the boundary cases (+1.000 PASS, +1.001 UNRESOLVED);
- the FPS-prior hold, too few legs, a noisy writer, and 6 load-rule cases.

**4. Boundary, dry run, and the 081-38 command**
- `_reviews\081-37-evidence\cost-boundary.json` pins feature head `ae56e17` (pushed), the committed predictions' hash (`fe52e4fa…`) and 15 harness files.
- Offline preflight replay: 0 problems. Walk of the real window runner: 7/7.
- Dry run 64/64 before the lock and 64/64 after, with 0 differences; all locked files unchanged.
- **081-38:** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\081-37-cost-window.py 081-38` (budget tag `081-38`).
  - Evidence goes to `_reviews\081-37-evidence\cost\`; bank folders are `M55B3R23_S3_*`.

**5. Deviations**
- **Choices of mine beyond the brief:**
  - the game-thread instrument is an external cycle sampler, not an engine metric;
  - the counted legs override the recipe's base line with run log off and bench gate 0;
  - I added the 5 % writer resolution guard and the 0.75× FPS-prior hold;
  - the foreign-load limits were set about one core above the measured idle baseline;
  - any person evidence voids a cost leg, which is stricter than 081-30's rule, as 081-37 orders.
- **Bugs caught by my own checks before the lock:**
  - the dry run showed the discard leg would have launched with the feature ON (fixed: discard runs feature off);
  - the sampler had a race naming threads (fixed).
- **Housekeeping:**
  - The only processes started were two synthetic Python loads of my own.
  - I deleted 66,840 synthetic dry-run PNGs (regenerable from the seeded script); all JSON kept.
  - The UNEXERCISED lines are referenced from the predictions, not copied there; the Stage 3 report carries them verbatim.
  - The 081-23 boundary was not re-issued (the requalification windows are finished).

**6. Commits pushed:** `ae56e17` on `feat/m55-change-evidence` (predictions, journal, G299, G300, CLAUDE.md status). Main checkout still `m51` `53bf725`; master `031a103` and tags untouched.

**7. NEEDS-DECISION:** one optional item, and it doesn't block 081-38.
- **The issue:** at 1080p the image writer is saturated and the m55 stage shares its thread pool, so I predict a real writer drop of 1–3 %. Under the ruled wording, that can read UNRESOLVED if the spread between legs is tight.
- **Decision needed:** if you want a practical tolerance, it has to be ruled before 081-38; that changes the harness and needs a boundary re-issue. Without a ruling, the declared literal rule stands.
api-equivalent cost (not billed on Max): $22.70 | turns: 122 | running total across all briefs: $2,298.20
