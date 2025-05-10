test_line_iterator_name = 'test_line_iterator.c'
test_line_iterator_exe = executable('test_line_iterator',
  sources: [
    test_line_iterator_name,
    test_runner.process(test_line_iterator_name),
  ],
  dependencies: test_dependencies,
)
test('test_line_iterator', test_line_iterator_exe)


test_value_iterator_name = 'test_value_iterator.c'
test_value_iterator_exe = executable('test_value_iterator',
  sources: [
    test_value_iterator_name,
    test_runner.process(test_value_iterator_name),
  ],
  dependencies: test_dependencies,
)
test('test_value_iterator', test_value_iterator_exe)

test_args_iterator_name = 'test_args_terator.c'
test_args_iterator_exe = executable('test_args_iterator',
  sources: [
    test_args_iterator_name,
    test_runner.process(test_args_iterator_name),
  ],
  dependencies: test_dependencies,
)
test('test_args_iterator', test_args_iterator_exe)
