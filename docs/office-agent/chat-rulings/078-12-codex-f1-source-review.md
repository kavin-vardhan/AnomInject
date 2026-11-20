# 078-12 - independent source review of the frozen F1 change

2026-09-06. Scope: full production diff of6713f510166a0d9458387caaae711779b07fad64, plus current snapshot construction, drain/retry, observability, run-boundary reset and label-writer call sites. This is an independent source review, not a runtime PASS or a complete plugin audit. No source was changed.

## F1 findings

No new blocking defect established in the narrow F1 diff from this read. `CaptureCurrentFrame` freezes `bAwaitingTargetMask` from the same `bTargetMaskEffective` that gates the target-mask arm and relative mask filename (AnomalyCaptureSubsystem.cpp4219-4231). The requested flag remains separate for existing serialization semantics. `FCaptureSnapshot` initializes the waiting counter to zero. The ineffective requested-mask drain initializes per-fire pixel/drawn arrays to-1, drawn bounds empty and state unmeasured; downstream observability takes the unmeasured early-out. The label writer still consumes the original requested flag and fields; the new internal waiting flag is not a schema change. StartRun resets the one-shot warning state and outcome/held-frame state.

The effective predicate remains requested && measure && async && EffectiveOutputHeight==0. Do not add bSveCapture based on the color path: the target-mask extension is independent, while backbuffer×target-mask runtime coverage remains OPEN. The historical commit message's assertion that that path works is stronger than the available runtime evidence.

The new counter is per snapshot; its unit is **drain visits while at the waiting head**, not independently measured elapsed game frames. ProcessCompletedFrames moves the remaining completed batch into TargetMaskHeldFrames when the head waits; finalization calls ProcessCompletedFrames repeatedly in an eight-iteration loop with rendering flushes (cpp4066-4078). Therefore this source does not establish a four-game-frame end-to-end age bound or eliminate head-of-line delay. That is a limit on the timeout wording, not a demonstrated new F1 regression. Do not carry a wall-time guarantee into later lifecycle predictions without measurement.

## Existing release blockers remain

The writer path `AnomalyLabelWriter.cpp::EncodeAndWriteFrame` checks the RGB SaveArrayToFile result, but ignores the result of `SaveStringToFile` when appending labels.jsonl and then returns true (around535-540). This corroborates the existing F7/F8 receipt/completeness work: successful RGB write cannot establish committed image+label integrity. No tiny patch is released here, because failure receipts, orphan policy, publication and the auditor must agree under the reviewed writer design and can-fail tests.

The existing render/request-identity and mask payload-loss findings are not fixed by F1. This read does not certify geometry freezing, stale cancellation, generation handling, tag leases, bounded writer memory or either fixture's full regression set. Frozen pairing smoke is only the current bounded step. Every code tranche still needs direct source review plus independently checked behavior and failure evidence before release.

## Independent bank reconciliation while07813 preflights

Read the actual labels.jsonl, annotation.json, run_summary.json and RGB/mask filename sets in all four native mask-off banks, without launching a capture or modifying any artifact:

| bank | session | RGB/rows | missing of90 | duplicate row indices / row-image set differences | events |
|---|---|---:|---:|---|---:|
| M51_F1A_MOFF_NAT | 20260906-170307 | 33/33 |57|0/0|3|
| M51_F1A_MOFF_SYN | 20260906-170352 | 33/33 |57|0/0|3|
| M51_F1B_MOFF_NAT | 20260906-172240 | 90/90 |0|0/0|8|
| M51_F1B_MOFF_SYN | 20260906-172258 | 90/90 |0|0/0|8|

Every row is mask_state unmeasured, every mask_file null, zero mask PNGs, label_schema2, and unavailable count equals written count. All22 anomaly rows per A bank and all59 per B bank have target_pixels=-1, target_drawn_pixels=-1, observable=null and bbox_drawn_px=null. This confirms the named artifact outcomes independently of the report. It is not a new physical attempt or a pixel-visibility check. Candidate indices cover0..89 exactly; originalA failures remain failures.

Also traced PixelOwner in AnomalyMaskSceneViewExtension.cpp511-528 through ServiceTargetMask.cpp1191-1204: only the first pixel requester receives TightPixels; a later pixel requester can reach the unavailable branch with an empty buffer. This source reading corroborates the existing request-ID-joined finding; it does not identify which rendered frame any request should own. FAnomalySveCapturer::ArmWanted stores just the ID and ConsumeWantedForPublish pops FIFO, with FamilyFrameNumber used for logging. No new frame-identity guarantee is inferred from those logs.
