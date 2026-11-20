# 091-03 — Ruling (ACCEPTED): annotation.json now lists every event's settling frames; temporal detection is fail-safe; the off-window is 40 from measurement; the deferred 091-02 §0 passes. Office pull: `feat/m52-proxy-blur` `d660b5d`

Date: 2026-10-04 ~02:45 IST · Author: Claude Code Desktop (orchestrator/reviewer) · Implementer: Codex `gpt-6-astra` / `max`, full access · Run `_relay\runs\2026-10-03-091-03-annotation-transitions\`. Sessions: 14:00–20:45 (NEEDS-DECISION: gate closed) → resume 1 20:49–21:22 (RED: device removed, environmental) → resume 2 00:42–02:24 (**GREEN**). Rulings 1–2 and resume prompts 1–2 are in the run folder.

**Disposition: ACCEPTED.** Heads pushed by Codex (fast-forward, verified):
- `feat/m52-proxy-blur` `d660b5d` (Source = proved `58386be`, exe `C7FB5212`)
- `feat/m53-uv-normal-corruption` `783a768` (Source = proved `e874e28`)
- `fix/m52-label-timing` `8badc23` (Source = proved `1413a73`)

master `b5f15a3` is untouched.

## Origin
The owner, on the second office host (10-03, `e09fa9c`): about 2 frames after a blurry event's labels end still show the texture reverting, and nothing in annotation.json marks them. His run_summary read 171 transition entries with an off-window of 16. So the frames were flagged per frame in labels.jsonl, but annotation.json had no transition field: a client reading annotation.json alone would take them as clean negatives.

## What changed (reviewed in source: 9 files, +256/−29 on the proxy branch versus `e09fa9c`)
- **annotation.json schema 2.1:** each event carries `event_id`, sorted `transition_frames` (session indices) and `transition_reasons`; the root carries `transition_frame_count` (the union). It is built from the same per-snapshot transition predicates that write labels.jsonl. README §8.7: drop the union; never use as negatives or positives.
- **Fail-safe temporal detection:** the AA method is observed in the capture callback, plus third-party temporal upscaler presence. An unknown or missing view counts as temporal. `IsTemporalMethod` now treats anything other than none/FXAA/MSAA as temporal. Confirmed AA-off runs have 0 temporal frames; independent reasons (`pie_end_settle`) stay, per ruling 1.
- **Off-window default 16 → 40** for every fire-window anomaly under temporal AA (cap 64). Set from the measured E1 t10 tail (36) plus 4. The final legs reach t10 37 at most (TAA PIE), and static hold reaches 25.
- **Cause analysis:** the long faint tail is temporal history, not the source texture re-streaming (a passive residency observer saw the source's resident mips unchanged through the tail).
- **Office kit and readers:** proxy-route PIE settle and fire-window reasons are recognised, and invalid flags still fail.

## My independent checks
| check | result |
|---|---|
| origin heads | as above; master unchanged |
| pushed Source/tools/Config/Content/descriptor versus the proved commits | identical on all three branches (docs-only after the proof) |
| annotation.json versus labels.jsonl, my own script (keyed by `event_id`, `session_index`) over all 16 final legs | **80 events, 0 mismatches, 0 orphan transition events**; tails start exactly one frame after the last labelled frame; staged AA-off lists are empty; PIE AA-off is one `pie_end_settle` per event |
| release archive `09103-58386be-C7FB5212.zip` | SHA-256 matches the report; the 6 binaries match |
| lever audit on the archived exe | 53/53 PASS |

## Proof accepted (Codex's tables, cross-checked by the recount)
- 16 final legs (proxy TSR/TAA/off × PIE/staged; UV/normal TSR PIE+staged; m53 UV/normal AA-off PIE+staged; static hold TSR PIE+staged): **0 unflagged t50 and t10 frames**, 0 missing masks, co-users unchanged.
- **091-02 §0 passes** on the final builds (UV, normal and shared blur, PIE and staged).
- The office kit on the final TSR PIE capture: 8/8, start/end 0.
- Can-fails: the old export, doctored annotation variants, a one-frame label shift, the old 16 window, and the shared-source mutant all fail.

## Accepted deviations
- The m53 AA-off UV/normal legs used the 090-10e successor reader, because 084-09's UV reader banned the PIE flags that ruling 1 requires. The raw edges are 0/0 in both readers, and the formal 084-09 FAIL is retained. Accepted.
- Two existing CVar help strings were updated, which is outside the "fields + log line only" string expectation. Accepted.
- Optional items not run (runtime AA-switch, capture-boundary carry engine controls, UV/normal/staged residency cause checks): covered by pure tests; follow-up.

## Notes
- Resume 1's two DXGI device-removed failures were environmental: the owner's UE 5.7 was holding VRAM on the 8 GB laptop GPU, and the loss came at the first Present, before any plugin work. **With no foreign editor, 16/16 legs ran clean.** Standing rule for bench legs on this PC: **no foreign UnrealEditor open at all.**
- The cost of the 40-frame window: about 1.3 s of frames dropped after each event under temporal AA (none with AA off).

## Next
1. The owner's office check on the second office host, with `d660b5d` (checklist in chat; mirrored in the STATE file).
2. Job E: a read-only Codex review of 090-10b2 → these heads (launched after this ruling).
3. Job F: the extended smoke, merges, package, rehearsal; the Sunday ship/limit/slip call.
