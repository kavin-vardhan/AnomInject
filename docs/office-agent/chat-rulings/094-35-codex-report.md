C5 is **not qualified**. The implementation builds, but the street-map clean pre-pass rejected all six poses. The game exited normally after 3,996 paired frames; no anomaly fired. The retry remains unused because no quick tested fix for the rejected poses was established.

The changes port 5.1's rotating census, inclusive 0.5%-25% admission, bounded ledger batches and later candidate discovery ([census policy](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCensus.h:43); [rotating batches](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCensus.cpp:703); [provider seam](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:4378)). Exact flag/stencil restoration uses [the shared ledger](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyStencilTag.cpp:152), with write-mask restoration added in the 4.25 adapter. Capture follows [the player POV](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/AnomalyViewport.cpp:710); forced cameras require the explicit bench switch and remain off by default. Annotation serialization copies [5.1 schema 2.1](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp:1459), with [separate injected and observable membership](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:10219). The opt-in native driver never saves the map.

Focused tests passed: census rotation/capacity and coverage boundaries, reserved-value/restore negatives, view mismatch, annotation membership, static fixture/Rotator checks, and mask-reader synthetic negatives. No in-editor Python changed, so the Python 2.7 harness was inapplicable.

| Build | Start / minimum free commit, GiB | Result |
|---|---:|---|
| Editor 01 | 9.789 / 8.052 | PASS, zero errors |
| Editor 02 | 9.728 / 7.807 | PASS, zero errors |
| Game 01 | 9.770 / 8.283 | PASS, zero errors |

All UBT starts used BelowNormal priority and two parallel actions. No build memory stop occurred. Final native source: `18235f3`; all 27 rebound artifacts match [the size/SHA-256 manifest](D:/IA_Europa/Receipts/09435/c5-425-binaries-01.json). No archive was created.

The game session ran **09:37:12–09:45:39 IST**, exit 0, D3D11, format 18, 1280×720, zero drops. The gate cleared on one reading. Shaders and streaming settled before capture. Six fixed yaw poses used 18 turn, 30 settle, 90 clean-hold and 480 scored-hold frames. References and unstable maps were frozen before injection could start.

| Pose / yaw offset | Unstable pixels | Disposition |
|---|---:|---|
| 0 / 0° | 40.576% | Dropped |
| 1 / 60° | 41.533% | Dropped |
| 2 / 120° | 26.204% | Dropped |
| 3 / 180° | 36.349% | Dropped |
| 4 / 240° | 38.936% | Dropped |
| 5 / 300° | 41.329% | Dropped |

The fixed limit remains 5%. Kept differences show foliage, shadows and particles changing locally, with near-zero global brightness drift. Pose 0 has 13.41% of pixels varying by more than eight levels over its clean hold. The wall pose also has a changing bottom strip correlated with census ownership; a render-proxy/depth-order contribution is unconfirmed. [Diagnostic measurements and material references](D:/IA_Europa/Receipts/09435/prepass-ranges-01.json) and [the difference image](D:/IA_Europa/Receipts/09435/diagnostic-diff-48-137.png) are retained. Exposure and thresholds were unchanged.

| Anomaly | Fired / scored events | C2; IoU, precision, spill, completeness |
|---|---:|---|
| blinking | 0 / 0 | Unqualified; not measured |
| lighting_mismatch | 0 / 0 | Unqualified; not measured |
| lod_corruption | 0 / 0 | Unqualified; not measured |
| missing_object | 0 / 0 | Unqualified; not measured |

Zero comparisons are not a C2 pass. The original reader's C2 subsection was vacuously PASS while its overall result was FAIL. A tested coverage requirement now rejects that case; [the separate re-read](D:/IA_Europa/Receipts/09435/reader-recheck-01.json) preserves the overall FAIL. Real shift/wrong-target negatives remain unqualified. All 3,996 clean frames have zero mask pixels.

Census: first clean rotating samples measured **167 drawn actors** at one held pose, with zero ledger exhaustion. This is a held-view aggregate, not a simultaneous whole-scene readback. The native legacy bounds-visible peak is 20 because it applies the older 6% bounds floor. No scored late-entering target was demonstrated.

Custom depth: all **1,980** component flag/value/write-mask states match before and after; all **3,996** mid samples satisfy ownership. The three non-default setup states restore exactly; reserved value **203** is never allocated. View: all **3,996** rendered/player POV comparisons pass. Annotation: root/schema passes; real event sets are empty, so live membership parity remains unproved despite passing unit tests.

Entry, per-session protection and lean exit checks passed. Protected Content/Config/descriptors/project and protected refs are unchanged; rebuilt artifacts match the bound manifest. No crash or upload sign; the adopted crash monitor exited normally. No root-entry changes, push or tags. One premature runner invocation was rejected before a slot or launch; the armed invocation above was the only game session. [Receipt index](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09435-street-capture-receipts.json).

Commits: `120493b`, `18235f3`, `54e11c9`, and **`2143215`** on `feat/europa-port`, with W4. Worktree clean.

VERDICT: NEEDS-DECISION | All clean poses failed the fixed stability rule; C5 anomaly gates remain unqualified.
