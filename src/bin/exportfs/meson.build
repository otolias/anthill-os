exportfs_sources = [
  'main.c',
]

exportfs = executable('exportfs',
  sources : exportfs_sources,
  link_args : exec_link_args,
  dependencies : [ libc_dep ],
  pie : true,
  install : true,
)
