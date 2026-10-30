from __future__ import print_function
import ast
import os


SCRIPTS=('author_425_fixture.py','fixture_smoke_425.py','fixture_launcher.py',
         'runtime_editor_425.py','runtime_scripts.py')


def check_source(source,name):
    failures=[]
    for node in ast.walk(ast.parse(source,filename=name)):
        if not isinstance(node,ast.Call):continue
        target=node.func
        if not ((isinstance(target,ast.Attribute) and target.attr=='Rotator') or
                (isinstance(target,ast.Name) and target.id=='Rotator')):continue
        keywords=[item.arg for item in node.keywords]
        supplied=bool(node.args or keywords or getattr(node,'starargs',None) or getattr(node,'kwargs',None))
        if supplied and (node.args or set(keywords)!=set(('pitch','yaw','roll')) or len(keywords)!=3
                         or getattr(node,'starargs',None) or getattr(node,'kwargs',None)):
            failures.append(dict(file=name,line=node.lineno,rule='FI2 requires pitch, yaw and roll by keyword'))
    return failures


def check_all(directory=None):
    directory=directory or os.path.dirname(os.path.abspath(__file__))
    rows=[]
    for name in SCRIPTS:
        with open(os.path.join(directory,name),'rb') as stream:source=stream.read()
        failures=check_source(source,name)
        if failures:raise AssertionError(failures)
        rows.append(dict(file=name,result='PASS'))
    return rows
