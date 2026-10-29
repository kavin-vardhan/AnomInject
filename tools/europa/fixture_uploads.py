import hashlib
from pathlib import Path
import re

SEND = re.compile(r'(?i)(\bPOST\b|\b(?:uploading|uploaded)\b|\b(?:sending|sent)\s+(?:report|data|analytics|session)|\bdata.?router\b.*\b(?:send|upload)\b)')


def check(session):
    rows=[]
    findings=[]
    for name in ('Runtime.log','CrashClient.log'):
        path=Path(session)/name
        if not path.is_file():
            rows.append(dict(path=str(path),exists=False))
            continue
        data=path.read_bytes()
        lines=data.decode('utf-8',errors='replace').splitlines()
        hits=[dict(line=i,text=line) for i,line in enumerate(lines,1) if SEND.search(line)]
        rows.append(dict(path=str(path),exists=True,size=len(data),sha256=hashlib.sha256(data).hexdigest(),hits=hits))
        findings.extend(hits)
    return dict(result='PASS' if all(r['exists'] for r in rows) and not findings else 'FAIL',logs=rows,
        scope='Both session logs; upload/send/POST evidence. Absence is not network packet capture.')
