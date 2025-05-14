test_parser_name = 'test_parser.c'
test_parser_exe = executable('test_parser',
  sources: [
    test_parser_name,
    test_runner.process(test_parser_name),
  ],
  dependencies: test_dependencies,
)
test('test_parser', test_parser_exe)
