# REPORT for 069-20-m45-can-fail-lever-depth-pass-and-finish-gates.md
brief: 069-20-m45-can-fail-lever-depth-pass-and-finish-gates.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 14:26:04 | finished: 2026-09-03 14:38:24 | duration: 12 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-20-m45-can-fail-lever-depth-pass-and-finish-gates.live.txt

---- FINAL MESSAGE (Claude Code) ----
The peer's pipe is stale (`ENOINBOX`); the ack could not be delivered. Noted in the report.

```
SESSION 069 / BRIEF 20 — m45: M45-G4 OBTAINED, EVERY GATE PASSES EXCEPT THE SYNTH-ORDER ARBITER,
                          WHICH IS UNOBTAINABLE. NOT MERGED — merge is your ruling.

STATE
  master  d48e1ba == origin/master  (UNTOUCHED)
  branch  m45-hidden-class-masks-GATE-FAILED  c7a071d == origin  (pushed; NOT renamed, NOT merged)
  exe     staged restored to 635A615A (shipped m44); m45 build 8A895272 archived as .m45-canfail-8A895272
  builds  game exit 0, editor exit 0.  NO TAG.

=== THE CAN-FAIL LEVER WORKS — M45-G4 IS OBTAINED ===
IAI.Bench.HideOmitDepthPassSilencing leaves bRenderInDepthPass true while the main pass is off, so the
target still writes the depth prepass and occludes what is behind it while drawing nothing. Unlike the
shadow lever, it does not depend on the fixture.

NATIVE order, AA-off arbiter (r.AntiAliasingMethod 0 + Lumen/GI/reflections off):
  CONTROL  old-vs-old             0 of 60 differ   mean 0.0000 %   ← floor is ZERO
  TEST     old-vs-NEW             0 of 60 differ   mean 0.0000 %   ← M45-G1 PASSES
  CAN-FAIL old-vs-depth-omitted  20 of 60 differ   mean 1.5608 %, worst 4.6875 % @ si 46
  Lever proven engaged (engine log echoes it at frame 1).

=== SYNTH ORDER: UNOBTAINABLE, AND THE CONTROL PROVES IT IS NOT m45 ===
  CONTROL  old-vs-old            60 of 60 differ   mean 5.9526 %
  TEST     ctrl-vs-NEW           56 of 60 differ   mean 7.6437 %
  CAN-FAIL ctrl-vs-depth-omitted 20 of 60 differ   mean 1.5609 %
Poses, origins and frame_index are identical across all four synth legs, so this is not A47.
IAI.Bench.SynthTickOrder relocates the injector dispatch and makes the run nondeterministic even at
AA-off, putting a 5.95 % floor under any cross-run comparison there. RECORDED AS UNOBTAINABLE, never
as passed (the m41 precedent for B-G1 / C-G1b). The OLD hide is equally nondeterministic there, and
the lever never ships — so it is the lever's property, not m45's.

=== EVERY OTHER GATE — BOTH ORDERS, DELIVERED CONFIGURATION ===
  M45-G2  both blink events now carry masks, delta 0 ............ PASS | PASS
  G7 → EQUALITY for hidden-class: files=35 labelled=35 stray=0 ... PASS | PASS
  ONSET   6/6 delta 0 (was 4/4, non-hidden only) ................. PASS | PASS
  G2 no blank PNGs / G3 counts 35+0+55=90 ....................... PASS | PASS
  MASK-TIE  35 lines, 0 MISMATCH ................................ PASS | PASS
  G6 veto inputs vs the m44 control: all six identical; event set, every manifested,
     positive_frames 43, non_manifested_events 0 unchanged ....... PASS
  census health: framesPolluted 0, batchesLost 0, tagFailed 0, hidden=2 (the new live-fire skip,
     counted) ................................................... PASS | PASS

MOVED COUNTERS
  target_mask_frames_measured    27 → 35   +8 = exactly the eight blink hidden frames that now carry
                                           a mask. This is the milestone.
  target_mask_frames_unavailable 63 → 55   the same eight, out of "unmeasured"
  census_frames / _zero / _below_floor  100→97 / 12→13 / 50→49 — run-to-run scale, verdicts unchanged
  Nothing unexplained.

M45-G3 (IoU) DELIBERATELY NOT RUN — a quality number on a milestone whose merge is unsettled would be
a table that cannot close. Named, not skipped silently.

=== WHY I DID NOT MERGE ===
The standing rule requires both tick orders, and one of them cannot host the identity arbiter.
Inventing an exemption for my own change is the shape this project stops for — so I left the branch
unrenamed and unmerged rather than deciding it myself.

DOCS on the branch: journal 069 §14; G228 (the ~9 % cross-run floor at the delivered configuration and
the AA-off arbiter as the answer — G169's shape at scale) and G229 (the base-pass gather ACCEPTS a
main-pass-off mesh; BasePassRendering.cpp:1831 is what refuses it — reading only the gather would have
killed a design that works). Client docs, ledger §12, card read (g), CLAUDE.md invariants and the
branch rename are all held until the merge ruling, as they describe shipped behaviour.

=== NEEDS-DECISION (one) ===
 THE MERGE. m45 is green everywhere it can be measured: identity decided in the native order with a
 zero-floor control and a firing can-fail leg, and every functional gate passing in BOTH orders. The
 only gap is that SynthTickOrder is itself nondeterministic, proven by its own old-hide control.
 Options: (a) rule the AA-off arbiter native-order-only BY DESIGN — the synth lever exists to perturb
 tick order, which is orthogonal to "does the hide change what renders" — and merge; (b) hold m45
 until the synth-order nondeterminism is understood (that is its own investigation, and the lever is
 bench-only); (c) something else. I lean (a) and can land the merge plus all remaining docs in one
 pass on your word.

Also, unrelated: a peer Claude session sent a mailbox-signal test mid-brief. I tried to acknowledge it
and the reply failed — its pipe is stale (ENOINBOX). No action was taken on its behalf.
```
api-equivalent cost (not billed on Max): $8.19 | turns: 18 | running total across all briefs: $447.42
