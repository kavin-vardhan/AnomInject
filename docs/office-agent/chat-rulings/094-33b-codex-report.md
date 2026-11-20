094-33b: the texture flag and a second reduction-buffer usage defect are fixed. C3 produces masks normally and keeps labels aligned, but lighting_mismatch and lod_corruption still fail the frozen coverage requirement. Both authorized sessions are spent.

`TexCreate_ShaderResource` now sits in `TargetableFlags` for Evidence, Mask and Drawn, per [4.25 RendererInterface.h:109](E:/EpicGames/UE_4.25/Engine/Source/Runtime/RenderCore/Public/RendererInterface.h:109). The Flags-only descriptor negative is refused. Slot 01 exposed a handled `BUF_SourceCopy` ensure; the reduction descriptor now carries that usage, per [D3D11Commands.cpp:2320](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Windows/D3D11RHI/Private/D3D11Commands.cpp:2320). Its missing-flag negative is refused too. Other descriptors were reviewed. Focused tests and keyword-Rotator check pass; no in-editor Python changed. The explicit 240-frame C3 reader profile preserves the default 480-frame C2 contract and pixel/edge checks.

Both target pairs ran BelowNormal with two actions, with no memory stop. The one compile error was fixed by an explicit `EBufferUsageFlags` cast.

| Build | Result | Start/min commit GiB | Errors |
|---|---|---:|---|
| editor-01 | PASS | 10.015 / 8.301 | 0 |
| editor-02 | FAIL | 9.759 / 8.787 | 1 AnomalyCapture C2678 |
| editor-03 | PASS | 9.725 / 8.288 | 0 |
| game-01 | PASS | 9.836 / 8.122 | 0 |
| game-02 | PASS | 9.766 / 7.412 | 0 |

Sessions: slot 01 03:48:27-03:48:44 IST, 16.923 s, exit 0; slot 02 04:01:53-04:02:10 IST, 16.790 s, exit 0. Each produced 240 paired 1280x720 colour/mask/label frames, D3D11, format 18, AA 0, zero drops. Both mask shaders dispatched; all GPU reduction tables matched independent CPU counts. Stencil and pacing restored.

Final retry metrics below cover three events per anomaly. Required IoU >=0.90, coverage >=0.95, spill <=0.01; lighting outside-receiver pixels also <=0.01. Lighting outside-receiver fraction is 0. All clean edges are empty and clean-frame noise is 0.

| Anomaly/state | Active frames | IoU min-max | Spill max | Coverage min/median/p95/max | Result |
|---|---:|---:|---:|---|---|
| blinking drawn | 21 | 1.000-1.000 | 0.000 | - | PASS |
| blinking hidden | 21 | 1.000-1.000 | 0.000 | - | PASS |
| lighting_mismatch | 42 | 1.000-1.000 | 0.000 | 0.564548/0.564618/0.564687/0.564694 | FAIL |
| lod_corruption | 42 | 1.000-1.000 | 0.000 | 0.049315/0.049354/0.049390/0.049394 | FAIL |
| missing_object | 42 | 1.000-1.000 | 0.000 | - | PASS |

Both real-mask negatives fail independently: the one-frame shift produces dirty clean edges and onset failures; wrong-target masks fail even on blinking and missing_object, whose originals pass. The same-session C2 reader passes 240/240 frames with zero mismatches or ambiguity: three apply/revert edges per anomaly and six interior blink toggles. Each apply, revert and blink label-shift negative also fails.

The remaining changes extend beyond the required silhouettes: visible halos, plus LOD's removed cube footprint and shadow changes. Expanding slot 01's masks to cover all changed pixels would spill 41.12% for lighting and 82.07% for LOD. All 240 colour images, masks and raw mask records are byte-identical between sessions. Thresholds, references, targets and render settings remained unchanged. [Diagnosis and original frame/mask paths](D:/IA_Europa/Receipts/09433b/diagnosis.json).

Crashes, upload signs and protected changes: none. Slot 01's handled ensure is absent in slot 02. Exact-PID crash monitors exited normally. Entry/exit protected refs/files and the final 27-file manifest pass; no drive-root changes. [Full distributions, raw receipt paths and exe/DLL hashes](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09433b-mask-receipts.json); [bound manifest](D:/IA_Europa/Receipts/09433b/c3-425-binaries-02.json). No archive, push or tags.

Commits: `e129eba` (texture flags/readers), `0d2c4b5` (copy-source flag), `47f9be2` (4.25 enum cast); closeout `d43635d` on `feat/europa-port`.

VERDICT: NEEDS-DECISION | Descriptor fixes and label sync pass; lighting and LOD mask coverage fail frozen FO3 metrics.
