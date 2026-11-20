# REPORT for 087-01-office-check-kit.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 087-01-office-check-kit.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 13:05:49 | finished: 2026-09-29 17:08:33 | duration: 243 min | exit code: 0
context at last turn: 930323 tokens (93% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\087-01-office-check-kit.live.txt

---- FINAL MESSAGE (Claude Code) ----
The work is done: the kit, card, proofs, AnomDash fix and docs are all committed and pushed. Two things need chat's decision; the first is due before tonight's run.

VERDICT: NEEDS-DECISION

## Summary for the owner
The office check is ready. You capture one session with a still camera, run two commands, and read back one short line per anomaly. It only prints numbers, and a result of 0 means the labels are in sync. It has been checked against every banked capture that has a known answer, and it failed every deliberately broken label except one case the design cannot see. The dashboard bundle also no longer ships this PC's token.

## 1. Selftest (`label_sync_check.py --selftest`)
- **Result:** 32 checks on 28 synthetic sessions pass. With Pillow installed it runs both decoders and their numbers match. It also passes 32/32 with Pillow hidden (`python -S`). It takes about 50 s.
- **Decoder:** the standard-library decoder reproduces every written pixel across all five PNG row filters, for RGB, RGBA, greyscale and partial rows.
- **Known answers (all correct):**
  - exact labels PASS 0/0;
  - onset late or early by 1 and offset late or early by 1 all FAIL;
  - an effect 3 frames late FAILs at +3;
  - an 8 % reappear ghost reads 0 at 50 % and 10 %, and +1 strict;
  - slow drift after an event is censored; a moving camera is not judged; a missing mask is counted;
  - a gap beside an edge counts as a gap, and still FAILs when the label is also late;
  - a wrong object is counted.
- **The two cases you asked for:** a flagged anti-aliasing smear PASSes transition-aware with raw end +1, and a label late by 1 with a stray flag covering it FAILs at 50 %.
- **084-07 cases:** stuck_low_mip partial frames above the noise band pass; a partial frame inside the band passes only if the render record backs it and fails without it; an off frame backed only by `unresolved` fails.
- **Other checks:** `camera_clipping` is not judged, and the report contains no paths or names. If any doctored case reads PASS, the selftest refuses and exits 1.

## 2. Proofs on real banked captures
- **Master-build stuck_low_mip must fail:** it does, in the same direction and size as 084-06's re-evaluation (rebank) of the 084-03 bank.
  - B9_BASE (AA off): FAIL 21, start −1 ×21, end +4.
  - B0_BASE (TSR): FAIL 21, start +4 ×21, end +5 to +9.
  - B3_BASE and B7_BASE: FAIL, same shape.
  - The kit censors more ends than 084-06 did. It has no matched empty (null) capture to divide out MainWorld's cycling scene (G396). Every edge it does judge agrees.
- **Fixed builds must read 0 at 50 % transition-aware:**
  - B9_FIX (AA off): 0 FAIL, every judged edge 0.
  - B0/B7/B3_FIX (TSR, no flags in the data): with the build's 3/8 flags synthesised, 0 FAIL.
  - `E9FF019A` legs with real flags: B0 (CENSORED 12, PASS 4, raw start +1/+2 becomes 0), B3, B9 and the B0L lever match 084-06's own readings exactly.
  - B9 reads FAIL 16 at start +2. Those are the partial-application frames that build did not flag yet, so this is the correct reading for it.
  - I gave a copy of B9 the 084-07 schema and synthesised `partial` flags from its own per-texture render record. It reads rule "new", 0 FAIL, start raw +2 becoming 0, with 32 partial frames confirmed.
- **The four proven types against 086-01** (159 sessions, 498 events):
  - At 50 %, every edge both tools judge is equal: 871/871 starts, 744/744 ends, 0 different. The per-type histograms are identical.
  - At 10 %, 4 ends differ. These are 086-01's 12–16 % blinking reappear residuals; the kit measures one at 6 % against its settled post-event reference.
  - This comparison runs with R1 off. R1 is 084-09's reference-amendment rule, which the kit ships with on. With R1 on, 418 of these events are *confounded reference* (the bank spaces events only 4 frames apart), and the remaining 80 agree exactly. The office recipe's 30-frame spacing does not trigger R1.
- **084-06's REAL legs:** R1 legs PASS 10/10 at 0/0; R2 legs PASS with 2–4 ends censored for scene drift; MASK55 has 32 judged events, all at 0/0.
- **Doctored labels (shifted −1, +1, +3):** every judged event FAILs on all REAL legs and both stuck_low_mip legs. The one exception is stuck_low_mip +1 under TAA, which reads PASS/CENSORED. That is the known onset blind spot from G353: under temporal anti-aliasing, a label up to 3 frames early is indistinguishable in pixels. The same shift FAILs 16/16 with AA off.

## 3. Runtime
Measured on the 56-event set: 4 MainWorld sessions, 1,200 frames.

| decoder | time |
|---|---|
| Pillow | 180 s |
| standard library only | 857 s |

The two reports are identical line for line apart from the decoder name and the seconds. For the office recipe (3,000 frames) the card says about 5–10 minutes with Pillow and 30–45 without.

## 4. What the kit can't judge
- `camera_clipping`: it has no target mask, and its label is a whole-frame proxy.
- Moving-camera events, which is why the recipe is hands off.
- Events that start in the first 30 frames, which is why the recipe adds a 40-frame lead-in.
- Effects too faint to measure, and targets whose change is swamped by the scene.
- stuck_low_mip ends on a cycling scene: without a null capture these are censored, never passed.
- Wrong-object: it prints the count tied to the label beside an upper bound, and shadows and reflections can trip the bound.

## 5. Commits pushed
- **Plugin, `feat/office-check-kit` (off `fix/m52-label-timing` `02d15a4`, no tag, no merge):**
  - `bd8c780` the kit;
  - `d964e53`, `281b505`, `2ecf3c8`, `0cd2bb0` fixes found by the proofs;
  - `ed5eba2` read-back wording;
  - `94dd112` docs: card, readme Step 7, journal, status block, G395–G396, and a fixed-in note on G376.
- **AnomDash `618b8df..ab6e63f`:**
  - the bundler always removes the dev `config.json` and writes the token from the delivered build's log;
  - it refuses placeholder tokens;
  - it exits 4 and says NOT COMPLETE when the token or plugin repo is missing.
- **AnomDash proof with `check_delivery_bundle.py`:** the old bundle STOPs on its token, and the new one PASSes. There is no real log of a build with a full-length token on this PC, so the passing case used a synthetic log in the engine's exact format; the delivery rehearsal should repeat it with the real build's log.
- `m51`, `master` and every bank are untouched, and the temp copies are deleted.

## 6. NEEDS-DECISION
1. **Tonight's `084-09-eval.py` shares a flaw I found in 084-06's per-frame method (G395).** A reference can contain the very frame it is judging. On known answers 084-09 still FAILs, but it prints start +4 for a label 1 frame late, and end −3 for a label ending 1 frame early. A reference that ends on a later change can also inflate its threshold; that produced a false FAIL on a banked event in an early version of the kit. Before 22:30, chat should decide whether to patch 084-09 or to run this kit beside it as a third reading.
2. **Merge ordering.** AnomDash's manifest already ships `tools/label_sync_check.py` and `OFFICE-CHECK.md`, so building the bundle against a plugin ref without this branch fails loudly until it is merged with the fix. Separately, AnomDash's `dist/` is older than `src/` on this PC; the delivery bundle needs a fresh `npm run build` first.
api-equivalent cost (not billed on Max): $43.78 | turns: 279 | running total across all briefs: $3,656.20
