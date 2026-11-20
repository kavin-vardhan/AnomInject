# 093-06 — Chat ruling: J2 is NOT YET QUALIFIED; the stop was right; fix the projection defect on this branch, then requalify J2

**Date:** 2026-10-14, about 11:30 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- Codex's J2 job: `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\report.md` (`gpt-6-astra`/`max`, full access, 21,241 s, VERDICT NEEDS-DECISION).
- Claude Code's review: `_reviews\093-06-code-review.md` (30 OK · 2 GAP · 1 OPEN; observations A–D).
- Mailbox report 093-06: NEEDS-DECISION, 375 min, $17.53 API-equivalent.

## Verdict
**J2 is not qualified. Nothing was pushed, which is correct.**
- Remote `fix/followup-capture` stays at `1bf7054`. Local head `bb9e741`; product frozen at `f608d56`; archive `followup-capture-J2-09306-f608d56-4BB0F074-B0F2B30C-UNQUALIFIED`.
- **Part 0 (J1 closure) is accepted in full** (K1).
- The 22 passing engine cases are accepted as evidence for `f608d56`. They do not qualify the final binary (K7).
- **The stop was right** under D2: a product gate failed. Codex was also right not to reframe the fixture to make it pass.
- Code's review is upheld, both GAPs included.

## Rulings

### K1. Part 0: accepted
- The wrong-object can-fail rejects in PIE and staged (exit 1/1), and the originals pass (0/0). Code re-ran it himself. J1's can-fail list is now complete.
- Status block corrected (`59b2fa1`). The masked-alpha limitation is recorded (G479 and the readme). The 3-instead-of-4 events are explained: attempt 0 was refused as `texture_uniform:pending` in both staged proxy runs.

### K2. The stop: a real, pre-existing projection defect
**What happens.**
- `AnomalyViewport.cpp` (about `:711-728`, unchanged since the release `d7eee97`) rebuilds the view from the camera FOV. It assumes the FOV is horizontal at the viewport's own aspect.
- UE 5.1 does that only under `AspectRatio_MaintainXFOV`.
- Under `AspectRatio_MaintainYFOV` (Lyra's setting), `CameraStackTypes.cpp:176-183` treats the FOV as horizontal **at the camera's own aspect ratio** (4:3 by default), then widens it to the viewport.

**Size of the error**, on both axes: viewport aspect ÷ camera aspect. Prediction and measurement agree:

| Case | Predicted | Measured |
|---|---|---|
| Lyra, 942×869 selected viewport (the stop) | 0.813 | 0.814 x / 0.813 y |
| Lyra, 1123×870 PIE window (the *passing* legs) | 0.968 | 0.9672 x / 0.9677 y |
| StackOBot (MaintainXFOV) | 1.000 | 0.9997–1.0009 |

**What it breaks.** Per-frame `labels.jsonl` only. In the failing session `annotation.json` stayed correct (`manifested:false`).
- `bbox_px` is mis-scaled about the view centre on any MaintainYFOV host whose viewport aspect differs from the camera's.
- The projected `visible_positive` is wrong near the screen edges:
  - **Viewport narrower than the camera aspect:** off-screen objects are counted as on-screen. That is the stop: 96 rows labelled with zero target pixels and no mask.
  - **Viewport wider** (for example 16:9 against 4:3, factor 1.33): boxes are pushed outward by a third, and on-screen objects near the edges are treated as off-screen.
- Targeting uses the same check, so on such a host every anomaly type is affected. The natural work would be too: "on screen" is decided here.

**History.** The m51-era review (`_reviews\078-16-draft-first-read.md` §C.3) listed this reconstruction's gaps:
- the axis constraint and the constrained view rect;
- the hard-coded perspective projection;
- off-centre projection;
- host projection hooks.

It proposed taking the geometry from the engine instead. That was never implemented. J2's Lyra coverage has now shown one of the gaps is real.

### K3. Fix it now, on this branch (P10 amended)
**Where:** `fix/followup-capture`. It is a defect in shipped behaviour, not natural-cause work, so it belongs with J1 and J2.
- **P10 is amended:** this branch carries J1, J2 and the projection fix. Still no natural-cause code.
- It still enters a regular delivery only by the owner's decision, after office checkpoint I.

**Design (binding).** Every projection the product uses (target eligibility, `visible_positive`, `bbox_px`) comes from **the engine's own projection for the resolved capture owner**. One of:
- `ULocalPlayer::GetProjectionData(<owner viewport>, ProjectionData)` on the game thread, after that frame's camera update. Then `ComputeViewProjectionMatrix()` and the projection data's view rect.
- The owner view's own matrices and rects, taken from the existing scene view extension for that frame and frozen with the request (the 078-16 "geometry record"). Use the unjittered projection.

Mapping and process:
- Map through normalised coordinates of the same rect the capture copies, so screen percentage and output resampling cannot shift a box.
- Codex picks the route and writes a design note into the journal and predictions. The note covers the callers, the thread, the timing and the rect mapping.
- The note also lists **every product use of the camera FOV or aspect**. Each one either moves to the engine projection or is shown not to depend on it.
- The note is committed **before** any product code changes.

**Not allowed**, each one a STOP (NEEDS-DECISION):
- any FOV/aspect reconstruction left in the targeting or label path;
- an engine edit;
- a private header beyond what capture already uses;
- a new module dependency.

**Scope:** the projector and its direct callers in the targeting and label path. A needed change anywhere else is a STOP.

**Labels:**
- No key changes meaning. `fovDeg` stays the camera's configured FOV.
- On MaintainYFOV hosts, `bbox_px` and `visible_positive` become correct. That is a fix, not a change of meaning.
- Diagnostics go in an additive `projection` block in `run_summary.json` only: effective axis constraint, camera aspect, viewport size, view rect, projection mode.

The engine route is expected to close the rest of the 078-16 §C.3 list as well (constrained rect, projection mode, off-centre, host projection hooks). Only the axis constraint and the constrained rect get engine gates now.

### K4. Gates for the projection fix
Predictions are written first, and each gate has its can-fail.

1. **PF-1: the observed failure, kept as a negative test.**
   - The setup: the exact 093-06 stop arrangement (Lyra, selected viewport 942×869, MaintainYFOV, target just off the left edge).
   - The fixed build must label the target visible on no row. No event fires on it, or the attempt is refused with a counted reason.
   - Can-fail: the retained `f608d56` session (96 false rows). The checker must reject it.
2. **PF-2: box agreement.**
   - The check: the projected box against the mask-drawn box, per axis. A scale estimate of the two boxes' centres about the view centre must be **1.000 ± 0.010**.
   - Codex freezes the exact estimator, including which rows count, in the predictions before any leg.
   - PIE matrix (12 legs): both fixtures × {MaintainXFOV, MaintainYFOV} × {942×869, 1123×870, 1280×720}. The constraint is set by a process-local override only; never edit a host config file.
   - Staged (4 legs): both fixtures × both constraints at 1280×720.
   - Can-fail: the archived `f608d56` Lyra sessions must read about 0.967 and 0.813 and be rejected. The archived StackOBot sessions must pass.
3. **PF-3: no change on MaintainXFOV hosts.** StackOBot regression legs, fixed build against `f608d56`:
   - same events, targets and edges;
   - identical `visible_positive`;
   - `bbox_px` within 0.5 px.

   The comparison method is frozen in the predictions.
4. **PF-4: letterbox.** One StackOBot PIE leg with a constrained-aspect camera (black bars). It gives correct boxes, or a counted refusal. Never a wrong box.

### K5. The office reader
- **GAP 1 upheld.** Remove `accepted_ambiguous` from the `capture_ownership` block and `a=` from the line, because they can never be anything but 0. `ambiguous_owner` and the first-wins mutant already prove the ambiguity refusal.
- **GAP 2 upheld.** The reader must never crash on a labelled row with `mask_file: null`.
  - It counts those rows as `m=`: labelled without a measured mask. PASS needs 0.
  - It still computes everything else it can.
- **No hand-written input.** The 20-frame inspection becomes a contact sheet that the reader writes next to the session.
  - The sheet shows 20 distributed frames, with the frame number under each. It stays on the office PC.
  - The owner looks once and appends ` g=<how many don't show the game view>`. PASS needs `g=0`.
  - The manual edge-pair inspection is dropped: edges are measured (`e=`).
- **Compact line (P6).**
  - PASS line: `I<h>.<case> PASS v=<visible events> c=<axis constraint: 0 X, 1 Y, 2 major> b=<box scale x1000> #<check>`.
  - PASS also requires w = r = 360, e=0, x=0, m=0, masks paired and |b − 1000| ≤ 10.
  - FAIL line: the full numbers `r w v e x m c b`, the check digit, then the I-failure counters.
- **Box read on an existing session.** A second mode reads any older session folder, including sessions captured with delivered builds. It prints `B<h> n=<rows used> b=<scale x1000> #<check>`. This measures whether delivered data on that host was affected.
- **Python.**
  - No 3.11-only calls (`hashlib.file_digest`).
  - No packages beyond what the office kit already uses.
  - The minimum version is stated in `office_checkpoint_i.md`, and the self-test proves it.
- **Self-tests:** a PASS, a FAIL (the null-mask session), a box-read PASS (StackOBot) and a box-read FAIL (Lyra `f608d56`).

### K6. Camera switch (Code's observation A): measure it
- The leg: one per fixture, PIE and staged, with scripted view-target switches during active events. Include both a cut and a blend.
- **Gate:** no label/picture disagreement. That means edges 0, every labelled frame with a measured mask, and PF-2 agreement on the moving frames.
- **Reading, not a gate:**
  - frames dropped per switch;
  - whether each active event continued or ended, and its recorded reason.

  Report the numbers.

### K7. Requalify J2 on the final binary
- **What re-runs.** The fix changes the product, so the full 093-06 matrix re-runs on the new candidate (G201), plus K4 and K6. That means:
  - the 22 passed cases;
  - the failed one;
  - the 61 unrun.
- **Selected viewport:**
  - The original framing becomes PF-1.
  - The positive selected-viewport case is reframed so the target is truly on screen, judged by the engine's projection.
  - Reframing is legitimate now, because the failure is kept as its own test.
- **Order (as D3):**
  1. Every PIE gate first: PF-1 to PF-4, then the J2 matrix on both fixtures, then K6.
  2. The cook.
  3. Every staged gate.
  4. The push: fast-forward, `fix/followup-capture` only, when every gate is green.
- **Docs:**
  - a gotcha (next free number, re-checked at commit);
  - the architecture note;
  - the journal and the status block;
  - Code's observation C: replace the two literal `?` characters.
- The D2 stop rule applies unchanged. Continuations are allowed when the matrix doesn't fit one session.

### K8. An office read the owner can do now
- **The read:** on each office host (by RDP is fine), search the project's `Config` folder for `AspectRatioAxisConstraint`, then type back the value, or "none".
- It is a setting, not office data. It tells us early whether delivered data is likely affected. The K5 box read measures it properly later.
- **If either host reads MaintainYFOV:** chat works out which delivered fields were affected and brings the owner a product call: what to tell the client, and whether to ship the fix sooner.

## Next
- **093-07 is released:** Codex continues J2 (resuming its session is preferred), and Code reviews.
- **After J2 qualifies at home:**
  1. Office checkpoint I with the K5 reader: the second office host is required, the first is optional.
  2. `research/natural-causes` is created from the J2 head, and J3 starts (unchanged from P2: J3 may start once J2 passes at home).
