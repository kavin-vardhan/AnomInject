import ast
import json
from pathlib import Path

import capture_c3_reader as reader
import capture_c3_kit as kit
from rotator_contract import check_source

def run():
    result=reader.tests()
    root=kit.J/'tests/c3-command'
    argv=kit.command('C3',root)
    assert '-game' in argv and '-unattended' in argv and '-nowrite' in argv
    assert sum('IAI.Capture.C3 ' in a for a in argv)==1
    assert all('defaultbackbufferpixelformat' not in a.lower() for a in argv)
    assert any('bSendUnattendedBugReports=False' in a for a in argv)
    try:kit.command('C2',root)
    except RuntimeError:pass
    else:raise AssertionError('Unarmed leg escaped')
    try:kit.validate(dict(arming_job='09432',stage='C3'))
    except RuntimeError:pass
    else:raise AssertionError('Spent arming escaped')
    files=('capture_c3_kit.py','capture_c3_reader.py','capture_c3_tests.py','capture_425_driver.py')
    for name in files:
        source=(Path(__file__).parent/name).read_text()
        ast.parse(source)
        assert not check_source(source,name)
    result.update(armed_command='PASS',older_and_unarmed_refusal='PASS',keyword_rotators='PASS',changed_in_editor_scripts=[])
    return result

if __name__=='__main__':
    result=run()
    (kit.J/'tests/c3.json').write_text(json.dumps(result,indent=2))
    print(json.dumps(result))
