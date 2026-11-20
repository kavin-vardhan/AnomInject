# REPORT for 073-01-m49-a1-fix-turn-and-full-gates.md
brief: 073-01-m49-a1-fix-turn-and-full-gates.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-04 16:14:30 | finished: 2026-09-04 17:03:27 | duration: 49 min | exit code: 0
context at last turn: 528361 tokens (53% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\073-01-m49-a1-fix-turn-and-full-gates.live.txt

---- FINAL MESSAGE (Claude Code) ----
Everything is in place and pushed.

---

## 1. Plain-language rundown

The one defect that stopped 072-02 is fixed and Phase A1 is on `master`. The bug was a single wrong term: `observable` asked "is this fire active?" using `FireActive`, which is meaningless for texture-swap anomalies and quietly answers *false* — so all four texture events shipped an empty `affected_frames`. It now asks the same question the mask-arming code asks on that same tick, and it's sampled per frame rather than read later. The counter that was hiding behind that dead term (`frames_condition_lost`) was blind too, and now stands on its own.

Everything then passed. StackOBot: 14 legs, both tick orders — onset join clean, `target_pixels` ties exactly to the mask PNG's own pixel count, G-EDGE 8/8 with zero shifts, and the schema comparison against master's own binary adds *only* the declared keys with `frame_indices` identical value-for-value. Lyra is now a gating host and the check built for it fired on exactly the case that motivated it: Lyra's heroes turn on custom depth themselves, and the hygiene check correctly attributes that to the host instead of accusing us.

The nicest result is on Lyra: a blink anomaly was injected on four frames but the camera slid and the target walked off the right edge, so two of those frames drew nothing — and the labels now say so. First time this project has produced `affected_frames ⊊ injected_frames` for a measured reason.

Leg B — "does Lyra's outline material actually draw on our tags?" — is **not answered**, and I'm reporting that rather than dressing up a number. The measurement that could isolate it has no power on this host, and the one that ran is confounded.

## 2. The fix, and the predictions amendment

`observable`'s active term became `IsFireLabelledThisFrame` (whose `default:` returns `true`), sampled per captured frame into a new `Snap->FireLabelled` in `SampleDeferredActiveState` beside `FireActive` — deliberately *not* read in the drain, which would have been the same stale-read class A1 had just fixed. `FramesConditionLost` moved above the unmeasured early-out and now counts from its own two samples (labelled **and** not held). `LG-3` added one key, `census_host_pp_customdepth_reader_names` — **the count key already shipped at `m41`, so the brief's "two keys" is one**. Plus two trailing newlines and a blank line, no behaviour. Total: 8 files, 46 insertions / 14 deletions.

**AMENDMENT 1** was appended to `docs/predictions/2026-09-04-m49-phase-a1.md` **before any leg ran** — 95 insertions, 0 deletions — recording the OBS-1 miss, the corrected predicate, the rejected "count `!bHeld` unconditionally" variant *with its reason*, the LG-3 key-count correction, and the prediction that OBS-1 now reads `injected == affected` on every event.

**Rebase or merge:** rebased first, then **undid it and merged `master` in** — a rebase of a pushed branch needs a force-push, which this repo forbids.

## 3. StackOBot gate table

| gate | native | synth |
|---|---|---|
| **OBS-1** `injected == affected`, delta 0 | **5/5** | **6/6** |
| A1-ONSET-JOIN (first fire-active carries own tag) | 0 of 5 zeros | 0 of 6 zeros |
| first **LABELLED** frame: own tag + `target_pixels ≥ 0` | 0 of 5 failing | 0 of 6 failing |
| TARGET-PIXELS TIE (label vs mask PNG) | MATCH 32 / MISMATCH 0 | MATCH 35 / MISMATCH 0 |
| `frames_condition_lost` | 0 | 0 |
| G2/G3/G3b/G3c/G7/G1 | ALL PASS, stray 0 | ALL PASS, stray 0 |
| MASK-TIE | 32 lines, 0 MISMATCH | 35 lines, 0 MISMATCH |
| black-frame gate | PASS, DARK FIRST 0 | PASS, DARK FIRST 0 |
| MASK-PICTURE-PAIRING 100 % | N0 P0 | **N1** P0 → see below |
| MASK-PICTURE-PAIRING 50 % | N0 P0 | N0 P0 |
| G-EDGE ×8 | PASS 4 · SHIFT 0 · NV 0 · NM 0 | same |
| P-C7 v3 ×8 pairs | declared-only keys; `frame_indices` identical 4/4 | same |
| A44 / both build targets | 18 symbols + 3 controls / exit 0, 0 warnings | — |

The `NEITHER 1` is at `si=8` and **is not binary-attributable**: `EC60E9B9` read Δ49.5 (N1) and Δ33.6 (N0) on two runs, and **`DE65F84A` — master's own binary — read Δ41.2 (N1) on the same frame in a fresh run**. Branch (a) of the pre-declared rule; `PREVIOUS == 0` everywhere. The 49.5 is larger than any previously banked value — stated, not smoothed; tolerance not widened.

Two notes I'd flag: the pre-declared A1-ONSET-JOIN conjunct is **unsatisfiable for hide-class ids** (a blink's first fire-active frame is *visible*, so by m44's own rule it has no mask) — a defect in the pre-declaration, reported as two separate halves, closed file not edited. And five SP50 attempts were discarded, both causes attributed: eye **Z 273.3 vs 260** with `rot (0,0,0)` (a *position* difference `A47` doesn't cover) and a genuine `A47` rotation bifurcation.

## 4. Lyra Leg A, and Leg B's numbers

| gate | result |
|---|---|
| LG-1 | **PASS** — 0 of 7 / 0 of 5; TIE MATCH 40 / MISMATCH 0 |
| LG-2 | ⚠ **did not occur naturally — said so, not faked** (`unmeasured_frame_count` 0 on all 6 events) |
| LG-3 | **PASS** — `["MaterialInstanceDynamic_162(depth+stencil)"]` in `run_summary` |
| **LG-4 (blocking)** | **PASS — branch fired**: `final HOST-ATTRIBUTED n=3 (ours=0 host=3)`, value 0, does **not** fail `P-C6` |
| LG-5 | **PASS** — occlusion 3/4/4/**1**/3/3 of 9 |
| LG-6 / LG-8 | **PASS** — `PASS 4 · SHIFT 0 · NOT-VISIBLE 0 · NOT-MEASURABLE 2`, every `[LOW] … CONTAMINATED=n` quoted |
| LG-7 | **PASS** — `frames_shaders_pending 0` vs process-global `pending=7031` |

**Leg B, three regions** (`missing_texture` on `B_Manny_C_12`, si 74→75; 4 of 6 events skipped, camera moved):

| ring | INTERIOR | RING | AMBIENT | RING/AMBIENT |
|---|---|---|---|---|
| 2 px | 97.778 (99.35 %) | 21.529 (56.94 %) | 1.030 (0.99 %) | 20.90 |
| 6 px | 97.778 | 17.746 (38.20 %) | 0.973 (0.88 %) | 18.24 |
| 12 px | 97.778 | 16.401 (31.37 %) | 0.894 (0.75 %) | 18.34 |
| 20 px | 97.778 | 15.369 (27.61 %) | 0.800 (0.60 %) | 19.22 |

**Verdict: NOT ANSWERED.** The ring decays only 21.5→15.4 out to 20 px — far too slow for a thin outline, consistent with the anomaly's own Lumen response — and the pair differs in *both* tagging and appearance, so it cannot separate them. The tag-vs-untag differential, the only design that isolates tagging, had **3 treatment vs 2 control pairs with the control noisier** (ratio 0.48): no power, and structural rather than statistical. Cheapest fixture proposed, not built: `IAI.Bench.CensusMaskDump` — the census already does the readback and reduce, so it's a PNG write, not a new pass.

## 5. State

`master` = **`dc69c51`** = `origin/master`, containing merge **`b0cbf1d`** (two parents `bcdf9f9` + `60c3ddf`; `git diff master branch` empty). Branch `m49-a1-GATE-MISS-firewindow-active` **deleted local and origin**. **No tag, no cook.** Staged exe **`EC60E9B9`** (archived); container quartet `EF8EB23C`/`A8BFFF88`/`3C026A8D` + `A16A18A8`/`C70ECDAA` byte-unchanged, hashed before and after. Main checkout: `master` `dc69c51`, clean but for the owner's untracked handoffs. Lyra worktree: **`fe89e11`, detached, clean** — source byte-identical to what it was built from. CaptureBench `ebab804`. 40 banked dirs incl. every discarded attempt. Journal `docs/sessions/2026-09-04-073-m49-a1-fix.md`; CLAUDE.md status block replaced; gotchas **G240–G244** appended.

Three things worth calling out plainly: I overwrote a banked session's `anomaly_log.txt` with the engine log and **restored it, hash-verified, bank re-verified 127/127**. Two of my own checkers were wrong first — the tie-checker and the region stencil (PIL `multiply` is `a*b/255`, which returned 0.0000 everywhere) — **both caught by known-answer controls before any verdict was read**. And `G141` caught me: `Set-Content -Encoding UTF8` put a BOM on the merge subject; amended before pushing, parents unchanged.

## 6. NEEDS-DECISION

1. **`FramesConditionLost`'s scope** — ruling 1 said "counts independently of it". It is now independent of the `observable` expression and of measuredness, but still conditioned on `IsFireLabelledThisFrame`. Counting `!bHeld` unconditionally would read non-zero on every un-popped `lod_popping` frame (its condition-held *is* its labelled-ness), turning the signal into noise. Recorded as an interpretation in AMENDMENT 1 §A1.3 — **confirm or correct in A2.**
2. **Leg B's rule is not decidable from this data.** A2 should authorise the `CensusMaskDump` fixture before any refuse-or-warn rule for `MPP_Outline` is written — the alternative is writing a rule on a confounded reading.
api-equivalent cost (not billed on Max): $38.07 | turns: 152 | running total across all briefs: $837.67
