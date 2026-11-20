# REPORT for 085-02-m53-dc-source.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 085-02-m53-dc-source.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 08:45:39 | finished: 2026-09-29 09:57:36 | duration: 72 min | exit code: 0
context at last turn: 545969 tokens (55% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\085-02-m53-dc-source.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Summary for the owner:** The m52 fix is merged into the m53 branch, and all the delivery-cut code is written, built and tested offline. Codex's source review, run in the background, found 3 high and 2 medium problems. Two of the high ones sit on the exclusion path that tonight's bench would run, so chat needs to rule on them before 085-03. The docs pass also found that the dashboard can't send an m53 mode for a targeted capture.

**3. The merge (`7cda2f1`, pushed on its own)**
- **Conflicts:** the same 8 files as the dry run. The fix branch had moved on to `02d15a4` (084-08b docs and 084-10's bench gate) but added no new conflicts. Every resolution keeps both sides and is listed in the commit message.
  - `AnomalyCaptureSubsystem.cpp` had 6 hunks. The sync path now builds m53's telemetry from m52's `SyncFrame.Fires`, and there is one `WriteRunSummary` call.
  - The label writer and both headers take the combined signatures.
  - In gotchas.md the numbers run in order: G308, then G309–G342, then G343–G377. In CLAUDE.md both status blocks are kept.
- **The destructor:** the two definitions are now one. It removes the registry entry first, then calls `StopHoldMonitor`.
  - `tools/stuck_mip_destructor_check.py` passes on the merged tree. Its selftest runs 6 cases, and every mutant fails as it should. It also fails on the pre-merge m53 parent.
  - It is a structural check, not a runtime one: the destructor can't run outside the engine, and launching was off-limits.
- **The merge commit's own suites:**
  - Both targets built with exit 0 and 0 warnings.
  - m52 282/0, camera 154/0, texcorrupt 172/0 (its mutant fails 34).
  - `--label-rule` 24 OK, label-pixel-gate 98, change-oracle 35, consistency OK.
  - The lever audit fails on the bare merge: 10 R1 failures, all m53 lever strings that compile into Shipping. 084-10's hand-off predicted this, and the DC commit clears it (G378).

**4. The DC day items (`b1cfc8a`)**
- **Four modes:** `TexCorruptCore.h`, the mode parse inside `Apply` in `Anomaly_TexCorrupt.cpp`, the knobs in `TexCorruptState.cpp`, and an enum `mode` in `GetAuthoredSpec`.
- **Mode refusals, produced by a real Apply:** `no_mode`, `unknown:<arg>`, `family:<arg>`, `not_in_delivery:<mode>` (ND-7), and `no_mode_enabled` for the pool's reserved `!none`.
- **Scramble:** a CRC over (seed, per-id attempt counter) in `TexCorruptPure.h`, with `ordinal`, `scramble_h`, `a` and `b` logged.
- **Auto-pool mode draw:** both ids are in the pool on keys 8 and 9, off by default. `DrawAttempt` draws id, target, hold, and then exactly one mode for m53 ids. The `Auto.Draw` log line follows the RNG contract.
- **All-or-nothing (ND-1):** a new step `V1P` between V1 and V2 refuses `partial_footprint:q/n:reason` and touches nothing.
- **m52 ⟂ m53 exclusion:**
  - The rule itself is in a new pure header, `AnomalyExclusion.h` (trail-aware, ND-2).
  - The capture registers the trail provider from `StartRun` to `FinishRun`/`Deinitialize`.
  - Excluded ids are dropped before any draw in `TryFireOnce`, checked again in `TryFireSpecific`, and caught by the `ApplyAnomaly` backstop (ND-3).
  - `Auto.Exclude` lines, the `auto_excluded_*` counters and the overlap-frames counter are all in place.
- **N1 label rule:** the id-to-source table moved into a new pure header, `AnomalyActiveSource.h`. m53 ids resolve to FireWindow, so `labelled` comes from `IsAnnotationMember`, and there is a test.
- **Office census:** `IAI.TexCorrupt.Census [all]`, in every configuration, printing counts only.
- **Cost:** `apply_ms` and `revert_ms` on the log lines, plus a bench-only `TEXCORRUPT-COST` line per captured frame.
- **Docs:**
  - client-readme §8.8 covers the normal-mode visibility limit (written as "rate: to be measured") and the MainWorld floor example.
  - The raw-Apply limitation wasn't stated anywhere in the readme; §8.8 now says an overlap is refused on every route.
  - Also updated: the architecture catalogue, six checklist boxes, and gotchas G378–G380.

**5. PURE-KAT and G-LEVER-AUDIT** (I re-ran all of these myself on the committed tree)

| suite | clean | can-fail |
|---|---|---|
| pure test (CRC, mode, V1P) | 251/0 | mutant fails 56 |
| draw known-answer test | 286/0 | skipped draw fails 107; always `Modes[0]` fails 13; one extra draw fails 131 |
| exclusion truth table | 102/0 | "m52 live until BeginRevert" fails 25; "ignores revert_settling" fails 6 |
| active-source | 47/0 | mutants fail 7, 14 and 1 |

- G-LEVER-AUDIT passes with 46 names and 0 failures; `--selftest` runs 11 cases OK.

**6. Build**
- Clean plugin rebuild on the warm `_r53_host`: Editor 16 actions, Game 7 actions, exit 0, 0 warnings.
- Exe `27055E55` (242,431,488 B).
- Binary lever check: 46/46 names, echo present; the five Editor DLLs pass too.
- A44: every new DC token is in the exe, and two existing strings used as controls are present.
- Archived at `_binary_baselines\m53dc-27055E55\`, 6/6 re-hashed there. Not cooked, not staged, not launched.

**7. The Codex review (1,394 s)** — collected to `_reviews\085-02-codex-m53-dc-source-review.md` and not acted on. Its verdict is NEEDS-DECISION.
- **F1 high:** once an m52 trail closes, m53 is let in, but m52's anti-aliasing tail frames can still be emitted. This is on the EXCL-A leg with AA on.
- **F2 high:** a trail detached with receipts still outstanding can reopen m52 after m53 was let in. This is on EXCL-A's release path.
- **F3 high:** re-applying with a missing target drops the active record but leaves the effect running, which defeats the backstop. Not on the planned recipes.
- **F4 medium:** the census shares the pool's candidate listing, which can print actor names and change the run's stats. This is on G-CENSUS.
- **F5 medium:** a bench-delayed restore can outlive `FinishRun` and leak into the next run. This is on the run-end path of the RestoreDelay can-fail legs.
- Codex also confirmed the merge loses no m52 label-sync behaviour and `DrawAttempt` matches UE 5.1's random stream.

**8. Commits pushed on `feat/m53-uv-normal-corruption`:**
- `7cda2f1` merge
- `b1cfc8a` DC source
- `0bb6dfb` destructor test
- `c9a098d` docs

No tag and no merge to master. `m51`, `master` and `fix/m52-label-timing` are unchanged. Context used at the end: roughly 55 % (my estimate).

**9. NEEDS-DECISION**
1. **Codex F1 and F2 (high, on EXCL-A).** Does the m53 admission wait for m52's tail and outstanding receipts to finish? Or does 085-03 go ahead with these recorded as known risks?
2. **Codex F3, F4 and F5.** Fix now, or record as limitations? F4 sits on G-CENSUS and F5 on the RestoreDelay can-fail legs.
3. **Dashboard targeted capture sends no mode.** A targeted m53 capture from the dashboard is refused `no_mode` on every burst. My recommendation: when no mode is given on a targeted capture, use the first enabled mode and log it. That needs no dashboard change before Thursday; the other options are a dashboard change or keeping the console route in the readme.
4. **Smaller calls to confirm:**
   - A deferred mode of the other family refuses `family:`, not `not_in_delivery:` (e.g. `uv_corruption flat` reads `family:flat`).
   - The old `Auto.Fire` log line is kept beside `Auto.Draw`.
   - In free-running mode with no capture, m52 counts as restoring while its restoring set is non-empty; it's unproven that this ever ends.
   - `IAI.Auto.Pool all 1` now also turns on the two m53 ids, which changes the draw sequence for an "all" pool.
api-equivalent cost (not billed on Max): $63.47 | turns: 104 | running total across all briefs: $3,487.17
