mod_pl011_sources = [
  'main.c',
  'pl011.c',
]

mod_pl011 = executable('mod_pl011',
  sources : mod_pl011_sources,
  include_directories : include_directories('..' / '..' / 'kernel' / 'include'),
  link_args : exec_link_args,
  dependencies : [ libc_dep, libvfs_dep ],
  install : true,
  install_dir : 'modules',
)
