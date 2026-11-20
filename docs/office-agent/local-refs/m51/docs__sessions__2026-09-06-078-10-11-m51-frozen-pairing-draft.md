# 2026-09-06 — session 078-10 / 078-11 — `m51` frozen-pairing campaign, drafted and corrected

**Documentation and read-only preflight only. No capture, staging, build, cook, lever, source,
harness or tool change ran in either brief.** Production frozen at `6713f51`; `master` ==
`origin/master` == `ac13700`; 10 untracked owner handoffs preserved; CaptureBench `472a409`
tracked-clean; Lyra detached `4a4bfcc`, clean.

## 078-10 — the draft (`e0d63f1`)

Created `docs/predictions/2026-09-06-m51-frozen-pairing-recheck.md` (358 lines): nine full
commands with no ellipses and no inherited "same as" placeholders, each mechanically validated
against the unchanged `run_leg.ps1`. Five campaign legs in fixed order — fresh A-side natural
reference, then candidate NAT / SYN / NAT_SP50 / SYN_SP50 — plus four conditional A-controls of
which **at most one may ever run**, selected by the first valid candidate pairing failure, then
**STOP regardless of result**. Bank prefix `M51R_` verified free across 45 bank paths and 36
attempt directories. Recipe derived from the banked Phase B legs' own configs (`G184`).

**The substantive find:** 078-05's smoke subrecord pinned `-Marker 0` while asserting it
reproduced `PB1F_F_PAIR_*` *"exactly (G184)"*. The banked legs pass no `-Marker` and ran at the
runner default **`Marker 1`** — the identical-recipe claim was false. The campaign issues
`-Marker 1` explicitly.

## 078-11 — five bounded corrections (AMENDMENT 1)

Codex's review found five documentation defects. All are corrected by an **append-only**
amendment that explicitly supersedes the faulty paragraphs; the draft stays intact.

| # | defect | correction, and its evidence |
|---|---|---|
| 1 | §9 attribution overclaimed and contradicted its own next sentence | both-fail ⇒ the failure occurs **without `F1`**, which neither isolates nor excludes an `F1` contribution; control-passes ⇒ **consistent with**, time/state confounded. Neither is causal proof |
| 2 | §2 claimed **no** banked Marker receipt | **wrong** — `CaptureBench.Marker = "1"` at **line 1533** of all four `_tryN\_leg_game.log`, full launch command at lines 17/207; four files hashed. `_leg_geometry.json` blindness stands, but blindness ≠ absence. Accepted alias carries **no** `_leg_game.log` (measured) |
| 3 | §7 named a screen-percentage read-back without identifying it | it is `Capture(mask): M23 PASS` → `internalViewRect` / `unscaledViewRect` (SP50: 640×360 vs 1280×720, `r.ScreenPercentage = "50"` at line 1542). **100 % read from rendered geometry** — no explicit 100 echo exists. Added artifact preconditions and a **runnable** exactly-one-session checker invocation |
| 4 | §3 "not overridable from the CLI"; §6 "every physical attempt is retained"; §9 folded three stop classes into one | `-ExtraExecCmds` is appended **after** Fps/Config (`:224` vs `:219-220`) so override **is** possible — these commands just don't; the `no run_summary` (`:322-324`) and `focus timeout` (`:371-373`) paths `continue` **before** the `:408-418` bank copy, so they leave **no bank and no log**; REFUSED / UNDECIDABLE / `<3 events` restored as three distinct stop classes; exit 3 **unexpected, not impossible** |
| 5 | §9 said "three hours earlier" | false — the A-side passes are **19:07:43** and **19:19:36**; no relative duration asserted |

Unchanged by the amendment: the nine commands and their order, the `MaxAttempts 4`
pose-invalidity budget, never retrying a valid failure, the at-most-one control then STOP, the
predicate and tolerances, the even-frame blind spot, FPS/`speed_ratio` as telemetry only, and
every hold.

## Still open

Root cause of the 078-05/078-06 pairing failure · why the box ran at ~1.00 in the evening and
3.4–3.6 an hour earlier · `M-F1b` UNRUN · the identity/lifecycle unit (all of it unproved design
work) · Lyra arm not waived. **Pairing release certification and every `m51` implementation item
remain BLOCKED.**

**VERDICT: NEEDS-DECISION** — the corrected exact snapshot awaits Codex and Chat review before
any leg can be released.

---

# 078-12 — the coverage floor (appended 2026-09-06)

Chat approved the campaign **conditionally** and Codex dispositioned that approval; this brief
implements the condition and nothing else. ⛔ **No capture, staging, build, cook, source or tool
change — nothing was run.** `AMENDMENT 2` is appended to the prediction record, `AMENDMENT 1` and
§§0–10 are intact, and where they disagree the amendment governs.

**The change:** §5's `decidable >= 1` non-vacuity test becomes a **coverage floor of `decidable >= 20`
of 40**. A leg with two decidable frames and `PREVIOUS 0` would have PASSED — `G146`'s vacuous-zero
shape. The basis is **half the request** (40 captured frames ⇒ 40 probe opportunities), fixed
prospectively from the request size, and the amendment states in as many words that it is **not**
statistical sufficiency, **not** a tolerance, **not** an FPS band, and **expressly not** derived from
the failing bank — choosing a floor above the failing legs and then citing them as its justification
would be fitting the rule to the results it judges. Attainability is cited only from receipts read
in-session: `M51D_HE_RESIDENT_NAT` **35** and `M51D_HE_CLOSED_NAT` **37**; no other count is repeated.

**What an under-covered leg does:** stops the campaign, releases no control, is not re-run — **but
still reports its counts and every failing index**, because insufficient coverage prevents
certification without making an observed mismatch disappear. Historical legs keep their original
classifications, including the earlier failures at 13 decidable. The stop taxonomy grows to **four**
classes (REFUSED · UNDECIDABLE · 🆕 coverage · event count), none releasing a control.

**Also carried:** verdict + counts + sustained FPS + `speed_ratio` on **one row** (legibility, not a
gate; pacing stays telemetry) · Chat's paraphrase of the attribution rule **explicitly rejected**,
`A1.1` restated as governing · "quartet" corrected to the **five-file quintet**, with the note that
§1 was already right · swap and restoration receipts fixed as **explicit policy** for the execution
brief · a PASS scoped as pairing smoke with the even-frame blind spot, **not** release certification.

**Unchanged:** nine commands and order · `MaxAttempts 4` · no retry of a valid failure · at-most-ONE
control then STOP · predicate and tolerances · at-least-3 events · every hold in §10.

⚠ **Process note:** the first append attempt was blocked by a shell path guard before executing —
the prediction file was verified byte-identical afterwards, and the text was routed through a file
instead. Nothing partial was written.

**VERDICT: NEEDS-DECISION** — no leg is released by 078-12; execution awaits the separate Codex
release brief.
