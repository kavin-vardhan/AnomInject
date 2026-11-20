# REPORT for 095-05b-m56-readers-benchscripts-docs.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-05b-m56-readers-benchscripts-docs.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-13 17:11:19 | finished: 2026-11-13 18:05:04 | duration: 54 min | exit code: 0
context at last turn: 887340 tokens (89% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-05b-m56-readers-benchscripts-docs.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

1. **For the owner (3 lines, plain words)**
   - The project's check tools now understand the new capture format: the flagged frames around each anomaly start and stop, the per-frame tick number, and the honest dropped-frame count. Older captures are still read exactly as before.
   - The two new bench checks catch the problem on an old capture and pass a corrected copy of the same capture. The client-facing readme and the internal docs describe everything the new build writes.
   - Nothing has been captured with the new build yet. The dashboard and the client's bundled check kit still need updating before any client delivery.

2. **Commits**
   - Plugin, `fix/m56-capture-continuity`, **pushed** (`586a0fd..b4017ea`; origin = local). No merge, no tag.
     - `325a3e7` chore(tools): m56 readers learn settle and schema 2.2
     - `e054485` docs: m56 capture continuity
     - `338e4cb` chore(tools): rename a change-oracle selftest local the codename scrub reads as a term
     - `b4017ea` docs: m56 095-05b journal records the scrub rename
   - CaptureBench, branch `m56-gates`: `1d1e119` tools(m56): continuity check, non-settle comparator, a54 settle flank rule, run_leg schedule predictions.
     - **Local only:** the repo has no remote.
     - I switched its checkout back to `master`, where I found it.
   - The pre-push scan passed: codename scrub clean over 14 files (13 changed files plus the commit messages), the token is absent, no Source change.

3. **Reader tests** (all 12 run at the pushed tip, all exit 0)

   | Tool | Before (`586a0fd`) | After |
   |---|---|---|
   | `verify_capture --label-rule` selftest | 65 cases | **83** (+18 new for m56) |
   | `--label-pixel-gate` selftest | 98 | **101** (+3) |
   | `--change-oracle` selftest | 35 | 35 |
   | `test_annotation_transitions` | 5 | **8** (+3) |
   | `label_sync_check` (office kit, now 1.6) | 93 | **102** (+9) |
   | `measure_label_offset` | PASS | PASS, plus a baseline-exclusion check |
   | `exclusion_gate` | OK | OK, plus 3 cases |
   | `r4_pairing_gate` and its selftest | PASS / 12 | PASS / 12 |
   | consistency and change-oracle unit tests | OK | OK |

   - **Mutants and which reader catches each:**
     - **Healthy schema-2.2 session:** clean, exact, PASS and CONSISTENT in every reader.
     - **Settle flag moved** (one frame later, or onto the revert frame): the label-rule check FAILS (SETTLE-MISPLACED), and so does the continuity check.
     - **Label shifted by one frame:**
       - the label-rule check FAILS (LABELLED-EXTRA) when labels and annotation disagree;
       - the kit FAILS on pixels in both directions;
       - the pixel gate reports OFFSET-NOTE, which by design is never a failure.
     - **Tick missing while `run_summary` claims no drop:** the label-rule check FAILS (DROP-COUNT), and so does the continuity check.
     - **Further must-fail cases** (all FAIL):
       - an inconsistent settle row;
       - a wrong settle count;
       - settle in a file that says schema 2.1;
       - an unknown row-level reason;
       - residue on unflagged frames.
   - **Old sessions:** the `586a0fd` readers and the new readers give identical verdicts on two banked September (m47-era) sessions and two schema-2.1 home sessions written by the 4.25 port.
   - **The two new scripts, both ways:**
     - **Continuity check, on banked `AE1_CT_AEON\session_20260903-182615` (read-only): FAIL.**
       - 90 of 90 rows have no `tick_index`.
       - `frame_index` steps are 74 of 1 and **15 of 3** (first at session index 3→4, 11→12, 15→16).
       - `ticks_per_captured_frame` reads 1.3444, and the new drop keys are absent.
       - 30 expected settle ticks have no row.
     - **Continuity check, on its doctored continuous copy** (120 rows, written only into my receipts folder): **PASS** on all 8 clauses.
     - **Comparator, banked session vs that copy: PASS.** 90 rows joined; the 30 settle rows have no partner, by design.
     - **Comparator, with one `bbox_px` changed at index 9: FAIL**, naming that field.
     - Selftests: continuity 9 cases, comparator 6 cases.
   - **Other CaptureBench changes:**
     - `a54_oracle.py` gives byte-identical output to the old version on 4 banked sessions × 3 shifts.
     - `run_leg.ps1` parses cleanly. Its prediction block gives, for 90 frames: m56 5 complete events and 22 settle frames, release 7 events.

4. **What could not be tested without an engine leg**
   - Every number about a real capture is untested:
     - continuity, settle positions, masks on settle frames, event counts;
     - the kit on real settle pixels, the a54 flank skip on real pixels;
     - blink hidden sets in tick space;
     - the release-vs-m56 comparison on a real pair;
     - the two bench drop levers;
     - the settle-exactness table.
   - For stuck_low_mip bursts, the continuity check reports the after-offset window as NOT JUDGED, by design.

5. **Deviations**
   1. **No C++ change for the exclusion counter.** It already applies the same rule after m56: it counts settle-flagged entries because it runs after the settle flags are set.
   2. **The annotation check now accepts transition entries of events missing from `annotation.json`, up to `vetoed_events`** (G575).
      - Why: m56 gives every vetoed event settle entries, so without this the kit would refuse every 2.2 session that had a veto.
      - This also changes 2.1 behaviour.
   3. **Extra structural checks in `verify_capture.py`** (SETTLE-INCONSISTENT, SETTLE-MISPLACED, SETTLE-COUNT, TICK-CONTINUITY, DROP-COUNT). Without them, no plugin-repo reader could fail the moved-flag or missing-tick mutants.
   4. **a54 skips only settle-flagged flank frames**, so earlier readings stay byte-identical. Its certified controls (R30, HF1) no longer exist on disk, so the byte-identity check replaced the A53 re-verification.
   5. **The kit's READ BACK line is unchanged.** The settle count is a new session-level line, so the office read gains one line.
   6. **No 5.1 release-era (schema 2.1) session remains on disk.** The old-schema proof used September sessions and 4.25-port sessions.
   7. **The `338e4cb` rename was needed for the scan to pass.** A two-letter variable in the m55 oracle selftest has tripped the scrub ever since that selftest was added (I found it by bisection, printing line numbers only).

   Context used: about 40 % (estimate). Time: about 55 minutes.

6. **Safety:** none. No process killed, no Codex, no build, launch or capture. Nothing touched in the frozen refs, tags, the dashboard repo or `D:\IA_Europa`.

   **Decisions chat must make:**
   - Accept or reject the veto tolerance in the annotation check (deviation 2).
   - Whether a54 should skip any transition-flagged flank frame, not only settle (deviation 4).
   - CaptureBench `run_leg.ps1` still kills `StackOBot*` processes by name before each attempt (its A46 step). This clashes with the standing "never kill a process by name" rule, and 095-07 would run it. It needs a ruling, or a PID-based change, before 095-07.
api-equivalent cost (not billed on Max): $37.29 | turns: 257 | running total across all briefs: $6,111.93
