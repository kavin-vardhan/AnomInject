# REPORT for 094-49-europa-cleanup.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-49-europa-cleanup.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-06 20:50:39 | finished: 2026-11-06 21:31:33 | duration: 41 min | exit code: 0
context at last turn: 547208 tokens (55% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-49-europa-cleanup.live.txt

---- FINAL MESSAGE (Claude Code) ----
**VERDICT: GREEN**

**For the owner:**
- **Space freed:** 74.15 GiB on D: (free space went from 26.4 to 99.9 GiB). Nothing was deleted on C: or E:; their free space moved a little during the session from other activity, not from this cleanup.
- **What was deleted:** the five home test-capture sessions (`Evidence\09447u`–`09447y`). Also every file over 1 MB in `Receipts\` and `Temp\`: captured frames and masks, `.u32`/`.zlib` evidence, old ledger snapshot databases, inventory copies, compiler object files and the superseded dashboard builds.
- **What was kept on purpose:** the Paris host, caches, the 5.1 baselines, Tools/Launch, your dashboard (`dashboard-build-r4`) and every small receipt. I also kept the large inputs that the owner kit's P9 check and a few tool tests still read, plus 3 items I wasn't sure about. The current game package lives on E:, so this cleanup couldn't reach it.

**Size per top-level folder** (GiB, before → after)

| Folder | Before | After | Files after |
|---|---:|---:|---:|
| Receipts | 83.82 | 20.93 | 137,458 |
| Evidence | 8.02 | 0.00 | 0 |
| Temp | 3.42 | 0.18 | 16,435 |
| ReGate51 | 33.60 | 33.60 | 17,731 |
| Baseline51 | 18.85 | 18.85 | 13,618 |
| StylizedParisStreet | 14.30 | 14.30 | 9,058 |
| Caches | 2.35 | 2.35 | 4,672 |
| Launch / Tools | 0.007 / 0.000 | unchanged | 16 / 4 |

**Manifest counts**
- **Inventory:** 260,255 files. No file under `D:\IA_Europa` has more than one hardlink, so the hardlink rule protected nothing here.
- **Delete manifest:** 61,263 files. The Evidence sessions were 42,285 files (8.02 GiB); large Receipts/Temp files were 18,978 (66.13 GiB). All 61,263 were deleted, with 0 skipped and 0 errors. 192 folders left empty were removed; no folder named by a card, a Launch config or a tool was removed.
- **Keep manifest:** 4,463 kept files over 1 MB.
- **Large kept files I added beyond the brief's list:**
  - 433 files (1.13 GiB) that `owner_p9.py` reads. The owner-leg launcher in `Launch` runs it, so deleting them would break P9 for any future job. They are `Receipts\P9\`, `Receipts\09405\ledger\`, the 09406 engine-differences file, the 0940x/09415/09421 `classified-changes.jsonl.gz` files, and a few 09403/09415 inputs.
  - 7 files (0.21 GiB) whose paths are written into tool scripts. These include the test fixtures read by `action_reader_tests.py`, `capture_ft_tests.py` and `capture_fx_tests.py`.

**Checks after the delete**
- All 198,992 kept files are still there with the same size and timestamp; none of the deleted files remain.
- The package on E: matches all 34 files and SHA-256 hashes in 094-48c's list. It started with `-game -nullrhi -ExecCmds="quit"` and exited 0 in 3 s. I pointed its user folder and log at my receipts, and it added no files to the package.
- The dashboard exe is present and its SHA-256 (`c8120946…`) matches the 094-47 card.
- The worktree is at `2828f6a` on `feat/europa-port` and clean. `m51` is unchanged (`53bf725`, the same 15 untracked files).

**Kept, unsure** (30 MiB in total)
1. `Receipts\09447t\dashboard-r3\prior-AnomalyDashboard.exe` (10.7 MiB): the older dashboard build. The repo's release folder now holds r4, and I found no other copy of this one.
2. `Temp\09448c\library-harness\europa_library_gate.exe` (2.3 MiB): the Library check tool 094-48c used. It can be rebuilt from its kept source.
3. `Receipts\_cleanup-094-37\plan.json` (17.4 MiB): the record of an earlier cleanup.

**Safety:** none. No process was running from `D:\IA_Europa` before the delete, nothing was killed, no git writes were made, and nothing outside `D:\IA_Europa` changed except my receipts.

**Decisions for chat:**
- Delete the three "kept, unsure" items, or keep them.
- Optionally, whether to keep the P9 and tool inputs long-term. They are 1.34 GiB and are needed only if the owner kit's P9 check or those tests will run again.

Everything is in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-49\`; `exit-receipt-094-49.txt` is the summary to open first.
api-equivalent cost (not billed on Max): $12.23 | turns: 85 | running total across all briefs: $5,907.28
