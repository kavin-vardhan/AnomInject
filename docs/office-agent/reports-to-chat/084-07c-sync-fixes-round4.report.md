# REPORT for 084-07c-sync-fixes-round4.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-07c-sync-fixes-round4.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 04:22:58 | finished: 2026-09-29 05:15:46 | duration: 53 min | exit code: 0
context at last turn: 755027 tokens (76% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-07c-sync-fixes-round4.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Summary for the owner:** All eight findings from Codex's last review were fixed on `fix/m52-label-timing`. Each fix has unit tests that pass on the fix and fail on the old behaviour. Both targets rebuilt with 0 warnings, and the branch was re-cooked and archived; nothing was staged or launched. Codex's second check says four items are fully resolved and four are partly resolved, and nothing blocks under the loop rule. Chat still has to rule on four remaining cases, and one of them is a small defect this round introduced.

**N1–N8: where each fix lives, and the test in both directions**
- **N1:** `AnomalyLabelSync::IsAnnotationMember` is now the one rule that decides both `labelled` and the event's frame list in `annotation.json`. Both writers, the counter and the annotation read the same two per-frame values (`ResolveAnnotationPolicy`, `FillAnnotationInputs` → `FirePolicy` and `FireOnScreen`).
  - Tested for all four policy classes. Reading the activity bit fails for texture swaps, reading "event running" fails for the other classes, and reading the game-thread state fails for `stuck_low_mip`.
  - `verify_capture.py --label-rule` fails the `FF41BFF3` shape and passes the fixed shape.
  - None of the 2,586 banked `labels.jsonl` files carries a `labelled` key, so no banked session has the defect shape.
- **N2:** `PrepareConfirmSegment` traces a too-short clipped segment along the whole slab. A candidate with fewer than 6 valid traces becomes the new outcome `TooFewValidTraces` (flagged unconfirmed, never a miss). The zero-thickness wall is now a confirmed positive; the old rule is shown to produce 144 zero-length segments; the all-degenerate case is flagged.
- **N3:** `MakeMatrixBox` builds each instance's box from the renderer's product `PerInstanceSMData[i].Transform × component matrix`. Codex's case gives X [50,250] and passes; the old composed `FTransform` gives X [140,160] and rejects it. The 45° shear case and the exact-when-orthogonal case are also tested.
- **N4:** a texture set is now full, partial, unresolved or not held (`ClassifyHeldSet`), and an invalid held endpoint counts as unresolved. There is a new transition reason, `unresolved`, and an unresolved frame never sets the full-set boundary. Each case is tested against the 084-07 rule.
- **N5:** partial and unresolved frames are stored as exact ranges with no cap (`TPartialEdgeTrack`). A 45-frame test keeps every frame; the old 32-slot track loses 8.
- **N6:** `DecideRunEndCarry` now accepts a carried detached tail as a source, and a run that writes no frames passes the history on unchanged. The test crosses two run boundaries (1 + 1 + 6 = 8 frames); under the old rule the third run gets 0.
- **N7:** `GAppliedValues` records every component a value was ever applied to, and retirement runs through the pure `RetireHolders`, which the test calls directly. Codex's case now fails verification; the old 084-07 path passes it and the value aliases.
- **N8:** I used the marker, not a full snapshot: `IAI.Capture.Shot` rows carry `label_rule: "legacy_shot"`, and the readme and `--label-rule` handle it. A single shot has no render record and no transition history, so a "full snapshot" couldn't reproduce the run rule for `stuck_low_mip` anyway.
- **Test counts:** m52 selftest 262/0 (was 220), camera selftest 128/0 (was 118), `--label-rule` 24 cases (was 15). The other verify suites produce identical output. A mutation check re-inserted each old behaviour into the headers, and all nine versions fail the tests. Codex correctly pointed out that the N3 mutant was not a faithful replay of the old behaviour; the journal now says so.

**Build and cook**
- **Exe:** `16053B80` (242,115,072 B), archived at `_binary_baselines\m52fix-16053B80\`.
  - The first build passed compile but failed the string scan: a log line still named the old `..._v2` rule. I fixed that one token and rebuilt.
  - Final rebuild: editor 4 actions / 26 s, game 3 / 37 s, 0 warnings in all four build logs. The comment stripper changed 0 files.
- **Cook:** 48 s, 794 packages, 0 warnings. Pairing is exe `16053B80` + `59C20959` / `0EC2587E` / `26FFC026` + `462B8AC6` / `BB05CF99`, archived at `_binary_baselines\m52fix-cook-59C20959\` (6/6 re-hashed at the destination).
  - **Descriptor:** the cooked `.uplugin` is byte-identical to the branch's (`9EFB9B49…`), with `AnomalyBench` first and Shipping-denied.
  - **Maps:** the four maps are present and `CB_TexCorruptLevel` is absent, as declared.
  - **Content:** the project content manifest is identical before and after the cook (`E4C7F38D…`, 2,182 files).
  - **Unexplained difference:** 3 of 1,972 container entries differ from 084-07b's cook (two shader-archive tables, and `MainWorld.umap` +50 B). The source content is identical and I have not established the cause.
  - Nothing was staged; the m53 set is still in BenchGate.

**Codex re-check v2** (1,119 s; collected to `_reviews\084-07c-codex-sync-recheck2.md`)
- N1 RESOLVED.
- N2 PARTIAL: new medium issue. The full-slab fallback can trace a welded parent's body and report its geometry as the child's.
- N3 PARTIAL, one **HIGH** left: the traces run against the instance's collision body, which the engine still places with the composed `FTransform`. No banked capture has exercised this path (instances tested: 0).
- N4 PARTIAL: new medium issue that this round introduced. A forced-unknown result defaults to "not held", so it gets no `unresolved` reason.
- N5 RESOLVED.
- N6 RESOLVED.
- N7 PARTIAL: a medium variant remains where a component's saved prior value can write a retired value back later.
- N8 RESOLVED.
- Codex's verdict: PARTIAL, no blocking finding under the loop rule. I did not act on any of it in source; the readme now documents all four cases.

**Commits pushed:** `3d6f702` (the fix) and `11317a4` (docs: journal, status block, readme limits, architecture, G363–G365). `fix/m52-label-timing` now matches origin at `11317a4`. `m51`, `master`, tags and the main checkout are untouched. Roughly 5% of the token budget was used.

**NEEDS-DECISION:** chat rules on Codex v2 (details in journal §7):
1. The remaining N3 HIGH: either confirm instances against geometry that matches what is drawn, or flag them unconfirmed.
2. NEW-2: default an unclassified held set to unresolved. This is the defect from this round.
3. NEW-1: welded candidates.
4. The N7 saved-prior variant.

Each fix is a small source change plus a rebuild and re-cook.

Several MCP connectors (Gmail, Calendar and others) need authorization, and the pdf-viewer server failed to connect. This run did not need any of them.
api-equivalent cost (not billed on Max): $28.33 | turns: 211 | running total across all briefs: $3,324.56
