# 095-05 — Chat ruling: m58 and m56 plugin code ACCEPTED as written; deviations ruled; next light briefs

**Date:** 2026-11-13, about 17:15 IST · **Author:** chat Claude (orchestrator)
**Inputs:** mailbox reports 095-04e (GREEN, m58 at `a948046`, pushed) and 095-05 (GREEN, 37 min, $28.24 API-equivalent, m56 commits 1 to 4 at `586a0fd`, pushed). Both are Editor-compiled with 0 errors and 0 warnings and pass their pure selftests. **No engine leg has run on either.** The Game target is not built.

## m58 (terrain blink subtype) — deviations accepted
- Two bench levers instead of one (the plan's own C-G2 can-fail needs the second).
- `run_summary.terrain_unlabelable_refusals` added beside `blink_terrain_events`.
- Patterns match an object's own class name only, never parent classes.
- A manual multi-object `IAI.Apply` drops landscape targets and blinks the rest.

## m56 (capture continuity) — deviations ruled
- **Capturer-level drop causes rolled into `unresolved_at_end`: accepted for A-1.** Every missing tick still gets exactly one named cause, and the capturers' own log lines keep the specific reason. Splitting them touches both capturers and their completion queue; it moves to A-2, and only if the gate campaign shows drops that need telling apart.
- `orphan_completions` as its own key; the extra writer keys; `lever_audit.py` left alone; failed mask PNGs counted as unavailable; the teardown flush cap at 120 s: all accepted. Every new key is documented in the readme (next brief).
- **Gotcha numbering:** m58 holds G574, so m56 starts at G575.
- **Noted:** commit 4's compile started under the 4 GiB memory floor because memory fell between the check and the launch. Code's script now re-checks at launch. No action.

## Next (all light; the bench stays closed until the owner frees the PC)
1. **095-05b (xhigh):** m56 commit 5 (the Python readers learn `settle` and schema 2.2, with tests), the CaptureBench scripts (leg predictions, the oracle flank rule, the two new check scripts, each proven both ways offline against a banked release-era session and a doctored copy), and commit 6 (docs).
2. **095-06 (xhigh, fresh side session, read-only):** independent review of both branches against the plans and rulings.
3. **095-07 (high, heavy, on the owner's word):** Game build, one cook, every gate for both units, the settle-exactness table.
