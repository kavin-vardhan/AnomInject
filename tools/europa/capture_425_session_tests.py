import json
from pathlib import Path
import sys

import capture_425_kit as kit


def check(route):
    calls=[]
    original=kit.execute
    kit.execute=lambda mode,*args,**kwargs:calls.append((mode,args,kwargs))
    try:
        for leg in ('Smoke','C1PIE','C1Game'):
            route(leg,'owned-root',deadline=90)
    finally:
        kit.execute=original
    assert [x[0] for x in calls]==['Smoke','Load','C1Game'],calls
    assert all(x[1]==('owned-root',) and x[2]==dict(deadline=90) for x in calls)


def main(output):
    check(kit.run_session)
    rows=[dict(case='smoke_editor_PIE_and_game_routes',result='PASS')]
    def old(leg,*args,**kwargs):
        return kit.execute('Load' if leg=='C1PIE' else 'C1Game',*args,**kwargs)
    try:check(old)
    except AssertionError:rows.append(dict(case='old_smoke_as_game_mutant',result='EXPECTED_FAIL'))
    else:raise AssertionError('Old smoke route was accepted')
    result=dict(result='PASS',cases=rows,launches=0)
    Path(output).write_text(json.dumps(result,indent=2),encoding='utf-8')
    print(json.dumps(result))


if __name__=='__main__':main(sys.argv[1])
