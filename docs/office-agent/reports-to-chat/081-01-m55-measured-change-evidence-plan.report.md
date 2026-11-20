# REPORT for 081-01-m55-measured-change-evidence-plan.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-01-m55-measured-change-evidence-plan.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-20 23:12:22 | finished: 2026-09-20 23:31:11 | duration: 19 min | exit code: 0
context at last turn: 454404 tokens (45% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-01-m55-measured-change-evidence-plan.live.txt

---- FINAL MESSAGE (Claude Code) ----
Cleanup complete. Everything is exactly as found.

---

## 081-01 — `m55` "measured change evidence": PLAN DELIVERED

**Branch `feat/m55-change-evidence` `287ad3e`, pushed, == origin.** Two files, 861 insertions, nothing else:
- `docs/predictions/2026-09-20-m55-change-evidence.md` (the plan, 713 lines)
- `docs/sessions/2026-09-20-081-01-m55-change-evidence-plan.md` (the journal)

**Repo state:** `master` unmoved at `031a103` == origin. **`m51` `53bf725` untouched and the main checkout was never moved** — the work ran in a `git worktree` outside the StackOBot tree (shared-tree rule 2 covers doc-only commits; outside `Plugins/` because UE scans it recursively), so "leave the checkout as found" is a property of the method rather than something to remember. Worktree removed at close, branch kept. Stripper `changed 0, nochange 104`; diffstat read before commit; both files no-BOM, valid UTF-8, LF, no mojibake.

### The design in one paragraph
Per captured frame and per tagged target, count the target's own mask pixels that differ from **the previous captured frame's delivered colour**, plus the mean per-pixel magnitude, plus the same two statistics over the rest of the frame as a paired control. Publish all of it additively. Latch a per-event `change_evidence` verdict **at onset** and let `observable` consult it, admitting wherever the measurement isn't sound. Mechanism: a new change-reduce CS at the colour SVE's own hook, consuming the mask published from the Tonemap hook within the same graph; retention is `ConvertToExternalTexture` on the texture the colour SVE **already builds** for the PNG. The existing reduce is untouched, so `MASK-TIE`, `target_pixels`, `target_drawn_pixels`, ONSET and pairing are unchanged *by construction*.

### Two findings that changed the design
**1. The brief's control arithmetic should not be built.** It asks that `change_frac_target − change_frac_control` be what the label carries. `G260` built that subtraction, gated it, and had it refuted in both directions — it doesn't cancel camera motion (parallax is content-weighted), and on a pinned bench leg it went **negative at the exact frame the anomaly started**, because the premise that the effect stays inside the region is false here (`m26`'s `A35`: hiding `SM_Ramp2` changed *more* outside its bbox than inside). The brief's justification removes a *pairing* confound; `G260` was never about pairing. Both failure modes are properties of the scene, so measuring in-engine repairs neither. The plan publishes both fractions, subtracts neither, and uses the control as a **refusal input** per `G260`'s own rule. Both fields ship, so a subtractive rule stays one arithmetic step away if chat overrules — no code change, no re-capture.

**2. A temporal difference measures transitions, not states — and `observable` is per-frame.** Every steady-state class produces identical consecutive frames once engaged, so the literal per-frame gate would mark the onset frame observable and every later held frame not, emptying `affected_frames` to one frame per event across the dataset. Evidence is therefore event-level, latched at onset — and needs no lookahead, because the onset's change is measured on the very frame that needs it (rows can't be retro-edited). `m52`'s `080-06` window helps here: it already puts the labelled window at the first held frame, so the measured pair straddles unheld→held.

### Source facts worth carrying
- The mask is made at `EPostProcessingPass::Tonemap`; the **delivered** pixels are grabbed at `VisualizeDepthOfField`, two subscribable passes later ⇒ delivered colour is post-FXAA, the mask's hook is not.
- **`m49` reads no colour at all** (custom stencil + custom depth + scene depth) ⇒ `m55` would be the producer's first colour-derived label evidence.
- The mask SVE is active only while arms are pending; the colour callback runs on exactly the captured frames ⇒ **retention must ride the captured-frame cadence**, or the last pre-roll frame — the predecessor of *every* onset — would never be retained. That single constraint chose the mechanism.
- **`m51`'s `F1` is NOT on `master`** (`6713f51` is not an ancestor; `bAwaitingTargetMask` absent) — no code interaction, but two design surfaces to reconcile at merge.
- **Every GPU route needs a full cook** (`G129`), retiring a container byte-unchanged since `m49` Phase B. A CPU fallback with identical semantics and no cook is costed, not dismissed.
- `lighting_mismatch` has no mask and can't be given one here (lights carry no primitive component) — it already ships `observable: null`.

### Open questions for chat — none decided here
- **Q1** The subtraction (above). Confirm the `G260`-compliant shape, or overrule with the reason recorded.
- **Q2** The cook: GPU + full cook now, or the CPU build with identical semantics and no cook.
- **Q3** Does `observable` actually change in `m55`, or does `m55` ship measurements + the per-event verdict and a follow-up flips it once floors are derived? The second is slower and strictly safer.
- **Q4** `K`, the provisional-admission window (proposed 4, matching the veto's arm budget).
- **Q5** The light class — confirm a future `m53` brings its own region source rather than `m55` growing one.
- **Q6** `stuck_low_mip`'s default-on decision, parked pending `m55`; gate `G-3` produces the reading that settles it.

### Deliberately not done
No source/shader/tool/fixture/schema change, no build, cook, capture, leg, tag or merge. No floor or `tau` chosen — §8 is a derivation procedure, predeclared before the implementation gate. `P6` doesn't move, `label_schema` stays 2 (`m51` owns the bump), `change_*` is never an input to the `m26` veto. **`CLAUDE.md`'s status block was not touched** — it isn't in the brief's deliverables, the unit is a proposal pending review rather than an advanced milestone, and editing it on an unmerged branch would create a second `CLAUDE.md` divergence beside the `m51` one.

**Next: independent Codex review of the plan, then chat rules Q1–Q6.**

VERDICT: GREEN
api-equivalent cost (not billed on Max): $13.65 | turns: 54 | running total across all briefs: $1,977.13
