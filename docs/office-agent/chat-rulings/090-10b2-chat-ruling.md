# 090-10b2 — Chat ruling (NEEDS-DECISION): accepted; m53 `d7e396d` is the morning pull for blurry textures; the PIE end+1 is mitigated by a PIE-only end-settle flag (090-10e); the census actor-list split goes in the same small round; blurry at the office waits on the office counts

Date: 2026-10-02 ~01:55 IST · Author: chat Claude · Heads m53 `d7e396d` (code = `61d186a`, exe `E2BCE76B`), fix `d3c9423` (exe `5E93A74E`) · Report `_mailbox\to_chat\090-10b2-m52-pie-wrapup-census.report.md`

## Accepted
- **The 090-10b write-up,** cause and can-fail (the office exe fires 0 in PIE).
- **The new readiness order** `5fba7df`:
  1. metadata;
  2. **identity** (`default_material_path`);
  3. **draw readiness** (the main pass's vertex and pixel shaders for the component's vertex factory: `compile_pending`, or `draw_shaders_missing`).

  Four mutants fail. In PIE: identity refused every time with 0 events; compile-pending refused and then admitted (13/13 visible); healthy 28/28. **Codex's other direction is closed in-engine,** except a permanent `draw_shaders_missing`, which rests on the pure test and the source. That's acceptable.
- **Part B's premise is refuted, and accepted:** the PIE refusal on the fixture is the deliberate shared-texture pair. The editor world is not counted. Blurry passes E1 in PIE 17/17 and staged 16/16; the auto-pool blurred 5/5. **Chat's premise was wrong; owned.**
- **The census bound** (no frame over 100 ms): MainWorld PIE 36.7 ms and staged 18.0 ms; office scale (15,343 actors) 63.9 ms in PIE. The can-fail fires on the office exe (150.8 / 192.3 ms).
- **Handled correctly:** 090-10b's interrupt leftovers (the staged package, the stale DLLs set aside); 10.6 GB banked; checklist §0 "visibly worked in PIE and staged".

## Rulings
1. **Morning pull for the office: m53 `d7e396d`.** It gets blurry textures working in PIE on home content; UV/normal still meets `runtime_lod_bias` on office content until 090-10c lands.
   - **The owner's errand:** a blurry-only auto-capture in PIE, then `python tools\anomaly_refusal_counts.py "<the game's log>"` (verified, numbers only) and read back the m52 lines.
   - **Why blurry never fired at the office stays unknown until those counts arrive.** It could be real-content texture sharing (the purity rule), the ratio wall, or LOD bias / residency. No code is written blind.
2. **The PIE end+1 (about one event per PIE run; the picture is still corrupted one frame after the label ends; not in staged; also on `0a69ea3`):** this is a label-sync defect in the configuration the office uses, so it must not ship as an unflagged mislabel.
   - **Mitigation in 090-10e:** in PIE only, the first captured frame after a FireWindow label ends carries `transition_reason: pie_end_settle` (unlabelled, dropped).
   - **Proof:** the PIE end+1 events then read 0 off outside flagged frames; staged is unchanged (no flag); a mutant that flags staged frames or flags nothing must fail.
   - **The root cause is investigated as a follow-up item,** not this week.
3. **Census actor-list time-slicing** (the first frame at 40k actors is 195 ms; the 650 ms frame's cause is unknown): it goes in 090-10e with the same bound and proof. Office scale already passes.
4. **090-10c** (the m53 LOD bias / A1–A4 / B / all-reasons census) is running since 01:38. **090-10e is queued behind it.**
