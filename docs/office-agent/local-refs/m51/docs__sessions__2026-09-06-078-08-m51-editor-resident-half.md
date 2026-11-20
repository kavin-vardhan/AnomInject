# 2026-09-06 — session 078-08 — `m51` H-editor comparison, RESIDENT half only

**Predeclared before the leg:** `docs/predictions/2026-09-06-m51-editor-comparison.md` (`39afe83`).
**Evidence:** `_bench_sessions_bank\M51_EDITOR_DIAG_EVIDENCE\` (11 files), bank
`M51D_HE_RESIDENT_NAT`. **No source, shader, harness, config, build, cook, Lyra or office action;
no push, merge or tag.** Production frozen at `6713f51`; `master` == `origin/master` == `ac13700`.

## What ran

One diagnostic leg on the archived A-side `B8AC65D8` over the frozen Phase B container, using the
recovered morning recipe (blinking / `StaticMeshActor_49` / seed 4242 / cap 40 / native 1280×720 /
`Marker 1` / natural order / `IAI.Bench.MaskPairingProbe 1`), with the owner's own UE 5.7 editor
(PID 20300, started 18:59:43) resident and untouched. Accepted on **attempt 1**; the 4-attempt pose
budget was otherwise unused. Probe enablement was read back from the run log — spawn at tag 255
plus 40 alternating STEP lines — so the checker judged rather than refused.

## The result, and it contradicts the primary prediction

**RESIDENT PASSES**, at full throughput, with an editor resident.

| leg | time | editor | speed_ratio | fps | masks | decidable | CUR | PREV | NEITHER | verdict |
|---|---|---|---|---|---|---|---|---|---|---|
| `M51D_HI_OLD_NAT` | 18:25 | resident | 3.6412 | 8.24 | 19 | 13 | 4 | 9 | 0 | FAIL |
| `M51D_HI_NEW_NAT` | 18:26 | resident | 3.4256 | 8.76 | 19 | 13 | 4 | 9 | 0 | FAIL |
| `M51D_HE_RESIDENT_NAT` | 19:07 | resident | **1.0053** | **29.84** | 36 | 35 | **35** | **0** | **0** | **PASS** |

Same binary, same container, same recipe, same box, ~1 hour apart. **An editor is resident on all
three rows and the verdict still moves**, so residency is not sufficient to produce the failure —
now measured twice, counting 078-06's `F1A` at ratio 1.0002. 078-06's **throughput** association is
what strengthens: five pairing legs at ≈ 1.00 pass, five at ≥ 1.59 fail. ⛔ Still an association,
confounded on time and throughput; **no mechanism, no threshold, no editor policy** (`G120`).

## The limitation that prices the pass

The resident workload is a **restarted** editor and is measurably lighter: **1.32 cores sustained
across the leg window** (100.2 s CPU / 76 s wall) against PID 25324's **2.63**. It therefore does
not reproduce the original resident condition, and its pass is weak evidence about that condition.

## Consequence

The pair assumed RESIDENT would reproduce the failure so CLOSED could test recovery. It did not, so
CLOSED could now only return `(pass, pass)` — nothing about the failure, at the cost of an owner
save/close cycle. **The pair has lost its discriminating power.** Reported, not worked around.

## Unrun / deliberately not done

CLOSED leg · the 078-07 batching read (no failure to decompose) · any control, load injection or
candidate smoke · every `m51` implementation item, still blocked.

**VERDICT: NEEDS-DECISION** — fixed in advance, because CLOSED is deliberately unrun.

---

# 078-09 — the CLOSED half (appended 2026-09-06, after owner-confirmed closure)

Closure was verified **independently** before staging: zero `UnrealEditor.exe` processes, PID
20300 gone, zero Unreal-like processes of any name, zero `StackOBot` processes. Same A-side
`B8AC65D8`, same frozen container (all five hashes identical to the resident receipt), same
byte-identical runner and checker, same recipe, same predictions file — the editor's absence is
the only intended difference. Accepted on **attempt 1** again, at the identical pose.

## Both halves pass

| leg | accepted | editor | speed_ratio | fps | tpcf | masks m/b/u | decid | CUR | PREV | NEITH | no-data | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| `M51D_HE_RESIDENT_NAT` | 19:07:43 | resident | 1.005284 | 29.842 | 1.375 | 36/4/0 | 35 | 35 | 0 | 0 | 5 | PASS |
| `M51D_HE_CLOSED_NAT` | 19:19:36 | absent | 1.000230 | 29.993 | 1.350 | 39/1/0 | 37 | 37 | 0 | 0 | 3 | PASS |

Gap between accepted legs **11 min 53 s**. Predeclared category: **both pass ⇒ the current
failure is NOT REPRODUCED and the pair discriminates nothing.** The predeclared **inversion**
(resident pass / closed fail) **did not occur** — it was retained and tested, not assumed away.

Closing the editor moved the readings only marginally (`speed_ratio` 1.0053 → 1.0002, fps 29.84 →
29.99). **The box was already at full throughput with the editor resident**, so this pair never
contained the failing condition it was built to test. Against the 18:25/18:26 legs — ratio
3.64/3.43 at 8.2/8.8 fps, same binary, container, recipe and box — the **throughput** association
survives and the **editor-presence** one does not. ⛔ No root cause is asserted (`G120`).

## Two corrections to the RESIDENT half above

🔻 **"CLOSED could now only return `(pass, pass)`" was an OVERSTATEMENT.** The predeclared
inversion remained possible, which is precisely why the full pair was retained. CLOSED did in fact
pass, but that does not retrospectively make the reasoning sound.

🔻 **"1.32 cores vs 2.63" DOES NOT ESTABLISH that the tested workload was half as heavy.**
Measured from each leg's own `labels.jsonl` `t_wall`, the **true capture span is 1.709 s
(RESIDENT) and 1.700 s (CLOSED)**. The 1.32 figure averaged the **76 s between receipts** — about
45× the capture, and mostly engine launch and shutdown — while 2.63 averaged **hours**. Different
quantities over different intervals; neither describes GPU load during the ~1.7 s actually
measured. **Workload equivalence remains UNMEASURED**, and the restarted-editor limitation stands
unchanged on top of it.

⚠ Also carried, unchanged in scope: **failure without `F1` does not exclude an `F1` contribution**,
and current script mtimes are not historical hash receipts. The independently banked 078-07
payload-loss finding stands on its own and is neither repeated nor extended here.

## Still open

Root cause of the 078-05/078-06 pairing failure · why this box ran at ~1.00 tonight and 3.4–3.6 an
hour earlier · **whether closing an editor restores a FAILING run — untested, because no failing
condition was present in either half** · `M-F1b` unrun · pairing release certification and every
`m51` implementation item still blocked.

**VERDICT: NEEDS-DECISION**