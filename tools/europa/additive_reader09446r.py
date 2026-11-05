import collections,json
from pathlib import Path

def kind(v):
    if isinstance(v,bool):return 'boolean'
    if isinstance(v,(float,int)):return 'number'
    if isinstance(v,str):return 'string'
    if isinstance(v,list):return 'array'
    if isinstance(v,dict):return 'object'
    return 'null'

def compare(a,b,filename):
    errors=[];new=[];changed=collections.Counter();checked=0
    def walk(x,y,path):
        nonlocal checked
        checked+=1
        if kind(x)!=kind(y):errors.append(dict(path=path,reason='type changed',old=kind(x),new=kind(y)));return
        if isinstance(x,dict):
            for key in x:
                if key not in y:errors.append(dict(path=path+'/'+key,reason='key removed'))
                else:walk(x[key],y[key],path+'/'+key)
            new.extend(path+'/'+key for key in y if key not in x)
        elif isinstance(x,list):
            if len(y)<len(x):errors.append(dict(path=path,reason='array shortened',old=len(x),new=len(y)))
            for i,(xx,yy) in enumerate(zip(x,y)):walk(xx,yy,path+'/'+str(i))
        elif x!=y:changed[path.rsplit('/',1)[-1]]+=1
    walk(a,b,filename)
    return dict(result='PASS' if not errors else 'FAIL',values_checked=checked,errors=errors,added_keys=new,changed_values_information_only=dict(changed))

def read(old,new):
    old,new=Path(old),Path(new);results=[];missing=[]
    old_names={p.name for p in old.iterdir() if p.is_file()};new_names={p.name for p in new.iterdir() if p.is_file()}
    for name in sorted(old_names):
        if name not in new_names:missing.append(name);continue
        if not name.endswith(('.json','.jsonl')):continue
        a=(old/name).read_text();b=(new/name).read_text()
        aa=[json.loads(s) for s in a.splitlines()] if name.endswith('.jsonl') else json.loads(a)
        bb=[json.loads(s) for s in b.splitlines()] if name.endswith('.jsonl') else json.loads(b)
        results.append(compare(aa,bb,name))
    return dict(result='PASS' if not missing and all(v['result']=='PASS' for v in results) else 'FAIL',rule='GO3: every old output file and recursively every key retained, no JSON type changed. Existing values compared and reported as information only.',old=str(old),new=str(new),missing_files=missing,added_files=sorted(new_names-old_names),files=results)

def selftest():
    assert compare({'count':4},{'count':4,'whole_frame':False},'x')['result']=='PASS'
    assert compare({'count':4},{},'x')['result']=='FAIL'
    assert compare({'count':4},{'count':'4'},'x')['result']=='FAIL'
    assert compare({'count':4},{'count':5},'x')['result']=='PASS'
    assert compare({'count':4},{'count':False},'x')['result']=='FAIL'
    assert compare([{'a':True}],[{}],'x')['result']=='FAIL'
    return dict(result='PASS',added_key_positive=True,removed_key_canfail=True,changed_type_canfail=True,bool_number_canfail=True,nested_removed_key_canfail=True,changed_values_reported=True)
