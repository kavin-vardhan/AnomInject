VERDICT: GREEN

Part C is complete: dashboard captures now default to the 5.1 delivery file set, without verification extras.
The new package, Rust reader and real desktop Library passed the Part C checks; console capture keeps its existing evidence.
Nothing was pushed. Publication remains held because the non-whole-frame texture failures are still blocking under GR6.

This GREEN applies only to the Part C criteria specified in 094-48c. It is not clearance of 094-48/48b or a client-delivery approval.

The comparison configuration is **5.1 delivery ON, DeliveryLabels ON, native PNG frames and target masks ON, diagnostic/bench outputs OFF, no host video encoder**. I inspected the read-only m51 Git source and the release parity source, plus dashboard b8ffaca. Its plugin-written set is:

- `Actual_Frames/frame_NNNNN.png`.
- `target_mask/frame_NNNNN.png`, only when nonempty.
- `annotation.json`, `labels.jsonl`, `mask_map.json`, `run_summary.json`.

Delivery-OFF files (`run.json`, `selection_provenance.json`, `anomaly_log.txt`) and diagnostic files are outside that configuration. [Reference comparison and complete before inventory](D:/IA_Europa/Receipts/09448c/layout-before-and-reference.json).

| Capture | Frames | PNG | JSON | JSONL | U32 | ZLIB | Writer layout/result |
|---|---:|---:|---:|---:|---:|---:|---|
| Preserved 48b S1, before change | 600 | 1208 | 8 | 1 | 600 | 600 | Flat; verification extras and 315 blank masks; missing mask_map |
| E1, Evidence default 0 | 300 | 440 | 3 | 1 | 0 | 0 | 444 files; exact selected 5.1 set; zero extras |
| E2, Evidence 1 | 300 | 448 | 9 | 1 | 300 | 300 | 1058 files; verification extras available |
| E3, console C4 | 600 | 893 | 8 | 2 | 600 | 600 | 2103 files under epoch01; console evidence retained |

E1/E2 each contain 300 images and 140 nonempty mask PNGs; 160 blank masks are omitted. E2 additionally contains eight UI-control PNGs, six verification JSON receipts (`capture`, `host-custom-depth`, `labels-receipt`, `private-availability`, `routes`, `ui-check`), and the 300 reduction/evidence pairs. Its second configuration does not restore blank PNGs. E3 contains 600 images, 285 masks, eight UI-control PNGs, and the existing private-pass log. Exact names, sizes and hashes are in the [E1 inventory](D:/IA_Europa/Receipts/09448c/E1-file-set.json), [E2 inventory](D:/IA_Europa/Receipts/09448c/E2-file-set.json) and [E3 inventory](D:/IA_Europa/Receipts/09448c/E3/reader.json).

| Gate | Measured result and evidence |
|---|---|
| E1 | PASS: 300 written/paired, zero drops, first fire26, input enabled, no sweep rows, game alive afterward. All 11 mask-map entries match event-scoped measured tags. No dangling evidence references. [Protocol](D:/IA_Europa/Receipts/09448c/E1/E1-protocol.json), [file-set check](D:/IA_Europa/Receipts/09448c/E1-file-set.json). |
| E1 Rust Library | PASS: actual b8ffaca `is_session_dir`, `describe`, `plan_session`, `plan_outlines`; complete=true,300 frames,140 boxes/outlines,5845424 contained mask pixels. Missing-folder and wrong-count controls reject. [Before desktop](D:/IA_Europa/Receipts/09448c/E1-Rust-library-result.json), [after native finalization](D:/IA_Europa/Receipts/09448c/E1-Rust-after-finalize-result.json). |
| E1 actual desktop | PASS on corrected observation: exact new session/root/300 count rendered by the existing desktop EXE. [Native/UI evidence](D:/IA_Europa/Receipts/09448c/E1-Library-03/actual-library.json), [screenshot](D:/IA_Europa/Receipts/09448c/E1-Library-03/library.png). |
| E2 output/readability | PASS: 300 written/paired, zero drops, complete evidence pairs, retained strict reader executed. Input enabled, first fire26, game alive, mask pairing and annotation PASS. [Protocol](D:/IA_Europa/Receipts/09448c/E2/E2-protocol.json), [original reader](D:/IA_Europa/Receipts/09448c/E2/E2-reader.json). Strict texture remains FAIL, as detailed below. |
| E3 | PASS: 600 requested/written/paired, zero drops, PASS receipts, restoration and normal exit0; epoch01 and evidence retained with Evidence default0. [Result](D:/IA_Europa/Receipts/09448c/E3-result.json), [reader](D:/IA_Europa/Receipts/09448c/E3/reader.json). Existing 2352-frame C5 H4/strict-texture proof and file set are [bound here](D:/IA_Europa/Receipts/09448c/E3-C5-parent-binding.json); no full C5 rerun. |

The real Library performed its standard finalization: 140 masks and `mask_map.json` moved byte-equal into `.dashboard/pending`, and it added `.dashboard/state.json` and `thumb.jpg`. Those are ordinary desktop outputs, not plugin verification extras. The writer-equality inventory was sealed before this step. The final folder has 440 PNG,4 JSON,1 JSONL,1 JPG; all original bytes are preserved, and the Rust reader passes the finalized folder too. [Exact transition proof](D:/IA_Europa/Receipts/09448c/E1-native-library-finalization-proof.json).

**E2 strict texture under GR6 remains FAIL:** whole-frame measurements=0, so their min/mean/max are not applicable. Non-whole-frame measurements=88; all88 are below IoU1.000: min0.2897693846, mean0.6900932190, max0.9877851807. Spill=0; dirty clean edges=0; polluted=0; tagged=0; drops=0; annotation=PASS. Classification uses each actual `whole_frame` flag, not the clean-edge `target_pixels` field. No tolerance, scene calibration or reader exclusion was changed. [GR6 disposition](D:/IA_Europa/Receipts/09448c/E2-GR6-result.json). The previous F1/F3/F5/S4 failures and unpublished commits remain unchanged.

One new local commit: **feat/europa-port `2828f6ab85406cacc6b5e4c06840a3a1b4cebbc0`**, `fix(europa): keep dashboard verification files opt-in`. Parent commits `6b171b50f0c38638333ccf0e99495a62c2039560` and `1667d0576f817bb7ad6e1d4568034cd331321559` remain intact. Fresh ls-remote still reports plugin origin `bd767ca8ac7cd1ca18abe34d9cc66764fee100a6`. Dashboard remains clean at `b8ffaca1d88a2c89d1ed69adaac85501cc2fc0b5`. **Pushed commits: none.** No client-facing card was written.

Nine compatibility files implement output gating, event/tag mask-map writing and nonempty-mask counts. `IAI.Capture.Evidence` defaults0, is explicitly absent in Shipping, and is latched at dashboard session start. Console C4/C5 command bodies and C5 admission remain unchanged; console evidence is always enabled independently of the CVar. Private-pass computation, mask/label meaning and CPU/GPU agreement checks remain. Documentation/status/journal and G573 were updated. The comment check found zero changes across468 files.

Editor and Game normal builds passed once each, with6/5 actions and zero engine/engine-plugin actions. Each strict target compiled14 affected units sequentially, retained183 hash-bound actions, and passed the full verifier and missing-unit control. Existing C4996 and UBT XML warnings remain visible; no warning suppression was added. Commit-bound binary audits and provenance rejection controls pass. [Editor build](D:/IA_Europa/Receipts/09448c/builds/editor-01/result.json), [Game build](D:/IA_Europa/Receipts/09448c/builds/game-01/result.json), [strict Editor](D:/IA_Europa/Receipts/09448c/strict-editor-05-result.json), [strict Game](D:/IA_Europa/Receipts/09448c/strict-game-05-result.json), [binary audits](D:/IA_Europa/Receipts/09448c/part-c-audit-provenance), [Shipping/file-set controls](D:/IA_Europa/Receipts/09448c/shipping-and-fileset-controls.json).

The restaged [Development package](E:/IA_BuildCache/09448c-package/flat-01/WindowsNoEditor) has34 files totaling2369809539 bytes:33 immutable hardlinked dependencies and one independent new EXE. No cook, UAT, content/configuration change, 5.1 work or dashboard rebuild occurred. [Package manifest](D:/IA_Europa/Receipts/09448c/flat-package-manifest.json), [complete file list](D:/IA_Europa/Receipts/09448c/package-file-list.md).

| Artifact | Bytes | SHA256 |
|---|---:|---|
| Development game EXE | 139691520 | `d7633cc5c0ac365040ff081351c1c63d5f598c440546e8ca1b9bb6fee5f60718` |
| Editor Capture DLL | 1194496 | `8c93ea75c26a9092654ea6136687213b141acaf1cdf32a315c636ebcc11308c8` |
| Unchanged cooked pak | 2209460478 | `d926673c1e26886f69292162a87bdfcfbfaade283ef83b21e71e48c8dc940f56` |
| Unchanged desktop EXE | 11190784 | `c8120946ff5bb4daa713286510f091b75760ae41f0895480adea1a14e481be28` |

Every EXE/DLL, including the test harness, and its live/archive path appears in the [hash appendix](D:/IA_Europa/Receipts/09448c/all-executable-hashes-v2.md) and [machine manifest](D:/IA_Europa/Receipts/09448c/executable-manifest-v2.json). The27-file native archive has four new copies and23 immutable references at [binary-archive-flat](E:/IA_BuildCache/09448c-evidence/binary-archive-flat).

The retained Rust executable hardcoded600, so its job-local harness was parameterized for300 and compiled once against the existing immutable dependency libraries. Production parser bodies stayed unchanged. Harness SHA256: `9ca35e3d564c27fd749fb68083ae42faf8147e3ddeaca574b5939c7f2a502c1b`. Re-run:

```powershell
& 'D:/IA_Europa/Temp/09448c/library-harness/europa_library_gate.exe' 'C:/IA_EuropaEvidence/09448c/E1/E1/session_20261106_171430_639295622700230000' 300
```

Preserved deviations/failures: helper preparation initially rejected a terminal-newline comparison, then an inherited strict-response mapping was incomplete; neither started compilation. A copied audit-helper filename also collided before audit execution. A documentation reader needed explicit UTF-8 before commit. The first Library observer falsely accepted an old300-frame session; screenshot review rejected it. The second stopped on a vanished owned WebView temporary file. The final observer requires the exact session/root/count and uses a bounded three-attempt re-enumeration only for disappearing owned WebView scratch; protected/evidence misses still fail. All original receipts remain. [Observer disposition](D:/IA_Europa/Receipts/09448c/E1-Library-observer-disposition.json), [allocation controls](D:/IA_Europa/Receipts/09448c/allocation-retry-controls.json), [failure index](D:/IA_Europa/Receipts/09448c/helper-failures-final.json). No game capture was repeated to improve texture numbers: E1,E2,E3 each launched once; desktop Library launched three times.

Safety checks pass: host Content/Config/descriptors, protected m51 HEAD/index/status and other refs are unchanged; all native changes reconcile to authorized builds. The feature is clean. Tokens were masked in retained logs, private runtime logs were removed by receipt, and the credential scan passes. All98 recorded owned identities are closed. Games exited0: E1 PID22308/birth134384390652742396; E2 PID29300/birth134384391119647620; E3 PID37320/birth134384393371511646. The accepted desktop exited0; the failed observer's owned tree was safely drained. No process was killed by name, no owner application was stopped, and no machine/security/paging/power setting changed. [Protection](D:/IA_Europa/Receipts/09448c/final-protection.json), [closure](D:/IA_Europa/Receipts/09448c/owned-closure-final.json), [credential scan](D:/IA_Europa/Receipts/09448c/credential-scan.json).

Every runtime admission records `idle_wait_waived=true`; GPU, physical-memory, foreign UE/build, disk and severe-pressure checks remained. E1/E2 used noIAIBench; E3 usedIAIBench. Process-local settings retained D3D11,1280x720, nativePNG,seed4242,four defaults,AA0,screen100,dynamic-resolution0,motion-blur0,eye-adaptation0,bloom0,exposure-offset-1.875,t.MaxFPS30 and minimum coverage0.5. E2 alone used `IAI.Capture.Evidence 1`; process closure ended it. Mask/pacing restoration passed. Each temporary desktop capture-folder preference was restored byte-equal with original mtime; credential settings and the owner's existing Library stayed unchanged.

Resource growth before report: C211615744/D799784960/E512188416 bytes. Free bytes: C17181831168/D33790214144/E15547518976; all reserves remained satisfied. The complete forecast was retained, not replaced by the ceiling. [All resource samples](D:/IA_Europa/Receipts/09448c/resource-samples.jsonl), [large-write/launch admissions](D:/IA_Europa/Receipts/09448c/large-write-admissions.json), [accounting with parent carry](D:/IA_Europa/Receipts/09448c/final-resource-accounting.json). Report/helper overhead is additionally charged in the final report accounting. Parent48b conservative charges C4031107072/D531410944/E1234931712 and all earlier charges remain separate and retained. No cache cleanup, evidence compression or historical raw rehash occurred. New raw evidence is3607 files/203003161 bytes; after the final reader, all sealed sizes/times still match. [Raw seal](D:/IA_Europa/Receipts/09448c/new-raw-evidence.json), [post-reader closure](D:/IA_Europa/Receipts/09448c/post-reader-closeout.json).

**Chat decision still required for publication:** resolve the non-whole-frame texture failures under GR6 rule4. Part C changes neither that gate nor release authority. Claude Code owns independent review, any permitted push and the office card. No client-facing Part C delivery is claimed here.

VERDICT: GREEN
