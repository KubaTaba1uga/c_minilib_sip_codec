unity_subproject = subproject('unity')

unity_dependency = unity_subproject.get_variable('unity_dep')

test_dependencies = [unity_dependency, c_minilib_sip_codec_dep]

test_runner = unity_subproject.get_variable('gen_test_runner')

test_link_args = ['-fsanitize=memory,undefined']

test_include_dirs = [include_directories('.')]

subdir('test_unity.d')

# List of test source files (extend this list as needed)
test_files = [
  'test_parser.c',
  'test_parse_sip.c',
  'test_arg_iterator.c',
  'test_decoder.c',
  'test_sipmsg.c',
  'test_generator.c'
]

foreach test_file : test_files
  test_name = test_file.split('.')[0]  # e.g., "test_parser"
  test_exe = executable(test_name,
    sources: [
      test_file,
      test_runner.process(test_file),
    ],
    dependencies: test_dependencies,
    include_directories: test_include_dirs,
  )
  test(test_name, test_exe)
endforeach
