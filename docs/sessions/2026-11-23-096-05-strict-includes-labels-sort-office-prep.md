# 096-05 — strict-include build, labels.jsonl sorted at run end, office pack prepared for O-002

**Date:** 2026-11-23. **Agent:** Claude Code (Opus 5.5, xhigh), headless from the GDP mailbox. **Rulings:** 096-04 (D1 to D5),
analysis `_reviews\096-04-o001-followups.md`. **Light brief:** Editor-target compiles only (2 parallel actions, started with at
least 4 GiB free), pure selftests and Python tests. No Game build, no editor or game launch, no capture, no cook, no Codex.
Receipts: `D:\IntrusiveAnomalies\_relay\runs\2026-11-20-096-office-agent\code-receipts-096-05\`.

## 1. Engines (owner, 2026-11-23)

Concorde UE 5.1, Bates UE 5.3.2, Europa UE 4.25. `CLAUDE.md` (the m35 entry) and the office pack said Bates was 5.1; both are
corrected (G584). The home side has UE 5.1 (build), the read-only UE 5.7.4 tree and UE 4.25; it has no 5.3 headers.

## 2. Part 1 — strict-include build (ruling D5)

- Tool: `tools/strict_include_build.py` (unchanged), driven by `strict_wrap.py` in the receipts, which only adds
  `-MaxParallelActions=2` to each UBT call. Target `StackOBotEditor` only, `--no-normal-rebuild`. Launched by
  `launch_gated.ps1` once free memory read 4.08 GiB.
- Switches the tool sets in every plugin `Build.cs` for the pass (restored byte for byte afterwards): `bUseUnity = false`,
  `PCHUsage = PCHUsageMode.NoPCHs`, `IncludeOrderVersion = EngineIncludeOrderVersion.Latest`, `bEnforceIWYU = true`; plus one
  generated translation unit per plugin header (75) so every header must compile on its own.
- Commit under test: `feat/m57-lod-spatial-trigger` `d8d3a7e` (contains `integ/m56-m58` `0c71f6f`; m56, m57, m58, m59).
- Result: 158 actions, **0 errors, 0 warnings**, exit 0, 1167 s. Lever check: per-file compile responses without PCH in every
  module (AnomalyBench 3, AnomalyCapture 43, AnomalyControlServer 8, AnomalyInjector 76, AnomalyShaders 7), header TUs 75 of
  75. Restore OK (Build.cs hashes identical, generated folders removed, git status unchanged). **No include needed fixing.**
- Extra check for 5.3 (no 5.3 headers at home): `include_presence_check.py` resolved every engine `#include` of `d8d3a7e`
  (194, plugin headers and UHT outputs excluded) against the 5.1 tree and the 5.7.4 tree. 180 are in both; the other 14 are
  5.7-only headers that sit behind `__has_include` guards (added in 091-02). None is unguarded and missing from either tree.
  This is evidence, not proof, for 5.3.2: a header present in 5.1 and 5.7.4 is very likely in 5.3, and a deprecation would
  show as a warning, not an error.

## 3. Part 2 — labels.jsonl in frame order (ruling D3)

**What.** The writer appends each row when its job finishes, so rows land in job-completion order (the client's M2 session:
314 of 2999 out of place). `FinishRun` now calls `AnomalyLabel::SortLabelsFile` after the writer flush and the continuity
accounting, immediately before `WriteRunSummary`. It reads `labels.jsonl` as bytes, splits on `\n`, takes each row's top-level
`session_index` (the token `"session_index"` followed by a colon and an integer), stable-sorts, writes `labels.jsonl.tmp`,
checks its size, and replaces the file with `IFileManager::Move` (delete then rename, G585). Every row is kept byte for byte.

**When it does not sort** (file left as written, `labels_sorted: false`, reason in `labels_sorted_reason`): `not_written`
(delivery mode with labels off), `writer_stalled`, `writer_flush_incomplete`, `no_file`, `read_failed`,
`row_without_session_index`, `write_failed`, `replace_failed` (if the original is then gone, the log says the complete rows
are in `labels.jsonl.tmp`). A file already in order is not rewritten (`labels_sorted: true`, log `result=already_in_order`).

**Fields.** `run_summary.json` gains `labels_sorted` (always, in the continuity block) and `labels_sorted_reason` (only when
false). `annotation.json` and the `labels.jsonl` field set do not move. **No schema bump:** the readme's minor version moves
when a label's meaning or the frame set changes (2.1 transitions, 2.2 continuity); this is a file-order guarantee plus two
additive `run_summary` keys, shipping inside 2.2, which is not released yet (the delivered release is 2.1). The readme's
2.1 to 2.2 changelog lists both as "ships with 2.2".

**Readme.** "Reading `labels.jsonl` — line order" replaces "the rows are not in order": lines are in frame order when
`labels_sorted` is true; older, stalled and killed sessions are not; key or sort by `session_index` stays the advice for every
session. The §8.6c key table explains both keys and every reason.

**Code.** `Source/AnomalyCapture/Private/AnomalyLabelsOrder.h` (new, pure, no engine types: parse, split, stable merge sort,
assemble, the skip rule and the reason words); `AnomalyLabelWriter.h/.cpp` (`FLabelsSortResult`, `SortLabelsFile`, two
continuity fields and two keys); `AnomalyCaptureSubsystem.cpp` (the call and the `Capture(m56): LABELS ORDER` log line,
Warning when a skip is not `not_written`).

**Tests.**
- `tools/labels_order_selftest.cpp`: 41 checks, 0 failures, compiled `/W4 /WX`: out-of-order rows, a stalled run left
  unsorted (and an incomplete flush, and a run that wrote no file), an empty file, a row without the key, the key inside a
  string value, equal keys stay in written order, CRLF rows, a last row without a newline, blank lines, 3000 shuffled rows
  against `std::stable_sort`.
- Mutants (`labels_order_mutants.py`): unstable merge, stall ignored, colon check removed, last newline dropped, empty file
  not counted as sorted: **5 of 5 fail.** A first mutant (removing a key-position check) survived; the check was unreachable
  with valid JSON (the colon rule already does that work), so it was removed and the colon rule got its own case.
- Real banked sessions through the header (`real_sort_check.py`): a 5.1 m53 probe session (100 rows, 5 adjacent pairs out of
  order, 10 rows moved) sorts byte-identically to a Python `json` sort; two 4.25 sessions (2352 rows each) were already in
  order and stay identical; an old Lyra session whose rows have no `session_index` is refused, as the engine path would.
- Editor target of the integration tip: exit 0, 0 warnings. All 19 pure C++ selftests, the Python suites (label rule 94,
  pixel gate 101, change oracle 35, kit 102, unit tests, exclusion gate, measure offset), lever audit source and binary
  (57 names, 0 failures), terrain mutants 5 of 5 fail.

## 4. Part 3 — office pack (`docs/office-pack`)

- Item 1 (`6e3a5b7`): engine facts in START-HERE (first section, the Bates entry, the "older documents" note, the delivered
  line) and O-002. Archived records are left as written.
- Item 3 (`7b00595`): office-tools README note. Every office reader that reads `labels.jsonl` keys or sorts by
  `session_index`; checked with `office_tools_order_check.py`, each run on the same synthetic sessions with rows sorted and
  shuffled: 6 of 6 comparisons identical in exit code and readings (a failing dropped-tick session included).
- Item 2: O-002 amendments — the commit under test becomes the new `feat/m57` tip; order of work; Bates runs in PIE in the
  editor with the owner attending (the agent automates console commands and the control server; asks only for PIE, OK or
  play; a host-side fix for the boxes or the standalone crash only as a logged host change); Step 0 (targeted
  `stuck_low_mip`, `uv_corruption`, `normal_corruption` on the current copy, with the existing timer lines and keys, repeated
  in Q3 on the new build); Q4 on Bates: count what each ground exclusion pattern removes and how many are not ground, then
  host change H-001 (patterns out, terrain class word in `TerrainNamePatterns`), Q4 last; Q2 reads `labels_sorted` and the
  row order.

## 5. Deviations and judgement calls

- Part 1 found nothing to fix, so it has no commit.
- The replace is UE's `IFileManager::Move` (delete then rename), not a single Windows `MoveFileExW` call: Windows headers in
  a unity-built capture file were judged a larger risk for the Bates 5.3 build than the gap, which no reader can see because
  `run_summary.json` is written after it. Recorded as G585.
- O-002's Bates paragraph allows one fallback the brief did not list: if the agent finds no way to issue a console command
  itself, it gives the owner the exact line to type.

## 6. State and hand-off

- `integ/m56-m58`: `df5a595` (sort) and `88cd068` (CLAUDE.md engine fact), then this docs commit. `feat/m57` merges it; the
  merge commit is what O-002 names (see the 096-05 report and the pack). Both pushed with `docs/office-pack`.
- The host's plugin Editor DLLs match the `feat/m57` merge tip after this session's last build; the checkout is left on `integ/m56-m58`. The Game exe is still not built.
- Next: O-002 at the office (owner). 095-07b stays held.
