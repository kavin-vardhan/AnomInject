# SESSION: new
# EFFORT: xhigh
# NAME: 068 adversarial re-read

FRESH SESSION — one task: try to BREAK journal 068 §1 (the transition-driver + toggle-line-anchor source read). Headless: your FINAL MESSAGE is the report; never wait for input. Codename discipline absolute (office hosts = Concorde / Bates / Deimos only; AnomalyCapture.Build.cs is never edited; never paste raw scrub-verifier output).

Bootstrap: CLAUDE.md, then docs/sessions/2026-09-02-068-p9-transition-drivers-and-toggle-anchor.md §1 ONLY (do not read §4/§5 first — form your own view of §1 before anything else), then docs/invisible-anomaly-mechanisms.md §8.6a. Verify `git rev-parse HEAD` == origin/master (expected 970bf1d or later; report the SHA).

THE TASK. §1 is now load-bearing: the owner's Bates C-3 bundle will be interpreted through it. Your job is to REFUTE it, claim by claim, from source — not to restate it. For each of the following, either produce a concrete counter-example with file:line, or state "could not refute" with the specific evidence you checked:
  R1. The flip table (§1.1): is there ANY tick ordering, registration order, or code path under which flip 1 is NOT on an uncaptured settle tick, or under which flips 2/3 land on a different tick than claimed? Check the subsystem tick registration order claim explicitly — is "capture ticks before injector" actually guaranteed, and what happens to the table if it is reversed?
  R2. The two-clocks claim (§1.2): is there any conversion or reconciliation between FramesSinceToggle and SessionFrameIndex that §1 missed? Is the "local ratio 1.0 inside Positives" claim true on every phase transition, including the first burst after LeadIn?
  R3. The prefix anchor (§1.3): is `GFrameCounter % 1000` in the log prefix ALWAYS the same value the arm stamps into frame_index on that tick? Look for any path where the log line is serialized on a different thread or after GFrameCounter++ (buffered/async logging, the log's own thread, `-LogTimes` variants), and for any capture arm that happens outside UWorld::Tick.
  R4. The "n+7 carries no row" claim and the "exactly 3 toggle lines per event" expectation (§1.4/§1.5): find a config or path (sync fallback, half-period ≠ 3, burst truncated at run end, auto-pool vs targeted) where either is false — and say whether the card's C-3 config (2 4 8 4 0, blinking, 90 frames) is inside or outside that case.
  R5. The three-way comparison (§1.5): is there an outcome the bundle could produce that §1's table classifies WRONGLY — i.e. a case where "log sides with labels" would be read as "pixels are the outlier" but the code admits another reading? Name it if it exists.
  ⛔ NO MECHANISM FOR P9 (B). You are auditing a mapping, not solving the bug. No source edits, no build, no legs, no commits to source. Docs-only: append your findings as journal 068 §6 "ADVERSARIAL RE-READ OF §1" (refuted / not-refuted per R1–R5, with evidence), commit `docs(p9): adversarial re-read of the transition-driver source read`, push.

FINAL MESSAGE: the R1–R5 table (verdict + one-line evidence each), any correction §1 needs (exact wording), the commit SHA, and NEEDS-DECISION items or "none".
