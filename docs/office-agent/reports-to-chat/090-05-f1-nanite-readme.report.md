# REPORT for 090-05-f1-nanite-readme.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 090-05-f1-nanite-readme.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-30 13:19:02 | finished: 2026-09-30 16:36:05 | duration: 197 min | exit code: 0
context at last turn: 929715 tokens (93% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\090-05-f1-nanite-readme.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:** both office hosts should pull **`feat/m53-uv-normal-corruption` at `5cda4e9`** on Thursday; a host that tracks the fix branch pulls `fix/m52-label-timing` at `25f9450`. This round makes texture labels stop the moment the effect disappears and skips Nanite objects by default, so every labelled event has a mask. The second office host's strict build should now read 0 errors. Tonight's bench smoke is ready and was dry-run only. One decision is needed: the plugin cannot currently build in Shipping (item 10).

**3. F1: a texture label ends when the effect ends**
- **Rule:** a FireWindow label (missing_texture, corrupted_texture, uv_corruption, normal_corruption) now requires the box on screen *and* the effect installed on that frame.
  - "Installed" is each anomaly's own `IsVisualConditionHeld`: our material, or m53's MID with its texture bindings, is still on every targeted slot.
  - It is sampled at the same point as the activity bit today, and the sync capture path now samples it too.
- **What an uninstalled frame becomes:** a transition-only entry with `transition_reason: effect_interrupted`, `labelled: false`, not in `annotation.json`, and no mask.
  - `IsFireLabelledThisFrame` follows the same bit, so mask and m26 arms also stop on those frames.
  - `run_summary` gains `label_transition_effect_interrupted_entries`, and the rule name becomes `annotation_membership_per_policy_v2_effect_installed`.
- **m53 exclusion:** an interrupted m53 event still counts as live for the m52/m53 exclusion until its retained entry clears. The rows still carry that entry, and the co-entry rule counts any m53 entry, labelled or not.
- **Cost:** instrumented but not yet measured on a run. It is one map lookup plus a per-slot material read. The `Capture(F1): EFFECT-INSTALLED CHECK …` log line gives calls, total, mean and max, and tonight's smoke will read it.
- **Tests (both branches):**
  - Codex's case (apply 10, raw revert 20, scheduled end 40) now labels 10–19 only, with 20–39 flagged.
  - Host replacement at 15 with re-install at 18 labels 10–14 and 18–24, with 15–17 flagged. The old rule is shown to disagree.
  - Two mutants (ignore the installed check; never flag) both fail.
  - `verify_capture --label-rule` is now 27 cases; `effect_interrupted` on a labelled entry fails.
  - The exclusion selftest's old expectation ("m53 labelled 10–39") is replaced by labelled 10–19 and flagged 20–39, with admission still at 40.
- **Side effects:**
  - `frames_condition_lost` now stays 0 for texture types (the count moves to the new field).
  - m53's CommitDelay bench lever can no longer serve as a can-fail check (G426).
- **New bench lever, not in the brief:** `IAI.Bench.RawRevertAt <si>`. Nothing else could issue `IAI.Revert` at a chosen frame in an unattended leg. It is Shipping-excluded; the lever audit passes.

**4. Nanite skip**
- **Setting:** `IAI.Targets.AllowNanite <0|1|default>`, ini `[AnomalyInjector] AllowNaniteTargets`. The default 0 refuses any target that draws at least one Nanite part, as `nanite_unmaskable`.
- **Where it applies:**
  - **Auto-pool:** in the candidate loop, before the target is drawn. On m53 that is inside `CandidateCount`, before `DrawAttempt`.
  - **Targeted fire:** before the hold draw.
  - **`ApplyAnomaly` backstop:** on m53 it runs before the partner exclusion.
  - **m53 decision tree:** a new step `E7N`, so the office census counts `nanite_unmaskable` as its own reason. The four-line `v1` format is unchanged.
  - `camera_clipping` has no target and is unaffected.
- **Reporting:** `run_summary.refused_nanite` (distinct actors) and `nanite_target_policy`.
- **Predicate:** the capture module's existing Nanite check, registered with the injector as a probe at startup. The engine's own call is protected, and re-implementing it would give the injector a RenderCore dependency (G429).
- **KAT effect** (seed 4242, reusing m53's known answers):
  - With no Nanite candidate, the draws are identical.
  - With 2 of 7 Nanite: still 3 draws and the same seed after each attempt; the target becomes index 3 of 5 (Rock5), confirmed by the independent Python replica.
  - With every candidate Nanite: 1 draw and nothing fires.
  - With the setting at 1: byte-identical to today.
- **Tests:** `target_policy_selftest` 25/0; a KAT on m53's production `DrawAttempt`; 3 mutants (ignore the setting in the decision, ignore it in the filter, ignore Nanite) all fail. The census checker is now 38 cases and passes.

**5. Readme corrections**
- The actual release rule (half-strength edge, 0 frames off, flagged frames excluded), plus what it does not prove, including the TAA onset blind spot.
- stuck_low_mip "the fade ended 4–6 frames after" now reads "fell below half strength 4–6 frames after".
- "Unflagged frames agree pixel for pixel" is removed.
- §8.7a now lists exactly what was run:
  - content, tick order and AA setting per anomaly, all at 1280×720 paced at 30 fps;
  - "AA on" means TSR; plain TAA, other resolutions and the office hosts are not run;
  - synthetic order with AA off was not run (except lod_popping), and the m53 repeat was natural order only.
- F3 counts: 16 complete events plus 1 whose end runs past the capture.
- F4: mode correctness was checked on separate oracle objects, not the timing objects.
- F5: application is all-or-nothing over the *selected* meshes, and the box/mask identify the object, not proof that every pixel changed.
- Any negative transition value means the default.
- Nanite: skipped by default, the setting, and the trade-off (fewer eligible objects on Nanite-heavy content).
- `effect_interrupted` added to the field semantics, the reason tables and the rule name.
- The checklist's strict-include box now reads "0 errors".

**6. D: the vtable-helper constructor, and the Shipping code read**
- **Strict build, 3 → 0 errors on both targets, both branches:** fix `8d78bfc` Game 0 / Editor 0; m53 `2d5a5ea` Game 0 / Editor 0. The "before" is 090-04's runs 2 and 4, on the same source bytes.
- **Generated code:** before, the `.gen.cpp` had `DEFINE_VTABLE_PTR_HELPER_CTOR(UAnomalyCaptureSubsystem)` at line 74. After, neither branch has it; `generated.h` keeps only the `_CALLER` form that calls our constructor.
- **Normal build:** 0 warnings. Every changed UTF-16 string is a new 090-05 string, apart from known one-byte adjacency artefacts; the other differences are relinked code bytes.
- **Shipping-only code read:** 140 branches, 132 OK, 8 RISK (4 distinct problems, each on both branches), all in the Shipping build of `AnomalyCaptureSubsystem.cpp`. Details are in item 10 and in `_reviews\090-05-shipping-code-read.md`.

**7. Builds and archives**

| | Fix branch | m53 |
| --- | --- | --- |
| Head built | `8d78bfc` | `2d5a5ea` |
| Exe | `129E8E53` | `97D292F8` |
| Strict build | PASS, 0 errors | PASS, 0 errors |
| Normal build | 0 warnings | 0 warnings |
| Lever audit | 34/34 | 47/47 |
| C++ suites | all pass, mutants fail | all pass, 28 mutants fail |
| Python suites | 11/11 | 18/18 |
| Archive (6/6 files re-hashed) | `_binary_baselines\m52fix-09005-129E8E53\` | `_binary_baselines\m53-09005-97D292F8\` |

**8. Tonight's smoke (`_reviews\090-06-*`, dry run only)**
- **22 capture legs on the m53 exe `97D292F8`, one binary swap onto the existing m53 container:**
  - FireWindow legs, missing and corrupted texture, AA on and off.
  - m53 fixture, 4 modes, plus the 6 mode-check legs.
  - F1 raw revert and host replacement at session_index 139 (fire 7; the third fire falls in the camera settle).
  - A can-fail leg on the old exe, predicted to FAIL: labels run on while the picture is clean.
  - Nanite skip at the default setting: 0 `reason=nanite` lines, `refused_nanite` > 0, no unmeasured labelled frame. Then `AllowNanite 1`: Nanite events return.
  - stuck_low_mip B9, B0 and the B5 null.
- **Minutes:** 53.25 declared, about 56 with the evaluation time the dry run measured; the target was ≤ 75.
- **Checks:** harness selftest 83/83. The dry run is clean: pins complete, no preflight problems. The only live blocker right now is the idle gate, because a UE 5.7 editor is open.
- **Part 0 cannot run tonight:** no delivery package with this code exists, and staging still fails on `dbghelp.dll`.
- The real bench staging was not touched.

**9. Commits pushed**
- `fix/m52-label-timing`: `8d78bfc` (source, tests, readme, checklist) and `25f9450` (journal, status block, architecture note, gotchas G426–G429).
  - `25f9450`'s subject starts with a stray BOM character (PowerShell wrote it). It is cosmetic and was not rewritten.
- `feat/m53-uv-normal-corruption`: `2d5a5ea` (merge plus the m53 side) and `5cda4e9` (docs merge plus the m53 status block).
- `m51`, `master`, tags, `ToCodex\` and `E:\<personal-folder>` were not touched.
- One slip: I used a directory-scoped `git add` once on m53; nothing untracked was swept in.

**10. NEEDS-DECISION**
1. **Nothing ever compiles Shipping, and Shipping would not compile.**
   - The 090-04 ruling assumed the delivery package is a Shipping build. It is Development or Test by rule (PRE-DELIVERY-CHECKLIST: "Build is Development or Test, not Shipping").
   - In Shipping, about 14 functions in `AnomalyCaptureSubsystem.cpp` sit outside the capture guard but use types and members that exist only inside it. The destructor also deletes two incomplete types.
   - This is pre-existing and on both branches (G428). It does not affect delivery, but a client who builds their own game in Shipping with the plugin enabled would hit a compile error.
   - **Proposed:** wrap those functions in the capture guard (as their neighbours already are), make the two destructor types complete, and prove it with one Shipping compile on a scratch host (about 75 minutes). Alternatively, declare Shipping unsupported in the client readme. Not fixed here, because a fix can't be verified without that compile.
2. **F1's cost** is instrumented but unmeasured; tonight's smoke reports it.

Several claude.ai connectors (Gmail, Calendar, some plugin servers) need authorising in their connector settings, and the pdf-viewer server failed to connect; none was needed here.
api-equivalent cost (not billed on Max): $124.44 | turns: 350 | running total across all briefs: $4,019.37
