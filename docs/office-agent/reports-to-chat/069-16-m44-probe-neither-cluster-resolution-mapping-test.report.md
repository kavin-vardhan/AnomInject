# REPORT for 069-16-m44-probe-neither-cluster-resolution-mapping-test.md
brief: 069-16-m44-probe-neither-cluster-resolution-mapping-test.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 12:47:21 | finished: 2026-09-03 13:02:12 | duration: 15 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-16-m44-probe-neither-cluster-resolution-mapping-test.live.txt

---- FINAL MESSAGE (Claude Code) ----
```
SESSION 069 / BRIEF 16 — HYPOTHESIS #3: DEAD AS THE EXPLANATION, CONFIRMED AS A REAL DEFECT.
                          AND 069-15's FINDING IS RETRACTED — IT WAS MY OWN FIXTURE.

STATE
  master   62bd287 == origin/master  (UNTOUCHED)
  branch   m44-GATE-G1-FAILED-do-not-merge  3bcec95 == origin  (pushed; name unchanged)
  exe      366C4B94  archived .m44-b16-probe-366C4B94.  Both targets exit 0. No cook. NO TAG.
  No fix written — F1 was gated on P1–P3 all holding; two failed.

=== P1 — FAILED ===
 Added View.ViewRect (FViewInfo; SceneRendering.h was already included) to the M23 PASS line.
 Default bench config, ALL 51 passes, no exception, including every pass that served a NEITHER frame:
   viewRect=1280x720  internalViewRect=(0,0)-(1280,720) 1280x720  unscaledViewRect=1280x720
 Internal == output == unscaled. There is no mismatch to explain anything. Bench runs at 100%.

=== P2 — FAILED ===
 r.ScreenPercentage 100 + r.DynamicRes.OperationMode 0: 52 passes, all 1280x720 internal and output;
 probe still 9 CURRENT / 8 NEITHER / 20 no-data. Forcing 100% changed nothing — it was already 100%.

=== P3 — HELD (the prove-it-can-fail leg, and it fired) ===
 r.ScreenPercentage 50: internalViewRect=(0,0)-(640,360) 640x360 against viewRect=1280x720 on all 52
 passes; probe CURRENT 0 / NEITHER 25 of 26 decidable.
 ⇒ THE MECHANISM IS REAL AND DEMONSTRATED. AnomalyVisibleMask.usf:23 samples scene textures at
 SvPosition + ViewRectMin — output space against internal-resolution textures. ANY host on dynamic
 resolution, a screen percentage ≠ 100, or a temporal upsampler gets wrong masks. All three are
 ordinary shipped-game settings. It is simply NOT ACTIVE at the bench default, so it explains nothing
 observed in 069-15. Two of three predictions failed ⇒ hypothesis dead as the explanation.

=== 🚨 RETRACTION — 069-15 §9.4 WAS MY INSTRUMENT, TWICE OVER ===
 (1) TAG COLLISION. The probe hardcoded stencil 250, inside the allocator range (200..254). The census
     tagged 78 candidates over 16 cycles: it handed 250 to another actor AND re-tagged the probe,
     which is an ordinary visible static mesh and so an ordinary candidate.
     MEASURED: IAI.Capture.Census 0 → no-data 10 → 0.
 (2) COLOUR COLLISION. The probe used M_CorruptedTexture_Pink — the same material corrupted_texture
     swaps its target to. On exactly the live frames the picture detector merged two objects:
     picN ~8,000 → 16,341–17,321 on session_index 15–22. The MASK on those frames was one clean
     cluster at the correct place (si=1 bands [(720,216)(800,3457)(880,3640)(960,2730)], centroid 908
     vs commanded ~907).
 WITH BOTH REMOVED:
   leg               order   decidable  CURRENT  PREVIOUS  NEITHER  no-data
   P5_CLEAN_NAT      native      40       40        0        0        0
   P5_CLEAN_SYNTH    synth       40       40        0        0        0
 ⇒ THE TARGET MASK IS CORRECTLY PAIRED WITH THE PICTURE, FRAME FOR FRAME, IN BOTH TICK ORDERS.
 "Wrong in content on a quarter of frames and absent on another quarter" is WITHDRAWN.
 ✅ 069-15's load-bearing conclusion is UNAFFECTED: the frame-handshake hypothesis stays refuted — a
 tag collision cannot manufacture a previous-position silhouette, and the lag cvar is still a no-op.

=== INSTRUMENT NOW COLLISION-PROOF, RE-VERIFIED WITH THE CENSUS ON ===
   P6_CENSUS_ON_NAT   native  33 decidable  33 CURRENT  0 PREVIOUS  0 NEITHER  7 no-data
   P6_CENSUS_ON_SYN   synth   31 decidable  31 CURRENT  0 PREVIOUS  0 NEITHER  9 no-data
 Probe tag is now ReservedStencilMax (255), which AllocateTag can never return. The residual no-data
 is the census legitimately tagging the probe on some cycles — reported, not hidden. Gate predicate is
 therefore NEITHER == 0 AND PREVIOUS == 0 over decidable frames, not "40 of 40", which would only be
 obtainable by making the fixture special.

=== BENCH'S ACTUAL SETTINGS (asked for) ===
 100% screen percentage, no dynamic resolution in play; r.TSR.History.ScreenPercentage 100,
 r.Bloom.ScreenPercentage 70.711 (bloom only). That is why P2 was a no-op.

=== WHAT THIS NARROWS ===
 The pipeline is correctly paired, so §8's +1 is NOT a pairing fault. It is specifically a newly
 applied tag not being present in that frame's custom depth — the probe, tagged once at spawn, never
 exercises that path and is right on every frame. Consistent, and the +1 stays OPEN. No mechanism
 asserted.

 NEXT DISCRIMINATOR I WOULD RUN (not a new named cause): a second probe actor tagged at OnWorldTickEnd
 of frame n instead of at spawn, reading its tableCount at n and n+1 against the same trusted picture.
 That isolates §8's +1 from everything else.

DOCS: journal 069 §10, predictions appendix 3, G225 (post-upscale pass sampling pre-upscale textures)
and G226 (a fixture sharing a namespace with the system under test). ⚠ This branch predates master's
G223/G224 commit, so gotchas.md here reads G222 → G225; the merge reconciles it. No CLAUDE.md
invariant added — no fix landed. Client mask docs still wait.

=== NEEDS-DECISION ===
 D1. The P3 defect is real, demonstrated, and independent of everything else in flight. It makes every
     mask wrong on any host not at 100% screen percentage. Do you want it fixed now as its own small
     milestone (the F1 mapping: P_out → InternalMin + (P_out − OutputMin) × InternalSize/OutputSize,
     nearest and clamped, which is nearest by construction and so also retires the mask-resampling
     follow-up)? I did not build it because F1 was gated on predictions that failed.
 D2. No moved-counter ruling is owed — no fix landed, so nothing moved.
 D3. Merge ruling: nothing on this branch is shippable yet; it holds the m44 work (G1 still failing),
     the probe lever and two refuted hypotheses. It should stay unmerged under its current name.
 D4. F3 (the Bates fog-card / reused-tag question) was NOT tested — but G226 is direct evidence that
     tag reuse across the 200–254 pool can make one actor appear under another's value. That is now a
     measured mechanism looking for a host observation, rather than a guess. Worth its own brief.
```
api-equivalent cost (not billed on Max): $14.22 | turns: 28 | running total across all briefs: $383.88
