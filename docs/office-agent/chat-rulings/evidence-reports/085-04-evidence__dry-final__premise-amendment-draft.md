# m53 DC B-M53 premise lock (085-04 night) - AMENDMENT to `2026-09-29-m53-dc.md`, committed BEFORE any gate leg

Premise lock `premise-lock.json` sha256[:16] **fc1867acc42604c7** (run root `E:\IA_BuildCache\_r854_dry_0853_final`; stub=True).

## Dispositions

- **ND-4:** no cap raise: every family with a host has one eligible at the delivered 128 MiB. 
- **hosts:** H1 uv StaticMeshActor_UAID_A036BC6AB247EBF902_2044254803 / normal StaticMeshActor_UAID_A036BC6AB247EBF902_2044254803 (one host for both families); H2 uv StaticMeshActor_UAID_A036BC6AB247EBF902_2048592804 / normal StaticMeshActor_UAID_A036BC6AB247EBF902_2048592804
- **P-ORC:** FAIL: the mode-oracle rows cannot run (reported, never PASS)
- **P-SYN:** synth pose [0, 0, 0] == native (0,0,0)
- **P-FLOOR:** first decision 12/30 final=APPLY: G-AON-MW exercised

## Values the gate legs read

- uv hosts H1 `StaticMeshActor_UAID_A036BC6AB247EBF902_2044254803`, H2 `StaticMeshActor_UAID_A036BC6AB247EBF902_2048592804`; normal hosts H1 `StaticMeshActor_UAID_A036BC6AB247EBF902_2044254803`, H2 `StaticMeshActor_UAID_A036BC6AB247EBF902_2048592804`; cap declared False (None B)
- EXCL-A DEFER 400, pre0 None; EXCL-B DEFER 150 / 190
- ND-4 yield at 128 MiB: `[{"head": "scope=view candidates=4 cap_bytes=134217728 uv_modes=tile+scramble normal_modes=invert+green_flip", "ids": {"uv_corruption": {"eligible": 2, "refused": 2, "reasons": {"over_budget": 2}}, "normal_corruption": {"eligible": 2, "refused": 2, "reasons": {"over_budget": 2}}}, "end": "end stats_unchanged=1"}, {"head": "scope=all candidates=9 cap_bytes=134217728 uv_modes=tile+scramble normal_modes=invert+green_flip", "ids": {"uv_corruption": {"eligible": 2, "refused": 7, "reasons": {"over_budget": 7}}, "normal_corruption": {"eligible": 2, "refused": 7, "reasons": {"over_budget": 7}}}, "end"`

