# 094-47w / 094-46s — Chat ruling (owner-relayed, 2026-11-06 ~09:30 IST): classify the exact-comparison differences against a console-vs-console baseline using captures that already exist; drop the D3D12 full pair; record camera_clipping's odd-size side effect; then push the plugin and the dashboard and hand over the packaged build. Priority: finish today

**Author:** chat Claude (orchestrator). This is the chat review that Codex's takeover record requires for "changes to correctness tolerances or plugin release authority". It binds Codex (the orchestrator during the takeover) and the worker.

## GR1. The exact D3D11 comparison is not "identical or fail". Classify every difference against a measured baseline, with no new capture
- **What was reported:** 1,893 differences:
  - 1,639 census-cycle;
  - 172 in-flight-batch;
  - 76 private-pass verified-count;
  - 6 label-schema entries.
- The two captures each pass every image and label gate on their own. The strict texture checks reach IoU 1.000 with 0 spill.
- **The baseline:** run the **same comparator on two console-started captures that already exist**, 094-47v's reference console and 094-47w's console D3D11 (the same package and settings). This is console A against console B. No new launch.
- **Rule:**
  1. **A difference class that also appears in console A vs console B, at a similar magnitude,** is session nondeterminism (async census or readback timing, batch scheduling, per-frame verification bookkeeping). It is **recorded and excluded** from equivalence, with both magnitudes reported. It is not hidden and not given a new tolerance.
  2. **The 6 label-schema entries:** explain each one.
     - If an entry is a path, session ID or timestamp on the plan's normalization list, it's normalized.
     - If it is a real key or type difference in the label schema between the console and dashboard runs, that is a **real bug**: fix the C5 bridge and rerun only the dashboard D3D11 capture.
  3. **Always required, compared directly between the dashboard and console runs:**
     - the seed, routes and route order;
     - the fire schedule (event types, order, session-relative frames);
     - per-frame label content and mask pixels (equal, or within the measured session floor);
     - requested, written and paired counts, receipt PASS and restoration.

     Any difference here is a real bug.
- **If (1) and (2) clear and (3) holds,** D3D11 equivalence **passes**.

## GR2. The D3D12 full console/dashboard pair is dropped
- The transport and C5 bridge are RHI-independent: the dashboard only starts the same C5 path.
- D3D12 is already covered:
  - the D3D12 lifecycle cases passed on the current package and dashboard (discovery, auth, catalog, start, stop, reconnect, second capture, restoration);
  - D3D12 console C5 captures passed in 094-45r and 094-46.
- *Why:* this saves roughly two full captures plus review, and buys nothing the existing evidence doesn't already show.

## GR3. camera_clipping at 1279×719 (094-46s): option (d), a recorded size-dependent side effect
- **What was measured:** during a held near-100 session at 1279×719, the floor's farthest visible rows (373–375) speckle by up to 117 levels on EA_Floor texture frames, in the far section where nothing is clipped.
  - The plain floor stays within 2 levels, and neither 1280×720 leg shows it.
  - This fits depth-precision loss on far geometry once the near plane moves. That's a property of how camera_clipping renders, as in 5.1, not a labelling error.
- **Ruling:**
  - camera_clipping is **qualified at 1280×720 on D3D11 and D3D12** (D4 under GP2), and stays **opt-in only**;
  - the card records the known side effect: during held camera_clipping sessions, distant geometry can show depth-precision speckle at some resolutions.
  - The positive's 1-pixel strict-G3 miss at (375,239) is the same artifact, so it's accepted **only inside held camera_clipping sessions**. Default-pool captures keep strict G3 at IoU 1.000.
- **Compression:** compressing archived EXE/DLL/PDB copies (hashes and mtimes unchanged) is accepted after the fact. From now on, compress only on receipts and evidence, not binaries.
- **GO3's default-only key diff:** information-only results already show no key removed and no type changed. It runs once at the push gate (GR4), on the final packaged build.

## GR4. Push and handover today
Once GR1 passes:
1. **One final protection check:** FJ1 plus a single full P9 `main()` at exit. No more per-unit 100 GB rehashes before this point.
2. **The pre-push scan** of `c5d0919..<plugin tip>`.
3. **Push `feat/europa-port`** under the 094-39 rules (fast-forward, no tags or force, confirmed with `ls-remote`).
4. **Push the dashboard `feat/europa-425`** (two files) under the same rules. The dashboard's `master` is untouched.
5. **The handover card** (update `_reviews\094-47-office-card-europa-dashboard.md`):
   - the pushed plugin commit;
   - how to package Development (the office repeats BuildCookRun with the fork);
   - the token, starting the listener, and connecting AnomDash (the desktop exe, or a build from the pushed branch);
   - the preset (Random mix, the four defaults, PNG, native, seed 4242, 2352);
   - camera_clipping is opt-in, with GR3's note;
   - numbers-only reads.

   Put a copy in the claude.ai project too.
6. **If GR1 finds a real bug,** fix only that, rerun only the dashboard D3D11 capture, and then push.

## GR5. Pace
- The owner's priority is to finish today.
- **Skip:** extra independent re-hashes of evidence that's already sealed, and new auxiliary measurements.
- **Keep:** the gates above, and never loosen a correctness gate.
- **Report:** a short final report to `_reviews\094-47-final-report.md`, with a plain owner summary at the top.
