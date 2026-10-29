# 094-26b — UBT guard, memory limits and native smoke

FD2/FD3 qualified; Smoke exposed two unavailable 4.25 anti-aliasing properties; no recipe ran and no package was created.

FD2 now detects actual build invocations, compiler/linker/UHT children and build-output writes; it admits non-build UBT modes without an allowlist. FD3 uses 6 GiB physical and 8 GiB commit at admission, and 4 GiB commit in-session. The 43 FD cases, affected FC/EY/session/gate/environment suites and six changed-check stand-in cases passed against the committed tooling. In-editor scripts were unchanged, so their Python 2.7 harness was not rerun. FD5 required zero dry runs.

The one Smoke editor exited normally with code 0 after 23.169 seconds. The retained fixture module remained alive. The kit stopped after SMOKE_FAIL; EA_Core and EA_Travel were never started.

Native smoke inspection found PostProcessSettings.override_anti_aliasing_method and anti_aliasing_method absent. The unchanged A5 authoring recipe sets both. FC3 stops progression on failed smoke. This job made no recipe fix or retry after that result. A compatible recipe change and fresh authorization/re-arm are required before another run.

The probe recorded one skipped cleanup and one probe_stop with no PIE. Both log upload checks passed. The only Saved delta was an engine-managed autosave metadata timestamp; all Content/config and build-output hashes stayed equal. Marker 31124 stayed unchanged. Five owned processes exited, including the ValidatePlatforms child and crash monitor tree.

Full P9 v2 entry/exit proofs passed. The receipt index records the launch-second canary result and its exact read gaps, audit, hashes, unchanged packages and EX3 empty quarantine. Final report.md binds later documentation/audit tails. No anomaly behavior, recipe, engine source or binary changed; no push or tags.
