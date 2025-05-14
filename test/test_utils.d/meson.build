# List of test source files (extend this list as needed)
test_files = [
  'test_parser.c',
  'test_parse_sip.c',
]

foreach test_file : test_files
  test_name = test_file.split('.')[0]  # e.g., "test_parser"
  test_exe = executable(test_name,
    sources: [
      test_file,
      test_runner.process(test_file),
    ],
    dependencies: test_dependencies,
  )
  test(test_name, test_exe)
endforeach
