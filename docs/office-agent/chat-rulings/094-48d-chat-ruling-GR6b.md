## Chat ruling GR6b (binding; replaces GR6)
**Correction:** GR6 assumed the failing texture fires were whole-frame. 094-48c showed they are not (`whole_frame: false` on every texture row). GR6 is withdrawn, and 48c was right to hold the push.

**What 48b measured on free-play** (F1, F3, F5, S4):
- changed pixels outside the mask: **0**;
- changed pixels on the clean frames just before and after every event: **0**;
- labelled frames with no change at all: **0**;
- labels on exactly the right frames, with every mask pixel inside its box;
- the part of the mask that stays within 2 levels: about 64 % for missing_texture, and 1–5 % for corrupted_texture.

The C5 bench fixture, built so every target pixel changes, passes strict IoU 1.000 with 0 spill (F4, S5).

**Ruling:**
1. **The texture label is an object mask:** the target's silhouette, as in 5.1. It is not a per-pixel "changed region". In real scenes, part of a textured object doesn't visibly change: dark or unlit areas, other material slots, regions whose colour happens to match. That's expected, not a label defect, and label meaning stays unchanged.
2. **Texture-label correctness stays qualified by the C5 bench** (strict IoU 1.000, 0 spill). That gate is unchanged.
3. **For free-play captures, the blocking texture checks are:**
   - 0 changed pixels outside the mask;
   - 0 changed pixels on clean neighbour frames;
   - 0 labelled frames with no change;
   - polluted 0, tagged 0;
   - 0 drops;
   - annotation PASS;
   - masks inside their boxes.

   Changed/mask coverage (IoU) is **recorded as information** (count, min, mean, max per anomaly type). It doesn't block.
4. **FUTURE (not now):** per-material-slot or changed-region texture masks.
