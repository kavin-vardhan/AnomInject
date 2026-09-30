# m53 re-bench premise lock (089-04) - AMENDMENT to `2026-09-29-m53-dc.md` and `2026-09-29-m53-dc-lock.md`, committed BEFORE any gate leg

Premise lock `premise-lock.json` sha256[:16] **3639d7d4526168fa** (run root `D:\IntrusiveAnomalies\_reviews\089-04-evidence\run`; stub=False). Build under test: exe `6B671987` + the 089-02b re-cooked container `m53dc2-cook-45E2FA86`.

## Dispositions

- **G403:** G403 fixed (final parsed as head[:detail]). Host tiers, best first: view 128 MiB -> whole-world 128 MiB framed by L2 -> the declared raise. uv: no view host at 128 MiB; 26 whole-world hosts at 128 MiB are NOT framable (L2: IAI.Bench.PlaceView refuses map_not_allowed outside CB_GateLevel / L_ShooterGym and requires -IAIBenchFixture; its pose is a fixed constant); budget-only view hosts under the declared raise: SM_Ramp2_UAID_B42E9936F5429ADA00_2086822137 need 213949084 -> cap 268435456, RoomBuilderSquare_C_UAID_00155DE1D4771FCF00_1080865874 need 219541488 -> cap 268435456 | normal: NO host - no view actor at 128 MiB, the 9 whole-world hosts cannot be framed (L2), and no view refusal is budget-only ({'partial_footprint': 3, 'texture_not_parameter': 2}): normal MainWorld legs NOT-RUN-PREMISE
- **hosts:** uv H1 SM_Ramp2_UAID_B42E9936F5429ADA00_2086822137 / H2 RoomBuilderSquare_C_UAID_00155DE1D4771FCF00_1080865874 (cap 268435456); normal H1 None / H2 None (cap 134217728 delivered); probes {"U": {"hosts": [["SM_Ramp2_UAID_B42E9936F5429ADA00_2086822137", "raise", 268435456], ["RoomBuilderSquare_C_UAID_00155DE1D4771FCF00_1080865874", "raise", 268435456]], "probes": {"C1U": {"not_fully_resident": 2, "APPLY": 4}, "C2U": {"not_fully_resident": 1, "partial_footprint": 1, "APPLY": 4}}}, "N": {"hosts": [], "probes": {"C1N": null, "C2N": null}}}
- **ND-4:** bench-only cap raise DECLARED before any gate leg: uv 268435456, normal None B on that family's MainWorld and COST legs only; the delivered default stays 134217728; declared because no 128 MiB host can be framed (L2 cannot frame MainWorld on this build)
- **P-ORC:** FAIL: the G-MODE rows cannot run (reported, never PASS)

## Values the gate legs read

- uv hosts H1 `SM_Ramp2_UAID_B42E9936F5429ADA00_2086822137`, H2 `RoomBuilderSquare_C_UAID_00155DE1D4771FCF00_1080865874`, cap 268435456; normal hosts H1 `None`, H2 `None`, cap None
- ND-4 yield at 128 MiB: `[{"head": "scope=view candidates=5 cap_bytes=134217728 uv_modes=tile+scramble normal_modes=invert+green_flip", "ids": {"uv_corruption": {"eligible": 0, "refused": 5, "reasons": {"over_budget": 2, "partial_footprint": 1, "texture_not_parameter": 2}}, "normal_corruption": {"eligible": 0, "refused": 5, "reasons": {"partial_footprint": 3, "texture_not_parameter": 2}}}, "end": "end stats_unchanged=1"}, {"head": "scope=all candidates=343 cap_bytes=134217728 uv_modes=tile+scramble normal_modes=invert+green_flip", "ids": {"uv_corruption": {"eligible": 26, "refused": 317, "reasons": {"excluded_group": 1, "host_mid": 1, "no_eligible_slot": 2, "over_budget": 72, "partial_footprint": 86, "texture_not_pa`
- E1: E1 = the frozen 089-03 evaluator (089-03-eval.py sha256 8e48e883..., G395 / G405 / G406 fixes on), pinned by hash with its dependencies

