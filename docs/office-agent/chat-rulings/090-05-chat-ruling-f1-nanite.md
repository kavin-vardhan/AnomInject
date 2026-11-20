# 090-05 — Chat ruling on F1 + Nanite skip + readme + constructor (NEEDS-DECISION): all accepted; Shipping is EXCLUDED by descriptor (not repaired); the office kit must understand `effect_interrupted` before Thursday; F1's cost gets a declared threshold tonight

Date: 2026-09-30 ~16:55 IST · Author: chat Claude · Heads `fix/m52-label-timing` `25f9450`, `feat/m53-uv-normal-corruption` `5cda4e9` · Report `_mailbox\to_chat\090-05-f1-nanite-readme.report.md` (197 min; the session ended at 93 % context)

## Accepted
- **F1.**
  - The label requires the effect installed on that frame, via each anomaly's `IsVisualConditionHeld`. Frames without it get `effect_interrupted`, `labelled:false`, no annotation, no mask. `run_summary` has its own counter, and the rule is `…_v2_effect_installed`.
  - **Tests pass both ways:** Codex's 10/20/40 case; host replace 15, re-install 18; two mutants fail; the label-rule has 27 cases; the exclusion selftest expectation is replaced and stated.
  - **The m53 exclusion's "live":** an interrupted m53 stays live until its retained entry clears. **Accepted.** It is the conservative reading: m52 can't be admitted into a window where an m53 entry still exists.
  - **`IAI.Bench.RawRevertAt`** (not in the brief): **accepted.** It is bench-gated and Shipping-excluded, the lever audit passes, and without it no unattended leg can issue a raw revert at a chosen frame.
  - **G426** (m53's CommitDelay lever no longer works as a can-fail): accepted as a consequence of F1.
- **The Nanite skip.**
  - `IAI.Targets.AllowNanite` (default 0, ini `AllowNaniteTargets`), applied in the auto-pool before the draw, targeted fire, the `ApplyAnomaly` backstop, and m53's decision step `E7N`.
  - `refused_nanite` and `nanite_target_policy` go in `run_summary`. camera_clipping is unaffected.
  - The probe registration avoids a RenderCore dependency (G429).
  - **KAT:** no Nanite = identical draws; 2 of 7 Nanite = same seed stream, the target is index 3 of 5, confirmed by an independent replica; all Nanite = nothing fires; setting 1 = byte-identical to today. 3 mutants fail. The census format is unchanged apart from its own reason.
- **The readme corrections:** all 10 items as listed, including the TAA onset blind spot, "fell below half strength", the exact §8.7a run list, F3's 16 + 1 censored, F4/F5 softening, "any negative = default", the Nanite trade-off, and `effect_interrupted`.
- **D, the constructor:**
  - strict 3 → 0 on both targets, both branches;
  - `DEFINE_VTABLE_PTR_HELPER_CTOR` is gone from the `.gen.cpp`;
  - the normal build has 0 warnings;
  - every changed string is explained.
- **Builds:** fix exe `129E8E53`, m53 exe `97D292F8`. Lever audits 34/34 and 47/47, suites pass, archives re-hashed.
- **090-06 harness:**
  - 22 legs plus the OLD-exe can-fail, about 56 min;
  - selftest 83/83, and the final dry run clean;
  - Part 0 is not runnable (no package with this code; `dbghelp.dll`).
- **Minor, accepted:** the stray BOM in `25f9450`'s subject (cosmetic), and the one directory-scoped `git add` (nothing was swept in).

## Chat's own error
The 090-04 ruling said "the final delivery package (a normal Shipping build) remains the Shipping compile gate." **That was wrong.** The delivery build is Development or Test by the owner's 2026-08-20 ruling, and the checklist says so. So nothing on our path has ever compiled Shipping. Code's code read caught it.

## Rulings
1. **Shipping: EXCLUDE, don't repair.**
   - Every plugin module gets `"TargetConfigurationDenyList": ["Shipping"]` in the `.uplugin`, exactly as `AnomalyBench` already has (so the syntax is proven on 5.1 in our own descriptor).
   - **Why exclude:**
     - a tool that breaks rendering on purpose has no place in a retail build;
     - the owner already ruled delivery builds are Development/Test;
     - repairing ~14 function guards in the capture subsystem days before delivery touches the file every label comes from;
     - exclusion is a descriptor change with no source change.
   - A client who builds their game in Shipping with the plugin enabled then gets a clean build without the plugin, instead of a compile error.
   - **Evidence it must carry (can-fail):**
     - UBT's target description for the Shipping configuration lists the plugin modules **before** and none **after**, for example `-Mode=JsonExport` or the equivalent, with no compile needed;
     - the Development, Test and Editor module lists are unchanged;
     - the normal Development build's code is unchanged, by the string scan.

     A real Shipping compile on the scratch host is **not required**; do it only if it fits by day. The `ANOMALY_CAPTURE=0` code stays as it is (now dead) and G428 records why.
   - **The readme and checklist** say: "The plugin is excluded from Shipping builds by design; capture with Development or Test."
2. **The office kit must understand `effect_interrupted` before Thursday.**
   - Code noticed at 13:48 that `label_sync_check.py` has no reason whitelist and would misread `effect_interrupted` frames and a raw-revert end. The report doesn't close it, and the kit is unchanged since `fee8c7a`.
   - The owner runs this kit on real office content on Thursday, where hosts can swap materials mid-event. So it goes in **090-07**:
     - the reason is recognised;
     - an interrupted event's label end is judged against the picture at the interruption, with the same 0-frames-off rule;
     - a labelled–interrupted–labelled event (re-install) is judged per labelled run, with its gap read back as its own count, never as a failure or as a proof by itself;
     - the READ BACK line gains `interrupted N`, and `OFFICE-CHECK.md` explains it in one line.
   - **Tests both ways:** doctored copies of real sessions in the F1 shapes read 0/0; a mistimed copy fails; a mutant that treats `effect_interrupted` as a TAA flag, or ignores it, fails.
3. **The office log counter as a tool:**
   - `tools/m52_log_counts.py` (stdlib), the same readings as chat's PowerShell function (`_reviews\090-04b-office-m52-log-counts.ps1.txt`), numbers only;
   - a selftest on a synthetic log built from the source's own format strings, plus a mutant;
   - one line on the office card.

   It saves the owner a paste at the office.
4. **F1's cost:** tonight's smoke reads the `Capture(F1): EFFECT-INSTALLED CHECK` line.
   - **PASS:** mean ≤ 0.05 ms per captured frame and max ≤ 0.5 ms.
   - **Above that:** a finding for chat, not a delivery blocker by itself.
5. **Codex re-check** of the 090-05 source (F1 + the Nanite skip) is queued after the smoke, before Friday's merges, following the 084-07b pattern.

## Next
1. **090-07** (by day, compile-only): the Shipping exclusion, the kit's `effect_interrupted` handling, and `m52_log_counts.py`. On the fix branch, then merged into m53. **Its m53 head becomes the Thursday pull target.**
2. **090-06, tonight's smoke** (idle-gated), on the pinned m53 exe `97D292F8`. 090-07 changes no Development code, so the smoke speaks for its head, and 090-07 must prove that with the string scan.
3. **090-08:** the Codex re-check of 090-05.
4. **Thursday:** the owner's office test on the 090-07 head.
