import datetime,json,re,subprocess,sys
from pathlib import Path
J=Path('D:/IA_Europa/Receipts/09446s');B=Path('E:/IA_BuildCache/09446s-evidence')
leg='D3D12-1279-default';root=B/leg/'01/plugin-frames';session=J/'H4'/leg/'01/session'
assert json.loads((J/'H4'/leg/'01/result.json').read_text())['result']=='PASS'
card=Path('D:/IntrusiveAnomalies/_reviews/094-45r-office-card-europa.md')
text=card.read_text(encoding='utf-8');start=text.index('13. **');end=text.index('### 5.1',start)
blocks=re.findall(r'```[^\n]*\n(.*?)```',text[start:end],re.S)
commands=[line.strip() for block in blocks for line in block.splitlines() if line.strip()]
assert len(commands)>=20 and all(c.startswith(('findstr ','find /c ','dir /b ','if exist ','powershell -NoProfile -Command ')) for c in commands)
out=J/'card-reads';out.mkdir(exist_ok=False);results=[]
for i,command in enumerate(commands):
    line=command.replace('<Out>\\Runtime.log',str(session/'Runtime.log')).replace('<Out>',str(root))
    path=out/('read-%02d.bat'%i);path.write_text('@echo off\n'+line+'\n',encoding='utf-8')
    r=subprocess.run(['cmd.exe','/d','/c',str(path)],cwd=out,capture_output=True,text=True,creationflags=subprocess.CREATE_NO_WINDOW|subprocess.BELOW_NORMAL_PRIORITY_CLASS)
    results.append(dict(command=command,substituted=line,exit_code=r.returncode,stdout=r.stdout,stderr=r.stderr))
    (out/('read-%02d.txt'%i)).write_text(r.stdout+r.stderr)
value=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),card=str(card),commands=results,substitutions=dict(capture_root=str(root),runtime_log=str(session/'Runtime.log')),result='PASS' if all(not r['stderr'] for r in results) else 'FAIL',note='The home EA_Core bench uses the ruled0.5% floor, so the office6% line is expected0. Zero-count find returns1; the complete stdout is retained.')
(J/'card-reads.json').write_text(json.dumps(value,indent=2));print(json.dumps(value))
