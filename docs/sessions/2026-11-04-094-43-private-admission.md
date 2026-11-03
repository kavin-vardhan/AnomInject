# 094-43 / 094-43r: private scene-depth admission

The first entry attempt stopped at the stale P9 ref equality assertion. It made no product changes. GE1 authorizes a narrow ref policy without changing the P9 baseline.

GE1 implementation: `owner_ref_policy.py` validates all baseline refs first, then four exact names. Fixed SHA values protect the redaction backup and 42c spike; exported git ancestry queries admit origin at/before local feat and the 43a spike at/after c1193bd. `owner_p9.py` writes the admission receipt and continues its unchanged remaining checks. Eight tests pass, including current real state and all four requested negatives; missing-baseline and invalid-ancestry cases also fail as intended. Evidence: D:/IA_Europa/Receipts/09443r/ge1-tests.json.

Full P9 is pending this entry commit. Stage A must then prove GD3 using the saved frame scene depth, without changing tolerance after measurement. Stage B remains blocked until all Stage A gates pass. No build or runtime launch has occurred in this resume.
