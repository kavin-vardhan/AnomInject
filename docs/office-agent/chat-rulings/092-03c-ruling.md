# 092-03c ruling: Proof A (blurry on virtual textures) is GREEN

Date: 2026-10-06 ~01:15 IST. Author: Claude Code Desktop (orchestrator).
- Report: `_reviews\092-03c-codex-vt-proxy-blur-report.md` (run `_relay\runs\2026-10-05-092-03c-vt-proxy-blur-resume\`; Astra max, full access, 30,882 s, exit 0, VERDICT: GREEN).
- Branch `feat/vt-proxy-blur` pushed at `af79520` (tested runtime source `c80ef9b`). Release `d7eee97` untouched.

**Disposition: ACCEPTED as a bounded proof.** It is not a delivery and is not merged anywhere.

## What I checked myself
- **Refs.**
  - origin `feat/vt-proxy-blur` = `af79520`; `master` = `b5f15a3` and `release/2026-10-05` = `d7eee97`, both unchanged.
  - No tag contains any new commit.
  - The build host plugin `E:\IA_BuildCache\_r53_host\StackOBot\Plugins\AnomalyInjector` is at `23098fa` with a clean status.
- **Identity, native 1920×1080 (`fixture-native-final-06`).** I recomputed it from the PNGs: full-frame mean absolute difference against frame 100.
  - The null (0–119) peaks at 0.418, the bound span (120–299) at 0.420 and the restored span (300–479) at 0.420.
  - Codex's own ROI result: target max 1.229 ≤ limit 1.273, with 0 failures and both controls passing.
  - The swap at frame ~124 (log: `prefetch_ready attempts=2 pages=341`, then `installed ... drop=0`) is invisible.
- **Positive edges, recomputed.** In `positive-staged-targeted-06` and `positive-pie-auto-06`, frames 120–167 sit at about 14.44 full-frame MAD and frames 119 and 168 are at the null (~0.42). The annotation says `start_frame 120, end_frame 167`, `stuck_mip_route: proxy_vt`. That is 0 frames off at both edges. The PIE Auto leg flags 168 as `pie_end_settle`; that is conservative, and the pixels there are already restored.
- **Visual check.** Frames 100/140/170 of the staged leg. The target's fine pattern goes soft during the event and comes back exactly. The co-user and the ordinary control never change.
- **Code** (`AnomalyPrivateVT.cpp` @ `c80ef9b`):
  - It uses the exported constructor with borrowed built data and a strong source pin.
  - It checks layout, format, tile and border against the source.
  - Prefetch requests and loads pages only on the private allocation, then reads the private page table back and requires every entry at the exact level before binding.
  - The 512-page, 120-attempt and 10 s bounds refuse as `prefetch_incomplete`.
  - Release is fenced.
  - The bench-only skip switch is compiled out of Shipping. There are no comments (repo convention).

## Accepted
- **Identity under the unchanged frozen criterion, with no fallback used.** Both the original fixture and the native 1920×1080 fixture pass, including the bind and restore edges.
- **21 positive fires** (PIE and staged, targeted and Auto): 0 unflagged E1 edges, target-only masks, annotation == labels, co-user isolation and clean revert. The 10-cycle lifetime and mixed VT + ordinary binding pass, and so do destroy, displace and world-end.
- **Can-fails bite.**
  - The no-bind mutant fails the positive checker.
  - The no-request prefetch refuses.
  - The release build refuses the target.
  - Stack, unnamed-sample and flat-texture refusals produce zero events.
  - Reader and audit mutants are caught.
- **Regression.** Hold, proxy, UV and normal 3/3 each, 0 unflagged. Gates are 0/0, including strict include. The lever audit passes and Shipping excludes the bench.
- **Failed intermediate designs are retained honestly** (`FE338251` stale page table, `0F79605D` uniformity refusal).

## Disclosed deviation, accepted
The first `RunUAT.bat` cook ran BuildUAT. That wrote one generated `AutomationTool.csproj.AssemblyReference.cache` and five empty `BuildWithSkipAnalyzers` markers under `D:\UESource\UnrealEngine`. These are generated build intermediates, not engine source or binaries (`AutomationTool.dll` hash unchanged). There was no "before" backup, so leaving them is right. Later cooks used `-nocompile`. No action.

## Scope limits (carry forward; not defects of this proof)
1. **Single-layer VT stacks only.** A material whose VT samples merge into one multi-layer stack (for example base colour + normal + ORM on the same UVs, which is common) refuses as `virtual_texture:stack_layout`. This is likely the main yield limit on Bates. It is folded into 092-04, question 5.
2. **Unnamed VT samples refuse** (`texture_not_parameter`), as before. Proof C territory.
3. **The coarsest blur is one 128 px tile.** Under the unchanged 8× Auto rule, the target needs about 1,024 px or more on screen. All six measured Auto picks were 1280/128 = 10× with obvious blur. This is the owner item flagged in the 092-03b ruling; no threshold was changed or proposed.
4. **`Initialize` and release each wait on a render fence** on the game thread: a small per-event stall. Watch it in the first office capture's frame timing; no action now.

## Owner items
- **Office reading on Bates with this branch** (numbers only): `IAI.TexCorrupt.Census all`, giving the blurry route counts `proxy_vt_eligible` / `auto_proxy_vt` and the `virtual_texture:stack_layout` and `prefetch_incomplete` refusal counts. These numbers size the real gain before any delivery decision.
- The 128 px / 8× yield question (above).
- Whether and when to ship it as a follow-up drop. That needs a merge plan onto a delivery line, an extended smoke test and office validation; nothing is merged now.

## Next
Launch 092-04 (design round B, read-only), as already ruled in 092-01. Its stack question is widened to cover `stuck_low_mip` proxy_vt on multi-layer stacks.

Chat review status: per AGENTS.md this is Codex's convention. Chat review NOT REQUIRED for accepting this bounded proof or for the read-only 092-04. REQUIRED (owner or chat) before any merge of `feat/vt-proxy-blur` into a delivery line.

## Amendment 2026-10-06 ~06:20 IST (from 092-05)
- `af79520` crashes at capture start (D3D12 CopyCommandList Close E_INVALIDARG, an invalid R32_TYPELESS to R8G8B8A8_TYPELESS page-table copy). Conditions: a multi-layer VT stack is in view, and either UV, normal or blurry (non-hold) is enabled or a legacy census form is run. The cause is the uniformity warm-up's private-VT page-table `CopyTexture` readback.
- The release `d7eee97` is not affected.
- `feat/vt-blur-stacks` @ `5dc8717` replaces that readback and supersedes `af79520`. Never take `af79520` to an office host.
- The office reading in "Owner items" above is replaced by: pull `5dc8717` and run `IAI.TexCorrupt.Census vt` then `IAI.TexCorrupt.Census all vt` (passive, numbers only). See `092-05-ruling.md`.
