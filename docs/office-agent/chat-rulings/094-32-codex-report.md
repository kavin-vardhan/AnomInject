<!-- collected by Claude Code (mailbox brief 094-32) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md, 5054 B, sha256 56167CFEDC8901028FA543C1B0B9033737EA13D0D00A2CB172EABD2E8D10F775, at 2026-11-01T01:39:08.943456+05:30 -->
C1 and C2 passed on stock 4.25. PIE and standalone each produced 60 valid plugin frames. C2 checked 480 labelled frames across both tick orders with zero mismatches or ambiguous frames, followed by 12 clean travel frames.

FN2 now decodes PF_A2B10G10R10 using `(v10*255+511)/1023`; alpha is unused. B8G8R8A8 and R8G8B8A8 remain accepted; other formats are counted and rejected before writing. The channel layout follows [D3D11Device.cpp:164](/E:/EpicGames/UE_4.25/Engine/Source/Runtime/Windows/D3D11RHI/Private/D3D11Device.cpp:164) and [D3D11RenderTarget.cpp:546](/E:/EpicGames/UE_4.25/Engine/Source/Runtime/Windows/D3D11RHI/Private/D3D11RenderTarget.cpp:546). Every launch omitted the backbuffer override.

Compiled assertions exercised the production decoder: known packed values produced exactly `(0,0,0)`, `(255,128,0)` and `(0,127,255)`; unsupported formats and padded, undersized, zero and null mappings were refused. Focused reader, argv, keyword-Rotator and engine Python 2.7.14 checks passed. [Native test receipt](/D:/IA_Europa/Receipts/09432/tests/native-conversion.json), [C2 tests](/D:/IA_Europa/Receipts/09432/tests/c2-summary.json).

Both targets were built for C1, then rebuilt for C2. All UBT starts used BelowNormal priority and two parallel actions. Compile errors: AnomalyCapture **0**, other modules **0**; no build-memory stops or corrective rebuilds were needed.

| Build attempt | Start free commit GiB | Minimum GiB | Exit |
|---|---:|---:|---:|
| Editor 01 — C1 | 11.17 | 9.86 | 0 |
| Game 01 — C1 | 11.22 | 9.01 | 0 |
| Editor 02 — C2 | 11.29 | 9.80 | 0 |
| Game 02 — C2 | 11.21 | 9.05 | 0 |

[Build logs and readings](/D:/IA_Europa/Receipts/09432/build-summary.json). The current 27-file manifest matches disk; complete executable/DLL hashes are in [artifact-hashes.json](/D:/IA_Europa/Receipts/09432/artifact-hashes.json). No binary archive was created.

Each C1 session produced indices 0–59 at 1280×720, with no missing, duplicate, black or unknown-format frame. All used **PF_A2B10G10R10 (18), D3D11, AA 0, zero drops**, and exited normally with code 0.

| C1 session | Frames | Capture seconds | Pixel gate |
|---|---:|---:|---|
| PIE01, EA_Core | 60 | 64.195 | PASS |
| Game01, EA_Travel | 60 | 9.140 | FAIL — wrong-map reference |
| Game02, EA_Travel | 60 | 9.136 | PASS |

Game01 exposed different saved material bindings between the maps. Its independently read source and copy agreed, and its plugin pixels matched the same-map source. The authorized retry used that hash-bound EA_Travel reference; ROI and thresholds stayed unchanged. Wrong-map and black-frame negatives failed. Game01 remains FAIL. The older 094-31b black-output cause remains unconfirmed. [Diagnosis](/D:/IA_Europa/Receipts/09432/game-diagnosis.json), [session evidence](/D:/IA_Europa/Receipts/09432/session-summary.json).

C2 adds immutable epoch/world/request metadata, view-family/render-thread pairing, frozen label serialization, guarded native event control and an independent pixel reader. Its single real session lasted **26.66 seconds**, below 180 seconds. All 492 primary/travel requests paired, with zero unmatched or dropped frames. Each anomaly completed three events in each order:

| Anomaly | Normal order: apply / revert / interior toggle | Synthetic order: apply / revert / interior toggle | Mismatches |
|---|---|---|---:|
| blinking | 3 / 3 / 6 | 3 / 3 / 6 | 0 |
| lighting_mismatch | 3 / 3 / 0 | 3 / 3 / 0 | 0 |
| lod_corruption | 3 / 3 / 0 | 3 / 3 / 0 | 0 |
| missing_object | 3 / 3 / 0 | 3 / 3 / 0 | 0 |

The Pace 1 block used a fixed simulation step; Pace 0 disabled it. Wall-rate pacing and throughput remain unqualified. The three separate negatives passed before launch using kept pictures, then independently rejected one-frame shifts against the real session:

| Negative | Shifted edges | Reader result |
|---|---:|---|
| Apply only | 24 | FAIL as required |
| Revert only | 24 | FAIL as required |
| Blink interior toggles only | 12 | FAIL as required |

[C2 reader, per-frame measurements and negatives](/D:/IA_Europa/Receipts/09432/c2-attempts/C2/01/reader.json). Qualification covers the four-anomaly C2 route; ordinary C1 remains frame-only. Masks, ControlServer and complete exporter parity remain later work.

Crashes, upload signs and protected changes: **none**. All six sessions, including both binding smokes, exited normally. Raw evidence is hash-sealed. Lean entry/exit checks confirm unchanged m51/index/status/protected refs and host Content, Config, descriptors and project file. No drive-root entries changed. [Entry](/D:/IA_Europa/Receipts/09432/entry.json), [exit](/D:/IA_Europa/Receipts/09432/exit.json).

Commits on `feat/europa-port`: `1686c2b` (writer), `ff16419` (lean disk lines), `5b7ef2c` (map reference), `13ced10` (C2), and **`e19fefc`** (closeout). W4 date overrides were absent; the worktree is clean. No push, tags or machine-setting changes.

VERDICT: GREEN | C1 and C2 pass on stock 4.25; zero frame-label mismatches.
