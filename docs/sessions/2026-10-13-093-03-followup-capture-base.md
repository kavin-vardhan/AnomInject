# 093-03 J1 follow-up integration — 2026-10-13

Status: **NEEDS-DECISION**, stopped on a failed F6 can-fail expectation. No delivery,
natural mechanism, F1 WIP or new label key. No head was pushed.

## Integration

Created `E:/IA_BuildCache/_r933/AnomalyInjector` from the existing `_r53_src` worktree,
on `fix/followup-capture` at release `d7eee97`. Merged `5d6352b`, not cherry-picked.
The only conflict was `CLAUDE.md`; the dated release and 091-05 follow-up status
blocks were both retained. Merge commit: `72f3eb3e2df1dff5587716fcd5bc0a650ae8000c`.

`AnomalyLabelSync.h` defaults remain 0/0/0. The existing m52 window test passes all
316 assertions, including explicit 3/40/1 overrides. For `Source Shaders tools`,
`5d6352b..72f3eb3` equals `d660b5d..d7eee97` after excluding Git blob-index lines;
all patch hunks and offsets are identical. Both normalized patches have SHA-256
`439b778cc84570a7fa02cfa52799ba3777e22758e2e1a436eeb2e7aa695d1e41`.
Final journal/status edits change documentation only.

## Builds and static proof

Host: `E:/IA_BuildCache/_r53_host/StackOBot`, plugin detached at `72f3eb3` for builds.
Normal and strict Editor/Game builds pass with zero errors and warnings. Strict
builds compile 69 independent header translation units per target and verify
response-file levers. Build.cs is restored by hash. All 15 C++ CPU programs, 51 Python
tests, 18 existing mutants, five additional default/override/ownership mutants,
and 98 label-pixel reader cases meet their declared expectations. Source lever
audit finds 53 names/53 registrations; all 12 audit negative/self-tests pass.
Source plus EXE/five-DLL audit passes. String review accounts for 1,462 changed
printable sequences, with two manual byte-level resolutions and zero unresolved.
Those two apparent UTF-16 prefixes straddle ASCII `.cpp`/UTF-16 boundaries; the
adjacent literals already exist in the release. Planted string and changed-byte
archive controls are detected.

EXE SHA-256: `e41abf93bd6abc231deb7f93ace6594784ee74348b4db737cbdd465a0d3dd37f`.
The EXE and five editor DLLs are frozen in `E:/IA_BuildCache/_r933/build-manifest.json`.
Archive: `D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J1-72f3eb3-E41ABF93`.
All six archive files rehash correctly. Its manifest/README mark qualification
incomplete. The F1 guard is present and WIP absent; J2 must not infer J1 completion.

## Frozen predictions and engine evidence

`docs/predictions/093-natural-foundation-miss.md` was written before any engine leg;
candidate hashes and the four-attempt first-leg schedule were bound before launch.
Evidence root: `E:/IA_BuildCache/_r933`. Both launches pass fresh idle receipts.
No foreign process was terminated and no security setting was changed.

Only two PIE captures ran, both new-window PIE with two forced realtime editor-world
views, native AA off, UV tile on `StaticMeshActor_0`. Candidate `F1_P_uv_aaoff` has
1,152 rows/images, four visible fires, 192 labelled frames with measured masks,
and a game-only marker in all images. Pixel onset and recovery are 0/0 for every
fire. Accounting is 192 measured + 960 unavailable = 1,152 written rows.
`verify_capture` reports four CONSISTENT events, no offset note and no unassessable
event. These are UV-only results, not a qualification of the full matrix.

Shipped-release control `F1_P_uv_release_control` uses archived `218BA4C7` source
`23098fa` and its five editor DLLs, rehashed 6/6, replacing deleted 091-05 archives.
It writes 1,152 pictures with the marker absent in all of them, including all 192
labelled frames. The F1 ownership can-fail is reproduced. Its missing masks and
pixel failures are expected consequences, not candidate passes. This already-running
control completed normally before the stop's shutdown check; no later leg launched.

## Stop: invalid F6 can-fail isolation

In a copy of the candidate capture, session index 191 changed from
`uv_corruption@307|StaticMeshActor_0` to `uv_corruption@999999|WrongTarget`.
The intact capture passes the new hook. The corrupted copy fails the new CLI
(exit 1) and hook, but also fails the old release hook with
`labels event_id mismatch at 191`. Prediction required the old hook to pass.

The selected row was unflagged, but the capture retained four `pie_end_settle`
transitions, one for each event. This does not isolate the transition-free defect.
`controls/F6_F1_P_uv_aaoff/result.json` and `F6-gate-stop.json` preserve the failed
comparison. Proof stopped without altering the input or acceptance rules or running
a replacement control. This is a harness/evidence limitation, not an established
production regression. A decision to continue is needed for a valid transition-free
F6 control and the unrun matrix.

Unrun: proxy ownership, F2 scope/tag-retirement engine proof, F3 alpha pair, deliberate
clean/active dropped-mask samples, hold/proxy/normal regression, temporal diagnostics,
synthetic-order checks, copied-image/mask controls, and all staged proofs. The retained
host staged container lacks the required scope/alpha/hold maps; a future staged proof
needs a new bench cook. This job did not cook or stage anything.

## Settings, deviations and restoration

Process arguments: `-IAIBench -NoLiveCoding -unattended -nosplash`. Per-capture settings:
CaptureBench.Marker=1; SVE=1, Delivery=0, Pace=1, Fps=30, Mask=1, TargetMask=1,
RunLog=1, FocusGate=0; AA method=0, auto exposure=0, eye adaptation=0,
Lumen diffuse=0, dynamic GI=0, reflections=0. Config `0 192 48 192 4`, PNG seed777,
cap1400. Temporal CVars stayed at -1, resolving to 0/0/0. Review0916 `PIE multi`,
`Observe` and `Report` reuse the existing bench helper only; no LateStart or Skip
command, synthetic tick-order lever or product WIP was used. Play settings were
transiently duplicated for a 1280x1024 requested window; game viewport metadata
reported 1280x992, while captured PNGs were 1123x870. Two editor views were forced
realtime. Process exits and configuration restoration remove these settings.

The independent in-scene barcode replaces the deleted green-plane ownership harness;
four attempts were predicted in advance to require at least three visible fires.
No old 091-05 result is inherited as new proof. Its temporal 3/40/1 settings are
replaced by the mandated release 0/0/0 defaults.

Pre-proof harness corrections are retained: MSVC command quoting/PATH and a missing
mutant include path; deliberate constexpr mutant C4702 after disabling optimization
did not resolve it, so only negative fixtures use `/wd4702` (production builds keep
their strict flags); own slow PE scanner was stopped by PID and restarted with an
equivalent indexed range lookup; string controls now scan separate ASCII/UTF-16
buffers after an invalid concatenated-buffer exact-match assertion; an initial
relocation-prefix hypothesis was rejected by byte inspection; idle sampling accepts
only latest valid counter status 0/1 and rejects unknown live samples. None changes
an engine acceptance threshold. The F6 control was not repaired after the stop.

`receipts/initial.json` backs up 201 original files. `receipts/host-restored.json`
verifies all 201 byte-identical, source detached at original
`23098fa7415f148e935acbd4a97e18da04e6c46c`, clean, and removes only two newly created
CrashReportClient INIs by receipt. Both editor processes exited 0; directory names
are not evidence of a crash. Ordinary generated caches/logs/intermediates remain.
The release-DLL control swap was restored to candidate before final original-host
restoration. Staging was never mutated. No master/release/tag/main-checkout write,
no natural branch, no delivery and no push occurred.

Full report: `D:/IntrusiveAnomalies/_relay/runs/2026-10-13-093-03-j1-natural-base/report.md`.

VERDICT: NEEDS-DECISION
