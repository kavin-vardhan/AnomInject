import argparse
import hashlib
import json
import os
from pathlib import Path
from environment import filtered, masked
from runtime_attribution import validate_redirects

ROOT=Path('D:/IA_Europa')
HOSTS={
 '425':(ROOT/'StylizedParisStreet/StylizedParisStreet.uproject',Path('E:/EpicGames/UE_4.25'),'Europa425DDC','UE4Editor.exe'),
 '51base':(ROOT/'Baseline51/StackOBot/StackOBot.uproject',Path('D:/UESource/UnrealEngine'),'Europa51BaseDDC','UnrealEditor.exe'),
 '51port':(ROOT/'ReGate51/StackOBot/StackOBot.uproject',Path('D:/UESource/UnrealEngine'),'Europa51PortDDC','UnrealEditor.exe')}

def owned(path):
    if not path.is_absolute() or not path.resolve().is_relative_to(ROOT.resolve()):
        raise RuntimeError('Path must resolve inside physical-D Europa scratch')
    return path

def prepare(host, output, probe=False, audit=False, authoring=False):
    owned(output)
    project,engine,graph,exe=HOSTS[host]
    owned(project)
    if audit and not probe: raise RuntimeError('Audit requires the probe')
    argv=[str(engine/'Engine/Binaries/Win64'/exe),str(project),
          '-UserDir='+project.parent.as_posix()+'/',
          '-ShaderWorkingDir='+(ROOT/'Caches'/host/'ShaderWorking').as_posix()+'/',
          '-DDC='+graph,'-abslog='+(output/'Runtime.log').as_posix(),
          '-AbsCrashReportClientLog='+(output/'CrashClient.log').as_posix(),
          '-NoLiveCoding','-NoHotReload','-windowed','-ResX=1280','-ResY=720']
    if probe: argv+=['-IAIBench','-EuropaPhaseA','-EuropaProbeOut='+(output/'probe').as_posix()]
    if audit: argv+=['-EuropaTickAudit']
    if authoring: argv+=['-EnablePlugins=PythonScriptPlugin,EditorScriptingUtilities']
    validate_redirects(argv,graph)
    return argv

def selftest():
    cases=[]
    for host in HOSTS:
        a=prepare(host,ROOT/'Receipts/synthetic-runtime',True,True,True)
        assert '-IAIBench' in a and not any('ExecutePythonScript' in x for x in a)
        cases.append(dict(case=host,pass_=True))
    for path,probe,audit in [(Path('C:/rejected'),True,True),(ROOT/'Receipts/x',False,True)]:
        try: prepare('425',path,probe,audit)
        except RuntimeError: cases.append(dict(case=str(path)+'_audit_'+str(audit),pass_=True))
        else: raise AssertionError('Invalid runtime preparation accepted')
    return dict(result='PASS',cases=cases,launches=0)

def main():
    p=argparse.ArgumentParser()
    p.add_argument('--job-config',type=Path,required=True)
    p.add_argument('--host',choices=HOSTS,default='425')
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--probe',action='store_true');p.add_argument('--tick-audit',action='store_true')
    p.add_argument('--authoring-plugins',action='store_true');p.add_argument('--self-test',action='store_true')
    args=p.parse_args()
    owned(args.output);owned(args.job_config)
    if args.output.exists(): raise RuntimeError('Fresh output required')
    scope=json.loads(args.job_config.read_text())
    if not args.output.resolve().is_relative_to(owned(Path(scope['job_root'])).resolve()): raise RuntimeError('Output outside job')
    args.output.mkdir(parents=True)
    if args.self_test: result=selftest()
    else:
        argv=prepare(args.host,args.output,args.probe,args.tick_audit,args.authoring_plugins)
        project=HOSTS[args.host][0]
        files=[Path(argv[0]),project]
        for folder in (project.parent/'Binaries',project.parent/'Plugins/AnomalyInjector/Binaries',project.parent/'Plugins/CaptureBench/Binaries'):
            if folder.exists(): files+=sorted(x for x in folder.rglob('*') if x.is_file())
        hashes=[dict(path=str(x),size=x.stat().st_size,sha256=hashlib.sha256(x.read_bytes()).hexdigest()) for x in files]
        env=filtered(os.environ)
        env.update(TEMP=str(ROOT/'Temp'/Path(scope['job_root']).name/'runtime'),TMP=str(ROOT/'Temp'/Path(scope['job_root']).name/'runtime'),
                   GIT_OPTIONAL_LOCKS='0',PYTHONDONTWRITEBYTECODE='1')
        result=dict(result='PREPARED_ONLY',argv=argv,environment=masked(env),
                    runtime_env_redirects={k:env[k] for k in ('TEMP','TMP')},binary_and_project_hashes=hashes,
                    scope_sha256=hashlib.sha256(args.job_config.read_bytes()).hexdigest(),launches=0,
                    admission='A later authorized runner must verify all hashes, idle admission and scoped ledger before execution; this tool has no execute mode.')
    (args.output/'prepared.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(result=result['result'],launches=0,receipt=str(args.output/'prepared.json'))))

if __name__=='__main__': main()
