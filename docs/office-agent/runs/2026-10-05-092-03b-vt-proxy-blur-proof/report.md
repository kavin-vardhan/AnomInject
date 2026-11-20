VERDICT: NEEDS-DECISION

Drop 0 bound a genuine private VT, but four captured frames visibly blurred and failed identity.  
Branch `feat/vt-proxy-blur`, local head `465f2e7510b0a4e077c8f36ec4f5595e5db65421`.  
The build host is restored; production routing remains unchanged and nothing was pushed.

Report file: [report.md](D:/IntrusiveAnomalies/_relay/runs/2026-10-05-092-03b-vt-proxy-blur-proof/report.md).

The implementation is a constructor probe, stopped before production `proxy_vt` integration. Tested source is `efd990d99d6e25aaddb6b9c176a00c65d6843455`, based on `release/2026-10-05` at `d7eee97731812384d4fd00af9032254923363ada`.

- [AnomalyPrivateVT.h:11](E:/IA_BuildCache/_r921/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/AnomalyPrivateVT.h:11) defines the transient UTexture, virtual material/resource queries, dimensions, address metadata and strong source reference.
- [AnomalyPrivateVT.cpp:49](E:/IA_BuildCache/_r921/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/AnomalyPrivateVT.cpp:49) validates source/layout and retains borrowed VT data. Line 112 constructs the genuine private resource; line 124 fences release before dropping source retention. Line 41 suppresses the stock editor thumbnail path that writes the source TextureReference. No second UTexture2D owns the source platform data.
- [ProxyVTBench.cpp:31](E:/IA_BuildCache/_r921/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/ProxyVTBench.cpp:31) restores the original binding; line 92 installs the private texture in a private MID. The command requires `-IAIBench` and is excluded from Shipping. Twelve engine-source checks and unchanged source hashes are recorded in [source-evidence.json](E:/IA_BuildCache/_r921/source-evidence.json).

All existing production source files are unchanged. Blurry remains off by default; ordinary hold/proxy and UV/normal retain their existing behavior and VT refusals. No `proxy_vt` annotation, frame route, Auto dispatch or census eligibility was implemented. Stack/mixed-binding admission, source-reimport hardening and production lifetime integration remain unfinished.

The identity proof recorded 480 PIE frames at 942×869 and 30 fps. The private binding reported Drop 0, first mip 0, 2048×2048, tile 128, virtual=1, one resource and one producer. The frozen criterion included binding and restoration edges. Reference frame 100 was compared with clean frames 0–119 to obtain the null; the target's nonzero clean variation is retained in the figures below. The [checker](E:/IA_BuildCache/_r921/AnomalyInjector/tools/proof09203/check_identity.py:41) uses mean absolute RGB difference on a 0–255 scale.

| Identity measurement | Result |
|---|---|
| Target null / allowed limit | 6.579029 / 6.629029 |
| Target peak after binding | **11.678702 — FAIL** |
| Failing session frames | **124–127**, engine frames 3013–3016 |
| Measured transient | Four samples at 30 fps: 0.1333 s; frame 128 returned within the gate |
| Later private binding, frames 240–294 | Maximum 6.593956, within the limit |
| Later restored target, frames 360–479 | Maximum 6.579029, within the limit |

The constructor did bind through the compiled VT sampler and later rendered within the null range. The transient is consistent with demand-loaded fallback pages; no page-level trace established the cause. The explicit identity stop condition still applies. See the [baseline frame](E:/IA_BuildCache/_r921/runs/identity-pie-01/out/session_20261005-160826/Actual_Frames/frame_00100.png), [failing frame 124](E:/IA_BuildCache/_r921/runs/identity-pie-01/out/session_20261005-160826/Actual_Frames/frame_00124.png), and [complete pixel result](E:/IA_BuildCache/_r921/runs/identity-pie-01/identity-result.json). Raw captures were not altered.

| Co-user isolation during this identity cycle | Allowed limit | Peak after bind | Result |
|---|---:|---:|---|
| Shared-VT co-user | 0.384127 | 0.334197 | Within limit |
| Ordinary-texture control | 0.387558 | 0.337652 | Within limit |

| Positive proof | Targeted fires | Auto fires | Status |
|---|---:|---:|---|
| PIE | 0 / required 5 | 0 / required 3 | Not run after identity stop |
| Staged | 0 / required 5 | 0 / required 3 | Not cooked, staged or run |

Target-only masks/bboxes, positive onset/offset, E1 t50 zero-unflagged edges and positive annotation/label agreement are **unproved**. The empty-event capture passes the strengthened annotation/label checker with zero mismatches; it has zero labelled frames and zero measured target-mask frames. That is not positive-label certification.

| Revert and lifetime | Result |
|---|---|
| One explicit bind/restore cycle | Later restored pixels within null; resource/producer counters 1/1 → 0/0 |
| Ten consecutive cycles/allocation trend | Not run |
| Destroy target mid-event; interrupted active event; active world teardown | Not run |

| Can-fail | Result |
|---|---|
| Release `218BA4C7` refusing this VT target with zero events | Not run after stop |
| Installed-without-binding/non-VT-in-VT-slot mutant | Not run after stop |
| Supplemental pixel-reader mutation | In-memory +32 RGB target mutation caught: MAD 31.999994 > 6.629029; raw frames unchanged |
| Audit controls | Shipping-lever mutant, appended semantic string and damaged archive-copy hash all caught |

| Required final PIE regression | Fires | E1 result |
|---|---:|---|
| Existing hold fixture | 0 / 3 | Not run |
| Existing ordinary proxy fixture | 0 / 3 | Not run |
| UV corruption fixture | 0 / 3 | Not run |
| Normal corruption fixture | 0 / 3 | Not run |

The final tested source passed these build and archive gates. Earlier failed attempts exposed unity/member name shadowing and a missing direct Texture2D include; their logs remain available. No warning suppression was introduced.

| Gate | Result and evidence |
|---|---|
| Strict Game and Editor | Each 0 errors / 0 warnings; 69 independent headers per target; zero strict lever problems |
| Normal Game and Editor after strict restoration | Each 0 errors / 0 warnings |
| Strict build restoration | Original Build.cs hashes restored; generated header TUs removed; host status restored by harness |
| Source/binary lever audit | 54 registrations, zero failures; disposable Shipping mutant failed R1 |
| String scan | 152 candidates reviewed, zero unresolved; 20 exact non-text pointer/RVA fragments have byte/address evidence; appended semantic mutant caught; self-comparison zero differences |
| Archive/re-hash | Both six-binary probe archives rehashed; six restored baseline binaries rehashed; damaged scratch copy rejected |
| Source hygiene | Comment-strip and diff checks clean; only three added runtime source files; inspected engine source hashes unchanged |

Evidence: [strict summary](E:/IA_BuildCache/_r921/builds/probe-strict2/summary.json), [string result](E:/IA_BuildCache/_r921/audits/probe-efd990d-5899E2A4/strings-result.json), [lever can-fail](E:/IA_BuildCache/_r921/lever-can-fail.json), [archive can-fail](E:/IA_BuildCache/_r921/archive-hash-can-fail.json), [final state](E:/IA_BuildCache/_r921/final-state.json).

Commits pushed: **none**. These seven commits remain local on `feat/vt-proxy-blur`:

- `aa97116003f74f71634c9239705817c6f83d8b9f` — feat(stuck-low-mip): add fenced private VT constructor probe
- `83e58265547796a4b9f20f85e5dabf3b6f0f93fb` — fix(stuck-low-mip): isolate VT probe names in unity builds
- `bdd937804a1d3c95a26d3f04891ca4baab798958` — fix(stuck-low-mip): avoid inherited texture member shadowing
- `a92a1262229ff255d722c489e78be744215ae374` — docs: record private VT prototype and pending proof gates
- `efd990d99d6e25aaddb6b9c176a00c65d6843455` — fix(stuck-low-mip): include source type for standalone VT header
- `af13fa3a05a7852a94cb566b99c115b1c0ec4626` — test(stuck-low-mip): preserve failed VT identity proof harness
- `465f2e7510b0a4e077c8f36ec4f5595e5db65421` — docs: record VT identity stop and restored build host

Archives:

- Tested probe: `E:\IA_BuildCache\_r921\archives\probe-efd990d-5899E2A4`.
- Earlier unlaunched probe: `E:\IA_BuildCache\_r921\archives\probe-bdd9378-01BA2183`.
- Original/restored baseline: `E:\IA_BuildCache\_binary_baselines\release-2026-10-05-23098fa-218BA4C7`.
- Six new fixture assets, rehashed: `E:\IA_BuildCache\_r921\fixture-archive`. No cooked fixture archive exists.

| File | Tested probe SHA-256 | Restored baseline SHA-256 |
|---|---|---|
| `StackOBot.exe` | `5899e2a420cc30dbc2ce131f1547270776373e725a6687235000d58af0251c4d` | `218ba4c79bb2bf6d9e14706772c04b3c53027cc05b84d4ab100a3349b650b7d6` |
| `UnrealEditor-AnomalyBench.dll` | `1bb2d486ed4824a16f660b317a2d55b31c36f294206920895620695855027469` | `9f967665827cd1358b5f914466c36ecc45e47e5e972c749aa0221283bc92dfce` |
| `UnrealEditor-AnomalyCapture.dll` | `bd56ffbfc0bad3b1d221aecbc265bfe124d82ed4bacad46108f66a51b6da9a46` | `470ebe5f6c5d8fa11d9872392035c62c2bab2480aee462a4d4d58ac59e8a5be3` |
| `UnrealEditor-AnomalyControlServer.dll` | `f2327e7721f44955d0db5e529204ced9a1fd1089aae62ab5137c93d20b68f74c` | `0c9c329bedf22be975a09042eb242a21cda726fcab63553240a46685b1dd8cf1` |
| `UnrealEditor-AnomalyInjector.dll` | `36d16adc1d71c330771e1dc92e8e145c2b1860c4a0c09579de6e3867131bc9c1` | `45a1512f0c7b59e321d8de97dd03e23b9bee175f503f974fb5a200f99b5a1b90` |
| `UnrealEditor-AnomalyShaders.dll` | `d728647dc3f39656afcb5e9dcc1d6d3ab687888c3f9ebafab1a6f1bf592f6b4b` | `43d78d878cd1e8cd41b23db1fb747fc5c6bd6a103cb5089bb4fb984b1afc47db` |

The earlier probe's `StackOBot.exe` hash is `01ba218382822ee47e9e733e5d840222e75d7022daede26ed0ef4bb7e4748b10` and its `UnrealEditor-AnomalyInjector.dll` hash is `9b6f9fd1f37b1525f9badd1164f05a0b93477dadf049067c1da4d00edc25c5ea`. Its other four DLL hashes equal the restored baseline values above. Full archive manifests and [all probe re-hashes](E:/IA_BuildCache/_r921/all-probe-binaries-rehashed.json) are retained.

The deliberately damaged, never-launched hash-check copy `E:\IA_BuildCache\_r921\archive-hash-mutant.dll` has SHA-256 `255f36360b44e2c9ae7c1ef5e55b045fc9f7c15d68a9f2b19291c5f246eabee3`; it is not a candidate build.

Levers and state were handled as follows:

| Change | Restoration/disposition |
|---|---|
| Host plugin checkout and six binaries | Restored clean, detached at `23098fa7415f148e935acbd4a97e18da04e6c46c`; original hashes verified |
| Strict unity/PCH/IWYU/include-order settings and generated TUs | Restored by the strict harness; max two compile actions was process-local |
| `TEMP`/`TMP` → job scratch | Child-process environment only |
| `-IAIBench`; `IAI.Bench.ProxyVT StaticMeshActor_77 Tex 0` | Explicit `restore`; counters zero; owned editor exited normally |
| Auto pool all off; capture config `0 600 30 30 0`; SVE, Pace, Mask, TargetMask, RunLog on; Delivery/FocusGate off; 30 fps; per-run `oh=0`; marker off | Process-local; editor exited; saved settings restored |
| AutoExposure/EyeAdaptation off; AA=0; Lumen diffuse, dynamic GI and reflections off | Process-local; no default/config policy change |
| New texture VT flag/materials/map | Six new fixture assets retained as authorized evidence; existing source textures unchanged |
| Config/Saved Config | All 156 original files verified/restored; two job-created crash-client INIs removed by receipt |

Protected release/master/m51/tag refs are unchanged. Staging and the optional second host were untouched. No foreign PID or security setting was changed; no engine source/config, global/pool VT bias or forced source residency was used. Generated build/cache outputs and authorized fixture/evidence files remain in the named host/work areas.

Deviations and limits:

- The explicit identity stop prevented production integration and every later proof leg. The failure threshold was neither weakened nor retuned. Steady identity is not reported as an all-frame pass.
- The embedded PIE viewport ignored the requested 1920×1080 window dimensions. ROIs were projected from the authored planes using the actual 942×869 view and fixed 90° horizontal FOV. Actual target coverage was 28.194%, above the existing 25% census ceiling, and its roughly 565-pixel longest side cannot establish the planned 8× ratio against a 128-pixel VT top. Auto readiness was not proved or adjusted after failure.
- The deliberately named-plus-constant VT material was authored; compiled stack layout and atomic refusal were not proved.
- MainWorld's read-only scan covered 348 mesh actors, 1031 slots and 3097 parameter names. No named VT was found among handled materials, but 96 dynamic-material lookups were unhandled. Whether an eligible MainWorld target uses either previously recorded VT object remains **not established**.
- Fixture authoring emitted an object-path deprecation warning and source-control checkout dialogs for new uncontrolled assets; all six saved assets were verified. PIE emitted a handled distance-field matrix-NaN ensure before the private binding and editor-world getter diagnostics from the reused PYSTEP driver. Both owned engine processes exited 0. Those runtime diagnostics are retained and are separate from the clean build results.
- D fell below 50 GiB before strict compilation; five-minute checks cleared the floor after about 20 minutes. The foreign editor closed without intervention. Both launches passed fresh idle/disk gates, and runtime observations found no foreign editor. Final free space: D 77.36 GiB, E 193.95 GiB.
- Restoration initially encountered missing parent folders for old crash-client INIs after editor cleanup. The helper was corrected and every backed-up config file restored. Only the two new job-created INIs were explicitly deleted by receipt; no pre-existing data was manually deleted.
- The optional plotting helper could not run because Matplotlib is absent. Raw images, per-frame numeric traces and tables provide the evidence; no gate depends on that helper.

The requested status block, gotchas G462–G463, [journal](E:/IA_BuildCache/_r921/AnomalyInjector/docs/sessions/2026-10-05-092-03-vt-proxy-blur.md), architecture note and client-readme §8.3aa paragraph are committed with this stopped status. The feature worktree is clean. First-attempt receipts and its report were preserved.

NEEDS-DECISION: a ruling on the four-frame Drop 0 binding transient is required before resuming implementation. The evidence supports genuine binding and later rendering, but it fails the frozen identity requirement at onset. No exception, substitute route or further runtime work was assumed. The orchestrator must independently review this evidence.

VERDICT: NEEDS-DECISION
