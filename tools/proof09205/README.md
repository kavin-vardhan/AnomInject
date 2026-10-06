# 092-05 evidence and reproduction

This is the bounded `feat/vt-blur-stacks` proof, based on `af79520`. Part A was pushed
at `94779af` before Part B began. Its tested runtime is `16b5f58` / `381ED90F`.
The corrected Part B runtime is `502de71` / `4458129F`. This is not a delivery.

Office inventory commands:

```
IAI.TexCorrupt.Census vt
IAI.TexCorrupt.Census all vt
```

The first uses the current view; the second uses all loaded drawable components.
Read the numeric family, format-set and address legends in client-readme 8.8.
Predictions are in `predictions.md`, `part-b-design-before.md`,
`part-b-pixel-gates-before.json`, `part-b-matrix-before.md` and the named correction notes.
No corrected count or pixel tolerance was inferred silently from an observed result.

The harness paths pin `E:/IA_BuildCache/_r921/09205`, source worktree
`E:/IA_BuildCache/_r921/AnomalyInjector`, isolated host `E:/IA_BuildCache/_r53_host/StackOBot`
and local UE 5.1.1 source. Fixture content is outside the plugin repository; both fixture ZIP
manifests are in `records/archives`. The original single-layer fixture remains unchanged.
Every runtime launch records executable/DLL hashes, owned PID, capture settings and the strict
idle gate. `runtime_guard.py` watches the owned engine process. Only one engine leg runs at a time.

`strict_build.py` performs isolated include builds and restores normal build settings.
`rebuild_audit_v2.py` records the corrected final archive, lever audit and string classification.
`cook_stacks.py` invokes the existing AutomationTool directly with `-nocompile`.
`run_runtime_v2.py` runs corrected census checks first, then the full final proof matrix.
It may overlap a CPU pixel reader with the next owned engine process; engine launches remain
serial and each has a fresh idle gate. The thresholds and readers are unchanged.

The final matrix uses `-v2` run names. `part_b_runtime_gates.py` checks those exact receipts
against the final archive. The earlier F3A8C68C matrix remains in raw evidence: it passed the
pixel/runtime requirements but its final census failed eight fields. The one-line diagnostic
fix preserves inferred required layers after an early unnamed-stack refusal. That failed census
is retained as a can-fail; its expected counts were not changed.

`check_stack_layers.py` measures colour, normal and roughness bands independently, including
intended-blur appearance at both edges. Whole-target change alone cannot prove VT readiness.
`stage_upload_delay.py` records original production-scheduling CVars, delays new private pages
for the early-bind mutant, and restores the queried values. A normal implementation control
uses the same delay. The uncontrolled early-bind PIE result is explicitly inconclusive.

`restore_host.py` restores the original detached host commit, six binaries and 156 configuration
files by receipt. `final_verification.py` rehashes restored files, engine-source evidence,
single-fixture content, fixture ZIPs, cooked artifacts and protected refs.
The final report and session journal identify all superseded/failed attempts and exact commits.

Raw evidence remains at `E:/IA_BuildCache/_r921/09205`. Compact final gate records are copied
under `records`; they reference the full logs, PNGs, labels and annotation files in that folder.
The original Part A helpers remain available for its pushed checkpoint, while final Part B uses
the corrected-head queue described above.
