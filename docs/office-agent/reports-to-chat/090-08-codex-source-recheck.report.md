# REPORT for 090-08-codex-source-recheck.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 090-08-codex-source-recheck.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-10-01 06:38:22 | finished: 2026-10-01 07:18:59 | duration: 41 min | exit code: 0
context at last turn: 349707 tokens (35% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\090-08-codex-source-recheck.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION. Codex returned **CHANGES REQUIRED**, so this is not GREEN. Nothing was launched, built, edited or deleted, and none of its findings were acted on.

**Plain summary for the owner:** Codex agrees the specific early-revert bug from its last review is fixed, and that the Shipping exclusion and the constructor change are fine. It found new holes in the "label ends when the effect ends" fix, the Nanite skip and the office kit's interrupted rule, so chat needs to rule before Friday's merges.

**Codex verdict: CHANGES REQUIRED.** It ran on gpt-6-astra at max effort, read-only, took 1,702 s and exited 0. The review is collected at `_reviews\090-08-codex-source-recheck.md`: one header line, then the body byte-identical to the original (checked by SHA-256). The run folder is `_relay\runs\2026-10-01-090-08-source-recheck\` and the ledger row is added.

Answers to the six questions:
1. **F1 is not closed.** The original raw-revert case is repaired, and m53 co-entry is closed during normal capture.
2. **The Nanite skip leaks**, and with no probe registered it fails open. The random-draw contract holds for identical classified inputs, but "setting 1 is byte-identical" is too broad.
3. **Shipping exclusion and constructor: accept.** Development, Test and Editor build graphs are unchanged, and the Shipping graph has no Anomaly modules left. The constructor body matches the engine macro exactly.
4. **Kit 1.1 is not sound as an unconditional release gate:** a mistimed label can pass (R7).
5. **The readme is partly corrected.** F2–F5, LOW and the F3 counts now hold, but stale text and overclaims remain (R9).
6. **Other routes:** one note about manual re-apply (last row of the table below).

| # | Severity | Finding (`F` = the m53 tree, `X` = the fix tree) | Evidence |
|---|---|---|---|
| R1 | HIGH | "Every slot still holds our material" is not the same as "the effect is on screen." If one slot is replaced, the whole event becomes `effect_interrupted` (treated as a clean negative) while the other slots still show the effect. This affects all four types. | F `Anomaly_MissingTexture.cpp:267–280`, `Anomaly_CorruptedTexture.cpp:267–280`, `Anomaly_TexCorrupt.cpp:1414–1446`, `AnomalyLabelSync.h:64–90` |
| R2 | HIGH | Revert can leave a checker or pink override alive at a slot index above the current mesh's material count. It reappears unlabelled after a later mesh change. m53's own material instances are not affected. | F `Anomaly_MissingTexture.cpp:184–190, 231–252` (same in CorruptedTexture); engine `StaticMeshComponent.cpp:2164–2224` |
| R3 | HIGH | The installed tests check the slots recorded at apply time, not what renders now. A hidden or replaced component, or a slot or mesh change, gives a false installed or not-installed reading. | F `Anomaly_MissingTexture.cpp:255–280`, `Anomaly_TexCorrupt.cpp:1423–1438`, `AnomalyLabelWriter.cpp:698–712` |
| R4 | HIGH | Label inputs are not tied to the captured picture. The async path recomputes on-screen membership from the live actor when the readback completes. The sync path (`IAI.Capture.Async 0` and the fallback) pairs the new game state with the previous picture. | F `AnomalyCaptureSubsystem.cpp:5880–5927, 1009–1094, 5343–5367`; `AnomalyLabelWriter.cpp:182, 698–727` |
| R5 | HIGH | The Nanite test is filtered by the target exclusion patterns, so an excluded Nanite part on an eligible actor slips through. Nanite is also not re-checked during an event, so a component added later is missed. | F `AnomalyViewport.cpp:98–150, 791–825, 1416–1439`; capture frame gate `:6022–6053` |
| R7 | HIGH | Kit 1.1 can silently skip judging a re-installed run and still say PASS. Codex reproduced this offline: one missing reference image, second run one frame early, result OK/PASS. | F `tools/label_sync_check.py:411–429, 1131–1152, 1246–1256, 1278–1317` |
| R6 | MEDIUM | With `AllowNanite` 0 and no Nanite probe registered, the injector fails **open**: it admits Nanite targets. | F `AnomalyViewport.cpp:77, 1401–1413, 1430–1454`; `AnomalyCaptureModule.cpp:11–24` |
| R8 | MEDIUM | A frame flagged `effect_interrupted` that also carries an AA reason still gets the AA excuse. Reproduced offline; no normal capture is known to produce this combination. | F `tools/label_sync_check.py:896–918` |
| R9 | MEDIUM | The readme still has stale rules and overclaims: the texture row; m53 missing from the interruption table; `effect_interrupted` described as a clean picture; an early-revert capture proof that was never run; "every labelled event has a real mask"; "contains none of the plugin" for Shipping. | F `docs/client-readme.md:1045, 906–907, 1303–1306, 946, 1167–1168, 1152–1165, 53, 1087–1088` |
| R11 | MEDIUM | A spline mesh built on a Nanite-enabled asset is wrongly classed as Nanite and dropped from the pool. | F `AnomalyMeasurability.cpp:25–43`; engine `SplineMeshComponent.cpp:190–214` |
| R10 | LOW | The F1 timing log measures time per call, not the per-captured-frame budget the ruling set. | F `AnomalyCaptureSubsystem.cpp:6044–6051, 8955–8965` |
| NOTE | — | A manual raw re-apply of the same anomaly to another actor can make the earlier event read as installed. This route is already documented as unsupported. | F `AnomalyInjectorSubsystem.cpp:1119–1126` |

Codex also notes that its "no failures" result is not proof of correct timing for one-frame gaps, which come out as censored rather than failed. "Identical reports" on uninterrupted sessions holds only after normalisation, though it does hold for the numbers by construction. The 090-06 smoke is still unrun.

**Disk:** D: 127.38 GB free of 625.75 GB; E: 74.87 GB free of 931.50 GB. Nothing deleted. The first launch attempt never reached Codex because the overnight UE render had nearly exhausted memory. I waited until memory freed up at 06:49, so the render was not disturbed.

**NEEDS-DECISION:**
1. **Whether Friday's merges go ahead.** That means ruling on R1–R5 and R7, the four HIGH label/picture findings and the kit false pass.
2. **R6:** should `AllowNanite` 0 fail closed when the Nanite probe is not registered?
3. **R9 and R11:** take the readme wording fixes and the spline classification fix in this round or later?
4. **The NOTE:** enforce or document the manual re-apply restriction?
api-equivalent cost (not billed on Max): $4.86 | turns: 33 | running total across all briefs: $4,062.18
