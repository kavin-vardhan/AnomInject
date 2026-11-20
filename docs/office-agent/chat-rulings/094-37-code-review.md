# 094-37 — Code's short review (fixes built and tested; cleanup in scope; the smoke never reached its gates: both slots stopped on the 4 GiB commit floor)

**Date:** 2026-11-01, about 22:20 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**Inputs:**
- **Codex's report:** collected byte-identical as `_reviews\094-37-codex-report.md` (sha256 `F1867C65…`). Exit 0 after 14,260 s (17:56 → 21:54), one resume try. Verdict **NEEDS-DECISION**.
- **Commits on `feat/europa-port`:** `cedb505` (native fixes, built), `0284a0d` (kit admission order, tools only), `07c853e` (closeout docs). `Source` is identical at `cedb505` and `07c853e`.
- **My receipts:** `code-receipts-094-37\` (before / after-cleanup / final tree listings, Paris lists, gates, polls) and `D:\IA_Europa\{Receipts,Temp}\CodeReview09437\`.

## 1. The cleanup (FU4) — in scope
- **My before/after listings of all of `D:\IA_Europa` and the relay run folder agree with `removed.tsv` exactly.**
  - 3,008 files and 9,011,502,233 B are gone. Every one is listed, with size and mtime matching my before-list.
  - Every gone file lies in the 74 scope folders (`09403`…`09433c`, `CodeReview09403`…`CodeReview09433c`).
  - 0 files gone outside scope; 0 directories removed; Temp untouched (4,815 files before, 0 missing after).
  - D: 23.62 → 32.02 GiB at the cleanup; the receipt's byte delta equals the removed bytes.
- **Keep-list, verified:**
  - Top-level `P9`: 63 files, hash-equal.
  - `09434`–`09436`: all files equal; their CodeReview folders hash-equal.
  - FH2's 6 probe `.jsonl` files, 09427e's two contact sheets and their indexes, and 09427b's crash analysis with its 51 crash-client files: all present and unchanged.
  - 0 `.json` / `.md` / `.txt` / `.log` files under 1 MiB removed.
- **What went:** pictures 1,821 (50 MB), ledger databases 668 (2.48 GB), archives 354 (216 MB), logs 6 (14 MB), other files ≥ 1 MiB 150 (6.25 GB).
- **My own reference scan** (tools, launch kits, relay folder) found one removed path named in current tools: `09427c\scratch`. It is a write target that must not pre-exist (`runtime_python27_tests.py:142` asserts `not os.path.exists`), so nothing breaks. Every other mention is in historical receipts or `events.jsonl`.
- ⚠ **Three minor notes:**
  - 9 tiny `.bin` stand-ins (905 B in total) fall outside FU4's named classes.
  - 124 files ≥ 1 MiB came out of job-level `…\0940x\P9\entry|exit\` folders. I read FU4's "P9" as the top-level `Receipts\P9`, which is intact.
  - The keep rule followed references transitively and kept whole referenced directories. 32.9 GiB of the 41.3 GiB in scope stays (for example `09403\ledger`, 6.47 GB), so only 8.39 GiB was freed. That is the safe direction, but it frees little.

## 2. The diff against the cited 5.1 code
- **Lost-batch timeout and discard** (`AnomalyMasks425.cpp`, new `AnomalyCensusBatch425.h`) match `AnomalyCensus.cpp:629-699` / `.h:87`:
  - A batch with no result is lost when `Now > Armed + 8`. It is counted, released and requeued, and no verdict changes.
  - A frame with a missing or short mask, or a render-thread rejection (sent through an MPSC queue), is a no-pass: it is released and requeued.
  - A frame with a reserved-range tag outside the allowed set (event, census, host-reserved) is polluted: it is released and requeued.
  - Credit only goes to actors still holding their tag. Resolved frames are processed before the expiry check. In flight stays capped at 2.
  - The focused test is a pure-function unit test (8/9-tick boundary, outcome priority) plus source-text checks of the wiring. **No in-engine dropped-readback can-fail exists.**
- **Far plane:** 0 (infinite) is written as 1,000,000, matching 5.1 (`AnomalyCaptureSubsystem.cpp:594`, `:10291`). One difference: 5.1 writes 1,000,000 unconditionally, while the port keeps a finite view far plane.
- **`coverage_pct` (`2b9dc19`) and G500 (`05c55e2`)** are now built, but no session wrote an annotation or got past the first hold, so neither is shown at runtime.
- **The self-check line** is emitted on the first resolved frame and on frame 300.
  - Measured fields: the viewport client class, size, format, RHI, hook count, fixed-step and synthetic-order flags, and census state with lost / no-pass / polluted counts.
  - Not measured: `tick=OnWorldPostActorTick/LEVELTICK_All` and the `/FSceneViewport` suffix are literal strings, and `pre_ui=1` only means "the hook fired" (a route claim; FO2's pixels are the proof on stock 4.25).
- **The parity read:** Codex reports the four anomaly `.cpp` files byte-identical to `53bf725`; I confirm the apply and revert paths are shared code. ⚠ **It misses one 4.25-only choice.**
  - `AnomalyAutoInjectorSubsystem.cpp:630-637` passes LOD arg `1` (ForcedLodModel 1 = LOD0) unless the mesh is already forced to LOD0.
  - The 5.1 default with no index is `WorstLodSentinel`, the coarsest LOD (`Anomaly_LodCorruption.cpp:29-32`, `AnomalyLod.cpp:144-152`).
  - On nearby street props, auto-LOD already shows LOD0, so the swap is a no-op. That fits 094-36's 0.004–0.011 % change. **D3 is therefore at least partly 4.25-specific**, and FU3 says to fix such a difference.
  - Blinking, lighting (immediate setters, no ramp in our code) and missing_object match.
- **Kit fix `0284a0d`** moves the gate wait to after setup, with a metadata check across the wait and a full rehash after the run. Thresholds are unchanged. Gap measured 1.38 s.
- **Builds:** Editor 62.7 s and Game 86.7 s, 1 attempt each, exit 0, BelowNormal, 2 actions. 0 C++ errors and 0 C++ warnings (the only two UBT warnings are about the user's `BuildConfiguration.xml` schema). No comments in the 114 added source lines.

## 3. The smoke, from the raw files
| Slot | Launched (IST) | Free commit: admitted → min | Ended | Frames |
|---|---|---|---|---:|
| 01 | 21:14:13 | 9.57 → 1.05 GiB, with D: 37.5 → 21.7 GB in the same 2.4 s | 21:14:24, FH7 owned stop, exit 125 | 0 |
| 02 (retry) | 21:40:00 | 8.73 → 3.67 GiB in 54 s | 21:40:55, FH7 owned stop, exit 125 | 24 |

- **Every FU5 gate is unreached.** Both slots stopped during the first census-off hold, and labels, annotation and the after-depth state were never flushed.
- **No anomaly fired and no mask exists**, so I could not open one per anomaly.
- **What the raw files do show:**
  - 24 PNGs at 1280×720, mean luma 112.4–112.6.
  - 27 evidence planes with 0 mask pixels and stencil values only {0, 203} (203 is the game's own custom-depth actor).
  - The start self-check line, which is sane: `format=18 RHI=D3D11 size=1280x720 PostRenderViewFamily=4 pre_ui=1 fixed=1 synth=0 census=bench_off lost=0 no_pass=0 polluted=0`. Format and RHI equal the slot's runtime log. The frame-300 line was never reached.
- **Why it stopped:** the session's own commit footprint is large.
  - 094-36's passing session started at 12.18 GiB free and bottomed at 7.82, about 4.4 GiB of its own.
  - Admitted at the 8 GiB start line, it is bound to land near the 4 GiB floor.
  - Foreign load came on top: another Claude session's UE 5.7 DCDemo cook/pak, DaVinci Resolve (from 19:20), wlanext 5.9 GB and vmmem 4.1 GB.
  - Free commit read 0.3–1.1 GiB at times, and D: dipped 31.8 → 15.4 GiB (19:40–21:10; not ours, writer not established).

## 4. Safety
- **Crash:** none. Both stops were deliberate owned stops (`crashed=false`), no new crash folder since job start, and no assert or fatal line in either log. Slot 01 never created its runtime log (the process lived about 11 s; slot 02's first log line came 9 s after launch).
- **Upload:** none. The only hits are the crash client's `DataRouterUrl` print (receiver disabled) and the privacy flags on the command line. No map or config save.
- **The Paris host, by my own hashes:**
  - Content (1,456 files, `SampleMap.umap` `9D35E89E…`), plugin Content, Config, the descriptor and the `.uproject`: hash-, size- and mtime-equal.
  - Only the host exe/pdb and `UE4Editor-AnomalyCapture.dll`/`.pdb` changed, all at 19:03–19:05 with our UBT alive.
  - The rebound 27-file manifest matches disk.
- **Git:**
  - `m51` equals P9 at start, prelaunch and exit (HEAD `53bf725`, index `526aba7f`, 15 untracked).
  - The feature branch is clean at `07c853e`, with no origin branch, no push and no tags.
- **Nothing was deleted outside FU4:** my final tree differs from the before-tree only by the 3,008 receipt files.
- **Not written:** the pre-push scan and the office card. Both are conditional on a passing smoke. My scan tooling ran once on `cedb505` and is ready in `code-receipts-094-37\scan\`.
