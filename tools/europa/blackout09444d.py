import datetime
import time

LAUNCH_CUTOFF = 1793778000
CLOSE_DEADLINE = 1793779500
CLOSE_RESERVE = 60


def require_launch(seconds=900, now=None):
    now = time.time() if now is None else now
    if now >= LAUNCH_CUTOFF or now + seconds + CLOSE_RESERVE >= CLOSE_DEADLINE:
        raise RuntimeError('GI7: launch cutoff or insufficient close reserve')


def require_runtime(now=None):
    now = time.time() if now is None else now
    if now >= CLOSE_DEADLINE - CLOSE_RESERVE:
        raise RuntimeError('GI7: owned stop before blackout')


def admission_deadline(now=None):
    now = time.time() if now is None else now
    require_launch(now=now)
    return datetime.datetime.fromtimestamp(min(now+10800, LAUNCH_CUTOFF), datetime.timezone.utc).isoformat()


def require_reader(seconds=300, now=None):
    now = time.time() if now is None else now
    if now + seconds >= CLOSE_DEADLINE:
        raise RuntimeError('GI7: pixel reader cannot finish before blackout')
