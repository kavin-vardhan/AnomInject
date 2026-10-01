# 090-10b — "Nothing corrupts" on the second office host: the editor's shader-map flag refused every texture-corruption fire; the census was O(N²)

Date: 2026-10-01 · Brief `_mailbox` 090-10b (Code, Opus 5.5, headless, URGENT) · Heads at start: fix `b7da624`, m53 `ff868b2`
(the office ran m53 `31078c7`, exe `8131479A`; its code is `dcdb95b`).

> ⚠ **THIS SESSION ENDED INTERRUPTED** (its mailbox watcher died at 17:38; no final message). The code, builds, archives and legs
> below were complete by then; only the old-build bisect and the docs were in flight. **This journal was written by 090-10b2
> (`2026-10-01-090-10b2-m52-pie-wrapup-census.md`) from 090-10b's own run records** (`E:\IA_BuildCache\_r910b\runs\*.json`,
> session folders under `_r910b\out\`, logs under `_r910b\ev\`), its drafts (`_r910b\journal_full.md`, `builds_section.md`) and the
> pushed heads. Nothing here is reconstructed beyond those records. The owner pushed m53 `0a69ea3` and fix `cf422d5` himself.

## 0. What the office saw (numbers only, from the owner)
Second office host, plugin replaced wholesale with m53 `31078c7`. stuck_low_mip, uv_corruption and normal_corruption fired
"multiple times"; **nothing visibly corrupted**. `IAI.TexCorrupt.Census` read 23 candidates, 23 refused: `shader_map_incomplete` 21,
`host_mid` 1, `partial_footprint` 1, the same every time. `IAI.TexCorrupt.Census all` **froze the game**. Neither office project uses
Nanite. The office runs the plugin in the editor (Play In Editor). (`_bates_reads\2026-10-01-second-office-host-m53-census-owner-reads.md`)

## 1. Reproduction at home — it is the editor, not the content
Harness `_reviews\090-10b-*` (common, go, run.ps1, pie.py, census-timing, edges). Three ways to run the same fixture leg:
- **staged**: the BenchGate package, exe swapped, container DC2 (`45E2FA86`, which carries `CB_TexCorruptLevel`);
- **editor `-game`**: `UnrealEditor.exe <host uproject> <map> -game` with the build's editor DLLs swapped into `_r53_host` (GIsEditor 0);
- **true PIE**: `UnrealEditor.exe <host uproject> <map> -pie -ExecCmds="py 090-10b-pie.py"` — a slate-tick driver that waits for the
  PIE world and issues the leg's console commands in it (GIsEditor 1, EWorldType::PIE).

Pixel reading (declared before any read, `090-10b-common.py: pixels`): per event, the mean absolute RGB difference inside the event's
drawn box between each labelled frame and the last clean frame before it; **CORRUPTS** iff the median is ≥ max(4.0, 4 × the null), the
null being the same difference between the two clean frames before the event. Fixture `CB_TexCorruptLevel`, AA-off arbiter, targeted
uv on `StaticMeshActor_0`, targeted normal on `StaticMeshActor_12`, 120 frames.

| build (m53 code) | staged uv / normal (visibly corrupt) | editor `-game` uv | PIE uv / normal | PIE refusal |
|---|---|---|---|---|
| `45FD344C` (090-04) | 6/6 · 6/6 | — | **0 events · 0 events** | `shader_map_incomplete` 6 · 6 |
| `97D292F8` (090-05) | — | — | **0 events** · (not run, bisect interrupted) | `shader_map_incomplete` 6 |
| `A3EC29D8` (090-07) | 6/6 · 6/6 | — | **0 · 0** | `shader_map_incomplete` 6 · 6 |
| `8131479A` (090-09, the office build) | 6/6 · 6/6 | 6/6 | **0 · 0** | `shader_map_incomplete` 6 · 6 |
| `85395644` (090-10) | 6/6 · 6/6 | — | **0 · 0** | `shader_map_incomplete` 6 · 6 |
| `01DBA661` (`c0896df`, fix attempt 1) | — | — | **0 · 0** | `shader_map_incomplete` 6 · 6 |
| `2F924FB3` (`c7bbc51`) | 6/6 · 6/6 | — | 6/6 · 6/6 | — |
| `07A65316` (`9f3bae1`) | 6/6 · 6/6 | — | 6/6 · 6/6 | — |
| **`65607703` (`0a69ea3`, final)** | **6/6 · 6/6** | — | **6/6 · 6/6** | — |

**Reading.** Every build corrupts in the staged game (and the office build in editor `-game`), and **no build before the fix corrupts
in PIE**: the decision tree refuses every slot `shader_map_incomplete` (`TEXCORRUPT-REFUSED ... final=shader_map_incomplete step=V1`).
A 090-09 regression is ruled out: the Nanite probe is registered in PIE (`IAI-STARTUP ... nanite_probe=registered`), no
`REFUSED-NANITE*` line fires on the fixture, `nanite_midevent_reverts` is 0, and `45FD344C` (090-04) fails the same way. The bisect
(PIE legs on the archived 090-04/05/07/09/10 builds) was running when the session died; the five builds it reached all fail in PIE.

## 2. The cause
`TexCorruptTree.cpp` `ReadActiveBindings` (m53 S1 `782bb5b`, line 712 at `dcdb95b`):

    if (!Res->IsGameThreadShaderMapComplete()) { return; }   // -> slot refused shader_map_incomplete (S6)

In the editor this whole-map flag stays 0 for host materials that visibly draw their own textures (nothing compiling:
`LogShaderCompilers: Shaders Compiled: 0`). Completeness is a whole-shader-map property (`MaterialShader.cpp:2651-2769` walks every
expected permutation and vertex-factory layout), the editor compiles on demand (`r.ShaderCompiler.JobCacheDDC=true`,
`r.ShaderCompiler.JobCache=1`, both Constructor, read back at the office), and a mesh draws with the shaders its material has for its
vertex factory (`BasePassRendering.cpp:507` `TryGetShaders`). The m47 finding again (G232(b)). **Measured, not assumed:** fix attempt 1
(`c0896df`, exe `01DBA661`) asked the render-thread flag instead and PIE still read 0 events
(`TEXCORRUPT-SHADERMAP 'M_TC_UC1' game_thread_complete=0 render_thread_complete=0` while M_TC_UC1 draws its texture). Cooked games have
complete maps, which is why the staged bench never saw it; m53 had never run in PIE (journals 082 to 090-10).

## 3. The fix (m53 `c0896df` census + attempt 1, merge `c7bbc51` the per-VF test, `0a69ea3` corruptors; code head `0a69ea3`)
- **Host shader readiness (S6)** — `HostShadersReady(Material, Component, World)`: admitted if the whole-map flag is set; else (editor)
  the material's compile must be finished (`FMaterial::IsCompilationFinished`) **and** its mesh shader map for the component's vertex
  factory (`FLocalVertexFactory`, `FInstancedStaticMeshVertexFactory`, `TGPUSkinVertexFactory<Default|Unlimited>`) must hold shaders. One
  `TEXCORRUPT-SHADERMAP ... -> admitted|refused` line per material. ⚠ This landed inside the merge commit `c7bbc51` (its message says so),
  on top of `c0896df`, whose render-thread-flag version was refuted in PIE. 🔻 *090-10b2 refined it — see that journal: the whole-map
  short-circuit is gone, the per-VF test asks for the base pass's VS and PS, identity is checked first, and the refusals are named.*
- **Corruptor readiness** (`0a69ea3`) — `CorruptorShadersReady`: the plugin's two corruptor materials are complete, else (editor) one
  `EnsureIsComplete` (the m47 prewarm's own call, which otherwise runs only at capture start) and check again. Before it a PIE census
  outside a capture read `corruptor_not_ready` on all 343 MainWorld candidates (both the office build and `9f3bae1`).
- **Eligibility-aware auto-pool** for the texture-corruption types: candidates for a drawn uv/normal id are filtered through the read-only
  tree (`AnomalyTexCorrupt::IsEligibleTarget`, census mode, actor pointer). ⚠ Changes which target a seeded auto-pool draw selects for
  those two ids (`G140`-shaped boundary).
- **Editor texture compile**: a texture still compiling (`IsDefaultTexture`) is refused `resource_not_ready:compiling` instead of
  `GetPlatformData()` forcing a synchronous wait (`Texture2D.cpp:298-301`).

## 4. Proof from pixels (final exe `65607703`, code `0a69ea3`)
Same reading as §1. stuck_low_mip on the MainWorld rock `StaticMeshActor_UAID_A036BC6AB247EBF902_2044254803` with
`IAI.Targets.AllowNanite 1`.

| leg | where | events | visibly corrupt | median drawn-box diff | max null |
|---|---|---|---|---|---|
| targeted uv_corruption `StaticMeshActor_0` | PIE | 6 | **6/6** | 39.80 – 53.59 | 0.337 |
| targeted uv_corruption | staged game | 6 | **6/6** | 42.82 – 58.46 | 0.344 |
| targeted normal_corruption `StaticMeshActor_12` | PIE | 6 | **6/6** | 9.35 – 17.49 | 0.347 |
| targeted normal_corruption | staged game | 6 | **6/6** | 11.14 – 22.31 | 0.343 |
| targeted stuck_low_mip, MainWorld rock (AllowNanite 1) | PIE | 4 | 3/4 (1 unjudged) | 4.72 – 5.44 | 0.458 |
| targeted stuck_low_mip, MainWorld rock | staged game | 3 | 1/3 | 3.93 – 7.06 | 1.957 |
| auto-pool, three types, MainWorld (AllowNanite 0) — stuck_low_mip | PIE | 5 | **5/5** | 4.26 – 4.79 | 0.531 |
| auto-pool, three types, MainWorld (AllowNanite 0) — stuck_low_mip | staged game | 6 | 5/6 | 4.00 – 5.44 | 0.520 |
| auto-pool uv + normal, fixture (`SetMinScreenCoverage 0`) | PIE | uv 11, normal 17 | uv 7, normal 11 | | |
| auto-pool uv + normal, fixture | staged game | uv 11, normal 17 | uv 5, normal 6 | | |
| auto-pool three types, fixture | PIE / staged | uv 8, normal 10 / same | 7, 6 / 4, 4 | | |

The staged rock leg reads the same on the office exe `8131479A` (1/3, medians 3.93 – 7.06, null 1.96 so its threshold is 7.83).
**Edges** (`090-10b-edges.py`, mask-independent: first frame whose drawn-box difference crosses the threshold vs the first labelled
frame, and first frame back under it vs the frame after the label): uv PIE 5/6, uv staged 5/6, normal PIE 5/6, normal staged 5/6 in sync
at **both** edges (onset 0, end 0); the sixth event of each is UNJUDGED (its label ends at frame 119, the last captured frame); **0
OFFSET**. stuck_low_mip, targeted rock in PIE: 3 of 3 judged in sync. ⚠ **stuck_low_mip on MainWorld otherwise starts 2 – 4 frames
before its pixels** (onset +2..+4 on 3 of 5 PIE auto-pool events, 5 of 5 staged, 2 of 3 staged rock; end edge 0 everywhere judged) —
**identical frame for frame on the office exe** (`S_8131479A_SMROCK` onset 8/0/4, `S_2F924FB3_MWAUTO` 2/4/3/2/2), and the plugin's own
`stuck_mip_partial_onset_frames` reads 16 on the staged auto-pool leg. Pre-existing m52 partial-onset behaviour, not changed here (§10).

The invisible auto-pool fixture events are labelled events on targets whose texture changes little under the corruption (flat or
periodic textures, e.g. the normal map on `StaticMeshActor_22`); the same on the staged game and in PIE and on every build that corrupts.

**Can-fail.** Exe `8131479A` (the office build) under the same PIE legs: targeted uv **0 events**, targeted normal **0 events**,
auto-pool **0 events** — every slot refused `shader_map_incomplete` (6, 6, 28). Exe `01DBA661` (render-thread flag) also reads 0 and 0:
the first fix attempt is the can-fail for the second.

## 5. Never silent again (fix branch `6a7a29e`, `b9716ea`, `cf422d5`; merged into m53)
- **Per type, per round:** `Auto.Yield <type>: 0 of N candidates eligible - <reason> n, ... (auto-pool|targeted, R round(s) since the
  last line). Nothing of this type was applied; the line repeats at most every 10 s while it stays at zero.` Reasons come from the Nanite
  gate, the census, the texture-corruption read-only tree, and `IAnomaly::GetLastRefusalReason()` (stuck_low_mip reports its dominant
  HELD NONE bucket: `shared`, `not_streamable`, `virtual`, `too_small_for_ratio`, ...).
- **No visible candidate:** a round with nothing on screen says so and names the coverage cull and poll radius (rate-limited).
- **Startup:** one `IAI-STARTUP` line per Game/PIE world (world type, editor, Nanite policy, probe registered/MISSING,
  r.VirtualTextures, r.Nanite.ProjectEnabled).
- The dashboard snapshot has no field for these; they go to the log, and the yield text is the in-game auto HUD's `Last:` line.

## 6. The census freeze
Old build (one frame): MainWorld `all`, 343 candidates — **0.972 s** in a single frame on the staged game; fixture, 82 candidates —
**0.807 s**. New build (`65607703`, staged): MainWorld **0.008 s** from `begin` to `end`, fixture **0.001 s**, identical counts
(MainWorld uv eligible 25 / refused 318, normal 9 / 334; fixture uv 69 / 13, normal 8 / 74), `stats_unchanged=1`. Mechanism (G441):
each candidate was resolved by name through a whole-world `TActorIterator` scan, twice per family and for two families, inside one frame
— work proportional to candidates × actors. Now: actor pointers, about 4 ms of work per frame, `progress scanned=K of=N seconds=S` every
2 s, a hard stop at 120 s, a second census refused while one runs. PIE census on MainWorld after `0a69ea3`: uv eligible 17 / refused 326
(`nanite_unmaskable` 266, `partial_footprint` 46, **`shader_map_incomplete` 8**, ...), normal 0 / 343; before it, `corruptor_not_ready`
on all 343 (office build and `9f3bae1`). 🔻 *The office's census after `0a69ea3` completed without a freeze (15,906 candidates).*

## 7. What only the office can confirm
Whether the office content has eligible uv/normal/stuck_low_mip targets. Read-out (fix `b9716ea`, also on m53), stdlib Python, numbers
only: `python Plugins\AnomalyInjector\tools\anomaly_refusal_counts.py --selftest`, then the same with `"Saved\Logs\<project>.log"`.
🔻 *The owner's census on `0a69ea3` (2026-10-01 ~20:30) answered it: 15,906 candidates, 0 eligible, `runtime_lod_bias` 14,454 first —
the next round's subject (090-10c, Codex's `compatibility-review.md`).*

## 8. Builds, gates, archives, pushes

| | m53 `feat/m53-uv-normal-corruption` | fix `fix/m52-label-timing` |
|---|---|---|
| code head (proven) | `0a69ea3` | `cf422d5` |
| exe | **`65607703`** | **`DA903919`** |
| normal build | game + editor exit 0, 0 errors 0 warnings | same |
| strict include pass | **0/0**, 64 header TUs, Build.cs restored | **0/0**, 58 header TUs, restored |
| string scan (UTF-16, vs the previous archived exe) | vs `85395644`: 79 = 59 new 090-10b strings + 4 census help text (old/new) + 16 one-byte string-pool artefacts | vs `F636D0A7`: 53, this round's strings + one-byte artefacts |
| lever audit | 47/47 PASS (exe and DLLs) | 34/34 PASS |
| C++ suites | base 6/6, mutants 23/23 failing | base 5/5, mutants 20/20 failing |
| Python suites | 27/27 | 19/19 |
| archive | `_binary_baselines\m53-0910b-65607703\` (6/6 re-hashed) | `_binary_baselines\m52fix-0910b-DA903919\` (6/6) |

Provenance of `65607703`: `0a69ea3` committed 16:14:20; editor and game built 16:14–16:18; the strict pass relinked the exe at 16:28
(`summary.json`: strict and normal 0/0 on both targets, Build.cs restored). Code-only, no cook: staged legs swapped the exe into BenchGate
over the DC2 container. ⚠ At the interrupt BenchGate held `65607703` over DC2 (not the m53 S1 set) and `_r53_host`'s editor DLLs were the
bisect's `97D292F8` copies; 090-10b2 found and recorded both. The 090-10b session pushed nothing; the owner pushed both heads.

## 9. Notes
- **PIE harness:** `-ExecutePythonScript=<driver>` with `-pie` ends PIE and closes the editor one frame after the script; `-ExecCmds="py
  <driver>"` stays up. Mechanism not established (G440).
- **Coverage cull in PIE:** the default PIE viewport here is 824 × 869; the fixture's candidates cover 0.2 – 4.5 % of it, under the 6 %
  default (m19), so auto-pool in PIE reads `no_visible` on every build. The auto-pool proof legs set `IAI.SetMinScreenCoverage 0` and say
  so; the shipped default is unchanged.
- **Strays:** a 0-byte `090-10b-go.py` was written into the main checkout by `[IO.File]` with a relative path (G442); moved out to
  `E:\IA_BuildCache\_r910b\stray_from_main_checkout\`.

## 10. Open, not changed here
- stuck_low_mip's label starts 2 – 4 frames before its pixels on most MainWorld events (identical on the office exe; m52 partial onset).
- On MainWorld's settled view the visible uv/normal candidates are Nanite (`nanite_unmaskable`) or `texture_not_parameter`.
- In PIE, the fixture's stuck_low_mip target `StaticMeshActor_77` refuses `shared` on every build: 🔻 *090-10b2 showed this is the
  fixture's designed purity negative control (it shares `M_TC_M52Shared` with `_78` in the same world), not a PIE defect.*
