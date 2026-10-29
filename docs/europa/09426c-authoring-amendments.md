# 094-26c prediction addendum — 2026-10-30

Written at 2026-10-29T23:21:57.025145+00:00 before tests, re-arm or runtime. Authority: 094-26b ruling FE1, FE2 and FE4.

The sole recipe behavior change removes override_anti_aliasing_method and anti_aliasing_method from the post-process setter list. The six exposure/motion-blur entries, every actor, geometry, light, camera, map and output path remain identical. The A5 recipe binding is revised in 09426c-a5-fixture-manifest.json; the 27/39/44 binary manifests remain frozen. AA-off is capture-time work for A7, not this job.

Prediction: the actual Python 2.7.14 harness records 90 Core and 87 Travel calls, no asset writes; the runpy-dictionary mutant fails. The updated Smoke should contain exactly the 51 present checks in Receipts/09426b/fixture-attempts/Smoke/01/smoke.json and pass with zero create/load/save calls. Existence checks do not prove all native call semantics: any real failure ends the run with no repair or retry. If authoring completes, Core adds only its map and two-LOD mesh, Travel adds only its map; each clean editor exit has one probe_stop and a skipped cleanup row, no PIE, crash, upload or protected change. The receipt remains AUTHORED_NOT_QUALIFIED.

FE2 excludes only event logs under Windows/System32/winevt/Logs that the OS denies read access to; readable logs are scanned, other denied paths remain gaps. Count and folder are recorded on every scan. FD2/FD3 remain byte-identical to 09e776b; FE3 changes belong to 094-27. FC2 launcher, FC4 privacy switches and FC5 file classes are unchanged.

Fresh 09426c slots authorize one Smoke, one Core and one Travel. Old armings refuse. No dry runs (FE4/FD5). FA5 requires 600 seconds idle before each start; the measured gate, 6 GiB physical/8 GiB commit admission, 4 GiB commit stop and disk lines stand. No new session after 2026-10-30T02:15:00Z (07:45 IST); an existing session may finish. No push, tags, build or global setting change.
