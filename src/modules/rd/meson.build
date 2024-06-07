mod_rd_sources = [
  'main.c',
  'rd.c',
]

mod_rd = executable('mod_rd',
  sources : mod_rd_sources,
  link_args : exec_link_args,
  dependencies : [ libc_dep, libvfs_dep ],
  pie : true,
  install : true,
  install_dir : 'modules',
)
