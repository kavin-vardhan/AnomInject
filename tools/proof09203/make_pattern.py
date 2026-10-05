import hashlib
import json
from pathlib import Path
import numpy as np
from PIL import Image

work = Path('E:/IA_BuildCache/_r921')
size = 2048
y, x = np.indices((size, size))
checker = ((x // 3 + y // 5) % 2).astype(np.uint8)
diagonal = ((x + 2*y) % 37 < 5).astype(np.uint8)
red = (25 + 150*checker + 45*diagonal).astype(np.uint8)
green = (35 + 125*(1-checker) + 60*(x > 1300)).astype(np.uint8)
blue = (35 + 130*((x//7 + y//11) % 2) + 40*(y < 650)).astype(np.uint8)
pattern = np.stack([red, green, blue], axis=-1)
pattern[70:190, 80:600] = [240, 25, 35]
pattern[120:680, 90:210] = [240, 25, 35]
path = work / 'T_VTProof.png'
assert not path.exists()
Image.fromarray(pattern).save(path)
(work / 'pattern.json').write_text(json.dumps(dict(path=str(path), size=[size, size], sha256=hashlib.sha256(path.read_bytes()).hexdigest(), purpose='Asymmetric multi-frequency identity and blur fixture'), indent=2), encoding='utf-8')
