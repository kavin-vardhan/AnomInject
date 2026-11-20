# REPORT for 075-01-m49-a2b-docs-and-m50-plan.md
brief: 075-01-m49-a2b-docs-and-m50-plan.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-04 18:25:50 | finished: 2026-09-04 18:51:20 | duration: 26 min | exit code: 0
context at last turn: 472577 tokens (47% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\075-01-m49-a2b-docs-and-m50-plan.live.txt

---- FINAL MESSAGE (Claude Code) ----
Pushed. `master` == `origin/master` == `e9203ff`; tree clean apart from the eight untracked owner handoffs (never staged).

---

## 1. Plain-language rundown

1. **`m49` A2b is written.** The client README's §8 went from four bullets to a full schema-v2 label reference — every `annotation.json` and `labels.jsonl` field with type, since-version and meaning, plus a v1→v2 changelog. Every name and semantic was read out of the writer source, not recalled.
2. **The one thing that can silently break a client parser is called out loudly:** `affected_frames` kept its key and changed its meaning. The changelog marks it ⚠ MEANING CHANGED and gives the one-line migration — *point v1 code at `injected_frames` and nothing changes*.
3. **`client-delivery.md` gained a paste-ready changelog paragraph for the next drop**, and it carries its own instruction to delete the `m50` paragraph if `m50` is not in the build.
4. **Ruling 1 is "no rule".** Leg B found no detectable effect, so the honest default is no refuse-or-warn rule; the reader names still ship and the census knob is the escape hatch, with the `G120` limit shipped verbatim.
5. **Office card Section G moves the verifier read onto the host box.** That gate has been reported UNRUNNABLE three sessions running while waiting for a session folder to be copied here. Now it runs there and only transcribed numbers come back.
6. 🚨 **The plan-time finding that redirects `m50`: the stencil ownership ledger ruling 2 asks for already exists and already guards both directions.** Building the briefed wording would change nothing and ship a green tick over a live defect.
7. **So journal 074's mechanism sentence is withdrawn as established** — it rests on a log line that prints a *range*, not the set. The observation, the 4.9 % incidence and the MASK-TIE blindness all stand; only the cause does not follow.
8. **`m50` is planned at ≈ 3 sessions, no cook**, with step 0 as a diagnosis (branch table `D1`–`D4`), ten gates in both tick orders, and four NEEDS-DECISION items gating it.

---

## 2. The v1 → v2 changelog table (verbatim, `client-readme.md` §8.5)

| Key | v1 meaning | v2 meaning | Additive? |
| --- | --- | --- | --- |
| `label_schema` (root) | *absent* | `2` — the version marker | **Added** |
| `affected_frames` | the frames the anomaly was **applied** on | the frames the anomaly is judged **visible** on (a subset of `injected_frames`) | ⚠ **MEANING CHANGED** — same key, narrower set |
| `affected_frames.frame_indices` | applied frames | visible frames | ⚠ **MEANING CHANGED** |
| `affected_frames.start_frame` / `end_frame` | first / last **applied** frame | first / last **visible** frame | ⚠ **MEANING CHANGED** |
| `affected_frames.frame_count` | number of entries in `frame_indices` | number of entries in `frame_indices` | Unchanged (it was never a span) |
| `affected_frames.span_frame_count` | *absent* | `end_frame − start_frame + 1` | **Added** |
| `injected_frames` | *absent* | **exactly what `affected_frames` meant in v1** | **Added** |
| `observable_frame_count` | *absent* | count of injected frames measured **and** visible | **Added** |
| `unmeasured_frame_count` | *absent* | count of injected frames with no measurement | **Added** |
| `observability_measured` | *absent* | whether `affected_frames` is a measurement or a fallback | **Added** |
| `bbox_source` | *absent* | `"drawn"` or `"projected"` | **Added** |
| `labels.jsonl` → `target_pixels` | *absent* | measured drawn pixels, `-1` = unmeasured | **Added** |
| `labels.jsonl` → `observable` | *absent* | `true` / `false` / `null` | **Added** |
| `labels.jsonl` → `bbox_drawn_px` | *absent* | measured drawn box, or `null` | **Added** |
| `run_summary.json` → `observable_frames`, `frames_condition_lost`, `observable_min_pixels` | *absent* | see §8.4 | **Added** |
| everything else | — | — | Unchanged |

🔑 **The one-line migration:** *if your v1 code read `affected_frames`, point it at `injected_frames` and nothing changes.* Then adopt `affected_frames` when you want the visible subset.

⛔ **Nothing was removed and nothing was renamed.** Every v1 key is still present with its v1 type.

---

## 3. Card Section G (verbatim, `docs/office-rdp-card.md`)

```markdown
# SECTION G — `m49` LABEL-vs-PIXEL VERIFIER, RUN ON THE HOST BOX

✅ **RDP-valid.** Four steps, one of them optional-until-later. **Nothing is copied off that machine.**

## G-0. WHY THIS SECTION EXISTS, AND THE ONE RULE THAT SHAPES IT

The `m49` verifier answers the question the client actually asks: **does the first labelled frame
match the first frame whose pixels change, and is the frame after the last labelled one clean?**

🚨 **THE SESSION NEVER LEAVES THE HOST MACHINE. THE ONLY THING THAT COMES BACK IS NUMBERS THE OWNER
TYPES.** That is why the tool runs *there* rather than the session being copied *here*.

⚠ **THIS IS THE THIRD ATTEMPT AND THE FIRST TWO WERE NOT FAILURES — THEY WERE UNRUNNABLE.** The
`m49` A1 and A2 campaigns both declared this gate a required read and both had to report **UNRUNNABLE,
never passed**, because they were waiting for a session folder to arrive at
`D:\IntrusiveAnomalies\_bates_reads\` and none ever did (journals 072-02 §, 073 §, 074 §6). **Section G
removes that dependency: the reading is produced where the data already is.**

📌 **A reading is the deliverable. A PASS is not required for this section to have done its job** —
an `ONSET-SHIFT` here is exactly the finding the whole `m49` milestone was built to surface.

## G-1. Update the box

`git status` clean → pull → **confirm the SHA**. As in `E-1`/`F-0`.

```
git -C <plugin-repo> status --short
git -C <plugin-repo> pull
git -C <plugin-repo> log --oneline -1
```

**REQUIRED: the tip is `0d05c4b` or later.** *(That commit is `m49` phase A2's docs close-out; the
verifier itself has shipped since `b062832`, but the A2 tip is what this card was written against.)*
⚠ **No rebuild is needed for G-2 or G-3** — the verifier is a Python script and reads an
already-captured session. A rebuild is only needed for **G-4**.

## G-2. Run the verifier, READ-ONLY, on the session the owner flagged

**Which session.** The one the owner flagged as *"the anomaly appears one frame before the label
says"* — the session containing **two texture-type events early in the run**
(`missing_texture` / `corrupted_texture`).

⚠ **NAMING IT IS THE OWNER'S STEP AND THIS CARD CANNOT DO IT** — this box has never seen either
host's disk. If you cannot identify that session with confidence, **say so and stop**; running the
gate on the wrong session produces a confident number about the wrong thing, which is worse than no
number. If more than one candidate exists, run it on each and label the readings.

**The command.** Use whichever copy of the tool that box has:

```
:: from a delivered bundle
python <delivery-root>\host-tools\verify_capture.py --label-pixel-gate --report-only --dir <sessionDir>

:: from the plugin checkout you just pulled
python <plugin-repo>\tools\verify_capture.py --label-pixel-gate --report-only --dir <sessionDir>
```

🚨 **`--report-only` IS NOT OPTIONAL HERE.** It prints every reading and **always exits 0**, so the
step cannot end early on a non-zero exit before the per-event lines have been read. The exit code is
not what this section is collecting; **the lines are.**

⚠ **PROVE IT CAN FAIL FIRST — one extra command, and it takes seconds:**

```
python <same path>\verify_capture.py --label-pixel-gate --selftest
```

**It must print `SELFTEST: OK`.** *A gate that has never been shown to fire cannot make a clean read
mean anything (`G96`). If the selftest does not print OK, everything below is void — report that
instead.*

⚠ **If it refuses with a message about `measure_label_offset.py`, that file is missing beside it.**
The gate REFUSES rather than running degraded, on purpose. Copy `measure_label_offset.py` from the
same `tools/` or `host-tools/` folder and re-run. **Do not work around it.**

## G-3. Transcribe the reading — numbers only

Write into a NEW file on **this box's** shared folder:

```
D:\IntrusiveAnomalies\_bates_reads\2026-09-0X-bates-verifier-read.md
```

*(Replace `X` with the day. If the session came from the other office host, name the file for that
host instead — the folder is the drop point, not a claim about which host produced the read.)*

**Transcribe, verbatim and complete:**

1. the **header block** — the `session`, `frames / labels / events`, `masks present`,
   `diff threshold`, `edge search window` and **`MEASURABLE RANGE`** lines;
2. the **`baseline frames`** line;
3. **EVERY per-event line, in full.** Each is one line and looks like:
   `idx=<n> <anomaly_type> <target_name>  <VERDICT>  [HIGH|LOW] tau=<n> base=<n> [CONTAMINATED=<n>] [appearance=<n>]`
   — where `<VERDICT>` is one of `PASS`, `ONSET-SHIFT(±n)`, `END-SHIFT(±n)`, `NOT-VISIBLE`,
   `NOT-MEASURABLE(<why>)`. **Include the bracketed reason on any non-PASS line.**
4. the **summary** line — `PASS n   SHIFT n   NOT-VISIBLE n   NOT-MEASURABLE n   (of n event(s))`;
5. the **`VERDICT`** line.

⛔ **Do NOT copy the session folder, any frame, `annotation.json`, `labels.jsonl`, a log, or a
screenshot of the game.** The transcription is the entire channel.
⛔ **Do NOT re-run to get a tidier reading.** Whatever it says the first time is the result; if you
run it twice, report both.

### 🔴 THE PRE-DECLARED READING — read this BEFORE running G-2, and do not adjust it afterwards

**PRE-DECLARED, 2026-09-04, before any run of this card:**

| line | expected |
| --- | --- |
| the two flagged texture events | **`ONSET-SHIFT(-1)`** — the pixels changed ONE FRAME BEFORE the label said so |
| every other event | **`PASS`** |
| a truncated FINAL event, if the run has one | **`NOT-MEASURABLE`**, with its reason printed |
| summary | `SHIFT 2`, and `NOT-VISIBLE 0` |

🔑 **The sign matters and is easy to get backwards: `n` is SIGNED and NEGATIVE means the PIXELS moved
FIRST.** `ONSET-SHIFT(-1)` = the anomaly is visible on frame `start_frame − 1`.

⛔ **NO MECHANISM IS ASSERTED HERE.** A first-use shader-compile stall on that build (it predates
`m47`'s shader-readiness work) is a **CANDIDATE explanation and nothing more** — it has not been
measured on that host, and an observation and its explanation are separate claims (`G120`).
**Whatever the tool prints is the result; the candidate is not a reason to prefer one reading.**

⚠ **ANY OTHER READING IS A RESULT, NOT A FAILURE OF THE STEP, AND IS NOT RE-RUN:**

- **All `PASS`** ⇒ the flagged events do not reproduce under a pixel-ground-truth reading. That is a
  finding: it would mean the original observation was a numbering or playback difference rather than a
  label offset, and it must be reported as loudly as a shift.
- **`NOT-MEASURABLE` on the flagged events** ⇒ the gate could not judge them, usually because the
  clean gap between windows is too small to measure the offset (`MEASURABLE RANGE` says how small) or
  the camera was moving. **That is an unread surface, not a pass** — transcribe the reason.
- **`CONTAMINATED=n` / `[LOW]` on a line** ⇒ that row's baseline had frames above threshold, so its
  confidence is low. **Transcribe it; do not drop the row and do not promote it.**
- **A shift on an event nobody flagged** ⇒ report it. It is new information.

## G-4. AFTER an `m47`-or-later build exists on that box — the same recipe again

⛔ **DO NOT DO THIS IN THE SAME SITTING AS G-2.** G-2 reads the build the owner already has; G-4 reads
a NEW build. Running them together makes the comparison meaningless — **one variable at a time.**

1. Rebuild the **EDITOR** target on that box (runbook §8.6 STEP 3.5 is not optional — `G47`).
2. Capture again with **the same recipe as the flagged session** — same map, same anomaly, same
   `IAI.Capture.Config`, same frame count, same seed if it is known. ⚠ **If the original recipe is
   not known, say so** — an unmatched re-capture is a different experiment and cannot answer G-4.
3. Run the same G-2 command on the new session and transcribe it the same way, into a file named for
   the new date, **beside the G-3 file and never overwriting it.**

**PRE-DECLARED for G-4: `PASS` on every event, `SHIFT 0`, `NOT-VISIBLE 0`.**
⚠ **A remaining shift there is the more valuable outcome of the two** — it would say the timing class
is not what the newer build fixed, and that is a finding this project has no other way to obtain.
🚨 **Keep the G-3 reading. It is the BEFORE picture and there is no way to reconstruct it once the box
is rebuilt.**
```

---

## 4. The `m50` plan — summary

**Step 0 is a DIAGNOSIS, not an implementation**, because the ledger already exists (§3.1 of the journal). One product change worth making whatever the branch: `AnomalyCensus.cpp:794-797` prints the ARM's **tag SET**, plus `EventClaimed` at the same tick. Branch table `D1` direct collision · `D2` release window (leading) · `D3` rate dependence · `D4` `TAG-OVERTAKEN` residue (expected excluded, counted so the exclusion is measured).

**Files**

| ruling | file | change |
|---|---|---|
| 2 | `AnomalyStencilTag.h/.cpp` | ledger gains `Quarantined` map; `IsFree()` excludes it. ⛔ `IsAssignable()` untouched |
| 2 | `AnomalyCensus.cpp` | `ReleaseBatch` quarantines; `RecentlyReleased` **collapsed onto the ledger** (one predicate, two consumers) |
| 2 | `AnomalyMaskMeasure.cpp` | **no change** — `AllocateTag` already calls `IsFree` and inherits it |
| 3 | **new** `AnomalyMeasurability.h/.cpp` | promote `ComponentRendersAsNanite` out of the census's anonymous namespace. ⛔ **never `AnomalyViewport`** (`G127`) |
| 3 | `AnomalyCensus.cpp` | `ClassifyCandidate` calls the shared predicate; verdicts and counters must not move |
| 3 | `AnomalyMaskMeasure.h/.cpp` | classify at record creation; **`ArmIfMeasurable` SKIPS** such a record |
| 3 | `AnomalyLabelWriter.h/.cpp` | one new key, `unmeasurable_targets_admitted` |
| gate | `CaptureBench/tools/m50_single_owner.py` | per frame, every mask value has exactly one owner in the log |

🔑 **Skipping the arm is why the veto and the label writer need no change at all** — `R.State` stays `NotMeasured`, `MaskStateVetoes` admits, and `target_pixels -1` / `observable null` / `observability_measured false` / the `injected_frames` fallback / `bbox_source "projected"` all fall out of existing A1/A2 code.

**Gates (10, both tick orders):** `G1` single-owner 0 violations · **`G2` its can-fail collision lever** (without it the gate is worthless — it replaces `MASK-TIE`, which passed this defect for two milestones) · `G3` re-count 0 against the banked 22 of 448 · `G4` MASK-TIE unchanged, *and pre-declared as not evidence for G1* · `G5` full regression set · `G6` allocator headroom, 0 `TAG-POOL EXHAUSTED` · `G7` unmeasurable admission at `SM_Ramp2` **with the census ON** · **`G8` its BEFORE picture on `8839D806`** (if the bench cannot reproduce the `LG-9` shape, `G7` proves nothing and must say so) · **`G9` `LG-9` on Lyra must deliver 6 events with `observability_measured: false` instead of `anomalies: []`** · `G10` `P-C7 v3`.

**`P-C7 v3`:** `run_summary` 64 → 65, added exactly `unmeasurable_targets_admitted`; `labels.jsonl` and `annotation.json` add 0, remove 0. 🚨 **`frame_indices` is NOT expected identical on legs where either path bit — declare per leg in advance or it reads as a regression.** ⚠ `G140` boundary on `mask_value`.

**Effort ≈ 3 sessions. NO COOK** (no shader, no shader parameter struct).

**Per host:** ruling 2 host-independent · ruling 3 **inert on Concorde as configured** (Support Nanite disabled — the fix costs nothing and guards the checkbox, the `H6` risk) · **Bates' Nanite status has never been read**, and `census_unmeasurable_nanite` is the reading that answers it — a named read, not an assumption.

**Priced before building:** the quarantine shrinks a **55-value pool** against a census that ran **77 candidates in one leg**, so `TAG-POOL EXHAUSTED`, `tagOvertaken` and the `census_fires_*` counters are pre-declared reads on every leg.

---

## 5. Commits, SHAs, checkout state

| SHA | commit |
|---|---|
| `dc3457c` | `docs(client)`: schema v2 field reference, v1→v2 changelog, the drop paragraph |
| `c0f028b` | `docs(delivery)`: verifier read moves onto the host box (card Section G), m50 placeholders |
| `e9203ff` | `docs(075)`: journal, CLAUDE.md status, `G249`/`G250`, `G246` amended in place |

- **Start `0d05c4b` → tip `e9203ff`; `master == origin/master == e9203ff`, pushed.**
- **Checkout never moved** — on `master` at start and end. No worktree touched.
- **Path-scoped adds only**; the eight untracked owner `CHAT-HANDOFF-*.md` remain untracked.
- **No source edit, no build, no leg, no cook, no tag.** Staged bench exe stays `8839D806`; container quartet untouched.
- Diffstat read before every commit (751 insertions / 13 deletions across 6 files) — proportionate, no `G115` blowup. A stray BOM at byte 388830 of `gotchas.md` was checked and **predates this session** (present in `HEAD`, outside both changed hunks).
- **Codename scrub: `VERDICT: clean over 7 file(s)`, exit 0**, selftest fired on the synthetic fixture first.

---

## 6. NEEDS-DECISION

1. 🚨 **Ruling 2's design direction is already implemented, so `m50` step 0 is a diagnosis rather than the implementation the brief describes.** Confirm the substitution — building the briefed wording as-is would be a measurable no-op shipped as a fix. The gate half of ruling 2 is unaffected and planned as written.
2. **Journal 074 §4's mechanism sentence is withdrawn as established** (the `tags=A..B` log prints a range, not a set). Observation and 4.9 % incidence untouched. Confirm the withdrawal riding here + the `G246` in-place amendment, rather than editing 074.
3. **Section G cannot name the session.** ⚠ The brief places it on Bates with the drop in `_bates_reads\`, while the "−1 at onset" texture findings on record were made against the *other* office host's M2 bundle at `2b6c93f` (journal 071 §1). The card works either way and says so, but **which host holds the flagged session is the owner's to state.**
4. **`unmeasurable_targets_admitted` is a new `run_summary` key** (64 → 65), proposed because an admission that leaves no trace is as bad as a deletion that leaves no trace — the reason `vetoed_events` exists. ⛔ `annotation.json` does not move. Veto or bless.
api-equivalent cost (not billed on Max): $20.42 | turns: 77 | running total across all briefs: $943.66
