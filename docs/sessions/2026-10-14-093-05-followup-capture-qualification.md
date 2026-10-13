# 093-05 J1 qualification completed — 2026-10-14

Status: **GREEN for the specified home integration gates**, on `fix/followup-capture`.
This is not a delivery. Product source remains merge `72f3eb3`; EXE E41ABF93 and the five
editor DLLs are unchanged. No Source, Shaders, tools, natural mechanism, F1 WIP or label
key change was made. Predictions remain byte-identical. The owner decides delivery;
J2 follows here, and natural work starts on its separate branch after J2.

## Goal and work done

Binding ruling 093-04 D2 distinguishes an implementer harness defect from a product gate
failure. The remaining PIE matrix ran before the corrected bench cook, followed by the
staged matrix. Each launch has a fresh idle receipt and binary identity; staged launches
also re-hash all five containers. All qualifying native AA-off picture edges are zero
frames off at 0/0/0. At least three visible events remain mandatory. Forty-three new
launches include controls, diagnostics and the retained failed helper input. Earlier UV
ownership and corrected PIE F6 evidence remain valid. `qualification.json` indexes 196
successful checks; raw logs and negative findings, rather than that index alone, are evidence.

Evidence root: `E:/IA_BuildCache/_r933/continuation09305`. Full report:
`D:/IntrusiveAnomalies/_relay/runs/2026-10-13-093-03-j1-natural-base/report.md`.

| Gate | PIE | Staged | Can-fail |
|---|---|---|---|
| F1 ownership | UV and shared proxy: 1,152 pictures each, four visible fires, 192 measured labelled masks; game marker in every picture under two forced editor-world draws | UV four/192; proxy three/144; game marker in every picture | Shipped-release PIE loses all 1,152 markers in each mode and all labelled masks; erased-marker copies fail in PIE/staged; pure wrong-owner/family controls reject |
| F2 scope/retirement | UV/normal four fires each; C1 mask 50,176 pixels, C2 zero; C2 retired and raw state restored | UV/normal four each; C1 65,536, C2 zero; actual console snapshots show C2 retired and exact material/depth/stencil restore | Release PIE and staged NoPartScope include C2; 20-level C2 changes fail in both; stale/unowned CPU controls and copied after-state stencil controls reject |
| F3 alpha | Opaque zero events/four refusals; masked four visible/192 measured | Same | Release admits four invisible opaque events in each mode; always/never-alpha CPU mutants fail opposite halves |
| F6 | Existing capture's unflagged row 191, intact old/new pass | New staged row 191, intact old/new pass | Consistent start_frame=999999/target_name=WrongTarget/event_id: old hook misses, new hook/CLI reject missing annotation; event-ID-only is rejected by both |
| Dropped masks | Three clean and three active samples deliberately skipped; 1,149 written rows, 192/189 measured labels; separate engine-command helper also proves three drops | Three clean and three active samples; 1,149 rows, 192/189 measured | Summary total +3 and dropped counter zero reject; no dropped image/row/annotation or orphan mask outcome |
| m52 hold | Four visible fires, 220 measured labelled masks; E1 strict/t10/t50 onset and recovery all zero | Four fires, 196 measured; all E1 edges zero | NoHold null has zero held/events in 1,100 frames; frozen-target pictures fail, both modes |
| Shared proxy | Four visible/192; 20 unchanged measured peers, masks/recovery, raw restore | Three/144; all 20 measured peers pass in separate Marker=0 capture; masks/recovery pass | Frozen target and 20-level shifted peer rejected, both modes |
| UV/normal and co-users | Four fires per configuration, 192 measured labels, zero edges; named shared-material peer unchanged | Same | Frozen target/wrong peer copies rejected in both modes |
| Label pixels | Actual same-size masks, exact counts/bounds and every labelled frame visible; zero native edges | Same | Onset/offset +/-1, missing/blank/wrong-count mask, physical mask-pixel deletion and erased marker rejected in each mode |
| Synthetic order | Identity/membership and mask/image joins pass | Same | Wrong-ID membership controls above test the same reader; synthetic pictures do not certify pixel timing |

The reader is unchanged. On wrong-view release captures, `verify_capture.py
--label-pixel-gate` exits **0 with four UNASSESSABLE events**; rejection comes from independent
marker and measured-mask checks. Shifted labels produce exit 0/offset notes; the frozen
strict zero-edge predicate rejects them. Alpha-release invisibility is likewise rejected
by the independent picture check, not a claimed reader exit failure. Positive labelled
masks are measured, never projected substitutes. Only negative-copy shift controls supply
the fixed-view target mask on an added boundary row, to make that synthetic edge assessable.

TSR uses 0/0/0 and remains diagnostic as predicted. Hold/proxy t50 recovery tails are 8-9
frames in PIE and 7-8 staged; strict/t10 tails are larger. Peer mean/mask checks pass for
proxy and both co-user types under TSR. No temporal zero-frame claim is made. Capture-size
cause for original 1123x870 PNGs versus 1280x992 metadata from a 1280x1024 request: **unknown**.

## Cook, archive and restoration

The repaired `-CookOutputDir=.../continuation09305/cooked/Windows` allows UAT BuildCookRun
to cook, package, stage and archive successfully in 41.976 seconds. Development Win64,
`-skipbuild -nocompile -nocompileeditor`, twelve existing fixture maps and `-IAIBench`
were retained. All thirteen required map names including Entry are found; an additional
absent required-map control exits 1. A copied container byte mutation is detected and
never launched. No security block occurred. No product rebuild was needed after 093-03.

New add-only archive:
`D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J1-09305-E41ABF93-5A39646C`.
Its eleven files are the unchanged EXE/five DLLs and five cooked containers; each re-hashes.
The manifest gives every full SHA-256; container prefixes are pak D146D10B, project utoc
5A39646C, project ucas 10259986, global utoc 508E61F0 and global ucas F80A5DA4.
This is J2's guard-present/WIP-absent control. The older E41 archive is preserved; its
historical F6 explanation is superseded by G477 and the corrected PIE/staged controls.

Host restored to detached `23098fa7415f148e935acbd4a97e18da04e6c46c`: all 201 original
host files and all 1,327 original live-stage files match. All 2,756 content files are
unchanged. Job-created host/stage files are removed only by exact-path/hash receipts;
generated cook output and evidence remain. All launched game/editor processes ended by
their own PIDs; no foreign process or security setting was touched. Floors remain above
50 GiB. `receipts/restored.json` and `receipts/closure.json` record the full accounting.

Temporary process settings: `-IAIBench`, PIE `-NoLiveCoding`, unattended/nosplash,
1280x1024 PIE request or 1280x720 staged; PNG/30fps/seed777, cap1400 or null1100;
SVE=1, Delivery=0, Pace=1, Mask=1, TargetMask=1, RunLog=1, FocusGate=0; Config
`0 192 48 192 4`; AA=0 or diagnostic TSR=4, auto exposure/eye adaptation/Lumen/GI/reflections=0.
Transition CVars stay -1 and resolve 0/0/0. Marker=1 only for ownership, otherwise 0;
StuckMipRoute auto/hold, StuckMipNoHold=1 for nulls, TexCorruptNoPartScope=1 for its control,
Review916 PIE multi/Observe/Report/Skip for PIE ownership/drops, SynthTickOrder=1 for alignment.
LateStart was never invoked. Engine DEFER disables/restores rendering for three ticks;
staged GETALL probes at 0/210/1160 read state only. The temporary Project02 LODBias=1
device profile (SHA-256 aca6bdf45b81b6c7759654304f85c1f1f6154375581aa046656cf8560a47a76e)
and binary/container swaps were restored by receipt; process-local settings ended with the processes.

## Problems, fixes and deviations

Every failed harness reading and earlier implementation is retained. No gate, threshold
or prediction was changed; no problem took more than two fixes. `harness-fixes.json`
contains exact evidence paths and attempt counts.

1. Shift controls initially edited an unused annotation list and read the wrong offset
   field. Fix 1 updates both frame lists and uses `end`; fix 2 supplies an actual copied
   fixed-view mask on the added synthetic boundary, avoiding UNASSESSABLE. All four +/-1
   edges now fail the strict predicate. Original RGB is unchanged.
2. The 093-04 CookOutputDir omitted Windows. One corrected path gives complete containers.
3. The integrity walker used its dictionary variable while visiting a list. One variable
   correction removes UnboundLocalError without changing a check.
4. Separate DEFER scheduling callbacks produced five disabled ticks, and the three-drop
   gate correctly failed. One batched scheduling repair produces exactly three; both
   corrupted-summary controls still reject. The failed five-drop capture is retained.
5. The peer checker over-applied the C2-only zero-pixels-over-12 condition to all co-users.
   One correction restores the unchanged prediction's scope. All means and the 20-level
   can-fail remain mandatory; the earlier failed proxy-TSR result is explicitly superseded.
6. An initial extra-source comparator control was not a well-formed patch. One valid new
   Source-file patch is accepted by `git apply --numstat` and rejected by the comparator.
7. The staged ownership barcode overlapped a co-user ROI. One separate Marker=0 capture
   passes all full original ROIs; no pixels are excluded. Original failure is retained (G478).
8. Development component logs omit expected category/command echoes. First parser repair
   still assumed command echoes; the second parses the three ordered six-property groups
   and requires their actual frame offsets to be 0/210/1160. Actual state and a mutated
   after-state stencil prove pass/fail. Both failed parser receipts remain.

Earlier 093-03 compiler quoting/PATH/include repairs, `/wd4702` for deliberate unreachable
mutants only, own slow-scanner replacement, separate ASCII/UTF-16 string controls and valid
GPU-counter handling remain disclosed in that report. The original F6 mutation diagnosis
was corrected by ruling 093-03; it is not attributed to transitions.

The deleted 091-05 archives are replaced by the mandated shipped 218BA4C7 control. Defaults
are now 0/0/0 instead of 3/40/1; the barcode replaces the deleted green-plane harness.
Four attempts preserve the minimum of three visible fires: staged proxy yields three,
PIE four. Hold lengths differ (220/196 measured rows) with zero actual picture offsets.
Some fixtures log handled distance-field NaN ensures/material-usage diagnostics; no crash
or target-proof failure resulted, and cause is not established. Built-in broad collateral
summaries are not substituted for independent measured named peers. No office, auto-pool,
shader-readiness, broad stress or 15,000-actor census claim is made by these home gates.

## State and hand-off

Normal/strict Editor and Game builds from 093-03 remain valid (all 0 errors/0 warnings;
69 independent headers per strict target). Fifteen C++ suites, 51 Python tests, 98 reader
cases, 18 existing plus five integration mutants passed their predictions; four additional
alpha/scope mutants reject. Lever audit covers 53 registrations and 12 audit test cases.
String review has zero unresolved additions. Product source equals tested `72f3eb3`; the
release delta comparator and extra-source can-fail pass. Comments are stripped before
commit; all-ref gotcha scan reserves G478 after G477. Only this feature may be pushed,
fast-forward, after the final restoration and source/ref checks. Exact pushed head, complete
binary hashes, runtime receipts and limits are in the full run report. NEEDS-DECISION: none.

VERDICT: GREEN
