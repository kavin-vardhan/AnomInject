> GDP Relay job (Claude Code, brief 082-03) - run folder `_relay\runs\2026-09-27-082-03-m53-delta-check` - model `gpt-6-astra` - effort `max` - sandbox `workspace-write` - duration 556 s (2026-09-27T00:53:05 to 01:02:21) - exit 0. Content below is review.md verbatim.

Independent m53 delta review — 082-03 — 2026-09-27

The revision substantially improves the design, but does not close all twelve findings. S1 is not yet ready for authorization. **Chat review REQUIRED** to resolve the remaining contract choices and accept the corrected gates before S1.

Reviewed the complete live revision and its journal at `b118a66d1cd06afe19948024ac33f20722b2cd50`, against 082-02 and chat's 082-03 disposition, with targeted UE 5.1.1 source checks. Local and origin m53 refs match that commit; the checkout remains `m51` at `53bf725`. Below, **L** means lines of `b118a66:docs/predictions/2026-09-26-m53-uv-normal-corruption.md`, not the checkout. RESOLVED means resolved **in design**, not experimentally qualified. No build, cook, launch, capture, repository write or bench change occurred.

**Finding-by-finding disposition**

| 082-02 finding | Delta disposition |
|---|---|
| **P1-1** active bindings | **RESOLVED** for the reported alias/permutation defect: resource-specific entries retain parameter identity, with ByInfo overrides and alias/layer fixtures (L112–180, L949). The separately disclosed dead-output limitation at L163–167 remains a limitation, not proof of output dependence. |
| **P1-2** texture-resource fidelity | **PARTIAL.** Format allowlisting, full dimensions, budget refusal and sampler matching improve the contract (L219–306). Regenerated lower mips remain unresolved (L310–342). N1(b) admits an undetectable failure class rather than implementing chat's refusal; N1(a) is a proposal, not yet the operative design (L1048–1056). Transformed fidelity is only diagnostic (L951). |
| **P1-3** residency | **PARTIAL.** Zero wait, no forcing, valid-resource/pending checks and snapshot metadata are specified (L361–409). However, L389–411 admits a runtime-biased chain with `snapshot_mip > 0` while calling it the full cooked chain. Chat should explicitly settle the required cooked index; otherwise refuse when that required index is unavailable. This is a contract ambiguity, not a return to force-residency. |
| **P1-4** host MIDs | **PARTIAL.** Clone support is removed (L439–454), but S3 checks only `Raw`, allowing an asset-default MID through (L494, L699–701). See Δ1. |
| **P1-5** Route A fallback | **RESOLVED.** No automatic takeover; corruptor-specific and shared-path failures have distinct scope/stop decisions, with restrictions returned to chat (L874–886). |
| **P2-6** repeated clears | **RESOLVED.** Every draw clears, then draws/generates mips; snapshot and output are distinct; drift costs include the work (L615–636). The new gate's drift criterion needs the separate correction in Δ3. |
| **P2-7** restore/ownership | **RESOLVED.** Raw versus effective state, strong originals, Nanite inspection, ownership checks and extended lifecycle cases are specified; cleanup is distinguished from exact restoration (L699–745, L956). |
| **P2-8** atomic coherence | **PARTIAL.** Caps no longer trim and small maps remain in admitted sets (L522–545), but T3 removes whole LOD groups *within* a selected slot (L507, L519). A patterned UI-group mask can remain fixed while its albedo moves. Proposed fix: refuse such a slot unless the excluded binding is proven outside the promised spatial map set; a group tag alone does not prove that. |
| **P2-9** refusal tree | **PARTIAL.** Ordered aggregation, readiness reasons and separate counters are substantial fixes (L464–585). But a `Virtual` entry fails T1's `Standard2D` requirement before reaching T2 (L505–506), producing `unsupported_type` instead of the promised `virtual_texture`. Proposed fix: classify virtual entries first and give that branch a reachable fixture. |
| **P2-10** gates | **PARTIAL.** Q/D/O classes, counts, runnable host controls, Lyra success requirements and pending cost acceptance are present (L922–967). Remaining problems: Δ2–Δ3, and incomplete encoding fixtures—L901–915 does not enumerate the admitted linear DXT/BC7/BGRA8 cases or BC5-as-data from L219–222. Specify each applicable corruptor/format/sRGB-class row. |
| **P2-11** readiness/prewarm | **RESOLVED.** The helpers are split; transaction preconditions, enqueue ordering and rollback are specified; packaged PSO readiness is not inferred from shader completeness. Cold cost remains an explicit owner decision with m55 held constant (L644–670, L770–824). |
| **P3-12** strength prior | **RESOLVED.** `expected_strength_class` is explicitly a mode prior, universal content claims are withdrawn, and m55 remains outside runtime decisions (L830–838). |

**Exactness of the revised rulings**

D2 implements a seeded bijection and removes `channel1` (L89–90, L674–687). D6 implements the required helper split and PSO distinction (L770–804). D1 implements zero wait and removal of automatic fallback, but the runtime-bias interpretation above needs confirmation. D7's unconditional refusal is consistent with the revised default, but its predicate is incomplete (Δ1). Thus the four rulings are not yet implemented exactly as a complete admission contract.

**New defects introduced by the revision**

- **Δ1 — P1: raw-only MID refusal has a bypass.** With no component override, a host can put a runtime MID in the mesh's material array. `Raw` is null, `Resolved` is that MID, and S3 passes; step 6 then attempts to parent a new MID to it (L494, L650, L699–701). UE permits access to the asset material array but rejects MID parents: `Engine/Source/Runtime/Engine/Classes/Engine/StaticMesh.h:898–903`, `Private/Components/StaticMeshComponent.cpp:2659–2667`, and `Private/Materials/MaterialInstance.cpp:3076–3083`. This can replace host rendering with the default-material path. Proposed fix: test **Resolved** for MID status and add the null-override/asset-MID refusal fixture.

- **Δ2 — P2: the sRGB wrong-copy control need not be wrong.** L931 changes the target to linear; L950 requires a difference of at least 16. A decoded linear value written to a linear RT and sampled linearly can preserve the same value as an sRGB write/read pair, apart from quantization. A valid instrument can therefore be declared invalid. UE's D3D12 path selects both RTV and SRV formats from the same sRGB flag (`Engine/Source/Runtime/D3D12RHI/Private/D3D12Texture.cpp:1407–1410`). An arithmetic counterexample with source bytes 64/128/192 round-trips to those same display bytes through linear RGBA8; this was not a rendered test. Proposed fix: inject an actual encoding mismatch, such as deliberate double encoding with target flags held fixed, and predeclare a sensitive fixture. Assign each fault to its applicable encoding/geometry/redraw row rather than requiring unrelated faults to fail every row.

- **Δ3 — P2: new gate requirements conflict with the staged behavior.** S1 is identity-only (L976), but G-BIND requires a strong mode and a changed layer region (L949). Correct identity cannot supply that evidence. G-RD also applies its unchanged-versus-NoApply and frame-N-equals-frame-1 criteria to `drift` (L952), which would reject correct motion. Proposed fix: include one bench-only nonidentity binding probe in S1; separate identity-redraw stability from drift fidelity against a reference at the same offset/time.

**S1 readiness**

The scope and Q/D/O distinction are much clearer, and the failure policy correctly holds on invalid instruments or fidelity/lifecycle failures. Minimum applications and the no-fallback rule are useful safeguards. Nevertheless, the S1 brief is not yet executable as written.

N1 and N2 are explicit blockers, and **N4 also requires acceptance before S1** (L987–988, L1066–1068). Selecting N1(a) requires a concrete per-mip source/index/count contract—including sources with no lower mips—scratch-memory accounting and revised dependency/build gates; G-BIND/G10 still require unchanged dependencies (L949, L965). N1(b) would require an explicit purity/scope ruling, not merely disclosure. N3 can retain a declared 64 MiB baseline for S1, consistent with L1064–1065.

The proposed next step is a bounded correction covering these choices, Δ1–Δ3, the complete encoding/control matrix, and the remaining tree/coherence gaps. S1's stop column should name **every S1 Q failure**, including G-ID-M, G-RD and G-REASON, consistently with L892–893. Cost acceptance and default-on remain later owner decisions; none is implied by this delta review.

VERDICT: CHANGES REQUIRED
