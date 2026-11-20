# ACK-VERDICT: 090-05 NEEDS-DECISION ruled in _reviews\090-05-chat-ruling-f1-nanite.md — all accepted; Shipping EXCLUDED by descriptor; the office kit learns effect_interrupted; m52_log_counts.py; F1 cost threshold tonight
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 090-07 Shipping exclusion + kit effect_interrupted + m52 log counts (Code, Opus 5.5)

# 090-07 — Three small pre-Thursday items: (A) exclude every plugin module from Shipping via the `.uplugin`; (B) the office kit `label_sync_check.py` understands `effect_interrupted`; (C) `tools/m52_log_counts.py`. Fix branch first, then merge into m53. Compile-only; NO game launch. Its m53 head is the owner's Thursday pull target

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going. End the run only when every step is done, a stop condition fires, or something needs the owner. The final message follows the structure at the end and starts with a `VERDICT:` line. Builds outlive the tool timeout: run them in the background and poll. **Keep context lean** (finish below 70 %; the last session ended at 93 %). Write docs with the file tools (G331), not PowerShell (it added a BOM to `25f9450`'s subject).

**The PC may be in use (a UE 5.7 editor is open). Compiling is allowed; launching the game and anything that takes focus are not.**

## Bootstrap: read these first, then summarise in three lines
1. `_reviews\090-05-chat-ruling-f1-nanite.md`: **THE SPEC** (rulings 1–3).
2. The 090-05 report (`_mailbox\to_chat\090-05-f1-nanite-readme.report.md`): F1's semantics, and item 10's Shipping finding with `_reviews\090-05-shipping-code-read.md`.
3. `tools/label_sync_check.py` and `tools/OFFICE-CHECK.md` at `25f9450`, and chat's PowerShell counter `_reviews\090-04b-office-m52-log-counts.ps1.txt`.

Heads: fix `25f9450`, m53 `5cda4e9`.

## The work (on `fix/m52-label-timing`, then merge into `feat/m53-uv-normal-corruption`)
**A. Shipping exclusion (descriptor only).**
- Add `"TargetConfigurationDenyList": ["Shipping"]` to every module in `AnomalyInjector.uplugin` that lacks it. `AnomalyBench` already has it.
- **No source change.** `ANOMALY_CAPTURE=0` code stays; update G428 to say it is now unreachable and why.
- **Evidence (can-fail):**
  - the Shipping target's module list from UBT **without compiling** (for example `-Mode=JsonExport`, or the equivalent that works on 5.1): the plugin modules present on the unchanged descriptor, absent after;
  - the Development, Test, Editor and Game module lists identical before and after;
  - the normal Development build: code unchanged by the string scan;
  - the lever audit and the strict-include build still 0 errors.

  A real Shipping compile is optional, only if the scratch host is free and it fits; say whether you did it.
- **Readme + checklist:** "The plugin is excluded from Shipping builds by design; capture with Development or Test." Put it in the readme's build/setup section and in the checklist's build box.

**B. The office kit understands `effect_interrupted`.**
- Whitelist the reason. An entry that is `effect_interrupted`-only is not a TAA flag and is never inferred as one.
- **An interrupted event:** its label end (the last labelled frame) is judged against the picture at the interruption, by the kit's own offset rule, **0 frames off** to pass.
- **A labelled–interrupted–labelled event** (host replace, then re-install): each labelled run is judged at its own edges. The gap frames must not show the effect (read from pixels), and the gap is read back as its own count, never as a failure or as a proof by itself.
- The **READ BACK** line gains `interrupted N`. `OFFICE-CHECK.md` gets one plain line explaining it.
- **Tests both ways:**
  - doctored copies of banked real sessions in the F1 shapes (Codex's 10/20/40; replace 15 with re-install 18) read 0/0;
  - a copy with the label running 4 frames past the interruption FAILS;
  - mutants that treat `effect_interrupted` as a TAA flag, or ignore it, FAIL;
  - `--selftest` still ends `OK`.
- **The kit output stays numbers only.**

**C. `tools/m52_log_counts.py`** (stdlib only, numbers only).
- The same readings as chat's PowerShell function:
  - PURITY ENUMERATION; HOLDING (the `- HOLDING n of m` form only, not "HOLDING RIGHT NOW");
  - HELD NONE, plus the six bucket totals;
  - `shared_world` count, the users 2–8 vs ≥ 9 split, and min/max;
  - `baseline_pending`; HOLD CONTAMINATED; `stuck_low_mip: matched 0 mesh`.
- `--selftest`: a synthetic log built from the **source's own format strings** at this head. It must agree with the PowerShell function's expected numbers on chat's sample (PURITY 2, HOLDING 1, HELD NONE 2, buckets 1/1/0/3/1/0, shared_world 4 = 2 + 2, min/max 3/12, baseline_pending 1, CONTAMINATED 2, matched 0 mesh 1). A mutant that counts "HOLDING RIGHT NOW" must fail.
- One line on `OFFICE-CHECK.md`: `python tools\m52_log_counts.py "<the game's log>"`.

**Then:**
1. Fix branch: the suites (all, plus the new ones), the kit selftest, a normal build (string scan: code unchanged), strict-include 0 errors, the lever audit, archive. Commit and push.
2. Merge into m53; the same checks. Commit and push.
3. **Docs:** the journal `docs/sessions/2026-09-30-090-07-shipping-kit-logcounts.md`, status blocks, gotchas.

## Hard rules
- No game launch, no cook, no staging. Don't touch `_reviews\090-06-*` (frozen for tonight).
- **Never change any security setting.** If `dbghelp.dll` blocks something, report it.
- Never kill anything that is not yours, and never by name.
- Don't touch `m51`, `master`, tags, `ToCodex\` or `E:\<personal-folder>`.
- Codenames only (the office hosts are "the first/second office host").

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner**: 2 lines, with **the Thursday pull target** (both office hosts).
3. A: the descriptor change and the before/after module lists (Shipping, Development, Test, Editor); the Shipping compile done or not.
4. B: the kit's rule for interrupted events, and the tests both ways; the new READ BACK line.
5. C: the counter's readings and its selftest.
6. Builds (both branches): code unchanged (string scan), strict 0, lever audit, archives. **State whether the 090-06 pinned exe `97D292F8` still represents the m53 head's Development code.**
7. Commits pushed.
8. NEEDS-DECISION items, or "none".
