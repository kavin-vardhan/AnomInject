# 081-19 predictions — m55 change oracle on the banked m55 captures

Written 2026-09-24 before the oracle was run on any banked session. The oracle itself is frozen at
the `feat(verify)` commit `ea9946a` (`tools/verify_capture.py`), which precedes this file's commit; it is not changed after the banked run
starts. If the oracle and a sidecar disagree on any banked row, the oracle is not changed; the rows
are reported and the round is NEEDS-DECISION.

## What is compared (spec §6.2)

For every `"kind":"pair"` row with `chg_measured: true`: decode `Actual_Frames/frame_%05d.png` for
`session_index` and `prev_session_index`, and `target_mask/frame_%05d.png` for `session_index`.
Target region `mask == mask_value`, control `mask == 0`, `d = max(|dR|,|dG|,|dB|)` in bytes,
counted when `d > tau_px`. Rows naming a reference (`ref_session_index` with `ref_gt8`/`ref_mean`
non-null) are recomputed against that delivered frame over the current target region.

- **Exact:** `chg_n`, `chg_gt8`, `chg_sum`, `chg_hist` (8 bins), `ctl_n`, `ctl_gt8`, `ctl_sum`,
  `ctl_hist`, `ref_gt8`; `tau_px == 8`; `prev_session_index == session_index - 1`;
  `frame_file == Actual_Frames/frame_%05d.png`; both denominators positive.
- **Mean tolerance, declared now:** `|published_mean − sum/n/255| ≤ 0.00005` (plus a 1e-12
  float guard) for `chg_mean`, `ctl_mean`, `ref_mean`. Reason: the producer publishes
  `round(sum/n/255, 4)`, so half a unit in the fourth decimal is the largest honest difference.
  With `*_sum` also compared exactly, a mean mismatch without a sum mismatch would mean a rounding
  or serialisation fault.
- **Unavailable, never guessed:** a missing or unreadable file, a non-PNG colour frame (JPEG), a
  mask that is not 8-bit grayscale, a PNG whose size differs from the receipt rect (resampled),
  `capture_path == "backbuffer"`, a missing receipt rect.
- **Extra reading (beyond §6.2, reported separately, not in the claimed counts):** `empty_region`
  refusals are cross-read against the mask PNG — agrees when the target or control count is 0,
  disagrees when both are positive, unverifiable when no mask PNG exists (none is written for an
  all-zero mask, nor after a failed mask write).

## Scope

All 228 unique session folders under `_bench_sessions_bank` from m55 rounds that carry a
`change_evidence.jsonl` (`inventory.json`, metadata only, no pixel read). Session folders present
under two bank names (alias + `_tryN`) are run once and listed with their aliases. 59 sessions carry
measured rows: **1,234 claimed rows, 1,232 reference claims**. Binary per session: from the round
journals where stated (081-11 D0867240 / A699E9EE, 081-12..14 A699E9EE + Lyra 6ef55ade, 081-15
A699E9EE for A_SOLID and E8E2E43F for B_*, 081-17 8E64FA39 + Lyra 48b9e048); 09-21 Stage 2 legs
(5EADBCBD, 2C21EDE5) and all Stage 1 legs by archive time, labelled *inferred*.

## Predictions

1. **P1 — every claimed row is compared and matches.** Claimed 1,234 · unavailable 0 ·
   mismatched 0 · matched 1,234, on every binary including the inferred ones. Basis: the stage
   measures the exact frozen mask buffer the PNG writer encodes (`AnomalyCaptureSubsystem.cpp`
   frozen-mask hand-off to both consumers) and the canonical colour buffer the PNG encoder reads,
   and PNG is lossless; every claimed row in the inventory has a 1280×720 or 1920×1080 receipt rect.
2. **P2 — every reference claim is compared and matches:** 1,232 claimed, 1,232 matched.
3. **P3 — 169 sessions have no claimed row.** Stage 1 sessions report only "stage-1 identity row"
   not-claimed entries; Stage 2 sessions without measured rows (EMPTY, NOHOLD, the Lyra 081-14 G270
   A2 and similar) report refusal reasons only. Exit 0 on all 228; exit 3 on none.
4. **P4 — empty_region cross-read: disagrees 0.** Agrees or unverifiable only.
5. **Build-2 minimum set** (081-17R Stack 1080p twins ×4, Lyra G270 A2): 20 claimed rows each,
   20/20 matched, 20/20 reference matched.
6. **Runtime:** under 15 minutes for the whole bank on this box (1,234 rows × ≤4 PNG decodes).

## What would falsify, and what happens then

Any mismatched row falsifies P1/P2. Per the brief it is a finding: the oracle is not edited, the
exact rows (session, pair, both values, the images) are reported and the verdict is NEEDS-DECISION.
An unavailable row falsifies P1's "unavailable 0" but is not a mismatch; its reason is reported.
