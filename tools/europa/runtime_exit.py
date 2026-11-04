import datetime


def stop_with_intent(receipt, pid, reason, root_running, persist, stop):
    intent = dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  pid=pid, reason=str(reason), root_running=bool(root_running))
    receipt.setdefault('stop_intents', []).append(intent)
    persist(receipt)
    return stop()


def classify_exit(execution):
    code = execution.get('exit_code')
    for intent in execution.get('stop_intents', []):
        if intent.get('pid') != execution.get('pid') or not intent.get('root_running'):
            continue
        try:
            began = datetime.datetime.fromisoformat(execution['started_utc'])
            requested = datetime.datetime.fromisoformat(intent['utc'])
            ended = datetime.datetime.fromisoformat(execution.get('root_ended_utc') or execution['ended_utc'])
            if not began <= requested <= ended:
                continue
        except (KeyError, TypeError, ValueError):
            continue
        return 'owned-stop' if code is not None else 'stop-pending'
    if 'stop_intents' not in execution and execution.get('stopped_owned_processes'):
        return 'owned-stop'
    if execution.get('launches') and code not in (0, None):
        return 'crash'
    return 'normal' if code == 0 else 'incomplete'
