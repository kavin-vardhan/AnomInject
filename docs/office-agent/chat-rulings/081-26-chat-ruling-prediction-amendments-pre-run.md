# 081-26 — chat ruling: G8 correction accepted; PQ-A, PQ-B and the G270 empty_region clause amended BEFORE those legs run (chat Claude, orchestrator, 2026-09-25 12:35 IST)

## Disposition
- G8 clause corrected and proven both ways (38/38; whole bank: 801 stub receipts all complete, 73,456 delivered receipts all fail the clause). `C1_G8_N` **PASS-REEVALUATED** from its banked evidence, original FAIL row kept. Boundary re-issued (lib hash + feature head `148050f` only). Dry run 134/134. Accepted.
- **G290 accepted with thanks:** the 081-23 runners filed a recorded failure under "done", so an unchanged re-run would have silently run past it. Hardening all four runners to re-raise a recorded failure is accepted (deviation 3a), as is the redirecting dry-run wrapper (3b). Correction to the 081-25 journal noted.
- Scorecard: three Code-authored harness defects from the 081-23 prep (G288, G289, G290), all found by Code at or before first use, zero product impact.

## Rulings — pre-run prediction amendments (legitimate: none of these legs has run on build 3)
1. **PQ-A** (`lib:211`, predictions §2.1.4): `change_view_rejected` is **0 except G8, G2 and G15, which are ≥ 1**. Reason: G2's dropped-arm frame still has its legacy mask served with no m55 family data, and G15's bench mask hold counts each held pass (MSVE:184, MSVE:160); Stage 1 already recorded 1 on both gates. Honest counting, not a defect.
2. **PQ-B** (`lib:52-62`, §2.1.3): **one `HIGH-WATER-PEAK` line per closure** (count = 1 + `change_epoch_resets`), the last equal to the run summary's peak census, every census closed. Reason: G7 and G15 reset the epoch and close twice; the 081-17 field reference already documents one line per closure.
3. **G270 (`lyra_g270_eval`, 081-14 §3.4):** a required pair refused as **`empty_region`** (the target's own mask has no pixels in that frame — honest occlusion/off-screen) is **not** a failure; PASS = ≥ 3 events and every required pair either measured or honestly refused as `empty_region`, with **none** lost to `closure_timeout` or `budget_exceeded` (§3.4's stop branch is unchanged). Chat's 081-14 wording ("every labelled phase's required pairs measured") was too strict for a gameplay host; correcting it now, before the leg.
Each amendment: dated in the predictions, proven both ways (a stand-in that must pass and a mutation that must fail), boundary re-issued for exactly the changed hashes.

## Next
081-27 (bench-free) applies these three. Windows c1 → c3 resume in the brief after, **only when the owner says the PC is free** — the quiet gate cannot see a Blender render.
