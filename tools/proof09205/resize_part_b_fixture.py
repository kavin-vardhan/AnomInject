import hashlib
import json
from pathlib import Path
import unreal

w=Path('E:/IA_BuildCache/_r921/09205')
root='/Game/CaptureBenchTexCorrupt/ProxyVT09205'
report=dict(changed=[],errors=[])
assets=unreal.EditorAssetLibrary
tools=unreal.AssetToolsHelpers.get_asset_tools()
try:
    for name,compression,srgb in [('Color',unreal.TextureCompressionSettings.TC_DEFAULT,True),('Normal',unreal.TextureCompressionSettings.TC_NORMALMAP,False),('Roughness',unreal.TextureCompressionSettings.TC_MASKS,False)]:
        for virtual in (True,False):
            asset_name=('T_Stack' if virtual else 'T_Ordinary')+name
            assert assets.does_asset_exist(root+'/'+asset_name)
            task=unreal.AssetImportTask()
            task.filename=str(w/'part-b-patterns'/(asset_name+'.png'))
            task.destination_path=root
            task.destination_name=asset_name
            task.automated=True
            task.save=True
            task.replace_existing=True
            task.replace_existing_settings=True
            tools.import_asset_tasks([task])
            assert task.imported_object_paths == [root+'/'+asset_name+'.'+asset_name], task.imported_object_paths
            texture=unreal.load_asset(root+'/'+asset_name)
            texture.set_editor_property('compression_settings',compression)
            texture.set_editor_property('srgb',srgb)
            texture.set_editor_property('lod_bias',0)
            texture.set_editor_property('num_cinematic_mip_levels',0)
            texture.set_editor_property('never_stream',False)
            texture.set_editor_property('mip_gen_settings',unreal.TextureMipGenSettings.TMGS_SIMPLE_AVERAGE)
            texture.set_editor_property('virtual_texture_streaming',virtual)
            assets.save_loaded_asset(texture)
            report['changed'].append(texture.get_path_name())
    for name in ['M_Stack','M_Ordinary','M_Unnamed']:
        material=unreal.load_asset(root+'/'+name)
        unreal.MaterialEditingLibrary.recompile_material(material)
        assets.save_loaded_asset(material)
except Exception as e:
    report['errors'].append(str(e))
finally:
    (w/'part-b-fixture-resize.json').write_text(json.dumps(report,indent=2))
    unreal.log('VTSTACKS-RESIZE '+json.dumps(report))
