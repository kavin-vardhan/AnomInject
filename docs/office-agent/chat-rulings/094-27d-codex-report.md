<!-- collected by Claude Code (mailbox brief 094-27d) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md, 4316 B, sha256 CF45095489DA10B055B612554E4662A81555BA06CFC2DF72633894573F41DF59, at 2026-10-31T07:03:15.9431975+05:30 -->
A7 remains incomplete: lighting_mismatch has 5 counted events and missing_object now has 6. Blinking and LOD still have 0. No new editor or game session launched before the 07:00 IST deadline because physical memory never met the 6 GiB start line.

**Entry.** Read-only m51/protected-ref checks passed; Paris Content, Config, descriptors, project and bound files were recorded. The 4.25 manifest passed 27/27. Free space was C: 26.80, D: 56.55, E: 278.72 GiB. 094-27c showed no protected change or upload. [Entry receipt](D:/IA_Europa/Receipts/09427d/entry.json).

**Diagnoses and fixes.** [Measurements](D:/IA_Europa/Receipts/09427d/diagnosis-measurements.json) and [source citations/design](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09427d-a7-lean.md):

- **LOD:** forced model 2 selected the cube already visible under automatic LOD. The object lies inside the checked region, but no image pixel changed by eight levels. The driver now requests model 1, the sphere. This parameter fix still needs real picture validation.
- **Missing object / blinking control noise:** clean frames differ only by balanced one-level quantization; brightness gain is 1.0, not exposure drift. The corrected clean-frame check accepts that bounded noise while retaining active-change and restoration tests. Missing-object re-reading passes six events. No exposure override was added.
- **Blinking:** the native probe opens its append file exclusively; the monitor hit a sharing violation. Probe reads now retry conflicts, and A7 monitoring avoids live probe reads entirely. Completed-picture signals trigger closure; the probe is read after exit.

**Tests.** Eight changed-code test groups passed, including drift/residue, sharing, gate and old-LOD negatives. The engine Python 2.7.14 harness passed four cases, including multi-tick load and single-shot failure. Keyword Rotators passed; no Python comment lines remain. Because gate/monitor core changed, three stand-in cases and all three armed dry runs passed; Smoke was armed before blinking and LOD. [Qualification](D:/IA_Europa/Receipts/09427d/qualification.json).

**Evidence and counts.** Attempted includes the two refusal checks where present. Kept raw files and old verdicts remain unchanged.

| Anomaly | Attempted | Applied | Reverted | Counted | Reader / picture sheet |
|---|---:|---:|---:|---:|---|
| lighting_mismatch | 8 | 6 | 6 | 5 | [PASS; counted sheet](D:/IA_Europa/Receipts/09427c/runtime-attempts/A7_-lighting_mismatch-positive/01/lighting_mismatch-contact-sheet.png) |
| missing_object | 8 | 6 | 6 | 6 | [PASS; counted sheet](D:/IA_Europa/Receipts/09427d/rereads/missing_object/missing_object-contact-sheet.png) |
| lod_corruption | 8 | 6 | 6 | 0 | [FAIL; diagnostic, NOT COUNTED](D:/IA_Europa/Receipts/09427c/diagnostic-contact-sheets/lod_corruption-diagnostic-contact-sheet.png) |
| blinking | 0 | 0 | 0 | 0 | [FAIL; diagnostic, NOT COUNTED](D:/IA_Europa/Receipts/09427c/diagnostic-contact-sheets/blinking-diagnostic-contact-sheet.png) |

[Owner contact-sheet index](D:/IA_Europa/Receipts/09427d/contact-sheet-index.json) includes per-picture AA=0 evidence for counted events. The blinking diagnostic uses the deferred SkipRestore control and does not prove restoration. Lighting is retained from 094-27c; missing-object and LOD were re-read. No new session was used for either qualified anomaly.

**Run window.** Smoke waited from 02:36:33 to 07:00:00 IST (450 readings). Physical memory ranged 3.15–5.32 GiB; the start line is 6 GiB. Smoke, blinking and LOD never started. [Gate summary](D:/IA_Europa/Receipts/09427d/gate-summary.json).

**Safety and exit.** New crashes, upload signs and protected changes: none. No real session logs were created. Exit hashes, m51/protected refs and the 27 bound files passed; drive-root changes are recorded in [the exit receipt](D:/IA_Europa/Receipts/09427d/exit.json). No machine settings changed. FJ1’s deferred scans and audits were not run.

**Commits.** `9360c18043ee2310af3fcc2aad065454ef63cda8` (fixes) and `cda5cdd92e0788364765a1c4494e7fd2965f4057` (closeout), on `feat/europa-port`, with actual W4 dates. No push or tags.

VERDICT: NEEDS-DECISION | lighting 5 and missing_object 6 qualified; blinking and LOD remain unproven after the launch window
