# 2026-08-24 — session 059 — the stale-present display fix, and the 054 bisect re-read as TWO phenomena

**Status at close: the mask "hitch" the owner could SEE is FIXED and owner-eye-gated on the bench.
It was never a performance stall — it was a DISPLAY-ONLY stale-present defect (the mask SVE
designated the chain's final writer and ignoring `OverrideOutput`), present since m26, datasets
never affected. Branch `feature/mask-gpu-reduce`: `ead7764` (predictions AMENDMENT 1, first) →
`310a87f` (A-I1 instrument) → `b05066f` (the fix + gate record), pushed. ⛔ Still NO TAG, NO
MERGE — G-R7(ii) on Concorde, now re-specified, remains the merge gate.** Master untouched;
Monday's delivery build ships UNCHANGED per the chat ruling (the artifact remains the documented
limitation there; the client note is being amended chat-side).

## §1 How it was found (owner-driven; the eye was the instrument every probe missed)

The owner tested the m34 build in StackOBot and reported the hitch unchanged. The chain that
followed, in order, with each step's correction on the record:

1. My first t_wall reading claimed ~100 ms stalls throughout their runs. **WRONG — instrument
   artifact:** the exact-100 ms deltas are the burst cadence's scheduled 3-tick gaps (df=3 ×
   33.3 ms) between captured rows. Corrected by normalising per ENGINE frame
   (`t_wall Δ − frame_index Δ × 1/30`): every leg and every owner run reads excess ≤ +0.4 ms —
   the game thread was ALWAYS on budget. ⚠ Consequence for any future reader: **consecutive-row
   `t_wall` deltas measure capture cadence, not frame health; always normalise by
   `frame_index` delta.** (This also retro-explains G-R7(i)'s "baseline spikes" — same artifact,
   like-for-like in both legs, the cpu-vs-gpu null undisturbed.)
2. Owner bisect on the same binary: `Mask 0` smooth · `Mask 1` hitches · `MaskReduce cpu` vs `gpu`
   vs a warm second run ALL equally hitchy (PSO compilation excluded by the warm-run control; the
   reduction path exonerated by eye — consistent with the instruments).
3. **The decisive owner observation:** OBS of the live display shows the screen RUBBERBANDING back
   to the injection-start frame ~3–4 frames after each injection, repeating per window, normal
   after FinishRun — while a Blender reassembly of the captured PNGs is perfectly smooth.
   ⇒ display-only; the capture output was never wrong.

## §2 The mechanism (source-read, then MEASURED per the chat ruling)

- Engine contract: `PostProcessing.cpp:412-433` runs after-pass callbacks;
  `AcceptOverrideIfLastPass` (`OverridePassSequence.h:116-138`) hands `OverrideOutput` — THE FINAL
  SCREEN RENDER TARGET — to the last callback of the last enabled pass, which is then expected to
  WRITE it.
- `FAnomalyMaskSceneViewExtension::AfterTonemap_RenderThread` ignored `Inputs.OverrideOutput` and
  returned SceneColor. On armed frames (the only frames the mask SVE registers — m26 design), with
  Tonemap the chain's last enabled pass (this packaged config: TSR/TAA, no FXAA, native SP), the
  screen target was never written and the swapchain re-presented stale content. Unarmed frames:
  no callback registered, Tonemap accepts the override itself — clean. The capture SVE
  (`AnomalySceneViewExtension.cpp`, subscribed at the disabled VisualizeDepthOfField slot) never
  becomes the designated writer on this config — which is why plain captures were clean — but
  carried the identical latent shape.
- **A-I1 (the measurement, run BEFORE the fix; predictions AMENDMENT 1 written before the leg):**
  the `M23 PASS` line gained `overrideOutput=<0|1>` = `Inputs.OverrideOutput.IsValid()`.
  Leg `M34_AI1_OVR` (Mask 1, gpu): **`overrideOutput=1` on 29/29 armed frames** — the predicted
  reading exactly. Mechanism MEASURED, not inferred.

## §3 The fix (`b05066f`)

The engine's own OCIO pattern (`OpenColorIODisplayExtension.cpp:139-145`): when
`Inputs.OverrideOutput.IsValid()`, `AddDrawTexturePass` SceneColor into it and return it; else
return SceneColor unchanged. Applied on EVERY return path of both callbacks —
`AnomalyMaskSceneViewExtension.cpp` (the defect) and `AnomalySceneViewExtension.cpp` (defensive;
chat pre-authorized). Requires `SceneRendering.h` (FViewInfo complete type for the
`static_cast<const FViewInfo&>` — the OCIO cast; Renderer private access already held per G100).
⚠ **Unity-build gotcha, caught by the compiler:** the module unity-builds both cpps into one TU,
so an identically-named `static` helper in each was a C2084 redefinition (reported as C2065 at the
later call sites). The helpers carry distinct names (`FinalizeMaskAfterPassOutput` /
`FinalizeSveAfterPassOutput`). Cost of the fix: one fullscreen blit on armed frames only — the
copy the engine would otherwise have done inside Tonemap.

## §4 Gates (AMENDMENT 1, all pre-declared; zero-effect on measurement BY GATE)

- **G-F1 EYE GATE — OWNER PASS** (2026-08-24): MainWorld paced Mask-1 capture on `7F37A4AC` —
  rubberband GONE, smooth; the earlier Mask-0 control unchanged.
- **G-F2 (= G-R3 re-run) PASS:** 145/145 per-frame `MASK-REDUCE COMPARE IDENTICAL`, zero
  FIRST-DIFF across the five legs (`M34_F2_*`). `_49` 66,843–66,878 px (band exact).
  **`_73` read 48,590–48,597 — the ORIGINAL banked band**, closing session 058's CYL73 note: the
  count is pose-scoped and both readings are the same instrument at two admissible poses.
  Spline MEASURED_ZERO ×8 · `SM_Ramp2` NOT_MEASURED ×8 · probe fired ·
  `overrideOutput=1` still on every armed frame (still the designated writer — now honoring it;
  the copy path ran crash-free, i.e. the blit uses engine shaders already in the container, no
  cook needed).
- **G-F3 (= G-R5 re-run) PASS:** cpu/gpu kept-event pair identical (8/8 incl. frame_indices);
  spline veto pair identical (8 vetoed both, `anomalies: []`); delivery-ON veto outcome identical
  with the correct file set; true INERT (`Mask 0`) zero mask lines, `provided=false`, 0 vetoed.
- **G-F4 (= G-R6 re-run) PASS:** annotation flat keyset 48/48 diff 0; run_summary keyset +0 vs
  the m33-binary leg.
- **G-F5 build identity:** code-only hot-swaps (G103), container = the m34 quartet
  (`2A66CA57`/`A7EF9B12`/`D8009AD7`) unchanged throughout. Exe chain:
  `17DEAA74` (m34 cook, archived) → `64568A5D` (A-I1 instrument; NOT archived before overwrite —
  loss bounded: rebuilds from `310a87f`, its leg banked with the hash in this journal; the m26
  archive-gap shape, recorded not hidden) → **`7F37A4AC` (the fix, archived at
  `_binary_baselines\StackOBot.exe.m34-fix-candidate-7F37A4AC`)**. Editor target rebuilt after the
  commit (exit 0) so the editor dlls match the branch state.
- ⚠ **G165-candidate (tooling):** PowerShell 5.1 mangles a native-command argument containing
  embedded double quotes even inside a single-quoted here-string — a `git commit -m` carrying a
  quoted phrase silently became multiple pathspecs and NO COMMIT happened. The failure was loud
  (pathspec errors) but easy to misread as committed. Keep commit messages free of `"`.

## §5 THE 054 BISECT, REINTERPRETED — "hitch" was TWO stacked phenomena (chat-accepted)

The session-054 owner bisect on Concorde ("empty pool no hitch; full pool hitches DURING windows;
`Mask 0` removes it") identified THE MASK SYSTEM, not a component. Two distinct phenomena satisfy
that bisect and were conflated under one word:

1. **The throughput starvation** — real, wall-clock-measured on Concorde (the 23 s-for-4 s-video
   artifact; ~5.2 fps effective), scale-dependent (6.4 MP + heavy scene), the mechanism m34's GPU
   reduction addresses. The bench never had it (2.1 MP with headroom — G-R7(i)'s null, which
   stands as explained).
2. **The stale-present display defect** — this session's finding: config-dependent, not
   scale-dependent, visible wherever Tonemap ends the chain, datasets unaffected, fixed at
   `b05066f`.

The m34 handoff's mechanism sentence attributed the hitch to the readback+scan alone — reasoned
from the bisect but never component-isolated (G120's shape; the owner's bench A/B is what caught
it). **Consequence, chat-ruled: G-R7(ii) on Concorde is SPLIT — the eye/OBS judges ONLY the
display fix; throughput is read EXCLUSIVELY from the m33 wall instruments (`t_wall` span vs
frames/VideoFps; `speed_ratio` with `game_clock_speed_ratio` beside it). The eye is never again
the throughput instrument.**

## §6 NOT done, named

- Monday's master/delivery build ships WITHOUT this fix (chat venue ruling; the display artifact
  stays the documented limitation there; client-note amendment is chat-side).
- G-R7(ii) Concorde (both halves, re-specified) — still the MERGE gate. NO TAG, NO MERGE.
- The m33 watcher-estimator ratify-or-revert — untouched, not this branch's business.
- No CLAUDE.md status edit (master-side; lands with the merge).
- `encode_watcher.py`, `feature/stencil-capture` (`76cac74`), P6 — untouched. No ratio, no
  threshold, anywhere.

## §7 Evidence bank

`M34_AI1_OVR` (the measurement leg) · `M34_F2_*` ×5 · `M34_F3_*` ×6 (every attempt banked) ·
owner sessions `test_maskoff` / `test_maskon` / `test_maskon2` / `test_maskcpu` / `mytest` /
`eyetest_*` beside the staged exe · the owner's OBS recording and Blender reassembly (owner-held)
· session-058/059 transcript instruments (`hitch_excess.py` method: normalise by frame_index).
