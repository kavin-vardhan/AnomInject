# 093-04 J1 qualification continuation — 2026-10-13

Status: **NEEDS-DECISION** on `fix/followup-capture`. Source remains exactly merge `72f3eb3`;
candidate EXE remains E41ABF93. No source/reader change is authorized. Nothing is pushed
until every frozen gate passes. Predictions remain byte-identical to `72edc48`.

## F6 correction and first result

Binding ruling: `D:/IntrusiveAnomalies/_reviews/093-03-ruling.md`, C3/C5. The original
event-ID-only mutant triggered the row self-consistency check in both versions, before
the transition filter. Its rejection was not caused by retained PIE-settle transitions.

The corrected control uses existing `F1_P_uv_aaoff`, session index 191, with no transition
on that row. In a copy only, start_frame=999999, target_name=WrongTarget and matching
event_id are changed together. Old hook: `[]`; new CLI: exit 1; new hook:
`labels event missing from annotation: uv_corruption@999999|WrongTarget`. PASS.
Intact capture passes both hooks and new CLI. Event-ID-only control fails both hooks
with `labels event_id mismatch at 191` and new CLI exit 1, as now predicted.
Original annotation/labels/summary hashes are unchanged. Evidence:
`E:/IA_BuildCache/_r933/continuation09304/controls/F6_F1_P_uv_aaoff/result.json`.

The existing label-pixel reader exits 0 with four UNASSESSABLE events on the wrong-view
release control. Rejection comes from the job's independent marker and measured-mask
checks. The reader is not modified. Gotcha G465 was renumbered G477 after scanning all
70 refs, including level-three headings, with a second check before commit.

Capture-size cause for the original 1123x870 PNGs versus 1280x992 viewport metadata
(1280x1024 requested window): **unknown**. This is not a gate.

## Cook/package stop and restoration

Fresh receipts back up 201 host files and 1,327 live-stage files before mutation. Host
source was detached at `72f3eb3`; six candidate binaries were restored from the existing
archive and checked against the E41ABF93 manifest. The retained 091-05 fixture setting
Project02 LODBias=1 was supplied in a temporary DefaultDeviceProfiles.ini. No project,
plugin, engine or reader source was edited. The predictions file remains unchanged.

RunUAT BuildCookRun used the named host, Development Win64, `-skipbuild -nocompile
-nocompileeditor`, and twelve existing maps: GateLevel, MainMenu, MainWorld, base
TexCorrupt, ProxyDetail/ProxyDetailScope, HoldDetail, and the five Review0915 maps.
Cook output, cook staging and archive output were isolated under continuation09304.
The commandlet received `-IAIBench`. Fresh idle receipt passed three samples, with no
foreign editor/game/build, GPU 0%, 9.3 GiB physical headroom and 4,881 s human idle.

The cook commandlet exited 0 after 928 cooked packages. UnrealPak exited 0. Then UAT
threw System.NullReferenceException in `Project.RunIoStore`,
`CopyBuildToStagingDirectory.Automation.cs:3518`, and exited 1. That expression assembles
CookedDirectory/PackageStoreManifest/Commands; the exact null operand is not recorded.
Do not infer a product source defect from this packaging exception. No retry was made.
The isolated cooked output contains packagestore.manifest and scriptobjects.bin, but
the packaging output contains only a 7,466,614-byte partial `.pak`, no `.utoc` or `.ucas`;
the archive output directory is empty. It is not a usable or qualified container.
Evidence: `continuation09304/cook-receipt.json`, `cook.log`, and `gate-stop.json`.

Proof stopped before any new PIE/staged leg. E41ABF93 remained byte-identical, and all
2,756 project/plugin content files matched their pre-cook hashes. Restoration returned
the host to detached `23098fa`, clean, with all 201 original files matching. Only the
temporary profile and one job-created CrashReportClient INI were receipt-removed.
Live staging was never mutated. Generated cook outputs, logs and caches are retained;
no foreign process was killed and no security setting changed. Cook driver PID36048
and its commandlets exited; no bench/editor/game process remained at closure.

The existing candidate archive is unchanged. Its old README's explanation blaming
PIE-settle transitions is superseded by ruling 093-03 C3 and this journal. No new binary
archive or delivery was created. Source, Shaders and tools still equal tested `72f3eb3`.

## Remaining work / decision needed

Resolve the UAT packaging failure in an authorized continuation, then cook the required
existing fixtures into a complete bench container, preserve the E41ABF93
EXE, record separate container hashes, and stage/restore by receipt. Complete shared-proxy
ownership, F2, F3, deliberate dropped-mask samples, hold/proxy/normal/co-user regression,
corrected staged F6, and pixel/mask can-fails in both modes. No threshold is weakened.

Nothing is pushed. Full report overwrites the requested run folder's `report.md`; the
prior report remains `report-093-03.md`. The unresolved item is the failed bench-container
gate plus the unrun matrix, not the corrected PIE F6 control.

VERDICT: NEEDS-DECISION
