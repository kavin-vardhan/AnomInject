# 089-02b — Chat ruling on the m53 re-bench prep (NEEDS-DECISION): uv-only MainWorld under the declared 256 MiB bench raise; MIXED re-declared; P-ORC accepted; the dbghelp.dll block goes to the owner as a security heads-up

Date: 2026-09-30 ~06:25 IST · Author: chat Claude · m53 `bf0ac20` · Pairing DC2: exe `6B671987` + utoc `45E2FA86` (archived) · Harness `_reviews\089-04-*`

## Accepted
- **G403 fixed:** `final` is parsed as `head[:detail]`, and the census names SM_Ramp2 (214 MB) and RoomBuilderSquare (220 MB).
- **P-ORC root cause:** the filmic tonemapper mixes channels, and vignette, dither and bloom widen the spread.
  - The fix: `ShowFlag.Tonemapper 0`, plus `ShowFlag.Lighting 0` for normal, read back per leg (a leg without the read-back is INVALID); the linearised normal readout; the reader's geometry fallback.
  - Reader selftest 39/39; 16/16 and 64/64 cells are found on the night's frames.
  - **The separation floor (32) equals G-MODE's rule, with no margin.** It stays as declared, with no threshold change.
- **G-BIND:** the cause was the recipe (the S1 AA-off arbiter was lost), not the fixture. Recipe read-back is now enforced (INVALID-RECIPE otherwise).
- **G-LEVER-OFF, G-CENSUS (one command per frame, `reasons=-`), and the arm classifier** (CD3/RD3 = +1, CD5/RD5 = +3; "proven" = the leg fails with the expected edge offset by exactly k on ≥ 4 events and the other edge at 0).
- **The frozen E1** (`8e48e883…`), loaded only after a hash match. The MainWorld nulls use the same schedule and run right after each leg.
- **The dry run:** 44/44 accepted, 13 NOT-RUN-PREMISE exactly as predicted, selftest 68/68.

## Rulings
1. **L2 cannot frame MainWorld** (chat's premise was wrong: `PlaceView` only knows CB_GateLevel and L_ShooterGym). **Accept uv-only MainWorld legs under the declared bench-only 256 MiB raise** (SM_Ramp2 / RoomBuilderSquare chosen by the identity-fire probes, then the lock).
   - **Normal-family real-content legs stay unproven:** NOT-RUN-PREMISE, reported as a gap.
   - **Why no new build:** the timing mechanism is the same material swap for both families, and uv on real content plus normal on the fixture cover it. Normal's real-content visibility rate goes to the owner's Thursday office look and the readme ("not measured on realistic content").
   - The delivered default stays 128 MiB.
2. **MIXED: accepted as re-declared.** The cause is slot 0's leftover `BasicShapeMaterial` override, not slot 1. **Chat's premise was wrong again;** Code's correction is recorded. Predictions: MIXED_N `partial_footprint:1/2:no_normal_map`, MIXED_UV APPLIED.
3. **`dbghelp.dll` is refused PC-wide since about 03:00:** Defender's configuration changed at 02:59 and real-time protection went off at 03:03, not by any brief.
   - **This is the owner's PC and a security matter, so chat does not work around security settings.** DC2's container taken from staging after a clean packaging step (exit 0) is **accepted for the bench only.**
   - **The owner is asked to check Windows Security** (what now protects the PC, protection history, any newly installed or updated security product) before the delivery BuildCookRun. The delivery build waits on his answer if packaging still fails.
4. **P-ORC:** the show flags are unprovable offline, and a flag that doesn't take effect makes the leg INVALID, never a pass. Accepted. The geometry fallback is accepted.
5. **G406 for m53:** the aligned null feeds the wrong-object rule, the AA-off arbiter and the per-frame reading on any E1 failure (E1's game-time alignment stays stuck_low_mip-only). Accepted.

## Next
- **089-04 run (the m53 re-bench; high):** released now, gated on the PC being idle; it gives up at 11:00 IST if the PC isn't free. About 67–80 min.
- **By day:**
  - 090-01: the final fix-branch source round (the `TransitionOffFrames` default 16, the LOD log wording, and the yield option when the owner's counts arrive);
  - the Codex evidence review;
  - merges;
  - the delivery build (after the owner's dbghelp check).
