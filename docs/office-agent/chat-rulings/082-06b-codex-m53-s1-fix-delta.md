> Relay run: _relay\runs\2026-09-27-082-06b-m53-s1-fix-delta | model gpt-6-astra | effort max | sandbox workspace-write | duration 649 s (2026-09-27T04:26:22 -> 04:37:11) | copied verbatim from review.md by Claude Code

**082-06b delta review — 2026-09-27.** Seven findings RESOLVED; P2-2 and P3-3 PARTIAL. No new P1/P2 found.

Reviewed git objects `193bd35` → `2f81b3b2d203d6e4f33b6f40015549ae310a6048`, the original review, Chat's disposition, Code's journal and banked evidence. All references below are at that head. Injector basenames are under `Source/AnomalyInjector/Private/Anomalies/`; capture references are under `Source/AnomalyCapture/Private/`. Engine references are relative to the local UE 5.1.1 `Engine/` directory.

| Finding | Status | Delta evidence |
|---|---|---|
| P2-1 | RESOLVED | The same live slot/parameter predicate measures apply and NoApply; no lever or commit-count shortcut. `Anomaly_TexCorrupt.cpp:1172`; `TexCorruptPure.h:482`. |
| P2-2 | PARTIAL | Primitive collection bypasses candidate filters and counts shader/residency/cap gaps, but still silently omits null materials and unresolved bindings. `Anomaly_TexCorrupt.cpp:421`, `:450`, `:466`, `:536`. See below. |
| P2-3 | RESOLVED | Async accumulation receives snapshot telemetry and reads its mode under the existing event identity; sync supplies capture-time telemetry. `AnomalyCaptureSubsystem.cpp:4265`, `:4648`, `:6011`, `:6022`. |
| P2-4 | RESOLVED | Combined usages, per-LOD/shared lighting, spline, cloth and morph requirements are checked; unknown component/render data refuses. Packaged rules agree with the inspected engine paths. `TexCorruptTree.cpp:171`, `:227`, `:267`; `TexCorruptPure.h:568`, `:582`. |
| P2-5 | RESOLVED | Each created allocation is accounted immediately, including created-then-rejected targets; releases enter pending and only never-created reservation is unreserved. `Anomaly_TexCorrupt.cpp:149`, `:163`, `:253`, `:264`; `TexCorruptDraw.cpp:52`; `TexCorruptPure.h:438`, `:452`. |
| P2-6 | RESOLVED | NoApply 2 calls the same owner-watch registration before success. `Anomaly_TexCorrupt.cpp:748`, `:979`. |
| P3-1 | RESOLVED | Revert records frame R; the normal endpoint runs after ledger retirement at R+2, independent of tick countdown. Early/late/cancelled endpoints are explicit. `Anomaly_TexCorrupt.cpp:565`, `:1012`, `:1166`. |
| P3-2 | RESOLVED | An unexamined tail/cycle returns `LimitReached` and the named `host_mid` subreason. `TexCorruptPure.h:657`; `TexCorruptTree.cpp:138`. |
| P3-3 | PARTIAL | Peak semantics and successful-revert balance are corrected, but the new rollback criterion is premature. `docs/predictions/2026-09-26-m53-uv-normal-corruption.md:1304`; `TexCorruptState.cpp:332`, `:345`. See below. |

**Residual P2-2 — incomplete material coverage can still read complete.** A recently drawn non-target static mesh can have a null material slot. UE's `GetUsedMaterials` returns that null (`Source/Runtime/Engine/Private/Components/StaticMeshComponent.cpp:2725`), while its scene proxy renders the default material (`Source/Runtime/Engine/Private/StaticMeshRender.cpp:2225`). The collector skips it at `Anomaly_TexCorrupt.cpp:450`; with no other gaps, `collateral_complete` becomes true at `:1242` without measuring the rendered fallback. Binding extraction also has no incompleteness path when a texture entry cannot be resolved (`TexCorruptTree.cpp:728`; `Anomaly_TexCorrupt.cpp:466`). Proposed fix: collect the effective fallback where determinable, otherwise count unresolved material/binding coverage as incomplete. Add collection/completeness cases; the harness's primitive-selection booleans do not exercise this boundary (`tools/texcorrupt_pure_test.cpp:602`). This is the remaining coverage portion of the original P2-2.

**New P3 within the P3-3 fix — rollback gate expects zero too soon.** The revised plan requires zero live/pending “on the rollback line” (`docs/predictions/2026-09-26-m53-uv-normal-corruption.md:1307`). After an allocation-ordinal failure, that line correctly has nonzero pending (`Anomaly_TexCorrupt.cpp:174`); `FailAt` schedules no terminal sample. A literal checker therefore fails a correct rollback. Proposed fix: require live=0 and pending=created at failure F, then live=pending=0 at a frame-qualified F+2 reading, with that reading emitted for rollback too.

**Evidence:** saved harness 151/0, mutant 24 failures, including 129 normal rollback sequences. These exercise shared allocation/ledger helpers, not the full UE admission tree or collector. G10 editor/game exit 0, unchanged module declarations, comment strip 0 changes; archived executable and the two changed-module DLL hashes match their manifest. G10's `1d7cada` differs from the reviewed head only in documentation. No builds, harness execution, cooks, launches or bench/repository mutations were performed here.

**Ready for authoring, cook and legs: NO.** The exact blocking item under Chat's no-unresolved-P2 rule is P2-2's unreported material/binding coverage gaps. The P3 rollback criterion also needs correction before evaluating that gate. Deferred in-engine proofs remain unrun, as Code declares.

**Chat review REQUIRED** for disposition and release of the next stage; these are findings and proposed fixes, not accepted implementation instructions.

VERDICT: CHANGES REQUIRED
