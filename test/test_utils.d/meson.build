test_dynamic_buffer_name = 'test_dynamic_buffer.c'
test_dynamic_buffer_exe = executable('test_dynamic_buffer',
  sources: [
    test_dynamic_buffer_name,
    test_runner.process(test_dynamic_buffer_name),
  ],
  dependencies: test_dependencies,
)
test('test_dynamic_buffer', test_dynamic_buffer_exe)
