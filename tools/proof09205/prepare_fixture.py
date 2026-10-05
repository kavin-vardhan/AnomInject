import json
from pathlib import Path
import numpy as np
from PIL import Image

w = Path('E:/IA_BuildCache/_r921/09205')
size = 2048
y, x = np.indices((size, size))
region = x * 3 // size
stripe = ((x // 4 + y // 16) % 2).astype(bool)
color = np.full((size, size, 3), 150, dtype=np.uint8)
color[(region == 0) & stripe] = [240, 35, 70]
color[(region == 0) & ~stripe] = [25, 210, 225]
normal = np.zeros((size, size, 3), dtype=np.uint8)
normal[:] = [128, 128, 255]
normal[(region == 1) & stripe] = [220, 175, 204]
normal[(region == 1) & ~stripe] = [35, 80, 204]
roughness = np.full((size, size, 3), 166, dtype=np.uint8)
roughness[(region == 2) & stripe] = 12
roughness[(region == 2) & ~stripe] = 245
for name, pixels in [('Color', color), ('Normal', normal), ('Roughness', roughness)]:
    Image.fromarray(pixels, 'RGB').save(w / ('T_Stack' + name + '.png'))
(w / 'fixture-pattern-recipe.json').write_text(json.dumps(dict(size=size, regions='U thirds: color, normal, roughness', stripe='(x // 4 + y // 16) % 2', independent=True), indent=2))
