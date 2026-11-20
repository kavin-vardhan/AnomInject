> GDP Relay job (Claude Code, brief 082-04) - run folder `_relay\runs\2026-09-27-082-04-m53-delta2` - model `gpt-6-astra` - effort `max` (both pinned by the runner, requested and confirmed in run-meta.txt) - sandbox `workspace-write` - duration 518 s (2026-09-27T01:47:22 to 01:56:00) - exit 0. Content below is review.md verbatim.

Independent m53 delta check #2 — 082-04 — 2026-09-27

**No new P1 found. S1 is not ready as written: three P2 text corrections remain.** Chat review REQUIRED for their disposition in the S1 authorization brief.

Reviewed `49dece14707a10115fb15cb17df73b00985b6104` (local/origin agree; parent `b118a66`) through git objects, both earlier reviews, both rulings, the revision journal and targeted local UE 5.1.1 source. **L** means the amended plan blob’s line numbers. RESOLVED means design resolution, not experimental qualification. Only this report was written. No repository/bench changes, builds, cooks, launches or captures occurred.

| Item | Disposition and plan evidence |
|---|---|
| Δ1 | **RESOLVED** — resolved/effective parent walks and asset-slot MID fixture: L759–791, L848. |
| Δ2 | **PARTIAL** — genuine encoding faults replace the old lever, but the new texel-shift proof is false: L1438–1455, L1547; A below. |
| Δ3 | **RESOLVED** — S1 tile probe; identity-only redraw; separate drift reference: L1459–1463, L1475–1479, L1578. |
| P1-2 | **RESOLVED in design** — explicit source mip, preserved chain, single-mip case and scratch accounting: L363–395, L449–626. |
| P1-3 | **PARTIAL** — cooked mip 0 is enforced, but per-texture budget bias remains expressly unclassified: L677–724; C below. |
| P2-8 | **RESOLVED** — every exclusion blocks the slot; only proven 1×1 exemption: L877–919, L947. |
| P2-9 | **RESOLVED** — virtual-first ordering and producer/precedence table: L861–871, L921–963. |
| P2-10 | **RESOLVED** for the encoding gap — complete admitted-format matrix; BC5-as-data refused: L325–352, L1504–1518. |
| S1 stops | **RESOLVED** — all S1 Q failures explicitly covered: L1617–1638. |

**N1–N4 exactness.** N1(a)’s private dependencies and per-mip design implement the ruling (L275–308, L449–626). **The copy mechanism is sound for the specified cooked Windows path**, subject to its admission preconditions and unrun qualification. UE’s runtime RT allocation supports the declared mip count; the initial deferred update removes itself before drawing ([TextureRenderTarget2D.cpp:50](D:/UESource/UnrealEngine/Engine/Source/Runtime/Engine/Private/TextureRenderTarget2D.cpp:50), :682–718). Canvas End resolves without generating mips ([KismetRenderingLibrary.cpp:761](D:/UESource/UnrealEngine/Engine/Source/Runtime/Engine/Private/KismetRenderingLibrary.cpp:761)). RDG performs a same-format RHI copy ([RenderGraphUtils.cpp:245](D:/UESource/UnrealEngine/Engine/Source/Runtime/RenderCore/Private/RenderGraphUtils.cpp:245)); `NumMips=1` defaults correctly ([RHI.h:1989](D:/UESource/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHI.h:1989)), and D3D12 uses the destination mip’s subresource ([D3D12Texture.cpp:2924](D:/UESource/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12Texture.cpp:2924)). These are runtime paths. Epic’s [API contract](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/RenderCore/AddCopyTexturePass) also specifies matching formats; the version-specific conclusion rests on local 5.1.1 source.

N2’s additive assets, manifests, precook archive and delivery exclusion match (L1309–1416). N3’s 128 MiB CVar, refusal and three-budget census match (L396–413, L1489), but its diagnostic needs B. N4’s 2/16 thresholds and error histogram match (L1476); its instrument is not executable until A is corrected. The no-allocation null and direct mip-0 draw are appropriate interpretations.

**Remaining/new findings and proposed fixes**

- **A — P2, new: diagonal checker shift is an identity.** L1353 defines one-texel checkers; L1449 shifts both axes by one texel; L1547 predicts ≥128/89 error. For checker parity `(x+y)%2`, `C(x+1,y+1)=C(x,y)`. Offline arithmetic gives zero, so the mandatory proof stops S1 even with a correct copy. Proposed fix: shift one axis only, e.g. `(1/W_m,0)`, and recompute the exact fixture/readout proof before authoring.
- **B — P2, new: collateral counts can hide extra mip loss.** L1487 subtracts counts of textures that dropped since Apply, not matched textures’ mip levels. If one texture falls 13→12 in NoApply and 13→11 when applied, both counts are one; the reported difference is zero despite additional loss. Proposed fix: pair texture identities and frame offsets, compare actual resident levels, then aggregate deficits. Truncated coverage must remain incomplete.
- **C — P2, remaining exactness gap:** With `UsePerTextureBias=1`, L697–699 substitutes `not_fully_resident` for the ruled runtime-bias refusal. Budget bias can change before residency: UE [StreamingTexture.cpp:418–435](D:/UESource/UnrealEngine/Engine/Source/Runtime/Engine/Private/Streaming/StreamingTexture.cpp:418) changes `BudgetMipBias/MaxAllowedMips`, separately from resident state. Proposed fix: establish effective bias through a safe query, or conservatively refuse cases where zero bias cannot be established, with a named `runtime_lod_bias` subreason. This is a refusal-contract defect, not a demonstrated wrong mip-0 copy.
- **P3, new arithmetic:** two same-class 4096² chains share scratch, totaling **192 MiB**, not ≥213.33 (L381–395 versus L1679, L1705–1706). Correct the figures; the 128/256 admission outcomes stay the same.

**S1 ready: No, as written. Exact blockers: A, B and C.** All are plan-text corrections suitable for the authorization brief; no new mechanism or purity ruling is proposed. Builds/cooks and all runtime gates remain UNRUN.

VERDICT: APPROVE-WITH-CHANGES
