# REPORT for 084-05a-sync-fixes-source.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-05a-sync-fixes-source.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 23:13:42 | finished: 2026-09-28 23:46:42 | duration: 33 min | exit code: 0
context at last turn: 628773 tokens (63% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-05a-sync-fixes-source.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Summary for the owner**
All three fixes chat ruled on are built and archived as exe `2AC0523E`. That covers G350, the TAA `transition` flag and mask-ID recycling. Nothing was launched or staged, so there's no runtime evidence yet. The 189 offline unit checks pass, and each fix is shown failing the old way and passing the new way. The 5 unmeasured-mask frames from 084-03 are all the known G295 problem, not something new.

**Each item**
- **G350 (`AnomalyCaptureSubsystem.cpp` `ApplyRenderTruthToSnapshot`, `AnomalyLabelWriter.cpp` `BuildFrameLabelRecord`):** a `stuck_low_mip` entry now counts as positive only on a frame labelled for it. Frames after the event closes carry no positive entry, and neither do frames before the blur takes hold. That second part goes a little beyond "post-closure", but it is what the ruling's wording says. `anomaly_present`, `visible_positive` and `positive_frames` all follow from this.
  - The internal bookkeeping that keeps a closed event attached (F2) is unchanged.
  - Rows for every other anomaly are byte-identical: with no per-entry flags set, the code reduces exactly to the old `Fires.Num() > 0` rule and emits the same entries in the same order. The argument is in journal §1.
  - The unit test for this works at the logic level. The JSON writer needs the engine, so the byte-for-byte claim rests on that source argument.
- **`transition` flag:**
  - **Detection:** at run start the plugin reads the engine's anti-aliasing setting (`r.AntiAliasingMethod`); TAA or TSR counts as temporal. The method and the values used are logged once per run and written to `run_summary` (`label_*` keys).
  - **Settings:** `IAI.Label.TransitionOnFrames`, `IAI.Label.TransitionOffFrames` and `IAI.Label.TransitionHideFrames`. The default `-1` means 2 / 8 / 1 under temporal AA; without temporal AA every value is 0, even an explicitly set one.
  - **stuck_low_mip:** the first 2 labelled frames keep their label and add `transition: 1`. The 8 frames after a labelled frame get a transition-only entry that does not set `anomaly_present`, including after the event has detached.
  - **blinking / missing_object:** the first captured frame after the object comes back is flagged.
  - Rows with any flagged entry get `transition_present: true`. The pure logic is in the new header `AnomalyLabelSync.h`.
- **Mask-ID recycling (`AnomalyMaskMeasure.cpp`, `AnomalyStencilTag::RestoreComponentsCarrying`, `RefreshMaskTagReleasability`):** when all 55 values are taken, the plugin takes back the value of the oldest finished event and restores that event's object first.
  - "Finished" means the fire has ended, any stuck-mip restore trail is detached with nothing in flight, all its frames and masks have been read back, and its extra mask measurements are done.
  - Runs that never run out of values allocate exactly as before.
  - Recycles and the peak number of live values are logged (`TAG RECYCLED` lines) and written to `run_summary` as `mask_tag_recycles`, `mask_tag_peak_live` and `mask_tag_exhausted`.
- **G295 finding:** all 5 frames log `TARGET MASK UNAVAILABLE … pixels=0` (B3 si 1171, B9 si 194, B7 si 126/613/615). In B9 si 194, one mask render served both si 193's and si 194's requests, and the pixels went to si 193. That is G295's coalesced-mask signature. I didn't fix it: the shared render belongs to si 193's picture, so copying it to si 194 would pair a mask with the wrong frame.

**Unit tests:** 189 checks, 0 failures (130 existing, 59 new).
- **G350:** the old rule puts `anomaly_present` on 34 unlabelled frames; the new rule puts it on exactly the 12 labelled ones.
- **Transition gate:** the old code fails 7 of 7 cases built from 084-03's measured smear; the new code passes 7 of 7, and still fails at +9 or onset +3, so the check can fail.
- **Hide-type return frames:** fail on the old code, pass on the new.
- **More than 55 events:** the old allocator leaves 35 of 90 events without a mask; the new one tags all 90 with 35 recycles.
- **No aliasing:** the old allocator never recycles, so it can't fail this case. The failing comparison is a deliberately broken variant that releases a value as soon as the fire ends: it puts two events under one value and even recycles a carried-over event's value. The ruled logic does neither, including across a run boundary.

**Build**
- Game target exit 0 (3 actions, 65 s) and Editor target exit 0 (10 actions, 38 s), 0 compiler warnings in both.
- A first Game attempt failed at a compile error (`UWorld::GetFeatureLevel` doesn't exist in 5.1). I fixed it; its log was overwritten, and it showed no warnings.
- Exe `2AC0523E26B9B75EB3FE542F25CF99A25207CEB3E5A2968FA6351E1628205125`, archived with both logs at `_binary_baselines\m52fix-2AC0523E\` (hash re-checked there).
- String scan finds all 15 new tokens; the two control strings read 5 and 6, as before.
- Not staged: the staged exe is still `2FCDF059`, and the main checkout is still `m51` `53bf725`.

**Commits pushed** to `fix/m52-label-timing` (`44584e7..ce84394`, equal to origin): `05c7649` (source and tests) and `ce84394` (journal `2026-09-28-084-05a-…`, status block, client readme §8.6a plus the field rows, pre-delivery checklist, architecture, G350 marked fixed, and a new G351). No tag, no merge; `master` is still `b5f15a3`.

**NEEDS-DECISION:** none. These interpretations are in journal §7 in case chat wants to reverse any:
- **G350 applies to stuck_low_mip only.** Extending it to all anomalies would change blinking's visible in-burst rows and break the byte-identity requirement.
- **Recycling happens only when the pool runs out**, not the moment an event ends. Releasing early would change the census's choices, and so which targets get picked, on every census-on run, including short runs that never get near 55.
- **084-05b note:** the harness should also leave `transition_present` rows out when choosing reference frames.

Several connectors (Gmail, GitHub and others) need authorising in their connector settings, and the PDF viewer server failed to connect. This task didn't use any of them.
api-equivalent cost (not billed on Max): $21.11 | turns: 176 | running total across all briefs: $3,177.99
