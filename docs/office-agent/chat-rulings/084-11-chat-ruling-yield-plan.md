# 084-11 — Chat ruling on the m52 yield plan (NEEDS-DECISION): the plan is accepted; A-core is NOT built blind tonight; the office counts decide A-core vs B vs "available-but-off"; the rehearsal runs now

Date: 2026-09-29 ~17:50 IST · Author: chat Claude · Branch `fix/m52-label-timing` `ebe856d` · Plan `docs/predictions/2026-09-29-m52-yield.md`

## Accepted
- **The measurement:**
  - the commandlet scans agree with the runtime purity lines up to 7 users; G397 (32×32 textures before async compile) was caught and re-run;
  - MainWorld: 29 streamable textures; 610 sampling components, of which **445 are Nanite**; 342 renderable actors, 244 entirely Nanite.
- **Yield on home content:**
  - today 2/342, A K = 8 2/342, B 2/342; A′ (Nanite unmasked) 6;
  - the settled-camera set is unchanged across the rules;
  - the measured fire rate is 9 in 24 bursts;
  - **the ratio wall** (≥ 512 px on screen) is a second limit, separate from sharing.
- **The A-core design:**
  - greedy admission, fewest users first;
  - one mask value per component;
  - one entry per affected component, from the one annotation authority;
  - K = 1 reproduces today byte-for-byte;
  - the picker filter `IsTargetEligible`;
  - A-ext after Thursday.
- **The gate list,** with can-fails (G-COAFF, G-YIELD, G-CAP, G-UNMASK, G-K1, G-MASKVAL, G-BYTE, sync unchanged).
- **The honest 40 % readiness estimate and the fallbacks.**

## Rulings
1. **Do not build A-core blind tonight.** At home, A barely moves yield: the limits are Nanite and the ratio wall, not sharing alone.
   - **The office counts decide,** read per plan §1.1:
     - **mostly `shared_world` with N ≤ 8 and maskable co-users:** A-core Wednesday;
     - **few eligible targets wasted by the picker:** B, the picker filter with K = 1, about 3 h and low risk;
     - **Nanite or the ratio wall dominant, or N ≥ 9:** fallback 2, ship `stuck_low_mip` **available but off by default** with a readme limitation. The proxy-texture approach goes to the follow-up delivery.
   - The owner has been asked for the counts.
2. **Nanite co-users: REFUSE** (no A′). A label without a mask fails the release rule's mask half.
3. **K = 8: accepted.**
4. **The picker filter is part of A.** With K = 1 it is option B.
5. **The ratio wall:** reported; no tuning this week.

## Codename note
Code reports that the repo's codename rule lists the host name in the brief as **retired (G222)**. The owner used that name today. Chat asks the owner which codename to use; until he answers, docs use "the office host".

## Next
- **088-01 (the delivery rehearsal), released now:** merge the kit branch into the fix branch, then fix into m53, then rehearse both paths. Useful whatever the yield decision is; re-run after any later change.
- **The night bench at ~22:30** as planned (part 1b gives the home yield reading).
- **Wednesday:** A-core or B or nothing, per the office counts.
