import inspect
import json
import os

CALLS = [
    ('Paths.project_dir', 0), ('EditorAssetLibrary.does_asset_exist', 1),
    ('EditorAssetLibrary.duplicate_asset', 2), ('EditorAssetLibrary.save_loaded_asset', 2),
    ('load_asset', 1), ('EditorStaticMeshLibrary.set_lod_from_static_mesh', 5),
    ('EditorStaticMeshLibrary.get_lod_count', 1), ('EditorLoadingAndSavingUtils.new_blank_map', 1),
    ('EditorLevelLibrary.spawn_actor_from_class', 2), ('EditorLevelLibrary.spawn_actor_from_class', 3),
    ('EditorLoadingAndSavingUtils.save_map', 2), ('SystemLibrary.execute_console_command', 2),
    ('EditorLevelLibrary.get_all_level_actors', 0), ('Actor.set_actor_label', 2),
    ('Actor.set_editor_property', 3), ('Actor.get_editor_property', 2),
    ('Actor.set_actor_scale3d', 2), ('Actor.get_actor_label', 1), ('Actor.get_actor_rotation', 1),
    ('SceneComponent.set_mobility', 2), ('StaticMeshComponent.set_static_mesh', 2),
    ('Object.set_editor_property', 3), ('StructBase.set_editor_property', 3),
    ('World.get_world_settings', 1), ('Vector', 3), ('Rotator', 3), ('Name', 1),
]
NAMES = [
    'StaticMeshActor', 'PointLight', 'CameraActor', 'PostProcessVolume', 'PlayerStart', 'GameModeBase',
    'ComponentMobility.MOVABLE', 'AntiAliasingMethod.AAM_NONE',
    'StaticMeshActor.static_mesh_component', 'PointLight.light_component', 'CameraActor.camera_component',
]
PROPERTIES = {
    'Actor': ['tags'], 'PointLightComponent': ['intensity', 'attenuation_radius', 'cast_shadows'],
    'CameraComponent': ['field_of_view'], 'PostProcessVolume': ['unbound', 'settings'],
    'PostProcessSettings': ['override_auto_exposure_min_brightness', 'override_auto_exposure_max_brightness',
        'auto_exposure_min_brightness', 'auto_exposure_max_brightness', 'override_motion_blur_amount',
        'motion_blur_amount'],
    'WorldSettings': ['default_game_mode'],
}


def lookup(root, name):
    value = root
    for part in name.split('.'):
        value = getattr(value, part)
    return value


def inspect_call(root, name, count):
    row = dict(name=name, arguments=count, exists=False, signature='unavailable')
    try:
        value = lookup(root, name)
        row['exists'] = callable(value)
        if not row['exists']:
            row['error'] = 'Not callable'
            return row
        try:
            args, varargs, keywords, defaults = inspect.getargspec(value)
            row['signature'] = dict(args=args, varargs=varargs, keywords=keywords, defaults_count=len(defaults or ()))
            if not varargs and not len(args)-len(defaults or ()) <= count <= len(args):
                row['error'] = 'Recipe argument count does not fit the inspectable signature'
        except (TypeError, ValueError):
            row['signature'] = 'Native binding; Python 2.7 inspect cannot determine arity'
            row['binding_doc'] = (getattr(value, '__doc__', '') or '').split('\n')[0]
    except (AttributeError, TypeError) as error:
        row['error'] = str(error)
    return row


def check(fixture, receipt_path):
    import unreal
    rows = [inspect_call(unreal, name, count) for name, count in CALLS]
    for name in NAMES + [kind+'.'+prop for kind, props in PROPERTIES.items() for prop in props]:
        row = dict(name=name, exists=False)
        try:
            lookup(unreal, name)
            row['exists'] = True
        except AttributeError as error:
            row['error'] = str(error)
        rows.append(row)
    alive = callable(fixture.author) and fixture.os is os and fixture.json is json
    receipt = dict(result='SMOKE_PASS' if alive and all(r['exists'] and 'error' not in r for r in rows) else 'SMOKE_FAIL',
        module_alive=alive, checks=rows, creates=0, saves=0, loads=0,
        scope='Read-only Python binding inspection; no Unreal function invoked')
    if os.path.exists(receipt_path):
        raise RuntimeError('Smoke receipt already exists')
    with open(receipt_path, 'w') as stream:
        json.dump(receipt, stream, indent=2)
    return receipt
