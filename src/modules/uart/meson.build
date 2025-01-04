mod_uart_sources = [
  'main.c',
  'uart.c',
]

mod_uart = executable('mod_uart',
  sources : mod_uart_sources,
  include_directories : include_directories('..' / '..' / 'kernel' / 'include'),
  link_args : exec_link_args,
  dependencies : [ libc_dep, libvfs_dep ],
  install : true,
  install_dir : 'modules',
)
