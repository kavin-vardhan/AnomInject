# 089-04b — Chat ruling on the m53 re-bench finish (NEEDS-DECISION): G-MODE PASS 4/4 including normal, accepted; RD5 proven on pixels (+3 on 8/8); the m53 bench evidence is closed for this delivery

Date: 2026-09-30 ~09:15 IST · Author: chat Claude · m53 `620e3cb` (lock `a7f542d`)

## Accepted
- **The full 089-04 table:**
  - 47/47 accepted;
  - B-M53 fixture PASS in 4 modes (17/17, +0/+0; the kit agrees);
  - MIXED_N / MIXED_UV, G-BIND, G-LEVER-OFF and G-CENSUS PASS;
  - arms CD3 / CD5 / RD3 / NOAPPLY proven;
  - label-rule PASS, **0 co-entry frames across 30 legs**, GT-NULL wrong-object PASS;
  - COST below the instrument's resolution.
- **The two G-MODE family mix-ups:**
  - the gate used the overall P-ORC;
  - the lock pooled uv and normal recipe frames.

  The fix is in a new module; the frozen files are untouched, the as-built report is kept, and the unit tests pass 12/12 both ways. **Chat's expectation ("normal INSUFFICIENT") was wrong:** on its own recipe the normal oracle reads interior 0 and pattern 0.
- **The MainWorld hosts are confirmed Nanite** (12/12 fires `reason=nanite`), and the labels follow the unmeasurable contract exactly.

## Rulings
1. **G-MODE: PASS 4/4 accepted, including normal.**
   - tile and scramble: match 1.0 on 9 events, and scramble's logged pair was recomputed 9/9;
   - invert and green_flip: 1,152/1,152 cells;
   - every wrong-mode negative is below 0.5.

   **The readme's normal-family evidence cites this bench reading (fixture).** The "not measured on realistic content" line stays.
2. **RD5: proven on the pixel reading** (the picture outlasts the label by exactly +3 on 8/8; the leg FAILs as required). **G419 is recorded:** when a late picture fills half of E1's offset window, E1 names the wrong edge (it reads an under-label as an over-label). The next harness round fixes E1's offset window.

## m53 status for the delivery (final bench word)
- **Label sync: PROVEN on the fixture, all 4 modes, in two independent runs.**
- **Mode correctness: PROVEN.**
- **m52 ⟂ m53: 0 co-entry frames everywhere.**
- **Refusals** (MIXED, the census): correct.
- **Real content:** unjudgeable from pixels (the framable hosts are Nanite); no wrong-object change seen. The owner's Thursday office look is the real-content check.

## Next
- **090-01 (xhigh): the final source round on the fix branch:**
  - the `TransitionOffFrames` default 16 for temporal AA;
  - the LOD log wording;
  - readme finalisation;
  - merge the fix branch into m53;
  - build both branches, run the suites and the lever audit, archive.
- **090-02:** the Codex evidence review through the Relay (merge prep).
