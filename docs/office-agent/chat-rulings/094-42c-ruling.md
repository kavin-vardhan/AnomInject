# 094-42c — Chat ruling: GREEN. The leak is proven at home (including the outline of a hidden object), and the guard holds. The depth-only route's direct link fails, so 094-43a proves the name-lookup form before the FZ2 build

**Date:** 2026-11-03, about 23:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-42c:** GREEN. It ran 17:04–23:25 IST (380 min).
- `_reviews\094-42c-codex-report.md`, `_reviews\094-42c-code-review.md`, and `_reviews\094-42c-example-frame746-hidden-blink-outline.png`.

## Verdict
**GREEN.** All four gates pass, and Code re-checked each against the raw files with its own readers:
- **P9 v3:** exactly v2 minus the deleted overflow folder.
- **The repro:** the outline leak shows on every tagged object.
- **The guard:** it refuses before any tag.
- **FZ6(b):** measured.

## Rulings

### GC1. Accepted: P9 v3 and the kit lines (`de29514`), plus the evidence commits `0b8baca` and `d840cdb` (local, unpushed)
- **The tests:** P9 v3 was tested by calling `derive()` and running the inventory and missing-file code copied out of `main()`. That is accepted under FJ1.
- **Still owed:** the full `owner_p9.py` `main()` (rehash, toolchain and fsck checks) runs once, start to finish, at 094-43's entry, as a gate.

### GC2. Accepted: FZ5's repro and guard gates. The fixture becomes the home gate for FZ2
- **The repro:** the control has 0 magenta pixels, the untagged frames are pixel-identical, lod_corruption shows outlines on 404 of 404 frames and blinking on 222 of 222. Frame 746 is the worst case from 094-41: the outline of an object the labels say is hidden.
- **The guard:** `C5 refused: host custom-depth reader`, `tagged=0`, custom depth left at mode 1, no frames written.
- **The outline material stays as it is** (Decision 3). It fills the tagged floor rather than drawing a constant-width edge, which makes it a stricter detector: any custom-depth write lights up thousands of pixels. FZ2's gate is "no new magenta against the outline-off control", and a fill catches more than an edge would. No new fixture material.

### GC3. Accepted: FZ6(b) as measured
- **The direct link fails.** Naming `StaticType` for `TDepthOnlyVS<false/true>`, `FDepthOnlyPS<false>`, `FDepthOnlyHS` and `FDepthOnlyDS` fails with 5× LNK2001 plus LNK1120. Those symbols are not exported from the installed 4.25 Renderer.
- **The fallback's cost:** a new `FMeshMaterialShader` type re-requested shaders for 47 materials and took about 2,884 s (48 min) extra on first launch, with 2 workers. This is one pair of runs, and Europa's material count will make it larger.

### GC4. The route: depth-only stays preferred, in a form that never names a private symbol. 094-43a proves it before any FZ2 build (Decision 1)
- **The form:**
  - Find the depth-only shader types **at runtime, by name**, through RenderCore's exported shader-type registry.
  - Take them from the material's own shader map for the mesh's vertex factory, through `FMaterial`'s exported shader lookup.
  - Fall back to the default material exactly as the engine does when a shader is missing.
  - Bind and draw them through the exported mesh-pass machinery: `FMeshPassProcessor`, `FMeshMaterialShader::GetShaderBindings` and `DrawDynamicMeshPass`.
  - Draw into **plugin-owned** depth and ID targets, from the existing view-extension hook.
  - No `bRenderCustomDepth`, stencil, write mask or `r.CustomDepth`, and no read or write of the host's custom-depth targets.
- **What must be proven:**
  1. It **links** (0 LNK errors).
  2. The private coverage matches the current label mask for opaque, masked, WPO and a blink-hidden target.
  3. It adds **no first-launch recompile.** A warm launch logs 0 `Missing cached shader map` lines, which is the proof that no new shader type exists.
  4. With the outline on, the saved frames carry **0 new magenta** against the control.
- **If it fails:** stop with NEEDS-DECISION, naming the exact blocker (a missing export, a binding the base class cannot fill, or a vertex-factory gap). Chat then rules the fallback using GC3's numbers.
- *Why:* the fallback costs every machine a long first-launch recompile, on Europa's whole material set, every time its shader changes. The office PC would pay that cost too. One small proof is worth a night before taking that cost.

### GC5. FZ9 runs inside 094-43a, read-only, by Code while Codex works
- **The question:** did any recorded 5.1 host scanner reading ever show a custom-depth reader?
- **The method:** home records and the `_bates_reads` typed reads only. No office data is requested.

### GC6. The kit records a stop's intent before it classifies the exit (Decision 4b)
- A deliberate stop must never be first logged as a crash.
- This is a small kit fix in 094-43a, committed with W4 on `feat/europa-port`.
- The WinError 5 seen while stopping `spike\01` stays an open note. Watch for it.

### GC7. Housekeeping
- **The spike branch** `spike/europa-09442c-mesh-shader` stays local, unmerged and unpushed. It is kept as evidence for the fallback.
- **The push** of `feat/europa-port` stays with 094-43 (scan, then push), as planned.
- **Foreign load is noted, not ours:** Resolve reached about 12.6 GB, and D: dipped to 46.5 GiB with the writer not established.
- **Cosmetic:** the duplicate ledger id `…-resume48` is noted and needs no action.

## Forward plan
1. **094-43a (now, overnight):** prove the name-lookup route on a new throwaway fixture branch, plus FZ9 and GC6.
2. **094-43:** the FZ2 implementation on `feat/europa-port`, using whichever route 094-43a settles, with FZ7 folded in. Then a scan, a push, and the new office card.
3. **The office re-test,** then Phase B.

## For the owner
- 🟢 **The leak is reproduced at home.** Even a hidden "missing" cube gets an outline drawn exactly where it was.
- 🟢 **The safety lock works.** It refuses to capture on a game like that.
- 🟡 **The cheap fix isn't confirmed yet.** One small test tonight decides it. If it fails, the backup fix works but makes the game recompile shaders on first launch, about 48 minutes on our small test level and longer on the real game. That would also happen at the office, one time.
