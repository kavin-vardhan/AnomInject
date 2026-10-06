from pathlib import Path
import json


def step(context):
    unreal=context['unreal']
    plan=context['plan']
    actor=next(a for a in unreal.GameplayStatics.get_all_actors_of_class(context['pie'],unreal.StaticMeshActor) if a.get_name()=='StaticMeshActor_77')
    component=actor.get_component_by_class(unreal.StaticMeshComponent)
    before=component.get_material(0).get_path_name()
    material=unreal.load_asset('/Game/CaptureBenchTexCorrupt/ProxyVT09205/'+plan['fixture_material'])
    assert material is not None
    component.set_material(0,material)
    result=dict(actor=actor.get_name(),before=before,after=component.get_material(0).get_path_name())
    Path(plan['fixture_setup_result']).write_text(json.dumps(result,indent=2))
    context['say']('FIXTURE SETUP '+json.dumps(result))
    return True
