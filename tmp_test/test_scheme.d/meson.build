test_scheme_name = 'test_scheme.c'

test_scheme_exe = executable('test_scheme',
  sources: [
    test_scheme_name,
    test_runner.process(test_scheme_name),
  ],
  dependencies: test_dependencies,
  # include_directories: include_directories('../../include'),
)

test('test_scheme', test_scheme_exe)
