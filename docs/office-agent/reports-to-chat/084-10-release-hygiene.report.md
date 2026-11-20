# REPORT for 084-10-release-hygiene.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-10-release-hygiene.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 08:00:07 | finished: 2026-09-29 08:39:59 | duration: 40 min | exit code: 0
context at last turn: 703727 tokens (70% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-10-release-hygiene.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Summary for the owner:** Bench-only test entries and every `IAI.Bench.*` console command are now hidden in a delivered build unless it is launched with `-IAIBench` or `-IAIBenchFixture`, and they are compiled out of Shipping. A new audit script proves this and fails when an unguarded registration is planted. The packaging items are now scripts; each was run both ways on real packages and bundles. Labels, masks and product rows are unchanged, and nothing was launched.

**3. The gate and G-LEVER-AUDIT**
- **One test**, `AnomalyBenchGate::IsEnabled()` (new file `Public/AnomalyBenchGate.h`): false in Shipping, otherwise true when `-IAIBench` or `-IAIBenchFixture` is on the command line (ND-6).
- **When the gate is shut:**
  - `null_effect` and `solid_swap` are not registered, so they are not in `IAI.ListAnomalies`, the catalogue or the dashboard.
  - AnomalyBench's four commands and the two `StuckMipLegacy*` commands are never registered.
  - Every other `IAI.Bench.*` lever gets the engine's "unregistered" flag at `OnPostEngineInit` and at every injector `Initialize`, so the console refuses it. The log line reads `IAI bench levers: DISABLED (n masked) … preset=p`.
  - The flag is added to the existing flags. `SetFlags` replaces the flag word, and `UnregisterConsoleObject` would delete commands, so neither is used directly. This follows the engine source (G374).
- **Lookups by name:** the control server now uses a mask-aware `FindLeverCommand`, because `FindConsoleObject` ignores the flag.
- **The inline `-IAIBench` checks** (the texture-user logging, the StuckMip levers, `CameraSchedule`) now read the gate. One consequence: `-IAIBenchFixture` alone now also opens them, per ND-6.
- **Shipping:** the nine Shipping-compiled AnomalyInjector levers are wrapped, including `DestroyTarget`'s globals, latch and call. So are ten `SetBench*` setters and three log lines.
- **The audit, `tools/lever_audit.py`:**
  - Its first run **failed** on 26 lever strings still compiled into Shipping from setters sitting outside `#if ANOMALY_CAPTURE`. A grep of the registrations would have passed; I fixed them before committing (G375).
  - Final source run: PASS. 33 levers (27 masked statics, 6 gated runtime registrations) and 2 bench catalogue entries, all gated.
  - `--selftest`: 11 cases OK. The clean copy passes, and all 10 planted violations fail on their own rule, including an unguarded static and an unguarded runtime registration.
  - Binary check: PASS on the new Game exe `667FD4EF` and on the Editor DLLs; the old pinned exe `8A6074AA` FAILs.
- **Not yet observed at runtime.** No launch was allowed, so the masking, the log lines and the missing twins are predicted from source. The G-LEVER night legs are their first observation. No Shipping build exists; the Shipping claim is a source-level proof.

**4. Packaging items**
- **Dashboard `.env`:** the dashboard no longer reads a `.env`; its token lives in `dashboard\config.json`. `tools/check_delivery_bundle.py` checks that file against the delivered build's log (selftest 7 OK), and the checklist §2 has the box.
  - **Found (G376):** AnomDash's bundler ships the owner's **dev** token. `npm run build` copies the gitignored `public/config.json` into `dist/`, which the bundler copies, even though it prints "config.json was NOT copied". A real bundle's file was byte-identical to the dev one. That is the M2 incident's shape; I did not change AnomDash.
- **Readme host-tools path:** Step 6 now runs `host-tools\verify_capture.py`. On a real bundle the old readme STOPs and the new one passes. §9.8 now says the twins are not listed.
- **AnomalyBench/fixture exclusion lines:** `tools/stage_plugin_delivery.py` stages the plugin from `git archive` through an allowlist without `Source/AnomalyBench/` and its descriptor entry. It also prints the two `DirectoriesToNeverCook` lines for the fixture folders. Staging the pre-change commit `37bb750` FAILs; `92c3500` PASSes.
- **G354 check:** `tools/check_package_descriptor.py`, with its expected output in the checklist. It diffs the cooked descriptor and scans the container for fixture content (selftest 8 OK):

| Package | Result |
|---|---|
| 084-08b bench cook | STOP (`AnomalyBench` present, fixtures cooked) |
| same, `--bench-cook` | PASS (descriptor SHA-256 `9EFB9B49…`, matching 084-07b's reading) |
| 2026-07-15 build | STOP (`AnomalyShaders` missing, no fixtures) |

- **Uncommitted AnomDash edits on the home PC: none.** One clone at `D:\IntrusiveAnomalies\anomaly-dashboard`, `618b8df` == origin, clean, no stash, no extra worktree.

**5. Unchanged, and the build**
- **Unit suites:** m52 282/0, camera 154/0 (both `/W4 /WX`), `--label-rule` 24 OK; the other verifier suites (98, 35) are unchanged too.
- **String scan, `8A6074AA` → `667FD4EF`:** every added or removed string is a gate or bench string; no product string moved.
- **Build:** Game `667FD4EFB7F1…`; both targets exit 0 with 0 warnings. Archived with the Editor DLLs at `_binary_baselines\m52fix-667FD4EF\` (6/6 hashes re-checked there).
- Not cooked, not staged, not swapped in. The 084-09 harness stages from the archived pins, not from this build.
- Evidence is in `_reviews\084-10-evidence\`.

**6. Commits pushed** to `fix/m52-label-timing` (`37bb750..02d15a4`); no tag, no merge:
- `92c3500` feat(release): the gate and the four tools.
- `02d15a4` docs(release): journal `2026-09-29-084-10-release-hygiene.md`, status block, checklist, readme, architecture, gotchas G374–G377.

**7. Decisions for chat (none block tonight or 085-02)**
1. Should Code fix AnomDash's bundler: leave the dev `config.json` out, write the delivered token from the delivered build's log, and correct its message? Until then the checklist's token box is a manual step on Thursday.
2. Confirm that the `.env` item means the token file in `dashboard\config.json`.
3. Consequence to note: from this build on, any leg that uses a bench lever needs `-IAIBench`, and should check the `ENABLED` log line (G377). 085-03's harness must pass it. At 085-02, re-run `lever_audit.py` and its `--selftest` on the merged tree.

Separately, several MCP connectors (Gmail, Calendar, and some plugin connectors) need authorisation in claude.ai or `/mcp`; none were needed for this work.
api-equivalent cost (not billed on Max): $25.19 | turns: 202 | running total across all briefs: $3,423.70
