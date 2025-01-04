project('anthill_os', 'c',
  default_options : [
    'c_args=-g3',
    'optimization=0',
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

default_c_args = [
  '-D_FORTIFY_SOURCE=1', '-ffunction-sections', '-fno-common',
  '-fno-short-enums',
  '-Wdouble-promotion', '-Wformat-security', '-Wmisleading-indentation',
  '-Wnull-dereference', '-Wshadow', '-Wunused'
]

kernel_c_args = default_c_args + ['-ffreestanding']
kernel_link_args = [
  '-nostdlib',
  '-T', meson.project_source_root() / 'linker.ld'
]

lib_link_args = [ '-Wl,-pic' ]

exec_link_args = [ '-nodefaultlibs', '-Wl,-pie' ]

subdir('src' / 'kernel')
subdir('src' / 'lib')
subdir('src' / 'bin')
subdir('src' / 'modules')

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
