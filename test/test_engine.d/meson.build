test_message_type_name = 'test_message_type.c'

test_message_type_exe = executable('test_message_type',
  sources: [
    test_message_type_name,
    test_runner.process(test_message_type_name),
  ],
  dependencies: test_dependencies,
)

test('test_message_type', test_message_type_exe)
