test_suite_sources = [
  'main.c',
  'mqueue.c',
]

hello = executable('test_suite',
  sources : test_suite_sources,
  link_args : exec_link_args,
  dependencies : [ libc_dep ],
  pie : true,
  install : true,
)
