# 2026-08-18 — 043 — Session close-out: I10 window re-check discharged, S3 closed at `m24`

## 0. COLD START — read this block first

You are picking up a **fresh session**. This journal is what you bootstrap from.

| | |
|---|---|
| **`S3` is CERTIFIED and TAGGED `m24`** | pushed, remote-confirmed, tree clean. Ratio-independence discharged; delivery mode orthogonal |
| **The I10 hand-chosen-window debt is DISCHARGED** | I10's null **SURVIVES**, narrowed by one leg |
| **`S4` is NEXT and NOT STARTED** | backbuffer demotion + defaults flip. **Not planned. Do not start it without a brief** |
| **Production code is UNCHANGED since S3a-3** | the entire S3b arc was validation and docs |

**Reading order:** this block → `2026-08-18-042-s3b-matrix-certified.md` (the whole S3b arc, and §10 is
the re-check) → `CHAT-HANDOFF-s3a-sve-landed.md` → the three S2 handoffs.

⚠ **`P1` IS STILL OPEN AND IS NOW DOWN TO ONE NAMED LEAD.** See §3. This outranks the green.

---

## 1. What this session did

Ran the **I10 window re-check** — the debt journal 042 §8 flagged with *"do not let this tag bury it."*
Paper only: banked data, the mechanical settle rule, the certified oracle. No runs, no code, no re-windowing.

**VERDICT: I10's null SURVIVES, narrowed by one leg.** Five of six legs certify under a purely mechanical
window, every one ALL-ALIGNED with all events decidable and margins 0.108–0.112, positive control decisive
in both directions on each. **No leg returned SHIFTED or ABSENT.** Full table and evidence in **042 §10**.

⇒ **CPU starvation remains REFUTED as a cause of `P1`.** I10's null no longer rests on hand-chosen windows.

### 1.1 The finding that strengthened it

The concern was that a window chosen *after seeing a leg* could exclude the frames where a defect would
show. **It did the opposite.** What the hand-chosen windows excluded was **startup marker noise** — L1's
five bad marker rows sit at indices 0–4, which its published window (16..89) removed and the **mechanical
window (0..89) INCLUDES**. L1 still returns **ALL-ALIGNED, 7/7 decidable**. The mechanical rule is
**stricter** here and the verdict does not move.

### 1.2 ✅ B1 passed a known-answer case

`L3_client39` — the banked known-ALIGNED, **bifurcated-pose** control — returned **NOT-A54-CERTIFIABLE,
not a false ABSENT.** That is exactly the property B1 was adopted for, exercised on the case it was
adopted against. **A control that PASSED**, not a leg that dropped out.

---

## 2. Branch R — journal 031's `532 / 534` reconciles, and my explanation was wrong

I had proposed that journal 031's denominator came from its windows filtering the corpus. **Its own
per-leg table refutes that.** The published counts (`L1 85, L2 90, L3 87, L4 90, L5 90, L6 90`) are over
**full 90-frame legs** — L1's 85 and L3's 87 both **exceed** their own published windows (74 and 60).

They sum to **532 matching out of a 540-frame corpus**, and the implied per-leg non-matching
(**L1 5 · L3 3 · others 0**) is **identical to this re-check's independent measurement**.

- The `534` is an **arithmetic slip** — `532 + 2`, written to match a "two exceptions" narrative.
- **"The only two exceptions … in L3" is wrong**: there are **eight**, in **L1 (five) and L3 (three)**.
  L1 is not mentioned in that sentence at all.

⛔ **My window-filtering explanation is WITHDRAWN.** It was plausible, it fitted the narrative, and it
would have entered the record unchallenged had the reconciliation not been demanded before writing.
**None of journal 031's conclusions are disturbed** — the matching count and its per-leg distribution are
both correct.

⚠ The same slip propagates to journal 031 §9.2's combined `1052 / 1054` (true corpus **1080**). The
**render half is NOT re-measured** — out of scope — so that figure is **flagged, not corrected**.

---

## 3. `P1` — the narrowing, stated plainly and not softened

I10's null now certifies on **five** legs and covers **every band it claimed**:
nominal 1.0000 · mild 1.0558 · **client 1.2342** · deep 3.0027 · pacing-off 0.3312.

> **BUT THE CLIENT BAND IS NOW CARRIED BY ONE LEG (`L6_client40`) WHERE IT PREVIOUSLY HAD TWO. `P1` is a
> client-band phenomenon. The refutation of CPU starvation at ~1.2 therefore rests on a single certified
> leg plus one honestly unjudgeable leg. This overturns nothing and it is not a defect — it is the
> THINNESS OF THE EVIDENCE, stated now rather than discovered later if H1 also comes back clean.**

> **`B2`'s payoff now includes recovering `L3_client39` and restoring the client band to two legs.** That
> is a concrete gain B2 did not have when it was filed. **B2 remains FILED, NOT SCHEDULED** — it is a
> definition change needing its own eight-control gate and it may reopen m23's DA60 floor.

⛔ **`L3`'s bifurcation is NOT to be investigated, recovered, or re-windowed. It stays unjudgeable.**

**`P1` lead count is UNCHANGED by this re-check.** CPU starvation does **not** return as a live lead.
**`H1` (GPU-load starvation shape) remains `P1`'s ONLY named lead, and H1 has no lever in existence. If
H1 also comes back clean, `P1` HAS NO NAMED LEADS.** An H1 lever can now be designed without the worry
that CPU starvation was settled on a soft foundation — which was the point of running this first.
Lever design stays **chat-side first, never same-turn as its first measurement.**

---

## 4. The arc's closing note — the hardening was not delay

**Four turns of instrument work ran before a single leg.** Each caught something that would have
corrupted the result:

1. a destructive `else` silently re-parented by an append (**G102**);
2. the A54 oracle **did not exist** — every prior certification had been graded by scripts that no longer
   existed (**G106**);
3. the rebuild had **correct verdicts and wrong confidence**, caught only by reproducing published
   *numbers* rather than published *conclusions*;
4. **TAU is not camera-pose invariant** — a bifurcated pose produced a **false ABSENT**, which A50 reads
   as defect reproduction (**G107**, **P8**).

Then **five legs and three pairs, all green, first time.**

> **THE HARDENING WAS NOT DELAY. It is why the matrix was believable when it arrived.**

**And the connection worth recording:** `m24` carries its **own scope statement in the tag object** —
what certified, the four scope limits, what is still open, and the P1 line. A tag whose meaning depends
on a reader finding the right journal is a tag that will eventually be over-read. **That is the direct
fix for the problem hit at Stage 2a** — an instrument whose definition lived only in prose and was lost.
Same disease, same cure: **the artifact carries its own meaning.**

---

## 5. State at close

- `AnomalyInjector` — docs commits only. **Tagged `m24`**, pushed, remote-confirmed, tree clean.
  **Production code unchanged since S3a-3.**
- `CaptureBench` — `a54_oracle.py`, `run_leg.ps1`, `eval_leg.py`, `check_pose.py`,
  `verify_lastrundir.ps1`. **Local-only, no push. Probe untouched the entire arc.**
- **Bank 58 dirs.** Staged exe unchanged, SHA-256 `3BA854FB…`.
- **No tag this session.** `m24` was tagged in the previous turn and stands.

**Open debts** — the complete list is **042 §8**. Headline: **`A11` OPEN** (the design prevented the
condition it wanted to observe) · **client-band thinness** (§3) · **`P7`** · **`P8`/`B2` filed not
scheduled** · **two bifurcated legs LOST to a compliance failure, recorded as lost** · **`H4`** · **`P6`**
· journal 031's combined-figure slip, flagged not corrected.

**NEXT: `S4` — backbuffer demotion + defaults flip. NOT STARTED, NOT PLANNED.** ⚠ **C2: it is a
CLIENT-VISIBLE change, not a silent default flip** — the pre-Slate SVE grab is **UI-free by
construction**, so flipping the default **changes delivered image content**. **Depth remains PARKED and
UNNUMBERED.**
