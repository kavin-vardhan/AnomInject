# Capture and label integrity: schema 3

Branch: `fix/capture-label-integrity`, based on `21c1e0d` (M50).

This change makes capture artifacts internally auditable and preserves uncertain
measurements. It does **not** certify the client's game. Delivery remains blocked
until a target-game visual comparison passes.

## Consumer contract

- `labels.jsonl` uses `schema_version: 3`; `annotation.json` uses `label_schema: 3`.
- `anomaly_present` and `visible_positive` are true only when an entry has
  `observable: true`. Membership in `anomalies` means an injection existed, not
  that it was visible. `injection_present` distinguishes that membership.
- Every entry has an `event_id`, `target_pixels`, and tri-state `observable`.
  `target_pixels: -1` / `observable: null` means **not measured**, never clean or
  positive. Shader-incomplete frames may have measured geometry pixels but still
  have unknown observability.
- `injected_frames` contains captured frames belonging to the event, including
  inactive phases of a toggling anomaly. `affected_frames` contains only its
  positive subset, including gaps. Empty windows have start/end `-1` and count 0.
- `frame_observations` joins each event to the same per-frame measurements in
  `labels.jsonl`. Invisible and unmeasured events remain in the annotation.
- A `present` mask is saved before its label row is committed. `empty` means a
  successful measurement with no target pixels; `unmeasured` means no valid
  measurement. Both use `mask_file: null`, so consumers must inspect the state.
- `capture_complete`, `requested_frames`, and `written_frame_indices` expose
  missing captures. A failed write or lost frame cannot pass the auditor.
- Event-level `mask.provided` means at least one injected frame has a valid pixel
  measurement. `observability_measured` means every injected frame was measured.

## Capture changes

Game-thread snapshots freeze event membership, conditions, transforms, target
identity, and bounds at world tick end. The eligible viewport's render family
supplies the actual camera projection. Color and mask readbacks carry matching
render-family identities; delayed results never re-read current actor state.
Stale arms are rejected instead of being assigned to a later render.

Stencil ownership is frozen for each rendered batch. Collisions and lost leases
invalidate measurements. Finished events release tag leases without erasing their
history, allowing captures longer than the finite stencil pool. Nanite-containing
actors are conservatively unmeasured on this UE 5.1 measurement path. Translucent-
only targets are excluded by default, including explicit target selection.

The writer processes frames in sequence with at most 8 pending jobs and 256 MiB
of queued raw data. It waits up to 10 seconds for space before rejecting a frame;
one job larger than the budget is rejected immediately. Session finalization joins
the writer before publishing annotations. JSON manifests use temporary files and
rename on success. This is not a filesystem-wide transaction: interrupted writes
can leave partial artifacts, which the auditor rejects.

Material restoration retains strong references through garbage collection.
No-op material swaps and known no-op LOD changes are rejected. Existing shader
prewarming is retained, and incomplete shader maps suppress positive labels.
Manual console/remote injection changes are blocked during capture. The exposure
flag is a luminance heuristic restricted to injection-free windows; it does not
identify the physical cause of a brightness change.

## Supported measurement scope and limits

The validated mask/color pairing path is asynchronous SVE capture from one mono
game viewport at native output resolution. Resized, synchronous, and backbuffer
captures retain images and injection records with unknown observability. Stereo,
multiple-view families, scene captures, and reflection captures cannot consume a
mono capture request. Global camera/time effects have no target-mask evidence
and remain unmeasured.

`observability_basis` is explicitly `target_mask_and_sampled_condition`. A depth-
tested target silhouette plus a held material/hide condition is **not an RGB
counterfactual**. It cannot prove that a material visibly differs from the original,
that a partial material swap affects the measured pixels, or that temporal
reconstruction, translucency, or host post-processing preserves the effect.
Hidden-object masks describe where the removed surface would contribute, not
pixels actually drawn by the missing surface. The pixel threshold is configurable
with `IAI.Capture.ObservableMinPixels`; its default of 1 proves pixel presence,
not human recognizability. These limits require target-game calibration and the
visual gate below; they must not be advertised as solved by code inspection.

The existing custom-depth pass and its depth tolerance remain engine-dependent.
Cooked builds, shader/PSO first use, moving/occluded targets, mixed materials,
host custom-depth consumers, Nanite-heavy maps, and repeated sessions require
target-game validation. Direct host C++ calls that bypass the guarded console and
remote entry points remain the host integrator's responsibility.

## Read-only verification

Requires Python 3.9+, Pillow, and NumPy. Run from the plugin directory:

```powershell
python -m pip install -r tools/requirements-verification.txt
python -B tools/verify_integrity.py <session> --integrity-only
python -B tools/verify_integrity.py <session> --reference <aligned-control-session>
python -B -m unittest discover -s tools/tests -v
```

`Config/FilterPlugin.ini` includes the auditor, its dependencies list, regression
tests, and this document in Unreal Automation Tool `BuildPlugin` exports. Custom
delivery scripts must preserve these files too. No packaged export was built in
this local validation.

The control must have the same image filenames, camera, simulation state, and
render settings, with injection disabled. Adjacent video frames or separately
recorded uncontrolled gameplay are not valid references. Differences within the
target mask (or projected bounds for a zero measurement) are compared against
each measured label. Other simultaneous effects inside that region confound the
comparison. The script cannot establish deterministic alignment itself.

Exit codes: **0** requested checks passed; **1** inconsistency/failure; **2** visual
verification unavailable. Omitting the reference never passes the visual gate.
Unmeasured observations also prevent visual certification. `--integrity-only`
checks files, references, tag counts/ownership, every event's frame observations,
window boundaries, positive flags, and completeness; it is not a delivery pass.

Unreal regression suite: `Automation RunTests AnomalyInjector.CaptureIntegrity`.
It covers pixel layouts, near-plane projection, single-use render keys, stale
arms, tri-state positives, tag retirement, and mask/label write commitment.

## Local validation

- UE 5.1.1 source-engine Development Editor build succeeded.
- Seven Unreal automation tests passed; twelve Python auditor tests passed.
- Existing capture verifier self-tests passed, including the label/pixel gate.
- A 90-frame, four-event missing-texture capture at 640x360 passed the schema-3
  integrity audit, with no missing frames or unmeasured observations.
- A 90-frame, three-event blinking capture passed the integrity audit, retaining
  off phases in injected windows and excluding them from positive windows.
- Both captures passed the existing fixture-specific label/pixel edge gate (seven
  events total, no shifted boundaries or invisible positives detected).
- A 30-frame offscreen test retained the injection through frame 20, ended its
  positive window at frame 11, and measured zero pixels on frames 12 through 20.
- A 30-frame forced-collision test invalidated all 13 contaminated observations
  as `target_pixels: -1` / `observable: null`, retained the event, and passed the
  integrity audit. This validates rejection of contaminated evidence, not visual
  certification of that deliberately broken capture.

The sample project's existing bench map was used only as a test host. No sample
code was changed. No client-game capture or aligned control was available for
this validation, so target-game visual certification remains outstanding.
