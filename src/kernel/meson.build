arch = host_machine.cpu_family()

kernel_sources = files([
  'boot.S',
  'elf.c',
  'entry.S',
  'io.c',
  'main.c',
  'mqueue.c',
  'mm.c',
  'rd.c',
  'string.c',
  'syscalls.c',
  'task.c',
])

subdir('arch' / arch)

kernel_elf = executable('kernel8.elf',
  sources : kernel_sources,
  include_directories : include_directories(
    'include'
  ),
  c_args : kernel_c_args,
  link_args : kernel_link_args
)
