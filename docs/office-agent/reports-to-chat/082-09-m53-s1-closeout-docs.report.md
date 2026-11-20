# REPORT for 082-09-m53-s1-closeout-docs.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-09-m53-s1-closeout-docs.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-28 07:42:47 | finished: 2026-09-28 07:46:08 | duration: 3 min | exit code: 0
context at last turn: 455768 tokens (46% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-09-m53-s1-closeout-docs.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**2. Plain summary for the owner**
- Stage 1 of the texture-corruption feature is formally closed as a *scoped* result. The record now says exactly what was proven, what was not, and which test rules were loosened along the way.
- Everything Codex's review found has a home: one label bug goes to m51, the rest go to Stage 2 or Stage 3.
- Stage 1 authorises neither turning the feature on by default nor a release. Next is planning Stage 2.

**3. What was written** — commit `2a8e350` on `feat/m53-uv-normal-corruption`, pushed. Docs only; no tag, no merge. `m51` is still `53bf725` and `master` `b5f15a3`.
- **"S1 RESULT" section** appended to `docs/predictions/2026-09-27-m53-s1-legs.md`, after AMENDMENT 4; nothing above it was edited:
  - **R1, established**, with the corrected counts: 43 F-SYN pixel-plus-log restore rows + 1 MainWorld log-only row; TRIP 191 measured zero + 2 census N/A; G-REASON 19, including A1's intended APPLY exception.
  - **R2, not established.**
  - **R3, the amendment table**, one line each citing Codex's assessment:
    - corrections: G3, the fixture repairs, the half locator, G-RD, the pinned trajectory, the GC switch, TRIP's source;
    - coverage deferrals: T10, CAN, A2;
    - declared weakenings: the G-BIND ring and global half < 32, F-MW partial yield, and TWO's refusal waiver, which never fired.
  - **R4, the P2-1/P2-3 scoping note**, with Codex's presence scan (9,657 entries, 21 omissions, all after the target was destroyed) and the G-BIND per-frame reading (1,192 px on 57/57 frames). These discharge Codex's P2-2 and P2-4 for this bank.
  - **R5, the findings disposition**; **R6, the debt list**; **R7**, "S1 authorises neither default-on nor a release."
- **Journal** `docs/sessions/2026-09-28-082-09-code-m53-s1-close-out.md`. It includes the P3-1 correction to the 082-07l record's headline counts.
- **Status block:** "m53 S1 COMPLETE (scoped); Codex evidence review APPROVE-WITH-FIXES, dispositioned; next: the S2 plan".
- **Gotcha G337:** headline counts must say measured vs log-only vs N/A.

**4. The consolidated S2/S3 debt list (S1 RESULT R6, as written)**

*The five declared debts (S2):*
1. T10: a live `not_fully_resident` refusal — 082-07h ruling, A3
2. A Q `no_normal_map` refusal producer — 082-07d ruling, A1, corrected in A3
3. G4 LEVEL-CHANGE, which needs a bench lever — S1 plan, declared UNEXERCISED
4. G4 RECREATED, which needs a bench lever — S1 plan, declared UNEXERCISED
5. Cancel-before-focus, which needs a "treat as unfocused" lever — 082-07j ruling, A4

*S2 harness:*

6. G-BIND per-frame (minimum) layer check, reporting the failing si — Codex P2-2
7. The global-half far strip (> 16 px from the layer half) as a gate at ≤ 2 — Codex's cheapest escape
8. TRIP requires the counter key on summary-bearing legs; census legs are N/A — Codex P2-4
9. An active-lifetime block-presence assertion — Codex's block-scope condition

*S2 source:*

10. Partial-application telemetry: positive component/slot identities and the texture-to-slot association — Codex P2-3

*Outside S2:*

11. m51/M3: a known termination gives `observable: false` with `target_pixels: -1` — Codex P2-1

*S3:*

12. A MainWorld arbiter for a nanite (mask-unmeasurable) target — Codex P3-1, 082-07l ruling 4
13. The owner's budget call: the floor needs 209.4 MiB (uv) / 192 MiB (normal) against the 128 MiB cap; decide using G7's cost numbers at 64/128/256 MiB — 082-07l ruling 3

*Cause-read list:*

14. The halo — 082-07f, 082-07g
15. The start-up element — 082-07d
16. TC_Layer's global-half gradient — 082-07g
17. P5a, 10 of 991 differing — 082-07i, A3.7
18. `T_TC_Stream2k`'s full chain — 082-07h
19. The 51.7 s input — 082-07k
20. The MainWorld control-pair nondeterminism (whole-frame max 254) — 082-07l ruling 4

*Housekeeping:*

21. Person detector: persist input timestamps and excuse the runner's own last keystroke — 082-07k, G335
22. The stale `ARCHIVE` constant in `082-07-common.py` (it still names `…30FE0FDE`) — 082-07g ruling

*Codex's further holes, for S2 planning to place (not individually ruled):*
- clean collateral completeness;
- comparing the installed slot sets across host twins;
- resource and renderer lifetime evidence beyond the feature's own accounting;
- dedicated Q producers for reasons that have none.

**5. NEEDS-DECISION:** none.
api-equivalent cost (not billed on Max): $9.72 | turns: 13 | running total across all briefs: $2,850.78
