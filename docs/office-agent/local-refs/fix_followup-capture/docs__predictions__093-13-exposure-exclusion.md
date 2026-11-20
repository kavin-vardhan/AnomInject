# 093-13 predictions: exposure reset and scoped J2 qualification

Frozen before product edits or engine legs. Authority: 093-12 ruling AA2-AA5.
Design and exact identity-summary exceptions: docs/sessions/2026-10-16-093-13-exposure-exclusion-design.md.
Baseline eff797c banks and retained captures stay read-only. New evidence: E:/IA_BuildCache/_r9313.

## Order and gates

1. Build all eight normal/strict Editor/Game targets, both fixtures, with zero errors/warnings. Run CPU, source/binary lever and string audits; freeze a new manifest and rehash before every leg.
2. PIE camera reference then active cut/blend, Stack then Lyra, using the exact _r9312 specs: cut at session 60, blend at 300 for 1.5 seconds; config 0 30 180 60 4, expected requested 990. The frozen camera reader gates at least three visible events, edges 0, measured masks on all labelled rows, per-axis PF-2 including moving rows, frame/world identity and W2 accounting. Record dropped pre-switch pending samples, drops per switch, event continuation/end/reasons, and identity-reference change-stage extent_mismatch. The retained _r9312 Stack reference crash is the old-build can-fail; do not rerun it. Existing identity-self/stale-projection copied controls still apply.
3. PIE reach on both fixtures: delayed viewport and resize, exact _r939 specs/readers/controls. Delayed requests/writes 360/360, v>=3, e=0, measured masks, correct view. Resize r=w+named drops; zero accepted stale/mismatched frames, no unexplained/pending result, v>=3, e=0, measured masks and PF-2 agreement on written frames. Only the exposure-exclusion state/count is expected to differ functionally; report the existing log count (it is not currently a JSON field).
4. PIE identity: healthy and native UV regression on both fixtures, exact baseline specs/sessions listed in the design. Healthy 360/360, UV 1152/1152; original visibility/mask/edge gates. Frozen PF-3 comparator plus byte-identical masks, field-identical parsed label rows, summary equality except the design's exact names. Pictures byte-identical is a reading. No label clock/ID normalization; any difference is a STOP. All copied controls must reject while originals rehash unchanged.
5. Dedicated TSR50 PF-2 on both fixtures, PIE; then cook; then all staged cases, including the dedicated TSR50 legs.
6. Only after every gate passes: fast-forward push fix/followup-capture. No other branch, merge or tag.

## Dedicated TSR50 arrangement and estimator (AA4)

Use /Game/CaptureBenchTexCorrupt/CB_TexCorruptLevel, StaticMeshActor_0, uv_corruption tile, 360 requested frames, config 0 30 24 60 4. Use the existing Review938.Projection helper and the same PF-2 target arrangement well inside the picture; do not edit fixture content. Request viewport 1280x720. Set axis 0 (MaintainXFOV) on Stack and axis 1 (MaintainYFOV) on Lyra, process-local only. The helper frames the target using the engine projection. Marker remains OFF as on the retained PF-2 legs.

Recipe: historic host AA/TSR, Lumen diffuse indirect, dynamic GI and reflections; omit all four arbiter overrides. Keep r.DefaultFeature.AutoExposure 0 and r.EyeAdaptationQuality 0. Set r.Editor.Viewport.OverridePIEScreenPercentage 0, r.DynamicRes.OperationMode 0, r.DynamicRes.TestScreenPercentage 0, r.SecondaryScreenPercentage.GameViewport 100, r.ScreenPercentage.Mode 0, r.ScreenPercentage.MinResolution 0, r.ScreenPercentage.MaxResolution 0, r.ScreenPercentage 50. The same recipe applies PIE/staged; no config file changes.

Use the frozen office PF-2 estimator unchanged: measured, paired, dimension/count/bounds-valid masks on labelled non-transition rows with finite projected boxes at least one pixel inside the image. Per axis, require mask-centre displacement from picture centre of at least 5% of that picture dimension. Take the median projected-centre/mask-centre displacement ratio about that same centre, without trimming/refitting. Gate raw X and Y each in [0.990,1.010], with at least 20 distinct qualifying rows each. Fewer than 20 on either axis is a STOP, never a reading/pass. Each axis's 0.96 projected-centre scaling copy must reject. Preserve original inputs. Requested=written=360, every labelled row has a measured mask. The dedicated box test does not redefine temporal-tail visibility at TSR; no AA-off edge threshold is transplanted into it. The other native and camera gates retain their original zero-edge requirements.

Also verify reduced render from captured-frame mask-pass receipts: half-size internal rect on each axis, consistent output picture/mask/label dimensions. Historic 100% log fails the half predicate. This supports the setup, without changing the PF-2 estimator.

## Staged matrix

Use the existing full _r9312 staged matrix and original can-fails on the new candidate: healthy/delayed, resize, same-world ambiguity, split, stereo, all three NotGameView negatives, native UV/proxy regression, synthetic UV/proxy alignment, native/synthetic pairing at 100/50, exact J1 delayed and pre-F1 wrong-view controls, admission probes and camera reference/active legs. Also run both fixtures x both axis constraints at 1280x720 for ordinary PF-2, staged PF-3 where already specified, and both dedicated TSR50 PF-2 legs above.

Every pairing uses the adopted historic recipe and frozen 54928b2e... reader, 40 px tolerance, decidable floor 1, PREVIOUS=NEITHER=0 and shifted-mask rejection. Z2 parts 1, 2 and 4 apply to 50% pairing: every captured mask-pass shows half internal extent; every written image/mask and labels agree on full output size; unchanged pairing/control. Z2 part 3 is retired for pairing by AA4 and replaced by the dedicated non-vacuous box legs. Record coverage and speed_ratio. Any positive PREVIOUS stops without retry. Any NEITHER stops after Z3 spill readings from that same leg, with no new leg.

Cook uses -nodebuginfo only if staged EXE/DLL bytes equal the new candidate manifest. Packaging metadata and marker budgets remain spent. New camera failure is a STOP; do not retry the fix. Identity difference outside named summary fields and TSR per-axis n<20 are STOPs. Other D2 conditions remain: product failure, needed change outside scope, idle deadline 4 hours, security block or disk floor. No predicate/reader/fixture/product changes beyond AA2/AA4 are authorized.
