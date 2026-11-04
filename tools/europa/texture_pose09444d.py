from __future__ import print_function
import json
import math
import os
import time
import unreal

EXPOSURES = [-1.5 - i * 0.0625 for i in range(17)]
FOV = 30.0
DISTANCE = 1300.0
SIDE = 125.0


def write(path, value):
    with open(path, 'w') as stream:
        json.dump(value, stream, indent=2)


def vector(v):
    return unreal.Vector(v[0], v[1], v[2])


def camera_settled(began, now):
    return began is not None and now - began >= 1.0


def setup(world, pc, root):
    unreal.SystemLibrary.execute_console_command(world, 'fov 30')
    pc.set_control_rotation(unreal.Rotator(pitch=-12.0, yaw=0.0, roll=0.0))
    camera = unreal.GameplayStatics.get_player_camera_manager(world, 0)
    location = camera.get_camera_location()
    assert abs(location.x-100.0)<0.01 and abs(location.y+700.0)<0.01 and abs(location.z-2000.0)<0.01
    pitch = math.radians(-12.0)
    forward = [math.cos(pitch), 0.0, math.sin(pitch)]
    targets = []
    actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
    for actor in actors:
        if actor.actor_has_tag('OF_LOD_A'):
            actor.set_actor_location(unreal.Vector(650.0, -450.0, 300.0), False, False)
        skin = actor.actor_has_tag('EA_Skeletal')
        if not skin and not actor.actor_has_tag('OF_LOD_B'):
            continue
        actor.set_actor_scale3d(unreal.Vector(5.0 if skin else 1.3, 5.0 if skin else 1.3, 5.0 if skin else 1.3))
        actor.set_actor_rotation(unreal.Rotator(pitch=-12.0, yaw=0.0, roll=0.0), False)
        center, extent = actor.get_actor_bounds(False)
        old = actor.get_actor_location()
        wanted = [location.x + DISTANCE * forward[0], location.y + (-SIDE if skin else SIDE),
                  location.z + DISTANCE * forward[2]]
        actor.set_actor_location(vector([old.x + wanted[0] - center.x, old.y + wanted[1] - center.y,
                                         old.z + wanted[2] - center.z]), False, False)
        meshes = actor.get_components_by_class(unreal.MeshComponent)
        assert meshes
        baseline = unreal.load_object(None, '/Engine/EngineMaterials/WorldGridMaterial.WorldGridMaterial')
        assert baseline
        for mesh in meshes:
            for slot in range(mesh.get_num_materials()):
                mesh.set_material(slot, baseline)
        targets.append((actor, meshes))
    assert len(targets) == 2
    write(os.path.join(root, 'pose-setup.json'), dict(runtime_only=True, fov=FOV, distance=DISTANCE,
          side=SIDE, rotation=[-12.0, 0.0, 0.0], actors=[a.get_name() for a, m in targets],
          camera=[location.x, location.y, location.z], exposure_ladder=EXPOSURES))
    return targets


def main(root, mode, exposure=None):
    assert os.path.normcase(os.path.abspath(unreal.Paths.project_dir())) == os.path.normcase('E:\\IA_Fixtures\\Outline425')
    assert 'IAIBench' in unreal.SystemLibrary.get_command_line()
    assert mode in ('preflight-cubes', 'texture-off', 'texture-on', 'texture-mutant')
    state = dict(began=time.time(), ready=False, native_at=None, step=-1, pending=None, samples=[], targets=[], materials=[], handle=None, world=None, pc=None)
    unreal.SystemLibrary.execute_console_command(unreal.EditorLevelLibrary.get_editor_world(), 'IAI.Capture.PIEStart')

    def finish():
        for actor, meshes in state['targets']:
            for mesh in meshes:
                for slot in range(mesh.get_num_materials()):
                    mesh.set_material(slot, state['materials'][0])
        unreal.unregister_slate_post_tick_callback(state['handle'])
        unreal.SystemLibrary.execute_console_command(unreal.EditorLevelLibrary.get_editor_world(), 'QUIT_EDITOR')

    def tick(delta):
        try:
            if time.time() - state['began'] > 800:
                raise RuntimeError('No-fire pose probe timed out')
            if time.time() - state['began'] < 5:
                return
            world = unreal.EditorLevelLibrary.get_game_world()
            if world is None:
                return
            pc = unreal.GameplayStatics.get_player_controller(world, 0)
            if pc is None:
                return
            if state['native_at'] is None:
                native = 'OutlineFixture.TextureBench ' + os.path.join(root, 'texture-setup.json').replace('\\', '/')
                native += (' mutant ' if mode == 'texture-mutant' else ' good ') + '-2'
                unreal.SystemLibrary.execute_console_command(world, native, pc)
                unreal.SystemLibrary.execute_console_command(world, 'OutlineFixture.Blendable ' + ('1' if mode == 'texture-on' else '0'), pc)
                state['native_at'] = time.time()
                return
            if not camera_settled(state['native_at'], time.time()):
                return
            if not state['ready']:
                state['targets'] = setup(world, pc, root)
                state['materials'] = [state['targets'][0][1][0].get_material(0),
                    unreal.load_object(None, '/AnomalyInjector/Engine425/Materials/M_MissingTexture_Checker.M_MissingTexture_Checker'),
                    unreal.load_object(None, '/AnomalyInjector/Engine425/Materials/M_CorruptedTexture_Pink.M_CorruptedTexture_Pink')]
                assert all(state['materials'])
                state['ready'] = True
                state['ready_at'] = time.time()
                return
            if time.time() - state['ready_at'] < 2:
                return
            if mode != 'preflight-cubes':
                assert exposure is not None
                unreal.SystemLibrary.execute_console_command(world, 'r.ExposureOffset ' + str(exposure))
                path = os.path.join(root, 'plugin-frames').replace('\\', '/')
                command = 'IAI.Capture.C5 ' + path + ' 4242'
                if mode == 'texture-mutant':
                    command += ' missing_texture'
                import capture_depth_read09444d
                state['depth_observer'] = capture_depth_read09444d.start(root)
                unreal.SystemLibrary.execute_console_command(world, command)
                unreal.unregister_slate_post_tick_callback(state['handle'])
                return
            if state['pending']:
                if not os.path.isfile(os.path.join(state['pending'], 'capture.json')):
                    return
                state['pending'] = None
            state['step'] += 1
            step = state['step']
            if step == len(EXPOSURES) * 3:
                write(os.path.join(root, 'calibration', 'calibration.json'), dict(preview_count=step,
                      auto_injection=False, samples=state['samples']))
                finish()
                return
            bias, kind = EXPOSURES[step // 3], step % 3
            unreal.SystemLibrary.execute_console_command(world, 'r.ExposureOffset ' + str(bias))
            centers = []
            for actor, meshes in state['targets']:
                for mesh in meshes:
                    for slot in range(mesh.get_num_materials()):
                        mesh.set_material(slot, state['materials'][kind])
                center, extent = actor.get_actor_bounds(False)
                point = pc.project_world_location_to_screen(center, False)
                assert point is not None
                centers.append(dict(actor=actor.get_name(), x=point.x, y=point.y, projected=True))
            directory = os.path.join(root, 'calibration', 'preview-%02d' % step).replace('\\', '/')
            state['pending'] = directory
            state['samples'].append(dict(step=step, material=kind, exposure=bias, centers=centers, directory=directory,
                exposure_readback=unreal.SystemLibrary.get_console_variable_float_value('r.ExposureOffset'),
                method_readback=unreal.SystemLibrary.get_console_variable_int_value('r.EyeAdaptation.MethodOverride')))
            unreal.SystemLibrary.execute_console_command(world, 'IAI.Capture.C1 ' + directory + ' 8')
        except Exception as error:
            write(os.path.join(root, 'pose-error.json'), dict(result='FAIL', error=str(error), step=state['step']))
            finish()

    if mode == 'preflight-cubes':
        os.mkdir(os.path.join(root, 'calibration'))
    state['handle'] = unreal.register_slate_post_tick_callback(tick)
    return state
