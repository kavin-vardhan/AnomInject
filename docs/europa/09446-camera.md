094-46 as built

The 4.25 summary now includes label_transition_camera_clipping_unconfirmed_entries. It sums existing transition reason-entry counts, matching release d7eee97 AnomalyLabelWriter.cpp:1350 and CountEntryReasons in AnomalyCaptureSubsystem.cpp. It is present as zero when no such entry exists.

The additive compatibility keys session_global_baseline_near, session_global_restored_near and session_global_near_restored expose the held-session baseline and Stop restoration. They do not replace any 5.1 key.

The new IAICameraRegression bench switch additionally requires IAIBench, IAICameraSlabBench, EA_Core, a requested camera_clipping route and a non-Shipping build. It keeps the original 2352 frames and appends six48-frame alternating near/far holds. The home helper hands the held camera to the capture schedule only after pre-fire approval. The normal camera-only schedule and default-only capture length are unchanged. No custom-depth or host asset/config write is added.

GN2 measures RGB maximum absolute channel difference for every pixel. Two baseline-near sessions provide per-frame256-bin histograms and counts above every level. Their global maximum delta and largest count at that level are sealed before the positive comparison. Both bounds must hold on every unlabelled frame. Positive frames must have more than1% of the frame above that floor and exceed its maximum-level noise count.
