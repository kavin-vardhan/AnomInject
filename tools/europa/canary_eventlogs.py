from pathlib import PureWindowsPath


FOLDER = PureWindowsPath('C:/Windows/System32/winevt/Logs')
WINDOWS = PureWindowsPath('C:/Windows')


def excluded_denial(path, error):
    return exclusion_class(path,error) is not None


def exclusion_class(path,error):
    path = PureWindowsPath(path)
    if '..' in path.parts:return None
    code=getattr(error,'winerror',None)
    if code==5 and path.parent==FOLDER and path.suffix.casefold()=='.evtx':
        return 'FE2 OS-protected Windows event log'
    if code in (32,33) and WINDOWS in path.parents:
        return 'FI5 OS-locked Windows file'
    return None
