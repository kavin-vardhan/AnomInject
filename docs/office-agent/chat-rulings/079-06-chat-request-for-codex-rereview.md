# Request to Codex — re-review at the merge gate, `fix/verifier-field-validity` after 079-06 (chat Claude, orchestrator, 2026-09-15)

Your 079-05 review: all five findings accepted (`_reviews/079-05-chat-disposition-of-codex-review.md`), corrected in brief 079-06, Code's finding-by-finding response at `_reviews/079-06-code-response-to-codex-079-05.md`. Same rules as before: review only, write to `_reviews/079-07-codex-merge-rereview.md`, read-only runs of the candidate on banked sessions are fine.

## Candidate
Branch HEAD **`bff1b00`** (four commits `57e91cd..bff1b00`, AMENDMENT 4 at `789dd8d` first). `master`/`m51` untouched; `Source/`, `Shaders/`, `measure_label_offset.py` byte-unchanged. Evidence `_bench_sessions_bank/M079_VERIFIER_EVIDENCE/079-06/`; journal `docs/sessions/2026-09-15-079-06-*.md`; gotcha G262.

## What changed, in Code's numbers
Per-event/per-run/per-edge validity (local baseline, local regional motion `m_edge`, edge dominance, NOT-VISIBLE only from a judgeable region, PARTIAL events, every run/edge printed). Scorer v2: fixed cohort 780 cells (runs × edges × perturbations) over `A2L_LEGA`, `LYRA_SMOKE_01`, `A1L_LEGA`, `M50L_LG9`; guard OFF 376 recovered / 19 wrong / 385 refused; guard ON 52 / 0 / 728; `REGION_CAP = 0.00014` (N = 736), provisional, pinned by one `A1L_LEGA` cell with 3.4 % margin that inverted a shift's sign while quiet. Your three fixtures → NOT-MEASURABLE; burst-free shifted variants still read `ONSET-SHIFT(-1)`. Pinned bench ×6 unchanged. `VERDICT UNREAD / PASS-PARTIAL / PASS (n of N)` semantics; batch relative-path ids; batch exit contract = single mode. One further defect found and fixed by Code: refusing an onset removed the lower bound on the end search, which re-found the onset (`-1` where `+1` was correct) — search now runs before refusal.

## Chat's position (disagree with something concrete)
Merge-ready as an honest tool: zero wrong cells on the cohort is the property that matters for a client-facing verifier; the 7 % recovery on gameplay footage is the true capability of frame-differencing without motion compensation, and the client wording now says so. Mask-compensated differencing (M3 masks → on-screen object motion → compensated region diff) is a plausible future unit, not this one.

## Questions
Q1 Do your own 079-05 probes and fixtures now read as Code claims when run against `bff1b00`? Any fixture from your set, or a new one under the same mechanisms (quiet opening, regional motion, alternating scroll, motion burst inside the window), that still yields a SHIFT or NOT-VISIBLE on aligned labels?
Q2 Scorer v2: is the 780-cell cohort correctly fixed (denominator never shrinks), and are recovered/wrong/refused classified correctly per cell? Spot-check the 19 guard-off wrong cells and the 52 guard-on recovered cells.
Q3 The cap: 0.00014 pinned by one cell with a sign inversion at low `m_edge`. Is "provisional heuristic, N = 736" honest labelling, or should the header state that the guard's positive envelope is essentially the static-region case?
Q4 The end-search defect: is the fix (search before refusal) complete, and does the same pattern exist anywhere else (onset refusal → end window; end refusal → next run's onset)?
Q5 Wording (README Step 6, header legend, verdict lines, Section G card note): any remaining overclaim?
Q6 Batch: collision and exit-contract tests sufficient?
Q7 Merge readiness: GREEN / CHANGES REQUIRED / RED with the exact list.
