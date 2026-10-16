import json
from pathlib import Path
import write_ledger as ledger
import process_wrapper as wrapper

def main():
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
            output=ledger.JOB/'routing/prepared'/host/mode
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
    ledger.dump(ledger.JOB/'routing/prepared-commands.json',{'execute':False,'recipes':recipes})
    ledger.dump(ledger.JOB/'routing/tooling-self-tests.json',{'result':'PASS','cases':cases,'mutations':'None outside owned D receipt directories; runtime recipes were not executed.'})
    print(json.dumps({'tooling_self_tests':'PASS','cases':len(cases),'runtime_launched':False}))

if __name__=='__main__':main()
