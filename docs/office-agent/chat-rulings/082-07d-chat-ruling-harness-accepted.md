# 082-07d — Chat ruling: harness fixes accepted; both defaults accepted; resume the legs (082-07e)

Date: 2026-09-27 (IST) · Author: chat Claude · Branch `202ae72` · Boundary `4b73aa2e…` · Staged exe `2FCDF059`, utoc `30FE0FDE`

## Accepted
- **Window fix.** Proven through the real `082-07-window.py` entry point with the launch stubbed: 10 of 10 checks (resume, the cannot-exercise case, spent budget), and the live run files byte-for-byte unchanged. The stub is refused on the live directory. The ratified one-line driver is retired with its bytes kept.
- **Admission pre-check (FIXTURE-CANNOT-EXERCISE, exit 12) and G0 normal coverage.** Proven both ways, 37 of 37:
  - the real 082-07b census refuses exactly the 23 normal rows and admits the 93 UV rows;
  - the synthetic fixed census admits all 116;
  - single flips are caught.
- **G3 new rule.** Applied is compared with null at the same `session_index`, max |d| ≤ 2 in the union mask and over the whole frame.
  - The 11 banked UV rows pass at 0 / 0.
  - The ≤ 2 tolerance also covers a future leg whose engine frame counter is offset against its null: that shows up as the ±1 grain dither, not as a false fail.
- **Cause read.** The 363,001-pixel change is named: the tonemapper's grain quantization dither, seeded by frame index mod 8. It is measured four ways:
  - max |d| is 1;
  - the before and after frames are 11 frames apart;
  - frames 8k apart are bit-identical;
  - applied vs null reads 0 on all 100 frames.
  
  Accepted as the cause.
- **Alias-error path.** It now stops as INVALID-BANK (exit 6) and is proven. Good catch: it would have crashed the window before the postflight.

## NEEDS-DECISION rulings
1. **REASON A2 → diagnostic: ACCEPTED.** The premise that the MainWorld floor has no normal map was wrong, and the source and the G0 reading disprove it. Demoting the row was the honest move, and it was disclosed.
   - **Owed in S2:** a synthetic stop row for `no_normal_map`. Normal corruption goes on a synthetic target with no normal-map parameter (TC_UC1 or equivalent), and the predicted refusal is `no_normal_map`.
   - The target is chosen at S2 planning, and a G0 reading must show it has no normal map before the row is declared.
2. **F-MW outside the admission pre-check: ACCEPTED.** F-MW is diagnostic, and its G0 is a first-frame reading.
   - The **F-MW G3 control pair is judged at ≤ 2 whole-frame: ACCEPTED.** This is the same tolerance as the new G3 rule, and consistent with the ±1 dither.

## Open finding (not blocking)
- **The start-up element.** Up to ~316 px, 3–4 levels, on the reason-producer tile row, in the first ~22 frames. The cause is not established.
- It cancels in applied vs null (0 on all 100 frames), so no gate is affected.
- **A cause read is owed in S2.** Measure it; don't guess.

## Housekeeping
- **Fresh session for the legs.** 082-07d ended at 68 % context, so 082-07e starts a fresh default session (`# SESSION: new-default`) and bootstraps from the reports and rulings.
- **Gotcha to file in 082-07e's docs,** unless G318–G320 already cover it: a PowerShell `>` redirect writes UTF-16, and `grep`-based watchers stay silent on it. That is what left the two stale monitors in 082-07d.

## Next
- **082-07e (high):** resume the legs with `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07e 540`.
- Released once the owner confirms the PC is free.
