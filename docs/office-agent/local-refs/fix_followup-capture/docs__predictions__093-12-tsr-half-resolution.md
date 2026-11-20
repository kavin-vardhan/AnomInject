# 093-12: historic pairing at reduced internal resolution

Authority: 093-11-ruling Z1-Z3. Freeze before any new engine leg. Candidate
eff797c and all product/helper binaries remain unchanged. Worktree _r933,
fix/followup-capture; evidence _r9312. No product, fixture or permanent reader edit.

## Recipe and retained evidence

Every pairing leg uses the adopted historic recipe: host values for anti-aliasing,
Lumen diffuse indirect, dynamic GI and reflections. No arbiter override for those
four settings is issued. Auto-exposure off, marker, native/synthetic ordering,
screen-percentage commands, targets, frame counts and all other settings remain
as frozen in 093-11. Recipe budget is closed, two of two attempts used.

The stopped 093-11 Stack native50 session remains unchanged and is classified
"size assertion encoded AA-off". Its Z2 assessment is a reading only. A fresh
Stack native50 historic session is the gate. Any AA-off 50% leg would retain the
original half-picture size assertion; none is planned.

## Z2: four requirements for every historic 50% leg, PIE and staged

1. Reduced render: every captured frame's mask-pass line must report internal
   width and height exactly half the output view rect. Parse actual M23 PASS
   lines and their served target-mask request IDs, not the requested CVar.
   Require coverage of every captured index and no missing/duplicate serving
   target request. The probe forces one target-mask arm per sample; its serial
   starts at one and is encoded with bit 61, as in eff797c. Also require every
   mask-pass line in this one-run log to satisfy the ratio, including drain
   samples. PIE example: 640x496 internal inside 1280x992 output; staged sizes
   come from that leg. Can-fail: historic 100% log lines must reject this test.
2. One size throughout: every picture and every mask file equals the written
   picture size in readback_layout, every label row's width/height matches it,
   each row names its matching picture/mask and each valid pixel box is bounded
   by those dimensions. Existing measured-mask count/bounds checks remain.
   Can-fail: a copied label row with altered width must reject. Source hashes
   must remain unchanged.
3. Box agreement: call the unchanged office PF-2 measured_geometry and box_scale
   estimator on labelled rows. It keeps the existing finite/non-transition,
   one-pixel-inside and 5%-centre-displacement exclusions, median per-axis
   projected/mask centre-displacement ratio, and no trimming/refitting. Each
   axis with at least 20 distinct qualifying rows gates at [0.990,1.010]. An
   axis with fewer rows is a reading, including zero rows/unavailable scale;
   report n and scale per axis and do not invent a pass for that axis.
   Can-fail: on each gating axis, a copied projected-box 0.96 scale about the
   view centre must reject. Preserve row selection and original input hashes.
4. Pairing: unchanged permanent m44_pairing_probe.py, SHA-256
   54928b2ecce48d97a1af4bea16b00184ca091e8b22670fe68e8e3165671f3525,
   tolerance 40 px, PREVIOUS=NEITHER=0, decidable floor 1. A shifted-mask copy
   must reject. Positive originals rehash unchanged. Requested=written with
   zero dropped/unpaired/pending samples remains required for pairing.

At 100%, retain the existing full-picture assertion and shifted-mask control.
The 100% comparison for a native50 leg may use its already-passed 093-11 native100
session. Do not rerun accepted 100% positives just to obtain a local path.

Probe coverage remains the permanent reader's sampled bright-magenta pixels
inside tag-255 mask divided by sampled mask area, median over decidable frames.
speed_ratio remains the summary's m51 wall/game-span reading or unavailable.
Neither reading gates. For any new NEITHER, measure outside-mask bright-magenta
count, bounding extent and centroid on the reader's exact grid, with output-pixel
coordinates; report the grid explicitly. Relate counts to consecutive same-position
dwell using the retained probe STEP positions and per-frame mask centroid, without
changing the reader or drawing a causal conclusion from correlation alone.

## Order and unchanged floors

After preflight and this commit: Stack native50, synthetic100, synthetic50;
Lyra native50, synthetic100, synthetic50; exact J1/release engine controls;
supplemental admission; both PIE camera proofs; cook; every staged gate including
four PF-2 cases, resize and all eight historic pairings. The staged EXE and DLL
bytes must equal the candidate manifest when -nodebuginfo is used. No push until
every gate is green. PF-1 through PF-5 and completed matrix results stand only
after identical candidate rehash. W2 resize/camera keeps r=w+named drops, no stale
or mismatched acceptance; every other healthy leg keeps w=r. Existing event,
edge, mask, marker and can-fail floors are unchanged.

## Z3 stop rules (verbatim from 093-11-ruling)

**Stop rules, carried and added:**
- **Pairing recipe: closed** (2/2 used). Any PREVIOUS > 0 on any pairing leg is a STOP and is never retried.
- **Lyra spill.** Lyra's historic pass had a thin margin: about 4,700 bright-magenta pixels outside the mask pulled its centroid about 29 px, against the 40 px tolerance (Code, observation C). If any pairing leg reads NEITHER > 0 with PREVIOUS = 0, it is a STOP. First take these readings on that leg (no new leg):
  - the bright-magenta pixels outside the current mask on each NEITHER frame: count, extent and centroid;
  - whether that count grows the longer the probe has stayed in one place.
- **Packaging metadata: spent** (2/2). A cook failure for that reason is a STOP.
- **Marker: spent**, as Y5.
- D2 is otherwise unchanged.
- **Not authorised:**
  - a reader, tolerance or predicate change;
  - a fixture content change;
  - any product source change.

The predicate change expressly authorized by Z2 is only the four-part 50% proof
above. No other reader, tolerance or predicate change is authorized.
