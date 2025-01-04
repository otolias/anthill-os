kernel_sources = files([
  'boot.S',
  'cpu_context.S',
  'elf.c',
  'entry.S',
  'irq.c',
  'kprintf.c',
  'main.c',
  'mqueue.c',
  'mm.c',
  'ramdisk.c',
  'string.c',
  'syscalls.c',
  'task.c',
  'timer.c',
  'uart.c',
])

kernel_inc_directories = include_directories('include')

kernel_elf = executable('kernel8.elf',
  sources : kernel_sources,
  include_directories : kernel_inc_directories,
  c_args : kernel_c_args,
  link_args : kernel_link_args
)
