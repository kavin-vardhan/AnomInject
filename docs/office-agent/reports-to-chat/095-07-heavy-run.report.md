# REPORT for 095-07-heavy-run.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-07-heavy-run.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-14 14:25:16 | finished: 2026-11-14 14:47:53 | duration: 23 min | exit code: 0
context at last turn: 469012 tokens (47% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-07-heavy-run.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**The run never started.** The memory check in pre-flight failed. Nothing was built, cooked or captured, so there are no clips.

1. **For the owner (plain words):**
   - The test run did not start. The PC had about 8.5 GB of memory free, and the brief needs 12 GB before a build or cook may begin.
   - Your own Blender renders were running the whole time: two loops (LawnChairShort and StretchCloneShort) start a new headless Blender about once a minute. I did not touch them.
   - I waited 20 minutes. Free memory stayed between 6.6 and 9.0 GB.
   - No clips exist yet. Frame skipping is not measured yet. Nothing failed, because nothing ran.

2. **The clips:** none were made.

3. **Gate table:** no gate or leg ran.

| Pre-flight check | Reading | Result |
|---|---|---|
| Tips equal origin | integ `0c71f6f`, m57 `d8d3a7e`, m58 `5e1bc03` all equal origin; CaptureBench `m57-gates` `4ca8524` (local) | PASS |
| Primary checkout clean | on `integ/m56-m58`, no tracked changes | PASS |
| E: ≥ 85 GiB, D: ≥ 20 GiB | E: 116.5 GiB, D: 69.1 GiB | PASS |
| Free memory ≥ 12 GiB | 8.10 GiB at 14:25; 6.62–9.01 GiB over a 20-minute wait (one reading a minute) | **FAIL** |
| No Unreal, build or StackOBot process | none | PASS |

4. **Settle-exactness table:** not measured, so nothing can be said yet about K = 0.

5. **Exes and cook:**
   - None built. No cook exists.
   - The host binaries are as 095-14 left them: the plugin Editor DLLs match `integ/m56-m58` `0c71f6f`, and the Game exe is still the 24 Sep build.
   - The fixture worktree was not created.

6. **Faults found:** none. Bench-script fixes: none.

7. **Disk and memory:**
   - Start (14:25): E: 116.49 GiB, D: 69.07 GiB, memory 8.10 GiB free of 23.6 GiB.
   - End (14:47): E: 115.42 GiB, D: 69.07 GiB, memory 8.55 GiB free.
   - Besides Blender, other large memory users at the start were 16 Claude processes (4.5 GiB), the WSL/VM process (4.0 GiB) and Chrome (3.7 GiB).
   - Receipts are in `code-receipts-095-07\` (`preflight-095-07.txt`, `preflight-memwait.log`).
   - Context used: about 15 %.

8. **Safety:** none. No process was touched, nothing was deleted, and no repo, ref or setting was changed.

**Decisions chat must make:**
- **D1. When to run 095-07.** It needs the owner's Blender render loops stopped, or finished, and free memory back at 12 GiB or more. Nothing needs re-doing before the next attempt.
- **D2. Whether to lower the memory floor for this run.** At about 8.5 GiB free the builds and the cook would still run, but slowly: the build tool limits itself to about one compile at a time when memory is short, and that already happened here once with a foreign editor open. The Blender loops also use memory unpredictably, which raises the risk of stopping partway through on the brief's own rule (under 2 GiB free for five minutes).

My recommendation is to keep the 12 GiB floor and re-send 095-07 once the owner confirms the renders are done.
api-equivalent cost (not billed on Max): $6.45 | turns: 30 | running total across all briefs: $6,465.00
