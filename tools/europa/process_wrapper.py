import argparse
import json
import os
from pathlib import Path
import sys
from write_ledger import ROOT,JOB,dump,run_child,capture_child

HOSTS={'425':(ROOT/'StylizedParisStreet/StylizedParisStreet.uproject',Path('E:/EpicGames/UE_4.25'),'Europa425DDC'),
 '51base':(ROOT/'Baseline51/StackOBot/StackOBot.uproject',Path('D:/UESource/UnrealEngine'),'Europa51BaseDDC'),
 '51port':(ROOT/'ReGate51/StackOBot/StackOBot.uproject',Path('D:/UESource/UnrealEngine'),'Europa51PortDDC')}

def command(host,mode,target,configuration,output):
    if mode == 'build' and (host, target, configuration) not in (
        ('425', 'StylizedParisStreetEditor', 'Development'), ('51port', 'StackOBotEditor', 'Development')):
        raise RuntimeError('A3 build mode admits only owned 4.25 and ReGate51 Editor Development targets')
    project,engine,graph=HOSTS[host]
    if not output.resolve().is_relative_to(ROOT.resolve()):raise RuntimeError('Output must resolve under physical-D Europa root')
    output.mkdir(parents=True,exist_ok=True)
    user=project.parent if mode=='editor' else ROOT/'Runtime'/host/output.name
    runtime=['-UserDir='+user.as_posix()+'/', '-ShaderWorkingDir='+(ROOT/'Caches'/host/'ShaderWorking').as_posix()+'/', '-DDC='+graph, '-abslog='+(output/'Runtime.log').as_posix(),'-AbsCrashReportClientLog='+(output/'CrashClient.log').as_posix()]
    ubt=engine/('Engine/Binaries/DotNET/UnrealBuildTool.exe' if host=='425' else 'Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe')
    if mode in ('json','build'):
        args=[str(ubt),target,'Win64',configuration,'-Project='+str(project),'-Log='+str(output/'UBT.log'),'-NoHotReload','-NoEngineChanges','-Verbose']
        if mode=='json':args+=['-Mode=JsonExport','-OutputFile='+str(output/'target.json')]
    elif mode=='editor':args=[str(engine/('Engine/Binaries/Win64/UE4Editor.exe' if host=='425' else 'Engine/Binaries/Win64/UnrealEditor.exe')),str(project),*runtime]
    elif mode=='game':args=[str(project.parent/'Binaries/Win64'/(project.stem+('.exe' if configuration=='Development' else '-Win64-'+configuration+'.exe'))),*runtime]
    else:
        args=[str(engine/'Engine/Build/BatchFiles/RunUAT.bat'),'BuildCookRun','-project='+str(project),'-noP4','-platform=Win64','-clientconfig='+configuration,'-build','-cook','-stage','-pak','-archive','-archivedirectory='+str(ROOT/'Packages'/host/output.name),'-stagingdirectory='+str(ROOT/'Stage'/host/output.name),'-AdditionalCookerOptions='+' '.join(runtime)]
    return args

def run_json_batch(host,pairs,label):
    project,engine,graph=HOSTS[host]
    port=ROOT/'StylizedParisStreet/Plugins/AnomalyInjector'
    prediction='docs/predictions/094-europa-phase-a.md'
    if not (port/prediction).is_file():raise RuntimeError('Predictions not present')
    run_child(['git','-C',str(port),'diff','--exit-code','HEAD','--',prediction])
    prediction_commit=capture_child(['git','-C',str(port),'log','-1','--format=%H %cI','HEAD','--',prediction]).strip()
    if not prediction_commit:raise RuntimeError('Predictions are not committed')
    folder=Path(os.environ['EUROPA_OPERATION_DIR'])
    output=JOB/'matrices'/label
    output.mkdir(parents=True,exist_ok=False)
    os.environ['EUROPA_DESCRIPTOR_PROBE']=str(output/'descriptor-predicates.json')
    sample=command(host,'json',pairs[0][0],pairs[0][1],output)
    argv=[sample[0],'-Mode=JsonExport','-Log='+str(output/'UBT.log'),'-Verbose','-NoEngineChanges','-NoHotReload']
    for target,config in pairs:
        arg=[target,'Win64',config,'-Project='+str(project),'-OutputFile='+str(output/(target+'-'+config+'.json'))]
        argv.append('-Target='+' '.join(arg))
    if host!='425':
        os.environ['DOTNET_ROOT']=str(engine/'Engine/Binaries/ThirdParty/DotNet/6.0.302/windows')
        os.environ['DOTNET_MULTILEVEL_LOOKUP']='0'
    dump(output/'invocation.json',{'host':host,'engine_root':str(engine),'project':str(project),'mode':'JsonExport','argv':argv,'prediction_commit_and_time':prediction_commit,'exec_code_gen_actions':False,'expected_Cxx_actions':0,'expected_engine_actions':0,'runtime_redirects_not_applicable':True,'environment_in_child_receipt':str(folder)})
    run_child(argv,cwd=project.parent)

def main():
    p=argparse.ArgumentParser();p.add_argument('--host',choices=HOSTS,required=True);p.add_argument('--mode',choices=['json','build','editor','game','uat'],default='json');p.add_argument('--target');p.add_argument('--configuration',default='Development');p.add_argument('--output',type=Path,required=True);p.add_argument('--execute',action='store_true');p.add_argument('--batch-json',type=Path);a=p.parse_args()
    if a.batch_json:
        data=json.loads(a.batch_json.read_text());run_json_batch(a.host,data['pairs'],data['label']);return
    target=a.target or (HOSTS[a.host][0].stem+'Editor')
    if a.execute and a.mode == 'build':
        if (a.host, target, a.configuration) not in (('425', 'StylizedParisStreetEditor', 'Development'), ('51port', 'StackOBotEditor', 'Development')):
            raise RuntimeError('A3 build target/configuration is not admitted')
        if a.output.resolve() != (JOB/'builds'/a.output.name).resolve():
            raise RuntimeError('A3 build output must be a labelled folder in this job builds receipt directory')
        from working_build import main as build_main
        sys.argv = [sys.argv[0], '--host', a.host, '--label', a.output.name]
        build_main()
        return
    argv=command(a.host,a.mode,target,a.configuration,a.output)
    dump(a.output/'command.json',{'argv':argv,'mode':a.mode,'host':a.host,'execute':a.execute})
    if a.execute:
        if a.mode!='json':raise RuntimeError('09403 admits only JsonExport; other recipes are prepared for later authorized jobs')
        run_child(argv)
    else:print(json.dumps({'mode':'prepared only','argv':argv}))

if __name__=='__main__':main()
