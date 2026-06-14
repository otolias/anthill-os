arch = host_machine.cpu_family()

kernel_sources = files([
  'elf.c',
  'io.c',
  'main.c',
  'mqueue.c',
  'panic.c',
  'rd.c',
  'string.c',
  'syscalls.c',
  'syscalls' / 'sys_exit.c',
  'syscalls' / 'sys_fork.c',
  'syscalls' / 'sys_mmap.c',
  'syscalls' / 'sys_munmap.c',
  'task.c',
  'test' / 'all.c',
  'test' / 'mem.c',
])

subdir('arch' / arch)

kernel_elf = executable('kernel8.elf',
  sources : kernel_sources,
  include_directories : include_directories('include'),
  c_args : [ '-ffreestanding', '-mgeneral-regs-only' ],
  link_args : [
    '-nostdlib',
    '-T', meson.current_source_dir() / 'arch' / arch / 'link.ld'
  ]
)
