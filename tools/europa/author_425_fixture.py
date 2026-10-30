import json
import os


def author(recipe, receipt_path):
    import unreal

    ROOT = '/Game/EuropaPhaseA'
    PROJECT = os.path.normcase(os.path.abspath(unreal.Paths.project_dir()))
    EXPECTED = os.path.normcase(os.path.abspath('D:/IA_Europa/StylizedParisStreet'))
    if PROJECT != EXPECTED:
        raise RuntimeError('Fixture authoring is confined to the owned Paris copy')
    EAL = unreal.EditorAssetLibrary
    ELL = unreal.EditorLevelLibrary
    SM = unreal.EditorStaticMeshLibrary
    OUT = receipt_path
    if recipe not in ('EA_Core','EA_Travel'):
        raise RuntimeError('Unknown fixture recipe')
    paths = [ROOT+'/'+recipe] + ([ROOT+'/SM_EA_TwoLod'] if recipe == 'EA_Core' else [])
    if any(EAL.does_asset_exist(p) for p in paths):
        raise RuntimeError('No overwrite: existing fixture needs a separate hash/admission decision')
    if not os.path.isdir(os.path.dirname(OUT)): os.makedirs(os.path.dirname(OUT))
    if os.path.exists(OUT):
        raise RuntimeError('Author receipt already exists')
    for asset in paths:
        suffix = '.uasset' if asset.endswith('SM_EA_TwoLod') else '.umap'
        file_name = os.path.join(PROJECT,'Content',asset[len('/Game/'):]+suffix)
        if os.path.exists(file_name):
            raise RuntimeError('No overwrite: existing package file '+file_name)
    receipt = dict(result='INCOMPLETE',recipe=recipe,project=PROJECT,asset_paths=paths,created=[],runtime_qualification=False)

    def record():
        with open(OUT,'w') as stream:json.dump(receipt,stream,indent=2)

    def need(value,label):
        if not value:raise RuntimeError(label)
        return value

    def tag(actor,name):
        actor.set_actor_label(name)
        actor.set_editor_property('tags',[unreal.Name(name)])
        return actor

    def static(mesh,name,position,scale):
        actor=tag(need(ELL.spawn_actor_from_class(unreal.StaticMeshActor,unreal.Vector(*position)),name),name)
        component=actor.static_mesh_component
        component.set_mobility(unreal.ComponentMobility.MOVABLE)
        component.set_static_mesh(mesh)
        actor.set_actor_scale3d(unreal.Vector(*scale))
        return actor

    record()
    world=need(unreal.EditorLoadingAndSavingUtils.new_blank_map(False),'unsaved blank map')
    camera=tag(need(ELL.spawn_actor_from_class(unreal.CameraActor,unreal.Vector(-550,0,480),
        unreal.Rotator(pitch=-12.0, yaw=0.0, roll=0.0)),'fixture camera'),'EA_Camera')
    rotation=camera.get_actor_rotation()
    actual={key:float(getattr(rotation,key)) for key in ('pitch','yaw','roll')}
    expected=dict(pitch=-12.0,yaw=0.0,roll=0.0)
    normalized={key:(value+180.0)%360.0-180.0 for key,value in actual.items()}
    passed=all(abs((normalized[key]-expected[key]+180.0)%360.0-180.0)<=0.01 for key in expected)
    receipt['camera_check']=dict(actual=actual,normalized=normalized,expected=expected,tolerance_degrees=0.01,
        passed=passed,checked_before_any_save=True)
    record()
    if not passed:
        receipt.update(result='AUTHORING_FAIL',reason='FI2 camera angle assertion failed',exit_requests=1)
        record()
        unreal.SystemLibrary.execute_console_command(world,'QUIT_EDITOR')
        return receipt
    camera.camera_component.set_editor_property('field_of_view',50.0)
    cube=need(unreal.load_asset('/Engine/BasicShapes/Cube'),'engine cube')
    sphere=need(unreal.load_asset('/Engine/BasicShapes/Sphere'),'engine sphere')
    if recipe == 'EA_Core':
        lod=need(EAL.duplicate_asset('/Engine/BasicShapes/Sphere',ROOT+'/SM_EA_TwoLod'),'owned LOD duplicate')
        receipt['created'].append(ROOT+'/SM_EA_TwoLod');record()
        need(SM.set_lod_from_static_mesh(lod,1,cube,0,True)==1,'LOD1 copy')
        need(SM.get_lod_count(lod)>=2,'Two real render LODs required')
        need(EAL.save_loaded_asset(lod,False),'save owned LOD mesh')
        receipt['lod_count']=SM.get_lod_count(lod)
    else:
        lod=need(unreal.load_asset(ROOT+'/SM_EA_TwoLod'),'previously authored mesh')
        need(SM.get_lod_count(lod)>=2,'Two real render LODs required')
        receipt['lod_count']=SM.get_lod_count(lod)

    for name in (recipe,):
        level=ROOT+'/'+name
        static(cube,'EA_Floor',(700,0,-50),(18,18,1))
        static(cube,'EA_HideCube',(600,-260,110),(2,2,2))
        static(cube,'EA_BlinkCube',(600,260,110),(2,2,2))
        static(lod,'EA_LOD',(800,0,160),(3,3,3))
        light=tag(ELL.spawn_actor_from_class(unreal.PointLight,unreal.Vector(450,0,650)),'EA_Light')
        light.light_component.set_mobility(unreal.ComponentMobility.MOVABLE)
        light.light_component.set_editor_property('intensity',5000.0)
        light.light_component.set_editor_property('attenuation_radius',1800.0)
        light.light_component.set_editor_property('cast_shadows',True)
        pp=ELL.spawn_actor_from_class(unreal.PostProcessVolume,unreal.Vector(0,0,0))
        pp.set_editor_property('unbound',True)
        settings=pp.get_editor_property('settings')
        for field,value in [('override_auto_exposure_min_brightness',True),('override_auto_exposure_max_brightness',True),
                            ('auto_exposure_min_brightness',1.0),('auto_exposure_max_brightness',1.0),
                            ('override_motion_blur_amount',True),('motion_blur_amount',0.0)]:
            settings.set_editor_property(field,value)
        pp.set_editor_property('settings',settings)
        world.get_world_settings().set_editor_property('default_game_mode',unreal.GameModeBase)
        ELL.spawn_actor_from_class(unreal.PlayerStart,unreal.Vector(-550,0,500),unreal.Rotator(pitch=0.0, yaw=0.0, roll=0.0))
        need(unreal.EditorLoadingAndSavingUtils.save_map(world,level),'save '+level)
        receipt['created'].append(level);record()
        receipt.setdefault('maps',[]).append(dict(path=level,actors=sorted(a.get_actor_label() for a in ELL.get_all_level_actors()),
            anomaly_targets=dict(missing_object='EA_HideCube',blinking='EA_BlinkCube',lod_corruption='EA_LOD',lighting_mismatch='EA_Light')))
        record()
    receipt['result']='AUTHORED_NOT_QUALIFIED'
    receipt['qualification_pending']=['cold reopen','shader/streaming ready','20-frame noise','ROI freeze','three events per route','travel/GC','owner preview and eye check']
    record()
