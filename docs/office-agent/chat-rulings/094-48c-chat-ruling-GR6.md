# 094-48c - chat ruling GR6 (binding), recorded verbatim

Source: brief 094-48c-europa (mailbox), section "Chat ruling GR6 (binding)". Recorded 2026-11-06 by Claude Code, unedited.

## Chat ruling GR6 (binding)
**What 094-48 and 094-48b measured:**
- In free-play (F1, F3, F5, and S4 if it repeats them), `strict_texture` fails only on **whole-frame** texture fires.
  - `target_pixels = 921600` (the full 1280×720 frame);
  - IoU about 0.29;
  - edge frames are clean (0 changed pixels).
- Every other check passed: counts, first fire, input, polluted 0, tagged 0, mask pixels, annotation (5.1 schema) and game alive.
- The C5 bench regression (F4) passed H4 and the unchanged strict texture gates on the same label code.

**Ruling:**
1. A whole-frame texture fire is labelled with a whole-frame extent **by design**, which is 5.1's label meaning, and we never change label meaning. In an open street view, sky, emissive and untextured pixels don't change, so changed/mask IoU against a whole-frame mask depends on the scene. It is **not** a label defect.
2. **Texture-label correctness stays qualified by the C5 bench** (strict IoU 1.000, 0 spill), which passed (F4 / S5).
3. **For free-play captures,** whole-frame texture IoU is **recorded as information** (count, min, mean, max). It doesn't block. This is not a looser gate for C5 or for non-whole-frame fires.
4. **Still blocking in free-play:**
   - any non-whole-frame texture fire below IoU 1.000 or with spill > 0;
   - any changed pixel on a clean edge frame;
   - a non-zero polluted or tagged count;
   - a drop;
   - an annotation FAIL.
5. **The desktop observer's `framesWritten` vs `frames` mismatch** is a test-script mapping issue, already confirmed by direct observation. It isn't a product defect.
6. **FUTURE (not now):** changed-region labels for whole-frame texture fires.
