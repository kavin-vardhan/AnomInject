import argparse
import datetime as dt
import fnmatch
import json
import os
from pathlib import Path
import shutil
import sqlite3
import subprocess
import sys
import threading
import time

ROOT=Path('D:/IA_Europa')
JOB=ROOT/'Receipts/09403'
LEDGER=JOB/'ledger'
USER=Path('C:/Users/kavin')
LOCAL=USER/'AppData/Local'
GIB=1024**3

def now():return dt.datetime.now(dt.timezone.utc).isoformat()

def dump(p,x):
    p.parent.mkdir(parents=True,exist_ok=True)
    p.write_text(json.dumps(x,indent=2)+'\n',encoding='utf-8')

def floors(start=False):
    data={d:{'resolved':str(Path(d+':/').resolve(strict=True)),'free':shutil.disk_usage(d+':/').free,'floor':(10 if d=='C' else 50)*GIB,'warning':(11 if d=='C' else 55)*GIB} for d in 'CDE'}
    if any(v['free']<v['floor'] for v in data.values()):raise RuntimeError('EA15 floor breach: '+json.dumps(data))
    if start and (data['C']['free']<13*GIB or data['E']['free']<52*GIB):raise RuntimeError('EB2 headroom')
    return data

def specs():
    items=[
      ('engine51','D:/UESource/UnrealEngine','tree',True),
      ('engine425','E:/EpicGames/UE_4.25','tree',True),
      ('paris_original','E:/Unreal Projects/StylizedParisStreet','tree',True),
      ('unreal_engine',str(LOCAL/'UnrealEngine'),'tree',False),
      ('ubt',str(LOCAL/'UnrealBuildTool'),'tree',False),
      ('uht',str(LOCAL/'UnrealHeaderTool'),'tree',False),
      ('crashclient',str(LOCAL/'CrashReportClient'),'tree',False),
      ('epic','C:/ProgramData/Epic','tree',False),
      ('c_root','C:/','top',False),('e_root','E:/','top',False),
      ('windows','C:/Windows','tree',False),
      ('wer_system','C:/ProgramData/Microsoft/Windows/WER','tree',False),
      ('wer_user',str(LOCAL/'Microsoft/Windows/WER'),'tree',False),
      ('gpu_nvidia',str(LOCAL/'NVIDIA'),'tree',False),
      ('gpu_nvidia_program','C:/ProgramData/NVIDIA Corporation/NV_Cache','tree',False),
      ('gpu_dx',str(LOCAL/'D3DSCache'),'tree',False),
      ('gpu_amd',str(LOCAL/'AMD'),'tree',False),
      ('tool_claude',str(USER/'.claude'),'tree',False),
      ('tool_codex',str(USER/'.codex'),'tree',False),
      ('tool_temp','C:/ClaudeTemp','tree',False),
      ('port',str(ROOT),'tree',True)]
    for i,p in enumerate(dict.fromkeys([os.environ.get('TEMP','C:/ClaudeTemp'),str(LOCAL/'Temp'),'C:/Windows/Temp'])):
        items.append(('ue_temp_'+str(i),p,'ue_temp',False))
    return items

def snapshot(folder,label):
    print('LEDGER '+label+' begin '+now(),flush=True)
    database=folder/(label+'.sqlite')
    con=sqlite3.connect(database)
    con.execute('PRAGMA journal_mode=MEMORY')
    con.execute('CREATE TABLE files(scope TEXT,path TEXT,size INTEGER,mtime INTEGER,PRIMARY KEY(scope,path)) WITHOUT ROWID')
    errors=[]; totals={}; links=[]
    for name,raw,mode,strict in specs():
        root=Path(raw)
        if not root.exists():
            totals[name]={'path':raw,'present':False,'files':0,'bytes':0};continue
        physical=root.resolve(strict=True)
        count=0;size=0;batch=[]
        stack=[physical];visited=set()
        while stack:
            directory=stack.pop()
            key=str(directory).casefold()
            if key in visited:continue
            visited.add(key)
            if name=='port' and directory==LEDGER:continue
            try:
                with os.scandir(directory) as entries:
                    for entry in entries:
                        p=Path(entry.path)
                        if mode=='ue_temp' and directory==physical and not any(fnmatch.fnmatch(entry.name.lower(),pat) for pat in ('unrealshaderworkingdir*','unrealbuildtool*','ue4*','unreal*','crashreportclient*')):continue
                        try:
                            s=entry.stat(follow_symlinks=False)
                            if s.st_file_attributes & 1024:
                                target=p.resolve(strict=True)
                                links.append({'scope':name,'path':str(p),'target':str(target)})
                                if entry.is_dir():
                                    if strict:raise RuntimeError('Unexpected link in strict ledger scope: '+str(p))
                                    continue
                            if entry.is_dir(follow_symlinks=False):
                                if mode!='top':stack.append(p)
                            elif entry.is_file():
                                batch.append((name,str(p),s.st_size,s.st_mtime_ns));count+=1;size+=s.st_size
                                if len(batch)>=4000:
                                    con.executemany('INSERT INTO files VALUES(?,?,?,?)',batch);batch=[]
                        except OSError as e:
                            if strict:raise
                            errors.append({'scope':name,'path':str(p),'error':str(e)})
            except OSError as e:
                if strict:raise
                errors.append({'scope':name,'path':str(directory),'error':str(e)})
        if batch:con.executemany('INSERT INTO files VALUES(?,?,?,?)',batch)
        con.commit()
        totals[name]={'path':raw,'resolved':str(physical),'present':True,'files':count,'bytes':size}
        print(json.dumps({'ledger':label,'scope':name,'files':count,'bytes':size,'errors':sum(e['scope']==name for e in errors)}),flush=True)
        floors()
    con.close()
    result={'time_utc':now(),'totals':totals,'access_errors':errors,'links':links,'disks':floors()}
    dump(folder/(label+'.json'),result)
    return result

def classification(scope,path,port_kind):
    p=path.lower().replace('\\','/')
    if scope=='port':return 'port_D','owned scratch/host path'
    if scope.startswith('tool_'):return 'tooling','orchestration overhead, EB1'
    if scope=='engine51':return 'STOP','5.1 engine tree mutation'
    if scope=='engine425':
        if '/engine/intermediate/build/buildrules/' in p:return 'port_E_exception','EA4 installed rules cache'
        return 'STOP','4.25 engine mutation outside BuildRules'
    if scope=='paris_original':return 'STOP','Paris original changed'
    if scope=='unreal_engine':
        suffix=p.split('/unrealengine/',1)[-1]
        if suffix.startswith(('5.7/','common/','zen/')) or 'ue_5.7' in suffix:return 'foreign','owner UE 5.7 / shared Zen activity; port DDC redirected'
        if suffix.startswith('editor/'):return 'foreign','shared editor settings; A1/A2 launches no editor, with child receipts retained'
        if suffix.startswith('4.25/') or (suffix.startswith('xmlconfigcache-') and suffix.endswith('.bin')):return 'port_C_exception','EA4 EngineUserDir / XML cache, conservatively charged'
        if port_kind=='read_only':return 'foreign','no Unreal child in this operation'
        return 'STOP','unclassified UnrealEngine location'
    if scope in ('ubt','uht'):
        if port_kind=='read_only':return 'foreign','no UE/UBT/runtime child in this operation'
        if Path(path).suffix.lower() in ('.log','.txt','.json') and Path(path).name.lower().startswith(('log','unreal')):return 'port_C_exception','EA4 UBT/UHT log, conservatively charged'
        return 'STOP','UBT/UHT data outside the named log exception'
    if scope=='crashclient':
        if port_kind=='read_only':return 'foreign','no UE/UBT/runtime child in this operation'
        return 'port_C_exception','EA4 monitor exception, conservatively charged'
    if scope.startswith('gpu_'):return 'foreign','A1/A2 do not launch a renderer, game, cook or shader worker'
    if scope.startswith('ue_temp_'):
        if 'crashreportclient' in p:
            return ('foreign','no runtime child') if port_kind=='read_only' else ('port_C_exception','EA4 monitor temp')
        if port_kind=='read_only':return 'foreign','no Unreal child; child TEMP/TMP routed to D'
        return 'STOP','UE temp in inherited foreign TEMP root; port TEMP/TMP are D'
    if scope in ('windows','wer_system','wer_user'):return 'foreign','Windows-managed activity; declared targeted exception inventory'
    if scope=='epic':return 'foreign','Epic launcher/services; port does not launch Epic or use its data'
    if scope in ('c_root','e_root'):
        if Path(path).name.lower() in ('pagefile.sys','swapfile.sys','hiberfil.sys','dumpstack.log.tmp','dumpstack.log'):return 'foreign','Windows-managed paging/crash file'
        if port_kind=='read_only':return 'foreign','read-only child and only explicit D outputs'
        return 'STOP','unclassified drive-root mutation'
    return 'STOP','unclassified path'

def finish(folder,kind):
    after=snapshot(folder,'after')
    con=sqlite3.connect(folder/'before.sqlite')
    con.execute('ATTACH DATABASE ? AS aft',(str(folder/'after.sqlite'),))
    query="""SELECT a.scope,a.path,b.size,a.size,b.mtime,a.mtime FROM aft.files a LEFT JOIN main.files b ON a.scope=b.scope AND a.path=b.path WHERE b.path IS NULL OR a.size!=b.size OR a.mtime!=b.mtime UNION ALL SELECT b.scope,b.path,b.size,NULL,b.mtime,NULL FROM main.files b LEFT JOIN aft.files a ON a.scope=b.scope AND a.path=b.path WHERE a.path IS NULL"""
    changes=[];classes={};charged={'C':{},'E':{}};stops=[]
    for scope,path,bsize,asize,btime,atime in con.execute(query):
        c,why=classification(scope,path,kind)
        item={'scope':scope,'path':path,'before_size':bsize,'after_size':asize,'before_mtime_ns':btime,'after_mtime_ns':atime,'classification':c,'basis':why}
        changes.append(item);classes[c]=classes.get(c,0)+1
        if c=='STOP':stops.append(item)
        if c in ('port_C_exception','port_E_exception'):
            drive='C' if c=='port_C_exception' else 'E'
            charged[drive][path.casefold()]=max(0,(asize or 0)-(bsize or 0))
    con.close()
    current={d:sum(v.values()) for d,v in charged.items()}
    previous={d:0 for d in 'CE'}
    for p in LEDGER.glob('*/summary.json'):
        r=json.loads(p.read_text());
        for d in 'CE':previous[d]+=r['exception_growth_charged'][d]
    cumulative={d:previous[d]+current[d] for d in 'CE'}
    for d,limit in [('C',250_000_000),('E',50_000_000)]:
        if cumulative[d]>limit:stops.append({'budget':d,'bytes':cumulative[d],'limit':limit})
    dump(folder/'changes.json',changes)
    before=json.loads((folder/'before.json').read_text())
    result={'result':'STOP' if stops else 'PASS','operation':folder.name,'kind':kind,'time_utc':now(),'classification_counts':classes,'exception_growth_charged':current,'cumulative_job_exception_growth':cumulative,'stops':stops,'disks_before':before['disks'],'disks_after':after['disks'],'port_D_bytes_before':before['totals']['port']['bytes'],'port_D_bytes_after':after['totals']['port']['bytes'],'tooling_before':{k:v for k,v in before['totals'].items() if k.startswith('tool_')},'tooling_after':{k:v for k,v in after['totals'].items() if k.startswith('tool_')},'scan_access_errors_before':len(before['access_errors']),'scan_access_errors_after':len(after['access_errors']),'scope_note':'Per-operation metadata inventories and before/after changes; mtime identifies the in-window files. Access-denied OS/tooling entries are listed, never fabricated as empty. Protected scope errors are fatal.'}
    dump(folder/'summary.json',result)
    print(json.dumps(result),flush=True)
    if stops:raise RuntimeError('EB1/EA15 ledger STOP')
    return result

def child_env(folder):
    env=os.environ.copy()
    for key in ('GIT_AUTHOR_DATE','GIT_COMMITTER_DATE'):
        if key in env:raise RuntimeError('W4: '+key)
    temp=ROOT/'Temp/09403'/folder.name
    temp.mkdir(parents=True,exist_ok=True)
    routing={'TEMP':str(temp),'TMP':str(temp),'uebp_EngineSavedFolder':str(temp/'UAT/Saved'),'uebp_LogFolder':str(temp/'UAT/Working'),'uebp_FinalLogFolder':str(temp/'UAT/Final'),'DOTNET_CLI_HOME':str(temp/'DotNet'),'DOTNET_CLI_TELEMETRY_OPTOUT':'1','DOTNET_SKIP_FIRST_TIME_EXPERIENCE':'1','DOTNET_NOLOGO':'1','NUGET_PACKAGES':str(temp/'NuGet'),'GIT_OPTIONAL_LOCKS':'0','PYTHONDONTWRITEBYTECODE':'1','EUROPA_OPERATION_DIR':str(folder)}
    env.update(routing)
    for name in ('DOTNET_ROOT','DOTNET_MULTILEVEL_LOOKUP','EUROPA_DESCRIPTOR_PROBE'):
        if name in env:routing[name]=env[name]
    return env,routing

def run_child(argv,cwd=None,acceptable=(0,)):
    folder=Path(os.environ['EUROPA_OPERATION_DIR'])
    lognum=len(list(folder.glob('child-*.json')))+1
    env,routing=child_env(folder)
    receipt={'argv':list(map(str,argv)),'windows_command_line':subprocess.list2cmdline(list(map(str,argv))),'cwd':str(cwd or Path.cwd()),'started_utc':now(),'environment':routing,'W4':{x:None for x in ('GIT_AUTHOR_DATE','GIT_COMMITTER_DATE')}}
    path=folder/f'child-{lognum:03}.json';dump(path,receipt)
    proc=subprocess.Popen(argv,cwd=cwd,env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,encoding='utf-8',errors='replace',bufsize=1)
    receipt['pid']=proc.pid
    dump(path,receipt)
    def consume():
        with (folder/f'child-{lognum:03}.log').open('w',encoding='utf-8') as output:
            for line in proc.stdout:
                output.write(line);output.flush();print(line.rstrip(),flush=True)
    thread=threading.Thread(target=consume);thread.start()
    try:
        while True:
            try:code=proc.wait(timeout=20);break
            except subprocess.TimeoutExpired:floors()
    except BaseException:
        proc.terminate();proc.wait();raise
    finally:thread.join()
    receipt.update({'ended_utc':now(),'exit_code':code});dump(path,receipt)
    if code not in acceptable:raise RuntimeError('Child exit '+str(code)+': '+receipt['windows_command_line'])
    return code

def capture_child(argv,cwd=None):
    folder=Path(os.environ['EUROPA_OPERATION_DIR'])
    lognum=len(list(folder.glob('child-*.json')))+1
    run_child(argv,cwd)
    return (folder/f'child-{lognum:03}.log').read_text(encoding='utf-8')

def main():
    ap=argparse.ArgumentParser();ap.add_argument('operation');ap.add_argument('--kind',choices=['read_only','setup','ubt'],default='setup');ap.add_argument('command',nargs=argparse.REMAINDER);a=ap.parse_args()
    argv=a.command
    if argv and argv[0]=='--':argv=argv[1:]
    if not argv:raise RuntimeError('No child command')
    floors(start=a.operation=='01-entry')
    folder=LEDGER/a.operation;folder.mkdir(parents=True,exist_ok=False)
    dump(folder/'operation.json',{'operation':a.operation,'kind':a.kind,'started_utc':now(),'command':argv})
    snapshot(folder,'before')
    os.environ['EUROPA_OPERATION_DIR']=str(folder)
    failure=None
    try:run_child(argv)
    except BaseException as e:failure=str(e);dump(folder/'child-failure.json',{'error':failure})
    finish(folder,a.kind)
    if failure:raise RuntimeError(failure)

if __name__=='__main__':main()
