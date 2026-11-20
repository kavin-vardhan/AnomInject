# 094-46s read-only diagnostic lead: screen-space rendering remains a candidate

2026-11-05. Codex. Information for a later bounded camera investigation; not a ruling, root-cause finding or physical-run authorization.

The existing 1279x719 positive/control comparison fails on 131 texture frames, concentrated at the floor's far edge (rows373-375). Root reproduced raw pairs767/769 and their outside-mask pixel at(375,239). The existing crop767 shows an edge strip; no resizing/cropping was used for the numerical checks.

Both runtime logs show r.SSR.Quality=3 during initialization and r.PostProcessAAQuality=0 at the run command. They also show allocated SSRTemporalAA resources at exit. These establish that disabling ordinary AA does not prove all temporal screen-space processing was disabled; they do not prove which pass caused the strip or its exact settings at every captured frame.

Installed UE4.25 engine evidence (relative to E:/EpicGames/UE_4.25/Engine):
- Source/Runtime/Renderer/Private/ScreenSpaceRayTracing.cpp:99-129 admits SSR based on its show flag, view state, SSR quality, intensity and shading path. At114, quality<=0 disables it.
- The same file:142-150 requests SSR temporal processing when the AA method is not temporal AA OR the SSR temporal cvar is nonzero. Therefore r.SSR.Temporal=0 alone is not a reliable way to suppress that path while ordinary AA is off.
- Source/Runtime/Renderer/Private/IndirectLightRendering.cpp:711-724 selects SSR and its temporal filter, also subject to denoiser settings.
- Source/Runtime/Renderer/Private/CompositionLighting/PostProcessAmbientOcclusion.cpp:66-75 documents r.AmbientOcclusionLevels=0 as disabling AO. The old logs initialize this to-1; later per-frame effective AO was not independently established here.

Runtime logs: D:/IA_Europa/Receipts/09446s/H4/D3D12-1279-{control-a,positive}/01/session/Runtime.log. Relevant lines:303/410 SSR quality;633 AA disabled; positive4499/4538 SSRTemporalAA allocations. Existing images: D:/IA_Europa/Temp/CodeReview09446s/crop-767.png and crop-767-ctl-full.png.

Candidate next investigation, not acceptance: distinguish a near-plane-dependent screen-space effect from geometry/label error. A matched pair with one explicitly recorded effect disabled could discriminate; it must use identical settings on both sides and fresh sealed predictions. Do not compare a changed-settings positive against an old control. Do not infer a cause from the correlation or substitute a post-failure floor. First establish capacity and serialize after09447; all current D4/G3 failures remain.

Chat review NOT REQUIRED for this read-only lead. Any later physical brief must be separately bounded under OD6; changing qualification semantics or acceptance thresholds still requires owner/Chat disposition.

## Read-only addendum, 2026-11-05 about 15:19 IST

The saved per-frame camera metadata confirms the positive/control near values are 100/10 throughout the 1279x719 pair. The 2-level bound is an image-channel delta bound, not a near-plane setting. All existing failure classifications stand.

Additional engine evidence: ScreenSpaceRayTracing.cpp:170-196 computes SSR parameters; when ordinary temporal AA is off, lines185-188 still derive FrameRandom from ViewState->GetFrameIndex(8). IndirectLightRendering.cpp:752-766 sends SSR through its own temporal pass and SSRHistory. These support checking SSR independently of ordinary AA. They do not identify the actual cause of the failed pixels, establish the per-frame view-state phase from GFrameCounter, or prove that disabling SSR will fix them.

An informational aggregation of the already-computed 131 failing frames by session_index modulo8 is {0:20,1:20,2:20,3:20,4:10,5:10,6:10,7:21}; this is not a discriminating phase test and is not used to assert temporal causation. The investigation must still distinguish projection/depth or texture-edge effects from temporal history through a separately authorized matched diagnostic. No new floor, exclusion, threshold, shader, capture or acceptance change.