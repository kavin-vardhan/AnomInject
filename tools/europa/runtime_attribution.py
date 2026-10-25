from pathlib import Path

def process_class(image, executable, owned):
    if not owned:
        return 'foreign'
    name = image.casefold()
    if name in ('unrealeditor.exe', 'ue4editor.exe'):
        return 'editor'
    if name == 'shadercompileworker.exe':
        return 'shader_worker'
    if name == 'crashreportclient.exe':
        return 'crash_reporter'
    if name in ('unrealbuildtool.exe', 'unrealheadertool.exe', 'cl.exe', 'link.exe', 'mspdbsrv.exe', 'vctip.exe'):
        return 'compile_child'
    path = str(executable).replace('\\', '/').lower()
    if path.startswith('d:/ia_europa/') and '/binaries/win64/' in path and name.endswith('.exe'):
        return 'game'
    if name in ('python.exe', 'python3.exe', 'powershell.exe', 'pwsh.exe', 'cmd.exe', 'conhost.exe'):
        return 'orchestration'
    return 'unknown_owned'

def validate_redirects(argv, graph):
    expected = ('-userdir=', '-shaderworkingdir=', '-abslog=', '-abscrashreportclientlog=')
    values = {}
    for prefix in expected:
        found = [arg[len(prefix):] for arg in argv if arg.lower().startswith(prefix)]
        if len(found) != 1:
            raise RuntimeError('Missing or ambiguous runtime redirect: ' + prefix)
        p = Path(found[0])
        if not p.is_absolute() or not p.resolve().is_relative_to(Path('D:/IA_Europa').resolve()):
            raise RuntimeError('Runtime redirect escapes physical-D scratch: ' + prefix)
        values[prefix] = str(p)
    if [arg for arg in argv if arg.lower().startswith('-ddc=')] != ['-DDC=' + graph]:
        raise RuntimeError('Runtime DDC graph mismatch')
    return values

def self_test():
    rows = []
    for name, want in [('UE4Editor.exe', 'editor'), ('UnrealEditor.exe', 'editor'),
                       ('ShaderCompileWorker.exe', 'shader_worker'), ('CrashReportClient.exe', 'crash_reporter'),
                       ('cl.exe', 'compile_child'), ('python.exe', 'orchestration')]:
        assert process_class(name, '', True) == want
        assert process_class(name, '', False) == 'foreign'
        rows.append({'case': name + '_owned_and_foreign', 'pass': True})
    assert process_class('Game.exe', 'D:/IA_Europa/Host/Binaries/Win64/Game.exe', True) == 'game'
    assert process_class('Unknown.exe', 'C:/Unknown.exe', True) == 'unknown_owned'
    rows.append({'case': 'game_path_and_unknown_fail_closed', 'pass': True})
    good = ['-UserDir=D:/IA_Europa/Runtime/425', '-ShaderWorkingDir=D:/IA_Europa/Caches/425/ShaderWorking',
            '-abslog=D:/IA_Europa/Receipts/test/runtime.log', '-AbsCrashReportClientLog=D:/IA_Europa/Receipts/test/crash.log',
            '-DDC=Europa425DDC']
    validate_redirects(good, 'Europa425DDC')
    for index in range(len(good)):
        bad = list(good)
        bad[index] = bad[index].replace('D:/IA_Europa', 'C:/rejected') if index < 4 else '-DDC=Wrong'
        try:
            validate_redirects(bad, 'Europa425DDC')
        except RuntimeError:
            rows.append({'case': 'redirect_mutant_' + str(index), 'pass': True})
        else:
            raise AssertionError('Runtime redirect mutant admitted')
    return {'result': 'PASS', 'cases': rows, 'runtime_launches': 0}
