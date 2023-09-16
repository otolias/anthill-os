libc_sources = files([
  'printf.c',
])

libc_includes = files([
  'include' / 'stdio.h',
])

libc_inc_directories = include_directories(
  ['include', '../../include'],
  is_system : true
)

install_headers(libc_includes)

crt = custom_target('crt0',
  input : [ 'crt0.S' ],
  output : [ 'crt0.o' ],
  command : [gcc, '-c', '@INPUT@', '-o', '@BUILD_ROOT@' / '@OUTPUT@'],
  install : true,
  install_dir : 'usr' / 'lib',
  build_by_default : true,
)

libc = static_library('c',
  sources : libc_sources,
  include_directories : libc_inc_directories,
  link_depends : crt,
  c_args : ['-nostdlib'],
  pic : true,
  install : true,
)

libc_dep = declare_dependency(
  include_directories : libc_inc_directories,
  link_with : libc,
)

if tests_enabled
  libc_test_sources = [
    meson.source_root() / 'test' / 'lib' / 'libc' / 'printf.c',
  ]

  libc_test = executable('libc_test',
    sources : libc_test_sources,
    include_directories : libc_inc_directories,
    dependencies : unity_dep,
    native : true,
  )

  test('libc_test', libc_test)
endif
