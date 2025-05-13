field_tests = [
  'first_line',
  'parsingheaders',
  'to',
  'via',
  # 'from',
  'cseq',
  'allow',
  'invite',
  # 'call_id',


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
