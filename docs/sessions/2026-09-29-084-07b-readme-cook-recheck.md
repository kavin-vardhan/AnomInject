# 084-07b — the client readme's exact per-frame label semantics, a cook of `fix/m52-label-timing`, and Codex's re-check of 084-07 (2026-09-29)

Brief: `_mailbox` 084-07b (Code, Opus 5.5, xhigh), released by `_reviews\084-07-chat-ruling-source-round3.md`. Spec:
`_reviews\084-06-chat-ruling-bench.md` ruling 3 (the cook) and `_reviews\084-05b-chat-ruling-harness-codex-red.md`
decision 3 (F1: `anomaly_present` = the event is active; visibility is `visible_positive` / `observable` / `transition`).
Branch `fix/m52-label-timing` at `a6a4893` (source `8b8b4d0`), worked in the warm scratch host `E:\IA_BuildCache\_r84_host`.
⛔ **No game launch, no staging, no source edit, nothing that took focus.** Compiling and cooking only. Evidence:
`_reviews\084-07b-evidence\`.

## 1. Codex re-check of 084-07 — RED (collected, not acted on)

GDP Relay `runs\2026-09-29-084-07-sync-recheck\`, `gpt-6-astra` at `max` (requested == rollout), **1,233 s**, exit 0,
frozen `source-a6a4893` extract. Collected byte-identical under a one-line header to
`_reviews\084-07-codex-sync-recheck.md`; ledger row appended.

| item | Codex | one line |
|---|---|---|
| 1 m52 `partial` | PARTIAL | right on the banked sets (replayed independently: onset 2 per event, warm-up 5); an `Unknown` texture reads as partial, an invalid endpoint reads as held, the endpoint is a prediction, and the per-event listing caps at 32 (N4, N5) |
| 2 camera_clipping confirmation | PARTIAL | a zero-thickness (planar) box clips every ray to a zero-length segment ⇒ unflagged miss (N2, high); collision ≠ render geometry; landscape bypasses the eligibility checks; raw `CTF_UseDefault` |
| 3 F2 | PARTIAL | the composed `FTransform` loses a rotated instance under non-uniform component scale (N3, high) |
| 4 F4 | PARTIAL | a restored holder outside the old actor's component list can escape verification (N7) |
| 5 F5 | PARTIAL | a detached carried tail is lost at the next short-run boundary (N6) |
| 6 F3 | RESOLVED | for the original event-set and hide-sampling defect |
| 7 row families / `labelled` | PARTIAL | **N1, high: `labelled` reads `FireActive`, which is `IsLogicallyHidden` for every FireWindow id, so `missing_texture` / `corrupted_texture` entries read `labelled: false` and rows `visible_positive: false`** — the same defect found independently here (§3.3, D1) |

`IAnomaly.h` +1 line: an additive member only. Runs that fire no m52 / camera_clipping are not byte-identical (N1).
⛔ **Codex's findings are information for chat; none was acted on here.** The readme was, however, brought into line
with what the source *does* on the four limits Codex names (§3.2), because the brief requires every readme statement to
match the source.

## 2. The cook — a complete staged set for `fix/m52-label-timing`, archived, NOT staged

- **Pre-declared** (`cook-predeclare.md`, written before the cook): map set `CB_GateLevel` + `MainMenu` + `MainWorld`
  (`Entry` by dependency), `CB_TexCorruptLevel` deliberately excluded (no m53 code on this branch); four predictions.
- **Content:** the r84 host has none of its own, so two temporary junctions were made (`Content` → the D: project,
  `Plugins\RoomGenerator\Content` → its D: content), the arrangement the m53 host uses. **D: content manifest (2,182
  files, path|size|mtime|SHA-256) identical before and after (`E4C7F38D…` both).** Junctions removed afterwards; D:
  content verified still present (2,174 + 8 files).
- **Editor target up to date before cooking** (G47): `Target is up to date`, 3 s.
- **Command:** `cook-a1-command.txt` (the 082-07c command minus the m53 level), archive dir
  `E:\IA_BuildCache\_r84_cookout_a1` (the complete staged tree is kept there). **Wall 60 s (03:57:28 → 03:58:27); cook
  commandlet 17.6 s; 794 packages, 0 errors, 0 warnings; exit 0.**
- **Predictions, all four met:**
  1. exe **`FF41BFF3`** (242,093,056 B) — byte-identical to the 084-07 archive; the Game target was up to date.
  2. **Descriptor proof:** `UnrealPak <pak> -Extract -Filter=*.uplugin` → the cooked `AnomalyInjector.uplugin` is
     **byte-identical to the branch's** (SHA-256 `9EFB9B496F6C12A2…`); five modules, **`AnomalyBench` first,
     `TargetAllowList [Editor, Game]`, `TargetConfigurationDenyList [Shipping]`**, then `AnomalyShaders`
     (`PostConfigInit`), `AnomalyInjector`, `AnomalyCapture`, `AnomalyControlServer`; module-list diff against the branch
     **empty**. Saved as `uplugin-in-m52fix-cook.json`.
  3. `verify_cooked_maps.ps1` **PASS**: `CB_GateLevel`, `Entry`, `MainMenu`, `MainWorld`; the inverted probe for
     `CB_TexCorruptLevel` reports it MISSING, as declared.
  4. D: content unchanged (above).
- **Hashes (SHA-256, first 8):** exe `FF41BFF3` · `StackOBot-Windows.utoc` **`87A46A27`** (294,235 B) · `.ucas`
  `55A65BA4` (364,596,784 B) · `.pak` `26FFC026` (10,115,711 B) · `global.utoc` `462B8AC6` · `global.ucas` `BB05CF99`
  (the globals equal the m53 set's).
- **Archive:** `_binary_baselines\m52fix-cook-87A46A27\` — the six files, each re-hashed at the destination
  (`archive-hashes.json`, 6/6 match), plus the command, the predeclaration, the descriptor and the map-gate output.
  **Pairing: exe `FF41BFF3` + container `87A46A27` / `55A65BA4` / `26FFC026` + `462B8AC6` / `BB05CF99`.**
- **Not staged:** `Builds\BenchGate` still holds the m53 set (`2FCDF059` + `20DA6F98` / `534C5863` / `FD766B7B` +
  `462B8AC6` / `BB05CF99`), read back after the cook (`staged-set-unchanged.txt`).
- ⚠ **This cook carries D1 (§3.3) and Codex's N2/N3.** If chat rules a source fix, it is a rebuild **and** a re-cook; the
  cook itself is one minute, the descriptor proof and archive about five.

## 3. The client readme (`docs/client-readme.md`)

### 3.1 Written
- **§8.7 (new) — "The per-frame label, anomaly by anomaly — and how to build a training label from it":** the five
  fields (`anomaly_present`, `labelled`, `visible_positive`, `observable`/`target_pixels`, `transition`/`_reason`) and
  the question each answers; a per-anomaly table (active window, labelled frames, active-but-unlabelled rows, box,
  where `observable` is measured, reasons that can appear) for all seven delivered types; the mask-value recycling and
  how to key a mask; the one-paragraph training-label recipe (including the veto exception: `annotation.json` decides).
- **§8.6:** `anomaly_present` (camera_clipping only on positive frames; pointer to §8.7); `visible_positive` — **the
  084-07 change and that sessions delivered before this build use the old rule, told apart by the presence of
  `labelled`**; `labelled` = membership in `injected_frames`, absent on older files and on `IAI.Capture.Shot`; the
  row-family table gains the texture-box-off-screen family and the reverse combination (hidden object off screen:
  labelled, no box).
- **§8.6a:** decision 3 (a `stuck_low_mip` event's first 1–5 frames can be partial, the order varies), the two source
  limits of the partial flag (unknown ⇒ partial; the endpoint is a prediction), the 32-frame listing cap, and a
  per-reason "what to do" table (strict vs larger set).
- **§8.6b:** the collision-mesh basis, four no-flag misses (sub-grid sliver, zero-thickness plane, rotated instance under
  non-uniform scale, one-sided collision from behind), the landscape heightfield caveat, and the cost (16 µs/frame of
  grid arithmetic at 16 candidates measured outside the engine; trace cost unmeasured in-engine; budget 0.5 ms; the
  run_summary keys).
- **"A note on `camera_clipping`" rewritten** — it still described the bounds-only rule (hollow meshes over-label,
  no-collision objects count), which 084-07 replaced.

### 3.2 Corrected because the source says otherwise (found while tabulating)
- `annotation.json` writes **`blink`** for `blinking` (`MapAnomalyToClient`, `AnomalyCaptureSubsystem.cpp:523–535`);
  §8.2 listed `blinking` and §8.6 said `id` "matches `anomaly_type`". Both fixed.
- §8.4's `frames_drawn_unexpected` sentence still said "it should be `0`" beside the paragraph that expects a residual
  on some titles (the 077 ruling). Aligned.
- §8.6a's partial bullet claimed the flag marks "exactly the partial frames and no others"; the source flags an
  `Unknown` level and trusts a predicted endpoint (Codex N4). Rewritten to say both.

### 3.3 🚨 D1 — the build does not implement the readme's `labelled` rule for texture swaps
`labelled` is written from `FireActive` (`AnomalyLabelWriter.cpp:264–269`). `FireActive` is `ComputeFireActive`
(`AnomalyCaptureSubsystem.cpp:5723–5728`), whose non-`AnomalyState` branch returns `IsLogicallyHidden(actor)`
(`:7568`) — false for `missing_texture` and `corrupted_texture` (FireWindow ids, `:550–551`). Their annotation frame list
is `AffectedFrames` (`:8900–8903`, filled at `:8770–8780`), not the `FireActive` subset. So on build `FF41BFF3` every
texture-swap entry reads **`labelled: false`**, every such row **`visible_positive: false`**, while `observable` (which
reads `FireLabelled`, `:4520,4550`) can read `true` on the same entry and `annotation.json` lists the frame. **G241's bug
class again** (two per-fire "active" bits; the new consumer took the one that does not cover every source) → **G361**.
No runtime evidence exists for this build; the reading is from source, and Codex found it independently (N1).
- The readme states the **ruled** rule (`labelled` ⟺ in `injected_frames`); the field table below marks this row as
  NOT MATCHING the source. A fix that makes them agree must reproduce the annotation's own test for FireWindow ids —
  fire live **and** the projected box valid (`:8770–8780`; the writer's `bValid` is the same function on the same view,
  `AnomalyLabelWriter.cpp:177–187`) — **not** `IsFireLabelledThisFrame` alone, which is true on off-screen frames the
  list excludes. Chat rules; nothing was changed in source.
- `verify_capture.py --label-rule` (§4) fails such a session with `LABELLED-MISSING`, so the next bench cannot miss it.

### 3.4 Field → source (commit `8b8b4d0`; unchanged at `a6a4893` and at this commit)
`LW` = `Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp`, `CS` = `…/AnomalyCaptureSubsystem.cpp`, `LS` =
`Source/AnomalyInjector/Public/AnomalyLabelSync.h`, `SW` = `…/Public/AnomalyStuckMipWindow.h`.

| # | field (file) | written at | rule it follows | readme matches source |
|---|---|---|---|---|
| 1 | `session_index` (labels) | LW:126 | the frame number / join key | yes |
| 2 | `frame_index` (labels) | LW:125 | engine counter | yes |
| 3 | `t`, `t_wall` | LW:127–128 | game / wall seconds | yes |
| 4 | `image`, `width`, `height` | LW:129–131 | written frame | yes |
| 5 | `anomaly_present` | LW:132–133, `CountEntries` LW:80–105 | any entry emitted `Normal` (LW:87–90); stuck_low_mip Normal only on members (CS:7303, 7323; LS:95–102); camera_clipping entry only when positive (CS:5625–5637) | yes |
| 6 | `transition_present` | LW:134–137 | any entry with transition, incl. transition-only (LW:95–103) | yes |
| 7 | `visible_positive` | LW:301 with LW:274–277 | present AND a labelled entry with a valid box | yes (as a rule; its input is D1) |
| 8 | entry `labelled` | LW:264–269 | `FireActive` (CS:5723–5728, 7308, 7551–7569); absent when the caller passes none (LW:668–669, 1350) | **NO for FireWindow ids (D1)**; yes otherwise |
| 9 | entry `transition` / `transition_reason` | LW:258–263, 57–78; names LS:17–33 | bits: temporal_aa (CS:7322–7326, LS:203–230), hide_return (CS:7110–7156), partial (CS:7324–7326, 6911; SW:660–693), camera_clipping_unconfirmed (CS:5650–5657) | yes (limits per Codex N4 now stated) |
| 10 | transition-only entries | LW:280–298 | `TransitionOnly` emit (LS:95–102) and detached tails (CS:7191–7227); FireIndex `INDEX_NONE` ⇒ `labelled:false` | yes |
| 11 | suppressed entries (no entry) | LW:282–286 | stuck_low_mip non-member, non-off-window (LS:101) | yes |
| 12 | defaults 3/8/1, 0 without temporal AA | LS:13–15, 65–76; CS:6979–6984 | | yes |
| 13 | entry `observable` | LW:160–169; CS:4520–4577 | `FireLabelled` && condition held && px ≥ min; `Unknown` render membership ⇒ unmeasured | yes |
| 14 | entry `target_pixels`, `target_drawn_pixels` | LW:152–158 | −1 = unmeasured | yes |
| 15 | entry `bbox_valid`, `bbox_norm`, `bbox_px` | LW:174–195 | whole frame for whole-frame / no-actor fires, else projected bounds | yes |
| 16 | entry `bbox_drawn_px` | LW:197–208 | measured box or null | yes |
| 17 | entry `mask_value` | LW:145–150; CS:5782–5799 | the event's record tag | yes |
| 18 | entry `id`, `target_name`, `start_frame`, `seconds_remaining` | LW:143–144, 171–172 | engine id (`blinking`, not `blink`) | yes (after the §3.2 fix) |
| 19 | `stuck_mip.*` entry keys | LW:210–256 (telemetry); CS:7341–7361 | render record overrides `held` | yes |
| 20 | `camera_clipping.*` row keys | LW:331–351; CS:5569–5582 | evaluation present | yes |
| 21 | `mask_file`, `mask_state` | LW:303–315 | | yes |
| 22 | annotation `anomaly_type` / `_subtype` | LW:1198–1199; CS:523–535 | `blink` for blinking | yes (after the §3.2 fix) |
| 23 | annotation `injected_frames` | LW:1229–1236; CS:8882–8905 | FireWindow ⇒ `AffectedFrames` (box on screen); else the `FireActive` subset | yes |
| 24 | annotation `affected_frames` | LW:1210–1217; CS:8907–8946 | observable subset when measured, else the injected list | yes |
| 25 | annotation `observable_frame_count`, `unmeasured_frame_count`, `observability_measured`, `manifested`, `bbox_source` | LW:1239–1243; CS:8883–8898, 8926–8958 | | yes |
| 26 | `mask_map.json` value / `first_frame` / `last_frame` | LW:759–764 | | yes |
| 27 | mask recycling | `AnomalyMaskMeasure.cpp`:106–146, 148–230, 232–279; `AnomalyStencilTag.h`:35–37; `LS`:362–431 | free value first, then the longest-releasable finished event; retire + verify; quarantine on failure | yes (Codex N7 is a verification gap, not a statement the readme makes) |
| 28 | run_summary `label_*` | LW:1074–1098 | | yes |
| 29 | run_summary `stuck_mip_partial_*` | LW:967–978; CS:8316–8350 | per-event list capped at 32 (SW:706–746) | yes (cap now stated) |
| 30 | run_summary `camera_clipping_*` | LW:1105–1130 | | yes |
| 31 | camera_clipping positive / unconfirmed | CS:5550–5669; `AnomalyViewport.cpp`:590–628; `AnomalyNearClipSlab.h`:425–431, 660–675 | SAT then complex traces; skinned / no-collision / no-complex / caps ⇒ unconfirmed | yes (misses per Codex N2/N3 now stated) |
| 32 | stuck_low_mip refused without a render record | CS:3509–3522 | not SVE+async ⇒ refused | yes |

**32 rows; 31 match, 1 does not (D1).**

## 4. `verify_capture.py` — the label-rule reader

- **New mode `--label-rule`** (also printed at the end of the overlay mode). It picks the rule from the session: NEW when
  every anomaly entry carries `labelled`, OLD when none does — **and says which** — and it refuses a mix (exit 3). NEW:
  recomputes `visible_positive`, counts active-but-unlabelled rows per type, cross-checks `labelled` against
  `annotation.json` both ways (`LABELLED-EXTRA`, reported not failed when `vetoed_events` > 0; `LABELLED-MISSING`), and
  that every listed frame has an entry (`ENTRY-MISSING`, `FRAME-MISSING`). OLD: checks `visible_positive` against the old
  rule and counts the rows the new rule would turn false. Both: `transition_present` consistency, known reasons,
  reason-to-anomaly placement, `partial` / `camera_clipping_unconfirmed` only on labelled entries (NEW), and
  `temporal_aa` / `hide_return` only when `label_temporal_aa` is not false; a pre-084-07 `transition` with no reason is
  counted as legacy under OLD. Exit 0 / 1 / 3.
- **Selftest, both ways — 15 cases** (`--label-rule --selftest`, and appended to bare `--selftest`): an event-active
  unlabelled row is not a visible positive, and a row that claims it is fails `VP-MISMATCH`; an old session is read as
  OLD and says so, and an old row missing its old-rule positive fails; the D1 shape fails `LABELLED-MISSING` while the
  correct texture session passes; labelled-not-listed fails, the vetoed event passes with its count; stuck_low_mip
  reasons pass, `partial` on a texture swap fails; missing `transition_present`, hide_return without temporal AA, and a
  reasonless transition under NEW each fail; a reasonless transition under OLD passes as legacy; a mixed session is
  refused.
- **Before / after:**

  | suite | before | after |
  |---|---|---|
  | bare `--selftest` | 2 black-frame cases, OK | the same 2 lines unchanged + 15 label-rule cases, OK |
  | `--label-pixel-gate --selftest` | 98 cases OK | 98 cases OK, output **identical** (0 diff lines) |
  | `--change-oracle --selftest` | 35 cases OK | 35 cases OK, output **identical** (0 diff lines) |
  | `test_verify_capture_change_oracle` | 19 tests OK | 19 tests OK |
  | `test_verify_capture_consistency` | 25 tests OK | 25 tests OK |

- **On real data** (the 31 `M52SYNC_*_A1` banks from 084-06, build `E9FF019A`): every labelled session is read **OLD**, all
  exit 0 once pre-084-07 reasonless transitions are counted as legacy (the first run failed them, which is how that
  case was found). The "old-rule positives not listed by annotation.json" count reproduces 084-07's independent row-family
  classification exactly: **B0L 345, LOD 80 per leg, MASK55 128, REAL_BL 56 per leg**, and 0 on REAL_MO / REAL_MT /
  REAL_CT / B9 / CCMW_ON. Overlay mode on `LOD_NAT_A1` runs and appends the summary. (`label-rule-banked\`)

## 5. The packaging checklist (`docs/PRE-DELIVERY-CHECKLIST.md` §1)
New box before the F9 `AnomalyBench` box: extract the cooked descriptor from the delivered `.pak`, diff its module list
against the intended set (the branch's descriptor minus `AnomalyBench`) — a missing module is a STOP, `AnomalyBench`
present is a STOP; launch without `-IAIBench` / `-IAIBenchFixture` and confirm no `Capture(m52): -IAIBench present`
line and no `AnomalyBench` module line, plus F9's A44 scan; record the descriptor's SHA-256. Its measured instance is
G354 and its proof is §2's extraction.

## 6. Deviations and limits
- Codex's findings were not acted on in source; the readme was aligned with the source's *current* behaviour on the limits
  it names (§3.2), which the brief's "every statement must match the source" requires.
- The field-to-source table lives here, not in the client readme: file:line references rot and the readme ships as the
  client's `README.md`.
- The cook's `-build` step compiled nothing (the Game target was up to date), so the cooked exe is the 084-07 exe.
- No runtime evidence for D1; source reading plus Codex's independent N1.

## 7. Hand-off
- **Chat:** rule on D1 (N1) and on Codex's N2–N7 before the 084-08 harness and the next bench; a source fix needs a
  rebuild and a re-cook of this branch (§2's recipe).
- **084-08 / the 086-01 checker / the 087-01 kit:** read `labelled` and `visible_positive` as `--label-rule` does, and run
  it on every bench session; on a build carrying D1 it fails every texture-swap session with `LABELLED-MISSING`.
- **Next bench (if chat stages `87A46A27`):** its descriptor lists `AnomalyBench`, so B-CC's `IAI.Bench.CameraSchedule`
  registers under `-IAIBench`; read the lever back from each leg's own log (G354).
