# 093-07 — Chat ruling: the projection fix holds; PF-1 stopped on targeted fires reaching an off-screen target; add on-screen admission and continue J2

**Date:** 2026-10-14, about 17:10 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- Codex's continuation: `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\report.md`. It is the resumed J2 thread, `gpt-6-astra`/`max`, 10,690 s over two tries (try 1 hit a model-capacity error; the relay retried it). VERDICT NEEDS-DECISION.
- The mailbox session for brief 093-07 hung at about 12:27 IST. Its live feed stopped, and it never noticed Codex finishing at 14:15. **There is no Code review and no mailbox report for 093-07.**

## Verdict
**J2 is still not qualified. Nothing was pushed, which is correct.**
- Remote `fix/followup-capture` stays at `1bf7054`. Local head `9514242`; product `f9b9328`; archive `followup-capture-J2-projection-09307-f9b9328-7A69B4EE-7A499731-UNQUALIFIED`.
- **The projection fix holds where it was tested** (chat's reading of Codex's evidence; Code reviews the diff in 093-08):
  - On the exact 093-06 stop arrangement (Lyra, MaintainYFOV, 942×869), the new build wrote **0** rows claiming the off-screen target visible. The old build wrote 96.
  - The new `projection` block reads axis constraint 1, camera aspect 1.333, view rect 942×869, from the serving view's unjittered geometry.
  - The design note and predictions were committed first (`a28c72f`).
  - A second FOV reconstruction was found and replaced: the near-clip projector behind camera clipping.
- **The stop was right** under D2 and the frozen PF-1 prediction. The off-screen target still received **4 applied fires**. The existing MEASURED_ZERO veto then removed their annotations, so no wrong label reached the data. But PF-1 required that nothing fires on an off-screen target, or that the attempt is refused with a counted reason before it applies.
- **The office reader changes are accepted, pending Code's review:**
  - `a=` removed;
  - `m=` counts unmeasured labelled rows without crashing (`m=96` on the retained session);
  - contact sheet instead of a hand-written file;
  - box read `b=999` (StackOBot) and `b=967` (Lyra, old build);
  - Python 3.10 proven.

## Rulings

### L1. Root cause of the stop
- **The path:** `BeginFire` → `TryFireSpecific` finds the named actor and reaches `ApplyAnomaly` with no on-screen query. This path is unchanged since the release.
- **What happens:** in targeted mode the plugin applies the anomaly to the named object wherever it is. Only the mask veto afterwards stops a bad label.
- **Why it matters:** labels stay right, but it breaks the client's rule that every injection happens on screen (093-02 §1a). The natural-cause lifecycle will need the same admission check anyway.

### L2. Fix: on-screen admission for targeted fires
**Before** a fire is applied to a named object target, test the target's bounds against the engine projection of the resolved capture owner (the 093-07 projector).
- **Frustum only.** Occlusion stays with the existing mask veto, so a visible but partly hidden target is not refused.
- **Off screen:** refuse and never apply. Count `targeted_refused_off_screen`.
- **No owner projection yet** (for example, a delayed viewport): refuse. Count `targeted_refused_no_projection`.
- **Counters:** both go in `run_summary.json` only, as additive diagnostics. A refused fire is retried on the existing cadence.
- **Other anomalies:** frame-wide anomalies keep their own eligibility.

**Process and scope:**
- The design note lists every path that applies an anomaly to a named target without a visibility check, including dashboard and console single-target fires. It is committed with the predictions **before** any product change.
- Scope: the fire-admission path and the two counters. Anything else is a STOP. So is an engine edit, a new module dependency, or a private header beyond what capture already uses.

### L3. Timing: Codex's open point, accepted as built
- **Labels, masks and visibility flags** come from the frozen serving record (world-tick end, then the serving view's unjittered geometry carried through readback). This meets K3.
- **Target selection and the new admission check** may use the last completed camera, which can be one frame old. The mask veto is the backstop, and the camera-switch leg (K6) measures the effect at cuts.
- K3 is satisfied with this clarification.

### L4. Harness items (D2 harness rules apply)
1. Rebuild and verify the Lyra Editor helper (its `Switches` command was missing) before any camera-switch leg.
2. The camera-switch reader counts pending pre-switch samples as drops (Codex's own note).
3. Run the prepared archived-binary replay for the mask-based 0.813 can-fail. The retained session has no measured masks, so it cannot supply that number. That was disclosed before any leg, which is correct.
4. Produce the reader's healthy `I` PASS line from a new engine session.

### L5. Predictions after the admission change
- **Re-freeze before any leg:** the admission can change event counts where fires used to land before the owner view resolved. Re-freeze those predictions first, with the reason for each. Code reviews every amendment.
- **The floors never move:**
  - v ≥ 3;
  - edges 0;
  - every labelled frame with a measured mask;
  - w = r.
- **PF-1 re-run:**
  - 0 rows visible;
  - 0 applied fires on the off-screen target;
  - at least one counted `targeted_refused_off_screen`.
  - Can-fail: the 093-07 PF-1 session (4 applied fires) must be rejected by the checker.

### L6. Order and review (unchanged from K7, plus the missing review)
- **Order:**
  1. PIE: PF-1 re-run, then PF-2 to PF-4, then the J2 matrix on both fixtures, then the camera switch.
  2. The cook.
  3. Every staged gate.
  4. The push, only when everything is green.
- **Code's review of 093-07:** Code reviews the 093-07 product diff (`a28c72f..f9b9328`) **in parallel**, read-only from git objects, while Codex works. If it finds a defect that would invalidate the continuation's legs, it stops the Codex job by PID (only the relay-started process) and reports NEEDS-DECISION.
- **Specific check:** the "camera axis override when available" commit (`f9b9328`) must be a runtime-optional lookup that compiles and behaves correctly on UE 5.1. Nothing UE 5.7-only may be used.

### L7. The hung mailbox session (owner action)
- **What happened:** Code's 093-07 session stopped producing output at about 12:27 IST. The watcher still shows it as running, so the next brief can't start.
- **Likely trigger:** a stalled API call or output pipe around the time of a network blip. Codex saw 503 errors at the same moment. This is not established.
- **Owner action:** close the watcher window, which ends that session, then start the watcher again. The watcher files 093-07 as interrupted and picks up 093-08, which carries the `ACK-VERDICT` line.

## Next
- **093-08 is queued:** Codex continues J2 with L2 and L4–L6, and Code reviews (both the 093-07 diff and the new work).
- **After J2 qualifies at home:**
  1. Office checkpoint I with the new reader: the second office host is required, the first is optional.
  2. Then `research/natural-causes` and J3.
- **Still open from K8:** the owner's `AspectRatioAxisConstraint` read on both office hosts.
