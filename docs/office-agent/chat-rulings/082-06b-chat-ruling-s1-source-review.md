# 082-06b — Chat disposition of Codex's S1 source review (082-06, CHANGES REQUIRED: 0 P1, 6 P2, 3 P3). All accepted; fix before authoring

Date: 2026-09-27 (IST) · Author: chat Claude (orchestrator) · Review `_reviews/082-06-codex-m53-s1-source-review.md` (gpt-6-astra / max, 25 min) · Code at `193bd35`

Codex confirms the per-mip draw-and-copy design holds up in the engine source. All nine findings are accepted.

## Fixes (each fix gets a commit, a proof, and an entry in the plan's resolution table)
- **P2-1. NoApply must go through the real path.** It must not short-circuit `IsVisualConditionHeld`.
  - The predicate evaluates the live slot and parameter ownership, and returns false only because nothing is installed.
  - Proof: with NoApply on, the predicate code runs and reads "not installed". With apply on, the same code reads "installed".
- **P2-2. G-COLL measures every rendered primitive in view**, not the injection candidates. Use a recently-rendered test such as `GetLastRenderTimeOnScreen` within the capture window.
  - If a cap truncates the set, set `collateral_truncated`, add an incompleteness count, and never report a clean result on a truncated set.
  - Proof: a synthetic case where a collateral texture sits outside the candidate filter is still measured.
- **P2-3. The annotation subtype comes from the captured event record,** never from the live mode at frame completion.
  - Proof: a frame completed after revert keeps its event's subtype, and two events of one id carry their own modes.
- **P2-4. The default-material guard covers every usage flag that applies,** mirroring the engine's full rule. That includes instanced and Nanite meshes, static lighting across **all** LODs, skeletal meshes and splines.
  - If a requirement cannot be determined, **refuse**.
  - Declared deviation 5 is rejected.
  - Proof: fixture cases for each flag class, or offline rows where the engine rule can be evaluated without the game.
- **P2-5. Every scratch and output release goes through the two-frame pending ledger,** including a partial scratch-set failure.
  - Add pure-harness tests that execute the ledger and rollback sequences: tree-order execution, partial failure at each step, balance back to zero.
  - The mutant run must fail them.
- **P2-6. NoApply 2 registers the target watches, `EndPlay` included,** just like apply.
  - Declared deviation 10 is rejected.
- **P3-1.** The post-revert sample is counted in **rendered frames**, not ticks.
- **P3-2.** A chain walk that goes past its link limit is **refused** with a named reason. It is never accepted as clean.
- **P3-3.** Balance is judged from live and pending bytes returning to zero. `texcorrupt_rt_bytes_peak` stays a run peak. Fix the plan's gate text.

## Loop
- **082-06b (xhigh):**
  1. the fixes;
  2. a comment strip;
  3. an editor and game rebuild on the kept warm host (`G10`; archive the binaries, **do not stage**);
  4. the extended harness, with the mutant check;
  5. a **Codex delta check through the Relay** on the fix commits;
  6. collect it and **do not act** on it.
- **Next:** if the delta check has no P1 and no unresolved P2, **082-06 (authoring + cook)** follows.
