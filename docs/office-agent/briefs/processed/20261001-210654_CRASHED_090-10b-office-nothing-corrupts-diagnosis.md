# ACK-VERDICT: (chat fills in at release)
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 090-10b URGENT: nothing corrupts on the second office host (Code, Opus 5.5)

# 090-10b — URGENT, in-engine: on the second office host, m53 head `31078c7` (exe `8131479A` here) fires stuck_low_mip, uv_corruption and normal_corruption repeatedly and **nothing visibly corrupts, ever**. `IAI.TexCorrupt.Census all` then **froze the game**. Neither office project uses Nanite. Find the cause **in a running game at home**, fix it, and prove the fix **from pixels** before anything else ships

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going. End the run only when every step is done, a stop condition fires, or something needs the owner. The final message follows the structure at the end and starts with a `VERDICT:` line. Builds and game runs outlive the tool timeout: run them in the background and poll. **Keep context lean (finish below 70 %).**

**The owner says the home PC is free. Game launches are allowed when the harness-style idle gate passes** (no UnrealEditor of any engine, no foreign GPU-heavy process, the person gate). Poll, and launch after 3 consecutive idle reads. Never touch the owner's processes.

## What the owner saw (his words, numbers only; office data never comes here)
- **Host:** the second office host, plugin replaced wholesale with `feat/m53-uv-normal-corruption` `31078c7`, compiled clean.
- He ran all three anomalies, "multiple times": **"Absolutely nothing is getting corrupted."**
- `IAI.TexCorrupt.Census all` → **the game is stuck.**
- **Neither office project uses Nanite. Treat that as fact.**
- It is not known whether m53 **ever** corrupted anything at the office. This is its first office test. m52 already fired zero on the first office host (the yield problem).

## Hypotheses (rank them by evidence; don't assume)
- **H1, a 090-09 regression:**
  - the new Nanite gate misclassifies ordinary primitives (probe returns true for non-Nanite component types: ISM/HISM, landscape, spline, skeletal, decals, particles, text);
  - or fails closed because the probe isn't registered in that host's configuration (editor/PIE, module load order);
  - or R5's per-frame check reverts every fire at the next tick;
  - or R3's "registered and rendering" test reads false and something reverts on it.
- **H2:** **virtual textures**: m52 and m53 refuse VT, and a UE 5.1 office project may use VT throughout. StackOBot likely doesn't, so we'd never have seen it.
- **H3:** budget/size: m53's 128 MiB cap with 4K textures, all-or-nothing per object; m52's purity / ratio wall.
- **H4:** editor/PIE vs packaged differences; ini defaults (auto-pool enables, per-type OFF flags) not present on a fresh plugin drop; streaming settings.
- **H5:** anything else the code shows.

## The work
1. **Reproduce in-engine at home, on content with NO Nanite data**:
   - the m53 fixture `CB_TexCorruptLevel`, plus any non-Nanite real level available;
   - also MainWorld with `IAI.Targets.AllowNanite 0` as a control.

   **Exes to compare (bisect)**, each run in **both** a packaged/staged game and the **editor PIE** (the office very likely runs in the editor):
   - `8131479A` (`31078c7`);
   - the 090-07 m53 exe `A3EC29D8` / `97D292F8` (`2015cfa`);
   - the 090-04 m53 exe `45FD344C` (`fee8c7a`, before any Nanite gate);
   - the 090-10 head if 090-10 finished.

   For each: auto-pool with the three types ticked, plus targeted fire on a known-eligible fixture object. Record:
   - **whether the picture changes**, from pixels: a frame diff on the target;
   - every refusal or revert line with its reason;
   - `run_summary`: `refused_nanite`, `refused_nanite_probe_missing`, `nanite_midevent_reverts`, `label_effect_partial_frames`, `effect_interrupted` counts, m53/m52 refusal buckets.
2. **Name the cause**, with file:line and the log line that proves it, and the first build where it appears. **If a cause can only be confirmed with office readings** (VT, budget), say exactly which **numbers-only** read-out the owner should take, and give him a **verified** one-command tool for it (stdlib Python, numbers only, selftested; e.g. extend `m52_log_counts.py` into `tools\anomaly_refusal_counts.py` covering m52 + m53 + the Nanite gate reasons).
3. **Fix it** (fix branch first, then merge into m53).
   - **Tests both ways,** plus the **mandatory in-engine proof:** on the non-Nanite content, uv_corruption and normal_corruption **visibly corrupt** (pixel diff above a declared threshold) via the auto-pool **and** targeted fire, on the fixed exe, in the editor PIE **and** the staged game. stuck_low_mip fires where eligible.
   - **The label basics still hold:** the labelled window matches the changed pixels on at least 3 fires per type, 0 frames off at both edges, AA off.
   - **Can-fail:** the same proof on the broken exe shows no corruption.
4. **Never silent again.**
   - Whenever an auto-pool round or a targeted fire finds **zero** eligible targets for a type, log one line per type (rate-limited), and show it on the dashboard if the dashboard has a status channel: `<type>: 0 of N candidates eligible — <reason> n, <reason> n, …`.
   - Also a one-shot startup line: Nanite policy, probe registered yes/no, VT enabled yes/no.
5. **The census freeze.**
   - Time `IAI.TexCorrupt.Census all` on MainWorld and the fixture.
   - Find why it can block (a game-thread scan of the whole world? waits on texture compile or streaming?).
   - Make it unable to freeze the game: time-sliced, or bounded with progress lines and a clear "scanned K of N, stopped at T s" ending.
   - Prove it in-engine.
6. **Then:**
   - builds on both branches;
   - the string scan (every difference explained);
   - strict 0/0;
   - the lever audit;
   - archive.

   Commit and push. **The m53 head you push must be the one that passed the in-engine proof.**
7. **Docs:** the journal `docs/sessions/2026-10-01-090-10b-office-nothing-corrupts.md`, status blocks, and gotchas. Add a checklist rule: **no build goes to an office host until it has visibly corrupted in a running game at home, in both PIE and the staged game.**

## Hard rules
- **Never change any security setting.** If `dbghelp.dll` blocks staging, say exactly what you couldn't stage and prove in PIE instead.
- Never kill anything that is not yours, and never by name. Close only the game/editor you launched, by its PID.
- Delete nothing. Bank to `D:\IA_BankOverflow\`.
- Don't touch `m51`, `master`, tags, `ToCodex\`, `E:\<personal-folder>`, or the frozen `_reviews\090-06-*`.
- **Codenames only.** No office project names, ever.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner**: 3 lines: why nothing corrupted, what's fixed, and the **exact commit to pull**, with the proof that it corrupts in a game.
3. The bisect table: exe × (PIE / staged) × type → corrupts yes/no, with the reason lines.
4. The cause (file:line, log evidence) and the fix; tests both ways; the in-engine pixel proof and its can-fail.
5. What only the office can confirm, and the verified one-command read-out for it.
6. The zero-eligibility line and the startup line, as built.
7. The census: the timing before and after; why it froze; the fix.
8. Builds, archives, commits pushed.
9. NEEDS-DECISION items, or "none".
