# 083-02 — Disk cleanup: the approved zero-loss set from the 083-01 inventory

Date 2026-09-26. Brief 083-02, ruling `_reviews/083-02-chat-ruling-disk-cleanup.md`, inventory `_reviews/083-01-evidence/`. Scripts, plans,
receipts and every per-item log are in `_reviews/083-02-evidence/`. No source, build, cook, capture or bench leg was involved. The main
checkout stayed on `m51` `53bf725`. These docs were committed from a scratch `master` worktree outside the StackOBot tree.

## What was done, in order

| step | what | size | verification |
|---|---|---|---|
| 0 freeze | re-verified every list against the live disk | — | A0 922/922 (path + exact size; session and leg-dir entries re-matched to a bank copy by session id + relpath/size manifest, loose files included; baseline exe duplicates SHA-256 matched to `_binary_baselines`). B0 758/758 (manifests still identical). Strip 119/119 (image bytes unchanged). Six legs present, no bank-name collision. **0 of 329,282 files open** (exclusive-open test). **Nothing dropped.** |
| 1 bank | `G4_TEARDOWN`, `G4_TEARDOWN2`, `M52E_LYRA_STUCKMIP`, `M52E_LYRA_STUCKMIP_CO8`, `PB8L_LEGA`, `PB8L_LG9` copied to `_bench_sessions_bank\<name>`, verified, source deleted | 2.99 GB, 1,969 files | manifest equal and SHA-256 equal per file before delete; receipts `bank_receipt_<name>.json` |
| 2 A0 | deleted exactly the 922 listed paths | E: 79.35 GB, D: 27.51 GB | each entry re-verified immediately before its deletion (leg dirs against their bank copy); in-use check per entry |
| 3 B0 | redundant bank copies replaced by NTFS hardlinks to the keeper | 72.16 GB, 99,954 files, 758 pairs | SHA-256 of both sides equal before linking; link = same file id as keeper; atomic replace. After: 1,988 bank legs have identical file lists and sizes; all 99,954 names re-hash equal to their pre-link hash. Bank audit: 234.03 GB logical, 161.87 GB unique |
| 4 B1+B2 | `png-manifest.json` written (relpath, size, SHA-256) and read back, then `Actual_Frames\*.png` deleted | 119 sessions, 9,590 PNG names, 5.74 GB unique | hardlink guard: no target shares frames with a kept name, so none dropped. Bank-wide diff afterwards: the only changes are the removed frame PNGs and the 119 manifests; masks, labels, sidecars, logs and JSON untouched (2,426 mask files kept in stripped legs) |
| 5 compress | `compact /c /s /i /q` on `_binary_baselines` | 30.74 GB stored in 19.11 GB (10.84 GB saved) | `E0BE6F0A` (staged archive), `002805CF`, `85A39CFB` (m23), `A7EF9B12` (m45 ucas) hash-identical before and after |
| 6 verify | final state | — | staged exe still `E0BE6F0A`; the 5 staged container files hash-identical; `m51` HEAD, status and tags unchanged; the m55 change oracle (`master:tools/verify_capture.py`, `b399e884…`) exits 0 on `M55B3R23_C2_SOLID_N_A1` and on the hardlinked alias `M55B3R23_CB_N3`, with byte-identical output before and after |

## Free space (GB)

| drive | before | after | delta |
|---|---|---|---|
| C: | 38.10 | 37.62 | −0.48 (not touched by this work) |
| D: | 174.22 | 181.61 | +7.39 |
| E: | 106.95 | 273.09 | +166.13 |

**D: freed 7.39 GB against 27.51 GB deleted.** The deletion is complete: the x64 tree is gone and all 922 entries are verified gone. D: has
no multiply-linked file (full scan, 1,175,293 files), and D: showed no ongoing writes afterwards. The sibling `Win64\UnrealEditor` tree
is uncompressed; the deleted tree's own attributes can no longer be read. `D:\pagefile.sys` reads 60.09 GB and was last written at 17:45:54, inside the A0 window. A system-managed pagefile growing would
account for the gap. ⛔ **Cause not established:** its prior size was never read.

**E: rose 9.69 GB over the compression step against 10.84 GB saved by `compact`.** Something else wrote to E: during that window. Not
isolated.

## Not done, by ruling

A1 (StackOBot `Intermediate` and `Saved\Cooked`), A2, B3 (pre-m52 history frames, the owner's call), and the Unknown list (the owner's
call). Policy items 1–2 (harness) wait for m53's first bench brief.

## Docs

Runbook §8.6 step 0b: the 50 GB E: floor and policy items 3–7. G308: hardlinked bank names.
