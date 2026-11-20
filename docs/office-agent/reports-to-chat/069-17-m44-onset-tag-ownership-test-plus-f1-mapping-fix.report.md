# REPORT for 069-17-m44-onset-tag-ownership-test-plus-f1-mapping-fix.md
brief: 069-17-m44-onset-tag-ownership-test-plus-f1-mapping-fix.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 13:08:13 | finished: 2026-09-03 13:33:07 | duration: 25 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-17-m44-onset-tag-ownership-test-plus-f1-mapping-fix.live.txt

---- FINAL MESSAGE (Claude Code) ----
```
SESSION 069 / BRIEF 17 — THE +1 IS FIXED (tag ownership). F1 built but BLOCKED ON A COOK.

VERDICT IN ONE SENTENCE: hypothesis #4's mechanism is confirmed — an actor under a live fire could be
carrying somebody else's stencil value and ArmTargetMaskOwn accepted "already tagged" as "tagged for
me" — but its stated source was too narrow: the foreign value comes from a PREVIOUS EVENT on the same
actor as often as from the census, which is exactly why census-off cured only half.

STATE
  master   62bd287 == origin/master (UNTOUCHED)
  branch   m44-GATE-G1-FAILED-do-not-merge  2d66b90 == origin  (pushed; NOT merged, NOT renamed)
           fafb81f  fix(capture): tag ownership — VALIDATED
           2d66b90  wip(capture): F1 resolution mapping — UNVALIDATED, blocked on a cook
  exe      staged 635A615A (Task A, boots) — archived .m44-tagownership-635A615A
           57B132A4 was the F1 build and DOES NOT BOOT; deliberately not staged, not archived
  builds   game 0, editor 0.  Container unchanged, no cook. NO TAG.

=== A1 — census OFF does NOT cure it (native, 90-frame auto-pool) ===
  census ON  : 1/4 delta 0  — failures corrupted@27, corrupted@63, missing@87
  census OFF : 2/4 delta 0  — failures corrupted@51, missing@75
  The brief's stop rule fired on the hypothesis as written; A2 then showed why it was half right.

=== A2 — TAG-OWNERSHIP, on exactly the four +1 events and no others (4 of 27 armed frames) ===
  si  actor                eventTag   value actually on the actor   owner
  27  StaticMeshActor_73     222        204                          census
  51  StaticMeshActor_49     224        242                          census
  63  StaticMeshActor_49     226        224                          PREVIOUS EVENT (si 51's tag)
  87  StaticMeshActor_49     229        226                          PREVIOUS EVENT (si 63's tag)
  Two sources, not one — census-off removes the first two and leaves the last two.

=== A3 — the asymmetry, from source ===
  m26 ArmIfMeasurable calls TagActor(Actor, R.Tag) UNCONDITIONALLY (AnomalyMaskMeasure.cpp:231) — no
  hole. ArmTargetMaskOwn tested instead of asserting. The census skips "already tagged"
  (AnomalyCensus.cpp:726), so its guard is not "under a live fire" and on a fire's first frame the
  target is not yet tagged and the census can take it.

=== THE FIX + THE POOL-SPLIT DEVIATION ===
  Ownership rule: an actor under a live fire belongs to its event; ArmTargetMaskOwn retags
  unconditionally. Counter TargetMaskEventRetags + a TAG-OWNERSHIP line naming the displaced value.
  ⛔ F-A1 (partition the pool) DELIBERATELY NOT DONE, and the numbers are why: 55 assignable values
  (200..254) against 77 census candidates in a 90-frame leg. A split would make tag EXHAUSTION more
  likely, not less, to fix something a one-line ownership rule removes. Stated as a deviation.

=== GATE TABLE, BOTH ORDERS ===
  M44-G1 onset delta 0 ........ 4/4 PASS   |  4/4 PASS      (was 0/4 and 1/4)
  M44-G2 no blank PNGs ........ 0 PASS     |  0 PASS
  M44-G3/3b/3c count identity . 27+0+63=90 |  27+0+63=90
  M44-G7 masks ⊆ labelled ..... 0 stray    |  0 stray
  M44-G6 veto inputs .......... PASS — all six counters identical to the m43 control
  MASK-TIE .................... 27 lines, 0 MISMATCH  |  27 lines, 0 MISMATCH
  m26 known-answer control .... mask_probe_arms = 1 (the detector fires, so its zeros are readings)
  census health ............... framesPolluted 0, batchesLost 0, tagFailed 0
  P-C7 v2 ..................... frame_index delta ONE constant (0); the only field differing outside
                                t_wall is mask_value, itself a declared mask key
  both build targets .......... game exit 0, editor exit 0
  present = 27 is exactly the labelled frames of the four non-hidden events (8+8+8+3). The control
  wrote 29 — two frames of content on frames NOT labelled for their event, the old G7 defect.

=== MOVED COUNTERS (all explained; nothing unexplained) ===
  target_mask_frames_hidden_blank  61 → 0    intended: blank PNGs no longer written
  target_mask_frames_unavailable    0 → 63   intended: unarmed/hidden frames are unmeasured
  target_mask_frames_measured      29 → 27   intended: the two unlabelled-frame masks are gone
  mask_value (labels)              30 rows   intended: it IS the value being corrected
  census_frames / census_cycles    96→100 / 31→32   the census's already-tagged skip sees a different
                                   set now that live-fire targets are retagged; cycle boundaries
                                   shift. Run-to-run scale, P-C2 precedent
  census tagOvertaken             0–1 → 2–3  ⚠ the ownership rule made visible — the target mask takes
                                   back an actor the census had tagged, landing in the counter the
                                   census built for exactly this class. framesPolluted 0, batchesLost
                                   0, verdicts and the event set unchanged.
  UNCHANGED: event set, every manifested, positive_frames 43, non_manifested_events 0, all six veto
  counters. G6 does NOT trip its stop condition.

=== 🚨 F1 IS BLOCKED ON A COOK ===
  The mapping is implemented (P_in = InternalRectMin + clamp(round(P_out × InternalSize/OutputSize)))
  and compiles. It CANNOT RUN here:
    "Shader FAnomalyVisibleMaskPS's parameter structure has changed without recompilation"
  — fatal at engine init. Global shaders live in the cooked container, which the code-only hot-swap
  does not touch (G129). A cook retires the container quartet every m41/m43/m44 measurement was taken
  on, and cooks here are owner-sequenced (G118: never inside a measurement sequence). I did not run
  one. B2's 50% gate was NOT run; the commit is marked UNVALIDATED and the bench is left on the
  Task-A build.

DOCS on the branch: journal 069 §11, predictions appendix 4, G227 (two consumers of one shared
attribute, only one asserting ownership). CLAUDE.md invariants deliberately deferred to the merge
commit so they land once, on master, with the ruling. Client docs and card Section F still wait.

=== NEEDS-DECISION ===
 D1. THE COOK. F1 cannot be validated without one, and a cook also unblocks anything else needing a
     container refresh. Do you want it run as its own sequenced operation (declare the map set first;
     re-verify the map gate, the enforced token and the A44 scan afterwards; archive the quartet)?
     Until then F1 ships nothing and the mask is correct only at 100% screen percentage.
 D2. MERGE RULING. Task A (fafb81f) is validated on every gate in both orders and is, on its own, a
     complete and shippable m44: G1 4/4, G2/G3/G7 pass, G6 clean, MASK-TIE 0 mismatch. F1 (2d66b90)
     is not. Options: (a) merge Task A now and keep F1 on a branch until the cook; (b) hold both until
     the cook so m44 ships once. I lean (a) — the +1 is the client-visible defect and it is fixed —
     but the branch name says do-not-merge and I did not rename or merge it.
 D3. tagOvertaken 2–3 is the one counter that moved for a reason worth your ruling rather than mine:
     it is the ownership rule working as designed, but it is also the census losing an actor it had
     measured. framesPolluted and batchesLost are 0 and no verdict moved, so I read it as PASS-WITH-
     READING on the m43 gate-D precedent — confirm.
```
api-equivalent cost (not billed on Max): $16.55 | turns: 31 | running total across all briefs: $400.43
