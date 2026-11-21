# 092-05 ruling: VT census diagnostics and multi-layer stack blur are GREEN

Date: 2026-10-06 ~06:20 IST. Author: Claude Code Desktop (orchestrator).
- Report: `_reviews\092-05-codex-vt-blur-stacks-report.md` (run `_relay\runs\2026-10-06-092-05-vt-blur-stacks\`; Astra max, full access, 14,628 s, exit 0, VERDICT: GREEN, Part A PASS, Part B PASS).
- Branch `feat/vt-blur-stacks` pushed at `5dc8717` (tested runtime `502de71`; later commits are docs/harness only).

**Disposition: ACCEPTED as a bounded proof.** It is not a delivery and is not merged. **`feat/vt-blur-stacks` @ `5dc8717` supersedes `feat/vt-proxy-blur` @ `af79520` for any further testing, including the office** (see the crash below).

## What I checked myself
- **Refs and host.**
  - origin `feat/vt-blur-stacks` = `5dc8717`; `feat/vt-proxy-blur` = `af79520`, `master` = `b5f15a3` and `release/2026-10-05` = `d7eee97`, all unchanged. No tag contains the new commits.
  - The worktree is clean. The build host plugin is at `23098fa` and clean.
  - `git diff 502de71 5dc8717 -- Source Shaders *.uplugin` is empty.
- **Identity (`stacks-identity-final-v2`), recomputed from the PNGs** (full-frame MAD vs frame 100): null 0–119 max 0.0806; bound 120–299 max 0.0836; restored max 0.0806; 0 pixels over 24 anywhere. Codex's ROI result: target 0.328 ≤ limit 0.362, with both controls passing.
- **Positive edges (`stacks-staged-targeted-final-v2`), recomputed.**
  - Frames 120–167 are at about 3.93 full-frame MAD from the very first frame (no ramp, so all three layers change together). Frames 119 and 168 are at the null (~0.080).
  - The annotation gives 120–167, 264–311, 408–455, 552–599 and 696–743, all `proxy_vt`, with 0 transition frames. That is 0 frames off.
- **Visual check** (frames 100/140/170): the roughness highlight, normal detail and colour bands all go soft together and return exactly. The co-user and the ordinary control are unchanged.
- **Part A prints numbers only.** Every new census line is `IAI-VT-CENSUS v1 family=<0 UV|1 normal|2 blurry> ...`, made of counts and `key:count` histograms. There are no asset, material, actor or level names.
- **The bad copy path is gone.** `af79520` read the VT page table with `RHICmdList.CopyTexture` (`AnomalyPrivateVT.cpp:305`). `5dc8717` reads it with a compute shader into a buffer (`AnomalyVTPageCheck.usf`, `AnomalyPrivateVT.cpp:323–346`) and resolves RHI references after queued updates.

## Accepted
- **Part A, VT diagnostics:**
  - `IAI.TexCorrupt.Census vt` (in view) and `IAI.TexCorrupt.Census all vt` (whole level). The diagnostic mode is **passive**: it reads the existing uniformity cache and starts no measurement or residency request.
  - Expected = printed on both fixtures and both scopes, and the real miscount mutants fail.
- **Part B, multi-layer stacks for blurry `proxy_vt`:**
  - One private VT per layer, with readiness proven on the **combined** material allocation and one atomic commit.
  - Identity, and 21 positive fires in PIE and staged (targeted and Auto), with each layer independently blurred (colour 34.6, normal 19.8, roughness 5.4 MAD).
  - Edges are 0/0. Co-user and controls are unchanged. Revert, 10-cycle lifetime, destroy, displace and world-end all pass.
- **Can-fails:**
  - The release refuses the target.
  - `af79520` refuses it as `stack_layout`.
  - Unnamed layer → `stack_unnamed`.
  - The one-layer-only mutant and the no-bind mutant are caught.
  - A controlled early-bind experiment produced 14 wrong onset frames, caught, while the real implementation under the same delay passes.
- **Regression:** hold, proxy, single-layer proxy_vt, UV and normal, 3/3 each, 0 unflagged. Gates 0/0 and strict include 0/0. The lever audit passes with Shipping excluded. The string scan has 209 entries, all resolved.
- **Readiness cost:** 3.6–6.8 ms per event (measured). The existing 512-entry, 120-attempt and 10 s bounds are unchanged.

## Finding against the accepted 092-03c build (`af79520`): a crash
- **What happened.** With a multi-layer VT stack in the scene, `af79520` crashed at capture start with D3D12 `CopyCommandList Close E_INVALIDARG`. The D3D debug layer showed an invalid R32_TYPELESS → R8G8B8A8_TYPELESS copy.
- **Where.** The uniformity warm-up (`KickUniformMeasurements`, `AnomalyCaptureSubsystem.cpp:1111`) creates private VTs and reads their page tables with a plain texture copy, before the `stack_layout` refusal can act.
- **When it triggers.** The warm-up runs when UV, normal or blurry (non-hold route) is enabled (`IsTexCorruptWarmWanted`). The legacy `IAI.TexCorrupt.Census` forms also start uniformity measurement.
- **The release (`d7eee97`) is not affected:** it declines VT before any uniformity measurement. The release refusal leg ran with warm-up on and did not crash.
- **`5dc8717` removes the copy path.** All final warm-ups pass on both fixtures. Codex did not isolate the exact caller. The most likely cause, consistent with its findings (a page-table reference that can resolve to the engine's black fallback, and references that change after queued RHI updates), is that the copy's source and destination formats disagreed.
- **Consequence:** never take `af79520` to an office host. This amends the 092-03c ruling's owner item.

## Scope limits (carry forward)
- Proven on a three-layer stack. The code handles complete supported layouts up to the engine's eight layers, but that is not runtime-proven.
- No preallocated/runtime-VT stack fixture was authored. Those refuse by design.
- **The legacy (active) census forms and capture warm-up still measure VT uniformity on `5dc8717`.** They passed on both fixtures, but they are untested on office content. **At the office, run the passive `vt` form first.**
- Unchanged from before: the 128 px tile floor plus the 8× rule (about 1,024 px on screen), blurry OFF by default, and UV/normal still refusing VT.

## Owner items (updated)
1. **Office reading on Bates** (numbers only):
   - Pull `feat/vt-blur-stacks` @ `5dc8717`.
   - Run `IAI.TexCorrupt.Census vt` at the usual capture view, then `IAI.TexCorrupt.Census all vt`.
   - Type back every `IAI-VT-CENSUS v1` line.

   Only afterwards, and as a test with no other work open, try a short capture with blurry switched on.
2. The 128 px / 8× question.
3. The follow-up drop decision (owner or chat REQUIRED before any merge).
4. The UV/normal VT producer and its new subtypes (092-04): an owner go-ahead is needed before any build.

Chat review status: per AGENTS.md this is Codex's convention. Chat review NOT REQUIRED for accepting this bounded proof. REQUIRED (owner or chat) before any merge into a delivery line and before any UV/normal VT build.
