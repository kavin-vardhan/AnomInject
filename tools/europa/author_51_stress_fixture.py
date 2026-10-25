import json
import os
import unreal

project=os.path.normcase(os.path.abspath(unreal.Paths.project_dir()))
allowed=[os.path.normcase(os.path.abspath(p)) for p in ('D:/IA_Europa/Baseline51/StackOBot','D:/IA_Europa/ReGate51/StackOBot')]
if project not in allowed: raise RuntimeError('Owned 5.1 fixtures only')
path='/Game/EuropaPhaseA/EA_Census15000'
if unreal.EditorAssetLibrary.does_asset_exist(path): raise RuntimeError('Existing fixture is never overwritten')
out=os.path.join(project,'Saved','EuropaPhaseA','census-authoring.json')
if os.path.exists(out): raise RuntimeError('Existing authoring receipt')
if not os.path.isdir(os.path.dirname(out)): os.makedirs(os.path.dirname(out))
receipt=dict(result='INCOMPLETE',map=path,created_count=0,recipe='150x100 deterministic engine-cube actor grid',qualified=False)
def save_receipt():
    with open(out,'w') as stream: json.dump(receipt,stream,indent=2)
save_receipt()
eal=unreal.EditorAssetLibrary
ell=unreal.EditorLevelLibrary
if not ell.new_level(path): raise RuntimeError('New census map failed')
cube=unreal.load_asset('/Engine/BasicShapes/Cube')
if not cube: raise RuntimeError('Missing engine cube')
for index in range(15000):
    x=index%150
    y=index//150
    actor=ell.spawn_actor_from_class(unreal.StaticMeshActor,unreal.Vector(500+x*160,(y-50)*160,50))
    if not actor: raise RuntimeError('Population incomplete')
    actor.set_actor_label('EA_Census_%05d'%index)
    actor.set_editor_property('tags',[unreal.Name('EA_Census')])
    actor.static_mesh_component.set_static_mesh(cube)
    actor.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
    receipt['created_count']=index+1
    if (index+1)%500==0: save_receipt()
light=ell.spawn_actor_from_class(unreal.DirectionalLight,unreal.Vector(0,0,500),unreal.Rotator(-45,0,0))
light.light_component.set_mobility(unreal.ComponentMobility.MOVABLE)
camera=ell.spawn_actor_from_class(unreal.CameraActor,unreal.Vector(-500,0,300),unreal.Rotator(0,0,0))
camera.set_editor_property('tags',[unreal.Name('EA_Camera')])
ell.get_editor_world().get_world_settings().set_editor_property('default_game_mode',unreal.GameModeBase)
ell.spawn_actor_from_class(unreal.PlayerStart,unreal.Vector(-500,0,300))
if not ell.save_current_level(): raise RuntimeError('Save failed')
receipt.update(result='AUTHORED_NOT_QUALIFIED',qualification_pending=['cold reopen actor count','paired content manifests','positive eligible cases','all refusal reasons in complementary canonical CaptureBench fixture','100ms census hard cap'])
save_receipt()
