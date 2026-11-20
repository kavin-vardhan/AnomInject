# 084-02 — Chat ruling on the m52 fix build and Codex's review (CHANGES-REQUIRED): correct F1–F7 before the bench; a two-sided end gate; the purity scope

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `fix/m52-label-timing` `749a3bd` · Fix exe `562EE7A4` (archived) · Review `_reviews\084-02-codex-m52-fix-review.md`
Governing rule: `085-00` ADDENDUM. Label sync, 0 frames off on **both** edges, from pixels.

## Accepted
- **The wrong-object cause from source:** the old check counted only on-screen, polled, ≥ 6 %-coverage actors, only static and skeletal meshes, skipped targeted fires, and ran once at Apply.
- **The design:**
  - a per-captured-frame render record where unknown means labelled, never clean;
  - one authority driving label, mask, `held` and `observable`;
  - the `RestoreTrail` phase and burst gating;
  - the `restore_unresolved` timeout;
  - `TEXUSERS`;
  - levers only under `-IAIBench`.
- **Builds:** 0 plugin warnings; unit test 41/41; the harness dry run reproduces the diagnosed baseline on banked pixels; doctored labels are proven to fail on real pixels.
- **G344:** the streamer keeps sharpening for ~150 frames, so the pixel reference is the flat pre-event level. Good catch.

## Decision 1: correct first, then the bench (not a measurement night on a known-flawed build)
A plugin rebuild takes ~85 s on the warm host, so there is time to correct by day and run tonight.
- **F1:** the trail may close only when the render record is at baseline on 2 consecutive frames **and** no streaming work is pending for any held texture (restore generation / pending-update check), **and** the 084-01 "back but still streaming" race is resolved.
- **F2:** trail updates are strictly ordered by captured frame across drain batches. A held (or unknown) receipt arriving for a frame at or after the closure **reopens** the event and re-labels those frames. Closure is final only after an ordered, gap-free window.
- **F3:** a frame with an m52 fire but no watch list is **unknown** and therefore labelled, unless it is proven to be before Apply by frame ordering (the capture index is below the Apply frame).
- **F4:** purity is established below (decision 3).
- **F5:** an unresolved restore survives run stop and start: its ownership, reservation and refusal persist until the record shows baseline. A frame-cap cut during a live window is counted and reported.
- **F6:** m55 is never given a provisional "not labelled". Membership is final before m55 consumes an entry, and queued entries flush at shutdown.
- **F7:** the deferred-mask age is carried across service, so the 16-tick drop fires as advertised.
- **Shipping:** the legacy-purity flag and its branch compile out of Shipping, like the levers.
- **Render-resource replacement:** a held texture's render resource changing identity mid-event means that frame is **unknown**, so it is labelled, and this is logged.

## Decision 2: the end gate is TWO-SIDED
- The owner's rule is 0 frames off on both edges, so **over-labelling after the last visible frame is also a failure**, as is labelling before the first visible frame.
- **Gate (i) becomes:**
  - labelled window start = the first pixel-visible frame;
  - labelled window end = the last pixel-visible frame;
  - both on measurable content, with the visibility threshold taken from independent nulls;
  - frames in uncaptured gaps are declared censored, and a **valid clean suffix** is required.
- **Can-fail cases, all of which must FAIL:** onset +2, offset −5, early-onset over-label, and late-offset over-label (Codex's 4-frame counterexample).
- **`SettleTailFrames` stays at 0 in the build.** Tonight's legs **measure** the residual after the render record returns to baseline (with and without anti-aliasing), and report the closure rule that would give 0 frames on both edges. If a residual exists, chat rules the closure rule after tonight: a measured constant, or an in-engine settling detector using m55's per-frame change inside the target mask. The fix legs may fail the two-sided gate tonight by exactly that residual. That is expected and reported, not hidden.

## Decision 3: the purity scope
- **At Apply:** count users across **all loaded levels, active or not**, including **unregistered** components in loaded levels, every primitive type and decals.
- **During the hold:** subscribe to level-added and component-registered events. If a new user of a held texture appears:
  - **revert immediately;**
  - flag every frame from that point until the render record shows baseline with `stuck_mip.contaminated = 1`;
  - log it.
- `TEXUSERS` must use the same scope, and gate (iii) checks it. The legacy-purity lever must be caught.

## Sequence
1. **084-02b (xhigh, fresh session; 084-02 ended at 95 %):**
   - F1–F7, Shipping, resource replacement, the purity scope;
   - unit tests extended to each fix, both ways;
   - the harness gate made two-sided, with all four can-fail cases proven;
   - rebuild (Game and Editor, 0 warnings), archived;
   - a **Codex mini-delta** through the Relay (collect it; do not act on it);
   - the 084-03 invocation.
2. **084-03 (high), tonight on the owner's go:** part B on the baseline and the corrected build.
