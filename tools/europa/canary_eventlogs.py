from pathlib import PureWindowsPath


FOLDER = PureWindowsPath('C:/Windows/System32/winevt/Logs')


def excluded_denial(path, error):
    path = PureWindowsPath(path)
    return (getattr(error, 'winerror', None) == 5 and '..' not in path.parts
            and path.parent == FOLDER and path.suffix.casefold() == '.evtx')
