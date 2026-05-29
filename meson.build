project('anthill_os', 'c',
  default_options : [
    'optimization=2',
    'c_std=c17',
    'warning_level=2',
    'prefix=' + meson.project_build_root() / 'sysroot',
    'libdir=lib',
    'includedir=usr/include',
  ],
)

cc = meson.get_compiler('c').cmd_array()
objcopy = find_program('objcopy')
tar = find_program('tar')

add_project_arguments(
  '-g3',
  '-fno-common', '-fno-short-enums',
  '-fstrict-flex-arrays=3', '-fPIC',
  language: 'c',
)

lib_link_args = [ '-Wl,-pic' ]

exec_link_args = [ '-nodefaultlibs' ] + \
  meson.get_external_property('exec_link_args')

subdir('src' / 'kernel')
subdir('src' / 'lib')
subdir('src' / 'bin')
# subdir('src' / 'modules')

kernel_bin = custom_target('kernel8.bin',
  input : [ kernel_elf ],
  output : [ 'kernel8.bin' ],
  command : [objcopy,
    '-O', 'binary',
    '@INPUT@',
    '@OUTPUT@',
  ],
  build_by_default : true,
)

# Generate ramdisk
meson.add_install_script('create_ramdisk', tar)
