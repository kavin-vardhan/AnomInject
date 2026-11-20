# GR1 clarification — owner-relayed Chat ruling, 2026-11-06

The owner relayed the following Chat ruling after reviewing the measured console baseline: classify census-cycle differences as session nondeterminism and record both magnitudes. Console A versus console B: 1,339 differing rows, maximum absolute delta 8, mean absolute delta 2.72. Console versus dashboard: 1,639 differing rows, maximum absolute delta 26, mean absolute delta 11.00.

The likely cause is that cycles is a cumulative counter driven by readback timing, and serving live dashboard snapshots adds game-thread work. This is a likely mechanism, not an experimentally isolated causal finding. The classification is authorized because labels, masks, events, schedule, routes, seed and counts agree, while private verification and in-flight differences match the console baseline.

The six label-schema entries arise from one JSON number representation: timestamp `t=320` is serialized as an integer. Normalize this representation only. FUTURE: write time fields as floats; no code change is required for this delivery.

**GR1 passes.** Proceed with GR4: final FJ1 plus one full P9 `main()` at exit, pre-push scan, fast-forward-only publication of plugin `feat/europa-port` and dashboard `feat/europa-425`, no tags or force, confirm both with `ls-remote`, update the handover card and copy it to the claude.ai project, and write `_reviews/094-47-final-report.md`.

GR2–GR5 in `094-47w-chat-ruling.md` remain in force. The D3D12 full comparison pair is dropped. Camera clipping is opt-in, qualified at 1280×720 on D3D11/D3D12, with the documented odd-size depth-precision side effect; default captures retain strict G3.

Chat review NOT REQUIRED for these authorized publication and handover actions. The next mandatory Chat checkpoint is a new correctness failure or a proposed change to capture/label meaning, correctness tolerance, or release scope.
