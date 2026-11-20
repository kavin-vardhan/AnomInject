# 083-02 — Chat ruling on the disk inventory (083-01): the zero-loss cleanup is approved now; B3, the unknowns and A1 are held for the owner or later

Date: 2026-09-26 (IST) · Author: chat Claude (orchestrator) · The owner asked for action: "we need to do something about it, I barely have space"

## Approved now (083-02 brief)
- **A0 (106.85 GB: E: 79.34, D: 27.51), zero cost.** This covers:
  - the exe-side leg folders that are byte-identical to the bank;
  - the duplicate `Saved\AnomalyCaptures` and Lyra legs;
  - the `.baseline` exe duplicates;
  - the stale pdb;
  - `StagedBuilds` and `Crashes`;
  - the Lyra `.vs`;
  - the UE 5.4-era `Intermediate\Build\Win64\x64` tree on D:.
- **B0 (72.16 GB), lossless.** Replace the duplicate alias and `_tryN` copies in the bank with **NTFS hardlinks** to the canonical copy, after a byte-for-byte check. All names and citations stay valid.
- **B1 + B2 (5.75 GB).** Strip `Actual_Frames` PNGs only, from legs on the superseded `5D40BA28` binary and from invalid pose attempts.
  - Keep labels, sidecars, logs, masks and JSON.
  - Write a PNG hash manifest per stripped session first.
- **Bank the six unbanked legs** (`G4_TEARDOWN`, `G4_TEARDOWN2`, `M52E_LYRA_STUCKMIP`, `M52E_LYRA_STUCKMIP_CO8`, `PB8L_LEGA`, `PB8L_LG9`) **before** any deletion touches their source folders. Move each by copy-verify-delete.
- **NTFS-compress `_binary_baselines`** (about 11 GB). This is transparent; hashes don't change.

## Not now
- **A1 (14.9 GB, StackOBot `Intermediate` + `Saved\Cooked`).** Keep it. m53 Stage 1 needs builds soon, and deleting this would turn each into a 73-min full rebuild. Revisit if space gets tight.
- **A2 (1.75 GB).** Skip; the value is too low.
- **B3 (59.5 GB of pre-m52 history frames).** This is the **owner's call**. Chat's recommendation: keep for now. After the approved cleanup, E: has about 280 GB free, and those frames are the only way to re-verify old milestones at pixel level.
- **Unknown (21 GB).** This is the **owner's call**: his older copies and packages (`Builds\MidRepro`, `Builds\Windows`, the 07-13 `E:\Unreal Projects\StackOBot` copy, the July captures, dashboard smoke folders, a stale clone, `_m47_legs`, `AnomalyIntegrity*`).

## Policy adopted (083-01 §6)
1. Delete the exe-side leg copy once it is banked and the manifest is verified.
2. Bank alias names as hardlinks.
3. At each milestone merge, strip frames from invalid attempts and superseded-build legs, keeping a PNG hash manifest. Accepted evidence keeps its frames.
4. Tear down scratch hosts once their binaries are archived.
5. Delete old engine-version intermediates after an engine change.
6. Compress the baselines; archive only exes that a gate ran on; no stray pdbs.
7. Add a 50 GB free-space floor on E: to runbook §8.6 step 0.

Items 1–2 go into the harness with m53's first bench brief. The rest are recorded in the runbook and gotchas now.

## Owner's decision (2026-09-26, after this ruling)
"Lets keep those both if required". **B3 (pre-m52 history frames) and the Unknown list are both KEPT.** Do not propose them again unless space gets tight.

## Result (083-02 GREEN)
- **E:** free space went from 107 to 273 GB.
- **D:** free space went from 174 to 182 GB. 27.5 GB was deleted, but `D:\pagefile.sys` (60 GB, system-managed) was rewritten during the run. A reboot may return part of the difference; this is unproven.
- **Nothing was lost.** 758 duplicate copies became hardlinks, and every stripped session has a PNG manifest.
- The exe, the container and `m51` are unchanged.
- **Docs:** `b5f15a3` on master (runbook step 0b, gotcha G308).
