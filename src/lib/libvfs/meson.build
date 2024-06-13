libvfs_sources = files([
  'vfs_client_add_fid.c',
  'vfs_client_create.c',
  'vfs_client_get.c',
  'vfs_client_get_fid.c',
  'vfs_client_remove.c',
  'vfs_client_remove_fid.c',
])

libvfs_inc_directories = include_directories('include')

libvfs = static_library('vfs',
  sources : libvfs_sources,
  include_directories : libvfs_inc_directories,
  dependencies : [ libc_dep ],
  pic: true,
  install : true,
)

libvfs_dep = declare_dependency(
  include_directories : libvfs_inc_directories,
  link_with : libvfs,
)
