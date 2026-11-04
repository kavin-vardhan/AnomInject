import datetime

def require_launch(*args):
    assert datetime.datetime.now(datetime.timezone.utc)>=datetime.datetime(2026,11,4,9,45,tzinfo=datetime.timezone.utc),'Owner blackout still active'

def require_runtime(*args):
    require_launch()

def require_reader(*args):
    require_launch()

def admission_deadline():
    return (datetime.datetime.now(datetime.timezone.utc)+datetime.timedelta(hours=3)).isoformat()
