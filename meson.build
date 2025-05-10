# C Minilib Sip Codec
# ******************************************************************************
# *    Project
# ******************************************************************************
project('c_minilib_sip_codec', 'c',
         version: '0.0.1',
	 meson_version: '>=1.0.1',
	 default_options: [
            'werror=true',
            'c_std=c11',
         ],
)

# ******************************************************************************
# *    App
# ******************************************************************************
if get_option('backtrace')
  add_project_arguments('-DCME_ENABLE_BACKTRACE', language: 'c')
endif

if get_option('buildtype').startswith('debug')
  add_project_arguments('-fno-inline-functions', language : 'c')
endif


# Enable posix functions
add_project_arguments('-D_POSIX_C_SOURCE=200809L', language: 'c')

subdir('src')

c_minilib_error_sub = subproject('c_minilib_error')
c_minilib_error_dep = dependency('c_minilib_error',
  fallback: ['c_minilib_error', 'c_minilib_error_dep'],
  required: true,
)

c_minilib_sip_codec_deps = [c_minilib_error_dep]
c_minilib_sip_codec_inc = include_directories('include', 'src')
c_minilib_sip_codec_lib = library('c_minilib_sip_codec',
                         sources,
			 include_directories: c_minilib_sip_codec_inc,
                         dependencies: c_minilib_sip_codec_deps,
)

c_minilib_sip_codec_dep = declare_dependency(
  link_with: c_minilib_sip_codec_lib,
  include_directories: c_minilib_sip_codec_inc,
  dependencies: c_minilib_sip_codec_deps,  
)



# ******************************************************************************
# *    Tests
# ******************************************************************************
if get_option('tests')
   subdir('test')
endif

# ******************************************************************************
# *    Examples
# ******************************************************************************
if get_option('examples')
   subdir('example')
endif
