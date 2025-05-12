test_parse_msgempty_name = 'test_parse_msgempty.c'
test_parse_parsingheaders_name = 'test_parse_parsingheaders.c'

# Core parser tests
test_parse_msgempty_exe = executable('test_parse_msgempty',
  sources: [
    test_parse_msgempty_name,
    test_runner.process(test_parse_msgempty_name),
  ],
  dependencies: test_dependencies,
)
test('test_parse_msgempty', test_parse_msgempty_exe)

test_parse_parsingheaders_exe = executable('test_parse_parsingheaders',
  sources: [
    test_parse_parsingheaders_name,
    test_runner.process(test_parse_parsingheaders_name),
  ],
  dependencies: test_dependencies,
)
test('test_parse_parsingheaders', test_parse_parsingheaders_exe)

# List of field-level parser test files
field_tests = [
  'to',
  'from',
  'cseq',
  'call_id',
  'via',
  'allow',
  'accept',
  'expires',
  # Add more fields here
]

foreach field : field_tests
  test_name = 'test_parse_' + field
  source_name = test_name + '.c'

  test_exe = executable(test_name,
    sources: [
      source_name,
      test_runner.process(source_name),
    ],
    dependencies: test_dependencies,
  )

  test(test_name, test_exe)
endforeach

# Iterator tests
subdir('test_iterators.d')
