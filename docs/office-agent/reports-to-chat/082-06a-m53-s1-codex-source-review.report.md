# REPORT for 082-06a-m53-s1-codex-source-review.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-06a-m53-s1-codex-source-review.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: medium
started: 2026-09-27 03:26:49 | finished: 2026-09-27 03:54:00 | duration: 27 min | exit code: 0
context at last turn: 325819 tokens (33% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-06a-m53-s1-codex-source-review.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Summary for the owner:** Codex finished its independent review of the m53 S1 source and the report is collected. Its verdict is **CHANGES REQUIRED**. That comes from six P2 findings and no P1. Codex says the N1(a) draw-and-copy design holds up in the engine source, so this is not a rejection of the approach. None of the findings has been acted on; they wait for chat's rulings.

**Run facts**
- **Model and effort:** `gpt-6-astra` / `max`. The runner passed both explicitly, and `run-meta.txt` shows the same values in Codex's own session record. Sandbox was `workspace-write` with no network access.
- **Duration:** 1,526 s (about 25 min), 03:27:58 → 03:53:24, exit 0.
- **Review's last line:** `VERDICT: CHANGES REQUIRED`
- **Collected to:** `D:\IntrusiveAnomalies\_reviews\082-06-codex-m53-s1-source-review.md`. It has one header line added; the rest is byte-identical to `review.md`, which I checked.
- **Ledger:** row `082-06-m53-s1-source-review` added.
- **Prompt:** the request verbatim, then the relay block, with nothing of mine added. Codex read the branch through git objects at `193bd35`. The live checkout stayed on `m51` and the plugin repo was not touched.

**Findings (as Codex reports them; no rulings made here)**
- **P2-1:** With the NoApply lever on, the condition check (`IsVisualConditionHeld`) returns false straight away. So the NoApply control never tests the binding measurement it is meant to validate. `Anomaly_TexCorrupt.cpp:1051`
- **P2-2:** G-COLL collects its set from the injection candidates, which applies the poll radius, the 6% coverage floor and the foliage, translucent and pattern exclusions. Visible collateral outside that set is never measured, and it is not reported as incomplete. `Anomaly_TexCorrupt.cpp:342–378`
- **P2-3:** The annotation subtype is read from the live mode when the frame completes, not from the captured event. After a revert it comes out empty, and a later event of the same id can put the wrong mode on it. `AnomalyCaptureSubsystem.cpp:6022`
- **P2-4:** The default-material guard stops at the first usage flag that applies. Instanced and Nanite meshes are never checked for static lighting, and only LOD0 lighting data is looked at. The engine can still swap in the default material on an admitted slot. `TexCorruptTree.cpp:140–175`. Codex also judges declared deviation 5 not acceptable for this reason.
- **P2-5:** If a scratch set fails partway through allocation, the ledger undercounts on rollback. The scratch levels already created are released without passing through the two-frame pending ledger, so the budget cap can be exceeded. `Anomaly_TexCorrupt.cpp:649–666`
- **P2-6:** NoApply 2 returns early and never registers the target watches. A weak pointer does not see an `EndPlay` (removed from world) while the object is still alive. `Anomaly_TexCorrupt.cpp:595–604`. Codex also judges declared deviation 10 not acceptable for this reason.
- **P3-1:** The post-revert sample is counted in ticks rather than frames, so it lands 1 or 2 frames after revert depending on tick order.
- **P3-2:** The Δ1 chain walk stops at 16 links and accepts whatever lies beyond as clean, instead of refusing it.
- **P3-3:** The plan expects `texcorrupt_rt_bytes_peak` to return to zero after each event. It is a run peak, so it never does; balance has to be judged from the live and pending bytes instead.
- **The ten declared deviations:** Codex accepts 8. It rejects #5 (P2-4) and #10 (P2-6).
- **Offline harness:** Codex agrees `TexCorruptPure.h` is genuinely shared with the plugin. It adds that the 83 checks do not execute the tree order, the ledger, rollback or render commands, so they miss P2-5.

**NEEDS-DECISION:** none for this brief. Chat still has to rule on all nine findings.
api-equivalent cost (not billed on Max): $3.47 | turns: 15 | running total across all briefs: $2,565.57
