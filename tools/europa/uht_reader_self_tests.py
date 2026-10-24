import argparse
import copy
import json
from pathlib import Path
import sys


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, required=True)
    root = parser.parse_args().output.resolve()
    if not root.is_relative_to(Path('D:/IA_Europa').resolve(strict=True)):
        raise RuntimeError('Synthetic fixtures must remain in owned D scratch')
    root.mkdir(parents=True, exist_ok=False)
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    import strict_include_build as strict
    cases = []
    for name, native, engine, target, kind in [
        ('425game', 'UE4', 'UE4Editor', 'SyntheticGame', 'Game'),
        ('425editor', 'UE4Editor', 'UE4Editor', 'SyntheticEditor', 'Editor'),
        ('51editor', 'UnrealEditor', 'UnrealEditor', 'SyntheticEditor', 'Editor')]:
        fixture = root / name
        plugin, host = fixture / 'Plugin', fixture / 'Host'
        module = plugin / 'Source/Sample'
        module.mkdir(parents=True)
        header = module / 'Demo.h'
        header.write_text('#include "Demo.generated.h"\n')
        ordinary = module / 'One.cpp'
        ordinary.write_text('SYNTHETIC NOT COMPILED\n')
        header_tu = module / 'SIC_Demo.cpp'
        header_tu.write_text('SYNTHETIC NOT COMPILED\n')
        generated = plugin / 'Intermediate/Build/Win64' / native / 'Inc/Sample'
        if name == '51editor':
            generated /= 'UHT'
        generated.mkdir(parents=True)
        generated_files = [generated / 'Demo.gen.cpp', generated / 'Sample.init.gen.cpp']
        for path in generated_files:
            path.write_text('SYNTHETIC NOT COMPILED\n')
        manifest = host / 'Intermediate/Build/Win64' / target / 'Development' / (target + '.uhtmanifest')
        manifest.parent.mkdir(parents=True)
        manifest.write_text(json.dumps({'Modules': [{'Name': 'Sample', 'BaseDirectory': str(module),
            'OutputDirectory': str(generated), 'ClassesHeaders': [], 'PublicHeaders': [str(header)], 'PrivateHeaders': []}]}))
        object_dir = plugin / 'Intermediate/Build/Win64' / target / 'Development/Sample'
        object_dir.mkdir(parents=True)
        actions = []
        for cpp in [ordinary, header_tu, *generated_files]:
            response = object_dir / (cpp.name + '.obj.response')
            response.write_text('/std:c++17\n')
            actions.append({'Type': 'Compile', 'PrerequisiteItems': [str(cpp), str(response)],
                            'ProducedItems': [str(object_dir / (cpp.name + '.obj'))]})
        resource = module / 'Synthetic.rc2'
        resource.write_text('SYNTHETIC NOT COMPILED\n')
        actions.append({'Type': 'Compile', 'CommandPath': 'C:/SYNTHETIC/rc.exe',
            'PrerequisiteItems': [str(resource)], 'ProducedItems': [str(object_dir / 'Synthetic.res')]})
        strict.PLUGIN_ROOT = str(plugin)
        modules = [{'name': 'Sample', 'dir': str(module)}]
        action_file = fixture / 'actions.json'
        action_file.write_text(json.dumps({'Actions': actions}))
        archived = fixture / 'archived'
        archived.mkdir()
        archived_map = {}
        for index, action in enumerate(actions[:4]):
            original = Path(action['PrerequisiteItems'][1])
            saved = archived / (str(index) + '.response')
            saved.write_bytes(original.read_bytes())
            archived_map[str(original)] = str(saved)
        response = Path(actions[0]['PrerequisiteItems'][1])
        before = response.read_bytes()
        response.write_text('/Yu"SYNTHETIC.h"\n')
        _, _, problems = strict.verify_strict_took_effect(str(host), target, kind, modules, str(action_file), engine)
        assert any('PCH in actual compile action' in p for p in problems)
        cases.append({'case': name + '_live_PCH_mutant', 'result': 'REJECTED'})
        _, _, problems = strict.verify_strict_took_effect(str(host), target, kind, modules, str(action_file), engine,
            source_tus=[str(ordinary), str(header_tu)], response_archive=archived_map)
        assert not problems, (name, problems)
        cases.append({'case': name + '_frozen_response_and_source_receipts', 'result': 'PASS'})
        response.write_bytes(before)
        assert response.read_bytes() == before
        counts, headers, problems = strict.verify_strict_took_effect(str(host), target, kind, modules, str(action_file), engine)
        assert counts == {'Sample': 2} and headers == 1 and not problems, (name, problems)
        cases.append({'case': name + '_native_manifest_positive', 'result': 'PASS'})
        mutated = copy.deepcopy(actions)
        mutated.pop(2)
        action_file.write_text(json.dumps({'Actions': mutated}))
        _, _, problems = strict.verify_strict_took_effect(str(host), target, kind, modules, str(action_file), engine)
        assert any('generated TU absent' in p for p in problems), (name, problems)
        cases.append({'case': name + '_missing_generated_action', 'result': 'REJECTED'})
        action_file.write_text(json.dumps({'Actions': []}))
        _, _, problems = strict.verify_strict_took_effect(str(host), target, kind, modules, str(action_file), engine)
        assert 'zero compile/header denominator' in problems
        cases.append({'case': name + '_zero_action', 'result': 'REJECTED'})
        action_file.write_text(json.dumps({'Actions': actions[2:]}))
        _, _, problems = strict.verify_strict_took_effect(str(host), target, kind, modules, str(action_file), engine, source_tus=[])
        assert 'zero compile/header denominator' in problems
        cases.append({'case': name + '_zero_header_TU', 'result': 'REJECTED'})
        action_file.write_text(json.dumps({'Actions': [actions[-1]]}))
        _, _, problems = strict.verify_strict_took_effect(str(host), target, kind, modules, str(action_file), engine)
        assert 'zero compile/header denominator' in problems
        cases.append({'case': name + '_resource_only', 'result': 'REJECTED'})
        mutated = copy.deepcopy(actions)
        mutated[0]['PrerequisiteItems'] = [str(ordinary)]
        action_file.write_text(json.dumps({'Actions': mutated}))
        _, _, problems = strict.verify_strict_took_effect(str(host), target, kind, modules, str(action_file), engine)
        assert any('no response for actual compile action' in p for p in problems)
        cases.append({'case': name + '_missing_Cpp_response', 'result': 'REJECTED'})
        mutated[0]['CommandPath'] = 'C:/SYNTHETIC/rc.exe'
        mutated[0]['ProducedItems'] = [str(object_dir / 'disguised.res')]
        action_file.write_text(json.dumps({'Actions': mutated}))
        _, _, problems = strict.verify_strict_took_effect(str(host), target, kind, modules, str(action_file), engine)
        assert any('no response for actual compile action' in p for p in problems)
        cases.append({'case': name + '_Cpp_disguised_as_resource', 'result': 'REJECTED'})
        if name == '51editor':
            response_only = copy.deepcopy(actions)
            for action in response_only:
                paths = action.pop('PrerequisiteItems')
                action['WorkingDirectory'] = str(fixture)
                if 'CommandPath' not in action:
                    action['CommandPath'] = 'SYNTHETIC-cl.exe'
                    response = Path(paths[1])
                    response.write_text('"' + paths[0] + '"\n/std:c++17\n')
                    action['CommandArguments'] = '@"' + str(response) + '"'
                else:
                    action['CommandArguments'] = '"' + paths[0] + '"'
            action_file.write_text(json.dumps({'Actions': response_only}))
            counts, headers, problems = strict.verify_strict_took_effect(str(host), target, kind, modules, str(action_file), engine)
            assert counts == {'Sample': 2} and headers == 1 and not problems, problems
            cases.append({'case': '51_native_no_PrerequisiteItems', 'result': 'PASS'})
            response_only[0]['CommandArguments'] = ''
            action_file.write_text(json.dumps({'Actions': response_only}))
            _, _, problems = strict.verify_strict_took_effect(str(host), target, kind, modules, str(action_file), engine)
            assert any('no response for actual compile action' in p for p in problems)
            cases.append({'case': '51_missing_response_argument', 'result': 'REJECTED'})
        action_file.write_text(json.dumps({'Actions': actions}))
    (root / 'result.json').write_text(json.dumps({'result': 'PASS', 'cases': cases,
        'synthetic_only': True, 'actual_compiles': 0}, indent=2))
    print(json.dumps({'UHT_reader': 'PASS', 'cases': len(cases)}))


if __name__ == '__main__':
    main()
