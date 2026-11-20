# 093-02 — Chat ruling: the natural-foundation plan is ACCEPTED WITH AMENDMENTS; implementation waits for the owner's go

**Date:** 2026-10-13, about 16:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- Codex's plan: `_reviews\093-02-codex-natural-foundation-plan.md` (`gpt-6-astra`/`max`, 2,741 s).
- Code's review: `_reviews\093-02-code-plan-review.md` (14 ruling rows: 9 OK, 4 GAP, 1 CONFLICT).
- Mailbox report 093-02: GREEN, 57 min, $11.78 API-equivalent.
- Binding before this: `_reviews\093-01-ruling.md` (R1–R11).

## Verdict
**ACCEPTED WITH AMENDMENTS (P1–P9).**
- Codex's plan is the reference for the job breakdown. Where it and this ruling disagree, this ruling wins.
- Code's review findings are all upheld.
- **Nothing is implemented until the owner gives his go (P9).**

## Amendments

### P1. Base: merge, don't cherry-pick
Code showed that the plan's reason against merging (that the merge "reverses defaults to 3/40/1") is a two-dot-diff artifact. The facts:
- A real 3-way merge of `5d6352b` into `d7eee97` has **one conflict (`CLAUDE.md`) and keeps 0/0/0**.
- The F1 work-in-progress applies cleanly on top.
- A merge reproduces the proven `5d6352b` source exactly. A cherry-pick would make a tree that was never proven.

The base therefore is:
1. Branch `feat/natural-foundation` from `release/2026-10-05` `d7eee97`.
2. `git merge 5d6352b`, resolving the one `CLAUDE.md` conflict.
3. Verify the transition defaults read 0/0/0.
4. Re-proof list: as in the plan's §A2 (F2 scope and tag retirement, F3 pair, F6, dropped-mask accounting, m52/m53 regression at 0/0/0), PIE and staged.

### P2. Identity: option (a), office-robust F1, with Code's additions
- **J2 applies the saved WIP** (patch plus untracked files) on the J1 base, then completes it within the plan's §A3 boundaries 1–5. Those are:
  - owner and generation frozen into each request;
  - uniqueness proven before any fallback;
  - single-view scope;
  - colour, mask and m55 bound to one family.
- **The WIP's new `NotGameView` rejection** (even when the owner is known) must be:
  - named as a risk;
  - counted by sub-reason (show flag / game-thread view / controller id);
  - proven on both home fixtures.
- **Ownership counters are serialized.** Add an additive, diagnostic-only `capture_ownership` block to `run_summary.json`, so the office reader can print them. It never appears in labels.
- **Can-fail sourcing:**
  - The **J1 build** (guard present, WIP absent) is archived as the F1 can-fail: new hash, same source, per G201.
  - The shipped **release `218BA4C7`** is the pre-F1 control.
  - J2 **re-creates** the delayed-viewport harness (the old one was deleted on 10-10).
- **J3 may start once J2 passes at home.** Office checkpoint I runs at the owner's next office visit. If it fails there, a fix job goes in before J11.

### P3. Publication: transition route, no exclusion, no renumbering
This supersedes the plan's §B4 publication choice.
- **Why:** exclusion cuts frames out exactly at every natural onset and offset. A model can learn the cut instead of the anomaly; this is a classic shortcut. Exclusion also breaks real-time video and touches every shipped anomaly's output path. R7 allows "excluded **or** transition"; chat picks **transition**.

**Classification and flags:**
- Frames of an active natural attempt are classified **per frame**, before their label row is built: clean / affected / transition.
  - A **bounded in-order hold** for those frames only is allowed.
  - **Published indices never change:** frame N = image N = mask N = label N = video N.
- **Applied but not visibly changed** (onset or recovery latency, or the ambiguous band):
  - in the event's `injected_frames` but not its `affected_frames`;
  - `transition_reasons`: `natural_onset`, `natural_unresolved` or `natural_recovery`.
- **Unknown** (a missing receipt): transition with `natural_unknown`, never clean.

**Which attempts produce an event:**

| Attempt | Event |
|---|---|
| Every frame proven within noise | **No event.** The frames are clean, truthfully |
| Any ambiguous or unknown frame, but no positive frame | **An unresolved event:** `affected_frames` empty, its frames listed as transitions, `effect_observation: unknown`. It never counts as a success, so no uncertain frame reads as clean |
| At least one positive frame | A normal event |

**Other points:**
- **Measurement settings** are accepted as experiment settings and frozen in the predictions file before the first leg:
  - visible threshold `V_T = max(4, 4·N_T)`, in 8-bit code values;
  - 5 clean samples;
  - at most 4 paired samples;
  - a 2 s watchdog;
  - 2 recovered samples.
- **J5 shrinks** to:
  - per-frame classification before label construction;
  - the bounded hold;
  - mask-write acknowledgement before a labelled frame is published;
  - the new transition reasons;
  - **G-CONTROLLED-IDENTITY**: a controlled-only run's output stays field-identical before and after J5 (Code's risk 3).
- **Office edge readings:** with no exclusion, the office kit's edge readings are meaningful directly. Checkpoint N uses the kit as is, and the diagnostic source window is not required.

### P4. Labelling MISS-1 and MISS-2
- **MISS-1:** `missing_texture`, `anomaly_subtype: engine_default_material`, `injection_mode: controlled`.
  - **No cause keys.** It is **not** in `natural_causes[]` and has no `cause_family` (it is our swap, not a defect).
- **MISS-2:** `missing_texture`, a new subtype for the engine placeholder (the plan names it), `injection_mode: natural`, `cause_id: MISS-2`, `cause_family: content_error`, recorded in `natural_causes[]`.
- New subtype values are additive.
- Fix the plan's example records in J6: two events in one session cannot share a `mask_value`.

### P5. Cause families and the shipped cause tags
- **Three families stay.** A settings mistake is a kind of `content_error` (design §3). CAM-1 = `content_error` is correct; R2's word "config" was descriptive. No fourth family.
- **GONE-3 is dropped** from the shipped tags. Blinking's mode 0 is bench-only, so the tag could never appear in client data.
- **Shipped controlled anomalies that get cause tags:** LOD-1, NRM-1 (green_flip only) and CAM-1. No others.

### P6. Office forms: compact for the owner
- A local reader script on the office PC computes the full numeric forms (I and N), which stay on the office PC.
- It prints **one short line per case**: PASS/FAIL, the handful of numbers chat needs, and a check digit.
- **Target:** at most about 20 typed numbers per host per checkpoint, instead of about 450 in total.
- Host codes: **1 = first office host, 2 = second office host**, never names.
- The I-failure counters print only on FAIL.

### P7. Job split adjustments
- **J2** includes rebuilding its can-fail harness (P2).
- **J5** is smaller (P3).
- **J11 is split:**
  - **J11a:** home full regression, PIE and staged, both fixtures, both pilots;
  - **J11b:** office-package prep and the checkpoint N reader.
- **G-CONTROLLED-IDENTITY** runs in J5 and J11a.
- Continuations are allowed when a matrix doesn't fit one bench session, without weakening any gate.
- **J10 (the UI) waits for the owner's A/B choice.**

### P8. How implementation runs
- The **mailbox Code orchestrates and reviews; Codex implements** through the implementer Relay. This is the owner's setup of 2026-10-02, with full access per `090-12s`.
- **Part 0 of the first implementation brief** is a small guard change in `relay_run_codex_gdp_impl_v1.ps1`:
  - a new `-OrchestratorBrief <name>` parameter allows a launch while the watcher runs, **only** when the watcher's running brief is exactly that name;
  - the name is recorded in `run-meta.txt`, and the README is updated.
- **One bench, one implementer is unchanged:** an orchestrating mailbox brief never runs bench legs itself. Its review is reading and recounting only.

### P9. Owner decisions (asked now)
1. **Go / no-go** for implementation, in phases:
   - J1–J2 (base + F1) → office checkpoint I;
   - J3–J9 (foundation + MISS-1/MISS-2);
   - J10–J11 (UI + qualification) → office checkpoint N.

   Codex estimates 40–62 h of its time; Code expects more. Bench legs run only when the PC is idle with no foreign editor.
2. **UI shape for J10.** Chat recommends **A** (a Controlled/Natural choice beside each pool row).
3. **Which office hosts the checkpoints cover.** Chat recommends: the second office host (it received M3) required; the first (VT-heavy) optional and informational.
4. **When the client hears about the new label keys.** Chat recommends: with the first natural build.

### P10. Branch policy (owner, 2026-10-13): the natural work never mixes with regular deliveries
**The owner's rule.** The natural-cause work lives on its own branch, separate from regular deliveries. It can be paused at any time to switch back to the usual workflow, where new anomalies are added the usual way and the usual builds ship. When the time comes, natural anomalies join regular builds the way any new anomaly does.

**Branches:**
- **`fix/followup-capture`** holds J1 and J2 only:
  - the release `d7eee97`;
  - a merge of `5d6352b` (the 091-05 follow-up fixes);
  - the office-robust F1.

  These fix shipped behaviour and contain **no natural-cause code**. They enter a regular delivery only when the owner decides, after office checkpoint I.
- **`research/natural-causes`** is created from the `fix/followup-capture` head once J2 passes at home. J3 onward lives **only** here.
  - It may merge *from* the regular line, one way only, to stay current.
  - It is **never** merged into `master`, a release branch or any delivery branch without the owner's explicit go.
- **Dashboard:** J10's natural controls go on a `research/natural-causes` branch of the dashboard repo, never on its `master`, which is the client's dashboard.
- **Regular work** (for example m54 flicker, object clipping) continues on its usual branches from the release line, untouched by this track.

**Pausing and resuming:**
- Each natural job ends at a clean, pushed commit, with the bench and hosts restored by receipt.
- When the owner switches to regular work, chat sends no further natural briefs.
- On resume, the natural branch first merges the regular line's new commits.

### Owner answers (2026-10-13)
1. **Go** for all three phases.
2. **UI shape A.**
3. **Office checkpoints:** the second office host is required; the first office host is optional and informational.
4. **Client notice:** telling the client about the new label keys is **not needed now**. It comes when natural anomalies are added to a regular delivery the usual way.

## Next
- **Released:** 093-03, with Part 0 (the relay guard) and **J1** on `fix/followup-capture`. Codex implements and Code reviews.
