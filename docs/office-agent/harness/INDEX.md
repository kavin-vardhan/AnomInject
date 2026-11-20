# Harness scripts from the home `_reviews` folder (as text)

Every bench campaign, gate evaluator, oracle and preflight that chat or Code wrote in `_reviews\` sat there
as a script, outside every repo. These are those 364 top-level scripts, each copied with an added `.txt`
suffix so nothing here is built or run by accident. Subfolders of `_reviews` (frozen copies of repo tools,
negative-control copies, evidence data) are not included. Redactions as in [`REDACTIONS.md`](../REDACTIONS.md).

Methods are knowledge: read a harness together with the predictions it enforced (`../runs/`, `../chat-rulings/*-README.md`,
the predictions files in the repo's `docs/predictions/`) and the ruling that accepted or rejected its result.

**How to read the table.** One row per harness (scripts sharing a brief id). "What it measured" is the harness README's
title where one exists, otherwise the first script's own docstring or first comment, otherwise empty (the repo's
no-comment convention reached some harness code too; open the script). "Rulings" are the chat rulings in
`../chat-rulings/` that name one of its scripts or carry its brief id.

Total: 364 scripts, 3.39 MiB, 51 harness groups.

| harness | scripts | what it measured | rulings |
|---|---|---|---|
| 090-10f2 | [090-10f2-author-bias.py](090-10f2-author-bias.py.txt), [090-10f2-common.py](090-10f2-common.py.txt), [090-10f2-cook.ps1](090-10f2-cook.ps1.txt), [090-10f2-go.py](090-10f2-go.py.txt), [090-10f2-twopart.py](090-10f2-twopart.py.txt) | 090-10f2 Item 3 fixture: cooked per-texture, texture-group and cinematic LOD bias. Run in an editor commandlet (-run=pythonscript). Creates NEW assets only under /Game/CaptureBenchTexCorrupt/Bias0910f2: for each variant  | [090-10f2-ruling](../chat-rulings/090-10f2-ruling.md) |
| 090-10e | [090-10e-common.py](090-10e-common.py.txt), [090-10e-edges.py](090-10e-edges.py.txt), [090-10e-eval.py](090-10e-eval.py.txt), [090-10e-go.py](090-10e-go.py.txt), [090-10e-read.py](090-10e-read.py.txt), [090-10e-recheck.py](090-10e-recheck.py.txt), [090-10e-strings.py](090-10e-strings.py.txt) | 090-10e bench harness. Reuses 090-10c-common (idle gate with the clarified open-but-idle editor rule, editor-DLL swap, PIE driver, staged runner, pixel reader) with its own work dir E:\IA_BuildCache\_r910e. Nothing here  | - |
| 090-10c | [090-10c-common.py](090-10c-common.py.txt), [090-10c-go.py](090-10c-go.py.txt), [090-10c-read.py](090-10c-read.py.txt), [090-10c-sum.py](090-10c-sum.py.txt) | 090-10c bench harness. Reuses 090-10b-common (staging, pixel reader, leg launcher) and 090-10b2's PIE driver. Own work dir. The idle gate follows the brief's clarification: a foreign editor that is open but idle does not | [090-10c-chat-ruling-r1](../chat-rulings/090-10c-chat-ruling-r1.md) |
| 090-10b2 | [090-10b2-common.py](090-10b2-common.py.txt), [090-10b2-go.py](090-10b2-go.py.txt), [090-10b2-pie.py](090-10b2-pie.py.txt), [090-10b2-read.py](090-10b2-read.py.txt) | 090-10b2 bench harness. Reuses 090-10b-common (quiet gate, staging, pixel reader); own work dir, own PIE driver (090-10b2-pie.py: per-tick frame-time recorder and PYSTEP hooks), own editor-DLL swap without a backup step  | [090-10b2-chat-ruling](../chat-rulings/090-10b2-chat-ruling.md) |
| 090-10b | [090-10b-census-timing.py](090-10b-census-timing.py.txt), [090-10b-common.py](090-10b-common.py.txt), [090-10b-edges.py](090-10b-edges.py.txt), [090-10b-go.py](090-10b-go.py.txt), [090-10b-pie.py](090-10b-pie.py.txt), [090-10b-run.ps1](090-10b-run.ps1.txt) | 090-10b census timing from a game log: how long IAI.TexCorrupt.Census held the game thread. Old builds run the whole census inside one frame: the stall is the time from the last line of an earlier frame to the census 'sc | - |
| 090-06 | [090-06-common.py](090-06-common.py.txt), [090-06-eval.py](090-06-eval.py.txt), [090-06-gateselftest.py](090-06-gateselftest.py.txt), [090-06-legs.py](090-06-legs.py.txt), [090-06-lib.py](090-06-lib.py.txt), [090-06-run.ps1](090-06-run.ps1.txt) | 090-06 — tonight's bench smoke of the 090-05 m53 head (prepared, NOT run) | [090-06-README](../chat-rulings/090-06-README.md), [090-06-chat-ruling-not-idle](../chat-rulings/090-06-chat-ruling-not-idle.md) |
| 089-04b | [089-04b-gmode.py](089-04b-gmode.py.txt), [089-04b-rd5.py](089-04b-rd5.py.txt) |  | [089-04b-chat-ruling-m53-finish](../chat-rulings/089-04b-chat-ruling-m53-finish.md), [090-06-README](../chat-rulings/090-06-README.md) |
| 089-04 | [089-04-common.py](089-04-common.py.txt), [089-04-container-diff.py](089-04-container-diff.py.txt), [089-04-cook.ps1](089-04-cook.ps1.txt), [089-04-eval.py](089-04-eval.py.txt), [089-04-gateselftest.py](089-04-gateselftest.py.txt), [089-04-legs.py](089-04-legs.py.txt), [089-04-lib.py](089-04-lib.py.txt), [089-04-make-eval.py](089-04-make-eval.py.txt), [089-04-make-lib.py](089-04-make-lib.py.txt), [089-04-oracle.py](089-04-oracle.py.txt), [089-04-run.ps1](089-04-run.ps1.txt), [089-04-window.py](089-04-window.py.txt) | 089-04 — the m53 re-bench (prepared by 089-02b; the bench runs as 089-04) | [089-04-README](../chat-rulings/089-04-README.md), [089-04-chat-ruling-m53-rebench](../chat-rulings/089-04-chat-ruling-m53-rebench.md), [090-06-README](../chat-rulings/090-06-README.md) |
| 089-03 | [089-03-common.py](089-03-common.py.txt), [089-03-container-diff.py](089-03-container-diff.py.txt), [089-03-eval-new.py](089-03-eval-new.py.txt), [089-03-eval.py](089-03-eval.py.txt), [089-03-gateselftest.py](089-03-gateselftest.py.txt), [089-03-kit.py](089-03-kit.py.txt), [089-03-legs.py](089-03-legs.py.txt), [089-03-lib.py](089-03-lib.py.txt), [089-03-lodfix-illum.py](089-03-lodfix-illum.py.txt), [089-03-lodfix-ue.py](089-03-lodfix-ue.py.txt), [089-03-make-eval.py](089-03-make-eval.py.txt), [089-03-part0.ps1](089-03-part0.ps1.txt), [089-03-run.ps1](089-03-run.ps1.txt), [089-03-ue.ps1](089-03-ue.ps1.txt), [089-03-window.py](089-03-window.py.txt) | 089-03 — the targeted re-bench on the fix branch (prepared by 089-02a; the bench runs as 089-03) | [089-03-README](../chat-rulings/089-03-README.md), [089-03-chat-ruling-rebench](../chat-rulings/089-03-chat-ruling-rebench.md), [089-04-README](../chat-rulings/089-04-README.md), [090-06-README](../chat-rulings/090-06-README.md) |
| 089-01 | [089-01-tables.py](089-01-tables.py.txt) |  | [089-01-chat-ruling-adjudication](../chat-rulings/089-01-chat-ruling-adjudication.md) |
| 085-04 | [085-04-common.py](085-04-common.py.txt), [085-04-eval.py](085-04-eval.py.txt), [085-04-gateselftest.py](085-04-gateselftest.py.txt), [085-04-legs.py](085-04-legs.py.txt), [085-04-lib.py](085-04-lib.py.txt), [085-04-m53rows.py](085-04-m53rows.py.txt), [085-04-oracle.py](085-04-oracle.py.txt), [085-04-run.ps1](085-04-run.ps1.txt), [085-04-window.py](085-04-window.py.txt) | 085-04 — B-M53 harness: the m53 DC label-sync release gate, the mode oracle and every DC night gate (written by 085-03; the bench ... | [085-04-README](../chat-rulings/085-04-README.md), [089-04-README](../chat-rulings/089-04-README.md), [090-06-README](../chat-rulings/090-06-README.md) |
| 085-03 | [085-03-cmdlet.ps1](085-03-cmdlet.ps1.txt), [085-03-container-diff.py](085-03-container-diff.py.txt), [085-03-content-manifest.py](085-03-content-manifest.py.txt), [085-03-cook.ps1](085-03-cook.ps1.txt), [085-03-nd4-preread.py](085-03-nd4-preread.py.txt) | 085-03 container diff: list two cooked pairings with UnrealPak -List (utoc chunks + pak entries) and compare per-entry hashes. Usage: 085-03-container-diff.py <tag> <old_paks_dir> <new_paks_dir> -> 085-03-evidence/contai | [085-03-chat-ruling-fixture-harness](../chat-rulings/085-03-chat-ruling-fixture-harness.md), [085-04-README](../chat-rulings/085-04-README.md) |
| 084-09 | [084-09-common.py](084-09-common.py.txt), [084-09-eval-new.py](084-09-eval-new.py.txt), [084-09-eval.py](084-09-eval.py.txt), [084-09-gateselftest.py](084-09-gateselftest.py.txt), [084-09-legs.py](084-09-legs.py.txt), [084-09-lib.py](084-09-lib.py.txt), [084-09-lodscan-ue.py](084-09-lodscan-ue.py.txt), [084-09-lodselect.py](084-09-lodselect.py.txt), [084-09-make-eval.py](084-09-make-eval.py.txt), [084-09-run.ps1](084-09-run.ps1.txt), [084-09-window.py](084-09-window.py.txt) | 084-09 — label-sync harness, round 2 (written by 084-08; the bench runs as 084-09) | [084-09-README](../chat-rulings/084-09-README.md), [087-01-chat-ruling-office-kit](../chat-rulings/087-01-chat-ruling-office-kit.md), [089-03-README](../chat-rulings/089-03-README.md), [089-04-README](../chat-rulings/089-04-README.md), [090-06-README](../chat-rulings/090-06-README.md) |
| 084-08b | [084-08b-container-diff.py](084-08b-container-diff.py.txt), [084-08b-cook-baseline.py](084-08b-cook-baseline.py.txt), [084-08b-lodfix-scan-ue.py](084-08b-lodfix-scan-ue.py.txt), [084-08b-lodfix-select.py](084-08b-lodfix-select.py.txt), [084-08b-probe-ue.py](084-08b-probe-ue.py.txt) |  | [084-08b-chat-ruling-lod-fixture](../chat-rulings/084-08b-chat-ruling-lod-fixture.md), [084-09-README](../chat-rulings/084-09-README.md), [089-03-README](../chat-rulings/089-03-README.md) |
| 084-06 | [084-06-common.py](084-06-common.py.txt), [084-06-eval.py](084-06-eval.py.txt), [084-06-gateselftest.py](084-06-gateselftest.py.txt), [084-06-legs.py](084-06-legs.py.txt), [084-06-lib.py](084-06-lib.py.txt), [084-06-lyra.ps1](084-06-lyra.ps1.txt), [084-06-proofs.py](084-06-proofs.py.txt), [084-06-run.ps1](084-06-run.ps1.txt), [084-06-window.py](084-06-window.py.txt) | 084-06 — label-sync bench harness (m52 confirmation, MASK55, B-CC, B-REAL, B-LOD, B-CC-MW; B-LYRA optional) | [084-06-README](../chat-rulings/084-06-README.md), [084-06-chat-ruling-bench](../chat-rulings/084-06-chat-ruling-bench.md), [084-09-README](../chat-rulings/084-09-README.md), [089-03-README](../chat-rulings/089-03-README.md), [089-04-README](../chat-rulings/089-04-README.md), [090-06-README](../chat-rulings/090-06-README.md) |
| 084-03 | [084-03-common.py](084-03-common.py.txt), [084-03-gateselftest.py](084-03-gateselftest.py.txt), [084-03-lib.py](084-03-lib.py.txt), [084-03-run.ps1](084-03-run.ps1.txt), [084-03-window.py](084-03-window.py.txt) | 084-03 — part B harness (m52 label-sync, baseline vs fix) | [084-03-README](../chat-rulings/084-03-README.md), [084-03-chat-ruling-bench-results](../chat-rulings/084-03-chat-ruling-bench-results.md), [084-06-README](../chat-rulings/084-06-README.md), [089-03-README](../chat-rulings/089-03-README.md), [089-04-README](../chat-rulings/089-04-README.md), [090-06-README](../chat-rulings/090-06-README.md) |
| 082-14 | [082-14-s20-common.py](082-14-s20-common.py.txt), [082-14-s20-lib.py](082-14-s20-lib.py.txt), [082-14-s20-postflight.py](082-14-s20-postflight.py.txt), [082-14-s20-preflight.py](082-14-s20-preflight.py.txt), [082-14-s20-synth.py](082-14-s20-synth.py.txt), [082-14-s20-tokens.py](082-14-s20-tokens.py.txt), [082-14-s20-window.py](082-14-s20-window.py.txt) |  | [082-14-chat-ruling-s20-harness](../chat-rulings/082-14-chat-ruling-s20-harness.md), [085-04-README](../chat-rulings/085-04-README.md) |
| 082-07 | [082-07-common.py](082-07-common.py.txt), [082-07-legs.py](082-07-legs.py.txt), [082-07-lib.py](082-07-lib.py.txt), [082-07-postflight.py](082-07-postflight.py.txt), [082-07-preflight.py](082-07-preflight.py.txt), [082-07-run.ps1](082-07-run.ps1.txt), [082-07-window.py](082-07-window.py.txt) |  | [082-07b-chat-ruling-harness-accepted](../chat-rulings/082-07b-chat-ruling-harness-accepted.md), [082-07c-chat-ruling-s1-legs-red](../chat-rulings/082-07c-chat-ruling-s1-legs-red.md), [082-07d-chat-ruling-harness-accepted](../chat-rulings/082-07d-chat-ruling-harness-accepted.md), [082-07f-chat-ruling-halo-and-amendment-2](../chat-rulings/082-07f-chat-ruling-halo-and-amendment-2.md), [082-07g-chat-ruling-defaults-accepted](../chat-rulings/082-07g-chat-ruling-defaults-accepted.md), [082-07i-chat-ruling-amendment-3-accepted](../chat-rulings/082-07i-chat-ruling-amendment-3-accepted.md), and 4 more |
| 081-44 | [081-44-build.ps1](081-44-build.ps1.txt), [081-44-stage.py](081-44-stage.py.txt) |  | [081-44-chat-ruling-fixes-accepted-merge](../chat-rulings/081-44-chat-ruling-fixes-accepted-merge.md) |
| 081-37 | [081-37-cost-lib.py](081-37-cost-lib.py.txt), [081-37-cost-sampler.py](081-37-cost-sampler.py.txt), [081-37-cost-window.py](081-37-cost-window.py.txt) |  | [081-37-chat-ruling-requal-complete-stage-3-cost](../chat-rulings/081-37-chat-ruling-requal-complete-stage-3-cost.md), [081-38-chat-ruling-cost-design-accepted](../chat-rulings/081-38-chat-ruling-cost-design-accepted.md) |
| 081-23 | [081-23-b1.py](081-23-b1.py.txt), [081-23-c0-analyze.py](081-23-c0-analyze.py.txt), [081-23-c0-compare.py](081-23-c0-compare.py.txt), [081-23-c0-part2.py](081-23-c0-part2.py.txt), [081-23-chunk0.py](081-23-chunk0.py.txt), [081-23-chunk1.py](081-23-chunk1.py.txt), [081-23-chunk2.py](081-23-chunk2.py.txt), [081-23-chunk3.py](081-23-chunk3.py.txt), [081-23-common.py](081-23-common.py.txt), [081-23-derive.py](081-23-derive.py.txt), [081-23-dryrun.py](081-23-dryrun.py.txt), [081-23-lib.py](081-23-lib.py.txt), [081-23-lyra-pre.py](081-23-lyra-pre.py.txt), [081-23-lyra-restore.py](081-23-lyra-restore.py.txt), [081-23-lyra-run.ps1](081-23-lyra-run.ps1.txt), [081-23-lyra-stage.py](081-23-lyra-stage.py.txt), [081-23-measure.ps1](081-23-measure.ps1.txt), [081-23-nohold-run.ps1](081-23-nohold-run.ps1.txt), [081-23-postflight.py](081-23-postflight.py.txt), [081-23-preflight.py](081-23-preflight.py.txt), [081-23-wait.py](081-23-wait.py.txt), [081-23-window.py](081-23-window.py.txt) | The bench's foreground-focus gate requires a visible game window. | [081-25-chat-ruling-g8-predicate-correction](../chat-rulings/081-25-chat-ruling-g8-predicate-correction.md) |
| 081-22 | [081-22-a.py](081-22-a.py.txt), [081-22-analyze.py](081-22-analyze.py.txt), [081-22-b.py](081-22-b.py.txt), [081-22-b1.py](081-22-b1.py.txt), [081-22-close.py](081-22-close.py.txt), [081-22-common.py](081-22-common.py.txt), [081-22-compare.py](081-22-compare.py.txt), [081-22-derive.py](081-22-derive.py.txt), [081-22-f1.ps1](081-22-f1.ps1.txt), [081-22-legacy-run.ps1](081-22-legacy-run.ps1.txt), [081-22-legacy-selftest.py](081-22-legacy-selftest.py.txt), [081-22-legacy.py](081-22-legacy.py.txt), [081-22-lyra-pre.py](081-22-lyra-pre.py.txt), [081-22-lyra-restore.py](081-22-lyra-restore.py.txt), [081-22-lyra-run.ps1](081-22-lyra-run.ps1.txt), [081-22-lyra-stage.py](081-22-lyra-stage.py.txt), [081-22-measure.ps1](081-22-measure.ps1.txt), [081-22-oracle-p2.py](081-22-oracle-p2.py.txt), [081-22-part2.py](081-22-part2.py.txt), [081-22-start.py](081-22-start.py.txt), [081-22-wait.py](081-22-wait.py.txt) | The bench's foreground-focus gate requires a visible game window. | [081-22-chat-ruling-legacy-d-accepted-requal-prep](../chat-rulings/081-22-chat-ruling-legacy-d-accepted-requal-prep.md) |
| 081-21 | [081-21-build.ps1](081-21-build.ps1.txt), [081-21-close.py](081-21-close.py.txt), [081-21-common.py](081-21-common.py.txt), [081-21-idle.py](081-21-idle.py.txt), [081-21-lyra-restore.py](081-21-lyra-restore.py.txt), [081-21-stage.py](081-21-stage.py.txt), [081-21-start.py](081-21-start.py.txt) |  | [081-21-chat-ruling-build3-accepted-bench-when-quiet](../chat-rulings/081-21-chat-ruling-build3-accepted-bench-when-quiet.md) |
| 081-18 | [081-18-analyze.py](081-18-analyze.py.txt), [081-18-b1.py](081-18-b1.py.txt), [081-18-close.py](081-18-close.py.txt), [081-18-common.py](081-18-common.py.txt), [081-18-compare.py](081-18-compare.py.txt), [081-18-derive.py](081-18-derive.py.txt), [081-18-lyra-pre.py](081-18-lyra-pre.py.txt), [081-18-lyra-restore.py](081-18-lyra-restore.py.txt), [081-18-lyra-run.ps1](081-18-lyra-run.ps1.txt), [081-18-lyra-stage.py](081-18-lyra-stage.py.txt), [081-18-lyra.py](081-18-lyra.py.txt), [081-18-measure.ps1](081-18-measure.ps1.txt), [081-18-stack.py](081-18-stack.py.txt), [081-18-start.py](081-18-start.py.txt) | The bench's foreground-focus gate requires a visible game window. | [081-18-chat-ruling-bench-blocked-oracle-next](../chat-rulings/081-18-chat-ruling-bench-blocked-oracle-next.md) |
| 081-17 | [081-17-b1.py](081-17-b1.py.txt), [081-17-build.ps1](081-17-build.ps1.txt), [081-17-close.py](081-17-close.py.txt), [081-17-lyra-pre.py](081-17-lyra-pre.py.txt), [081-17-lyra-restore.py](081-17-lyra-restore.py.txt), [081-17-lyra-run.ps1](081-17-lyra-run.ps1.txt), [081-17-lyra.py](081-17-lyra.py.txt), [081-17-measure.ps1](081-17-measure.ps1.txt), [081-17-stage.py](081-17-stage.py.txt), [081-17-start.py](081-17-start.py.txt), [081-17-suite.py](081-17-suite.py.txt) | The bench's foreground-focus gate requires a visible game window. | [081-17-chat-ruling-build2-accepted-throughput-readings](../chat-rulings/081-17-chat-ruling-build2-accepted-throughput-readings.md) |
| 081-16 | [081-16-audit.py](081-16-audit.py.txt), [081-16-close.py](081-16-close.py.txt) | Pre-build source audit. No Unreal launch, build, or candidate mutation. | [081-16-chat-ruling-memory-criterion-and-handover](../chat-rulings/081-16-chat-ruling-memory-criterion-and-handover.md) |
| 081-15 | [081-15-b1-core.py](081-15-b1-core.py.txt), [081-15-b1.py](081-15-b1.py.txt), [081-15-build.ps1](081-15-build.ps1.txt), [081-15-capture-pose.py](081-15-capture-pose.py.txt), [081-15-finish.py](081-15-finish.py.txt), [081-15-legacy-run.ps1](081-15-legacy-run.ps1.txt), [081-15-legacy.py](081-15-legacy.py.txt), [081-15-lyra-pre.py](081-15-lyra-pre.py.txt), [081-15-lyra-restore.py](081-15-lyra-restore.py.txt), [081-15-lyra-run.ps1](081-15-lyra-run.ps1.txt), [081-15-lyra.py](081-15-lyra.py.txt), [081-15-measure.ps1](081-15-measure.ps1.txt), [081-15-nohold.py](081-15-nohold.py.txt), [081-15-prepare.py](081-15-prepare.py.txt), [081-15-report.py](081-15-report.py.txt), [081-15-stage.py](081-15-stage.py.txt), [081-15-start.py](081-15-start.py.txt), [081-15-suite.py](081-15-suite.py.txt) | A missing target must not silently disappear from the modal denominator. | [081-15-chat-ruling-budget-bound-build-2](../chat-rulings/081-15-chat-ruling-budget-bound-build-2.md) |
| 081-14 | [081-14-finish.py](081-14-finish.py.txt), [081-14-lyra-pre.py](081-14-lyra-pre.py.txt), [081-14-lyra-restore.py](081-14-lyra-restore.py.txt), [081-14-lyra-run.ps1](081-14-lyra-run.ps1.txt), [081-14-lyra-stage.py](081-14-lyra-stage.py.txt), [081-14-lyra.py](081-14-lyra.py.txt), [081-14-nohold.py](081-14-nohold.py.txt), [081-14-report-finalize.py](081-14-report-finalize.py.txt), [081-14-start.py](081-14-start.py.txt) | The bench's foreground-focus gate requires a visible game window. | [081-14-chat-ruling-closure-clock-g270](../chat-rulings/081-14-chat-ruling-closure-clock-g270.md) |
| 081-13 | [081-13-cause-read.py](081-13-cause-read.py.txt), [081-13-finish.py](081-13-finish.py.txt), [081-13-nohold-run.ps1](081-13-nohold-run.ps1.txt), [081-13-nohold.py](081-13-nohold.py.txt), [081-13-start.py](081-13-start.py.txt) |  | [081-13-chat-ruling-nohold-requalify-g2](../chat-rulings/081-13-chat-ruling-nohold-requalify-g2.md) |
| 081-12 | [081-12-b1.py](081-12-b1.py.txt), [081-12-finish.py](081-12-finish.py.txt), [081-12-legacy-run.ps1](081-12-legacy-run.ps1.txt), [081-12-legacy.py](081-12-legacy.py.txt), [081-12-measure-v2.ps1](081-12-measure-v2.ps1.txt), [081-12-start.py](081-12-start.py.txt), [081-12-suite-v2.py](081-12-suite-v2.py.txt) | The bench's foreground-focus gate requires a visible game window. | [081-12-chat-ruling-nohold-on-mainworld](../chat-rulings/081-12-chat-ruling-nohold-on-mainworld.md) |
| 081-11 | [081-11-b1.py](081-11-b1.py.txt), [081-11-build.ps1](081-11-build.ps1.txt), [081-11-finish.py](081-11-finish.py.txt), [081-11-legacy-run.ps1](081-11-legacy-run.ps1.txt), [081-11-legacy.py](081-11-legacy.py.txt), [081-11-measure-v2.ps1](081-11-measure-v2.ps1.txt), [081-11-measure.ps1](081-11-measure.ps1.txt), [081-11-preflight.ps1](081-11-preflight.ps1.txt), [081-11-stage.py](081-11-stage.py.txt), [081-11-start.py](081-11-start.py.txt), [081-11-static.py](081-11-static.py.txt), [081-11-suite-v2.py](081-11-suite-v2.py.txt), [081-11-validate-v2.py](081-11-validate-v2.py.txt), [081-11-validate.py](081-11-validate.py.txt) | The bench's foreground-focus gate requires a visible game window. | [081-11-chat-ruling-nohold-target-finish-stage-2](../chat-rulings/081-11-chat-ruling-nohold-target-finish-stage-2.md) |
| 081-10 | [081-10-build.ps1](081-10-build.ps1.txt), [081-10-finish.py](081-10-finish.py.txt), [081-10-preflight.ps1](081-10-preflight.ps1.txt), [081-10-prepare.py](081-10-prepare.py.txt), [081-10-report.py](081-10-report.py.txt), [081-10-stage.py](081-10-stage.py.txt), [081-10-start.py](081-10-start.py.txt), [081-10-validate.py](081-10-validate.py.txt) | The bench's foreground-focus gate requires a visible game window. | [081-10-chat-ruling-fix-guard-and-revalidate](../chat-rulings/081-10-chat-ruling-fix-guard-and-revalidate.md) |
| 081-09 | [081-09-diagnose.py](081-09-diagnose.py.txt), [081-09-finish.py](081-09-finish.py.txt), [081-09-preflight.ps1](081-09-preflight.ps1.txt), [081-09-report.py](081-09-report.py.txt), [081-09-start.py](081-09-start.py.txt) | A missing target must not silently disappear from the modal denominator. | [081-09-chat-ruling-fixture-fix](../chat-rulings/081-09-chat-ruling-fixture-fix.md) |
| 081-08 | [081-08-audit.py](081-08-audit.py.txt), [081-08-boundary.py](081-08-boundary.py.txt), [081-08-build.ps1](081-08-build.ps1.txt), [081-08-close-read.py](081-08-close-read.py.txt), [081-08-current.py](081-08-current.py.txt), [081-08-edit-stage.py](081-08-edit-stage.py.txt), [081-08-finish-boundary.py](081-08-finish-boundary.py.txt), [081-08-integrate.py](081-08-integrate.py.txt), [081-08-l2.py](081-08-l2.py.txt), [081-08-legacy.py](081-08-legacy.py.txt), [081-08-lyra-suite.py](081-08-lyra-suite.py.txt), [081-08-pose.py](081-08-pose.py.txt), [081-08-prepare-runners.py](081-08-prepare-runners.py.txt), [081-08-stage.py](081-08-stage.py.txt), [081-08-suite.py](081-08-suite.py.txt), [081-08-write-docs.py](081-08-write-docs.py.txt) | Retire an unused invalid recipe and its private B1 exception; original runner is untouched. | [081-08-chat-rulings-stage-2-continuation](../chat-rulings/081-08-chat-rulings-stage-2-continuation.md) |
| 081-07 | [081-07-boundary.py](081-07-boundary.py.txt), [081-07-finalize.py](081-07-finalize.py.txt), [081-07-inspect.py](081-07-inspect.py.txt) | Freeze whole authoritative source scopes, so a negative finding is reviewable. | [081-07-chat-ruling-lyra-scoped-stage-2-released](../chat-rulings/081-07-chat-ruling-lyra-scoped-stage-2-released.md) |
| 081-06 | [081-06-freeze.py](081-06-freeze.py.txt), [081-06-ledger.py](081-06-ledger.py.txt), [081-06-pose.py](081-06-pose.py.txt), [081-06-postflight.py](081-06-postflight.py.txt), [081-06-restore.py](081-06-restore.py.txt), [081-06-suite.py](081-06-suite.py.txt) |  | [081-06-chat-ruling-lyra-placement](../chat-rulings/081-06-chat-ruling-lyra-placement.md) |
| 081-05 | [081-05-artifact-delta.py](081-05-artifact-delta.py.txt), [081-05-audit.py](081-05-audit.py.txt), [081-05-build-rest.ps1](081-05-build-rest.ps1.txt), [081-05-build-v4.ps1](081-05-build-v4.ps1.txt), [081-05-build-v5.ps1](081-05-build-v5.ps1.txt), [081-05-build-v6.ps1](081-05-build-v6.ps1.txt), [081-05-controls.py](081-05-controls.py.txt), [081-05-final-coverage.py](081-05-final-coverage.py.txt), [081-05-final-suite.py](081-05-final-suite.py.txt), [081-05-freeze.py](081-05-freeze.py.txt), [081-05-ledger.py](081-05-ledger.py.txt), [081-05-legacy-compare.py](081-05-legacy-compare.py.txt), [081-05-lyra-matrix.py](081-05-lyra-matrix.py.txt), [081-05-lyra-suite.py](081-05-lyra-suite.py.txt), [081-05-mask-tag-audit.py](081-05-mask-tag-audit.py.txt), [081-05-matrix.py](081-05-matrix.py.txt), [081-05-postflight.py](081-05-postflight.py.txt), [081-05-predecessor-bench.py](081-05-predecessor-bench.py.txt), [081-05-predecessor-diagnostic.py](081-05-predecessor-diagnostic.py.txt), [081-05-prepare-lyra.py](081-05-prepare-lyra.py.txt), [081-05-prepare.py](081-05-prepare.py.txt), [081-05-restore.py](081-05-restore.py.txt), [081-05-snapshot.py](081-05-snapshot.py.txt), [081-05-source-audit.py](081-05-source-audit.py.txt), [081-05-stage.py](081-05-stage.py.txt), [081-05-tidy-journal.py](081-05-tidy-journal.py.txt) | Stage 1 artifact/identity audit only; no change arithmetic or event verdict. | [081-05-chat-rulings-stage-1-qualification](../chat-rulings/081-05-chat-rulings-stage-1-qualification.md) |
| 081-04 | [081-04-snapshot.py](081-04-snapshot.py.txt) | Read-only Git-object snapshot for the independent m55 design review. | [081-04-chat-rulings-release-stage-1](../chat-rulings/081-04-chat-rulings-release-stage-1.md) |
| 081-02 | [081-02-snapshot.py](081-02-snapshot.py.txt) | Read-only Git-object snapshot for the independent m55 design review. | - |
| 079-15 | [079-15-doc-verification.py](079-15-doc-verification.py.txt) | Confirm the regenerated characterisation document differs from 079-14's copy only in the intended ways: the added Run outcome column, the added caveat sentence, the reworded provenance and disposition lines. Every measur | - |
| 079-14 | [079-14-fixtures.py](079-14-fixtures.py.txt), [079-14-fixtures2.py](079-14-fixtures2.py.txt), [079-14-fixtures3.py](079-14-fixtures3.py.txt), [079-14-mutation-probe.py](079-14-mutation-probe.py.txt), [079-14-probe-raw-d.py](079-14-probe-raw-d.py.txt) | 079-14 independent-review fixtures. Written from the 079-14 brief section 1 semantics. | - |
| 079-13 | [079-13-audit.py](079-13-audit.py.txt), [079-13-bank-validation.py](079-13-bank-validation.py.txt), [079-13-boundary.py](079-13-boundary.py.txt), [079-13-complete-checks.py](079-13-complete-checks.py.txt), [079-13-final-validation.py](079-13-final-validation.py.txt), [079-13-prepare-validation.py](079-13-prepare-validation.py.txt), [079-13-source-scope.py](079-13-source-scope.py.txt), [079-13-update-current.py](079-13-update-current.py.txt), [079-13-update-docs.py](079-13-update-docs.py.txt), [079-13-write-report.py](079-13-write-report.py.txt) | Check measured banks, frozen-source identity and prior fixture coverage. | [079-13-codex-ruling-cap-decoupled-peaks-ties](../chat-rulings/079-13-codex-ruling-cap-decoupled-peaks-ties.md) |
| 079-12 | [079-12-bank-validation.py](079-12-bank-validation.py.txt), [079-12-boundary.py](079-12-boundary.py.txt), [079-12-complete-checks.py](079-12-complete-checks.py.txt), [079-12-final-validation.py](079-12-final-validation.py.txt), [079-12-finish-evidence.py](079-12-finish-evidence.py.txt), [079-12-source-audit.py](079-12-source-audit.py.txt), [079-12-update-current.py](079-12-update-current.py.txt), [079-12-write-report.py](079-12-write-report.py.txt), [079-12-zero-cell-audit.py](079-12-zero-cell-audit.py.txt) | Read-only bank validation of the working079-12 implementation. | [079-12-codex-ruling-monotone-nearest-assignment](../chat-rulings/079-12-codex-ruling-monotone-nearest-assignment.md) |
| 079-10 | [079-10-bank-validation.py](079-10-bank-validation.py.txt), [079-10-complete-checks.py](079-10-complete-checks.py.txt), [079-10-final-validation.py](079-10-final-validation.py.txt), [079-10-write-report.py](079-10-write-report.py.txt) | Read-only bank validation of the working079-10 implementation. | [079-10-codex-delta-boundary-masks-and-rulings](../chat-rulings/079-10-codex-delta-boundary-masks-and-rulings.md) |
| 079-09 | [079-09-bank-validation.py](079-09-bank-validation.py.txt), [079-09-complete-checks.py](079-09-complete-checks.py.txt), [079-09-final-validation.py](079-09-final-validation.py.txt), [079-09-write-report.py](079-09-write-report.py.txt) | Read-only bank validation of the working079-09 implementation. | - |
| 079-08 | [079-08-evidence-summary.py](079-08-evidence-summary.py.txt), [079-08-ordering-probes.py](079-08-ordering-probes.py.txt), [079-08-original-regressions.py](079-08-original-regressions.py.txt), [079-08-review-validation.py](079-08-review-validation.py.txt) | Condense the stored independent review evidence without rerunning any tests. | - |
| 079-07 | [079-07-review-validation.py](079-07-review-validation.py.txt) | Independent bff1b00 review: frozen modules, scratch writes only, bank read-only. | - |
| 079-05 | [079-05-review-validation.py](079-05-review-validation.py.txt) | Independent offline review probes. Loads frozen git blobs; writes only beside this file. | - |
| 078-17 | [078-17-audit-documents.ps1](078-17-audit-documents.ps1.txt), [078-17-check-boundary.ps1](078-17-check-boundary.ps1.txt) |  | - |
| 078-16 | [078-16-verify-boundary.ps1](078-16-verify-boundary.ps1.txt) |  | - |
| 078-15 | [078-15-offline-analysis.py](078-15-offline-analysis.py.txt) | Offline comparisons only. Creates new review evidence; never alters a bank. | - |

## Per-script notes

| script | bytes | first docstring or comment |
|---|---|---|
| [090-10f2-twopart.py](090-10f2-twopart.py.txt) | 5205 | 090-10f2 Item 2 reader: B's component scope on the two-part bench actor. Usage: 090-10f2-twopart.py <session dir> <twopart json> [game.log] Per event: target-mask pixels of the event's value inside the eligible part's pr |
| [090-10f2-go.py](090-10f2-go.py.txt) | 11480 | 090-10f2 legs. Usage: 090-10f2-go.py <phase> <build> [args] a <build> <pie/staged> <runs csv> [types csv] 090-10e's A recipe (fixture, AA-off arbiter, 150 frames, 7 fires, seeds 777..) runs >= 11 add IAI.TexCorrupt.Censu |
| [090-10f2-cook.ps1](090-10f2-cook.ps1.txt) | 2015 | - |
| [090-10f2-common.py](090-10f2-common.py.txt) | 3360 | 090-10f2 bench harness. Reuses 090-10c-common (idle gate with the clarified open-but-idle editor rule, editor-DLL swap, PIE driver, staged runner, pixel reader) and 090-10e's edge reader, with its own work dir on D: (E:  |
| [090-10f2-author-bias.py](090-10f2-author-bias.py.txt) | 6777 | 090-10f2 Item 3 fixture: cooked per-texture, texture-group and cinematic LOD bias. Run in an editor commandlet (-run=pythonscript). Creates NEW assets only under /Game/CaptureBenchTexCorrupt/Bias0910f2: for each variant  |
| [090-10e-strings.py](090-10e-strings.py.txt) | 1878 | 090-10e string scan: printable runs (>= 6 chars) in UTF-16LE and ASCII, set difference between two builds' binaries. Usage: 090-10e-strings.py <old binary> <new binary> [token ...]. Each differing string is classified as |
| [090-10e-recheck.py](090-10e-recheck.py.txt) | 3604 | 090-10e recheck of the earlier "PIE end +1" readings. For every session under the given roots, every event is read twice: (1) 090-10b-edges.py as 090-10b2/090-10c ran it (label = annotation affected_frames, the m49 obser |
| [090-10e-read.py](090-10e-read.py.txt) | 4409 | 090-10e readings. Usage: 090-10e-read.py edges <label>... per-leg edge summary (090-10e-edges) and every event's onset/raw_end/off/pie 090-10e-read.py tools <label>... verify_capture --label-rule, the kit (label_sync_che |
| [090-10e-go.py](090-10e-go.py.txt) | 5120 | 090-10e legs. Usage: 090-10e-go.py <phase> <build> [args] a <build> <mode pie/staged> <runs csv> [types csv] pie_end_settle legs on the fixture, AA-off arbiter, 150 frames, 7 fires; runs >= 11 add IAI.TexCorrupt.Census b |
| [090-10e-eval.py](090-10e-eval.py.txt) | 76559 | - |
| [090-10e-edges.py](090-10e-edges.py.txt) | 6327 | 090-10e label edges from pixels, with pie_end_settle frames left out. Per annotation.json event: L = its labelled frame list (injected_frames; affected_frames is the mask-observable subset since m49 and drops frames whos |
| [090-10e-common.py](090-10e-common.py.txt) | 1507 | 090-10e bench harness. Reuses 090-10c-common (idle gate with the clarified open-but-idle editor rule, editor-DLL swap, PIE driver, staged runner, pixel reader) with its own work dir E:\IA_BuildCache\_r910e. Nothing here  |
| [090-10c-sum.py](090-10c-sum.py.txt) | 5233 | 090-10c summary: proof table (variant x path x type -> events, corrupts, identity, edges) and can-fail table. Usage: 090-10c-sum.py <new_exe8> <old_exe8> [out.json] |
| [090-10c-read.py](090-10c-read.py.txt) | 5769 | 090-10c readers: corruption (090-10b pixel rule), edges (090-10b-edges), identity exactness, and B's mask-vs-change coverage. Usage: 090-10c-read.py leg <label> [...] / census <label> [...] |
| [090-10c-go.py](090-10c-go.py.txt) | 16451 | 090-10c run script. Usage: 090-10c-go.py <phase> [label ...] Phases run legs through 090-10c-common (idle gate, editor-DLL swap, PIE driver, staged runner) and write runs/<label>.json. Scratch configuration (device profi |
| [090-10c-common.py](090-10c-common.py.txt) | 3955 | 090-10c bench harness. Reuses 090-10b-common (staging, pixel reader, leg launcher) and 090-10b2's PIE driver. Own work dir. The idle gate follows the brief's clarification: a foreign editor that is open but idle does not |
| [090-10b2-read.py](090-10b2-read.py.txt) | 7129 | 090-10b2 readings. Usage: 090-10b2-read.py legs <label>... / ism <label>... / e1 <leg> <null> / census <label>... / users <label> |
| [090-10b2-pie.py](090-10b2-pie.py.txt) | 4804 | - |
| [090-10b2-go.py](090-10b2-go.py.txt) | 7229 | 090-10b2 leg batch. Usage: 090-10b2-go.py <BUILD> <leg> [<leg> ...]. Leg names are templates resolved against the build sha8. Every leg waits for the quiet gate (no editor, no game, no foreign GPU-heavy process, 60 s wit |
| [090-10b2-common.py](090-10b2-common.py.txt) | 4953 | 090-10b2 bench harness. Reuses 090-10b-common (quiet gate, staging, pixel reader); own work dir, own PIE driver (090-10b2-pie.py: per-tick frame-time recorder and PYSTEP hooks), own editor-DLL swap without a backup step  |
| [090-10b-run.ps1](090-10b-run.ps1.txt) | 4798 | - |
| [090-10b-pie.py](090-10b-pie.py.txt) | 3275 | - |
| [090-10b-go.py](090-10b-go.py.txt) | 7843 | 090-10b driver. Usage: 090-10b-go.py <leg-label> [<leg-label> ...] / restore / gate Each leg: wait for the quiet gate, stage the exe (staged mode) or swap the host's editor DLLs (game / pie mode), run, read. |
| [090-10b-edges.py](090-10b-edges.py.txt) | 3420 | 090-10b label edges from pixels, independent of masks. For each event: the first frame whose drawn box differs from the clean reference (two frames before the label) by >= the event threshold, and the first frame after t |
| [090-10b-common.py](090-10b-common.py.txt) | 15653 | 090-10b bench harness: idle + person gate, staging, editor-DLL swap, leg runner (staged game / editor -game / editor PIE), and the pixel reader. Nothing here kills a process it did not start; the launcher kills only its  |
| [090-10b-census-timing.py](090-10b-census-timing.py.txt) | 2575 | 090-10b census timing from a game log: how long IAI.TexCorrupt.Census held the game thread. Old builds run the whole census inside one frame: the stall is the time from the last line of an earlier frame to the census 'sc |
| [090-06-run.ps1](090-06-run.ps1.txt) | 7892 | - |
| [090-06-lib.py](090-06-lib.py.txt) | 41639 | - |
| [090-06-legs.py](090-06-legs.py.txt) | 17154 | - |
| [090-06-gateselftest.py](090-06-gateselftest.py.txt) | 33219 | - |
| [090-06-eval.py](090-06-eval.py.txt) | 32350 | - |
| [090-06-common.py](090-06-common.py.txt) | 15305 | - |
| [089-04b-rd5.py](089-04b-rd5.py.txt) | 5461 | - |
| [089-04b-gmode.py](089-04b-gmode.py.txt) | 19132 | - |
| [089-04-window.py](089-04-window.py.txt) | 29914 | - |
| [089-04-run.ps1](089-04-run.ps1.txt) | 8486 | - |
| [089-04-oracle.py](089-04-oracle.py.txt) | 98635 | 085-04 texture-corruption mode oracle (m53 DC plan section 6, S2 plan R2.1). |
| [089-04-make-lib.py](089-04-make-lib.py.txt) | 6853 | - |
| [089-04-make-eval.py](089-04-make-eval.py.txt) | 26710 | ---------------------------------------------------------------------------------------------- 089-02b overrides |
| [089-04-lib.py](089-04-lib.py.txt) | 28807 | - |
| [089-04-legs.py](089-04-legs.py.txt) | 13443 | - |
| [089-04-gateselftest.py](089-04-gateselftest.py.txt) | 22720 | - |
| [089-04-eval.py](089-04-eval.py.txt) | 57085 | - |
| [089-04-cook.ps1](089-04-cook.ps1.txt) | 1828 | - |
| [089-04-container-diff.py](089-04-container-diff.py.txt) | 3766 | - |
| [089-04-common.py](089-04-common.py.txt) | 10198 | - |
| [089-03-window.py](089-03-window.py.txt) | 32186 | - |
| [089-03-ue.ps1](089-03-ue.ps1.txt) | 1384 | - |
| [089-03-run.ps1](089-03-run.ps1.txt) | 8430 | - |
| [089-03-part0.ps1](089-03-part0.ps1.txt) | 2851 | - |
| [089-03-make-eval.py](089-03-make-eval.py.txt) | 8953 | - |
| [089-03-lodfix-ue.py](089-03-lodfix-ue.py.txt) | 7700 | - |
| [089-03-lodfix-illum.py](089-03-lodfix-illum.py.txt) | 2956 | - |
| [089-03-lib.py](089-03-lib.py.txt) | 25452 | - |
| [089-03-legs.py](089-03-legs.py.txt) | 8306 | - |
| [089-03-kit.py](089-03-kit.py.txt) | 7941 | - |
| [089-03-gateselftest.py](089-03-gateselftest.py.txt) | 24647 | - |
| [089-03-eval.py](089-03-eval.py.txt) | 81691 | - |
| [089-03-eval-new.py](089-03-eval-new.py.txt) | 10734 | - |
| [089-03-container-diff.py](089-03-container-diff.py.txt) | 3493 | - |
| [089-03-common.py](089-03-common.py.txt) | 8154 | - |
| [089-01-tables.py](089-01-tables.py.txt) | 15442 | - |
| [085-04-window.py](085-04-window.py.txt) | 32322 | - |
| [085-04-run.ps1](085-04-run.ps1.txt) | 8486 | - |
| [085-04-oracle.py](085-04-oracle.py.txt) | 97319 | 085-04 texture-corruption mode oracle (m53 DC plan section 6, S2 plan R2.1). |
| [085-04-m53rows.py](085-04-m53rows.py.txt) | 97593 | - |
| [085-04-lib.py](085-04-lib.py.txt) | 26018 | - |
| [085-04-legs.py](085-04-legs.py.txt) | 13021 | - |
| [085-04-gateselftest.py](085-04-gateselftest.py.txt) | 19580 | 085-04 offline gate selftest: every B-M53 gate proven both ways before any leg runs (no game, no bench). Writes 085-04-evidence/gateselftest.json; exit 0 only if every check passes. |
| [085-04-eval.py](085-04-eval.py.txt) | 31299 | - |
| [085-04-common.py](085-04-common.py.txt) | 8960 | - |
| [085-03-nd4-preread.py](085-03-nd4-preread.py.txt) | 2193 | 085-03 offline ND-4 pre-read (no launch): the S1 bank's MainWorld census (M53S1_G0_FMW, S1 build, frame 1, compiled 128 MiB cap) re-read under the DC rules. Under all-or-nothing (DC plan section 7) a decision with 0 < qu |
| [085-03-cook.ps1](085-03-cook.ps1.txt) | 1783 | - |
| [085-03-content-manifest.py](085-03-content-manifest.py.txt) | 7216 | 085-03 content manifest: hash the project content roots (M0 before the DC authoring, M1 after it, M2 after the cook) and diff two manifests under the DC rule: added must be exactly the declared DC files under Content/Cap |
| [085-03-container-diff.py](085-03-container-diff.py.txt) | 2710 | 085-03 container diff: list two cooked pairings with UnrealPak -List (utoc chunks + pak entries) and compare per-entry hashes. Usage: 085-03-container-diff.py <tag> <old_paks_dir> <new_paks_dir> -> 085-03-evidence/contai |
| [085-03-cmdlet.ps1](085-03-cmdlet.ps1.txt) | 1175 | - |
| [084-09-window.py](084-09-window.py.txt) | 24313 | - |
| [084-09-run.ps1](084-09-run.ps1.txt) | 8430 | - |
| [084-09-make-eval.py](084-09-make-eval.py.txt) | 3318 | - |
| [084-09-lodselect.py](084-09-lodselect.py.txt) | 6059 | - |
| [084-09-lodscan-ue.py](084-09-lodscan-ue.py.txt) | 5210 | - |
| [084-09-lib.py](084-09-lib.py.txt) | 20401 | - |
| [084-09-legs.py](084-09-legs.py.txt) | 7458 | - |
| [084-09-gateselftest.py](084-09-gateselftest.py.txt) | 18455 | - |
| [084-09-eval.py](084-09-eval.py.txt) | 68552 | - |
| [084-09-eval-new.py](084-09-eval-new.py.txt) | 19492 | - |
| [084-09-common.py](084-09-common.py.txt) | 7845 | - |
| [084-08b-probe-ue.py](084-08b-probe-ue.py.txt) | 2021 | - |
| [084-08b-lodfix-select.py](084-08b-lodfix-select.py.txt) | 7585 | - |
| [084-08b-lodfix-scan-ue.py](084-08b-lodfix-scan-ue.py.txt) | 4795 | - |
| [084-08b-cook-baseline.py](084-08b-cook-baseline.py.txt) | 1737 | - |
| [084-08b-container-diff.py](084-08b-container-diff.py.txt) | 3369 | - |
| [084-06-window.py](084-06-window.py.txt) | 22415 | - |
| [084-06-run.ps1](084-06-run.ps1.txt) | 8430 | - |
| [084-06-proofs.py](084-06-proofs.py.txt) | 15210 | - |
| [084-06-lyra.ps1](084-06-lyra.ps1.txt) | 8269 | - |
| [084-06-lib.py](084-06-lib.py.txt) | 18389 | - |
| [084-06-legs.py](084-06-legs.py.txt) | 7103 | - |
| [084-06-gateselftest.py](084-06-gateselftest.py.txt) | 9915 | - |
| [084-06-eval.py](084-06-eval.py.txt) | 48448 | - |
| [084-06-common.py](084-06-common.py.txt) | 6951 | - |
| [084-03-window.py](084-03-window.py.txt) | 30654 | - |
| [084-03-run.ps1](084-03-run.ps1.txt) | 8430 | - |
| [084-03-lib.py](084-03-lib.py.txt) | 55348 | - |
| [084-03-gateselftest.py](084-03-gateselftest.py.txt) | 21200 | - |
| [084-03-common.py](084-03-common.py.txt) | 6618 | - |
| [082-14-s20-window.py](082-14-s20-window.py.txt) | 2517 | - |
| [082-14-s20-tokens.py](082-14-s20-tokens.py.txt) | 4264 | - |
| [082-14-s20-synth.py](082-14-s20-synth.py.txt) | 10935 | - |
| [082-14-s20-preflight.py](082-14-s20-preflight.py.txt) | 3219 | - |
| [082-14-s20-postflight.py](082-14-s20-postflight.py.txt) | 2570 | - |
| [082-14-s20-lib.py](082-14-s20-lib.py.txt) | 39084 | - |
| [082-14-s20-common.py](082-14-s20-common.py.txt) | 2316 | - |
| [082-07-window.py](082-07-window.py.txt) | 1525 | - |
| [082-07-run.ps1](082-07-run.ps1.txt) | 8337 | - |
| [082-07-preflight.py](082-07-preflight.py.txt) | 2974 | - |
| [082-07-postflight.py](082-07-postflight.py.txt) | 2791 | - |
| [082-07-lib.py](082-07-lib.py.txt) | 95180 | - |
| [082-07-legs.py](082-07-legs.py.txt) | 12685 | - |
| [082-07-common.py](082-07-common.py.txt) | 5422 | - |
| [081-44-stage.py](081-44-stage.py.txt) | 4994 | - |
| [081-44-build.ps1](081-44-build.ps1.txt) | 907 | - |
| [081-37-cost-window.py](081-37-cost-window.py.txt) | 5703 | - |
| [081-37-cost-sampler.py](081-37-cost-sampler.py.txt) | 12460 | - |
| [081-37-cost-lib.py](081-37-cost-lib.py.txt) | 43816 | - |
| [081-23-window.py](081-23-window.py.txt) | 1555 | - |
| [081-23-wait.py](081-23-wait.py.txt) | 1392 | - |
| [081-23-preflight.py](081-23-preflight.py.txt) | 2226 | - |
| [081-23-postflight.py](081-23-postflight.py.txt) | 2658 | - |
| [081-23-nohold-run.ps1](081-23-nohold-run.ps1.txt) | 27689 | - |
| [081-23-measure.ps1](081-23-measure.ps1.txt) | 12504 | The bench's foreground-focus gate requires a visible game window. |
| [081-23-lyra-stage.py](081-23-lyra-stage.py.txt) | 1824 | - |
| [081-23-lyra-run.ps1](081-23-lyra-run.ps1.txt) | 13000 | The bench's foreground-focus gate requires a visible game window. |
| [081-23-lyra-restore.py](081-23-lyra-restore.py.txt) | 1879 | - |
| [081-23-lyra-pre.py](081-23-lyra-pre.py.txt) | 668 | - |
| [081-23-lib.py](081-23-lib.py.txt) | 129482 | - |
| [081-23-dryrun.py](081-23-dryrun.py.txt) | 22216 | - |
| [081-23-derive.py](081-23-derive.py.txt) | 4486 | - |
| [081-23-common.py](081-23-common.py.txt) | 3957 | - |
| [081-23-chunk3.py](081-23-chunk3.py.txt) | 1240 | - |
| [081-23-chunk2.py](081-23-chunk2.py.txt) | 898 | - |
| [081-23-chunk1.py](081-23-chunk1.py.txt) | 1381 | - |
| [081-23-chunk0.py](081-23-chunk0.py.txt) | 1236 | - |
| [081-23-c0-part2.py](081-23-c0-part2.py.txt) | 5927 | - |
| [081-23-c0-compare.py](081-23-c0-compare.py.txt) | 9819 | - |
| [081-23-c0-analyze.py](081-23-c0-analyze.py.txt) | 18868 | - |
| [081-23-b1.py](081-23-b1.py.txt) | 530 | - |
| [081-22-wait.py](081-22-wait.py.txt) | 1392 | - |
| [081-22-start.py](081-22-start.py.txt) | 5238 | - |
| [081-22-part2.py](081-22-part2.py.txt) | 5927 | - |
| [081-22-oracle-p2.py](081-22-oracle-p2.py.txt) | 1193 | - |
| [081-22-measure.ps1](081-22-measure.ps1.txt) | 12441 | The bench's foreground-focus gate requires a visible game window. |
| [081-22-lyra-stage.py](081-22-lyra-stage.py.txt) | 1824 | - |
| [081-22-lyra-run.ps1](081-22-lyra-run.ps1.txt) | 12937 | The bench's foreground-focus gate requires a visible game window. |
| [081-22-lyra-restore.py](081-22-lyra-restore.py.txt) | 1879 | - |
| [081-22-lyra-pre.py](081-22-lyra-pre.py.txt) | 661 | - |
| [081-22-legacy.py](081-22-legacy.py.txt) | 18440 | - |
| [081-22-legacy-selftest.py](081-22-legacy-selftest.py.txt) | 3791 | - |
| [081-22-legacy-run.ps1](081-22-legacy-run.ps1.txt) | 27708 | - |
| [081-22-f1.ps1](081-22-f1.ps1.txt) | 6826 | - |
| [081-22-derive.py](081-22-derive.py.txt) | 3988 | - |
| [081-22-compare.py](081-22-compare.py.txt) | 9816 | - |
| [081-22-common.py](081-22-common.py.txt) | 3768 | - |
| [081-22-close.py](081-22-close.py.txt) | 5069 | - |
| [081-22-b1.py](081-22-b1.py.txt) | 523 | - |
| [081-22-b.py](081-22-b.py.txt) | 4461 | - |
| [081-22-analyze.py](081-22-analyze.py.txt) | 18863 | - |
| [081-22-a.py](081-22-a.py.txt) | 9286 | - |
| [081-21-start.py](081-21-start.py.txt) | 4475 | - |
| [081-21-stage.py](081-21-stage.py.txt) | 3858 | - |
| [081-21-lyra-restore.py](081-21-lyra-restore.py.txt) | 2081 | - |
| [081-21-idle.py](081-21-idle.py.txt) | 634 | - |
| [081-21-common.py](081-21-common.py.txt) | 2652 | - |
| [081-21-close.py](081-21-close.py.txt) | 6024 | - |
| [081-21-build.ps1](081-21-build.ps1.txt) | 1332 | - |
| [081-18-start.py](081-18-start.py.txt) | 4886 | - |
| [081-18-stack.py](081-18-stack.py.txt) | 4099 | - |
| [081-18-measure.ps1](081-18-measure.ps1.txt) | 11874 | The bench's foreground-focus gate requires a visible game window. |
| [081-18-lyra.py](081-18-lyra.py.txt) | 4422 | - |
| [081-18-lyra-stage.py](081-18-lyra-stage.py.txt) | 2109 | - |
| [081-18-lyra-run.ps1](081-18-lyra-run.ps1.txt) | 12385 | The bench's foreground-focus gate requires a visible game window. |
| [081-18-lyra-restore.py](081-18-lyra-restore.py.txt) | 2843 | - |
| [081-18-lyra-pre.py](081-18-lyra-pre.py.txt) | 661 | - |
| [081-18-derive.py](081-18-derive.py.txt) | 3196 | - |
| [081-18-compare.py](081-18-compare.py.txt) | 9816 | - |
| [081-18-common.py](081-18-common.py.txt) | 3268 | - |
| [081-18-close.py](081-18-close.py.txt) | 6345 | - |
| [081-18-b1.py](081-18-b1.py.txt) | 523 | - |
| [081-18-analyze.py](081-18-analyze.py.txt) | 18863 | - |
| [081-17-suite.py](081-17-suite.py.txt) | 7646 | - |
| [081-17-start.py](081-17-start.py.txt) | 4078 | - |
| [081-17-stage.py](081-17-stage.py.txt) | 3529 | - |
| [081-17-measure.ps1](081-17-measure.ps1.txt) | 10666 | The bench's foreground-focus gate requires a visible game window. |
| [081-17-lyra.py](081-17-lyra.py.txt) | 7642 | - |
| [081-17-lyra-run.ps1](081-17-lyra-run.ps1.txt) | 11177 | The bench's foreground-focus gate requires a visible game window. |
| [081-17-lyra-restore.py](081-17-lyra-restore.py.txt) | 2143 | - |
| [081-17-lyra-pre.py](081-17-lyra-pre.py.txt) | 661 | - |
| [081-17-close.py](081-17-close.py.txt) | 5665 | - |
| [081-17-build.ps1](081-17-build.ps1.txt) | 1096 | - |
| [081-17-b1.py](081-17-b1.py.txt) | 523 | - |
| [081-16-close.py](081-16-close.py.txt) | 9169 | - |
| [081-16-audit.py](081-16-audit.py.txt) | 8699 | Pre-build source audit. No Unreal launch, build, or candidate mutation. |
| [081-15-suite.py](081-15-suite.py.txt) | 12718 | - |
| [081-15-start.py](081-15-start.py.txt) | 3129 | - |
| [081-15-stage.py](081-15-stage.py.txt) | 2135 | - |
| [081-15-report.py](081-15-report.py.txt) | 25324 | - |
| [081-15-prepare.py](081-15-prepare.py.txt) | 11548 | Dated amendment BEFORE build and new captures |
| [081-15-nohold.py](081-15-nohold.py.txt) | 6370 | - |
| [081-15-measure.ps1](081-15-measure.ps1.txt) | 10666 | The bench's foreground-focus gate requires a visible game window. |
| [081-15-lyra.py](081-15-lyra.py.txt) | 4973 | - |
| [081-15-lyra-run.ps1](081-15-lyra-run.ps1.txt) | 11177 | The bench's foreground-focus gate requires a visible game window. |
| [081-15-lyra-restore.py](081-15-lyra-restore.py.txt) | 1780 | - |
| [081-15-lyra-pre.py](081-15-lyra-pre.py.txt) | 661 | - |
| [081-15-legacy.py](081-15-legacy.py.txt) | 5662 | - |
| [081-15-legacy-run.ps1](081-15-legacy-run.ps1.txt) | 27592 | - |
| [081-15-finish.py](081-15-finish.py.txt) | 7316 | - |
| [081-15-capture-pose.py](081-15-capture-pose.py.txt) | 836 | - |
| [081-15-build.ps1](081-15-build.ps1.txt) | 1096 | - |
| [081-15-b1.py](081-15-b1.py.txt) | 523 | - |
| [081-15-b1-core.py](081-15-b1-core.py.txt) | 2918 | A missing target must not silently disappear from the modal denominator. |
| [081-14-start.py](081-14-start.py.txt) | 2713 | - |
| [081-14-report-finalize.py](081-14-report-finalize.py.txt) | 23298 | - |
| [081-14-nohold.py](081-14-nohold.py.txt) | 6257 | - |
| [081-14-lyra.py](081-14-lyra.py.txt) | 4460 | - |
| [081-14-lyra-stage.py](081-14-lyra-stage.py.txt) | 1677 | - |
| [081-14-lyra-run.ps1](081-14-lyra-run.ps1.txt) | 11177 | The bench's foreground-focus gate requires a visible game window. |
| [081-14-lyra-restore.py](081-14-lyra-restore.py.txt) | 1780 | - |
| [081-14-lyra-pre.py](081-14-lyra-pre.py.txt) | 661 | - |
| [081-14-finish.py](081-14-finish.py.txt) | 5962 | - |
| [081-13-start.py](081-13-start.py.txt) | 2639 | - |
| [081-13-nohold.py](081-13-nohold.py.txt) | 6124 | - |
| [081-13-nohold-run.ps1](081-13-nohold-run.ps1.txt) | 27573 | - |
| [081-13-finish.py](081-13-finish.py.txt) | 5857 | - |
| [081-13-cause-read.py](081-13-cause-read.py.txt) | 4220 | - |
| [081-12-suite-v2.py](081-12-suite-v2.py.txt) | 7803 | - |
| [081-12-start.py](081-12-start.py.txt) | 2285 | - |
| [081-12-measure-v2.ps1](081-12-measure-v2.ps1.txt) | 10630 | The bench's foreground-focus gate requires a visible game window. |
| [081-12-legacy.py](081-12-legacy.py.txt) | 5697 | - |
| [081-12-legacy-run.ps1](081-12-legacy-run.ps1.txt) | 27592 | - |
| [081-12-finish.py](081-12-finish.py.txt) | 5763 | - |
| [081-12-b1.py](081-12-b1.py.txt) | 524 | - |
| [081-11-validate.py](081-11-validate.py.txt) | 4270 | Control B1 failures are observations, never a failure of the required ON matrix. |
| [081-11-validate-v2.py](081-11-validate-v2.py.txt) | 4284 | Control B1 failures are observations, never a failure of the required ON matrix. |
| [081-11-suite-v2.py](081-11-suite-v2.py.txt) | 7913 | - |
| [081-11-static.py](081-11-static.py.txt) | 3317 | - |
| [081-11-start.py](081-11-start.py.txt) | 2212 | - |
| [081-11-stage.py](081-11-stage.py.txt) | 2137 | - |
| [081-11-preflight.ps1](081-11-preflight.ps1.txt) | 7759 | The bench's foreground-focus gate requires a visible game window. |
| [081-11-measure.ps1](081-11-measure.ps1.txt) | 10308 | The bench's foreground-focus gate requires a visible game window. |
| [081-11-measure-v2.ps1](081-11-measure-v2.ps1.txt) | 10630 | The bench's foreground-focus gate requires a visible game window. |
| [081-11-legacy.py](081-11-legacy.py.txt) | 4901 | - |
| [081-11-legacy-run.ps1](081-11-legacy-run.ps1.txt) | 26813 | - |
| [081-11-finish.py](081-11-finish.py.txt) | 6922 | - |
| [081-11-build.ps1](081-11-build.ps1.txt) | 1096 | - |
| [081-11-b1.py](081-11-b1.py.txt) | 524 | - |
| [081-10-validate.py](081-10-validate.py.txt) | 3873 | Control B1 failures are observations, never a failure of the required ON matrix. |
| [081-10-start.py](081-10-start.py.txt) | 6265 | Predictions before implementation/build/validation |
| [081-10-stage.py](081-10-stage.py.txt) | 2137 | - |
| [081-10-report.py](081-10-report.py.txt) | 20261 | Outcome — first enabled preflight refused, matrix stopped |
| [081-10-prepare.py](081-10-prepare.py.txt) | 2685 | Stage helper: unique module archive key comes from the changed fixture DLL, not unchanged Capture DLL. |
| [081-10-preflight.ps1](081-10-preflight.ps1.txt) | 7502 | The bench's foreground-focus gate requires a visible game window. |
| [081-10-finish.py](081-10-finish.py.txt) | 11427 | - |
| [081-10-build.ps1](081-10-build.ps1.txt) | 1096 | - |
| [081-09-start.py](081-09-start.py.txt) | 6296 | Prediction and recipe before any launch |
| [081-09-report.py](081-09-report.py.txt) | 20728 | Final diagnosis — NEEDS-DECISION, no correction branch selected |
| [081-09-preflight.ps1](081-09-preflight.ps1.txt) | 6361 | The bench's foreground-focus gate requires a visible game window. |
| [081-09-finish.py](081-09-finish.py.txt) | 9231 | - |
| [081-09-diagnose.py](081-09-diagnose.py.txt) | 5124 | A missing target must not silently disappear from the modal denominator. |
| [081-08-write-docs.py](081-08-write-docs.py.txt) | 5993 | 2026-09-21 outcome amendment — bounded qualification halt |
| [081-08-suite.py](081-08-suite.py.txt) | 3028 | - |
| [081-08-stage.py](081-08-stage.py.txt) | 2089 | - |
| [081-08-prepare-runners.py](081-08-prepare-runners.py.txt) | 2341 | - |
| [081-08-pose.py](081-08-pose.py.txt) | 2754 | - |
| [081-08-lyra-suite.py](081-08-lyra-suite.py.txt) | 4092 | - |
| [081-08-legacy.py](081-08-legacy.py.txt) | 4883 | - |
| [081-08-l2.py](081-08-l2.py.txt) | 2737 | Check the entire flushed/final log by timestamp, including buffered lines after the marker. |
| [081-08-integrate.py](081-08-integrate.py.txt) | 3231 | if !UE_BUILD_SHIPPING |
| [081-08-finish-boundary.py](081-08-finish-boundary.py.txt) | 7659 | All restore inputs verified before either checkout is changed. |
| [081-08-edit-stage.py](081-08-edit-stage.py.txt) | 3751 | - |
| [081-08-current.py](081-08-current.py.txt) | 5273 | - |
| [081-08-close-read.py](081-08-close-read.py.txt) | 4104 | Retire an unused invalid recipe and its private B1 exception; original runner is untouched. |
| [081-08-build.ps1](081-08-build.ps1.txt) | 1095 | - |
| [081-08-boundary.py](081-08-boundary.py.txt) | 5878 | - |
| [081-08-audit.py](081-08-audit.py.txt) | 7320 | - |
| [081-07-inspect.py](081-07-inspect.py.txt) | 2789 | Freeze whole authoritative source scopes, so a negative finding is reviewable. |
| [081-07-finalize.py](081-07-finalize.py.txt) | 4866 | - |
| [081-07-boundary.py](081-07-boundary.py.txt) | 5877 | - |
| [081-06-suite.py](081-06-suite.py.txt) | 3888 | - |
| [081-06-restore.py](081-06-restore.py.txt) | 2192 | - |
| [081-06-postflight.py](081-06-postflight.py.txt) | 2083 | - |
| [081-06-pose.py](081-06-pose.py.txt) | 2663 | - |
| [081-06-ledger.py](081-06-ledger.py.txt) | 1429 | - |
| [081-06-freeze.py](081-06-freeze.py.txt) | 1174 | - |
| [081-05-tidy-journal.py](081-05-tidy-journal.py.txt) | 802 | - |
| [081-05-stage.py](081-05-stage.py.txt) | 2472 | - |
| [081-05-source-audit.py](081-05-source-audit.py.txt) | 2359 | Preserve all observable assignments and the existing field emit sites exactly. |
| [081-05-snapshot.py](081-05-snapshot.py.txt) | 2457 | Read-only Git-object snapshot for the independent m55 design review. |
| [081-05-restore.py](081-05-restore.py.txt) | 2192 | - |
| [081-05-prepare.py](081-05-prepare.py.txt) | 1732 | - |
| [081-05-prepare-lyra.py](081-05-prepare-lyra.py.txt) | 1758 | Fresh uniquely named outputs only; the original archival move paths are unreachable. |
| [081-05-predecessor-diagnostic.py](081-05-predecessor-diagnostic.py.txt) | 1498 | - |
| [081-05-predecessor-bench.py](081-05-predecessor-bench.py.txt) | 1489 | - |
| [081-05-postflight.py](081-05-postflight.py.txt) | 2071 | - |
| [081-05-matrix.py](081-05-matrix.py.txt) | 2102 | - |
| [081-05-mask-tag-audit.py](081-05-mask-tag-audit.py.txt) | 1562 | - |
| [081-05-lyra-suite.py](081-05-lyra-suite.py.txt) | 1258 | - |
| [081-05-lyra-matrix.py](081-05-lyra-matrix.py.txt) | 2537 | - |
| [081-05-legacy-compare.py](081-05-legacy-compare.py.txt) | 3646 | - |
| [081-05-ledger.py](081-05-ledger.py.txt) | 3207 | - |
| [081-05-freeze.py](081-05-freeze.py.txt) | 1107 | - |
| [081-05-final-suite.py](081-05-final-suite.py.txt) | 512 | - |
| [081-05-final-coverage.py](081-05-final-coverage.py.txt) | 1335 | - |
| [081-05-controls.py](081-05-controls.py.txt) | 3162 | - |
| [081-05-build-v6.ps1](081-05-build-v6.ps1.txt) | 1233 | - |
| [081-05-build-v5.ps1](081-05-build-v5.ps1.txt) | 1203 | - |
| [081-05-build-v4.ps1](081-05-build-v4.ps1.txt) | 1205 | - |
| [081-05-build-rest.ps1](081-05-build-rest.ps1.txt) | 982 | - |
| [081-05-audit.py](081-05-audit.py.txt) | 9408 | Stage 1 artifact/identity audit only; no change arithmetic or event verdict. |
| [081-05-artifact-delta.py](081-05-artifact-delta.py.txt) | 3168 | - |
| [081-04-snapshot.py](081-04-snapshot.py.txt) | 2457 | Read-only Git-object snapshot for the independent m55 design review. |
| [081-02-snapshot.py](081-02-snapshot.py.txt) | 2436 | Read-only Git-object snapshot for the independent m55 design review. |
| [079-15-doc-verification.py](079-15-doc-verification.py.txt) | 2683 | Confirm the regenerated characterisation document differs from 079-14's copy only in the intended ways: the added Run outcome column, the added caveat sentence, the reworded provenance and disposition lines. Every measur |
| [079-14-probe-raw-d.py](079-14-probe-raw-d.py.txt) | 1931 | Print the raw per-pair d series the gate computes, using the gate's OWN machinery, so a NO-TRANSITION / suppressed-peak claim can be checked against numbers rather than inferred. |
| [079-14-mutation-probe.py](079-14-mutation-probe.py.txt) | 4071 | 079-14 mutation probe: does the 94-case selftest + 22 contracts actually PIN the rules they claim to? Each mutation is applied to a COPY in scratch; the branch is never touched. |
| [079-14-fixtures3.py](079-14-fixtures3.py.txt) | 2262 | 079-14 round 3 — the two coverage rules the README states explicitly: * "Corrupt masks and invalid dimensions are errors, not permission to extrapolate." * "An explicit absent ID is an error inside the span; outside it,  |
| [079-14-fixtures2.py](079-14-fixtures2.py.txt) | 3942 | 079-14 round 2 — probes aimed at R1 (extrapolated NO-TRACE), F5 (unresolved id), R7 (motion caveat, never a refusal), R2 (lighting caveat on a CONSISTENT run), R3 (thresholded absence) and one exact-arithmetic B2 check. |
| [079-14-fixtures.py](079-14-fixtures.py.txt) | 16947 | 079-14 independent-review fixtures. Written from the 079-14 brief section 1 semantics. |
| [079-13-write-report.py](079-13-write-report.py.txt) | 19195 | Build the journal before commit, or the evidence report after verified restoration. |
| [079-13-update-docs.py](079-13-update-docs.py.txt) | 11391 | Apply bounded UTF-8 byte replacements, preserving unrelated document bytes. |
| [079-13-update-current.py](079-13-update-current.py.txt) | 5929 | Prepend the completed boundary, preserving every prior CURRENT byte. |
| [079-13-source-scope.py](079-13-source-scope.py.txt) | 1864 | Prove unchanged helpers and exact sources used by every final validation. |
| [079-13-prepare-validation.py](079-13-prepare-validation.py.txt) | 1679 | Freeze the exact verifier/scorer and prepare the remaining checks. |
| [079-13-final-validation.py](079-13-final-validation.py.txt) | 1769 | Capture every permanent selftest's complete readings, plus the batch contracts. |
| [079-13-complete-checks.py](079-13-complete-checks.py.txt) | 3211 | 079-13 scored classes, eligibility and CLI contracts. |
| [079-13-boundary.py](079-13-boundary.py.txt) | 5353 | Freeze committed feature artifacts, restore m51 and verify the completed boundary. |
| [079-13-bank-validation.py](079-13-bank-validation.py.txt) | 1595 | Read-only bank validation of the working079-13 implementation. |
| [079-13-audit.py](079-13-audit.py.txt) | 3280 | Check measured banks, frozen-source identity and prior fixture coverage. |
| [079-12-zero-cell-audit.py](079-12-zero-cell-audit.py.txt) | 2377 | Reproduce one measured zero-baseline wrong cell with full peak candidates. |
| [079-12-write-report.py](079-12-write-report.py.txt) | 26086 | Build the completed 079-12 report from retained evidence, never inferred verdicts. |
| [079-12-update-current.py](079-12-update-current.py.txt) | 5786 | Prepend the verified 079-12 handoff without altering earlier CURRENT bytes. |
| [079-12-source-audit.py](079-12-source-audit.py.txt) | 3225 | Prove final calibration behavior equals the frozen sweep apart from wording. |
| [079-12-finish-evidence.py](079-12-finish-evidence.py.txt) | 3411 | Validate final fixture/bank/calibration artifacts and retain comparisons. |
| [079-12-final-validation.py](079-12-final-validation.py.txt) | 1769 | Capture every permanent selftest's complete readings, plus the batch contracts. |
| [079-12-complete-checks.py](079-12-complete-checks.py.txt) | 3145 | 079-12 scored classes, eligibility and CLI contracts. |
| [079-12-boundary.py](079-12-boundary.py.txt) | 5162 | Freeze committed feature artifacts, restore m51 and verify the completed boundary. |
| [079-12-bank-validation.py](079-12-bank-validation.py.txt) | 1595 | Read-only bank validation of the working079-12 implementation. |
| [079-10-write-report.py](079-10-write-report.py.txt) | 16962 | Assemble the completed 079-10 report from retained final evidence. |
| [079-10-final-validation.py](079-10-final-validation.py.txt) | 1769 | Capture every permanent selftest's complete readings, plus the batch contracts. |
| [079-10-complete-checks.py](079-10-complete-checks.py.txt) | 5695 | 079-10 calibration identity equivalence, scored classes and CLI contracts. |
| [079-10-bank-validation.py](079-10-bank-validation.py.txt) | 1595 | Read-only bank validation of the working079-10 implementation. |
| [079-09-write-report.py](079-09-write-report.py.txt) | 19270 | Assemble the implementation report from frozen source and captured readings. |
| [079-09-final-validation.py](079-09-final-validation.py.txt) | 1769 | Capture every permanent selftest's complete readings, plus the batch contracts. |
| [079-09-complete-checks.py](079-09-complete-checks.py.txt) | 4023 | Verify final endpoint hardening is inert on every calibration shadow, plus CLI/scorer contracts. |
| [079-09-bank-validation.py](079-09-bank-validation.py.txt) | 1595 | Read-only bank validation of the working079-09 implementation. |
| [079-08-review-validation.py](079-08-review-validation.py.txt) | 16324 | Third independent review. Frozen candidate; repository and bank reads only. |
| [079-08-original-regressions.py](079-08-original-regressions.py.txt) | 3066 | Run the original review generators against the frozen third-review candidate. |
| [079-08-ordering-probes.py](079-08-ordering-probes.py.txt) | 2065 | Additional D1 checks, declared before execution; no repository or bank writes. |
| [079-08-evidence-summary.py](079-08-evidence-summary.py.txt) | 2183 | Condense the stored independent review evidence without rerunning any tests. |
| [079-07-review-validation.py](079-07-review-validation.py.txt) | 16679 | Independent bff1b00 review: frozen modules, scratch writes only, bank read-only. |
| [079-05-review-validation.py](079-05-review-validation.py.txt) | 8701 | Independent offline review probes. Loads frozen git blobs; writes only beside this file. |
| [078-17-check-boundary.ps1](078-17-check-boundary.ps1.txt) | 6741 | - |
| [078-17-audit-documents.ps1](078-17-audit-documents.ps1.txt) | 3268 | - |
| [078-16-verify-boundary.ps1](078-16-verify-boundary.ps1.txt) | 4548 | - |
| [078-15-offline-analysis.py](078-15-offline-analysis.py.txt) | 6440 | Offline comparisons only. Creates new review evidence; never alters a bank. |
