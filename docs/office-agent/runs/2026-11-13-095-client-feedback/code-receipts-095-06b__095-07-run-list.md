# What 095-07 must run (from 095-06b)

Precondition (not done in 095-06b): E: at or above its floor (or chat waives it for an Editor-only compile), then the
Editor compile of `integ/m56-m58`, `feat/m58-terrain-blink-subtype` and `fix/m56-capture-continuity` (0/0 each) and
the pushes of the three plugin branches.

## Exes (one cook, code-only swaps, G103)
1. **Release** exe from the archive (the 2026-10-05 release, `release/2026-10-05` = `d7eee97` source): can-fail side and
   comparison side.
2. **m58-only** exe from `feat/m58-terrain-blink-subtype` (`5e1bc03`).
3. **Integration** exe from `integ/m56-m58` (tip after 095-06b): the build that ships.

## Cook
`CB_GateLevel`, `MainMenu`, `MainWorld`, `Entry` (read the map set back with `verify_cooked_maps.ps1`), plus the m58
landscape fixture level if C-G2 needs one cooked in. One cook serves all three exes.

## Legs per gate
- m56 gates G1-G5 and the settle-exactness table: integration exe; release exe for the can-fails (G4a continuity FAIL,
  G4d mask-wait) and as G5's comparison side with a same-binary release control pair.
- m58 C-G2, C-G3, C-G6: integration exe.
- m58 C-G4, C-G5 ("nothing else changed"): m58-only exe against the release exe.
- Readers on every m56 leg: `verify_capture.py --label-rule`, `m56_continuity_check.py` (not C7 on a targeted held
  `camera_clipping` leg), `m56_nonsettle_compare.py` (G5, `labels.jsonl` only), `m56_settle_content_check.py` (P23).
- `run_leg.ps1` now refuses to start while any other StackOBot process runs; free the box first.

## Frozen predictions
`code-receipts-095-05\predictions-095-05.txt` with its dated AMENDMENT 1 (095-06b; SHA-256 after amendment
`47399F2BEF82788A2AC7AEB4C7BD3D8EFE855FE7DCD43CD1C2F03089E9B9A237`), and CaptureBench `tools/m56_gate_predictions.md`
amendment 1 (branch `m56-gates` `1978cd8`).
