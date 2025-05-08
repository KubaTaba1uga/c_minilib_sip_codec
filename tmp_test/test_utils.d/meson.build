test_string_name = 'test_string.c'

test_string_exe = executable('test_string',
  sources: [
    test_string_name,
    test_runner.process(test_string_name),
  ],
  dependencies: test_dependencies,
  # include_directories: include_directories('../../include'),
)

test('test_string', test_string_exe)
