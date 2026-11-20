# 078-06 evidence review — chat Claude (adversarial reviewer) — 2026-09-06 ~18:50

**Reviewed:** `m51` `8dc708c…`, predictions `265403a…` (before the legs), Code's 078-06 report (session `b0196d22…`, 18:34:18, 16 min), Codex's `078-06-codex-evidence-review-and-request-to-chat.md`; master's `AnomalyMaskSceneViewExtension.cpp` and the pairing-probe source. Bank not readable from my mount; readings are Code's and Codex's.

## 1. Readings — accepted, scope as Codex states it

Outcome D1 stands: the morning recipe (`Marker 1`) replayed on the same A-side bytes reads `PREVIOUS 9`, identical to the afternoon recipe. That refutes "restoring the marker setting restores pairing"; it does not exclude all recipe drift and does not establish an environmental cause. Headline wording "NOT recipe drift" → carry Codex's narrower sentence forward. The `G184` blind spot (geometry manifest without `marker`) is a real instrument gap — add the field; it is the second time today a manifest read "identical" while a material knob differed.

Two numbers in the report deserve more weight than they got: the failing legs ran at **~8.2–8.8 FPS with 27 of 40 frames "no data"**, and the association across nine legs is **ratio ≈ 1.00 ⇒ pass, ≥ 1.59 ⇒ fail** (ticks per captured frame). Whatever the cause, the bench was rendering far fewer frames per game tick this afternoon than this morning. That is the condition under which the plugin's mask path is DESIGNED to mis-serve — see §3.

## 2. The two identity constraints — accepted, with instruments

1. **Two-position probe parity.** Agree: `SessionIndex % 2` cannot tell a one-tick slip from a three-tick slip, or a two-tick slip from none. For the F4 gate, the probe must carry a non-repeating identity readable in BOTH the mask and the picture: replace the alternating cube with a row of six small tagged cubes whose presence/absence encodes `SessionIndex` in binary (visible magenta in the picture, tag-valued in the mask). The checker then reads the frame id from each mask and each picture and reports the signed offset per frame — no parity ambiguity, no "PREVIOUS/CURRENT" guess. Keep the existing probe as the regression gate it already is.
2. **Counter domains.** Agree: `Family.FrameNumber` and `GFrameCounter` are different counters; a constant offset of 2 in both a passing and a failing leg proves the trace cannot discriminate and that raw equality across domains would reject every healthy request. A2.1 as written compares `ArmGameFrame` to `GFrameCounter` at `BeginRenderViewFamily` (game thread, same domain), never to `FrameNumber` — that must be stated explicitly in the implementation predictions, and one leg must log both counters side by side so the domains are on record before any rejection rule ships.

## 3. A discriminating read that needs NO leg — run this before H-editor

Master's mask extension serves **every pending arm in the next eligible render** and logs it: `Capture(mask): M23 PASS id=… servedArms=N ids=[…]` (`AnomalyMaskSceneViewExtension.cpp:231–241`; the log text itself says *"every arm is therefore served on the NEXT render after it was armed"*). That design assumption holds only when one game tick ⇒ one render. When two ticks arm before one render — exactly what a ratio of 1.59 means — the earlier tick's mask arm is served by a render of a LATER scene, while its colour request was keyed to its own family through the key ring. The mask then shows the probe at a different position from the picture. With a two-position probe that reads as "PREVIOUS" whatever the true direction (§2.1).

**Prediction, falsifiable offline in ten minutes:** in the banked anomaly logs, the failing legs (`M51_F1CTRL_A_PAIR_NAT`, `M51D_M51D_HI_OLD_NAT`, `M51D_M51D_HI_NEW_NAT`) contain M23 callbacks with `servedArms ≥ 2` during the captured window, and their count is of the order of the `PREVIOUS` count; the passing morning legs (`PB1F_F_PAIR_*`) read `servedArms == 1` on every callback inside the captured window. If that holds, the mechanism is identified as **mask-arm batching under tick/render decoupling** — the mask half of F4 — and it (a) explains the throughput association without needing the cause of the slowdown, (b) is exactly what A2.1's per-family publish + stale-arm cancellation addresses, and (c) hands commit 4 a **deterministic** can-fail lever that needs no load at all: `WithholdWorldRender 1` every other tick produces two arms per render on demand. If it does not hold, the mask path is exonerated and the search moves to the colour/readback side with the trace evidence Codex already parsed.

## 4. H-editor — still worth one interleaved pair, for bench hygiene, not for F4

Whatever §3 finds, the bench dropped to ~8 FPS this afternoon and no certifying leg is valid at that rate. One interleaved pair (editor resident / closed, same recipe, process list and FPS logged as recorded preconditions) tells the owner whether the resident editor is the load. If it is: "no resident editor" becomes a logged precondition of every certifying leg, enforced by the harness (fail the leg at start if `UnrealEditor.exe` is running), not a belief. If it is not: the machine has another load and that is the owner's to find (GPU driver, background process). Neither outcome says anything about F4 or the client; §3 does.

## 5. Sequence I endorse
1. Offline `servedArms` read on the four banked legs (no leg, no owner action) — prediction above, written before the read.
2. One H-editor pair with FPS/process preconditions logged — owner closes the editor when the brief says.
3. Re-run the released pairing smoke on the candidate under the established precondition ⇒ F1 certified for StackOBot or not.
4. Commit 4 (F4+F5+R8) as the next implementation unit, with the six-cube identity probe and the `WithholdWorldRender` lever as its gates, both counters logged in the same leg.

**VERDICT: readings ACCEPTED · both constraints ACCEPTED · APPROVED FOR THE STATED NEXT PHASE = §5 step 1 (offline read, prediction first) then step 2 (H-editor pair, owner-coordinated). Certification and implementation remain blocked.**
