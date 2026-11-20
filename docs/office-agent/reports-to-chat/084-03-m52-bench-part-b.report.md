# REPORT for 084-03-m52-bench-part-b.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-03-m52-bench-part-b.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-28 21:49:47 | finished: 2026-09-28 23:07:27 | duration: 78 min | exit code: 0
context at last turn: 462976 tokens (46% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-03-m52-bench-part-b.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

The window completed, all 13 legs are banked, the m53 set is restored and verified, and postflight passed. But the final 084-04a evaluator could not judge a single sync event on either build (NO-JUDGEABLE-EVENT everywhere), so the sync answers below come from two declared variant measurements. Four items need a ruling.

**2. Plain summary for the owner**
- With anti-aliasing off, the fix lines the labels up with the blur exactly: 0 frames off at both ends, on all 16 events. The old build is 1 frame late at the start and 4 frames short at the end on every event.
- With anti-aliasing as delivered, the picture blurs about 2 frames after the label starts, and stays blurred 1–7 frames after the label ends. That is the anti-aliasing smearing the frames around a label that is otherwise exact.
- The wrong-object check held: every normal fix leg passed, and the deliberately broken version failed as it should.

**3. Gate table (final evaluator 084-04a; baseline vs fix)**

| leg | (i) sync | (ii) window | (iii) purity | 086-01 checker, unmodified (50 %) |
|---|---|---|---|---|
| B0_BASE / B0_FIX | NO-JUDGEABLE / NO-JUDGEABLE | – / PASS (18 events) | – / PASS | FAIL (start +4 ×14) / NO-REFERENCE 16/16 |
| B3_BASE / B3_FIX | NO-JUDGEABLE / NO-JUDGEABLE | – / FAIL (1 frame) | – / PASS | FAIL (−1 ×8) / NO-REF 2, FAIL 3, CONFOUNDED 3 |
| B9_BASE / B9_FIX (AA off) | NO-JUDGEABLE / NO-JUDGEABLE | – / FAIL (1 frame) | – / PASS | FAIL (−1 ×17) / NO-REFERENCE 16/16 |
| B7_BASE / B7_FIX (synthetic tick order, optional) | NO-JUDGEABLE / NO-JUDGEABLE | – / FAIL (3 frames) | – / PASS | FAIL (−1 ×14) / NO-REFERENCE 16/16 |
| B0L_FIX (timing can-fail) | NO-JUDGEABLE (can-fail not proven here) | n/a | PASS | **FAIL, −1 ×17: can-fail proven** |
| P0_FIX / P1_FIX (purity can-fail) | | | PASS-REFUSED / **FAIL as required** | |
| B5_BASE / B5_FIX (nulls) | PASS / PASS | | | |

- **Why (i) could not judge:** the threshold includes the null's worst drop over the whole leg. Tonight that read 27–29 % of the pre-event level, against 2.7 % on the 90-frame null the harness was proven on. The rock's own sharpness rises and falls on about a 12 s cycle, which matches the moving platform's period (an association only). That cycle made every event "not measurable" (G349).
- **Gate (ii):** the label window equals the render-record window on all 62 events checked. The failures are 5 isolated interior frames with an unmeasured mask (`target_pixels` −1) out of about 680 labelled frames; no log line gives a cause.
- **Where the two checkers disagree:** the 086-01 checker picks its reference frames where `anomaly_present` is false. On the fix build that flag is true on 986 of 1,027 unlabelled rows (baseline: 345 of 1,046), because a closed event stays attached until the next fire (084-04 F2). So the checker has no reference and returns NO-REFERENCE on every fix event (G350).
- **Declared 086-01 variant:** same checker, with reference frames chosen by annotation membership. On the fix build it gives B9_FIX onset +0 on 16/16 and offset +0 on the 6 events that were not censored by the scene-drift rule. With anti-aliasing on, B0_FIX and B3_FIX start at +2. On baseline legs this variant misbehaves, so baseline numbers come from the unmodified checker and from the §4 variant.
- **The si-497-style early blur** appears in tonight's nulls at the same rate as in the hold legs (3 of 19 and 3 of 17 null events; 1–3 per leg elsewhere). It comes from the scene, not the hold. No correctly labelled second blur run occurred (zero reopens).

**4. Residual**

This is the harness's own `measure_event` with the null-drop term removed, so the threshold is max(5σ, 2 % of pre). Start = first visible frame − first labelled frame; end = last visible − last labelled. On the fix, label equals record on every event, so the offsets against the record are the same.

| leg | anti-aliasing | events | start at 50 % | end at 50 % |
|---|---|---|---|---|
| B9_BASE | off | 21 | −1 ×21 | +4 ×21 |
| **B9_FIX** | **off** | **16** | **+0 ×16** | **+0 ×16** |
| B0_BASE | delivered | 21 | +4 ×21 | +3 to +9 (plus +16, +22) |
| B0_FIX | delivered | 16 | +2 ×16 | +1 ×3, +3 ×2, +4 ×3, +5 ×2, +6 ×2, +16 ×1 (3 unconfirmed) |
| B3_FIX | delivered | 8 | +2 ×8 | +3 to +5 (plus +9, +15) |
| B7_FIX | delivered, synthetic order | 16 | +0 ×3, +1 ×8, +2 ×5 | +1 to +7 (plus +15, +27) |

**Proposed closure rule:**
- **Anti-aliasing off:** none is needed; it is 0/0 today.
- **Anti-aliasing as delivered, at the 50 % rule:** the start is one constant (defer the first labelled frame by 2). The end is not one constant (+1 to +7), so no fixed settle tail gives 0. It needs either an in-engine settling detector or a documented anti-aliasing residual. `IAI.StuckMip.SettleTailFrames` stays 0.

**5. Edge-path frequencies (fix legs)**
- **Never happened:** trail reopens (including in grace and after detach), ORDER HOLD, gap-unknowns, the 64-tick fallback, late receipts after the force, and unrecoverable reopen frames.
- **Carried trails:** 2, on B0_FIX and B9_FIX. Each is the event cut by the 1,200-frame cap, which also accounts for the 2 "restore unresolved at run end".
- **Contaminations:** 4, all on the P1 purity lever and none on an ordinary leg. They are detected before the event's first member frame, so rule (d) lists them as not handled as ruled.
- **Hold timeouts:** 22, from the null's no-hold lever (18) and P1 (4). Restore timeouts: 0.
- **Legacy levers behaved as expected:** B0L echoes the legacy game-thread label source and fails in the 086-01 checker; P1 finds 19 of 19 textures shared.

**6. Monitor cost (from each fix leg's run summary)**
- **Hold monitor:** mean 0.18–0.21 ms, p95 0.25–0.33 ms, max 0.42–0.63 ms per held tick on ordinary legs (230–576 scans), walking 1,180 components. P1: 10 scans, p95 and max 1.33 ms.
- **Streamer fence:** 4–22 fences per leg, at most 0.26 ms, 0 incomplete.
- `stuck_mip_reopens_after_detach` = 0 and `stuck_mip_late_receipt_after_force` = 0 on every leg. Cost on a heavy host is still unmeasured.

**7. Staging and restore**
- **Runs:** pass 1 `live` (11 required legs, 21:51–22:15), then pass 2 `live --with-optional` (22:16–22:35). Pass 2 resumed and ran only B7_BASE and B7_FIX. Pass 1's receipts are kept in `run\pass1\`.
- **Attempts:** 13 legs, 13 attempts, no retries. Focus was held for every whole leg, and the quiet gate never refused.
- **Person gate:** input was seen before 5 legs, with the longest wait 214.6 s before B7_FIX (22:17–22:21 IST). Every wait cleared at 60 s idle, so no leg ran with a person present.
- **Load-affected legs** (speed ratio above 1.05): B3_FIX 1.069 (frame-time p95 50.7 ms), B7_FIX 1.053, P1_FIX 1.125 (one 1.24 s hitch). No conclusion rests on these alone. Every other leg ran at 1.00–1.03, p95 34.6–41.5 ms (P0_FIX, a 300-frame leg, read 98.9 ms).
- **Staging:** baseline `E0BE6F0A` and fix `B725678B` were each staged with container `67EA1FE0`; every receipt verified.
- **Restore:** m53 restored in both passes and verified; the six files re-read by hash as `2FCDF059` / `20DA6F98` / `534C5863` / `FD766B7B` / `462B8AC6` / `BB05CF99`.
- **Postflight PASS:** main checkout still `m51` `53bf725`, fix branch equal to origin, 15,268 alias hardlinks, no exe-side copies left, no process of ours running. `UnrealTraceServer` was running and foreign; I left it alone.
- **Harness edit:** the only change was `FIX_ARCHIVE_NAME='m52fix-FD621B27'` → `'m52fix-B725678B'`. `084-03-common.py` also had no postflight step, so I wrote one as a new file (`_reviews\084-03-evidence\postflight.py`) alongside the analysis scripts; no other harness file was touched.

**8. Commits pushed**
- `44584e7` on `fix/m52-label-timing` (`c71f826..44584e7`, equal to origin): journal `2026-09-28-084-03-code-m52-bench-part-b.md`, the status block, and G349–G350.
- No tag, no merge. `master` is still `b5f15a3`.

**9. NEEDS-DECISION**
1. **Harness threshold:** the null-calibrated threshold can't judge 1,200-frame legs in this scene (G349). Should verdicts be re-derived with a threshold computed only around each edge, or with a detrended null?
2. **`anomaly_present`** is now true on about 96 % of unlabelled rows on the fix build, and `labels.jsonl` ships to clients (G350). Keep it as it is and document it, or clear it once an event closes?
3. **Anti-aliasing residual:** a +2 start deferral is available as a constant. The end (+1 to +7) needs a settling detector or a documented limitation.
4. **Gate (ii):** 5 isolated unmeasured-mask frames inside labelled windows, cause not established. Do they block, or are they reported and investigated?

Some MCP connectors (GitHub, Slack and others) need authorising via `claude mcp` or the claude.ai connector settings, and the PDF viewer failed to connect. Nothing here used them.
api-equivalent cost (not billed on Max): $10.68 | turns: 83 | running total across all briefs: $3,156.88
