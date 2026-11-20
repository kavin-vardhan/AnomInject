# 093-06 — Claude Code review of Codex's J2 job (office-robust F1, plus J1 closure Part 0)

**Date:** 2026-10-14, about 11:00 IST · **Author:** Claude Code (mailbox brief `093-06-j2-office-robust-f1.md`), reviewer only.

**Inputs:**
- Codex's report: `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\report.md`. New session, thread `<uuid>`; rollout `turn_context` reads `gpt-6-astra` / `max` / `danger-full-access`. 21,241 s, exit 0, **VERDICT NEEDS-DECISION**. No resume was needed.
- Branch `fix/followup-capture`: local head `bb9e741`, product frozen at `f608d56`. **Not pushed**: the remote is still `1bf7054`.
- Evidence under `E:\IA_BuildCache\_r936`.

**Method:** reading and recounting only. No build, cook, stage, editor or game launch, and no git write. Before launch I took my own baselines: all of `StackOBot\Builds` (5,764 files) with key hashes, both `Content` trees, refs, `ls-remote`, worktree heads, processes, and the StackOBot host and Lyra binaries. I proved the relay guard both ways first (exact brief name exit 0, truncated name exit 6, nothing written). My scripts, baselines and outputs are in `code-receipts\` in the run folder. Every saved session I read has the same file count, byte total and latest mtime before and after my reads.

## Counts
**30 OK · 2 GAP.** One more row is **OPEN** because of the stop (the unrun matrix). One row was read but not re-run (builds and audits).

## Gate table

| # | Item | Codex result | My check | Verdict |
|---|---|---|---|---|
| 1 | Launch and session | New Codex job | Fresh thread; `orchestrator_brief=093-06-j2-office-robust-f1.md watcher_match=True`; model and effort match the request | OK |
| 2 | Predictions before legs | Committed first | Predictions committed in `e13ddc5` (04:58), then amended in `f608d56` (05:45); first engine leg 07:43. The amendment only adds the world-stamp marker and provenance notes. No threshold changed. Blob `c870f03c` is unchanged at `bb9e741` | OK |
| 3 | Part 0: wrong-object can-fail, PIE | Rejected | I re-ran the unchanged J1 `assess.py` / `integrity.py` (hashes `463F3968…` / `C73456E9…`, same as Codex's before/after receipts) into my own scratch folders. Original: exit 0/0, visible, onset/end 0. Wrong-object copy: exit **1/1**, `visible=False`, claimed-target null 0.334 against threshold 4. RGB: 1,152/1,152 files with equal byte totals; 28 sampled frames hash-identical (incl. 191, 200, 238) | OK |
| 4 | Part 0: wrong-object can-fail, staged | Rejected | Same check: original exit 0/0; copy exit **1/1**, `visible=False`; 28/28 sampled frames identical | OK |
| 5 | Part 0: status-block correction | Corrected | `59b2fa1` replaces "All required can-fails were exercised" with a sentence saying J1 omitted the wrong-object can-fail and 093-06 Part 0 closes it | OK |
| 6 | Part 0: masked-alpha limitation | Recorded | G479 and a `client-readme.md` "Known limitations" bullet; both carry the union-of-silhouettes future item. G479 and G480 exist only on `fix/followup-capture`; G481 is on no ref | OK |
| 7 | Part 0: why 3 events, not 4 | `texture_uniform:pending` | `F1_S_proxy/game.log:4936` and `Proxy_S_aaoff_marker0/game.log:4933` both read `TEXCORRUPT-REFUSED … final=texture_uniform:pending step=V1`, then `REFUSED … ordinal=0` | OK |
| 8 | §A3 (a): activation only participates; GT resolution; ambiguity refused; refresh | Done | `IsActiveThisFrame` returns `IsActive()` and only counts a null context world. `RefreshCaptureOwner` runs on GT at `StartRun`, each tick, world-tick-end, frame processing and inside `ArmWanted`/`ArmMask` before every issue. More than one display in the capture world gives `AmbiguousOwner`; more than one local player gives `MultiView` | OK |
| 9 | §A3 (b): frozen identity; family/view bound before dequeue; re-check on RT; change invalidates | Done | `FIdentity` carries run epoch, generation, scene, target, local player, viewport client, view actor, controller id and size. It is frozen per request in the colour capturer and the mask extension; census (`AnomalyCensus.cpp:823`) and probe (`AnomalyMaskMeasure.cpp:516/564`) arm through `ArmMask`; m55 gets `CaptureOwner`. `ConsumeWantedForPublish` needs `FamilyId≠0`, `ViewIndex==0` and the current owner. `SubmitInFlight` re-checks the binding on RT. `f608d56` adds a publication check that colour and mask carry the same family binding. A change invalidates pending colour and mask work, census, m55 epoch, exposure baselines and frozen anchors | OK |
| 10 | §A3 (c): resolved target required; no first-wins | Done | `RejectReason` refuses an unknown target. The fallback is not implemented, so `target_fallback_accepts` cannot be non-zero; that is stricter than the boundary allows | OK |
| 11 | §A3 (d): single view; refusals counted | Done | Code: `MultiView` / `StereoView` / `AmbiguousOwner`. Legs: split 360→0 (`multiple_views=1414`), stereo 360→0 (`stereo_view=728`), two same-world displays 360→0 (`ambiguous_owner=2163`) | OK |
| 12 | §A3 (e): F1 family ids, same-sample expiry, dropped accounting | Kept | `NextId`, `IsSampleFrame` and the unpaired accounting are kept. `744b36d` keeps a rejected readback alive in flight until GPU completion before discarding it | OK |
| 13 | NotGameView by sub-reason, both fixtures | Done | In each forced leg on both fixtures, only its own sub-reason increments and the aggregate equals it: StackOBot 1067/1069/1067, Lyra 1072/1067/1067 (controller / game-thread / show flag). Healthy legs read 0 on both fixtures | OK |
| 14 | `capture_ownership`: additive, `run_summary` only | Done | 23 of 25 sessions carry it; the two without it are the old-binary controls. **0** `labels.jsonl` / `annotation.json` files contain it. The WIP warning now points at the block | OK |
| 15 | "accepted ambiguous" diagnostic | Reads 0 | **Structurally unreachable.** `AcceptedAmbiguous` increments only on an accept, with `bAmbiguous = (Resolution == AmbiguousOwner)`. But `AnomalyCaptureFamily::Reject` returns a non-`None` `Owner.Resolution` first, so an ambiguous owner is never accepted. The field, and the office line's `a=`, cannot show anything but 0 | **GAP** |
| 16 | Office reader, PASS (my run) | — | `stack_P_multi` as case 13 with `--output` to my scratch: `I2.13 PASS r=360 w=360 v=4 e=0 x=0 a=0 #5` (exit 0). Reader SHA-256 `7380E31B…`, as in the report | OK |
| 17 | Office reader, FAIL on the stop session (my run) | — | `lyra_P_selected` as case 11: `I2.11 FAIL r=360 w=360 v=-1 e=-1 x=-1 a=0 #3 I-fail=360,360,385,0,0,3,0,0,0,0,0,0,0,0,0,0`. It fails, but through its exception handler: `unsupported operand type(s) for /: 'WindowsPath' and 'NoneType'`, from `session / row.get('mask_file','')` when a labelled row has `mask_file: null`. Labelled-without-mask is exactly the failure shape that matters, and it prints −1 for visible events, edge and wrong view instead of counting the missing masks. It fails closed | **GAP** |
| 18 | Recount: delayed viewport (`stack_P_delayed`) | 360/360, 4 events, edges 0 | `verify_capture.py --label-pixel-gate` exit 0, output **byte-identical** to Codex's: 4/4 CONSISTENT, 8 edges `delta=+0`. Rows 360 = images, 96 labelled rows each with a present non-zero mask. Ownership: target unknown at start, `owner_updates=1`, generation 2 | OK |
| 19 | Recount: new window + second realtime editor view (`stack_P_multi`) | 360/360, wrong view 0 | Byte-identical reader output; 4/4 CONSISTENT, 8 edges +0; 96 measured masks; `scene_mismatch=2477` non-owner rejections in the block | OK |
| 20 | Recount: J1 regression (`stack_P_reg_uv`) | 1152/1152, edges 0 | Byte-identical reader output; 4/4 CONSISTENT, 8 edges +0; 1,152 rows = images; 192 labelled rows with measured masks | OK |
| 21 | Can-fail: J1 `E41ABF93` on the delayed viewport | Drops all | Ran in PIE with the exact archived J1 editor DLLs; their manifest hashes equal my rehash of the archive (`E176B8E0…` capture, `5171BA2F…` injector). Session: **0 images**, `capture_unpaired_dropped=360`, `SVE-WANT-SUMMARY marksIssued=360 publishesSeen=0`. The J2-only `OWNERSHIP START` line is absent, so old code ran. The staged form (the `E41ABF93` exe itself) was not run (stop) | OK |
| 22 | Can-fail: release `218BA4C7` captures the wrong view | Wrong view on all 360 | Exact release DLLs (`470EBE5F…` / `45A1512F…`, equal to the archive). All 360 pictures are **942×869** (the editor viewport), against J2's 1123×870 PIE window; game barcode missing on all 360; no `OWNERSHIP START` line. Staged form not run (stop) | OK |
| 23 | Resize mid-run | Generation +1, stale rejected | Frames 0–97 at 1123×870, **98–99 dropped**, 100+ at 960×720; 0 row/image and 0 mask/row size mismatches. 360 = 358 + 2. `stale_generation=5`, generation 2, `owner_updates=1`. The log shows a real `Scene viewport resized to 960x720` | OK |
| 24 | Two-client PIE (StackOBot) | Both correct | Client 0 and client 1: 360/360 each, `scene_mismatch` 1015 / 1056, no drops. World-stamp pixels not recounted by me | OK |
| 25 | Lyra selected-viewport stop (D2) | Product gate failed | Verified on the saved session: **96 rows** with `anomaly_present` and `visible_positive` true, `bbox_valid` true at x = 23.55 (w 47.1), `target_pixels 0`, `observable false`, `mask_state empty`, `mask_file null`. `annotation.json`'s four events are `manifested:false` with empty injected/affected sets. The stop rule was applied correctly, and the fixture was not reframed | OK |
| 26 | Engine matrix completeness | 22 PASS · 1 FAIL · 61 not run | All staged legs (both fixtures), the Lyra controls, the remaining Lyra PIE cases, synthetic order and pairing probe are **not run**. The brief's "PIE and staged, both fixtures" is therefore not met | OPEN (stop) |
| 27 | Builds, strict build, CPU tests, string scan, lever audit | All pass | Receipts read, not re-run (no builds allowed for me) | not re-run |
| 28 | Archive | UNQUALIFIED, 470/470 | Manifest SHA-256 `507E2AEF…` as reported; 470 entries plus the manifest. Rehashed EXEs: R3 `4BB0F074…` / `B0F2B30C…`, R2 `3CEBAE22…` / `2F958040…`, R1 `53184E08…` / `19FEAE58…`; capture DLLs `D45CE046…` / `9F0155F8…`, as reported | OK |
| 29 | Live staging | Unchanged | Against my baseline: 5,764 files, 0 added / removed / changed in path, size or mtime; every key exe/container hash identical | OK |
| 30 | Build hosts | Restored | StackOBot plugin worktree detached `23098fa`, Lyra plugin worktree detached `caa68c6`, both clean. All binaries hash-identical. Four files (`StackOBot.exp` and three `UnrealEditor.modules`) have the same content and a moved mtime | OK |
| 31 | Project and plugin content | Unchanged | `StackOBot\Content` 2,743 files and plugin `Content` 2 files: 0 differences | OK |
| 32 | Refs and push | Nothing pushed | `ls-remote` identical to my pre-launch snapshot. Locally only `fix/followup-capture` moved, `1bf7054 → bb9e741` (ancestor check exit 0). `m51`, `master`, `release/2026-10-05` and all tags unchanged. Not pushing was correct: not every gate is green | OK |
| 33 | Processes and safety | Own PIDs only | The only `Stop-Process` in the event stream targets Codex's own `rebuild_harness_r1.py` helper, by PID, after checking its command line. No new engine or Codex processes remain. The owner's Codex desktop processes and `UnrealTraceServer` 42072 are alive. Some owner apps (Claude desktop, `blender-mcp`, Creative Cloud) exited during the six hours; no Codex command touched them. No UE 5.7 or Blender ran. Free space: D: 176.6 GB, E: 163.6 GB | OK |
| 34 | Branch policy (093-02 P10) | Kept | Product changes are confined to `Source/AnomalyCapture` (20 files) plus tools and docs. No natural-cause code, no merge | OK |

## What is wrong, with evidence
1. **GAP (row 15): `accepted_ambiguous` cannot fire.** The ambiguity refusal itself is proven, by `ambiguous_owner=2163` with zero frames and by the first-wins CPU mutant. But the counter that office checkpoint I prints as `a=` is zero by construction, so a `0` there is not a reading.
2. **GAP (row 17): the office reader crashes on a labelled row whose `mask_file` is null.** It still prints FAIL, but through its exception path, with visible / edge / wrong-view as −1 and a Python error as the only issue. On an office host, "labelled frames without masks" would therefore come back without the counts chat needs.

## The stop: projection check (for chat)
Codex names Lyra's `AspectRatio_MaintainYFOV` and the product's horizontal-FOV view reconstruction (`AnomalyViewport.cpp:711-728`) as the likely cause. That file is unchanged since the release (`d7eee97`, `23098fa`), so the defect predates J2. I checked the magnitude against UE 5.1's projection code:
- `CameraStackTypes.cpp:176-183`, under MaintainYFOV, takes the view info's FOV as horizontal **at the view info's own aspect ratio**. It converts that to a vertical half-angle, then widens it to the viewport. With FOV 90 and the default 4:3 view-info aspect, the real horizontal half-tangent is (viewport aspect) × 0.75.
- The product assumes 1.0 (FOV 90 horizontal at any viewport aspect). The labels record `fovDeg 90`.
- **Prediction for 942×869 (aspect 1.084): 0.813.** Codex's actual-vs-claimed coordinates give 0.814 in x and 0.813 in y.
- **Prediction for 1123×870 (aspect 1.290): 0.968.** On the *passing* Lyra legs (`lyra_P_delayed_r1`, `lyra_P_multi`), the projected `bbox_px` centre offset divided by the mask-drawn `bbox_drawn_px` offset is **0.9672 (x) / 0.9677 (y)** over 96 rows.
- StackOBot (no axis-constraint setting) reads **0.9997–1.0009**.

So the mechanism is consistent with engine source and with both measured magnitudes. It is still not confirmed by a fix.

What this means:
- On a MaintainYFOV host, `bbox_px` and the projected `visible_positive` in `labels.jsonl` are mis-scaled whenever the viewport aspect differs from the view info's aspect.
- That includes Lyra's passing legs, about 3 % off. No J2 gate checks the projected box against the drawn box.
- In this session `annotation.json` stayed correct (`manifested:false`); `labels.jsonl` did not.
- Which axis constraint the office hosts use was not read.

## Observations (not counted)
- **A. The view target is part of the owner's display identity.** `SameDisplay` compares `ViewActor`, and the owner is refreshed every tick. So a mid-run camera view-target change would raise the generation and drop in-flight frames. That is from reading the code, not measured. Checkpoint I's cases are static arrangements.
- **B. The office reader needs Python 3.11 or later** (`hashlib.file_digest`), numpy, Pillow and the repo's `verify_capture.py`. The inspection file must be hand-written: SHA-256 and `correct_view` for at least 20 distributed PNGs, and no helper produces it. Neither point is in `office_checkpoint_i.md`.
- **C. Encoding loss.** Two added doc headings carry a literal `?` (0x3F) where a dash belongs: the `CLAUDE.md` 093-06 status heading and the journal's "J2 implementation checkpoint ? proof pending".
- **D. Harness fixes (11) are disclosed**, each within two attempts, with failed evidence retained. Two of them changed only the harness game EXEs (R1 → R2 → R3); the product editor DLLs stayed identical. The Lyra world-stamp fix uses the actual constrained projection, the same projection the product gets wrong.

Nothing was fixed. Chat rules.
