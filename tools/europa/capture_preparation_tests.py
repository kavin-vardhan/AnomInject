import copy
import json
from pathlib import Path
import re
import sys
import capture_c5_reader as reader

def run():
    root=Path(__file__).resolve().parents[2]
    source=(root/'Source/AnomalyCapture/Private/Compat/AnomalyCaptureLabels425.h').read_text()
    mutant=source.replace('Prepared == Next && ','')
    def ready(text,prepared,index,street_ready):
        expression=re.search(r'bool Ready\(\) const \{ return (.*?); \}',text).group(1)
        expression=expression.replace('Street->Ready(Next)','street_ready').replace('!Street','False').replace('&&','and').replace('||','or')
        return eval(expression,{'Prepared':prepared,'Next':index,'street_ready':street_ready})
    boundaries=(288,528,816,1056,1344,1584,1872,2112,2400,2640,2928,3168)
    for index in boundaries:
        prepared=index-1
        for unused in range(3):assert not ready(source,prepared,index,False)
        assert ready(mutant,prepared,index,True)
        assert not ready(source,prepared,index,True)
        assert ready(source,index,index,True)
    rows=[dict(census=dict(enabled=i>=240)) for i in range(480)]
    off=[dict(index=i) for i in range(240)];on=[dict(index=i) for i in range(240,480)]
    assert reader.census_protocol(rows,off,on)['result']=='PASS'
    rows[240]['census']['enabled']=False
    failed=reader.census_protocol(rows,off,on)
    assert failed['result']=='FAIL' and failed['disabled_in_on']==[240]
    declared=dict(player_rotation=[-12,0,0],player_location=[0,0,0],player_fov=90)
    assert reader.initial_view_check(declared,declared)['result']=='PASS'
    first=copy.deepcopy(declared);first['player_rotation'][1]=300
    check=reader.initial_view_check(first,declared)
    census=reader.enforce_initial_view(dict(result='PASS',late_scored_targets=['candidate']),check)
    assert census['result']=='FAIL' and census['late_scored_targets']==[]
    event=dict(first_index=1,end_index=3)
    values=[dict(index=i,pose=2,state=v) for i,v in enumerate((False,True,None,False))]
    assert reader.event_scoring(event,values)==dict(scored=True,passed_edges=False)
    assert not reader.event_scoring(event,values[1:])['scored']
    eligible=[dict(anomaly=a,scored=True) for a in reader.ROUTES for unused in range(3)]
    assert reader.c2_result([dict(sweep=dict(score=True))],eligible,[],[dict(index=2)])['result']=='FAIL'
    return dict(result='PASS',production_expression=True,multi_tick_boundaries=boundaries,negatives=['old getter admits unprepared capture after readback','census-off frame cannot satisfy census-on hold','wrong first scored view cannot qualify late targets','ambiguous eligible events remain scored and fail C2'],native_fix='Production getter tested against a delayed readback; runtime qualification is separate',reader='Protocol FAIL preserved while other measurements are collected')

if __name__=='__main__':
    result=run()
    if len(sys.argv)>1:Path(sys.argv[1]).write_text(json.dumps(result,indent=2))
    print(json.dumps(result))
