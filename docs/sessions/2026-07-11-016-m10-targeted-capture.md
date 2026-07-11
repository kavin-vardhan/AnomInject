# 016 — m10: Targeted capture modes + pre-run clean slate + entry-point parity (2026-07-11 close; built 2026-07-10)

## Goal
Let a capture run fire ONE chosen anomaly on ONE chosen actor ("targeted" mode) instead of only the
random auto-pool mix; guarantee the captured dataset starts from a clean, fully-labeled state (no
unlabeled contamination from manual injects); and make the console and WS/dashboard entry points
behave identically (pause/resume of the auto-injector, argument surface). Dashboard follows suit:
capture-first UI (Targeted/Auto-pool), manual-inject controls removed.

## What was done (as-built)
Plugin (`feat(capture)` squashed commit, tagged **m10**):
- **`UAnomalyAutoInjectorSubsystem::TryFireSpecific(FName Id, const FString& ActorName)`** — mirrors
  `TryFireOnce` minus random selection: MaxConcurrent + one-instance-per-id (`IsIdLive`) +
  one-anomaly-per-actor guards kept; exact-match targeting via the `=`+name token; records the same
  `FAutoLiveFire` so labels/annotation see targeted fires identically to pool fires.
- **`UAnomalyCaptureSubsystem::StartRun`** gains defaulted `InTargetAnomaly`/`InTargetActor`;
  BOTH set → targeted mode (each burst fires exactly that pair via `TryFireSpecific`); only one set →
  warn + fall back to auto-pool; neither → auto-pool (unchanged). Mode + target recorded in
  `run.json` (`mode`/`target_anomaly`/`target_actor`) and the STARTED log line.
- **Pre-run clean slate:** `StartRun` now calls `Auto->RevertAllLiveFires()` (kept) **plus
  `Injector->RevertAllActive()`** — any manually-injected anomaly (object, component, or global
  scope) is reverted before frame 0, so nothing unlabeled can leak into the dataset (G63).
- **Entry-point parity / centralized pause-resume:** `StartRun` itself pauses the auto-injector's
  Run (replaces the old warn-only) and `FinishRun` resumes it (`bAutoWasRunning`), guarded by
  `bDeinitializing` so PIE teardown never resumes into a dying world (G62). The control server's
  local pause block + Tick resume block are DELETED — the WS path now just calls the 6-arg
  `StartRun` (parity by construction).
- **Console:** `IAI.Capture.Start [outDir] [png|jpeg] [seed] [maxFrames] [anomaly] [targetActor]`
  with `""` placeholders resolving to defaults for leading args (G60 — the tokenizer delivers a
  literal 2-char `""`; a `Slot()` normalizer maps `""`/`''`/empty → default).
- **WS:** `capture_start` accepts optional `anomaly` + `target` fields.

Dashboard (own `feat` commit in the dashboard repo, untagged per m8/m9 precedent):
- CapturePanel: **Targeted / Auto-pool** mode toggle; targeted mode = anomaly dropdown
  (object-scoped catalog entries) + on-screen target picker (shares the preview's selected actor);
  Start disabled until both chosen.
- AutoPanel → **"Capture pool"**: free-run controls (Run/Stop, cadence, fire-once/step dev row)
  removed; the pool checkboxes remain (they feed auto-pool capture) and gray out in targeted mode.
- **InjectPanel + ArgControls deleted** (manual injection UI retired from the dashboard;
  ActivePanel + PreviewCanvas preserved). `store.ts` gains `captureMode` (reset on disconnect).

## Gates (all green)
- **Stage-1 (owner-run, StackOBot):** targeted capture via console fires ONLY the chosen anomaly on
  ONLY the chosen actor — verified with a pool anomaly AND non-pool `lod_popping`; auto-pool path
  unchanged; pause/resume parity confirmed on BOTH console and WS entry points.
- **Contamination test (bridge-run):** manual `missing_object` + manual `time_dilation` applied,
  then targeted capture start → `IAI.DumpActive` = 0 immediately after `StartRun`; artifacts
  (`session_20260710-134342` under `Saved/AnomalyCaptures/contam_test`) show ONLY the targeted
  `blinking`; zero unlabeled contamination.
- **Stage-2 + trims (owner-eyeballed):** mode toggle works; targeted dropdown showed exactly
  `missing_object`/`blinking`/`missing_texture`; free-run controls gone; "Capture pool" relabel;
  Inject/Arg panels gone; ActivePanel + PreviewCanvas intact.
- **Item-2 runtime verify (this close turn, Simulate over the bridge):**
  `IAI.Capture.Start "" png "" 60 blinking SM_Ramp3_UAID_B42E9936F5429ADA00_2086822138` →
  STARTED shows `RunDir = .../Saved/AnomalyCaptures/session_20260711-160022` (NOT `""/session...`),
  `mode=targeted[blinking on SM_Ramp3_...]`, seed auto (-1632304481), frameCap=60; run finished
  60/60 frames (positive=40), `run.json` carries `mode=targeted` + both target fields.

## Problem → Resolution
- **Literal `""` console tokenizer (Item-2, G60):** UE's console tokenizer delivers a quoted empty
  placeholder as the 2-character string `""` (and `''` similarly), not as an empty string — so
  `IAI.Capture.Start "" png ...` produced a literal `""` output dir (`""/session_...`). Fix: the
  `Slot()` normalizer in the Start command maps empty/`""`/`''` to "use default". Root-caused +
  fixed in the build session; runtime-verified green at this close (above).
- **Who pauses the auto-injector?** The m9-era WS handler paused/resumed it locally (control-server
  member + Tick polling) while the console path only warned. Centralizing both into
  StartRun/FinishRun removed the WS-only member/Tick code and gave both entry points identical
  behavior, including the `bDeinitializing` teardown guard (G62).

## Reconciliation record (why this journal closes 2026-07-11, not -10)
The build session (2026-07-10) implemented + gate-verified everything above but its commit turn
never executed — the work sat uncommitted in both working trees (and this journal did not exist,
which is why the next session could not identify the dirty files). Closed this turn as:
- `16a5c19` `docs:` — `capture-fps.md` (the fixed-timestep session's own doc, previously untracked)
  + the CLAUDE.md copy-block report rule.
- `6d4eb01` `fix(capture):` — the **m9-era follow-on** that was also sitting uncommitted:
  client-shaped `affected_frames` object (`{start_frame, end_frame, frame_count, frame_indices}`,
  hide-type fires emit only the observed hidden out-frames; `_debug` block dropped) + seedless
  `session_<ts>` naming with a same-second `-2/-3` disambiguation loop. Attribution evidence: the
  2026-07-08 re-strip session recorded exactly these capture files as pre-existing owner WIP; the
  m9 project notes already describe both behaviors; `git log -S FrameIndices` shows no pushed
  commit contained them.
- the m10 squashed `feat(capture)` commit (this journal + gotchas/architecture/runbook/CLAUDE.md
  updates included), tagged **m10**.
- the dashboard repo's own `feat` commit (Stage-2 UI + trims; the transient `catalogFull`
  experiment netted out to nothing in the final diff).

**Naming correction:** earlier notes/briefs used "m10" as shorthand for the fixed-timestep
capture-fps commit cluster (`c5d58b0`/`500eac7`/`417833a`). No m10 tag ever existed (locally or on
origin) — that cluster is untagged post-m9 work on master, documented in `docs/capture-fps.md`.
**m10 = THIS milestone (targeted capture).** The approved capture-pacing + honest-fps-stamping
plan is **m11**, next turn. That shorthand lived only in session briefs and assistant memory,
never in a repo doc (grep confirms no `.md` here mentions m10 before this journal).

## Deviations
- Gotcha numbering: master jumps G57 → **G60** (G58/G59 are reserved — the unmerged
  `feature/stencil-capture` branch already uses at least G59; append-only means we leave the gap).
- Step-B evidence had to be a fresh run: the 2026-07-10 investigation sessions (R1/R2) never passed
  literal `""` tokens (bare console Start / WS with omitted fields), so they could not prove the
  Slot fix.
- The comment-strip invariant was applied before committing (the WIP files had re-added comments;
  3 files stripped, diff shrank by ~120 comment lines).

## State / Hand-off
- Plugin master: `m9 → 16a5c19 → 6d4eb01 → m10(feat)` — pushed with the m10 tag this turn.
  Dashboard master: one feat commit — pushed this turn. Both trees clean.
- Capture catalog/config surface after m10: targeted or auto-pool bursts; clean-slate start;
  console/WS parity; sessions named `session_<ts>`; `run.json` carries mode/target provenance.
- **Next milestone (m11, plan approved 2026-07-11):** real-time frame pacing during capture
  (`IAI.Capture.Pace`, default ON) + span-based wall/game ratio measurement + one-sided honest
  `video.fps` stamping + dashboard badge + `docs/capture-fps.md` rewrite (two-clock model). See
  the approved plan in the owner's records; implementation starts on these clean trees.
