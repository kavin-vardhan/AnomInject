# B28 - PRE-DECLARED RULE for the si=8 synth-order excursion

Written BEFORE the two deciding legs ran, and left unedited afterwards.

## Observation (already measured, both banked)

    B22_A4_PAIR_SYN         (m46 60AE8C61, blinking/4242/40, synth)  si=8  picCx 322.0  picN 8351  delta 33.6  CURRENT
    B28_B4_m48_blink_synth  (m48 DE65F84A, same recipe,      synth)  si=8  picCx 314.9  picN 8653  delta 40.7  NEITHER

`maskCx` 355.6 and `maskN` 10333 are IDENTICAL on that frame in both legs, so the
excursion is PICTURE-side. The tolerance is 40 px: this is a 0.7 px threshold crossing.

## Deciding legs (2), same recipe, binary swap only

    L1  m48 DE65F84A, synth, second attempt   -> is the m48 reading repeatable?
    L2  m46 60AE8C61, synth, fresh attempt    -> is the m46 reading repeatable?

## Rule, fixed before either of them ran

- **(a)** If si=8's delta STRADDLES 40 across the two m48 legs -> RUN-TO-RUN VARIANCE at a
  marginal frame. Report as such. NOT attributed to the binary.
- **(b)** If BOTH m48 legs read > 40 AND BOTH m46 legs read < 40 -> the shift is
  BINARY-ATTRIBUTABLE. Report it as a measured, small, PICTURE-side shift. It is still NOT
  a pairing fault: PREVIOUS == 0 on every decidable frame of every leg, and PREVIOUS is the
  half that detects mis-pairing. NO MECHANISM IS ASSERTED (G120).
- **(c)** If the m46 legs also cross 40 -> the excursion is fixture-inherent and pre-dates
  m47/m48.

Every leg is banked and reported, including any that disagree. No leg is discarded for what
it showed (A63: a leg is discarded for HOW it ran, never for WHAT it showed). **The
tolerance is NOT widened under any branch** - that would be laundering.

## Result

    leg                          si8_picCx  si8_picN  si8_delta  totals
    B22_A4_PAIR_SYN   (m46)      322.0      8351      33.6       35/35  N0 P0
    B28_L2_m46_blink_synth (m46) 316.7      8545      38.9       33/33  N0 P0
    B28_B4_m48_blink_synth (m48) 314.9      8653      40.7       32/33  N1 P0
    B28_L1_m48_blink_synth2(m48) 317.4      8543      38.2       34/34  N0 P0

**BRANCH (a).** The m48 readings straddle 40 (38.2 and 40.7) and the m46 band reaches 38.9,
so the two binaries' bands OVERLAP. Run-to-run variance at a marginal frame; not
attributable to the binary. PREVIOUS == 0 on all four legs.

si=8 is the same frame in every leg and is always the most extreme frame of its leg, so the
excursion is deterministic in POSITION and variable in MAGNITUDE. NO MECHANISM IS ASSERTED.

**Named consequence, filed not fixed:** the synth-order clause of MASK-PICTURE-PAIRING sits
about 2 px from its tolerance at si=8 on this fixture and can read NEITHER 1 by ordinary
run-to-run variance on EITHER binary. That is a property of the gate, measured here for the
first time, and it is not what brief 28 was chasing (NEITHER 54). It is reported so that a
future NEITHER 1 on this frame is recognised rather than re-investigated.
