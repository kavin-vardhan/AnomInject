# 084-10 evidence (release hygiene on fix/m52-label-timing, source 92c3500, docs 02d15a4)

Journal: docs/sessions/2026-09-29-084-10-release-hygiene.md on fix/m52-label-timing. Binaries: _binary_baselines\m52fix-667FD4EF\.

- lever-audit-source.txt - G-LEVER-AUDIT on the committed source: VERDICT PASS (every REG / LOOKUP / CATALOG line).
- lever-audit-selftest.txt - 11 cases: clean copy PASS, 10 planted violations each FAIL on its rule.
- lever-audit-binary-game-667FD4EF.txt / lever-audit-binary-editor.txt - binary cross-check PASS.
- lever-audit-binary-old-8A6074AA.txt - the pinned pre-gate exe FAILs B3 (no echo strings).
- g354-selftest.txt - 8 cases OK.
- g354-benchcook-BF06AE61-delivery-mode.txt - STOP EXTRA AnomalyBench, FIXTURE (a bench cook, as expected).
- g354-benchcook-BF06AE61-bench-mode.txt - PASS with --bench-cook.
- g354-builds-windows-2026-07-15.txt - STOP MISSING AnomalyShaders, fixture hits none.
- stage-plugin-delivery-at-37bb750.txt - pre-gate source: STAGE VERDICT FAIL (lever audit R1-R6).
- stage-plugin-delivery-at-92c3500.txt - STAGE VERDICT PASS - deliverable.
- bundle-selftest.txt - 7 cases OK.
- bundle-real-1-as-bundled.txt - real make_delivery.py bundle, current README: PASS (token = the dev public/config.json, G376).
- bundle-real-2-readme-at-37bb750.txt - same bundle with the 37bb750 README: STOP README tools/verify_capture.py.
- bundle-real-3-M2Smoke.txt - the July M2-era folder: STOP (README and two host-tools files missing).
- string-scan-8A6074AA-vs-667FD4EF.txt - every string in both Game exes: only gate/bench strings move.