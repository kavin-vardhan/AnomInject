# 082-07c G-COOK — pre-declared before any 082-07c cook ran

Written 2026-09-27 before cook e1 started. Ruling: `_reviews/082-07c-chat-ruling-s1-legs-red.md` item 1 / Sequencing
("G-COOK under the amended rule"); the amended `MainWorld` row is the control-pair form of `_reviews/082-06d-chat-ruling-gcook-fixture.md`.

**Cooks.** Three identical-input cooks, same command as 082-06d (`cook-e1-command.txt`), each archived to a fresh
`E:\IA_BuildCache\_r53_cookout_e1…e3`. **e1 is the candidate; e2 and e3 are controls.** Named now, before any reading.

**Container A** = the archived pre-m53 container `67EA1FE0` (082-06's `iostore-precook.csv`, `_r53_extract\A`), the same
A as 082-06d.

| row | rule | pass iff |
|---|---|---|
| M1″ | M1′ vs M0 and vs M1pre | clean vs M0; M1′ − M1pre = exactly the 4 normal readout materials |
| M2″ | after the cooks and after staging | clean vs M0, and ≡ M1′ |
| map gate | `verify_cooked_maps.ps1` in-process on e1, `-Required` the five maps | exit 0 |
| Q-GATE | `CB_GateLevel` chunk hashes, A vs e1 (e2, e3 reported) | identical |
| Q-MW | `MainWorld` control pair | `.uheader` byte-identical in A, the 13 082-06d cooks and e1–e3; `.uexp` sizes equal; **R = varying(d1…d13) ∪ varying(e1, e2, e3)** — the union of the per-byte differences inside each identical-input set (082-06d's region reused and extended by this session's set); **diff(A, e1) lies inside R at every one of the four 4-byte word alignments** |
| Q-FIX | every fixture source package in M1′ and the 3 plugin assets have an ExportBundleData chunk in e1 | all present |
| Q-NORM (reported, not a gate) | non-shader chunks d1 (082-06d candidate, staged) → e1 | the 4 normal materials change; any other change is listed |

Readings reported beside the verdict, never used to pass: diff(A, e1) against 082-06d's R alone and against
varying(e1, e2, e3) alone; diff(A, e2), diff(A, e3); the region growth.

**If Q-MW fails:** stop. Do not stage. No further cooks this session; the 082-06d "run more identical-input cooks"
route needs its count fixed in writing first, and it is not pre-authorised here.
