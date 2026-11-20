# REPORT for 069-15-m44-g1-mask-render-pairing-hypothesis-test-and-fix.md
brief: 069-15-m44-g1-mask-render-pairing-hypothesis-test-and-fix.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 12:22:29 | finished: 2026-09-03 12:39:55 | duration: 17 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-15-m44-g1-mask-render-pairing-hypothesis-test-and-fix.live.txt

---- FINAL MESSAGE (Claude Code) ----
```
SESSION 069 / BRIEF 15 — FRAME-HANDSHAKE HYPOTHESIS: **REFUTED**. No fix written; Task B not entered.

VERDICT IN ONE SENTENCE: the mask SVE genuinely has no frame key — chat's source read is correct in
every particular — but that gap is NOT the cause of the +1, because the probe never once shows the
previous frame's position and forcing r.OneFrameThreadLag 0 changes nothing.

STATE
  master   62bd287 == origin/master  (UNTOUCHED)
  branch   m44-GATE-G1-FAILED-do-not-merge  7cc8a70 == origin  (pushed; name unchanged, it did not pass)
  exe      346ED33F  archived .m44-probe-refutation-346ED33F   (4EB2EA5C = the first, wrong probe build)
  builds   game exit 0, editor exit 0.  Container unchanged, no cook. NO TAG.
  bench    CaptureBench tools/m44_pairing_probe.py committed

=== SOURCE READ — CONFIRMED AS FACT ===
 ArmMask (AnomalyMaskSceneViewExtension.cpp:52) pushes a bare PendingArms under StateCS from the game
 thread; AfterTonemap_RenderThread (:101, taking at :119-126) takes ALL of them whenever it next runs.
 BeginRenderViewFamily is an EMPTY override (header :41) — the exact hook m31 uses.
 One correction to the brief: ConsumeWantedForPublish is a FIFO and only LOGS FamilyFrameNumber
 (AnomalySveCapturer.cpp:45-73). The real m31 keying is the key ring — PublishKey(InViewFamily.
 FrameNumber, …) game-side at BeginRenderViewFamily (AnomalySceneViewExtension.cpp:66), recovered by
 LookupKey(View.Family->FrameNumber) (:93-96). The binding is made game-side; the mask has none.

=== A1 / A2 — FOUR LEGS, BOTH ORDERS, BOTH LAG SETTINGS ===
  leg             order   lag       decidable  CURRENT  PREVIOUS  NEITHER  no-mask
  A1_NAT2         native  default       28       18        0        10       12
  A2_LAG0         native  0             28       17        0        11       12
  A1_SYNTH        synth   default       30       20        0        10       10
  A2_SYNTH_LAG0   synth   0             29       19        0        10       11

 PREDICTION 1 FAILED — PREVIOUS = 0 on every decidable frame of every leg.
 PREDICTION 2 FAILED — lag 0 changes nothing: same indices, same verdicts, same pixel counts to the
 digit (16005 / 16130 / 16016 / 16123 on both native legs' NEITHER rows).

 BOTH LEVERS PROVEN ENGAGED, so this is a reading and not blindness (G114): the engine echoed
 `r.OneFrameThreadLag = "0"` at frame 1, and the probe echoed its spawn plus 40 STEP lines per leg.
 THE ANALYSER IS NOT BLIND EITHER, from its own numbers: on CURRENT rows the distance to the current
 picture centroid is ~15–30 px against ~596 px to the previous position — a PREVIOUS would have been
 caught with a 20× margin.

=== WHAT THE PROBE DID FIND (no mechanism asserted, G120) ===
 The failure is not a uniform shift. Per 40-frame leg: ~18–20 masks CORRECT; ~10 carry an EXTRA
 silhouette the picture does not contain; ~10–12 have no mask at all. x-band evidence, session_index 1:
   [(320,384) (400,2476) (480,2602) (560,315)]  and  [(800,3130) (880,3640) (960,3458)]
 — the correct cluster at position B PLUS a second cluster centred ~470, which is NEITHER commanded
 position, while the picture that frame is a clean single silhouette (picN 8008, picCx 951.0).
 Not the thread lag (A2), not the tick order (both identical), not a proxy recreate (probe tagged once
 at spawn, tagFlips=2). I am not naming a cause: the previous two named causes were each refuted by the
 next measurement, and this would be the third.

 CONSEQUENCE THAT SURVIVES AND MATTERS MORE THAN THE +1: the target mask is not merely one frame late —
 on roughly a quarter of frames it is WRONG IN CONTENT and on another quarter it is ABSENT. That is a
 stronger reason not to ship masks than the onset delta ever was, and it is measured.
 Possible, NOT claimed: the §8 onset +1 may be a symptom of this rather than a separate fact.

=== INSTRUMENT ===
 New lever IAI.Bench.MaskPairingProbe (console only, default OFF, loudly echoed, never in a client
 payload): movable magenta cube 600 units in front of the settled camera, tagged ONCE at spawn, position
 alternated per captured tick by SetActorLocation — a transform update, never a render-state recreate.
 Its first build was WRONG and its own telemetry said so (tagFlips=80 — the probe had been pulled into
 the tag/restore machinery, the very confound it exists to exclude). Corrected: tagFlips=2.

 G225 was deliberately NOT minted — the drafted wording asserts the mechanism this brief refuted.

=== NEEDS-DECISION ===
 D1. The B-series fix is unauthorised by its own precondition (A did not confirm). The frame-handshake
     gap is nonetheless REAL and worth closing on its merits — it just is not this defect's cause. Do
     you want it built as hygiene, gated by the probe (which would then have to stay MIXED, proving the
     handshake was not the fix), or left alone until the actual cause is found?
 D2. The next measurement I would run, if authorised: instrument the mask SVE to log, per served arm,
     the family frame number and the number of arms served, and join that against the probe table — that
     distinguishes "one render served several arms" from "one arm served by a render that already
     contained stale custom depth" without guessing between them.
 D3. m43's shipped masks are affected by whatever this is, not only at onset. Nothing with masks has
     reached a client, so no retraction is owed — but the client-facing mask documentation should not be
     written until this closes.
```
api-equivalent cost (not billed on Max): $18.70 | turns: 50 | running total across all briefs: $369.66
