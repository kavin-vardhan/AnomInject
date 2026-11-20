The follow-up base was merged locally; normal and strict Editor/Game builds passed.
Proof stopped because the F6 can-fail did not isolate a transition-free event; J1 qualification is incomplete.
The host was restored, staging was untouched, the incomplete build was archived, and nothing was pushed.

**2. Heads, merge and defaults**

Worktree: `E:/IA_BuildCache/_r933/AnomalyInjector`, created from `_r53_src` using the requested command. `_r933` was unused. Branch: `fix/followup-capture`. **Heads pushed: none**, because the gate stop prevented qualification.

| Local commit | Purpose |
|---|---|
| `72f3eb3e2df1dff5587716fcd5bc0a650ae8000c` | `fix(capture): merge approved follow-up base with release defaults` |
| `72edc488d99b018328474426ac96fdd1dd18565e` | `docs(capture): record J1 proof stop and restored host` — current local head |

The merge parents are `d7eee97731812384d4fd00af9032254923363ada` and `5d6352b0f503f62d980b83ef0eb741af59426f84`. The sole conflict was `CLAUDE.md`; I retained the dated release status and follow-up notes and added the J1 status. There were no source conflicts. No natural mechanism, F1 WIP or new label key was added.

`AnomalyLabelSync.h` defaults are **0/0/0**; both PIE captures also report 0/0/0. The existing explicit 3/40/1 override assertion passes within the 316-check window test.

For `Source Shaders tools`, `git diff 5d6352b HEAD` and `git diff d660b5d d7eee97` have identical hunks and offsets after removing Git blob-index lines. Raw patch bytes differ in that metadata; both normalized patches hash to `439b778cc84570a7fa02cfa52799ba3777e22758e2e1a436eeb2e7aa695d1e41`. [Diff receipt](E:/IA_BuildCache/_r933/receipts/diff-equality.json).

The final documentation commit leaves the tested source/shader/tool tree unchanged. Comment stripping changed zero of 178 files; diff checks passed. All other branch and tag refs match the initial receipt, including m51, master and the release branch. No natural branch or delivery was created. [Final audit](E:/IA_BuildCache/_r933/receipts/final.json).

**3. Builds, hashes, archive and restoration**

All four builds on the detached `_r53_host` candidate passed with **zero errors and zero warnings**: normal Editor, normal Game, strict Editor, strict Game. Each strict target compiled 69 independent header translation units; response-file settings were checked. Temporary strict settings were `bUseUnity=false`, `PCHUsage=NoPCHs`, latest include order and `bEnforceIWYU=true`; Build.cs hashes were restored. [Build summary](E:/IA_BuildCache/_r933/strict/summary.json).

Candidate source is `72f3eb3`; product hashes are SHA-256:

| Candidate binary | SHA-256 |
|---|---|
| StackOBot.exe | `e41abf93bd6abc231deb7f93ace6594784ee74348b4db737cbdd465a0d3dd37f` |
| UnrealEditor-AnomalyBench.dll | `2d014241ec9f65ced693d6836494ab6e85f173dc22fb24e6153263d99efaf5bc` |
| UnrealEditor-AnomalyCapture.dll | `e176b8e0b4ccb2efe5089f6929fbb830147bc61abe381df47bf9602640263425` |
| UnrealEditor-AnomalyControlServer.dll | `10c64f857e8c271d0f44c43e8c63beaac4ee67e2cde348f918f5ad0dd4195a70` |
| UnrealEditor-AnomalyInjector.dll | `5171ba2fbfd180ea8a7fe6618334b73655726a93db9b223f4c70f4844e35a35f` |
| UnrealEditor-AnomalyShaders.dll | `589602b04bc775caeef7ccf8b1f21eee7cb39a91de95935ed57c5249e1d4f75e` |

Archive: [followup-capture-J1-72f3eb3-E41ABF93](D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J1-72f3eb3-E41ABF93/README.md). All six files rehashed correctly. Its README/manifest explicitly mark **NEEDS-DECISION, incomplete J1, not a delivery**. It preserves the requested guard-present/WIP-absent J2 control, but does not establish a qualified J1 base. No cooked container is included and no existing archive/index was changed.

The shipped-release control archive was rehashed 6/6:
[release-2026-10-05-23098fa-218BA4C7](D:/IntrusiveAnomalies/_binary_baselines/release-2026-10-05-23098fa-218BA4C7).

| Release control binary | SHA-256 |
|---|---|
| StackOBot.exe | `218ba4c79bb2bf6d9e14706772c04b3c53027cc05b84d4ab100a3349b650b7d6` |
| UnrealEditor-AnomalyBench.dll | `9f967665827cd1358b5f914466c36ecc45e47e5e972c749aa0221283bc92dfce` |
| UnrealEditor-AnomalyCapture.dll | `470ebe5f6c5d8fa11d9872392035c62c2bab2480aee462a4d4d58ac59e8a5be3` |
| UnrealEditor-AnomalyControlServer.dll | `0c9c329bedf22be975a09042eb242a21cda726fcab63553240a46685b1dd8cf1` |
| UnrealEditor-AnomalyInjector.dll | `45a1512f0c7b59e321d8de97dd03e23b9bee175f503f974fb5a200f99b5a1b90` |
| UnrealEditor-AnomalyShaders.dll | `43d78d878cd1e8cd41b23db1fb747fc5c6bd6a103cb5089bb4fb984b1afc47db` |

PIE used `D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe`, SHA-256 `5fcbbf4fe39adae10d1d67a3b21e998723ae07ed258c0fc7fa4103b029b6381b`, with the corresponding DLL set. Neither game EXE was launched. Retained bench-helper DLL hashes: CaptureBench `a7443041103affbcd06b724a1ee1cd66f30c636d7ba246d2b34e661b0e89fb51`; Review0916 `297478d189e5a41a78369c06a12641102d2d063bc8aeea20484545d3975bfccb`. All 38 CPU-test/mutant EXE hashes are enumerated in the [test binary manifest](E:/IA_BuildCache/_r933/test-binary-hashes.json); original host binary hashes are in the restoration receipt.

Archive corruption control: a non-executed scratch copy of AnomalyShaders.dll with one byte changed hashes to `1495e3ce1179515abbb715c8181ab858b74e5dc027e58a167bb4a5152ba037a2`; the mismatch was detected. [Receipt](E:/IA_BuildCache/_r933/archive-can-fail.json).

The host is restored detached at original `23098fa7415f148e935acbd4a97e18da04e6c46c`, clean. All **201** backed-up project/binary/configuration files match their original hashes. Only two job-created CrashReportClient INIs were removed, with exact paths/hashes recorded before deletion. Ordinary generated caches/logs/intermediates remain. Staging received **no write, cook or staged launch**. The release DLL swap was restored to candidate, then final restoration reinstated the original host. [Restoration receipt](E:/IA_BuildCache/_r933/receipts/host-restored.json).

Both launch idle receipts passed three consecutive samples: no foreign editor/game/build or GPU-heavy process, GPU 1%, physical memory headroom 12.28/12.04 GiB. Human idle was 989.656/301.218 seconds; the 15-minute foreign-editor exception was not needed because no foreign editor was present. Own editor PIDs 41132 and 18984 exited normally with code 0. No foreign PID or security setting was touched. Final free space was D: 189,620,899,840 bytes and E: 282,875,535,360 bytes, above both floors.

**4. Predictions and gate results**

[Predictions](E:/IA_BuildCache/_r933/AnomalyInjector/docs/predictions/093-natural-foundation-miss.md) were written before the first engine leg, with hashes and the four-attempt schedule bound before launch. Thresholds were not retuned. “Not run” below means no proof is claimed.

| Gate | Prediction | Result | Can-fail result |
|---|---|---|---|
| Merge/defaults/delta | Approved merge; 0/0/0; release-only delta | PASS, with blob-index normalization described above | Old-default mutant fails three assertions; extra-source-edit comparison control not run |
| Explicit override | Existing explicit 3/40/1 behavior preserved | PASS, 316 window assertions | Ignored-override mutant fails three assertions |
| Normal/strict builds | Both targets compile cleanly; strict settings verified | PASS, four builds, 0 errors/0 warnings | Lever audit's planted violations rejected; 12 audit cases pass |
| Unit/pure | Existing suites pass | PASS: 15 C++ programs, 51 Python tests, 98 label-pixel reader cases | 18 existing mutants plus five default/override/ownership mutants rejected |
| Strings/levers/archive | No unresolved string/lever change; archive bytes match | PASS: 1,462 changed printable sequences reviewed, two manual resolutions, zero unresolved; source plus EXE/five-DLL audit passes | ASCII/UTF-16 planted-string controls, audit controls and changed archive byte detected |

| Engine gate | Prediction | PIE result | PIE can-fail result | Staged result | Staged can-fail |
|---|---|---|---|---|---|
| F1 owner guard | Game marker in every picture; UV and shared proxy in home modes | **Partial:** UV multi-viewport passes 1,152 images; proxy not run | Release control misses marker in all 1,152 images, including all 192 labelled frames; owner/stale/family CPU mutants rejected. Erased-marker copy not run | Not run | Not run |
| F2 C1/C2 scope and tag retirement | C1-only masks/boxes, C2 unchanged, owned tags retire | Not run; six scope CPU assertions pass only | Engine/perturbed-image controls not run | Not run | Not run |
| F3 opaque/masked alpha pair | Opaque zero events; masked at least three visible fires | Not run; 36 uniformity CPU assertions pass only | Release and alpha-policy mutant pair not run | Not run | Not run |
| F6 wrong event ID | Intact transition-free capture passes; new checker rejects wrong ID; old checker misses it | Intact capture passes; corrupted copy rejected by new CLI/hook | **STOP: expectation failed. Old hook also rejects; retained PIE-settle transitions make the control invalid** | Not run | Not run |
| Dropped-mask accounting | Three deliberate clean/active drops leave no rows/images/mask outcomes | Deliberate drops not run; ordinary accounting is 192 measured + 960 unavailable = 1,152 rows | Accounting corruption control not run; mask-service CPU suite passes 16 assertions | Not run | Not run |
| m52 blurry hold | At least three visible blur events and zero-frame edges | Not run | No-hold/frozen-image controls not run | Not run | Not run |
| Shared proxy | Visible blur, unchanged co-user, measured masks and recovery | Not run | Identity/frozen-target/peer controls not run | Not run | Not run |
| UV/normal regression | At least three visible fires/type; correct object and recovery | UV only: four visible tile fires, eight edges at zero frames; normal/co-user not run | Dedicated identity/frozen-target/wrong-peer controls not run | Not run | Not run |
| Label-pixel and masks | Zero-frame onset/recovery; measured mask on every labelled image | UV passes: four CONSISTENT events, 192 measured masks, no offset/unassessable result | 98 reader self-tests pass and wrong-view release capture is rejected; planned copied shift/missing/blank/wrong-count controls not run | Not run | Not run |

Candidate evidence: [F1_P_uv_aaoff assessment](E:/IA_BuildCache/_r933/legs/F1_P_uv_aaoff/assessment.json), session `session_20261013-203157`. Release control: [F1_P_uv_release_control assessment](E:/IA_BuildCache/_r933/legs/F1_P_uv_release_control/assessment.json), session `session_20261013-203932`. The control's `passed:true` means the expected ownership failure was detected, not that its pictures passed candidate gates.

The pixel reader uses actual in-span masks and extrapolates them into clean baselines by up to 27 frames; independent fixed-bbox pixel measurements also give zero-frame edges. No measured clean-frame mask claim is made. Native order/AA off alone was exercised; synthetic-order alignment and temporal diagnostics remain unrun.

F6 detail: in a copied capture, row 191 changed from `uv_corruption@307|StaticMeshActor_0` to `uv_corruption@999999|WrongTarget`. The new CLI exits 1 and both new and release hooks report `labels event_id mismatch at 191`. The selected row was unflagged, but its event was not transition-free: four `pie_end_settle` transitions remain, one per event. This is my harness isolation error; it does **not** establish a production F6 regression. [Failed control](E:/IA_BuildCache/_r933/controls/F6_F1_P_uv_aaoff/result.json), [stop receipt](E:/IA_BuildCache/_r933/F6-gate-stop.json). No replacement control or later leg ran. The already-running release ownership control had completed before the shutdown check; its result was retained.

**5. Differences, settings and deviations**

- Release `218BA4C7` replaces the deleted 091-05 `C7FB5212` archive, as instructed. Defaults are 0/0/0 rather than 091-05's 3/40/1. Independent PIE-settle transitions remain possible.
- The retained CaptureBench in-scene barcode replaces the deleted green-plane ownership harness. Existing Review0916 bench infrastructure supplies two non-owner editor draws; its product WIP and late-viewport command are absent/unused. Four attempts were predicted before launch, maintaining the minimum of three visible fires. PNGs measured 1123x870; game viewport metadata reported 1280x992 from a requested 1280x1024 window. This differs from the prior dimensions and is reported without an inferred cause.
- All unrun modes, controls and staged gates above remain missing. The retained host staged container lacks the required scope/alpha/hold maps, so continuation needs a new bench cook. No 091-05 evidence is substituted for new proof; no office qualification is claimed.
- Pre-proof harness corrections: MSVC command quoting/PATH, missing mutant include path, and C4702 in deliberately unreachable mutant code. Removing optimization did not resolve C4702; only negative fixtures use `/wd4702`, retaining `/W4 /WX`; production/strict flags and assertions were unchanged. Logs retain the failed attempts.
- The slow PE scanner was stopped by its own PID 31828 and restarted using equivalent indexed ranges. A concatenated ASCII/UTF-16 string-control assertion was corrected to independent buffers; its failed attempt is retained. A relocation-prefix hypothesis was rejected: byte inspection resolved two apparent prefixes as ASCII `.cpp`/UTF-16 adjacency, with both component literals already in release. Idle sampling was corrected to use latest valid counter statuses 0/1 and reject unknown live samples. These are disclosed harness changes; no pixel or acceptance threshold changed.
- After the F6 stop, work was limited to evidence preservation, archive/restoration/static closure, documentation and reporting. The branch was committed locally but not pushed because the required proof remained incomplete.

Settings used in both captures, restored through process exit and the configuration receipt:

- Arguments: `-IAIBench -NoLiveCoding -unattended -nosplash`; job-specific map, driver, plan and log paths.
- `CaptureBench.Marker=1`; `IAI.Capture.SVE=1`, `Delivery=0`, `Pace=1`, `Fps=30`, `Mask=1`, `TargetMask=1`, `RunLog=1`, `FocusGate=0`; `IAI.Capture.Config 0 192 48 192 4`; Start PNG, seed 777, cap 1400, `uv_corruption StaticMeshActor_0 tile`.
- `r.DefaultFeature.AutoExposure=0`, `r.EyeAdaptationQuality=0`, `r.AntiAliasingMethod=0`, `r.Lumen.DiffuseIndirect.Allow=0`, `r.DynamicGlobalIlluminationMethod=0`, `r.ReflectionMethod=0`. Temporal transition CVars stayed at -1, resolving to 0/0/0; only CPU tests exercised explicit overrides.
- `IAI.Bench.Review916.PIE multi`, `Observe`, `Report`; transient duplicated play settings and two realtime editor views at `(-1200,2500,1000)` / `(-1200,3500,1000)`, rotation `(-20,-90,0)`. No Skip, LateStart or SynthTickOrder command was used.

Journal/status/gotcha G465 and current architecture were updated in the local documentation commit. Full evidence is under `E:/IA_BuildCache/_r933`; this report is saved at [report.md](D:/IntrusiveAnomalies/_relay/runs/2026-10-13-093-03-j1-natural-base/report.md).

**6. NEEDS-DECISION items**

1. Continuation needs a valid transition-free F6 control in PIE and staged, with the frozen new-rejects/old-misses expectation. The current control does not qualify. It was not repaired or rerun after the stop.
2. Complete the unrun PIE/staged matrix and its can-fails, including a bench cook with the missing fixtures, before qualifying J1 or pushing the feature head. No gate should be weakened, no J2/natural work should be inferred complete, and no delivery is authorized by this report.

VERDICT: NEEDS-DECISION
