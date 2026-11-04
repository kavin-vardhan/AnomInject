import hashlib,json,re,subprocess
from pathlib import Path
P=Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector')
J=Path('D:/IA_Europa/Receipts/09445r/Stage2');O=Path('E:/IA_BuildCache/09445r-evidence/Stage2/selftests');O.mkdir(parents=True,exist_ok=False)
def original(path):return subprocess.check_output(['git','-C',str(P),'show','d7eee97:'+path]).decode('utf-8').replace('\r\n','\n')
parity=[]
for ext in ('cpp','h'):
 path='Source/AnomalyInjector/Private/Anomalies/Anomaly_CameraClipping.'+ext
 actual=(P/path).read_text(encoding='utf-8').replace('#include "Compat/AnomalyEngineVersion.h"\n','').replace('#include "Compat/AnomalyEngineCompat.h"\n','').replace('AnomalyCompat::CameraCachePOV(PC->PlayerCameraManager)','PC->PlayerCameraManager->GetCameraCacheView()')
 assert actual.rstrip()==original(path).rstrip(),path
 parity.append(dict(file=path,release='d7eee97',normalized_equal=True))
helper=(P/'Source/AnomalyCapture/Private/Compat/AnomalyCameraSession425.cpp').read_text()
releasewriter=original('Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp')
keys=lambda s:set(re.findall(r'TEXT\("(camera_clipping[._][^"\n]+)"\)',s))
assert keys(helper)==keys(releasewriter),(keys(helper)-keys(releasewriter),keys(releasewriter)-keys(helper))
sub=original('Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp')
begin=sub.index('FCameraClipRunAccum& A = CameraClipAccum;',sub.index('void UAnomalyCaptureSubsystem::AppendViewDependentGlobals'))
block=sub[begin:sub.index('\n\t\t}\n\n\t\tif (!bPositive)',begin)].replace('CameraClipAccum','Accum')
assert block in helper,'Release accumulation block changed'
test='tools/camera_clipping_slab_selftest.cpp'
assert (P/test).read_text().rstrip()==original(test).rstrip(),'Slab selftest changed'
flags=subprocess.CREATE_NO_WINDOW|subprocess.BELOW_NORMAL_PRIORITY_CLASS
vc='C:/Program Files (x86)/Microsoft Visual Studio/2017/BuildTools/VC/Auxiliary/Build/vcvars64.bat'
batch=O/'slab.bat';target=O/'slab.exe'
batch.write_text('@echo off\ncall "'+vc+'" >nul\nif errorlevel 1 exit /b 1\ncl /nologo /EHsc /std:c++14 /O2 /MD "'+str(P/test)+'" /Fo"'+str(O/'slab.obj')+'" /Fe"'+str(target)+'"\nexit /b %errorlevel%\n')
r=subprocess.run(['cmd.exe','/d','/c',str(batch)],cwd=O,capture_output=True,text=True,creationflags=flags)
(O/'compile.log').write_text(r.stdout+r.stderr);assert r.returncode==0,r.stdout+r.stderr
r=subprocess.run([str(target)],cwd=O,capture_output=True,text=True,creationflags=flags)
(O/'run.log').write_text(r.stdout+r.stderr);assert r.returncode==0,r.stdout+r.stderr
defaults=(P/'Source/AnomalyCapture/Private/Compat/AnomalyCaptureLabels425.cpp').read_text().split('TArray<FString> CaptureRoutes425 = ')[1].split(';')[0]
assert 'camera_clipping' not in defaults
result=dict(result='PASS',parity=parity,row_and_summary_key_count=len(keys(helper)),counter_block_equal=True,unchanged_slab_test=True,output=r.stdout,exe_sha256=hashlib.sha256(target.read_bytes()).hexdigest(),default_routes=defaults)
(J/'selftests.json').write_text(json.dumps(result,indent=2));print(json.dumps(result))
