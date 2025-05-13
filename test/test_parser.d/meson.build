test_parse_first_line_name = 'test_parse_first_line.c'
test_parse_first_line_exe = executable('test_parse_first_line',
  sources: [
    test_parse_first_line_name,
    test_runner.process(test_parse_first_line_name),
  ],
  dependencies: test_dependencies,
)
test('test_parse_first_line', test_parse_first_line_exe)


field_tests = [
  'to',
  # 'from',
  # 'cseq',
  # 'call_id',
  # 'via',
  # 'allow',
  # 'accept',
  # 'expires',
  # 'subject',
  # 'organization',
  # 'user_agent',
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
