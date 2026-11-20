# REPORT for 080-03-m52-hold-start-window-verified-restore-and-missing-gates.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 080-03-m52-hold-start-window-verified-restore-and-missing-gates.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-20 20:04:25 | finished: 2026-09-20 21:08:33 | duration: 64 min | exit code: 0
context at last turn: 680287 tokens (68% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\080-03-m52-hold-start-window-verified-restore-and-missing-gates.live.txt

---- FINAL MESSAGE (Claude Code) ----
Everything verified. Report follows.

```
# 080-03 — m52: window-at-first-held, verified restore, per-texture telemetry, missing gates, verifier allowlist

BRANCH feat/m52-stuck-mip  ced1d80 -> fd5928d (pushed, == origin)
master 3ff88db == origin (untouched) · m51 53bf725 (local-only, recorded at start, RESTORED as checkout)
CaptureBench 472a409 tracked-clean · NO merge · NO tag · NO cook · NO default flipped · NO threshold tuned
BINARY DC8BA0C1 (241,508,352 B) built == staged == archived; predecessor A8742A4A hash-verified AT ITS
ARCHIVE before the staged copy was overwritten (A62) and is LOAD-BEARING as G3b's A-side.
CONTAINER QUINTET 67EA1FE0 / 2CEFB8F4 / E03C6610 + A16A18A8 / C70ECDAA — hashed before AND after, BYTE-UNCHANGED.
A44 both encodings: 19 new symbols utf16-present/ascii-absent · 3 pre-existing controls PRESENT (scan SOUND)
· 3 retired symbols ABSENT (the renames took, read out of the artifact) · 2 invented symbols ABSENT (DISCRIMINATES).
AMENDMENT 2 committed 03728ef BEFORE any leg ran.

## GATES — every leg accepted on attempt 1, B1 correctly NOT APPLICABLE (G117)

G0  BUILD      StackOBotEditor (modular) AND StackOBot (monolithic), both exit 0, ZERO warnings.   PASS
               LyraEditor Win64 Development exit 0, ZERO warnings.
G2  CAN-FAIL   IAI.Bench.StuckMipNoHold 1 -> held false every frame, 3 events all manifested:false  PASS
               with empty injected_frames, frames_held 0, onset_preroll_max -1.
               AND stuck_mip_hold_timeouts 2 of 3 fires — AMENDMENT 2 predeclared that a can-fail
               leg which does NOT time out means R1's wait is not wired. It timed out.
G3  HOLD       2 fires, 2 windows of 7 CONTIGUOUS held frames, unheld-in-window NONE,               PASS*
               non_manifested_events 0 (080-02: 3 of 7 non-manifested).  *reading corrected, see DEFECTS
G3b F1 A/B     A-side A8742A4A (banked): ZERO held frames, 24x "already at or below".               PASS
               B-side DC8BA0C1, same recipe: 4 fires, 4 manifested, 28 held frames,
               refused_not_restored 0, already_held 0, and NOT ONE "already at or below" line.
               => AMENDMENT 2 branch (a): the defect is FIXED, not merely made visible.
G4  RESTORE    4 of 5 exits MEASURED; 1 UNRUN with its structural reason.                           PARTIAL
               normal revert          restored=N left-to-game=0 unresolved=0 + RESTORE VERIFIED per texture
               FinishRun (frame cap)  revert of 6 - restored=6 left-to-game=0 unresolved=0 already-back=6
               cancel before focus    0 frames, 0 bursts, ZERO fires applied — nothing held to restore
               world teardown (live)  "Subsystem deinitializing; reverted 1 active anomaly(ies)." then
                                      "revert of 6 held texture(s) - restored=6 left-to-game=0 unresolved=0",
                                      exit code 0, "Object subsystem successfully closed.", zero asserts
               target destroyed       UNRUN — no lever exists, none built. m52 holds texture ASSETS,
                                      which outlive the actor; unresolved reads 0 on every leg, which is
                                      a reading about this fixture, not a proof of that branch.
               LIMIT, in the teardown line itself: "awaiting-restore=6 (total tracked 6)" — the bias IS
               cleared but the restore is NOT verified, because nothing ticks after the world is gone.
G5  ONSET      first labelled frame IS the first held frame; window contiguous; 4 legs, BOTH orders. PASS
G6/R7 HITCH    real instrument, PACING OFF (the pacer absorbs the very variance a hitch test seeks): PASS
               all 89 frames  m52 max 64.089 p99 52.656 | control max 68.711 p99 59.183
               si >= 30       m52 max 49.671 p99 49.521 | control max 53.771 p99 51.891
               m52 faster on every statistic; both maxima at si=16 (fixture startup, not the anomaly).
               LIMIT (G169): within-build spread is 17 -> 69 ms, so a +2 ms effect is NOT resolvable
               here. This says no LARGE hitch. It does not say "no cost".
G7  YIELD      PRINTED, NO THRESHOLD — table below.                                                 READING
G8  LYRA       shipped MaxCoAffected 0 -> 9 fires, 0 held, refused_shared 27. m52 yields NOTHING     PARTIAL
               on ShooterGym under the shipped default.
               bench override StuckMipMaxCoAffected 8 -> 3 fires, 3 manifested, 7 contiguous held
               frames each, observability_measured true, 0 non-manifested, 0 vetoed. Targets Cube x2
               and SKM_Manny — a SKELETAL target, a class no StackOBot leg has exercised.
               RESTORE VERIFIED after 34 frames; stuck_mip_restore_frames_max 75 (StackOBot: 7).
               refused_not_restored 4 — R3 working on a second host.
               stuck_mip_refused_virtual 0 on BOTH legs => F-e UNEXERCISED, reported as never
               exercised rather than as a proven counter, per AMENDMENT 2's own branch.
G9  SCHEMA     blinking leg vs a pre-m52 banked session: row keys 13->13, anomaly keys 12->12,       PASS
               added 0 removed 0. Stuck-mip entry adds exactly the 14 stuck_mip.* keys.
               run_summary 68 -> 83, added exactly the 15 stuck_mip_* keys, removed 0.
               annotation.json root 4 -> 4 AND per-event keys 16 -> 16, added 0 removed 0 — P6 DOES NOT MOVE.
               label_schema still 2.
G10 ORDERS     native and SynthTickOrder IDENTICAL: first held si 35 both, n=7 both, same fires /   PASS
               frames_held / preroll_max.
G11 VERIFIER   targeted legs NO-TRACE 0, exit 0 — AND the class is now AVAILABLE.                   ** FAIL **
               auto-pool leg: VERDICT FAIL: NO-TRACE on 2 event(s), exit 2.

## G11 — THE FAILURE, WITH ITS NUMBERS

  run[252..258] NO-TRACE - no change above the noise floor (tau=0.0530) on this target
                across frames 252..258; the label claims a visible change
  run[581..587] NO-TRACE - no change above the noise floor (tau=0.0867) ...

Both on StaticMeshActor_UAID_..._2048592804, asset SM_rock: bbox_px 401x269 (11.7 % of frame),
target_pixels ~25,000, 3 textures held baseline 10 -> resident 7, top_resident_px 64,
held:true, held_all:true, observable:true. 401 px against a 64 px top mip is a ratio of 6.3,
above the 4.0 perceptibility gate — and stuck_mip_refused_imperceptible read 0, so the gate
never fired at all.
CAUSE NOT ESTABLISHED AND NOT CHASED (G120). Candidates NAMED, none claimed: the ratio's own
declared tiling weakness; the held set may not include the texture that dominates that surface;
or a 10->7 drop on low-frequency rock maps sits near the instrument's floor. RATIO NOT TUNED.
The instrument is NOT blind there — the same leg's SM_rock_02 events return OFFSET-NOTE.
080-02's G11 zero was blindness and said so in writing. The route is reachable now and the
first thing it did was find two labelled events the pixels do not support.

## F1 MECHANISM — with the log evidence, and 080-02's diagnosis CORRECTED

  revert of 6 held texture(s) - restored=6 left-to-game=0 unresolved=0 relinked=0
                                already-back=0 awaiting-restore=6 (total tracked 6).
  RESTORE VERIFIED 'T_rock_02_N' resident 11 >= baseline 11 after 7 frame(s),
                   1 re-asserted stream-in request(s) and 7 frame(s) where the engine was already busy.

The revert-time StreamIn WAS issued — the !HasPendingInitOrStreaming() guard did NOT skip it (that
log line never printed). already-back=0: not one of six textures was back when the revert returned.
The restore then took 7 more frames.
=> THE MECHANISM IS LATENCY, NOT A DROPPED REQUEST. 080-02's "silently skipped StreamIn" candidate
is WITHDRAWN as the explanation. The "7 frames where the engine was already busy" are frames during
which OUR OWN stream-in was completing — stated so the number is not read backwards later.
ALSO CORRECTED: 080-02 attributed the "already at or below" line to a failed restore. It fired on
the FIRST TWO fires of a session, before any hold had ever happened — the textures had simply not
streamed in yet. The observation stands; the attribution does not.

## G7 — THE NEW YIELD TABLE

                          080-02 (A8742A4A)   080-03 (DC8BA0C1)
  bursts                        49                  23
  fires applied                 32                  18
  events written                32                  17  (1 vetoed by m26)
  MANIFESTED                     6                  16
  non-manifested                26                   1
  manifested / applied fire  18.75 %             88.9 %

  fires_applied 18   frames_held 119   onset_preroll_max 19   restore_frames_max 7
  refused_shared 76  refused_not_streamable 52  refused_no_eligible_textures 3
  refused_not_restored 2  refused_already_held 0  hold_timeouts 0  restore_timeout 0
  refused_imperceptible 0  refused_virtual 0  textures_awaiting_restore 0
  manifested targets: SM_FloorBase x6, SM_rock_02 x5, SM_rock x3, SM_SpawnPad_Base x2

R1 trades bursts for correct windows (49 -> 23); the dataset number is manifested events, 6 -> 16.
Against chat's >= 25 % steer, 88.9 % clears it widely. NO DEFAULT WAS FLIPPED — REPORTED per R6,
and it must be read beside G11 and G8.

## DEFECTS — MINE, FOUND BY READING THE ARTIFACT, FIXED MID-SESSION

1. G267. The per-texture record's "name" field shipped as "Name". FAnomalyTelemetry keyed on FName,
   and FName::ToString() returns the case of the FIRST registration anywhere in the process — the
   engine registers "Name" long before any plugin runs. Nothing warns; value and type correct, KEY a
   different string from the source. The bag now keys on FString. Most transferable finding here.
2. stuck_mip.onset_latency_frames and the run counter disagreed by one AND were not the same
   quantity. The row key counts every captured frame from the apply tick; the run counter counts only
   the frames the WINDOW skipped, one frame later. Run counter renamed stuck_mip_onset_preroll_max;
   both intervals now defined in client-readme.md. (This is F3's lesson applied to my own output.)
An intermediate binary FA9C1737 carried both, ran one leg, and was NOT archived — the m40 DC16710D
precedent. Bounded and rebuildable; the leg is banked as M52B_PREFIX_FA9C1737_NAMEKEY_EVIDENCE.

## DEFECTS IN MY OWN PRE-DECLARATION — recorded, not reconciled

1. AMENDMENT 2 predicted "exactly PositiveFrames = 8 labelled frames". Measured max is 7. On the 8th
   Positives tick the FSM captures, decrements to zero and calls BeginRevert() IN THE SAME TICK, so
   FinalizeArmedLabel() samples an empty live-fire list. Pre-existing; every anomaly obeys it and
   gets its 8th frame back at the other end (the BeginFire tick, m18's [3..10]). A deferred-onset
   anomaly correctly cannot claim that frame. NOT FIXED — the alternative moves BeginRevert a tick
   later for every anomaly on a shipped, gated system.
2. AMENDMENT 2 said run_summary goes "8 -> 16"; measured 15. The list named seven additions; 8+7=15.

## OBSERVED, NOT CHASED

The m26 veto and the m49 observability measurement disagreed about one event: deleted as
MEASURED_ZERO maxCount=0 while its own rows carry target_pixels 16,020-16,184 and observable:true
on every frame. Incidence as an ASSOCIATION only: 2 vetoes across 25 applied fires on DC8BA0C1
against 0 across 46 on the banked A8742A4A legs. NO CAUSE ESTABLISHED; NOT attributed to R1 and R1
NOT excluded. Direction matters — it deletes an event the labels call visible.
Also: the two extra row keys on stuck-mip legs are m48's conditional exposure_dip pair, fixture-driven.

## COMMITS (pushed)
  03728ef docs(m52): AMENDMENT 2 — pre-declared before any leg
  7a40350 feat(stuck-mip): m52 — window at first held frame, verified restore, per-texture telemetry
  58dc99d feat(tools): stuck_low_mip earns a NO-TRACE route, gated on the producer's own held flag
  fd5928d docs(m52): 080-03 journal, G267/G268, client field reference, cold-start block

VERDICT: NEEDS-DECISION — G11 FAILS with NO-TRACE on 2 auto-pool events (SM_rock, tau 0.053/0.087,
perceptibility gate never fired); cause not established, ratio not tuned. G4 is 4 of 5 exits with
target-destroyed UNRUN. G8's virtual-texture path UNEXERCISED on both fixtures. G7 reads 88.9 %,
above chat's 25 % steer, and no default was flipped.
```

Two things worth your attention beyond the verdict:

**The verifier change is the one that earned its keep.** 080-02 reported `G11` as a zero and said in writing that the zero was weaker than it looked, because the only failing verdict the tool could produce was unreachable for this class. Making it reachable — narrowly, gated on the producer's own per-frame flag, proven both ways in the selftest — immediately surfaced two labelled events whose pixels do not support them. That is a gate failure, and I have not touched the perceptibility ratio that would make it green.

**`G267` generalises past this project.** An `FName` used as a serialisation key ships the case of its first registration *anywhere in the process*, so `FName(TEXT("name"))` emitted `"Name"`. Nothing warns, the value and type are right, and the defect lives in code that ran earlier and elsewhere. Any future telemetry key would have been one unrelated engine registration away from the same silent rename.

---- STDERR ----
[mcp-sdk] SEP-2352: stored OAuth credential has no 'issuer' stamp (pre-upgrade storage or provider not round-tripping the value). SEP-2352 isolation is inactive for this read; ensure your provider round-trips the issuer field.

api-equivalent cost (not billed on Max): $61.00 | turns: 208 | running total across all briefs: $1,844.49
