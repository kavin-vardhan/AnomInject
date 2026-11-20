C5 remains **unqualified**. Both targets built, and one street-map session exited normally with **6,336 paired frames and zero drops**. It exposed a preparation/readback ordering bug and unresolved visual failures. The preparation and annotation-provenance corrections are committed and unit-tested, but unbuilt: FM1's three Editor attempts are spent. The retry slot is unused.

Census batches now remain tagged until their own readback, and event targets remain tagged through the event, following 5.1 `AnomalyCensus.cpp:629-699`. Individual restoration follows `AnomalyStencilTag.cpp:237-259`. Host custom-depth actors are excluded and untouched; G498 records this deliberate 5.1 follow-up. Screen share and bounding-box provenance follow `AnomalyLabelWriter.cpp:768-801` and `AnomalyCaptureSubsystem.cpp:10076-10103,10270-10272`; clip planes come from the actual view, with zero denoting infinite far. [Changes, citations and frozen protocol](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/sessions/2026-11-01-094-36-street-capture.md).

The normal 4.25 display path forces grain quantization despite its CVar (`PostProcessTonemap.cpp:215-219,810-819`). FT3's fallback uses seven phase-matched references, keyed by actual renderer `GetFrameIndex(8)` modulo seven (`PostProcessTonemap.cpp:492-506`, `ScenePrivate.h:1047-1051`). Exposure and AA remain runtime-only. Targeted phase, instability, lifetime, host-state, freeze, storage and reader tests passed. No in-editor Python changed; keyword-Rotator checks passed. Post-run tests reproduce the old readiness acceptance and reject incorrect annotation provenance.

| Actual build launch | Start / minimum free commit, GiB | Result |
|---|---:|---|
| editor-01 | 8.046 / 2.888 | Owned memory stop |
| editor-04 | 11.681 / 7.472 | AnomalyCapture: one C2027 |
| editor-05 | 10.129 / 6.323 | PASS |
| game-01 | 11.700 / 2.823 | Owned memory stop |
| game-02 | 8.901 / 5.812 | PASS |

Every UBT ran BelowNormal with two actions. `ScenePrivate.h` fixed the C2027; final builds had zero errors. Memory stops reached only owned trees. Admission-only waits did not launch UBT. Built source: `9a785e0`; [27 bound files and SHA-256s](D:/IA_Europa/Receipts/09436/c5-425-binaries-01.json) match at exit. No archive was created.

The gate waited 57.6 minutes. The session ran **15:11:47-15:23:01 IST**, exit 0, D3D11, format 18, 1280x720. Each declared census-on hold contains one off frame, at indices 288, 816, 1344, 1872, 2400 and 2928: **all six protocol checks fail**. These image measurements remain diagnostic; "Kept" means the driver used that pose. The frozen clean threshold is 0.1% changed stable pixels.

| Pose | Stable share | Self-check / maximum | Interference: maximum pixels / share | Driver |
|---|---:|---|---|---|
| 0 | 33.6075% | FAIL / 36.4566% | FAIL: 1168 / 0.3771% | Dropped |
| 1 | 70.7297% | FAIL / 0.3905% | FAIL: 1830 / 0.2807% | Dropped |
| 2 | 85.3928% | PASS / 0.0309% | PASS: 659 / 0.0837% | Kept |
| 3 | 52.2540% | FAIL / 2.9932% | PASS: 55 / 0.0114% | Dropped |
| 4 | 77.4797% | PASS / 0.0010% | PASS: 3 / 0.0004% | Kept |
| 5 | 69.8015% | FAIL / 0.1023% | FAIL: 780 / 0.1213% | Dropped |

Freeze chronology passes against the first scored image's write time. No z-fighting exception was used. At the wall pose's worst interference frame, rows 686-719 have zero changed stable pixels; no alternating equal-depth winner proof was established or exclusion added. [Residual images and measurements](D:/IA_Europa/Receipts/09436/diagnosis-01.json).

All four anomaly gates fail. "Scored" below means FT3-eligible by hold and stable-target coverage, independently of visual success. Complete visible apply/revert pairs are respectively 0, 1, 0 and 0. Metrics use stable pixels only; dashes mean no qualifying pixel-active comparison.

| Anomaly | Fired / scored | C2 mismatch / ambiguous | IoU min | Precision min / median | Spill max | Completeness min |
|---|---:|---:|---:|---:|---:|---:|
| blinking | 14 / 9 | 13 / 41 | 0.0000 | 0.0000 / 0.9966 | 1.0000 | 0.0000 |
| lighting_mismatch | 6 / 6 | 15 / 47 | 0.0000 | 0.0000 / 0.0944 | 1.0000 | 0.0000 |
| lod_corruption | 12 / 10 | 130 / 0 | - | - / - | - | - |
| missing_object | 4 / 0 | 0 / 0 | - | - / - | - | - |

C2 totals: **960 scored hold frames, 365 mismatches, 88 ambiguous comparisons**; 207 mismatches are outside the event rows above. All clean-labelled frames have zero mask pixels. Lighting's maximum measured precision is only 0.2022. Full distributions and 128 reference issues remain in the [separate re-read](D:/IA_Europa/Receipts/09436/reader-final-02.json); original verdicts and sealed evidence are unchanged.

Neither negative is qualified for all four routes. Shifted masks are rejected on 43/80/130/0 frames; wrong-target masks on 67/27/0/0, in table order. Missing-object has no eligible event; LOD lacks a pixel-active wrong-target comparison.

The rotating held-view census peaks at **141 drawn actors**, with zero ledger exhaustion. Late-target qualification fails because the first scored view has yaw 300 instead of the declared pose zero. The tested `Prepared == Next` guard prevents capture after a readback changes readiness but before the driver prepares that index.

Custom depth passes: **1,980** before/after components, **6,336** mid samples, zero restoration or host-state differences. Runtime test states were off/value19, on/value203, and on/value18/write-mask2; reserved value203 was never allocated. Player/render view agreement passes all 6,336 frames.

Annotation schema, affected/injected sets, screen share, bounding-box source and clip planes pass for 36 events. Selection provenance remains unrecorded: `coverage_pct` is -1. 5.1 freezes it at `AnomalyCaptureSubsystem.cpp:8643` and exports it at 10271; it is not 100 times mean screen share. The source now records the shared evaluator's value and validity. Its focused negative passes; the saved annotation stays unchanged and unqualified.

Entry, session protection and lean exit pass. **No crash, upload sign or protected change.** The adopted monitor exited normally; drive-root entries are unchanged. Bench view, pacing and custom-depth state restore; no saved settings changed. Commits: `b1710b7`, `a4a0df6`, `9a785e0`, unbuilt fixes `05c55e2` and `2b9dc19`, closeout **`df5dfdc`**, with W4. Worktree clean; no push or tags. [Receipt index](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09436-street-capture-receipts.json).

VERDICT: NEEDS-DECISION | C5 remains unqualified; tested source fixes require a new build budget and measured rerun.
