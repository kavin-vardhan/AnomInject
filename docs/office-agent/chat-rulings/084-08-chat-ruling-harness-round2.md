# 084-08 — Chat ruling on part A (four fixes) and the 084-09 harness (NEEDS-DECISION): accepted; the LOD fixture is authored by day; MASK55 v2 via the m52 trail route

Date: 2026-09-29 ~06:25 IST · Author: chat Claude (owner offline, per his instruction) · Branch `fix/m52-label-timing` `17b9d5d` (source `4bf0915`) · Exe `8A6074AA` + cook utoc `4BF63FB8` (archived, not staged)

## Accepted
- **Part A:** A1 (shear-case instances are unconfirmed), A2 (a forced-unknown set is `unresolved`), A3 (welded candidates are unconfirmed), A4 (the `mask_prior_collision` tripwire and quarantine).
  - Selftests 282 + 154; **16/16 mutants fail**. Build 0 warnings.
  - Re-cook with every prediction met, a byte-identical descriptor, and an identical content manifest.
- **The 084-09 harness:**
  - G355 (the injected lock fails twice, then succeeds on attempt 3; a permanent lock raises);
  - G356 (the null calibrates from its own series);
  - the partial guard with all four must-fail cases;
  - `--label-rule` on every leg;
  - B-CC / B-CC-MW / MASK55 v2 gates, each shown to read the 084-06 bank as INVALID or FAIL;
  - **G368**, found by its own selftest before any real read;
  - the dry run: 29/29 launches accepted.
- **The G356 re-evaluation:** B0 now 16/16 PASS. **B3 has one FAIL: a single visible frame at label end +16, not connected to the tail.** Tonight's B3 re-run must read it again. If it recurs on the new build, it is a finding to investigate, not a harness artefact to excuse.
- **The missing_texture re-check:** amendment R1 was declared before the read and was not loosened after it (G369, correct conduct). StaticMeshActor_85 = CONFOUNDED-REFERENCE, and **the MT85 wide-gap pair decides it tonight.**

## Rulings
1. **LOD fixture: author one by day. Keep the rock premise legs as the realistic check.** No MainWorld mesh meets the declared rule (the best ratio is 7.997, and chat will not relax a declared rule by rounding).
   - **084-08b (xhigh):** a bench-only fixture map (for example `CB_LodFixture`) in the StackOBot project, with meshes built from engine shapes by an editor commandlet:
     - LOD0 and the forced LOD must differ strongly in silhouette **and** carry distinct per-LOD materials;
     - **one variant must use a material with dithered LOD transitions** (the 086-01 candidate lag), and one must not.
   - Then re-cook. The **content manifest diff must show only the new fixture files.**
   - Pin the new pairing in the 084-09 harness, and add the LOD premise + gate legs on the fixture (≥ 16 judged events per condition, AA on and off, both tick orders).
   - Dry run.
   - **If the dithered variant shows a fade across frames, that is a real lod_popping label issue** (a partial transition, like m52's). It is reported, not excused.
2. **MASK55 v2: accept the m52-trail route.** Put `stuck_low_mip` in the pool (its restore trail overlaps the next event, as 084-06's B0 showed with peak live 2), use `IAI.Bench.TagPoolLimit 12` to force recycling, and treat peak live ≥ 2 as a validity condition.
   - **G366 is a product fact** (one anomaly per burst in capture), and it goes to the m53 plan: the only way m52 and m53 can overlap in auto-injection is through a restore trail or revert latency.
3. **B-CC on the part-A cook:** correct, per the 084-07c ruling.

## Next
1. **085-01 (m53 DC plan), released now.** Its ACK carries G366.
2. **084-08b** (the LOD fixture, re-cook, harness pins, dry run).
3. **087-01** (the office kit).
4. **085-02** (m53 source).
5. **Tuesday night:** 084-09 (~66–95 min), and the m53 premise if 085-02/03 are ready. Otherwise Wednesday night for m53.
