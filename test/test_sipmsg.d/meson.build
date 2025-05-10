test_sipmsg_name = 'test_sipmsg.c'
test_sipmsg_exe = executable('test_sipmsg',
  sources: [
    test_sipmsg_name,
    test_runner.process(test_sipmsg_name),
  ],
  dependencies: test_dependencies,
)
test('test_sipmsg', test_sipmsg_exe)
