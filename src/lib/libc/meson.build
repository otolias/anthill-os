libc_sources = files([
  'ctype' / 'isdigit.c',
  'ctype' / 'isspace.c',
  'errno' / 'errno.c',
  'fcall' / 'fcall_msg_size.c',
  'fcall' / 'fcall_msg_to_buf.c',
  'fcall' / 'fcall_buf_to_msg.c',
  'internal' / 'formatter.c',
  'internal' / 'stdlib.c',
  'internal' / 'strtox.c',
  'stdio' / 'printf.c',
  'stdio' / 'puts.c',
  'stdio' / 'snprintf.c',
  'stdio' / 'sprintf.c',
  'stdio' / 'vsnprintf.c',
  'stdio' / 'vsprintf.c',
  'stdlib' / 'exit.c',
  'stdlib' / 'free.c',
  'stdlib' / 'malloc.c',
  'stdlib' / 'strtol.c',
  'stdlib' / 'strtoll.c',
  'stdlib' / 'strtoul.c',
  'stdlib' / 'strtoull.c',
  'string' / 'strchr.c',
  'string' / 'strcmp.c',
  'string' / 'strcpy.c',
  'string' / 'strlen.c',
  'string' / 'strncpy.c',
  'string' / 'strcspn.c',
  'string' / 'strspn.c',
  'string' / 'strtok.c',
  'mqueue' / 'mq_close.c',
  'mqueue' / 'mq_open.c',
  'mqueue' / 'mq_receive.c',
  'mqueue' / 'mq_send.c',
  'mqueue' / 'mq_unlink.c',
  'syscalls.c',
  'sys' / 'mman' / 'mmap.c',
  'sys' / 'mman' / 'munmap.c',
  'unistd' / 'getpid.c',
])

libc_includes = files([
  'include' / 'ctype.h',
  'include' / 'errno.h',
  'include' / 'fcall.h',
  'include' / 'fcntl.h',
  'include' / 'limits.h',
  'include' / 'mqueue.h',
  'include' / 'stddef.h',
  'include' / 'stdint.h',
  'include' / 'stdio.h',
  'include' / 'stdlib.h',
  'include' / 'syscalls.h',
  'include' / 'sys' / 'mman.h',
  'include' / 'sys' / 'types.h',
  'include' / 'unistd.h',
])

libc_inc_directories = include_directories(
  ['include', '..' / '..' / 'include'],
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
