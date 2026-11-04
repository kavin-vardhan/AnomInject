import hashlib
import json
from pathlib import Path
import subprocess
import sys


def run(output):
    root=Path(output);root.mkdir(parents=True,exist_ok=False)
    p=Path(__file__).resolve().parents[2]
    flags=subprocess.CREATE_NO_WINDOW|subprocess.BELOW_NORMAL_PRIORITY_CLASS
    vc=Path('C:/Program Files (x86)/Microsoft Visual Studio/2017/BuildTools/VC/Auxiliary/Build/vcvars64.bat')
    results=[]
    def compile_case(name,source,expected=True,include=None):
        target=root/(name+'.exe');batch=root/(name+'.bat')
        command='cl /nologo /EHsc /std:c++14 /O2 /MD '+('/I"'+str(include)+'" ' if include else '')+'"'+str(source)+'" /Fo"'+str(root/(name+'.obj'))+'" /Fd"'+str(root/(name+'.pdb'))+'" /Fe"'+str(target)+'"'
        batch.write_text('@echo off\ncall "'+str(vc)+'" >nul\nif errorlevel 1 exit /b 1\n'+command+'\nexit /b %errorlevel%\n')
        r=subprocess.run(['cmd.exe','/d','/c',str(batch)],cwd=root,capture_output=True,text=True,creationflags=flags)
        (root/(name+'-compile.log')).write_text(r.stdout+r.stderr)
        assert (r.returncode==0)==expected,(name,r.stdout,r.stderr)
        if expected:
            q=subprocess.run([str(target)],capture_output=True,text=True,creationflags=flags)
            (root/(name+'-run.log')).write_text(q.stdout+q.stderr)
            assert q.returncode==0,(name,q.stdout,q.stderr)
        results.append(dict(name=name,compile_exit=r.returncode,expected_compile=expected))
    parity=[]
    for name in ('MissingTexture','CorruptedTexture'):
        file='Source/AnomalyInjector/Private/Anomalies/Anomaly_'+name+'.cpp'
        actual=(p/file).read_text(encoding='utf-8')
        original=subprocess.check_output(['git','-C',str(p),'show','d7eee97:'+file]).decode('utf-8').replace('\r\n','\n')
        restored=actual.replace('#include "Compat/AnomalyEngineCompat.h"\n','')
        restored=restored.replace('AnomalyCompat::OverrideMaterial(Mesh, Index)','Mesh->OverrideMaterials[Index].Get()')
        restored=restored.replace('AnomalyCompat::OverrideMaterial(Component, i)','Component->OverrideMaterials[i].Get()')
        assert restored.rstrip()==original.rstrip(),name+' behavior differs beyond compat'
        parity.append(dict(file=file,release='d7eee97',normalized_equal=True,release_sha256=hashlib.sha256(original.encode()).hexdigest()))
    native=(p/'Source/AnomalyInjector/Private/Compat/AnomalyEngineCompat.cpp').read_text()
    body=native[native.index('UMaterialInterface* AnomalyCompat::OverrideMaterial'):native.index('const UObject* AnomalyCompat::SkinnedAssetIdentity')]
    common='''#include <cassert>
using int32=int;
struct UMaterialInterface {};
struct ObjectPtr { UMaterialInterface* Value=nullptr; UMaterialInterface* Get() const { return Value; } };
template<class T> struct Array { T Values[2]; bool IsValidIndex(int I) const { return I>=0 && I<2; } T operator[](int I) const { return Values[I]; } };
struct UMeshComponent { Array<ELEMENT> OverrideMaterials; };
namespace AnomalyCompat { UMaterialInterface* OverrideMaterial(const UMeshComponent*,int32); }
'''
    for version,element,assignment in [(425,'UMaterialInterface*','&Material'),(51,'ObjectPtr','ObjectPtr{&Material}')]:
        source=root/('compat-'+str(version)+'.cpp')
        text='#define ANOMALY_ENGINE_425 '+str(int(version==425))+'\n'+common.replace('ELEMENT',element)+body
        text+='int main(){UMaterialInterface Material; UMeshComponent Mesh{}; Mesh.OverrideMaterials.Values[0]='+assignment+'; assert(AnomalyCompat::OverrideMaterial(&Mesh,0)==&Material); assert(!AnomalyCompat::OverrideMaterial(&Mesh,1)); assert(!AnomalyCompat::OverrideMaterial(&Mesh,-1)); assert(!AnomalyCompat::OverrideMaterial(&Mesh,2)); assert(!AnomalyCompat::OverrideMaterial(nullptr,0));}\n'
        source.write_text(text);compile_case('compat-'+str(version),source)
        if version==425:
            bad=root/'old-get-mutant.cpp';bad.write_text(text.replace('return Mesh->OverrideMaterials[Index];','return Mesh->OverrideMaterials[Index].Get();'))
            compile_case('old-get-mutant',bad,False)
    install=p/'tools/install_state_selftest.cpp'
    old=subprocess.check_output(['git','-C',str(p),'show','d7eee97:tools/install_state_selftest.cpp'])
    assert install.read_bytes().replace(b'\r\n',b'\n')==old.replace(b'\r\n',b'\n')
    compile_case('install-state',install)
    compile_case('routes',p/'tools/europa/capture_fz425_test.cpp',include=p/'Source/AnomalyCapture/Private/Compat')
    private=(p/'Source/AnomalyCapture/Private/Compat/AnomalyPrivatePass425.cpp').read_text()
    assert 'Item.Proxy=Item.Component->SceneProxy' in private
    result=dict(result='PASS',compiler='MSVC14.16',priority='BelowNormal',parity=parity,cases=results,install_state_unchanged=True,fresh_proxy_per_frame=True)
    (root/'result.json').write_text(json.dumps(result,indent=2));print(json.dumps(result))
    return result


if __name__=='__main__':
    run(sys.argv[1])
