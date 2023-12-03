hello_sources = [
  'main.c'
]

hello = executable('hello',
  sources : hello_sources,
  link_args: exec_link_args,
  dependencies : [ libc_dep ],
  pie : true,
  install : true,
)
