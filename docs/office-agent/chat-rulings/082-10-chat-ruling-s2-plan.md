# 082-10 — Chat ruling on the S2 plan (NEEDS-DECISION): defaults accepted; the MainWorld shadow trace moves up front as S2-0; Codex design review before any S2 code

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `f6b47d4` · Plan `docs/predictions/2026-09-28-m53-s2.md`

## Accepted for design review (not yet for implementation)
- **The R6 placement table.** All 22 items and Codex's holes are placed, with reasons.
- **Cause read (a), T10.** The first streaming pass has no instance data for Movable fixture meshes, so the "unknown reference" rule wants the full chain.
  - The producer is `PauseTextureStreaming` in the frame-1 batch: 7/12, not pending, no bias change.
  - One `InvestigateTexture` probe must settle "kept or re-requested" **before** the lock.
- **Cause read (b).** MainWorld's frame index, time and view are bit-identical across launches; the differences are four animated zones plus streaming-timed speckle.
  - No S2 Q row judges MainWorld pixels. Region R is declared from nulls.
- **The premise audit (§7)** and its design changes: G1 synth, the four G5 fixes, T10, the fixture producers for `no_normal_map` and `normal_unconnected`, the TreatUnfocused lever, the new-world reading, the renamed successor component, excluding scramble's no-op pairs, and the seed test with the census off.
- **1 build pair and 1 cook.** The cook is for fixture additions only; the plugin assets already carry every mode.

## Decisions
1. **The premise window: ADOPTED.** Read-only probe legs on the new build, then a lock amendment, before any gate leg. This is the direct answer to S1's four mid-campaign stops.
2. **The optional plumbing block (force-predicate): INCLUDED, labelled plumbing-only.** It never counts as live-refusal evidence for its reason.
3. **No per-fire `strength` argument: ACCEPTED.**
4. **G8's pre-m53 comparison:** use a matching banked run if one exists. Otherwise one pre-m53 leg in the premise window.
5. **Mode parameter values** (tile × 8, drift 0.25 UV/s, scramble K = 8, flat 1.0, noise 0.5): **ACCEPTED as defaults.** S3 G-STR may retune them by declared amendment.
6. **D5, the default mode sets:** the proposal is provisional (uv tile / scramble / drift; normal invert / green_flip / noise; swap and flat off). It is the owner's product call **at S3, with G-STR numbers**. Not asked now; it changes nothing in S2.
7. **m52's shared-texture check can't see an m53 actor: ACCEPTED as a recorded limitation,** provided the plan cites source for "cannot arise inside a capture run". Codex checks that claim.
8. **`IAI.Bench.DestroyTarget` compiles into Shipping:** S2-1 excludes it from Shipping, together with every new lever that lives outside `AnomalyBench`, proven from source. This goes on the M3 packaging checklist too.
9. 🚨 **The MainWorld post-revert shadow trace is treated as a potential restore defect on real game content, so it moves up front as S2-0.**
   - After an identity install and revert, the ramp's contact shadow stays different (max 67–76, 43–55 px) through si 99, while the non-installing legs agree within 1.
   - If this is real, "nothing left behind" is false on host content, a G3/G4 shape. If the cause is `SetMaterial` on static geometry, it may affect **every shipped material-swap anomaly**.
   - **S2-0 runs P-VSM on the current staged S1 build, before any S2 source work,** so a fix can go into S2-1. It uses:
     - identity and NoApply with `r.Shadow.Virtual.Cache 0`;
     - `corrupted_texture` (an existing, shipped material swap) on the same floor with the cache on;
     - a non-installing control pair.
   - Predictions are declared first. The reading is the ramp contact-shadow region, max and px ≥ 16, per si through si 99.
   - **Decision table:**
     - **the trace disappears with the cache off:** a shadow-cache invalidation;
     - **`corrupted_texture` shows it too:** a pre-existing, plugin-wide restore issue, escalated to the owner as a shipped-anomaly finding;
     - **only m53 shows it:** an m53 restore defect, fixed in S2-1.
10. **The Codex design review of this plan, via the Relay, comes BEFORE S2-1.** The plan's brief sequence omitted it; it goes back in. The review also looks at the shadow-trace evidence and the P-VSM design.

## Sequence
1. **082-11 (medium, fresh session):** the Codex design review of the S2 plan. Bench-free.
2. **082-12, S2-0 (high):** the P-VSM probe on the current build. **It needs the owner's PC OK.**
3. **Chat rules** on both, then S2-1 source.
