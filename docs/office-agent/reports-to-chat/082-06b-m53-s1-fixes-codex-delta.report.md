# REPORT for 082-06b-m53-s1-fixes-codex-delta.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-06b-m53-s1-fixes-codex-delta.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-27 03:57:30 | finished: 2026-09-27 04:38:46 | duration: 41 min | exit code: 0
context at last turn: 642685 tokens (64% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-06b-m53-s1-fixes-codex-delta.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Summary for the owner**
- I fixed all nine review findings, both builds are clean, and the offline tests pass (151/151). A deliberately broken copy of the tested code fails 24 of them, so the tests can catch these faults.
- Codex's check found two of the nine only partly fixed and says the code is **not ready** for authoring and cook. I recorded that and changed nothing; chat needs to rule.
- Nothing was staged, cooked or tagged. The bench still runs `E0BE6F0A`.

**1. Fix table** (all on `feat/m53-uv-normal-corruption`, pushed `193bd35..40aa07a`)

| Finding | Commit | Proof: expected vs actual |
|---|---|---|
| (shared pure code, used by every fix) | `127b7e1` | the 83 original checks still pass (the pure header keeps the result) |
| P2-1 NoApply bypassed the condition test | `034564a` | one test with no NoApply input: applied reads `installed`, NoApply 1 `slot_not_installed`, NoApply 2 `no_expected_set` — all as expected (harness group 12) |
| P2-2 collateral check limited to injection candidates | `f57004d` | every primitive drawn on screen, with no injection filter: a 2 % prop, a far backdrop, foliage and glass are measured; target, shadow-only and unregistered are not — as expected (group 14). Incompleteness is counted and never reported as clean. |
| P2-3 subtype read from live state | `1067481` | subtype now comes from each frame's captured record; `GetLiveModeName` is deleted. Proof is the source path; no offline test is possible. |
| P2-4 only one material-usage flag checked | `c297ee2` | 18 offline rows match the engine rule, including Codex's instanced + lightmapped case (`ism+static_lighting`); anything undetermined is refused. Deviation 5 withdrawn. |
| P2-5 partial scratch failure undercounted | `dbc7033` | 129 rollback sequences (a failure at every allocation step, before and after creating the resource): expected 0 balance violations, actual 0. New lever `FailStep 2 <ordinal>`. |
| P2-6 NoApply 2 skipped target watches | `034564a` | the watch is now registered on that path too. Deviation 10 withdrawn. |
| P3-1 post-revert sample timed in ticks | `f57004d` | sample now at revert frame + 2 by frame counter, and it logs whether it landed on that frame |
| P3-2 chain walk accepted an unchecked tail | `c297ee2` | 16 links reads clean; 17 links and a cycle read `limit_reached`, refused as `host_mid` |
| P3-3 peak used to prove balance | `f57004d`, `1d7cada` | a frame-stamped ledger line after every revert; plan §R9.4 corrected |

- **Extra commit `9382173`:** fixes two compile errors of mine that the first editor build caught (a variable shadowing a class member, and a dropped local).
- **Harness (`accdb9f`):** 151 checks, 0 failures on the real header. A scripted mutant with 8 faults fails 24 checks and fails every new group (9–14).
- **Plan (`1d7cada`):** revision 3.2 adds a resolution table with one row per finding, line references and proofs.
- **My own test error:** I first wrote the expected sequence count as 81; the correct figure is 129. I corrected the test, not the code.
- **Still to be shown in 082-07's legs:** the in-engine proofs for P2-1, P2-2, P2-3, P2-5 and P2-6. This brief allowed no legs; journal §4 item 7 lists them.

**2. G10 and archived binaries**
- Built on the warm host at `1d7cada`, with the plugin's own build outputs deleted first (G309) so every module compiled from scratch.

| Target | Actions | Exit | Time | Warnings |
|---|---|---|---|---|
| Editor | 16 | 0 | 43 s | 0 |
| Game | 7 | 0 | 74 s | 0 |

- **Modules:** no `Build.cs` or `.uplugin` change since `193bd35`. `AnomalyInjector` links exactly Core, CoreUObject, Engine, Foliage, InputCore, RenderCore and RHI.
- **A44:** every new token is present in the new exe, and the existing controls read 7 and 5, as before.
- **Comment strip:** 0 files changed.
- **Archived (not staged), hashes verified at the destination:**
  - game exe `8BF054FA` as `_binary_baselines\StackOBot.exe.m53-s1fix-8BF054FA`;
  - editor DLLs as `m53-s1fix-editor-1d7cada\`.

**3. Codex delta**
- Astra at effort max (requested and actual agree), 649 s, exit 0. Collected verbatim to `_reviews\082-06b-codex-m53-s1-fix-delta.md`, ledger row added.
- **`VERDICT: CHANGES REQUIRED`**
- **Resolved:** P2-1, P2-3, P2-4, P2-5, P2-6, P3-1, P3-2.
- **Partial P2-2:** a drawn mesh with an empty material slot renders the engine's default material, but my collector skips it. Texture entries it cannot resolve are not counted either. Either way the collateral set can read "complete" when it isn't.
- **Partial P3-3, plus one new P3:** the corrected plan text asks for pending bytes = 0 on the rollback line, but a correct rollback shows them > 0 there. Codex proposes a frame-stamped rollback reading two frames later.
- **New P1 or P2:** none.
- **Ready for authoring, cook and legs?** **No.** The blocker is the P2-2 remainder, plus the P3 rollback criterion.

**4. NEEDS-DECISION**
1. Chat's ruling on the delta: the P2-2 remainder and the P3-3/new P3 rollback wording. 082-06 (authoring and cook) waits for it.

Everything else is unchanged: `m51` is still at `53bf725`, `master` at `b5f15a3`, no tags, and the host tree is clean at `1d7cada` and kept.
api-equivalent cost (not billed on Max): $21.32 | turns: 182 | running total across all briefs: $2,586.89
