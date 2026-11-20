# REPORT for 069-18-m44-merge-ruling-and-ship-docs.md
brief: 069-18-m44-merge-ruling-and-ship-docs.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: medium
started: 2026-09-03 13:42:48 | finished: 2026-09-03 13:56:16 | duration: 13 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-18-m44-merge-ruling-and-ship-docs.live.txt

---- FINAL MESSAGE (Claude Code) ----
```
SESSION 069 / BRIEF 18 — m44 IS MERGED AND SHIPPED ON master.

MERGE SHAPE: a MERGE COMMIT, not a fast-forward. master (62bd287) and the branch had diverged at
42061dc — master took the "G1 FAILED" docs commit while the branch carried on. Three doc files
conflicted (gotchas.md, the predictions file, journal 069); all three were pure appends on both sides
and were resolved as ours + the branch's added tail.

FINAL STATE
  master  d48e1ba == origin/master   (pushed)
            d48e1ba  docs(m44): … gates both orders, G223-G227
            c5b3d27  merge: m44 target mask onset fix — F1 deliberately NOT included
            fafb81f  fix(capture): m44 tag ownership — G1 is 4/4
  F1      m44-f1-resolution-mapping-UNVALIDATED  2d66b90 == origin  (pushed, unmerged)
  deleted m44-GATE-G1-FAILED-do-not-merge, local and origin — its name was accurate to the end and
          it was never merged under it
  NO TAG. Office batch: … m41 → m43 → m44.

F1 IS NOT IN MASTER — verified two ways as asked:
  git diff 62bd287 master -- AnomalyVisibleMask.usf AnomalyVisibleMaskShader.h  → EMPTY (byte-identical)
  git merge-base --is-ancestor 2d66b90 master                                    → exit 1 (not contained)

BUILDS: game exit 0, editor exit 0 on merged master.
BINARIES: staged stays 635A615A — the binary every m44 gate ran on, archived
  .m44-tagownership-635A615A. master's own post-merge build is 06657B35 and is deliberately NOT
  staged (G201: identical Source, different hash across two links). 0EF535DC remains load-bearing as
  m44's A-side.

=== ONE THING I DID BEYOND THE BRIEF, AND WHY ===
The IAI.Capture.TargetMask help string and the StartRun echo both still read "A BLANK png means
MEASURED AND NOTHING VISIBLE". After m44 no blank PNG is ever written, so both lines asserted
something false. I corrected them in code as well as in the README — the run's own echo is the
client-facing contract (G139), and fixing only the README would have left the build telling the
client the opposite of the truth. Both now state the present|empty|unmeasured contract and that a
file exists iff it has content.

=== DOCS LANDED ===
 CLAUDE.md — status block refreshed (standing convention); invariants: (i) tag ownership — an actor
   under a live fire belongs to its event, every consumer that tags ASSERTS and never tests
   "already tagged"; (ii) records exist at OnWorldTickEnd of a fire's first frame, never born in a
   drain; (iii) any pass reading scene textures maps through the internal view rect — marked PENDING
   F1 with the branch name and the "correct only at 100% screen percentage" consequence; (iv)
   STANDING RULE: per-frame alignment gates run in BOTH tick orders; (v) two permanent gates ONSET
   and MASK-PICTURE-PAIRING with their instruments, added to the milestone template; (vi) the
   stencil-pool split recorded as REFUSED with its numbers — 55 assignable values (200..254) against
   77 census candidates in one 90-frame leg — so nobody re-proposes it.
 client-readme.md — mask_state table (present | empty | unmeasured), no file unless content,
   mask_map lists only existing files, the kept counter name explained
   (target_mask_frames_hidden_blank counts "empty"; _unavailable counts "unmeasured"; the three sum
   to the frame count), the temporal-AA sentence (6–8% vs ~0.5% baseline), and the hidden-class
   limitation in exactly the words specified. The old "a blank PNG means hidden" text is gone.
 client-delivery.md — same contract; the quoted echo updated to the shipped text.
 PRE-DELIVERY-CHECKLIST §1.1 — ⚠ "one PNG per captured frame" was the old completeness test and is
   now WRONG; reconciliation is against run_summary. New boxes: ONSET both orders, zero blank PNGs +
   G7, MASK-PICTURE-PAIRING (100% leg, with the 50% leg noted as joining when F1 lands).
   MaskPairingProbe added to the bench-lever grep.
 Card SECTION F — F-0 pull + editor rebuild; F-1 one 90-frame run with blinking ticked; F-2 six
   reads. (a)–(d) have stated expected values and all use the existing m44_gates.py — no new tool
   invented. (e) and (f) are readings with NO expected value: internalViewRect vs viewRect (this is
   what decides whether F1 must land before the client cook) and the fog card.
 Ledger §11 — the four Bates observations → O1 fixed, O2 → m45, O3 fixed, O4 not reproduced and
   shipped as documentation; the fifth defect nobody reported (masks on unlabelled frames); the
   ownership table with all four measured values; the pool-split refusal numbers; the tagOvertaken
   PASS-WITH-READING citing the m43 gate-D precedent; the three refuted hypotheses one line each.
 Journal 069 §12. _binary_baselines README.

G-NUMBERS, unique and in order on master:
  G223 a record born in the drain is one cycle late
  G224 a tie gate and a count gate are both satisfied by a uniformly late artifact
  G225 a post-upscale pass sampling pre-upscale textures with unscaled coordinates (REAL, demonstrated
       at 50% screen percentage, fix pending F1)
  G226 a bench fixture sharing a namespace with the system under test
  G227 two consumers of one shared attribute, only one asserting ownership
  (G222 precedes them; no gap, no reuse.)

NEEDS-DECISION: none.

Carried, not decisions: the cook remains authorised for a later brief after m45's measurement legs
(G118), and F1 is unvalidated until then — so masks, the census and m26 are correct only at 100%
screen percentage, which is exactly what card read F-2(e) is for.
```
api-equivalent cost (not billed on Max): $17.84 | turns: 31 | running total across all briefs: $418.27
