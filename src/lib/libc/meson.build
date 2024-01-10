libc_sources = files([
  'exit.c',
  '_malloc.c',
  'malloc.c',
  'printf.c',
  'syscalls.c',
])

libc_includes = files([
  'include' / 'stddef.h',
  'include' / 'stdint.h',
  'include' / 'stdio.h',
  'include' / 'stdlib.h',
  'include' / 'syscalls.h',
])

libc_inc_directories = include_directories(
  ['include', '../../include'],
  is_system : true
)

ld_sources = [
  'ld.c',
]

ld = shared_library('ld',
  sources : ld_sources,
  include_directories : libc_inc_directories,
  link_args : ['-nostdlib'],
  install : true,
  name_prefix : '',
)

crt0 = custom_target('crt0',
  input : [ 'crt' / 'crt0.S' ],
  output : [ 'crt0.o' ],
  command : [gcc, '-c', '@INPUT@', '-o', '@BUILD_ROOT@' / 'crt0.o'],
  build_by_default : true,
)

libc = static_library('c',
  sources : libc_sources,
  include_directories : libc_inc_directories,
  link_depends : [ crt0 ],
  c_args : ['-nostdlib'],
  pic : true,
  install : true,
)

libc_dep = declare_dependency(
  include_directories : libc_inc_directories,
  link_with : libc,
)

if tests_enabled
  libc_tests = {
    'stdint': [ meson.source_root() / 'test' / 'lib' / 'libc' / 'stdint.c' ],
    # TODO: Mock sys_handler
    # 'printf': [ meson.source_root() / 'test' / 'lib' / 'libc' / 'printf.c' ],
  }

  libc_test_include_directories = include_directories('.')

  foreach name, sources : libc_tests
    libc_test = executable(name,
      sources : sources,
      include_directories : [libc_inc_directories, libc_test_include_directories],
      dependencies : unity_dep,
      native : true,
    )

    test(name, libc_test,
      suite : 'libc',
    )
  endforeach
endif
