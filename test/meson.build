unity_subproject = subproject('unity')

unity_dependency = unity_subproject.get_variable('unity_dep')

test_dependencies = [unity_dependency, c_minilib_sip_codec_dep]

test_runner = unity_subproject.get_variable('gen_test_runner')

test_link_args = ['-fsanitize=memory,undefined']

subdir('test_unity.d')
subdir('test_utils.d')
subdir('test_parser.d')