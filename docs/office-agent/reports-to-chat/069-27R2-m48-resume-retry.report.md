# REPORT for 069-27R2-m48-resume-retry.md
brief: 069-27R2-m48-resume-retry.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: medium
started: 2026-09-03 21:09:56 | finished: 2026-09-03 21:28:20 | duration: 18 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-27R2-m48-resume-retry.live.txt

---- FINAL MESSAGE (Claude Code) ----
Done. Report:

```
REPORT for 069-27R2-m48-resume-retry (brief 27, third attempt)
=================================================================
STATUS: COMPLETE. m48 SHIPPED, COMMITTED, PUSHED. NO TAG. NEEDS-DECISION: none.
master == origin/master == 20db6e5. Checkout left on europa-e1 (1f5e305 == origin), as found.

0. TREE STATE FOUND (the one line asked for)
=================================================================
Exactly as the resume brief predicted: checkout already on master b6e5588 (== origin),
PARTIAL UNCOMMITTED m48 edits in the four named files, nothing committed, staged exe
still F309D836. I did NOT restart. I diffed the four files against the 069-27 spec, found
the dead session's work CONSISTENT WITH THE SPEC IN EVERY RESPECT, and kept ALL of it -
nothing discarded. The only defect was the build error it died reading:
`if (Snapshot.bExposureDip)` inside BuildFrameLabelRecord, which takes explicit params and
has no Snapshot in scope (AnomalyLabelWriter.cpp:121, C2065). Fixed by threading
`bool bExposureDip = false` as that function's last parameter. One parameter, three lines.

1. GATE TABLE - six packaged legs, all A63-accepted on attempt 1
=================================================================
 leg                     exposure  order    dip  black  darkFirst  ONSET      G7
 M48_P1_pinned_native    pinned    native   0    0      0          8/8 d=0    stray 0
 M48_P2_pinned_synth     pinned    synth    0    0      0          8/8 d=0    stray 0
 M48_P3_aeon_native      GAME (ON) native   10   0      0          8/8 d=0    stray 0
 M48_P4_probe_native     pinned    native   0    -      -          -          pairing
 M48_P5_probe_synth      pinned    synth    0    -      -          -          pairing
 M48_A1_m47probe_native  pinned    native   n/a  -      -          -          pairing A-side
pose_match=True, modal_rot (0,0,0), distinct=1, modal 100%, speed_ratio 1.0000016-1.0000023.

2. THE DETECTOR'S FIRED COUNT, AND ITS PROOF
=================================================================
frames_exposure_dip = 10 on the AE-ON leg, at session_index 8,9,10,12,13,15,16,17,18,19.
INSIDE the predicted 9-12 range, and sitting on the session-start convergence transient
069-26 measured. Pinned legs read 0 in BOTH tick orders.

AUTO-EXPOSURE PROVEN LIVE ON THAT LEG INDEPENDENTLY OF THE DETECTOR, from the black-frame
gate's own luminance line: whole-frame mean spread 31.7 (74.2-105.9) vs the pinned legs'
7.2 (102.3-109.6); mean 80.0 vs 105.7 (-24%). A 0 there was a pre-declared FAIL; a 0 read
without that proof would have been blindness, not a clean result. G96 satisfied.
Black-frame gate --selftest: PASSES grey, FAILS all-black.

3. KEY COUNTS - P-C7 v2, additive, vs the banked m47 P1 leg
=================================================================
run_summary        58 -> 59   ADDED exactly {frames_exposure_dip}   REMOVED none
labels.jsonl       21 -> 21   IDENTICAL (healthy runs gain no key - m47 precedent)
annotation.json    diff EMPTY - P6 did not move
Both build targets exit 0: packaged StackOBot AND editor StackOBotEditor (the modular
link, G221).

4. THE PAIRING PROBE - REPORTED WITH ITS A-SIDE; ITS LITERAL CLAUSE DOES NOT PASS
=================================================================
The standing rule is NEITHER==0 AND PREVIOUS==0. On this fixture with the census on,
NEITHER is not 0. Rather than call that an m48 regression or wave it through, I ran an
A-side control on the ARCHIVED m47 exe F309D836 (staged and re-staged around it):
  m47 F309D836 native : decidable 79  CURRENT 25  PREVIOUS 0  NEITHER 54  no-data 11
  m48 DE65F84A native : decidable 80  CURRENT 26  PREVIOUS 0  NEITHER 54  no-data 10
  m48 DE65F84A synth  : decidable 79  CURRENT 26  PREVIOUS 0  NEITHER 53  no-data 11
PREVIOUS==0 - the half that detects a one-frame mis-pairing - holds on all three, and the
NEITHER band is IDENTICAL on the pre-m48 binary => m48 MOVED NOTHING, which is what
"pairing unchanged" asked for. This is NOT a claim the NEITHER==0 clause passed; it did
not, including on the control. Reported, NOT attributed.

5. WHAT SHIPPED
=================================================================
Per-frame `exposure_dip: true`, additive, emitted only when true. Threshold 4.0% below the
rolling mean of the previous 8 CAPTURED frames - DERIVED TWO-SIDED (pinned max drop
<= 2.39%, AE-ON reaches 7.27-9.01%; margins 1.67x / 1.82x). run_summary
`frames_exposure_dip` (+1). The first 8 frames of a session can never be marked - stated in
the log line, client README, ledger, architecture.md.
Luminance: AnomalyCaptureSubsystem.cpp:3128-3142 (ProcessCompletedFrames, on the raw
readback before any resample) calling AnomalyLabel::ComputeSubsampledMeanLuma
(AnomalyLabelWriter.cpp:187-241) - stride 3, i.e. 1 pixel in 9, 102,400 samples of 921,600,
one pass, no allocation. The per-pixel decode was EXTRACTED into DecodeTightPixel and is
now shared with ConvertTightToBGRA, so luminance and the delivered image cannot disagree
about what a pixel is. Two-pass over the drained batch, because the render-thread drain
appends in reverse.
(iii) REFUSED as briefed: the plugin never forces exposure.

6. BINARIES / TOOLING
=================================================================
Staged bench exe DE65F84A (241,236,992 B). Predecessor F309D836 archived FIRST as
_binary_baselines\StackOBot.exe.m47-shaderprewarm-F309D836, hash-verified at the archive,
and LOAD-BEARING as m48's pairing A-side. Container quartet UNCHANGED
(EF8EB23C/A8BFFF88/3C026A8D) - code-only hot-swap, NO COOK (G103).
A44 both encodings on the STAGED artifact: exposure_dip 0/5, frames_exposure_dip 0/3,
"EXPOSURE DIP" 0/2, "EXPOSURE DIP SUMMARY" 0/1, alongside pre-existing shaders_pending 0/5
and IAI.Capture.ShaderPrewarm 0/7 => reached the package AND the scan is sound, not blind.

m47_lum_table.py's latent bbox_px bug: ALREADY FIXED, and NOT in b6e5588 - that tool lives
in the CaptureBench repo (local-only, no remote), corrected at d0d8643. The plugin repo
does not contain the file, so it could not have been fixed there. Stated rather than
silently satisfied. run_leg.ps1's -AutoExposure switch was likewise already in place from
069-26, so no harness change was needed.

7. DOCS
=================================================================
journal 069 §19 (incl. §19.0 recording the two API-500 deaths and the resume) · ledger 11.6
· G234 (an onset reading under live AE must control for session position) · client-readme
(the paragraph, near-verbatim) · client-delivery (the AE-ON smoke leg, with "prove exposure
is live before reading the count") · PRE-DELIVERY-CHECKLIST (three boxes, incl. "dip 0 with
AE live = detector FAILURE, blocks delivery") · office-rdp-card F-2 (h) · architecture.md ·
CLAUDE.md status. Comment stripper: 0 changed / 95 no-change. Diffstat checked before
commit (G115): 510/40, proportionate, no encoding smell. Owner's 8 untracked CHAT-HANDOFF
files NOT staged.

8. LIMITS CARRIED
=================================================================
m48 is NOT a fix for (2) "the target renders BLACK" - that stays UNEXPLAINED; AE was
refuted for it structurally and m48 marks a GLOBAL dip. The mark is not a defect flag.
G135 guard carried: the AE-ON leg's DARK FIRST FRAMES = 0 is a null on a target that is
7.2-7.8% of frame.
```
api-equivalent cost (not billed on Max): $16.33 | turns: 84 | running total across all briefs: $642.25
