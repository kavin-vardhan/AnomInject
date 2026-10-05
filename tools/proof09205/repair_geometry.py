import hashlib
import json
import shutil
from pathlib import Path
import unreal

w=Path('E:/IA_BuildCache/_r921/09205')
asset=Path('E:/IA_BuildCache/_r53_host/StackOBot/Content/CaptureBenchTexCorrupt/ProxyVT09205/CB_ProxyBlurStacks.umap')
backup=w/'fixture-before-geometry-repair.umap'
assert not backup.exists()
shutil.copyfile(asset,backup)
report=dict(errors=[],actors=[],before=hashlib.sha256(asset.read_bytes()).hexdigest(),backup=str(backup))
try:
    path='/Game/CaptureBenchTexCorrupt/ProxyVT09205/CB_ProxyBlurStacks'
    world=unreal.EditorLoadingAndSavingUtils.load_map(path)
    for actor in unreal.GameplayStatics.get_all_actors_of_class(world,unreal.Actor):
        if isinstance(actor,unreal.StaticMeshActor):
            before=str(actor.get_actor_rotation())
            actor.set_actor_rotation(unreal.Rotator(pitch=90,yaw=180,roll=180),True)
            report['actors'].append(dict(name=actor.get_name(),before=before,after=str(actor.get_actor_rotation()),up=str(actor.get_actor_up_vector())))
        if isinstance(actor,unreal.PointLight):
            actor.point_light_component.set_editor_property('intensity',2000.0)
        if isinstance(actor,unreal.DirectionalLight):
            actor.set_actor_rotation(unreal.Rotator(pitch=-15,yaw=15,roll=0),True)
            actor.light_component.set_editor_property('intensity',1.0)
    unreal.EditorLoadingAndSavingUtils.save_map(world,path)
    report['after']=hashlib.sha256(asset.read_bytes()).hexdigest()
except Exception as error:
    report['errors'].append(str(error))
finally:
    (w/'repair-geometry-result.json').write_text(json.dumps(report,indent=2))
