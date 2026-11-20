# REPORT for 094-38-europa-port-finish-take2.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-38-europa-port-finish-take2.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-01 22:21:48 | finished: 2026-11-01 23:52:19 | duration: 91 min | exit code: 0
context at last turn: 553642 tokens (55% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-38-europa-port-finish-take2.live.txt

---- FINAL MESSAGE (Claude Code) ----
**VERDICT: NEEDS-DECISION.** Codex fixed the LOD bug, but its Editor build used up FM1's three attempts. That blocked the launch, so the smoke never ran, and FV6, the pre-push scan and the office card were not done.

**For the owner:**
- The 4.25 port is not ready for the office capture test yet.
- The code fixes are in, and the Game target builds. The Editor target (which the test launches) failed its three allowed builds: two were stopped by low memory, and one hit a small compile error that is now fixed.
- One more Editor build should unblock the smoke test, if memory allows: free commit memory dropped to 0.5 GiB tonight under other work on the PC.

**The LOD fix and the sweep:**
- **LOD:** fixed. The auto-injector no longer forces the finest LOD. It now uses 5.1's default, the coarsest LOD (checked against `release/2026-10-05` (`d7eee97`), the port's real 5.1 base; `m51`'s `53bf725` is not that base).
  - Side effect: the same commit removed the 4.25 filter that skipped single-LOD meshes, and 5.1's `lod_corruption` has no such guard. An auto fire can now pick a single-LOD mesh, which is labelled but shows no change.
- **The sweep:** Codex listed 104 4.25-only lines and removed 5 differences, including missing_object's `-IAIBench` requirement. My own diff finds no 4.25-only line missing from its list. Three things fall outside the list:
  - **Pool change on both engines.** Codex added `lighting_mismatch` and `lod_corruption` to the shared pool, so 5.1 builds of this branch change too: 11 pool slots instead of 9, `IAI.Auto.Pool all` enables 11, and the two new slots have no key and can't be bound. Simply removing them would have broken the 4.25 capture, which enables its four anomalies through that pool.
  - **Compatibility changes on both engines,** such as the new tickable-subsystem base and raw pointers. They should behave the same on 5.1, but that wasn't tested.
  - **The 4.25 capture's own firing schedule:** 0.8 s interval, 0.4 s hold, one fire at a time. It is part of the earlier accepted capture design, but it is a 4.25-only choice and isn't in the list.

**The self-check fields:** all are now live reads:
- the viewport type, client class and size;
- the captured frame format and the real backbuffer format;
- the RHI and hook count;
- the census lost / no-pass / polluted counts;
- the fixed-step, fixed-frame-rate and smoothing state.

Two caveats:
- The `tick=` hook name and tick type can never vary: there is only one caller, and it only passes `LEVELTICK_All`.
- `pre_ui` is a real pixel test (a marker drawn on the canvas, found in the presented image, absent from captures). But it doesn't record which captured frames had the marker drawn, so the overlap rests on timing.

**The far plane:** now 1,000,000 in all cases, exactly as 5.1 writes it.

**The smoke:** not run. No map was launched, so every gate is unreached, and there are no masks, event counts or LOD pictures.

**FV6:** not run, and no history was rewritten. Only `09434-live-capture-receipts.json` holds a real drive-root listing, and the same unchanged file sits in 22 commits from `47a6972` to the tip. A rewrite would touch all 22.

**Scan and card:** not written, because both depend on a passing smoke.

**Safety:** none.
- No session ran, so there was no crash and no upload.
- The Paris host's Content, Config, descriptor and `.uproject` are hash-equal by my own hashes.
- Binaries changed only while our builds were running.
- `m51` equals P9 at start, launch and exit.
- The feature branch is clean at `8bdcb4b`, with 8 new commits, not pushed and no tags.
- The only files deleted were 4 build-tool outputs; one partial build file was moved aside with a receipt.

**Decisions for chat:**
1. A new Editor build budget, then the smoke and FV6.
2. The pool change: keep `lighting_mismatch` and `lod_corruption` on 5.1 too (and give the two new slots keys), or make them 4.25-only again.
3. Single-LOD meshes for `lod_corruption`: add a two-LOD guard like `lod_popping`'s, or restore the 4.25 filter.
4. Whether the 4.25 capture's firing schedule needs listing or a ruling in the sweep.

Files are in `D:\IntrusiveAnomalies\_reviews`:
- `094-38-code-review.md`
- `094-38-codex-report.md`
api-equivalent cost (not billed on Max): $14.00 | turns: 101 | running total across all briefs: $5,458.16
