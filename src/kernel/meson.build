kernel_sources = files(
  'syscalls' / 'sys_exit.c',
  'syscalls' / 'sys_fork.c',
  'syscalls' / 'sys_mmap.c',
  'syscalls' / 'sys_msb_close.c',
  'syscalls' / 'sys_msb_create.c',
  'syscalls' / 'sys_msb_recv.c',
  'syscalls' / 'sys_msb_send.c',
  'syscalls' / 'sys_munmap.c',
  'test' / 'all.c',
  'test' / 'mem.c',
  'test' / 'msgbuf.c',
  'test' / 'string.c',
  'elf.c',
  'io.c',
  'main.c',
  'mqueue.c',
  'msgbuf.c',
  'panic.c',
  'rd.c',
  'string.c',
  'syscalls.c',
  'task.c',
)

arch = host_machine.cpu_family()
subdir('arch' / arch)

kernel_elf = executable(
  'kernel8.elf',
  sources : kernel_sources,
  include_directories : 'include',
  c_args : [ '-ffreestanding', '-mgeneral-regs-only' ],
  link_args : [ '-nostdlib', '-T', meson.current_source_dir() / 'arch' / arch / 'link.ld' ],
)
