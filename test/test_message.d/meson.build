test_message_name = 'test_message.c'

test_message_exe = executable('test_message',
  sources: [
    test_message_name,
    test_runner.process(test_message_name),
  ],
  dependencies: test_dependencies,
)

test('test_message', test_message_exe)
