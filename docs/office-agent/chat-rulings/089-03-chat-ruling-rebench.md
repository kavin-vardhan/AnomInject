# 089-03 — Chat ruling on the fix-branch re-bench (NEEDS-DECISION): stuck_low_mip, blinking, lod_popping and MASK55 are PROVEN; camera_clipping AA-off accepted on the confirmed-hold reading; E2's drift FAILs are E2 reference confounds

Date: 2026-09-30 ~04:20 IST · Author: chat Claude (owner asleep) · `fix/m52-label-timing` `d1ab068` · Pairing `8A6074AA` + `62633720` · Frozen E1 `8e48e883…`

## Accepted (prospective confirmation of 089-01)
- **stuck_low_mip: SYNC-PROVEN** on AA on (B0, B3 auto-pool) and AA off (B9). **E1 and KIT-GT both PASS every event.**
  - The onset carries 2 record-backed partial frames (3 in MASK55A; 4–5 on warm-ups), flagged `partial`. Flagged-invisible frames: 0–10 per leg.
  - The offset is +4..+6 under TAA, inside the 16 off-frames (read back as 16); exactly 0 with AA off.
  - **The 1,800-frame aligned null covered every leg; no event was unjudgeable.**
  - The timing and purity levers FAIL as required.
- **blinking: SYNC-PROVEN** in both tick orders on all three tools (G405 is confirmed as the old E1's artefact).
- **lod_popping: SYNC-PROVEN on the fixture:** N and D variants, AA on and off, both orders, 23/23 per leg at +0/+0 on all three tools; **0 of 46 dithered edges faded** (G372 confirmed).
  - Realistic content (the rocks) is too faint to measure. The mechanism (a forced LOD in the same frame's proxy) doesn't depend on content, so the fixture proof stands, and the readme states it.
- **MASK55A: VALID and PASS:** peak live 2, 6 recycles, 0 aliasing, 0 prior collisions, **199/199 labelled frames masked.** F4 is now proven at runtime.
- **Part 0: PASS on both packages,** with 0 prompts.

## Rulings
1. **camera_clipping with AA off: adopt the confirmed-hold reading** (proposed in 089-02a before the run and declared as a second reading). It judges the frames the label claims as **confirmed**: 0 off and edges 0/0.
   - The as-built gate is structurally unjudgeable, because unconfirmed frames change no pixels.
   - **The 20 unconfirmed frames (D 0) are false positives by design, and they are flagged.** The readme's recipe tells clients to drop `camera_clipping_unconfirmed` frames.
   - **camera_clipping is SYNC-PROVEN on AA on and AA off** (confirmed frames).
2. **E2's scene-drift FAILs on B0, B3 and MASK55A:** these are **E2 reference confounds,** the same class 089-01 established for B9.
   - Every one carries E2's own drift note, and E1 and KIT-GT both pass those events against the game-time-aligned null.
   - E2 (086-01) is retired as a release instrument for stuck_low_mip on scenes with a sharpness cycle. It stays as a second reading elsewhere.

## Status per delivered anomaly (04:20 IST 30 Sep)
| anomaly | label sync |
|---|---|
| blinking, missing_object, missing_texture, corrupted_texture | **PROVEN** |
| camera_clipping | **PROVEN** (AA on + AA off confirmed frames); unconfirmed frames flagged |
| stuck_low_mip | **PROVEN** (partial / TAA frames flagged). **Yield on real content is still open** (the office counts) |
| lod_popping | **PROVEN on the fixture** (real-content pops too faint to measure) |
| uv / normal corruption | **PROVEN on the fixture, 4 modes**; MainWorld legs next (089-02b → re-bench) |
| masks | **PROVEN** (recycling and no aliasing at runtime) |

## Next
- **089-02b (the m53 re-bench prep; xhigh), released now.**
- **Then the m53 re-bench** when the PC is idle.
- **Wednesday by day:**
  1. the final fix-branch source round: the `TransitionOffFrames` default 16 and the LOD log wording, plus the yield option once the owner's counts arrive;
  2. **the Codex evidence review** (merge prep);
  3. merges;
  4. the final delivery build and rehearsal, with Step 1 and §8.8 filled.
