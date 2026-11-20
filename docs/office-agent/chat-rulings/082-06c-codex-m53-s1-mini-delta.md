> Relay run: _relay\runs\2026-09-27-082-06c-m53-s1-mini-delta | model gpt-6-astra | effort max | sandbox workspace-write | duration 290 s (2026-09-27T05:00:35 -> 05:05:24) | copied verbatim from review.md by Claude Code

**082-06c mini-delta: both specified remainders RESOLVED.** Reviewed git objects `40aa07a` -> `02987b76e0a65d2faeaed17536dd9bf2f1131728`, the prior delta, Chat's ruling, Code's journal and banked evidence. Code citations below are at that head; injector basenames are under `Source/AnomalyInjector/Private/Anomalies/`.

1. **P2-2 remainder: RESOLVED.** Null slots now pass through the engine default material and its active bindings (`Anomaly_TexCorrupt.cpp:426`, `:459`, `:470`). Unresolved texture entries increment the counter (`:477`; `TexCorruptPure.h:709`), enter the incompleteness sum (`Anomaly_TexCorrupt.cpp:551`) and prevent `collateral_complete` (`:1265`). An unavailable default also counts unresolved (`TexCorruptPure.h:695`). Harness cases exercise default-material texture collection and unresolved entries remaining incomplete (`tools/texcorrupt_pure_test.cpp:724`, `:735`).

2. **P3-3: RESOLVED.** Rollback schedules F+2 (`Anomaly_TexCorrupt.cpp:192`); the unconditional tick retires pending bytes before taking that reading (`:1033`). The frame-stamped rollback line includes live/pending and the balance verdict (`:594`). The shared judge permits no verdict before F+2 and requires both counters zero afterward (`TexCorruptPure.h:746`). The plan now explicitly requires the F+2 0/0 reading (`docs/predictions/2026-09-26-m53-uv-normal-corruption.md:1334`). Harness cases allow pending > 0 at rollback and reject residual live or pending bytes at F+2 (`tools/texcorrupt_pure_test.cpp:758`).

**Anything new?** No new defect found in this bounded delta. Additions include unresolved telemetry, null-slot diagnostics and a post-rollback collateral sample. The documented non-null usage-fallback limitation predates these commits and remains outside the specified remainder (`docs/predictions/2026-09-26-m53-uv-normal-corruption.md:108`).

**Evidence:** banked harness 172/0; combined mutant 34 failures; five new faults independently fail 1/3/3/6/1 checks. G10 editor/game exits are 0, comment strip changed 0 files, and module declarations are unchanged. Independently rehashed all 12 archived artifacts: sizes and hashes match. Build source `1fc20ea` differs from the reviewed head only in documentation. No builds, harness execution, cooks, launches or repository/bench writes were performed for this review.

**Ready for authoring, cook and legs: YES.** No blocker remains in these two items. The null-slot and exact F+2 in-engine readings remain due during the planned legs; this is code readiness, not runtime qualification.

**Chat review REQUIRED** for disposition and release of the next stage, as Chat retains that ruling under this brief.

VERDICT: APPROVE
