# 094-41 — Chat ruling: GREEN. The outlines are our custom-depth tagging, and they leak labels into the saved frames. Fix: a private ID/depth pass. First, an interim guard, client-only routes, an outline fixture at home and a shader-cost spike (094-42)

**Date:** 2026-11-03, about 00:30 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-41:** GREEN. It ran 22:45–23:36 IST.
- **The plan:** `_reviews\094-41-codex-plan.md`, from a fresh thread `01a19d4a`.
- **Code's review:** `_reviews\094-41-code-review.md`. All 90 citations were checked, and all held.
- **The office reads:** `_bates_reads\2026-11-02-europa-094-40-office-capture-reads.md`.
- **Queued owner decisions:** `094-41-queued-decisions.md`.

## Verdict
**GREEN.** The diagnosis is confirmed by source.
- **Four writers set custom depth during C5:**
  1. the forced `r.CustomDepth 3`;
  2. the census batches, re-tagged every request;
  3. the event targets, for the whole live fire;
  4. the street bench's three planted tags, which at the office landed on real game objects.
- **Tagging runs in the same frame whose colour is saved,** so the labelled object carries custom depth on its saved frames.
- **Hidden objects are the worst case:** for hidden objects (missing_object, and blinking while hidden), custom depth is the only thing still drawing them. A host outline therefore draws exactly the object the label says is gone.

## Rulings

### FZ1. The office capture of 11-02 is not training-clean
- Its frames must not be used or shared as data.
- They stay on the office PC (the HARD RULE). Deleting them there is the owner's choice.
- *Why:* host outlines sit on the labelled objects, which is label leakage. The run still proves the plumbing: the fires, frames, masks, annotation and exit.

### FZ2. The fix is a plugin-owned primitive ID/depth pass for both the census and the masks
The pass never sets `bRenderCustomDepth`, a stencil value, a write mask or `r.CustomDepth`, and never reads or writes the host's custom-depth targets.
- **Preferred: drive it with the engine's existing depth-only shader types.** They are already compiled for every material, so no new material shader type is added.
- **Only if that is shown infeasible:** add a new `FMeshMaterialShader` type, with its first-launch recompile cost measured (FZ6) and C5's warmup budget sized to it.
- **The same-frame scrub is not adopted.** It leaks at overlaps, DBuffer and fork passes.
- **A second scene capture** is a fixture and prototype aid only.
- *Why:* only a private pass is isolated from whatever the host does with custom depth, and that holds on any game, which is the plugin's whole contract.

### FZ3. Interim guard, until FZ2 ships
1. **C5 plants no custom depth on anything but the bench fixture's own `EA_` actors.** The planted tags are bench-only, behind `-IAIBench` plus a fixture check. They are never applied on an arbitrary level.
2. **Port the host-reader scanner to 4.25,** with `reader / writer / unknown` results.
3. **A labelled C4 or C5 refuses on `reader` or `unknown`, before any tag is set,** and logs `C5 refused: host custom-depth <result>`.
   - It never disables the host's effect to pass.
   - It stays in place after FZ2, as the fallback whenever the private pass is unavailable.

### FZ4. The owner's product call (11-02): capture routes default to the client's anomalies
- **The default:** blinking and lod_corruption. The texture bugs join after Phase B.
- **Test-only:** lighting_mismatch and missing_object. They are enabled only by an explicit route list.
- **The route list:** an optional route list on `IAI.Capture.C5` (a third argument) and on C4, comma-separated, validated against the pool. The routes used are logged and written to the receipt.

### FZ5. A home outline fixture
- **What it is:** a separate C++ 4.25 project on E:, using the installed engine, with a post-process outline material that reads CustomDepth and is authored by an editor script. Never the Paris host.
- **The repro gate:** with the d1cdc7b behaviour (the guard off), C5's saved frames show outlines on tagged objects, proved by pixel difference against an untagged control.
- **The guard gate:** with FZ3 on, C5 refuses before tagging.

### FZ6. A shader-route spike, results only
1. **From the 4.25 source, with citations:** can the private pass use the engine's depth-only (prepass) shader types and a private depth-stencil target, for opaque, masked and WPO materials?
2. **On the fixture, on a throwaway branch:** the first-launch recompile cost of adding one trivial `FMeshMaterialShader` type — the materials recompiled, and the seconds taken.
3. **Not merged.** Chat picks the route from these numbers.

### FZ7. Folded into the FZ2 implementation (094-43), not now
- **Size:** any viewport size, with pitch-aware colour and mask copies, recording the size.
- **D3D12:** qualification, including an odd width.
- **The self-check:** `size`, `RHI` and `client` become readings.
- **The card:**
  - "run the launch line exactly as written" (a quoted `-ExecCmds` runs every command on stock 4.25);
  - R1 is read from the clone the plugin was copied from;
  - keep the copied folder out of the game repo.

### FZ8. The relay: read-only jobs run on fresh review-runner threads
The full-access resume is kept only for jobs that build or launch.

### FZ9. QUEUED after 094-42: a read-only 5.1 check
- **The question:** 5.1 has the same tagging. Did the 5.1 hosts' recorded scanner readings ever show a custom-depth reader?
- **The method:** home records and typed reads only.
- *Why:* earlier 5.1 data may carry the same leak.

### FZ10. Foreign load is noted, not ours
ComfyUI and another session's renders grew D:'s page file, taking D: to 2.5 GiB free and the commit to 1.2 GiB. 094-42 uses E: for the fixture.

## For the owner
- **🟢 Found it.** Our counting and labelling light up the game's own outline effect, right on the bugged objects.
- **🔴 Don't use the office frames from 11-02 as data.** They stay on the office PC.
- **Next (094-42):** a safety lock, so we never capture on a game like that until the real fix lands; blinking + LOD by default; a home test level that has outlines; and a quick cost test to pick the cleanest fix.
