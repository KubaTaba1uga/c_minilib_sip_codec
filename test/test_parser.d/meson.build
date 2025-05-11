test_parse_msgempty_name = 'test_parse_msgempty.c'

test_parse_msgempty_exe = executable('test_parse_msgempty',
  sources: [
    test_parse_msgempty_name,
    test_runner.process(test_parse_msgempty_name),
  ],
  dependencies: test_dependencies,
)

test('test_parse_msgempty', test_parse_msgempty_exe)

test_parse_parsingheaders_name = 'test_parse_parsingheaders.c'

test_parse_parsingheaders_exe = executable('test_parse_parsingheaders',
  sources: [
    test_parse_parsingheaders_name,
    test_runner.process(test_parse_parsingheaders_name),
  ],
  dependencies: test_dependencies,
)

test('test_parse_parsingheaders', test_parse_parsingheaders_exe)



subdir('test_iterators.d')
