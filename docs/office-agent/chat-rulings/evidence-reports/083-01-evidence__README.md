# 083-01 disk inventory — evidence index (READ-ONLY run, nothing deleted/moved/renamed)

Measured 2026-09-26. Method: .NET enumeration (sizer*.ps1), per-session manifest = SHA1 of sorted `relpath|size`
(session_manifest.ps1), identity = same session id AND same manifest; hardlink audit by NTFS file id (hardlink_audit.ps1:
zero hardlinks anywhere, so every byte counted is real).

| file | what |
|---|---|
| iabc_tree.csv, builds_tree.csv, other_tree.csv, intermediate_tree.csv | size/files/dates trees |
| bank_children.csv | per bank folder: bytes, image bytes, dates |
| sessions.csv | every session_* folder found (3,703) with manifest |
| outside_bank_sessions.csv | non-bank sessions vs bank: DUP-EXACT / ID-IN-BANK-MANIFEST-DIFFERS / UNBANKED |
| legdir_check.csv | exe-side and Lyra leg folders: full duplicate of bank (sessions + loose files) or not |
| b22_rescue_check.csv | B22_PRECOOK_RESCUE copies vs PRECOOK077_* |
| bank_refs.csv, bank_refs_reviews.csv | which docs/ledgers name each bank folder |
| bank_legs_classified.csv, bank_class_totals.json | bank classification |
| hardlink_audit.txt | unique on-disk bytes |
| A0-delete-zero-cost.txt | class A, no rebuild cost (106.85 GB: E: 79.34, D: 27.51) |
| A1-delete-with-rebuild-cost.txt | class A, one-time rebuild cost (14.93 GB) |
| A2-optional-engine-orphan-binaries.txt | optional, low value (1.75 GB) |
| B0-dedupe-hardlink-candidates.txt | lossless dedupe inside the bank (72.16 GB) |
| B1-strip-superseded.txt, B2-strip-invalid-attempts.txt | PNG strip (1.27 + 4.48 GB unique) |
| B3-strip-historical-premerge-NEEDS-DECISION.txt | PNG strip of pre-m52 history (59.47 GB unique) |
| C-keep.txt | keep |
| UNKNOWN-or-unbanked.txt | not proposed; some are unbanked accepted evidence |

Strip lists name every copy of a session (alias and _tryN). Run B0 first, or the saving is counted twice.
Stripping means Actual_Frames/*.png only; target_mask PNGs are ~0.05 GB in total and are kept, so MASK-TIE and
TARGET-PIXELS TIE stay recomputable.
