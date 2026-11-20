# REPORT for 081-02-m55-plan-revision-per-codex-review.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-02-m55-plan-revision-per-codex-review.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-21 11:33:55 | finished: 2026-09-21 11:59:27 | duration: 26 min | exit code: 0
context at last turn: 492722 tokens (49% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-02-m55-plan-revision-per-codex-review.live.txt

---- FINAL MESSAGE (Claude Code) ----
Done. Checkout restored to `m51` `53bf725`, tree clean, temp copy removed.

---

## 081-02 — `m55` plan revised

**`docs/predictions/2026-09-20-m55-change-evidence.md` rewritten as v2** (`ddc2361`, citation fixes `ae2a967`), v1 preserved byte-verbatim under a `SUPERSEDED` fold — git confirmed it independently: the first commit was **945 insertions, 0 deletions**, so the old text was recognised as unchanged context. Journal `docs/sessions/2026-09-21-081-02-m55-plan-revision.md`. Branch `feat/m55-change-evidence` == `origin` == `ae2a967`. `master` `031a103` and `m51` `53bf725` untouched; checkout left as found. **Still plan only — no source, build, cook, capture or leg.**

**All nine rulings landed** as three contracts (§1 numeric, §2 identity/lifetime, §3 temporal) + a twelve-value closed refusal enum + a durable sidecar + the declared bench twin + deferred calibration + a gate table where **every row of Codex §4 and §6.6 has a gate or an explicit UNEXERCISED line with its reason**.

### Three source findings that changed the shape

1. **The ordering inversion is exact, not hypothetical.** `Drain_RenderThread` scans in-flight items *backwards* (`AnomalySveCapturer.cpp:287`), appends to `Completed` (`:359`), and `PopCompleted` is FIFO (`:371-379`) — so **two frames ready in one drain pass are popped newest-first**. Held batches re-order again. `R3`'s ordered stage is a necessity, not a precaution.
2. **A per-row change field cannot ride `labels.jsonl`.** The row is serialised in the drain (`AnomalyCaptureSubsystem.cpp:4132`) and handed to the writer at `:4151`, *before* that frame's canonical buffer exists. ⇒ the per-pair numbers go to the new `change_evidence.jsonl` — which also closes `F3`'s delivery hole, since `:4150` can switch `labels.jsonl` off. Consequence: **v1 touches no `labels.jsonl` row shape, no `annotation.json`, no `label_schema`.**
3. **With the GPU route parked, `m55` v1 needs NO COOK.** No shader, no parameter struct, no new `Content` — `solid_swap` reuses the already-cooked `M_CorruptedTexture_Pink` via `GetCorruptedTextureMaterial()`. Code-only hot-swap (`G103`); the container quintet and every `_binary_baselines` pairing survive. New gate `G-NOCOOK` holds it to that.

### Flagged, not silently bent

**No ruling was made impossible by the source.** Three items are constrained and are declared as such:

- **`R6`'s pre-onset visibility record.** `prev_target_pixels` is published as instructed — **and its value is `-1` on every onset pair by construction**, because mask arming is gated on the fire being labelled (`AnomalyCaptureSubsystem.cpp:1156`, predicate `:4845-4865`), so the frame before a phase is never armed. The field exists; the record it was meant to carry does not. **`m55` v1 does not establish pre-onset visibility.**
- **`R3`'s cut handling.** Level/world/reset is covered by the epoch; a **same-size camera cut is not detectable** by anything in the producer and is reported only via `cam_moved`, which `R3` itself defines as a caveat. Carried as UNEXERCISED with what would exercise it named.
- **Gradual sub-threshold ramp.** Covered in two halves (`solid_swap delay=3` end-to-end; Codex's `0,3,6,9,12` counterexample as a verifier selftest fixture) with the producer-side ramp declared UNEXERCISED — no runtime-authorable sub-threshold colour ramp exists without a material parameter, which is a `Content` change that would force the cook.

**One wording deviation, for you to overrule:** `solid_swap` uses the shipped **Lit** magenta, not "a flat unlit colour" — `G50`/`m29` measured unlit-emissive magenta lighting the Lumen scene and glowing onto neighbours, which inflates exactly the control this pair exists to read; and a new material asset forces the cook.

**Also declared:** the registered catalogue moves **10 → 12** (visible in `IAI.ListAnomalies` and the dashboard), contained by exclusion from `GAutoPool`/`GAutoPoolDefaultEnabled`/`GAnomalyChoices` and a refusing `Apply`.

The prior that most shapes §8's deferral: on all three banked `G270` Lyra events the **control is the same order as the target, and on event 1 it is higher** (0.1774 % vs 0.2687 %).

VERDICT: GREEN
api-equivalent cost (not billed on Max): $25.43 | turns: 89 | running total across all briefs: $2,002.56
