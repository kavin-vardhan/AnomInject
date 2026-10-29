def script(author_path, recipe, receipt_path, smoke_path):
    lines = [
        'import imp',
        'fixture_module = imp.load_source("europa_fixture_author_425", %r)' % author_path,
    ]
    if recipe == 'Smoke':
        lines += [
            'smoke_module = imp.load_source("europa_fixture_smoke_425", %r)' % smoke_path,
            'smoke_module.check(fixture_module, %r)' % receipt_path,
        ]
    else:
        lines += ['fixture_module.author(%r, %r)' % (recipe, receipt_path)]
    return '\n'.join(lines) + '\n'
