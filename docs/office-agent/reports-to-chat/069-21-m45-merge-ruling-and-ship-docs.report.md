# REPORT for 069-21-m45-merge-ruling-and-ship-docs.md
brief: 069-21-m45-merge-ruling-and-ship-docs.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: high
started: 2026-09-03 14:39:34 | finished: 2026-09-03 14:49:45 | duration: 10 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-21-m45-merge-ruling-and-ship-docs.live.txt

---- FINAL MESSAGE (Claude Code) ----
```
SESSION 069 / BRIEF 21 — m45 IS SHIPPED AND MERGED.

FINAL STATE
  master  d204598 == origin/master  (pushed)
            d204598  docs(m45): hidden-class target masks shipped … G228-G230
            4eab312  merge: m45 hidden-class target masks — identity arbiter native/AA-off, can-fail
                     via depth-pass omission
  branches m45-hidden-class-masks  c7a071d == origin  (renamed before the merge; GATE-FAILED name
                                    deleted from origin)
           m44-f1-resolution-mapping-UNVALIDATED  2d66b90  (untouched, waiting on the cook)
  exe     staged 8A895272 — the binary every m45 gate ran on; archived .m45-canfail-8A895272
  builds  game exit 0, editor exit 0.  Container unchanged, no cook.  NO TAG (… m44 → m45).

F1 STILL OUT OF MASTER, verified two ways:
  git diff 62bd287 master -- AnomalyVisibleMask.usf AnomalyVisibleMaskShader.h  → EMPTY
  git merge-base --is-ancestor 2d66b90 master                                    → exit 1

=== M45-G3 — the would-be silhouette, both tick orders ===
  event                                 hidden  visible ref            pixels          IoU      camera delta
  blinking       StaticMeshActor_73      si 40   si 27 corrupted_tex   48,590/48,591   0.9987   (0,0,0)/(0,0,0)
  missing_object StaticMeshActor_73      si  3   si 27 corrupted_tex   48,568/48,591   0.9969   (0,0,0)/(0,0,0)
  Identical to four decimals in both orders. Pass bar 0.9.

  ⚠ THE REFERENCE HAD TO BE CHOSEN, AND THAT IS THE POINT: there is no mask on a blink's visible
  in-between frames BY DESIGN (G7 forbids it), so "the last visible frame's mask" does not exist inside
  a hidden-class event. The sound reference is the same actor's silhouette from a non-hidden event in
  the same run at the same camera — StaticMeshActor_73 has both.

  ⚠ ONE MEASUREMENT THROWN AWAY BEFORE IT WAS READ: my first missing_object legs ran on the m44 binary
  (I had restored the bench to the shipped exe at the end of brief 20 and did not re-stage). They gave
  visible=0 on all 40 frames and no masks — exactly what the m44 hide should give. Diagnosed from the
  leg's own NOT-ARMED counters, not guessed, and re-run on 8A895272.

=== THE RULING, RECORDED AS SCOPE ===
"Identity/pixel arbiters run native-order at the AA-off configuration; alignment gates run both orders."
SynthTickOrder is nondeterministic even at AA-off by its own old-hide control (60/60 frames, mean
5.95 %, where the native control is 0/60), so it hosts alignment gates and never a pixel comparison —
and never ships. G230 carries it. The both-orders requirement is untouched for the alignment gates, all
of which passed in both orders.

=== DOCS ON MASTER ===
 client-readme  — limitation paragraph REMOVED; replaced by the hidden-class semantics (would-be
                  silhouette, occlusion-aware, none on a blink's visible frames, Nanite excluded per
                  G134, with the measured IoU range).
 client-delivery — identity arbiter + its can-fail leg as cook-time gates, G7-equality, ONSET 6/6, and
                  the D2 limitation in as many words: identity is proven at the AA-off arbiter; at the
                  delivered configuration cross-run comparison has a nondeterminism floor and no pixel
                  claim is made. Bench-lever grep extended.
 PRE-DELIVERY-CHECKLIST §1.1 — three m45 boxes, incl. "all three readings or the gate proves nothing".
 Card SECTION F read (g) — expected: every blink row's 1stMsk == 1stLbl (not NONE), and G7 files ==
                  labelled with stray 0.
 CLAUDE.md      — status block refreshed; invariants for the hidden-class hide, the logical-hidden
                  registry as the single source of "hidden" for labels/m26/census, the permanent
                  arbiter and lever, the scope rule, and `git add <directory>` banned alongside
                  `git add -A` after the handoff sweep.
 Gotchas G228 (the ~9 % cross-run floor and the AA-off arbiter), G229 (the base-pass gather accepts a
                  main-pass-off mesh; the PROCESSOR refuses it), G230 (SynthTickOrder hosts alignment
                  gates, never pixel arbiters) — unique and in order after G227.
 Ledger §12, Journal 069 §15, _binary_baselines README (635A615A is now m45's A-side as well as the
                  shipped m44 binary).

NEEDS-DECISION: none.

Carried: the cook remains authorised for a later brief — F1 (resolution mapping) is unvalidated until
then, so masks, the census and m26 are correct only at 100 % screen percentage, which is what card
read F-2(e) exists to check on the host.
```
api-equivalent cost (not billed on Max): $16.32 | turns: 18 | running total across all briefs: $463.74
