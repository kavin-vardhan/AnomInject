# 084-03 — Chat ruling on the m52 bench (part B): results accepted; G350 fixed; a TAA transition flag; mask-ID recycling; edge-local thresholds; overnight sequence

Date: 2026-09-28 23:20 IST · Author: chat Claude · Branch `fix/m52-label-timing` `44584e7` · Fix exe `B725678B`
**Owner (23:15 IST):** "Continue with the other stuff, we don't have time … you don't have to wait for my approval for checking if PC is free." The owner has logged off.
- Chat applies its own recommendations on the two pending product calls (the TAA transition flag and the mask-ID fix). The owner can reverse either on Thursday.
- The bench may run whenever the PC's quiet and person gates allow.

## Results (accepted)
- **Anti-aliasing off:** the fix is **0/0 on 16/16 events.** The baseline is −1 at the start and +4 at the end on every event.
- **Render window = label = mask** on 62/62 events. The purity can-fail is proven (P1 fails; P0 refuses).
- **The timing can-fail is proven** by the 086-01 checker (B0L −1 × 17).
- **Monitor** ≈ 0.2–0.3 ms. The streamer fence is ≤ 0.26 ms.
- **Edge paths:** reopens, ORDER HOLD, gaps and the fallback never occurred.
- **The noisy null was the scene's own ~12 s sharpness cycle (G349), not the owner's background rendering.** Chat's earlier guess is withdrawn.
- **Under temporal AA as delivered:** the picture reaches the 50 % blur **+2 frames** after the exact render flip at onset, and clears **+1…+7** frames after restore (outliers +15/+16/+22/+27 are unconfirmed and may be the scene cycle).

## Decisions
1. **G350 — FIX, don't document.** `anomaly_present` and the per-frame anomaly entries in `labels.jsonl` reflect **only anomalies labelled on that frame.** Post-closure bookkeeping never sets `anomaly_present` and never appears as a positive entry.
2. **The TAA smear — an additive `transition` flag** (chat's recommendation, applied). Labels stay on the exact render truth.
   - **The rule:**
     - under a temporal AA method (TAA or TSR, detected at runtime from the cvars), the first **K_on** labelled frames and the **K_off** frames after the last labelled frame carry `transition = 1`;
     - the frame gets `transition_present = true`;
     - the transition frames after the end do **not** set `anomaly_present`.
   - **Values:** CVars `IAI.Label.TransitionOnFrames` / `IAI.Label.TransitionOffFrames`, with provisional defaults **2 / 8** under temporal AA and **0 / 0** without it.
   - **The final values** are set from the re-evaluated bank plus tonight's confirmation legs, as the high percentile with a stated margin, by lock amendment (a CVar, no rebuild).
   - **The same flag covers the hide anomalies** (blinking, missing_object): the first frame after the object returns under temporal AA (086-01 ruling decision 1).
   - **The gate:** outside transition frames, the pixel sync gate is 0/0. Every unlabelled pixel-visible frame **must** carry `transition = 1`, otherwise FAIL.
3. **Mask-ID limit — FIX now** (chat's recommendation, applied). Mask IDs are recycled once an event's last masked frame has been read back. Proven offline, and on a bench leg with > 55 events in which every labelled frame has a mask.
4. **G349 — the harness threshold** is computed **locally around each edge** (null noise in a matched window at the same phase/si range, or a detrended null), proven both ways. **Tonight's bank is re-evaluated with it.**
5. **Gate (ii)'s 5 isolated unmeasured-mask frames** (−1 is honest) do not block m52. **Investigate them** (the G295 coalesced-arm signature?) together with the mask-ID recycling.

## Overnight sequence (chat dispatches without waiting for the owner)
1. **084-05a (xhigh):** source for decisions 1–3, then rebuild and unit tests.
2. **086-02 (xhigh):** the camera_clipping label fix (the view-slice between the normal and pushed near planes, pawn included, per 086-01) and the B-CC bench lever; rebuild.
3. **084-05b (xhigh):** the harness — edge-local thresholds, the transition-aware gate, the > 55-event mask leg, B-LOD, B-CC, B-REAL (and B-LYRA if time allows), the m52 confirmation legs; re-evaluate the bank; a dry run. Plus a **Codex source review** (Relay) of 084-05a + 086-02, collected, not acted on.
4. **084-06 (high):** the bench window, whenever the gates allow.
5. **Tuesday by day:** the m53 delivery-cut plan, source and build; the merge prep.
