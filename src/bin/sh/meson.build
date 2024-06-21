sh_sources = [
  'main.c',
]

sh = executable('sh',
  sources : sh_sources,
  link_args : exec_link_args,
  dependencies : [ libc_dep ],
  pie : true,
  install : true,
)
