# 089-01 — Chat ruling on the offline adjudication (NEEDS-DECISION): no real label problem found; the aligned null is accepted provisionally and must be confirmed prospectively; stuck_low_mip TAA off-frames go to 16; Route A for MASK55; arms re-declared

Date: 2026-09-30 ~00:05 IST · Author: chat Claude (owner asleep) · `fix/m52-label-timing` `89dc788` · Review `_reviews\089-01-adjudication.md`

## Accepted
- **Every disputed FAIL is classified, with per-event evidence:**
  - blinking SYN: E1 artefact (G405);
  - B9 / B0 / B3: 086-01 reference drift, or E1 null misalignment (G406), shown against a game-time-aligned null;
  - RD3: an E1 artefact (G395); the truth is end +1.
- **B3 si 181 is not a desync.** The render record is back at 11/11 baseline mips on the first unlabelled frame. No other anomaly was active, and B9 on the same rock has exact offsets. The "41-frame tail" is TSR history (0.81 / 0.36 / 0.14 of depth at +1 / +8 / +16) measured against a null 16 frames off in game time.
- **MASK55's 270 unmeasured frames** are three Nanite targets, admitted unmeasured by the product's designed Nanite rule. It is not G295, not the lever, and not a regression.
- **LOD `worst=2`:** a 1-based engine value beside a 0-based index, so only a log wording issue.
- **The arm sizes:** the levers count engine frames and the capture skips 2 settle frames, so a +3 arm needs D = 5. The arms still prove can-fail at +1.

## Rulings
1. **The game-time-aligned null: ACCEPTED as the stuck_low_mip comparison basis, PROVISIONALLY.** The classifications resting on it are accepted for now. **They must be confirmed prospectively on the re-bench:**
   - a null of ≥ 1,500 frames, run immediately after the m52 legs and covering every leg's time range;
   - Part 0 runs last, so every leg starts at frame 1;
   - E1 and the kit both use the aligned null.

   A re-bench FAIL under the aligned null is REAL until shown otherwise.
2. **The TSR residual after a stuck_low_mip label: keep t50 as the release rule, and raise the flagged tail to 16 frames** (`IAI.Label.TransitionOffFrames 16` under temporal AA; today it applies only to stuck_low_mip).
   - The residual is below 50 % by +5..+8, but 5–45 % of the blur lingers to +13..+29, and a flagged extra frame is the cheaper error (084-05b's reasoning).
   - **Re-bench:** pass it via ExecCmds on the AA-on m52 legs. **Delivery:** the source default becomes 16 in the next source round, and the readme states it.
3. **MASK55 concurrency: Route A** (B0's recipe + `TagPoolLimit 12`; peak live ≥ 2 and recycles ≥ 1 as validity conditions; no build). If it doesn't reach peak 2, it reads INVALID and F4 rests on its unit tests (as today).
4. **The arms:** add CD5 / RD5 as the +3 arms, predicted in captured frames, and keep CD3 / RD3 re-declared as +1 arms. **No lever change;** the levers' engine-frame unit is documented.
5. **Nanite targets (a product fact the owner will hear):** on UE 5.1, events on Nanite objects carry labels and boxes but **no pixel mask** (declared unmeasured). That is the existing, documented behaviour. Chat recommends keeping it: excluding Nanite would remove most real-content targets. It goes on the owner's Thursday list as a product call.
6. **The LOD log wording** (`forced_lod_model=2 (LOD index 1 of 2)`): in the next source round.

## Next (overnight)
- **089-02a (xhigh), fix-branch side:**
  - E1 fixes (G395 reference exclusion, G405, G406 → game-time-aligned null);
  - the LOD fixture light + cook;
  - the re-bench leg set for m52 (null ≥ 1,500 frames placed right after, OffFrames 16 on AA-on legs), MASK55 Route A, the LOD fixture premise + gate, and blinking SYN;
  - Part 0 last;
  - a dry run.
- **089-02b (xhigh), m53 side:**
  - G403, with hosts eligible at 128 MiB reached by placement (the declared raise only if none can be framed);
  - the P-ORC / G-MODE fixture, G-BIND, and the MIXED fixture (the colour-only slot gets a base-colour texture) + cook;
  - the G-LEVER-OFF predicate, G-CENSUS one command per frame, and the CD5 / RD5 arms;
  - a dry run.
- **089-03 (high):** the targeted re-bench of both when the PC is idle.
