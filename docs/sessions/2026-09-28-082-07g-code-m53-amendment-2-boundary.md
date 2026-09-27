# 082-07g — m53 S1: AMENDMENT 2 (the new build, MASKED's PRE row, the halo ring R = 42, G-BIND (3) split), three flagged defaults from the ruling-4 review, the supersede record, the proofs and the boundary re-issue

Session 082-07g, 2026-09-28 (IST), Claude Code (Opus 5.5), headless from the GDP mailbox, fresh session. Rulings:
`_reviews/082-07f-chat-ruling-halo-and-amendment-2.md` (the spec) and `_reviews/082-07e-chat-ruling-masked-fixture.md`.
Branch `feat/m53-uv-normal-corruption`, parent `1395377`. Evidence: `D:\IntrusiveAnomalies\_reviews\082-07g-evidence\`.
**Bench-free: no leg, no build, no cook, no plugin source or CaptureBench change, no tag.**

**Outcome.**
- AMENDMENT 2 is written (`docs/predictions/2026-09-27-m53-s1-legs.md`). It carries everything the ruling listed: the build
  under test `20DA6F98`, MASKED re-running whole with a PRE row proven both ways, the cited evidence, `common.py`'s two staged
  hashes, and the findings.
- **R = 42 px**, measured from the bank before any G-BIND leg: the exact maximum halo extent over 27 pairs. The ruling's
  "48" was a band edge.
- G-BIND (3)'s outside clause is split as ruled: ring < 32, far field ≤ 2, and a per-tile leak reading. It is proven both ways
  on every banked halo pair, and 0 earlier verdicts flip.
- **The ruling-4 review found three more clauses that would stop 082-07h.** Each gets a proven default, flagged for chat:
  - G-BIND (3)'s global half, which the halo reaches from inside the tile;
  - G-BIND (3)'s half-locating step, because the rendered global half is not uniform at ±2;
  - G-RD's drift clause, a within-leg comparison that the tonemapper dither fails by construction (G318).
- G0_FSYN also re-runs first, so admission reads a census of the build under test (A1.4); that is flagged too.

## 1. Bootstrap state

- Feature branch `1395377` == origin, clean. `m51` `53bf725`, `master` `b5f15a3`.
- The staged build equals the 082-07f receipt: exe `2FCDF059`, utoc `20DA6F98`, ucas `534C5863`, pak `FD766B7B`, globals
  `462B8AC6`/`BB05CF99`. The archive `m53-s1-maskedfix-cook-20DA6F98` holds the same 6 files.
- The harness files matched the 082-07d boundary byte for byte before any edit (copies in `082-07g-evidence/harness-before/`).
- The live run had 55 accepted legs, 150 live gate rows and the recorded halt `G-ID MASKED FAIL`.

## 2. R, measured from the bank (`prep/measure_r.py`, `halo-r.json`)

- **Definition:** the extent is the largest Euclidean distance (`scipy.ndimage.distance_transform_edt`) from the event region of
  any outside pixel with max-channel |d| > 2. The event region is the union of both legs' target masks at that
  `session_index`. It is taken over every labelled frame; R = ⌈max⌉.
- **27 pairs:**
  - every accepted wrong-copy attempt (live and superseded) against the null accepted just before it: 23 live, 2 superseded
    U-N1 A1 pairs, and the MASKED pair (d = 0);
  - the 082-07f probe against 082-07e's `NULL_MASKED_A1`.

| pair set | max extent (px) | max \|d\| outside the region |
|---|---|---|
| 25 non-zero wrong-copy pairs | 15 (U-D6 chanswap) | 17 (U-D3 alpha, U-D5 chanswap) |
| 082-07f probe | **42.0** at si 20, pixel (926, 41), d = 3, on `TC_NN1` | 7 |
| probe cross-check: `TC_Masked` rect region, all 100 frames | 42.0 | — |

**R = 42.** The probe profile's 32–48 px band carries 7 pixels > 2 (max 3), and nothing > 2 lies beyond 42.0.

## 3. Harness changes (`_reviews/082-07-lib.py`; `082-07-common.py`)

| change | where | proof |
|---|---|---|
| halo helpers `dist_from`, `halo_extent`, `halo_split`, `halo_ok`, `tile_rects`, `tile_leaks` (`HALO_R` 42, `RING_LT` 32) | lib | P2, P4 |
| G-BIND (3) outside clause: ring < 32, far field ≤ 2, per-tile leak reading, published halo | `ev_bind_layer` | P2a–d, P4a–c |
| G-BIND (3) global half: within R of the layer half < 32, beyond ≤ 2 (**default**) | `ev_bind_layer` | P4a, P4d–i |
| locating TC_Layer's halves: "uniform" = range < 16 (**default**) | `layer_split` | P3a–b |
| G-RD drift phase-matched mod 8 (**default**); the N-vs-1 count kept as a diagnostic | `ev_grd`, `rd_drift` | P5a–e |
| `PRE-CLIP MASKED` (class PRE, legs `[NULL_MASKED]`, before G-ID MASKED) | `pre_clip`, `ev_pre_clip`, `gate_rows` | P1a–h |
| a PRE row reading FIXTURE-CANNOT-EXERCISE stops with code 12; a recorded one keeps a resume at 12 | `evaluate_ready`, `run_sequence` | P1f–g |
| `STAGED` utoc and ucas → `20da6f98…`, `534c5863…` (the diff is two lines) | common | preflight |

Gate rows: 548 → **549**. Legs: 194, unchanged. The legs, runner, preflight, postflight and window are byte-unchanged.

## 4. The ruling-4 review

The full table is AMENDMENT 2 §A2.6. It covers every row not yet run (415 of 549).
- **The halo trips G-BIND (3)'s outside clause** (ruled) and its global-half clause (default A2.6.1).
- **Two clauses fail for other measured reasons:**
  - **G-RD drift:** a within-leg frame N vs frame 1 comparison. On the bank every event with a frame N differs, 2,065–2,544 of
    4,096 px at max 1, while every phase-matched pair reads 0. The G318 rule already named this, but it had only been applied to
    G3.
  - **Locating TC_Layer's halves:** the rendered global half has a horizontal gradient of up to 8 levels on every sampled banked
    frame (708 frames, 59 legs), so no half is "uniform" at ±2. G-BIND (3) would stop INVALID-FIXTURE on the null picture
    alone.
- **Every other not-yet-run pixel clause is region-only or post-revert,** so it is unaffected.

## 5. Proofs (all on the final lib)

| suite | result | file |
|---|---|---|
| AMENDMENT 2 (`proof-082-07g.py`: PRE both ways + through the sequencer; G-BIND split on 27 real pairs + synthetic leaks; locating on 708 real frames; G-BIND end to end on real TC_Layer pixels; G-RD on the bank + synthetic dither) | **33/33** | `proof-082-07g.json` |
| evaluators (the 082-07d suite ported; one expectation changed, the declared cost: global +20 now PASSES, +40 FAILS) | **210/210** | `prep/proof-evaluators.console.txt` |
| admission / G0 / G3 (082-07d suite) | **37/37** | `prep/proof-082-07d.console.txt` |
| sequencer | **20/20** | `prep/proof-sequencer.console.txt` |
| disk | **15/15** | `prep/proof-disk.console.txt` |
| no-flip, pre-supersede: every live recorded row re-evaluated by the final lib | **150/150, 0 flips** | `noflip-pre-supersede-locked-lib.json` (🔻 082-07h citation fix: `noflip-pre-supersede.json` ran on the intermediate lib `16730610`; same 150/150) |
| no-flip, post-supersede, like for like | **138/138, 0 flips** | `noflip-post-supersede-like-for-like.json` |
| dry run, pre-lock | 194 legs, 549 rows, **0 problems**, 0 tokens absent | `prep/dryrun-pre-lock-082-07g.json` |

- **The post-supersede re-read against the current census reads NO-CENSUS on the four standing ADMIT rows** (U-N1, N-N1),
  because `G0_FSYN` is superseded pending its re-run. That file is kept as `noflip-post-supersede-current-census.json`.
  - The harness never re-evaluates a recorded ADMIT row.
  - Re-read against the census each row recorded, all four stay ADMITTED (G327).
- **The post-lock dry run, the preflight negatives, the preflight replay and the end-to-end window proof** run after the
  boundary; their results are in the 082-07g report.

## 6. The supersede record (entry 3)

- **Entry 3 supersedes:**
  - `G0_FSYN_A2`, `ID_MASKED_A1`, `NULL_MASKED_A1` and `WC_MASKED_ALPHA_A1`;
  - their 12 live rows, including the halt `G-ID MASKED FAIL`;
  - the never-evaluated rows, which are named: `TRIP` and `G11 WC_MASKED_ALPHA`.
- **Four aliases** were renamed `…__SUPERSEDED_082-07g`, with the hardlinks verified before and after.
- **After the act:** 51 live accepted legs, 138 live rows, no halt, no ready-but-unevaluated row, record `85fb48dd…`.
- **Nothing is deleted.**

## 7. Findings

- **The halo** reaches inside a tile as well as around it. Mechanism open; the cause read goes to S2. For the S4 client docs:
  masks do not include the few-pixel screen-space halo that a visible anomaly casts on its surroundings.
- **TC_Layer's global half renders with a gradient of up to 8 levels.** Cause not established.
- **Gotchas G325–G327.**

## 8. Hand-off

- **082-07h** resumes with a new budget tag (the command is in the report). It runs G0_FSYN (A3), then MASKED whole (the PRE row
  is read before the wrong copy), then SHARED onward: 143 legs.
- **Chat decides the three flagged defaults and A2.3** before 082-07h. Rejecting any of them means a boundary re-issue.
