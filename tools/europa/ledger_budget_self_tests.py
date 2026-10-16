import contextlib
import importlib.util
import io
import json
from pathlib import Path
import sqlite3

ROOT=Path('D:/IA_Europa/Receipts/09403/synthetic-ledger-budgets')
SOURCE=Path('D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/write_ledger.py')

def main():
    ROOT.mkdir(exist_ok=False)
    spec=importlib.util.spec_from_file_location('budget_subject',SOURCE)
    m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
    results=[]
    for name,drive,size,prior,expected in [('C_limit','C',250000000,0,'PASS'),('C_over','C',250000001,0,'STOP'),('E_limit','E',50000000,0,'PASS'),('E_over','E',50000001,0,'STOP'),('cumulative_C','C',249999901,100,'STOP')]:
        m.LEDGER=ROOT/name
        folder=m.LEDGER/'operation';folder.mkdir(parents=True)
        metadata={'totals':{'port':{'bytes':0}},'disks':{},'access_errors':[]}
        m.dump(folder/'before.json',metadata)
        for phase in ('before','after'):
            con=sqlite3.connect(folder/(phase+'.sqlite'))
            con.execute('CREATE TABLE files(scope TEXT,path TEXT,size INTEGER,mtime INTEGER,PRIMARY KEY(scope,path)) WITHOUT ROWID')
            if phase=='after':
                scope='unreal_engine' if drive=='C' else 'engine425'
                path='C:/Users/kavin/AppData/Local/UnrealEngine/4.25/SYNTHETIC-NOT-A-FILE' if drive=='C' else 'E:/EpicGames/UE_4.25/Engine/Intermediate/Build/BuildRules/SYNTHETIC-NOT-A-FILE'
                con.execute('INSERT INTO files VALUES(?,?,?,?)',(scope,path,size,1))
            con.commit();con.close()
        if prior:m.dump(m.LEDGER/'prior/summary.json',{'exception_growth_charged':{'C':prior,'E':0}})
        m.snapshot=lambda folder,label:metadata
        caught=False
        with contextlib.redirect_stdout(io.StringIO()):
            try:m.finish(folder,'ubt')
            except RuntimeError as e:
                if str(e)!='EB1/EA15 ledger STOP':raise
                caught=True
        result=json.loads((folder/'summary.json').read_text())
        assert result['result']==expected and caught==(expected=='STOP')
        results.append({'case':name,'expected':expected,'actual':result['result'],'pass':True,'actual_C_E_files_written':0})
    m.dump(ROOT/'results.json',{'result':'PASS','cases':results,'scope':'Production ledger charge/comparison code on synthetic SQLite metadata, isolated from the live job ledger. No synthetic path exists or is created on C/E.'})
    print(json.dumps({'ledger_budget_can_fails':'PASS','cases':len(results),'actual_C_E_files_written':0}))

if __name__=='__main__':main()
