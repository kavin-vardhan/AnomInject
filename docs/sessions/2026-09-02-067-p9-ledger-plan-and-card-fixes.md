# 2026-09-02 — session 067 — 066's wrap-up verified · `P9` opened and planned · RDP card fixed

**Docs and measurement-planning only. NO code, NO build, NO leg, NO tag, NO cook.** One commit is
authorised (the §1 verification fixes); everything else is drafted and **held for chat's OK** before
it is committed, per the session brief.

---

## §1 Verification of session 066's wrap-up — PASS/FAIL with evidence

### §1.1 Tree anchor — ⚠ **PASS WITH A CORRECTION TO THE BRIEF**

```
git status --porcelain   -> only the three untracked owner CHAT-HANDOFF docs (expected, never staged)
git rev-parse HEAD       -> a73f87f030a5f35dbcd511008c2d234ee58fbd54
git rev-parse origin/master -> a73f87f030a5f35dbcd511008c2d234ee58fbd54
```

**HEAD == origin/master, both directions, at `a73f87f`.**

🚨 **The brief expected `27dfba3` both ways. It is NOT the tip and it is not supposed to be.**
`27dfba3` is **two commits back**; `f6d8863` (the m36 bench close-out) and `a73f87f` (journal 066)
landed after the handoff was written:

```
a73f87f  docs: session 066 - Bates Section A+B results, m35 remote close-out, P9 minted
f6d8863  docs(m36): bench-phase close-out - rulings recorded, G194-G201 minted, m37 carry-forwards
27dfba3  docs: correct the stop block - master no longer carries the readback crash   <- the brief's SHA
```

⇒ **The tree is AHEAD of the brief, not divergent from it.** Nothing is missing and no action is
taken. ⚠ **This is the third stale-SHA reading in three sessions** (`CLAUDE.md` item 2 carried
`9c94c55` for the same reason). Fixed in the direction that stops it recurring: the status block now
says the **invariant** — *master is the m34+m35+m36 merge line, and its tip is whatever
`git rev-parse origin/master` says* — beside the SHA.

### §1.2 Remote refs — ✅ **PASS**

```
715187558abda8eb5f51abb1d7982b278580814c    refs/heads/feature/mask-gpu-reduce
7f82d52f03c65d848c119d46ca081f7bbc92694a    refs/heads/feature/selection-census
a73f87f030a5f35dbcd511008c2d234ee58fbd54    refs/heads/master
```

**Exactly master + the two feature branches**, as the handoff requires.
✅ **No `wip/*` on origin. `git branch --list 'wip/*'` locally returns NOTHING.** The 066 close-out
held — no verification of `wip/session-061-backup`'s contents was needed because the ref does not
exist in either place. §1.3 confirms the consequence.

📌 **Two readings the brief's wording would not have predicted, REPORTED, NO ACTION TAKEN:**

1. **`feature/stencil-capture` is at `76cac74` — but LOCAL ONLY. It is not on origin.** The brief
   said "present at `76cac74`"; it is present, at that exact SHA, in the local ref list. It has
   never been pushed. ⛔ Not checked out, not deleted, not touched.
2. Two further local branches exist whose upstreams are gone:
   `m29-GATE-FAILED-lod-popping-invisible` (`ab2fb41` `[gone]`) and
   `s3a-2-GATE-FAILED-do-not-merge` (`087f4d9` `[gone]`). Both are named as failed gates, both are
   pre-existing. ⛔ **No action.**

### §1.3 Scrub verifier over ALL origin refs — ✅ **PASS**

CaptureBench `28249c5`. Three fresh read-only worktrees, removed and pruned afterwards; the
mandatory self-test fired before every verdict.

| ref | worktree HEAD | verdict |
|---|---|---|
| `origin/master` | `a73f87f` | `VERDICT: clean over 198 file(s)` — 2 binary skipped, 1 printed exclusion |
| `origin/feature/selection-census` | `7f82d52` | `VERDICT: clean over 196 file(s)` |
| `origin/feature/mask-gpu-reduce` | `7151875` | `VERDICT: clean over 189 file(s)` |

Every run opened `SELFTEST ok: fires on a synthetic all-terms fixture, and the mapping clears it.`
and listed `EXCLUDED Source/AnomalyCapture/AnomalyCapture.Build.cs`.
✅ **"No reachable ref, origin or local" HOLDS.**

⚠ **Master's denominator moved 197 → 198**, which is the **correct** direction: 066's run predates
`a73f87f`, which added one file — the 066 journal. A denominator that had **shrunk** would have been
the finding (`G191`).

🚨 **NEW OBSERVATION — the verifier's self-test PRINTS THE DECODED TERM TABLE TO STDOUT.** That is
how it proves it fires, so it is correct behaviour. But it means **any captured console log of a
verifier run is a plaintext copy of the scrub terms**, even though the table is base64 on disk.
⛔ **Never paste raw verifier output anywhere. Quote the `SELFTEST ok` and `VERDICT:` lines only.**
**CAUSE UNDERSTOOD, NO FIX PROPOSED** — suppressing the echo would weaken the proof. Filed in
journal 066 §A.4 as a property of the instrument.

### §1.4 Journal 066 vs handoff §3 — ⚠ **FAIL, FIXED BY APPENDING**

066 recorded the **conclusions** and omitted most of the **figures**. Missing and now appended as
**§A.1–§A.4** (existing text untouched, nothing rewritten):

- the `READBACK-LAYOUT` line in full (`sourceExtent=1170x765 rect=(0,138)-(1170,628) picture=1170x490
  bufferHeight=490 rowPitchInPixels=1216 fmt=18`) and the two further 90/90 runs at
  `rect=(0,69)-(1170,559)`, `sourceExtent=1170x627`;
- the twelve counters per leg (frames/cycles/candidates **90/28/62** and **90/29/63**;
  zero/below_floor/translucent **37/21/1** and **38/15/1**; unmeasurable-hidden 2 and 3;
  nanite/tag_failed/fires_fallback_all all 0; `vetoed_events` **0** and **0**; eligible **2** vs **~8**);
- the drawn-coverage histogram and the named-candidate percentages;
- the shared values — `pattern_excluded_targets` 112, `mask_nopass_discards` 2, key ring 126/126 and
  121/121, `wanted_matches` 90, `tickpin_compiled` **false**, `capture_game_ticks` 122.

✅ Already present in 066 and left alone: guard/clamp **UNREAD, not zero**; `ticks_per_captured_frame`
1.3556 flagged **UNEXPLAINED**; the P9 mint and its P8-is-taken deviation.

### §1.5 `CLAUDE.md` status block — ⚠ **FAIL, FIXED IN PLACE**

| item | before | after |
|---|---|---|
| master's SHA | `9c94c55` (**two commits stale**) | `a73f87f`, **plus the invariant** so the next staleness is a known shape |
| what master carries | "m35 AND m36" | "**m34**, m35 AND m36" — the m34 merge was already there and unnamed |
| tag sequence (2 places) | `m31 → m33 → m34 → m35` | `m31 → m33 → m34 → m35 → **m36**` |

✅ Already correct, left alone: census compiled default **OFF** and client-inert by measurement
(`P-C7`); **NO TAGS**, highest `m30`; the stop block no longer claims master carries the readback
crash.

### §1.6 Environment — ✅ **PASS, and the trash folder is GONE**

All junctions resolve to live targets:

| link | target |
|---|---|
| `StackOBot\Binaries` | `E:\IA_BuildCache\StackOBot\Binaries` |
| `_binary_baselines` | `E:\IA_BuildCache\_binary_baselines` |
| `_bench_sessions_bank` | `E:\IA_BuildCache\_bench_sessions_bank` |
| `StackOBot\Builds` | `E:\IA_BuildCache\StackOBot\Builds` |

*(`Saved` and `Intermediate` are junctioned to E: as well — not on the brief's list of four, noted.)*

**Free space: D: 190.1 GB · E: 416.5 GB · C: 33.2 GB.**
✅ **`Binaries` lives on E: with 416.5 GB free ⇒ GO** by the runbook rule (<10 GB = NO-GO for any
link). The disk crisis the handoff describes is fully resolved.

📌 **`D:\IntrusiveAnomalies\_TRASH_pending_owner_delete` DOES NOT EXIST.** Searched `D:\`,
`D:\IntrusiveAnomalies` and `E:\IA_BuildCache` for any `*TRASH*` directory — none. ⇒ **the owner has
already deleted it**, which is the outcome the runbook wanted. `setup-runbook.md:377` still points
at it and is now stale; ⛔ **not edited this session** — it is outside §1's authorised commit.

### §1.7 The staged binary — ✅ **PASS, and a RECORD GAP FOUND AND FILLED**

```
D:\IntrusiveAnomalies\StackOBot\Builds\BenchGate\Windows\StackOBot\Binaries\Win64\StackOBot.exe
sha8  D2BB25A5      size 241,026,048      mtime 2026-09-01 23:27:39
```

Container at the same path re-verified: `utoc 2A66CA57` · `ucas A7EF9B12` · `pak D8009AD7` ·
`global.utoc C70ECDAA` · `global.ucas A16A18A8` — **the unchanged m34 quartet**, so `D2BB25A5` is a
code-only hot-swap (`G103`), as recorded.

🚨 **`_binary_baselines\README.md` HAD NO ENTRY FOR IT — NOR FOR SIX OTHER ARCHIVED EXES.** The
files were archived correctly and all seven re-verify by hash, but the README's tables stop at m35
Build B. By the file's **own deletion rule** they were undeletable-but-undefended: nothing recorded
what depended on them.

**Fixed:** a new table for `6B579F91`, `1F6A2188`, `02C1DFA2`, `E046D1CA`, `CBBF6644`, `70F6B72C`,
`D2BB25A5` with **hash + size + what depends on each** (7/7 re-hashed and re-sized at the location),
plus the `G201` note that `70F6B72C` and `D2BB25A5` share a size and differ in hash on
byte-identical `Source/`, plus a stated gap: **there is no m36 quartet — the container was never
re-cooked.** ⚠ `README.md` is **outside version control** (`G112`), so this fix rides no commit;
that is the same property that let it drift.

---

## §2 `P9` — the ledger entry

Written into `docs/invisible-anomaly-mechanisms.md` as **§8**. Status OPEN, class *labelling ↔
manifestation mismatch at hide boundaries*, `blinking` only, `missing_object` not reported affected,
3 owner-observed instances across both Section B legs, full run configuration, the one transcribed
instance, why it is **not** `P1`'s constant shift, what is not known, the tick-ratio co-observation,
**no mechanism**, the mitigation, and the `P1`/`P3`/`P5` cross-references.

🔢 **THE NUMBER IS `P9`, NOT `P8`, AND THE DEVIATION IS DELIBERATE.** The brief said P8 twice; `P8`
has been taken since 2026-08-18 (TAU is not pose-invariant) and numbers are never reused. Journal
066 §3 already minted this as P9 and `CLAUDE.md`'s ledger already carries P9 — so the entry
**completes that mint** rather than opening a second one. **Both chat handoffs are wrong about the
number and right about everything else.**

📌 One source verification folded into the entry: `47 − 44 = 3` **is** one half-period at the
blinking default — `DefaultHalfPeriodFrames = 3`, `Anomaly_Blinking.h:28`, bound by `static_assert`
to `AnomalyDefaults::BlinkingHalfPeriodCompiled` so the echoed and used numbers cannot diverge.
⛔ **Recorded as arithmetic of the observed numbers. It is not a mechanism and names no code path.**

---

## §3 The `P9` measurement plan

`docs/predictions/2026-09-02-p9-blinking-boundary-repro.md`, pre-declared, **no leg has run**.

### §3.1 🚨 THE FINDING THAT SHAPED IT: **the A54 oracle cannot see `P9`, and it fails toward "clean"**

`a54_oracle.py:135` — `SHIFTS = (-1, 0, 1)`. A54's whole hypothesis space is **uniform displacement
of the claimed set**. `P9`'s signature is a **set difference in two directions at once**, which no
shift expresses. On the transcribed instance three of four claimed frames really are hidden, so
`score(0)` stays far above `TAU` while `score(±1)` collapses ⇒ **A54 returns `ALIGNED` on a `P9`
event.**

⇒ **Running A54 alone and reporting "not reproduced" would be a false negative dressed as a
certification** — `G192`'s shape on a new axis, and a repeat of `G106`/`G142`/`G172`. The plan
therefore specifies a **new instrument that reports the OBSERVED HIDDEN SET ITSELF**
(`CaptureBench/tools/p9_hidden_set.py`), with A54 **demoted to ruling out `P1`**.

### §3.2 The A53 gate — RUN AND PASSED TODAY, before the plan was finished

| control | result |
|---|---|
| known-ALIGNED `M23\R30_regress` | **12/12 ALIGNED, 12/12 decidable**, median \|margin\| 0.10478, `ALL-ALIGNED`, exit 0 |
| known-ABSENT `I10HF\HF1_nat120` | **12/12 ABSENT, 0/12 decidable**, median 0.000133, exit 1 |
| positive control `R30_regress --shift 1` | **12/12 `SHIFTED(-1)`**, margins collapsing 0.105 → 0.050 |

Both legs read `A56` CERTIFIABLE (modal 100.0 % of 99 rows, 1 distinct) and `B1` pose-match YES.
**`TAU` is already frozen at 0.04684 in the file and was not re-derived.**

⚠ **ONE DISCREPANCY, REPORTED NOT CHASED:** the oracle's header claims it reproduces *"the published
R30 median (0.10737)"*. Today: **0.10478** on `R30_regress`, **0.109008** on `M23B\R30_recert`.
**Neither is 0.10737. CAUSE NOT ESTABLISHED.** The A53 gate is unaffected — it turns on
ALIGNED-vs-ABSENT and that separation is total. ⛔ Nobody should cite 0.10737 as reproduced.

### §3.3 The rest of the plan, in one line each

- **Config** Bates-shaped, every value by read-back (`A48`); letterbox lever ON with `minY > 0`
  asserted from the lever's own line, on **`MainWorld`** because it refuses on `CB_GateLevel`'s
  `SpectatorPawn` (`G193`).
- **Legs** paced 30 **and** unpaced, targeted (oracle-clean) **and** auto-pool (Bates fidelity), exe
  `D2BB25A5`; the family-A target is chosen from a dry leg's histogram and **recorded before any P9
  leg**, excluding Nanite actors (`G134`).
- **Discriminators** REPRODUCED / NOT-`P9`-constant-shift / ONE-DIRECTIONAL / NOT REPRODUCED /
  UNDECIDABLE, pre-declared; NOT REPRODUCED requires **both** A54 ALIGNED and empty set differences.
- **Delivery mode** OFF on oracle legs, with a bracket leg at ON. ⚠ **The brief's premise that
  delivery suppresses the `labels.jsonl` bbox is WRONG from source** —
  `bWriteLabelsInDeliveryDefault` is compiled **ON** (`AnomalyCaptureSubsystem.cpp:3591`); what
  delivery suppresses is `run.json`. Same conclusion, correct reason.
  ⛔ **Bates' own delivery setting for Section B is NOT RECORDED anywhere** — hence the bracket.
- **Tick ratio** derived from source: it is exactly `capture_game_ticks / total_frames`
  (`AnomalyLabelWriter.cpp:546-548`), and 122/90 = 1.3556. **The unexplained part is the 32 surplus
  ticks**, for which the settle phases and `DrainTail` account arithmetically (~34) — offered as a
  hypothesis to measure, not a finding.
- **Census-vs-fired-target** source-verified: four guards cited at file:line; the engine-side
  question (what a deferred proxy recreate does to a frame) is named as **NOT decidable from plugin
  source**, with the searched surface stated (`G136`).

---

## §4 RDP card fixes

| fix | what changed |
|---|---|
| **A-6** | the two counters become **one command** printing four labelled lines including the log path and MTIME, so `= 0` and "did not look" can never be reported the same way |
| **A-7** | 🚨 **the old instruction could not answer its own question** — grepping `bRenderCustomDepth` matches the reservation line on every healthy run (`AnomalyCaptureSubsystem.cpp:1423`). Replaced with a command counting the **six named collision signals** |
| **B-2** | the `reserved=` line's **exact text** and **exact moment** (StartRun), plus a command printing only the short front half **and** the independent `hostReserved=` cross-read |
| **A-8** | tag order gains `m36`; Section A recorded as **DONE** with counters 3/4 explicitly outstanding |
| **new Section C** | "next RDP visit": (a) the counters command · (b) the `reserved=` line · (c) the collision question · (d) one glance at a leg-1 black frame · (e) **standing mitigation: blinking unticked on Bates until `P9` closes** · (f) optional clean-dataset repeat. The former "C. IF SOMETHING FAILS" is renamed **D**, unchanged in content |

---

## §5 DRAFT — the one-event typed ask for Bates. ⛔ NOT SENT. NOT ON THE CARD.

**Used only if the bench cannot reproduce `P9`.** Asking the owner to hand-transcribe what the bench
could produce is the wrong order of work, and the owner is worn out on this bug class. Held here.

> **One `blinking` event, from either Section B leg. Four things, all from files already on that box
> — no new run.**
>
> 1. From `annotation.json`, the **whole event object** for one blinking event: `anomaly_type`,
>    `affected_frames.start_frame`, `end_frame`, `frame_count` and the full `frame_indices` array,
>    and the `nodes[]` entry's `name`.
> 2. From `labels.jsonl`, **every row whose frame index falls in that span, plus one row either
>    side** — whole lines, copied, not summarised.
> 3. From the Output Log, every line mentioning that target between the fire and the revert.
> 4. The **`session_index` ↔ `frame_index` mapping** for that span: for each PNG in
>    `Actual_Frames\`, its filename beside the `frame_index` in the matching `labels.jsonl` row.
>
> **Why item 4 is the one that matters:** every other item is meaningless if the frame the label
> calls 44 is not the PNG we think it is. `frame_count` is a **span**, not a count (m20), and this
> project has already been bitten once by 0-based versus 1-based numbering.

---

## §6 What was NOT done, named

- ⛔ **No capture leg ran.** *(Post-verdict: Steps 0–3 run this session; the A/A′/B/B′/C legs are the
  next turn.)*
- ⛔ **No ceiling knob, no floor default change.** The floor/ceiling DECIDE is with the owner.
- ⛔ **No fix, no mechanism, no "likely cause"** anywhere in the `P9` material.
- ⛔ **`p9_hidden_set.py` was SPECIFIED, NOT WRITTEN, at the time this section was first written.**
  *(Post-verdict: it is written, committed to CaptureBench and gated against its four controls this
  session — §9.)*
- ⛔ **No tag, no cook, no checkout of `feature/stencil-capture`, no history rewrite,
  `AnomalyCapture.Build.cs` untouched.**
- 🕳 **`setup-runbook.md:377` still points at the deleted `_TRASH_pending_owner_delete`** — stale,
  left alone, named here so it is not lost.

## §7 Commit state

**Commit 1, `ae4fbaf`, pushed** — the §1.4/§1.5 verification fixes (`CLAUDE.md` + journal 066),
path-scoped, authorised outright by the brief.
**Commit 2** — the four held docs plus amendments `A1`–`A5`, after the chat verdict of the same day
(§8). `_binary_baselines\README.md` is outside version control and rides no commit.

---

## §8 CHAT VERDICT — APPROVED WITH AMENDMENTS, and what changed

**Verdict: the plan is approved; this session executes Steps 0–3 and the legs move to the next
turn.** Six findings accepted onto the record as chat errors: the number is **`P9`**; the tree anchor
was two commits stale and the `CLAUDE.md` invariant is the right fix; **delivery does NOT suppress
`labels.jsonl`, it suppresses `run.json` — the old claim is RETIRED**; **A54 cannot see `P9`** and the
new reader is the instrument; `feature/stencil-capture` is local-only. Running the `A53` controls
from banked data before plan approval was ruled **not a deviation**. **Leg C (the delivery bracket)
is APPROVED.**

### §8.1 The amendments, and the one that changes the instrument's shape

| # | amendment |
|---|---|
| **A1** | the ledger's tick-ratio co-observation tightened: the ratio is `capture_game_ticks / total_frames` **by construction** (122/90); **the unmeasured quantity is the 32 surplus ticks**, enumerated in the predictions file and **not attributed** |
| **A2** | the reader spec — see §8.2, it is the substantive one |
| **A3** | RDP card Section C gains **(g)**: read Bates' **effective delivery setting**, both the ini key and the `StartRun` echo, since Section B's is unrecorded |
| **A4** | `setup-runbook.md` marks `_TRASH_pending_owner_delete` deleted-and-verified · `CLAUDE.md` records `feature/stencil-capture` as **local-only, never pushed, never push without a clean verifier pass** · **`G202`** minted |
| **A5** | this journal records the `0.10737` non-reproduction |

### §8.2 🚨 THE READER GAINS A SECOND SYNTHETIC CONTROL, AND IT IS THE ONE THAT MATTERS

The plan as I wrote it had three reader controls: known-ALIGNED, known-ABSENT, and a synthetic `P9`
fixture. **Chat added a fourth — a synthetic CONSTANT SHIFT (`+1` applied to `R30`'s whole
annotation), which must read `SHIFTED(1)` with an EMPTY residual and NEVER `P9`-SHAPE.**

**That closes a hole I had left open.** My three controls could all have been passed by a reader that
called *every* disagreement `P9` — it would bless (a), decline (b), and fire on (c), scoring three
for three while being unable to tell `P1` from `P9`. Since keeping those two apart is the entire
discriminating job, **(c) and (d) are a load-bearing PAIR: one proves the reader sees the shape, the
other proves it does not mistake `P1` for it.** `G189`'s lesson arriving one level up — not "a check
that cannot fail", but "a check that cannot fail *in the direction that matters*".

Also folded in: the visible cluster is **anchored** by two flank frames per side (removing the
polarity guess); a **best-`k` shift search over −6…+6** with a fixed classification order, so `P1`
exits as `SHIFTED(k)` before `P9` is ever considered; **per-EVENT bbox and per-EVENT `A56`** so the
auto-pool legs are readable at all, with an `A56` failure landing as **UNDECIDABLE, never a silent
skip**; and pre-declared event counts and seeds (primary **4242**, fallback **777**).

📌 **One guard I added while implementing the anchor rule, recorded because it is not in the
verdict:** the four flank frames must all land in the **same** cluster. If they do not, the window's
edges are not all visible — a `blinking` hidden phase can reach a flank — and the polarity would
invert, turning a clean read into a **confident wrong answer**. Such an event is
**UNDECIDABLE (anchor unreliable)**, never guessed.

### §8.3 `A5` — the `0.10737` note, and why the oracle is NOT being edited

`a54_oracle.py`'s header claims it reproduces *"the published R30 median (0.10737)"*. Measured
2026-09-02: **`0.10478`** on `M23\R30_regress` and **`0.109008`** on `M23B\R30_recert`. **Neither is
0.10737. CAUSE NOT ESTABLISHED. ⛔ Do not cite 0.10737 as reproduced.**

⛔ **`a54_oracle.py` IS LEFT UNTOUCHED — including its header.** Any edit to the oracle re-triggers
`A53`, so a one-line comment fix would invalidate the control run that this session's whole plan
rests on. The provenance hunt is a **docs pass**, deliberately deferred, and the discrepancy lives
here and in the predictions file until then.

⚠ The `A53` gate itself is untouched by this: it turns on ALIGNED-versus-ABSENT, and that separation
is total (0.105 against 0.0001).
