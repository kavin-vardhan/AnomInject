VERDICT: GREEN

Private VT blur passes the bounded proof, including the original frozen identity and PIE/staged pixel checks.
Branch `feat/vt-proxy-blur` is pushed at `af79520437e1b470904b0638e99c3a15dd0fcc70`; tested runtime source is `c80ef9bbbe6c6faf60dafe1e640dfd0e469ea098`.
The build host is restored. This is not a delivery; the unchanged 8x selection rule remains an owner follow-up.

[Saved report](D:/IntrusiveAnomalies/_relay/runs/2026-10-05-092-03c-vt-proxy-blur-resume/report.md).

**Design and scope**

Resumed from `465f2e7` without rewriting any commit. The final implementation uses a transient UTexture with a strong source reference and borrowed built VT data; it never installs another texture's owning platform-data pointer. The constructor creates a distinct genuine VT producer with the mip offset. The original binding remains installed while private preparation runs.

| Implementation | Evidence on the feature branch |
|---|---|
| Truthful virtual material type and texture metadata | `Source/AnomalyInjector/Private/Anomalies/AnomalyPrivateVT.h:25`; `AnomalyPrivateVT.cpp:79` |
| Exported VT resource constructor, strong source ownership, fenced release | `AnomalyPrivateVT.cpp:43`, `:146`, `:153` |
| Renderer-managed root priming, root readback, full-range requests and exact-mip readback | `AnomalyPrivateVT.cpp:179`, `:237`, `:275`, `:286` |
| Named streaming VT admission and exact supported stack checks | `TexCorruptTree.cpp:471`, `:534` |
| One transaction for ordinary and VT outputs; restore bindings before releasing resources | `Anomaly_TexCorrupt.cpp:255`, `:304`, `:427`, `:1217`, `:1634` |
| Auto dispatch, actual route telemetry and census | `Anomaly_StuckLowMip.cpp:729`, `:781`; `Anomaly_TexCorrupt.cpp:1770`; `TexCorruptTree.cpp:2013` |

Paths in this table are under `E:\IA_BuildCache\_r921\AnomalyInjector`; abbreviated filenames share the listed Private/Anomalies directory. The 44 cited engine-source claims are hashed in `09203c/source-evidence.json`. No engine source or binary was edited. The generated-tool-file deviation is disclosed below.

Preparation is bounded at 512 pages, 120 attempts and 10 seconds. Requests name only the private allocation. UE owns page-table writes; no global/pool VT bias or source-residency forcing is used. This proof supports exact single-layer streaming-VT stacks. Unsupported stacks and unnamed samples refuse. Mixed ordinary/VT bindings use one event. Blurry remains OFF by default; UV/normal and forced hold still refuse VT. Capture and control-server production source are unchanged from the release base.

The new fixture is `/Game/CaptureBenchTexCorrupt/ProxyVT0921C/CB_ProxyBlurVT`: a genuine 2048x2048 streaming VT with a fine asymmetric pattern, named VT sampler, shared-texture co-user, ordinary control, mixed binding, flat VT and unsupported-stack/constant-sample cases. Its saved camera produces native 1920x1080 images. The target is 1280x320 pixels: 19.753% coverage. The ten authored assets and their hashes are retained in `09203c/fixture-final-archive`.

**Identity proof**

The frozen criterion remains every frame, including bind/restore edges: MAD <= that run's null maximum + 0.05. No frame or threshold was removed. Both legs bind genuine Drop 0 / first mip 0 / 2048x2048 / tile 128 and drain from 1 resource + 1 producer to 0/0. The fallback was not used.

| Leg | Frames / size | Target peak / limit | Co-user peak / limit | Ordinary peak / limit | Result |
|---|---|---|---|---|---|
| Original frozen fixture, identity-prefetch-03 | 480 / 942x869 | 6.595010 / 6.630434 | 0.336204 / 0.386204 | 0.339916 / 0.389916 | PASS, 0 failing frames |
| Native fixture, fixture-native-final-06 | 480 / 1920x1080 | 1.228993 / 1.272724 | 0.333440 / 0.383440 | 0.334016 / 0.384005 | PASS, 0 failing frames |

**Positive PIE and staged proof**

All rows below use executable `1ea292e815e5acf5c6ab214043b7d255c0f35d7e456c23c0d4e0a67fe489e891` and its archived Editor DLLs. All processes exited 0. Every event has measured target-only masks/bboxes, annotation.json == labels.jsonl, and zero labelled frames without a measured mask. E1 strict, t10 and t50 have zero unflagged frames and 0/0-frame edges. Independent raw RGB edges also agree at 0/0, including end-settle frames.

| Leg | Fires | Frames | Labelled frames | Median target MAD range | Sharpness loss | E1 t50 |
|---|---:|---:|---:|---|---|---|
| positive-pie-targeted-06 | 10 | 1561 | 480 | 72.356455..72.357187 | 99.55% | 0 unflagged |
| positive-pie-auto-06 | 3 | 553 | 144 | 72.356804..72.356826 | 99.55% | 0 unflagged |
| positive-staged-targeted-06 | 5 | 841 | 240 | 72.356866..72.357475 | 99.55% | 0 unflagged |
| positive-staged-auto-03 | 3 | 553 | 144 | 72.356969..72.356991 | 99.55% | 0 unflagged |

Total: 21 fires, 3,508 frames and 1,008 labelled frames. All positive VT outputs use first mip 0, Drop 4, effective 128x128, tile 128. Each of the six Auto picks measures 1280/128 = 10.0000 against the unchanged required 8.0000. Final fixture census: hold_eligible=0, proxy_eligible=1, proxy_vt_eligible=3; auto_hold=0, auto_proxy=1, auto_proxy_vt=3. Route accounting passes. The release refuses the named target that now applies through proxy_vt.

Per-event measured pixels (frame indices are zero-based; first restored frame is the offset; nominal capture rate 30 fps):

| Leg | Event | First changed | First restored | Median MAD | Sharpness loss | Auto ratio |
|---|---:|---:|---:|---:|---:|---|
| positive-pie-targeted-06 | 1 | 120 | 168 | 72.356594 | 99.55% | targeted |
| positive-pie-targeted-06 | 2 | 264 | 312 | 72.356574 | 99.55% | targeted |
| positive-pie-targeted-06 | 3 | 408 | 456 | 72.356554 | 99.55% | targeted |
| positive-pie-targeted-06 | 4 | 552 | 600 | 72.357055 | 99.55% | targeted |
| positive-pie-targeted-06 | 5 | 696 | 744 | 72.356455 | 99.55% | targeted |
| positive-pie-targeted-06 | 6 | 840 | 888 | 72.356475 | 99.55% | targeted |
| positive-pie-targeted-06 | 7 | 984 | 1032 | 72.357174 | 99.55% | targeted |
| positive-pie-targeted-06 | 8 | 1128 | 1176 | 72.356583 | 99.55% | targeted |
| positive-pie-targeted-06 | 9 | 1272 | 1320 | 72.357187 | 99.55% | targeted |
| positive-pie-targeted-06 | 10 | 1416 | 1464 | 72.356529 | 99.55% | targeted |
| positive-pie-auto-06 | 1 | 120 | 168 | 72.356826 | 99.55% | 10.0000 |
| positive-pie-auto-06 | 2 | 264 | 312 | 72.356812 | 99.55% | 10.0000 |
| positive-pie-auto-06 | 3 | 408 | 456 | 72.356804 | 99.55% | 10.0000 |
| positive-staged-targeted-06 | 1 | 120 | 168 | 72.356991 | 99.55% | targeted |
| positive-staged-targeted-06 | 2 | 264 | 312 | 72.356970 | 99.55% | targeted |
| positive-staged-targeted-06 | 3 | 408 | 456 | 72.356969 | 99.55% | targeted |
| positive-staged-targeted-06 | 4 | 552 | 600 | 72.357475 | 99.55% | targeted |
| positive-staged-targeted-06 | 5 | 696 | 744 | 72.356866 | 99.55% | targeted |
| positive-staged-auto-03 | 1 | 120 | 168 | 72.356991 | 99.55% | 10.0000 |
| positive-staged-auto-03 | 2 | 264 | 312 | 72.356970 | 99.55% | 10.0000 |
| positive-staged-auto-03 | 3 | 408 | 456 | 72.356969 | 99.55% | 10.0000 |

Onsets are frame 120 + 144n and offsets are frame 168 + 144n: nominally 4.0 + 4.8n seconds and 5.6 + 4.8n seconds. Every measured event lasts 48 frames. Baseline/during/restored final-build images were visually inspected; the receipt is `final-visual-inspection-c80ef9b.json`.

**Co-user isolation and revert**

| Leg | Co-user peak / limit | Ordinary peak / limit | Restored target peak / limit | Result |
|---|---|---|---|---|
| positive-pie-targeted-06 | 0.333227 / 0.382651 | 0.336496 / 0.385776 | 1.222516 / 1.272516 | PASS on every checked frame |
| positive-pie-auto-06 | 0.334325 / 0.383440 | 0.336512 / 0.385899 | 1.223626 / 1.273626 | PASS on every checked frame |
| positive-staged-targeted-06 | 0.332880 / 0.382464 | 0.336101 / 0.385429 | 1.220619 / 1.270619 | PASS on every checked frame |
| positive-staged-auto-03 | 0.332880 / 0.382464 | 0.336101 / 0.385429 | 1.220619 / 1.270619 | PASS on every checked frame |

**Lifetime and mixed binding**

| Case | Measured result |
|---|---|
| Ten consecutive targeted cycles | Ten 0/0 resource/producer drains; live/pending ledger returns to zero every cycle; identical 73,984-byte peak, no growing allocation ledger |
| All 21 required positive events | 21 clean 0/0 drains; baseline restored after each event |
| Mixed VT + ordinary, mixed-pie-02 | PASS; one proxy_vt event, held=true; named Tex -> virtual 128px, Drop 4; OrdTex -> ordinary 64px, resident drop 4; pixel/mask/label/E1 checks pass |
| destroy-pie-02 | PASS; last anomalous frame 152, effect_interrupted from 153; 0/0 drain, no label/pixel disagreement, missing mask or residue; late-label mutant caught |
| displace-pie-02 | PASS; last anomalous frame 152, effect_interrupted from 153; 0/0 drain, no label/pixel disagreement, missing mask or residue; late-label mutant caught |
| world-end-pie-02 | PASS; 155 captured frames, 35 labelled; pixels/labels agree through the final captured image, then 0/0 drain and clean exit; nonzero-drain mutant caught |

Actor destruction leaves the target absent, as intended. World teardown has no post-viewport picture, so no restored-picture claim is made after that viewport ceases to exist.

**Can-fails and named refusals**

| Leg/control | Result |
|---|---|
| Release 218BA4C7, release-vt-refusal-03 | PASS as negative: same target reports one virtual candidate and HELD NONE; 265 frames, zero events/labels, unchanged target/controls; executable swap restored and rehashed |
| no-bind-pie-03 | Correctly FAILS positive checker: 1 event / 48 labelled frames but no measured target change; E1 has no judgeable event |
| prefetch-no-request-04 | PASS as negative: prefetch_incomplete after 120 attempts / 341 pages; no bind, 720 unchanged frames, zero events/labels, 0/0 drain |
| stack-refusal-pie-03 | PASS: named Tex + TexSecond stack refuses virtual_texture:stack_layout; zero events and unchanged pixels |
| constant-refusal-pie-03 | PASS: unnamed VT sample refuses texture_not_parameter; zero events and unchanged pixels |
| uniform-refusal-pie-02 | PASS: flat VT refuses texture_uniform:no_spatial_variation; zero events and unchanged pixels |
| Reader/audit controls | Identity +32 RGB mutant; missing/duplicate native image; positive/mask/annotation/co-user/revert mutants; delayed label and nonzero drain; route omission; Shipping-lever mutation; binary byte corruption and appended string all caught |

**Short final-build regression**

| Existing fixture route | Fires / judgeable | E1 unflagged | Masks and annotation |
|---|---|---:|---|
| hold, regression-hold-01 | 3 / 3 | 0 | PASS |
| proxy, regression-proxy-01 | 3 / 3 | 0 | PASS |
| uv, regression-uv-01 | 3 / 3 | 0 | PASS |
| normal, regression-normal-01 | 3 / 3 | 0 | PASS |

MainWorld inventory: three snapshots of 604 mesh components. The two project VT resources, `SVT_Crater_H:Texture` and `SVT_Crater:Texture`, each have zero mesh users, named bindings and eligible blurry targets. Six engine default VT textures also have zero mesh users. No VT slot is unread; private counters are 0/0. This is the supported mesh-target scope, not a claim that landscape VT resources are unused.

**Gates, restoration and push**

| Gate | Result |
|---|---|
| Normal Editor and game | 0 errors / 0 warnings, readiness-root-prime-01; final normal restoration in production-strict-06 also 0/0 |
| Strict include Editor and game | Both 0/0; 70 isolated headers per target; strict flags and generated TUs restored |
| Lever audit | PASS: Development 56 names, Editor 57; 57 registrations, 0 unknown/missing/failures; Shipping-guard mutant caught |
| String scan | 324 reviewed, 0 unresolved; five new pointer fragments verified against PE relocations; semantic-string and shifted-pointer can-fails pass |
| Immutable E1 reader | 13 selftests pass; 400 parity inputs, 0 differences; SHA-256 95cfb3fc1724da487fe73d371f8bedb6fcda70ea047dcad25b2933c2d7ef9962 |
| Label/census readers | 74 label cases and 32 census checks pass, including proxy_vt and must-fail cases |
| Archives and fixture | 18 archives / 108 binaries rehashed, 16 distinct complete binary sets; 10 fixture assets unchanged; final cooked executable and containers rehashed |
| Source invariants | 44 engine claims / 25 source files verified; AutomationTool binary unchanged; capture/control production source and all protected refs unchanged |
| Final HEAD | Source, Shaders and uplugin Git objects equal tested runtime source c80ef9b; later changes are documentation/harness only |
| Host restoration | Detached 23098fa7415f148e935acbd4a97e18da04e6c46c; six recorded binaries and 156 Config/SavedConfig files restored by hash; 70 created crash-client INIs removed by receipt; regression profiles removed |
| Push | Only feat/vt-proxy-blur, no force or tags; new remote branch verified as af79520437e1b470904b0638e99c3a15dd0fcc70 |

The host worktree is clean. The requested new fixture content remains. No delivery staging, protected branch, tag, foreign process, security setting or owner configuration was changed. Engine legs passed the stricter no-foreign-editor gate; interrupted attempts were excluded. Disk floors were respected for writes.

Every pushed commit on `feat/vt-proxy-blur` (release base excluded):

| Commit | Subject |
|---|---|
| `aa97116003f74f71634c9239705817c6f83d8b9f` | feat(stuck-low-mip): add fenced private VT constructor probe |
| `83e58265547796a4b9f20f85e5dabf3b6f0f93fb` | fix(stuck-low-mip): isolate VT probe names in unity builds |
| `bdd937804a1d3c95a26d3f04891ca4baab798958` | fix(stuck-low-mip): avoid inherited texture member shadowing |
| `a92a1262229ff255d722c489e78be744215ae374` | docs: record private VT prototype and pending proof gates |
| `efd990d99d6e25aaddb6b9c176a00c65d6843455` | fix(stuck-low-mip): include source type for standalone VT header |
| `af13fa3a05a7852a94cb566b99c115b1c0ec4626` | test(stuck-low-mip): preserve failed VT identity proof harness |
| `465f2e7510b0a4e077c8f36ec4f5595e5db65421` | docs: record VT identity stop and restored build host |
| `152391ae761743a6e7bc5bb5b6f63aff1dd0740b` | feat(stuck-low-mip): prefetch private VT pages before probe binding |
| `28e96be4e0cb89ec36eb7634751999374f90cba9` | fix(stuck-low-mip): use UE atomic increment for VT readiness |
| `21034c21f7432a1f88ed02d6f843f2480f642340` | test(vt-blur): add gated new-window PIE fixture capture |
| `f3713519a422159e3b08676957d920d16291c818` | test(vt-blur): fix PIE native render extent for geometry proof |
| `9cb82ccae0b71721464f12a94371ee896b88e028` | feat(stuck-low-mip): integrate prefetched private VT proxy route |
| `30663d8b814c51f34608fcce77b86a8728a62ab0` | fix(vt-blur): inspect public stack layout and isolate PIE viewport |
| `0122bb1fc14d215196d368b32a55993fa7e55686` | test(vt-blur): add world inventory and record prefetch resume evidence |
| `83dfe55d2e1421b80be3f45fa1380ec6bf9d52ea` | Keep editor viewport families out of native PIE fixture captures |
| `8829491156e55aba288178209047c89a59c7e3b2` | Drain private VT bindings before releasing producer resources |
| `74e2c533a018b2bd1759c5bd872c1b9111e56001` | Recognize proxy VT FireWindow labels and preserve failed harness evidence |
| `1b2c4323de173b1051300390ce3e78e3370b3ed6` | chore(vt-blur): preserve resumed proof harness and partial results |
| `a2632b00d95ec5dcba10c29fefdac4cf553479b4` | fix(vt-blur): classify census routes from admitted replacements |
| `46a52d052b6177652b2739d08725472dedd59b80` | fix(vt-blur): report named VT support at startup and preserve proof evidence |
| `a7d487497e85690d01b9a7279fd0d2010915f1df` | fix(vt-blur): initialize private page mappings before readiness checks |
| `c80ef9bbbe6c6faf60dafe1e640dfd0e469ea098` | fix(vt-blur): prime private root mappings through renderer requests |
| `af79520437e1b470904b0638e99c3a15dd0fcc70` | docs(vt-blur): record complete private VT proof and restored host |

**Executable/DLL hashes and archives**

Final runtime archive: `E:\IA_BuildCache\_r921\09203c\archives\probe-c80ef9b-1EA292E8`. Full retained binary sets follow; paths name the actual archived files, and duplicate archives with identical sets are grouped.

All hashes are SHA-256. Each set lists the executable and all five Editor DLLs.

Set B1:

- `E:\IA_BuildCache\_r921\09203c\archives\probe-0122bb1-73253809` (source `0122bb1fc14d215196d368b32a55993fa7e55686`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `73253809acf7bed4073b8af3a3a8b9edce89f7be5159da62737397d5c1baa728` |
| editor/UnrealEditor-AnomalyBench.dll | `42fa50dc41e8dedca8acd0319537273d8e7face71b1f67fc03eb224939d68d06` |
| editor/UnrealEditor-AnomalyCapture.dll | `f0e6ab30ba0162e8221f1c3dd69b74241a821a6becb721b9bd42af7bcb7aaa18` |
| editor/UnrealEditor-AnomalyControlServer.dll | `f6fe4df60a3701a10bd8143fdee5b5a4b3800e3f33918cee40d64e816856c592` |
| editor/UnrealEditor-AnomalyInjector.dll | `dea3c5a928f77aa8fc83d27c260c98f6c06951bb449a11d21cb84541c3ba821d` |
| editor/UnrealEditor-AnomalyShaders.dll | `f6884612931c8e768643d70b59aadcd43a7b61cc50a8210654174b7f6840e0f2` |

Set B2:

- `E:\IA_BuildCache\_r921\09203c\archives\probe-1b2c432-311950E8` (source `1b2c4323de173b1051300390ce3e78e3370b3ed6`)
- `E:\IA_BuildCache\_r921\09203c\archives\probe-74e2c53-311950E8` (source `74e2c533a018b2bd1759c5bd872c1b9111e56001`)
- `E:\IA_BuildCache\_r921\09203c\archives\probe-8829491-311950E8` (source `8829491156e55aba288178209047c89a59c7e3b2`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `311950e87ab9430b5e452fe4c07ca4312f2a7ec7be5d4395450efedcacf927c8` |
| editor/UnrealEditor-AnomalyBench.dll | `3866e9f104c59704b1650b8846beb6ed248979a6fd3735ee303fb080442394c8` |
| editor/UnrealEditor-AnomalyCapture.dll | `f75287261e6b844c1f5f2152d90e1d2c5707fa8d7f2f55503531793560503d2d` |
| editor/UnrealEditor-AnomalyControlServer.dll | `be132137e3cbdc53b21b87b464e8df7f43e2c8a6258b9f0441c0650e539678c7` |
| editor/UnrealEditor-AnomalyInjector.dll | `3f98c1a4d1f2275b4c9b68ce3596bf06ca39987b984ccc95f452a7c0acc22f19` |
| editor/UnrealEditor-AnomalyShaders.dll | `bc38421fa57a3c8c1270a53bf3513b6710dff088435173a29c694800c228a041` |

Set B3:

- `E:\IA_BuildCache\_r921\09203c\archives\probe-21034c2-9B1EBDB5` (source `21034c21f7432a1f88ed02d6f843f2480f642340`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `9b1ebdb544b3ae3953160c9b945735f9945b624a63481b7446f72bc046407849` |
| editor/UnrealEditor-AnomalyBench.dll | `9e6665a101a5bff294a87e92e73485cba178a22a92edd7fe031c844ddb582bef` |
| editor/UnrealEditor-AnomalyCapture.dll | `c78952f91609174f2b8f3a15a203dabce3fde46353fbd369cf83f42000726fff` |
| editor/UnrealEditor-AnomalyControlServer.dll | `2005b8cded99247bc98becd18be8092c80cc78bb2d02911d9a2737abd586a3e4` |
| editor/UnrealEditor-AnomalyInjector.dll | `a48c8ae573be1656cf4d42023a5198c819f73274a71cae9d58b552eaaaf76308` |
| editor/UnrealEditor-AnomalyShaders.dll | `72638ba5d144150e6c8e833e2de6de3596e12ec94ddf59ff7bdcbf257280ddec` |

Set B4:

- `E:\IA_BuildCache\_r921\09203c\archives\probe-28e96be-9B1EBDB5` (source `28e96be4e0cb89ec36eb7634751999374f90cba9`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `9b1ebdb544b3ae3953160c9b945735f9945b624a63481b7446f72bc046407849` |
| editor/UnrealEditor-AnomalyBench.dll | `0b12f33a3ec8d511556373feeabb0c695ccfb9bbd33ed9f5d3ed694ac909ce88` |
| editor/UnrealEditor-AnomalyCapture.dll | `c78952f91609174f2b8f3a15a203dabce3fde46353fbd369cf83f42000726fff` |
| editor/UnrealEditor-AnomalyControlServer.dll | `2005b8cded99247bc98becd18be8092c80cc78bb2d02911d9a2737abd586a3e4` |
| editor/UnrealEditor-AnomalyInjector.dll | `a48c8ae573be1656cf4d42023a5198c819f73274a71cae9d58b552eaaaf76308` |
| editor/UnrealEditor-AnomalyShaders.dll | `72638ba5d144150e6c8e833e2de6de3596e12ec94ddf59ff7bdcbf257280ddec` |

Set B5:

- `E:\IA_BuildCache\_r921\09203c\archives\probe-30663d8-ACE7CF7D` (source `30663d8b814c51f34608fcce77b86a8728a62ab0`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `ace7cf7da0ba0c0ca9ffaaf4c6b3a8ddb10bcfdde10c8614702e1d6f136b5ad1` |
| editor/UnrealEditor-AnomalyBench.dll | `77f80c564024f5c47353047d35cda0f5f17028725f2663dff74502f3ec8aad76` |
| editor/UnrealEditor-AnomalyCapture.dll | `c78952f91609174f2b8f3a15a203dabce3fde46353fbd369cf83f42000726fff` |
| editor/UnrealEditor-AnomalyControlServer.dll | `2005b8cded99247bc98becd18be8092c80cc78bb2d02911d9a2737abd586a3e4` |
| editor/UnrealEditor-AnomalyInjector.dll | `876a5feb80b337a32bd667ab7accf3e89cf05b9e598475f3305d7bd16aa9a0e6` |
| editor/UnrealEditor-AnomalyShaders.dll | `249dcd75bf46467aff084f493dea67bb8c6ca88df63c1920205deda2bbaae926` |

Set B6:

- `E:\IA_BuildCache\_r921\09203c\archives\probe-46a52d0-FE338251` (source `46a52d052b6177652b2739d08725472dedd59b80`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `fe338251aab6bbfef0f0dc39baf7b2d91ce9e2356ba06d9763da314b28b20651` |
| editor/UnrealEditor-AnomalyBench.dll | `854b9f4e96b651856718f5241ea09efb4e1658b76459d1d524aca3a2ddcbf082` |
| editor/UnrealEditor-AnomalyCapture.dll | `c4a2c4c418ee02c02f0b00fe205c24ed470b2e570605d1d6b403dd69230217a0` |
| editor/UnrealEditor-AnomalyControlServer.dll | `bfbe2fb2216f4aca71844ea432c1362b0261b2d49f74c077c73cec5eb45cecdf` |
| editor/UnrealEditor-AnomalyInjector.dll | `c7f065d7430e9fa48e9a0e5f9e537c035bbbb2cd6a6ca51890ae30f2ac9a234e` |
| editor/UnrealEditor-AnomalyShaders.dll | `47d7f8f3e7beaad0e25d3a14be11be181fde9c46be9cee99dcd6225421570e95` |

Set B7:

- `E:\IA_BuildCache\_r921\09203c\archives\probe-83dfe55-10E64F63` (source `83dfe55d2e1421b80be3f45fa1380ec6bf9d52ea`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `10e64f63b4b8ec6588f404e76e21d6b0a4d62bbf683868fccc14309d9b1c5dd4` |
| editor/UnrealEditor-AnomalyBench.dll | `4bce7c7663d5cd989700300bdcb0c949f78c0194b81beb1b96ed693c388c148a` |
| editor/UnrealEditor-AnomalyCapture.dll | `f0e6ab30ba0162e8221f1c3dd69b74241a821a6becb721b9bd42af7bcb7aaa18` |
| editor/UnrealEditor-AnomalyControlServer.dll | `f6fe4df60a3701a10bd8143fdee5b5a4b3800e3f33918cee40d64e816856c592` |
| editor/UnrealEditor-AnomalyInjector.dll | `112d97db9e917f20886c838d10a53af50b3aeec0dbde4d898ca5aad7fdc92a92` |
| editor/UnrealEditor-AnomalyShaders.dll | `f6884612931c8e768643d70b59aadcd43a7b61cc50a8210654174b7f6840e0f2` |

Set B8:

- `E:\IA_BuildCache\_r921\09203c\archives\probe-8829491-1208F86C` (source `8829491156e55aba288178209047c89a59c7e3b2`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `1208f86c49f3e55c1ea9d571db1ddeb32b387d948a907882b4b473229a38c2a4` |
| editor/UnrealEditor-AnomalyBench.dll | `4bce7c7663d5cd989700300bdcb0c949f78c0194b81beb1b96ed693c388c148a` |
| editor/UnrealEditor-AnomalyCapture.dll | `f0e6ab30ba0162e8221f1c3dd69b74241a821a6becb721b9bd42af7bcb7aaa18` |
| editor/UnrealEditor-AnomalyControlServer.dll | `f6fe4df60a3701a10bd8143fdee5b5a4b3800e3f33918cee40d64e816856c592` |
| editor/UnrealEditor-AnomalyInjector.dll | `5b93c1994e7d1fca83cc6dc8061480137767e95352ce99eb99f7ea431ca0a87f` |
| editor/UnrealEditor-AnomalyShaders.dll | `f6884612931c8e768643d70b59aadcd43a7b61cc50a8210654174b7f6840e0f2` |

Set B9:

- `E:\IA_BuildCache\_r921\09203c\archives\probe-a2632b0-3C135195` (source `a2632b00d95ec5dcba10c29fefdac4cf553479b4`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `3c1351952eb9de316fdc3488c5a60f3357a5339995c15f63662ed3a089e890e1` |
| editor/UnrealEditor-AnomalyBench.dll | `6a771cf8357a9b7e48ed13020475d9b72ec2e6dbce6e3e8a65029e92b86df292` |
| editor/UnrealEditor-AnomalyCapture.dll | `0d2512ef89dc55489cfb873b10adad18332809786377723d180611bb9762cb09` |
| editor/UnrealEditor-AnomalyControlServer.dll | `11b334a9329f84b99a6e21654daf954c851792c5aaf72cb583c565a058b9384a` |
| editor/UnrealEditor-AnomalyInjector.dll | `5c29387d15f5c786753ce0c57261a2853e30f13194a05be390f724b82996f2ee` |
| editor/UnrealEditor-AnomalyShaders.dll | `9dee12739a214401690cb34319aaa22aa5e1bb20f2fbed9281ad9c25c5ea7b7d` |

Set B10:

- `E:\IA_BuildCache\_r921\09203c\archives\probe-f371351-9B1EBDB5` (source `f3713519a422159e3b08676957d920d16291c818`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `9b1ebdb544b3ae3953160c9b945735f9945b624a63481b7446f72bc046407849` |
| editor/UnrealEditor-AnomalyBench.dll | `7984076432011e331c083a63426bf5d1218f854e181ad5ab8e2178ac38962776` |
| editor/UnrealEditor-AnomalyCapture.dll | `c78952f91609174f2b8f3a15a203dabce3fde46353fbd369cf83f42000726fff` |
| editor/UnrealEditor-AnomalyControlServer.dll | `2005b8cded99247bc98becd18be8092c80cc78bb2d02911d9a2737abd586a3e4` |
| editor/UnrealEditor-AnomalyInjector.dll | `a48c8ae573be1656cf4d42023a5198c819f73274a71cae9d58b552eaaaf76308` |
| editor/UnrealEditor-AnomalyShaders.dll | `72638ba5d144150e6c8e833e2de6de3596e12ec94ddf59ff7bdcbf257280ddec` |

Set B11:

- `E:\IA_BuildCache\_r921\archives\probe-bdd9378-01BA2183` (source `bdd937804a1d3c95a26d3f04891ca4baab798958`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `01ba218382822ee47e9e733e5d840222e75d7022daede26ed0ef4bb7e4748b10` |
| editor/UnrealEditor-AnomalyBench.dll | `9f967665827cd1358b5f914466c36ecc45e47e5e972c749aa0221283bc92dfce` |
| editor/UnrealEditor-AnomalyCapture.dll | `470ebe5f6c5d8fa11d9872392035c62c2bab2480aee462a4d4d58ac59e8a5be3` |
| editor/UnrealEditor-AnomalyControlServer.dll | `0c9c329bedf22be975a09042eb242a21cda726fcab63553240a46685b1dd8cf1` |
| editor/UnrealEditor-AnomalyInjector.dll | `9b6f9fd1f37b1525f9badd1164f05a0b93477dadf049067c1da4d00edc25c5ea` |
| editor/UnrealEditor-AnomalyShaders.dll | `43d78d878cd1e8cd41b23db1fb747fc5c6bd6a103cb5089bb4fb984b1afc47db` |

Set B12:

- `E:\IA_BuildCache\_r921\archives\probe-efd990d-5899E2A4` (source `efd990d99d6e25aaddb6b9c176a00c65d6843455`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `5899e2a420cc30dbc2ce131f1547270776373e725a6687235000d58af0251c4d` |
| editor/UnrealEditor-AnomalyBench.dll | `1bb2d486ed4824a16f660b317a2d55b31c36f294206920895620695855027469` |
| editor/UnrealEditor-AnomalyCapture.dll | `bd56ffbfc0bad3b1d221aecbc265bfe124d82ed4bacad46108f66a51b6da9a46` |
| editor/UnrealEditor-AnomalyControlServer.dll | `f2327e7721f44955d0db5e529204ced9a1fd1089aae62ab5137c93d20b68f74c` |
| editor/UnrealEditor-AnomalyInjector.dll | `36d16adc1d71c330771e1dc92e8e145c2b1860c4a0c09579de6e3867131bc9c1` |
| editor/UnrealEditor-AnomalyShaders.dll | `d728647dc3f39656afcb5e9dcc1d6d3ab687888c3f9ebafab1a6f1bf592f6b4b` |

Set B13:

- `E:\IA_BuildCache\_binary_baselines\release-2026-10-05-23098fa-218BA4C7` (source `23098fa7415f148e935acbd4a97e18da04e6c46c`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `218ba4c79bb2bf6d9e14706772c04b3c53027cc05b84d4ab100a3349b650b7d6` |
| editor/UnrealEditor-AnomalyBench.dll | `9f967665827cd1358b5f914466c36ecc45e47e5e972c749aa0221283bc92dfce` |
| editor/UnrealEditor-AnomalyCapture.dll | `470ebe5f6c5d8fa11d9872392035c62c2bab2480aee462a4d4d58ac59e8a5be3` |
| editor/UnrealEditor-AnomalyControlServer.dll | `0c9c329bedf22be975a09042eb242a21cda726fcab63553240a46685b1dd8cf1` |
| editor/UnrealEditor-AnomalyInjector.dll | `45a1512f0c7b59e321d8de97dd03e23b9bee175f503f974fb5a200f99b5a1b90` |
| editor/UnrealEditor-AnomalyShaders.dll | `43d78d878cd1e8cd41b23db1fb747fc5c6bd6a103cb5089bb4fb984b1afc47db` |

Set B14:

- `E:\IA_BuildCache\_r921\09203c\archives\probe-a7d4874-0F79605D` (source `a7d487497e85690d01b9a7279fd0d2010915f1df`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `0f79605d3d9e66e6fc0685afaededf3430f5368db35fa452f1adf307c7315bcb` |
| editor/UnrealEditor-AnomalyBench.dll | `efe4ea85d5fdbaa636fa9bf396fc507cbb8932441c3fc6a2b93c64f15afa1286` |
| editor/UnrealEditor-AnomalyCapture.dll | `aecdfa1afeeb2d88cc1180b30394a0209626985cf5310871237a82b78eef2a66` |
| editor/UnrealEditor-AnomalyControlServer.dll | `940fa0c236c3b43d6d3cd7c658919ecc6a0010ec1e6b2d87891fdfea2f1e97c6` |
| editor/UnrealEditor-AnomalyInjector.dll | `f5ea130af87d7f0d84c92c84cefa1c1ff5779badb59929f6cba030467e628326` |
| editor/UnrealEditor-AnomalyShaders.dll | `a3c412fd099fc2f0049d8cc737972520d7c4cd486e4ecbbc0fcde0b4c3ee45d0` |

Set B15:

- `E:\IA_BuildCache\_r921\09203c\archives\probe-c80ef9b-006C5389` (source `c80ef9bbbe6c6faf60dafe1e640dfd0e469ea098`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `006c5389b9866fb458bf98e3323aa41e90a604e4f205b0b03c5ff1afac239690` |
| editor/UnrealEditor-AnomalyBench.dll | `efe4ea85d5fdbaa636fa9bf396fc507cbb8932441c3fc6a2b93c64f15afa1286` |
| editor/UnrealEditor-AnomalyCapture.dll | `aecdfa1afeeb2d88cc1180b30394a0209626985cf5310871237a82b78eef2a66` |
| editor/UnrealEditor-AnomalyControlServer.dll | `940fa0c236c3b43d6d3cd7c658919ecc6a0010ec1e6b2d87891fdfea2f1e97c6` |
| editor/UnrealEditor-AnomalyInjector.dll | `b7fd8abd11803c489a02b5815a296b08cb05b20c0859c68ca59e9fbab1d4a4c1` |
| editor/UnrealEditor-AnomalyShaders.dll | `a3c412fd099fc2f0049d8cc737972520d7c4cd486e4ecbbc0fcde0b4c3ee45d0` |

Set B16:

- `E:\IA_BuildCache\_r921\09203c\archives\probe-c80ef9b-1EA292E8` (source `c80ef9bbbe6c6faf60dafe1e640dfd0e469ea098`)

| Binary | SHA-256 |
|---|---|
| StackOBot.exe | `1ea292e815e5acf5c6ab214043b7d255c0f35d7e456c23c0d4e0a67fe489e891` |
| editor/UnrealEditor-AnomalyBench.dll | `89b38938235cb95f4a30b40fd51798729736aed5a02cc4479eabd9fa4fc95f23` |
| editor/UnrealEditor-AnomalyCapture.dll | `9a746e84cca24c6644a7a8995e92e89f89b53044e526656e9497e27bb6ce5000` |
| editor/UnrealEditor-AnomalyControlServer.dll | `3145f4c0b1f187159bfa827981c4a0f01cf54d67a36e64d0cb90907c1ffeba68` |
| editor/UnrealEditor-AnomalyInjector.dll | `c74bbd3cd0b4305cd08d1097943228d6dca175f447885a43c6c9c28ac4c0344a` |
| editor/UnrealEditor-AnomalyShaders.dll | `6e6e5e0d301abc087a04819833ab1768550cc725f35213241df59b389af52e79` |

Final cooked archive: `E:\IA_BuildCache\_r921\09203c\cooks\vt-cooked-10\archive\Windows\StackOBot` (cook 26.41 seconds, exit 0). Its executable hash is the final hash above. Container SHA-256:

| Container | SHA-256 |
|---|---|
| global.ucas | `57349d07bd158eb879bdc8b6f367014f730e9fe70fcedb145e35cff1b81654c8` |
| global.utoc | `016de38c52ee8070e7ee0fbc0548b959a80c266ec3b394060e1378f465820fde` |
| StackOBot-Windows.pak | `c91a20b8346f820b87197a081db1155cd8179ae17a6ba518f113e54528cc6719` |
| StackOBot-Windows.ucas | `6c6301da3474cf1581f45ef14baa83f18e5dda6bf9899f867fa42cd9a0fe7be4` |
| StackOBot-Windows.utoc | `2b93d61f820dc8d16eb8f755e3568a30ae34341f729d58f00777883cae3dcbd4` |

Earlier successful cooked archives are retained below, each with its result.json executable/container hash receipt. These are superseded evidence, not the final proof binaries.

| Cooked archive | Executable SHA-256 |
|---|---|
| `E:\IA_BuildCache\_r921\09203c\cooks\vt-cooked-01\archive\Windows\StackOBot` | `311950e87ab9430b5e452fe4c07ca4312f2a7ec7be5d4395450efedcacf927c8` |
| `E:\IA_BuildCache\_r921\09203c\cooks\vt-cooked-03\archive\Windows\StackOBot` | `311950e87ab9430b5e452fe4c07ca4312f2a7ec7be5d4395450efedcacf927c8` |
| `E:\IA_BuildCache\_r921\09203c\cooks\vt-cooked-05\archive\Windows\StackOBot` | `311950e87ab9430b5e452fe4c07ca4312f2a7ec7be5d4395450efedcacf927c8` |
| `E:\IA_BuildCache\_r921\09203c\cooks\vt-cooked-06\archive\Windows\StackOBot` | `3c1351952eb9de316fdc3488c5a60f3357a5339995c15f63662ed3a089e890e1` |
| `E:\IA_BuildCache\_r921\09203c\cooks\vt-cooked-08\archive\Windows\StackOBot` | `fe338251aab6bbfef0f0dc39baf7b2d91ce9e2356ba06d9763da314b28b20651` |
| `E:\IA_BuildCache\_r921\09203c\cooks\vt-cooked-09\archive\Windows\StackOBot` | `0f79605d3d9e66e6fc0685afaededf3430f5368db35fa452f1adf307c7315bcb` |

Interrupted/invalid cook attempts -02/-04/-07 are retained separately under the same cooks root and do not count as proof.

**Levers and settings changed/restored**

- All bench commands ran with `-IAIBench`; they remain excluded from Shipping. Private identity used `IAI.Bench.ProxyVT ... Tex 0`, `no_request`, then `restore`; the no-bind mutant used `IAI.Bench.ProxyVTNoBind 1`. Process exit cleared those process-local settings. The release executable was restored after each staged swap by SHA-256.
- Capture: SVE=1, Delivery=0, Pace=1, Fps=30, Mask=1, TargetMask=1, RunLog=1, FocusGate=0; CaptureBench.Marker=0. Config values included 0/600/30/30/0 for identity, 0/121/48/96/N for final positives, and 0/192/48/192/3 for regressions. The failed early schedule used preFrames=120 and is preserved. Capture starts used seed 777 and oh=0.
- Rendering: AntiAliasingMethod=0, DefaultFeature.AutoExposure=0, EyeAdaptationQuality=0, DynamicGlobalIlluminationMethod=0, Lumen.DiffuseIndirect.Allow=0, ReflectionMethod=0, ScreenPercentage=100, SecondaryScreenPercentage.GameViewport=100, DynamicRes.OperationMode=0, Editor.Viewport.OverridePIEScreenPercentage=0 in PIE. No VT bias was changed.
- Routing/pool: StuckMipRoute=auto, and hold for its regression; Auto.Pool all=0, then stuck_low_mip=1 for Auto legs. Census allreasons and the MainWorld inventory command are read-only diagnostics. Targeted regressions use existing tile/invert modes.
- PIE window size/visibility/realtime changes were bench-only and restored on EndPIE or process exit. Native legs used 1920x1080; original identity and legacy regressions used 942x869. Staged used ForceRes; DEFER 300/850/900 schedules only harness setup/census/start.
- Each regression temporarily created DefaultDeviceProfiles.ini with TEXTUREGROUP_Project02 LODBias=1, MinLODSize=1, MaxLODSize=16384, aniso/point filtering and SimpleAverage mip generation; each file was removed by its exact receipt. The complete 156 saved host settings were restored afterward.
- Fixture-only material swaps, actor destruction, material displacement and EndPIE were confined to transient bench sessions. The ten requested authored assets remain. Build strict flags were restored; MaxParallelActions=2 and TEMP/TMP/UAT log-folder overrides were child-process settings only. No global configuration was written.

Exact per-run commands and restoration receipts: `09203c/levers-used-final.json`, `restoration.json`, each regression profile-created/profile-restored.json, and each staged baseline-exe-restored.json.

**Deviations and failed attempts retained**

1. The inherited four-frame onset failure was not accepted. Genuine private prefetch removed it under the original criterion. Later, FE338251 passed positive/identity legs but the no-request control exposed stale GPU page-table values and five changed frames. Its apparent readiness was invalid. The manual private-region clear in 0F79605D passed identity/no-request but refused all ten positive fires during early uniformity prewarming. Both implementations, failed predictions and binaries remain. The final renderer-managed root/full-range protocol passes the unchanged controls; no fallback or relaxed threshold was used.
2. Fixture/harness corrections were predeclared: actual native viewport selection; saved fixed camera after a staged rotation/isolation failure; and preFrames 120 -> 121 after the first fire contaminated the frozen 0..119 null. ROIs, null+0.05, 8x and 25% rules were not retuned. Label-reader support for the new route was added with must-fail coverage. The inherited extra keyboard-idle requirement was removed to match the brief; no foreign editor, GPU, memory or disk limit was relaxed. Post-capture hold shortened from 10 to 1 seconds after complete capture/drain; interrupted runs were excluded.
3. Foreign GPU activity interrupted earlier native, staged, authoring and cook attempts. Only owned PIDs, or creation-stamp-verified owned child trees, were stopped. Those launches and failures are retained. Final required captures all exit 0. Slow later Editor startup did not change any timeout or criterion.
4. The first RunUAT.bat cook unexpectedly entered BuildUAT (3.83 seconds, 0/0), touching six generated engine tool intermediates and an AutomationTool Saved cook log. No engine source or binary changed. AutomationTool.dll remains SHA-256 05480cdcfe167797f39c79932095609c2f8de2ae74439a9901b4c79a95e4a576. The generated files are AutomationTool.csproj.AssemblyReference.cache (252,608 bytes; c3ff506c8e4dbcc44df97cba282c1d8793c6c651576f4450d30ecf1ba8592ac1) and five empty BuildWithSkipAnalyzers markers for EpicGames.Build/Core/Serialization/UHT and UnrealBuildTool. Full paths, times and hashes are in uat-engine-generated-files.json. There was no before backup, so no guessed engine cleanup was performed. Later cooks invoked the existing AutomationTool.dll directly with -nocompile and redirected temporary/log paths. This incidental engine-filesystem write is a disclosed deviation.
5. The build host used detached feature commits because the named branch was attached to the source worktree; every build/leg records the exact commit. It is restored to its original detached commit. Three earlier local commits retain non-Conventional-Commit subjects (83dfe55, 8829491, 74e2c53); history was not rewritten. Optional second-host stretch was skipped after the mandatory proof; that host was not changed. This remains a bounded single-layer fixture proof, not an office delivery or a claim of support for arbitrary VT stacks/assets.

**NEEDS-DECISION items**

None blocking this bounded proof. Owner follow-up, ruling item 4: the minimum effective VT top is one 128px tile, so the unchanged 8x Auto rule needs roughly 1024px on screen. All six measured Auto picks are 1280px / 128px = 10x with visible blur, but yield on the first office host can be limited. No selection rule or threshold was changed or proposed.

Evidence root: `E:\IA_BuildCache\_r921\09203c`. Final proof tables are backed by final-c80-positive.json, the per-run pixel/E1/label results, can-fail-final-c80ef9b.json, mainworld-inventory-final.json, the build/audit folders, archive manifests, final-source-certification.json, restoration.json and push-final.json. Earlier unsuccessful and superseded results remain identifiable in their separate run folders.

VERDICT: GREEN
