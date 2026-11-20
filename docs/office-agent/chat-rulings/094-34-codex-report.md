C4 passes on stock 4.25. Both seeded auto-injection sessions produced 1,440 frames with matching labels and masks, zero drops, and normal exits. FR2, the static check, both mask negatives and the same-seed check pass.

FR2 re-score: all 42 kept lighting frames have completeness **1.000**. The best single receiver scores **0.440136**; omitting the smallest scores **0.840638**. Both fail the frozen 0.95 threshold. [Receipt](D:/IA_Europa/Receipts/09434/fr2-rescore.json).

The change binds actual `GetLiveFires()` targets, uses the shared stencil ledger and measured stencil/depth census, derives dimensions from the view, and bounds pending writes to four. The 5.1 reuse citations are [live-target binding](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:1686), [ledger ownership](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyStencilTag.cpp:75), and [census admission](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:4378). The guarded 5.1 paths remain intact.

FR3's reference rule was committed before any session: use the session's last captured frame strictly before each live-fire start, without selecting by labels or picture scores. Static scanning passes **164 Source files**, with four injected violations refused. Keyword-Rotator checks pass. No in-editor Python changed, so the Python 2.7 harness is not applicable. The compiled queue test preserves 32 ordered outputs with peak four; its unbounded negative peaks at 32 and fails. [Tests and detailed receipts](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09434-live-capture-receipts.json).

Three incremental Editor/Game build pairs passed under FM1: BelowNormal, two parallel actions, admission readings 9.51–9.81 GiB free commit. There were **zero compiler errors and zero build memory stops**. The final [27-file manifest](D:/IA_Europa/Receipts/09434/c4-425-binaries-02-C4_900.json) matches disk; it lists sizes and SHA-256 hashes. No archives were created.

Both accepted sessions use D3D11, stock format 18, exposure locked and AA off. Each has 819 clean-labelled frames with **zero mask pixels**. All events below completed apply/revert; C2 also checked 38 interior blink toggles per session.

| Session | Anomaly | Events | C2 mismatches | IoU | Precision range | Spill | Completeness |
|---|---|---:|---:|---:|---:|---:|---:|
| 1280×720 | blinking | 19 | 0 | 1.000 | 0.9388–1.0000 | 0 | 1.000 |
| 1280×720 | lighting_mismatch | 11 | 0 | 1.000 | 1.0000 | 0 | 1.000 |
| 1280×720 | lod_corruption | 16 | 0 | 1.000 | 0.9786–0.9794 | 0 | 1.000 |
| 1280×720 | missing_object | 12 | 0 | 1.000 | 0.9388–0.9789 | 0 | 1.000 |
| 1600×900 | blinking | 19 | 0 | 1.000 | 0.9374–1.0000 | 0 | 1.000 |
| 1600×900 | lighting_mismatch | 11 | 0 | 1.000 | 1.0000 | 0 | 1.000 |
| 1600×900 | lod_corruption | 16 | 0 | 1.000 | 0.9798–0.9803 | 0 | 1.000 |
| 1600×900 | missing_object | 12 | 0 | 1.000 | 0.9370–0.9857 | 0 | 1.000 |

There are **zero ambiguous edges**. One-frame mask shifts fail for all four anomalies, as do nonempty wrong-target substitutions. The complete **58 anomaly/target pairs match** across both resolutions and each session's actual `Auto.Fire` log. [Same-seed receipt](D:/IA_Europa/Receipts/09434/same-seed.json).

Each resolution used its single authorized retry. The first 720p run selected a bounds-visible pawn with zero stencil pixels; census admission fixes that. The first 900p run hit the 4 GiB commit floor with a write backlog and was stopped by owned PID, monitor first. The bounded-writer retry peaked at four pending writes and recorded a minimum of 7.109 GiB free commit. Both failed slots remain frozen. Accepted session durations were 40.00 s and 86.52 s; their readers are [720p](D:/IA_Europa/Receipts/09434/c4-attempts/C4_720/02/reader.json) and [900p](D:/IA_Europa/Receipts/09434/c4-attempts/C4_900/02/reader.json).

Temporary settings were seed 4242, interval 0.8 s, hold 0.4 s, one concurrent fire, fixed 1/30 step, and capture pacing on. Auto settings were saved/restored; receipts confirm stencil and pacing restoration. Exposure/AA overrides were process-local; no Config edit occurred.

**No crash, upload sign or protected change.** Entry and exit fast checks pass; m51, protected refs, Content, Config, descriptors and the project file remain unchanged. Drive-root names are unchanged. [Exit receipt](D:/IA_Europa/Receipts/09434/exit.json).

Committed on `feat/europa-port`: `f16256b`, `93fec56`, `455506e`, and closeout **`47a69722526dc95fdf80da42ef7f5043746b89f9`**. W4 dates are current; the worktree is clean. Nothing pushed; no tags.

VERDICT: GREEN | FR2 and both C4 sessions pass all gates; seeded frames, labels and masks agree at both resolutions.
