# SESSION: new-default
# NAME: 080 m52 stuck low mip
# MODEL: claude-opus-5
# EFFORT: xhigh

# 080-01 — m52 "stuck low mip" anomaly: source read + design + predictions. PLAN ONLY, no implementation.

## 0. Context
Fresh session. Bootstrap from CLAUDE.md (master `3ff88db`), then `docs/CHAT-HANDOFF-079-verifier-closed-codex-implementer.md` (§0, §3), `docs/architecture.md`, the anomaly registry and the existing texture-class anomalies (`missing_texture`, `corrupted_texture` — the material/texture plumbing pattern and how their m49 observability evidence is produced), `docs/gotchas.md` entries on materials/streaming/hitching (search: UpdateResource, streaming, hitch, GetUsedTextures, MID). Branch `feat/m52-stuck-mip` off master. `m51` untouched. Engine source: `D:\UESource\UnrealEngine` (UE 5.1.1) — cite file:line for every engine claim.

## 1. What m52 is
Anomaly `stuck_low_mip` (texture class): a target actor's main texture is held at a LOW resident mip for the event span — the object looks blurry/low-res while everything else is sharp — then restored. Owner priority for the next client release, with `flicker` (m53) and `uv_corruption` (m54) following. Game-agnostic: Core/CoreUObject/Engine only; targets by name/class; no host types.

## 2. Source read — the lever (answer with engine citations, then choose)
Evaluate, for a specific `UTexture2D` at runtime in a packaged/-game build:
L1 `UStreamableRenderAsset::StreamOut(NewMipCount)` / `StreamIn` and what makes the streamer stream it back in on the next update (wanted-mips computation, `bForceMiplevelsToBeResident`, `NumCinematicMipLevels`, `bIgnoreStreamingMipBias`, `NeverStream`, last-render-time touch).
L2 Removing the texture from the streaming manager for the span (`IStreamingManager::Get().GetTextureStreamingManager()` add/remove or the equivalent in 5.1) so `StreamOut` sticks, then re-adding — hitch profile of both calls.
L3 `LODBias` / `MaxTextureSize` + `UpdateResource()` — expected to REJECT (recreates the RHI resource; hitch; also affects every user of the texture) unless proven otherwise; measure once if cheap.
L4 Any per-texture mip-bias sampler path that does NOT touch the resource (material-level `MipValueMode`/`MipBias` on the sampler is material-side and would need the m54-style material swap — note it as the fallback if L1/L2 cannot hold the mip).
Requirements the chosen lever must meet: (a) no full-frame hitch beyond the existing mask-pass cost (measure on the bench: capture FPS + max frame time with/without); (b) holds for the whole span under normal streaming pressure (the streamer must not restore it mid-span — measure resident mips every frame); (c) restores fully on FinishRun / cancel / actor destruction / level change; (d) no global cvar or streaming-pool change.

## 3. The sharing hazard (the real design problem — plan must solve it with numbers)
A texture is usually shared by many components/actors. Blurring it blurs every visible user, but the label names ONE target. Options to evaluate and measure on BOTH fixtures (count, per candidate target, how many OTHER visible actors use the same texture in a typical view): S1 restrict eligibility to targets whose chosen texture has no other visible user in the frame (census/visibility pass already knows what's visible); S2 label every visible user as an affected target in the same event (multi-target events — check the schema/annotation supports it; additive keys only); S3 exclude shared textures entirely. Recommend one with the measured numbers (e.g. "in StackOBot 62 % of candidate targets have a unique base texture; Lyra 18 %") and state the fallback.
Texture choice per target: from the renderable component's materials via `GetUsedTextures` (runtime-safe; cite), pick the largest 2D texture that streams (skip non-streaming, UI, lightmaps, virtual textures — VT needs its own note: if the host uses VT for base colour, m52 must say NOT_APPLICABLE for that target).

## 4. Evidence (the m49 bar)
E1 **Engine fact per frame:** the target texture's resident mip count / `GetNumResidentMips()` vs `GetNumMips()` and the forced mip written into each `labels.jsonl` row for the span (new additive keys, e.g. `stuck_mip.resident_mips`, `stuck_mip.full_mips`, `stuck_mip.held`) — this is ground truth the verifier cannot give and the client can trust.
E2 **Visibility:** silhouette unchanged, pixels change ⇒ the texture-class observability rule (A1: target_pixels + change) applies as is; confirm at source.
E3 **Perceptibility floor:** a low mip only LOOKS blurry if the object's on-screen footprint is large relative to the mip's resolution. Define the rule (e.g. forced mip texel size ≥ 4 screen pixels over the target's bbox at capture resolution ⇒ observable; below ⇒ `observable:false` with reason) and put the numbers in the predictions. Without this, the anomaly is "injected but invisible" — the client's F2 complaint.

## 5. Deliverables of THIS brief
- `docs/predictions/2026-09-2x-m52-stuck-low-mip.md`: chosen lever with citations and the rejected ones with reasons; sharing policy with fixture measurements; evidence keys; perceptibility rule; console commands (`IAI.Anomaly.StuckMip.*`, mirroring existing naming), dashboard toggle, default-pool question (owner's call — state a recommendation); the gate list: bench can-fail (a deliberately non-holding variant must read `held:false` and `observable:false`), hold-through-span on both fixtures under streaming pressure (a camera sweep that would normally stream the mip back in), restore-on-every-exit, hitch measurement, Lyra gate, schema additive check, verifier consistency read (the 079 tool on the m52 session: readings only, no NO-TRACE).
- File-by-file implementation plan (registry entry, subsystem hooks, label keys, capture evidence, docs).
- Journal `docs/sessions/…-080-01-m52-plan.md`. No code. No captures except the fixture sharing/streaming measurements (read-only probes are fine). Commit the docs on the branch, push.
- Report: the plan summary, the sharing numbers, the lever decision, open questions for chat, `VERDICT: GREEN` alone on the last line (or NEEDS-DECISION).
