# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 095-05b m56 capture continuity - check tools learn settle and schema 2.2, bench check scripts proven offline, docs - no engine legs, no Codex

# 095-05b — m56: the readers, the bench check scripts, and the docs

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Where things stand:** an earlier session of yours implemented m56's plugin side on `fix/m56-capture-continuity` (tip `586a0fd`, pushed; commits 1 to 4 of the plan). New sessions now carry `settle`-flagged frames, `tick_index`, `t_wall_run`, an honest drop count, and schema minor 2. **The project's own check tools do not understand any of that yet,** and two of them would reject every new session. This brief fixes the tools, proves the new bench check scripts offline, and writes the docs.

**The machine is shared** and the owner's work comes first. This brief is **light**: Python, small standalone tests, docs. **No Game build, no editor or game launch, no capture leg, no cook.**

**Read first, in this order (binding):**
1. `D:\IntrusiveAnomalies\_reviews\095-05-ruling.md`: what chat accepted, and the gotcha numbering.
2. `D:\IntrusiveAnomalies\_reviews\095-02-plan-A1-capture-continuity.md`: **§3 (keys and every reader), §7 (gates and the new scripts), §8, §10, §11.** This is the specification.
3. `D:\IntrusiveAnomalies\_reviews\095-02-ruling.md` (Unit A-1 sections) and `095-03b-ruling.md` (E2, E4).
4. The 095-05 report in `D:\IntrusiveAnomalies\_mailbox\to_chat\` and the journal `docs/sessions/2026-11-13-095-05-m56-capture-continuity.md`: what was actually built, including the deviations and the exact key names.

**Receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-05b\`. Python: `C:\Python313\python.exe`.

## Pre-flight: stop with NEEDS-DECISION if any fails
- The primary checkout is on `fix/m56-capture-continuity` = `586a0fd`, clean apart from the known untracked files, equal to origin.
- E: is at or above its 50 GiB floor.

## Part 1 — plan commit 5: the plugin repo's readers
`chore(tools): m56 readers learn settle and schema 2.2`. Every row of plan §3.2 marked **BREAK**, **SILENT** or **update** that lives in the plugin repo:
- `tools/verify_capture.py`: accept minor 2; `settle` in the reason list, the per-type reason table and the labelled / unlabelled lists; not in the temporal-only list.
- `tools/label_sync_check.py` (the office kit, which ships to the client): accept minor 2; know `settle`; excuse `settle` frames in both directions; re-check its `frame_index` jump logic now that jumps are gone.
- `tools/annotation_transition_check.py` and `tools/test_annotation_transitions.py`: minor 2, a `settle` fixture.
- `tools/measure_label_offset.py`: exclude transition frames from its baseline.
- `tools/exclusion_gate.py`: align its overlap rule. **If the matching C++ counter needs a change,** make it as its own small commit and compile it under the gentle rules (Editor target only, 2 parallel actions, at least 4 GiB free **re-checked at launch**).
- `tools/r4_pairing_gate.py`: re-run it against the new source.
- `tools/OFFICE-CHECK.md`: wording and counts.
- **Tests:** every tool's existing tests pass. Add synthetic schema-2.2 fixtures: one healthy session with settle frames must pass each reader; a copy with a settle flag moved, a copy with a label shifted by one frame, and a copy with a tick missing must each **fail** the reader that should catch it. Old schema-2.1 sessions must still be read correctly: test one banked release-era session (read-only).

## Part 2 — the CaptureBench scripts (its own repo, `Plugins\CaptureBench`)
On a branch `m56-gates` in that repo (commit there; push only if it has a remote and the scan passes):
- `tools/run_leg.ps1`: the config line and the per-leg predictions for the 16-frame cycle.
- `a54_oracle.py`: the flank rule skips flagged frames.
- **New `m56_continuity_check.py`** (plan §7 G1) and **new `m56_nonsettle_compare.py`** (plan §7 G5).
- **Prove both new scripts both ways, offline:**
  - the continuity check must **FAIL** on the banked release-era session `D:\IntrusiveAnomalies\_m47_legs\AE1_CT_AEON\session_20260903-182615` (read-only; expected 15 `frame_index` jumps of 3 in 90 rows) and must **PASS** on a doctored continuous copy written into your receipts folder, never into the bank;
  - the comparator must pass on a doctored identical pair and fail on a pair with one changed label field.
- The other bench comparators that plan §3.2 lists as needing new predictions: update the predictions, note what you could not test without an engine leg.

## Part 3 — plan commit 6: docs
`docs: m56 capture continuity`:
- `docs/client-readme.md`: the row and `run_summary` tables with **every** new key as built (including the extra writer keys and `orphan_completions`), the transition-reason table with `settle`, a 2.1 → 2.2 changelog row, and the `t_wall` fix (absolute clock; `t_wall_run` is the run-relative one). Plain, client-facing wording; plan §10 is the content.
- `docs/architecture.md`, `docs/capture-fps.md`, `docs/PRE-DELIVERY-CHECKLIST.md` (the `run_summary` key-count literal), the status block, the journal, gotchas **from G575**.
- The docs must say plainly what is **not yet proven**: no engine leg has run; the default K = 2 flags two frames per edge; the dashboard's encode step and the client's bundled kit are not updated yet.

## Finish
- Pre-push scan, then push `fix/m56-capture-continuity`. **No merge to `master`, no tag.**
- Leave the primary checkout on `fix/m56-capture-continuity`, clean.

## Rules
- **No Codex.** No `--force`, `clean`, `reset --hard` or `stash`. Delete nothing outside your own outputs. The bench bank and `_m47_legs` are read-only.
- **Never touch the owner's processes.** **Never kill a process by name.** No machine settings.
- **Never touch:** the `m51` ref, `fix/followup-capture`, `feat/europa-port`, `feat/m58-terrain-blink-subtype`, `D:\IA_Europa`, tags, the dashboard repo.
- Never print a token. Codenames only. Office data never comes home.
- **Commit each part as soon as it is done.** If your context passes 60 %, finish and commit the part you are on, push, and report what remains as NEEDS-DECISION.
- **How your turn ends:** do not end while anything you started is still running, and do not end on a progress update or a question. End only with the final message below. Target: under 3 hours.

## Final message
- **The very first line is exactly `VERDICT: GREEN`, `VERDICT: NEEDS-DECISION` or `VERDICT: RED`.** Bare line: no number in front, no code fence around the message. GREEN means Parts 1 to 3 are committed and pushed and every test named above gave its expected result.
- Then:
  1. **For the owner (3 lines, plain words).**
  2. **Commits** in each repo (hash and subject), tips, pushed or local.
  3. **Reader tests:** per tool, pass and must-fail results. **The two new scripts:** the both-ways proof, with the numbers read from the banked session.
  4. **What could not be tested without an engine leg.**
  5. **Deviations,** each with its reason. Context used.
  6. **Safety,** or "none". **Decisions chat must make,** or "none".
