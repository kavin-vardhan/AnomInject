# 081-02 — chat disposition of Codex's m55 design review (chat Claude, orchestrator, 2026-09-21)

**Ruling: all nine findings ACCEPTED. The plan at `287ad3e` is withdrawn as written and rewritten around three explicit contracts (numeric, identity, temporal). m55 v1 = measurements only; m49 `observable` unchanged; no present/absent verdict word in v1; calibration removed from m55 into its own later unit; GPU route parked.**

Brief 081-02 to Claude Code (plan revision, still no code) carries the rulings verbatim; the next Codex checkpoint is the review of the v1 implementation's identity / arithmetic / coverage / cost evidence, as Codex proposed. Changing the meaning of `observable` stays an owner/client decision (operating contract §3) and will be put to the owner only once a calibrated policy exists.

| Finding | Disposition |
|---|---|
| F1 missing onset ⇒ ABSENT; first-K cannot see gradual change | Accepted. Phase-scoped onset pair with `pending` → `indeterminate` when unmeasured; no negative reading from later quiet pairs; pre-onset reference comparison published (caveated, never used for a negative call). K=4 documented as a budget, not a completeness bound. |
| F2 identity insufficient; `PendingSnapshots` lookup expires | Accepted in full, including §4's table: immutable receipts with the retained bytes, receipt-based joins, ordered CPU stage with bounded wait, enumerated refusal reasons, epoch reset clears everything together, late results rejected. No workaround via longer `PendingSnapshots` retention. |
| F3 progressive output vs latch inconsistent; delivery hole | Accepted. v1 has no verdict; running aggregates named as running; durable final per-event record outside `labels.jsonl`; finalisation enumerated; phase (contiguous held run) is the scope. |
| F4 G-1 lever supplies no labelled pairs | Accepted. Lever kept as a no-label control; two bench-only anomalies declared (`null_effect`, `solid_swap`) as the null / positive twins through the real path. |
| F5 verifier outcomes as truth; G270 ≠ zero change | Accepted. Calibration leaves m55. G-3 becomes an observation gate with Codex's bank probe as the prior. 8-bin per-pixel-diff histogram published so thresholds can be swept later without images. |
| F6 verifier agreement compares different quantities | Accepted. `--change-oracle`: same pair ids, same region, same threshold, count-vs-count and mean-vs-mean, coverage reported; peak machinery untouched. |
| F7 GPU pass ≠ delivered bytes; retention not free | Accepted. Canonical pixels = the writer's BGRA8 bytes; PNG-only supported; shared immutable ref with the writer; byte budget stated; GPU parked. |
| F8 G-4 makes drawn==0 mandatory | Accepted. Supporting evidence only; depth-silencing case stays accepted; already-occluded onset ⇒ `empty_region` ⇒ indeterminate. |
| F9 control refusal destroys raw data | Accepted. Raw always published for valid pairs; validity (`chg_measured`) separate from eligibility (`chg_eligible`, reserved); both denominators guarded; no control threshold in v1. |

Plan Q1–Q6 as Codex disposed: no subtraction; CPU first; `observable` preserved; K=4 experimental; light class stays with m53; stuck-mip default-on unchanged.
