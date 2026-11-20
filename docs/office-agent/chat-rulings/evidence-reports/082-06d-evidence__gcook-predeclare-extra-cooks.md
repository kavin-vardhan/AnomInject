# 082-06d G-COOK - pre-declaration for the additional identical-input cooks

Written 2026-09-27 before cooks d4..d13 ran. Nothing below is changed after they run.

## What was read on d1..d3 (3 identical-input cooks)
- CB_GateLevel exact, 101/101 fixture + 3/3 plugin packages present, MainWorld .uheader byte-identical, .uexp sizes equal.
- MainWorld word test (candidate d1 vs archive A, region R = union of pairwise diffs of d1, d2, d3), at every one of the
  four word alignments, as pre-declared in gcook_082_06d.py: outside R = 4 / 20 / 25 / 0 words. FAIL as pre-declared.
- Byte level: 49 bytes of diff(A, d1) lie outside the byte region. The same 4 / 20 / 25 / 0 appear for A-d2 and A-d3,
  i.e. they are bytes where d1 = d2 = d3 and A differs.
- Hex read (mw_hex_probe.py): the varying data are 16-byte (X, Y, Z, V) records with V a random value in [0, 1]; every
  differing byte lies inside a V field; the 49 bytes are all the MOST-SIGNIFICANT byte of V (0x3d / 0x3e / 0x3f),
  where the three new cooks landed in the same binade and A did not. V fields start at different residues mod 4 in
  different arrays, so a fixed-alignment 4-byte word straddles a V field and constant bytes.

## Why more cooks (ruling 082-06d section 2: "If the variance region needs another identical-input cook of the new
## inputs to be well defined, run it")
With three cooks the region under-samples V's top byte (P(three cooks share a binade) ~ 1/4 to 1/8 per record), so the
region is not well defined at byte or misaligned-word granularity. A byte that differs SYSTEMATICALLY between A and the
new inputs (never varies between identical-input cooks) can never enter the region however many cooks run; only
run-to-run-varying bytes can.

## Fixed in advance
- Exactly 10 more identical-input cooks, d4..d13 (13 in total), same command, host, binaries and inputs as d1..d3, each
  archived to its own fresh E: directory. The count is fixed now; no cook is dropped, none is added after.
- The candidate stays d1. d1 is not re-cooked or replaced.
- The verdict rule is UNCHANGED: region R = union of pairwise differences over all 13 identical-input cooks; PASS iff
  diff(A, d1) lies inside R at EVERY one of the four 4-byte word alignments, AND the .uheader is byte-identical in A and
  all 13 cooks, AND the .uexp sizes are equal. Byte-level outside counts and the region growth curve are reported.
  diff(A, dN) for every N is reported too.
- If it fails, nothing is staged and the result is reported as NEEDS-DECISION with the field-level reading attached.