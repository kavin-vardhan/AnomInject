# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 086 camera clipping label fix (Code, Opus 5.5)

# 086-02 — camera_clipping: replace the sphere-overlap label proxy with a view-slab test on render geometry (pawn included), and add the B-CC camera-schedule bench lever; rebuild. Compiling allowed; NO game launch

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long. Put status notes in the same message as your next tool call and keep going.

End the run only when:
- (a) every step below is done;
- (b) a stop condition below fires; or
- (c) something you need is deliberately protected from you or needs the owner.

Every final message follows the structure at the end and starts with a `VERDICT:` line. Builds outlive the tool timeout: run them in the background and poll. **Keep context lean.** Write docs with the file tools (G331).

## Why
**Label sync is the product** (`_reviews\085-00-chat-ruling-delivery-cut.md` ADDENDUM). The 086-01 audit (`_reviews\086-01-label-sync-audit.md` §5.3, and ruling `086-01-chat-ruling-audit.md`) found that camera_clipping's per-frame label is a **proxy** (`AnomalyViewport.cpp:857-885`: a sphere of radius `GNearClippingPlane` via `OverlapAnyTestByChannel`, `ECC_Visibility`, pawn ignored). It disagrees with the picture in three ways:
1. geometry behind the camera labels positive;
2. collision is not render geometry;
3. the pawn is ignored.

It also has a circular gate history (m30). The owner decides on Thursday whether it ships; this brief makes a proof possible.

## Bootstrap
Read §5.3 and the B-CC row of `_reviews\086-01-label-sync-audit.md`, `Anomaly_CameraClipping.cpp`, `AnomalyViewport.cpp:840-900`, and the AnomalyBench camera placement lever. Then summarise in two lines.

## The work (on `fix/m52-label-timing`, after 084-05a's commits; build through the warm host `E:\IA_BuildCache\_r84_host`)
1. **The label fix.** A frame is labelled camera-clipping iff some **rendered** primitive (visible this frame, pawn and its attached meshes **included**) has render geometry inside the view slab between the **baseline** near plane and the **anomalous** near plane, **within the view frustum**. So geometry behind or beside the camera no longer counts.
   - Use render bounds as the broad phase. Add a finer test (for example a per-section or per-triangle check against the slab for primitives whose bounds straddle it) if it's cheap enough, with the cost measured.
   - State exactly which cases the chosen test can still get wrong. The B-CC pixel oracle will judge it.
   - **Keep the old proxy's result as a diagnostic key** (`camera_clipping.sphere_proxy`) for comparison.
2. **The B-CC bench lever:** `IAI.Bench.CameraSchedule`, in `AnomalyBench`, registered only under `-IAIBench` and compiled out of Shipping.
   - It drives the view through a declared pose schedule: 150 → 50 → 150 cm from a wall, a pass where geometry is only behind the camera, a non-colliding prop close to the lens, and the pawn close to the lens if the fixture has one. It is deterministic per frame index.
   - **The matched null** is the same schedule with the near plane at baseline.
3. **Unit tests** for the slab test (synthetic bounds and poses), both ways: the old sphere proxy fails the behind-camera and non-colliding cases, and the new test passes them.
4. **Rebuild** Game and Editor, 0 plugin warnings. Archive under `_binary_baselines\m52fix-<hash>\` (it's the same integration branch; name the archive for the hash). **No staging.**
5. **Docs:** the journal, the client readme note on camera_clipping's label definition, and a gotcha if new. Commit and push. No tag, no merge.
6. **Give the B-CC leg spec** for 084-05b's harness: legs, frames, the pixel oracle (ON vs null at the same frame index), and the gate.

## Hard rules
- **No game launch, no staging, and nothing that takes focus.**
- Never kill anything that is not yours, and never by name.
- Don't touch `m51`, `master` (except branching), tags, `ToCodex\`, `E:\<personal-folder>`, or the m53 S2 files.
- Codenames only.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner**: 2 lines.
3. The label fix: the test used, its cost, its known limits.
4. The lever.
5. Unit tests, both ways.
6. Build: hashes, warnings, archive.
7. The B-CC leg spec.
8. Commits pushed.
9. NEEDS-DECISION items, or "none".
