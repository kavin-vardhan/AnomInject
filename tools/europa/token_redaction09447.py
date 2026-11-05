import re

def line(value):
    value=re.sub(r'(Control server token:\s*)\S+',r'\1<masked>',value,flags=re.I)
    value=re.sub(r'(\btoken\s*=\s*)[^\s|]+',r'\1<masked>',value,flags=re.I)
    value=re.sub(r'("token"\s*:\s*")[^"]*(")',r'\1<masked>\2',value,flags=re.I)
    return value

def selftest():
    values=['=== Control server token: example-placeholder (random per-session) ===','Control: listening ws://127.0.0.1:8077 | token=example-placeholder | 1 authed','{"type":"hello","token":"example-placeholder","v":1}']
    for value in values:
        cleaned=line(value)
        assert 'example-placeholder' not in cleaned and '<masked>' in cleaned
        assert line(cleaned)==cleaned
    assert line('Control: client authenticated.')=='Control: client authenticated.'
    return dict(result='PASS',cases=4)
