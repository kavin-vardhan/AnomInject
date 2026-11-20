# 082-07f G-COOK — pre-declared before any 082-07f cook ran

Written 2026-09-28 before cook f1 started. Ruling: `_reviews/082-07e-chat-ruling-masked-fixture.md` (Sequencing: re-cook,
G-COOK under the amended rule). The `MainWorld` row is the control-pair form of `_reviews/082-06d-chat-ruling-gcook-fixture.md`,
as used in 082-07c (`082-07c-evidence/gcook-predeclare.md`).

**Cooks.** Three identical-input cooks, same command as 082-06d / 082-07c (`082-07c-evidence/cook-e1-command.txt`), each
archived to a fresh `E:\IA_BuildCache\_r53_cookout_f1…f3`, run with `MSBUILDDISABLENODEREUSE=1` and `WaitForExit` (G316).
**f1 is the candidate; f2 and f3 are controls.** Named now, before any reading.

**Container A** = the archived pre-m53 container `67EA1FE0` (082-06 `iostore-precook.csv`, `_r53_extract\A`).

| row | rule | pass iff |
|---|---|---|
| M1‴ | M1‴ vs M0 and vs M1pre | clean vs M0; M1‴ − M1pre = exactly `CaptureBenchTexCorrupt/Materials/M_TC_Masked.uasset` |
| M2‴ | after the cooks and after staging | clean vs M0, and ≡ M1‴ |
| map gate | `verify_cooked_maps.ps1` in-process on f1, `-Required` the five maps | exit 0 |
| Q-GATE | `CB_GateLevel` chunk hashes, A vs f1 (f2, f3 reported) | identical |
| Q-MW | `MainWorld` control pair | `.uheader` byte-identical in A, d1–d13, e1–e3 and f1–f3; `.uexp` sizes equal; **R = varying(d1…d13) ∪ varying(e1…e3) ∪ varying(f1…f3)**; **diff(A, f1) lies inside R at every one of the four 4-byte word alignments** |
| Q-FIX | every fixture source package in M1‴ and the 3 plugin assets have an ExportBundleData chunk in f1 | all present |
| Q-MASK (reported, not a gate) | non-shader chunks e1 (082-07c candidate, staged) → f1 | `M_TC_Masked` changes; any other change is listed |

Readings reported beside the verdict, never used to pass: diff(A, f1) against the 082-06d + 082-07c region alone and
against varying(f1, f2, f3) alone; diff(A, f2), diff(A, f3).

**If Q-MW, Q-GATE, Q-FIX or the map gate fails:** stop. Do not stage. No further cooks this session.
