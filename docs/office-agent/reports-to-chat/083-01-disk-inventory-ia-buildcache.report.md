# REPORT for 083-01-disk-inventory-ia-buildcache.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 083-01-disk-inventory-ia-buildcache.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-26 17:12:04 | finished: 2026-09-26 17:29:07 | duration: 17 min | exit code: 0
context at last turn: 438484 tokens (44% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\083-01-disk-inventory-ia-buildcache.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

## 1. Summary for the owner
- `E:\IA_BuildCache` is **362 GB**, and every byte of it is real disk space: I found no hardlinks anywhere. It splits into the bench-session bank (231 GB, 99% PNG frames), StackOBot build output (102 GB) and exe archives (29 GB).
- **About 79 GB on E: can go at no cost.** Nearly all of it is exe-side leg folders that are byte-identical copies of sessions already in the bank.
- **Another 72 GB can come back with no information lost** by hardlinking the duplicate alias and `_tryN` copies inside the bank.
- **27 GB on D: can also go at no cost.** It's a leftover UE 5.4 engine build folder that the 5.1 build no longer reads or writes.
- After A plus the lossless and invalid-attempt part of B, E: free space goes from **107 to about 280 GB**. If you also approve stripping frames from the pre-m52 history, it reaches **about 340 GB**.
- Nothing was deleted, moved or renamed. Everything I wrote is under `_reviews\083-01-evidence\` (its `README.md` indexes every file). One stray 3-byte temp file I created by mistake (`C:\ClaudeTemp\x.csv`) was removed.

## 2. Drives
| Drive | Free | Total |
|---|---|---|
| C: | 38.2 GB | 326.6 GB |
| D: | 174.2 GB | 625.7 GB |
| E: | 107.4 GB | 931.5 GB |

## 3. Size table
"Duplicate" below always means the same session id **and** the same list of file paths and sizes, which is the PART TWELVE method.

| Item | GB | What it is | Referenced by | Regenerable / cost | Class |
|---|---|---|---|---|---|
| Exe-side leg folders `Builds\BenchGate\…\Binaries\Win64\*` (675 folders, plus 166 empty) | 59.00 | `run_leg.ps1` output next to the exe (G101) | Bank copies, which are identical, including the loose log and geometry files | Nothing is lost | **A0** |
| BenchGate `Saved\AnomalyCaptures` | 2.19 | Pre-077 test sessions | Duplicated in `PRECOOK077_*` | No cost | **A0** |
| StackOBot `Saved\AnomalyCaptures`, the 27 sessions that are duplicates | 4.26 | Old PIE sessions | Bank | No cost | **A0** |
| `E:\UEProjects\_lyra_legs`, the 37 legs that are duplicates | 5.48 | Lyra harness output | Bank | No cost | **A0** |
| Four `.baseline` exes in Win64 | 0.92 | Same SHA-256 as the copies in `_binary_baselines` | Those archives | No cost | **A0** |
| `Win64\StackOBot.pdb` | 1.83 | Dated 09-04; the staged exe `E0BE6F0A` is dated 09-26 | Nothing | Doesn't match the staged exe, so it can't symbolize it | **A0** |
| `Saved\StagedBuilds` + `Saved\Crashes` | 2.64 | 09-04 cook staging output and crash dumps | The container is archived in `m49b-postcook-container-2AFD2DDB` | Re-created by the next cook | **A0** |
| Lyra `.vs` | 3.08 | Visual Studio index | Nothing | Visual Studio reindexes it | **A0** |
| `D:\UESource\…\Intermediate\Build\Win64\x64` | 27.34 | UE 5.4.4-era intermediates: newest file 06-09, 47 modules with no `Build.cs` in the 5.1 tree | Nothing; 5.1 builds use `Win64\UnrealEditor` | No cost | **A0** |
| StackOBot `Intermediate` | 14.23 | Monolithic Game-target intermediates | The next Game build | About 73 min full rebuild (081-44 measurement) | **A1** |
| `Saved\Cooked` | 0.70 | Cook output | The next cook | The next cook can't be iterative | **A1** |
| Orphan 5.4-era engine DLLs and PDBs | 1.75 | Not loaded by 5.1 | Nothing | Low value; I'd skip it | A2 (optional) |
| Duplicate alias/`_tryN` copies in the bank, plus `B22_PRECOOK_RESCUE` (byte-identical to `PRECOOK077_*`) | 72.16 | A63 banks the accepted attempt twice | Both names are cited | Lossless if hardlinked | **B0** |
| Bank `X5D40_*` legs, frames only | 1.27 | Legs from the superseded binary `5D40BA28` | Journal 074 | Frames can't be recovered | **B1** |
| Discarded pose attempts outside M55/M52/M49/M51, frames only | 4.48 | Invalid A63 attempts | Journals | Frames can't be recovered | **B2** |
| Pre-m52 history (m23–m50, P*, S3*, PRECOOK077, PIE, …), frames only | 59.47 | Milestone gates that later builds have superseded | Journals, and the 081-05..20 bank listings | Frames can't be recovered | **B3 — NEEDS-DECISION** |
| Bank M55* (597 legs) | 85.56 incl. duplicates | c0–c3 requalification, Stage 3 cost, and the 081-43 replay, which reads every M55* folder with `change_evidence.jsonl`, alias and `_tryN` alike | 081-23 to 081-43 ledgers | No | **C** |
| Bank M52* / m49 (A1*, A2*, PB*, M49*) / M51* | 21.94 / 13.16 / 1.93 | Accepted gates; live m51 evidence | Journals, ledgers | No | **C** |
| `_binary_baselines` | 28.63 | 76 exe archives, container quartets, Lyra and editor builds, including the staged `E0BE6F0A` | Its README and the journals | No | **C** (could be compressed, see §6) |
| Staged exe, container, `StackOBot\Binaries`, engine `Binaries` and `UnrealEditor` intermediates, `_reviews`, DDCs | — | The live bench, editor and engine | m51 and upcoming m53 work | Hours to rebuild the engine | **C** |
| Unbanked `G4_TEARDOWN/2`, `M52E_LYRA_STUCKMIP*`, `PB8L_*` | 2.78 | Look like m52/m49 gate legs that never reached the bank | Unknown | No | **Unknown** (bank them) |
| `Builds\MidRepro`, `Builds\Windows`, `E:\Unreal Projects\StackOBot`, `E:\AnomalyCaptures`, `E:\Captures`, Dash smoke-test folders, stale clone, `_m47_legs`, `AnomalyIntegrity*`, other unbanked sessions | 21.2 | m17 repro package, an 08-06 package, a 07-13 copy of the host, July smoke tests, and so on | Unclear | — | **Unknown** |

The DDCs are small (engine 0.8 GB, project 0.4 GB, Lyra 1.0 GB), so they don't need a cap. `D:\AnomalyInjection` is the separate non-intrusive tool, which is out of scope, so I left it alone.

## 4. Totals
| Class | Size |
|---|---|
| A0 | 106.85 GB (E: 79.34, D: 27.51) |
| A1 | 14.93 GB |
| A2 (optional) | 1.75 GB |
| B0 (lossless) | 72.16 GB |
| B1 + B2 | 5.75 GB |
| B3 (needs decision) | 59.47 GB |
| B-compress (estimate) | about 11 GB |
| C, bank | 93.8 GB |
| C, baselines | 28.6 GB |
| Unknown | 24.0 GB |

The bank's C figure is 91.92 GB of unique sessions plus 1.83 GB of files outside session folders. The compression estimate assumes 0.35 compressibility for the exe archives.

- **E: reclaimable from A alone:** 79.3 GB, or 94.3 GB including A1. E: free goes from 107 to 202 GB.
- **E: reclaimable from A + B0 + B1 + B2:** 172.2 GB. E: free goes to about 280 GB, and `IA_BuildCache` shrinks from 362 to about 198 GB.
- **Adding B3:** another 59.5 GB, bringing E: free to about 339 GB.
- **D: from A0:** 27.5 GB, bringing D: free to about 202 GB.

## 5. What breaks or slows down
- **A0:**
  - Journal lines that cite exe-side paths will point at missing folders. The same session ids are in the bank.
  - The next re-stage needs a cook, which is the case anyway.
  - Visual Studio reindexes Lyra.
  - The deleted pdb would only have symbolized an older archived exe.
- **A1:** the next StackOBot Game build takes about 73 min (763 actions), and the next cook is not iterative.
- **B0:** nothing changes for readers. Two cautions:
  - Hardlinked files are shared, so anything that edits a bank file in place edits every link. Proof mutants must copy, which is already the rule.
  - `robocopy` or backup tools copy each link as a separate file, so a copy of the bank grows back to full size.
- **B1, B2, B3:** only `Actual_Frames` PNGs would be removed. Labels, annotation, `run_summary`, sidecars, logs and `target_mask` are kept; masks are only about 0.05 GB. What stops working on those sessions:
  - pixel replays: the A54 oracle, `m44_pairing_probe`, and the picture clauses of `m44_gates`;
  - `verify_capture --label-pixel-gate`, `--black-frame-gate` and the change oracle;
  - `measure_label_offset` and overlay review.
- **B-compress:** transparent NTFS compression. File hashes don't change. Copies are slightly slower.

## 6. Proposed going-forward policy
1. **Delete the exe-side leg copy once the harness has banked it and verified the manifest.** This stops the 59 GB duplicate from growing back. Trade-off: the second copy goes, but it was on the same disk, so it never protected anything.
2. **Bank the alias as hardlinks to the accepted `_tryN`, not as a copy.** Trade-off: the shared-mutable caution above.
3. **Strip frames by retention rule.** When a milestone merges, strip `Actual_Frames` from invalid attempts and superseded-build legs, and keep a PNG hash manifest. Only legs on a named accepted-evidence list keep their frames. Trade-off: those sessions can no longer be replayed at pixel level, although the manifest still proves what existed.
4. **Tear down scratch hosts and worktrees once their binaries are archived.** 081-44 already did this, and none remain.
5. **Delete the old intermediate tree after an engine version change.** Here that is the 5.4 `x64` tree.
6. **NTFS-compress `_binary_baselines` and archive only exes that a gate ran on.** Don't leave pdbs next to the staged exe. Trade-off: slower copies and fewer spare exes to bisect with.
7. **Add an E: free-space floor to runbook §8.6 step 0,** for example GO at 50 GB or more. The bank grows by about 50 GB for each m55-sized campaign.

## 7. NEEDS-DECISION for the owner
1. **B3:** strip frames from the pre-m52 history (59.5 GB)? Under a strict reading of "accepted evidence of a merged milestone" these are protected too. They're classed B only because they sit outside the four sets the brief named.
2. **B0:** accept hardlinks inside the bank (72 GB, lossless)? Or delete the redundant alias or `_tryN` names instead, which breaks citations that use those names.
3. **Unbanked evidence (2.8 GB):** bank `G4_TEARDOWN`, `G4_TEARDOWN2`, `M52E_LYRA_STUCKMIP`, `M52E_LYRA_STUCKMIP_CO8`, `PB8L_LEGA` and `PB8L_LG9` before any sweep. They look like accepted m52/m49 legs that never reached the bank.
4. **Unknown (21 GB):** keep or remove `Builds\MidRepro` (m17), `Builds\Windows`, `E:\Unreal Projects\StackOBot` (a 07-13 copy of the host), the July `E:\AnomalyCaptures` and `E:\Captures` sessions, the dashboard smoke-test folders, the stale clone backup, `_m47_legs` and `AnomalyIntegrity*`?
5. **A1:** take the 14.9 GB now at the cost of a 73-min rebuild, or leave it until space is tight?
api-equivalent cost (not billed on Max): $7.65 | turns: 52 | running total across all briefs: $2,451.90
