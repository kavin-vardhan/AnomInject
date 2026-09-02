# 2026-09-02 — session 066 — Bates results (Sections A + B) and the m35 remote close-out

**Short-form journal: owner-run results typed back from Bates, recorded verbatim in substance; one
new phenomenon minted; the wip backup deleted and the scrub re-verified over every remaining origin
ref. Docs only — no code, no build, no tag.**

## §1 Section A — m35 VALIDATED ON BATES. PASS.

**90/90 frames on disk, rect Y-origins 69 AND 138 exercised, no crash.** Two distinct non-zero
letterbox origins on the host that produced the original crash — the condition m35 exists for,
reached for real.
⚠ **Honest limit, recorded as the owner typed it: the guard/clamp counters were UNREAD — not
zero.** The pass rests on frames + origins + no-crash; the counters are an unread surface (G197's
shape) and are not claimed.

**Consequence executed per RDP card A-8:** `wip/session-061-backup` deleted from **origin AND
local** (remote-tracking ref pruned too; `for-each-ref` finds no `wip` ref anywhere). The scrub
verifier then ran over **all three remaining origin refs**, each materialized in a read-only
worktree, self-test firing on the synthetic fixture before every verdict:
`origin/master` clean over 197 files · `origin/feature/selection-census` clean over **196** files ·
`origin/feature/mask-gpu-reduce` clean over 189 files (Build.cs permanent exclusion listed on each).
⚠ The first selection-census read reported *clean over 0 files* — **a vacuous verdict, discarded
and re-run**, not a pass (G151/G189's shape; the re-run is the reading).
✅ **"No reachable ref, origin or local" is ACHIEVED** — the codename invariant now holds over
everything reachable.

## §2 Section B — the two-leg pair on Bates. THE YIELD PROBLEM IS CURED.

| leg | eligible pool | owner's eye judgment |
|---|---|---|
| floor **6.0** (shipped default) | **2** — SnowLandscape (34 %), player character (7 %) | pitch-black frames + the same 2–3 objects repeating |
| floor **0.5** | **8** | **~90 % of anomalies eye-visible** |

**`vetoed_events` = 0 on BOTH legs, against the banked Bates band of 12–15 per run.** The census
removed at selection time what the veto used to delete after the fact — **the Bates yield problem
is cured**, and the backstop had nothing left to catch.

🔴 **FLOOR DECIDE: PENDING WITH THE OWNER.** Chat's recommendation on the table: **floor 0.5 plus a
CEILING of 25** (SnowLandscape at 34 % is the pitch-black-frame producer — too big to be a useful
target). ⛔ **The ceiling is UNBUILT** — no knob exists for it; if adopted it is m37-adjacent work
with its own gates, not a tweak.

## §3 🆕 P9 MINTED — blinking annotation/observation mismatch on Bates (owner-observed)

⚠ **The instruction said "mint P8"; P8 IS TAKEN** (2026-08-18, TAU pose-invariance) and phenomenon
numbers are NEVER reused — so this is minted as **P9**, the verified next free number. Deviation
from the letter of the instruction, stated.

**P9:** `blinking` only, Bates, **owner-observed, 3 instances across BOTH Section B legs**: the
annotation's hidden set reads **{42,43,47,48}** while the observed hidden frames are
**{42,43,44,48}** — one frame differs inside the window, not a uniform shift. ⛔ **NO MECHANISM IS
CLAIMED** (candidates exist in the record — m20's one-tick-stale hidden state was FIXED, the m31
family — and none is asserted; G120). **The MEASUREMENT is owned by the NEXT session:** it needs a
pixel-ground-truth leg (the m18/m20 instrument class), not a re-read of the same eyes.

## §4 Also recorded

- **`ticks_per_captured_frame` = 1.3556 on Bates — UNEXPLAINED, QUEUED.** (Numerically equal to
  this bench's own unpinned home baseline from session 051; noted as a coincidence to check, not an
  explanation.)
- The owner's write-up arrives as `docs/CHAT-HANDOFF-m36-census-and-bates-results.md` (untracked
  owner doc — never staged, per the standing add rule).
- Open queue: floor DECIDE (owner) → P9 measurement (next session) → m37 predictions carrying
  journal 065 §11 → the physical-visit tag sequence m31→m33→m34→m35 after G-R7(ii).

---

# APPENDED 2026-09-02, session 067 — the Bates figures this journal summarised but did not record

⛔ **NOTHING ABOVE IS EDITED.** This block adds the numbers §1 and §2 stated in prose, copied from
the owner's write-up (`docs/CHAT-HANDOFF-m36-census-and-bates-results.md` §3, **untracked**). The
journal is the durable record and the handoff is not tracked, so a figure that lives only there is a
figure this repo does not have.

## §A.1 Section A — the `READBACK-LAYOUT` line, as typed back

```
Capture(sve): READBACK-LAYOUT sourceExtent=1170x765 rect=(0,138)-(1170,628) picture=1170x490 bufferHeight=490 rowPitchInPixels=1216 fmt=18
```

- That leg: `total_frames` **90**, files on disk **90**, no crash.
- **Two further Section-B runs on the same path, also 90/90**, at `rect=(0,69)-(1170,559)` with
  `sourceExtent=1170x627` — the window was resized between runs, so **both** legs carry a non-zero
  `Rect.Min.Y` (**138** and **69**) and neither is the degenerate zero-origin case (`G192`).
- ✅ `picture` equals `rect` width × height on the quoted line (1170 × 490), and `bufferHeight`
  equals the picture height — expected **by construction** post-m35, since the plugin owns the
  staging texture (`AnomalyFrameCapturer.cpp:70-82`). So that equality is a consistency read, **not**
  the discriminator. The discriminator is `rect` inside `sourceExtent`, which holds on both.
- ⚠ **`READBACK-GUARD` and `EXTENT-CLAMP` counters were NOT read.** Recorded as **UNREAD, not zero**
  (`G197`'s shape). One command next RDP visit — RDP card **Section C, item (a)**.
- ⚠ **No `bRenderCustomDepth` line was seen**, and the census `reserved=` list **was cut off in the
  photo**. Both are re-asks on the card, **C-(b)** and **C-(c)**.

## §A.2 Section B — the twelve counters per leg, both legs

| field | leg 1 (floor **6.0**, compiled default) | leg 2 (floor **0.5**, console) |
|---|---|---|
| `census_frames` | 90 | 90 |
| `census_cycles` | 28 | 29 |
| `census_candidates` | 62 | 63 |
| `census_zero` | 37 | 38 |
| `census_below_floor` | 21 | 15 |
| `census_excluded_translucent` | 1 | 1 |
| `census_unmeasurable_hidden` | 2 | 3 |
| `census_unmeasurable_nanite` | 0 | 0 |
| `census_unmeasurable_tag_failed` | 0 | 0 |
| `census_fires_fallback_all` | 0 | 0 |
| **`vetoed_events`** | **0** | **0** |
| eligible at that floor | **2** | **~8** |

**Shared across both legs:** `pattern_excluded_targets` **112** (pre-existing ini patterns) ·
`mask_nopass_discards` **2** · key ring clean (**126/126** and **121/121**, `missed` 0) ·
`wanted_matches` **90** · `tickpin_compiled` **false** (expected — the tick-pin's fork is Concorde's,
not Bates') · `capture_game_ticks` **122** · `ticks_per_captured_frame` **1.3556**.

📌 **`census_unmeasurable_nanite` = 0 on both legs is the addendum's EXPECTED reading**, not an
absence of evidence: Bates has no Nanite at all, so a non-zero there would have meant the classifier
was misfiring.

## §A.3 The drawn-coverage histogram — stable across cycles, and it IS the floor-decide input

```
zero=37–38   (0,1]=16–17   (1,3]=3   (3,6]=1   (6,12]=1   (12,25]=0   >25=1
```

Named candidates and their drawn share of the view rect:

| actor | drawn % |
|---|---|
| `BP_SnowLandscape_Test_Child_C_1` | **≈ 34.0** |
| `BP_BatesCharacter_Female_C_0` | ≈ 6.98 |
| `StaticMeshActor_1246` | 5.65–5.69 |
| `StaticMeshActor_1158` | 2.73 |
| `StaticMeshActor_1140` | 1.73 |
| `StaticMeshActor_55` | 1.21 |
| `StaticMeshActor_1550` | 0.88 |
| `StaticMeshActor_54` | 0.74 |
| `StaticMeshActor_1332` | 0.33 |
| `StaticMeshActor_84` | 0.31 |
| `BP_bushlarge_…_Snowy_UP_C_1` | 0.29 |
| `StaticMeshActor_1184` | 0.27 |
| `StaticMeshActor_1599` | 0.23 |

**Read the shape, not the rows:** ~60 % of everything in view draws **zero** pixels and another
~27 % draws **under 1 %**. At floor 6.0 the survivors are the landscape blueprint and the player
character — which is exactly the owner's eye report of pitch-black frames and the same 2–3 objects
repeating. ⛔ **No floor is recommended here.** The two distributions are the input to a decision the
owner has not yet made.

## §A.4 This journal's own scrub claim, RE-VERIFIED 2026-09-02 (session 067)

Re-run over all three origin refs in fresh read-only worktrees, self-test firing before every
verdict, worktrees removed and pruned afterwards:

| ref | worktree HEAD | verdict |
|---|---|---|
| `origin/master` | `a73f87f` | **clean over 198 files** (2 binary skipped, 1 printed exclusion) |
| `origin/feature/selection-census` | `7f82d52` | **clean over 196 files** |
| `origin/feature/mask-gpu-reduce` | `7151875` | **clean over 189 files** |

⚠ **Master's denominator moved 197 → 198 and that is the CORRECT direction:** §1's run predates
commit `a73f87f`, which added this journal. A denominator that grew by exactly the one file added is
the reading; a denominator that had **shrunk** would have been the finding (`G191`).

🚨 **NEW OBSERVATION, filed here because it is a property of the INSTRUMENT, not of any ref: the
verifier's mandatory self-test PRINTS THE DECODED TERM TABLE TO STDOUT** on every invocation — that
is precisely how it proves it fires. The terms are stored base64-encoded on disk, so journal 064's
claim (*no plaintext copy exists in any file*) is intact — but **any captured console log of a
verifier run IS a plaintext copy**. ⛔ Never paste raw verifier output into a doc, a journal, a
commit message or a handoff; quote the `SELFTEST ok` and `VERDICT:` lines only. **CAUSE IS UNDERSTOOD
AND NO FIX IS PROPOSED** — suppressing the echo would weaken the very proof it exists to give.
