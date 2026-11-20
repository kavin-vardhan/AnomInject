# 081-21 — chat ruling: build 3 accepted (offline); runtime legs + long-run readings run as soon as the PC is quiet (chat Claude, orchestrator, 2026-09-24 04:35 IST)

## Disposition — GREEN accepted, runtime still owed
- **Build 3** = source `85c99b3` (+ oracle `c27024d`, docs `94a56a9`; predictions `4e54d57` pushed first), Game **002805CF**, editor `m55-build3-main-editor-c27024d`, Lyra `m55-build3-lyra-cf0b38cb`; one build set, zero warnings; postflight PASS.
- F1 (no persist/no directory for a run that never started a session), F2/F3 (legacy colour and masks exactly pre-m55 whenever m55 cannot attach; `unsupported_delivery`; `change_multi_view_families`), F6, F7 (`change_unsupported_completion`), F9 (`BENCH-GATE-REFUSED`), oracle exit codes 0/1/3, and the F4/F5/F10–F13 docs — all accepted as specified. Deviations 1–6 accepted (the extra coverage in 2 and 3 is correct; gate 8 losing SI 8's legacy picture is a fixture-only fault, predicted before the build).
- Offline proof accepted: suites unchanged; oracle 24 cases (sabotage breaks 6); 228 banked sessions 1,234/1,234 still match; replay of 365 sidecars 0 violations; F2/F3 model equals pre-m55 on 24 scenarios + 4,000 fuzz runs (build 2 differed in 3,546); F1/F7 checks pass.
- **Nothing about build 3 has run in the engine yet** — the four legs (a)–(d) are owed.
- **Code's point on the re-exposed pre-m55 key-overwrite** (a later non-capture view in the same frame overwrites the frame's key; bench-unreachable; PIE with a non-realtime editor viewport) — accepted as ruled: it is pre-m55 behaviour, recorded beside the multi-view unsoundness for the m51 pairing work.
- **Session length:** 081-21 ended at 75 % of the 1M context. Every brief from now on starts a fresh session, and long campaigns are split.

## Next — 081-22 (Code, xhigh): bench work that waits for a quiet PC
The PC has had another project's Unreal editors open all night. 081-22 waits (checking every ~10 min, up to 4 h) for a quiet PC, then runs: build-3 legs (a)–(d) first, then the 081-18 long-run ON/OFF readings on build 3. If a foreign editor appears mid-leg, that leg is invalid and the brief waits again. If the PC never goes quiet, it stops and reports. The owner's scheduling decision still stands open; this just uses any quiet window tonight.
