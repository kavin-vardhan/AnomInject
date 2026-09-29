# 087-01 — The office label-sync check kit (`label_sync_check.py`), built to 086-01 §8, transition-aware, proven both ways on banked captures; the AnomDash bundler token fix (G376)

Date: 2026-09-29 (IST) · Author: Claude Code (Opus 5.5, headless) · Brief: `087-01-office-check-kit.md`
Branch `feat/office-check-kit` off `fix/m52-label-timing` `02d15a4` (worktree `E:\IA_BuildCache\_r87_kit`; the main
checkout stayed on `m51` `53bf725`, untouched). AnomDash `master` `618b8df` → `ab6e63f`.

**Bench-free.** No game launch, build, cook or staging; nothing took focus. Every bank and session folder was read
only. Copies with edited labels (flag synthesis, the 084-07 schema, doctored shifts) lived in
`E:\IA_BuildCache\_r87_tmp` with only `annotation.json` / `labels.jsonl` / `run_summary.json` rewritten — frames and
masks were read in place from the bank through absolute paths — and were deleted after each run. Proof drivers and
logs are in `_reviews\087-01-work\` (outside the repo).

## 1. What was built

**`tools/label_sync_check.py`** — one file, standard library only (Python 3.8+ grammar checked with
`ast.parse(feature_version=(3, 8))`; no 3.9+ API), read-only, numbers only. Pillow is used only to decode PNGs when it
is importable; every number after decoding is the same pure-Python arithmetic, so the two decoders agree by
construction, and `--selftest` proves it on 23 synthetic sessions (and on a real banked frame: identical rows).
**`tools/OFFICE-CHECK.md`** — the owner's one-page card. Both ship in the delivered `host-tools/` (AnomDash manifest,
`PLUGINFILE`), and `tools/check_delivery_bundle.py` now requires them.

**Method (per event, from `annotation.json`'s `injected_frames`; clean frames chosen by annotation membership, never by
`anomaly_present` — G350):**
- **Every type but `stuck_low_mip`: 086-01's per-frame D/P on the target silhouette** (mask union dilated 4 px, else the
  median box), with 086-01's constants (6σ / 1.0 floor, P > 8 levels, the 1 % / 2×noise P gate, SETTLE_SKIP 30,
  SPAN_PAD 16, measurability, confounding, the wrong-object blob on a 4× stride, mask missing/extra, the m55 witness),
  and **084-06's edge-local references**: an onset reference from the clean frames before each run and an offset
  reference from the first clean frames after it, each with its own leave-one-out noise.
- **`stuck_low_mip`: 084-06's sharpness path** (the 084-04a ROI, gradient sharpness, two-frame onset confirmation,
  three-frame clean suffix, edge-local noise from the local high-frequency residual, the render-held rule). **Basis:
  own-detrend** — there is no matched null at the office, so null division never runs; the header says so. An event
  the sharpness path cannot judge (short span, no pre-level, no noise model) is judged by the 086-01 path instead,
  counted in the header ("where a session has what they need; otherwise the 086-01 constants").
- **Thresholds `strict` / `t10` / `t50` all computed; the release reading is transition-aware at 50 %** (086-01
  ruling 1), with raw, 10 % and strict printed beside it.

**Transition-aware gate (the 084-06 gate, narrowed by direction and reason):**
- a flag excuses **only in the direction its reason allows**: `temporal_aa` on a labelled frame in the onset half of
  its run excuses "labelled, not yet visible" (the known K_on blind spot, G353); `temporal_aa` / `hide_return` on an
  unlabelled frame excuses "visible, not labelled" **only after a run end, contiguously, and only if the effect starts
  to decay at the first unlabelled frame by 3σ** (G353's shape rule). A flag before a label start, or a
  `temporal_aa` flag on a labelled frame in the offset half, excuses nothing — so **a label late by 1 with a flag
  wrongly covering it still FAILS**;
- **084-07 decision 1:** a labelled frame flagged `partial` that is not visible at the threshold is **confirmed
  partial** if its measured effect is above the noise band (the edge-local strict threshold — there is no null
  session) and **flagged-invisible** if within it, **provided** the per-texture record shows at least one texture at
  its held level (`stuck_mip.render_textures[].level == "held"`, or resident ≤ held level on older records); otherwise
  FAIL. An off frame backed only by `unresolved` FAILS (084-07b N4). Per-edge partial counts are reported, warm-up
  events separately, with edges over 8 partial frames counted for investigation (decision 2);
- an AA-only flag (`temporal_aa`, `hide_return`) on a session whose `run_summary` says no temporal AA is a FAIL;
- **legacy flags** (`E9FF019A`, `transition: 1` without `transition_reason`) get their reason inferred from position
  (labelled → `temporal_aa`; after an end → `hide_return` for hide types, else `temporal_aa`; before a start →
  unknown, excuses nothing).

**The 084-07 schema:** entries carrying `labelled` mark a new-rule session; the kit keeps judging against the
annotation frame lists (the one authority) and **counts every entry whose `labelled` disagrees with the annotation**
(expected 0). The header counts sessions per rule: new (`labelled`), flags without `labelled` (084-05a..084-06), old
(no flags; read raw).

**Output:** a header (kit, evaluator, decoder, sessions read / refused and why / duplicates, resolutions, rule,
temporal AA, masks, the reference and basis counts), one block per delivered type (events and why not judged; release
counts; start/end histograms at t50/t10/strict, transition-aware and raw; censored / unresolved / gap-bounded edges;
wrong-object events as an upper bound and how many also change on unlabelled frames; mask-missing / mask-extra;
post-edge residual; transition and partial counts; `labelled` disagreements; the m55 witness), and a **READ BACK**
block, one line per anomaly. `camera_clipping` (and any other type without target pixels) prints *not judgeable by
this kit* and why, never a number. The selftest asserts the report contains no path, folder, frame or object name.

### Deviations from 084-06, and why (found by the kit's own known answers — G395)
1. **The onset reference excludes the event's own unlabelled fire rows.** 084-06's `clean()` admits them, so a label
   that starts one frame late puts the effect frame into its own reference: 084-09 reads **start +4, not −1**.
2. **The offset reference's "settled" test judges the first post frame against the median and noise of the other post
   frames.** 084-06 builds the test from the same six frames, so one effect frame inflates the leave-one-out std, the
   test passes, and three labelled frames vanish below the inflated threshold: 084-09 reads **end −3, not +1**. When the
   first post frame is not settled, the kit judges the offset against the onset reference with 086-01's drift censor —
   which also makes a faint one-frame ghost read `+1` strict, `0` at 10 % and 50 %, as 086-01 does.
3. **A drift censor against the offset reference** (late clean frames beyond the reference vs the reference, ≥ 10 % of
   the effect): without it a slow drift after an event reads as a late end.
4. **Unsettled leading post frames are dropped one at a time** (each judged against the median and noise of the frames
   after it) before the kit gives up on the edge-local reference: a single TAA residual frame after a texture restore
   otherwise sent 4–5 ends per MainWorld leg to the onset reference, where the scene's drift censored them (the first
   version of deviation 2 did that; with this, the REAL legs read 084-06's PASS 10/10 again).
5. **The offset depth spans the run's labelled frames and the frames after it.** 084-06 takes it from the frames after
   the run's midpoint only; for a **one-frame** labelled run (blinking under the synthesised tick order) that is post
   frames alone, so a 15 % reappear residual normalises to 100 % and reads **+1 at 50 %** — 50 false +1 ends on the
   banked synth-order blinking legs until this was fixed (`d964e53`, regression case in the selftest; `bd8c780` fails
   it). 084-09 does not show it only because its lenient settled test (deviation 2) absorbs the residual.

6. **Unsettled trailing reference frames are dropped too** (`281b505`). A post reference that ends on the next burst's
   first changed frame inflated `max(MAD, std)` to 103.8 on banked `PB0_SMOKE_try2` event 6, and a hidden frame read
   "not visible" — a false FAIL (end −1) where 086-01 reads unresolved; the kit now reads it as 086-01 does.
7. **086-01's "unresolved" edge is honoured by the gate:** visible frames running contiguously into the window's edge
   after (or before) a run leave that edge unjudged and counted as censored, not failed. An isolated stray visible frame
   still fails (selftest).
8. **Any row carrying an entry that is not the event's own ends its window** (086-01's rule; `2ecf3c8`). 084-06 stops
   only at other *annotated* events, so a burst that is in `labels.jsonl` but not in `annotation.json` — the session's
   last burst cut by the frame cap, or a vetoed event — sat inside the window (banked `M51_F1B_SYN_try2`, event 6). The
   office capture's own last burst is cut the same way. Selftest: `281b505` reads the case FAIL +13, `2ecf3c8` PASS 0/0.

9. **stuck_low_mip noise and trend windows exclude the event's own rows before its label** (`0cd2bb0`). With a label 3
   frames late, the event's own blurred frames sat in those windows and every event read NOT-MEASURABLE (doctored +3 on
   B9_FIX and M52SYNC_B0, 16/16); now FAIL −3 (−2/−1 under TAA). Selftest `m52_label_late_3`: `2ecf3c8` reads it
   NOT-MEASURABLE.

Each deviation came from a synthetic known answer or a named banked event and carries a selftest case; deviations 5, 6,
8 and 9 were shown to change the verdict against the commit before them, and deviations 1–2 against 084-09's own
evaluator (G96).

**Commits on `feat/office-check-kit`:** `bd8c780` the kit · `d964e53` offset depth, R1 constant · `281b505` trailing
trim, unresolved edges · `2ecf3c8` foreign entries end the window · `0cd2bb0` stuck_low_mip own rows · then the docs.

Both 084-09 verdicts on the deviation-1/2 cases are still FAIL (as "labelled not visible"), so no doctored label passes
there — but the offsets it prints for a failing event are the wrong size and sign.

**084-09's reference amendment R1 is applied** (084-06 ruling 5: "the office kit gets the same rule"): an onset
reference uses clean frames at least 4 frames after the previous event; with fewer than 4, post-event frames are added;
still fewer ⇒ **confounded reference**, counted per type. On the banked **dense** captures (4-frame gaps) that is most
events; with the office recipe (`IAI.Capture.Config 2 40 8 30 0`, 30-frame gaps) it does not bind. R1 is a module constant
so the proof driver could compare the method with 086-01 with it off; the header says whether it is on.

## 2. Selftest (`--selftest`)
**32 checks, 28 synthetic sessions, OK on both decoders and with Pillow hidden (`python -S`).** Sessions are written
with a stdlib PNG writer that uses all five row filters; the decoder must reproduce every written pixel (RGB, RGBA,
grey, partial rows), and the whole suite's numbers must be identical under Pillow and the stdlib decoder. Known
answers (at 50 % unless stated): exact labels PASS 0/0 · label late by 1 FAIL −1 · early by 1 FAIL +1 · ends 1 early
FAIL +1 · ends 1 late FAIL −1 · effect 3 frames late FAIL +3 · an 8 % reappear ghost: 50 % 0, 10 % 0, strict +1 · a
one-frame hidden run then a 15 % residual: 50 % 0, strict +1 · slow drift after the event: end censored · a later change
running into the window end: unresolved, not failed · an isolated stray visible frame FAIL · a later burst missing from
the annotation ends the window: PASS 0/0 · a missing mask counted · a moving camera not judged · a gap beside the onset:
PASS with the gap counted, and FAIL when the label is also late · a second object changing: wrong-object counted, none
on the exact case · an old-rule session read raw, late label FAIL · a flagged TAA smear PASS transition-aware with raw
end +1 · **a label late by 1 with a stray flag covering it FAIL** · `camera_clipping` not judgeable · stuck_low_mip:
exact PASS 0/0, label 3 late FAIL −3, partial frames above the noise band PASS with raw start +2 and 2 confirmed,
a partial frame inside the band with the record PASS counted flagged-invisible, the same without the record FAIL, an
off frame backed only by `unresolved` FAIL, a flagged TAA tail PASS · the report contains no path, folder or object
name. **If any doctored case reads PASS the selftest prints a refusal line and exits 1.**

## 3. Proofs on banked captures
All with Pillow, parallel workers, from `_reviews\087-01-work\proofs.py` / `compare086.py`; logs `final_*.log`. The
D-path results ran on `2ecf3c8`, the stuck_low_mip results on the final `0cd2bb0` (the commit between them touches only
the stuck_low_mip path; every stuck_low_mip leg reads the same on both except one onset of the B0L lever, +1 → −2,
which still FAILs).

**3.1 Master-build stuck_low_mip must fail (084-03 baseline legs, `E0BE6F0A`) — it does, in 084-06's direction and size:**

| leg | kit t50 (start / end) | 084-06 rebank E1 |
|---|---|---|
| B9_BASE (AA off) | FAIL 21 · {−1:21} / {+4:10}, 11 censored | FAIL 21 · {−1:21} / {+4:21} |
| B0_BASE (TSR) | FAIL 21 · {+4:21} / {+5..+9}, 12 censored | FAIL 21 · {+4:21} / {+5..+9} |
| B3_BASE (auto) | FAIL 8 · {−1:8} / {+7:1, +8:1, +9:2} | FAIL 8 · {−1:8} / {+7..+9} |
| B7_BASE | FAIL 16, 5 censored · {+0:11, +1:9, +2:1} / {+6..+14} | FAIL 21 · {+0..+3} / {+6..+10} |

The kit censors more ends than the rebank because it has no null session (G396); every judged edge agrees in sign
and size, and 084-03's accepted reading (AA off: −1 at the start, +4 at the end) is reproduced.

**3.2 Fixed builds must read 0 at 50 % transition-aware:**
- **084-03 fix `B725678B`, AA off (B9_FIX):** PASS 5, CENSORED 11, **FAIL 0**; start {0:16}, end {0:5} (every judged edge 0,
  as 084-05b's "0/0 on 16/16"; the 11 ends are censored, G396).
- **084-03 fix under TSR (B0/B3/B7_FIX), no flags in the data:** raw FAIL with the TAA smear (start +1/+2, end +4..+6), as the
  rebank. **With 3/8 flags synthesised by the build's rule** (copies): B0_FIX PASS 4 / CENSORED 12, B7_FIX PASS 3 /
  CENSORED 13, B3_FIX PASS 3 / CENSORED 5 — **FAIL 0**, every judged edge 0 (the rebank: PASS 16 / 16 / PASS 3 + CENSORED 5).
- **084-06 `E9FF019A`, real flags:** **B0 (TSR) CENSORED 12, PASS 4, start raw {+1:13, +2:3} → transition-aware {0:16}**,
  end raw {+4:2, +5:2} → {0:4}; **B3 CENSORED 5, PASS 3** — both **exactly** 084-06's E1 readings.
  **B9 (AA off) FAIL 16, start +2 on 16/16 — exactly 084-06's reading**: the partial-application onset (084-06 ruling 1,
  reading (a)), which that build did not flag. **The B0L timing lever still fails (FAIL 12, CENSORED 9, as 084-06).**
- **The 084-07 schema on real pixels:** a copy of B9 given `labelled` keys and `partial` flags synthesised from its own
  per-texture render record (a texture below baseline beside one short of its held level) reads **rule "new", PASS 5,
  CENSORED 11, FAIL 0; start raw {+2:16} → transition-aware {0:16}; 32 partial frames confirmed** (two per event, above
  the noise band — 084-07's 6–22 %).

**3.3 The four proven types vs 086-01 (159 sessions, 498 judged events):**
- **Method comparison (R1 off, as 086-01):** at 50 % **every edge both judge is equal — 871/871 starts, 744/744 ends, 0
  different**; 126 ends censored by both; 1 end judged by the kit only (086-01: unresolved). The t50 histograms are
  identical per type (blinking start {0:680}, end {0:598} vs 086-01's {0:597}; corrupted_texture {0:130}/{0:103};
  missing_object {0:36}/{0:30}; missing_texture {0:25}/{0:14}). At 10 %: 869 starts and 735 ends equal; **4 ends
  differ** — 086-01's four blinking +1 residuals (12–16 % of the hide) read 0: against the kit's settled post-event
  reference the first frame after the return carries 6.3 % (`A1_A1_NAT`), because the post frames still hold part of
  the slow relight (086-01 §5.2); 6 ends judged by the kit only.
- **As the kit ships (R1 on):** 418 of the 498 events read *confounded reference* (these captures space events 4 frames
  apart); the other 80 agree edge for edge (102/102 starts, 91/91 ends, 11 both censored).

**3.4 The 084-06 bench legs (`E9FF019A`, MainWorld and CB_GateLevel):** REAL_BL/MO/MT/CT_R1 and the two AA-off legs
**PASS 10/10, 0/0** (084-06 E1: PASS 10); the R2 legs PASS 6–8 with 2–4 ends censored for scene drift (084-06: PASS 10);
MASK55 32 judged all 0/0, 86 confounded reference (its 4-frame gaps), which includes the missing_texture borderline 084-08
already called CONFOUNDED-REFERENCE; LOD_NAT not measurable (as 084-06), LOD_NAT_AAOFF PASS 1 / CENSORED 2.

**3.5 Doctored labels must fail** (copies with every event's frame list shifted; frames and masks unedited):

| session | −1 | +1 | +3 |
|---|---|---|---|
| REAL_BL_R1 (TSR, flags) | FAIL 10/10 (+1) | FAIL 10/10 (−1) | FAIL 7/7 (−3), 3 confounded |
| REAL_MO_R1 | FAIL 10/10 | FAIL 10/10 | FAIL 10/10 |
| REAL_MT_R1 | FAIL 10/10 | FAIL 10/10 | FAIL 10/10 |
| REAL_CT_R1 | FAIL 10/10 | FAIL 10/10 | FAIL 10/10 |
| REAL_BL_R1_AAOFF | FAIL 10/10 | FAIL 10/10 | FAIL 2/2, 8 confounded |
| B9_FIX (stuck_low_mip, AA off) | FAIL 16/16 | FAIL 16/16 | FAIL 16/16 (−3) |
| M52SYNC_B0 (stuck_low_mip, TSR, flags) | FAIL 16/16 | **PASS 3 / CENSORED 13** | FAIL 16/16 |

**No doctored label passes, with one documented exception:** stuck_low_mip under TAA with the label 1 frame *late* — the
K_on blind spot 084-05b ruled and G353 records (a label up to 3 frames before the first 50 %-visible frame is, in
pixels, the TAA onset lag the flag exists for); the same shift FAILs 16/16 on the AA-off leg. "Confounded" events are
not judged (the effect's peak falls outside the shifted window); none reads PASS.

## 4. Runtime
Measured single-process on this PC (Python 3.13; the PC was at about 30 % load from other applications), log
`_reviews\087-01-work\timing_result.txt`:

| run | Pillow | standard library only (`python -S`) |
|---|---|---|
| `--selftest` (28 sessions; with Pillow it runs both decoders) | 51 s | 46 s |
| **56 events / 1,200 frames** (REAL_BL/MO/MT/CT_R1, MainWorld, TSR) | **180 s** | **857 s** |
| stuck_low_mip, 16 events / 1,200 frames (B9_FIX; the sharpness path reads every frame) | 120 s | 552 s |

The two 56-event reports are **identical line for line** except the `decoder:` and `seconds` lines (111 of 113). The
standard-library decoder does ~0.3 s per 1280×720 RGBA frame (Pillow ~0.03 s), far below 086-01 §8's 1–3 s estimate.
**For the office recipe (3,000 frames, about 70 events):** about 7 minutes with Pillow and about 30 without, on this
PC; the card says 5–10 and 30–45 minutes to allow for an older Python.

## 5. The AnomDash bundler (G376), `ab6e63f`
- `make_delivery.py` **always removes** the `dashboard\config.json` that `npm run build` copied from the gitignored
  `public\config.json`, and says whether it removed one; `--token-log <a log of the delivered build>` (its
  `=== Control server token:` and `LISTENING on ws://` lines) or `--token-ini <its DefaultGame.ini>` **writes the token
  the delivered build enforces** (G118), read back after writing; an empty, short (< 32) or placeholder token is
  **refused, no bundle produced**; without a token source, or without `--plugin-repo`, the banner says **NOT
  COMPLETE** and the run **exits 4**; `make_delivery.bat` asks for the plugin repo and the log and no longer calls an
  incomplete bundle "ready" (it did, for the dashboard-only bundle too). The token is never printed. The manifest ships
  `label_sync_check.py` and `OFFICE-CHECK.md` as cross-repo files.
- **Proven both ways with `tools/check_delivery_bundle.py`:**

| bundle | checker | against | verdict |
|---|---|---|---|
| old (`618b8df` bundler) | `02d15a4` | a delivered-build log whose token differs (synthetic, engine line format) | **STOP TOKEN** (dev token shipped; the bundler had printed "config.json was NOT copied") |
| old | `02d15a4` | a real game log (`Builds\MidRepro`, placeholder era) | **STOP TOKEN** |
| old | `02d15a4` | this box's `DefaultGame.ini` | PASS — the dev token *equals* this box's project token, the coincidence G376 describes |
| old | new (kit + card required) | the synthetic log | STOP: TOKEN + the two kit files missing |
| **new** (`ab6e63f`, `--token-log` the synthetic log) | new | the same log | **PASS** |
| new, no token source | new | the synthetic log | STOP (config absent) — bundler exit 4, banner NOT COMPLETE |
| new, `--token-log` the real placeholder-era log | — | — | bundler **refuses**, exit 2, no bundle |

- ⚠ No real log of a build with a 64-character token exists on this box (the bench legs do not start the control
  server; the older delivery-shaped logs carry `TESTVALUE123`), so the passing leg used a synthetic log in the engine's
  exact line format. The delivery rehearsal should repeat it with a log of the real delivery build.
- ⚠ Observed while building the bundles: AnomDash `dist/` is **older than `src/`** on this box (the bundler's stale-build
  warning fired; `--yes` was used for the proof bundles only). The delivery build must `npm run build` first.

## 6. What the kit cannot judge
- `camera_clipping`: no target mask; its label is a whole-frame proxy that needs a matched null capture.
- Moving-camera events (counted as camera moved) — hence the hands-off recipe.
- Warm-up events (first labelled frame before session index 30) — hence `IAI.Capture.Config 2 40 8 30 0`.
- Effects too faint to measure (not measurable), and scenes whose change dominates the target (confounded).
- Targets with no measured mask: the box is used, at lower confidence.
- Frames the capture never saves (the FSM's settle gap): an edge beside one is judged per captured frame and counted.
- `stuck_low_mip` ends on a cycling scene: without a null session they are censored, not judged (G396).
- Wrong-object is an upper bound: shadows, GI and reflections trip it; a flagged event needs an eye check.
- Residuals below 10 % of the effect show only in the strict row.

## 7. State and hand-off
- **Branch `feat/office-check-kit`** (pushed; no tag, no merge): the kit, the card, `check_delivery_bundle.py` requiring
  both, the readme Step 7, this journal, the status block, G395/G396 and a G376 note. It merges with the fix at merge
  prep. ⚠ **AnomDash `ab6e63f` already lists `tools/label_sync_check.py` and `tools/OFFICE-CHECK.md` as cross-repo
  files**, so `make_delivery.py --plugin-repo <a ref without this branch>` fails loudly (by design) until the merge.
- **Office recipe (the card):** `IAI.Capture.Config 2 40 8 30 0`, Auto-pool with every anomaly but `camera_clipping`,
  PNG, native size, 3000 frames, hands off; then `--selftest` and one run; read back the READ BACK block.
- **For chat, before tonight's 084-09:** its D-path shares deviations 1–2 (G395): on known answers it still FAILs, but
  reports start +4 for a label 1 late and end −3 for a label ending 1 early, and a post reference ending on a later
  change can raise a false FAIL (deviation 6). The kit can be run beside it on the banked legs as a third reading.
- **Not done:** no real log of a build with a delivered-length token exists here (the bundle proof's passing leg used a
  synthetic log; the delivery rehearsal repeats it on the real build); AnomDash `dist/` is older than `src/` on this box.
- Temp copies deleted; nothing left in `E:\IA_BuildCache\_r87_tmp` but the synthetic token log and commit messages
  (removed at the end).
