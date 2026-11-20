# REPORT for 069-13-bates-mask-onset-defects-read-and-plan.md
brief: 069-13-bates-mask-onset-defects-read-and-plan.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 11:08:50 | finished: 2026-09-03 11:25:46 | duration: 17 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-13-bates-mask-onset-defects-read-and-plan.live.txt

---- FINAL MESSAGE (Claude Code) ----
Plan complete and committed. Final report:

```
SESSION 069 / BRIEF 13 — BATES SECTION-E: ROOT CAUSE, REPRODUCTION, m44/m45 PLAN
STATUS: DONE. Plan only. NO source change, NO feat commit, NO Bates contact. Stopped after the plan.

COMMITS
  AnomalyInjector  42061dc  docs(m44/m45): target-mask onset root-caused and reproduced in
                            BOTH tick orders - plan only   [PUSHED; HEAD == origin/master]
                            - journal 069 §7 (append: 153 insertions / 0 deletions, G115 clean)
                            - docs/predictions/2026-09-03-m44-m45-target-mask-onset-and-hidden-class.md
  CaptureBench     tools/m44_onset_table.py  (local-only repo, no remote)
  The six untracked owner CHAT-HANDOFF-*.md were NOT staged. Path-scoped adds only.

=== TASK A1 — first labelled frame vs first non-blank mask ===
Two legs, one binary 0EF535DC, seed 4242, 90 frames, CB_GateLevel, auto-pool.
M44_EXPORT (native)  /  M45_SYNTH2 (IAI.Bench.SynthTickOrder 1). Both banked.

NATIVE                                            1stLbl 1stMsk delta
  blink              _85   16,17,21,22               16   NONE   n/a
  corrupted_texture  _73   27..34                    27     28    +1
  blink              _73   40,41,45,46               40   NONE   n/a
  missing_texture    _49   51..58                    51     52    +1
  corrupted_texture  _49   63..70                    63     64    +1
  missing_texture    _49   87,88,89                  87     88    +1
SYNTH TICK ORDER
  blink              _85   16,17,18,22               16   NONE   n/a
  corrupted_texture  _73   27..34                    27     28    +1
  blink              _73   40,41,42,46               40   NONE   n/a
  missing_texture    _49   51..58                    51     52    +1
  corrupted_texture  _49   63..70                    63     64    +1
  missing_texture    _49   87,88,89                  87     88    +1

O1 REPRODUCES — IDENTICALLY IN BOTH ORDERS. delta = +1 on 4 of 4 non-hidden events.
O2 REPRODUCES — neither blink event has mask content on any labelled (hidden) frame.
O3 REPRODUCES with numbers — 90 mask files, 29 non-blank, 61 BLANK.

>> IAI.Bench.SynthTickOrder IS REFUTED AS O1'S MECHANISM. It was the stated first suspect.
   The lever DID engage: its echo is in the log and it moved the blink hidden set
   (21->18, 45->42 — the P9 interior-flip shape). So the null is a READING, not a misfire.

=== TASK A2 — ONSET (% pixels differing >8/255 from a clean reference at firstLabel-2) ===
                              n-1       n     n+1     n+2     n+3
  corrupted_texture _73 nat  0.471   5.935   6.175   6.192   6.695
  corrupted_texture _49 nat  0.500   8.157   8.521   8.929   9.633
  missing_texture   _49 nat  0.461   2.725   2.861   2.818   3.243
  missing_texture   _49 nat  0.438   2.727   2.841   2.851     —
  corrupted_texture _73 syn  0.587   6.008   6.188   6.154   6.652
  corrupted_texture _49 syn  0.500   8.162   8.409   8.773   9.468

O4 IS **NOT** REPRODUCED. The picture already differs at n by 5.9–8.2% against a ~0.5%
baseline — a factor of 12–16. The label at n is right and the pixels at n are right.
There IS a blend-in and it is reported, not dismissed: n->n+1 grows ~4% RELATIVE
(5.935->6.175, 8.157->8.521), consistent with temporal AA settling.
NO MECHANISM ASSERTED for the host observation. Candidate only: a host whose AA blends in
more slowly could make n look subtle TO THE EYE while the pixels have already moved. The
read that would settle it is this same onset table run on Bates — NOT requested here.
Because the onset is n, the conditional read of Anomaly_CorruptedTexture.cpp (material
readiness at apply) was NOT triggered and was NOT performed.

=== ROOT CAUSE OF O1, one sentence, with the lines ===
The target mask tags off an FAnomalyMaskRecord; records are created ONLY by FindOrAddRecord
(AnomalyMaskMeasure.cpp:124), called ONLY from AccumulateFrameEvents
(AnomalyCaptureSubsystem.cpp:4123), which on the async path is called ONLY from the DRAIN
(AnomalyCaptureSubsystem.cpp:2769) when a readback completes — one frame after the arm; so
on a fire's first frame ArmTargetMaskOwn (:836) finds no tagged record, returns false, and
the frame takes the blank path. => +1 BY CONSTRUCTION, IN ANY TICK ORDER.
The label is sampled at OnWorldTickEnd (m40) and is correct at n; the MASK side is keyed off
a structure that does not exist yet.
NO RENDER-TIMING OBSTACLE TO THE FIX: a tag set at OnWorldTickEnd of frame N is live for
frame N — World->SendAllEndOfFrameUpdates() runs inside FRendererModule::BeginRenderingViewFamily
(SceneRendering.cpp:4528), under the engine's own comment "Guarantee that all render proxies
are up to date before kicking off a BeginRenderViewFamily."

>> THE PREDICATE LESSON, AND IT IS THE LARGER HALF: m43 would not have caught this in EITHER
   order. The gap was not the missing tick order — NO GATE EVER COMPARED FIRST-LABEL TO
   FIRST-MASK. Gate (ii) checked the bit-exact tie on frames that WERE measured and (iii)
   checked a count identity; both are satisfied by a mask that is uniformly one frame late.

=== m44 PLAN (ship-blocking) ===
Invariant to be stated in code and CLAUDE.md: the target mask's arm, liveness and tag
decision come from the SAME per-frame snapshot as the labels (OnWorldTickEnd, m40's sample).
No tick order and no readback latency may split the mask from the label.
  AnomalyMaskMeasure.{h,cpp}   make record creation reachable without waiting for the drain
  AnomalyCaptureSubsystem.cpp  OnWorldTickEndMask (~:737): before ArmTargetMaskOwn, ensure a
                               record exists for every live fire on THIS frame's snapshot
                               (same Auto->GetLiveFires() source FinalizeArmedLabel uses)
  :2769 / :4123                unchanged; FindOrAddRecord is idempotent
  ServiceTargetMask / EnqueueTargetMaskPng   never write an all-zero file; count it "empty"
  blank path                   deleted entirely, with its deferred list and FinishRun flush
  AnomalyLabelWriter.{h,cpp}   mask_state = present|empty|unmeasured; mask_file stays
                               string|null and is null unless present; mask_map lists only
                               files that exist
  run_summary                  target_mask_frames_hidden_blank KEPT under its name (see D2)
  client-readme / client-delivery / CLAUDE.md   docs + the new both-orders gate rule
GATES M44-G1 onset==label==picture in BOTH orders (predict delta 0; today +1 on 4 of 4)
      M44-G2 zero blank PNGs (90 files -> ~29)   M44-G3 count identity
      M44-G4 schema additive   M44-G5 P-C7 v2   M44-G6 m26/veto untouched
RISK, NAMED: creating records a frame earlier changes when tags are allocated and could move
framesContributed — a VETO INPUT. M44-G6 exists for exactly that. A movement there is a STOP.

=== m45 PLAN (attempt; ships only on its identity gate) ===
Mechanism: drop the target from main+depth passes while KEEPING bRenderCustomDepth.
Paths that must also be silenced, each to be verified not assumed: bRenderInMainPass,
bRenderInDepthPass, CastShadow + bCastDynamicShadow/bCastStaticShadow/bCastContactShadow/
bCastVolumetricTranslucentShadow, VELOCITY, bAffectDynamicIndirectLighting (Lumen mesh
cards), bAffectDistanceFieldLighting, bVisibleInRayTracing, translucency, decals, HISM/ISM,
skeletal.
ARBITER M45-G1: pictures BYTE-IDENTICAL to the SetActorHiddenInGame legs, both orders. Any
pixel difference anywhere and m45 DOES NOT SHIP. G2 mask non-empty on hidden frames + tie
holds. G3 census/veto unmoved. G4 prove-it-can-fail by omitting one flag.
FALLBACK pre-declared: coarse projected-bbox masks from m39 bounds, flagged "coarse": true.
A coarse mask must never be presented as a silhouette.
>> HARD LIMIT KNOWN BEFORE ANY CODE: a NANITE target cannot produce a custom-depth mask at
   all on 5.1 (Nanite::FSceneProxy::GetViewRelevance never sets bRenderCustomDepth, G134).
   m45 CANNOT give hidden-class masks for Nanite targets.

=== NEW STANDING RULE (proposed for the milestone template) ===
Every capture-side gate asserting a per-frame alignment — labels, masks, onset, bbox — runs
in BOTH tick orders, and the onset instrument is permanent.

=== NEEDS-DECISION ===
D1. m45 vs Nanite. If the office title's structural geometry is largely Nanite (it is on
    StackOBot — 46 assets carry the signature), m45 buys hidden-class masks only for
    non-Nanite targets. Is it worth the milestone, or does O2 ship as a documented
    limitation with the coarse-bbox fallback? I have no read on that title's posture.
D2. run_summary naming. The brief said "run_summary counter naming"; I planned to KEEP
    target_mask_frames_hidden_blank rather than rename it — the value's meaning is unchanged
    (it always counted exactly these frames; only the file stopped being written) and a
    rename is a silent schema break for any consumer already reading it. Overrule if a
    rename is wanted; it must then be called out in client-readme as a break.
D3. O4 / the blend-in. Bench says the label at n is correct. Do you want the onset table run
    on Bates to settle whether the owner's n+1 is that host's AA? Not requested, not run.
D4. m45's first task is a SOURCE READ, not code: confirm a primitive with
    bRenderInMainPass=false is still gathered into the custom-depth pass at all. If it is
    not, m45's mechanism is dead before implementation and the fallback is the whole
    milestone. Confirm that read is in scope for m45's first turn.

NOT DONE, named: no source change; no build; Bates untouched; Anomaly_CorruptedTexture.cpp
not read (its premise did not fire); the ledger and card were planned (§7.5, Section F) but
not edited — say the word and they land in the m44 docs commit.
```
api-equivalent cost (not billed on Max): $19.98 | turns: 23 | running total across all briefs: $329.02
