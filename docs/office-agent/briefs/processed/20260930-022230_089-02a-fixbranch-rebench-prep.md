# ACK-VERDICT: 089-01 NEEDS-DECISION ruled in _reviews\089-01-chat-ruling-adjudication.md — aligned null provisional; OffFrames 16; MASK55 Route A; arms re-declared; LOD fixture light
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 089-02a fix-branch re-bench prep (Code, Opus 5.5)

# 089-02a — Fix-branch re-bench prep: E1 evaluator fixes (G395/G405/G406, game-time-aligned null), the LOD fixture made to render (light + cook), and the targeted re-bench leg set (m52 with a long aligned null and OffFrames 16; MASK55 Route A; the LOD fixture; blinking SYN), with Part 0 last. Dry run. NO game launch

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going. End the run only when every step is done, a stop condition fires, or something needs the owner. The final message follows the structure at the end and starts with a `VERDICT:` line. Commandlets and cooks outlive the tool timeout: run them in the background and poll. Keep context lean. Write docs with the file tools (G331).

**Nothing launches the game. Commandlets and cooks are allowed.**

## Bootstrap: read these first, then summarise in three lines
1. **`_reviews\089-01-chat-ruling-adjudication.md`: THE SPEC** (rulings 1–4 and 6).
2. `_reviews\089-01-adjudication.md` (G405, G406, B0's leg change, MASK55 Route A) and `_reviews\night-0930-chat-ruling.md` item 3 (the LOD fixture).
3. The 084-09 harness (`_reviews\084-09-*`, README first) and the 084-08b fixture journal.

## The work
1. **E1 fixes** in a new harness copy `_reviews\089-03-*`, built from 084-09; 084-09 stays as banked:
   - G395: a judged frame never sits inside its own reference;
   - G405: a one-frame label run is not scaled by its own post-label noise;
   - G406: **the null is aligned by game time, frame by frame**, and a leg outside the null's time range is UNJUDGEABLE, never judged.

   Prove each both ways on tonight's bank: the known answers still FAIL, and tonight's adjudicated events now read as classified. **Freeze and hash the evaluator before any leg runs.**
2. **The LOD fixture renders** (a content change):
   - add a directional light + sky light (or whatever makes the level render in the packaged build) to `CB_LodFixture` by commandlet;
   - the manifest diff shows only the fixture;
   - re-cook the fix branch (exe `8A6074AA`, unchanged);
   - archive the new pairing and prove the descriptor;
   - **prove the level is not black offline** if possible (a commandlet scene-capture render or equivalent); otherwise state it as the first premise check.
3. **The re-bench leg set** (on the new pairing):
   - m52 B0 / B3 / B9 / B0L / P1 with `IAI.Label.TransitionOffFrames 16` on the AA-on legs;
   - a **null of ≥ 1,500 frames placed immediately after the m52 legs**, covering every leg's time range;
   - MASK55 Route A (validity: peak live ≥ 2, recycles ≥ 1);
   - the LOD fixture premise + gate (both variants, AA on/off, both orders, ≥ 16 judged);
   - blinking SYN (both evaluators);
   - B-CC AA-off with a null that has signal, if cheap.

   **Part 0 last** (and never before a leg). The kit runs as the third reading. Declare the predictions per leg. Give the expected minutes (target ≤ 100).
4. **Dry run:** exit 0, every launch accepted.
5. **Docs:** the harness README, the journal `docs/sessions/2026-09-30-089-02a-rebench-prep.md`, and gotchas. Commit and push on `fix/m52-label-timing`. No tag, no merge.

## Hard rules
- No game launch, no staging outside the dry run's sandbox, nothing that takes focus.
- The project content changes only by the LOD fixture's files (manifest diff).
- Never kill anything that is not yours, and never by name.
- Don't touch `m51`, `master`, tags, `ToCodex\`, `E:\<personal-folder>`, or the m53 branch.
- Codenames only.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner**: 2 lines.
3. E1 fixes: the proofs both ways, and the frozen hash.
4. The LOD fixture: what changed, the cook hashes, and whether it is proven not black.
5. The leg list, predictions, and minutes; the dry run.
6. Commits pushed.
7. NEEDS-DECISION items, or "none".
