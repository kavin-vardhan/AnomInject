# Claude Code 084-01 findings, written BEFORE reading Codex's review (timestamp = file mtime)

ONSET (label late):
- Label = IsAnomalyCurrentlyAnomalous = any held tex UTexture2D::GetNumResidentMips() (== CachedSRRState.NumResidentLODs, GT copy)
  < baseline, sampled at OnWorldTickEnd of the captured frame (acs.cpp:4846-4876, 4955-4971; slm.cpp:892-907; Texture2D.cpp:481-483).
- GT NumResidentLODs only updates in UStreamableRenderAsset::TickStreaming when PendingUpdate->IsCompleted (StreamableRenderAsset.cpp:174-179),
  i.e. strictly AFTER the render-thread FinalizeStreaming swap (StreamableTextureResource.cpp:214-237) has run.
- Frame order: world tick (label sample) -> RedrawViewports -> IStreamingManager::Tick (GameEngine.cpp:1775/1891/1901). TickStreaming is staged
  (FramesForFullUpdate=5 slices + stage0 + final stage in-flight loop; StreamingManagerTexture.cpp:1647-1762, 1174-1297, StreamingTexture.cpp:262-278).
  D3D12 async reallocate completes on RHI thread (D3D12Texture.cpp:1261/272) so Finalize needs a later GT tick to schedule.
- => label lag >= 1 frame by construction, magnitude = streamer phase/slice dependent. Bank: all held textures flip on GT on the SAME frame (final stage),
  while pictures step at different earlier frames. StackOBot rock: main step at lab0-1 on every event, pre-steps at lab0-3/-4.

OFFSET (label ends early):
- BeginRevert -> RevertAllLiveFires -> Revert sets bActive=false (slm.cpp:764); FinalizeArmedLabel runs after BeginRevert in the same Tick
  (acs.cpp:759,806, 4666-4668) so the revert-tick frame itself is labelled clean; K settle frames uncaptured; PostGap captured and clean-labelled.
- Picture: stream-in latency (StackOBot: first recovered R+6..R+9, RESTORE VERIFIED R+7 on all), then MipBiasFade 0.3 s per mip (RenderResource.cpp:1054,1118):
  half-recovery R+10..R+19, full R+10..R+30+. => 4-5 captured fully-blurred clean-labelled frames on EVERY complete StackOBot rock event.
- Separator for "correct" office offsets: NOT established. Bench has zero correct-offset complete events. Lyra restore latency varies (verify +1..+166 frames,
  up to 15 re-issued stream-ins); L2 event with verify+1 has a short tail (3). Leading candidate: restore latency (streamer phase, IO, cancellation) and fade length (delta mips) vs K.

MASKS: same predicate (ArmTargetMaskOwn uses IsFireLabelledThisFrame, acs.cpp:1187) and iterate LiveFires only => move together; a post-revert tail
cannot be labelled by any predicate change because the fire leaves LiveFires at revert.

FIX: RT-truth per captured frame (read GetState().NumResidentLODs + MipBiasFade.CalcMipBias for held/restoring textures inside the captured frame's
RT pass), trailing event record after revert until RT truth says restored (with cap), FSM restore-settle before next fire; masks armed through the trailing window.
Can-fail gate: pixel sharpness step vs label per edge, both orders; IAI.Bench lever that removes the RT truth (label from GT) must fail.
