libsystem = static_library('system',
  sources: [
    'errno.c',
    'msb' / 'msb_close.c',
    'msb' / 'msb_create.c',
    'msb' / 'msb_recv.c',
    'msb' / 'msb_send.c',
    'syscalls.c',
  ],
  include_directories: [ 'include', '..' / '..' / 'kernel' / 'include' ],
  link_args: ['-nostdlib'],
  c_args: lib_link_args,
  install: true,
)

libsystem_dep = declare_dependency(
  include_directories: [ 'include', '..' / '..' / 'kernel' / 'include' ],
  link_with : libsystem,
)
