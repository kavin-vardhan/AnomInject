# REPORT for 094-42r-europa-guard-routes-fixture-spike.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-42r-europa-guard-routes-fixture-spike.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-03 11:05:29 | finished: 2026-11-03 12:05:12 | duration: 60 min | exit code: 0
context at last turn: 565759 tokens (57% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-42r-europa-guard-routes-fixture-spike.live.txt

---- FINAL MESSAGE (Claude Code) ----
**VERDICT: NEEDS-DECISION**

**For the owner (3 lines):**
- 🔒 The safety lock is in. Before it tags anything, a capture now checks the game for effects that read custom depth, and refuses if it finds one or can't tell. Planted tags only go on our own test-level objects. Default bugs are blinking + LOD, and the home regression test passed.
- ⏸ The home outline test level is built, but its first launch stopped on a safety rule: the `-nowrite` switch was missing, so Unreal wrote a settings file inside the test project. So the outline leak hasn't been reproduced yet, and we haven't seen the lock refuse it.
- 🧭 On the fix route, the source says the engine's existing depth-only shaders can drive a private pass, so no new shader type (and no big recompile) looks necessary. The recompile cost itself is still unmeasured.

**How it ran:** the thread `01a14334` resume worked on the first try (`-OwnerApprovedFullAccess`, 11:11:05 to 11:51:20, exit 0, 2,414 s). Review: `_reviews\094-42r-code-review.md`; Codex's report: `_reviews\094-42r-codex-report.md`; receipts are in `code-receipts-094-42r\`.

**FZ3, read by code path (`d1cdc7b..f7b32f0`, 8 local commits):**
- **Plants only on fixture `EA_` actors:** `AnomalyStreetBench425.cpp:48` requires `-IAIBench`, a recognized fixture map and an `EA_` actor tag. In the smoke, the planted actors were exactly EA_BlinkCube, EA_Floor and EA_HideCube; DefaultPawn_0 is no longer planted.
- **Scanner results:** reader, writer, unknown or none. A null blendable or a missing shader map counts as unknown.
- **Refusal before any tag:** `Admit` runs at `Labels425.cpp:89`, before the street-bench plants (`:97`) and before the stencil enable and tagging (`:100`). It logs `C5 refused: host custom-depth <result>` and never touches the host's effect.
- **Limit:** it scans once, at start, so a reader added later is not seen.

**FZ4:** the default is `blinking,lod_corruption`. An explicit list must sit inside the 4.25 pool with no empties and no duplicates, or the capture refuses. Routes are logged and written to `routes.json` and to the labels receipt. The tests cover the default, the explicit four-route list and six bad lists.

**EA_Core smoke, by my own checker (both Paris builds passed on attempt 1):**

| Gate | Reading |
|---|---|
| Frames | 2,352 at 1280×720, contiguous, joined 1:1 to labels |
| Routes | all four routes passed explicitly |
| Fires | blink 22, lighting 13, LOD 19, missing 15 |
| Masks | 731 affected frames all have masks; 1,621 clean frames have none |
| LOD | every one of the 19 events changes at least 14,456 target pixels per frame |
| Annotation | schema 2.1; far plane 1,000,000; `coverage_pct` valid (−1 only on the 13 light-target events, which are marked unmeasured) |
| Census and depth | lost / no-pass / polluted / exhausted all 0; tag 203 never allocated; host components untouched; depth restored exactly |
| Scanner | `none`, no bypass, 0 tags |
| View and UI | player view on 2,352 of 2,352 frames; UI marker absent; 8 of 8 controls positive |
| Exit | clean exit; 0 upload hits |

**The fixture:**
- Path: `E:\IA_Fixtures\Outline425\`, outside every protected tree. Authoring script `author_outline425.py`, SHA-256 `6E4174A1…44891C`.
- Builds: Editor needed 2 attempts (one fixture-only compile error), Game 1.
- **Repro gate: NOT MEASURED. Guard gate: NOT MEASURED.** The authoring launch had no `-nowrite` (the Paris smoke has it). The engine wrote `Saved\Config\Windows\Engine.ini` (1,502 B) and the kit stopped it with FB2 (exit 125, no crash). No fixture assets exist.

**The spike:**
- **(a):** I opened all 39 cited ranges and every claim holds. The depth-only route is **buildable by source with no new shader type**:
  - opaque materials without WPO draw with the default material's position-only VS; masked and WPO materials draw with their own `TDepthOnlyVS<false>`;
  - the lookup goes through the exported `FindShaderTypeByName`, and the bindings are inline in the private header;
  - it has not been compiled. One link risk remains (the shaders' unexported `StaticType`), and there is an 8-bit ID cap per pass.
- **(b):** NOT RUN. No spike branch exists, so nothing was merged or pushed.

**Safety: none.**
- No crash folder; 0 upload hits in all four session logs.
- Paris Content, Config, descriptor and `.uproject` are hash-equal by my own hashes.
- `m51` equals P9 at start and at exit.
- Binaries changed only while UBT ran.
- Nothing was deleted.
- `origin/feat/europa-port` is still `d1cdc7b` (checked with `ls-remote`); local is `f7b32f0`, unpushed.

**Decisions chat must make:**
1. **FF2 for the fixture:** rule that the fixture's own first-launch `Engine.ini` does not count against the Paris host, and stays as preserved evidence. Then authorize a continuation that adds `-nowrite` to the fixture launches, with fresh slots for authoring, repro, guard and FZ6(b). In 4.25 `-nowrite` blocks only config writes (`ConfigCacheIni.cpp:1478`), so the authoring script can still save its assets.
2. **FZ6(b)'s build budget:** the spike needs a fixture Editor rebuild on a throwaway branch, and only 1 of 3 Editor attempts is left. Does it get a fresh FM1 budget?
3. **Optional:** the kit's runtime admission now has no deadline (`DEADLINE=None`), so a blocked gate waits without limit, as it did in 094-42. Should it get a bound?
api-equivalent cost (not billed on Max): $15.16 | turns: 116 | running total across all briefs: $5,516.06
