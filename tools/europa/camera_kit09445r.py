import argparse,datetime,json,os,subprocess,sys
from pathlib import Path
R=Path(__file__).resolve().parent
P=Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector');T=P/'tools/europa';sys.path.insert(0,str(T))
J=Path('D:/IA_Europa/Receipts/09445r');S=J/'Stage2';B=Path('E:/IA_BuildCache/09445r-evidence/Stage2')
INSTALLED=Path('D:/IA_Europa/Launch/Europa425-09445r-Camera.json')
SPECS={'D3D11-control':('D3D11',True),'D3D11-positive':('D3D11',False),'D3D12-control':('D3D12',True),'D3D12-positive':('D3D12',False)}
import capture_425_kit as c1
import capture_c5_kit as old
import camera_reader09445r as reader
from fixture_policy import HOST,sha,verify_files
from runtime_policy import protection_snapshot,verify_protection
from owner_attempts import Guard,create_json
from runtime_runner import execute,crash_policy
from runtime_evidence import freeze,outcome
from fixture_uploads import check as uploads
from window09445r import require_launch,require_runtime,admission_deadline

def files(paths):return [dict(path=str(p),size=p.stat().st_size,sha256=sha(p)) for p in paths]

def arm(leg):
 require_launch(900);assert leg in SPECS
 head=subprocess.check_output(['git','-C',str(P),'rev-parse','HEAD'],text=True).strip()
 assert not subprocess.check_output(['git','-C',str(P),'status','--porcelain'],text=True).strip()
 root=S/'legs'/leg/'01';assert not root.exists()
 for prior_leg in list(SPECS)[:list(SPECS).index(leg)]:
  prior=json.loads((S/'legs'/prior_leg/'01/result.json').read_text());assert prior['result']=='PASS',prior_leg+' failed; stop'
 builds=[json.loads((J/'builds'/n/'result.json').read_text()) for n in ('editor-03','game-02')]
 assert all(b['gate_result']=='PASS' and b['exit_code']==0 and c1.native_source_equal(b['commit']) for b in builds)
 archive=json.loads((S/'binary-archive.json').read_text());verify_files(archive['current']);verify_files(archive['archive'])
 entry=json.loads((J/'entry.json').read_text())['protected'];protected=protection_snapshot()
 assert all(entry[k]==protected[k] for k in ('content','config','descriptors'))
 prior=json.loads(c1.INSTALLED.read_text());manifest=json.loads(Path(prior['manifest']['path']).read_text())
 for row in manifest['files']:
  p=Path(row['path']);row.update(size=p.stat().st_size,mtime_ns=p.stat().st_mtime_ns,sha256=sha(p))
 manifest.update(source_commit=head,tooling_commit=head,build_sources={b['target']:b['commit'] for b in builds},purpose='094-45r Stage2 camera_clipping held session D4',prior_manifest=prior['manifest'])
 path=S/('manifest-'+leg+'.json');create_json(path,manifest)
 cfg=dict(job='09445r-Camera',leg=leg,root=str(root),spec=SPECS[leg],source_commit=head,protected=protected,manifest=dict(path=str(path),sha256=sha(path)),engine=prior['engine_files'],tools=files([Path(__file__),R/'camera_reader09445r.py',R/'console_probe09443r.py',R/'c_space09445r.py',R/'window09445r.py',*T.glob('*.py')]),source_files=files([p for part in ('Source','Shaders') for p in (P/part).rglob('*') if p.is_file()]+list((P.parents[1]/'Source').rglob('*.cpp'))),limit=900)
 cfg.update(arming_job=cfg['job'],engine_files=cfg['engine'],tooling_files=cfg['tools'],stage='D4')
 create_json(S/('arming-'+leg+'.json'),cfg);create_json(S/('prior-default-'+leg+'.json'),prior)
 launcher=c1.INSTALLED.with_suffix('.bat');(S/('prior-default-'+leg+'.bat')).write_bytes(launcher.read_bytes())
 INSTALLED.write_text(json.dumps(cfg,indent=2));c1.INSTALLED.write_text(json.dumps(cfg,indent=2))
 launcher.write_text('@echo off\n"C:\\Python313\\python.exe" -B "'+str(Path(__file__).resolve())+'" run\n',encoding='ascii')
 print(json.dumps(dict(result='ARMED',leg=leg,source=head,manifest=cfg['manifest'])),flush=True)

def validate(cfg):
 assert cfg['job']=='09445r-Camera' and tuple(cfg['spec'])==SPECS[cfg['leg']] and cfg['limit']==900
 for key in ('engine','tools','source_files'):verify_files(cfg[key])
 assert sha(cfg['manifest']['path'])==cfg['manifest']['sha256']
 verify_files(json.loads(Path(cfg['manifest']['path']).read_text())['files']);verify_protection(cfg['protected'])

def command(cfg,root):
 argv=old.command('C5',root);old.select_map(argv,'fallback')
 output=B/cfg['leg']/'01';output.mkdir(parents=True,exist_ok=False)
 macro=root/'Binaries/capture.txt';macro.write_text('IAI.Capture.C5 '+(output/'plugin-frames').as_posix()+' 4242 camera_clipping\n',encoding='ascii')
 rhi,baseline=cfg['spec']
 argv=[a for a in argv if a.lower() not in ('-d3d11','-d3d12')]
 argv+=['-'+rhi.lower(),'-IAICameraSlabBench']
 if baseline:argv+=['-IAICameraBaseline']
 argv=[a.replace('-ExecCmds=','-ExecCmds=r.CustomDepth,') for a in argv]
 assert '-game' in argv and '-nowrite' in argv and '-IAIBench' in argv and '-BENCHMARK' not in argv
 assert not any('IAIBenchBypassHostDepth' in a or 'IAICaptureLegacyReference' in a for a in argv)
 c1.require_frame_format(argv);return argv

def run():
 cfg=json.loads(INSTALLED.read_text());validate(cfg);leg=cfg['leg'];root=Path(cfg['root'])
 limits=json.loads((J/'c5-job-config.json').read_text());limits['temp_root']='D:/IA_Europa/Temp/09445r/Stage2'
 limits['runtime_scope']['legs']=list(SPECS);limits['runtime_scope']['maximum_sessions_per_leg']=1
 path=J/'camera-job-config.json';path.write_text(json.dumps(limits,indent=2));os.environ['EUROPA_JOB_CONFIG']=str(path)
 import write_ledger as ledger
 import runtime_monitor
 from runtime_lean_gate import wait
 runtime_monitor.FLOOR_GIB=dict(runtime_monitor.FLOOR_GIB,E=50,D=50)
 original=runtime_monitor.Monitor
 class Monitor(original):
  def __init__(self,*args,**kwargs):
   super().__init__(*args,**kwargs)
   from console_probe09443r import Probe
   self.probe=Probe(B/leg/'01')
  def check(self,owned):
   require_runtime();super().check(owned);self.probe.check(self.pid)
 with Guard(dict(state_root=str(S/'state'/leg))) as guard:
  guard.check_unused_launch();root.mkdir(parents=True,exist_ok=False);(root/'session').mkdir()
  process=root/'process';process.mkdir();os.environ['EUROPA_OPERATION_DIR']=str(process)
  argv=command(cfg,root);create_json(root/'prepared.json',dict(argv=argv,spec=cfg['spec']))
  def admit():
   import shutil
   assert shutil.disk_usage('E:/').free>=50*2**30
   assert shutil.disk_usage('D:/').free>=53*2**30,'GJ4: D: needs50GiB floor plus3GiB launch reserve; STOP if blocked'
   gate=wait(root/'gate.jsonl',admission_deadline(),idle_seconds=300,memory=(8,4));assert gate['status']=='CLEAR','GA5 admission held3h'
   validate(cfg);require_launch(900);guard.reserve_launch(dict(number=1,leg=leg));return gate
  from c_space09445r import snapshot,finish
  before=snapshot();create_json(root/'c-space-before.json',before);runtime_monitor.Monitor=Monitor
  try:row=execute(leg,root,Path(limits['temp_root'])/leg/'01',HOST,cfg['protected'],argv,ledger,900,admit=admit)
  finally:
   runtime_monitor.Monitor=original;space=finish(before,snapshot());create_json(root/'c-space-after.json',space)
  assert space['floor_pass'],'GL4 C: below10GiB'
  try:validate(cfg);protected=dict(result='PASS')
  except Exception as e:protected=dict(result='FAIL',reason=str(e))
  upload=uploads(root/'session');create_json(root/'upload-check.json',upload)
  log=(root/'session/Runtime.log').read_text(encoding='utf-8-sig',errors='replace')
  try:
   assert row.get('exit_code')==0 and 'LogExit: Exiting.' in log
   result=reader.read(B/leg/'01/plugin-frames/epoch01',log,*SPECS[leg])
   if not SPECS[leg][1]:
    pair=reader.paired(B/leg/'01/plugin-frames/epoch01',B/(SPECS[leg][0]+'-control')/'01/plugin-frames/epoch01')
    create_json(root/'paired-pixels.json',pair);result['pixel_gate']=pair['result']
    if pair['result']!='PASS':result['result']='FAIL'
  except Exception as e:
   import traceback
   result=dict(result='FAIL',reason=type(e).__name__+': '+str(e),traceback=traceback.format_exc())
  create_json(root/'reader.json',result)
  raw=freeze(root,[p for p in root.rglob('*') if p.is_file() and p.name!='raw-evidence.json']+[p for p in (B/leg/'01').rglob('*') if p.is_file()])
  row.update(outcome(leg,row,protected,upload,result,crashes=row.get('new_crash_report_folders',[]),raw=raw))
  prior=sum(bool(json.loads(p.read_text()).get('crashed')) for p in (S/'legs').glob('*/*/result.json'))
  row=crash_policy(row,prior);create_json(root/'result.json',row)
  print(json.dumps(dict(leg=leg,result=row['result'],reason=result.get('reason'),frames=result.get('frames'),positive=result.get('positive_frames'),pixel_gate=result.get('pixel_gate'))),flush=True)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('action',choices=['arm','run']);p.add_argument('--leg',choices=SPECS);a=p.parse_args()
 if a.action=='arm':arm(a.leg)
 else:run()
