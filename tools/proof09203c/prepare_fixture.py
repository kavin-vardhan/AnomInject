import hashlib
import json
from pathlib import Path
import numpy as np
from PIL import Image

work = Path('E:/IA_BuildCache/_r921/09203c')
repo = work.parent / 'AnomalyInjector/tools/proof09203'
y, x = np.indices((2048, 2048))
value = np.where(((x // 8 + y // 64) % 2) == 0, 8, 247).astype(np.uint8)
pixels = np.stack((value, value, value), axis=-1)
pixels[96:224, 96:448] = [240, 15, 25]
pixels[96:640, 96:208] = [240, 15, 25]
pixels[320:416, 96:352] = [240, 15, 25]
pattern = work / 'T_VTProof.png'
assert not pattern.exists()
Image.fromarray(pixels).save(pattern)
(work / 'pattern.json').write_text(json.dumps(dict(size=[2048, 2048], sha256=hashlib.sha256(pattern.read_bytes()).hexdigest(), pattern='8 by 64 high-contrast checker; asymmetric red F'), indent=2), encoding='utf-8')
for name in ('author_vt.py', 'launch_author.py'):
    body = (repo / name).read_text(encoding='utf-8')
    body = body.replace("Path('E:/IA_BuildCache/_r921')", "Path('E:/IA_BuildCache/_r921/09203c')")
    body = body.replace('ProxyVT0921', 'ProxyVT0921C')
    if name == 'author_vt.py':
        body = body.replace('    scan_mainworld()\n', '')
        body = body.replace("('target', 620, 540, 1152, 832, vt_mat)", "('target', 704, 540, 1280, 320, vt_mat)")
        body = body.replace("    for actor in actors:\n        if actor.get_component_by_class(unreal.StaticMeshComponent):\n            actor.set_actor_location(unreal.Vector(-10000, 0, -10000), False, True)\n    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)",
            "    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)\n    for actor in actors:\n        if actor != target and actor.get_component_by_class(unreal.StaticMeshComponent):\n            subsystem.destroy_actor(actor)")
        body = body.replace("texture.get_path_name(), root + '/T_OrdinaryProof'", "root + '/T_VTProof', root + '/T_OrdinaryProof'")
    (work / name).write_text(body, encoding='utf-8')
