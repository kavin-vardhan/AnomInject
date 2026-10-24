import argparse
import json
from pathlib import Path
import write_ledger as ledger
import process_wrapper as wrapper

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    output_root = args.output.resolve()
    if not output_root.is_relative_to(ledger.ROOT.resolve(strict=True)):
        raise RuntimeError('Self-test output must be in owned D scratch')
    output_root.mkdir(parents=True, exist_ok=False)
    cases=[]
    def check(name,condition):
        cases.append({'case':name,'pass':bool(condition)})
        if not condition:raise AssertionError(name)
    for scope,path,expected in [
        ('engine51','D:/UESource/UnrealEngine/Engine/Binaries/Win64/probe.dll','STOP'),
        ('engine425','E:/EpicGames/UE_4.25/Engine/Binaries/Win64/probe.dll','STOP'),
        ('engine425','E:/EpicGames/UE_4.25/Engine/Intermediate/Build/BuildRules/probe.dll','port_E_exception'),
        ('paris_original','E:/Unreal Projects/StylizedParisStreet/probe','STOP'),
        ('unreal_engine','C:/Users/kavin/AppData/Local/UnrealEngine/XmlConfigCache-test.bin','port_C_exception'),
        ('unreal_engine','C:/Users/kavin/AppData/Local/UnrealEngine/4.25/probe','port_C_exception'),
        ('unreal_engine','C:/Users/kavin/AppData/Local/UnrealEngine/5.1/probe','STOP'),
        ('unreal_engine','C:/Users/kavin/AppData/Local/UnrealEngine/5.7/probe','foreign'),
        ('unreal_engine','C:/Users/kavin/AppData/Local/UnrealEngine/Editor/FilterBar.json','foreign'),
        ('ubt','C:/Users/kavin/AppData/Local/UnrealBuildTool/Log.txt','port_C_exception'),
        ('ubt','C:/Users/kavin/AppData/Local/UnrealBuildTool/settings.xml','STOP'),
        ('ue_temp_0','C:/ClaudeTemp/UnrealBuildTool/probe','STOP'),
        ('c_root','C:/port-leak.tmp','STOP'),
        ('e_root','E:/port-leak.tmp','STOP'),
        ('tool_codex','E:/CodexData/home/tool-output','tooling'),
    ]:
        check(scope+':'+path,ledger.classification(scope,path,'ubt')[0]==expected)
    recipes={}
    for host in wrapper.HOSTS:
        recipes[host]={}
        for mode in ('json','build','editor','game','uat'):
            output=output_root/'prepared'/host/mode
            if host == '51base' and mode == 'build':
                try:
                    wrapper.command(host,mode,'StackOBotEditor','Development',output)
                except RuntimeError:
                    check('Baseline51-build-rejected', True)
                else:
                    check('Baseline51-build-rejected', False)
                continue
            args=wrapper.command(host,mode,wrapper.HOSTS[host][0].stem+'Editor' if mode!='game' else wrapper.HOSTS[host][0].stem,'Development',output)
            recipes[host][mode]=args
            if mode in ('json','build'):check(host+mode+'-D-log',any(x.startswith('-Log=D:') for x in args))
            else:
                text=' '.join(args)
                check(host+mode+'-runtime-redirects',all(x in text for x in ('-UserDir=D:','-ShaderWorkingDir=D:','-DDC=Europa','-abslog=D:','-AbsCrashReportClientLog=D:')))
    try:
        wrapper.command('425','json','StylizedParisStreet','Development',Path('E:/rejected-europa-output'))
    except RuntimeError:check('non-D-output-rejected-before-mkdir',True)
    else:check('non-D-output-rejected-before-mkdir',False)
    ledger.dump(output_root/'prepared-commands.json',{'execute':False,'recipes':recipes})
    ledger.dump(output_root/'tooling-self-tests.json',{'result':'PASS','cases':cases,'mutations':'None outside owned D receipt directories; runtime recipes were not executed.'})
    print(json.dumps({'tooling_self_tests':'PASS','cases':len(cases),'runtime_launched':False}))

if __name__=='__main__':main()
