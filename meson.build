project('arm_os', 'c',
  default_options : [
    'c_args=-g3',
    'optimization=0',
    'c_std=c17',
    'warning_level=2',
    'prefix=' + meson.build_root() / 'sysroot',
    'libdir=lib',
    'includedir=usr/include',
  ],
)

add_project_arguments(
  '-D_FORTIFY_SOURCE=1',
  '-ffunction-sections',
  '-fno-common',
  '-fno-short-enums',
  '-Wdouble-promotion',
  '-Wformat-security',
  '-Wjump-misses-init',
  '-Wmisleading-indentation',
  '-Wnull-dereference',
  '-Wrestrict',
  '-Wshadow',
  '-Wunused',
  language: 'c',
)

gcc = meson.get_compiler('c').cmd_array()
objcopy = find_program('objcopy')
tar = find_program('tar')

# Unit testing
unity_dep = dependency('unity',
  native : true,
  required : false,
)

if unity_dep.found()
  tests_enabled = true
else
  tests_enabled = false
  message('Unity not found, disabling unit tests')
endif

kernel_sources = []
kernel_includes = []

subdir('src' / 'boot')
subdir('src' / 'drivers')
subdir('src' / 'kernel')

subdir('src' / 'lib')

exec_link_args = [
  '-nodefaultlibs',
  '-B' + meson.build_root(),
  '-z' + 'max-page-size=0x1000',
  '-z' + 'noseparate-code',
]

subdir('src' / 'bin')

kernel_inc_directories = include_directories('src' / 'include')

kernel_elf = executable('kernel8.elf',
  sources : kernel_sources,
  include_directories : kernel_inc_directories,
  c_args : ['-ffreestanding'],
  link_args : ['-nostdlib', '-T', meson.source_root() / 'linker.ld'],
)

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

if tests_enabled
  kernel_tests = {
    'string' : [ meson.source_root() / 'test' / 'kernel' / 'string.c' ],
    'mm' : [ meson.source_root() / 'test' / 'kernel' / 'mm.c' ],
  }

  kernel_test_inc_directories = include_directories('src')

  # Use -ffreestanding to omit calls from native libc during compilation,
  # but link it for Unity calls
  foreach name, sources : kernel_tests
    kernel_test = executable(name,
      sources : sources,
      include_directories : [ kernel_inc_directories, kernel_test_inc_directories ],
      c_args: ['-ffreestanding'],
      dependencies : unity_dep,
      native : true,
    )

    test(name, kernel_test,
      suite : 'kernel',
    )
  endforeach
endif
