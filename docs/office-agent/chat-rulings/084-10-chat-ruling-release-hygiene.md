# 084-10 — Chat ruling on release hygiene (GREEN): accepted; AnomDash's bundler gets fixed; the token file is the `.env` item

Date: 2026-09-29 ~08:45 IST · Author: chat Claude (owner offline) · Branch `fix/m52-label-timing` `02d15a4` (source `92c3500`) · Exe `667FD4EF` (archived, not cooked or staged)

## Accepted
- **`AnomalyBenchGate::IsEnabled()`:** twins and all 33 levers are gated, and compiled out of Shipping.
- **The masked-flag route (G374),** with the mask-aware lookup in the control server.
- **`lever_audit.py`:** its first run failed on 26 Shipping strings a grep would have missed, and they were fixed (G375). Selftest 11, with 10 planted violations failing. Binary PASS on `667FD4EF`, FAIL on `8A6074AA`.
- **The packaging tools,** each proven both ways on real packages: `check_delivery_bundle.py`, `stage_plugin_delivery.py`, `check_package_descriptor.py`, and the readme path.
- **No uncommitted AnomDash edits.**
- **Unit suites unchanged; the string scan is clean.**
- The runtime observation of the gate is owed to the G-LEVER night legs, as stated.

## Decisions
1. **Yes, fix AnomDash's bundler (G376).** It shipped the dev token: the M2 incident's shape. The delivery bundle must never carry the dev `config.json`; it writes the delivered token and tells the truth. This is folded into 087-01.
2. **Confirmed:** the packaging checklist's `.env` item is the dashboard token file `dashboard\config.json`.
3. **G377 noted.**
   - Bench legs pass `-IAIBench` and check the `ENABLED` line; this is added to 085-03.
   - `lever_audit.py` and its selftest re-run on the merged tree in 085-02.

## Next
- **085-02** (merge + m53 DC source + the Codex source review), released now.
