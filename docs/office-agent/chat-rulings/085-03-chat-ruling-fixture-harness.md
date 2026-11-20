# 085-03 — Chat ruling on the m53 fixture, cook and harness (NEEDS-DECISION): accepted; per-family MainWorld hosts; a normal-family MainWorld shortfall is a reported gap, not a stop; the m53 night runs TONIGHT after 084-09

Date: 2026-09-29 ~13:10 IST · Author: chat Claude · Branch `feat/m53-uv-normal-corruption` `031434b` · Pairing exe `6B671987` + utoc `F76F1119` (archived, not staged)

## Accepted
- **The fixture cells:** `TC_OracleUV16` / `UV64` / `N`, `TC_Mixed`, and `TC_M52Hold` / `Share`. Their 44 offline image checks pass, including wrong-mode separation ≥ 25 %, and scramble reproduces the C++ vectors.
- **Authoring conduct:** four failed runs each rolled back to the starting manifest by a re-hashing script. The final diff is exactly the 13 declared files plus the level.
- **The cook,** with the descriptor STOP / `--bench-cook` PASS behaviour (correct: a bench cook never ships). The lever audit passes on the binary.
- **The harness `_reviews\085-04-*`:**
  - 86 legs (7 premise + 79 gate), with predictions per leg;
  - gate selftest 54/54, mode oracle 39/39, ported checks 98/98;
  - identity-null legs must read `lever=identity`; the cold-census check; the glue check; G377; F5;
  - self-staging with receipts, and a restore from its own archive;
  - **dry run 86/86**, about 145–170 min.
- **The ND-5 definition** of "no visible change".
- **G392** (E1 can't judge the +3 sessions; the known answer rests on the 086-01 checker): accepted, and stated in the evidence.

## Ruling on ND-4 (host shape)
- **Accept per-family MainWorld hosts,** preferring an object eligible for both. The rock can never qualify (`texture_not_parameter`, G394).
- **A family with no MainWorld host at any cap:** its MainWorld legs are **not-run and reported as a gap, not a stop.**
  - m53's timing mechanism is a material swap in the fire tick, the same class as `corrupted_texture`, which is SYNC-PROVEN on MainWorld content. Timing is proven on the fixture; eligibility is a separate matter.
- **This is a product finding the owner hears today, not only on Thursday:** under the delivered rules (all-or-nothing, 128 MiB, parameter textures only), **StackOBot's MainWorld may yield ~0 eligible objects for UV and/or normal corruption.**
  - Thursday's office census shows the real yield on the client-class content.
  - The owner's default-on decision, and any "ship available but off" call, rests on that number.

## Next
1. **087-01 (the office kit + the AnomDash bundler fix), released now.**
2. **Tonight (the night brief, released when the PC is idle, ~22:30 IST):**
   - **084-09**, the sync re-bench (~87–100 min);
   - **then 085-04**, the m53 night (~145–170 min);
   - one Code session (high), each harness restoring its own staging.
3. **By day, after 087-01:** a delivery rehearsal (packaging checks on both the m52-only fallback and the full m53 path, nothing delivered).
