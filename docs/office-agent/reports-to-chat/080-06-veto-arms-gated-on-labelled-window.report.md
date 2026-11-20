# REPORT for 080-06-veto-arms-gated-on-labelled-window.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 080-06-veto-arms-gated-on-labelled-window.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: high
started: 2026-09-20 22:30:56 | finished: 2026-09-20 23:06:17 | duration: 35 min | exit code: 0
context at last turn: 497978 tokens (50% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\080-06-veto-arms-gated-on-labelled-window.live.txt

---- FINAL MESSAGE (Claude Code) ----
```
================================================================================
080-06 — m52 follow-up: veto arm gating. COMPLETE, MERGED, PUSHED.
================================================================================

master 031a103 == origin/master (pushed). Merge 031a103, --no-ff, revert handle
`git revert -m 1 031a103`. INERT: master^{tree} == fix/veto-arm-window^{tree} ==
48a47483…, the exact tree merge-tree forecast BEFORE the merge. NO TAG.
m51 53bf725 UNTOUCHED. Checkout found on m51, moved to master to build/merge,
RESTORED to m51 (shared-tree rule 4). Branch fix/veto-arm-window KEPT.

PREDICTIONS WERE COMMITTED FIRST — 89ea8ee, before a line of code was written and
before any leg ran. Journal §1–§3 is that file.

THE CHANGE (4 files, +207/−5, all inside AnomalyCapture)
  A record gains bAwaitLabelled (set at creation from DoesAnomalyHaveDeferredOnset)
  and bLabelledThisTick (set every tick-end from the SAME IsFireLabelledThisFrame the
  label snapshot uses). ArmIfMeasurable skips a record awaiting its window.
  ArmsIssued only increments on a real arm, so the skip IS the deferral — no budget
  arithmetic. Gated at BOTH ends: a fire that has ENDED is not labelled either, so
  leftover budget cannot be spent on a RESTORED object.
  Logs: M26 REDUCE-TABLE (per-tag reduce contents, every contributing frame) and
  M26 VETO-READ (tag/arms/resolved/maxCount/counts=[per-arm], every event).
  Lever: IAI.Bench.VetoArmUngated, default off, echoed at StartRun.
  No artifact field, no IAnomaly change, no default flipped. P6 did not move.

GATES — ALL GREEN
  build     both targets exit 0, ZERO warnings (UBT log). A44 both encodings, two
            pre-existing controls non-zero ⇒ sound, not blind.
  G-A       blinking, 116 fields: cross-binary diff ⊆ same-binary control pair, EXTRAS 0
  G-B       lod_popping, 158 fields: 0 differences either way
  G-C       arm window INSIDE 24 · OUTSIDE 0 · UNJOINED 0 ; veto-vs-rows 0 contradicting
            m49 ; verifier NO-TRACE 0 exit 0 after a 98-case selftest
  G-D       CAN-FAIL FIRED — lever ON: INSIDE 0 · OUTSIDE 14 · UNJOINED 14
  G-E       synth tick order identical to G-C on all three conjuncts
  P6        annotation 4/16 keys, run_summary 86 — IDENTICAL across binaries
  regression contracts 25/25

  Same record: ungated arms at ticks 21–24; gated at 42–45 = si 35–38, the first four
  frames of its own labelled window (onset_latency_frames: 20).

THE ZERO 080-05 COULD NOT READ IS NOW READABLE — AND IT AGREES WITH m49
  vetoed event (BP_SpawnPad_C, tag 209):
    ourCount=0 present=5 totalMasked=66839 table=[200:4 201:327 202:32430 203:24091 207:9987]
  passing event on the same leg: table contains 210:34388.
  The pass RAN and produced for five other values; our tag is simply absent. TAG-OWNERS
  shows the actor carrying 209 uncontested (shared=0), taggedComponents=2 stable on all
  four arms. And m49's rows on those seven labelled frames read target_pixels
  [0,0,0,0,0,0,0] — the veto's zero is CORROBORATED, not contradicted.
  ⛔ NO CAUSE CLAIMED (G120). Recorded: of 9 assigned values live on those frames only 5
  appear — 204, 205, 206 and 209 are ALL absent, so BP_SpawnPad_C is 1 of 4 tagged actors
  contributing nothing there. 080-05's taggedComponents churn lead is WEAKENED, not
  refuted (stable 2,2,2,2 here and the zero happens anyway).
  ⚠ PREDICTED IN ADVANCE AND CORRECT: the fix did NOT eliminate the zero. One event is
  still vetoed. What changed is that it is interpretable.

A CHECKER DEFECT OF MINE, CAUGHT BEFORE ITS VERDICT WAS READ (third G142/G243 instance)
  The first run read OUTSIDE=4 and would have been reported as a product RED. A VETOED
  event is removed from annotation.json BEFORE it is written, so the one event the gate
  is about was the one it could not judge. Fixed by changing the window SOURCE to
  labels.jsonl (written delivery-OFF, not retro-edited by the veto, L3) — still the LABEL
  path, not the arm path — and that route was proved against the known answer first
  (A53): {si : held} == injected_frames EXACTLY on 5 of 5 surviving events, mismatched 0.
  The tool refuses a verdict if that precondition fails; its --selftest FAILS a pre-roll
  arm set (G96).

DEVIATIONS FROM THE BRIEF, STATED
  1. The brief asked G-A/G-B to compare "M26 VETO-READ arm ids" across binaries. That
     line is NEW, so the A-side binary cannot emit it. Compared instead: M23 ARM offsets
     from StartFrame, the full M26S1 EVENT field set, and run_summary.vetoed_events —
     and under a same-binary control pair, which a bare A/B on a B1-less MainWorld leg
     could not justify. Declared in §3 before the legs ran.
  2. G-B needed IAI.Anomaly.LodMaxDistance 50000 to fire at all — at the compiled 200 cm
     default this bench produces ZERO lod_popping events (m30) and the leg would have
     been VACUOUS (G146). Declared in §3 before the leg ran, not reached for afterwards.

ENVIRONMENT
  staged exe 0844220E (241,566,208 B; built == staged), archived
  _binary_baselines\StackOBot.exe.m52-veto-arm-window-0844220E. Predecessor 5588F6FB
  hash-verified at its archive BEFORE the swap and STILL LOAD-BEARING (G-A/G-B A-side).
  Container quintet BYTE-UNCHANGED (67EA1FE0/2CEFB8F4/E03C6610 + A16A18A8/C70ECDAA) —
  code-only hot-swap, NO COOK (G103). 9 legs banked M52F_A_* / M52F_B_*, all accepted on
  attempt 1, B1 NOT APPLICABLE and declared (G117). New CaptureBench instruments
  (local-only, b2ca764): m52f_arm_window.py, m52f_subset.py, m52f_window_from_labels.py.
  Comment stripper: 0 changed / 104 no-change. G271 minted.

STILL OPEN, NAMED
  Whether BP_SpawnPad_C was genuinely invisible on those frames is NOT ANSWERED — both
  instruments agree it drew no pixels under its own tag, and neither is a statement about
  the picture. The 080-05 §5 mechanism stays OPEN, now a sharper question, and
  M26 REDUCE-TABLE is what a future session reads first. m55 and S2′ still named and not
  built; GAutoPoolDefaultEnabled is still the owner's call.
```

VERDICT: GREEN
api-equivalent cost (not billed on Max): $37.84 | turns: 156 | running total across all briefs: $1,963.48
