arch = host_machine.cpu_family()

kernel_sources = files([
  'elf.c',
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
  include_directories : include_directories('include'),
  c_args : [ '-ffreestanding', '-mgeneral-regs-only' ],
  link_args : [
    '-nostdlib',
    '-T', meson.current_source_dir() / 'arch' / arch / 'link.ld'
  ]
)
