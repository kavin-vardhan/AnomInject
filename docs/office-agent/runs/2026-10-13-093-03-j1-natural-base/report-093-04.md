The corrected F6 control passed on the existing PIE capture; its earlier diagnosis and reader wording are corrected.
The cook succeeded, but packaging failed before a complete container existed; J1 remains unqualified.
E41ABF93 and source are unchanged, the host is restored, live staging is untouched, and nothing was pushed.

**2. Heads, merge and defaults**

Branch `fix/followup-capture`, worktree `E:/IA_BuildCache/_r933/AnomalyInjector`. **Heads pushed: none.** Only documentation commits were added in 093-04:

| Local commit | Purpose |
|---|---|
| `72f3eb3e2df1dff5587716fcd5bc0a650ae8000c` | Original approved merge of release `d7eee97` and follow-up `5d6352b` |
| `72edc488d99b018328474426ac96fdd1dd18565e` | Original J1 proof-stop journal/status |
| `1e6dc652d314f92a885c5a741b2b4039c5df1928` | `docs(capture): correct F6 proof and record J1 packaging stop` |
| `ca2605f77fa6efb018169c70f2a57b2c662ba296` | `docs(capture): record cook output path mismatch` — current local head |

The original merge had only the expected `CLAUDE.md` conflict, resolved by retaining both dated release/follow-up blocks. Defaults remain **0/0/0**, with the existing explicit 3/40/1 override test passing. No source, shader, reader, tool, natural mechanism, F1 WIP or label-key change was made in this continuation. The predictions file is unchanged.

`git diff 5d6352b HEAD -- Source Shaders tools` still equals the release's `d660b5d..d7eee97` patch after excluding the two Git blob-index metadata lines; hunks and offsets are identical. Normalized SHA-256: `439b778cc84570a7fa02cfa52799ba3777e22758e2e1a436eeb2e7aa695d1e41`. All other branch/tag refs are unchanged. [Final audit](E:/IA_BuildCache/_r933/continuation09304/receipts/final.json).

The gotcha is now **G477**. All 70 refs were scanned, including level-three headings; G476 was the highest existing number. The check was repeated before commit, and the final G477 check finds only this feature branch. Status, architecture and both journals record the corrected F6 explanation. Comment stripping changed zero of 178 files; diff checks passed.

**3. Builds, hashes, cook receipt and restoration**

The accepted 093-03 normal/strict Editor and Game builds remain the candidate: all four passed with zero errors and warnings, with 69 independent header translation units per strict target. No product rebuild was performed in 093-04. [Build summary](E:/IA_BuildCache/_r933/strict/summary.json).

Candidate product hashes, SHA-256:

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `e41abf93bd6abc231deb7f93ace6594784ee74348b4db737cbdd465a0d3dd37f` |
| UnrealEditor-AnomalyBench.dll | `2d014241ec9f65ced693d6836494ab6e85f173dc22fb24e6153263d99efaf5bc` |
| UnrealEditor-AnomalyCapture.dll | `e176b8e0b4ccb2efe5089f6929fbb830147bc61abe381df47bf9602640263425` |
| UnrealEditor-AnomalyControlServer.dll | `10c64f857e8c271d0f44c43e8c63beaac4ee67e2cde348f918f5ad0dd4195a70` |
| UnrealEditor-AnomalyInjector.dll | `5171ba2fbfd180ea8a7fe6618334b73655726a93db9b223f4c70f4844e35a35f` |
| UnrealEditor-AnomalyShaders.dll | `589602b04bc775caeef7ccf8b1f21eee7cb39a91de95935ed57c5249e1d4f75e` |

Existing archive: [followup-capture-J1-72f3eb3-E41ABF93](D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J1-72f3eb3-E41ABF93/README.md), rehashed **6/6** again. It remains incomplete J1 evidence, guard present/WIP absent, not a qualified J2 base or delivery. No new binary archive was created. Its historical README's incorrect F6 cause is superseded by ruling 093-03 C3 and this report; existing archives/index were not edited.

Shipped-release control archive: [release-2026-10-05-23098fa-218BA4C7](D:/IntrusiveAnomalies/_binary_baselines/release-2026-10-05-23098fa-218BA4C7). Its accepted 093-03 hashes are:

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `218ba4c79bb2bf6d9e14706772c04b3c53027cc05b84d4ab100a3349b650b7d6` |
| UnrealEditor-AnomalyBench.dll | `9f967665827cd1358b5f914466c36ecc45e47e5e972c749aa0221283bc92dfce` |
| UnrealEditor-AnomalyCapture.dll | `470ebe5f6c5d8fa11d9872392035c62c2bab2480aee462a4d4d58ac59e8a5be3` |
| UnrealEditor-AnomalyControlServer.dll | `0c9c329bedf22be975a09042eb242a21cda726fcab63553240a46685b1dd8cf1` |
| UnrealEditor-AnomalyInjector.dll | `45a1512f0c7b59e321d8de97dd03e23b9bee175f503f974fb5a200f99b5a1b90` |
| UnrealEditor-AnomalyShaders.dll | `43d78d878cd1e8cd41b23db1fb747fc5c6bd6a103cb5089bb4fb984b1afc47db` |

Relevant engine executable hashes:

| Executable | SHA-256 |
|---|---|
| UnrealEditor.exe | `5fcbbf4fe39adae10d1d67a3b21e998723ae07ed258c0fc7fa4103b029b6381b` |
| UnrealEditor-Cmd.exe | `44f4aae265b37e62a7e167fff17cd55f8aa78030822cbd8425dc023737d81a48` |
| UnrealPak.exe | `1f88d51b0ad8c280f20e32d7cdaa584f7e8e000b93ce3eeb8689684272e3d73c` |

Retained helper DLLs: CaptureBench `a7443041103affbcd06b724a1ee1cd66f30c636d7ba246d2b34e661b0e89fb51`; Review0916 `297478d189e5a41a78369c06a12641102d2d063bc8aeea20484545d3975bfccb`. All 38 existing CPU-test/mutant EXE hashes remain in the [test manifest](E:/IA_BuildCache/_r933/test-binary-hashes.json). Original host binary hashes are enumerated in the restoration receipt. No game EXE or new PIE proof was launched in this continuation.

**Cook receipt:** [cook-receipt.json](E:/IA_BuildCache/_r933/continuation09304/cook-receipt.json), [cook.log](E:/IA_BuildCache/_r933/continuation09304/cook.log). RunUAT used the named host and Development Win64, with `-cook -stage -pak -archive -skipbuild -nocompile -nocompileeditor -nop4 -utf8output -unattended`. Cook/stage/archive outputs were isolated under `continuation09304`; the cook commandlet received `-IAIBench`. Twelve existing maps were requested: GateLevel, MainMenu, MainWorld, base TexCorrupt, ProxyDetail, ProxyDetailScope, HoldDetail, and all five Review0915 scope/alpha/co-user maps.

The commandlet cooked 928 packages and exited 0; UnrealPak also exited 0. AutomationTool then threw `System.NullReferenceException` in `Project.RunIoStore`, `CopyBuildToStagingDirectory.Automation.cs:3518`, and exited 1 after 42.58 seconds of the recorded invocation. UAT rebuilt its own automation tooling; no product source was changed or product target rebuilt.

Read-only diagnosis found a mismatch in **my harness's output path**: the commandlet wrote `cooked/StackOBot/Metadata/packagestore.manifest`, while staging appends `Windows` to a CookOutputDir not already ending in that platform name. The expected `cooked/Windows/StackOBot/Metadata/packagestore.manifest` is absent. Staging consequently skips loading the manifest before the later manifest access. This supports a harness packaging-path failure; it does not establish a product source defect. No retry or corrective cook ran after the stop.

**No complete cooked-container hash exists.** There is no `.utoc` or `.ucas`, and the archive output directory is empty. The retained, unusable partial `cook-stage/Windows/StackOBot/Content/Paks/StackOBot-Windows.pak` is 7,466,614 bytes, SHA-256 **`7c89da81cf5dcce1b49dc7ba174ea37ae95abb5fcbac884a2658cedd066fcdde`**. This is recorded separately from E41ABF93 in [partial-artifacts.json](E:/IA_BuildCache/_r933/continuation09304/partial-artifacts.json); it was never copied into live staging.

Restoration returned the host to detached `23098fa7415f148e935acbd4a97e18da04e6c46c`, clean. All **201** original host files match, all **2,756** project/plugin content files are unchanged, and a rehash confirms all **1,327** live-stage files unchanged. Only the temporary DefaultDeviceProfiles.ini and one job-created CrashReportClient INI were removed by exact-path/hash receipt. Generated cook outputs/logs/caches remain. [Restoration receipt](E:/IA_BuildCache/_r933/continuation09304/receipts/restored.json), [closure checks](E:/IA_BuildCache/_r933/continuation09304/receipts/close-precommit.json).

The cook's fresh idle gate passed three samples: no foreign editor/game/build or GPU-heavy process, GPU 0%, physical headroom about 9.3 GiB, human idle 4,881 seconds. UAT driver PID36048 and its commandlets exited; no engine proof process remained. No foreign PID was killed or security setting changed. Final recorded free space: D: 189,623,119,872 bytes; E: 278,106,128,384 bytes, above both floors.

**4. Predictions and gate results**

The [original predictions](E:/IA_BuildCache/_r933/AnomalyInjector/docs/predictions/093-natural-foundation-miss.md) remain unchanged. Ruling 093-03 C3 supplies the explicit F6 correction. No threshold was weakened or retuned. Accepted prior results below were not rerun unless stated.

| Gate | Prediction | Result | Can-fail result |
|---|---|---|---|
| Defaults/delta | 0/0/0; release-only delta | PASS, preserved | Old-default mutant rejects three assertions; extra-source-edit comparison control remains unrun |
| Explicit override | Explicit 3/40/1 preserved | PASS, 316 window assertions | Ignored-override mutant rejects three assertions |
| Editor/Game and strict builds | Four clean builds | PASS, accepted 093-03 evidence | Strict flags verified; 12 lever audit cases pass |
| Unit/pure | Existing suites pass | PASS: 15 C++ programs, 51 Python tests, 98 pixel-reader cases | 18 existing mutants and five default/override/ownership mutants rejected |
| Strings/levers/archive | No unresolved additions; matching archive bytes | PASS: 1,462 printable sequences reviewed, two manual resolutions, zero unresolved; source/binary lever audit passed; archive rehash 6/6 | Planted strings/lever violations detected; scratch archive corruption detected (`1495e3ce1179515abbb715c8181ab858b74e5dc027e58a167bb4a5152ba037a2`, never executed) |
| Bench container | Required fixtures packaged with unchanged E41ABF93 | **STOP: packaging exit 1; no complete container** | Map/container negative checks not reached |

| Engine gate | Prediction | PIE result | PIE can-fail | Staged result | Staged can-fail |
|---|---|---|---|---|---|
| F1 ownership | Correct game view for UV and shared proxy | UV passes: 1,152 images; shared proxy unrun | Release loses marker in all 1,152 images, including 192 labelled frames; marker/mask checks reject. Erased-marker copy unrun | Not run | Not run |
| F2 scope/tag retirement | C1-only masks/boxes, unchanged C2, owned tags retired | Not run; six CPU scope assertions only | Engine/perturbed-image controls unrun | Not run | Not run |
| F3 alpha pair | Opaque zero events; masked at least three visible fires | Not run; 36 CPU uniformity assertions only | Release/alpha-policy mutant pair unrun | Not run | Not run |
| F6 event identity | New rejects self-consistent wrong event; old misses it | **PASS, newly run on existing capture**; intact CLI/hook pass | Corrected mutant: new CLI exits 1, new hook reports missing annotation, old hook `[]`. Event-ID-only control rejected by both | Not run | Not run |
| Dropped-mask accounting | Three deliberate clean/active drops, no orphan outcomes | Deliberate drops unrun; ordinary 192 measured + 960 unavailable = 1,152 rows | Corrupted-summary control unrun; 16 mask-service CPU assertions pass | Not run | Not run |
| m52 blurry hold | At least three blurry events and zero-frame edges | Not run | No-hold/frozen-image controls unrun | Not run | Not run |
| Shared proxy blur | Visible blur, unchanged co-user, masks and recovery | Not run | Identity/frozen-target/peer controls unrun | Not run | Not run |
| UV/normal regression | At least three visible fires/type, correct object/recovery | UV: four visible fires, eight edges at zero; normal/co-user unrun | Dedicated frozen-target/wrong-peer controls unrun | Not run | Not run |
| Label-pixel/masks | Zero-frame edges; every labelled image has measured mask | UV: four CONSISTENT events, 192 measured masks, zero offsets | 98 reader self-tests pass; capture-specific shift/missing/blank/wrong-count controls unrun. Wrong-view reader result is UNASSESSABLE/exit 0, not rejection | Not run | Not run |

Corrected F6 evidence: [result.json](E:/IA_BuildCache/_r933/continuation09304/controls/F6_F1_P_uv_aaoff/result.json). On unflagged row 191, the copied entry changes `start_frame=999999`, `target_name=WrongTarget`, and `event_id=uv_corruption@999999|WrongTarget` together. The new hook reports `labels event missing from annotation: uv_corruption@999999|WrongTarget`; the old hook returns `[]`. The intact variant passes both. The separate event-ID-only variant yields `labels event_id mismatch at 191` in both versions. Original capture hashes remain unchanged. **No transition-free capture is needed.**

Prior pixel evidence remains [candidate assessment](E:/IA_BuildCache/_r933/legs/F1_P_uv_aaoff/assessment.json) and [release-control assessment](E:/IA_BuildCache/_r933/legs/F1_P_uv_release_control/assessment.json). The release control has **four UNASSESSABLE events and exit 0 from `verify_capture.py --label-pixel-gate`**. Rejection comes from the job's independent marker and measured-mask checks. The reader was not changed. Candidate in-span masks are measured; the existing reader extrapolates masks into clean baselines by up to 27 frames. Independent fixed-bbox readings also give zero-frame edges. Synthetic-order alignment and temporal diagnostics remain unrun.

**5. Differences, settings and deviations**

Capture-size cause for 1123x870 PNGs versus 1280x992 viewport metadata from a 1280x1024 request: **unknown**; this is not a gate.

Relative to 091-05, the mandated shipped release `218BA4C7` replaces the deleted pre-F1 archive, defaults are 0/0/0 instead of 3/40/1, and the retained in-scene barcode replaces the deleted green-plane harness. The prior four-attempt UV schedule was declared before launch without reducing the three-visible-fire minimum. Existing Review0916 infrastructure is bench-only; product F1 WIP remains absent. No office qualification or delivery is claimed.

093-04 changed only external harness scripts and documentation. The F6 mutant correction is expressly authorized by C3. The cook added isolated output directories and temporarily supplied the retained 091-05 Project02 LODBias=1 fixture profile for Windows/WindowsEditor: MinLODSize=1, MaxLODSize=16384, MinMagFilter=aniso, MipFilter=point, MipGenSettings=TMGS_SimpleAverage. Its SHA-256 was `aca6bdf45b81b6c7759654304f85c1f1f6154375581aa046656cf8560a47a76e`; it was removed by receipt. Candidate binaries were copied from the frozen archive into the detached host and restored afterward. Live staging was backed up but never changed. The new failure belongs to the cook/staging workflow; no source fix was attempted. No new engine proof lever was exercised.

Retained 093-03 capture settings were process-local: `-IAIBench -NoLiveCoding -unattended -nosplash`; CaptureBench.Marker=1; `IAI.Capture` SVE=1, Delivery=0, Pace=1, Fps=30, Mask=1, TargetMask=1, RunLog=1, FocusGate=0; Config `0 192 48 192 4`; PNG seed777/cap1400, UV tile on StaticMeshActor_0. Auto exposure, eye adaptation, AA, Lumen diffuse, dynamic GI and reflections were 0. Temporal CVars remained -1, resolving to 0/0/0. Review916 PIE multi/Observe/Report supplied two realtime editor views at (-1200,2500,1000) and (-1200,3500,1000), rotation (-20,-90,0), with duplicated 1280x1024 play settings. Skip, LateStart and SynthTickOrder were unused. All settings were restored through process exit/config receipts.

Earlier disclosed harness corrections remain preserved: MSVC quoting/PATH and mutant include path; `/wd4702` only for deliberate unreachable-code mutants, with production flags unchanged; own slow scanner PID31828 restarted with equivalent indexed lookup; string-control buffers separated and the mistaken relocation hypothesis rejected; idle counters restricted to valid current samples. No acceptance threshold changed. After the new packaging failure, work was limited to read-only diagnosis, restoration, evidence and documentation; no cook retry or proof leg ran.

Full report: [report.md](D:/IntrusiveAnomalies/_relay/runs/2026-10-13-093-03-j1-natural-base/report.md). The prior report remains `report-093-03.md`. Current journal: [093-04 qualification](E:/IA_BuildCache/_r933/AnomalyInjector/docs/sessions/2026-10-13-093-04-followup-capture-qualification.md).

**6. NEEDS-DECISION items**

1. Authorize continuation after the packaging gate failure to correct the cook/staging output-path mismatch and obtain a complete, hashed bench container. No product source change has been shown necessary; any required source change remains a stop condition.
2. Complete every unrun PIE/staged gate and can-fail above, including corrected staged F6, deliberate drops, proxy ownership, scope/retirement, alpha, hold/proxy/normal/co-user, synthetic alignment and capture-specific pixel/mask controls. Push only after all gates pass. Corrected PIE F6 is now qualified; J1 as a whole is not.

VERDICT: NEEDS-DECISION
