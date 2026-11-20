# REPORT for 082-13-m53-s2-rev21-minidelta.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-13-m53-s2-rev21-minidelta.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 10:04:12 | finished: 2026-09-28 10:42:11 | duration: 38 min | exit code: 0
context at last turn: 624912 tokens (62% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-13-m53-s2-rev21-minidelta.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Plain summary for the owner:** The S2 plan is revised to fix everything chat accepted from Codex's last review, and pushed. Codex's follow-up review says S2-0 still can't run as written: two spec corrections are needed first, and no new build.

**D1–D6 and the noise ruling, as applied** (plan `docs/predictions/2026-09-28-m53-s2.md`, new section "REVISION 2.1"):
- **D1 (R21.1, §3A.2/§3A.5):** family A keeps the banked schedule (warm on, apply at frame 42). Every family-B leg now runs warm off with lead-in 62 (apply 62), so C and the m53 arms share one clock. Each family has a full si→frame map, and eight timing checks (T1–T8) run on every leg. The first leg of each type stops the window on a mismatch.
- **D2 (R21.2, §3A.5):** every leg must show exactly one `Auto.FireSpecific … -> applied`. N2 and R need the NoApply-2 marker and zero APPLIED lines. N1 needs its marker plus `APPLIED noapply=1`.
- **D3 (R21.3, R2.1, §6.2):** the drift mode-oracle leg now runs `2 16 100 4 4` (432 frames, four complete events) with its own null. G-MODE-UV goes from 6 to 7 legs; the event floor is unchanged for every mode.
- **D4 (R21.4, R2.2/R2.6/§2.6/§6.2):** a post-GC render-target census judges retention. A separate new fault lever unbalances the ledger; it had to bypass `Close`, which would otherwise re-release the bytes (G339). G-SNAP-FAIL goes to 4 legs, each negative firing only its own clause.
- **D5 (R21.5, R2.7):** binding rows carry `association` and `layer_index`, keep the full texture path, and are compared by key. An association-swap negative on `BIND_ID_A1` must FAIL.
- **D6 (R21.6, §3A.7/§3A.8):** a direct C-vs-ID-F trace reading is added. The swap flag is descriptive only. The plugin-wide class (now PLUGIN-WIDE-RESTORE-DEFECT) needs the same trace plus defective post-revert state on both arms; otherwise the reading is "CAUSE UNRESOLVED".
- **Noise (R21.7, §6.0/§6.2/§11):** τ is measured on a new identity leg in the premise window, fixed at the lock (τ_id plus an offline arithmetic bound), and a FAIL after it. If no sound τ exists, chat rules before the lock. The "≤ 8, open debt" rule is withdrawn.

**S2-0 runnability check (R21.8), on the staged S1 build:**
- **Staged set:** hashes match (`2FCDF059` / `20DA6F98` / `534C5863`) and nothing was running.
- **Commands, levers and cvars:** every one S2-0 uses is in the exe, once each in UTF-16 (`IAI.Capture.TexCorruptWarmDraw`, `DEFER`, `Reattach.Components`, `r.Shadow.Virtual.Cache`, all BASE commands).
- **Log tokens:** every token the evaluator reads is present.
- **Absent controls:** five S2-only lever names read 0 in both encodings, so the scan can see an absence.
- **Command line:** the longest of the 24 generated batches is 3,106 characters against the engine's 16,384 limit.
- **Predicates, row by row:** each is satisfiable by a correct run. Two mechanisms have never run on this bench (nested `DEFER`, and the warm-off m53 schedule), so the first leg of each type validates them.
- My answer was yes, runnable with no new binary; Codex disagrees on two spec points (below).

**Commit:** `9d622e0` on `feat/m53-uv-normal-corruption` (plan, journal 082-13, gotchas G338/G339, status block), pushed `82c0ebd..9d622e0`. `m51`, `master` and tags are untouched; no merge. The token scanner and batch generator are in `E:\IA_BuildCache\_r82_13_scratch\`, not committed.

**Codex mini-delta** (`_reviews\082-13-codex-m53-s2-minidelta.md`, 545 s, gpt-6-astra/max, not acted on):
- VERDICT: CHANGES REQUIRED. D1–D5, P2-2 and P2-7 RESOLVED; D6, P2-1 and P2-3 PARTIAL; no P1.
- S2-0 runnable as specified: **NO**. Blockers are M1 and M2; they need spec or evaluator corrections, not a new binary. Codex independently re-hashed the staged set and reproduced the 24 batches.
- **P2-M1:** a `left-to-game` or `unresolved` count alone can wrongly yield PLUGIN-WIDE-RESTORE-DEFECT; it should require an actual Effective-material mismatch on both arms.
- **P2-M2:** the required bank proof of the timing checks can't pass. The bank logs have no census or cache-query lines, and the warm-free bank leg's first fire was "not applied".
- **P2-M3:** R21.3's drift offsets count saved frames; engine-frame counting gives k = 1, 4, 5 … 102, ending at 0.85 UV.
- **P3-M4:** an unstruck "≤ 8" remains in §6.2's G-MODE-N rule column.

**NEEDS-DECISION:** chat's disposition of M1–M4. Per Codex, M1 and M2 block S2-0; M3 and M4 affect later S2 gates. S2-0 still needs the owner's OK to use the PC.
api-equivalent cost (not billed on Max): $17.75 | turns: 141 | running total across all briefs: $2,963.25
