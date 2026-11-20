# 093-11 pairing recipe: freeze before C1

Candidate eff797c is unchanged on fix/followup-capture. Worktree _r933;
new evidence _r9311; retained evidence _r939. Y1-Y6 authorize harness and
documentation work only. No product source, fixture content, pairing reader,
tolerance or predicate change is authorized. Preflight rehashes both banks,
both restored hosts/content and live staging before the inherited gates stand.

## Recipe and fixed predicate

Attempt 1 of the single "pairing recipe" problem is the J2 recipe minus
`r.Lumen.DiffuseIndirect.Allow 0`, `r.DynamicGlobalIlluminationMethod 0` and
`r.ReflectionMethod 0`. Those three settings retain the host's own values.
This changes one lighting factor group from the failed leg. It applies to
every pairing leg: both fixtures, PIE and staged, native and synthetic,
100% and 50%. All other commands stay frozen, including AA off, auto-exposure
off, marker 1 and every screen-percentage setting. The full command arrays
are recorded before each launch and compared with the retained recipe.

The unchanged reader is CaptureBench/tools/m44_pairing_probe.py, SHA-256
54928b2ecce48d97a1af4bea16b00184ca091e8b22670fe68e8e3165671f3525.
Its centroid tolerance remains 40 px; PREVIOUS = NEITHER = 0 over decidable
frames. The existing wrapper requires CURRENT > 0, hence at least one
decidable frame. There is no larger frozen numerical floor. Same-position
and no-data counts remain readings. Every positive must reject its existing
shifted-mask copy, whose original masks are rehashed unchanged. The 50%
dimension check and requested/written ownership checks are unchanged.

C1 is StackOBot PIE native 100%, CB_GateLevel, blinking on StaticMeshActor_49,
seed 777, 80 requested frames, Config `0 12 12 12 4`, at the amended recipe.
C2 uses the failed leg's exact commands plus **`r.MotionBlurQuality 0`**.
C2 is a reading only, expected to retain NEITHER, and does not consume an
attempt. It runs after C1, unless C1 has PREVIOUS > 0, which stops immediately.
No lighting setting or fixture asset is changed for C2.

## Y2 decision table (verbatim)

| C1 result | Meaning | Action |
|---|---|---|
| PREVIOUS = NEITHER = 0, floor met, shifted-mask control rejects | Lighting was the cause | Adopt the amended recipe for every pairing leg and continue |
| Any PREVIOUS > 0 | A real pairing slip | **STOP** (product gate). Never retried |
| NEITHER > 0, PREVIOUS = 0 | Lighting alone isn't enough | Attempt 2: the historic pairing recipe exactly (no arbiter overrides at all), same leg and control. A pass means adopt it for every pairing leg. Otherwise **STOP** and report C1, C2 and attempt 2 |

Attempt 2 removes all four arbiter overrides, including `r.AntiAliasingMethod 0`,
and leaves those settings at the host's own values; the historic runner also
disabled auto-exposure. The same leg, marker, sample cap, screen-percentage
commands and shifted-mask control remain. A later NEITHER at the adopted
recipe consumes this same two-attempt budget, not a fresh per-host budget.
Any PREVIOUS > 0 in any engine leg, including C2, stops immediately, never
retried. Deliberately shifted-mask copies are negative controls, not engine
legs; their intended rejection does not trigger this stop.

## Readings, never additional gates

Probe coverage is the reader's own bright-magenta pixels inside the current
probe mask divided by that mask's area, median over frames the unchanged
reader calls CURRENT, PREVIOUS or NEITHER. Use its exact RGB tests
`r > 110`, `b > 110`, `g < r - 45`, `g < b - 45`, probe tag 255, and its
two-pixel sampling grid for numerator and denominator. No new brightness
threshold, region crop, colour or outlier rule is introduced. Preserve
per-frame counts and the aggregate; an empty decidable set is unavailable.

speed_ratio is the m51 reading from run_summary.json: first-to-last arm wall
span divided by nominal game span, not game_clock_speed_ratio and not launch
duration. Retain the summary value and source path. If absent or invalid,
report "unavailable"; do not infer it from process duration. Read both
quantities on C1, C2 and the retained failed leg, and coverage on a retained
historic passing pairing bank if present on disk. All originals stay unchanged.

The retained _r939/legs/stack_P_pair_native_100 is labelled
**instrument precondition not met (probe unlit)** under Y1. Its raw reader
output and failed assessment remain intact. This classification does not
turn it into a pass. C1 must establish the lighting factor and pass by running.

## Continuation order and unchanged floors

After the Y2 decision: remaining Stack PIE pairing x3, Lyra synthetic UV/proxy
alignment, Lyra pairing x4, exact J1 delayed/pre-F1 wrong-view engine controls,
supplemental admission probes, then camera switches on both fixtures. The
marker/barcode repair budget is spent: any marker/barcode failure stops with
no third attempt. Only then cook, then every staged gate including four PF-2
and all eight pairing legs at the adopted recipe. No push before all green.

The inherited 32 passes and PF-1 through PF-5 stand only for rehashed identical
candidate bytes. Existing pixel/mask/PF-2 floors, v >= 3 where specified,
0-frame edges, and w = r remain. Resize/camera retain W2: r = w + named drops,
no missing attribution, and zero stale/mismatched accepted samples. Pending
pre-switch samples count as drops. Camera active-event continuation/end and
reasons remain readings. No new product change or loosened gate is permitted.
