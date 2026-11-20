# Request to Codex (via the GDP Relay): independent evidence review of m53 S1 (chat Claude, 2026-09-28)

**Role:** adversarial evidence reviewer.
- Read-only. Write the report to `review.md`; it will be copied to `_reviews/082-08-codex-m53-s1-evidence-review.md`.
- Findings are P1 / P2 / P3. Each finding gives:
  - a file:line or an evidence path;
  - a concrete failure scenario.
- End with a `VERDICT:` line: APPROVE / APPROVE-WITH-FIXES / CHANGES REQUIRED.

**Context**
- m53 adds two texture-corruption anomalies, `uv_corruption` and `normal_corruption`, using route B: the host material is kept, and a MID binds corrupted per-mip copies of the host's own texture parameters.
- You approved the S1 source in 082-06c.
- S1 ran 193 gate legs over five runs: 082-07b, 07e, 07h, 07j and 07l.
- Along the way the harness was amended four times, after premise defects surfaced on the real bench: the fixture, the time axis, the focus axis and runtime conditions. Chat ruled each one.
- Chat has declared S1 complete. Your job is to test whether that is earned.

**Inputs** (read from git objects and disk)
- **Repo `feat/m53-uv-normal-corruption` at `2ca9faf`:**
  - plan `docs/predictions/2026-09-26-m53-uv-normal-corruption.md` (§R11, §R13.1, §R14);
  - S1 predictions `docs/predictions/2026-09-27-m53-s1-legs.md` with AMENDMENTS 1–4;
  - journals `docs/sessions/2026-09-2*-082-07*`; the complete S1 ledger is in `2026-09-28-082-07l-code-m53-s1-final-legs.md`;
  - gotchas G318–G336;
  - source `Source/**/TexCorrupt*`, `Anomaly_TexCorrupt.cpp` and the capture subsystem.
- **Harness, in `D:\IntrusiveAnomalies\_reviews\`:**
  - `082-07-lib.py`, `082-07-legs.py`, `082-07-common.py`, `082-07-window.py`, `082-07-run.ps1`, `082-07-preflight.py`, `082-07-postflight.py`, `082-07-mw-trajectory.json`;
  - boundary files and `supersede.json` in `082-07-evidence\`;
  - per-brief evidence dirs `082-07c-evidence\` … `082-07l-evidence\`.
- **Chat rulings:** `_reviews\082-07b-*` … `082-07l-chat-ruling-*.md`.
- **Bank:** `D:\IntrusiveAnomalies\_bench_sessions_bank\M53S1_*`: frames, `labels.jsonl`, logs. Large, so sample.

**Questions**
1. **Independent recomputation.** Recompute from the bank's raw frames and logs, **not through `082-07-lib.py`**, at least these, and compare with the recorded verdicts:
   - G-ID identity and wrong-copy minimum for U-C2 (alpha), N-N1 (normal) and MASKED (with PRE-CLIP);
   - one G-ID-M row;
   - G3 for two rows;
   - G-BIND (3) with its ring and far field;
   - one G-REASON row;
   - one G4 FailStep leg's rollback ledger.
2. **Amendments: correction or moved goalpost?** For each post-hoc change, say whether it is an evidence-backed correction of the encoding or a weakening. The changes:
   - G3 applied vs null at the same frame;
   - REASON A2 demoted;
   - the G-BIND ring split (R = 42, < 32);
   - the global half at < 32;
   - the half locator;
   - G-RD compared within the same phase;
   - the F-MW partial-yield rule;
   - pinning the MainWorld camera path;
   - the GC detector switch;
   - TWO's refusal disposition;
   - STOP-BEFORE-FIRE;
   - TRIP reading `run_summary.json`;
   - the T10 and CAN dispositions;
   - chat's P2-1/P2-3 scoping (ruling `082-07l`).

   For the whole set, name the **cheapest real feature defect that would now pass S1**.
3. **Coverage.**
   - What does S1 actually establish for route B: identity, fault visibility, restore, refusals, rollback, watches, collateral?
   - What does it not establish? The declared S2 debts are T10 live, `no_normal_map`, LEVEL-CHANGE, RECREATED and cancel-before-focus. Add any hole that isn't listed.
4. **Labels.**
   - **After `TARGET LOST`:** on the destroy legs, rows si 16–22 keep `anomaly_present: true` with no texcorrupt block. Do they carry `observable: false`? Is this honest, a capture-layer defect, or an m53 defect?
   - **Partial slots:** on the MainWorld floor, the first fire applies to 12 of 30 slots. Does the label say which slots were corrupted? Is the event honest as labelled?
5. **Integrity.**
   - Is the chain of boundaries, locked hashes, supersede records and bank aliases consistent?
   - Is there any sign of fishing, meaning re-running until a pass? Check the attempt counts against the ledger.
6. **Verdict.** Is S1 qualified as declared? Give the exact blockers, if any.

Put depth where the risk is: questions 1, 2 and 4.
