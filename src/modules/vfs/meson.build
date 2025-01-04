mod_vfs_sources = [
  'main.c',
  'vfs.c',
  'vnode.c',
]

mod_vfs = executable('mod_vfs',
  sources : mod_vfs_sources,
  link_args : exec_link_args,
  dependencies : [ libc_dep, libvfs_dep ],
  install : true,
  install_dir : 'modules',
)
