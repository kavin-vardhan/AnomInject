094-33c: all four positive C3 gates and same-session C2 pass after an exposure-matched re-read. **Lighting's real wrong-target negative still passes incorrectly**, so C3 remains NEEDS-DECISION. One session ran; the retry is unused.

Blinking now selects only hidden, installed targets. Clean frames record `mask_state=empty`, `mask=null`, and write no PNG, matching [m44 G1-G4](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/predictions/2026-09-03-m44-m45-target-mask-onset-and-hidden-class.md). The old source fails the focused test. All **93 clean-labelled frames have zero mask pixels**; 147 nonempty masks remain.

Precision counts mask pixels with an RGB change greater than one level. Synthetic wrong-target tests fail on precision alone for all four anomalies; the grain-boundary and Rotator checks pass. `-ExecCmds=r.EyeAdaptationQuality 0` locks exposure: [ShowFlags.cpp:368](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/ShowFlags.cpp:368) disables adaptation; [PostProcessEyeAdaptation.cpp:307](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Private/PostProcess/PostProcessEyeAdaptation.cpp:307) fixes exposure to 1.0. Every frame records quality 0; no Config edit.

Both FM1 builds passed first attempt, BelowNormal, two actions, zero compile errors or memory stops. Editor start/minimum free commit: **10.032/8.641 GiB**; Game: **10.038/8.695 GiB**. [Builds and complete binary hashes](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09433c-mask-precision-receipts.json).

The session exited normally: 240 paired 1280x720 frames, D3D11, format 18, AA 0, zero drops; three events per anomaly. The initial reader rejected the old exposure references. That FAIL remains intact. Directly inspected, hash-bound matching references yield the following separate [re-read results](D:/IA_Europa/Receipts/09433c/reread-01/masks.json), with unchanged thresholds:

| Anomaly | Anomalous frames | IoU | Precision min-max | Required | Spill | Positive gate |
|---|---:|---:|---:|---:|---:|---|
| blinking | 21 | 1.000 | 0.9391-0.9400 | >=0.90 | 0 | PASS |
| missing_object | 42 | 1.000 | 0.9396-0.9408 | >=0.90 | 0 | PASS |
| lod_corruption | 42 | 1.000 | 0.9786-0.9793 | >=0.80 | 0 | PASS |
| lighting_mismatch | 42 | 1.000 | 1.0000 | >=0.80 | 0 | PASS |

One-frame mask shifts fail for all four. Real wrong-target masks fail for three; **lighting accepts all 42**. The substituted cube is itself an affected receiver, so precision is 1 and spill 0 despite IoU 0.1594. FQ2 has no lighting completeness/IoU requirement. Rejecting this counterexample needs a ruled completeness or identity check; another identical session cannot resolve it. [Counterexample and evidence](D:/IA_Europa/Receipts/09433c/diagnosis.json).

[C2 re-read](D:/IA_Europa/Receipts/09433c/reread-01/labels.json): 240/240, zero mismatches or ambiguity, three apply/revert events per anomaly and six interior blink toggles. Apply, revert and blink label-shift negatives each fail. Raw files remain unchanged.

Crashes, upload signs and protected changes: **none**. Entry/exit protected files and refs pass; all 27 bound files match. Stencil and pacing restored; the exposure override ended with the process. No archive, push or tags.

Commits on `feat/europa-port`: `4147dcc` (native fix, precision reader and tests), `03bdef0` (reference binding and closeout).

VERDICT: NEEDS-DECISION | Positive C3 and C2 pass; the frozen lighting rules admit the real wrong-target subset.
