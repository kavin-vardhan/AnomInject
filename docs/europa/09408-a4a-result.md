# 094-08: A4a clean 4.25 and strict qualification

Authority: 094-07 ruling EH1-EH6. Qualified source is `489c82817108ac791171a20e2203df3e89f9027e`. The results commit changes documentation and the reach manifest only; it is not a separately compiled SHA.

Gate A1, gate A2 and gate A4's build-level Shipping exclusion pass on 4.25. The EH5 ReGate51 Editor Development working compile passes at the same source SHA. The complete receipts are under `D:/IA_Europa/Receipts/09408`; the run report records protected state, operation ledgers, memory admission, all build commands, warnings and hashes.

| Build | Target/configuration | Project / plugin / engine / engine-plugin actions | Exit | C++ warnings |
|---|---|---:|---:|---:|
| editor-final | StylizedParisStreetEditor Development | 11 / 24 / 0 / 0 | 0 | 0 |
| game-final | StylizedParisStreet Development | 7 / 9 / 0 / 0 | 0 | 0 |
| shipping-final | StylizedParisStreet Shipping | 5 / 0 / 0 / 0 | 0 | 0 |
| regate-final | StackOBotEditor Development | 0 / 4 / 0 / 0 | 0 | 0 |

Every 4.25 compile response uses compiler product 14.16.27051 and SDK 10.0.17763.0. Plugin responses use /std:c++17 without PCH. The two accepted user XML warnings remain environment warnings; no user XML was changed. Final hashes, including module manifests and target receipts, are in `qualification/final-425-binaries.json`; A6-A8 must use those artifacts. Shipping has no IA or probe module action or compiled product. The .uplugin remains a UFS runtime dependency in the host receipt: packaged/content exclusion is a later proof.

Strict attempt 4 passes for Editor and Game: 64 ordinary C++ sources, 77 header TUs (public, private and reflected), and 11 UHT generated TUs per target. Native UHT output directories determine the generated-file denominator. Plugin unity and PCH are off in actual responses. The actual missing-defining-include mutant fails with the predicted C2027; /Yu, unity, zero-coverage, missing-response and missing-generated-action reader mutants reject. All five Build.cs files restore byte for byte, generated test sources are removed, tracked status restores, and both normal rebuilds pass. Canonical evidence: `strict/attempt4/summary.json`.

The sole production change in A4a is UObject/WeakObjectPtr.h at its use site in AnomalyUnsupportedTexture.cpp, inside ANOMALY_ENGINE_425. Its 5.1 effect is N: enabled 5.1 tokens are unchanged. It fixes strict C2504 from the weak-pointer array reset. No reflected surface, dependency, C# rule, descriptor or host .uproject changed permanently. The exact restored rules and unchanged project hashes mean the full JsonExport matrix/mutant rerun was not triggered; fresh selection exports were recorded on both engines.

Earlier evidence remains intact: attempt 1's import-path failure occurred before UBT; attempt 2 found the defining-header omission; attempt 3 compiled both targets but exposed resource-action and UHT-directory reader errors. Both reader issues were corrected with 27 synthetic cases and immutable-evidence revalidation, then the entire strict sequence was repeated successfully. No gate denominator was reduced.

A4b still owns 5.1 baseline/candidate Editor, Game and Shipping qualification and the real 5.1 strict pass. This job does not authorize those targets in its wrapper. The probe is still the empty A3 skeleton. Runtime world/tick, visual, packaged/cooked and delivery claims remain later work. No runtime launch, cook, UAT, push or tag occurred.
