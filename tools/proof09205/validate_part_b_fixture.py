import json
from pathlib import Path
import unreal

w=Path('E:/IA_BuildCache/_r921/09205')
root='/Game/CaptureBenchTexCorrupt/ProxyVT09205'
report=dict(textures=[],errors=[])
try:
    world=unreal.EditorLoadingAndSavingUtils.load_map(root+'/CB_ProxyBlurStacks')
    unreal.SystemLibrary.execute_console_command(world,'Editor.AsyncAssetCompilationFinishAll')
    for name,compression,srgb in [('Color',unreal.TextureCompressionSettings.TC_DEFAULT,True),('Normal',unreal.TextureCompressionSettings.TC_NORMALMAP,False),('Roughness',unreal.TextureCompressionSettings.TC_MASKS,False)]:
        for virtual in (True,False):
            texture=unreal.load_asset(root+'/'+('T_Stack' if virtual else 'T_Ordinary')+name)
            row=dict(path=texture.get_path_name(),virtual=texture.get_editor_property('virtual_texture_streaming'),srgb=texture.get_editor_property('srgb'),compression=str(texture.get_editor_property('compression_settings')),width=texture.blueprint_get_size_x(),height=texture.blueprint_get_size_y())
            report['textures'].append(row)
            assert row['virtual']==virtual and row['srgb']==srgb and row['compression']==str(compression),row
            assert row['width']==row['height']==1024,row
    for name in ['M_Stack','M_Ordinary','M_Unnamed']:
        material=unreal.load_asset(root+'/'+name)
        unreal.MaterialEditingLibrary.recompile_material(material)
        unreal.EditorAssetLibrary.save_loaded_asset(material)
    unreal.EditorLoadingAndSavingUtils.load_map(root+'/CB_ProxyBlurStacks')
except Exception as e:
    report['errors'].append(str(e))
finally:
    (w/'part-b-fixture-validate.json').write_text(json.dumps(report,indent=2))
    unreal.log('VTSTACKS-VALIDATE '+json.dumps(report))
