# REPORT for 089-02b-m53-rebench-prep.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 089-02b-m53-rebench-prep.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-30 04:16:40 | finished: 2026-09-30 06:12:21 | duration: 116 min | exit code: 0
context at last turn: 859550 tokens (86% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\089-02b-m53-rebench-prep.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Plain summary for the owner:** The m53 re-bench is ready: the test harness is built, checked and dry-run (no game launch), and the test level is fixed and rebuilt. Two things need a call first. The ruled way of pointing the camera at new MainWorld objects doesn't exist in this build, so the normal-map family has no real-game target. And since about 03:00 this PC refuses to create any file named `dbghelp.dll`, which will also stop the delivery build's packaging.

## 3. G403 and the hosts
- **Parse fixed.** A decision's `final` is now read as `head[:detail]`. On the night's own census logs the rule names SM_Ramp2 (needs 213,949,084 B) and RoomBuilderSquare (219,541,488 B); the old rule names nothing on the same lines. Both are proven in the selftest.
- **The rulings' premise is false: L2 cannot frame MainWorld.** `IAI.Bench.PlaceView` refuses every map except CB_GateLevel and L_ShooterGym (`AnomalyBenchModule.cpp:105-114`) and its pose is a fixed constant (:475-476). So none of the 26 uv / 9 normal whole-world hosts eligible at 128 MiB can be framed. The harness reads this from the `270ed32` source at preflight. Codex had already rejected `BugItGo` as a placement in 081-07.
- **So the declared raise applies, as the ruling ordered for that case.**
  - uv: candidates are SM_Ramp2 and RoomBuilderSquare under a bench-only 256 MiB cap, applied to the uv MainWorld and COST legs only.
  - Two new identity-fire probe legs choose H1/H2; a probe that applied on every decision ranks first.
  - The hosts and cap go into a lock committed on the m53 branch before any gate leg.
  - **Normal has no MainWorld host at any cap.** Its view refusals are `partial_footprint` 3 and `texture_not_parameter` 2, none budget-only, so its 8 MainWorld legs are NOT-RUN-PREMISE.

## 4. Fixture fixes, proofs and the cook
- **P-ORC / G-MODE — cause:** the engine's filmic tonemapper mixes colour channels. uv16's missing cell 13 is on screen, unoccluded and flat, but displays (108,197,196) against the reader's prediction (83,199,192), just outside its tolerance. The interior spread of 4 comes from vignette, dither and bloom. The normal oracle is lit, and its linear readout gets compressed by the curve.
- **P-ORC / G-MODE — fix:**
  - `ShowFlag.Tonemapper 0` on every oracle leg (plus `ShowFlag.Lighting 0` on the normal ones), each read back from the leg's log or the attempt is INVALID.
  - The normal readout becomes `pow(N*0.5+0.5, 2.2)`.
  - Reader `089-04-oracle.py`: places at most a quarter of the cells by the fitted geometry, and no longer crashes when P-ORC fails.
- **P-ORC / G-MODE — offline proof:** the reader selftest passes 39/39. On the night's frames it now finds 16/16 and 64/64 cells, but still FAILs on interior spread, which only the show flag fixes. The show flag cannot be proven offline (commandlet renders are black), so the first premise leg decides.
- **G-BIND:** the cause is the harness recipe, not the fixture. The night's BIND legs lost S1's AA-off arbiter (their logs show TSR and Lumen on), which pushed the uniform-half range from 8 to 17 against a limit of 16. Now:
  - The BIND legs carry the arbiter again.
  - A leg run at any other recipe reads INVALID-RECIPE.
  - Every arbiter leg's settings are read back from its log.
  - Offline, the night's four legs read INVALID-RECIPE and S1's BIND bank reads OK.
- **MIXED:** the ruling's premise was wrong. The colour-only slot already had its colour texture (1 binding, qualified for uv). The refusal came from **slot 0, which carried a leftover component override `BasicShapeMaterial`**, so it resolved to a textureless engine material. Adding a base-colour texture to slot 1, as worded, would not have changed either verdict. The new `dcfix` mode empties that override; slot 1 is unchanged.
- **Content change:** exactly 2 files changed in the D: project (`CB_TexCorruptLevel.umap`, `M_TC_OracleN.uasset`), 0 added, 0 removed. Both were read back in a fresh editor process. CaptureBench commit `e64d6f9` (local-only repo).
- **Cook:** 912 packages, 0 errors, and the cook changed no D: content. Staging then failed three times on `dbghelp.dll` (access denied for that exact name anywhere; any other name copies). The packaging step had already finished with exit 0, so the container was taken from the staging folder.
- **Pairing DC2:** exe `6B671987` (the archived one; the cook relinked the host exe to `ECC3F085`, which is not used) plus utoc `45E2FA86`, ucas `8CA6FF5C`, pak `B35DBE96`, globals `462B8AC6`/`BB05CF99`. Archived in `_binary_baselines\m53dc2-cook-45E2FA86`, 6/6 re-hashed.
  - Map check: all 5 required maps present, and a map that was not cooked reads MISSING.
  - Against `F76F1119`, the only changed chunks are the fixture map, `M_TC_OracleN`, MainWorld and the two shader archives; 3 of 1,084 unnamed chunks were replaced (the new shader code).

## 5. Harness predicates and arms
- **G-LEVER-OFF** now requires: the DISABLED echo (preset 0), no ENABLED line, none of the three levers echoing or acting, `ListAnomalies` printed with no twin, and the lever commands actually in the batch. The night's no-flag log passes; the -IAIBench log and 8 doctored copies fail.
- **G-CENSUS:** one census command per engine frame. The night's host log fails the offline "cold" check (both blocks in one frame); the same log split one frame apart passes. The first dry run also found that the harness grammar rejected the product's valid `reasons=-`; fixed, with a check that reason counts sum to the refused count.
- **Arms:** CD3/RD3 are +1 arms and CD5/RD5 are +3 arms (captured frames), each in both AA conditions. "Proven" means the leg fails, the expected edge is offset by exactly k on at least 4 events, and the other edge is at 0. The 085-04 classifier read E1's histogram as the wrong type and could never report "proven". Under the frozen E1, the night's CD3 proves +1 and RD3 proves end +1.
- **E1:** the frozen `8e48e883…`, loaded only after it and its dependencies match 089-03's freeze.
- **MainWorld nulls:** capture time is relative to capture start, so a null with the same schedule lines up frame for frame. The nulls are 400 frames against 300-frame legs (so they reach the last label + 2 s) and run right after their legs. They feed the wrong-object rule at the same game time, the AA-off arbiter, and a per-frame reading on any E1 failure.

## 6. Legs, predictions, minutes and the dry run
- **Legs:** 57 defined, 44–47 launched (47 if RoomBuilderSquare's probe applies).
  - Premise 4, host probes 4 (2 for normal predicted NOT-RUN), lever-off 1, census 2.
  - Fixture regression 4 (one per mode), G-MODE 6, arms 9, MIXED 2, G-BIND 4.
  - MainWorld 16 (uv 8; normal 8 NOT-RUN), COST 5. Part 0 is not repeated.
- **Key predictions:**
  - Fixture: PASS at +0/+0.
  - P-ORC: all cells found, interior ≤ 2, show flags read back 0.
  - MIXED_N: every decision `partial_footprint:1/2:no_normal_map`; MIXED_UV: APPLIED.
  - Arms: CD3/RD3 at +1 and CD5/RD5 at +3, all failing as required.
  - MainWorld uv: PASS on every judged edge, at least 8 per mode on H1 AA-on, and no blob on the leg that its null lacks.
  - COST: a reading, no threshold.
  - Every leg's prediction is in `_reviews\089-04-README.md` §5.
- **Time:** about 67 min (about 52 of legs, about 11 of parallel evaluation), up to about 80 with a retry and person waits.
- **Proofs:** selftest 68/68 on the frozen bytes; the real (non-stub) preflight reports 0 problems.
- **Dry run** (stubbed, no launch): exit 0, 44 of 44 launches accepted, and the 13 NOT-RUN-PREMISE are exactly the predicted ones. The restore copied the held exe on attempt 3, label-rule passed everywhere, 765 s in total. Every evaluation branch ran on a stand-in that actually contains the effect, as G404 requires.

## 7. Commits pushed
- `feat/m53-uv-normal-corruption` `d690c9b..bf0ac20`: journal `docs/sessions/2026-09-30-089-02b-m53-rebench-prep.md`, status block, gotchas G413–G418. No tag, no merge.
- CaptureBench `e64d6f9` (local-only). `m51`, `master`, tags, `ToCodex\` and `E:\<personal-folder>` untouched; m53 S1 is still staged.

## 8. NEEDS-DECISION
1. **L2 cannot frame MainWorld.** Either accept uv-only MainWorld legs under the 256 MiB raise, with normal real-content legs unproven, or authorise a MainWorld pose for L2. That is a source change and a new build, which this brief ruled out.
2. **MIXED:** the fix is slot 0's override, not a base-colour texture on slot 1. Please accept the re-declared predictions.
3. **`dbghelp.dll` is refused box-wide since about 03:00.** Defender's configuration changed at 02:59 and its real-time protection went off at 03:03; I changed no security setting. Accept DC2's container being taken from staging after a clean packaging step. The owner needs to find the cause before the delivery BuildCookRun.
4. **P-ORC:**
   - The show flags cannot be proven offline; a flag that doesn't take effect makes the leg INVALID, never a pass.
   - Accept the reader's geometry fallback.
   - P-ORC's separation floor (32) equals G-MODE's 32 rule, so there is no margin; I did not change the threshold.
5. **G406 for m53:** the frozen E1 uses game-time alignment only for stuck_low_mip, so for m53 the aligned null feeds the extra rows in §5 rather than E1 itself. Please accept that as the m53 implementation.
api-equivalent cost (not billed on Max): $46.78 | turns: 189 | running total across all briefs: $3,832.92
