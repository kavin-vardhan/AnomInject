# ACK-VERDICT: 090-10e and 090-10f both RED only because Claude Code's login expired ("OAuth session expired") — 090-10e's work is committed locally (fix 257ff68, m53 9b911e7) but not wrapped up/pushed; 090-10f never started. This brief = finish 090-10e (Part 0) + mask-unmeasured ruling (Part M) + all of 090-10f. Rulings: _reviews\090-10c-chat-ruling-r1.md, _reviews\090-10b2-chat-ruling.md.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 090-10f2 finish 090-10e + PIE unmeasured-mask frames + uniform-texture refusal + B scope + cooked bias (Code, Opus 5.5)

# 090-10f2 — (0) finish 090-10e, which lost its login before wrap-up; (M) labelled PIE frames without a measured mask; (1) MUST-FIX: never label a UV/normal event whose texture can't show the effect (11 of 28 auto events on uniform 16×16 controls were labelled with no visible change); (2) prove B's component-scope mask in-engine on a static two-component fixture; (3) prove per-texture, group and cinematic bias in a cooked, staged build, and make the bias lever refuse in packaged builds. PIE and staged proof

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going. End the run only when every step is done, a stop condition fires, or something needs the owner. The final message follows the structure at the end and starts with a `VERDICT:` line. Builds, cooks and game runs outlive the tool timeout: run them in the background and poll. **Keep context lean (finish below 70 %).**

**Launches only when the idle gate passes** (090-10c's clarified rule: an open but idle foreign editor doesn't block when the person has been idle ≥ 15 min, GPU < 10 %, headroom ≥ 6 GB; never touch it).

## Bootstrap
0. **090-10e's live feed** `_mailbox\to_chat\090-10e-pie-endsettle-census-slice.live.txt`: its results and where it stopped. **Its session died on an expired login, not a failure.**
1. `_reviews\090-10c-chat-ruling-r1.md`: **THE SPEC** (items 1–3).
2. The 090-10c report and journal: the design as built, the proof tables, G445–G449.
3. Heads to build on: fix `257ff68`, m53 `9b911e7` (local, from 090-10e; verify, then push in Part 0).

## The work
**Part 0: finish 090-10e** (local, unpushed: fix `257ff68`, m53 `9b911e7`).
- Verify that what the live feed shows is committed. From the feed:
  - 12 A legs: 84 events in sync, with flags at e+1;
  - census timing: MainWorld PIE 27.6 ms, 15k 37.5 ms, 40k 50.8 ms (bound 100 ms);
  - Python 19/19 and 27/27; lever audit 52/52.
- Finish the missing steps: strict 0/0 on both, the string scan, archive, the journal `docs/sessions/2026-10-02-090-10e-pie-endsettle-census-slice.md`, status blocks, gotchas (G450+). Then **push.**
- **Record 090-10e's key finding:** the "PIE end+1" was a reader artefact. `affected_frames` drops a last labelled frame whose PIE mask was unmeasured; against the labelled set, 0 of 147 events are off.
- **Keep `pie_end_settle`** (it only flags an already-unlabelled frame; harmless and conservative) but correct its documentation to say why it exists.

**Part M: labelled PIE frames with no measured mask (release rule: labels AND masks).**
- 090-10e found labelled frames whose target mask was **unmeasured**, clustered near the start of PIE legs (warm-up).
- Establish:
  - the cause;
  - whether staged ever does it;
  - what the delivered data shows on such a frame (mask PNG missing? empty? `mask_state`?).
- **Ruling:** a labelled frame must never ship without its mask.
  - Fix the cause if it's a warm-up or arming issue, **or** flag such frames (`transition_reason: mask_unmeasured`: unlabelled, dropped, counted) so the label and the mask always agree.
  - Tests both ways. **PIE proof:** 0 labelled frames without a measured mask across the A-recipe legs.

**1. `texture_uniform` (rule 1; MUST-FIX).**
- Refuse a slot or binding whose required resident source can't show the requested family's effect: uv means no spatial variation within a declared tolerance; normal means a flat normal within a declared tolerance.
- Measure once per texture (keyed by texture and resident mip), cache it, keep it cheap: a GPU min/max/variance reduction of the resident top mip, or an equivalent with no game-thread readback stall. Declare the tolerance and justify it.
- B applies: the refused slot is skipped, and the event proceeds if other components qualify.
- **Tests both ways:**
  - pure logic, plus a mutant that ignores it;
  - **in PIE, the auto-pool on the office-like fixture: every labelled uv/normal event is visibly corrupt** (pixel rule, as 090-10c);
  - the 16×16 `T_TC_Chain*` controls are refused as `texture_uniform`;
  - real textures are still admitted (eligibility on the office-like fixture is reported before/after; any drop is explained);
  - the can-fail is `88801bd` (11 invisible).
- **Report any other "labelled but invisible" class** the auto-pool surfaces, and handle it the same way.

**2. B's component-scope mask, in-engine.**
- Author a **static, non-animated** fixture actor with two mesh components: one eligible, one ineligible (for example `texture_not_parameter`, or translucent).
- **In PIE:** uv and normal ≥ 7 fires each.
  - **Pixel check:** only the eligible component's pixels change.
  - **The mask, `bbox_px` and the m26 measure cover only that component** (compare the mask pixels to that component's own pixel footprint).
  - Edges are 0 off (outside flagged frames).
  - The can-fail is the `NoPartScope` lever (the mask covers the whole actor).

**3. Cooked-bias staged proof.**
- Add fixture textures with `LODBias` baked in and with cinematic mips, plus a **cooked** device-profile texture-group bias. Cook and stage, using the existing staging and receipt routine.
- **Staged:** V1, V2 and V3 are each 7/7 corrupt with identity exact and edges 7/7; the can-fail is `0a69ea3` / `65607703` refusing.
- **Make the bias bench lever refuse in packaged builds** (G449) and fix its help text.
- **If dbghelp blocks the cook or stage,** report exactly what failed and stop this item; never work around security.

## Then
- Both branches as applicable;
- builds;
- the string scan;
- strict 0/0;
- the lever audit;
- archive.

Commit and push; **the m53 head you push is the one that passed 1–3 in-engine.** Docs: the journal `docs/sessions/2026-10-02-090-10f2-uniform-bscope-cooked.md`, status blocks, gotchas, and the readme (`texture_uniform` reason; B mask semantics).

## Hard rules
- **Never change any security setting.**
- Never kill anything that is not yours, and never by name. Close only what you launched, by PID.
- Delete nothing, except scratch files you created, by receipt. Bank to `D:\IA_BankOverflow\`.
- Don't touch `m51`, `master`, tags, `ToCodex\`, `E:\<personal-folder>`, or the frozen `_reviews\090-06-*`.
- **Codenames only.**

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner**: 3 lines, with **the exact commit to pull**.
3. Part 0: what was finished and pushed (heads). Part M: the cause, the fix or flag, the PIE count.
4. Item 1: the rule, tolerance, cost, tests both ways; the auto-pool pixel table (labelled → visibly corrupt); eligibility before/after.
5. Item 2: the fixture, the pixel and mask-scope table, edges, can-fail.
6. Item 3: the cook, the staged table per variant, the lever fix (or exactly what blocked).
7. Builds, archives, commits pushed.
8. NEEDS-DECISION items, or "none".
