---
name: bench-bank-is-a-junction-to-e
description: "D:\IntrusiveAnomalies\_bench_sessions_bank is a junction to E:\IA_BuildCache\_bench_sessions_bank; hardlinking from it needs a destination on E:"
metadata:
  type: reference
---

`D:\IntrusiveAnomalies\_bench_sessions_bank` is a **junction** to `E:\IA_BuildCache\_bench_sessions_bank` (found 084-05b,
2026-09-29). `os.link` from a bank file to anywhere on D: fails with WinError 17 ("cannot move the file to a different disk
drive"); a stub/dry-run root that hardlinks banked sessions must live on E: (084-06 dry runs used `E:\IA_BuildCache\_r56_dry_*`).
Alias hardlinks inside the bank itself are fine (same volume). Related: [[scratch-host-build-when-checkout-is-protected]].

**`D:\IntrusiveAnomalies\_binary_baselines` is ALSO a junction**, to `E:\IA_BuildCache\_binary_baselines` (confirmed 093-14,
2026-10-16). Every archive written there lands on E:, so a free-space check on D: is the wrong check. In 093-14 Codex's
archiver drove E: from ~57 to 51.6 GB (its floor is 50 GiB), stopped it, and rebuilt the archive on physical
`D:\IA_BankOverflow\…` with a junction back under `_binary_baselines`. A 21.4 GB INCOMPLETE partial attempt was left on E:
(its deletion was refused by the sandbox approval policy). Check E: headroom before any job that cooks or archives.

**As of 2026-11-04 (094-43a FZ9 search) the bank target `E:\IA_BuildCache\_bench_sessions_bank` is EMPTY, and so is `E:\IA_BuildCache\StackOBot\Saved`.** Banked 5.1 session data that survives at home lives in `_reviews\0xx-evidence\` copies, `E:\UEProjects\_lyra_legs\*.log`, `m079_shift_*` and relay run receipts; do not assume a banked leg still exists.
