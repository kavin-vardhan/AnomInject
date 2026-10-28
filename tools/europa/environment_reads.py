import argparse
import ast
import json
from pathlib import Path

from environment import filtered


def inspect_source(text, name):
    tree=ast.parse(text)
    os_names={'os'}
    env_names=set()
    getter_names=set()
    constants={}
    for node in ast.walk(tree):
        if isinstance(node,ast.Import):
            os_names.update(a.asname or a.name for a in node.names if a.name=='os')
        if isinstance(node,ast.ImportFrom) and node.module=='os':
            env_names.update(a.asname or a.name for a in node.names if a.name=='environ')
            getter_names.update(a.asname or a.name for a in node.names if a.name=='getenv')
        if isinstance(node,ast.Assign) and isinstance(node.value,ast.Constant) and isinstance(node.value.value,str):
            for target in node.targets:
                if isinstance(target,ast.Name):constants[target.id]=node.value.value
    def is_env(node):
        return (isinstance(node,ast.Name) and node.id in env_names or
            isinstance(node,ast.Attribute) and node.attr=='environ' and isinstance(node.value,ast.Name) and node.value.id in os_names)
    changed=True
    while changed:
        changed=False
        for node in ast.walk(tree):
            if isinstance(node,ast.Assign) and is_env(node.value):
                for target in node.targets:
                    if isinstance(target,ast.Name) and target.id not in env_names:
                        env_names.add(target.id);changed=True
    parents={child:node for node in ast.walk(tree) for child in ast.iter_child_nodes(node)}
    def scope(node):
        while node in parents:
            node=parents[node]
            if isinstance(node,(ast.FunctionDef,ast.AsyncFunctionDef,ast.Lambda)):return node
        return tree
    def literal(node):
        if isinstance(node,ast.Constant) and isinstance(node.value,str):return node.value
        if isinstance(node,ast.Name):return constants.get(node.id)
        return None
    reads=[];writes=[]
    for node in ast.walk(tree):
        if isinstance(node,ast.Subscript) and is_env(node.value):
            key=literal(node.slice)
            if isinstance(node.ctx,ast.Load):reads.append((node,key,'subscript'))
            elif isinstance(node.ctx,ast.Store):writes.append((node,key,scope(node)))
        if not isinstance(node,ast.Call):continue
        function=node.func
        get=(isinstance(function,ast.Name) and function.id in getter_names or
             isinstance(function,ast.Attribute) and isinstance(function.value,ast.Name) and
             function.value.id in os_names and function.attr=='getenv')
        env_method=isinstance(function,ast.Attribute) and is_env(function.value)
        if get or env_method and function.attr in ('get','pop','setdefault'):
            reads.append((node,literal(node.args[0]) if node.args else None,'getenv' if get else function.attr))
        if env_method and function.attr=='setdefault' and node.args:
            writes.append((node,literal(node.args[0]),scope(node)))
    rows=[]
    for node,key,operation in sorted(reads,key=lambda row:row[0].lineno):
        local=bool(key and any(k==key and s is scope(node) and w.lineno<node.lineno and
            isinstance(parents.get(w),ast.Assign) and parents.get(parents[w]) is scope(node) for w,k,s in writes))
        survives=bool(key and key.upper() in filtered({key:'synthetic'}))
        rows.append(dict(file=name,line=node.lineno,name=key,operation=operation,
            same_process_unconditional_prior_assignment=local,survives_filtered=survives,
            result='PASS' if local or survives else 'FAIL',source=ast.get_source_segment(text,node)))
    return rows


def check(directory):
    files=sorted(Path(directory).glob('*.py'))
    rows=[]
    for path in files:rows.extend(inspect_source(path.read_text(encoding='utf-8'),path.name))
    return dict(result='PASS' if all(row['result']=='PASS' for row in rows) else 'FAIL',
        helpers=[p.name for p in files],reads=rows,values_recorded=False,
        contract='Every named environment read is set earlier unconditionally in that function, or survives filtered(). Dynamic unresolved reads fail. Whole-environment forwarding is separately checked by boundary tests.')


def self_tests():
    cases=[]
    for name,source,expected in (
        ('preserved','import os\nx=os.environ["PATH"]',True),
        ('orchestration_metadata_survives_filtered','import os\nx=os.getenv("EUROPA_JOB_CONFIG")',True),
        ('G1_mutant','import os\nx=os.environ["EUROPA_AUTHORING_MARKER"]',False),
        ('get_mutant','import os\nx=os.environ.get("FAKE_SECRET")',False),
        ('getenv_mutant','from os import getenv as read\nx=read("FAKE_TOKEN")',False),
        ('alias_mutant','import os as operating\ne=operating.environ\nx=e["FAKE_AUTH"]',False),
        ('constant_mutant','import os\nkey="FAKE_COOKIE"\nx=os.getenv(key)',False),
        ('dynamic_refused','import os,sys\nx=os.getenv(sys.argv[1])',False),
        ('same_process','import os\ndef f():\n os.environ["FAKE_SECRET"]="synthetic"\n return os.environ["FAKE_SECRET"]',True),
        ('conditional_write_refused','import os\ndef f(flag):\n if flag: os.environ["FAKE_SECRET"]="synthetic"\n return os.environ["FAKE_SECRET"]',False),
        ('other_function_write_refused','import os\ndef a():\n os.environ["FAKE_SECRET"]="synthetic"\ndef b():\n return os.environ["FAKE_SECRET"]',False)):
        rows=inspect_source(source,name)
        actual=bool(rows) and all(r['result']=='PASS' for r in rows)
        cases.append(dict(case=name,passed=actual==expected,reads=rows))
        assert actual==expected,name
    return cases


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--directory',type=Path,default=Path(__file__).parent);p.add_argument('--output',type=Path,required=True)
    args=p.parse_args();result=check(args.directory);result['self_tests']=self_tests()
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(dict(result=result['result'],helpers=len(result['helpers']),reads=len(result['reads']),self_tests=len(result['self_tests']))))
    raise SystemExit(0 if result['result']=='PASS' else 2)
