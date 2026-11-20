---
name: europa-port-51-base-is-release
description: "For 4.25-vs-5.1 parity diffs on feat/europa-port, diff against release/2026-10-05 (d7eee97), not m51's 53bf725"
metadata:
  node_type: memory
  type: reference
  originSessionId: <uuid>
  modified: 2026-11-01T18:21:55.160Z
---

`feat/europa-port`'s merge base with the 5.1 line is `release/2026-10-05` = `d7eee97` (checked 2026-11-01, 094-38).
- `d7eee97..07c853e` is +782/−81 lines in `Source/AnomalyInjector`.
- Diffing against m51's `53bf725` instead shows ~18,800 inserted lines (m52–m55 work) and buries the port delta.
- Codex's parity inventory (`docs/europa/09438-auto-parity.json`) counts only lines that differ from the same branch's 5.1 path. Shared edits that also change 5.1 (the pool +2 "shared_extension", the tickable-subsystem base, raw UPROPERTY pointers) don't appear in it as 4.25-only lines.
- Line numbers in the shared `AnomalyCaptureSubsystem.cpp` drift between the port branch and `d7eee97`: the default far plane is 594/10291 on the branch and 573/10184 at release.

Tools that check this: `D:\IA_Europa\Temp\CodeReview09438\sweep38.py` (inventory vs diff) and `rootnames38.py` (drive-root names in committed docs, counts only).

Related: [[europa-425-editor-runtime-facts]], [[gdp-impl-relay-orchestrator-guard]].
