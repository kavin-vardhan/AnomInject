# REPORT for 076-01-m50-step0-diagnosis-and-implement.md
brief: 076-01-m50-step0-diagnosis-and-implement.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-04 18:56:06 | finished: 2026-09-04 20:16:45 | duration: 81 min | exit code: 0
context at last turn: 591858 tokens (59% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\076-01-m50-step0-diagnosis-and-implement.live.txt

---- FINAL MESSAGE (Claude Code) ----
Mailbox restored to as-found. `m50` is shipped, gated and pushed.

---

## 1. Plain-language rundown

1. **The diagnosis found something different from what the plan expected, and that changed the fix.** The stencil-collision defect (`G246`) comes from **one line**: when the tag allocator runs out of values, it hands out one it has *just proven* is already taken.
2. Proof went from correlation to certainty: exhaustion count tracks defect count leg-for-leg; every affected frame carries a value that line re-issued, exactly 3 ticks earlier; and once I made the line print its own bookkeeping it read `Re-assigning 230, which is NOT FREE: censusClaimed=[…,230,…]`.
3. **The planned fix (a "quarantine") is therefore not built** — it shrinks the pool that was already running out, so it would have made this *more* frequent. The plan had priced that risk in advance; the measurement landed on the price rather than the benefit.
4. **What shipped instead:** the census (which can take less) leaves 8 values free for the event allocator (which cannot); and when the pool does run out, no tag is issued and the event ships honestly labelled "not measured" instead of carrying a wrong pixel count.
5. **Second fix, ruling 3:** a target we know we cannot measure (Nanite) is now skipped rather than measured-as-zero-and-deleted. On a real game this turned **0 delivered anomalies into 6**.
6. **My first version of the single-owner gate exonerated a real collision I had deliberately planted.** It was reading the allocator's records, and the planted intruder appeared in none of them. Rebuilt to read the live tag state — and it is ~7× more sensitive than the old proxy.
7. **All ten gates pass in both tick orders**, and the can-fail lever fires 22 times in each, so the zeros are readings rather than blindness.
8. **Two corrections to the record**, both declared before any result was read: the previously reported "22 of 448 (4.9 %)" does not reproduce — it is **12 of 448, all on one binary** — and the instrument that produced that number under-reads reality about sevenfold.
9. **No tag, no cook.** The container is byte-unchanged; this rides the code-only hot-swap.
10. One honest limitation stated up front: with 55 stencil values and one permanent value per event, a long enough run still exhausts the pool. The fix removes the measured cause and changes what happens at the ceiling; it cannot remove the ceiling.

## 2. Step 0 — the branch table

| source | tag-instances | multi | **D1** | D2 | D3 | D4 | unexplained |
|---|---|---|---|---|---|---|---|
| 16 banked G-EDGE legs (`EC60E9B9`+`8839D806`) | **448** | 12 | **12** | 0 | 0 | 0 | **0** |
| `S0_BL_NAT` / `S0_MO_NAT` (instrumented, natural) | 48 | 0 | — | — | — | — | — |
| `S0_POOL8_BL` (lever) | 16 | 6 | **6** | 0 | 0 | 0 | **0** |
| `M50PRE_B_CLEAN` (**no lever**) | 16 | 1 | **7 frames** | 0 | 0 | 0 | **0** |

**Branch = `D1`** — `AllocateTag`'s `TAG-POOL EXHAUSTED` fallback. D2/D3/D4 refuted (`tagOvertaken` 0 on every leg).

## 3. Gates

| # | result |
|---|---|
| G1 | ✅ 0 violations / 1,180 owner-checked frames, both orders (+90 Lyra) |
| G2 | ✅ **22 NATIVE, 22 SYNTH** — gate proven able to fail |
| G3 | ✅ 0 exhaustions, 0 violations vs **1 exhaustion / 7 frames** pre-fix |
| G4 | ✅ TARGET-PIXELS TIE 0 MISMATCH (pre-declared as *not* evidence for G1) |
| G5 | ✅ `m44_gates` ALL PASS stray 0 (10 legs) · onset-join 0 · OBS-1 delta 0 · pairing `NEITHER 0·PREVIOUS 0` ×4 incl. SP50 · black-frame PASS, DARK FIRST 0 · A44 both encodings · both targets exit 0, zero warnings |
| G6 | ✅ 0 `TAG-POOL EXHAUSTED` everywhere, incl. `TagPoolLimit 8` (headroom fired 108×) |
| G7 | ✅ 4 events delivered, `target_pixels -1` 32/32, `observable null` 32/32, `vetoed 0` |
| G8 | ✅ A-side `vetoed 4`, `anomalies: []` — `LG-9` shape reproduced on the bench |
| G9 | ✅ Lyra 0 → **6 events**, all `observability_measured: false` |
| G10 | ✅ 8/8 pairs: added 0 removed 0; `run_summary` 64→65 exactly; `frame_indices` identical 4/4 |

## 4. Lyra G9

`L_Convolution_Blockout`: 90 frames / 43 positive / 7 bursts. `vetoed_events` **6 → 0**; events **0 → 6**; `unmeasurable_targets_admitted` 6; `census_unmeasurable_nanite` 61; TAG-OWNER violations 0 and exhaustions 0 over 90 frames. `census_candidates` moved 63 → 95 and is **not attributed to m50** — batch composition changes by construction and the count was deliberately not pre-declared.

## 5. Commits, hashes, checkouts

- Plugin `master` `e9203ff` → **`21c1e0d`**, pushed, in sync, 0 tracked modifications. `40a5202` (instrument) → `e660320` (predictions, **before any gate leg**) → `7ecdf5f` (fix) → `21c1e0d` (docs). **No tag.**
- CaptureBench (local-only) `f35949e`.
- Staged exe **`A159EDFD`**; archived. Load-bearing: `9FAF70D5` (pre-fix A-side), `8839D806`, `EC60E9B9`.
- Container quartet **byte-unchanged** (`EF8EB23C`/`A8BFFF88`/`3C026A8D`+`A16A18A8`/`C70ECDAA`).
- Main checkout on `master` throughout. Lyra worktree `328ca28` → `7ecdf5f`, detached, clean both ways, never committed to.
- Pointers: `docs/sessions/2026-09-04-076-m50.md`, `docs/predictions/2026-09-04-m50.md`, gotchas **G251–G254**.

## 6. NEEDS-DECISION

1. **`m48`'s `exposure_dip` fires on hide-class anomalies at pinned exposure** — because the hide itself removes a bright object and drops luminance >4 %. That is the anomaly, not the game's auto-exposure, which is what the key is named for. Pre-existing (present on both sides of the A/B), out of `m50`'s scope, **filed not fixed**. Chat decides whether it earns a milestone.
2. **Process note against me, not a finding:** I wrote two temporary commit-message files into `_mailbox`, which the standing rule forbids. Both removed; the mailbox is verified as-found. No brief file was read, moved or staged.

---- STDERR ----
Background tasks still running after 600s; terminating. Set CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 to wait indefinitely.

api-equivalent cost (not billed on Max): $62.20 | turns: 233 | running total across all briefs: $1,005.86
