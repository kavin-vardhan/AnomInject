# m53 — UV / normal-map texture corruption — PRE-DECLARED DESIGN AND GATES

## REVISION 3.2 — 082-06b, 2026-09-27. Revision 3.1 plus the S1 source-review fixes; this is the live design.

Codex's S1 source review (082-06) found 6 P2 and 3 P3 in the S1 code. Chat accepted all nine and rejected
declared deviations 5 and 10 (`_reviews/082-06b-chat-ruling-s1-source-review.md`). The code is fixed on this
branch; this revision changes only the plan text those fixes touch, each paragraph marked **🔁 082-06b**, with
withdrawn text struck through. **§R0.000** lists the nine with commits, line ranges and proofs.

## REVISION 3.1 — 082-05, 2026-09-27. Revision 3 plus four plan-text fixes; superseded as the live design only by §R0.000.

**Branch `feat/m53-uv-normal-corruption`, parent `49dece1` (revision 3).** Chat's ruling
`_reviews/082-05-chat-ruling-s1-authorised.md` accepted Codex's delta #2 (`_reviews/082-04-codex-m53-delta2.md`) and
authorised S1 on the condition that Code folds its three P2 fixes (A, B, C) and its P3 into this file as the first
step of S1. This revision does only that. Every paragraph it changes carries **🔁 082-05**; a withdrawn claim is
struck through, not deleted. **§R0.00** lists the four fixes with their line ranges. The rest of revision 3, including
its header below and its §R0.0 table, stands unchanged.

## REVISION 3 — 082-04, 2026-09-27. Superseded as the live design only by the four fixes in §R0.00.

**Branch `feat/m53-uv-normal-corruption`, parent `b118a66` (revision 2, 082-03). PLAN ONLY.** No plugin
source changed, no build, no cook, no editor or game launch, no bench leg, no CaptureBench edit (N2's
fixture and its tools are written in S1), no tag.

**Inputs.**

- Chat's ruling `_reviews/082-04-chat-ruling-m53-revision-n1-n4-delta.md`:
  - N1 = (a): private `RenderCore` / `RHI` dependencies and a per-mip copy;
  - N2 approved, additive only;
  - N3 = 128 MiB set by a CVar, with a collateral-residency diagnostic;
  - N4 = a 2-level tolerance with ≥ 16-level wrong-copy separation;
  - Codex's Δ1–Δ3 and the six PARTIALs of `_reviews/082-03-codex-m53-delta-check.md`, all accepted with
    named fixes.
- What was executed, all read-only:
  - UE 5.1.1 engine source reads (`D:\UESource\UnrealEngine`);
  - plugin source reads of `master` through git objects;
  - offline arithmetic for the budget (§R3.2) and for the wrong-copy levers (§R12.4).

**How to read this file.**

- **§R0.0 is the live resolution table** for this revision: one row per ruled item, with the section and
  line range that implements it.
- Revision 3 amends revision 2 **in place**. Every paragraph it added or rewrote carries **🔁 082-04**.
  Revision 2's wording of a changed paragraph is in `b118a66` and is not repeated here.
- §R0.1 and §R0.2 are revision 2's tables, kept as the record of 082-03. Where revision 3 changes one of
  their rows, §R0.0 says so. ⚠ **Their line numbers refer to revision 2's file (`b118a66`), not to this
  one**; §R0.0's line numbers are this file's.
- The citation conventions and the SUPERSEDED v1 fold below are unchanged.

### Revision 2's header (082-03), kept as written

**Branch `feat/m53-uv-normal-corruption`, parent `946c1bf` (revision 1, 082-01). PLAN ONLY.** No plugin
source changed, no build, no cook, no editor or game launch, no bench leg, no tag.

What was executed, all read-only:

- engine source reads, UE 5.1.1 at `D:\UESource\UnrealEngine`;
- plugin source reads of `master` `4283fc8` through git objects (`master` is now `b5f15a3`, one docs-only
  commit later; `git diff --stat 4283fc8 master -- Source` is empty, so every plugin line cited here is
  also `master`'s);
- a second offline scan of content `.uasset` files, reading tagged properties (dimensions and authored
  mip settings). Its method and output are Appendix C.

**Inputs.**

- Codex's design review `_reviews/082-02-codex-m53-design-review.md`: CHANGES REQUIRED, 5 P1, 6 P2, 1 P3.
- Chat's disposition `_reviews/082-03-chat-ruling-codex-m53-design-review.md`: all twelve findings
  accepted; D1's residency and fallback revised, D7 reversed, D2 and D6 amended.

**How to read this file.**

- **§R0** is the finding-by-finding resolution table, with line ranges in this file.
- **§R1–§R16** are the design. They replace revision 1's design in full.
- Revision 1 (`946c1bf`) is kept verbatim at the end, under a SUPERSEDED fold.
  - Its **source facts** stay citable as `v1 §x`. §R0.2 lists exactly which.
  - Its **design** is withdrawn and may not be cited: route A, the v1 refusal table, the snapshot rule,
    the restore, the strength classes, the gate table, the costs, the stages, and D1–D7 as written.
- Citation conventions are v1's (see the v1 header inside the fold):
  - engine paths are relative to `Engine/Source`, and `[Shaders]` means `Engine/Shaders`;
  - a bare engine file name lives under `Runtime/Engine/…`;
  - plugin `file:line` is against `master` `4283fc8`.
- Anything measured later that contradicts this file is a **finding**. It is recorded as an amendment,
  never folded in silently.

---

## R0. Resolution table

### R0.000 🔁 082-06b — Revision 3.2 resolution table (Codex's S1 source review, chat's disposition)

Codex `_reviews/082-06-codex-m53-s1-source-review.md` (CHANGES REQUIRED: 0 P1, 6 P2, 3 P3); chat
`_reviews/082-06b-chat-ruling-s1-source-review.md` accepted all nine and **rejected declared deviations 5 and
10**. Code base `193bd35`. Pure core `127b7e1`, compile fix `9382173`, harness `accdb9f`. Proofs are the
offline harness (`tools/texcorrupt_pure_test.cpp` on the header the plugin compiles; mutant from
`tools/texcorrupt_make_mutant.py`) plus the source path; **no leg ran**. The in-engine readings listed are the
ones 082-07's legs must show.

| item | ruling (082-06b) | commit | implemented in | proof | status |
|---|---|---|---|---|---|
| **P2-1** NoApply short-circuits the condition | the predicate reads live slots and parameter ownership whatever NoApply is | `034564a` | §R10 `condition_held` (L1378–1387); telemetry (L1359–1360) | Harness [12]: applied → `installed`, NoApply 1 → `slot_not_installed`, NoApply 2 → `no_expected_set`, a lost binding → `binding_readback`. `TexCorruptPure::ConditionHeld` has no NoApply input. The mutant (empty set read as held) fails [12]. In engine: `texcorrupt.condition_detail` and the APPLIED / NoApply 2 lines. | **RESOLVED in code**; leg reading due in 082-07 |
| **P2-2** G-COLL measures injection candidates | every primitive drawn on screen, `GetLastRenderTimeOnScreen` within the window; a truncated or incomplete set is never clean | `f57004d` | §R12.3 `G-COLL` (L1596); §R10 (L1355–1358, L1376–1377) | `GatherCollateral` no longer calls `GetVisibleRenderableActors` or `IsRenderableComponent`. Harness [14]: in a synthetic scene a 2 % prop, a 500 m backdrop, foliage and a translucent-only mesh are measured; the target, a shadow-only primitive and an unregistered one are not. The mutant (target not excluded) fails. `collateral_incomplete` / `collateral_complete` / `texcorrupt_collateral_incomplete_frames` added. | **RESOLVED in code**; in-engine synthetic case due in 082-07 |
| **P2-3** subtype from live state at completion | subtype from the captured event record | `1067481` | §R10 `annotation.json` (L1335–1338) | `AccumulateFrameEvents` reads the frame's captured `texcorrupt.mode` (snapshot telemetry async, capture-moment telemetry sync), keyed by (id, start frame, target). `GetLiveModeName` is deleted. Not pure-testable; the proof is the source path. | **RESOLVED in code**; revert-before-completion leg due in 082-07 |
| **P2-4** one usage flag checked | every applicable flag, engine rule mirrored; undetermined ⇒ refuse; **deviation 5 rejected** | `c297ee2` | §R6.2 S7 (L928); `TexCorruptPure::RequiredUsages` | Harness [13]: 18 offline rows, among them instanced + lightmapped (Codex's case) → `ism+static_lighting`, Nanite ISM, per-LOD and shared lighting, spline, cloth, morph; plus 2 undetermined gaps. The mutant (first-category exit) fails [13]. Declared superset: min-LOD clamp ignored; morph = asset has morph targets. | **RESOLVED in code** (offline rows) |
| **P2-5** partial scratch failure undercounts | every created byte through the two-frame pending ledger; tests of tree order and a failure at every step | `dbc7033` | §R3.2 (L455–457, unchanged rule); `FEventAccount`, `PlanAllocations` | Harness [9]–[11]: 129 rollback sequences (every plan step × fail-before-create / created-then-rejected, plus a failure after all). Each ends with live 0 and pending == created, still pending at frame+1 and 0 at frame+2. Success paths balance at revert+2. The mutant (created bytes un-reserved; pending retired a frame early) fails [10] and [11]. Runtime lever `IAI.Bench.TexCorruptFailStep 2 <ordinal>`. | **RESOLVED in code** |
| **P2-6** NoApply 2 skips the watches | register the same watches; **deviation 10 rejected** | `034564a` | §R12.2 NoApply 2 (L1539–1540) | `RegisterTargetWatch` runs on the NoApply 2 path; it reserves, allocates and touches nothing. | **RESOLVED in code**; EndPlay-while-valid leg due in 082-07 |
| **P3-1** post-revert sample in ticks | count rendered frames | `f57004d` | §R12.3 `G-COLL` (L1596) | Revert stores `RevertFrame` and samples at the first tick with `GFrameCounter >= RevertFrame + 2`, logging `endpoint` on_frame / late / early / cancelled. Harness [14] checks the frame function. | **RESOLVED in code** |
| **P3-2** chain walk accepts an unexamined tail | past the link limit ⇒ refused with a named reason | `c297ee2` | §R5 predicate (L832–835) | `TexCorruptPure::WalkChain` reports `LimitReached`; the slot is refused `host_mid` with sub `<where>:chain_limit_16_unverified_tail`. Harness [14]: 16 links clean; 17 links and a cycle → `limit_reached`. The mutant fails. | **RESOLVED in code** |
| **P3-3** a peak cannot prove balance | judge live and pending returning to zero; peak stays a run peak | `f57004d` | §R9.4 (L1304–1309) | Every revert emits `TEXCORRUPT-LEDGER … kind=post_revert … live pending peak` at revert+2; the rollback line carries live/pending/frame. The gate text is corrected. Harness [10]: the peak is never reset. | **RESOLVED** (gate text + code) |

### R0.00 🔁 082-05 — Revision 3.1 resolution table (the four S1-authorisation fixes)

Line ranges are in this file at revision 3.1. Each row is chat's 082-05 disposition of Codex's delta #2.

| item | ruling (082-05) | implemented in | resolution | status |
|---|---|---|---|---|
| **A** (P2) the diagonal `texelshift` is an identity on a one-texel checker | shift along **one axis only**; re-prove offline at ≥ 32 on every assigned row; strike the false ≥ 128 claim | §R7.1 (L1039–1041); §R12.2 (L1502); §R12.4 (L1600) | `texelshift` samples `+(1/W_m, 0)`. For checker parity `(x+y) mod 2`, `C(x+1, y) = 1 − C(x, y)`, so every texel of chain (b) flips 64 ↔ 192 and every texel of chain (d) flips 83 ↔ 172: the predicted minimum per-texel error is 128 and 89. The diagonal claim is struck through. The proof itself is `texcorrupt_fixture_images.py`'s, run in 082-06 before any leg relies on the fault; if it reads below 32, S1 stops (§R13.1 row 12). | **RESOLVED in design**; proven offline in 082-06 |
| **B** (P2) subtracted counts can hide extra mip loss | pair textures by their actual resident mip levels, applied leg vs null, texture by texture; report every texture with fewer resident mips in the applied leg; stop subtracting counts | §R12.3 `G-COLL` (L1540); §R10 (L1312–1315) | Each sample (Apply, every labelled frame, 2 frames after revert) records every collateral texture's identity and resident level at its frame offset from Apply (`IAI.Bench.TexCorruptCollateralDetail 1`, one `TEXCORRUPT-COLL` log line per sample). The two legs are paired by (texture, frame offset). A texture's deficit is `max(0, null − applied)` resident levels; every texture with a positive deficit is reported by name, and the deficits are aggregated. A truncated collateral set (cap 256) stays flagged incomplete. `texcorrupt.collateral_drops` stays as a within-leg reading and is never subtracted across legs. | **RESOLVED in design** |
| **C** (P2) a budget or streaming-pool bias is reported as `not_fully_resident` | classify it as a `runtime_lod_bias` sub-reason before the residency check; add a fixture row that reaches it | §R4 step 4 (L720–746); §R6.2 T9 (L914); §R6.4 (L1000–1001) | The per-texture budget bias (`FStreamingRenderAsset::BudgetMipBias`) has no public game-thread query, so zero bias is established conservatively from public state. It is possible only on a streamed texture (`bSupportsStreaming`) while `r.Streaming.UsePerTextureBias` is non-zero and `r.Streaming.MipBias` > 0 (`TextureStreamingHelpers.cpp:299`, `:302`; the drops are capped at `GlobalMipBias`, `AsyncTextureStreaming.cpp:356`). In that state T9 refuses the binding `runtime_lod_bias`, sub-reason **`streaming_budget`**, before T10. Fixture row: the F-SYN streamed 2048² texture fired with `r.Streaming.MipBias 1` (packaged), which reads `streaming_budget`, against the same target without it, which reads `not_fully_resident`. | **RESOLVED** |
| **P3** arithmetic | two same-class 4096² maps come to 192 MiB | §R3.2 (L416–418); §R6.4 (L1006); §R12.1 (L1412); §R14 (L1726, L1732, L1758–1759) | `2 · 85.33 + 21.33 = 192.0 MiB`: same-class chains share one scratch set. 213.33 MiB is the figure only for two 4096² maps of **different** classes. Every affected figure is corrected; no admission outcome changes (192 > 128, ≤ 256). | **RESOLVED** |

### R0.0 🔁 082-04 — Revision 3 resolution table (the live one, amended by §R0.00)

Line ranges are in this file at revision 3.

| item | ruling (082-04) | implemented in | resolution | status |
|---|---|---|---|---|
| **N1(a)** mip chains (resolves **P1-2**) | private `RenderCore` + `RHI`; a per-mip copy that reproduces the source's own chain (authored, sharpened, alpha-coverage) instead of regenerating mips; refuse where a per-mip copy is impossible | §R3.4 (L449–626); §R2.5 (L263–308); §R7.3–§R7.4 (L1003–1032, L1034–1069) | Every output mip m is made from the source's own mip m: the corruptor samples it at an explicit LOD with the view mip bias off. Mip 0 is drawn straight into the output through the canvas pair that never regenerates mips. Each mip ≥ 1 is drawn into a 1-mip scratch target of that mip's size and copied into mip m with RDG `AddCopyTexturePass` (`DestMipIndex = m`), the engine's own render-target-update pattern. The output's only mip generation runs once, on the cleared target, at allocation. Refusals: `mip_chain_shape`, `runtime_lod_bias`, `not_fully_resident`, `unsupported_encoding`. A render-thread tripwire counts any source whose RHI mip count differs at draw time. Scratch bytes are budgeted. `architecture.md` and `CLAUDE.md` take the wording "engine modules only; no host-game types" in S1, with the dependency. | **RESOLVED in design**; qualified by `G-ID-M`'s authored-chain rows and the `mipgen` / `mipshift` wrong copies |
| **N2** fixture level | approved, additive only; a new folder; `CB_GateLevel`, `MainWorld` and every existing asset byte-unchanged, verified after the cook; archive the container before the cook; never in a client package; a checklist line | §R12.1 (L1309–1424) | `CB_TexCorruptLevel` and its assets in the new folder `/Game/CaptureBenchTexCorrupt/`, authored by CaptureBench tools written in S1. A source-byte manifest is taken before authoring, after authoring and after the cook. The container is archived and hash-verified before the cook. The cooked `CB_GateLevel` / `MainWorld` chunks are compared through the IoStore listing. The PRE-DELIVERY-CHECKLIST line is written out. | **RESOLVED in design**; built and checked in S1 (`G-COOK`) |
| **N3** budget | default 128 MiB by CVar; over budget refuses, never downsamples; `G7` at 64 / 128 / 256 MiB as a diagnostic; a collateral-residency diagnostic against a NoApply control | §R3.2 (L363–413); §R12.3 (`G7`, `G-COLL`) | `IAI.Anomaly.TexCorruptMaxRtBytes`, compiled 128 MiB. The reservation counts output chains, `drift` snapshots and per-mip scratch. `G7` runs its census at the three budgets. `G-COLL` compares the resident mips of non-target visible textures with a **no-allocation** null. | **RESOLVED**; interpretation I1 (§R15) |
| **N4** tolerance | 2 levels per 8-bit channel; wrong copies ≥ 16; exact-only rejected; per-encoding error distribution | §R12.3 (`G-ID`); §R12.4 (L1498–1553) | Declared: PASS iff max \|d\| ≤ 2 in every channel on every frame in the mask. Every wrong copy assigned to the row must read ≥ 16, or the instrument is invalid. A per-row error histogram is reported beside the verdict. | **RESOLVED** |
| **Δ1** (P1) a runtime material anywhere in the resolved chain | check the effective material and its parents for a MID or a transient outer; fixture: a mesh-asset-slot MID, which must refuse | §R5 (L751–807); §R6.2 step S3 | S3 walks `Resolved` and `Effective` up their parent chains. A link that is a MID, whose outermost package is the transient package, or that was not loaded from a package (`!RF_WasLoaded`) refuses `host_mid`, with `where` and `kind`. Fixture: `IAI.Bench.TexCorruptAssetSlotMid` installs a MID in the fixture's own duplicate mesh asset. | **RESOLVED in design**; `G-REASON`, `G6`; interpretation I3 |
| **Δ2** (P2) a wrong copy that is provably wrong | replace the sRGB lever; prove ≥ 16 offline before relying on it | §R12.2 (L1426–1468); §R12.4 | `TexCorruptWrongEncoding srgb` is withdrawn. New `IAI.Bench.TexCorruptWrongCopy <chanswap\|srgbtwice\|mipshift\|mipgen\|texelshift\|normal\|alpha\|noclear>`, each assigned only to rows it can affect. The fixture tool proves each lever's per-texel error ≥ 32 (twice the threshold) on its rows before a leg relies on it. | **RESOLVED in design**; the proofs run in S1 |
| **Δ3** (P2) S1 gates vs staged behaviour | one bench-only non-identity mode in S1 for `G-BIND` only; `G-RD` for identity only; `drift` gets its own criterion in S2 | §R12.2, §R12.3, §R13 | `IAI.Bench.TexCorruptTileProbe` (a fixed ×2 or ×4 tile, targeted only, not a product mode). `G-RD` = identity redraw stability. New `G-RD-DRIFT` (S2) compares sampled `drift` frames with a one-draw static reference at the same offset. | **RESOLVED** |
| **P1-2** (partial) | resolved by N1(a) | §R3.4 | as N1(a) | **RESOLVED in design** |
| **P1-3** (partial) | admit only `snapshot_mip == 0`; refuse any runtime LOD bias (global, per-texture, cinematic) with a named reason | §R4 (L649–747); §R6.2 steps E4, E5, T9 | Admission requires `AssetLODBias == 0`, `MaxNumLODs == M` and full residency, so `snapshot_mip` is 0 by construction. `runtime_lod_bias` has four sub-reasons: `global_sampler` (`r.MipMapLODBias`), `global_streaming` (`r.Streaming.MipBias` with per-texture bias off), `cinematic` and `per_texture`. Revision 2's clause admitting `AssetLODBias > 0` is withdrawn. | **RESOLVED** |
| **P2-8** (partial) | any excluded spatial map in a selected slot refuses the whole slot; only a proven non-spatial map is exempt, with a fixture | §R6.2 A1, §R6.3 (L892–919) | A binding excluded for any reason now blocks its slot; T3's "intentionally untouched" is withdrawn. The only exemption is a binding whose cooked chain is a single 1×1 mip (`non_spatial_exempt`, with an F-SYN fixture). | **RESOLVED** |
| **P2-9** (partial) | decide `virtual_texture` before the `Standard2D` check; re-verify each reason's reachability in the stated precedence | §R6.2 T1–T2 (L826–890); §R6.4 (L921–963) | T1 is now the virtual check and T2 the type check. §R6.4 gains a column naming, for each reason's producer, why no earlier step fires on it. | **RESOLVED** |
| **P2-10** (partial) | every admitted linear format in G-ID, or UNEXERCISED and refused until exercised | §R3.1 (L314–361); §R12.4 | 13 G-ID encoding rows cover every admitted colour, data and normal format through the corruptor that draws it. `PF_BC5` without `IsNormalMap()` cannot be cooked in 5.1, so it is removed from the allowlist (refused, UNEXERCISED). | **RESOLVED** |
| **S1 stops** | list every S1 qualification failure as a stop condition | §R13.1 (L1576–1649) | Fourteen stop rows: every S1 Q gate, including `G-ID-M`, `G-RD`, `G-REASON`, `G-COOK`, the offline lever proofs, the luma gate and the mip tripwire. | **RESOLVED** |

Revision 2's §R0.1 rows for P1-2, P1-3, P1-4, P2-8, P2-9 and P2-10 are superseded by the rows above, and
in its P2-6 row "then regenerate mips" is superseded by N1(a) (every draw still clears first, §R7.3). The
rest of §R0.1 stands. ⚠ The line numbers in §R0.1 below are revision 2's (`b118a66`).

### R0.1 Findings

| finding | resolved in | resolution | status |
|---|---|---|---|
| **P1-1** texture-pointer intersection is not an active binding | §R2 (L104–205) | Bindings are read from the uniform-expression set of the game-thread shader map of the resource the slot renders with (world feature level, active quality level, static permutation). A binding is a parameter iff its `ParameterInfo.Name` is not None; a constant use is refused `texture_not_parameter`. The full `FMaterialParameterInfo` (name, association, index) is kept and set with `SetTextureParameterValueByInfo`. `GetUsedTextures` and the pointer intersection are gone. Fixtures: the inactive-parameter / active-constant alias and a material-layer parameter (`G-BIND`). | **RESOLVED** in design; feasibility is `G-BIND`'s first check |
| **P1-2** an identity copy of mip 0 is not an identity texture resource | §R3 (L209–355) | An allowlist of admitted pixel formats replaces the compression denylist. Float, 16-bit, BC6H and LQ formats are refused `unsupported_encoding`. The render target takes the full cooked top-mip dimensions, keeps both dimensions and the aspect, and is never capped or downsampled; if it does not fit, the event is refused `over_budget`. The sampler is matched (LOD group, filter, address), and a host `r.MipMapLODBias` that the render target cannot follow is refused. A qualification matrix covers magnification, minification, grazing angles and mip transitions (`G-ID-M`). **One part cannot be done as ruled:** authored, sharpened or alpha-coverage mip chains are **undetectable** in a cooked build (the settings are editor-only data) and, within Core/CoreUObject/Engine, a render target can only **regenerate** its mips. So "refuse chains that cannot be preserved" cannot be implemented as a refusal. | **PARTIAL → counter-proposal N1** |
| **P1-3** force-residency contradicts isolation; "full" undefined | §R4 (L359–433) | The source must already be resident at its full cooked chain (`NumResidentLODs == MaxNumLODs`, with the resource valid and nothing pending in init or streaming); otherwise `not_fully_resident`. Wait is 0 frames / 0 ms. No force-resident call, no `WaitForStreaming`. `snapshot_mip` and the snapshot dimensions are recorded against the cooked chain. A prefetch phase is a separate, later decision, and only if yield demands it. | **RESOLVED** |
| **P1-4** a host-MID clone loses state; animation exception breaks purity | §R5 (L437–456) | A slot whose raw binding is a MID is refused `host_mid`. m53 builds **no clone path** and adds no clone switch. `host_mid_cloned` is reserved and never emitted. | **RESOLVED** |
| **P1-5** a route-B failure does not justify route A | §R11 (L872–886) | Route A is removed from the design, S1 and `G-ID`. A failure narrows the scope (defer `normal_corruption`) or holds, and any narrowing goes to chat with its evidence. A takeover would be a separate product decision. | **RESOLVED** |
| **P2-6** AlphaComposite must clear before every redraw | §R7.3 (L613–636) | Clear to (0,0,0,1), then draw, then regenerate mips, on **every** draw, including every `drift` frame. The snapshot and the output are distinct render targets. `G-RD` is a repeated identity-redraw gate with opaque and fractional-alpha inputs, and it has a can-fail lever that skips the clear. | **RESOLVED** |
| **P2-7** exact restore needs raw slot state and strong references | §R8 (L693–762) | Per slot, before any change: the raw override, the asset material, the resolved material and the effective (Nanite-substituted) material. Nanite routing is inspected from the resolved material. The displaced originals are held strongly until the restore completes. A slot is restored only while it still holds this event's MID. `G4` is extended with GC, an explicit Nanite override, foreign replacement, component recreation and two live ids. | **RESOLVED** |
| **P2-8** "every map coherently" conflicts with partial eligibility and the cap | §R6.3 (L534–552) | Atomic eligibility per slot: the family's whole required map set is transformable, or the slot is refused with the blocking binding's reason. A cap overflow refuses (`map_set_over_cap`) instead of keeping the 8 largest. The 64 px floor is renamed `below_size_policy` and is a policy, not a claim that small means constant. Small textures inside a qualifying set are transformed. Exclusions are counted per reason. | **RESOLVED** |
| **P2-9** refusal vocabulary: wrong size, not a decision tree | §R6 (L460–585) | One ordered decision tree (event → slot → binding → slot aggregation → event aggregation). Every binding and slot gets one counted disposition, and the event gets exactly one final reason. It covers asset and resource readiness, unsupported encoding, host-MID policy and residency. The shader-map checks come before an empty texture list is read as "no textures". §R6.4 names a fixture or lever for every reachable reason and marks the rest UNEXERCISED. | **RESOLVED** (most fixtures need **N2**) |
| **P2-10** gates can go green without qualifying behaviour | §R12 (L890–967) | Every row is marked **Q** (qualification), **D** (diagnostic) or **O** (owner decision). Each fixture has runnable matched controls: a NoApply null, a `corrupted_texture` positive (no fixture gate), and wrong-copy controls that must fail. `G-ID` runs through both corruptors and every admitted encoding. Functionality gates carry minimum counted applications. `G-LYRA` needs one successful counted event per id to claim Lyra support. `G-COST` has a predeclared comparison with m55 held constant, and its acceptance is an owner decision. | **RESOLVED** (fixture **N2**) |
| **P2-11** shader prewarm is not packaged PSO warmup; a void draw is not validation | §R7.4, §R9 (L638–670, L766–824) | The prewarm-only list is separated from the per-frame check. The Apply transaction is defined: decide → reserve → allocate and verify → re-check every draw precondition → enqueue → bind and read back → commit, with rollback. A non-capturing warm-draw phase runs before the lead-in. The cold first fire is **measured** for both corruptors and every format, and its acceptance is an owner decision. | **RESOLVED**; cold-first-fire acceptance is an owner decision |
| **P3-12** `strength_class` can only be a mode prior | §R10 (L828–868) | Renamed `texcorrupt.expected_strength_class` and documented as a mode prior, not a per-event measurement. v1's universal statements are withdrawn. m55 stays out of eligibility, ranking, seeds, verdicts and `observable`. | **RESOLVED** |

**Chat's revised rulings.** D1 → §R1, §R4, §R11 · D2 → §R7.5 · D3 → §R8.4 · D4 → §R12, §R14 ·
D5 → §R12 (`G-RD`, `G-COST`) · D6 → §R9.1–§R9.2 · D7 → §R5.

### R0.2 Revision-1 text that stays citable (source facts, not design)

These v1 sections record what the engine and the content do. They are unaffected by the review and may be
cited as `v1 §x`:

- v1 §1.1–§1.3 — why a takeover reconstructs rather than corrupts;
- v1 §2.2 — which texture fields a cooked build can read;
- v1 §3.1.1 — TexCoord[1] per vertex factory, and why `GetNumUVChannels` returns 0 when cooked;
- v1 §3.2's **draw facts only**: the render-target draw path, Surface domain not UI, Unlit, opaque writes
  A = 0, and the sRGB / normal encoding table rows;
- v1 §3.3's cook and office-host facts (hard references, editor binaries for the cook, runtime-load proof);
- v1 §3.4's **streaming facts only** (not its snapshot rule);
- v1 §4.4 (the target watch) and v1 §4.5's facts about m52 sharing;
- v1 §7.1–§7.4 (two ids, the mode draw, the `G150` precision, no dashboard change);
- v1 Appendix A (the name-table scan) and Appendix B (the citation index).

Everything else inside the fold is withdrawn.

---

## R1. The rulings as applied

- **D1 (revised): route B only, provisional.**
  - The host material is kept. Corrupted copies of its **active texture parameters** are drawn into
    render targets and bound with `…ByInfo` on a MID of the host's own material.
  - Residency is **already resident, or refuse**, with 0 frames / 0 ms of wait (§R4).
  - There is **no automatic route-A fallback** (§R11). Route A survives only inside the SUPERSEDED fold.
- **D2 (amended):** `uv_scramble` is a seeded **bijective** permutation of K×K cells (§R7.5). `channel1`
  is removed along with route A.
- **D3:** two ids, `uv_corruption` and `normal_corruption`, with state, cleanup and counters isolated per
  registration (§R8.4).
- **D4:** both ids are in `GAutoPool`, default OFF. An all-refused census is a yield result, never a
  support claim (§R12, §R14).
- **D5 (provisional):** defaults are UV `tile scramble drift` and normal `invert green_flip noise`.
  `drift` stays conditional on `G-RD` (repeated-draw fidelity) and `G-COST`.
- **D6 (amended):** the prewarm-only list is separated from the per-frame readiness check (§R9.1).
  Shader completeness is not packaged PSO readiness (§R9.2).
- **D7 (reversed):** slots holding host MIDs are refused, and in m53 unconditionally (§R5).
  `host_mid_cloned` is reserved for a future state-following milestone.

### R1.1 🔁 082-04 — N1–N4 as ruled

- **N1 = (a).** `AnomalyInjector` gains **private** dependencies on `RenderCore` and `RHI`. Every output mip
  is drawn from the source's own mip, and none is regenerated (§R3.4). Engine modules only; no host-game
  types (§R2.5).
- **N2 = approved, additive only.** The synthetic fixture level and its checks are in §R12.1.
- **N3 = 128 MiB** (§R3.2). `G7` is read at 64, 128 and 256 MiB, and `G-COLL` is the collateral-residency
  diagnostic (§R12.3).
- **N4 = 2 levels per 8-bit channel**, with wrong copies ≥ 16 (§R12.3, §R12.4).
- Codex's Δ1–Δ3 and the six PARTIALs are applied in the sections §R0.0 names.
- D1–D7 stand as revision 2 applied them, with two changes: D1's residency now admits only `snapshot_mip` 0
  (§R4), and D7's refusal covers the whole resolved chain (§R5).

---

## R2. Active texture bindings (P1-1)

### R2.1 Where the binding list comes from

For each slot, the **resolved** material `M` is used: the raw override if it is non-null, otherwise the
mesh asset's slot material (§R8.1). The Nanite-substituted effective material is never used; a slot where
it differs has already been refused (§R6, step S4).

1. **The resource the slot renders with:** `Res = M->GetMaterialResource(World->FeatureLevel,
   EMaterialQualityLevel::Num)`.
   - `Num` resolves to the **active** quality level, `GetCachedScalabilityCVars().MaterialQualityLevel`
     (`Material.cpp:2614-2630`; `UnrealEngine.h:461`).
   - An MIC with a static permutation returns its own `StaticPermutationMaterialResources`; otherwise
     the call forwards to the parent (`MaterialInstance.cpp:1684-1697`). This is the same choice the
     render thread makes (`MaterialInstance.cpp:223-248`).
   - A runtime MID never has its own static permutation in a cooked build
     (`MaterialInstance.cpp:666`; set only under editor guards at `:1971-1974`, `:3551-3669`), so it
     forwards to its parent.
   - This uses the **world's** feature level. v1's `GetUsedTextures(…, Num)` path used
     `GMaxRHIFeatureLevel` instead (`Material.cpp:1080-1093`).
2. **Its game-thread shader map:** `SM = Res->GetGameThreadShaderMap()` (`MaterialShared.h:2097-2101`).
   - `Res == nullptr` or `SM == nullptr` ⇒ refuse `shader_map_unavailable`.
   - `!Res->IsGameThreadShaderMapComplete()` (`MaterialShared.h:2109-2113`) ⇒ refuse
     `shader_map_incomplete`. In a cooked build such a material draws through the render-thread
     fallback chain towards the default material (`MaterialShared.cpp:4207-4229`), so any statement
     about "its textures" would describe a material that is not on screen.
3. **The binding list:** `Set = SM->GetUniformExpressionSet()` (`MaterialShared.h:1399`). For each
   `EMaterialTextureParameterType` (`MaterialShared.h:468-478`) and each `i < Set.GetNumTextures(Type)`,
   the entry is `Set.GetTextureParameter(Type, i)` (`:603`, `:605`).
   - This is `LAYOUT_FIELD` data of the cooked shader map (`MaterialShared.h:647`, `:732`, `:1130`).
     Only debug strings and sample-count estimates are editor-only (`:738-751`, `:1134-1137`).
   - **Static-switch-off branches are absent**, because the unselected input is never compiled
     (`MaterialExpressions.cpp:9324-9332`; `StaticSwitchParameter` `:9077-9109`; `QualitySwitch`
     `:9507-9533`).
   - v1 claimed this property for `GetUsedTextures`, which reads the same data. The defect was
     elsewhere: v1 kept only the texture **pointers** and then intersected them with the parameter list,
     which threw away which entry was a parameter.

### R2.2 Parameter or constant

Each entry is an `FMaterialTextureParameterInfo` (`MaterialShared.h:481-502`): `ParameterInfo` (name,
association, index), `TextureIndex`, `SamplerSource`, `VirtualTextureLayerIndex`.

- **A constant texture sample never sets `ParameterInfo`.** Its name stays None
  (`MaterialUniformExpressions.cpp:1399-1404`; the entry is default-constructed at
  `HLSLMaterialTranslator.cpp:1345`; the default is `MaterialTypes.h:96`). The runtime itself reads None
  as "use the referenced texture" (`MaterialUniformExpressions.cpp:1479`).
- **A texture parameter carries its name, its association (`GlobalParameter`, `LayerParameter`,
  `BlendParameter`) and its layer or blend index** (`HLSLMaterialTranslator.cpp:6781-6807`;
  `HLSLMaterialTranslator.h:689-693`).
- **The binding's current texture** is read with the engine's own game-thread resolver,
  `Set.GetGameThreadTextureValue(Type, i, M, *Res, OutTex)` (`MaterialShared.h:606`;
  `MaterialUniformExpressions.cpp:618-634`, `:1477-1483`). It returns what the renderer will sample:
  the instance chain's override for a parameter, the referenced texture for a constant.
- **Everything is keyed by the entry, never by the texture pointer.** One texture object can appear
  under a constant entry and a parameter entry in the same set.
- **Codex's alias case therefore resolves by construction.** An inactive parameter P defaulting to T,
  next to an active constant use of T, produces **no entry for P at all** (P's branch is not compiled).
  T is seen only as a constant and is refused `texture_not_parameter`.
- ⚠ **Named residual, not solved.** An entry means "a texture chunk was accessed during translation"
  (`HLSLMaterialTranslator.cpp:3333-3374`, `:1340-1347`), not "the texel reaches an output". A compiled
  read whose result is multiplied by zero, for example, is still an entry. The design cannot see that
  case. `G-BIND`'s dead-binding fixture reports what it does, and on host content such an event reads as
  no measured change in m55 (a `G-STR` reading, never a pass).

### R2.3 The override

- `FMaterialParameterInfo Info(Entry.ParameterInfo)` (explicit constructor, `MaterialTypes.h:56`,
  `:127-132`). Names are real `FName`s in a cooked build: frozen script names are patched from
  serialised names at load (`Core/Private/Serialization/MemoryImage.cpp:1597-1610`).
- `HostMid->SetTextureParameterValueByInfo(Info, Rt)` (`MaterialInstanceDynamic.h:67`, `.cpp:237-249`).
  A layer parameter is addressed by (name, association, index), so it is set on its own layer and never
  confused with a global of the same name.
- **Read-back:** `HostMid->GetTextureParameterValue(Info, Out)` (`MaterialInterface.h:802`) must return
  `Rt`, else the transaction rolls back (`param_readback_mismatch`, §R7.4).
  - ⚠ **Necessary, not sufficient.** It proves the MID stores the override, not that the shader samples
    it. That proof comes from `G-BIND`, `G-ID`, `G1` and `G2`.

### R2.4 Which entries are candidates

🔁 082-04 (P2-9): the virtual check comes **first**, so a virtual entry is named `virtual_texture` and never
falls through to the type check.

- `Virtual` entries, and entries whose texture is a `UTexture2D` that `IsCurrentlyVirtualTextured()`
  (`Texture2D.cpp:1210-1224`) ⇒ `virtual_texture`.
- Then only `Standard2D` entries whose current texture is a `UTexture2D` go on. `Cube`, `Array2D`,
  `ArrayCube`, `Volume`, and a `Standard2D` entry holding another texture class (a render-target asset, for
  example) ⇒ `unsupported_type`.
- **Runtime virtual textures are not in this set** (they have their own parameter array) and are never
  touched. With no clone path (§R5), the host's RVT overrides stay on the host's material by construction.

### R2.5 Module boundary

**The binding read** (§R2.1–§R2.3) uses Engine only. Every symbol is `ENGINE_API`, inline in an Engine
header, or a virtual call through an Engine class:

- `GetMaterialResource` is virtual; `GetGameThreadShaderMap` and `IsGameThreadShaderMapComplete` are
  inline; `FUniformExpressionSet::GetTextureParameter` and `GetNumTextures` are inline;
  `GetGameThreadTextureValue` is `ENGINE_API`.
- `FMaterialShaderMap::GetUniformExpressionSet` is inline and calls `GetContent()`, which is inline in
  `RenderCore/Public/Shader.h:2243`. That is a **header-only** use of a RenderCore type through Engine's
  public include path; it links no RenderCore symbol.

🔁 082-04 (N1(a)): **the per-mip copy (§R3.4) is what adds dependencies.** `AnomalyInjector` declares
`RenderCore` and `RHI` as **private** dependencies, in every configuration, because the anomaly runs in
every configuration. (`AnomalyCapture` declares the same two only outside Shipping,
`AnomalyCapture.Build.cs:340-353`.) What it links, all exported or header-only:

- `ENQUEUE_RENDER_COMMAND`, a header template (`RenderCore/Public/RenderingThread.h:256-291`). Its one linked
  symbol, `GetImmediateCommandList_ForRenderCommand`, is `RENDERCORE_API` (`:164`).
- `FRDGBuilder` and its `RegisterExternalTexture`; `CreateRenderTarget` (`RENDERCORE_API`,
  `RenderCore/Public/RendererInterface.h:600`); `AddCopyTexturePass` (`RENDERCORE_API`,
  `RenderCore/Public/RenderGraphUtils.h:674-678`).
- `FRHICopyTextureInfo` (`RHI/Public/RHI.h:1966-1993`), and `FRHITexture::GetNumMips` / `GetFormat` (inline,
  `RHI/Public/RHIResources.h:1903`, `:1906`).
- The render-target side is Engine and exported:
  - `UTextureRenderTarget2D::InitCustomFormat` and `UpdateResourceImmediate` (`TextureRenderTarget2D.h:150`,
    `:197`);
  - `GameThread_GetRenderTargetResource` (`TextureRenderTarget.h:43`);
  - `FRenderTarget::GetRenderTargetTexture` (`Public/UnrealClient.h:49`);
  - the four `UKismetRenderingLibrary` functions used, `ClearRenderTarget2D`, `DrawMaterialToRenderTarget`,
    `BeginDrawCanvasToRenderTarget` and `EndDrawCanvasToRenderTarget` (`KismetRenderingLibrary.h:43`, `:83`,
    `:183`, `:189`).
  - `FTextureRenderTarget2DResource` is **not** `ENGINE_API` (`Public/TextureResource.h:394`), so nothing
    here calls its non-virtual members.
- **The invariant holds: engine modules only, never a host-game type.**
  - `architecture.md`'s wording becomes "engine modules only; no host-game types".
  - `CLAUDE.md`'s invariant records `RenderCore` and `RHI` as a dated ruling (082-04), the way it records
    `Foliage`.
  - Both edits land in S1, with the dependency.

⚠ `G-BIND`'s first check and `G10` read the **S1 declared set** and nothing else:

- public `Core`, `CoreUObject`, `Engine`, `InputCore`;
- private `Foliage`, `RenderCore`, `RHI`.

A link failure, or any other module appearing, stops S1 (§R13.1).

---

## R3. The texture contract: format, dimensions, sampling, mips, memory (P1-2, Q6)

### R3.1 Admitted encodings

`GetPixelFormat()` is the only cooked fact about the encoding (`Texture2D.cpp:376-389`). The choice from
`CompressionSettings` to a pixel format is made at cook time (`Texture.cpp:2995-3342`, entirely inside
`#if WITH_EDITOR` `:3005-3339`). So the rule is an **allowlist on the pixel format**, default deny.

🔁 082-04 (P2-10): the table now also says how each row is cooked on Windows (`Texture.cpp`, the default
format selection; Windows passes its flags at
`Developer/Windows/WindowsTargetPlatform/Private/GenericWindowsTargetPlatform.h:336`), so
that every row has a fixture (§R12.4).

| source pixel format | how it is cooked on Windows | class | render-target format |
|---|---|---|---|
| `PF_DXT1`, `PF_DXT5`, `SRGB` true | `TC_Default` with sRGB on; DXT1 vs DXT5 by the source's alpha (`:3281-3297`; `Developer/TextureCompressor/Private/TextureCompressorModule.cpp:3438`; `[Plugins] Developer/TextureFormatOodle/Source/Private/TextureFormatOodle.cpp:638`, where `[Plugins]` = `Engine/Plugins`) | colour | `RTF_RGBA8_SRGB` |
| `PF_BC7`, `SRGB` true | `TC_BC7` (`:3269`) | colour | `RTF_RGBA8_SRGB` |
| `PF_B8G8R8A8`, `SRGB` true | `TC_VectorDisplacementmap` with sRGB on (`:3239`); an sRGB grayscale texture, which Windows remaps from G8 (`:3305-3307`; `Core/Public/Windows/WindowsPlatformProperties.h:101-104`); any sRGB texture forced uncompressed (`:3044-3051`, `:3108-3127`, `:3209`) | colour | `RTF_RGBA8_SRGB` |
| `PF_DXT1`, `PF_DXT5`, `SRGB` false | `TC_Masks`, whose sRGB is forced off (`:489-494`); or `TC_Default` with sRGB off | data | `RTF_RGBA8` |
| `PF_BC7`, `SRGB` false | `TC_BC7` with sRGB off | data | `RTF_RGBA8` |
| `PF_B8G8R8A8`, `SRGB` false | `TC_VectorDisplacementmap` with sRGB off; any linear texture forced uncompressed | data | `RTF_RGBA8` |
| `PF_BC4` | `TC_Alpha` (`:3257`) | data (one channel) | `RTF_RGBA8` |
| `PF_G8` | `TC_Grayscale` / `TC_Displacementmap` with sRGB off and an 8-bit source (`:3241-3253`); `TC_DistanceFieldFont` (`:3261`) | data (one channel) | `RTF_RGBA8` |
| `PF_BC5` with `IsNormalMap()` | `TC_Normalmap` (`:3235`); `IsNormalMap()` is `CompressionSettings == TC_Normalmap` (`Classes/Engine/Texture.h:1721-1723`) | normal (`.rg` decode) | `RTF_RGBA8` |

- 🔁 082-04 (P2-10): **`PF_BC5` without `IsNormalMap()` is removed from the allowlist.**
  - In 5.1 nothing but `TC_Normalmap` produces BC5 (`Texture.cpp:3235` is the only assignment), so that row
    had no cookable fixture.
  - It is refused `unsupported_encoding` and listed **UNEXERCISED** in the gate report. It returns only with
    a fixture.
- 🔁 082-04: **every remaining row is a `G-ID` row** (§R12.4), including all six linear data rows.
- **Refused `unsupported_encoding`, naming the format:**
  - `PF_FloatRGBA`, `PF_R16F`, `PF_R32_FLOAT`, `PF_A32B32G32R32F`, `PF_BC6H`. These come from
    `TC_HDR`, `TC_HDR_F32`, `TC_HalfFloat`, `TC_SingleFloat` and `TC_HDR_Compressed` (`Texture.cpp:3227`,
    `:3231`, `:3265`, `:3273`, `:3277`). Their range and precision do not survive RGBA8.
  - `PF_G16` (16-bit grayscale, `:3241-3254`): precision is lost.
  - `PF_R5G6B5_UNORM`, `PF_B5G5R5A1_UNORM` (`TC_LQ`, `:3213-3224`): not qualified by any fixture.
  - A normal map in any format other than `PF_BC5` (for example `DXT5n`, or a normal map forced
    uncompressed): not qualified by any fixture.
  - `PF_BC5` without `IsNormalMap()` (above).
  - Anything not in the table.
- **Single-channel formats (`PF_BC4`, `PF_G8`)** are read by the host through a grayscale or alpha sampler
  type, which replicates one channel of the lookup. The render target stores the corruptor's raw sample in
  RGBA8, so whether the host's replication reads the same value from it is not derived here; G-ID's BC4 and
  G8 rows prove it or refuse the class.
- **The contract in one line:** every admitted source decodes to values the render-target format holds
  without range loss. BC interpolants can decode to values between 8-bit steps; the render target stores
  the nearest 8-bit value. That single re-quantisation is what `G-ID`'s tolerance bounds (N4, §R12.3).
- `Compat.UseDXT5NormalMaps` non-zero refuses the normal family `dxt5_normal_host`. This is v1's rule,
  kept; the variable is read at runtime (v1 §3.2 encoding table).

### R3.2 Dimensions and memory

- **Size.** The output render target is **W×H = the dimensions of the platform's top mip**,
  `PlatformData->Mips[0]`, which admission makes the cooked mip 0 (§R4). On a normally cooked texture that
  equals `GetSizeX()` × `GetSizeY()`, the full cooked mip-0 size after cook-time stripping
  (`Texture2D.cpp:327-355`; `TextureDerivedData.cpp:2661-2667`).
  - Both dimensions and the aspect are kept. **There is no cap and no resize.**
- 🔁 082-04 **Mips.** A `UTextureRenderTarget2D` has more than one mip only when `bAutoGenerateMips` is set,
  and then exactly `FloorLog2(max(W, H)) + 1` (`TextureRenderTarget2D.cpp:50-62`; `NumMips` is private and
  has no setter, `TextureRenderTarget2D.h:219-222`, `:249-250`).
  - So the output takes `bAutoGenerateMips = (source mip count > 1)`.
  - A source chain that is neither one mip nor exactly that length is refused `mip_chain_shape` (§R3.4.5).
  - The flag only sizes the chain; §R3.4.4 makes sure the engine never generates mips over content.
- **Bytes per chain:** `B(W,H) = Σ_i 4 · max(1, W≫i) · max(1, H≫i)` over those levels, computed by our own
  arithmetic. The engine's `CalcTextureMemorySizeEnum` counts only the base level
  (`TextureRenderTarget2D.cpp:68-79`).
- 🔁 082-04 **Scratch (N1(a)).**
  - Mips ≥ 1 pass through 1-mip scratch targets, one per mip size (§R3.4.3).
  - A scratch set is shared by every texture of the event with the same (W, H, sRGB) class, because their
    commands run in order on one queue.
  - Scratch bytes for one class: `T(W,H) = B(W,H) − 4·W·H`. That is every level except mip 0, which is drawn
    straight into the output.

  | size | chain B | scratch T | B + T |
  |---|---|---|---|
  | 256² | 0.33 MiB | 0.08 MiB | 0.42 MiB |
  | 1024² | 5.33 MiB | 1.33 MiB | 6.67 MiB |
  | 2048² | 21.33 MiB | 5.33 MiB | 26.67 MiB |
  | 4096² | 85.33 MiB | 21.33 MiB | 106.67 MiB |

- 🔁 082-04 **An event's requirement** is `ΣB` over its output chains plus `ΣT` over its distinct scratch
  classes. `drift` doubles the chains (snapshot and output, §R7.3) and keeps its scratch for the event:
  `2·ΣB + ΣT`.
- 🔁 082-05 (P3) **Worked example.** Two 4096² maps of the **same** class share one scratch set:
  `2 · 85.33 + 21.33 = 192.0 MiB`. Two 4096² maps of **different** classes (an sRGB colour map and a linear
  normal map, for example) need two: `2 · 106.67 = 213.33 MiB`.
- 🔁 082-04 **The budget (N3).** A run-wide cap on live m53 render-target bytes, both ids together:
  `IAI.Anomaly.TexCorruptMaxRtBytes`, compiled **128 MiB (134,217,728 bytes)**.
  - It follows the `AnomalyDefaults` pattern: console > ini `[AnomalyInjector] TexCorruptMaxRtBytesDefault` >
    compiled, with a `G139` echo naming the source. A value outside [1 MiB, 1 GiB] is refused, never
    clamped (the upper bound keeps it inside a 32-bit console variable).
  - At Apply the event **reserves** its whole requirement (outputs, `drift` snapshots and scratch) before
    allocating anything. If it does not fit, the event is refused `over_budget` (§R6, step V2). **Never
    downsampled.**
  - Released bytes stay counted as `pending_release` until two frames after the release is enqueued. That
    is a bookkeeping rule. GPU deallocation timing is not observable from the game thread.
  - A static mode's scratch is released at the end of Apply and moves to `pending_release`. `drift` keeps
    its scratch for the event.
  - `texcorrupt_rt_bytes_peak` reports live plus pending bytes.
  - ⚠ **128 MiB caps our own allocations.** It is not a VRAM guarantee and says nothing about the host's
    streaming pool. Whether it is too high for a host is what `G-COLL` reads (§R12.3). The owner may choose
    another default from `G7`'s three readings (O2).
- ⛔ Revision 2's "64 MiB refuses every 4096² source" note is withdrawn: at 128 MiB a single 4096² map with
  its scratch needs 106.67 MiB and fits (§R14.3).

### R3.3 Sampling

- **How each sampler is built:**
  - The render target's sampler comes from **its own** `LODGroup` and `Filter` through the active device
    profile, with MipBias 0 and anisotropy from `r.MaxAnisotropy` (`TextureRenderTarget2D.cpp:647-655`).
  - An ordinary texture's sampler uses the same device-profile filter lookup
    (`Private/Rendering/StreamableTextureResource.cpp:101`; `TextureLODSettings.cpp:358-382`), the same
    `r.MaxAnisotropy` (`RHI/Public/RHIUtilities.h:844-850`), its own `AddressX/Y`
    (`Private/Rendering/Texture2DResource.cpp:81-82`), and **MipBias = `r.MipMapLODBias`**
    (`Private/Rendering/Texture2DResource.cpp:74-83`; `Texture2D.cpp:88-92`, `:1189-1193`).
- **Matched by construction.** Before the resource is created (`InitCustomFormat`,
  `TextureRenderTarget2D.h:150`), the render target gets:
  - `LODGroup = Src->LODGroup` and `Filter = Src->Filter`, so filter and anisotropy match;
  - `AddressX/Y = Src->AddressX/Y` and `MipsAddressU/V = Src->AddressX/Y`.
  - The sampler is rebuilt only when the resource is initialised (`TextureRenderTarget2D.cpp:647-655`),
    which is why these are set first.
- **MipBias cannot be matched.** The render-target sampler has none, and the engine's sampler refresh
  skips render targets (`UnrealEngine.cpp:1016-1057`). ⇒ At Apply, `UTexture2D::GetGlobalMipMapLODBias()`
  (`Texture2D.h:233`) non-zero refuses the event 🔁 **`runtime_lod_bias`, sub-reason `global_sampler`**
  (revision 2 named it `host_mip_bias`; §R4). The default is 0 and `r.MipMapLODBias` appears nowhere in
  `Engine/Config`; no plugin or bench script sets it.
- **Shared samplers.** A material sampling through `SSM_Wrap_WorldGroupSettings` / `Clamp` ignores the
  texture's sampler for the render target and the source alike (`SceneManagement.cpp:901-913`;
  `MaterialUniformExpressions.cpp:963-975`). The view's shared sampler carries the global and view mip
  bias for both (`Renderer/Private/SceneRendering.cpp:1445-1454`). Equal treatment, nothing to match.
- **sRGB.** Set from the source class through `InitCustomFormat`'s `bForceLinearGamma` (an override format's
  sRGB is `!bForceLinearGamma`, `TextureRenderTarget2D.h:237-247`), before creation.
- 🔁 082-04 **The corruptor's own reads of its source** (§R3.4.2):
  - They use an explicit mip with `AutomaticViewMipBias` off, through the source's own sampler
    (`SSM_FromTextureAsset`, bound at `MaterialUniformExpressions.cpp:965`).
  - That sampler's MipBias is `r.MipMapLODBias` (above), which E4 requires to be 0.
  - `TMVM_MipLevel` disables anisotropic filtering (`Classes/Engine/EngineTypes.h:311-318`).
  - At an integer LOD with pixel centres on texel centres, every filter returns the texel.

### R3.4 🔁 082-04 — Mips: the per-mip copy (N1(a))

**Why.** Revision 2 regenerated the render target's lower mips after every draw. That cannot reproduce an
authored, sharpened or alpha-coverage chain, and the settings that make such chains are editor-only, so a
cooked build cannot recognise them (revision 2's finding, kept: `Classes/Engine/Texture.h:1117-1275`;
Appendix C counts 8 of 131 StackOBot and 22 of 324 Lyra textures). N1(a) removes the question: **every mip
of the output is made from the same mip of the source**, so whatever chain the cook produced is carried
through, and nothing needs detecting.

#### R3.4.1 The chain the output must hold

- The source `Src` is a `UTexture2D` with cooked chain `Src->GetPlatformData()->Mips[0 … M−1]`, and
  W×H = `Mips[0]`. Admission guarantees `snapshot_mip` 0 (§R4).
- The output `Out` is a `UTextureRenderTarget2D`:
  - W×H;
  - format `PF_B8G8R8A8` (`RTF_RGBA8` and `RTF_RGBA8_SRGB` both map to it, v1 §3.2);
  - its sRGB flag from the source class (§R3.1);
  - `bAutoGenerateMips = (M > 1)`.
- **Admitted chain shapes:**
  - `M == 1`; or
  - `M == FloorLog2(max(W, H)) + 1`, with every `Mips[m]` equal to `max(1, W≫m) × max(1, H≫m)` (the render
    target's own level sizes).
  - Anything else is refused `mip_chain_shape` (step T6), because the render target cannot hold it (§R3.2).

#### R3.4.2 Reading source mip m (render thread, cooked build)

- **The sample.** The corruptors sample their source parameter with a `TextureSample` set as follows:
  - `MipValueMode = TMVM_MipLevel`, with the scalar parameter `SrcMip` as the MipValue input
    (`Classes/Engine/EngineTypes.h:311-318`; the input compiles any float expression,
    `MaterialExpressions.cpp:2813-2815`);
  - `SamplerSource = SSM_FromTextureAsset`.
  - The translator then emits `Texture2DSampleLevel(Tex, TexSampler, UV, SrcMip)`
    (`HLSLMaterialTranslator.cpp:6332-6342`), which is `Tex.SampleLevel(…)` with no bias added in the shader
    (`[Shaders] Private/Common.ush:327-330`).
- **`AutomaticViewMipBias` is false on every source sample.**
  - By default the translator adds `View.MaterialTextureMipBias` to an explicit mip
    (`HLSLMaterialTranslator.cpp:6104`, `:6145-6148`; the default is true, `MaterialExpressions.cpp:2403-2406`).
  - The canvas builds its own view, whose value keeps the default 0 (`Private/SceneView.cpp:2612`); the
    scene renderer sets it only under a temporal upscaler (`Renderer/Private/SceneVisibility.cpp:3761-3768`).
    Turning the flag off removes that dependency; it does not fix an observed error.
- **Which physical mip `SrcMip = m` reads.**
  - On D3D12 a streamed `UTexture2D`'s RHI texture holds only its resident mips
    (`Private/Rendering/Texture2DResource.cpp:103-109`, `:130-136`).
  - Stream-in creates a new texture with the requested count (`Private/Streaming/Texture2DStreamIn.cpp:148-155`,
    `FTexture2DStreamIn_IO_AsyncCreate`). That path is chosen at `Texture2D.cpp:1638-1651` because D3D12
    enables async creation (`D3D12RHI/Private/Windows/WindowsD3D12Device.cpp:1382`). The partially-resident
    path is compiled out (`PLATFORM_SUPPORTS_VIRTUAL_TEXTURES 0`, `Core/Public/HAL/Platform.h:286-287`).
  - RHI mip i is therefore asset mip `AssetLODBias + MaxNumLODs − NumResidentLODs + i`
    (`Public/Streaming/StreamableRenderResourceState.h:69-72`).
  - **Admission requires `NumResidentLODs == MaxNumLODs == M` and `AssetLODBias == 0`** (§R4). So RHI mip i
    **is** cooked mip i, and `SrcMip = m` reads cooked mip m.
- **The draw that reads it.** One draw per level:
  - into a target of exactly `Mips[m]`'s size;
  - with `UCanvas::K2_DrawMaterial(CorruptorMid_m, (0,0), (W_m, H_m), (0,0))` between
    `UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget` and `EndDrawCanvasToRenderTarget`
    (`KismetRenderingLibrary.cpp:702-759`, `:761-812`).
  - Pixel centres `(i + 0.5) / W_m` land on mip-m texel centres, so an identity draw returns each texel
    exactly under bilinear, point, or trilinear at an integer LOD.
- **Facts about that draw:**
  - The renderer draws the material tile on the render thread and flushes pending material-parameter
    updates first (`Renderer/Private/Renderer.cpp:125-153`, `:152-153`). A parameter set before a draw is
    the value that draw uses.
  - The canvas binds mip 0 of its target and no other (`Public/CanvasRender.h:33-34`; `Renderer.cpp:250`;
    `RenderCore/Public/ShaderParameterMacros.h:472`).
  - **`EndDrawCanvasToRenderTarget` does not regenerate mips.** It resolves with `TransitionAndCopyTexture`,
    which only transitions when the resolve target is the render target itself (`RHI/Public/RHIUtilities.h:802-826`).
  - That is why the Begin/End pair is used and `DrawMaterialToRenderTarget` is not: its
    `UpdateResourceImmediate(false)` regenerates mips 1…N from mip 0 on an auto-mip target
    (`KismetRenderingLibrary.cpp:213-214`; `TextureRenderTarget2D.cpp:706-714`).
  - Each level has its own corruptor MID (per texture, per level), so no parameter changes between two
    draws of one Apply.

#### R3.4.3 Writing output mip m (render thread)

- **Mip 0** is drawn straight into `Out`.
- **Mip m ≥ 1** is drawn into the scratch target `S_m`, then copied into `Out`'s mip m.
  - `S_m` has 1 mip, `Mips[m]`'s size, the same format and sRGB flag as `Out`, and `bAutoGenerateMips` false.
  - The copy is one render command:
    1. `FRDGBuilder GraphBuilder(RHICmdList)`;
    2. register both textures with `GraphBuilder.RegisterExternalTexture(CreateRenderTarget(Res->GetRenderTargetTexture(), Name))`;
    3. `AddCopyTexturePass(GraphBuilder, S_m, Out, Info)` with `Info.Size = (W_m, H_m, 1)` and
       `Info.DestMipIndex = m`;
    4. `GraphBuilder.Execute()`.
  - This is the engine's own render-target-update pattern (`TextureRenderTarget2D.cpp:693-718`). The per-mip
    form has engine precedent: `Renderer/Private/ReflectionEnvironmentCapture.cpp:1318-1324` (RDG), and
    immediate-mode `DestMipIndex` copies at `Landscape/Private/LandscapeEditLayers.cpp:1484-1517`.
  - RDG inserts the copy-source and copy-destination transitions. At the end of the graph it returns both
    textures to `SRVMask` (`RenderCore/Public/RenderGraphResources.h:303`;
    `RenderCore/Private/RenderGraphBuilder.cpp:3011`), the same state every canvas draw and clear leaves
    them in.
  - The D3D12 copy of one mip is a `CopyTextureRegion` on that subresource
    (`D3D12RHI/Private/D3D12Texture.cpp:2889-2958`); D3D11 uses `CopySubresourceRegion`
    (`Windows/D3D11RHI/Private/D3D11Texture.cpp:2117-2132`).
- **Formats.**
  - Neither RHI checks formats on a copy (above). RHI validation does, but only with `-RHIValidation` in a
    Development or Debug build (`RHI/Public/RHIValidationUtils.h:21-23`; `Core/Public/Misc/Build.h:456`;
    `WindowsD3D12Device.cpp:939-942`).
  - The RDG helper checks them (`RenderCore/Private/RenderGraphUtils.cpp:257`).
  - Here the source of every copy is a scratch render target and the destination is the output render
    target. Both are `PF_B8G8R8A8` with the same sRGB flag **by construction**, so the copied bytes are
    already in the destination's encoding.
  - **The block-compressed source is never a copy operand.** It is only ever read by the sampler
    (§R3.4.2).
- **Scratch sharing.** Within one Apply every command runs in submission order on the render thread
  (§R7.4). So one scratch set per (W, H, sRGB) class serves every texture of that class: draw into `S_m`,
  copy, then the next texture's draw into `S_m`.

#### R3.4.4 Nothing regenerates the output's mips

- A new render target puts itself on the deferred-update list at init (`TextureRenderTarget2D.cpp:644`, once
  only). The next scene render's `FDeferredUpdateResource::UpdateResources`
  (`Renderer/Private/SceneRendering.cpp:4321-4322`) would then clear mip 0 and, on an auto-mip target,
  **regenerate every mip** over our content (`TextureRenderTarget2D.cpp:682-719`;
  `TextureRenderTarget.cpp:123-162`).
- So Apply calls `UpdateResourceImmediate(true)` on every target it allocates, right after
  `InitCustomFormat` (transaction step 2).
  - That runs the same update **once, on the empty target, before our first draw**, and unlinks the target
    from the list (`TextureRenderTarget2D.cpp:163-175`, `:685`).
  - `BeginDrawCanvasToRenderTarget`'s own flush (`KismetRenderingLibrary.cpp:753-757`) then finds nothing
    to do (`TextureRenderTarget.cpp:151`).
- After that, the output is written only by the mip-0 draw and the copies. Nothing calls
  `UpdateResourceImmediate`, `DrawMaterialToRenderTarget` or `ResizeTarget` on it again.
- **The only mip generation the output ever sees runs on the cleared target at allocation.**
- ⚠ **Named limit.** An RHI device reset re-runs `InitDynamicRHI`, which re-creates the texture empty and
  re-adds it to the list (`TextureRenderTarget2D.cpp:611-645`). A reset during an event would blank the
  corruption. That is not handled beyond `condition_held`, which reads the binding (§R10), and it is not
  known to occur on the bench.

#### R3.4.5 Refusals, and sources with no lower mips

| case | step | reason |
|---|---|---|
| a chain the render target cannot hold (neither 1 mip nor the full chain, or a level of another size) | T6 | `mip_chain_shape` |
| any runtime LOD bias, which would make `snapshot_mip` non-zero | E4, E5, T9 | `runtime_lod_bias` (§R4) |
| a mip not resident, or `MaxNumLODs` below the cooked count | T10 | `not_fully_resident` |
| a format the RGBA8 write cannot hold, or one with no fixture | T5 | `unsupported_encoding` (§R3.1) |
| a draw precondition of any level's target failing | V3 | `draw_precondition_failed`, rolled back |

- **Sources with no lower mips** (`M == 1`, for example `NoMipmaps`): the output has one mip, and there is
  no scratch and no copy. The host clamps a single-mip source to that mip, and the render target does the
  same.
- **Render-thread tripwire.**
  - Admission derives "RHI mip i is cooked mip i" from game-thread state (§R3.4.2).
  - The first and last render commands of the Apply read the source's `TextureRHI->GetNumMips()` and extent
    on the render thread. They increment `texcorrupt_rt_mip_mismatch` if these differ from M and W×H.
  - Expected 0. Any non-zero fails `G-ID` and stops S1 (§R13.1).
  - It cannot refuse the event, because the draws are already queued. It exists so that a wrong premise is
    loud.

#### R3.4.6 The source mip each mode reads

Level m samples `SrcMip = min(m + k, M − 1)`, with k = log2 N for `tile` ×N (N a power of two) and k = 0
for every other mode.

- For `tile`, output mip m holds source mip m + log2 N repeated N×N times, texel for texel. That is exactly
  what a real ×N UV bug fetches at the footprint where the host samples output mip m.
- Under magnification (the host sampling output mip 0 at a LOD below 0), a real bug would reach a finer
  source mip than the output holds. This is recorded per event as `texcorrupt.tile_detail_mips = log2 N`
  (revision 2's limit, now confined to magnification).
- `swap` on a non-square source changes the footprint per axis. k stays 0, and the difference is a
  `G-ID-M` D reading.

#### R3.4.7 How it is qualified (§R12)

- `G-ID` (every encoding) and `G-ID-M` (magnification, minification, grazing, mip transitions) compare the
  identity output with the host's own sampling of the source.
- **The authored-chain fixture** is a DDS imported with its own mip chain, each level a different solid
  colour. The importer keeps a DDS chain as `TMGS_LeaveExistingMips`
  (`Editor/UnrealEd/Private/Factories/EditorFactories.cpp:3499-3503`). Any mip-selection or chain error
  shows up as a colour.
- **Wrong copies that must fail there:**
  - `mipgen`: skip the copies and let the engine regenerate mips 1…N from mip 0, which was revision 2's
    behaviour;
  - `mipshift`: level m reads source mip m + 1.
  - If identity passes and `mipgen` does not fail, the copies are not what defines the chain, and the
    instrument is invalid.
- Engine-authored chains (`Sharpen`, `Blur` and alpha-coverage fixtures) are `G-ID-M` identity rows too.
- The tripwire reads 0 on every leg.

### R3.5 What qualifies the contract

`G-ID-M` (§R12) runs the identity comparison on dedicated geometry in the synthetic fixture (N2):

- a target filling the view (magnification);
- a far target (minification);
- a grazing-angle plane (anisotropic);
- geometry that crosses mip transitions.

🔁 082-04: each geometry carries the chain fixtures of §R12.1:

- (a) the authored per-mip-colour chain;
- (b) a box-chain one-texel checker;
- (c) the engine-authored chains;
- (d) a BC5 normal.

It does **not** use `r.MipMapLODBias` to force minification, because that would refuse the event (§R3.3).
On host fixtures, `G-ID-M` reads only what the bench pose shows.

---

## R4. Residency (P1-3)

**Policy.** At Apply, every texture in the event's required set must already be resident at its full
cooked chain. Otherwise the event is refused `not_fully_resident`. 🔁 082-04 (P1-3): **and only
`snapshot_mip == 0` is admitted**. Any runtime LOD bias on the texture refuses it first, as
`runtime_lod_bias` (below).

- **Wait: 0 frames and 0 ms.**
- **Never** `SetForceMipLevelsToBeResident`, `bForceMiplevelsToBeResident`, `WaitForStreaming` or
  `WaitForPendingInitOrStreaming`. The last two loop on `FlushRenderingCommands` and `Sleep` with no
  timeout (`StreamableRenderAsset.cpp:334-368`), and they guarantee only that nothing is pending, not full
  residency.
- **A prefetch phase is a separate design decision**, raised only if `G7`'s yield demands it. Codex's
  conditions travel with it: a non-blocking phase outside labelled capture; frame **and** wall-time cutoffs;
  identical preparation in clean controls; ownership of changed residency settings; m52 exclusion
  throughout; cleanup on timeout, cancel and EndPlay; and never a labelled event while it is pending.

**The check.** Game thread, all public or inline:

1. `Tex->GetResource() != nullptr` and `!Tex->HasPendingRenderResourceInitialization()`
   (`Texture.h:1667-1668`; `Texture.cpp:1121-1124`). Else `resource_not_ready`.
2. `!Tex->HasPendingInitOrStreaming(false)` (`StreamableRenderAsset.cpp:229-265`). Else
   `streaming_pending`.
   - It clears the engine's own cached init hint on the game thread (`:239-249`). That is the engine
     recording an observed fact; residency does not change.
3. `S = Tex->GetStreamableResourceState()` (`StreamableRenderAsset.h:174-177`) must be valid. Else
   `resource_not_ready`. (An invalid state also describes virtual-texture resources, which step T1 has
   already refused.)
4. 🔁 082-04 **No runtime LOD bias** (step T9), with the sub-reason named:
   - **`cinematic`:** `Tex->NumCinematicMipLevels > 0`. It is a public, non-editor property
     (`Classes/Engine/StreamableRenderAsset.h:253-256`). Cinematic mips sit inside `MaxNumLODs` but outside
     what the streamer allows, unless the texture is force-resident with cinematic mips
     (`Private/Streaming/StreamingTexture.cpp:186-200`). So such a texture is not fully resident in normal
     play, and this names the cause instead of reporting a bare residency miss.
   - **`per_texture`:** `S.AssetLODBias > 0`, or `Tex->GetCachedLODBias() − NumCinematicMipLevels −
     S.AssetLODBias > 0` (`GetCachedLODBias` is `ENGINE_API`, `StreamableRenderAsset.h:168`; the streamer's
     own per-texture bias, `StreamingTexture.cpp:191-200`).
     - In a cooked build the texture's `LODBias`, its group's `LODBias` and `MaxTextureSize` are applied at
       cook time by stripping mips (`TextureDerivedData.cpp:2435-2440`) and are not applied again at
       runtime (`TextureLODSettings.cpp:173-186`).
     - What remains at runtime is the UI bias (UI textures are already `excluded_group`) and a device
       profile's `MaxLODSize` below the cooked top (`TextureLODSettings.cpp:188-205`), which gives
       `AssetLODBias > 0` (`Texture.cpp:1406-1414`, `:1516-1519`).
   - **`global_sampler`** and **`global_streaming`** are event-level (E4, E5, §R6.2):
     - `r.MipMapLODBias` non-zero (§R3.3);
     - `r.Streaming.MipBias > 0` with `r.Streaming.UsePerTextureBias` 0, which applies the bias to every
       streamed texture's allowed mips (`Private/Streaming/TextureStreamingHelpers.cpp:171-185`, `:299`;
       `StreamingTexture.cpp:191-200`, `:229-233`).
     - ~~With `UsePerTextureBias` at its default 1, the streaming bias is spent only under budget pressure,
       per texture (`Private/Streaming/AsyncTextureStreaming.cpp:356`, `:685-691`). The game thread cannot
       see that per-texture budget bias, so it shows up as `not_fully_resident`. **Stated, not solved.**~~
       🔁 082-05 (C): withdrawn; solved conservatively by the next sub-reason.
       The engine's scalability sets `r.Streaming.MipBias` to 16 at `TextureQuality@0` and 1 at `@1`
       (`Engine/Config/BaseScalability.ini:522`, `:533`).
   - 🔁 082-05 (C) **`streaming_budget`** (binding level, T9, after `cinematic` and `per_texture`, before T10):
     - With `UsePerTextureBias` at its default 1, the streamer raises a streamed texture's private
       `BudgetMipBias` under budget pressure (`Private/Streaming/StreamingTexture.cpp:418-435`, applied at
       `:194-196`). Nothing public exposes it to the game thread (`Public/ContentStreaming.h:458-467` offers only
       pool totals), so this design does not try to read it.
     - It establishes **zero** instead, from public state. The drops are capped at `GlobalMipBias`
       (`AsyncTextureStreaming.cpp:356`), which is `floor(max(0, r.Streaming.MipBias))`
       (`TextureStreamingHelpers.cpp:299`), and they run only while `UsePerTextureBias` is set (`:302`,
       `AsyncTextureStreaming.cpp:685`). So a budget bias is **impossible** when `r.Streaming.MipBias` ≤ 0, when
       `UsePerTextureBias` is 0 (E5 then decides), or on a texture that does not stream
       (`!S.bSupportsStreaming`).
     - **Otherwise** — a streamed texture, `UsePerTextureBias` non-zero and `r.Streaming.MipBias` > 0 — zero
       cannot be established, and the binding is refused `runtime_lod_bias`, sub-reason `streaming_budget`,
       **whether or not it is currently fully resident**. That is the conservative reading of the ruling
       ("refuse when any runtime LOD bias applies"): a bias that may apply cannot be told from one that does.
     - It is strict in the same way as E5: it reads `> 0` rather than the streamer's `floor ≥ 1`, and it does
       not carve out `r.Streaming.UseAllMips` or the editor's zeroed `GlobalMipBias` (`:299`, `:325-328`).
     - **Named residual.** A `BudgetMipBias` set while `r.Streaming.MipBias` was above 0 persists until the
       streamer's reset condition (`AsyncTextureStreaming.cpp:625-637`), so lowering the variable mid-run can
       leave a biased texture that this check admits. T10 then refuses it as `not_fully_resident`, which is
       the conservative direction.
5. **`S.MaxNumLODs == M` and `S.NumResidentLODs == S.MaxNumLODs`:** every mip of the cooked chain is
   resident (step T10). Else `not_fully_resident`, and the diagnostic line gives `resident_lods`,
   `max_lods`, `M` and the resident top dimensions.
   - `GetResourcePostInitState` can cap `MaxNumLODs` below the cooked count
     (`Texture.cpp:1406-1414`). That is refused here with the sub-reason `max_below_cooked`, because the
     output must hold every cooked mip (§R3.4.1).
   - **Optional mips.** If the top mips are optional and not mounted, `NumResidentLODs` can never reach
     `MaxNumLODs` (the streamer clamps to `NumNonOptionalLODs`, `Private/Streaming/StreamingTexture.cpp:224-229`).
     The event is refused `not_fully_resident` with the sub-reason `optional_unmounted`: an unmounted mip
     cannot be restored at runtime.
   - **Non-streaming resources** (`!S.bSupportsStreaming`) pass when fully loaded, because their resident
     count is set at init (`Texture.cpp:1468-1500`). A non-streaming texture whose optional first mip is
     missing has `NumResidentLODs = NumNonOptionalLODs < MaxNumLODs` and is refused, correctly.
   - ⛔ **Not `IsFullyStreamedIn()`.** It returns true for an invalid state and for every non-streaming
     resource whatever was dropped, and it subtracts a non-cinematic runtime bias twice
     (`StreamableRenderAsset.cpp:318-332`).
6. **`snapshot_mip` and the dimensions.** With steps 4 and 5 passed,
   `S.LODCountToAssetFirstLODIdx(S.NumResidentLODs)` = `AssetLODBias + MaxNumLODs − NumResidentLODs` = 0
   (`Public/Streaming/StreamableRenderResourceState.h:69-72`). **`snapshot_mip` is 0 by construction.** It
   is still recorded, with **`snapshot_px`** = `PlatformData->Mips[0]` [W, H] (`Public/TextureResource.h:44-89`).
   - ⛔ Revision 2 admitted a texture whose runtime `AssetLODBias` was above 0, with `snapshot_mip =
     AssetLODBias`, calling that "the platform's full chain". **Withdrawn** (Codex 082-03, P1-3): it is
     refused `runtime_lod_bias`, sub-reason `per_texture`.
   - ⚠ `FTexturePlatformData::GetNumNonOptionalMips` and its neighbours are not `ENGINE_API`, so the
     design uses the state struct's `NumNonOptionalLODs` instead.

**Why nothing can race the check.** Apply is one synchronous game-thread call. The residency check, the
m52 check (below) and the enqueue of every draw and copy all happen inside it. A stream-out already in
flight would have failed step 2. Anything the streamer requests afterwards is enqueued after our commands
on the same render-command queue (§R7.4). `drift` redraws only from the snapshot, never from the source
(§R7.3). 🔁 082-04: the render-thread tripwire (§R3.4.5) makes a broken premise loud.

**m52 interplay.** Before the residency check, a new additive query
`AnomalyStuckMip::IsTextureHeldOrRestoring(const UTexture2D*)` covers **both** m52's held list and its
restoring list, texture-wide. It is needed because no such public query exists today: the private
`IsAwaitingRestore` checks only the restoring list (`Anomaly_StuckLowMip.cpp:768-782`), and the public
`IsAnomalyCurrentlyAnomalous` returns false once the anomaly is inactive (`AnomalyInjectorSubsystem.cpp:800-808`).
A hit refuses the binding `held_by_stuck_low_mip`. There is no preparation phase, so there is no window in
which a new m52 hold could start between the check and the draw. The reverse direction is v1 §4.5's (m52's
auto-pool shared-user gate), and a **targeted** m52 fire still bypasses that gate by design; that is not
evidence of isolation.

**Measured consequence (m52's banked readings).** On the StackOBot bench poses the rock textures sat at
resident **11–12 of 13** mips, i.e. a top resident mip of 1024–2048 px, never 4096
(`2026-09-20-080-02-m52-implemented-and-gated.md:51`; `…080-03…:97`). Both rocks are therefore refused
`not_fully_resident` whatever the budget (§R14).

---

## R5. Host MIDs and runtime materials (P1-4, D7; 🔁 Δ1)

- 🔁 082-04 (Δ1): **the refusal covers a runtime material anywhere in the resolved chain**, not only a raw
  override.
  - Revision 2 tested `Raw` alone. A MID in the mesh asset's own material array (`Raw` null, `Resolved` =
    that MID) therefore passed S3.
  - Step 6 then parented a new MID to it. The engine rejects a MID as a parent (`MaterialInstance.cpp:3076-3083`),
    so the slot would have drawn the default material.
- **The predicate (step S3, §R6.2).** It is applied to `Resolved` and to `Effective` (§R8.1), walking each up
  its `UMaterialInstance::Parent` chain to the root `UMaterial`. 🔁 082-06b (P3-2): the walk is bounded at
  16 links. A chain that still has a parent after 16 links, a cycle included, is **refused** `host_mid` with
  sub `<where>:chain_limit_16_unverified_tail`; it is never accepted as clean (`TexCorruptPure::WalkChain`).
  A link fails if it is:
  - **`mid`**: `IsA<UMaterialInstanceDynamic>()`;
  - **`transient_outer`**: its outermost package is the transient package. `UMaterialInstanceDynamic::Create`
    puts a MID there when no outer is given (`MaterialInstanceDynamic.cpp:73-80`).
  - **`not_loaded`**: it lacks `RF_WasLoaded` (`CoreUObject/Public/UObject/ObjectMacros.h:539`).
    - The loader sets that flag on every export it creates, cooked IoStore builds included
      (`CoreUObject/Private/Serialization/AsyncLoading2.cpp:5485-5502`).
    - It catches a runtime-created material whose outer is not the transient package, which the first two
      tests can miss: a MID created with a component as its outer, or any material made with `NewObject`.
    - It is **stricter than the ruling's two tests** (interpretation I3, §R15). A material loaded from a
      package always carries the flag.
- **The refusal is `host_mid`.** The name is kept for continuity with `G6` and §R14. The REFUSED line and
  the slot disposition carry `where ∈ {override, asset_slot, parent, effective}` and
  `kind ∈ {mid, transient_outer, not_loaded}`.
- **Reachability:**
  - **`where=override, kind=mid`:** StackOBot `SKM_Bot` (`G46`).
  - **`where=asset_slot, kind=mid`:** the F-SYN fixture, produced by the bench lever
    `IAI.Bench.TexCorruptAssetSlotMid <actor>` (§R12.2).
    - The lever writes a MID into the fixture's own duplicate static mesh, at `GetStaticMaterials()[i]`.
      The non-const accessor exists at runtime (`Classes/Engine/StaticMesh.h:898-902`); the asset's
      `SetMaterial` is editor-only (`:1931-1937`).
    - It then calls `MarkRenderStateDirty()` on the using component. The component's resolve reads the asset
      slot when there is no override (`Components/StaticMeshComponent.cpp:2660-2667`).
    - The lever refuses any mesh outside `/Game/CaptureBenchTexCorrupt/`, and restores the asset slot when it
      is turned off or the run ends.
  - **`where=parent, kind=mid`:** **unreachable by construction**, since a material instance cannot take a
    MID as parent (`MaterialInstance.cpp:3076-3083`). Listed so that the parent walk is not mistaken for
    dead code.
  - **`where=effective`:** only through a Nanite override. A MID there fails S3; otherwise S4 refuses the
    slot. **UNEXERCISED.**
  - **`kind=transient_outer` without `mid`, and `kind=not_loaded`:** no runtime producer on the bench.
    **UNEXERCISED.**
- **m53 builds no clone path** (unchanged from revision 2).
  - `CopyParameterOverrides` first clears the MID's parameters, runtime-virtual-texture parameters
    included, and never copies RVT parameters back (`MaterialInstanceDynamic.cpp:484-503`;
    `MaterialInstance.cpp:3502-3519`).
  - A clone also stops following the host's later writes to its own MID (a hit flash, animated
    roughness).
  - Both change the picture beyond the named corruption.
- **Setting the parameter on the host's own MID is refused too.** It would mutate a host-owned object that
  anything else in the game may point at, and the restore could not tell which later host writes to undo.
- **`IAI.Anomaly.TexCorruptCloneHostMids` is not added** (v1 proposed it). `texcorrupt.host_mid_cloned` is
  reserved for a future state-following milestone and **never emitted** by m53.
- **Conditions for that future milestone** (chat's): copy every override type including RVT, define a
  policy for later host updates, and qualify with an RVT-override identity fixture and a host
  parameter-animation fixture.
- **Content:** the StackOBot Bot's slots hold runtime MIDs (`G46`), so the Bot is refused. Whether Lyra's
  characters do is not known offline; `G0` reads it.

---

## R6. The decision tree (P2-9) and the atomic map set (P2-8)

### R6.1 Rules of the tree

- The steps run in a fixed order: event level, then per slot, then per texture binding, then slot
  aggregation, then event aggregation. The first failing step decides.
- **No step has a side effect.** The first side effect, the budget reservation, happens only after the
  tree says APPLY.
- Every texture binding and every slot gets **exactly one** counted disposition.
- The event gets **exactly one** final outcome:
  - `APPLIED`, or
  - `REFUSED` with one reason: the first failing event-level step; or, when every slot is refused, the
    reason of the **earliest step** among the slots' reasons. That makes the reason deterministic.
- A slot is a (component, slot index) pair on every mesh component the target resolves.

### R6.2 The tree

🔁 082-04: changed steps are marked. `M` is the source's cooked mip count (§R3.4.1).

**Event level**

| step | check | reason |
|---|---|---|
| E1 | both corruptor materials and the noise texture resolved non-null (subsystem CDO hard references, v1 §3.3) | `assets_unavailable` — an installation failure, never a content fact |
| E2 | the corruptor this id needs has a complete game-thread shader map at the world's feature level | `corruptor_not_ready` |
| E3 | normal family: `Compat.UseDXT5NormalMaps` reads 0 | `dxt5_normal_host` |
| E4 | `r.MipMapLODBias` reads 0 (§R3.3) | 🔁 `runtime_lod_bias`, sub-reason `global_sampler` (revision 2: `host_mip_bias`) |
| E5 | 🔁 not (`r.Streaming.MipBias` > 0 and `r.Streaming.UsePerTextureBias` = 0) (§R4 step 4) | 🔁 `runtime_lod_bias`, sub-reason `global_streaming` |
| E6 | targeted fire: the requested mode is one of the id's modes | `mode_invalid` |
| E7 | the target resolves ≥ 1 static or skinned mesh component (`AnomalyLod::ResolveLodComponents`, viewport scoping as the other texture anomalies) | `no_mesh` |

**Slot level**, for every slot:

| step | check | disposition |
|---|---|---|
| S1 | the resolved material is non-null | `slot_empty` — intentionally untouched |
| S2 | the resolved material is not translucent | `slot_translucent` — intentionally untouched |
| S3 | 🔁 no runtime material anywhere in the `Resolved` or `Effective` chain (§R5) | `host_mid` with `where` and `kind` |
| S4 | static mesh component: not (`UseNaniteOverrideMaterials()` and `Resolved->GetNaniteOverride() != nullptr`) (`StaticMeshComponent.cpp:2264-2268`, `:2670-2675`; `MaterialInterface.h:510`) | `nanite_override` |
| S5 | `GetMaterialResource(FL)` and its game-thread shader map are non-null (§R2.1) | `shader_map_unavailable` |
| S6 | that shader map is complete | `shader_map_incomplete` |
| S7 | the root material carries the usage flag this component class needs, read side-effect-free with `UMaterial::NeedsSetMaterialUsage_Concurrent` + `GetUsageByFlag` (`Material.h:1235`, `:1252`; `Material.cpp:1705-1732`). ~~Checked: skeletal → `MATUSAGE_SkeletalMesh`; instanced static → `MATUSAGE_InstancedStaticMeshes`; Nanite static → `MATUSAGE_Nanite`; lightmapped static → `MATUSAGE_StaticLighting`.~~ 🔁 082-06b (P2-4): **every** flag the engine's proxy applies to this slot, not the first category. Skeletal: SkeletalMesh; plus Clothing if a cloth section maps to the slot; plus MorphTargets if the asset has morph targets (a superset). Nanite proxy: Nanite; plus InstancedStaticMeshes if instanced; plus StaticLighting if LOD0 is lit. Other static: InstancedStaticMeshes if instanced; SplineMesh if spline; StaticLighting if **any** LOD using the slot has surface lighting (per-LOD, with LOD0's data under shared lighting, which instanced meshes force; none under ForceVolumetric). The min-LOD clamp is ignored, a declared superset. Rule: `TexCorruptPure::RequiredUsages`, with 18 offline rows in the harness. A requirement that cannot be determined refuses with sub `usage_undetermined:<why>`. | `default_material_path` — the slot already renders the default material in game (`Material.cpp:1790-1820`; `StaticMeshRender.cpp:2225-2227`), so its textures are not on screen |
| S8 | the uniform-expression set has ≥ 1 texture entry | `no_textures` — "the compiled material samples no texture"; S5–S6 have already excluded the silent empty list of `MaterialShared.cpp:926-941` |

**Texture-binding level**, for every entry of the set (§R2). 🔁 082-04: T1 and T2 are swapped (P2-9); T6
and T9 are new; revision 2's T6–T9 (m52 hold, readiness, residency, transformable) are now T7, T8, T10 and
T11.

| step | check | disposition |
|---|---|---|
| T1 | 🔁 not virtual: the entry type is not `Virtual`, and a `UTexture2D` is not `IsCurrentlyVirtualTextured()` | `virtual_texture` |
| T2 | 🔁 a `Standard2D` entry holding a `UTexture2D` | `unsupported_type` |
| T3 | `LODGroup` not in {UI, Lightmap, Shadowmap, Terrain_Heightmap, Terrain_Weightmap, Bokeh} (m52's list, `Anomaly_StuckLowMip.cpp:51-65`) | `excluded_group` — 🔁 **blocks the slot** (A4) unless the binding is proven non-spatial (A1) |
| T4 | `ParameterInfo.Name` is not None | `texture_not_parameter` |
| T5 | pixel format admitted for its class (§R3.1) | `unsupported_encoding` |
| T6 | 🔁 the cooked chain is one the render target can hold (§R3.4.1) | `mip_chain_shape` |
| T7 | not held or restoring by `stuck_low_mip` (§R4) | `held_by_stuck_low_mip` |
| T8 | resource valid, initialised, nothing pending (§R4 steps 1–3) | `resource_not_ready` / `streaming_pending` |
| T9 | 🔁 no runtime LOD bias on this texture (§R4 step 4) | `runtime_lod_bias`, sub-reason `cinematic` / `per_texture` / 🔁 082-05 `streaming_budget` |
| T10 | fully resident at the cooked chain, `MaxNumLODs == M` (§R4 step 5) | `not_fully_resident` (sub-reasons `max_below_cooked`, `optional_unmounted`) |
| T11 | all passed | `transformable` |

**Slot aggregation**

| step | check | slot reason |
|---|---|---|
| A1 | 🔁 the family's **required set**. UV: **every** binding. Normal: every binding whose texture `IsNormalMap()`. The one exception (P2-8): a binding **proven non-spatial** — a `UTexture2D` whose cooked chain is a single 1×1 mip — that fails a T step is left out of the set, left untouched and counted `non_spatial_exempt`. A 1×1 binding that is transformable is transformed with the rest. Revision 2's "except the intentionally untouched ones (T3)" is withdrawn. | — |
| A2 | normal family: the required set is non-empty | `no_normal_map` |
| A3 | normal family: the root material's `IsPropertyConnected(MP_Normal)` (`Material.cpp:3961-3964`; a guard only, v1 §2.2) | `normal_unconnected` |
| A4 | **every** binding in the required set is `transformable` | the **first** failing binding's reason, by step order. The diagnostic counter `slots_partial_set` also counts slots where some bindings were transformable and others not. |
| A5 | the required set holds ≥ 1 texture at or above the size policy on both axes (`IAI.Anomaly.TexCorruptMinTexturePx`, compiled 64) | `below_size_policy` |
| A6 | the required set has ≤ `IAI.Anomaly.TexCorruptMaxTextures` bindings (compiled 8) | `map_set_over_cap` |

**Event aggregation**

| step | check | reason |
|---|---|---|
| V1 | ≥ 1 slot qualified | the earliest-step slot reason; if every slot was intentionally untouched, `no_eligible_slot` |
| V2 | the whole requirement, 🔁 scratch included, fits the run-wide cap (§R3.2) | `over_budget` |
| V3 | the transaction completes (§R7.4) | `rt_alloc_failed` / `draw_precondition_failed` / `param_readback_mismatch` (each rolls back) |

### R6.3 The atomic map set, stated plainly

- **Within a slot, the transformation is all or nothing.** A UV family event either moves every map the
  slot's compiled material samples, or it does not touch the slot. A normal family event corrupts every
  normal-map binding of the slot, or none.
- 🔁 082-04 (P2-8): **an excluded map blocks its slot, whatever the exclusion.** That covers a LOD group
  (T3), virtual (T1), type (T2), constant (T4), encoding (T5), chain shape (T6), an m52 hold (T7),
  readiness (T8), runtime bias (T9) and residency (T10).
  - Codex's example, a patterned UI-group mask that would stay fixed while its albedo moved, is now a
    refusal.
  - So is revision 2's example: an albedo parameter next to a constant-node normal map, or next to a
    virtual-textured detail map.
- 🔁 **The one exemption: a binding proven non-spatial.**
  - Its cooked chain is a single 1×1 mip, so every UV reads the same texel and no UV mode can move it.
  - Fixture: an F-SYN slot with an albedo and a 1×1 UI-group texture parameter (§R12.1).
  - A 1×N gradient, or anything larger than 1×1, is treated as spatial: whether it is sampled with the
    mesh's UVs cannot be read.
- **The cap refuses; it does not trim** (A6). v1's "keep the 8 largest" is withdrawn.
- **The size floor is a policy** (A5). It asks that an event have at least one texture of meaningful size.
  It does not claim that a small texture is constant. **Small textures inside a qualifying set are
  transformed with the rest** (they cost almost nothing), so a small patterned mask moves with its slot.
- **Between slots, partiality is allowed and labelled.** A slot refused for any reason is left untouched,
  and the label records `slots_corrupted` / `slots_total` plus each untouched slot's reason. This is
  deliberate: m53 is a per-material texture corruption, and a real texture bug is per material. Chat may
  tighten this to event level.
- **Counting.** Every binding disposition, every slot reason and every `non_spatial_exempt` binding is
  counted per run (§R10). Exclusions are deterministic because the tree is ordered and has no side
  effects.

### R6.4 Every reason, what produces it, and why nothing earlier fires (🔁 P2-9)

🔁 082-04: the last column re-verifies the precedence. It names why no earlier step fires on that producer,
so the producer yields **this** reason and not an earlier one. Each F-SYN producer's slot holds exactly one
binding that fails, and every other binding in it is transformable.

| reason | step | producer (stage) | why no earlier step fires |
|---|---|---|---|
| `assets_unavailable` | E1 | bench lever `IAI.Bench.TexCorruptForceMissingAsset` (S1) | first step |
| `corruptor_not_ready` | E2 | **UNEXERCISED** in a healthy packaged cook; the per-fire check is still built | — |
| `dxt5_normal_host` | E3 | **UNEXERCISED**: the variable is read-only at runtime (`Core/Private/HAL/ConsoleManager.cpp:2842-2852`), and the bench reads 0 | — |
| `runtime_lod_bias` / `global_sampler` | E4 | `r.MipMapLODBias 1` before a fire (S1) | E1–E3 pass on a healthy bench: assets present, corruptor complete, `Compat.UseDXT5NormalMaps` 0 |
| `runtime_lod_bias` / `global_streaming` | E5 | `r.Streaming.UsePerTextureBias 0` and `r.Streaming.MipBias 1` before a fire, restored after (S1) | E4 reads 0 |
| `mode_invalid` | E6 | targeted fire with an unknown mode (S2) | E1–E5 pass |
| `no_mesh` | E7 | targeted fire at F-SYN's mesh-less actor (S1) | E1–E6 pass; the actor exists, so resolution reaches E7 |
| `no_eligible_slot` (all `slot_empty`) | S1 → V1 | F-SYN duplicate mesh whose only asset slot is None (S1) | event level passes |
| `no_eligible_slot` (all `slot_translucent`) | S2 → V1 | MainWorld `SM_GenericPlane` (`M_HoloGridFence`); an F-SYN translucent slot (S1) | the material is non-null (S1) |
| `host_mid` / override | S3 | StackOBot `SKM_Bot` (`G46`) (S1) | the slots' materials are non-null and opaque or masked, which is not translucent (S1, S2) |
| `host_mid` / asset_slot | S3 | F-SYN target with `IAI.Bench.TexCorruptAssetSlotMid` (S1) | the MID's parent is an opaque, loaded fixture material (S1, S2) |
| `nanite_override` | S4 | F-SYN Nanite duplicate mesh whose material has a Nanite override (S2) | S3: both materials are loaded assets |
| `shader_map_unavailable` / `shader_map_incomplete` | S5 / S6 | **UNEXERCISED** on a healthy cook | — |
| `default_material_path` | S7 | F-SYN skeletal slot whose root material lacks `bUsedWithSkeletalMesh` (the `G49` class) (S2) | S1–S4: a loaded, opaque, non-Nanite material; S5–S6: its shader map exists and is complete for what it was compiled with; the usage flag is S7's own check |
| `no_textures` | S8 | `CB_GateLevel` (`BasicShapeMaterial`, v1 §6.1) (S1) | a loaded, opaque, non-Nanite `UMaterial` with complete shaders and the static-mesh usage its components need |
| `virtual_texture` | T1 | F-SYN streaming virtual texture (StackOBot has `r.VirtualTextures=True`, `Config/DefaultEngine.ini:20`) (S1); Lyra's virtual textures (080-04 §5) if a target reaches them (S3) | the slot passes S1–S8, and T1 is the first binding step |
| `unsupported_type` | T2 | F-SYN cube parameter, imported from a six-face DDS (`EditorFactories.cpp:3366`) (S1) | T1: a cube entry is not `Virtual` |
| `excluded_group` | T3 | F-SYN spatial (256²) UI-group texture beside an albedo (S1) | T1–T2: a standard, non-virtual `UTexture2D` |
| `non_spatial_exempt` (counted, not a refusal) | A1 | F-SYN 1×1 UI-group texture beside an albedo (S1) | the 1×1 fails only T3, and the albedo is transformable, so the slot is admitted |
| `texture_not_parameter` | T4 | F-SYN alias target (`G-BIND`) (S1) | T1–T3: a standard, non-virtual `UTexture2D` in the World group |
| `unsupported_encoding` | T5 | F-SYN `TC_HalfFloat` texture (`PF_R16F`) (S1) | T1–T4: a World-group texture parameter |
| `mip_chain_shape` | T6 | **UNEXERCISED**: no fixture in 5.1 produces a chain that is neither one mip nor full. The step stays, and `G0` reads every texture's `M` and level sizes. | — |
| `held_by_stuck_low_mip` | T7 | `G5`: m52 targeted first (held, then restoring) on a MainWorld texture that an m53 target shares (S2) | T1–T6: the shared rock/floor maps are World-group texture parameters in admitted formats with full chains |
| `resource_not_ready` / `streaming_pending` | T8 | **UNEXERCISED**: no deterministic producer without a force-residency call, which m53 must not make | — |
| `runtime_lod_bias` / `cinematic` | T9 | F-SYN texture with `NumCinematicMipLevels = 1` (S1) | T1–T8: a loaded, ready World-group texture parameter; E4–E5 read 0 |
| `runtime_lod_bias` / `per_texture` | T9 | **UNEXERCISED**: it needs a device profile whose `MaxLODSize` is below the cooked top, and no console route sets one | — |
| 🔁 082-05 `runtime_lod_bias` / `streaming_budget` | T9 | the F-SYN streamed 2048² texture (the T10 fixture below), fired in the packaged build with `r.Streaming.MipBias 1` set before the fire and restored after; `r.Streaming.UsePerTextureBias` stays at its default 1. Paired with the same target fired without the variable, which must read `not_fully_resident` (T10): the pair is what shows the order (S1) | E5 does not fire (`UsePerTextureBias` is 1); T1–T8 as for T10's producer; `cinematic` and `per_texture` read 0 |
| `not_fully_resident` | T10 | F-SYN streamed 2048² texture placed far away (S1); MainWorld `SM_rock` as a reading (its world-aligned detail map may be a constant, which gives `texture_not_parameter` first) | T1–T9: a streamed World-group parameter, admitted format, full chain, no cinematic mips; 🔁 082-05 `r.Streaming.MipBias` reads 0, so T9's `streaming_budget` cannot fire |
| `no_normal_map` | A2 | MainWorld `SM_FloorBase` (mask only), normal family (S1) | the slot passes S1–S8 |
| `normal_unconnected` | A3 | F-SYN normal map feeding a non-normal input (S2) | A2: the required set holds that normal map |
| `below_size_policy` | A5 | F-SYN slot whose maps are all 32² (S1) | A4: every binding transformable |
| `map_set_over_cap` | A6 | F-SYN slot with 9 parameter maps (S1) | A4–A5 pass |
| `over_budget` | V2 | F-SYN slot with **two never-streamed 4096²** maps of the same class, ~~213.33~~ 🔁 082-05 (P3) **192 MiB** > 128 MiB (S1) | every binding transformable; a never-streamed texture is resident once loaded |
| `rt_alloc_failed` / `draw_precondition_failed` / `param_readback_mismatch` | V3 | bench lever `IAI.Bench.TexCorruptFailStep <2..6>` (fails that transaction step once, to prove the rollback) (S1) | V2 fits |

An UNEXERCISED reason is written as UNEXERCISED in the gate report, never as a clean zero.

---

## R7. Corruptors, draws and the Apply transaction

### R7.1 The two corruptor materials

v1 §3.2's draw facts carry over unchanged: Surface domain (a UI-domain material has no local-VF shaders in a
cooked build), Unlit, value written as Emissive, no usage flag needed. 🔁 082-04: the draw call is
`K2_DrawMaterial` between `BeginDrawCanvasToRenderTarget` and `EndDrawCanvasToRenderTarget` (§R3.4.2), not
`DrawMaterialToRenderTarget`. It is the same canvas tile path without the trailing mip regeneration.

- **`M_CorruptTex_UV`**: AlphaComposite.
  - Parameters: `SrcColor` (Color sampler), `SrcData` (Masks), `SrcNormal` (Normal), `SrcKind` (0/1/2),
    `UvScale`, `UvOffset`, `UvSwap`, and the scramble scalars of §R7.5.
  - Output: `f(uv)` per mode (tile `frac(uv·N)`, swap `uv.yx`, drift `frac(uv + offset)`, scramble §R7.5),
    `Emissive = src.rgb`, `Opacity = 1 − src.a`.
  - A normal-class source is re-encoded `n.xy·0.5+0.5` into RG (v1 §3.2 encoding table).
- **`M_CorruptTex_Normal`**: Opaque.
  - `SrcNormal` is unpacked to `n`, then `NormalSign` (green flip, invert), `FlatMix` (flat), and `NoiseAmp`
    with a shipped tileable noise-normal texture (noise).
  - The result is re-encoded into RG.
- 🔁 082-04 **Every source sample** in both graphs is a `TextureSample` with:
  - `MipValueMode = TMVM_MipLevel` and MipValue = the scalar `SrcMip`;
  - `AutomaticViewMipBias = false`;
  - `SamplerSource = SSM_FromTextureAsset` (§R3.4.2).
  - The noise texture of `normal_noise` is sampled the same way at its own level.
- 🔁 082-04 **Bench-only scalars for the wrong-copy levers** (§R12.2): `DbgChanSwap`, `DbgSrgbTwice`,
  `DbgTexelShift` (with `TexelSizeU/V`, set per level by C++), `DbgSkipNormalEncode` and `DbgOpaque`.
  - 🔁 082-05 (A): `DbgTexelShift` moves the sample along **U only**, `uv + (TexelSizeU, 0) · DbgTexelShift`.
    `TexelSizeV` is still set per level and is unused by the graph; it stays so the parameter set does not
    change if a later fixture needs the V axis.
  - All are 0 by default and are set only by the lever, never in a client payload.
  - At 0 they are dead arithmetic and add no permutation.
- ⛔ **`AlphaFromSource` is removed.** v1 used it as a no-re-cook fallback that forced A = 1. Under §R11 a
  failing alpha path is a scope decision for chat, not a switch Code flips.

### R7.2 The encoding table

v1 §3.2's table (colour → sRGB target, data → linear target, normal → `.rg` re-encode into a linear
target) is carried, restricted to the admitted formats of §R3.1.

### R7.3 Every draw clears first (P2-6), per mip (🔁 082-04)

For **every** draw — every level's draw at Apply and on every `drift` frame — in this order:

1. `UKismetRenderingLibrary::ClearRenderTarget2D(World, Target, FLinearColor(0,0,0,1))`
   (`KismetRenderingLibrary.h:43`; `.cpp:46-67`).
   - It clears mip 0 of that target, which is the only mip a canvas draw writes (§R3.4.2).
   - It fails silently when its guard fails, so we check the guard (target, resource, world) first
     (§R7.4 step 4).
2. `BeginDrawCanvasToRenderTarget` → `K2_DrawMaterial(CorruptorMid_m, (0,0), (W_m, H_m), (0,0))` →
   `EndDrawCanvasToRenderTarget` (§R3.4.2).
3. 🔁 For m ≥ 1, the copy of `S_m` into the output's mip m (§R3.4.3).

- 🔁 **No mip regeneration.** Revision 2's `DrawMaterialToRenderTarget` → `UpdateResourceImmediate(false)`
  step is gone (§R3.4.4).
- The render target's own `ClearColor` is set to (0,0,0,1) before creation, because the one-time update at
  allocation clears with it (`TextureRenderTarget2D.cpp:700-704`; the engine checks the two agree, `:702`).
- **With the clear**, AlphaComposite gives `RGB = src.rgb` and `A = src.a` on every draw (v1's derivation,
  which Codex's Q1 found sound).
- **Without it**, the recurrence is `RGB_n = src.rgb + RGB_{n−1}·src.a` and `A_n = A_{n−1}·src.a`. An
  opaque source with RGB 0.4 goes 0.4 → 0.8 → 1.0, and fractional alpha decays (Codex P2-6).
- **`drift`.**
  - At Apply, identity draws copy the source into the **snapshot**, a full chain made by the same per-mip
    draws and copies.
  - Every frame, each output level m is drawn from the snapshot's level m (never from the source), then
    copied as above.
  - The snapshot and the output are distinct objects.
  - The per-frame cost (M clears, M draws and M − 1 copies per texture) is part of `G-COST`.
  - A snapshot adds one more 8-bit re-quantisation (source → snapshot → output). `G-RD-DRIFT` measures its
    effect.

### R7.4 The Apply transaction (P2-11)

Steps, in order. The slot is touched only at the last step.

| # | step | failure |
|---|---|---|
| 0 | the decision tree (§R6) says APPLY; nothing has been changed yet | the tree's reason |
| 1 | **reserve** the event's bytes under the run-wide cap, 🔁 scratch included (§R3.2) | `over_budget` |
| 2 | 🔁 **allocate** every render target: outputs, `drift` snapshots and the scratch sets. Each is `NewObject<UTextureRenderTarget2D>` in the transient package, strongly held by the subsystem. Set `ClearColor` (0,0,0,1), `LODGroup`, `Filter`, `AddressX/Y`, `MipsAddressU/V` and `bAutoGenerateMips` (outputs and snapshots: `M > 1`; scratch: false). Then `InitCustomFormat(W, H, PF_B8G8R8A8, bForceLinearGamma = !sRGB)`, then **`UpdateResourceImmediate(true)`** (§R3.4.4). Verify each: `GameThread_GetRenderTargetResource() != nullptr`, the size equals W×H, and `GetNumMips()` equals the expected count (M or 1). | `rt_alloc_failed`; roll back |
| 3 | create the corruptor MIDs (one per texture, mode and 🔁 level), set their parameters (🔁 `SrcMip`, and `TexelSizeU/V` per level), and read each back | `param_readback_mismatch`; roll back |
| 4 | **re-check every precondition of every draw ourselves**: `FApp::CanEverRender()`, the world is valid, the material is non-null, and 🔁 each target's resource is non-null. Those are the exits of `BeginDrawCanvasToRenderTarget` (`KismetRenderingLibrary.cpp:708-727`) and of the clear (`:50-52`). Also E2's corruptor shader-map completeness. After this step no draw can take a silent exit. | `draw_precondition_failed`; roll back |
| 5 | 🔁 **enqueue**: the tripwire read; then, per texture, per level, the clear → draw → copy sequence (§R3.4, §R7.3), snapshots first for `drift`; the tripwire read again; and, for static modes, the scratch release | — |
| 6 | create one host MID per distinct resolved material (parent = the resolved material, which §R5 guarantees is not a runtime material), `SetTextureParameterValueByInfo` for every binding in each qualifying slot's set, and read each back (§R2.3) | `param_readback_mismatch`; roll back |
| 7 | **commit:** `SetMaterial(i, HostMid)` on every qualifying slot, and record ownership (§R8) | — |

- **Ordering, and why no wait or fence is needed.**
  - Steps 5–7 enqueue render commands on the single render-command queue, which runs in submission order
    (`RenderCore/Public/RenderingThread.h:256-291`; the named-thread queue is a FIFO,
    `Core/Private/Async/TaskGraph.cpp:876-878`, `Core/Public/Containers/LockFreeList.h:523-524`, `:811`).
    The material-instance parameter updates use the same queue (`MaterialInstance.cpp:390-416`).
  - 🔁 Every level's draw and copy of every texture is enqueued inside this one call, before step 6's
    parameter commands.
  - The same frame's proxy recreate from `SetMaterial` (`SendAllEndOfFrameUpdates`,
    `Renderer/Private/SceneRendering.cpp:4528`) and its scene draw (`FDrawSceneCommand`, `:4650`) are
    enqueued **later**. So is the frame's `FDeferredUpdateResource::UpdateResources` (`:4321-4322`), which
    finds none of our targets on its list (§R3.4.4).
  - ⇒ Every level of every output is final before any scene draw can sample it, on the frame the label
    begins.
- **Rollback** releases everything steps 2–6 created, 🔁 scratch included (`ReleaseResource()`, then the
  strong references are dropped). It un-reserves the bytes and touches no slot, because the commit is step 7
  alone. Each rollback is counted per step.
- **What the transaction cannot prove.** It cannot prove that the GPU executed the draw or the copy
  correctly; a void call reports nothing. That proof is `G-ID` / `G-ID-M` (content) and `G11` (the packaged
  shader path draws non-default content).
- **Where it runs:** inside the fire tick. The labelled window begins on that frame, and `G1` (ONSET) checks
  that the picture changes on it.

### R7.5 `uv_scramble` as a true permutation (D2)

- Cells are indexed `i = y·K + x`, with `N = K²` cells (K compiled 8, a knob).
- The permutation is `π(i) = (a·i + b) mod N`:
  - `a` is chosen among values coprime to N, so π is a bijection;
  - `b` is chosen in [0, N);
  - the pair (1, 0), the identity, is excluded.
- **(a, b) are derived by hashing the run seed with the event's fire index.** Nothing is drawn from the
  auto-injector's stream, so `R-SEED`'s draw protocol (v1 §7.2) is unchanged, and the pair does not
  depend on whether an earlier Apply succeeded.
- The corruptor needs the inverse: output cell j samples source cell `π⁻¹(j) = a⁻¹·(j − b) mod N`. C++
  computes `a⁻¹ mod N` with the extended Euclidean algorithm and passes `(a⁻¹, b, K)` as scalars. The
  local cell UV is kept.
- **Apply verifies bijectivity** by enumerating all N cells on the CPU. The result is logged as
  `scramble_bijective`; a failure would refuse the event, and is unreachable by construction.
- Float arithmetic on integers below 2²⁴ is exact in the shader (K ≤ 64 ⇒ N ≤ 4096).
- It is deterministic across hosts. It is also seam-producing: sampling across a cell boundary is part of
  the look.

---

## R8. Slots, ownership and exact restore (P2-7)

### R8.1 What is captured per slot, before any change

| field | source |
|---|---|
| `Raw` | `Comp->OverrideMaterials.IsValidIndex(i) ? Comp->OverrideMaterials[i] : nullptr` (public, `Classes/Components/MeshComponent.h:26-28`), plus `RawArrayLen = OverrideMaterials.Num()` |
| `Asset` | static: `GetStaticMesh()->GetMaterial(i)` (`StaticMesh.cpp:7685-7693`); skinned: `GetSkinnedAsset()->GetMaterials()[i].MaterialInterface` (`SkinnedMeshComponent.cpp:1252-1264`) |
| `Resolved` | `Raw ? Raw : Asset` — the bound material before any Nanite substitution, exactly as the components resolve it (`StaticMeshComponent.cpp:2655-2667`; `SkinnedMeshComponent.cpp:1252-1264`) |
| `Effective` | `Comp->GetMaterial(i)`, with the Nanite substitution on static meshes (`StaticMeshComponent.cpp:2670-2675`) |

- **Nanite is inspected from `Resolved`** (S4): `UseNaniteOverrideMaterials()` and
  `Resolved->GetNaniteOverride()`. v1 read only `GetMaterial()`, which returns the override B rather
  than the slot's material A; saving B and restoring B would lose A, and checking B for an override could
  miss the refusal. `Effective != Resolved` is logged as a cross-check.
- **Strong references.** `Raw`, `Asset` and `Resolved` go into a `UPROPERTY` array on the injector
  subsystem (the `CorruptedTexturePink` pattern, `AnomalyInjectorSubsystem.h:133-137`) from capture until
  that slot's restore, or its abandonment, completes. The actor and the component stay weak.
  - This closes Codex's GC case: `corrupted_texture` keeps its original weakly
    (`Anomaly_CorruptedTexture.h:30`), so a runtime material held only by the slot could be collected
    mid-event and the revert would reset to the mesh default. m53 does not copy that.

### R8.2 Ownership

- An event owns a set of (weak component, component `FName`, owner, slot index) keys, each mapped to the
  host MID it installed, plus the render targets and corruptor MIDs it created.
- A render target is never shared across events or actors. One host MID per distinct resolved material
  within the event (v1 §4.2's sharing rule, kept for the MID only).

### R8.3 Restore

The m17 contract, mirrored, with the raw state:

1. Re-find the component: the weak pointer, otherwise by `FName` on the owner.
2. Touch slot i **only if `OverrideMaterials[i]` is still this event's host MID.** A raw read is correct
   here, because the host MID's parent has no Nanite override (S4). If it is not ours, the slot is left
   alone and logged `left-to-game`; the host replaced it and we do not stomp.
3. If `Raw` was non-null, `SetMaterial(i, Raw)`: the strongly held original **object**, which is an
   exact restore. Otherwise `SetMaterial(i, nullptr)`, and the slot falls back to `Asset`.
   - **Named limit:** `OverrideMaterials` may stay longer than `RawArrayLen`, with null entries.
     `SetMaterial` grows it with `AddZeroed` (`MeshComponent.cpp:59-62`), and nothing shrinks it without
     touching other slots. That renders identically but is not a byte-identical component state.
     `override_len_before/after` is logged.
4. **Sweep** every mesh component of each touched owner for a slot still holding one of our host MIDs
   (the component was re-created after Apply). That is **cleanup**, logged `swept`, and never evidence of
   an exact restore.
5. **Release:** each render target's `ReleaseResource()` and its reference (🔁 082-04: outputs, `drift`
   snapshots and any scratch the event still holds); the corruptor and host MIDs; then, only after the slot
   is restored, the strongly held originals. Bytes move from live to `pending_release` (§R3.2).
6. **Log per slot:** `restored-exact` (the raw object re-set) / `restored-default` (raw was null) /
   `left-to-game` / `unresolved` / `swept`.

⚠ Pointer restoration and dropped references do not prove byte-identical future frames or a synchronous
GPU deallocation. `G3` measures the frames.

### R8.4 Two ids, and which fire paths are supported

- `uv_corruption` and `normal_corruption` keep separate ownership arrays, render-target sets and counters.
- **Supported fire paths:** auto-pool `TryFireOnce` and targeted `TryFireSpecific`. Both exclude an actor
  already in `LiveFires` (`AnomalyAutoInjectorSubsystem.cpp:286`; `:454`, `:459-469`), so the
  one-anomaly-per-actor rule (`G30`) holds there.
- **Not supported:** raw `ApplyAnomaly` (`AnomalyInjectorSubsystem.cpp:705-730`) and its callers — console
  `IAI.Apply`, the control server's `inject`, the selector, and session globals. They do not check
  `LiveFires`, and m53 does not claim isolation for a raw apply that overlaps `missing_texture` or
  `corrupted_texture` on the same actor.

### R8.5 Exits

Normal revert, `FinishRun`, cancel before focus, target destroyed (`OnEndPlay` plus a weak-pointer poll,
v1 §4.4), level change, world teardown, and transaction rollback (§R7.4). Every one reaches §R8.3.

---

## R9. Prewarm, first use, cost (P2-11, D6)

### R9.1 The lists are split

- Today `GatherAnomalySwapMaterials` (`AnomalyCaptureSubsystem.cpp:2399-2412`) feeds both the prewarm
  (`PrewarmAnomalyShaders`, `:2518`) and the per-frame count (`CountIncompleteAnomalyMaterials`, `:2418`,
  called per frame at `:4442`).
- m53 adds a new `GatherAnomalyPrewarmMaterials` = that list plus the two corruptors, used **only** by the
  prewarm. `CountIncompleteAnomalyMaterials` keeps calling the unchanged `GatherAnomalySwapMaterials`, so
  the per-frame pending count does not widen (`G298` unchanged).

### R9.2 What the prewarm is not

- In a cooked build `EnsureIsComplete` is a no-op: its whole body is `#if WITH_EDITOR`
  (`MaterialInterface.cpp:1468-1480`). The m47 prewarm's own log says so (`AnomalyCaptureSubsystem.cpp:2540-2541`).
- PSO precaching is off in 5.1 (`RHI/Private/PipelineStateCache.cpp:103-110`, `:2131-2139`). The first
  draw of each new combination of shaders, render state and render-target format creates its PSO, and the
  command list waits for it (`:2067-2094`). That is a stall, not a wrong frame.
- **Distinct first-use PSOs:**
  - the UV corruptor × {RGBA8, RGBA8_SRGB};
  - the normal corruptor × RGBA8;
  - the clear;
  - 🔁 082-04: the one-time mip generation at allocation, per format (§R3.4.4; `RenderCore/Private/GenerateMips.cpp`).
  - At least five.
  - 🔁 The per-mip copy is a copy, not a draw, and creates no PSO.

### R9.3 The warm draw

- **Where.** The m47 prewarm runs in `BeginActualRun` (`AnomalyCaptureSubsystem.cpp:3517`), and the very
  next tick captures the first lead-in frame (`:718`). There is no uncaptured frame to use on the direct
  path.
- **What.** A new **non-capturing** phase, `TexCorruptWarm`, between `BeginActualRun` and `LeadIn`,
  modelled on `SettleAfterFire` (`:722-725`), which counts down without calling `CaptureCurrentFrame`.
  - It is entered only when an m53 id is enabled in the pool or is the targeted anomaly. Every other run
    is byte-identical.
  - It lasts 2 frames. It draws each corruptor once into a small scratch target of each format (🔁 082-04:
    the allocation update, a clear, a draw, and one per-mip copy into a second target) and releases them.
  - Knob `IAI.Capture.TexCorruptWarmDraw`, compiled on.
- **What it can and cannot claim.** It moves first-use PSO creation out of the captured frames **if** the
  scratch draws hit the same PSOs (same shaders, blend and formats). Engine offers no PSO introspection,
  so that is not proved.
  - Instead it is **measured**: `G-COST` compares the first labelled Apply frame's time with the warm draw
    on and off, for both corruptors and every target format.
  - The residual cold-first-fire cost is a **measurement with an owner acceptance decision** (O3).

### R9.4 `G-COST`, the predeclared method

- **Setup:** paced 30 fps, the same build, m55 held constant (on, on both sides). Order A,B,B,A after one
  declared discard leg (`G186`).
  - A = both m53 ids disabled.
  - B = targeted m53 legs on the fixture slot with the **most required maps that fit the budget**, one
    static mode; and separately `drift`.
- **Readings:**
  - per-frame wall time, median and p99; `speed_ratio`; capture throughput (frames written per wall
    second);
  - the first Apply frame's time with the warm draw on and off;
  - the `drift` per-frame redraw time, with 🔁 its clears, per-level draws and per-mip copies;
  - 🔁 082-04: the Apply-frame cost of the per-mip draws and copies against the number of levels M;
  - the cost of a cancel and of a rollback;
  - repeated allocation and release over ≥ 20 events, ~~with `texcorrupt_rt_bytes_peak` returning to 0
    after each event.~~ 🔁 082-06b (P3-3): balance is judged from **live and pending bytes returning to 0**
    on each event's frame-qualified terminal line, `TEXCORRUPT-LEDGER … kind=post_revert revert_frame=R
    frame=R+2 … live=0 pending=0` (and on the rollback line of a refused event). The ledger releases pending
    bytes at frame R+2. `texcorrupt_rt_bytes_peak` is the **run's maximum** live+pending and stays
    monotone; it cannot return to 0 and is never reset to satisfy this check.
- **Acceptance.** A difference no larger than the within-build spread across positions reads "below the
  resolution of this instrument" (`G169`). Anything larger is reported with its number and goes to the
  owner. **No threshold is invented here**; accepting the cost, the cold first fire and `drift` is an
  **owner decision** (O3).

---

## R10. Strength prior and labels (P3-12)

- **`texcorrupt.expected_strength_class`** ∈ {`strong`, `medium`, `weak`} is a **mode prior**. The client
  readme will say: "the class this mode is expected to fall in on typical content; it is not a
  measurement of this event."
  - Per-event pixel change is m55's (`chg_*`). Perceptibility is measured by nobody.
  - v1's universal statements are withdrawn. A constant or permutation-invariant texture above the size
    policy can be unchanged by tile or scramble; a flat normal map is unchanged by sign flips; lighting
    controls how much a normal change shows.
  - Priors: `strong` tile, scramble, invert; `medium` green_flip, noise, drift; `weak` swap, flat.
- **m55 stays out** of eligibility, ranking, seeds, verdicts and `observable` (the m55 v1 contract).
- **`G-STR`** (S3) may amend the default mode sets and the prior classes from offline measurement. The
  key's values move with that amendment.

**Label keys.** Additive; `label_schema` stays 2 (the m52 ruling, `2026-09-20-m52-stuck-low-mip.md:672-673`).

- **`annotation.json`:** `anomaly_type` = the id; `anomaly_subtype` = the mode. These are new values in
  existing fields, so the field set does not move (`P6`).
  - 🔁 082-06b (P2-3): the mode is the **captured frame's** `texcorrupt.mode` telemetry: the snapshot's own
    record on the async path, and the telemetry read at the capture moment on the sync path. It is taken on
    the event's anchor frame, keyed by (id, start frame, target). The live mode at frame completion is
    never read, so an event completed after revert keeps its subtype and two events of one id keep their own.
- **`labels.jsonl`, inside m53 anomaly entries only:**
  - `texcorrupt.mode`, `texcorrupt.expected_strength_class`;
  - `texcorrupt.slots_corrupted`, `texcorrupt.slots_total`, `texcorrupt.slots_untouched[]` of
    `{slot, reason}`;
  - `texcorrupt.condition_held`;
  - per-mode parameters: `texcorrupt.tile`, `texcorrupt.tile_detail_mips`, `texcorrupt.uv_offset`
    (`drift`, per frame), `texcorrupt.scramble_cells`, `texcorrupt.noise_amp`;
  - `texcorrupt.textures[]` of `{name, param, association, layer_index, class, pixel_format, snapshot_mip,
    snapshot_px, rt_bytes}`, 🔁 plus `mip_count` (M) and `non_spatial_exempt` (bool);
  - 🔁 082-04: `texcorrupt.required_bytes` (outputs, snapshots and scratch, §R3.2), so `G7` can re-read an
    event against another budget;
  - 🔁 082-04: `texcorrupt.collateral_drops` (§R12.3 `G-COLL`) for the event's labelled frames.
    - 🔁 082-05 (B): this is the within-leg count of collateral textures below their Apply-time level. It is
      a reading of one leg and is **never subtracted** from another leg's; `G-COLL`'s verdict comes from the
      paired per-texture levels in the `TEXCORRUPT-COLL` lines. `texcorrupt.collateral_count` and
      `texcorrupt.collateral_truncated` are emitted beside it so a truncated set is visible in the row.
    - 🔁 082-06b (P2-2): plus `texcorrupt.collateral_incomplete`, which sums four counts: paths dropped by the
      cap, materials without a complete shader map, textures whose residency is unknown at Apply or now,
      and 1 if nothing was drawn on screen at all. Plus `texcorrupt.collateral_complete`. A reading with
      `collateral_complete` false is **never a clean reading**, whatever `collateral_drops` says.
  - 🔁 082-06b (P2-1): `texcorrupt.condition_detail`, the branch of the one condition predicate that
    answered (see `condition_held` below).
- **Removed from v1:** `texcorrupt.route`, `texcorrupt.reconstructed`, `texcorrupt.strength_class`,
  `src_resident_top_px`, `rt_size`.
- **Reserved, never emitted:** `texcorrupt.host_mid_cloned`.
- **`run_summary.json`:**
  - `texcorrupt_fires_applied`;
  - one `texcorrupt_refused_<reason>` per event-final reason. 🔁 `texcorrupt_refused_host_mip_bias` becomes
    `texcorrupt_refused_runtime_lod_bias`; the sub-reasons are in the dispositions objects;
  - two dispositions objects, `texcorrupt_slot_dispositions` and `texcorrupt_binding_dispositions`
    (reason → count, 🔁 including `non_spatial_exempt` and each `runtime_lod_bias` / `host_mid`
    sub-reason), which keeps the key set bounded;
  - `texcorrupt_rt_bytes_peak`, `texcorrupt_rollback_<step>`;
  - `texcorrupt_restored_exact`, `texcorrupt_restored_default`, `texcorrupt_left_to_game`,
    `texcorrupt_swept`;
  - 🔁 082-04: `texcorrupt_rt_mip_mismatch` (the tripwire, §R3.4.5; expected 0) and
    `texcorrupt_collateral_drops` (§R12.3).
  - 🔁 082-06b (P2-2): `texcorrupt_collateral_incomplete_frames`, the labelled frames whose collateral
    reading came from an incomplete set.
- **`condition_held`** is read from live state, not special-cased: the slot's raw binding is this event's
  host MID **and** that MID's parameter still resolves to our render target. `NoApply` (§R12.2) therefore
  reads false because nothing was installed, not because a lever says so.
  - 🔁 082-06b (P2-1): the S1 code at `193bd35` did special-case it (`NoApply != 0` returned false first).
    Now one predicate (`TexCorruptPure::ConditionHeld`) takes the expected set, i.e. every qualified slot
    that received a host MID and every bound parameter, and it has **no NoApply input**. An empty expected
    set reads `no_expected_set`, never held. The reading is also emitted as `texcorrupt.condition_detail`
    (`installed` / `slot_not_installed` / `binding_readback` / `no_expected_set` / `no_event`) and printed
    on the APPLIED and NoApply 2 lines. So `NoApply 1` must read `slot_not_installed`, `NoApply 2` must read
    `no_expected_set`, and an applied event must read `installed`, all through the same code.

---

## R11. Failure policy (P1-5): no route A

| finding in S1 or S2 | action |
|---|---|
| `G-ID` fails for the **normal corruptor** only, and every UV path passes, including the UV corruptor's normal-class re-encode | defer `normal_corruption`; continue UV |
| `G-ID` fails on the **UV corruptor's normal-class path** | STOP and report. The proposal to chat: UV restricted to slots whose required set holds no normal-class binding. That is a narrower product, so chat rules it. |
| `G-ID` fails on a **colour, data or alpha** path | STOP and report. The proposal: refuse the failing class (`unsupported_encoding` naming it), if the remaining subset is non-empty. Chat rules it. |
| 🔁 `G-ID-M` fails on any Q row (magnification, minification, grazing, transitions), including the authored-chain and engine-authored chain rows | STOP; it bears on N1(a), and no fallback to regeneration is taken |
| 🔁 the mip tripwire `texcorrupt_rt_mip_mismatch` reads non-zero | STOP; the premise of §R3.4.2 is wrong on this build |
| a **wrong-copy control does not fail** (reads below 16 on a row it is assigned to), or 🔁 an offline lever proof is below 32 | the instrument is invalid; STOP (`G96`) |
| `G3` or `G4` fails (restore, lifecycle, rollback) | STOP |
| 🔁 `G-COOK` fails (a byte change outside the new folder, an archive not verified, a map gate failure, a changed `CB_GateLevel` / `MainWorld` chunk) | STOP before any leg runs on the new container |

- **Never:** switch to the takeover, relax a tolerance, re-enable mip regeneration, or widen a fixture so
  that a gate passes. A takeover is a separate product decision.
- **Code does not narrow the product silently.** A restriction is proposed to chat with the `G-ID`
  evidence, and applied only after a ruling.

---

## R12. Gates (P2-10)

**Classes.** **Q** = qualification gate: it can fail, and a failure stops the stage. **D** = diagnostic
reading: reported, no pass or fail. **O** = owner decision: numbers go to the owner.

### R12.1 Fixtures (🔁 082-04: F-SYN is N2, approved, additive only)

#### F-SYN — the synthetic fixture level

**Where it lives.**

- Everything new goes in **one new folder** of the StackOBot project, `/Game/CaptureBenchTexCorrupt/`
  (`Content\CaptureBenchTexCorrupt\`). It is a sibling of `/Game/CaptureBenchGate/`, where `CB_GateLevel` and
  `CB_LodCalib` live (`Content\CaptureBenchGate\`).
- The level is `/Game/CaptureBenchTexCorrupt/CB_TexCorruptLevel`. Like `CB_GateLevel` it is a standard level,
  not World Partition, so it writes no `__ExternalActors__` files.
- Nothing is authored into the plugin's `Content\` except the plugin's own shipped assets (the two corruptors
  and the noise normal). A plugin script in the `create_missing_texture_materials.py` pattern makes those in
  S1.

**The authoring tools** (CaptureBench, written in S1; ⛔ not touched by this brief):

- **`CaptureBench/tools/texcorrupt_fixture_images.py`**: plain Python, deterministic, no editor.
  - It generates every source image: PNG, and DDS where a mip chain or a cube is needed.
  - It writes `texcorrupt_fixture_manifest.json`: each image's SHA-256, its intended import settings, and
    the offline wrong-copy proofs (§R12.4).
- **`CaptureBench/tools/make_texcorrupt_fixture.py`**: editor Python, launched as `make_gate_level.py` is
  (`UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script=…`, `gotchas.md:1512`).
  - It imports the images with the manifest's settings, creates the materials, duplicates the meshes it
    needs into the folder, builds the level, and saves **only** that folder.
  - It **refuses** to run if the folder exists, unless given `--allow-overwrite-texcorrupt-fixture`, which
    deletes only inside that folder. This is `make_gate_level.py`'s guard pattern (`:14-24`).
  - It asserts that every asset path it creates starts with `/Game/CaptureBenchTexCorrupt/`.
  - It references engine content (`/Engine/BasicShapes/*`, engine materials) and never saves it. A mesh it
    must change (the asset-slot MID target, the Nanite target, the empty-slot and skeletal targets) is
    **duplicated** into the folder first.
  - Before exiting it lists the dirty packages. Any dirty package outside the folder **fails the run** and is
    not saved. (The m47 lesson: a script that saves unconditionally re-serialises assets it never meant to
    change.)
  - Its lights are movable: static lighting is off on StackOBot (`Config/DefaultEngine.ini:23`), and the
    gate level's own tool records that static lights render black there (`make_gate_level.py:106-109`).
- **`CaptureBench/tools/texcorrupt_bytecheck.ps1`**: the manifest check below.

**Contents.** All ≤ 1024² and never streamed unless stated. Each row names the gate it serves.

| group | assets | serves |
|---|---|---|
| encoding rows | one texture per §R3.1 row (the 13 G-ID rows of §R12.4). Each has an Unlit **readout** material that samples it through the matching sampler type (Color, LinearColor, Masks, Normal, Grayscale, Alpha) into Emissive, on a plane facing the camera. Multi-channel colour and data images keep \|R − B\| ≥ 96 and every channel in [48, 192]; one-channel images stay in [48, 192]; normal images keep \|n.x\|, \|n.y\| ≤ 0.35. | `G-ID` |
| alpha rows | sRGB DXT5 and sRGB BC7 with alpha in [0.25, 0.75], which lerps between two colours ≥ 192 levels apart; a linear DXT5 (`TC_Masks`) with alpha; a **masked** material whose opacity mask reads base-colour alpha | `G-ID` alpha half |
| chain rows | **(a) authored chain:** a DDS (`B8G8R8A8`, sRGB) whose every mip is a different solid colour. The importer keeps the chain as `TMGS_LeaveExistingMips` (`EditorFactories.cpp:3499-3503`), and the compression is set to `TC_VectorDisplacementmap` so it stays BGRA8. Consecutive mips differ by ≥ 64 in some channel, and every mip differs from mip 0 by ≥ 64. **(b) box chain:** a one-texel checker at mip 0 with contrast 128 (values 64 / 192), BGRA8 linear, engine mips. **(c) engine-authored chains:** `TMGS_Sharpen5`, `TMGS_Blur5`, and alpha-coverage scaling on an sRGB DXT5. **(d) normal:** a BC5 one-texel checker with n.x = ±0.35. | `G-ID-M` |
| geometry | a plane filling the view (magnification), a far plane (minification), a grazing plane, and a long receding plane that crosses mip transitions, each carrying the chain rows (a), (b), (c) and (d) | `G-ID-M` |
| shared sampler | a readout material that samples through `SSM_Wrap_WorldGroupSettings` | `G-ID-M` |
| redraw | an opaque BGRA8 linear input with RGB in [48, 128], and a fractional-alpha sRGB DXT5 input with alpha in [0.25, 0.75] | `G-RD` |
| binding | the material-layer target (a layer parameter with the same name as a global parameter in another layer); the alias target (a static-switch-off parameter P defaulting to T, beside an active constant sample of T); the dead-binding target (a parameter multiplied by 0) | `G-BIND` |
| runtime material | a duplicate of `/Engine/BasicShapes/Plane` in the folder, used by one actor only, for `IAI.Bench.TexCorruptAssetSlotMid` | Δ1, `G6` |
| reasons | a mesh-less actor (`no_mesh`); a duplicate mesh whose only asset slot is None (`slot_empty`); a translucent slot; a streaming virtual texture (`r.VirtualTextures=True`, `Config/DefaultEngine.ini:20`); a cube parameter (a six-face DDS, `EditorFactories.cpp:3366`); a spatial 256² UI-group texture beside an albedo; a **1×1** UI-group texture beside an albedo (`non_spatial_exempt`); a `TC_HalfFloat` texture; a texture with `NumCinematicMipLevels = 1`; a streamed 2048² texture placed far away (also 🔁 082-05 the `streaming_budget` producer, §R6.4); a slot whose maps are all 32²; a slot with 9 parameter maps; **two never-streamed 4096² maps of the same class in one slot** (~~213.33~~ 🔁 082-05 **192 MiB**, §R3.2); a Nanite duplicate mesh whose material has a Nanite override; a skeletal slot whose material lacks `bUsedWithSkeletalMesh` (a skeletal mesh duplicated into the folder); a normal map feeding a non-normal input | `G-REASON`, `G6` |
| reference bugs | copies of a readout material with a real ×N UV scale and a real `uv.yx` swap, in their own graphs | `G-ID-M` transformed rows (S2) |

- The level has a settled camera (a fixed player start and camera, as in `CB_GateLevel`) and movable lights.
- **Luma gate (`G151`)** on the level's first capture: `mean_luma > 0` and a non-zero pixel count > 0, or S1
  stops.

**Additive only: the byte check.**

- `texcorrupt_bytecheck.ps1` hashes (SHA-256) every file under `StackOBot\Content`, `StackOBot\Config` and
  the plugin's `Content\`, and writes a manifest.
- It runs three times:
  - **M0**, before any S1 authoring;
  - **M1**, after the fixture tool and the plugin's corruptor authoring;
  - **M2**, after the cook.
- **The rule:**
  - every file present at M0 is byte-identical at M1 and at M2;
  - no file is removed;
  - every added file is under `Content\CaptureBenchTexCorrupt\` or is one of the plugin's new corruptor and
    noise assets.
  - Anything else stops S1 before the next step. The report names `CB_GateLevel.umap`, `MainWorld.umap` and
    `MainWorld`'s external actors explicitly.
- **The cook needs no config change.** The map set is the `-map=` list (runbook §8.6,
  `setup-runbook.md:629`, `:722-730`), which gains `/Game/CaptureBenchTexCorrupt/CB_TexCorruptLevel`. So
  `Config\` stays in the unchanged set.

**The container, before and after the cook.**

- **Before the cook:**
  - the staged container (`StackOBot-Windows.utoc`, `.ucas`, `.pak`, `global.utoc`, `global.ucas`) and the
    staged exe are archived to `_binary_baselines\m53-s1-precook-container-<utoc8>\`;
  - they are hash-verified at the destination before anything is overwritten. This is the m46 pattern
    (`_binary_baselines\README.md:419-425`), and "a baseline is a quartet" (`:30-36`).
- **After the cook:**
  - The map gate `verify_cooked_maps.ps1` runs with `-Required CB_GateLevel,MainMenu,MainWorld,Entry,CB_TexCorruptLevel`
    and must exit 0. Its default set is not edited, so the new level is named rather than silenced.
  - The IoStore listing of both containers (`UnrealPak <uproject> IoStore -List=<utoc> -csv=<csv>`;
    `Developer/PakFileUtilities/Private/PakFileUtilities.cpp:5346-5348`;
    `Developer/IoStoreUtilities/Private/IoStoreUtilities.cpp:6422-6431`; CSV header `:3936`, per-chunk hash
    `:4013`) is compared by package name.
    - **Q:** every chunk of `CB_GateLevel`, `MainWorld` and `MainWorld`'s external actors has the same hash
      in both containers.
    - **D:** every other pre-existing package, listed as changed or unchanged, and whether its source changed
      since the archived container was cooked. The archived container predates S1, so for "unchanged by N2"
      the source manifest is the authority, not the container (interpretation I4, §R15).
    - The new folder's packages and the plugin's new assets must be present. That is the listing's positive
      control: it sees additions.
  - The startup read-back line (`G11`) and the A44 string scan of the staged exe.

**Never in a client package: the PRE-DELIVERY-CHECKLIST line.** It lands in S1, next to the "No bench lever
is on" box (`PRE-DELIVERY-CHECKLIST.md:254`):

> - [ ] ⛔ **No m53 bench fixture ships.** `CB_TexCorruptLevel` and everything under
>   `/Game/CaptureBenchTexCorrupt/` belong to the StackOBot bench project, never to the plugin:
>   `git -C <plugin> ls-files Content` lists only the plugin's shipped materials and textures, and nothing in
>   the plugin references `/Game/CaptureBenchTexCorrupt`. On any cooked build that leaves this box, the
>   cooked map index contains no `CB_TexCorruptLevel` (`verify_cooked_maps.ps1`, or the cook log's map list),
>   and `IAI.Bench.TexCorruptAssetSlotMid` was never typed (see the bench-lever box).

#### F-MW, F-GATE, F-LYRA (unchanged)

- **F-MW** — StackOBot `MainWorld` at its settled pose: `SM_FloorBase`, the rocks, `SKM_Bot`,
  `SM_GenericPlane`, the modular kit. Real-host readings.
- **F-GATE** — `CB_GateLevel`: refusals only (`no_textures`).
- **F-LYRA** — `L_ShooterGym` at Lyra's own defaults (TSR, Lumen, auto-exposure on): the cubes, the
  weapons, the characters.

### R12.2 Controls, available on every fixture

- **Null (matched): `IAI.Bench.TexCorruptNoApply 1`.** The same recipe, the same draws, the same MIDs; only
  step 7 of the transaction (the slot commit) is skipped.
- 🔁 082-04 **No-allocation null: `IAI.Bench.TexCorruptNoApply 2`**, for `G-COLL` (N3).
  - The decision tree and the reservation arithmetic run; nothing is allocated, drawn or committed.
  - 🔁 082-06b (P2-6): the target watches (EndPlay, destroy, world end) **are registered**, exactly as for an
    applied event, so an EndPlay while the actor is still a valid UObject ends both legs at the same frame.
  - The matched null above allocates the same bytes as the applied leg, so it would hide exactly the effect
    `G-COLL` looks for (interpretation I1, §R15).
- **Positive (matched): `corrupted_texture`**, targeted at the same actor from the same pose. It has no
  fixture gate and fires on MainWorld (`AnomalyInjectorSubsystem.cpp:185`; `Anomaly_CorruptedTexture.cpp:57-134`).
  On `L_ShooterGym`, m55's `solid_swap` and `null_effect` are also available; they are fixture-gated
  (`Anomaly_ChangeCase.cpp:21-23`) and that gate is **not** widened.
- 🔁 082-04 (Δ2) **Wrong copy (must fail): `IAI.Bench.TexCorruptWrongCopy <fault>`**, replacing
  `TexCorruptWrongEncoding`.
  - Each fault is **provably** wrong on the rows it is assigned to (§R12.4). It is never required to fail
    on a row it cannot affect.

  | fault | what it does | rows it is assigned to |
  |---|---|---|
  | `chanswap` | the corruptor writes `src.bgra` (R ↔ B) | every encoding row whose image has R ≠ B (not the sRGB-grayscale row); the shared-sampler row |
  | `srgbtwice` | the colour corruptor encodes to sRGB in the shader while the target's sRGB flag stays set (double encoding) | every sRGB colour row |
  | `mipshift` | level m samples source mip m + 1 | chain (a) at minification, grazing and transitions |
  | `mipgen` | the copies for m ≥ 1 are skipped and the engine regenerates mips from mip 0 (`UpdateResourceImmediate(false)` on the output), which was revision 2's behaviour | chain (a) at minification and transitions |
  | `texelshift` | each level samples one texel off ~~+(1/W_m, 1/H_m)~~ 🔁 082-05 (A) **along U only, +(1/W_m, 0)**. The diagonal shift was an identity on a one-texel checker: `C(x+1, y+1) = C(x, y)` | chains (b) and (d) at magnification |
  | `normal` | skip the `·0.5+0.5` re-encode | the two normal rows |
  | `alpha` | the colour corruptor draws with Opacity 1 (A = 0) | the alpha rows |
  | `noclear` | skip the clear before a redraw | the `G-RD` rows |

  - ⛔ `srgb` (a colour source into a linear target) is **withdrawn**. Codex showed that a linear round trip
    can reproduce the same bytes, so it is not provably wrong (`082-03` Δ2; `D3D12RHI/Private/D3D12Texture.cpp:1407-1410`).
  - Bench-only, console-only, default off, loudly echoed, never in a client payload.
- **Same-build control pair:** two NoApply legs. They establish the noise band at the AA-off arbiter (the
  m45 precedent: 0 frames differing).
- 🔁 082-04 (Δ3) **`IAI.Bench.TexCorruptTileProbe <2|4>`**: a **fixed ×2 or ×4 tile**. Targeted fire only,
  S1 only, in no mode list and not in the auto pool. `G-BIND` alone uses it: showing a changed region needs a
  non-identity draw, and S1 ships no product mode.
- 🔁 082-04 (Δ3) **`IAI.Bench.TexCorruptStaticOffset <u> <v>`** (S2): one `drift` draw at a fixed offset,
  never redrawn. It is `G-RD-DRIFT`'s reference.
- 🔁 082-04 (Δ1) **`IAI.Bench.TexCorruptAssetSlotMid <actor>`**: see §R5.
- **Other bench levers** (as revision 2): `TexCorruptForceMissingAsset`, `TexCorruptFailStep <n>`,
  `TexCorruptForeignReplace` (sets a foreign material on the slot mid-event), `TexCorruptIdentity` (identity
  mode), `TexCorruptIdentityRedraw` (identity with a forced per-frame redraw). All under `IAI.Bench.`, with
  the same rules as above.

### R12.3 The gate table

| id | class | stage | fixture | recipe | reading / criterion | minimum count |
|---|---|---|---|---|---|---|
| **G0** | D | S1 | all | eligibility census | per target: every slot's `Raw`/`Asset`/`Resolved`/`Effective` material and Nanite routing; every texture entry (type, name, association, index, texture, class, pixel format, dimensions, 🔁 `M` and the level sizes, `resident/max` LODs, `AssetLODBias`, 🔁 `NumCinematicMipLevels`, `GetCachedLODBias`); each binding's and slot's disposition; the final reason. A disagreement with §R14's predictions is a **finding**, never a pass. | every target on F-MW, F-LYRA; every F-SYN target |
| **G-BIND** | Q | S1 | F-SYN | 🔁 `TexCorruptTileProbe 2`, and identity, on the alias, layer and dead-binding targets | (1) both build targets link with the S1 declared set (§R2.5) and no other module; (2) the alias target is refused `texture_not_parameter`; (3) on the layer target, under the tile probe, only the layer's region differs from NoApply (the global parameter of the same name in the other layer is unchanged), and identity reads G-ID PASS; (4) the dead-binding target is reported (D) | 3 applied events on the layer target |
| **G-ID** 🚨 | Q | S1 | F-SYN (+ `SM_FloorBase` on F-MW if admitted) | identity through the row's corruptor on every §R12.4 encoding row; AA-off arbiter, native order; identity leg vs `NoApply 1` | 🔁 N4: **PASS** iff max \|d\| ≤ 2 (8-bit, any channel) on every frame in the target mask; **EXACT** if 0. Reported with the row's error histogram (bins 0, 1, 2, 3–7, 8–15, ≥ 16 of per-pixel max-channel \|d\|). Every wrong copy assigned to the row (§R12.4) must read max \|d\| ≥ 16 on the same mask, or the instrument is invalid. Exact-only is rejected: a BC-interpolated or BC5 decode re-quantised to RGBA8 cannot round-trip exactly. The mip tripwire reads 0. | 3 applied events per row |
| **G-ID-M** | Q on F-SYN; D elsewhere | S1 (identity), S2 (transformed) | F-SYN geometry × chain rows; F-MW / F-LYRA as readings | G-ID's comparison at magnification, minification, grazing, mip transitions and the shared sampler; in S2, baked `uv_tile` and `uv_swap` against the reference-bug materials at the same geometry | identity: G-ID's criterion, and each assigned wrong copy ≥ 16 (`texelshift`, `mipshift`, `mipgen`, `chanswap`; §R12.4). 🔁 N1(a): the authored-chain (a) and engine-authored (c) rows are **Q**; revision 2's "pending N1" is withdrawn. Transformed: a **D** reading of the difference between the baked mode and the real UV bug, per geometry (the `tile_detail_mips` limit of §R3.4.6 shows under magnification). On host content with authored chains (Appendix C) it is a reading. | 3 applied events per geometry × chain row |
| **G-RD** | Q | S1 | F-SYN redraw inputs | 🔁 Δ3: **identity redraw only**: `TexCorruptIdentityRedraw` for ≥ 90 frames, opaque and fractional-alpha inputs | every frame within G-ID's PASS against NoApply, **and** the applied leg's frame N vs its frame 1 reads 0 differing pixels (no drift, no brightening, no alpha decay). `noclear` must read ≥ 16. | 1 leg per input |
| **G-RD-DRIFT** | Q | S2 | F-SYN redraw inputs | 🔁 Δ3: `drift` for ≥ 90 frames; for three sampled frames k (first, middle, last), a `TexCorruptStaticOffset` leg at frame k's logged `texcorrupt.uv_offset` | frame k of the `drift` leg vs the static reference within G-ID's PASS; the logged offset follows the declared schedule; `noclear` must read ≥ 16 | 3 frames per input |
| **G1** | Q | S2 | F-SYN; F-MW `SM_FloorBase` if admitted (else D) | each mode, both tick orders | ONSET: the first labelled frame is the first frame differing from NoApply (m44), except `drift`, whose onset is read from `ref_*` | 4 applied events per mode per order |
| **G2** | Q | S2 | F-SYN | NoApply | `condition_held` false and `observable` false on every labelled frame; `frames_condition_lost` counts them; `injected_frames` non-empty and `affected_frames` empty; the event stays and is not vetoed (the host still draws, so m26 reads non-zero) | 4 events |
| **G3** | Q | S1 | F-SYN; F-MW if admitted | revert, then settle | the first post-revert frame vs the pre-onset reference over the **whole frame**, AA-off native: inside the same-build control band (0 differing); every touched slot logged `restored-exact` or `restored-default` with `Raw` pointer-identical; the event's render targets, 🔁 scratch included, released | 3 events per id |
| **G4** | Q | S1 | F-SYN | exits and failures | normal revert, `FinishRun`, cancel before focus, target destroyed mid-span, level change; a forced garbage collection mid-event (the engine console `obj gc` if it is available packaged; else UNEXERCISED); `TexCorruptForeignReplace` → `left-to-game`; a re-created component → `swept`; both ids live on two actors → independent ownership and release counts; `TexCorruptFailStep 2..6` → rollback, no slot touched, bytes un-reserved, nothing leaked. After every exit the live render-target bytes return to 0. | each exit once per id |
| **G5** | Q | S2 | F-MW | m52 targeted first (held, then restoring), then m53 on a target sharing the texture; and the reverse order | refused `held_by_stuck_low_mip` by name in both m52 states; in reverse order the auto-pool m52 pick refuses the shared texture. The targeted-m52 bypass is stated, not tested as isolation. | 1 per state |
| **G6** | Q (refusals), D (Nanite admit) | S1 (Δ1), S2 | F-SYN; F-MW modular kit | Nanite and runtime-material paths | `nanite_override` refused on the F-SYN target; `host_mid` refused on the Bot (`where=override`) and, 🔁 Δ1, on the F-SYN asset-slot MID (`where=asset_slot`); a Nanite target without an override, if it reaches APPLY, is labelled with `observability_measured` false (the m50 admit path). A case with no fixture is UNEXERCISED, never a pass. | 1 each |
| **G-REASON** | Q | S1–S2 | per §R6.4 | each reason's producer | 🔁 the producer yields **that** reason by name, not an earlier one (the §R6.4 precedence column), in the REFUSED line and in `run_summary`; each UNEXERCISED reason stays listed | 1 each |
| **G-COLL** | D | S1 (F-MW, `SM_FloorBase` if admitted), S3 (every `G7` leg) | 🔁 N3 | applied leg vs `NoApply 2` (no allocation), same recipe, both with `IAI.Bench.TexCorruptCollateralDetail 1` | Collateral = every `UTexture2D` bound (read as in §R2) to ~~a visible renderable actor other than the target~~ 🔁 082-06b (P2-2): **every primitive component drawn on screen** (`GetLastRenderTimeOnScreen` within max(0.2 s, frame delta), the engine's own tolerance; not `GetLastRenderTime`, which counts shadow-only draws) other than the target's, through `GetUsedMaterials`. **No injection-selection policy applies**: poll radius, coverage floor, translucent-only, foliage and exclusion patterns are all ignored. The set is capped at 256 by path order and flagged if truncated, and every incompleteness is counted (`collateral_incomplete`). It is sampled at Apply, on each labelled frame and ~~2 frames after revert~~ 🔁 (P3-1) at **frame revert+2** by `GFrameCounter`, whatever the tick order, logging its endpoint. ~~per labelled frame, the number of collateral textures whose `NumResidentLODs` fell below its Apply-time value … Reported as the applied-minus-null difference per budget.~~ 🔁 082-05 (B): **paired, texture by texture.** Each sample logs every collateral texture's path and `NumResidentLODs` with the sample's frame offset from Apply (`TEXCORRUPT-COLL`). The applied and null legs are joined on (texture path, frame offset); a pair's **deficit** is `max(0, null_resident − applied_resident)`. Every texture with a positive deficit is reported by name with its levels, and the deficits are summed per budget. **Counts are never subtracted across legs**: one texture falling 13 → 12 in the null and 13 → 11 in the applied leg is a deficit of 1, which subtracted counts would hide. A texture present in one leg's set and not the other's is listed as unpaired. A truncated set is reported **incomplete**, never as clean. A positive deficit is reported as `collateral_residency_drop` for that host and budget: a purity finding for the owner (O2), never tuned away. | every applied event |
| **G-COOK** | Q | S1 | the S1 cook | 🔁 N2 (§R12.1) | M0 = M1 = M2 on the manifest outside the allowed paths; the archive hash-verified before the cook; the map gate exits 0 with the new level named; the `CB_GateLevel` / `MainWorld` chunk hashes unchanged; the new packages present; other packages reported (D) | once |
| **G7** | O (D readings) | S3 | F-MW, F-LYRA | 🔁 N3: auto-pool with both ids enabled explicitly, **three legs per fixture, at `TexCorruptMaxRtBytes` 64, 128 and 256 MiB**, same seed | attempted, applied, refused per final reason, plus the slot and binding dispositions, per budget. Per event `texcorrupt.required_bytes`, so each leg's refusals can also be re-read against the other two budgets. That re-reading is arithmetic only: the live set differs between legs, because a refused event leaves its actor eligible for later picks. `G-COLL` at each budget. The owner chooses the default from these (O2). | one census per fixture per id per budget |
| **G8** | Q | S2 | F-MW | `P-C7 v3` against a pre-m53 control pair | `labels.jsonl` field set unchanged without m53; `run_summary` adds exactly the §R10 keys; the `annotation.json` field set unchanged (`P6`); new `anomaly_subtype` values only for the new ids | 1 pair |
| **G-STR** | D → O | S3 | F-SYN, F-MW, F-LYRA | every mode with m55 on | m55 onset and `ref_*` per mode against the matched NoApply and `corrupted_texture` twins (and `solid_swap` / `null_effect` on L_ShooterGym); the evidence for amending the default modes and priors | 3 events per mode per fixture where admitted |
| **G-COST** | O | S3 | F-SYN | §R9.4 | §R9.4's readings, 🔁 including the per-mip draws and copies; acceptance is the owner's (O3) | §R9.4 |
| **G9** | D | S2 | S2 legs | `--label-pixel-gate` + `--change-oracle` | readings only; the evidence rows must be present (exit 0 on absent evidence is not accepted) | all S2 legs |
| **G10** | Q | every build | — | editor and game targets | exit 0, zero warnings, 🔁 **the S1 declared dependency set (§R2.5) and no other module** | every build |
| **G11** | Q | S1, S2 | packaged | startup read-back + G-ID + G1 | a startup line names both corruptors and the noise texture and says each resolved non-null; every (corruptor, target format) pair drew non-default content, shown by G-ID's applied legs (a default-material draw fails G-ID); the noise texture's own sampling is proven in S2 by G1's `normal_noise` row, since identity draws it at zero amplitude | covered by G-ID and G1 |
| **G-LYRA** | Q per id, for a Lyra support claim | S3 | F-LYRA | targeted and auto-pool | at least 1 successful counted event per id. A named refusal alone is a yield reading, and that id is then **unqualified on Lyra**. Pixel identity is not claimed there (TSR). | 1 applied event per id |

### R12.4 🔁 082-04 — the G-ID matrix (P2-10) and the wrong-copy proofs (Δ2)

**Encoding rows** (identity through the named corruptor):

| row | corruptor | class | source format (how made, §R3.1) | readout sampler | wrong copies that must fail |
|---|---|---|---|---|---|
| U-C1 | UV | colour | DXT1 sRGB (`TC_Default`, no alpha) | Color | `chanswap`, `srgbtwice` |
| U-C2 | UV | colour | DXT5 sRGB, fractional alpha | Color | `chanswap`, `srgbtwice`, `alpha` |
| U-C3 | UV | colour | BC7 sRGB, fractional alpha (`TC_BC7`) | Color | `chanswap`, `srgbtwice`, `alpha` |
| U-C4 | UV | colour | BGRA8 sRGB (`TC_VectorDisplacementmap`, sRGB on) | Color | `chanswap`, `srgbtwice` |
| U-C5 | UV | colour | BGRA8 sRGB from sRGB grayscale (`TC_Grayscale`, sRGB on) | Color | `srgbtwice` (R = G = B, so `chanswap` cannot fail) |
| U-D1 | UV | data | DXT1 linear (`TC_Masks`, no alpha) | Masks | `chanswap` |
| U-D2 | UV | data | DXT5 linear (`TC_Masks`, alpha) | Masks | `chanswap`, `alpha` |
| U-D3 | UV | data | BC7 linear (`TC_BC7`, sRGB off), alpha | LinearColor | `chanswap`, `alpha` |
| U-D4 | UV | data | BGRA8 linear (`TC_VectorDisplacementmap`, sRGB off) | LinearColor | `chanswap` |
| U-D5 | UV | data, one channel | BC4 (`TC_Alpha`) | Alpha | `chanswap` (the value leaves R) |
| U-D6 | UV | data, one channel | G8 (`TC_Grayscale`, sRGB off) | Grayscale | `chanswap` |
| U-N1 | UV | normal | BC5 (`TC_Normalmap`), through the UV corruptor's `.rg` re-encode | Normal | `normal`, `chanswap` |
| N-N1 | Normal | normal | BC5 (`TC_Normalmap`) | Normal | `normal`, `chanswap` |

- **UNEXERCISED and refused:** `PF_BC5` without `IsNormalMap()` (not cookable in 5.1, §R3.1).
- **Alpha half:** U-C2, U-C3, U-D2, U-D3, plus the masked target (its opacity mask reads U-C2's alpha).
- **Geometry rows** (`G-ID-M`): {magnification, minification, grazing, mip transition} × chains (a), (b),
  (c) and (d), plus the shared-sampler row. Faults assigned:
  - `texelshift`: magnification × (b), (d);
  - `mipshift`: minification, grazing and transitions × (a);
  - `mipgen`: minification and transitions × (a);
  - `chanswap`: the shared-sampler row.
  - The (c) rows are identity **Q** rows. Their `mipgen` reading is **D**, because the engine's sharpen,
    blur and alpha-coverage output cannot be proven offline (interpretation I5, §R15).
- **Redraw rows** (`G-RD`): the opaque BGRA8 linear input and the fractional-alpha sRGB DXT5 input, with
  `noclear`.

**The offline proofs, run before any leg relies on a fault.**

- `texcorrupt_fixture_images.py` computes, from the exact images it generated, each assigned fault's
  predicted per-texel error over the whole region its row reads, and records the minimum in the manifest.
- A fault is **relied on** for a row only if that minimum is **≥ 32 levels**: twice the 16-level threshold,
  which leaves room for block compression.
- If a fault a row needs falls below 32, S1 stops (§R13.1 row 12). The row is not run with a weaker
  control.

| fault | construction | predicted minimum per-texel error on its rows |
|---|---|---|
| `chanswap` | \|R − B\| ≥ 96 on every texel of every multi-channel colour and data image; one-channel images in [48, 192]; normal images with \|n.x\|, \|n.y\| ≤ 0.35 | ≥ 96; one-channel ≥ 48; normal ≥ 66 |
| `srgbtwice` | colour channels in [48, 192] | ≥ 33 (the double encode of v differs from v by at least 33 levels on [48, 192]; computed) |
| `normal` | \|n.x\|, \|n.y\| ≤ 0.35 | ≥ 83 |
| `alpha` | alpha in [0.25, 0.75], lerping colours ≥ 192 levels apart | ≥ 64 in alpha; ≥ 48 in the picture |
| `mipshift`, `mipgen` | chain (a): solid colour per mip; consecutive mips ≥ 64 apart in some channel; every mip ≥ 64 from mip 0 | ≥ 64 |
| `texelshift` | chain (b): one-texel checker, 64 / 192; chain (d): n.x = ±0.35 (encoded 83 / 172) | ~~≥ 128; normal ≥ 89~~ (withdrawn: the diagonal shift it assumed maps the checker onto itself, error 0). 🔁 082-05 (A), for the U-only shift: every texel flips parity, so **128 on every texel of (b) and 89 on every texel of (d)** — predicted, and to be proven by `texcorrupt_fixture_images.py` in 082-06 from the images it generates, before any leg relies on it |
| `noclear` | opaque input with RGB in [48, 128] (a second composite adds the source again); fractional alpha in [0.25, 0.75] (alpha decays to a²) | ≥ 48; alpha ≥ 47 |

- **The in-leg reading is the authority.** A relied-on fault that reads max |d| < 16 in the picture makes the
  instrument invalid (`G96`, §R11). It is reported and never re-tuned in the same turn.
- The offline proof is what makes the in-leg reading interpretable: a fault that is not provably wrong could
  pass or fail for reasons unrelated to the copy.

---

## R13. Stages, builds and cooks

| stage | content | builds and cooks | gates | stop if |
|---|---|---|---|---|
| **S0** | revisions 2 and 3 ruled, including N1–N4 | — | — | — |
| **S1** — route B core, identity only | 🔁 082-04: §R13.1 in full | **2 builds (editor + game), 1 cook** (plugin assets + F-SYN), and **2 authoring runs** (the plugin's corruptor script; the F-SYN tool) | G0, G-BIND, G-ID, G-ID-M (identity), G-RD, G3, G4, G6 (Δ1 rows), G-REASON (S1 rows), G10, G11, G-COOK; G-COLL (D) | §R13.1's stop list, every row |
| **S2** — every mode, both ids | the eight modes; the auto-pool mode draw (`R-SEED`, v1 §7.2); `drift` only if G-RD passes, then qualified by 🔁 G-RD-DRIFT | **2 builds, 0 cooks**, if S1's graphs carry every mode's parameters (they are specified to) | G1, G2, G5, G6, G8, G9, G-ID-M (transformed, D), 🔁 G-RD-DRIFT, G-REASON (S2 rows), G10 | a mode fails ONSET; G2's falsifier cannot fire; a G-RD-DRIFT failure defers `drift` (D5); any §R11 STOP row |
| **S3** — measurement | yield, strength, cost, Lyra, 🔁 collateral residency | 0–1 bench builds + a Lyra worktree build (shared-tree rule 5) | 🔁 G7 at 64 / 128 / 256 MiB, G-STR, G-COST, G-LYRA, G-COLL | a restore, lifecycle or rollback defect found on host content (a G3/G4 shape) stops S3. Otherwise S3 produces numbers: an id with no counted application on a fixture is recorded **unqualified** there, and default-on, the budget and cost acceptance wait for the owner (O1–O3). |
| **S4** — docs and merge | client readme (the prior, the refusals, the limits), architecture, checklist, catalogue; comment strip | 1 build pair | G10 | — |

- **Counts:** about **8 bench builds** (2 per stage) plus 1–2 Lyra builds; **1 cook**. A second cook is
  needed only if an S1 graph lacks a parameter S2 needs.
- **"One cook" means one content iteration.** Lyra and each office host build and cook for themselves
  (Codex Q7). The office procedure still gains the startup read-back line (v1 §3.3).
- **Estimate: 5 implementation sessions.** S1 is two: it carries the fixture level and its checks, the
  transaction, the per-mip copy and the levers.
- 🔁 082-04: **N1 and N2 are ruled, so S1 is designable.** Revision 2's "S1 cannot start before N1 and N2 are
  ruled" is discharged. Whether S1 starts is chat's decision after Codex's delta check #2.

### R13.1 🔁 082-04 — S1 in full

**Scope.** Route B core, identity only, plus one bench-only tile probe for `G-BIND` (Δ3).

- **Plugin code:**
  - the §R6 decision tree in revision 3's order;
  - the §R7.4 transaction;
  - the §R3.4 per-mip draw and copy, with the tripwire;
  - §R8's restore;
  - the §R4 residency and runtime-bias checks;
  - the §R3.2 budget (128 MiB) with scratch accounting;
  - the §R5 chain predicate;
  - `G-COLL`'s probe (§R12.3);
  - the split prewarm list and the warm-draw phase (§R9);
  - both ids registered, identity and the tile probe only, nothing in the auto pool;
  - every §R12.2 bench lever S1 uses.
- **`AnomalyInjector.Build.cs`:** private `RenderCore` and `RHI` (N1(a)). `architecture.md` and `CLAUDE.md`
  take the invariant wording of §R2.5.
- **Plugin content:** the two corruptors (with `SrcMip`, `AutomaticViewMipBias` off and the `Dbg*` scalars)
  and the noise normal, made by a plugin authoring script; hard references on the subsystem CDO (v1 §3.3).
- **CaptureBench (N2):** the three tools of §R12.1, and the fixture itself.
- **Docs:** the PRE-DELIVERY-CHECKLIST line (§R12.1).

**Order.** Each step must pass before the next starts.

1. **Preconditions:** the branch equals origin; the staged exe and container hashes are recorded against
   `_binary_baselines\README.md`; the disk floor (runbook §8.6 step 0); **M0**, the source manifest.
2. **Code**, the comment strip, then the **editor build** and the **game build** (`G10`).
3. **Authoring:**
   - the plugin's corruptor script;
   - the fixture images and their offline proofs;
   - the fixture tool;
   - **M1**.
4. **Archive** the staged container and exe, hash-verified at the destination, and take the IoStore listing
   of the archive.
5. **The cook** (runbook §8.6, on the fresh editor binaries, `G47`), with
   `/Game/CaptureBenchTexCorrupt/CB_TexCorruptLevel` added to `-map=`. Then stage, the A44 scan, **M2**, and
   `G-COOK`.
6. **Legs:** the `G0` census; the luma gate on the fixture level; `G11`; `G-BIND`; `G-ID`; `G-ID-M`
   (identity); `G-RD`; `G3`; `G4`; `G6` (the Δ1 rows); `G-REASON` (S1 rows); `G-COLL` (D).

**The stop list: every S1 qualification failure stops S1.** §R11 applies: nothing is re-run until it passes,
no tolerance moves, and no fallback is taken.

| # | failure |
|---|---|
| 1 | `G10`: a build exits non-zero, emits a warning, or links a module outside the S1 declared set (§R2.5) |
| 2 | `G-BIND`: (1) the link check fails; (2) the alias target is not refused `texture_not_parameter`; (3) the tile probe changes anything outside the layer's region, or identity on the layer target fails G-ID; or fewer than 3 applied events on the layer target |
| 3 | `G-ID`: any row exceeds max \|d\| 2 on any frame; any assigned wrong copy reads below 16 (the instrument is invalid, `G96`); any row with fewer than 3 applied events; the alpha half UNEXERCISED |
| 4 | `G-ID-M`, the F-SYN Q rows: identity above 2 at any geometry, including the authored-chain and engine-authored chain rows; `mipgen`, `mipshift`, `texelshift` or `chanswap` below 16 where assigned |
| 5 | the mip tripwire `texcorrupt_rt_mip_mismatch` non-zero on any leg |
| 6 | `G-RD`: any redraw frame outside G-ID's PASS, frame N differing from frame 1, or `noclear` not failing |
| 7 | `G3`: a post-revert frame outside the control band, a slot not restored exactly, or a render target (scratch included) not released |
| 8 | `G4`: any exit or rollback path leaves a slot touched, bytes reserved or a render target alive, or ownership crosses between the ids |
| 9 | `G-REASON` and `G6` (Δ1): an S1 producer yields no refusal, or a reason other than its §R6.4 row (the precedence is wrong) |
| 10 | `G11`: the startup read-back does not name both corruptors and the noise normal as resolved |
| 11 | `G-COOK`: M1 or M2 differs from M0 outside the allowed paths; the archive was not verified before the cook; the map gate does not exit 0; a `CB_GateLevel` / `MainWorld` chunk hash changed; a new package is missing |
| 12 | an offline wrong-copy proof below 32 for a fault a row relies on |
| 13 | the luma gate fails on the fixture level |
| 14 | the fixture tool writes, or leaves dirty, anything outside `/Game/CaptureBenchTexCorrupt/` |

- `G0` and `G-COLL` are readings. A `G0` value that contradicts §R14 is a **finding**, reported beside the
  stop list, not a stop. A positive `G-COLL` difference is reported to the owner.
- **Builds and cook:** two builds (editor, game), one cook carrying the plugin's new assets and F-SYN, and
  two authoring runs. Revision 2 had the same build and cook counts; the authoring runs and the byte checks
  are new.

**Predicted yield in S1.** S1 claims no host yield.

- It applies identity only: on F-SYN (every encoding, chain and geometry row) and on `SM_FloorBase` if `G0`
  admits it (UV family, data class, 6.67 MiB).
- Its `G0` census tests §R14.1's per-target predictions. The only StackOBot host target predicted to reach
  APPLY is `SM_FloorBase` (UV family).
- The host-yield predictions for S3 are §R14.

---

## R14. Expected yield impact (🔁 082-04: 128 MiB, scratch counted)

**Method.** Three sources, none of them a measurement of m53:

- the 082-01 name-table scan (material chains and texture classes; v1 Appendix A);
- the 082-03 tagged-property scan (dimensions and authored mip settings; Appendix C);
- m52's banked residency readings (§R4).

🔁 082-04: the budget is now 128 MiB, and every figure includes per-mip scratch (§R3.2). Revision 3 adds
three refusals. P2-8 lets any excluded map block its slot. P1-3 refuses runtime LOD bias. Δ1 refuses a
runtime material anywhere in the chain. How much each removes is **unknown until `G0`**. These are
**predictions**, and `G0` and `G7` read the real values.

### R14.1 Per target, first failing step

| fixture target | textures (imported → cooked top) | v2 prediction (64 MiB) | 🔁 v3 prediction (128 MiB, scratch counted), UV family | 🔁 v3, normal family |
|---|---|---|---|---|
| MainWorld `SM_rock`, `SM_rock_02` | `T_rock_0x_D/N/AORM` 4096²; `T_detail_N` 2048² | `not_fully_resident`; `over_budget` if resident | **`not_fully_resident`** (measured resident 11–12 of 13 at the bench pose). If resident, `over_budget` at every budget read (325.33 MiB). If the world-aligned detail map is a constant, `texture_not_parameter` comes first. | `not_fully_resident`; if resident, `over_budget` at 128 (133.33 MiB), fits at 256 |
| MainWorld `SM_FloorBase` (most-fired m52 target) | `T_Grid_A` 1024² mask | eligible iff a parameter and resident | **eligible** iff `T_Grid_A` is a parameter, fully resident and free of runtime bias, and no other map in the slot is excluded (P2-8): **6.67 MiB** (`drift` 12.0). Data class only. | `no_normal_map` |
| MainWorld `SKM_Bot` | `T_Bot_*` 4096²; `T_Eyes_Atlas` 2048² (`LeaveExistingMips`) | `host_mid` | **`host_mid`** (`where=override`, S3 before any texture step). Its authored eye chain would now be carried (N1(a)), but S3 comes first. | same |
| MainWorld modular kit (Nanite) | `T_SandTileabe_BC`, `T_ConcreteTileable_N`, `T_Metal_Painted_N` 4096² | `over_budget` | `over_budget` if a set holds two 4096² maps (~~≥ 213.33~~ 🔁 082-05 **≥ 192 MiB**: 192 for the same class, 213.33 across two classes); a set with one 4096² map fits (106.67 MiB). Residency is likely to refuse first. | one 4096² normal map fits (106.67 MiB) if resident |
| MainWorld `SM_RockFlats_*` | `T_RockTileable_BC` 2048², `T_SandTileabe_BC` 4096² | `over_budget` if the sand map is active | `over_budget` at 128 (133.33 MiB) if the sand map is active, fits at 256; 26.67 MiB if not | `no_normal_map` |
| MainWorld `SM_GenericPlane` | translucent | `no_eligible_slot` | `no_eligible_slot` | same |
| `CB_GateLevel` | none | `no_textures` | `no_textures` | same |
| Lyra `Cube*` | `T_Paint_Diffuse` 2048², `T_Paint_Normal` 2048², `T_Paint_Glossiness` 2048² with LODBias 1 (→ 1024²), `T_Paint_Opacity` 2048² | fits (48); `over_budget` with the opacity map | **fits: 60.0 MiB** with three maps, **81.33 MiB** with the opacity map. **`drift` fits at 128** (108.0 MiB) without the opacity map; with it, 150.67 MiB, which needs 256. Residency plausible; virtual-texture status unknown. | **fits (26.67 MiB)** |
| Lyra weapons (Nanite) | `T_Rifle_*` 4096² with LODBias 1 (→ 2048²) × 4 | `over_budget` (85.33) | **fits at 128 (96.0 MiB)**, refused at 64. `drift` 181.33 MiB needs 256. | fits (26.67 MiB); observability unmeasured (Nanite, `G134`) |
| Lyra `SKM_Manny/Quinn` | `T_Manny_01_*` 8192² with `MaxTextureSize` 4096 (→ 4096²), `Sharpen1`/`Sharpen2` | `over_budget`; `host_mid` first if MIDs; sharpened chains (N1) | `host_mid` first if their slots hold MIDs; otherwise `over_budget` at every budget read (≥ 298.67 MiB). The sharpened chains are carried by N1(a) and are no longer a support restriction. | `host_mid` first if their slots hold MIDs; otherwise a single 4096² normal map fits (106.67 MiB), and a set with two does not (~~≥ 213.33~~ 🔁 082-05 **192 MiB**, both normal maps being one class) |

### R14.2 What that adds up to

- **StackOBot MainWorld:** at most **1 of the 5** measured non-Nanite targets (`SM_FloorBase`, UV only,
  data class only) is predicted to produce an event, as in revision 2. **No StackOBot host target exercises
  the colour or normal encodings**, which is why G-ID needs F-SYN (N2).
- **Lyra:** the cubes (UV static modes, `drift`, normal) and, 🔁 new at 128 MiB, the weapons' UV family
  besides their normal family.
- **`drift`** fits on `SM_FloorBase` (12.0 MiB) and on the Lyra cubes without the opacity map (108.0 MiB).
  Everything else needs 256 MiB or more.
- **Which refusal binds, and where:**
  - **residency** binds first on large, streamed textures at the bench poses (the rocks);
  - **the budget** binds on sets with two or more 4096² maps and on most `drift` sets;
  - **host MID** removes the StackOBot hero, and possibly Lyra's characters;
  - **atomic coherence** removes slots that mix parameters with constants, virtual maps or excluded groups
    (🔁 P2-8 widens this). The count is unknown until `G0`; the rocks' world-aligned detail map is the likely
    case;
  - 🔁 **runtime LOD bias** removes cinematic-mip textures and device-profile-clamped ones. The count is
    unknown until `G0` (the default `NumCinematicMipLevels` is 0).

### R14.3 Budget sensitivity (N3), arithmetic only, scratch included

| budget | fits | still refused |
|---|---|---|
| 64 MiB | `SM_FloorBase` UV (6.67) and `drift` (12.0); cube UV, three maps (60.0); cube and weapon normal (26.67) | cube UV with opacity (81.33); weapon UV (96.0); any 4096² map (≥ 106.67); cube `drift` (108.0) |
| **128 MiB (default)** | adds: cube UV with opacity (81.33); weapon UV (96.0); a single 4096² map (106.67); cube `drift` (108.0) | two 4096² maps (~~≥ 213.33~~ 🔁 082-05 **192** same class, 213.33 across classes); `SM_RockFlats` with the sand map (133.33); rock sets; cube `drift` with opacity (150.67); weapon `drift` (181.33) |
| 256 MiB | adds: `SM_RockFlats` (133.33); cube `drift` with opacity (150.67); weapon `drift` (181.33); two 4096² maps (~~213.33~~ 🔁 082-05 **192** same class, 213.33 across classes) | the rock UV set (325.33); the character sets (≥ 298.67) |

Raising the budget cannot recover the rocks at the bench poses, because residency refuses them first. That
lever is a prefetch design, deferred by ruling (§R4). `G7` reads all three budgets, and `G-COLL` says
whether the higher ones cost the host's other textures their mips.

---

## R15. NEEDS-DECISION

- ✅ **N1–N4 are ruled** by 082-04: N1 = (a), N2 approved additive-only, N3 = 128 MiB, N4 = 2 levels / 16.
- 🔁 **Interpretations Code made inside the ruling**, stated so chat can overrule any of them:
  - **I1:** `G-COLL`'s null is a **no-allocation** NoApply (`NoApply 2`), because the ruled NoApply
    allocates the same bytes as the applied leg (§R12.2).
  - **I2:** the output's mip 0 is drawn straight into the output, and only mips ≥ 1 go through scratch and a
    copy. This keeps scratch at about a quarter of the chain; the alternative, scratch for every level, would
    refuse every 4096² map at 128 MiB (170.67 MiB). "Per-mip copy" is read as "every mip made from the
    source's same mip, none regenerated" (§R3.4.3).
  - **I3:** Δ1's predicate adds `!RF_WasLoaded` to the two ruled tests (MID, transient outer). It is
    stricter (§R5).
  - **I4:** the cooked-chunk comparison is **Q** for `CB_GateLevel` and `MainWorld` only, and **D** for the
    other pre-existing packages, because the archived container predates S1. The source manifest is the
    authority for "unchanged by N2" (§R12.1).
  - **I5:** `mipgen` and `mipshift` are relied on only on the authored per-mip-colour chain, where they are
    provable. On the engine-authored chains they are readings (§R12.4).
- **Later owner decisions:**
  - **O1** default modes and pool membership (`G7` + `G-STR`);
  - **O2** the budget value, from `G7`'s three budgets and `G-COLL`;
  - **O3** acceptance of the cold first fire and of `drift`'s cost (`G-COST`);
  - **O4** a prefetch design, only if `G7` shows residency is the binding refusal.
  - Before any default-on decision, yield is also read on the office hosts as an eligibility census: counts
    only, run at the office, with the owner transcribing the readings (chat's 082-04 product rule).
- **Nothing on this revision's side blocks S1.** Whether S1 starts is chat's call after Codex's delta check #2.

---

## R16. What this revision does not claim

- ⛔ **No yield, strength or cost number.** §R14 is derived from offline scans and banked m52 readings;
  `G0` / `G7` / `G-STR` / `G-COST` produce the real ones.
- ⛔ **No claim that the encodings are correct** (`G-ID`), that the per-mip copy reproduces a chain
  (`G-ID-M`), or that the S1 dependency set links (`G-BIND`, `G10`).
- 🔁 **N1(a) is a design, not a measurement.** "Authored chains are carried" is what `G-ID-M`'s
  authored-chain rows must show, with `mipgen` failing beside it.
- ⛔ **No claim that the offline lever proofs hold in the picture.** The in-leg reading is the authority
  (§R12.4).
- ⛔ **No perceptibility claim.**
- ⛔ **No incidence claim about the office hosts' content.**
- ⛔ **Nothing here changes** `m51` (held at `53bf725`), `master`, any tag, any cooked container,
  CaptureBench (N2's tools are written in S1) or `ToCodex\`.

---

## Appendix C — the 082-03 tagged-property scan (EVIDENCE, not a measurement)

**Method.**

- For every `.uasset` under the content roots (StackOBot `Content`; Lyra `Content`, `ShooterCore`,
  `ShooterMaps`, `LyraExampleContent`), read the first 4 MiB.
- Parse the package summary's name map, then find tagged-property byte patterns: `ImportedSize`
  (`IntPoint`), `CompressionSettings` / `LODGroup` / `MipGenSettings` (enum), `MaxTextureSize` / `LODBias`
  (int), and `SRGB` / `bDoScaleMipsForAlphaCoverage` / `CompressionNoAlpha` (bool).
- The scripts are outside the repo: `C:\ClaudeTemp\m53scan\texprops.py` and `mipcensus.py`.

**What it can and cannot say.**

- These are **editor** assets, so they carry import and cook **settings**. The cooked top mip follows from
  `ImportedSize`, reduced by `MaxTextureSize` and by cook-time `LODBias` stripping
  (`TextureLODSettings.cpp:176-186`; `TextureDerivedData.cpp:2435`).
- **Default values are not serialised**, so an absent key means the default. Checked both ways: `SRGB: 0`
  appears on every normal map (where the editor turned it off) and is absent on the diffuse maps (default
  on).
- Device-profile group limits (`MaxLODSize`) are not applied. Placed-component overrides, Nanite status
  and virtual-texture status at runtime are not read.

**Dimensions of the targets' textures** (imported size; `LODBias` and `MaxTextureSize` where set):

| fixture | texture | imported | notes |
|---|---|---|---|
| StackOBot | `T_rock_01_D/N/AORM`, `T_rock_02_D/N/AORM` | 4096² | |
| StackOBot | `T_detail_N`, `T_grunge_mask`, `T_black`, `T_white`, `T_default_AORM` | 2048² | `T_default_normal` 2048², `MaxTextureSize` 512 |
| StackOBot | `T_SandTileabe_BC`, `T_ConcreteTileable_N`, `T_Metal_Painted_N` | 4096² | |
| StackOBot | `T_Grunge_A`, `T_ConcreteTileable_M`, `T_Grid_A` | 1024² | |
| StackOBot | `T_Ribbing_A`, `T_Ribbing_N` | 256² | `LODBias` 3 |
| StackOBot | `Wind` | 2048² | `TC_VectorDisplacementmap` |
| StackOBot | `T_WhitePixel` | 1×1 | |
| StackOBot | `T_Bot_Albedo/Normal/M_R_AO/Masks` | 4096² | |
| StackOBot | `T_Eyes_Atlas` | 2048² | `TMGS_LeaveExistingMips` |
| StackOBot | `T_RockTileable_BC` / `T_RockTint` | 2048² / 1×256 | |
| Lyra | `T_Paint_Diffuse`, `T_Paint_Normal`, `T_Paint_Opacity` | 2048² | |
| Lyra | `T_Paint_Glossiness` | 2048² | `LODBias` 1 |
| Lyra | `T_Rifle_D/Combined_N/AORM/Masks`, `T_Pistol_D/N` | 4096² | `LODBias` 1 |
| Lyra | `T_Weapon_D/N/AORM` | 512² | `MaxTextureSize` 32 |
| Lyra | `T_Manny_01_D/N/MSK` | 8192² | `MaxTextureSize` 4096; `T_Manny_01_D` `TMGS_Sharpen1` |
| Lyra | `T_Detail_Normal`, `T_OrangePeel_N` | 1024² | |

**Mip-generation census, all textures with an `ImportedSize` tag:**

| fixture | textures | `MipGenSettings` | alpha-coverage scaled | non-box chains |
|---|---|---|---|---|
| StackOBot | 131 | default 118 · `NoMipmaps` 5 · `LeaveExistingMips` 7 · `Sharpen10` 1 | 0 | **8** (`T_Eyes_Atlas`, `T_lens_flare_03`, 6 hash-named) |
| Lyra | 324 | default 179 · `NoMipmaps` 122 · `Sharpen1` 4 · `Sharpen2` 9 · `Sharpen4` 3 · `Sharpen7` 1 · `Blur1` 2 · `Blur5` 1 · `LeaveExistingMips` 2 · `SimpleAverage` 1 | 0 | **22** (every `T_Manny_0x_D/MSK1` and `T_Quinn_0x_*` map among them) |

**Size histogram (longest side):** StackOBot 4096: 44 · 2048: 55 · 1024: 8 · ≤ 720: 24. Lyra 8192: 27 ·
4096: 40 · 2048: 23 · 1024: 33 · ≤ 512: 201.

---
---

<details>
<summary><b>⛔ SUPERSEDED 2026-09-27 — revision 1, the 082-01 plan at <code>946c1bf</code>, kept verbatim. See
<code>_reviews/082-02-codex-m53-design-review.md</code> and
<code>_reviews/082-03-chat-ruling-codex-m53-design-review.md</code>. Its DESIGN is withdrawn and may not be
cited: route A and its fallback, the §2.3/§2.4 refusal tables, the §3.4 snapshot rule, the §4.2/§4.3 restore,
the §5.3 strength classes, the §6 gates, the §8 costs, the §9 stages and the §10 D1–D7 as written. Only the
SOURCE FACTS listed in §R0.2 above remain citable, as <code>v1 §x</code>.</b></summary>

# m53 — UV / normal-map texture corruption — PRE-DECLARED DESIGN AND GATES

**Written 2026-09-26, session 082-01, branch `feat/m53-uv-normal-corruption` off `master` `4283fc8`.**
**PLAN ONLY. No plugin source changed, no build, no cook, no game or editor launch, no bench leg, no tag.**
The only thing executed was a read-only, offline name-table scan of the StackOBot and Lyra content
trees (Appendix A). It opens no engine and touches no binary. The script lives outside the repo, and
its method is recorded here so it can be re-run.

Engine read against the canonical engine `D:\UESource\UnrealEngine`, **UE 5.1.1** (`Engine/Build/Build.version`).
Every engine claim carries `path:line`, relative to `Engine/Source` unless it is marked `[Shaders]`
(= `Engine/Shaders`). Path abbreviations:

- `Private/…`, `Classes/…` and `Public/…` mean `Runtime/Engine/Private/…` etc.
- A bare engine file name (`Material.cpp`, `MaterialShared.cpp`, `MaterialInstance.cpp`, …) lives
  under `Runtime/Engine/Private/Materials/`, or `Runtime/Engine/Private/` for texture and component
  files.
- Other modules are named explicitly (`RHI/…`, `Renderer/…`, `RenderCore/…`, `Developer/…`).

Every plugin claim carries `file:line` against `master` `4283fc8`.

Anything measured later that contradicts this file is a **finding**, and the finding wins. It is
recorded as an amendment, never folded in silently.

---

## 0. What this file is for, and the shape it starts from

m53 is the owner's next client anomaly class: **UV corruption** and **normal-map corruption**. Today
texture corruption covers only `missing_texture` (lit checker) and `corrupted_texture` (lit magenta).
Both replace the object's whole material with a constant look (`Anomaly_CorruptedTexture.cpp:112-124`).

**The shape agreed with the owner on 2026-09-22, quoted as the starting design:**

- Reuse the `corrupted_texture` material takeover, with two plugin materials, `M_CorruptedUV` and
  `M_CorruptedNormal`.
- Pull the host's own textures from the target's material with `GetUsedTextures`: the base colour is
  the sRGB texture, the normal map is the `TC_Normalmap` one. Create one MID per target.
- UV modes: tile, drift, U↔V swap, lerp to TexCoord[1]. Normal modes: green-channel flip, invert,
  drop (flat), noise.
- Modes are written as additive label keys, with no schema break. One cook covers both materials.
- Targets are opaque and single-sided only; everything else is refused.
- Mesh-level UV rewrite is rejected, because render data is shared and CPU vertex data is stripped.

This file answers the eight questions in brief 082-01 against source. **It also reports one finding
that the source and the content contradict in the agreed shape (§1).** That finding is the first
NEEDS-DECISION and it decides which of the two designs below is built. Both designs are specified in
full, so the ruling can be made on this file alone.

---

## 1. 🚨 THE HEADLINE FINDING — the takeover does not corrupt the host's material, it REPLACES it with a reconstruction

### 1.1 What the takeover actually draws

A takeover MID has **our** material as its parent. The MID reuses the parent's shader map and never
compiles (`Private/Materials/MaterialInstance.cpp:1695-1696`, `:238-244`, `:2122-2124`). So every
pixel of the target is shaded by **our** graph. All the takeover can copy from the host is the texture
**objects** it is handed. It cannot copy the host's material **graph**, because in a cooked build the
graph is stripped:

- `GetTexturesInPropertyChain`, the only API that says which texture feeds BaseColor or Normal, is
  `#if WITH_EDITOR` (`Classes/Materials/MaterialInterface.h:766-784`,
  `Private/Materials/Material.cpp:5647-5687`).
- The expression graph lives in `UMaterialEditorOnlyData` (`Classes/Materials/Material.h:296`,
  `:304-305`, `:319-320`, `:381-382`), which is stripped from cooked builds.
- **Sampler type is not stored anywhere at runtime** (`Public/MaterialTypes.h:393-438`;
  `MaterialUniformExpressions.h:100-103`).

⇒ Whatever the host graph did with its textures is **lost**: tint and colour parameters, vertex colour,
world-aligned or triplanar projection, packed ORM, detail layers, masks, parallax, material-function
logic. Our graph samples the two textures on UV0 with fixed lighting constants.

### 1.2 How often that matters — the content evidence (Appendix A; EVIDENCE, not a measurement)

| fixture | target family (measured target assets, m52 §3.2/3.3) | host material chain | what the takeover would lose |
|---|---|---|---|
| StackOBot MainWorld | 6 modular targets (`SM_Ramp`, `SM_SpawnPad_Base`, `SM_Modules_Platform`, `SM_Elevator`, `SM_Fan_Frame`, `SM_Modular_WallDoor`) + `SM_PressurePlate_Frame`, `SM_GratIng` | `MI_Carbon/Metal/Plastic/Glow` → `M_TileMaster` → `ML_Base` → **`MF_WorldSpaceUV`**, **`WorldAlignedTexture`**, vertex colour, RVT | **World-space projection, tint parameters, vertex-colour blending.** The only sRGB-class candidates the scan reaches are `T_SandTileabe_BC` and `T_WhitePixel`, so "the sRGB texture" is either a sand overlay or a 1-pixel constant. |
| StackOBot MainWorld | `SM_rock`, `SM_rock_02` (measured non-Nanite targets: m52 080-03 banked `target_pixels` ~16k–25k) | `MI_rock_0x` → `M_Master_kit_detail_Kit_rock` | Detail normal, AORM pack, **`WorldAlignedTexture`** detail. Closest to "texture on UV0" of anything on the fixture. |
| StackOBot MainWorld | `SM_FloorBase` (the most-fired measured target, m52 080-03) | `MI_GridObjects` → `M_Grid` | **Only texture is a mask (`T_Grid_A`).** No base colour, no normal. The takeover has nothing to pull. |
| StackOBot MainWorld | `SKM_Bot`, `SM_Bush`, `SM_Tree`, `SM_GenericPlane` | `BLEND_Masked` + `TwoSided` / translucent | Refused by the agreed opaque-single-sided rule. **The hero character is not eligible.** |
| Lyra `L_ShooterGym` | `Cube*` (m52 080-04's Lyra targets, `T_Paint_Normal`) | `MI_MS_*` → **`M_MS_Surface_Material_TriPlanar`** → `MF_WorldGrid`, `WorldAlignedTextureMip` | **Triplanar projection and the colour tint** (Blue/Orange/Red instances share `T_Paint_Diffuse`). |
| Lyra | `SM_Rifle`, `SM_Pistol`, `SM_Shotgun`, `SM_grenade` | `MI_Weapon_*` → `M_Weapon` | AORM + mask packs. Closest to "texture on UV0". These meshes carry the Nanite signature (CLAUDE.md 072 block), so the mask cannot measure them (`G134`). |
| Lyra | `SKM_Manny`, `SKM_Quinn` | `MI_*` → `M_Mannequin` (`BLEND_Masked` token, clear coat) | Refused by the agreed opaque rule. |

⚠ **Limits of the scan, stated with it** (the `m52_texture_sharing_scan.py` weaknesses, Appendix A):

- It reads asset **defaults**, not what a placed component overrides.
- A token in a name table is evidence of a reference, not proof of what a shader samples.
- 🔻 Two of the scan's own classifiers were **proven blind**:
  - `VirtualTextureStreaming` appears in 127 of 134 StackOBot texture name tables, but m52 measured only
    **2 of 188** loaded textures as virtual at runtime (080-04 §5).
  - The `SRGB` token is present on every texture, **including `T_rock_01_D`, a diffuse map**.
  - Both are registry tag names, not serialised values. **VT status and sRGB can only be read at
    runtime**, and the plan reads them there (§2).
  - The normal-map classifier is not blind: `TC_Normalmap` / normal LOD groups and the `_N` / `_Normal`
    name heuristic agree on **31 of 32** StackOBot normal maps. One texture was flagged by only one of
    the two signals.

### 1.3 Why this is a finding and not a cosmetic

**(a) The label claims a UV (or normal) bug, and the pixels show a material replacement plus that bug.**
On the modular kit and the Lyra gym cubes, the reconstruction alone changes colour, projection and
specular response. Only then does the named mode add its own change.

**(b) It is a shortcut feature for the client's model.** A detector trained on these positives can
learn "the object's colour/material changed" instead of "the texture is stretched / the lighting is
inside-out". It would then **miss** a real UV bug, where the colour is unchanged. The failure runs in
the direction the dataset exists to prevent.

**(c) The weak modes would be dominated by the reconstruction.** `normal_flat` and a small `drift`
change little by themselves. m55 would measure mostly the reconstruction, and the number would be read
as the mode.

**(d) The agreed "opaque, single-sided only" rule is a consequence of the takeover, not a property of
the anomaly.** It is needed because our material would otherwise change the target's blend mode and
culling. It removes the StackOBot hero (`SKM_Bot`) and the Lyra characters from the eligible set.

### 1.4 The alternative the source supports — HOST-PRESERVING, TEXTURE-SPACE corruption (route **B**)

Keep the host's material. Corrupt only the **texture** it samples.

1. For each eligible slot, create a MID whose parent is **the slot's own material**, not ours.
   - A MID can parent a `UMaterial` or a `UMaterialInstanceConstant`. **A MID cannot parent another
     MID — it renders the default material** (`MaterialInstance.cpp:3076-3083`, `:779-789`).
   - A slot that already holds a runtime MID (the StackOBot Bot does, `G46`) is therefore **cloned**:
     `UMaterialInstanceDynamic::Create(HostMID->Parent, Outer)` + `CopyParameterOverrides(HostMID)`
     (`Private/Materials/MaterialInstanceDynamic.cpp:484-503`, cooked-available).
2. Find which texture **parameters** of that material resolve to the textures the slot actually uses.
   - `GetAllTextureParameterInfo` + `GetTextureParameterValue` both work in a cooked build
     (`MaterialInterface.h:568`, `:802`; `MaterialInterface.cpp:1007-1010`, `:828-837`).
   - The result is intersected with `GetUsedTextures`, because the parameter list includes switched-off
     parameters (`MaterialCachedData.cpp:263-306` vs `Material.cpp:1113-1119`).
3. For each texture to corrupt, draw a plugin **corruptor** material into a `UTextureRenderTarget2D`
   **once, at Apply**.
   - The corruptor samples the host texture with the mode's transform: tile / swap / scramble / drift
     for UV, green-flip / invert / flat / noise for normal. §3.2 has the details and citations.
4. Set the MID's texture parameter to the render target and put the MID on the slot.
   - The host graph then shades the object exactly as before: its tint, its projection, its ORM, its
     blend mode, its two-sidedness, its usage flags, its Nanite path.
   - **Only the texels the graph reads are corrupted.**

What route B buys, each point against source:

- **Fidelity.** Everything but the corrupted texture is the host's. **The null twin becomes a real
  null**: an identity corruptor reproduces the original texture, up to 8-bit re-quantisation. That is a
  gate that can pass or fail (§6 `G-ID`), which the takeover cannot offer.
- **No new scene shaders or PSOs.** A MID uses its parent's shader map (`MaterialInstance.cpp:1695-1696`).
  The PSO is keyed by shaders and render state, and both are the host's. In a cooked 5.1 build PSO
  precaching is **off** (`RHI/Private/PipelineStateCache.cpp:103-110`, `:2131-2139`), so every *new*
  material × vertex-factory pair builds its PSO on first draw (`:2067-2094`).
  - Under the takeover that is one first-draw hitch per VF family per session, in the labelled
    window's first frame.
  - Under B the only new PSO is the corruptor's, drawn off-scene into a render target at Apply.
- **No usage-flag hazard in the scene.** Usage flags are read from the root `UMaterial`
  (`MaterialInstance.cpp:1432-1438`); a missing one swaps in the default material in a cooked game
  (`Material.cpp:1792-1819`). That is the defect class that already bit this project on Concorde
  (`G49`, `G157`, `set_material_usage_flags.py:15-38`). Under B the host material is already drawing
  that component, so its flags are correct by construction.
- **Blend mode and two-sidedness are preserved**, so the opaque-single-sided restriction is
  unnecessary. B refuses only translucent-only targets, which the picker already excludes
  (`AnomalyViewport.cpp:690-718`).
- **A UV mode corrupts every map coherently.** Base colour, normal and ORM all shift together, which is
  what a real UV bug looks like. The takeover can only move the two maps it reconstructs.

What route B costs, each point stated rather than discovered:

- **A texture must be a PARAMETER to be overridable.** A host graph that samples a hard-coded texture
  cannot be touched; that texture is refused (`texture_not_parameter`). Content evidence:
  - StackOBot's rocks, modular kit and Lyra's weapons and gym cubes are all MIC chains, which override
    parameters (Appendix A).
  - `M_Rock` on `SM_RockFlats_*` is a bare `UMaterial`, and its parameters are not established offline.
- **`uv_channel1` (lerp to TexCoord[1]) cannot be expressed in texture space.** It is a mesh-UV
  property. It is replaced by **`uv_scramble`**: a K×K cell permutation of the texture, the
  texture-space analogue of "wrong UV islands". The takeover's `channel1` is kept on the table only as
  an optional route-A mode (§10 `D2`).
- **Render-target memory:** one RGBA8 target + mips per corrupted texture (§8).
- **Snapshot resolution:** a render target is drawn at Apply from the texture's then-resident top mip,
  so a camera that later approaches sees the snapshot's resolution (§3.4, §8).
- **Nanite override materials:** a MID whose parent has a Nanite override is replaced by that override
  on Nanite meshes, and the MID's parameters are ignored (`Components/StaticMeshComponent.cpp:2670-2675`,
  `MaterialInstance.cpp:1748-1758`). Refused `nanite_override`.
- **Frozen host animation:** a cloned host MID stops following later parameter animation the game
  writes to its own MID (a hit flash, for example) during the event. Recorded as a named limit;
  `IAI.Anomaly.TexCorruptCloneHostMids` can refuse such slots instead.
- **More moving parts than the takeover:** render targets, their formats, their release, and a
  per-frame redraw for `drift`.

### 1.5 Recommendation (`D1`, chat and owner rule)

**Build route B for both families. Keep the agreed shape's intent — pull the host's own textures, two
plugin materials, one cook, one MID per target, additive keys — and change only where the corruption
happens.** Specifically:

- The two plugin materials become **render-target corruptors**, `M_CorruptTex_UV` and
  `M_CorruptTex_Normal`, instead of scene materials.
- The opaque-single-sided rule relaxes to "not translucent".
- `uv_channel1` becomes `uv_scramble`.

Stage 1 (§9) is built so the decision stays falsifiable. Its first gate is `G-ID`, the identity
round-trip on the fixtures. **If route B cannot reproduce the host picture within the null twin's
band, Stage 1 stops, and route A (the takeover, specified in full in §3.1) is the fallback.** Route A
then carries the reconstruction disclosure of §5.4.

---

## 2. Q1 — Texture discovery

### 2.1 How reliable `GetUsedTextures` is in a cooked build

| question | answer | source |
|---|---|---|
| Data source | The compiled **uniform expression set** of the active quality and feature level's shader map. Each slot is resolved through the leaf instance, so MIC and MID overrides are honoured. | `Material.cpp:1069-1158` (`:1103`, `:1113-1119`); `MaterialInstance.cpp:1053-1119` (`:1110`, `:1034`); `MaterialShared.cpp:926-941` |
| Cooked? | **Yes.** `UniformExpressionSet` is a plain `LAYOUT_FIELD`, its neighbours are the editor-only ones, and shader maps are serialised inline in cooked packages. | `MaterialShared.h:732` vs `:738-751`; `Material.cpp:720-797` |
| Parameter override chain | Followed. `GetTextureParameterValue` → `UMaterialInstance::GetParameterValue` walks `FMaterialInheritanceChain` and reads each instance's `TextureParameterValues`. | `MaterialUniformExpressions.cpp:1477-1483`; `MaterialInterface.cpp:828-837`; `MaterialInstance.cpp:935-978` |
| Other quality / feature levels | Only what was cooked **and** loaded for the running platform (`r.DiscardUnusedQuality`, active QL only). | `Material.cpp:841`, `:848-852`, `:2300-2312` |
| Static-switch-off branches | **Excluded** — only compiled chunks register uniforms. | `MaterialExpressions.cpp:9324-9332`, `HLSLMaterialTranslator.cpp:3333-3374` |
| Material functions and layers | Included if compiled; layer indices are remapped per instance level. | `HLSLMaterialTranslator.cpp:6785-6797`; `MaterialInstance.cpp:973` |
| Streaming virtual textures | Returned (the `Virtual` bucket). Tell them apart with `IsCurrentlyVirtualTextured()` (`Texture2D.cpp:1210-1224`). | `Material.cpp:1113-1116`; `Texture2D.cpp:1002-1009` |
| Runtime virtual textures | **Never returned** — `URuntimeVirtualTexture` is not a `UTexture`. | `Classes/VT/RuntimeVirtualTexture.h:15`; `MaterialUniformExpressions.h:314-337` |
| No shader map loaded | 🚨 **Returns an EMPTY list with no warning.** An empty list is therefore NOT evidence that the material has no textures. | `MaterialShared.cpp:929-935`; `Material.cpp:1073` (server-only is empty too) |
| Streaming state | Irrelevant to *which* textures are returned. Residency matters only to what a render-target snapshot captures (§3.4). | — |

**Consequences built into the design:**

- The per-component call is `UPrimitiveComponent::GetUsedTextures(Out, EMaterialQualityLevel::Num)`,
  exactly as m52 uses it (`Anomaly_StuckLowMip.cpp:101-116`).
- Per slot, the call is `Slot->GetUsedTextures(Out, Num, true, GMaxRHIFeatureLevel, false)`.
- An empty result is refused as **`no_textures`**, and the log line names **both** readings: "the
  material has none" and "no shader map is loaded for it". The two cannot be told apart at runtime.
  This is `G119`'s rule applied to a silent zero.

### 2.2 Which runtime texture fields can be read in a cooked build

`SRGB` (`Classes/Engine/Texture.h:1327`), `CompressionSettings` (`:1298`), `LODGroup` (`:1310`),
`VirtualTextureStreaming` (`:1357-1359`), `IsCurrentlyVirtualTextured()` (`Texture2D.h:302`),
`AddressX/Y` (`Texture2D.h:55-61`), `GetPixelFormat()` (`Texture2D.h:150`), `GetSizeX()` (`:147`),
`GetNumResidentMips()` (`:141`). **None is inside `WITH_EDITORONLY_DATA`.**

- `UTexture::IsNormalMap()` is runtime-safe: `CompressionSettings == TC_Normalmap`
  (`Texture.h:1720-1724`).
- ⚠ **The pixel format is not a normal-map test.** BC5 vs DXT5n is a cook-time choice
  (`Texture.cpp:3233-3236`, inside `#if WITH_EDITOR` 3005-3339).
- ⚠ **No cooked API says which texture feeds BaseColor or Normal** (§1.1). The closest runtime fact is
  `UMaterial::IsPropertyConnected(MP_Normal / MP_BaseColor)` (`Material.h:1739`, `Material.cpp:3961-3964`).
  It says whether the input is connected, not what feeds it. The plan uses it as a **guard** only: a
  normal-family fire on a material whose Normal input is unconnected is refused `normal_unconnected`,
  because a `TC_Normalmap` texture there is being used for something else.

### 2.3 Classifying each used texture

Applied per texture, in this order. The first rule that matches decides.

| # | test (runtime fields only) | class | effect |
|---|---|---|---|
| 1 | not a `UTexture2D` (cube, array, volume, render target) | `unsupported_type` | skipped |
| 2 | `IsCurrentlyVirtualTextured()` | `virtual_texture` | **skipped, never touched.** Binding a non-VT render target to a VT parameter renders **black**, with one warning; the reverse renders black with none (`MaterialShared.cpp:3711-3726`, `MaterialUniformExpressions.cpp:1230-1239`, `Texture2D.cpp:1335-1337`) |
| 3 | `LODGroup` ∈ {UI, Lightmap, Shadowmap, Terrain_Heightmap, Terrain_Weightmap, Bokeh} | `excluded_group` | skipped — the exact list m52 uses (`Anomaly_StuckLowMip.cpp:51-65`) |
| 4 | `GetSizeX() < 64` or `GetSizeY() < 64` | `trivial_texture` | skipped. A 1×1..32×32 texture (`T_WhitePixel`, `127grey`, `BlackPlaceholder`, `T_white`/`T_black`) is a **constant**, and no UV transform of a constant changes a pixel. Pre-declared as a design floor, not a calibrated threshold. |
| 5 | `CompressionSettings` ∈ {HDR, HDRCompressed, Alpha, DistanceFieldFont, Displacementmap, VectorDisplacementmap, EditorIcon} | `unsupported_format` | skipped — encoding any of these through an 8-bit target would change more than the mode (§3.2) |
| 6 | `IsNormalMap()` | **`normal`** | normal family: corrupted; UV family: transformed coherently |
| 7 | `SRGB` | **`colour`** | UV family: transformed |
| 8 | otherwise (`TC_Masks`, `TC_Grayscale`, linear default) | **`data`** | UV family: transformed (ORM / masks move with the UVs); normal family: untouched |

**Base vs normal is decided by `IsNormalMap()` first. Name heuristics (`_N`, `_Normal`, `_NRM`) are a
FALLBACK ONLY**, used solely to break ties inside the normal class. They never admit a texture the
flag rejects.

- On StackOBot the scan found flag and name disagreeing on two textures, one in each direction
  (32 by flag, 32 by name, 31 by both). One is `T_GratingTileable_Opacity`, flagged by a normal token
  and not by name (Appendix A). That is why the flag decides and the name only breaks ties.
- **In route B the base-vs-normal distinction matters only for the normal family and for the
  render-target encoding** (§3.2). Route B needs no "which one is THE base colour" decision at all.
- In route A it does, and the rule is in §3.1.

### 2.4 Zero, one or several candidates — the refusal vocabulary

A fire's candidate set is the union over the target's eligible slots of the textures that pass §2.3
**and**, in route B, resolve through a texture parameter (§1.4 step 2).

| candidates after §2.3 | UV family | normal family |
|---|---|---|
| 0 overall | refuse **`no_eligible_texture`** (counters say how many were virtual / trivial / excluded / unsupported / not a parameter) | same |
| no `normal` class | fire on the `colour` + `data` textures | refuse **`no_normal_map`** |
| several `normal` | transform all of them | corrupt **all** of them. A material with a main normal plus a detail normal (`T_rock_02_N` + `T_detail_N`) gets both. The label lists each texture, and ties are never resolved by name. |
| more than `TexCorruptMaxTextures` (compiled **8**) | keep the 8 largest by `GetSizeX()·GetSizeY()`; count the rest `texture_cap` | same |

**Target-level refusals** (closed vocabulary, each with a `run_summary` counter and a loud `REFUSED`
line in the `lod_popping` / `stuck_low_mip` shape — `Anomaly_StuckLowMip.cpp:555-567`):

| reason | when |
|---|---|
| `no_mesh` | the substring resolves no static or skinned mesh component |
| `translucent_only` | every slot is translucent. Route A adds `masked` and `two_sided` here (the agreed rule). |
| `no_textures` | `GetUsedTextures` returned nothing for every slot (§2.1) |
| `no_eligible_texture` | §2.4 table |
| `no_normal_map` / `normal_unconnected` | normal family (§2.2) |
| `texture_not_parameter` | route B: none of the eligible textures resolves through a parameter |
| `nanite_override` | route B: the slot is on a Nanite component and its material has a Nanite override (§1.4) |
| `held_by_stuck_low_mip` | an eligible texture is currently held or awaiting restore by `stuck_low_mip` — the snapshot would bake the blur into the render target (§4.5) |
| `mode_inapplicable` | targeted fire only: the requested mode cannot apply (route A `channel1` on a single-UV mesh, §3.1) |
| `rt_budget` | route B: the event's render targets would exceed `TexCorruptMaxRtBytes` (§8) |
| `dxt5_normal_host` | route B: `Compat.UseDXT5NormalMaps` reads non-zero, so the host unpacks `.ag`, which the render-target encoding does not serve (§3.2). The normal family is refused; the UV family skips `normal`-class textures and transforms the rest. |
| `alpha_texture` | route B, **only if S1 falls back**: an alpha-bearing source format while the alpha-preserving draw is unproven (§3.2) |

---

## 3. Q2 — Material design

### 3.1 Route A (the agreed takeover) — specified in full, as the fallback

**`M_CorruptedUV`** — Lit, **opaque**, **single-sided** (so it matches single-sided targets and does not
reveal back faces), `MSM_DefaultLit`.

- Parameters:
  - `BaseTex`: `TextureSampleParameter2D`, sampler **Color**.
  - `NormalTex`: sampler **Normal**, default = the engine's flat normal.
  - `UvScale` (vector), `UvOffset` (vector, written by C++ per tick for `drift`), `UvSwap` (scalar 0/1),
    `Uv1Mix` (scalar 0/1).
- UV expression: `uv = lerp(lerp(TexCoord0, TexCoord0.yx, UvSwap), TexCoord1, Uv1Mix) * UvScale + UvOffset`.
  **Both textures sample the same corrupted UV**, so the maps move together.
- Lighting constants: Roughness 0.6, Metallic 0, Specular 0.5, AO 1. These are plausible mid-values,
  not the host's, and that is the reconstruction loss §1.3 describes.

**`M_CorruptedNormal`** — same shading. Both textures sample `TexCoord0` unchanged, and the unpacked
normal `n` is transformed:

- `n.xy * NormalSign` (vector; (1,−1) = green flip, (−1,−1) = invert);
- `lerp(n, (0,0,1), FlatMix)`;
- `+ NoiseAmp * (shipped tileable noise normal)`, renormalised.

Route A needs these fixes; each would be a defect otherwise:

- **Two-sidedness must be FALSE.** The plugin's `_fresh_material` authors every material two-sided
  (`tools/create_anomaly_materials.py:28`), which is right for the magenta (a back-face hit must not
  draw the original). For m53 it would reveal back faces the host culls.
- **All seven usage flags** (`set_material_usage_flags.py:112-120`). Without `StaticLighting` the
  default material is drawn on a lightmapped static mesh (`StaticMeshRender.cpp:2225-2228`; the
  Concorde defect).
- **`BaseTex` must be fed an sRGB texture.** Color and LinearColor samplers generate identical code, and
  sRGB decode comes from the texture's own flag in hardware (`[Shaders] Private/MaterialTexture.ush:144-147`,
  `:168-171`; `StreamableTextureResource.cpp:113`). A linear texture in `BaseTex` would draw with the
  wrong gamma.
- **A partial, name-based guard against projected mapping is possible and is WEAK.** A cooked material
  keeps `CachedExpressionData.FunctionInfos` (`Public/MaterialCachedData.h:297-299`), so route A could
  refuse hosts that call the engine's `WorldAlignedTexture*` functions. Host-authored projections
  (StackOBot's `MF_WorldSpaceUV`, Lyra's `MF_WorldGrid`) carry host names, and matching them would be a
  host-specific list, which the game-agnostic invariant forbids. ⇒ It catches the engine-function
  case only, and the §5.4 disclosure stays mandatory.
- **The base colour is chosen as:** the §2.3 `colour` class; then `_D` / `_BC` / `_BaseColor` /
  `_Albedo` / `_Diffuse` / `_Color` as a tiebreak; then the largest texture. Anything still tied is
  refused **`ambiguous_base`**. On the modular kit this refuses or picks `T_SandTileabe_BC` (§1.2).
- 🚨 **`channel1` needs a second UV channel.** The vertex-factory behaviour for a missing TexCoord[1] is
  in §3.1.1. The mode is refused `mode_inapplicable` on a single-UV LOD, read from
  `FStaticMeshLODResources::GetNumTexCoords()` / `FSkeletalMeshLODRenderData::GetNumTexCoords()`.

#### 3.1.1 TexCoord[1] on a mesh with one UV channel

What the GPU returns for a missing channel **depends on the vertex factory, and none of the answers
is a visible "wrong channel":**

| path | TexCoord[1] on a one-UV LOD | source |
|---|---|---|
| `FLocalVertexFactory`, manual vertex fetch (PC SM5/SM6 default) | **clamped to the last channel = UV0** ⇒ `channel1` changes nothing | `[Shaders] Private/LocalVertexFactory.ush:614-622`; `RHI/Private/RHI.cpp:2490`; `Config/Windows/DataDrivenPlatformInfo.ini` |
| `FLocalVertexFactory` without manual fetch | the last UV stream fills empty slots | `LocalVertexFactory.cpp:481-499` |
| GPU skin VF (no skin cache) | `TexCoords0.zw`: the graphics API's default fill, likely (0,1) ⇒ a constant UV — **inference, API behaviour, not UE code** | `GPUSkinVertexFactory.cpp:608-627`; `[Shaders] Private/GpuSkinVertexFactory.ush:95-111`, `:348-361` |
| GPU skin cache (passthrough local VF) | clamped = UV0 | `Public/GPUSkinVertexFactory.h:479`; `GPUSkinVertexFactory.cpp:482-492` |
| Nanite (SM6) | **zero** | `[Shaders] Private/Nanite/NaniteAttributeDecode.ush:390-419`; `NaniteVertexFactory.ush:186-196` |

⇒ `channel1` is refused `mode_inapplicable` unless the drawn LOD has ≥ 2 UV channels.

🚨 **The channel count must NOT be read with `UStaticMesh::GetNumUVChannels`: in a cooked build it
returns 0**, because its body is `#if WITH_EDITORONLY_DATA` (`StaticMesh.cpp:5150-5162`, declared
unguarded at `StaticMesh.h:1525-1530`). A check written with it would refuse `channel1` everywhere in a
package and pass in the editor. Use these instead, all cooked-available:

- `UStaticMesh::GetNumTexCoords(LOD)` / `FStaticMeshLODResources::GetNumTexCoords()`
  (`StaticMesh.h:1747-1750`; `StaticMesh.cpp:911-914`, `:3443-3451`);
- `NaniteResources.NumInputTexCoords` (`Rendering/NaniteResources.h:279`);
- `FSkeletalMeshLODRenderData::GetNumTexCoords()` (`Rendering/SkeletalMeshLODRenderData.h:250-253`).

CPU vertex data is **not** kept for static meshes in a cooked build (`StaticMesh.cpp:498-505`), which
confirms the agreed rejection of a mesh-level UV rewrite.

### 3.2 Route B (recommended) — the two render-target corruptors

Both are **Surface-domain, Unlit** materials, drawn into a render target and never onto a mesh.

**How the draw works — `UKismetRenderingLibrary::DrawMaterialToRenderTarget`, available in a cooked build**
(`Private/KismetRenderingLibrary.cpp:152-217`; its only `WITH_EDITOR` blocks are `:28-34` and `:554-646`).

- It builds an `FCanvas`, draws one `FCanvasTileItem`, flushes it, then calls
  `UpdateResourceImmediate(false)`, which is what regenerates auto mips (`:191-214`;
  `UserInterface/Canvas.cpp:2056-2073`; `UserInterface/CanvasItem.cpp:475-518`).
- **No `FlushRenderingCommands` anywhere on this path**, so the game thread never blocks.
- The tile is drawn by `FTileVertexFactory : FLocalVertexFactory` with **one UV channel**
  (`CanvasTypes.h:1052-1060`; `TileRendering.cpp:98`, `:132-135`) through the base pass, with only
  render target 0 bound, load action `ELoad`, no depth and **no clear** (`Renderer/Private/Renderer.cpp:125-426`,
  `:249-250`, `:342`, `:370-392`).
- 🚨 **The material must be Surface domain, not UI.** `FLocalVertexFactory::ShouldCompilePermutation`
  returns `!!WITH_EDITOR` for `MD_UI` (`LocalVertexFactory.cpp:280-289`). A UI-domain corruptor would
  have no shaders in a cooked build and would draw the default material — while working in the editor
  and PIE, so the defect would be invisible until packaged.
- **No usage flag is needed** (`Classes/Materials/MaterialInterface.h:53-76`).
- **The value written is Emissive**, with no gamma applied in the shader and pre-exposure 1
  (`[Shaders] Private/BasePassPixelShader.usf:1392`, `:1437-1441`, `:2040-2044`; `[Shaders] Private/Common.ush:791`).
  That is why the corruptors are **Unlit**: a Lit model adds the base pass's diffuse and indirect terms.
- 🚨 **An opaque material writes alpha = 0** (`BasePassPixelShader.usf:1916`;
  `[Shaders] Private/LightAccumulator.ush:101`). A translucent one writes `old alpha × (1 − Opacity)`
  (`BasePassPixelShader.usf:1884-1886`; `BasePassRendering.cpp:261-279`). See "Alpha" below.
- **Engine-side readiness:** the call runs `Material->EnsureIsComplete()` first (`:183`). That is a
  blocking compile in the editor and a no-op in a cooked build (`MaterialInterface.cpp:1468-1480`). So
  an editor leg cannot bake the grey fallback into a render target. It can still hitch, which is what
  `D6`'s prewarm addresses.
- **Cost per draw (render thread):** a fresh `FSceneViewFamily` + `FSceneView`, tile vertex and index
  buffers, and a `FViewInfo`, with the engine's own comment "this is going to be slow for each tile"
  (`TileRendering.cpp:89-149`, `:272-291`; `Renderer.cpp:130-131`). Negligible once at Apply; it is
  why `drift`'s per-frame redraw is costed separately in `G-COST`.

**Render targets** (`Classes/Engine/TextureRenderTarget2D.h`, `Private/TextureRenderTarget2D.cpp`):

- `RTF_RGBA8` and `RTF_RGBA8_SRGB` both map to `PF_B8G8R8A8` (`TextureRenderTarget2D.h:18-67`).
- `IsSRGB()` gives the resource the sRGB flag, and D3D12 then gives both the render-target view and the
  shader-resource view `_SRGB` formats, so linear values round-trip (`TextureRenderTarget2D.h:225-247`;
  `TextureRenderTarget2D.cpp:583-604`; `D3D12RHI/Private/D3D12Texture.cpp:1407-1410`).
- `bAutoGenerateMips` sizes the mip chain and generates it with `MipsSamplerFilter` / `MipsAddressU/V`
  (`TextureRenderTarget2D.cpp:50-62`, `:682-719`). The corruptor copies the source's `AddressX/Y`
  into both the target and its mip addressing. A host material's shared sampler still overrides the
  address mode where it has one (`MaterialUniformExpressions.cpp:967-975`).
- A render target is accepted as a texture-parameter value: it reports `MCT_Texture2D`
  (`TextureRenderTarget2D.cpp:81-84`; `MaterialUniformExpressions.cpp:955-963`), and no sampler type is
  checked at runtime (`MaterialInstance.cpp:3428`).

**`M_CorruptTex_UV`** — blend mode **AlphaComposite** (see "Alpha" below). A scalar `AlphaFromSource`
(0/1) forces `Opacity = 0`, i.e. A = 1, **without a re-cook**. That matters because a MID cannot
override blend mode — it forwards to its parent (`MaterialInstance.cpp:1598-1641`) — so the alpha
fallback has to be a parameter, not a second material.

- Three source parameters: `SrcColor` (Color sampler), `SrcData` (Masks sampler), `SrcNormal` (Normal
  sampler).
- `SrcKind` (scalar 0/1/2) selects one. All three are sampled; the unused two carry 4×4 placeholders.
- Scalar parameters: `UvScale`, `UvOffset` (drift), `UvSwap`, `ScrambleCells` (K), `ScrambleSeed`.
- The output UV of each render-target texel is `f(uv)`:
  - **tile** — `frac(uv·N)`;
  - **swap** — `uv.yx`;
  - **drift** — `frac(uv + offset(t))`;
  - **scramble** — the K×K cell index permuted by a seeded hash, the local cell UV kept.

**`M_CorruptTex_Normal`** — blend mode **Opaque** (a normal map's alpha is never read on the default
`.rg` path, see the encoding table):

- One source parameter `SrcNormal` (Normal sampler), unpacked to `n`.
- Output: `NormalSign` (green flip / invert), `FlatMix` (flat), `NoiseAmp` with a shipped tileable
  noise-normal texture (noise).
- **`noise` uses a shipped texture, not the Noise node**, so the pattern is deterministic across runs
  and hosts.

**Render-target encoding — the part that must be exactly right.** Each encoding is checked by the
identity twin (`G-ID`), never assumed:

| source class | render-target format | written value | why |
|---|---|---|---|
| `colour` (`SRGB` true) | `RTF_RGBA8_SRGB` | the hardware-decoded linear colour; the sRGB write re-encodes it | The host's Color sampler decodes in hardware from the resource's sRGB flag. Color and LinearColor samplers emit identical code (`[Shaders] Private/MaterialTexture.ush:144-147`, `:168-171`; `StreamableTextureResource.cpp:113`), so the target's sRGB flag must equal the source's. |
| `data` (`SRGB` false) | `RTF_RGBA8` (linear; `bForceLinearGamma` default true, `TextureRenderTarget2D.cpp:29-44`) | the raw sample | same reason, other way round |
| `normal` | `RTF_RGBA8` (linear) | **`n.xy · 0.5 + 0.5` in R and G** (B = `n.z·0.5+0.5`, ignored) | The host's Normal sampler runs `UnpackNormalMap`, which reads `.rg` then `*2−1` and rebuilds z — unless `DXT5_NORMALMAPS`, when it reads `.ag` (`[Shaders] Private/Common.ush:1373-1384`). `DXT5_NORMALMAPS` comes from `Compat.UseDXT5NormalMaps` at shader compile and defaults to **0** (`ShaderCompiler.cpp:6056-6059`; `Core/Private/HAL/ConsoleManager.cpp:2842-2847`). The `.ag` form is **not** served, because the opaque normal corruptor writes A = 0. So the normal family **reads that console variable at Apply and refuses `dxt5_normal_host` when it is non-zero** — a host setting read at runtime, never assumed. |

⚠ **Alpha — the one encoding the default draw cannot reproduce.** Some hosts pack data into a
texture's alpha: opacity in base-colour alpha, or roughness. An **opaque** corruptor writes A = 0
(above), so a `colour`/`data` texture whose alpha the host reads would lose it. Nothing at runtime says
whether the alpha is read. The design therefore:

- **(primary)** draws `colour` and `data` sources with an **AlphaComposite** corruptor after clearing the
  target to (0,0,0,1), outputting `Emissive = src.rgb` and `Opacity = 1 − src.a`. From the
  translucent output and blend state
  (`BasePassPixelShader.usf:1884-1886`; `BasePassRendering.cpp:261-279`) this yields
  `RGB = src.rgb + 0·(1 − Opacity)` and `A = 1·(1 − Opacity) = src.a`.
  - ⚠ **That is a derivation, not a measurement.** `G-ID` has to prove it on a target whose host reads
    base-colour alpha. On StackOBot the masked Bot face (`MI_BotFace`) is the candidate. If no fixture
    target reads alpha, `G-ID` writes **UNEXERCISED** for the alpha half.
  - The extra clear is one more draw at Apply (`UKismetRenderingLibrary::ClearRenderTarget2D`).
- **(fallback)** if S1 cannot prove the primary path, refuse alpha-bearing source formats
  (`GetPixelFormat()` ∈ {DXT3, DXT5, BC7, B8G8R8A8, …}) as `alpha_texture`. **That is a yield cost**,
  because BC7 is common for albedo whose alpha is unused.

**Usage flags:** none. The tile vertex factory is a local vertex factory and `EMaterialUsage` has no
entry for it (`MaterialInterface.h:53-76`), so the seven-flag list does not apply and the permutation
count stays at the floor. **Material domain: Surface, never UI** (above).

**Lighting plausibility is inherited.** The host's graph does the lighting, so there is no constant to
choose and nothing to glow. `G50`'s emissive-bleed hazard does not arise, because the Unlit emissive
goes into an off-screen render target and never into the scene's Lumen surface cache.

### 3.3 Permutations, the first-use hitch, `m47`, and the cook

- **Permutations.**
  - Route A: two Lit materials × 7 usage flags × the project's vertex factories.
  - Route B: two Unlit materials × the draw path's one VF.
  - Both are one cook.
- **First-use hitch in a packaged build.** PSO precaching is off by default in 5.1
  (`PipelineStateCache.cpp:103-110`; `IsComponentPSOPrecachingEnabled` `PSOPrecache.cpp:36-39`), so the
  first draw of each new material × VF pair creates its PSO and the RHI thread waits for it
  (`PipelineStateCache.cpp:2067-2094`). **The pixels are still correct; it is a stall, not a wrong
  frame.**
  - Route A pays it in the scene, on the first labelled frame, once per VF family per session.
  - Route B pays it once per session on the render-target draw at Apply. It is **also** absent from the
    scene, where the MID reuses the host's shaders and render state.
  - Measured by `G-COST` (§6). No number is predicted.
- **Shader readiness (`m47`).** `GatherAnomalySwapMaterials` lists only the checker and the magenta
  today (`AnomalyCaptureSubsystem.cpp:2399-2412`).
  - **Route B:** the engine's render-target draw already calls `EnsureIsComplete()` on the corruptor
    before drawing (`KismetRenderingLibrary.cpp:183`). An editor leg therefore **cannot** bake the
    grey fallback into a target — it blocks instead, a hitch inside the Apply frame. Adding the two
    corruptors to the m47 prewarm moves that block to run start, which is the point of the prewarm.
  - **Route A:** the materials draw in the scene, so without the prewarm an editor leg's first frame
    could draw the grey fallback (`G231`). The prewarm is **required** there.
  - ⚠ Adding them also widens `G298`'s run-wide false-positive mark: one more material that can read
    incomplete and mark every frame.
  - **Recommended:** add them to the **prewarm** list and **not** to the per-frame
    `AnomalyMaterialsIncomplete` count (`:4442`) until `G298`'s filed fix (per-anomaly readiness) lands.
    `D6`.
- **Cook.** One cook carries both new `.uasset`s.
  - They are hard-referenced from the subsystem CDO exactly like the magenta
    (`AnomalyInjectorSubsystem.cpp:53-55`; `G45`), and `CanContainContent` is already true.
  - The cook runs on **editor** binaries, so the editor target is rebuilt first (`G47`, runbook §8.6
    step 3.5).
  - Presence is proven by a **runtime load and a use**, not by a `.pak` listing (`G48`).
- **Office hosts.** Concorde and Bates build and cook the plugin themselves, so both new assets must
  reach their content tree by git pull.
  - The office procedure gains a runtime-load read-back: a new one-time startup log line from the
    injector subsystem, naming each shipped material and whether it resolved non-null. No such line
    exists today for the checker or the magenta; their presence has been inferred from their use.
  - Route A would additionally need the `missing bUsedWith` log grep that `G157` prescribes.
  - Route B adds **no usage-flag dependency** on the host's lighting setup. Route A needs the static
    lighting flag on Concorde (`G157`), and this bench cannot verify that flag: StackOBot sets
    `r.AllowStaticLighting=False` (`Config/DefaultEngine.ini:23`).

### 3.4 Snapshot resolution and texture streaming

**How the streamer learns a component's textures.**

- `UStaticMeshComponent` hands `GetStreamingTextureInfoInner` its prebuilt data **only for Static
  mobility** (`Components/StaticMeshComponent.cpp:1101`).
- `FStreamingTextureLevelContext::ProcessMaterial` then uses either the component's prebuilt list
  (cooked-only branch, `Streaming/TextureStreamingBuild.cpp:649-670`) or `Material->GetUsedTextures`
  (`:672`). It skips non-streamable textures (`:680-686`), and when a texture has no entry it falls back
  to "a sampling scale of 1 using the UV channel 0" (`:712-713`).
- Material `TextureStreamingData` is cooked but **built only by editor tools** (`MaterialInterface.h:260-262`;
  `Developer/MaterialUtilities/Private/MaterialUtilities.cpp:2103-2248`).

**What `SetMaterial` does to a static component.**

- It calls `NotifyPrimitiveUpdated_Concurrent` (`MeshComponent.cpp:78-82`).
- That **adds** a dynamic registration and does **not** remove the static references
  (`Streaming/StreamingManagerTexture.cpp:1054-1064`; `DynamicTextureInstanceManager.cpp:183-185`).
- ⇒ **The original material's textures keep their static streaming entries for the rest of the level's
  life.** Residency is per texture and is the max over all users (`AsyncTextureStreaming.cpp:187-195`,
  `:213`; `TextureInstanceView.cpp:510-511`).

**Route B — the snapshot rule.**

- **The render target is drawn ONCE at Apply** from the source texture's then-resident mips, at a
  resolution of `min(top resident mip width, TexCorruptRtMaxSize)`. The resident mip count is read from
  `GetNumResidentMips()` (`Texture2D.cpp:455-491`), and the top width is the m52 arithmetic
  (`Anomaly_StuckLowMip.cpp:44-49`).
- So the corrupted texture has exactly the detail the viewer was seeing at the moment of the fire.
- `drift` redraws every frame from that **snapshot**, never from the source, so a later stream-out of
  the source cannot change the event's content.
- A render target is not a streamable asset. After Apply it is fixed-resolution for the event.
- 📌 **Named limit.** If the camera approaches during the event, the host would have streamed in finer
  mips while the snapshot stays at Apply-time detail, so the object can look softer than an
  uncorrupted neighbour. On a settled bench pose this cannot occur. The telemetry records
  `src_resident_top_px` and `rt_size` per texture, so the case is visible rather than hidden.
- ⛔ **No force-resident call** (`SetForceMipLevelsToBeResident`, `StreamableRenderAsset.cpp:218`): it
  raises residency **for every user of the texture** (`AsyncTextureStreaming.cpp:269-276`), and on a
  visible co-user that is an unlabelled change.

**Route A — streaming density under tiling.**

- A takeover MID has no streaming data of its own. `SetTextureParameterValue` records a
  placeholder-to-host rename (`MaterialInstanceDynamic.cpp:229-231`, `:505-519`), so the host texture
  gets the placeholder's density if the plugin material was built with streaming data, and the
  scale-1 UV0 fallback otherwise.
- 🔻 **Tiling does NOT blur under that fallback.** ×8 tiling means a sampling scale of ~8, which *lowers*
  the needed mips (`MaterialInterface.cpp:1373`; `[Shaders] Private/DebugViewModePixelShader.usf:86-96`).
  The streamer over-requests; only a UV scale below 1 would under-request.
- Virtual textures are outside the streamer altogether (`Texture2D.cpp:959-963`,
  `StreamableRenderAsset.cpp:277-293`), which is one more reason §2.3 skips them.

---

## 4. Q3 — Takeover (or override) and restore

### 4.1 Slot selection, and meshes with several materials

- Components come from `AnomalyLod::ResolveLodComponents` (`AnomalyLod.cpp:11-30`), the resolver all
  texture anomalies share. Viewport scoping applies when on (`Anomaly_CorruptedTexture.cpp:84-92`
  pattern).
- **Per slot**, not per component: a slot is corrupted iff its material passes the route's slot test
  and at least one of its textures passes §2.3 / §2.4.
  - **Route B** slot test: not translucent.
  - **Route A** slot test: opaque, not two-sided, not masked.
- A multi-material mesh therefore gets a **partial** corruption: glass stays glass. The label records
  `slots_corrupted / slots_total`.
- One fire covers the target's eligible slots on every matched component, as `corrupted_texture` does
  (`:99-127`). A target with zero eligible slots is refused.

### 4.2 MID lifetime, and sharing

- **One MID per distinct source material within an event.** When several slots or components of the
  target use the same source material, they share that one MID; its parameters are identical by
  construction. **Never across events and never across actors**: siblings sharing the host material
  keep the original, and that isolation is the anomaly's point (`G46`).
- **The render targets are keyed per (source texture, mode transform) within an event**, so a texture
  used by two slots is drawn once.
- Outer = the component, so the MID and the render targets are GC-reachable only through the
  component's override while applied, plus a strong `TArray<TObjectPtr<>>` held by the anomaly.
  - The anomaly is not a `UObject`, so the strong reference lives on the injector subsystem as a
    `UPROPERTY` array, the `CorruptedTexturePink` pattern (`AnomalyInjectorSubsystem.h:133-137`).
  - They are released at revert, so nothing outlives the event.
- A MID is never re-parented. A new fire always creates new instances.

### 4.3 Exact restore — the `m17` contract, mirrored

The contract is `Anomaly_CorruptedTexture.cpp:136-253`, copied rather than extracted (the m29
precedent, `G46`/`m17`):

1. **Apply** captures, per slot: owner, component `FName`, slot index, original material, and
   `bWasExplicitOverride` (`:114-121`).
2. **Revert** re-finds the live component (weak pointer, otherwise by name on the owner) and touches a
   slot **only if it still holds OUR MID**. That is tested by identity against the event's MID set,
   because there is no pink to walk to.
3. The slot is set back to the captured original if it was an explicit override, otherwise
   `SetMaterial(i, nullptr)` → the mesh asset's slot material (`MeshComponent.cpp:48-109`;
   `StaticMeshComponent.cpp:2655-2678`; `SkinnedMeshComponent.cpp:1252-1264`).
4. **Sweep** every mesh component of each touched owner for any slot still holding one of our MIDs
   (component re-created after apply).
5. **Log** `restored / default-reset / left-to-game / unresolved / swept / re-found`.
6. **Release** the render targets and the MID references.

⚠ **`SetTextureParameterValue(nullptr)` cannot clear an override** — a null texture enqueues nothing
(`MaterialInstance.cpp:3428`). The design never tries to; the restore swaps the whole slot back, which
is why the MID is ours and disposable.

### 4.4 Destruction, level change, teardown

- The target actor is **watched** through `AActor::OnEndPlay`, with a weak-pointer poll as backstop,
  exactly as `stuck_low_mip` does (`WantsTargetLostNotification` / `OnTargetLost`,
  `Anomaly_StuckLowMip.cpp:618-640`, `:784-797`; `IAnomaly.h:40-44`).
- On loss the anomaly reverts at once. Unlike m52, **no shared asset is touched**, so there is nothing
  left to restore on a destroyed actor. The watch exists to stop `IsVisualConditionHeld` labelling a
  target that is gone, and to release the render targets promptly.
- `FinishRun`, cancel-before-focus and world teardown reach `Revert` through `RevertAllActive`
  (`AnomalyInjectorSubsystem.cpp:199-230`).

### 4.5 Interplay with `stuck_low_mip`

m53 mutates **no asset**, so the one-anomaly-per-actor invariant (`G30`) is sufficient against every
anomaly **except** `stuck_low_mip`. That one holds a shared **texture** that an m53 target may also
sample. A render-target snapshot of a held texture would bake the blur in and label it as a UV or
normal bug. Hence the refusal `held_by_stuck_low_mip`, read from a new additive query
`AnomalyStuckMip::IsTextureHeldOrRestoring(const UTexture2D*)` in the same module.

The reverse direction (m52 firing while an m53 event is live) needs no new code:

- **A texture m53 has corrupted** is sampled by the target only through its render-target snapshot,
  so a later m52 hold cannot reach the corrupted target at all.
- **A texture m53 left untouched** is still sampled by the target's MID (e.g. the base colour during a
  normal-family event). `Component->GetUsedTextures` therefore still counts the target as a visible
  co-user, and m52's auto-pool gate (`StuckMipMaxCoAffected`, compiled 0) refuses it.
- A **targeted** m52 fire bypasses that gate by design, as it always has.

---

## 5. Q4 — Visibility evidence

### 5.1 What m55 gives, and what it may not be used for

- m55 measures, per labelled phase, the first four frames' changed-pixel statistics inside the
  target's delivered mask (`chg_*`), against the rest of the picture (`ctl_*`) and against the
  pre-onset frame (`ref_*`) (`docs/change-evidence-fields.md:40-77`).
- **It carries no verdict, and nothing it measures may feed labels, selection or observability**
  (`:7-21`; client readme §9.1).
- ⇒ **m53 cannot use an m55 number as a floor in v1.** Every weak-mode rule below is either a
  pick-time rule computed from bounds and texture fields, or a label caveat. m55 is the **bench
  instrument** that calibrates those rules and the **client-side evidence** the client can filter on.

### 5.2 Predicted strength per mode — DECLARED, no numbers

"Strong" means m55 onset `chg_gt8 / chg_n` clearly above the same leg's `null_effect` twin and control.
"Weak" means it may not be. ⚠ **m55 measures pixel difference, not perceptibility.** A transposed rock
texture changes nearly every pixel and still looks like rock. That limit is stated, not solved (§5.4).

| mode | expected m55 onset (window 0) | expected perceptibility | why |
|---|---|---|---|
| `uv_tile` ×8 | **strong** on any textured surface with spatial content | strong where the texture has features (panels, logos, ribbing); weaker on stochastic textures (rock, concrete) | the texel frequency jumps 8× |
| `uv_swap` | strong on anisotropic textures; still a high pixel change on stochastic ones | **weak on isotropic stochastic textures** | transpose preserves texture statistics |
| `uv_scramble` (B) / `uv_channel1` (A) | **strong** | strong | cell seams / island mismatch are content-independent |
| `uv_drift` | window 0: **small** (one frame's offset); windows 1–3: accumulating in `ref_*` | strong **in video**, weak in a still | §9.3 of the client readme is exactly this shape |
| `normal_invert` | strong under a directional key light; weak in flat ambient light | same | the lighting sign flips |
| `normal_green_flip` | medium: only the lighting's vertical component flips | medium | half the invert signal |
| `normal_flat` | **weak** on low-frequency normals (the m52 `T_Paint_Normal` finding, 080-04 §6); stronger on high-detail normals | weak/medium | removes detail rather than adding error |
| `normal_noise` | medium to strong, scaling with the amplitude | medium | adds high-frequency lighting noise |

### 5.3 How weak cases are handled — chosen, and why

**Chosen: the per-mode auto-pool mode set, plus pick-time floors that need no pixel read, plus a label
caveat. Not a measured-change floor and not a runtime refusal on measured change.**

1. **Mode set.** `IAI.Anomaly.UvCorruptModes` / `NormalCorruptModes` (ini + console, `AnomalyDefaults`
   pattern) decide which modes the **auto-pool** may draw. Targeted fire can always request any mode.
   - Compiled defaults, recommended and pending `G-STR`: UV = `tile scramble drift`; normal = `invert
     green_flip noise`.
   - `uv_swap` and `normal_flat` are **available but off by default**, because §5.2 predicts both can be
     imperceptible on common content.
   - The set is **re-ruled from `G-STR`'s measurement** in an amendment, the m52 ratio precedent
     (4.0 → 8.0 after measurement).
2. **Pick-time floors, bounds and fields only** (they stay on the right side of `G127`):
   - the §2.3 trivial-texture floor (constants cannot be corrupted);
   - the existing auto-pool coverage floor (`GMinScreenCoveragePct` 6 %, `AnomalyViewport.cpp:42`),
     unchanged;
   - normal family only: `normal_unconnected`.
   - ⛔ **No invented texel or screen-size threshold.** m52's size ratio was measured blind to texture
     content (080-04 §6), and this plan does not repeat it.
3. **Label caveat.** Each m53 anomaly entry carries `texcorrupt.strength_class`, written as the
   **declared** class from §5.2 and never a measurement, so a client can exclude weak modes without
   parsing m55:
   - `strong`: `tile`, `scramble`, `channel1`, `invert`;
   - `medium`: `green_flip`, `noise`, and `drift` (weak in a single still, strong in video);
   - `weak`: `swap`, `flat`.

   The default mode sets are exactly the strong and medium classes. `G-STR` either confirms the
   classes or corrects them in an amendment, and the correction moves the key's values and the
   default sets together. `observable` keeps the m49 A1 rule unchanged:
   `bLabelled && bHeld && Px >= ObservableMinPixels` (`AnomalyCaptureSubsystem.cpp:4072`), with `bHeld`
   from `IsVisualConditionHeld` (`:4884-4887`).

**Why not a floor on measured change:**

- m55's contract forbids it.
- The measurement arrives after the frame is written.
- A floor that deletes events on measured change is `m26`'s veto. Re-using that shape for a
  **perceptual** question would need a calibration campaign with complex content, and `G135` / `m26`
  ruled that no such campaign exists yet.

**Why not a refusal:** at pick time nothing tells a flat normal map from a detailed one. The texture's
CPU data is not available in a cooked build, and the GPU data is not readable at pick time without a
readback (`G127`).

### 5.4 If route A is ruled — the extra disclosure it needs

- Every route-A event writes `texcorrupt.reconstructed: true`.
- The client readme gains a paragraph: the object's material is replaced by a simplified copy built
  from its base-colour and normal textures; tints, packed maps, projections and masks are dropped; and
  the visible change **includes** that simplification.
- `G-FID` (§6) then **measures the reconstruction alone** (identity mode) per fixture target and
  reports it beside the mode's own change. That gives the client a number for how much of the change
  is not the named mode.

---

## 6. Q5 — Gates and fixtures

### 6.1 Which targets qualify, per fixture — PREDICTED from the scan, to be READ at runtime by `G0`

| fixture | target | route B prediction | route A prediction |
|---|---|---|---|
| StackOBot `MainWorld` | `SM_rock` (`…_2048592804`), `SM_rock_02` | **eligible, both families** — MIC chain overrides `T_rock_0x_D/N/AORM` parameters | eligible, both families (single-sided opaque; base `T_rock_0x_D`) |
| StackOBot `MainWorld` | modular kit (`SM_Ramp` & co.) | UV family eligible **if** its textures are parameters; normal family eligible (`T_*_N`). ⚠ mostly Nanite ⇒ mask-unmeasurable (`G134`) | base = `T_SandTileabe_BC` or `ambiguous_base`; high reconstruction delta |
| StackOBot `MainWorld` | `SM_FloorBase` | **refused `no_eligible_texture`** for normal (mask only), UV eligible on `T_Grid_A` (data class) if a parameter | refused `no_base` |
| StackOBot `MainWorld` | `SKM_Bot` | eligible (masked is fine in B); its slots hold runtime MIDs ⇒ **clone path** (§1.4) | **refused** `masked` / `two_sided` |
| StackOBot `CB_GateLevel` | every target | **refused 100 % `no_textures`/`no_eligible_texture`** — `BasicShapeMaterial` references no texture (m52 §3.2; `make_gate_level.py:60`, `:68-69`) | same |
| Lyra `L_ShooterGym` | `Cube*` (MI_MS_* triplanar) | **eligible, both** (`T_Paint_Diffuse` colour, `T_Paint_Normal` normal); ⚠ `T_Paint_Normal` is low-frequency ⇒ `normal_flat` predicted weak (080-04 §6) | eligible; tint + triplanar lost |
| Lyra `L_ShooterGym` | weapons `SM_*` | eligible; Nanite ⇒ measured only if a non-Nanite instance is on screen | eligible |
| Lyra | `SKM_Manny/Quinn` (`B_Quinn_C_10` measured in 080-04) | eligible, clone path if MID | refused `masked` |

⇒ **`CB_GateLevel` is a REFUSAL fixture only**, exactly as it was for m52. It is not an m53 gate
fixture and must not be edited into one (`G99`). **The settled-camera arbiter for m53 is a
`MainWorld` pose on `SM_rock`**, plus Lyra `Cube4` at `L_ShooterGym`'s bench pose (the m55 twin pose).

### 6.2 Twins

- **Null twin = the identity mode** (`uv_tile` with N = 1 and offset 0, route B), fired at the same
  target and pose. Route B predicts it indistinguishable from "nothing happened": its onset pair reads
  like the phase's later, unchanged windows, and on `L_ShooterGym` like m55's `null_effect`, whose
  banked onsets read < 0.01 on StackOBot and < 0.02 on Lyra 720p
  (`2026-09-24-m55-081-23-requalification.md:135`).
  - It is a **bench-only lever** (`IAI.Bench.TexCorruptIdentity`, console only, default OFF, never in a
    client payload), because a label naming a mode that changes nothing is exactly what must never
    ship.
- **Positive twin = `solid_swap`** (m55, unchanged), giving the full-change reference at the same pose.
- **Can-fail lever (`G96`):** `IAI.Bench.TexCorruptNoApply`. The anomaly does all its bookkeeping, draws
  the render targets, and deliberately does not set the slot. Its reading must be
  `IsVisualConditionHeld` false on every frame and `observable` false. Without it a green
  `condition_held` proves nothing.

### 6.3 The gate list — pre-declared readings

All legs are **packaged** (`G76`). **Both tick orders** (native + `IAI.Bench.SynthTickOrder`) wherever
a gate asserts per-frame alignment (the m44 standing rule). Pixel-identity claims are made at the
**AA-off arbiter in native order** only, with the same-build control reading 0 (the m45 identity
arbiter; `G228`, `G230`).

| id | gate | pre-declared reading |
|---|---|---|
| **G0** | Eligibility read-back per target: slots, blend mode, the per-texture §2.3 class, parameter mapping, VT, size | the §6.1 predictions are **read**, never assumed; every disagreement with §6.1 is reported as a finding (the scan is evidence, not truth) |
| **G-ID** 🚨 | route B identity round-trip, `SM_rock` + Lyra `Cube4`, AA-off arbiter, native order | Self-contained per phase: window-0 `chg_gt8` (original → identity) **inside the band of the same phase's windows 1–3**, where nothing changes by construction, and `ref_gt8` on windows 1–3 inside the same band. On `L_ShooterGym` the reading is **also** checked against the `null_effect` twin at the same pose. That twin is fixture-gated to `CB_GateLevel` / `L_ShooterGym` (`Anomaly_ChangeCase.cpp:21-23`), so MainWorld has only the self-contained form. **If it fails, Stage 1 STOPS and route A is the fallback (§1.5).** |
| **G1** | each mode engages, targeted, `SM_rock`, both orders | label source `FireWindow`: first labelled frame == first differing frame (m44 ONSET) for every mode except `uv_drift`, whose onset is sub-`tau` by prediction and is read from `ref_*` |
| **G2** | can-fail (`TexCorruptNoApply`) | `texcorrupt.condition_held` false and `observable` false on every labelled frame. `frames_condition_lost` counts them (`AnomalyCaptureSubsystem.cpp:4061`). `injected_frames` is non-empty and `affected_frames` is empty. The event **stays** in `annotation.json` (the m49 ruling) and is **not** vetoed: the host still draws, so `m26` reads non-zero. m55 onset reads inside the `null_effect` band. **A G1 pass without this is not a result (`G96`).** |
| **G-STR** | per-mode strength, the §5.2 table, both fixtures, m55 on | m55 onset and `ref_*` per mode **reported against the null and positive twins at the same pose**; the §5.2 ranks are either confirmed or corrected in an amendment; **this reading re-rules the default mode sets** |
| **G-FID** | route A only: reconstruction delta (identity mode) per target | **reported per target**, no threshold; published beside every mode's reading |
| **G3** | restore identity | after revert + settle, the post-revert frame vs the pre-onset reference inside the window-0 mask: `|d|>8` count within the null twin's band. `IsVisualConditionHeld` false from the revert frame on. Slot materials pointer-identical to the captured originals (log line). |
| **G4** | restore on every exit: normal revert, `FinishRun`, cancel before focus, target destroyed mid-span, level change | every slot restored or default-reset per §4.3; render targets released (a count read back); zero `swept` on a static fixture |
| **G5** | `held_by_stuck_low_mip` interplay | fire `stuck_low_mip` (targeted), then m53 on a target that samples a held texture ⇒ refused `held_by_stuck_low_mip` by name. In the reverse order, an auto-pool m52 pick on a texture the live m53 target still samples ⇒ m52 refuses it as shared. |
| **G6** | Nanite + clone paths | a Nanite target (modular kit) fires and is labelled with `observability_measured` false (the m50 admit path); a Bot slot holding a runtime MID is cloned and restored; a `nanite_override` case is refused by name, or **UNEXERCISED** is written if the fixture has none (never a clean zero) |
| **G7** | auto-pool yield and refusal census, both fixtures, both ids | **no value predicted**; fires attempted, applied, refused per reason; the number the default-pool decision needs |
| **G8** | schema additivity: `P-C7 v3` against a pre-m53 control pair | `labels.jsonl` field set unchanged on a run without m53; `run_summary` adds exactly the `texcorrupt_*` keys; `annotation.json` field set unchanged (`P6`); `anomaly_subtype` values are new strings for new ids only |
| **G-COST** | paced 30 fps, A/B on the same build, apply-heavy targeted leg | `speed_ratio` and the m55 cost statistic within the within-build spread; the first-use PSO hitch **measured and reported** (first-fire frame time vs the rest); "below the resolution of this instrument" is the most a small difference can read (`G169`) |
| **G9** | verifier read, `--label-pixel-gate` + `--change-oracle` on the m53 sessions | readings only; `--change-oracle` exit 0 (arithmetic); `NO-TRACE` on a weak mode is a reading for `G-STR`, not a failure |
| **G10** | both build targets, packaged + editor | exit 0, zero warnings (`G221` — every new export crosses a DLL boundary only in the editor target) |
| **G11** | cook + load | both new assets load at runtime in the packaged build, and one fire draws a non-default render target (`G45`, `G48`) |
| **G-LYRA** | one leg per id on `L_ShooterGym` at Lyra's own render defaults (TSR, Lumen, AE on) | fires or refuses **for a named reason**; clone path exercised on a character if one is on screen |

### 6.4 Yield

`G7` gives the per-fixture numbers. **No yield is predicted.** The scan says StackOBot's measurable
(non-Nanite) textured targets are few — the rocks, plus the Bot under route B — so a thin MainWorld
yield is **expected and is not a defect**.

---

## 7. Q6 — Auto-injection

### 7.1 Two ids, not one

`uv_corruption` and `normal_corruption` are **separate ids**, each with its own `mode` enum:

- separate pool keys, so the owner can ship the strong family without the weak one;
- separate `anomaly_type` values, so the client's classes stay distinct;
- separate refusal counters.

The catalogue grows by two. `NumPoolKeys` goes 7 → 9 (`AnomalyAutoInjectorSubsystem.h:40`), with keys
`EKeys::Eight` / `Nine` and the `IAI.Auto.Bind` usage string extended (`AnomalyAutoInjectorSubsystem.cpp:1012`).

### 7.2 Mode draw — keeping `R-SEED`

The auto-injector today draws id, target and hold, then calls `ApplyAnomaly(Id, {Token})` with no mode
(`AnomalyAutoInjectorSubsystem.cpp:272`, `:374-380`). **Recommended:** one extra `Stream.RandHelper`
draw **only when the drawn id declares an enum `mode` arg in its catalogue spec**, over the id's
enabled mode set, passed as the second arg.

- The draw count stays a deterministic function of earlier draws, so the same seed gives the same
  modes.
- The protocol stays independent of the apply result (`R-SEED`).
- Every run that never draws an m53 id is **byte-identical** in its draws.

🔻 **Precision to `G150`, stated not silently applied.** `G150` says adding a pool member re-rolls every
seeded draw. At source, the draw is over `Eligible`, which filters by `EnabledIds`
(`AnomalyAutoInjectorSubsystem.cpp:241-252`). So **a pool member that is not enabled does not change
`Eligible.Num()` and re-rolls nothing**. m29's instance re-rolled because its id was default-enabled.
⇒ Shipping both ids **default OFF** keeps every banked auto-pool comparison valid for runs that do not
turn them on.

### 7.3 Pool membership — RECOMMENDATION; the owner's product call

**Recommendation: both ids in `GAutoPool`, NOT in `GAutoPoolDefaultEnabled`, for the first release.**
Flip `uv_corruption` to default-on after `G7` + `G-STR`. That follows the m52 / m29→m30 precedent
(shipped first, then pool-default decided on a measured yield).

What the owner needs to rule on default-on:

1. `G7`'s applied / attempted fraction on MainWorld **and** Lyra.
2. `G-STR`'s per-mode strength table against the twins, with the frames themselves for a by-eye look.
3. `G-COST`'s first-use hitch number.
4. The chat-side steer m52 used: **default-on if at least 25 % of visible targets are eligible on
   MainWorld**, which m52 recorded as a **future decision rule, not a gate threshold** — `G7` prints
   and passes or fails on nothing (m52 AMENDMENT 1 §A1.1 ruling 1,
   `2026-09-20-m52-stuck-low-mip.md:666-669`).

`normal_corruption` is recommended to stay default-off until a host with high-detail normals (Lyra's
characters and weapons, or the office content) shows `G-STR` strength.

### 7.4 Dashboard

**No dashboard change is needed.** The pool checkbox list mirrors the engine catalogue
(`ControlSnapshot.cpp:184-195`; the m19 rule, m52 §5), so the two ids appear by themselves.

---

## 8. Q7 — Cost and risk

### 8.1 Expected cost (declared; `G-COST` measures)

| item | route B | route A |
|---|---|---|
| Apply | 1 MID per distinct source material + 1 render target per corrupted texture, drawn once; `GetAllTextureParameterInfo` + `GetUsedTextures` per slot | 1 MID per distinct source material; `GetUsedTextures` per slot |
| First use | one render-target PSO per session | one scene PSO per material × VF family per session, **inside the labelled window** |
| Per frame | nothing, except `drift`: one render-target redraw per corrupted texture per frame, sampling the Apply-time snapshot | nothing, except `drift`: one `SetVectorParameterValue` per frame (one render command, `MaterialInstance.cpp:390-416`) |
| Memory | a render target of `min(resident top mip, TexCorruptRtMaxSize = 1024)`² × 4 B × 4/3 per texture ≈ **5.6 MB at 1024**. `drift` needs two per texture (the Apply-time snapshot + the per-frame output). Capped by `TexCorruptMaxRtBytes` (compiled **64 MiB**) ⇒ refusal `rt_budget` | none beyond the MID |

### 8.2 Risks, named

1. **Route A's reconstruction** (§1) — the reason for `D1`.
2. **Route B's render-target encoding** (sRGB, normal convention, alpha): identity-tested (`G-ID`),
   never assumed. Two traps are found at source and designed around, not discovered later:
   - a **UI-domain** corruptor draws the default material in a cooked build while working in the
     editor (`LocalVertexFactory.cpp:280-289`);
   - an **opaque** corruptor writes alpha = 0 (`BasePassPixelShader.usf:1916`).
3. **Parameter coverage**: route B refuses textures that are not parameters. `G7` measures what that
   costs.
4. **MID-of-MID**: handled by cloning, with the frozen-animation limit (§1.4).
5. **Nanite override materials**: refused by name (§2.4).
6. **Snapshot resolution** on camera approach (§3.4).
7. **The m47 `G298` false-positive** widens if the new materials enter the per-frame count (§3.3, `D6`).
8. **Weak modes** are invisible on some content (§5), handled by the mode set, not hidden.
9. **Virtual textures**: a VT texture is **skipped** at the texture level, because binding across the
   VT boundary renders black (§2.3 #2). A host whose textures are all VT (Lyra's Megascans surface,
   080-04 §5) is refused, and `G0` must reach the `virtual_texture` branch on Lyra, or **UNEXERCISED**
   is written.
10. **Office hosts**: route B needs nothing from the host but its own material. Route A needs its usage
    flags to match the host's lighting (§3.3).

### 8.3 Where the agreed shape is contradicted by the source (summary; details in the sections cited)

| agreed | source / content says | § |
|---|---|---|
| reuse the takeover | a takeover cannot carry the host graph (editor-only), so it reconstructs; on most fixture content that changes tint / projection / packing | §1.1–1.3 |
| base colour = "the sRGB texture" | there is no cooked API for "feeds BaseColor"; on the modular kit the only sRGB candidates are a sand overlay and a 1-pixel constant | §1.2, §2.2 |
| opaque, single-sided only | a consequence of the takeover; under route B unnecessary, and under route A it removes the hero character | §1.3(d) |
| lerp to TexCoord[1] | needs a second UV channel: on a one-UV mesh the local VF returns UV0 (invisible) and Nanite returns zero; and the obvious channel-count API returns 0 in a cooked build. Not expressible in texture space. | §3.1.1, §1.4 |
| `M_Corrupted*` are Lit, two-sided like the magenta | two-sided would reveal culled back faces (route A). Route B's corruptors are Unlit, **Surface-domain** (a UI-domain one has no shaders in a cooked build), AlphaComposite render-target materials. | §3.1, §3.2 |
| mesh-level UV rewrite rejected | **confirmed** — the render data is shared; this plan does not revisit it | — |

---

## 9. Q8 — Implementation stages, briefs and estimates

*Nothing below is written. Sizes are estimates.*

### 9.1 Files

| file | change |
|---|---|
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.{h,cpp}` | **NEW.** One class, parameterised by family, registered twice (`uv_corruption`, `normal_corruption`). Discovery §2, slot policy §4.1, MID + render targets §1.4 / §3.2, the m17 restore §4.3, the target watch §4.4, `GetTelemetry` §9.3, the `drift` redraw in `TickAlways`. ~700 lines. |
| `Source/AnomalyInjector/Private/AnomalyTexCorruptRt.{h,cpp}` | **NEW.** Render-target creation / format / draw / release, isolated so its correctness is gated on its own. ~200 lines. |
| `Source/AnomalyInjector/Public/AnomalyTexCorruptStats.h` | **NEW.** Run stats + refusal counters (the `AnomalyStuckMipStats.h` shape). |
| `Source/AnomalyInjector/Public/AnomalyInjectorSubsystem.h` / `.cpp` | two `FObjectFinder` hard refs + getters (`:49-56` pattern); `UPROPERTY` strong-ref arrays for the live MIDs and render targets (§4.2); `Register` ×2 (`:177-190`); `GetAuthoredSpec` arms: `mode` enum + `strength` float (`:100-164`); a one-time `Initialize` line naming every shipped material and whether it resolved (the office read-back, §3.3) |
| `Source/AnomalyInjector/Public/AnomalyDefaults.h` / `Private/AnomalyDefaults.cpp` | knobs: mode sets ×2, tile N (compiled 8), drift speed (compiled 0.25 UV/s), scramble K (compiled 8), noise amplitude, `RtMaxSize` (1024), `MaxRtBytes` (64 MiB), `MaxTextures` (8), `CloneHostMids` (on). Each follows the `…Compiled` / `Key()` / `Get` / `Describe` / `Set…Override` / `Clear…Override` block exactly (`AnomalyDefaults.h:36-44`, `:72-88`): console > ini > compiled, a `G139` echo, out-of-range **refused, not clamped**. The compiled values are design parameters, not calibrated thresholds; `G-STR` reads them. |
| `Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp` / `Public/…h` | `GAutoPool` +2 (not default-enabled); `NumPoolKeys` 7 → 9; the mode draw (§7.2); bench levers `IAI.Bench.TexCorruptIdentity` / `TexCorruptNoApply` |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp` + `AnomalyStuckMipStats.h` | additive query `IsTextureHeldOrRestoring` (§4.5) |
| `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | `ResolveAnomalyActiveSource` +2 → `FireWindow` (`:287-306`); `MapAnomalyToClient` subtype = the event's mode (`:266-278`); `GatherAnomalySwapMaterials` +2 for the prewarm only (§3.3, `D6`); `run_summary` counters |
| `Source/AnomalyCapture/Private/AnomalyLabelWriter.{h,cpp}` | `texcorrupt_*` `run_summary` keys (the `:740-759` block pattern); per-frame keys already flow through the generic telemetry path (`:133-156`) |
| `Content/Materials/M_CorruptTex_UV.uasset`, `M_CorruptTex_Normal.uasset` + a 256² tileable noise-normal texture | **NEW**, authored by `tools/create_anomaly_materials.py` (extended), cooked once |
| `tools/verify_capture.py` | restore-identity reading for `G3` (pre-onset reference vs first post-revert frame, masked by the window-0 mask) |
| docs | catalogue rows + architecture (route, restore, limits); `client-readme.md` §8 field rows + the anomaly paragraph + the `strength_class` caveat; `PRE-DELIVERY-CHECKLIST.md` categorical box (runtime load of both assets; the pool line); gotchas as found |

### 9.2 Stages, each with a stop point

| stage | content | build / cook | stop if |
|---|---|---|---|
| **S0** | this plan ruled (`D1`–`D7`) | — | — |
| **S1** — route B core, **identity only** | asset authoring + one cook; render-target module; MID / clone / restore; `uv_corruption` with the identity lever only; `G0`, `G-ID` (colour, data, normal, and the alpha half), `G3`, `G4`, `G10`, `G11` | 2 builds (editor + game), **1 cook** | 🚨 **`G-ID` fails on colour/data/normal ⇒ STOP, report, fall back to route A** (a new S1 with the takeover; the same asset cook budget). If **only the alpha half** fails ⇒ switch `AlphaFromSource` off, adopt the `alpha_texture` refusal, and continue — no re-cook. |
| **S2** — all modes, both ids | the eight modes; `G1`, `G2`, `G5`, `G6`, `G8`, `G9`; both tick orders | 2 builds, **0 cooks** if S1's graphs already carry every mode's parameters (they are specified to) | any mode fails ONSET, or the can-fail cannot fire |
| **S3** — measurement | `G-STR`, `G7`, `G-COST`, `G-LYRA`; mode-set and pool amendment | 0–1 build (Lyra worktree build per shared-tree rule 5) | — |
| **S4** — docs + merge | client readme, architecture, checklist, catalogue; strip comments; both targets zero warnings | 1 build pair | — |

**Estimate: 4 implementation sessions after the ruling. 1 cook, and a 2nd only if S1 falls back to
route A or a material graph needs a parameter S1 did not author. About 8 builds across both targets.**

### 9.3 Label keys (additive; `label_schema` stays **2**, the m52 ruling `2026-09-20-m52-stuck-low-mip.md:672-673`, which leaves them for m51's schema-3 bump)

- **`annotation.json`**: `anomaly_type` = the id; **`anomaly_subtype` = the mode**
  (`tile|drift|swap|scramble|green_flip|invert|flat|noise`). This is a new **value** in an existing
  field (`AnomalyLabelWriter.cpp:889`), so the field set does not move (`P6`).
- **`labels.jsonl`, inside m53 anomaly entries only** (the m52 per-frame key precedent; generic path
  `AnomalyCaptureSubsystem.cpp:4890-4915`):
  - `texcorrupt.mode`, `texcorrupt.route` (`host_param` | `takeover`), `texcorrupt.strength_class`;
  - `texcorrupt.slots_corrupted`, `texcorrupt.slots_total`;
  - `texcorrupt.condition_held`;
  - per-mode parameters: `texcorrupt.tile`, `texcorrupt.uv_offset` (drift, per frame),
    `texcorrupt.scramble_cells`, `texcorrupt.noise_amp`;
  - route A only: `texcorrupt.reconstructed`;
  - an array `texcorrupt.textures[]` of `{name, class, param, src_size, src_resident_top_px, rt_size}`.
- **`run_summary.json`**: `texcorrupt_fires_applied`, one `texcorrupt_refused_<reason>` per closed
  reason (§2.4), `texcorrupt_textures_corrupted`, `texcorrupt_rt_bytes_peak`,
  `texcorrupt_clone_host_mid`, `texcorrupt_swept`.

---

## 10. NEEDS-DECISION

- **`D1` — route.** Route B (host-preserving, texture-space; recommended) or route A (the agreed
  takeover, with the §5.4 disclosure). **Blocks S1.**
- **`D2` — `uv_channel1`.** Drop it and ship `uv_scramble` in its place (recommended under B), or keep
  it as a route-A-only mode (a second mechanism in one anomaly).
- **`D3` — two ids** (`uv_corruption` / `normal_corruption`) rather than one. Recommended: two.
- **`D4` — pool.** In `GAutoPool`, **not** default-enabled until `G7` + `G-STR` (recommended); the
  owner's product call.
- **`D5` — default mode sets.** UV `tile scramble drift`, normal `invert green_flip noise`, with
  `swap` / `flat` off until `G-STR` (recommended).
- **`D6` — `m47`.** New materials in the prewarm list only, not in the per-frame pending count, until
  `G298`'s per-anomaly fix (recommended).
- **`D7` — cloned host MIDs.** Allow them, with the frozen-animation limit recorded (recommended), or
  refuse slots holding runtime MIDs.

---

## 11. What this file does NOT claim

- ⛔ **No yield, strength or cost number.** `G7`, `G-STR` and `G-COST` produce them.
- ⛔ **No claim that route B's encoding is correct.** It is derived from source and has to pass `G-ID`.
- ⛔ **No perceptibility claim.** m55 measures pixel change; §5.2's perceptibility column is a
  prediction.
- ⛔ **No incidence claim about the office hosts' content.** The scan covers StackOBot and Lyra only.
- ⛔ **Nothing here changes `m51` (held at `53bf725`), `master`, any tag, any cooked container, or
  CaptureBench.**

---

## Appendix A — the offline content scan (EVIDENCE, not a measurement)

**Method.**

- Read the first 256 KiB of every `.uasset` under the content roots. Extract object-path references
  from the name table (the `m52_texture_sharing_scan.py` method, `tools/m52_texture_sharing_scan.py:13-34`).
- Classify textures with m52's conjunction test.
- For each measured target mesh asset, follow mesh → material → parent chain, and report:
  - the chain's tokens: `BLEND_Masked`, `BLEND_Translucent`, `TwoSided`, `bOverride_TwoSided`,
    `SAMPLERTYPE_Normal`, `WorldAlignedTexture`, `MaterialExpressionVertexColor`, `WorldPositionOffset`,
    `RuntimeVirtualTexture`, `MSM_*`;
  - each reached texture's class. `TC_Normalmap` or a normal `TEXTUREGROUP_*` token means normal;
    `TC_Masks` / `TC_Grayscale` / `TC_HDR` means data.
- Roots:
  - StackOBot `Content` + `/Engine/BasicShapes`;
  - Lyra `Content`, `ShooterCore`, `ShooterMaps`, `LyraExampleContent`.

**Both-ways controls, and what they showed.**

- **Normal classifier:** the flag and the name heuristic agree on 31 of 32 StackOBot normals (20 of 20
  by name on Lyra). **It discriminates.**
- **`VirtualTextureStreaming` token:** 127 / 134 StackOBot textures vs **2 / 188 runtime-virtual**
  (m52 080-04 §5). **BLIND** — it is a registry tag name.
- **`SRGB` token:** present on every texture, including known diffuse maps. **BLIND**, same reason.

**Declared weaknesses** (they travel with every row of §1.2 / §6.1):

1. **Asset defaults, not placed-component overrides.** m52's `CB_GateLevel` lesson (the scan tool's
   weakness 6).
2. **A chain's defaults include textures the instance overrides away.** `T_Weapon_D` is `M_Weapon`'s
   default while `MI_Weapon_Rifle` uses `T_Rifle_D`. `GetUsedTextures` at runtime resolves this; the
   scan does not.
3. Head truncation: 165 StackOBot and 891 Lyra files exceeded 256 KiB.
4. A token is a reference, not proof of a sample.
5. **Nanite status is not read by this scan.** It is quoted from the project's earlier
   `nanite_signature_scan` records (CLAUDE.md 072 / 074 blocks) and from m52's measured
   `target_pixels`.

---

## Appendix B — engine citation index

All paths are relative to `Engine/Source`, UE 5.1.1.

| topic | citation |
|---|---|
| `GetUsedTextures` (UMaterial / MI), uniform-expression source | `Private/Materials/Material.cpp:1069-1158`; `MaterialInstance.cpp:1012-1119`; `MaterialShared.cpp:902-941`; `MaterialShared.h:732` |
| Silent empty list with no shader map | `MaterialShared.cpp:929-935`; `Material.cpp:1073`, `:2310` |
| Parameter resolution through the instance chain | `MaterialUniformExpressions.cpp:1477-1483`; `MaterialInterface.cpp:828-837`; `MaterialInstance.cpp:935-978` |
| `GetAllTextureParameterInfo` (cooked) | `MaterialInterface.h:568`; `MaterialInterface.cpp:985-1010`; `MaterialCachedData.cpp:650-693` |
| Graph / property chain is editor-only | `MaterialInterface.h:766-784`; `Material.cpp:5647-5687`; `Material.h:296-382` |
| `IsPropertyConnected` (cooked) | `Material.h:1739`; `Material.cpp:3961-3964` |
| Texture runtime fields | `Classes/Engine/Texture.h:1298`, `:1310`, `:1327`, `:1357-1359`, `:1720-1724`; `Texture2D.h:55-61`, `:141`, `:147`, `:150`, `:302`; `Texture2D.cpp:1210-1224` |
| Sampler codegen, hardware sRGB | `HLSLMaterialTranslator.cpp:5740-5800`; `[Shaders] Private/MaterialTexture.ush:144-171`; `StreamableTextureResource.cpp:98-113` |
| `UnpackNormalMap` (`.ag` vs `.rg`) | `[Shaders] Private/Common.ush:1373-1384` |
| No runtime sampler / VT validation of overrides | `MaterialInstance.cpp:3408-3439`, `:1209-1292`, `:2773-2776` |
| VT across the boundary ⇒ black | `MaterialShared.cpp:3711-3752`; `MaterialUniformExpressions.cpp:953-1000`, `:1230-1239`; `Texture2D.cpp:1335-1337`; `RHI/Public/RHIResources.h:2012-2025` |
| MID create / parent rules (no MID parent) | `MaterialInstanceDynamic.cpp:67-127`; `MaterialInstance.cpp:779-804`, `:3062-3117` |
| MID never compiles (parent shader map) | `MaterialInstance.cpp:666`, `:1695-1696`, `:2122-2124`, `:238-244` |
| MID setter cost; unknown parameter; null texture | `MaterialInstance.cpp:390-416`, `:3339-3346`, `:3415-3428`; `MaterialShared.cpp:82-87`, `:4000-4044` |
| `CopyParameterOverrides` | `MaterialInstanceDynamic.cpp:484-503` |
| Usage flags → default material in game | `Material.cpp:1660-1823`; `MaterialInstance.cpp:1411-1438`; `StaticMeshRender.cpp:2225-2228`; `SkeletalMesh.cpp:5788-5802`; `InstancedStaticMesh.cpp:1420-1423`; `SplineMeshSceneProxy.cpp:76-79` |
| Nanite audit / fixup / override | `Rendering/NaniteResources.cpp:565-567`, `:1918-2066`; `StaticMeshComponent.cpp:2264-2268`, `:2660-2675`; `MaterialInstance.cpp:1748-1758` |
| Material Time node | `MaterialExpressions.cpp:5738-5742`; `SceneView.cpp:2684-2688`; `Classes/Engine/World.h:4315-4320` |
| MIC overrides honoured; MID forwards to parent | `MaterialInstance.cpp:1598-1641`, `:1987-2095`, `:4121-4164` |
| PSO precaching off; first-draw PSO | `RHI/Private/PipelineStateCache.cpp:67-76`, `:103-110`, `:2067-2094`, `:2131-2139`; `Engine/Private/PSOPrecache.cpp:18-39` |
| Default-material fallback on the render proxy | `MaterialShared.cpp:4207-4229`; `BasePassRendering.cpp:1745-1760` |
| `SetMaterial` / `GetMaterial` / `GetNumMaterials` | `Components/MeshComponent.cpp:30-109`, `:195-198`; `StaticMeshComponent.cpp:2617-2678`; `SkinnedMeshComponent.cpp:1242-1264` |
| Streaming: component info, fallback, cooked-only branch | `StaticMeshComponent.cpp:869-882`, `:1101`; `MeshComponent.cpp:496-514`; `Streaming/TextureStreamingBuild.cpp:643-727` |
| Streaming: `SetMaterial` keeps the static entries | `MeshComponent.cpp:76-82`; `StreamingManagerTexture.cpp:1054-1064`; `DynamicTextureInstanceManager.cpp:183-198` |
| Streaming: per-texture max; wanted mips | `AsyncTextureStreaming.cpp:148-213`; `TextureInstanceView.cpp:510-511`; `StreamingTexture.cpp:188-503` |
| Tiling lowers the needed mips | `MaterialInterface.cpp:1373`; `[Shaders] Private/DebugViewModePixelShader.usf:86-96` |
| Force-resident semantics and cost | `StreamableRenderAsset.h:122-127`, `:161-162`; `StreamableRenderAsset.cpp:218`; `AsyncTextureStreaming.cpp:269-276` |
| `DrawMaterialToRenderTarget` (cooked, no flush) | `Private/KismetRenderingLibrary.cpp:152-217`; `UserInterface/Canvas.cpp:2056-2073`; `UserInterface/CanvasItem.cpp:475-518` |
| Tile VF (one UV); base pass into target 0, no clear | `Public/CanvasTypes.h:1052-1060`; `TileRendering.cpp:89-149`, `:272-291`; `Renderer/Private/Renderer.cpp:125-426` |
| UI domain has no local-VF shaders when cooked | `LocalVertexFactory.cpp:280-289` |
| Emissive written; opaque A = 0; translucent alpha | `[Shaders] Private/BasePassPixelShader.usf:1392`, `:1437-1441`, `:1884-1886`, `:1916`; `[Shaders] Private/LightAccumulator.ush:101`; `BasePassRendering.cpp:261-279` |
| Render-target formats, sRGB, mips | `Classes/Engine/TextureRenderTarget2D.h:18-67`, `:225-247`; `TextureRenderTarget2D.cpp:29-62`, `:583-719`; `D3D12RHI/Private/D3D12Texture.cpp:1407-1410` |
| Render target accepted as a texture parameter | `TextureRenderTarget2D.cpp:81-84`; `MaterialUniformExpressions.cpp:955-1000` |
| `DXT5_NORMALMAPS` source and default | `ShaderCompiler.cpp:6056-6059`; `Core/Private/HAL/ConsoleManager.cpp:2842-2847` |
| Missing TexCoord[1] per VF | `[Shaders] Private/LocalVertexFactory.ush:614-622`; `LocalVertexFactory.cpp:481-499`; `GPUSkinVertexFactory.cpp:482-492`, `:608-627`; `[Shaders] Private/Nanite/NaniteAttributeDecode.ush:390-461` |
| UV-channel count: `GetNumUVChannels` is 0 when cooked; use `GetNumTexCoords` | `StaticMesh.cpp:5150-5162`, `:3443-3451`, `:911-914`; `StaticMesh.h:1525-1530`, `:1747-1750`; `Rendering/SkeletalMeshLODRenderData.h:250-253` |
| CPU vertex data not kept (static) | `StaticMesh.cpp:498-505` |
| Cooked `CachedExpressionData`; MIC/MID return the base parameter list | `Public/MaterialCachedData.h:177-338`; `MaterialInterface.cpp:155-190`; `MaterialInstance.cpp:874-889` |

</details>
