# 09428 authoring fixes

The Python 2.7 launcher now retains its fixture module. A recording harness exercises all 4.25 in-editor sources, the smoke path and both complete recipe sequences; the observed runpy failure is a can-fail. Both usage analytics and unattended crash reports are disabled by required command-line privacy overrides. Smoke precedes recipe authoring, records API availability without creating assets, and must prove clean exit and no upload.

FC5 managed files are recorded separately; FC6 tracks owned UBT argv and rejects build actions. The retained suites pass, with FA1 alias tests using simulated reparse attributes instead of creating links. See `docs/europa/09428-authoring-amendments.md` for source citations, scope and limits. Scratch rehearsal, re-arming and real runtime evidence are pending at this tooling commit; the final receipt index will record their outcomes. No push or tags.

## Closeout — 2026-10-30

NEEDS-DECISION. Smoke stopped by an over-strict UBT guard; no recipe started and no package was created.

The real Smoke editor ran for 9.547 seconds, then the kit stopped its owned tree on -Mode=ValidatePlatforms -allplatforms. The guard admitted only QueryTargets. No smoke receipt or probe exit rows were produced. Both recipe slots remain unused; no retry or fix followed the failed Smoke.

4.25 TargetPlatformManagerModule.cpp:996-1031 launches this SDK-status check; ValidatePlatformsMode.cs:35-60 checks registered platform SDK status and emits VALID/INVALID lines. The command was not evidence of a build. No compiler/linker/UHT was recorded and build-output hashes are equal. All protected files, 1458 existing Content files, 110 bound artifacts, manifest hashes and marker 31124 are unchanged. No upload, crash, ensure or new crash folder was found in the retained 9.547-second session; the forced stop means a clean complete-session privacy proof is still missing. FC3 ends the run after failed Smoke. A future correction must account for the stock validation mode and obtain a fresh authorized Smoke; this job does not re-arm or retry it. A6 authoring and A7 pixel qualification are not complete.

The receipt index binds preparation, rehearsal, arming, dry-run, live and real-session evidence. The run-folder report carries final audit and canary boundaries. This closeout changes documentation only, with actual W4 dates; source remains c2e7318. No push or tags.
